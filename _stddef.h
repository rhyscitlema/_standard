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


#ifdef WIN32
typedef wchar_t mchar;
#else
typedef unsigned short mchar;
#endif

typedef struct _lchar
{   mchar mchr;
    unsigned int line;
    unsigned int coln;
    const mchar* file;
    struct _lchar *prev, *next;
} lchar;

#define lchar_get(n)     (n)->mchr
#define lchar_set(n,c)   (n)->mchr = (c);
#define lchar_copy(n,m) {(n)->mchr = (m)->mchr; \
                         (n)->line = (m)->line; \
                         (n)->coln = (m)->coln; \
                         (n)->file = (m)->file; }


enum ValueType {
    aNotval,
    aPoiter,
    aSeptor,
    aString,
    aaaaaa0,
    aaaaaa1,
    aaaaaa2,
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

typedef const mchar* Notval;
typedef struct _value* Poiter;
typedef struct _Septor { long len, cols; } Septor;
typedef lchar* String;
typedef long long SmaInt;
typedef struct _SmaRat { SmaInt nume, deno; } SmaRat;
typedef double SmaFlt;

// the below is just so to reserve memory for later casting
#define SmaCOM SmaRat
#define BigINT SmaRat
#define BigRAT SmaRat
#define BigFLT SmaRat
#define BigCOM SmaRat
#define Number SmaRat

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


#endif
