/*
=============================================================================
Filename: MAIN.C
=============================================================================
Autor:      Petr Smolík
Datum:      1.3.2018
Spolecnost: CVUT-FEL-K13114
Varianta:   1.00
Zmena:      --
Kompilator: MPLAB C18-V3.47
IDE:        MPLAB-X 3.61/MPLAB 8.92
Procesor:   PIC18F87J11
=============================================================================
*/
/*
=============================================================================
Vtahovane soubory:
  P18F87J11.H
  xx.h
=============================================================================
*/
/*
=============================================================================
Popis cinnosti:
Generator Fx = 40 [MHz]
Funkce:     xxxx
vstup:  	  xxxx
vystup: 	  xxxx
preruseni:  neni pouzito
============================================================================
*/
//----------------------------------------------------------------------------
#include <p18f87j11.h>
#include "filter_bin.h"
#include "vyhodnoceni_tlacitka.h"
#include "integer.h"
#include "uart_1.h"
//----------------------------------------------------------------------------
/* Ukazka vtahovani lokalnich souboru "*.h" */

//----------------------------------------------------------------------------
/* Hlavicky podprogramu.*/
void main (void);
void InterruptHandlerHigh (void);
void InterruptHandlerLow (void);
void InitVstupy(void);
void InitVystupy(void);
void InitTimer2(void);
void InitTimer3(void);
void InitCCP3(void);
void InitCCP5(void);
void InitRadicPreruseni(void);
void InitFLAGbits(void);
void InitK(void);
void Timer2On(void);
void Timer3On(void);
void HlidacHranicUart(void);
void HlidacHranicInk(void);
void ResetHranic(void);
void InitStructTlacitka(void);
/* Promenna slucujici vsechny potrebne promenne pro obsluhu serioveho 
 * komunikacniho kanalu "UART 1". Promenna musi byt ulozena ve specifikovane 
 * oblasti pameti typu RAM.
 */
#pragma udata UART_RAM
T_uart uart_1;
//----------------------------------------------------------------------------
/* Konfigurace procesoru: */
/* Volba oscilatoru */
#pragma config FOSC = HSPLL
/* Nejsou pouyity dva oscilatory */
#pragma config IESO = OFF
/* Monitor hodin */
#pragma config FCMEN = OFF
/* Zakladni mod procesoru, pouze vnitrni pamet */
#pragma config MODE = MM
/* Rozsirena instrukcni sada a indexove adresovani */
#pragma config XINST = OFF
/* Pri preteceni nebo podteceni zasobniku generovat RESET */
#pragma config STVREN = OFF
/* Watchdog timer */
#pragma config WDTEN = OFF
/* Nastaveni preddelicky pro Watchdog timer */
#pragma config WDTPS = 32768
/* Zamek na pomet programu */
#pragma config CP0 = OFF
//----------------------------------------------------------------------------
/* Pocatek kodu. */
#pragma code
//----------------------------------------------------------------------------
/* Globalni konstanty. */
#define MIN_STRIDA 500    	//pocet pulsu hodin odpovidajici 50 mikrosekundám
#define MAX_STRIDA 9500   	//pocet pulsu hodin odpovidajici 950 mikrosekundám
#define STRED_STRIDA 5000   //pocet pulsu hodin odpovidajici 500 mikrosekundám
#define PERIODA_1_MS 10000	//pocet pulsu hodin odpovidajici 1ms
#define PERIODA_50_MS 50	//pro opetovnou inicializaci promene citac50ms
//--------------------------------------
/* Promenne pro transformaci zpravy ze vstupu do vystupu. */
T_uint8 delkaZpravy, index, pom;
//----------------------------------------------------------------------------
/* Globalni promenne */
/* Promenne definovane ve vtahovanem modulu. */
extern char x1;
extern int x2;
/* Globalni promenne definovane v modulu "main". */
uint8_t perioda_1ms;                //Priznak 1ms
uint8_t filtrovana_stopa_A, filtrovana_stopa_B;  //Promene pro ukladani filtrovane stopy A,B
uint8_t hodnota_uart=0;		//Promena pro ukladani vysledku z uartu
int8_t hodnota_uart1=0;		//Promena pro ukladani vysledku z uartu 
int16_t RG4_1=5000;			//Promena pro ukladani poctu pulsu po ktere ma byt pin rg v 1
T_filter_bin S1T,S2T,S3T,S4T,S5T,S6A,S6B;    //Promene pro filtrovani tlacitek                       
T_vyhodnoceni_tlacitka hodnota;   //Prom?ná pro ulkádání výsledku z inkrementálního tla?ítka
uint8_t citac50ms;			//Promena pro hlidani 50ms
uint8_t perioda_1ms=1;
int prijato[5];            //Promena pro ukladani zpravy z uartu 
uint16_t PocetImpPoz;       //Promena pro preneseni Poctu informaci na pozadi
uint24_t Perioda_otacek;    //Promena pro ulozeni celkove periody
/*Structy*/
//Priznakove bity
struct{
	unsigned ONE_MS:1;
	unsigned PWM_SEMAFOR:1;
    unsigned DIR:1;
    unsigned RUN:1;
    unsigned CCP3F:1;
    unsigned TMR3F:1;
}FLAGbits;

T_Capture_Value K0;
T_Capture_Value K1;
T_Button_Struct STAV;
T_Button_Struct BLOCK;
T_Button_Struct FILTROVANA;
/* Hlavni program. */
void main ()
{
/* O S C I L A T O R 
   Pro zvyseni 4x vnitrnich hodin je nutne:
    1.) Konfiguracni bit FOSC nastaven na HSPLL
    2.) Odblokovat PLL u hodin oscilatoru
*/
OSCTUNEbits.PLLEN = 1;
//Inicializace
InitVstupy();
InitVystupy();
InitTimer2();
InitTimer3();
InitCCP3();
InitCCP5();
InitRadicPreruseni();
InitFilterBin(&S6A);
InitFilterBin(&S6B);
InitFilterBin(&S4T);
InitFilterBin(&S1T);
InitFilterBin(&S2T);
InitFilterBin(&S3T);
InitFilterBin(&S5T);
Init_Vyhodnoceni_Tlacitka(&hodnota);
InitFLAGbits();
InitK();
InitStructTlacitka();
Timer2On();
Timer3On();
initUart_1(&uart_1);

//Program na pozadi
while (1){	
	if(perioda_1ms){    //doslo k preruseni nizke priority?
		perioda_1ms=0;	//vynuluj flagbit
		citac50ms--;	//odecita od 50 pro pocitani 50 ms
		//// filtrovani tlacitek a zapis na ledku	
        FilterBin(&S1T,PORTBbits.RB0);
        FilterBin(&S2T,PORTAbits.RA5);
        FilterBin(&S3T,PORTJbits.RJ7);
        FilterBin(&S4T,PORTJbits.RJ6);
        FilterBin(&S5T,PORTJbits.RJ5);      
		//ukladani do promene
        FILTROVANA.S1=S1T.result;
        FILTROVANA.S2=S2T.result;
        FILTROVANA.S3=S3T.result;
        FILTROVANA.S4=S4T.result;	
        FILTROVANA.S5=S5T.result;
        
        /*Tla?ítko S1, vstup RB0, výstup RF2 MUX*/
        if((!FILTROVANA.S1)&&(!BLOCK.S1)){                      //Pokud je stiskle tlacitko a neni blokovano1
            BLOCK.S1=1;
            STAV.S1^=1;
        }
        if((FILTROVANA.S1)&&(BLOCK.S1))                       //Odblokovvání tla?ítka po jeho uvoln?ní
            BLOCK.S1=0;
        
        /*Tlacitko S2, VSTUP RA5, výstup RA5 DIR*/
        if((!FILTROVANA.S2)&&(!BLOCK.S2)){
            BLOCK.S2=1;
            STAV.S2^=1;
        }
        if((FILTROVANA.S2)&&(BLOCK.S2))
            BLOCK.S2;
        
        /*Tlacitko S3 vstup RJ7 vystup RF7 DOWN, S4 vstup RJ5 výstup RJ4 */
        if((FILTROVANA.S3)){
            PORTFbits.RF7=!FILTROVANA.S5;
            PORTDbits.RD3=!FILTROVANA.S5;
        }
        if((FILTROVANA.S5)){
            PORTJbits.RJ4=!FILTROVANA.S3;
            PORTDbits.RD2=!FILTROVANA.S3;
        }
                
        /*Tlacitko S4 vstup RJ6 vystup RD7*/
        if((!FILTROVANA.S4)&&(!BLOCK.S4)){
            BLOCK.S4=1;
            STAV.S4^=1;
            PORTDbits.RD7^=1;
        }
        if((FILTROVANA.S4)&&(BLOCK.S4))
            BLOCK.S4=0;
        
             
		if ((!citac50ms)) {             //Kazdych 50 ms
			citac50ms = PERIODA_50_MS;	//reinicializace             	
            /* Je nova zprava? */
            if (isNewMsgUart(&uart_1)) {
                /* Minula zprava byla odeslana? */
                if (isEmptyUart(&uart_1)) {
                    /* Nacteni delky prijate zpravy.*/
                    delkaZpravy = getLengthMsgUart(&uart_1); //vraci 6, 4 nebo 2
                    for (index = 0; index < delkaZpravy; index++) {
                        /* Z prijate zpravy jsou postupne cteny jednotlive
                         * slova(typu byte).
                         */
                        prijato[index] = getByteFromMsgUart(&uart_1, index);
                        /* Jednoliva slova typu "BYTE" jsou ukladana jako 
                         * zprava k odeslani.
                         */
                        setByteToMsgUart(&uart_1, prijato[index], index); 	// index je poradove cislo bytu
                    }
                    hodnota_uart=prijato[3];   // zapis low bytu parametru 1
					/* Zapisem delky odesilane zpravy se startuje proces 
                     * vysilani zpravy po seriovem komunikacnim kanalu.
                     */
                    setLengthMsgUart(&uart_1, delkaZpravy);
                    /* Nazani priznaku nove prijate zpravy. */
                    clearFlagNewMsg(&uart_1);
                }
        	}				
		}
		if(STAV.S4){	//pokud je S4 v nule je vstupem inkrementalni cidlo
			LATDbits.LATD6=FilterBin(&S6A,PORTJbits.RJ0);   // filtrovani stopy A, zapis na ledku	
			LATDbits.LATD5=FilterBin(&S6B,PORTJbits.RJ1);   // filtrovani stopy B, zapis na ledku
			filtrovana_stopa_A=S6A.result;                  // uloz vysledek po filtraci
    		filtrovana_stopa_B=S6B.result;
			Vyhodnoceni_Tlacitka(&hodnota,filtrovana_stopa_A,filtrovana_stopa_B);   // Vyhodnoceni stop A,B, ulozeni do promene hodnota
			PORTH=hodnota.result;   //zapis hodnoty na port H
			/* Cela perioda (1ms) trva 10000 Pulsu (hodiny jsou 10MHz)
			 * pro hodnotu=0 musi byt strida 0.5, tedy 5000 pulsu hodin ve stavu 1 a 5000 pulsu hodin ve stavu 0
			 */
			FLAGbits.PWM_SEMAFOR=0;
            RG4_1=STRED_STRIDA+(((hodnota.result*227)/64)*10);	//chyba prepoctu na krajich +4 pulsy hodin
            FLAGbits.PWM_SEMAFOR=1;	
            HlidacHranicInk();
		}
		else{
			PORTH=hodnota_uart;
			hodnota_uart1=hodnota_uart;
            FLAGbits.PWM_SEMAFOR=0;
			RG4_1=STRED_STRIDA+(((hodnota_uart1*227)/64)*10);
           	FLAGbits.PWM_SEMAFOR=1;
			HlidacHranicUart();
		}
	}		    		
	uart(&uart_1);	
}            
Nop();
}
void InitVystupy(void){
    TRISH=0x00;         // port H je vystup
	PORTH=0x00;         // inicializace
    TRISD=0x00;         // port D je vystup
    PORTD=0x00;         // inicializace
   	TRISGbits.RG4=0;    // pin RG4 je vystup
    TRISEbits.RE0=0;    // pin RE0 a RE1 jsou vystupy
    TRISEbits.RE1=0;   
    PORTEbits.RE0=0;    // pin RE0 a RE1 jsou 0        
    PORTEbits.RE1=0;
    WDTCONbits.ADSHR=1; // Nastaveni portu A a F jako digitalni vystup
    ANCON0=0b11111111;
    ANCON1=0b11111111;
    WDTCONbits.ADSHR=0;
    TRISFbits.RF2=0;    //MUX out
    TRISFbits.RF5=0;    //DIR
    TRISFbits.RF7=0;    //DOWN
    TRISJbits.RJ4=0;    //UP
}   // Inicializace vstupu
void InitVstupy(void){
    TRISJbits.RJ0=1;    // piny RJ0 a RJ1 jsou vstupy
    TRISJbits.RJ1=1;
	TRISJbits.RJ6=1;    //S4
    TRISBbits.RB0=1;    //S1
    TRISAbits.RA5=1;    //S2
    TRISJbits.RJ7=1;    //S3
    TRISJbits.RJ5=1;    //S5
    TRISGbits.RG0=1;    //Stopa A koder
    TRISGbits.RG3=1;    //Stopa B koder
}   // Inicializace vystupu
void InitTimer2(void){
    T2CONbits.TMR2ON=0;     // vypnout timer 2
    T2CONbits.T2CKPS1=0;    // p?edd?li?ka 1:4 (10 MHz/4)
    T2CONbits.T2CKPS0=1;
    T2CONbits.T2OUTPS3=1;   // 1:10 (2,5 MHz/10)
    T2CONbits.T2OUTPS2=0;
    T2CONbits.T2OUTPS1=0;
    T2CONbits.T2OUTPS0=1;
    PR2=249;                // 250* 0.25 MHz=1ms
    PIR1bits.TMR2IF = 0;    //
    PIE1bits.TMR2IE = 1;
    IPR1bits.TMR2IP = 0;
}   // Inicializace timeru 2
void InitTimer3(void){
	T3CONbits.TMR3ON=0; 	// vypnout timer3
	T3CONbits.T3CKPS1=0;	// prescaler 1:1 (10MHz)
	T3CONbits.T3CKPS1=0;	
	T3CONbits.RD16=1;		// 16 bit, timer tedy pocita do 65536
	T3CONbits.T3CCP2=1;		// Timer 3 a 4 jsou zdroje pro vsechny eccp a ccp
	T3CONbits.T3CCP1=1;
	T3CONbits.TMR3CS=0; 		// interni hodiny
}
void InitCCP3(void){
CCP3CONbits.CCP3M3=0;		// Capture jenotka kazou nab. hranu
CCP3CONbits.CCP3M2=1;
CCP3CONbits.CCP3M1=0;
CCP3CONbits.CCP3M0=1;
IPR3bits.CCP3IP=1;			//High priority
PIE3bits.CCP3IE=1;			//Interpution enable(IE) od CCP3
}
void InitCCP5(void){
	PIE3bits.CCP5IE = 1;	//povoleni preruseni od komp. jednotky
	IPR3bits.CCP5IP = 1;	// vysoka priorita preruseni
	CCP5CONbits.CCP5M3 = 1; //Komparaèní režim s poèáteèním stavem 0 který pøeklápí na 1
  	CCP5CONbits.CCP5M2 = 0;
  	CCP5CONbits.CCP5M1 = 0;
  	CCP5CONbits.CCP5M0 = 0;
	CCPR5=PERIODA_1_MS;		//aby PWN zacala po 1 ms od spusteni
}
void InitRadicPreruseni(void){
    RCONbits.IPEN=1;        // Interupt priority enable (povoleni priority preruseni)
    INTCONbits.GIEH=1;      // Povoleni preruseni high priority
    INTCONbits.GIEL=1;      // Povoleni preruseni low priority
}   // Inicializace Radice preruseni
void Timer2On(void){
    T2CONbits.TMR2ON=1;     // timer zacne pocitat
}   // Zapnutí timeru2
void Timer3On(void){
    T3CONbits.TMR3ON=1;     // timer zacne pocitat
}   // Zapnutí timeru3
void HlidacHranicUart(void){
    if ((hodnota_uart==0b10000001)){ // Je hodnota -127 (tzn vsechny bity v 1)? zapni LED V14
        PORTEbits.RE0 = 1;
						}
    else if ((hodnota_uart==0b01111111)){ // Je hodnota 127 zapni LED V13
        PORTEbits.RE1 = 1; 
						} 
    else {				//Není ?ádná krajní hodnota? vypni LED V13 a V14
        PORTEbits.RE0 = 0;
		PORTEbits.RE1 = 0;
	}  
}
void HlidacHranicInk(void){
    if ((hodnota.result==0b10000001)){ // Je hodnota -127 (tzn vsechny bity v 1)? zapni LED V14
        PORTEbits.RE0 = 1;
						}
    else if ((hodnota.result==0b01111111)){ // Je hodnota 127 zapni LED V13
        PORTEbits.RE1 = 1; 
						} 
    else {				//Není ?ádná krajní hodnota? vypni LED V13 a V14
        PORTEbits.RE0 = 0;
		PORTEbits.RE1 = 0;
	}  
}
void ResetHranic(void){
	PORTEbits.RE0 = 0;
	PORTEbits.RE1 = 0;
}
void InitFLAGbits(void){
	FLAGbits.ONE_MS=1;
	FLAGbits.PWM_SEMAFOR=1;
    FLAGbits.RUN=0;
}
void InitStructTlacitka(void){
    STAV.CELA=0;
    BLOCK.CELA=0;
}
void InitK(void){
    K0.CCPRL=0;
    K0.CCPRH=0;
    K0.TIMER=0;
    K1.CCPRL=0;
    K1.CCPRH=0;
}
//----------------------------------------------------------------------------
/* Vektor preruseni vyssi priority. */
#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh (void)
{
  _asm
    /* Skok na obsluzny program preruseni. */
    goto InterruptHandlerHigh
  _endasm
}
//----------------------------------------------------------------------------
/* Vektor preruseni nizsi priority. */
#pragma code InterruptVectorLow = 0x18
void InterruptVectorLow (void)
{
  _asm
    /* Skok na obsluzny program preruseni. */
    goto InterruptHandlerLow
  _endasm
}
//----------------------------------------------------------------------------
/* Podprogram obsluhy vyssiho preruseni. */
#pragma code
#pragma interrupt InterruptHandlerHigh

void InterruptHandlerHigh ()
{
/* Start podprogramu obsluhy preruseni. */
	uint16_t next_event;
	uint16_t RG4_1p;
    uint16_t Pocet_Imp;  //Lokalni promena pro ukladani rozdilu K0-K1
	next_event=CCPR5;
	if(FLAGbits.PWM_SEMAFOR)RG4_1p=RG4_1;
	PIR3bits.CCP5IF = 0;	// vynuluj priznak
    if (PORTGbits.RG4) { //  Pokud je výstup 1
    	CCP5CONbits.CCP5M0 = 1; //Pøíští hrana bude sestupná
		//FLAGbits.CELA_PERIODA=0;
      	next_event += PERIODA_1_MS-RG4_1p;	//pricti do registru kompare jednotky pocet pulsu po ktere ma byt rg4 v 0
		PORTDbits.RD0 = 0;
   	}
    else{   //Pokud je výstup 0
      	CCP5CONbits.CCP5M0 = 0; //Pøíští hrana bude nábìžná
		//FLAGbits.CELA_PERIODA=1;
      	next_event += RG4_1p;	//pricti do registru kompare jednotky pocet pulsu po ktere ma byt rg4 v 1
		PORTDbits.RD0 = 1;
	}
	CCPR5H=next_event>>8;
	CCPR5L=next_event;    
    // Mereni otacek
    if(PIR3bits.CCP3IF==1||PIR2bits.TMR3IF==1){
        //Ulozeni do lok. prom.
        FLAGbits.CCP3F=PIR3bits.CCP3IF;
        FLAGbits.TMR3F=PIR2bits.TMR3IF;
        
        //Zvolení sm?ru otá?ek... dát do ifu (FLAGbits.RUN)
        if(FLAGbits.CCP3F&&!PORTGbits.RG3)
            FLAGbits.DIR=0;
        if(FLAGbits.CCP3F&&PORTGbits.RG3)
            FLAGbits.DIR=1;
        //Je rotor v klidu?
        if(!FLAGbits.RUN){
            //Vyhodnocení rychlosti A
            //Nastala soucasna udalost?
            if(FLAGbits.CCP3F&&FLAGbits.TMR3F){
                K1.CCPRL=CCPR3L;
                K1.CCPRH=CCPR3H;
                //P?i?lo p?eru?ení d?íve od CCP?
                if(K1.CCPRH>0b10000000){
                    K1.TIMER=1;    
                }
                //P?i?lo p?eru?ení nejd?íve od TMR
                else
                    K1.TIMER=0;
                //Rotor se neto?í
                FLAGbits.RUN=0;
                //Nulování p?íznak?
                PIR3bits.CCP3IF=0;
                FLAGbits.TMR3F=0;
            }
            //Nenastala soucasna udalost (A)
            else{
                K0.TIMER=0;     //Má tam být K0?
                //P?i?la náb??ná hrana?
                if(FLAGbits.CCP3F){
                    K1.CCPRL=CCPR3L;
                    K1.CCPRH=CCPR3H;
                    FLAGbits.RUN=1;
                    PIR3bits.CCP3IF=0;
                }
               //Nep?i?la náb??ná hrana
                else{
                    PIR2bits.TMR3IF=0;
                }
            }
        }
        //Rotor není v klidu vyhodnocení rychlosti B
        else{
            K0.CCPRL=CCPR3L;
            K0.CCPRH=CCPR3H;
            //Nastala sou?asná událost?
            if(FLAGbits.CCP3F&&FLAGbits.TMR3F){
                //P?i?lo p?eru?ení nejd?íve od CCP?
                if(K0.CCPRH>0b01111111){
                    Pocet_Imp=K0.CELA-K1.CELA;
                    K1=K0;
                    K0.TIMER=1;
                }
                //P??lo p?eru?ení nejd?íve od TMR
                else{
                    K0.TIMER+=1;
                    //To?í se rotor? ... nastalo víc jak 20 p?ete?ení?
                    //Timer p?ete?e ka?dou ms => min. frekvence 50Hz
                    if(K0.TIMER>3){
                        FLAGbits.RUN=0; //Rotor se neto?í                                                
                    }
                    //Rotro se to?í
                    else{
                        Pocet_Imp=K0.CELA-K1.CELA;
                        K1=K0;
                        K0.TIMER=0;
                    }                      
                }
            }
            //Nenastala sou?asná událost (B)
            else{
                //P?etekl timer?
                if(FLAGbits.TMR3F){ 
                    K0.TIMER+=1;
                    //To?í se rotor?
                    if(K0.TIMER>3)
                        FLAGbits.RUN=0;
                    //Nulování p?íznaku
                    PIR2bits.TMR3IF=0;                    
                }
                else{
                    Pocet_Imp=K0.CELA-K1.CELA;
                    K1=K0;
                    K0.TIMER=0;
                    PIR3bits.CCP3IF=0;
                }
            }
        }
        PocetImpPoz=Pocet_Imp;    
    }
}
//----------------------------------------------------------------------------
// Podprogram obsluhy vyssiho preruseni
#pragma code
#pragma interruptlow InterruptHandlerLow
void InterruptHandlerLow ()
{
/* Start podprogramu obsluhy preruseni. */
	perioda_1ms=1;         // priznak preuseni pro program na pozadi
	PIR1bits.TMR2IF = 0;    // vynulovani priznaku preruseni od timeru2   
	isrUart_1(&uart_1);
}
//----------------------------------------------------------------------------
