#ifndef FILTER_BIN_H
#define FILTER_BIN_H
#include "boolean1.h"
typedef enum{S0,S1,S2,S3,S4,S5,S6}T_filter_bin_state;

typedef struct{
T_filter_bin_state state;
T_boolean result;
}T_filter_bin;

void InitFilterBin(T_filter_bin* filter);
T_boolean FilterBin(T_filter_bin* filter,T_boolean data);

#endif /*FILTER_BIN_H*/