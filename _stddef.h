#ifndef __STDDEF_H
#define __STDDEF_H
/*
    _stddef.h
*/

#include <stddef.h>

#ifdef _WINDOWS
#define inline _inline
typedef enum { false, true } bool;
#else
#include <stdbool.h>
#endif

#define SIZEOF(x) (sizeof(x)/sizeof(*(x)))


typedef wchar_t wchar;

typedef struct _lchar
{   wchar wchr;
    unsigned short line;
    unsigned short coln;
    unsigned short source;
    struct _lchar *prev, *next;
} lchar;

#define lchar_copy(n,m) {(n)->line = (m)->line; \
                         (n)->coln = (m)->coln; \
                         (n)->source = (m)->source; }


enum ValueType {
    aNotval,
    aPoiter,
    aSeptor,
    aString,
    aNumber,
    aSmaInt,
    aSmaRat,
    aSmaFlt,
    aSmaCom,
    aBigInt,
    aBigRat,
    aBigFlt,
    aBigCom
};

typedef const wchar* Notval;
typedef struct _value* Poiter;
typedef struct _Septor { long len, cols; } Septor;
typedef lchar* String;
typedef long long SmaInt;
typedef struct _SmaRat { SmaInt nume, deno; } SmaRat;
typedef double SmaFlt;

// the below is just so to reserve memory for later casting
typedef struct _SmaCOM { unsigned char memory[16]; } SmaCOM;
#define BigINT SmaCOM
#define BigRAT SmaCOM
#define BigFLT SmaCOM
#define BigCOM SmaCOM
#define Number SmaCOM

typedef struct _value
{
    enum ValueType type;
    union
    {   Notval msg;
        Poiter ptr;
        Septor sep;
        String str;
        SmaInt si;
        SmaRat sr;
        SmaFlt sf;
        SmaCOM sc;
        BigINT bi;
        BigRAT br;
        BigFLT bf;
        BigCOM bc;
    };
} value;


typedef struct {  char* data; long size; } Array1;
typedef struct { wchar* data; long size; } Array2;
typedef struct { const  char* data; long size; } const_Array1;
typedef struct { const wchar* data; long size; } const_Array2;
static inline const_Array1 ConstArray1(Array1 in)
{ const_Array1 out={in.data, in.size}; return out; }
static inline const_Array2 ConstArray2(Array2 in)
{ const_Array2 out={in.data, in.size}; return out; }


#endif
