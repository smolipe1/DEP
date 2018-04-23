#include "vyhodnoceni_tlacitka.h"

void Init_Vyhodnoceni_Tlacitka(T_vyhodnoceni_tlacitka* vysledek){
vysledek->state=S0;
vysledek->result=0;
}
int8_t Vyhodnoceni_Tlacitka(T_vyhodnoceni_tlacitka* vysledek,uint8_t dataA,uint8_t dataB){   
     switch(vysledek->state){
      uint8_t predchozi_data_A=0;  //Promena pro porovnavani predchozich stavu
      uint8_t predchozi_data_B=0;  
	case S0:{
      uint8_t predchozi_data_A=dataA;  
      uint8_t predchozi_data_B=dataB;
		if((dataA==0)&&(dataB==0)){
		vysledek->state=S1;
		}
		if((dataA==1)&&(dataB==0)){
		vysledek->state=S2; 
        }
        if((dataA==1)&&(dataB==1)){
		vysledek->state=S3;
		}
        if((dataA==0)&&(dataB==1)){
		vysledek->state=S4;
		}
      break;
		}
	case S1:{
        if((0==dataA)&&(0!=dataB)&&(vysledek->result>-127)){
		vysledek->state=S4;
        vysledek->result--;
		}
		if((0!=dataA)&&(0==dataB)&&(vysledek->result<127)){
		vysledek->state=S2;
        vysledek->result++;
		}
		if((vysledek->result==-127)||(vysledek->result==127)){
		vysledek->state=S0;
		break;
		}
       	break;
		}
	case S2:{
		
		if((1==dataA)&&(0!=dataB)&&(vysledek->result<127)){
		vysledek->state=S3;
        vysledek->result++;
		}
		if((1!=dataA)&&(0==dataB)&&(vysledek->result>-127)){
		vysledek->state=S1;
        vysledek->result--;
		}
		if((vysledek->result==-127)||(vysledek->result==127)){
		vysledek->state=S0;
		break;
        }
		break;
		}
	case S3:{
		
		if((1==dataA)&&(1!=dataB)&&(vysledek->result>-127)){
		vysledek->state=S2;
        vysledek->result--;
		}
		if((1!=dataA)&&(1==dataB)&&(vysledek->result<127)){
		vysledek->state=S4;
        vysledek->result++;
		}
		if((vysledek->result==-127)||(vysledek->result==127)){
		vysledek->state=S0;
		break;
        }
		break;
		}
    case S4:{
		
		if((0==dataA)&&(1!=dataB)&&(vysledek->result<127)){
		vysledek->state=S1;
        vysledek->result++;
		}
		if((0!=dataA)&&(1==dataB)&&(vysledek->result>-127)){
		vysledek->state=S3;
        vysledek->result--;
		}
        if((vysledek->result==-127)||(vysledek->result==127)){
		vysledek->state=S0;
		break;
        }
        break;
		}
	}
	return vysledek->result;	
}
