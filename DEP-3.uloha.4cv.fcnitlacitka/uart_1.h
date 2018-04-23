/*
Soubor:     uart_1.h
Autor:      ML
Datum:      25.2.2016
Spolecnost: CVUT-FEL-K13114
Varianta:   1.0
Zmena:      --
Procesor:   PIC18F87J11
 */
//----------------------------------------------------------------------------
#ifndef _UART_1_H
#define _UART_1_H
//----------------------------------------------------------------------------
/* Vtahovane soubory */
#include <p18f87j11.h>
#include "boolean.h"
#include "integer.h"
//
//----------------------------------------------------------------------------
/* Definice konstant */
//--------------------------------
#define ARRAY_LENGTH 10
//--------------------------------
/* Definovani delky zasobniku typu FIFO. */
#define FIFO_LENGTH 32
#define FIFO_MASK 0x1F
//--------------------------------
/* Pocet pomocnych pruchodu pro odeslani celeho obsahu fronty. */
#define UART_INIT_COUNTER 3
/* Znak pocatku zpravy */
#define SOF '*'
//----------------------------------------------------------------------------
/* Definice typu */
//--------------------------------

/* Struktura pole pro predavani zprav. */
typedef struct {
    T_uint8 length;
    T_uint8 data[ARRAY_LENGTH];
} T_array;
//--------------------------------
/* FIFO */

/* Slouzi k ulozeni stavovych informaci:
 * ovf...FIFO full/overflow,
 * udf...FIFO underflow,
 */
typedef struct {
    unsigned ovf : 1;
    unsigned udf : 1;
    unsigned : 6;
} T_fifoState;

/*
 * byte pro ulozeni stavovych informaci o fronte,
 * celkovy pocet obsazenych byt-u,
 * aktualni poloha indexu zapisujici vstupni byte,
 * aktualni poloha indexu ze krere je provadeno cteni byte,
 * vlastni fronta.
 * Popis:
 *     1.) delka = 0, identicka hodnota indexu, fronta je prazdna
 *     2.) zapis do fronty:
 *     2.1.) pocet polozek < max. pripustna hodnota
 *     2.2.) muhu zapsat hodnotu do fronty, nasledne posun indexu a inkrementace
 *           poctu bytu, je-li pocet roven max. hodnote delky nastalo preteceni.
 *     3.) cteni z fronty:
 *     3.1.) pocet polozek ve fronte = 0
 *     3.2.) neni co cist
 *     3.3.) mohu cist, provedu cteni, a nasledne posunu index a dekrementuji
 *           pocet polozek.
 */
typedef struct {
    T_fifoState state;
    T_uint8 length;
    T_uint8 indexIn;
    T_uint8 indexOut;
    T_uint8 fifo[FIFO_LENGTH];
} T_fifo;
//--------------------------------

/* */
typedef struct {
    unsigned sendMsg : 1;
    unsigned sendEnd : 1;
    unsigned newMsg : 1;
    unsigned newMsgOvf : 1;
    unsigned uart_1 : 1;
    unsigned uart_2 : 1;
    unsigned : 2;
} T_uartFlag;

/* */

typedef enum {
    START,
    MESSAGE_LENGTH_H, MESSAGE_LENGTH_L,
    DATA_HH, DATA_HL, DATA_LH, DATA_LL,
    CRC_H, CRC_L
} T_uartState;

/* */
typedef struct {
    T_uartFlag flag;
    T_uartState state;
    T_uint8 counter;
    T_uint8 length;
    T_fifo fifoTxD;
    T_fifo fifoRxD;
    T_array msgInput;
    T_array msgOutput;
    T_uint8 quantityCharactersInput;
    T_uint8 lengthMsgInput;
    T_uint8 indexMsgInput;
    T_uint8 suma;
    T_uint8 x;
} T_uart;
//----------------------------------------------------------------------------
/* Definici prototypu */
//--------------------------------
/*  A R R A Y  */
/* Mazani obsahu zpravy.
 * Vstup: 
 *      p_Array...ukazatel na pole.
 */
void clearArray(T_array *p_Array);

/* Procedure pro plneni pole zpravy po jednotlivych BYT-tech.
 * Vstup:
 *   p_Array...ukazatel na pole.
 *   value.....vkladana hodnota <0 - 255>
 *   index.....pozadovana poloha vkladane hodnoty v poli <0 - (ARRAY_LENGTH - 1)>
 */
void setByteToArray(T_array *p_Array, T_uint8 value, T_uint8 index);

/* Procedura pro zjisteni hodnoty ulozene na definovanem miste pole.
 * Vstup:
 *   p_Array...ukazatel na pole.
 *   index.....pozadovana poloha vkladane hodnoty v poli <0 - (ARRAY_LENGTH - 1)>
 * Vystup:
 *   hodnota, ktera je ulozena v pozadovanem miste pole.
 */
T_uint8 getByteFromArray(T_array *p_Array, T_uint8 index);

/* Procedura nastaveni delky zpravy ulozene v poli konstantni delky. Delka 
 * zpravy v poli je od "0" do "(ARRAY_LENGTH - 1)".
 * Vstup:
 *   p_Array...ukazatel na pole.
 *   length....pozadovana delka pole <0 - (ARRAY_LENGTH - 1)>
 *             length = 0.. priznak prazdneho pole
 */
void setArrayLength(T_array *p_Array, T_uint8 length);

/* Procedura informuje o aktualni hodnote delky pole, urcuje pocet BYT-u 
 * ulozenych v poli.
 * Vstup:
 *   p_Array...ukazatel na pole.
 * Vystup:
 *   pocet informacnich BYT-u v poli.
 */
T_uint8 getArrayLength(T_array *p_Array);

//--------------------------------
/*  F I F O  */
/* Inicializece datove struktury.
 * Vstup:
 *   p_Fifo...ukazatel na strukturu vyrovnavaci pameti typu FIFO.
 */
void initFifo(T_fifo *p_Fifo);

/* Procedura vkladajici jeden BYTE do vyrovnavaci pameti FIFO.
 * Vstup:
 *   p_Fifo...ukazatel na strukturu vyrovnavaci pameti typu FIFO.
 *   value....vkladana hodnota do vyrovnavaci pameti.
 */
void addByteToFifo(T_fifo *p_Fifo, T_uint8 value);

/* Procedura odbere nejstarsi hodnotu ulozenou ve vyrovnavaci pameti.
 * Pred pouzitim teto metody je nutne overeni prostrednictvim procedury
 * "lengthFifo" zda vyrovnavaci pamet obsahuje nejake informacni BYT-y.
 * Vstup:
 *   p_Fifo...ukazatel na strukturu vyrovnavaci pameti typu FIFO.
 * Vystup:
 *   Je vracena nejstarsi hodnota ulozena ve vyprovnavaci pameti.
 */
T_uint8 removeByteFromFifo(T_fifo *p_Fifo);

/* Procedura vraci aktualni mnozstvi informacnich BYT-u ve vyrovnavaci pameti.
 * Vstup:
 *   p_Fifo...ukazatel na strukturu vyrovnavaci pameti typu FIFO.
 * Vystup:
 *  Je vracena aktualni pocet informacnich BYT-u ve vyrovnavaci pameti.
 */
T_uint8 lengthFifo(T_fifo *p_Fifo);

//--------------------------------
/*  T R A N S F O R M A C E  -  H E X  */
/* Procedura testuje zda vstupni hodnota je hodnotou z predepsane mnoziny hodnot.
 * Vstup:
 *   value...ukazatel na promennou s testovanou hodnotou.
 * Vystup:
 *  Je vracena logicka hodnota TRUE=1 v pripade, ze hodnota je soucasti 
 *  predepsane mnoziny. 
 */
T_boolean isHexChar(T_uint8 *value);

/* Procedura provadi trnaformaci znaku ASCII <0,9>, <A,F> na prislusnou hodnotu.
 * Vstup:
 *    value...kod znaku ASCII
 * Vystup:
 *  ciselna hodnota prislusejici vstupnimu znaku ASCII. 
 *  Znak '0'=0D
 *  Znak 'A'=10D
 *  Znak 'F'=15D
 */
T_uint8 hex2Num(T_uint8 value);

/*Procedura provadi transformaci hodnoty z intervalu <0, 15> na prislusny znak
 * ASCII.
 * Vstup:
 *   value...hodnota z intervalu <0,15>, ktera bude transformovana.
 * Vystup:
 *   Vystupem je kod ASCII znaku
 *   Hodnota 0D = znak '0'
 *   Hodnota 10D = znak 'A'
 *   Hodnota 15D = znak 'F'
 */
T_uint8 num2Hex(T_uint8 value);

//--------------------------------
/*  U A R T  */
/* Inicializace UART_1
 *  Prenosova rychlost 9600 Baude za predpokladu nastavene hodinove frekvence
 *  f_osc=40 MHz
 *  Mod 8 bit,  (1 start + 8 data + 1 stop)
 *  parita neni pouzita
 *  Asynchronni mod cinnosti
 *  Pracuje na prerusovacim vektoru nizke urovne.
 */
void initUart_1(T_uart *p_Uart);

/* Procedura realizuje zakodovani vysilane zpravy do ramce vysilaneho seriovym 
 * prenosovym kanalem. Zaroven procedura provadi dekodovani ramce do prijate 
 * zpravy.
 * Procedura je vyhradne urcena pro beh na pozadi!
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 */
void uart(T_uart *p_Uart);

/* Procedura je urcena pro testovani, zda seriovym komunikacnim kanalem byla 
 * obdrzena nova zprava a tato zprava byla spravne dekodovana.
 * Tato procedura se musi volat pred vyzvedavanim zpravy!
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 * Vystup:
 *   Je vracena logicka hodnota TRUE=1 v pripade, ze je pripravena dekodovana 
 *   zprava.
 */
T_boolean isNewMsgUart(T_uart *p_Uart);

/* Procedura slouzi k nastaveni logicke hodnoty FALSE=0 do priznaku nove zpravy.
 * Procedura musi byt volana v pripade, ze provadim vyzvedavani jednotlivych 
 * byt-u z prijate a dekodovane zpravy.
 * Procedura bude pouzivana v pripade pouziti "getByteFromMsgUart".
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 */
//void clearNewMsgUart(T_uart *p_Uart);
void clearFlagNewMsg(T_uart *p_Uart);

/* Procedura slouzi ke kopirovani dekodovanou zpravu do promenne typu "T_array",
 * ktera bude obsahovat dekodovanou zpravu.
 * Procedura po provedenem kopirovani prijate zpravy provadi nastaveni logicke 
 * hodnoty FALSE=0 do priznaku nove zpravy. Neni nutne volat prislusnou 
 * proceduru. 
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 * Vystup:
 *   p_Array..ukazatel na strukturu typu pole ("T_array")
 */
void getMsgUart(T_uart *p_Uart, T_array *p_Array);

/* Procedura slouzi ke kopirovani jedotlivych BYT-u z prijate a dekodovane 
 * zpravy. 
 * Pred pouzitim teto procedury se musi urcit aktualni delka prijate zpravy 
 * prostrednictvim procedury "getLengthMsgUart".
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 *   index....specifikuje pozadovany BYTE.
 *            Hodnota musi byt z intervalu <0,delkaZpravy-1>
 * Vystup:
 *    hodnota, ktera je ulozena v pozadovanem miste dekodovane zpravy.
 */
T_uint8 getByteFromMsgUart(T_uart *p_Uart, T_uint8 index);

/* Procedura je urcena pro ziskani aktualni delky prijate zpravy.
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 * Vystup:
 *   Je vracena aktualni pocet informacnich BYT-u v prijate zprave.
 */
T_uint8 getLengthMsgUart(T_uart *p_Uart);

/* Procedura testuje zda vysilana zprava byla jiz odeslana.
 * Tato procedura musi byt volana pred vlastnim ctenim prijate zpravy.
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 * Vystup:
 *   Je vracena logicka hodnota TRUE=1 v pripade, ze zprava byla odeslana. Pouze
 *   v pripade priznaku TRUE je mozne zapsat do vysilaci promenne novou zpravu.
 */
T_boolean isEmptyUart(T_uart *p_Uart);

/* Procedura je urcena ke kopiruvani vytvorene vysilane zpravu do vnitrniho pole
 * urceneho pro odesilanou zpravu. 
 * Kopirovani je pripustne pouze v pripade, ze procedura "isEmptyUart" vrati 
 * logickou hodnotu TRUE. Tato hodnota informuje, ze zprava byla zakodovana a 
 * presunuta do vystupni vyrovnavaci pameti jednotky UART.
 * Procedura po uspesnem provedenem kopirovani nastavuje delku vystupni zpravy
 * na hodnotu "0".
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 *   p_Array..ukazatel na promennou typu "T_array", ktera je urcean pro ulozeni
 *            odesilane zpravy seriovym komunikacnim kanalem.
 * Vystup:
 *   Vystupem je logicka hodnota, ktera informuje zda kopirovani bylo uspesne.
 *             Navratova logicka hodnota TRUE = kopirovani bylo uspesne.
 *             Navratova logicka hodnota FALSE = minula vystupni zprava jeste
 *             nebyla zakodovana a odeslana do serioveho komunikacniho kanalu.
 */
T_boolean setMsgToUart(T_uart *p_Uart, T_array *p_Array);

/*Procedure je urcena ke kopirovani jedntlivych BYT-u prenasene zpravy do 
 * vnitrniho pole urceneho pro odesilanou zpravu. Poloha zapisovaneho BYT-u v 
 * poli odesilane zpravy je stanovena promennou "index".
 * Po provedeni sestaveni zpravy ve specifikovanem poli pro vysilanou zpravu 
 * musi se provest nastaveni delky zpravy prostrednictvim procedury 
 * "setLengthMsgUart". Nastavenim delky zpravy je priznakem pro start kodovani
 * zpravy do ramce a presum famce do vystupni vyrovanvaci pameti jednotku UART.
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 *   value....informacni BYTE vystupni zpravy.
 *   index....pozadovana poloha vkladaneho informacniho BYT-u
 *            interval hodnot indexu je <0, (ARRAY_LENGTH - 1)>
 * Vystup:
 *   Vystupni logicka hodnota:
 *            TRUE = hodnota byla uspesne ulozena do pole vystupni zpravy.
 *            FALSE = hodnota promenne "index" je mimo predepsany interval,
 *            FALSE = pokus o zapis BYT-u do vystupniho pole, ktere jeste nebylo
 *                    zakodovano a presunuto do vystupni vyrovnavaci pameti
 *                    jednotky UART.
 */
T_boolean setByteToMsgUart(T_uart *p_Uart, T_uint8 value, T_uint8 index);

/* Procedura slouzi k nasteveni delky vysilane zpravy.
 * Procedura musi byt volana az po nasteveni vsech informcnich BYT-u v poli
 * pro vysilanou vystupni zpravu prostrednictvim procedury "setByteToMsgUart".
 * Nastaveni delky zpravy je priznakem pro start kodovani zpravy do ramce a pro
 * presunuti zakodovane zpravy do vystupni vyrovnavaci pameti.
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 *   length...pocet informacnich BYT-u vysilane zpravy.
 *            interval pripustych hodnot je <0,ARRAY_LENGTH>
 */
T_boolean setLengthMsgUart(T_uart *p_Uart, T_uint8 length);

/* Lokalni procedura, ktera transformuje vytvorenou vystupni zpravu do ramce 
 * podle specifikace. 
 * Transformace je provedena v pripade, ze je specifikovan pocet informacnich 
 * BYT-u vystupni zpravy ruzny od hodnoty "0".
 * Vytvareny ramec je postupne ukladan do vyrovnavaci pameti typu FIFO.  
 * Transformace je pripustne pouze v pripade, ze minula vystupni zprava byla
 * jiz cela zakodovana a presunuta do vystupni vyrovanvaci pameti jednotky UART.
 * Vstup:
 *   p_Uart...ukazatel na strukturu promennych urcenych pro obsluhi serioveho 
 *            komunikacniho kanalu.
 */
void sendMsgUart(T_uart *p_Uart);

/* Procedura je urcena pro praci v obsluzne procedure prerusovaciho systemu.
 * Procedura provadi presun informace z registru prijmu modulu UART do prijimaci
 * vyrovnavaci pameti typu FIFO a soucasne provadi presun informaci z vystupni
 * vyrovanvaci pameti typu FIFO do vysilaciho registru modulu UART. */
void isrUart_1(T_uart *p_uart);

//----------------------------------------------------------------------------
/* Definice maker */
//--------------------------------
//----------------------------------------------------------------------------
/* Definice globalni promenne. */
//--------------------------------
//extern T_uart uart_1;
//--------------------------------

//----------------------------------------------------------------------------
#endif /* konec _UART_1_H */

