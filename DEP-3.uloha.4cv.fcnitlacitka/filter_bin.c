#include "filter_bin.h"

void InitFilterBin(T_filter_bin* filter){
filter->state=S0;
filter->result=TRUE;
}
T_boolean FilterBin(T_filter_bin* filter,T_boolean data){
	switch(filter->state){
	case S0:{
		if(data){
		filter->state=S1;
		}
		else {
		filter->state=S2;
		}
		break;
		}
	case S1:{
		if(data){
		filter->state=S3;
		}
		else {
		filter->state=S2;
		}
		break;
		}
	case S2:{
		if(data){
		filter->state=S1;
		}
		else {
		filter->state=S4;
		}
		break;
		}
	case S3:{
		if(data){
		filter->state=S5;
		}
		else {
		filter->state=S2;
		}
		break;
		}
case S4:{
		if(data){
		filter->state=S1;
		}
		else {
		filter->state=S6;
		}
		break;
		}
case S5:{
		if(data){
		filter->state=S5;
		}
		else {
		filter->state=S2;
		}
        filter->result=TRUE;
		break;
      
		}
case S6:{
		if(data){
		filter->state=S1;
		}
		else {
		filter->state=S6;
		}
	    filter->result=FALSE;
	    break;
		}
	}
	return filter->result;	
}