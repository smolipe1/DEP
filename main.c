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
void InitCCP5(void);
void InitRadicPreruseni(void);
void InitFLAGbits(void);
void Timer2On(void);
void Timer3On(void);
void HlidacHranicUart(void);
void HlidacHranicInk(void);
void ResetHranic(void);
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
uint16_t pocitadlo_1ms;             //Premena pro pocitani milisekund
uint8_t filtrovana_stopa_A, filtrovana_stopa_B;  //Promene pro ukladani filtrovane stopy A,B
uint8_t filtrovana_S4=0;	//Promena pro ukladani stavu tlacitka S4 po filtraci
uint8_t hodnota_uart=0;		//Promena pro ukladani vysledku z uartu
int8_t hodnota_uart1=0;		//Promena pro ukladani vysledku z uartu 
uint8_t znamenko_uart=0;	//Promena pro ukladani znamenka z uartu 1 je minus
int16_t RG4_0=5000;			//Promena pro ukladani poctu pulsu po ktere ma byt pin rg v 0
int16_t RG4_1=5000;			//Promena pro ukladani poctu pulsu po ktere ma byt pin rg v 1
uint8_t btn_S4=0;			//Promena pro blokovani tlacitka s4
uint8_t stav_S4=0;			//Promane pro ulkadani stavu tlacitka s4
T_filter_bin S6A,S6B,S4T;    //Promene pro filtrovani tlacitek                       
T_vyhodnoceni_tlacitka hodnota;   //Prom?ná pro ulkádání výsledku z inkrementálního tla?ítka
T_uint8 citac50ms;			//Promena pro hlidani 50ms
uint8_t perioda_1ms=1;
int prijato1[5];		//Promena pro ukladani zpravy z uartu

/*pøíznakové bity*/
struct{
	unsigned ONE_MS;
	unsigned CELA_PERIODA;
}FLAGbits;
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
InitCCP5();
InitRadicPreruseni();
InitFilterBin(&S6A);
InitFilterBin(&S6B);
InitFilterBin(&S4T);
Init_Vyhodnoceni_Tlacitka(&hodnota);
InitFLAGbits();
Timer2On();
Timer3On();
initUart_1(&uart_1);
//Program na pozadi
while (1){	
	if(perioda_1ms){    //doslo k preruseni nizke priority?
		perioda_1ms=0;	//vynuluj flagbit
		citac50ms--;	//odecita od 50 pro pocitani 50 ms		
		LATDbits.LATD7=FilterBin(&S4T,PORTJbits.RJ6);	// filtrovani tlacitka 4, zapis na ledku
		filtrovana_S4=S4T.result;	//nejspise zbytecna operace :)
		if ((!(filtrovana_S4))&&(!btn_S4)){  // Pokud je tlaèítko stlaèeno a neblokováno 
            btn_S4 = 1; // Zablokuje tlaèítko 
        	if (stav_S4 == 0){ // zámìna stavu (mezi 0 a 1) 
        		stav_S4 = 1;
				ResetHranic();				
	 		}
        	else{ 
          		stav_S4 = 0;
				ResetHranic();
			}
      	}
      	if ((filtrovana_S4)&&(btn_S4)) { // Pokud je tlaèítko uvolnìno a zblokováno 
      		btn_S4 = 0; // Odblokuje
      	}
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
                        prijato1[index] = getByteFromMsgUart(&uart_1, index);
                        /* Jednoliva slova typu "BYTE" jsou ukladana jako 
                         * zprava k odeslani.
                         */
                        setByteToMsgUart(&uart_1, prijato1[index], index); 	// index je poradove cislo bytu
                    }
                    hodnota_uart=prijato1[3];   // zapis low bytu parametru 1
					znamenko_uart=prijato1[2];  // zapis high bytu prametru 1
                    /* Zapisem delky odesilane zpravy se startuje proces 
                     * vysilani zpravy po seriovem komunikacnim kanalu.
                     */
                    setLengthMsgUart(&uart_1, delkaZpravy);
                    /* Nazani priznaku nove prijate zpravy. */
                    clearFlagNewMsg(&uart_1);
                }
        	}				
		}
		if(!stav_S4){	//pokud je S4 v nule je vstupem inkrementalni cidlo
			PORTDbits.RD7=1;		//ledka d8 ukazuje ze vstup je ink cidlo
			LATDbits.LATD6=FilterBin(&S6A,PORTJbits.RJ0);   // filtrovani stopy A, zapis na ledku	
			LATDbits.LATD5=FilterBin(&S6B,PORTJbits.RJ1);   // filtrovani stopy B, zapis na ledku
			filtrovana_stopa_A=S6A.result;                  // uloz vysledek po filtraci
    		filtrovana_stopa_B=S6B.result;
			Vyhodnoceni_Tlacitka(&hodnota,filtrovana_stopa_A,filtrovana_stopa_B);   // Vyhodnoceni stop A,B, ulozeni do promene hodnota
			PORTH=hodnota.result;   //zapis hodnoty na port H
			/* Cela perioda (1ms) trva 10000 Pulsu (hodiny jsou 10MHz)
			 * pro hodnotu=0 musi byt strida 0.5, tedy 5000 pulsu hodin ve stavu 1 a 5000 pulsu hodin ve stavu 0
			 */
            RG4_1=STRED_STRIDA+(((hodnota.result*227)/64)*10);	//chyba prepoctu na krajich +4 pulsy hodin
            RG4_0=PERIODA_1_MS-RG4_1;	
            /*if(hodnota.result>=0){
                RG_1=STRED_STRIDA+(((hodnota.result*227)>>6)*10);	//chyba prepoctu na krajich +4 pulsy hodin
                RG_0=PERIODA_1_MS-RG_1;	
            }
            else{
                kladna_hodnota_ink=hodnota.result^0b11111111;
                kladna_hodnota_ink+=1;
                RG_1=STRED_STRIDA-(((kladna_hodnota_ink*227)>>6)*10);	//chyba prepoctu na krajich +4 pulsy hodin
                RG_0=PERIODA_1_MS-RG_1;
            }*/
			HlidacHranicInk();
		}
		else{
			PORTDbits.RD7=0;		//ledka d8 ukazuje že vstup je ink cidlo
			PORTH=hodnota_uart;
			hodnota_uart1=hodnota_uart;
            RG4_1=STRED_STRIDA+(((hodnota_uart1*227)/64)*10);
            RG4_0=PERIODA_1_MS-RG4_1;            	
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
    PORTEbits.RE0=0;    // pint RE0 a RE1 jsou 0        
    PORTEbits.RE1=0;
}   // Inicializace vstupu
void InitVstupy(void){
    TRISJbits.RJ0=1;    // piny RJ0 a RJ1 jsou vstupy
    TRISJbits.RJ1=1;
	TRISJbits.RJ6=1;
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
	FLAGbits.CELA_PERIODA=1;
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
	next_event=CCPR5;
	PIR3bits.CCP5IF = 0;	// vynuluj priznak
    if (PORTGbits.RG4) { //  Pokud je výstup 1
    	CCP5CONbits.CCP5M0 = 1; //Pøíští hrana bude sestupná
		//FLAGbits.CELA_PERIODA=0;
      	next_event += RG4_0;	//pricti do registru kompare jednotky pocet pulsu po ktere ma byt rg4 v 0
		PORTDbits.RD0 = 0;
   	}
    else{   //Pokud je výstup 0
      	CCP5CONbits.CCP5M0 = 0; //Pøíští hrana bude nábìžná
		//FLAGbits.CELA_PERIODA=1;
      	next_event += RG4_1;	//pricti do registru kompare jednotky pocet pulsu po ktere ma byt rg4 v 1
		PORTDbits.RD0 = 1;
	}
	CCPR5H=next_event>>8;
	CCPR5L=next_event;
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
