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

value _CAbs (value v);
value _CArg (value v);
value _Real (value v);
value _Imag (value v);
value _Conj (value v);
value _Proj (value v);


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

enum TOSTR_INFO
{
	TOSTR_CATEGORY = 0x01,  // for %Cate , convert every single-value type to 'v'.
	TOSTR_VAL_TYPE = 0x02,  // for %Type , convert to: bool, enum, char, str, int, rat, flt, com, bigi, ptr, struct, data
	TOSTR_ESCAPE   = 0x02,  // for %Esca , specify to escape special characters
	TOSTR_NEWLINE  = 0x04,  // for %Line , specify to put a newline after each row
	TOSTR_PAD_ZERO = 0x08,  // for %Zero , specify to pad with zero (with space is default)

	TOSTR_ALIGN_LEFT = 0x10,  // for %-... , specify to align to left (right is default)
	TOSTR_ALIGN_CENT = 0x20,  // for %=... , specify to align to center (right is default)
	TOSTR_CHARS_MAXI = 0x40,  // for %Maxi , specify that info[16:25] is maximum number of characters
	TOSTR_EXACT_PREC = 0x80,  // for %.<n> , specify that info[26:31] is exact number of decimal places

	TOSTR_s = 0x000, // for %s , use default conversion to string
	TOSTR_d = 0x100, // for %d , use base 10
	TOSTR_x = 0x200, // for %x , use base 16 lower case abcdef
	TOSTR_X = 0x300, // for %X , use base 16 upper case ABCDEF
	TOSTR_o = 0x400, // for %o , use base 8
	TOSTR_b = 0x500, // for %b , use base 2
	TOSTR_E = 0x600, // for %E , use scientific notation base 10
	TOSTR_PP = 0x700, // for %% , only put the '%' character!
};
typedef struct { uint32_t info, length; } ToStrInfo;
ToStrInfo getToStrInfo (const wchar* str);
/*
	%format of length ToStrInfo.length converts to ToStrInfo.info:
	info[0 : 3] = 4-bit used as per 1st section of enum TOSTR_INFO
	info[4 : 7] = 4-bit used as per 2nd section of enum TOSTR_INFO
	info[8 :11] = 4-bit used as per 3rd section of enum TOSTR_INFO
	info[12:15] = 4-bit index of the target value. 0 for default.
	info[16:25] = 10-bit minimum number of characters. 0 for default.
	info[26:31] = 6-bit maximum number of decimal places. 0 for default.
*/
#define TOSTR_GET_OPER(info)      (info &  0xF00)
#define TOSTR_SET_OPER(info,n)  ( (info & ~0xF00) | (n & 0xF00) )

#define TOSTR_GET_INDEX(info) ((info>>12) & 0xF  )
#define TOSTR_GET_WIDTH(info) ((info>>16) & 0x3FF)
#define TOSTR_GET_PREC(info)  ((info>>26) & 0x3F )

#define TOSTR_SET_INDEX(info,n) ( (info & ~(0xF  <<12)) | ((n & 0xF  )<<12) )
#define TOSTR_SET_WIDTH(info,n) ( (info & ~(0x3FF<<16)) | ((n & 0x3FF)<<16) )
#define TOSTR_SET_PREC(info,n)  ( (info & ~(0x3F <<26)) | ((n & 0x3F )<<26) )

#define TOSTR_MAX_INDEX 0xF
#define TOSTR_MAX_WIDTH 0x3FF
#define TOSTR_MAX_PREC  0x3F

value VstToStr (value v, uint32_t info);


#include "_strfun.h" // for strcpy22()
static inline wchar* intToStr (wchar* out, SmaInt n)
{
	uint32_t v[100];
	VstToStr(setSmaInt(v, n), 0);
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
