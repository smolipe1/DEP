/* 
 * Soubor:      boolean.h
 * Autor:       ML
 *
 * Spolecnost:  CVUT-FEL-K13114
 * Varianta:    1.0
 * Zmena:       --
 * Kompilator:  C18-V3.47
 * Procesor:    --
 */

#ifndef _boolean_H
#define	_boolean_H
//----------------------------------------------------------------------------
/* Systemove hlavickove soubory: */
//----------------------------------------------------------------------------
/* Vlastni hlavickove soubory: */
//----------------------------------------------------------------------------
/* Definice symbolickych konstant vyuzivanych v jinych modulech: */
#define CLEAR 0
#define SET 1
//
#define LOW_PRIORITY 0
#define HIGH_PRIORITY 1
//
#define ENABLE_INTERRUPTS 1
#define DISABLES_INTERRUPTS 0
//----------------------------------------------------------------------------
/* Definice maker s parametry: */
//----------------------------------------------------------------------------
/* Definice globalnich typu: */
typedef enum {FALSE,TRUE} T_boolean;

//----------------------------------------------------------------------------
/* Deklarace globalnich promennych modulu: */
// extern typ jmeno;
//----------------------------------------------------------------------------
/* Uplne funkcni prototypy globalnich funkci modulu: */
//----------------------------------------------------------------------------
#endif	/* _boolean_H */
