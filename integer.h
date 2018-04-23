/*
Soubor:     integer.h
Autor:      ML
Datum:      3.4.2015
Spolecnost: CVUT-FEL-K13114
Varianta:   1.00
Zmena:      --
Kompilator:	C18_V3.47 - MPLAB 8.92
Procesor:   PIC18F87J11
 */
//----------------------------------------------------------------------------
#ifndef _DEF_INTEGER_H
#define _DEF_INTTEGER_H
//----------------------------------------------------------------------------
/* Vtahovane soubory: */
//----------------------------------------------------------------------------
/* Definice konstant */
//----------------------------------------------------------------------------
/* Definice typu */
/*-------------------------------*/
typedef unsigned char byte_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
/*-------------------------------*/
typedef signed int int16_t;
typedef unsigned int uint16_t;

typedef volatile union {
  struct {
    uint8_t byteL;
    uint8_t byteH;
  };
  int16_t word;
} int16s_t;

typedef volatile union {
  struct {
    uint8_t byteL;
    uint8_t byteH;
  };
  uint16_t word;
} uint16s_t;
/*-------------------------------*/
typedef signed short long int24_t;
typedef unsigned short long uint24_t;

typedef volatile union{
  struct {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
  };
  uint24_t word;
} uint24s_t;

typedef volatile union{
  struct {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
  };
  int24_t word;
} int24s_t;
/*-------------------------------*/
typedef signed long int32_t;
typedef unsigned long uint32_t;

typedef volatile union{
  struct {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
  };
  uint32_t word;
} uint32s_t;

typedef volatile union{
  struct {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
  };
  int32_t word;
} int32s_t;

/*typedef struct{
    unsigned B0 : 1;
    unsigned B1 : 1;
   }Jmeno
 
typedef struct{
    unsigned B0 : 1;
    unsigned B1 : 1;
    unsigned B2 : 1;
    unsigned B3 : 1;
    unsigned B4 : 1;
    unsigned B5 : 1;
    unsigned B6 : 1;
    unsigned B7 : 1;
}BYTE;

typedef volatile union{
struct{
	BYTE CCPRL;
	BYTE CCPRH;
	BYTE TIMER;
};
unsigned short long CELA;
}T_Capture_Value;
*/



/* Definice symbolickych konstant vyuzivanych v jinych modulech: */
#define T_BYTE_MAX 255
#define T_BYTE_MIN 0
#define T_UINT8_MAX 255
#define T_UINT8_MIN 0
#define T_INT8_MAX 127
#define T_INT8_MIN -128
#define T_INT16_MAX 32767
#define T_INT16_MIN -32768
#define T_UINT16_MAX 65535
#define T_UINT16_MIN 0
#define T_INT24_MAX 8388607
#define T_INT24_MIN -8388608
#define T_UINT24_MAX 16777215
#define T_UINT24_MIN 0
#define T_INT32_MAX 2147483647
#define T_INT32_MIN -2147483648
#define T_UINT32_MAX 4294967295
#define T_UINT32_MIN 0
//----------------------------------------------------------------------------
/* Definice maker s parametry: */
//----------------------------------------------------------------------------
/* Definice globalnich typu: */
typedef unsigned char T_byte;
typedef unsigned char T_uint8;
typedef signed char T_int8;
/*-------------------------------*/
typedef signed int T_int16;
typedef unsigned int T_uint16;

typedef volatile union {

    struct {
        T_uint8 byteL;
        T_uint8 byteH;
    };
    T_int16 word;
} T_int16s;

typedef volatile union {

    struct {
        T_uint8 byteL;
        T_uint8 byteH;
    };
    T_uint16 word;
} T_uint16s;
/*-------------------------------*/
typedef signed short long T_int24;
typedef unsigned short long T_uint24;

typedef volatile union {

    struct {
        T_uint8 byte0;
        T_uint8 byte1;
        T_uint8 byte2;
    };
    T_uint24 word;
} T_uint24s;

typedef volatile union {

    struct {
        T_uint8 byte0;
        T_uint8 byte1;
        T_uint8 byte2;
    };
    T_int24 word;
} T_int24s;
/*-------------------------------*/
typedef signed long T_int32;
typedef unsigned long T_uint32;

typedef volatile union {

    struct {
        T_uint8 byte0;
        T_uint8 byte1;
        T_uint8 byte2;
        T_uint8 byte3;
    };
    T_uint32 word;
} T_uint32s;

typedef volatile union {

    struct {
        T_uint8 byte0;
        T_uint8 byte1;
        T_uint8 byte2;
        T_uint8 byte3;
    };
    T_int32 word;
} T_int32s;

typedef volatile union{
    struct{
        unsigned S1:1;
        unsigned S2:1;
        unsigned S3:1;
        unsigned S4:1;
        unsigned S5:1;
    };
    unsigned short CELA;
}T_Button_Struct;
typedef volatile union{
    struct{
        unsigned char CCPRL;
        unsigned char CCPRH;
        unsigned char TIMER;
    };
    unsigned short long CELA;
}T_Capture_Value;
/*
typedef volatile union{
    struct{
        unsigned char B0:1;
        unsigned char B1:1;
        unsigned char B2:1;
        unsigned char B3:1;
        unsigned char B4:1;
        unsigned char B5:1;
        unsigned char B6:1;
        unsigned char B7:1;
        
    };
    unsigned short BYTE;
}T_BYTE;   
typedef volatile union{
struct{
	T_BYTE CCPRL;
	T_BYTE CCPRH;
	T_BYTE TIMER;
};
unsigned short long CELA;
}T_CPT_V;
*/

//----------------------------------------------------------------------------
/* Definice globalni promenne. */
//----------------------------------------------------------------------------
/* Definice maker */
//----------------------------------------------------------------------------
#endif /* konec _DEF_INTEGER_H */
