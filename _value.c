/*
	value.c
*/

#include "_math.h"
#include "_texts.h"
#include "_string.h"
#include "_malloc.h"


long vSize (const_value v)
{
	if(!v) return 0;
	uint32_t a = *v;
	switch(a>>28)
	{
		case VAL_MESSAGE: return a ? 1+(a & 0xFFFF) : 0;
		case VAL_OPERAT: return 1+(a & 0xFFFF);

		case VAL_OFFSET: assert((a>>28)!=VAL_OFFSET); return 0;
		//case VAL_OFFSAT: assert((a>>28)!=VAL_OFFSAT); return 0;
		case VAL_POINTER: assert(!(a & 0x08000000)); return 3;

		case VAL_CHARAC: return 1;
		case VAL_ENUMER: return 1;
		//case VAL_STRUCT: return 1+(a & 0xFFFF);

		case VAL_NUMBER:
			a &= 0x0FFFFFFF;
			switch(a){
			case 0: return 1+2;
			case 1: return 1+2;
			case 2: return 1+4;
			case 3: return 1+4;
			default: assert(false); return 1+a;
			}

		//case VAL_DATA:
		case VAL_VECTOR: return 2+v[1];

		case VAL_ARRRAY: { v++; // skip to element ValueType
			uint32_t b = *v;
			assert((b>>28)==VAL_CHARAC); // TODO: for now strings are the only arrays.
			a &= 0x0FFFFFFF; // get number of array elements
			if((b>>28)==VAL_ENUMER)
			{
				b = ((b>>16)&0x0FFF); // get 12-bit type
			     if(b==0) a = (a+31)/32; // if array of 1-bit boolean
			else if(b==1) a = (a+3)/4;   // if array of 8-bit UTF8 char
			else a = (a+1)/2; // else if any other enumeration value = 16-bit
			}
			else if((b>>28)==VAL_CHARAC
			     && sizeof(wchar_t)==2)
				a = (a+1)/2;
			else a *= vSize(v);
			return 2+a; }

		default: assert(false); return 0;
	}
}


value vcopy (value out, const_value n)
{
	if(out){
		if(n){
			uint32_t size = vSize(n);
			memmove(out, n, size*sizeof(*n));
			out = setOffset(out+size, size);
		} else *out++ = 0;
	}return out;
}

value vpcopy (value out, value v)
{ return vcopy(out, vGetPrev(v)); }


value tovector (value v, uint32_t count)
{
	assert(count>=2);
	if(VERROR(v)) return v;
	if(count==1) return v;
	if(count==0) { setVector(v,0,0); return setOffset(v+2,2); }

	uint32_t i, s, *n, *y, *t;
	for(i=0; i<count; i++) { assert(v==vnext(vPrev(v))); v = vPrev(v); }
	y = n = v;
	for(i=0; i<count; i++)
	{
		t = vnext(n);
		s = vSize(n);
		memmove(y, n, s*sizeof(*n));
		n = t;
		y += s;
	}
	i = y - v;
	memmove(v+2, v, i*sizeof(*n));
	setVector(v, count, i);
	return setOffset(2+y, 2+i);
}


static value onSetMsg (value v, int code, int argc, Str2 out, Str2 end)
{
	if(argc) assert(*out && *end=='\0');
	long size = argc ? (end-out)+1 : 0;
	if(sizeof(wchar_t)==2) size = (size+1)/2;
	assert(!(code & ~0x0FFF) && !(size & ~0xFFFF));
	*v = (VAL_MESSAGE<<28) | (code<<16) | size;
	return setOffset(v+1+size, 1+size);
}

value setMessage (value v, int code, int argc, const_Str2* argv)
{
	if(!v) return v;
	Str2 out = (wchar*)(v+1);
	Str2 end = sprintf2(out, argc, argv);
	return onSetMsg(v, code, argc, out, end);
}

value setMessageE (value v, int code, int argc, const_Str2* argv, const_Str3 str)
{
	if(!v) return v;
	Str2 out = (wchar*)(v+1);
	Str2 end = set_message(out, argc, argv, str);
	return onSetMsg(v, code, argc, out, end);
}

const_Str2 getMessage (const_value v)
{
	const_Str2 s=NULL;
	uint32_t a = v ? *v : 0;
	assert((a>>28)==VAL_MESSAGE);
	if(!a);
	else if(a & 0xFFFF)
		s = (wchar*)(v+1);
	else {
		a = (a>>16) & 0x0FFF;
		if(a==0) s = L"Unknown error.";
		else if(IS_TWST_ID(a))
			s = TWST(a);
		else assert(IS_TWST_ID(a));
	}
	assert(!s || *s);
	return s;
}


value onSetStr1 (value v, const_Str1 end)
{
	assert(*end=='\0');
	long size = end - (Str1)(v+2) + 1; // +1 to include '\0'
	v[0] = (VAL_ARRRAY<<28) | size;
	v[1] = (VAL_ENUMER<<28) | 0x10000;
	size = (size+3)/4;
	return setOffset(v+2+size, 2+size);
}

value onSetStr2 (value v, const_Str2 end)
{
	assert(*end=='\0');
	long size = end - (Str2)(v+2) + 1; // +1 to include '\0'
	v[0] = (VAL_ARRRAY<<28) | size;
	v[1] = (VAL_CHARAC<<28);
	if(sizeof(wchar_t)==2) size = (size+1)/2;
	return setOffset(v+2+size, 2+size);
}

value setStr21 (value v, const_Str1 in) { return onSetStr2(v, strcpy21((Str2)(v+2), in)); }
value setStr22 (value v, const_Str2 in) { return onSetStr2(v, strcpy22((Str2)(v+2), in)); }
value setStr23 (value v, const_Str3 in) { return onSetStr2(v, strcpy23((Str2)(v+2), in)); }


value value_alloc (value old, long size)
{
	void* ptr = _realloc(old, size*sizeof(*old), "value");
	return (ptr || !size) ? (value)ptr : old;
}

value check_arguments (value v, uint32_t c, uint32_t count)
{
	c = ((c>>28)==VAL_VECTOR) ? (c & 0x0FFFFFFF) : 1;
	if(c!=count) // if error
	{
		const_Str2 argv[4];
		argv[0] = L"Expected %s argument%s not %s.";
		argv[1] = TIS2(0, count);
		argv[2] = count>2 ? L"s" : L"";
		argv[3] = TIS2(1, c);
		v = setMessage(v, 0, 4, argv);
	}
	else v=NULL;
	return v;
}


#include "_stdio.h"
value ToValue (value out, int rows, int cols, const void* in, bool setFloat)
{
	int i, j;
	size_t s = sizeof(SmaInt);
	assert(s== sizeof(SmaFlt));

	const char* n = (const char*)in;
	bool b = n!=NULL;
	value v=out;

	if(cols==1)
	{
		if(rows!=1) v+=2;
		for(i=0; i<rows; i++)
		{
			*v++ = (VAL_NUMBER<<28) | setFloat;
			if(b) memcpy(v, n, s);
			else  memset(v, 0, s);
			v += 2;
			n += s;
		}
		if(rows!=1) setVector(out, rows, v-out-2);
	}
	else
	{
		v+=2;
		for(i=0; i<rows; i++)
		{
			value w=v;
			v+=2;
			for(j=0; j<cols; j++)
			{
				*v++ = (VAL_NUMBER<<28) | setFloat;
				if(b) memcpy(v, n, s);
				else  memset(v, 0, s);
				v += 2;
				n += s;
			}
			setVector(w, cols, v-w-2);
		}
		setVector(out, rows, v-out-2);
	}
	return setOffset(v, v-out);
}

value integToValue (value out, int rows, int cols, const SmaInt* in) { return ToValue(out, rows, cols, in, 0); }
value floatToValue (value out, int rows, int cols, const SmaFlt* in) { return ToValue(out, rows, cols, in, 1); }


static void onFail (value v, int rows, int cols, const char* name)
{
	if(name==NULL) name = "result";
	value stack = v;

	const_Str2 argv[4];
	if(rows==1 && cols==1)
	     argv[0] = L"Error: %s must evaluate to a single value.";
	else argv[0] = L"Error: %s must evaluate to a (%s by %s) matrix.";

	v = setStr21(v, name);
	argv[1] = getStr2(vGetPrev(v));

	argv[2] = (Str2)v; v+=64; intToStr((Str2)v, rows);
	argv[3] = (Str2)v; v+=64; intToStr((Str2)v, cols);

	vpcopy(stack, setMessage(v, 0, 4, argv));
}

static bool FromValue (value v, int rows, int cols,  void* out, bool getInt, const char* name)
{
	value y = vPrev(v);
	const_value n = vGet(y);
	if(!value_type(n)) return v;
	int i, j;
	const_value t;
	SmaFlt* outF = (SmaFlt*)out;
	int   * outI = (int   *)out;

	_size(setRef(v, n));
	t = v+2; // +2 to skip vector header
	i = getSmaInt(t);
	t = vNext(t);
	j = getSmaInt(t);

	if(i!=rows || j!=cols)
	{
		onFail(y, rows, cols, name);
		return false;
	}
	if(!out) return true;

	if(rows!=1 || cols!=1) n += 2; // skip vector header
	for(i=0; i<rows; i++)
	{
		t = vGet(n);
		if(cols!=1) t += 2; // skip vector header
		for(j=0; j<cols; j++)
		{
			if(getInt)
			{
				toInt(setRef(v, vGet(t)));
				if(value_type(v)!=aSmaInt)
				     *outI++ = 0;
				else *outI++ = (int)getSmaInt(v);
			}
			else // if(getFloat)
			{
				toFlt(setRef(v, vGet(t)));
				if(value_type(v)!=aSmaFlt)
				     *outF++ = 0;
				else *outF++ = getSmaFlt(v);
			}
			t = vNext(t);
		}
		n = vNext(n);
	}
	return true;
}

bool integFromValue (value v, int rows, int cols,  int  * out, const char* name) { return FromValue(v, rows, cols, out, 1, name); }
bool floatFromValue (value v, int rows, int cols, SmaFlt* out, const char* name) { return FromValue(v, rows, cols, out, 0, name); }


static uint32_t memory[100];

const_value VSTXX (int rows, int cols)
{
	int size = (cols==1) ? (rows==1) ? 3 : (2+rows*3) : (2+rows*(2+cols*3));
	if(size+1 >= SIZEOF(memory)) { assert(false); return NULL; }
	integToValue(memory, rows, cols, NULL);
	return memory;
}

