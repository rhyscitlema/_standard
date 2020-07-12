/*
	_math.c
	TODO: before introducing arrays of integers and the likes, first do something about the very confusing "string as an array of characters". Maybe treat these as special arrays, by treating some operations like addition and logical_not differently. Then provide a string-only array-to-vector operation.
*/

#include <math.h>
#include <complex.h>
#include "_string.h"
#include "_stdio.h"
#include "_texts.h"
#include "_math.h"
#include <stdio.h> // for sprintf() only



typedef double complex SmaCom;

#ifndef COMPLEX
#define COMPLEX 0

static SmaCom _cpow   (SmaCom n, SmaCom m) { return n+m; }
static SmaCom _csqrt  (SmaCom n) { return n; }
static SmaCom _cexp   (SmaCom n) { return n; }
static SmaCom _clog   (SmaCom n) { return n; }

static SmaCom _ccos   (SmaCom n) { return n; }
static SmaCom _csin   (SmaCom n) { return n; }
static SmaCom _ctan   (SmaCom n) { return n; }
static SmaCom _cacos  (SmaCom n) { return n; }
static SmaCom _casin  (SmaCom n) { return n; }
static SmaCom _catan  (SmaCom n) { return n; }

static SmaCom _ccosh  (SmaCom n) { return n; }
static SmaCom _csinh  (SmaCom n) { return n; }
static SmaCom _ctanh  (SmaCom n) { return n; }
static SmaCom _cacosh (SmaCom n) { return n; }
static SmaCom _casinh (SmaCom n) { return n; }
static SmaCom _catanh (SmaCom n) { return n; }

static SmaCom _cproj  (SmaCom n) { return n; }

static SmaFlt ccabs (SmaCom x)
{
	SmaFlt r = creal(x);
	SmaFlt i = cimag(x);
	return sqrt(r*r+i*i);
}

#else

#define _cpow   cpow
#define _csqrt  csqrt
#define _cexp   cexp
#define _clog   clog

#define _ccos   ccos
#define _csin   csin
#define _ctan   ctan
#define _cacos  cacos
#define _casin  casin
#define _catan  catan

#define _ccosh  ccosh
#define _csinh  csinh
#define _ctanh  ctanh
#define _cacosh cacosh
#define _casinh casinh
#define _catanh catanh

#define _cproj  cproj
#define ccabs   cabs

#endif



static inline SmaCom getSmaCom (const_value v) { return *(SmaCom*)(v+1); }

static value setSmaCom (value v, SmaCom x)
{
	const int l = 1+sizeof(SmaCom)/sizeof(*v); // get length
	assert(v!=NULL);
	v[0] = (VALUE_NUMBER<<28) | 3; // 3 for Small Complex
	*(SmaCom*)(v+1) = x;
	v[l] = (VALUE_OFFSET<<28) | l;
	return v+l+1;
}
value setSmaCom2 (value v, SmaFlt Re, SmaFlt Im) { return setSmaCom(v, Re+Im*I); }

#define OPER_FAIL_(code) v = setMessage(y, code, 0, 0)
#define OPER_FAIL OPER_FAIL_(Operand_Not_Supported);

#define NOT_AVAILABLE { return setError(vPrev(v), L"Operation not yet available!"); }



typedef struct {
	value y;
	const_value p;
	enum ValueType t;
	union {
		uint32_t s;
		SmaInt i;
		SmaFlt f;
		//SmaRat r;
		SmaCom c;
	} x;
} ValueInfo;

static ValueInfo getValueInfo (value v)
{
	ValueInfo n;
	n.y = vPrev(v);
	n.p = vGet(n.y);
	n.t = value_type(n.p);
	switch(n.t)
	{
		case aString: n.x.s = ARRAY_LEN(*n.p); break;
		case aSmaInt: n.x.i = getSmaInt(n.p); break;
		case aSmaFlt: n.x.f = getSmaFlt(n.p); break;
		case aSmaCom: n.x.c = getSmaCom(n.p); break;
		default: break;
	}
	return n;
}

#define ns _n.x.s
#define ni _n.x.i
#define nf _n.x.f
#define nc _n.x.c

#define ms _m.x.s
#define mi _m.x.i
#define mf _m.x.f
#define mc _m.x.c

#define _init_n \
	ValueInfo _n = getValueInfo(v); \
	value y = _n.y; \
	uint32_t a = _n.t; \
	const_value n = _n.p; \
	if(a==VALUE_MESSAGE) return v; \

#define _init_n_m \
	ValueInfo _m = getValueInfo(v); \
	uint32_t b = _m.t; \
	const_value m = _m.p; \
	if(b==VALUE_MESSAGE) return v; \
	\
	ValueInfo _n = getValueInfo(_m.y); \
	value y = _n.y; \
	uint32_t a = _n.t; \
	const_value n = _n.p; \
	assert(a!=VALUE_MESSAGE); \
	if(a==VALUE_MESSAGE) return _m.y; \

#define init_n(call) _init_n \
	if(a==VALUE_VECTOR) \
		return vCopy(y, OPER_TYPE1(v, n, call)); \

#define init_n_m(call) _init_n_m \
	if(a==VALUE_VECTOR || b==VALUE_VECTOR) \
		return vCopy(y, OPER_TYPE2(v, n, m, call)); \

// mix value types, used by switch statements only
#define MIX(a,b) ((a<<8)|b)


static value OPER_TYPE1 (value v, const_value n, value (*CALL)(value v))
{
	assert(VTYPE(*n)==VALUE_VECTOR);
	uint32_t i, count = VECTOR_LEN(*n);
	n += 2; // skip vector header
	value y = v;
	v += 2; // reserve space for vector header
	for(i=0; i<count; i++)
	{
		CALL(setRef(v, n));
		if(VTYPE(*v)) v = vNEXT(v);
		else return v; // quit on error
		n = vNext(n);
	}
	setVector(y, count, v-y-2);
	return y;
}


static value OPER_TYPE2 (value v, const_value n, const_value m, value (*CALL)(value v))
{
	uint32_t i, count;
	uint32_t a=*n, b=*m;

	if(VTYPE(a)==VALUE_VECTOR)
	{
		count = VECTOR_LEN(a);
		if(VTYPE(b)==VALUE_VECTOR)
		{
			if((VECTOR_LEN(b)) != count){
				setMessage(v, Operands_DoNot_Match, 0, 0);
				return v;
			}
		}
		else b=0;
	}
	else if(VTYPE(b)==VALUE_VECTOR){
		count = VECTOR_LEN(b);
		a=0;
	}
	else { assert(false); return v; }

	if(a) n += 2; // skip vector header
	if(b) m += 2;
	value y = v;
	v += 2; // reserve space for vector header

	for(i=0; i<count; i++)
	{
		CALL(setRef(setRef(v, n), m));
		if(VTYPE(*v)) v = vNEXT(v);
		else return v; // quit on error
		if(a) n = vNext(n);
		if(b) m = vNext(m);
	}
	setVector(y, count, v-y-2);
	return y;
}



//---------------------------------------------------------------

value combine (value v)
{
	_init_n_m
	long sn = vSize(n);
	long sm = vSize(m);
	if(a==VALUE_VECTOR) { a = VECTOR_LEN(*n); n+=2; sn-=2; } else a=1;
	if(b==VALUE_VECTOR) { b = VECTOR_LEN(*m); m+=2; sm-=2; } else b=1;
	memcpy(v+2   , n, sn*sizeof(*n));
	memcpy(v+2+sn, m, sm*sizeof(*m));
	setVector(v, a+b, sn+sm);
	return vCopy(y, v);
}


value _size (value v)
{
	_init_n
	uint32_t rows=1, cols=1;

	if(a==VALUE_ARRRAY){ // if an array but not a string
		assert(a!=VALUE_ARRRAY); // not allowed for now
		rows = ARRAY_LEN(*n);
	}
	else if(a==VALUE_VECTOR)
	{
		rows = VECTOR_LEN(*n);
		n += 2; // skip vector header

		if(rows==0) cols=0;
		else{
			uint32_t j = *vGet(n);
			cols = VTYPE(j)==VALUE_VECTOR ? VECTOR_LEN(j) : 1;
			n = vNext(n);

			uint32_t i;
			for(i=1; i<rows; i++)
			{
				j = *vGet(n);
				j = VTYPE(j)==VALUE_VECTOR ? VECTOR_LEN(j) : 1;
				if(j != cols) { cols=0; break; }
				n = vNext(n);
			}
		}
	}
	y = setSmaInt(y, rows);
	y = setSmaInt(y, cols);
	return tovector(y, 2);
}


static uint32_t get_span (const_value n)
{
	uint32_t c = *n;
	if(VTYPE(c)==VALUE_VECTOR){
		c = VECTOR_LEN(c);
		n += 2; // skip vector header
	} else c=1;

	uint32_t r = 0;
	while(c--)
	{
		const_value t = vGet(n);
		if(VTYPE(*t)==VALUE_VECTOR)
			r += get_span(t); // recursive call
		else r++;
		n = vNext(n);
	}
	return r;
}
value _span (value v) { _init_n  return setSmaInt(y, get_span(n)); }


static value get_sum (const_value n, value v)
{
	uint32_t c = *n;
	if(VTYPE(c)==VALUE_VECTOR){
		c = VECTOR_LEN(c);
		if(!c && !*(v-1))
			return vCopy(v-1, n);
		n += 2; // skip vector header
	} else c=1;

	while(c--)
	{
		const_value t = vGet(n);
		if(VTYPE(*t)==VALUE_VECTOR)
			v = get_sum(t, v); // recursive call
		else if(*(v-1)==0) // if first time
			v = vCopy(v-1, t); // then just copy
		else v = _add(setRef(v, t));
		n = vNext(n);
	}
	return v;
}
value _sum (value v) { _init_n  *v++=0;  return vPrevCopy(y, get_sum(n, v)); }


static const_value _max_min (const_value n, value v, value (*CALL)(value v))
{
	uint32_t c = *n;
	if(VTYPE(c)==VALUE_VECTOR){
		c = VECTOR_LEN(c);
		n += 2; // skip vector header
	} else c=1;

	const_value w, r = NULL;
	while(c--)
	{
		w = vGet(n);
		n = vNext(n);
		if(VTYPE(*w)==VALUE_VECTOR)
			w = _max_min(w, v, CALL); // recursive call
		if(r==NULL) r=w;
		else if(w)
		{
			CALL(setRef(setRef(v, r), w));
			if(isBool(v) && (*v & 1)) r = w;
		}
	}
	return r;
}
value _max (value v) { _init_n  return vCopy(y, _max_min(n, v, lessThan   )); }
value _min (value v) { _init_n  return vCopy(y, _max_min(n, v, greaterThan)); }



//---------------------------------------------------------------

value vStrLen (value v)
{
	init_n(vStrLen)
	if(a != aString) OPER_FAIL
	else v = setSmaInt(y, ARRAY_LEN(*n));
	return v;
}


value toStr (value v)
{
	_init_n

	a = *n;
	if(VTYPE(a)==VALUE_VECTOR){
		a = VECTOR_LEN(a);
		n += 2; // skip vector header
	} else a=1;

	const_value m = vGet(n);
	if(!isStr2(m))
		return setError(y, L"First argument <format> must be a string.");
	const wchar* format = getStr2(m);
	n = vNext(n); // skip the first argument

	wchar* out = (wchar*)v;
	*out = 0;
	int i=0;
	while(format && *format)
	{
		ToStrInfo info = getToStrInfo(format);
		if(info.info == ~0) // if error
		{
			out = (wchar*)(v+1000);
			strcpy22S(out, format, info.length);
			format = L"First argument has invalid place holder (%s).";
			const_Str2 argv[2] = { format, out };
			v = setMessage(y, 0, 2, argv);
			break;
		}
		int oper = (info.info & 0xF00);

		if(info.length <= 1  // if no place holder
		|| oper == TOSTR_PP) // or if "%%" was found
		{
			*out++ = *format;
			*out = '\0';
		}
		else{
			int j = (info.info >> 12) & 0xF; // get the target index
			i = j ? j : i+1; // use i+1 if target is the default j==0

			if(i >= a) // if index-out-of-range error
			{
				out = (wchar*)(v+1000);
				strcpy22S(out, format, info.length);
				format = L"Place holder (%s) uses index = %s >= %s.";
				const_Str2 argv[4] = { format, out, TIS2(0,i), TIS2(1,a) };
				v = setMessage(y, 0, 4, argv);
				break;
			}

			// Get element <m> corresponding to target index <i>
			for(m=n, j=1; j<i; j++) m = vNext(m);

			value t = (value)(out+1);               // typeof(*out) == wchar
			if((size_t)t % sizeof(*t)) t = (value)(out+2);  // in case sizeof(wchar) is 2 not 4

			t = VstToStr(setRef(t,m), info.info);
			out = strcpy22(out, getStr2(vGetPrev(t)));
		}
		format += info.length;
	}
	if(!format || !*format) // if there was no error
		v = setStr22(y, (wchar*)v);
	return v;
}


value toNum (value v)
{
	NOT_AVAILABLE
}


value toRat (value v)
{
	NOT_AVAILABLE
}

value toFlt (value v)
{
	init_n(toFlt)
	switch(a)
	{
		case aSmaInt: v = setSmaFlt(y, ni); break;
		case aSmaFlt: v = setSmaFlt(y, nf); break; // TODO: this line is due to setRef() in OPER_TYPE1
		case aSmaCom: v = setSmaFlt(y, creal(nc)); break;
		default: OPER_FAIL break;
	}
	return v;
}


static inline int aMODb (int a, int b) { a = a % b; if(a<0) a+=b; return a; }

value indexing (value v)
{
	_init_n_m

	if(b==VALUE_VECTOR){
		b = VECTOR_LEN(*m);
		m += 2; // skip vector header
	} else b=1;

	int i=b, start=0, stop=0, step=0;
	do{
		if(value_type(m)!=aSmaInt) break;
		start = (int)getSmaInt(m);
		if(--i==0) break; // if [index] given

		m = vNext(m);
		if(value_type(m)!=aSmaInt) break;
		stop = (int)getSmaInt(m);
		if(--i==0) break; // if [start, stop] given

		m = vNext(m);
		if(value_type(m)!=aSmaInt) break;
		step = (int)getSmaInt(m);
		--i; break; // if [start, stop, step] given
	}while(0);

	do{
		if(i) {
			v = setError(y, L"Indexing argument must be at most 3 integers.");
			break;
		}
		if(step==0) step = (start>=0) ? +1 : -1;

		i = a==aString; // or a==VALUE_ARRAY;
		if(i || a==VALUE_VECTOR){
			a = *n;
			a = i ? ARRAY_LEN(a) : VECTOR_LEN(a);
			n += 2; // skip vector header
		} else a=1;

		if(b==1)
		{
			if(start >= (int)a || a==0)
			{
				const_Str2 argv[3] = {
					TWST(Index_OutOf_Range),
					TIS2(0,start), TIS2(1,a)
				};
				v = setMessage(y, 0, 3, argv);
			}
			else{
				start = aMODb(start, a);

				if(i) // if indexing a string
					v = setCharac(y, WCHAR(n)[start]);
				else{
					while(start--) n = vNext(n);
					v = vCopy(y, vGet(n));
				}
			}
			break;
		}

		if(a==0) { v = vCopy(y, n-2); break; } // -2 so to unskip header
		else{
			if(start < -(int)a) start = aMODb(start, a);
			else if(start < 0) start += a;
			else if(start >= a && step<0) start = a-1;

			if(stop < -(int)a) stop = aMODb(stop, a);
			else if(stop < 0) stop += a;
			else if(stop >= a) stop = a-1;
		}
		//else if(step>0)
		//     { if(start > stop) stop += a*((start-stop+a-1)/a); }
		//else { if(start < stop) stop -= a*((stop-start+a-1)/a); }

		if(i) // if indexing a string
		{
			wchar* w = (wchar*)(v+2);
			while(true)
			{
				if(step>0)
				     { if(start > stop) break; }
				else { if(start < stop) break; }
				*w++ = WCHAR(n)[start];
				start += step;
			}
			*w = '\0';
			v = vPrevCopy(y, onSetStr2(v, w));
		}
		else if(step>0)
		{
			int r;
			r = 0; // = number of rows of result vector
			i = 0; // current position in source vector
			v = y+2;
			while(start<=stop)
			{
				for( ; i != start; i++) n = vNext(n);
				const_value t = vGet(n);
				long size = vSize(t);
				memmove(v, t, size*sizeof(*t));
				v += size; r++;
				start += step;
			}
			setVector(y, r, v-y-2);
			v = setOffset(v, v-y);
		}
		else{
			v = setError(y, L"Negative stepping on vector is not yet available.");
			// TODO: NOTE: no vectored indexing, must use range() then...?!
		}
	}while(0);
	return v;
}


value _range (value v)
{
	_init_n
	value e = check_arguments(y,*n,3); if(e) return e;
	n += 2; // skip vector header

	const_value start = n;          // get 1st element
	const_value stop = vNext(n);    // get 2nd element
	n = vNext(stop);                // get 3rd element

	n = vGet(n); // n points at the stepping value
	uint32_t b = *n;

	if(VTYPE(b)==VALUE_VECTOR){ // if a vector
		b = VECTOR_LEN(b); // get vector length
		n += 2; // skip vector header
	} else b=1;
	if(b==0) return vCopy(y,start);

	if((size_t)v % sizeof(void*)) v++; // get to aligned void**
	const_value* step = (const_value*)v;
	v = (value)(step+b);

	step[0] = start;
	v = setRef(v, start);
	for(a=1; a<b; a++)
	{
		step[a] = v;
		v = _add(setRef(setRef(v, step[a-1]), n));
		n = vNext(n);
	}
	const_value sum = v;
	v = _add(setRef(setRef(v, step[a-1]), n));
	v = _sub(setRef(v, start));
	if(VERROR(v)) return vPrevCopy(y,v);
	value w = v;

	lessThan(setSmaInt(setRef(v, sum), 0));
	if(!isBool(v)) v=0; // if result is not a boolean
	else{
		value (*check)(value) = (*v & 1) ? lessThan : greaterThan;

		v += 2; // reserve space for vector header
		v = vCopy(v, start); // deal with case a=0
		for(a=1; ; a++)
		{
			if(a>100000 && !user_confirm(L"Warning",
				L"Vector length is greater than 100000. Continue?"))
				return setError(y, L"Vector length is greater than 100000.");

			v = unOffset(v);
			e=v;

			v = setRef(v, step[a%b]);
			v = setSmaInt(v, a/b);
			v = setRef(v, sum);
			v = _add(_mul(v));

			check(setRef(setRef(v, e), stop));
			if(!isBool(v)) { v=0; break; }
			if(*v & 1) { v=e; break; }
		}
	}
	if(v==0) v = setError(y, L"At least one argument is not comparable.");
	else{
		setVector(w, a, v-w-2);
		v = vCopy(y, w);
	}
	return v;
}


value _vector (value v)
{
	_init_n
	value e = check_arguments(y,*n,3); if(e) return e;
	n += 2; // skip vector header

	const_value start = n;          // get 1st element
	const_value stop = vNext(n);    // get 2nd element
	n = vNext(stop);                // get 3rd element

	n = vGet(n); // n points at the stepping value
	uint32_t b = *n;

	if(VTYPE(b)==VALUE_VECTOR){ // if a vector
		b = VECTOR_LEN(b); // get vector length
		n += 2; // skip vector header
	} else b=1;
	if(b==0) return vCopy(y,start);

	if((size_t)v % sizeof(void*)) v++; // get to aligned void**
	const_value* step = (const_value*)v;
	v = (value)(step+b);

	step[0] = start;
	v = setRef(v, start);
	for(a=1; a<b; a++)
	{
		step[a] = v;
		v = _add(setRef(setRef(v, step[a-1]), n));
		n = vNext(n);
	}
	const_value sum = v;
	v = _add(setRef(setRef(v, step[a-1]), n));
	v = _sub(setRef(v, start));
	if(VERROR(v)) return vPrevCopy(y,v);
	value w = v;

	long count=0;
	if(value_type(stop)!=aSmaInt || (count = getSmaInt(stop))<=0)
		v = setError(y, TWST(Vector_Length_Invalid));

	else if(count>100000 && !user_confirm(L"Warning",
		L"Vector length is greater than 100000. Continue?"))
		return setError(y, L"Vector length is greater than 100000.");
	else
	{
		v += 2; // reserve space for vector header
		v = vCopy(v, start); // deal with case a=0
		for(a=1; a<count; a++)
		{
			v = unOffset(v);
			v = setRef(v, step[a%b]);
			v = setSmaInt(v, a/b);
			v = setRef(v, sum);
			v = _add(_mul(v));
		}
		assert(!VERROR(v));
		v = unOffset(v);
		setVector(w, a, v-w-2);
		v = vCopy(y, w);
	}
	return v;
}


value fullfloor (value v)
{
	NOT_AVAILABLE
/*
{
	long i, n, m, len;
	const value *N, *A;
	value y, sum, numerator;

	Expression* expression = eca.expression;
	eca.expression = expression->headChild;
	if(!expression_evaluate(eca)) return 0;

	Str2 errmsg = eca.garg->message;
	if(!check_first_level(eca.stack, 2, errmsg, expression->name)) return 0;

	N = eca.stack+1;          // get vector of values of n
	len = VST_SIZE(N);

	A = N + len;          // get the vector A
	m = VST_SIZE(A);
	if(m>1) { m--; A++; } // skip ',' separator
	//TODO: check if A is a single value or vector

	sum = setSmaInt(0);
	for(i=0; i<m; i++) sum = add(sum, A[i]);

	if(getSmaInt(logical_not(sum))) // if sum==0
	{
		for(n=len; n!=0; n--, N++, eca.stack++)
		{
			y = *N;
			if(!isVector(y))
				y = setSmaInt((SmaInt)0x8000000000000000);
			*eca.stack = y;
		}
	}
	else
	{
		for(n=len; n!=0; n--, N++, eca.stack++)
		{
			y = *N;
			if(!isVector(y))
			{
				numerator = y;
				y = setSmaInt(0);
				for(i=1; i<=m; i++)
				{
					y = add(y, idivide(numerator, sum));
					numerator = add (numerator, A[m-i]);
				}
			}
			*eca.stack = y;
		}
	}
	return 1;
}
*/
}


value getprimes (value v)
{
	NOT_AVAILABLE
}


value _srand (value v)
{
	NOT_AVAILABLE
}


value _rand (value v)
{
	NOT_AVAILABLE
}


value dotproduct (value v)
{
	NOT_AVAILABLE
}


//---------------------------------------------------------------

value _pos (value v)
{
	init_n(_pos)
	switch(a)
	{
		case aSmaInt: v = setSmaInt(y, +ni); break;
		case aSmaFlt: v = setSmaFlt(y, +nf); break;
		case aSmaCom: v = setSmaCom(y, +nc); break;
		default: OPER_FAIL break;
	}
	return v;
}

value _neg (value v)
{
	init_n(_neg)
	switch(a)
	{
		case aSmaInt: v = setSmaInt(y, -ni); break;
		case aSmaFlt: v = setSmaFlt(y, -nf); break;
		case aSmaCom: v = setSmaCom(y, -nc); break;
		default: OPER_FAIL break;
	}
	return v;
}


static const_value* set_matrix_pointers (value v, const_value n, int rows, int cols)
{
	if((size_t)v % sizeof(void*)) v++; // get to aligned void**
	const_value* p = (const_value*)v;
	int i, j;
	n += 2; // skip vector header
	for(i=0; i<rows; i++)
	{
		const_value m = vGet(n);
		if(cols>1) m += 2; // skip vector header
		for(j=0; j<cols; j++)
		{
			*p++ = m;
			m = vNext(m);
		}
		n = vNext(n);
	}
	return (const_value*)v;
}


value transpose (value v)
{
	_init_n
	if(a!=VALUE_VECTOR) return v;
	int i, j, rows, cols;
	const_value t;

	_size(setRef(v, n));
	t = v+2; // +2 to skip vector header
	rows = getSmaInt(t);
	t = vNext(t);
	cols = getSmaInt(t);
	if(cols==0) return OPER_FAIL_(Operand_IsNot_Matrix);

	const_value* p = set_matrix_pointers(v,n,rows,cols);
	v = (value)(p + rows*cols);
	value w = v+2;
	for(j=0; j<cols; j++)
	{
		value u=0;
		if(rows>1)
		{
			u=w;
			w+=2;
		}
		for(i=0; i<rows; i++)
			w = unOffset(vCopy(w, p[i*cols+j]));
		if(rows>1)
			setVector(u, rows, w-u-2);
	}
	setVector(v, cols, w-v-2);
	return vCopy(y, v);
}


value __mul (value v)
{
	_init_n_m
	if(a!=VALUE_VECTOR || b!=VALUE_VECTOR) return _mul(v);
	int i, j, k, rowN, colN, rowM, colM;
	const_value t;

	_size(setRef(v, n));
	t = v+2; // +2 to skip vector header
	rowN = getSmaInt(t);
	t = vNext(t);
	colN = getSmaInt(t);

	_size(setRef(v, m));
	t = v+2; // +2 to skip vector header
	rowM = getSmaInt(t);
	t = vNext(t);
	colM = getSmaInt(t);

	const_Str2 argv[5];
	enum ID_TWST error=0;

	int argc=1;
	argv[argc++] = TWSF(Oper_mul2);
	     if(colN==0) error = Left_IsNot_Matrix;
	else if(colM==0) error = Right_IsNot_Matrix;

	else if(rowN==3 && colN==1 && rowM==3 && colM==1)
	{
		// do vector multiplication
		const_value _n[3], _m[3];
		n+=2; _n[0]=n; for(i=1; i<3; i++) { n = vNext(n); _n[i]=n; }
		m+=2; _m[0]=m; for(j=1; j<3; j++) { m = vNext(m); _m[j]=m; }

		v = _mul(setRef(setRef(v, _n[1]), _m[2]));
		v = _mul(setRef(setRef(v, _n[2]), _m[1]));
		v = _sub(v);
		v = _mul(setRef(setRef(v, _n[2]), _m[0]));
		v = _mul(setRef(setRef(v, _n[0]), _m[2]));
		v = _sub(v);
		v = _mul(setRef(setRef(v, _n[0]), _m[1]));
		v = _mul(setRef(setRef(v, _n[1]), _m[0]));
		v = _sub(v);

		v = tovector(v,3);
		v = vPrevCopy(y, v);
	}
	else if(colN == rowM) // do matrix multiplication
	{
		const_value* pn = set_matrix_pointers(v,n,rowN,colN);
		v = (value)(pn + rowN*colN);
		const_value* pm = set_matrix_pointers(v,m,rowM,colM);
		v = (value)(pm + rowM*colM);
		value w = v+2;
		for(i=0; i<rowN; i++)
		{
			value u = w;
			if(colM!=1) w+=2;
			for(j=0; j<colM; j++)
			{
				w = setSmaInt(w,0);
				for(k=0; k<colN; k++)
				{
					w = setRef(w, pn[i*colN+k]);
					w = setRef(w, pm[k*colM+j]);
					w = _add(_mul(w));
				}
				w = unOffset(w);
			}
			if(colM!=1) setVector(u, colM, w-u-2);
		}
		if(rowN==1 && colM==1) v+=2;
		else setVector(v, rowN, w-v-2);
		v = vCopy(y, v);
	}
	else // invalid matrix multiplication
	{
		argc=1;
		argv[argc++] = TIS2(0, rowN);
		argv[argc++] = TIS2(1, colN);
		argv[argc++] = TIS2(2, rowM);
		argv[argc++] = TIS2(3, colM);
		error = MatrixMult_IsInvalid;
	}

	if(error){
		argv[0] = TWST(error);
		v = setMessage(y, error, argc, argv);
	}
	return v;
}


value __div (value v)
{
	_init_n_m
	if(!n && !m) return v; // does nothing, just for unused variable warning
	if(b==VALUE_VECTOR) { OPER_FAIL_(Right_IsNot_Single); return v; }
	return _div(v);
}

value power (value v)
{
	_init_n_m
	if(!n && !m) return v; // does nothing, just for unused variable warning
	if(b==VALUE_VECTOR) { OPER_FAIL_(Right_IsNot_Single); return v; }
	if(a==VALUE_VECTOR) { OPER_FAIL_(Left_IsNot_Single); return v; }
	return _pow(v);
}



//---------------------------------------------------------------

value _add (value v)
{
	_init_n_m

	if(a==aString || b==aString)
	{
		v = VstToStr(setRef(v,n), 0); n = vGetPrev(v);
		v = VstToStr(setRef(v,m), 0); m = vGetPrev(v);
		a = ARRAY_LEN(*n); n+=2;
		b = ARRAY_LEN(*m); m+=2;
		wchar* w = (wchar*)(y+2);
		memcpy(w  , n, a*sizeof(wchar));
		memcpy(w+a, m, b*sizeof(wchar));
		*(w+a+b) = '\0';
		return onSetStr2(y, w+a+b);
	}
	if(a==VALUE_VECTOR || b==VALUE_VECTOR) return vCopy(y,OPER_TYPE2(v,n,m,_add));
	if(b==aSmaInt && VTYPE(*n)==VALUE_CHARAC) return setCharac(y, (wchar)(ni + mi));
	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt): v = setSmaInt(y, ni + mi); break;
		case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, ni + mf); break;
		case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, ni + mc); break;

		case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, nf + mi); break;
		case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, nf + mf); break;
		case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, nf + mc); break;

		case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, nc + mi); break;
		case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, nc + mf); break;
		case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, nc + mc); break;

		default: OPER_FAIL break;
	}
	return v;
}


value _sub (value v)
{
	_init_n_m
	if(a==VALUE_VECTOR || b==VALUE_VECTOR) return vCopy(y,OPER_TYPE2(v,n,m,_sub));
	if(b==aSmaInt && VTYPE(*n)==VALUE_CHARAC) return setCharac(y, (wchar)(ni - mi));
	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt): v = setSmaInt(y, ni - mi); break;
		case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, ni - mf); break;
		case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, ni - mc); break;

		case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, nf - mi); break;
		case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, nf - mf); break;
		case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, nf - mc); break;

		case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, nc - mi); break;
		case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, nc - mf); break;
		case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, nc - mc); break;

		default: OPER_FAIL break;
	}
	return v;
}


value _mul (value v)
{
	init_n_m(_mul)
	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt): v = setSmaInt(y, ni * mi); break;
		case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, ni * mf); break;
		case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, ni * mc); break;

		case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, nf * mi); break;
		case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, nf * mf); break;
		case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, nf * mc); break;

		case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, nc * mi); break;
		case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, nc * mf); break;
		case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, nc * mc); break;

		default: OPER_FAIL break;
	}
	return v;
}


value _div (value v)
{
	init_n_m(_div)
	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt): v = setSmaFlt(y, ni / (SmaFlt)mi); break;
		case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, ni / mf); break;
		case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, ni / mc); break;

		case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, nf / mi); break;
		case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, nf / mf); break;
		case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, nf / mc); break;

		case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, nc / mi); break;
		case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, nc / mf); break;
		case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, nc / mc); break;

		default: OPER_FAIL break;
	}
	return v;
}



/* --------- Exponent and Logarithm Functions ----------- */

value _cbrt (value v)
{
	init_n(_cbrt)
	switch(a)
	{
		case aSmaInt: v = setSmaFlt(y, pow(ni, 1/3.0)); break;
		case aSmaFlt: v = setSmaFlt(v, pow(nf, 1/3.0)); break;
		case aSmaCom: if(COMPLEX) { v = setSmaCom(y, _cpow(nc, 1/3.0)); break; }
		default: OPER_FAIL break;
	}
	return v;
}


value _sqrt (value v)
{
	bool b=0;
	init_n(_sqrt)
	switch(a)
	{
		case aSmaInt: b=1; nf = ni; break;
		case aSmaFlt: b=1; break;
		case aSmaCom: if(COMPLEX) { v = setSmaCom(y, _csqrt(nc)); break; }
		default: OPER_FAIL break;
	}
	if(b){
		if(nf>=0) v = setSmaFlt(y, sqrt(nf));
		else v = setSmaCom2(y, 0, sqrt(-nf));
	}
	return v;
}


value _log (value v)
{
	int b=0;
	init_n(_log)
	switch(a)
	{
		case aSmaInt: b=1; nf = ni; break;
		case aSmaFlt: b=1; break;
		case aSmaCom: if(COMPLEX) { b=2; break; }
		default: OPER_FAIL break;
	}
	if(b==1)
	{
		if(nf>0) v = setSmaFlt(y, log(nf));
		else OPER_FAIL_(Argument_OutOf_Domain);
	}
	else if(b==2)
	{
		if(ccabs(nc)>0) v = setSmaCom(y, _clog(nc));
		else OPER_FAIL_(Argument_OutOf_Domain);
	}
	return v;
}



/* --------- Trigonometric and Hyperbolic Functions ---- */

static value TYPE1_OPR (
	value v,
	value (*CALL)(value v),
	SmaFlt (*SF_CALL)(SmaFlt x),
	SmaCom (*SC_CALL)(SmaCom x) )
{
	init_n(CALL)
	switch(a)
	{
		case aSmaInt: v = setSmaFlt(y, SF_CALL(ni)); break;
		case aSmaFlt: v = setSmaFlt(y, SF_CALL(nf)); break;
		case aSmaCom: if(COMPLEX) { v = setSmaCom(y, SC_CALL(nc)); break; }
		default: OPER_FAIL break;
	}
	return v;
}

static value INV_TRIGO (
	value v,
	value (*CALL)(value v),
	SmaFlt (*SF_CALL)(SmaFlt x),
	SmaCom (*SC_CALL)(SmaCom x) )
{
	int b=0;
	init_n(CALL)
	switch(a)
	{
		case aSmaInt: b=1; nf = ni; break;
		case aSmaFlt: b=1; break;
		case aSmaCom: if(COMPLEX) { b=2; break; }
		default: OPER_FAIL break;
	}
	if(b==1)
	{
		if(-1 <= nf && nf <= 1) v = setSmaFlt(y, SF_CALL(nf));
		else OPER_FAIL_(Argument_OutOf_Domain);
	}
	else if(b==2)
	{
		SmaFlt sf = ccabs(nc);
		if(-1 <= sf && sf <= 1) v = setSmaCom(y, SC_CALL(nc));
		else OPER_FAIL_(Argument_OutOf_Domain);
	}
	return v;
}

/*#ifndef acosh
static SmaFlt acosh(SmaFlt x) { return log(x + sqrt(x*x - 1)); }
static SmaFlt asinh(SmaFlt x) { return log(x + sqrt(x*x + 1)); }
static SmaFlt atanh(SmaFlt x) { return (log(1+x) - log(1-x))/2; }
#endif*/

value _exp   (value v) { return TYPE1_OPR (v, _exp, exp, _cexp); }

value _cos   (value v) { return TYPE1_OPR (v, _cos, cos, _ccos); }
value _sin   (value v) { return TYPE1_OPR (v, _sin, sin, _csin); }
value _tan   (value v) { return TYPE1_OPR (v, _tan, tan, _ctan); }

value _cosh  (value v) { return TYPE1_OPR (v, _cosh, cosh, _ccosh); }
value _sinh  (value v) { return TYPE1_OPR (v, _sinh, sinh, _csinh); }
value _tanh  (value v) { return TYPE1_OPR (v, _tanh, tanh, _ctanh); }

value _acos  (value v) { return INV_TRIGO (v, _acos, acos, _cacos); }
value _asin  (value v) { return INV_TRIGO (v, _asin, asin, _casin); }
value _atan  (value v) { return TYPE1_OPR (v, _atan, atan, _catan); }

value _acosh (value v) { return TYPE1_OPR (v, _acosh, acosh, _cacosh); }
value _asinh (value v) { return TYPE1_OPR (v, _asinh, asinh, _casinh); }
value _atanh (value v) { return TYPE1_OPR (v, _atanh, atanh, _catanh); }



/* --------- Mainly For Complex Numbers ---------------- */

#define COMPL_OPR(v, CALL, SR_CALL, SF_CALL, SC_CALL) \
{ \
	init_n(CALL) \
	switch(a) \
	{ \
		case aSmaInt: v = setSmaInt(y, SF_CALL(ni)); break; \
		case aSmaFlt: v = setSmaFlt(y, SF_CALL(nf)); break; \
		case aSmaCom: v = setSmaFlt(y, SC_CALL(nc)); break; \
		/* line above has conversion from complex to float */ \
		default: OPER_FAIL break; \
	} \
	return v; \
}

#define TYPE1_COM(v, CALL, SF_CALL, SC_CALL) \
{ \
	init_n(CALL) \
	switch(a) \
	{ \
		case aSmaInt: v = setSmaInt(y, SF_CALL(ni)); break; \
		case aSmaFlt: v = setSmaFlt(y, SF_CALL(nf)); break; \
		case aSmaCom: v = setSmaCom(y, SC_CALL(nc)); break; \
		default: OPER_FAIL break; \
	} \
	return v; \
}

#define sf_abs(n) (((n)>=0) ? (n) : -(n))
#define sf_arg(n)  0
#define sf_real(n) n
#define sf_imag(n) 0
#define sf_conj(n) n
#define sf_proj(n) n

value _cabs (value v) COMPL_OPR (v, _cabs, sr_abs , sf_abs , ccabs)
value _carg (value v) COMPL_OPR (v, _carg, sr_arg , sf_arg , carg)
value _real (value v) COMPL_OPR (v, _real, sr_real, sf_real, creal)
value _imag (value v) COMPL_OPR (v, _imag, sr_imag, sf_imag, cimag)
value _conj (value v) TYPE1_COM (v, _conj, sf_conj, conj)
value _proj (value v) TYPE1_COM (v, _proj, sf_proj, _cproj)



/* ----------- Integer operations ------------------ */

value _idiv (value v)
{ return toInt(_div(v)); }

value _isqrt  (value v)
{ return toInt(_sqrt(v)); }

value _ceil (value v)
{
	init_n(_ceil)
	switch(a)
	{
		case aSmaInt: v = setSmaInt(y, ni); break; // TODO: this line is due to setRef() in OPER_TYPE1
		case aSmaFlt: v = setSmaFlt(y, ceil(nf)); break;
		default: OPER_FAIL break;
	}
	return v;
}

value _floor (value v)
{
	init_n(_floor)
	switch(a)
	{
		case aSmaInt: v = setSmaInt(y, ni); break;
		case aSmaFlt: v = setSmaFlt(y, floor(nf)); break;
		default: OPER_FAIL break;
	}
	return v;
}

value toInt (value v)
{
	init_n(toInt)
	switch(a)
	{
		case aSmaInt: v = setSmaInt(y, ni); break;
		case aSmaFlt: v = setSmaInt(y, FltToInt(nf)); break;
		default: OPER_FAIL break;
	}
	return v;
}


value factorial (value v)
{
	SmaFlt sf;
	init_n(factorial)
	switch(a)
	{
		case aSmaInt:
			if(ni <= -1) OPER_FAIL_(Argument_OutOf_Domain);
			else {
				for(sf=1; ni > 1; ni--)
					sf *= ni;
				ni = (SmaInt)sf;
				if(ni==sf)
				     v = setSmaInt(y, ni);
				else v = setSmaFlt(y, sf);
			}
			break;

		case aSmaFlt:
			if(nf <= -1) OPER_FAIL_(Argument_OutOf_Domain);
			else v = setSmaFlt(y, tgamma(nf+1));
			break;

		default: OPER_FAIL break;
	}
	return v;
}


value _mod (value v)
{
	init_n_m(_mod)
	if(a==aSmaInt && b==aSmaInt)
	{
		if(mi) v = setSmaInt(y, ni % mi);
		else OPER_FAIL_(Division_By_Zero);
	}
	else OPER_FAIL_(Only_On_Integer_Number);
	return v;
}


static SmaInt get_gcd (SmaInt a, SmaInt b)
{
	SmaInt t;
	if(b==0) return 1;
	while(a != 0)
	{ t = a; a = b%a; b = t; }
	return b;
}

value _gcd (value v)
{
	_init_n
	value e = check_arguments(y,*n,2); if(e) return e;
	n += 2; // skip vector header
	const_value m = vNext(n);
	n = vGet(n);
	m = vGet(m);
	a = value_type(n);
	int b = value_type(m);
	if(a==aSmaInt && b==aSmaInt)
	{
		SmaInt N = getSmaInt(n);
		SmaInt M = getSmaInt(m);
		if(M) v = setSmaInt(y, get_gcd(N, M));
		else OPER_FAIL_(Division_By_Zero);
	}
	else OPER_FAIL_(Only_On_Integer_Number);
	return v;
}


value _ilog (value v)
{
	_init_n
	value e = check_arguments(y,*n,2); if(e) return e;
	n += 2; // skip vector header
	const_value m = vNext(n);
	n = vGet(n);
	m = vGet(m);
	a = value_type(n);
	int b = value_type(m);
	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt):
			v = _log(setRef(v,m));
			v = _log(setRef(v,n));
			v = toInt(_div(v));
			v = vPrevCopy(y, v);
			break;
		default: OPER_FAIL_(Only_On_Integer_Number); break;
	}
	return v;
}



/* --------- Bitwise Operations ------------------------ */

#define BWISE_OPR(v, CALL, SI_CALL) \
{ \
	init_n_m(CALL) \
	if(a==aSmaInt && b==aSmaInt) \
	{ \
		v = setSmaInt(y, SI_CALL(ni, mi)); \
	} \
	else OPER_FAIL_(Only_On_Integer_Number); \
	return v; \
}

#define si_shift_left(n,m)  (n << m)
#define si_shift_right(n,m) (n >> m)
#define si_bitwise_xor(n,m) (n ^  m)
#define si_bitwise_or(n,m)  (n |  m)
#define si_bitwise_and(n,m) (n &  m)

value shift_left  (value v) BWISE_OPR (v, shift_left , si_shift_left)
value shift_right (value v) BWISE_OPR (v, shift_right, si_shift_right)
value bitwise_xor (value v) BWISE_OPR (v, bitwise_xor, si_bitwise_xor)
value bitwise_or  (value v) BWISE_OPR (v, bitwise_or , si_bitwise_or)
value bitwise_and (value v) BWISE_OPR (v, bitwise_and, si_bitwise_and)

value bitwise_not (value v)
{
	init_n(bitwise_not)
	if(a==aSmaInt) v = setSmaInt(y, ~ni);
	else OPER_FAIL_(Only_On_Integer_Number);
	return v;
}



/* --------- Comparison Operations --------------------- */

value lessThan (value v)
{
	init_n_m(lessThan)
	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt): a = ni < mi; break;
		case MIX(aSmaInt, aSmaFlt): a = ni < mf; break;
		case MIX(aSmaFlt, aSmaInt): a = nf < mi; break;
		case MIX(aSmaFlt, aSmaFlt): a = nf < mf; break;

		case MIX(aString, aString):
			a = -1==strcmp22(getStr2(n), getStr2(m)); break;

		default: { OPER_FAIL return v; }
	}
	return setBool(y, a);
}

value greaterThan (value v)
{
	_init_n_m
	// swap n and m positions, since n>m = m<n
	v = lessThan(setRef(setRef(v,m),n));
	return vPrevCopy(y, v);
}


value equalTo (value v)
{
	init_n_m(equalTo)
	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt): a = ni == mi; break;
		case MIX(aSmaInt, aSmaFlt): a = ni == mf; break;
		case MIX(aSmaInt, aSmaCom): a = ni == mc; break;

		case MIX(aSmaFlt, aSmaInt): a = nf == mi; break;
		case MIX(aSmaFlt, aSmaFlt): a = nf == mf; break;
		case MIX(aSmaFlt, aSmaCom): a = nf == mc; break;

		case MIX(aSmaCom, aSmaInt): a = nc == mi; break;
		case MIX(aSmaCom, aSmaFlt): a = nc == mf; break;
		case MIX(aSmaCom, aSmaCom): a = nc == mc; break;

		case MIX(aString, aString):
			a = 0==strcmp22(getStr2(n), getStr2(m)); break;

		default: a=false; break;
	}
	return setBool(y, a);
}

value sameAs (value v)
{
	v = equalTo(v);
	const_value n = vGetPrev(v);
	if(!isBool(n))
	{
		assert(VTYPE(*n)==VALUE_VECTOR);
		v = _not(_sum(_not(v)));
	}
	return v;
}



/* --------- Logical Operations ------------------------ */

#define OR_AND(CALL, OPR) \
{ \
	init_n_m(CALL) \
	switch(a) \
	{ \
		case aString: a = ns!=0; break; \
		case aSmaInt: a = ni!=0; break; \
		case aSmaFlt: a = nf!=0; break; \
		case aSmaCom: a = nc!=0; break; \
		default: a=false; break; \
	} \
	switch(b) \
	{ \
		case aString: b = ms!=0; break; \
		case aSmaInt: b = mi!=0; break; \
		case aSmaFlt: b = mf!=0; break; \
		case aSmaCom: b = mc!=0; break; \
		default: b=false; break; \
	} \
	return setBool(y, a OPR b); \
}

value logical_or  (value v) OR_AND(logical_or , ||)
value logical_and (value v) OR_AND(logical_and, &&)
value logical_not (value v)
{
	init_n(logical_not)
	switch(a)
	{
		case aString: a = ns!=0; break;
		case aSmaInt: a = ni!=0; break;
		case aSmaFlt: a = nf!=0; break;
		case aSmaCom: a = nc!=0; break;
		default: a=false; break;
	}
	return setBool(y, !a);
}



/*-----------------------------------------------------------*/

value _pow (value v)
{
	init_n_m(_pow)
	SmaFlt sf;

	switch(MIX(a,b))
	{
		case MIX(aSmaInt, aSmaInt):
			sf=1;
			if(mi >= 0){
				while(mi--) sf *= ni;
				mi = (SmaInt)sf;
			}
			else{
				mi = -mi;
				while(mi--) sf *= ni;
				sf = 1/sf;
			}
			if(mi==sf) v = setSmaInt(y, mi);
			else       v = setSmaFlt(y, sf);
			break;

		case MIX(aSmaFlt, aSmaInt):
			sf=1;
			if(mi >= 0){
				while(mi--) sf *= nf;
			}
			else {
				mi = -mi;
				while(mi--) sf *= nf;
				sf = 1/sf;
			}
			v = setSmaFlt(y, sf);
			break;

		case MIX(aSmaInt, aSmaFlt):
			if(ni>=0 || floor(mf)==mf)
				v = setSmaFlt(y, pow(ni, mf));
			else if(COMPLEX)
				v = setSmaCom(y, _cpow(ni, mf));
			break;

		case MIX(aSmaFlt, aSmaFlt):
			if(nf>=0 || floor(mf)==mf)
				v = setSmaFlt(y, pow(nf, mf));
			else if(COMPLEX)
				v = setSmaCom(y, _cpow(nf, mf));
			break;

		#ifdef COMPLEX
		case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, _cpow(ni, mc)); break;
		case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, _cpow(nf, mc)); break;
		case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, _cpow(nc, mi)); break;
		case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, _cpow(nc, mf)); break;
		case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, _cpow(nc, mc)); break;
		#endif

		default: OPER_FAIL break;
	}
	return v;
}



/*static SmaRat RatSmaFlt (value v)
{
	SmaFlt n = getSmaFlt(v);
	int i, j, len=0;
	bool neg;
	SmaFlt t;
	SmaRat sr = {0,0x1000000000000000};
	SmaInt g, w, a, b, c, d, BB;
	const int B=2*5;
	int digits[30];

	if(n<0) { n=-n; neg=1; } else neg=0;
	w = (SmaInt)n;
	if(w<0) { a=w; b=neg; } // if n = inf
	else
	{
		n -= w;
		BB=B;
		while(1)
		{
			g = (SmaInt)(n*BB); // get next decimal digit
			digits[len++] = (int)(g%B);
			if(BB>=1000000000)
			{
				c = get_gcd(g, BB); a = g/c; b = BB/c;
				if(b<sr.deno) { sr.nume = a; sr.deno = b; }
			}

			j=0;
			for(i=len-1-2; (i+1)*2 >= len; i--)
			{
				for(j=0; j<len-1-i; j++)
				{ if(*(digits+len-1-j) != *(digits+i-j)) break; }
				if(j==len-1-i) { j=len; break; }
			}

			if(j==len) // if a repetition has been found
			{
				a=0; b=1;
				for(j=0; j <= 2*i-(len-1); j++)
				{ a = a*B + digits[j]; b*=B; }

				c=0; d=1;
				for( ; j <= i; j++)
				{ c = c*B + digits[j]; d*=B; } d--;

				a = a*d+c;
				b = b*d;
				t = a/(SmaFlt)b - n;
				if(-1E-08 <= t && t <= 1E-08) break; // if t is close to zero
			}
			BB *= B;
			if(BB<0) { a = sr.nume; b = sr.deno; break; }
		}
		c = get_gcd(a, b); a /= c; b /= c;
		a += w*b; // add whole part
	}
	if(neg) a = -a;
	sr.nume = a;
	sr.deno = b;
	return sr;
}*/


/******************************************************************************************/

#define StrToIntGet(base, a, b, d) \
	else if(c>=a && c<=b) \
	{ \
		if(dp) deno *= base; \
		nume = nume*base + d + (int)(c-a); \
		if(nume<prev) { dp=4; break; } else prev=nume; \
	} \

value StrToVal (value out, const_Str2 str)  // single output value
{
	const_Str2 end;
	const_Str2 argv[2];
	*argv=NULL;

	wchar c = str[0];
	long len = strlen2(str);
	if(len==0)
	{
		out = setError(out, L"Given constant is empty.");
	}
	else if(c=='\'') // if a character
	{
		if(len<2 || str[len-1]!=c)
		{
			out = setError(out, L"Given character constant must end with ' .");
		}
		else
		{
			str += 1; // skip the starting and ending ''
			end = str + len-2;
			int i = wstr_to_wchar(&c, str, end);
			if(i>0 && str[i]=='\'') // if end of character string reached
				out = setCharac(out, c);
			else
				out = setError(out, L"Given character constant is invalid.");
		}
	}
	else if(c=='"') // if a string
	{
		if(len<2 || str[len-1]!=c)
		{
			out = setError(out, L"String constant must end with \" .");
		}
		else
		{
			wchar* w = (wchar*)(out+2);
			str += 1; // skip the starting and ending ""
			end = str + len-2;
			while(true)
			{
				if(str >= end) // if done extracting string
				{
					*w = '\0';
					out = onSetStr2(out, w);
					break;
				}
				if(str[0]=='\\'
				&& str[1]=='#'
				&& str[2]=='{') // if "\#{" found
				{
					const wchar* from = str+3; // skip "\#{"
					str = skipComment2(str+1);

					if(str[-1]=='#' && str[-2]=='}')
						{ while(from < str-2) *w++ = *from++; }
					else
					{ out = setError(out, L"String has \\#{ not ended by }#"); break; }
				}
				else if(str[0]=='#' && str[1]=='{') // if "#{" found
				{
					out = setError(out, L"String must not have #{ , use #\\{ instead."); break;
				}
				else if(str[0]=='#' && str[-1]=='}') // if "}#" found
				{
					out = setError(out, L"String must not have }# , use }\\# instead."); break;
				}
				else
				{
					int i = wstr_to_wchar(&c, str, end);
					if(i>0){
						*w++ = c;
						str += i;
					}
					if(i<0){
						i = -i;
						wchar w[i+1];
						strcpy22S(w, str, i);
						argv[1] = w;
						argv[0] = L"String has invalid character code '%s'.";
						out = setMessage(out, 0, 2, argv); break;
					}
					if(i==0) { assert(str >= end); str = end; }
				}
			}
		}
	}
	else if(!(c=='-' || c=='.' || isDigit(c)))
		out = setStr22(out, str);
	else
	{
		const wchar* s = str;
		int i=0, dp=0, neg=0;
		SmaInt prev=0, nume=0, deno=1;

		if(s[i] == '-') { neg=1; i++; }

		if(s[i]=='0' && s[i+1]=='x')
		{
			for(i+=2; i<len; i++)
			{
				c = s[i];
				if(c=='_') continue;
				if(c=='.') { if(++dp==2) break; }
				StrToIntGet(16, '0', '9', 0)
				StrToIntGet(16, 'A', 'F', 10)
				StrToIntGet(16, 'a', 'f', 10)
				else { dp=3; break; }
			}
		}
		else if(s[i]=='0' && s[i+1]=='b')
		{
			for(i+=2; i < len; i++) { c = s[i]; if(c=='_') continue; if(c=='.') { if(++dp==2) break; } StrToIntGet(2, '0', '1', 0) else { dp=3; break; } }
		}
		else if(s[i]=='0' && s[i+1]=='o')
		{
			for(i+=2; i < len; i++) { c = s[i]; if(c=='_') continue; if(c=='.') { if(++dp==2) break; } StrToIntGet(8, '0', '7', 0) else { dp=3; break; } }
		}
		else if(s[i] != '.')
		{
			for(    ; i < len; i++) { c = s[i]; if(c=='_') continue; if(c=='.') { if(++dp==2) break; } StrToIntGet(10, '0', '9', 0) else { dp=3; break; } }
		}
		else *argv = L"Number must not begin with a decimal point.";
		if(c=='.'
		&& dp==1) *argv = L"Number must not end with a decimal point.";
		if(dp==2) *argv = L"Number has more than one decimal point.";
		if(dp==3) *argv = L"Number contains an invalid digit.";
		if(dp==4) *argv = L"Number has too many digits; BigNum is not yet supported.";

		if(*argv) out = setMessage(out, 0, 1, argv);
		else{
			if(neg) nume = -nume;
			if(deno==1) out = setSmaInt(out, nume);
			else out = setSmaFlt(out, nume/(SmaFlt)deno);
		}
	}
	return out;
}



static wchar* IntToStrGet (wchar* out, SmaInt n, int base, uint32_t info)
{
	wchar* str=out;
	int off=0;

	if(n==(SmaInt)0x8000000000000000L) // TODO: why casting?
		return strcpy21(out, "inf");

	if(n<0) // if number is negative
	{
		n=-n;
		if(n<0) return strcpy21(out, "inf");
		*out++ = '-';
		off=1;  // offset for string-reverse()
	}

	switch(base)
	{
		case  2: off+=2; *out++='0'; *out++='b'; break;
		case  8: off+=2; *out++='0'; *out++='o'; break;
		case 16: off+=2; *out++='0'; *out++='x'; break;
		default: break;
	}

	wchar a, A = TOSTR_GET_OPER(info)==TOSTR_X ? 'A' : 'a';
	while(true)
	{
		a = n % base;
		n = n / base;
		*out++ = (a<10) ? (a+'0') : (a-10+A);
		if(n==0) break; // placed here because n may initially be 0
	}
	*out=0;
	strrev2(str+off); // reverse the digits

	int precision = TOSTR_GET_PREC(info);
	if(precision > 0) // here integers are actually floats!
	{
		*out++ = '.';
		while(precision--)
			*out++ = '0';
	}
	return out;
}



static wchar* FltToStrGet (wchar* out, SmaFlt n, int base, uint32_t info)
{
	bool e = TOSTR_GET_OPER(info) == TOSTR_E;

	int precision = TOSTR_GET_PREC(info);
	if(precision==0 && !(info & TOSTR_EXACT_PREC))
		precision=6;
	info = TOSTR_SET_PREC(info,0); // clear precision

	SmaInt w, d, scaler = base;

	if(!e)
	{
		int i = precision;
		while(i--){
			w = scaler * base;
			if(w < scaler) break; // if overflow
			else scaler = w;
		}
		if(n < 0){
			if(-n < 0)
				e = true;
			else{
				n = -n;                   // make n positive
				*out++ = '-';
			}
		}
	}
	if(!e){
		w = (SmaInt)n;                    // get whole part of n
		if(w<0) e = true;                 // if n is too large
	}
	if(!e){
		n = n-w;                          // get decimal part of n
		d = (SmaInt)((1.0+n)*scaler);     // scale and get it into integer

		if(w==0
		&& (d/base == scaler/base)        // if number is close to 0
		&& !(info & TOSTR_EXACT_PREC)
		&& ((SmaInt)(n*1000000000000L)))
			e = true;                     // then use scientific notation
	}

	if(!e)
	{
		if(d%base >= base/2)              // round the last decimal digit
			d += base/2;

		if(d >= 2*scaler) {               // check if decimal part is close to 1
			w += 1;                       // then increase whole part by 1
			d=scaler;                     // and setup for no decimal part
		}
		out = IntToStrGet (out, w, base, info);

		*out++ = '.';                     // put decimal point
		d /= base;                        // remove last digit of d
		IntToStrGet (out, d, base, info);

		int i = 1 + (base==10 ? 0 : 2);   // Set i to after "1" or "0x1".
		while(out[i])                     // Shift so to remove the
		{ *out = out[i]; out++; }         //  sub-string "1" or "0x1".

		if(!(info & TOSTR_EXACT_PREC))
			while(*(out-1)=='0') out--;   // remove trailing zeros

		if(*(out-1)=='.') out--;          // make sure to not end with a '.'
	}
	else
	{
		const char* longDouble = sizeof(SmaFlt)==sizeof(double) ? "" : "L";
		const char* opened = (isfinite(n) && (info & TOSTR_ESCAPE)) ? "(" : "";
		const char* closed = *opened ? ")" : "";

		char format[20+1000];
		sprintf(format, "%s%%.%d%sE%s", opened, precision, longDouble, closed);
		//printf("format = '%s'\n", format);

		char* str = format+20;
		sprintf(str, format, n);

		while(true)
		{
			char c = *str++;
			if(c=='\0') break;
			if(c!='E') *out++ = c;
			else
			{
				if(!(info & TOSTR_EXACT_PREC))
					while(*(out-1)=='0') out--; // remove trailing zeros

				if(*(out-1)=='.') out--; // make sure to not end with a '.'

				out = strcpy21(out, "*10^");
			}
		}
	}
	*out = 0;
	return out;
}



static Str2 ValToStr (const_value n, Str2 output, uint32_t info)
{
	wchar temp[100];
	wchar str[1000] = {0};
	wchar* out = str;
	*out=0;

	enum ValueType type = value_type(n);
	int base = type==VALUE_POINTER ? 16 : 10;

	switch(TOSTR_GET_OPER(info))
	{
		case TOSTR_x: base = 16; break;
		case TOSTR_X: base = 16; break;
		case TOSTR_o: base =  8; break;
		case TOSTR_b: base =  2; break;
	}

	//if(type==VALUE_POINTER) IntToStrGet (out, (intptr_t)getPointer(n), base, info);
	     if(type==aSmaInt) IntToStrGet (out, getSmaInt(n), base, info);
	else if(type==aSmaFlt) FltToStrGet (out, getSmaFlt(n), base, info);
	else if(type!=aSmaCom) strcpy21(str, "Error in ValToStr().");
	else{
		info |= TOSTR_ESCAPE;

		SmaCom sc = getSmaCom(n);
		FltToStrGet (out, creal(sc), base, info);

		SmaFlt sf = cimag(sc);
		int i = base==10 ? 0 : 2;

		if((out[i]=='0' && out[i+1]=='\0')
		|| (out[0]=='-' && out[i+1]=='0' && out[i+2]=='\0'))
		{
			*out = 0; // if n == 0+bi

			if(sf<0){
				sf = -sf;
				*out++ = '-';
			}
			temp[i]=0;
			FltToStrGet (temp, sf, base, info);

			if(!(temp[i]=='1' && temp[i+1]=='\0')) // if n != 0+1i
				out = strcpy22(out, temp);

			out = strcpy22(out, TWSF(SQRT_of_Neg_One));
		}
		else
		{
			out += strlen2(out); // if n == a+bi

			if(sf<0){
				sf = -sf;
				*out++ = '-';
			} else *out++ = '+';

			temp[i]=0;
			FltToStrGet (temp, sf, base, info);

			if(!(temp[i]=='1' && temp[i+1]=='\0')) // if n != a+1i
				out = strcpy22(out, temp);

			out = strcpy22(out, TWSF(SQRT_of_Neg_One));
		}
	}
	return strcpy22(output, str);
}



static Str2 VstToStrGet (const_value v, Str2 out, uint32_t info)
{
	v = vGet(v);
	enum ValueType type = VTYPE(*v);

	uint32_t count;
	if(type==VALUE_VECTOR){
		count = VECTOR_LEN(*v);
		v += 2; // skip vector header
	} else count=1;

	if((info & TOSTR_NEWLINE) && type==VALUE_VECTOR)
	{
		const_value n = v;
		int c = count;
		while(true)
		{
			if(c==0) { info &= ~TOSTR_NEWLINE; break; }
			uint32_t a = *vGet(n);
			if(VTYPE(a)==VALUE_VECTOR && VECTOR_LEN(a)) break;
			n = vNext(n); c--;
		}
	}

	if(!(info & TOSTR_VAL_TYPE)) // do a number of checks for software error
	{
		assert(type != VALUE_MESSAGE);
		assert(type != VALUE_OFFSET);
		assert(type != VALUE_POINTER || (*v & 0x0F000000)==0);
	}
	if(type != VALUE_VECTOR)
	{
		if(info & TOSTR_CATEGORY)
		{
			const char* s = "v";
			if(info & TOSTR_VAL_TYPE)
			{
				switch(type)
				{
				case VALUE_MESSAGE: s = "error"  ; break;
				case VALUE_OFFSET : s = "offset" ; break;
				case VALUE_ENUMER : s = "bool"   ; break;
				case VALUE_CHARAC : s = "wchar"  ; break;
				case VALUE_POINTER: s = "pointer"; break;
				default:
					switch(value_type(v))
					{
					case aSmaInt    : s = "integer"; break;
					case aSmaFlt    : s = "float"  ; break;
					case aSmaRat    : s = "ratio"  ; break;
					case aSmaCom    : s = "complex"; break;
					case aString    : s = "string" ; break;
					default: s="???"; break;
					}
				}
			}
			out = strcpy21(out,s);
		}
		else if(type==VALUE_MESSAGE)
		{
			out = strcpy22(out, getMessage(v));
		}
		else if(type==VALUE_CHARAC)
		{
			wchar c = (wchar)(*v & 0x0FFFFFFF); // get character
			if(info & TOSTR_ESCAPE)
			{
				*out++ = '\'';
				if(c=='\\' || c=='\'') { *out++ = '\\'; *out++ =  c ; }
				else if(isPrintableASCII(c)) *out++ = c;
				else out = wchar_to_wstr(out, c);
				*out++ = '\'';
			}
			else *out++ = c;
			*out = 0;
		}
		else if(type==VALUE_ENUMER)
		{
			const char* s = NULL;
			uint32_t a = *v;
			if(isBool(v)) s = (a&1) ? "true" : "false";
			else out = IntToStrGet(out, (a & 0xFFFF), 8, 0); // base 8 for no reason!
			if(s) out = strcpy21(out,s);
		}
		else if(type==VALUE_ARRRAY)
		{
			if(info & TOSTR_ESCAPE)
			{
				*out++ = '"';
				if(isStr2(v))
				{
					const wchar* s = getStr2(v);
					for(; *s; s++)
					{
						wchar c = *s;
						if(c=='\\' || c=='"') { *out++ = '\\'; *out++ =  c ; }
						else if(isPrintableASCII(c)) *out++ = c;
						else out = wchar_to_wstr(out, c);
					}
				}
				else assert(false);
				*out++ = '"';
				*out = 0;
			}
			else if(isStr1(v)) out = strcpy21(out, getStr1(v));
			else if(isStr2(v)) out = strcpy22(out, getStr2(v));
			else assert(false);
		}
		else out = ValToStr(v, out, info);
	}
	else if(count)
	{
		out = strcpy21(out,"(");

		uint32_t info2 = info & ~TOSTR_NEWLINE; // remove the _newline flag
		out = VstToStrGet (v, out, info2);
		count--;
		v = vNext(v);

		while(count)
		{
			out = strcpy21(out,",");
			if(info & TOSTR_NEWLINE) out = strcpy21(out,"\r\n");
			out = strcpy21(out," ");
			out = VstToStrGet (v, out, info2);
			count--;
			v = vNext(v);
		}
		out = strcpy21(out,")");
	}
	return out;
}

value VstToStr (value v, uint32_t info)
{
	assert(v!=NULL); if(!v) return v;
	_init_n
	wchar* out = (wchar*)(v+2);
	bool b = (info & TOSTR_CATEGORY) && (VTYPE(*n)!=VALUE_VECTOR || VECTOR_LEN(*n)==0);
	if(b) *out++ = '(';
	out = VstToStrGet(n, out, info);
	if(b) *out++ = ')';
	*out = 0;
	return vPrevCopy(y, onSetStr2(v, out));
}

