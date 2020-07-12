#ifndef _VALUE_H
#define _VALUE_H
/*
	value.h
*/

#include "_stddef.h"

#define vGetPrev(v) vGet(vPrev(v))

#define VERROR(v) ((*vGetPrev(v)>>28)==VAL_MESSAGE)

#define VEC_LEN(a) ((a) & 0x07FFFFFF)

#define WCHAR(v) ((wchar*)(uintptr_t)(v))

#define vDebug(v,n,i) puts2(getStr2(vGet(vPrev(VstToStr(setRef(v,n),i,-1,-1)))))


long vSize (const_value n);
value vcopy (value out, const_value n);
value vpcopy (value out, value v);

static inline enum ValueType value_type (register const_value n)
{
	assert(n!=NULL); // keep this
	if(!n) return 0;
	uint32_t a = *n;
	enum ValueType type = a >> 28;
	switch(type)
	{
		case VAL_CHARAC:
		case VAL_ENUMER: type = aSmaInt; break;

		case VAL_NUMBER:
			a &= 0x0FFFFFFF;
			switch(a){
			case 0: type = aSmaInt; break;
			case 1: type = aSmaFlt; break;
			case 2: type = aSmaRat; break;
			case 3: type = aSmaCom; break;
			} break;

		case VAL_ARRRAY:
			if((n[1]>>28)==VAL_CHARAC)
				type = aString;
			break;

		default: break;
	}
	return type;
}


static inline SmaInt getSmaInt (register const_value v)
{
	uint32_t a = v[0];
	if((a>>28)==VAL_CHARAC) return (a & 0x0FFFFFFF);
	if((a>>28)==VAL_ENUMER) return (a & 0x0000FFFF);
	return *(SmaInt*)(v+1);
}
static inline value setSmaInt (register value v, register SmaInt si)
{
	assert(v!=NULL);
	v[0] = (VAL_NUMBER<<28) | 0;
	*(SmaInt*)(v+1) = si;
	v[3] = (VAL_OFFSET<<28) | 3;
	return v+4;
}
static inline value setSmaFlt (register value v, register SmaFlt si)
{
	assert(v!=NULL);
	v[0] = (VAL_NUMBER<<28) | 1;
	*(SmaFlt*)(v+1) = si;
	v[3] = (VAL_OFFSET<<28) | 3;
	return v+4;
}
static inline SmaFlt getSmaFlt (register const_value v) { return *(SmaFlt*)(v+1); }

static inline SmaInt FltToInt (SmaFlt n) { return (SmaInt)(n + (n>=0 ? +0.000001 : -0.000001)); }



static inline value setOffset(value v, long  off) { if(v) *v++ = (VAL_OFFSET<<28) | (off & 0x0FFFFFFF); return v; }
static inline value setCharac(value v, wchar chr) { if(v) *v++ = (VAL_CHARAC<<28) | (chr & 0x0FFFFFFF); return setOffset(v,1); }
static inline value setBool  (value v, bool  val) { if(v) *v++ = (VAL_ENUMER<<28) | (val             ); return setOffset(v,1); }

static inline void setVector (value y, uint32_t rows, uint32_t size)
{ assert(y); { y[0] = (VAL_VECTOR<<28) | (rows & 0x0FFFFFFF); y[1] = size; } }

static inline value setEmptyVector(value y) { setVector(y, 0, 0); return setOffset(y+2, 2); }


static inline value vPrev (register value v)
{
	assert(v!=NULL);
	uint32_t a = *--v; // note the --v
	if((a>>28)==VAL_OFFSET)
	{
		v -= (a & 0x0FFFFFFF);
		assert(a & 0x0FFFFFFF);
		assert((*v>>28)!=VAL_OFFSET); // TODO: on vPrev(): remove this line
	}
	else assert((a>>28)==VAL_POINTER && (a&0x08000000)); // assert is a small setRef()
	return v;
}

static inline const_value vNext (register const_value v)
{
	assert(v!=NULL);
	uint32_t a = *v;
	if((a>>28)==VAL_OFFSET)
	{
		v += (a & 0x0FFFFFFF);
		assert(a & 0x0FFFFFFF);
	}
	v += vSize(v);
	return v;
}
#define vNEXT(v) ((value)(uintptr_t)vNext(v))

static inline value vnext (register value v)
{
	assert(v!=NULL);
	v = vNEXT(v);
	assert((*v>>28)==VAL_OFFSET);
	v++; // skip the VAL_OFFSET
	return v;
}
#define unOffset(v) (v-1) // skip the VAL_OFFSET


static inline const_value vGet (register const_value v)
{
	assert(v!=NULL);
	while(true)
	{
		uint32_t a = *v;
		if((a>>28)==VAL_OFFSET)
		{
			v += (a & 0x0FFFFFFF);
			assert(a & 0x0FFFFFFF);
		}
		else if((a>>28)==VAL_POINTER)
		{
			if(a & 0x08000000)
			{
				v -= (a & 0x07FFFFFF);
				assert(a & 0x07FFFFFF);
			}
			else
			{
				int64_t n = *(int64_t*)(v+1);
				assert(n!=0);
				if(a & 0x04000000)
					v -= n;
				else if(a & 0x02000000)
				{
					v = (const_value)(intptr_t)n;
					if(v==NULL) break;
				}
			}
		}
		else break;
	}
	return v;
}

static inline value setRef (value v, const_value n) // set Reference pointer
{
	assert(v!=NULL);
	register uint64_t b = v>n ? (v-n) : -(uint64_t)(n-v);
	if(b < 0x08000000)
		*v++ = (VAL_POINTER<<28) | 0x08000000 | (uint32_t)b;
	else{
		v[0] = (VAL_POINTER<<28) | 0x04000000;
		*(int64_t*)(v+1) = b;
		v[3] = (VAL_OFFSET<<28) | 3;
		v += 4;
	}
	return v;
}

static inline value setAbsRef (value v, const_value n)
{
	assert(v!=NULL);
	register int64_t b = (intptr_t)n;
	v[0] = (VAL_POINTER<<28) | 0x02000000;
	*(int64_t*)(v+1) = b;
	v[3] = (VAL_OFFSET<<28) | 3;
	return v+4;
}


value setMessage  (value v, int code, int argc, const_Str2* argv);
value setMessageE (value v, int code, int argc, const_Str2* argv, const_Str3 str);
const_Str2 getMessage (const_value v);

static inline value setError (value v, const wchar* msg)
{ return setMessage(v, 0, 1, &msg); }

static inline value setErrorE (value v, const wchar* msg, const_Str3 str)
{ return setMessageE(v, 0, 1, &msg, str); }


static inline bool isBool (const_value v)
{ return v && (v[0] & 0xFFFFFFFE)==(VAL_ENUMER<<28) ; }

static inline bool isStr1 (const_value v)
{ return v && (v[0]>>28)==VAL_ARRRAY && v[1]==(VAL_ENUMER<<28 | 0x10000) ; }

static inline bool isStr2 (const_value v)
{ return value_type(v)==aString; }

static inline const_Str1 getStr1 (const_value n)
{
	assert(isStr1(n));
	Str1 s = (char*)(n+2);
	assert(!s[(*n & 0x0FFFFFFF)-1]); // assert '\0' termination
	return s;
}
static inline const_Str2 getStr2 (const_value n)
{
	assert(isStr2(n));
	Str2 s = (wchar*)(n+2);
	assert(!s[(*n & 0x0FFFFFFF)-1]); // assert '\0' termination
	return s;
}
value onSetStr1(value v, const_Str1 end);
value onSetStr2(value v, const_Str2 end);
value setStr21 (value v, const_Str1 in);
value setStr22 (value v, const_Str2 in);
value setStr23 (value v, const_Str3 in);

value value_alloc (value old, long size);
#define value_free(old) value_alloc(old, 0)

value check_arguments (value v, uint32_t c, uint32_t count);


// if in==NULL then the output matrix is filled with zeros
value integToValue (value out, int rows, int cols, const SmaInt* in);
value floatToValue (value out, int rows, int cols, const SmaFlt* in);

bool integFromValue (value v, int rows, int cols,  int  * output, const char* name);
bool floatFromValue (value v, int rows, int cols, SmaFlt* output, const char* name);

// return a 0-filled integer matrix
const_value VSTXX (int rows, int cols);
#define VST11 VSTXX(1,1)
#define VST21 VSTXX(2,1)
#define VST31 VSTXX(3,1)
#define VST41 VSTXX(4,1)
#define VST61 VSTXX(6,1)
#define VST33 VSTXX(3,3)


#endif
