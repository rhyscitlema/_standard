#ifndef __MATH_H
#define __MATH_H
/*
    _math.h
*/

#include "_value.h"


/*-------------- Baisc operations ----------------*/

value positive (value n);
value negative (value n);

value add      (value n, value m);
value subtract (value n, value m);
value multiply (value n, value m);
value  divide  (value n, value m);
value idivide  (value n, value m);

value _ceil    (value n);
value _floor   (value n);


/* --------- Exponent and Logarithm Functions ----------- */

value power  (value n, value m);
value _sqrt  (value n);
value _isqrt (value n);
value _log_e (value n);
value _log_10(value n);
value _exp_e (value n);


/* --------- Trigonometric and Hyperbolic Functions ---- */

value _cos (value n);
value _sin (value n);
value _tan (value n);

value _cosh (value n);
value _sinh (value n);
value _tanh (value n);

value _acos (value n);
value _asin (value n);
value _atan (value n);

value _acosh (value n);
value _asinh (value n);
value _atanh (value n);


/* --------- Mainly For Complex Numbers ---------------- */

value __abs (value n);
value _carg (value n);
value _real (value n);
value _imag (value n);
value _conj (value n);
value _proj (value n);


/* ----------- Integer operations ------------------ */

value factorial (value n);
value modulo    (value n, value m);
value find_gcd  (value n, value m);


/* --------- Bitwise Operations ------------------------ */

value shift_right (value n, value m);
value shift_left  (value n, value m);
value bitwise_or  (value n, value m);
value bitwise_xor (value n, value m);
value bitwise_and (value n, value m);
value bitwise_not (value n);


/* --------- Logical Operations ------------------------ */

value logical_or  (value n, value m);
value logical_and (value n, value m);
value logical_not (value n);


/* --------- Comparison Operations --------------------- */

//value compare        (value n, value m);
value equalTo        (value n, value m);
value notEqual       (value n, value m);
value lessThan       (value n, value m);
value greaterThan    (value n, value m);
value lessOrEqual    (value n, value m);
value greaterOrEqual (value n, value m);


/*--- only used for when user wants to 'explicitly' convert between value types. ---*/

value toRat (value n); // mainly for Flt to Rat, 'never' done implicitly.
value toFlt (value n); // mainly for Num to Flt/Com, important conversion!


/*------------- get array from value structure -------------------*/

bool floatFromVst(SmaFlt* output, const value* input, int count, mchar* errormessage, const char* name);
bool integerFromVst (int* output, const value* input, int count, mchar* errormessage, const char* name);


/*----------------- string-value conversion ---------------------*/

value StrToVal (const mchar* str);  // single output value

/*
    if(info&3 == 0) print the values (number and string) normally.
    if(info&3 == 1) print strings as 't' and numbers as 'n'.
    if(info&3 == 2) print numbers as 'sn' or 'bn'.
    if(info&3 == 3) print numbers as si,sr,sf,sc,bi,br,bf,bc.
    if(info&4 != 0) put new line, used so to print matrix well.

    On success, return end of result string. On error,
    set an error message as the result then return NULL.
*/
mchar* VstToStr (const value* vst,  // input value structure
                 mchar* result,     // output string to store result
                 char info,         // extra information used
                 char base,         // base: 2, 8, 10, 16, or -1 for default
                 char  wplaces,     // number of whole places: -1 for default
                 short dplaces);    // number of decimal places: -1 for default



static inline mchar* intToStr (mchar* str, SmaInt n)
{
    value y = setSmaInt(n);
    VstToStr(&y,str,0,-1,-1,-1);
    return str;
}

static inline mchar* fltToStr (mchar* str, SmaFlt n)
{
    value y = setSmaFlt(n);
    VstToStr(&y,str,0,-1,-1,-1);
    return str;
}

static inline bool strToInt (const mchar* str, int *n)
{
    value v = _floor(StrToVal(str));
    if(getType(v)) { *n = (int)getSmaInt(v); return true; }
    else { //strcpy22(errorMessage(), getNotval(v));
        return false; }
}

static inline bool strToFlt (const mchar* str, SmaFlt *n)
{
    value v = toFlt(StrToVal(str));
    if(getType(v)) { *n = (int)getSmaFlt(v); return true; }
    else { //strcpy22(errorMessage(), getNotval(v));
        return false; }
}


#endif // end #ifndef __MATH_H
