#ifndef __MATH_H
#define __MATH_H
/*
    _math.h
*/

#include "_value.h"


/*-------------- Custom operations ----------------*/

value setSmaCom2 (value v, SmaFlt Re, SmaFlt Im);
value tovector (value v, uint32_t count);
value combine (value v);

value _size(value v);
value _span(value v);
value _sum (value v);
value _max (value v);
value _min (value v);

value indexing (value v);
value _vector  (value v);
value _range   (value v);

value transpose (value v);
value dotproduct(value v);


/*-------------- Basic operations ----------------*/

value _pos  (value v);
value _neg  (value v);
value __mul (value v);
value __div (value v);
value power (value v);

value _add  (value v);
value _sub  (value v);
value _mul  (value v);
value _div  (value v);
value _pow  (value v);


/* ----------- Integer operations ------------------ */

value _idiv (value v);
value _mod  (value v);
value _gcd  (value v);
value _ilog (value v);
value _isqrt(value v);
value _floor(value v);
value _ceil (value v);

value factorial (value v);
value fullfloor (value v);
value getprimes (value v);
value _srand    (value v);
value _rand     (value v);


/* --------- Exponent and Logarithm Functions ----------- */

value _sqrt (value v);
value _cbrt (value v);
value _exp  (value v);
value _log  (value v);


/* --------- Trigonometric and Hyperbolic Functions ---- */

value _cos  (value v);
value _sin  (value v);
value _tan  (value v);
value _acos (value v);
value _asin (value v);
value _atan (value v);

value _cosh (value v);
value _sinh (value v);
value _tanh (value v);
value _acosh(value v);
value _asinh(value v);
value _atanh(value v);


/* --------- Complex Numbers Functions ---------------- */

value _cabs (value v);
value _carg (value v);
value _real (value v);
value _imag (value v);
value _conj (value v);
value _proj (value v);


/* --------- Bitwise Operations ------------------------ */

value shift_left  (value v);
value shift_right (value v);
value bitwise_xor (value v);
value bitwise_or  (value v);
value bitwise_and (value v);
value bitwise_not (value v);


/* --------- Logical Operations ------------------------ */

value logical_or  (value v);
value logical_and (value v);
value logical_not (value v);
#define _not logical_not


/* --------- Comparison Operations --------------------- */

value sameAs      (value v);
value equalTo     (value v);
value lessThan    (value v);
value greaterThan (value v);

static inline value notSame       (value v) { return logical_not(sameAs(v)     ); }
static inline value notEqual      (value v) { return logical_not(equalTo(v)    ); }
static inline value lessOrEqual   (value v) { return logical_not(greaterThan(v)); }
static inline value greaterOrEqual(value v) { return logical_not(lessThan(v)   ); }


/*------------- convert between value types -------------------*/

value toStr (value v);
value toNum (value v);
value toInt (value v);
value toRat (value v); // mainly for Flt to Num, where Num is a Rat
value toFlt (value v); // mainly for Num to Flt, also for Int to Flt

value vStrLen (value v);


/*----------------- string-value conversion ---------------------*/

// <str> must be a literal: a number, a string, a character.
value StrToVal (value out, const wchar* str);

enum PUT_INFO {
    PUT_NORMAL   = 0x000,   // print the values (number and string) normally.
    PUT_CATEGORY = 0x100,   // print everything as 'v'
    PUT_VAL_TYPE = 0x200,   // print strings as 'str' and numbers as int, rat, flt or com
    PUT_ESCAPE   = 0x400,
    PUT_NEWLINE  = 0x800    // put newline; used so to print matrix with each row on its own line
};

value VstToStr (value v,            // input value to convert = vPrev(v)
                int info_base,      // base: 2, 8, 10, 16, 0 for default
                int t_places,       // number of total places, -1 for default
                int d_places);      // number of decimal places, -1 for default


#include "_strfun.h" // for strcpy22()
static inline wchar* intToStr (wchar* out, SmaInt n)
{
    uint32_t v[100];
    VstToStr(setSmaInt(v, n), 0,-1,-1);
    assert(v == vGet(v));
    return strcpy22(out, getStr2(v));
}

static inline SmaInt strToInt (const wchar* str, value stack)
{
    assert(stack!=NULL);
    toInt(StrToVal(stack, str));
    if(value_type(stack)==aSmaInt)
        return getSmaInt(stack);
    else return 0;
}


#endif // end #ifndef __MATH_H
