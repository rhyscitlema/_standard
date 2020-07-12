#ifndef __STDDEF_H
#define __STDDEF_H
/*
    _stddef.h
*/

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#define SIZEOF(x) (sizeof(x)/sizeof(*x))


typedef wchar_t wchar;

typedef struct _mchar
{   wchar c;
    uint32_t line;
    uint32_t coln;
    uint16_t source;
} mchar;

typedef struct _lchar
{   mchar chr;
    struct _lchar *prev, *next;
} lchar;


typedef  char* Str1; // UTF-8
typedef wchar* Str2; // UTF-32
typedef struct { lchar *ptr, *end; } Str3;

typedef const  char* const_Str1;
typedef const wchar* const_Str2;
typedef       Str3   const_Str3;

typedef uint32_t* value;
typedef const uint32_t* const_value;

typedef int64_t SmaInt;
typedef double SmaFlt;


enum ValueType // use (VT << 28)
{
    // this must be = 0x0
    VAL_MESSAGE     = 0x0,  // [12-bit code + 16-bit size]
    VAL_OPERAT      = 0x1,  // [12-bit oper + 16-bit size]

  /* uses [single] or [high],[low] depending on offset size */
    VAL_OFFSET      = 0x2,  // [single] = unsigned 28-bit integer
  //VAL_OFFSAT      = 0x3,  // [high],[low] = signed 2*28-bit integer

  /* uses first [0, 28) bits only */
    VAL_CHARAC      = 0x4,  // [value] = unsigned 28-bit UTF32 character
    VAL_ENUMER      = 0x5,  // [12-bit type + 16-bit value] = unsigned

    // VAL_ENUMER type 0 is the 1-bit boolean
    // VAL_ENUMER type 1 is the 8-bit UTF8 char

  //VAL_STRUCT      = 0x6,  // [12-bit type + 16-bit size]

  /* uses first [0, 28) bits for size */
    VAL_INTFLT      = 0x7,  // [size = 2 or 3 or more]
    VAL_NUMBER      = 0x9,  // [size = 4 or 6 or more]

  /* uses first [0, 28) bits for count */
    VAL_ARRRAY      = 0xA,  // [count] , [E_VT]
    VAL_VECTOR      = 0xB,  // [count] , [size]

  /* uses first [0, 28) bits for type|info */
  //VAL_DATA        = 0xC,  // [type|info] , [size]
    VAL_POINTER     = 0xD,  // [type|info] , size = 1+2

  //VAL_FREE        = 0xE,
  //VAL_EXTENDED    = 0xF,

    aSmaInt = 0x10,
    aSmaFlt = 0x20,
    aSmaRat = 0x30,
    aSmaCom = 0x40,
    aString = 0x50,
};

#endif
