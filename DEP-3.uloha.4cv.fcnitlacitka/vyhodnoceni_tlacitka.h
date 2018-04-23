/* 
 * File:   vyhodnceni tlacitka
 * Author: smolik
 *
 * Created on 2. b?ezna 2018, 15:45
 */

#ifndef VYHODNOCENI_TLACITKA_H
#define	VYHODNOCENI_TLACITKA_H
#include "boolean1.h"
#include "integer.h"
typedef enum{S0,S1,S2,S3,S4}T_vyhodnoceni_tlacitka_state; // vytvo? prom?nou T_vyhodnocení_state

typedef struct{     
T_vyhodnoceni_tlacitka_state state;
int8_t result;
}T_vyhodnoceni_tlacitka;

void Init_Vyhodnoceni_Tlacitka(T_vyhodnoceni_tlacitka* vysledek); //Predpis pro inicializaci 
int8_t Vyhodnoceni_Tlacitka(T_vyhodnoceni_tlacitka* vysledek,uint8_t dataA,uint8_t dataB); //Predpis pro fci vstup data ze stop A,B vystup - vysledek

#endif	/* VYHODNOCENI_TLACITKA_H */

