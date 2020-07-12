/*
	_strfun.c
*/
#include "_string.h"
#include "_malloc.h"
#include "_math.h"


Str1 str1_alloc (Str1 s, long n)
{
	if(n) n++;
	void* ptr = _realloc(s, n*sizeof( char), " char");
	return (ptr || !n) ? (char*)ptr : s;
}

Str2 str2_alloc (Str2 s, long n)
{
	if(n) n++;
	void* ptr = _realloc(s, n*sizeof(wchar), "wchar");
	return (ptr || !n) ? (wchar*)ptr : s;
}


static lchar* lchar_new ()
{
	lchar* lchr = (lchar*)_malloc(sizeof(lchar), "lchar");
	memset(lchr, 0, sizeof(lchar));
	return lchr;
}

Str3 str3_alloc (Str3 s, long n)
{
	if(n>0)
	{
		lchar *lchr1, *lchr2;
		str3_free(s);

		lchr1 = lchar_new();
		if(lchr1) while(n--)
		{
			lchr2 = lchar_new();
			if(!lchr2) break;
			lchr2->next = lchr1;
			lchr1->prev = lchr2;
			lchr1 = lchr2;
		}
		return C37(lchr1);
		// The end-of-string character is
		// also added here implicitly
	}
	else if(s.end==NULL) // if s is allocated memory
	{
		lchar* ptr = s.ptr;
		while(ptr)
		{
			lchar* lchr = ptr;
			ptr = ptr->next;
			memset(lchr, 0, sizeof(lchar));
			_free(lchr, "lchar");
		}
	}
	return C37(NULL);
}


Str3 astrcpy32 (Str3 out, const_Str2 in, const_Str2 source)
{
	out = str3_alloc(out, strlen2(in));
	strcpy32(out, in); // must come before
	set_line_coln_source(out, 1, 1, source);
	return out;
}


Str3 set_lchar_array (lchar* array, long n, const_Str2 in, const_Str2 source)
{
	assert(array);
	assert(n >= strlen2(in)+1);
	const mchar empty = {0};
	long i;
	for(i=0; i<n; i++)
	{
		array[i].next = (i+1< n) ? &array[i+1] : NULL;
		array[i].prev = (i-1>=0) ? &array[i-1] : NULL;
		array[i].chr = empty;
	}
	Str3 out = {array, NULL};
	strcpy32(out, in); // must come before
	set_line_coln_source(out, 1, 1, source);
	out.end = &array[n-1];
	return out;
}


/***********************************************************************************************************/

// static
Str1 strcpy13 (Str1 out, const_Str3 in);
Str3 strcpy31 (Str3 out, const_Str1 in);

static Str1 str12={0}, str13={0};
static Str2 str21={0}, str23={0};
static Str3 str31={0};

static long len12=0, len13=0,
			len21=0, len23=0,
			len31=0;

void CXX_clean()
{
	str12 = str1_free(str12); len12=0;
	str13 = str1_free(str13); len13=0;
	str21 = str2_free(str21); len21=0;
	str23 = str2_free(str23); len23=0;
	str31 = str3_free(str31); len31=0;
}

#define CXX(str, len, _str_alloc, _strlen, _strcpy) \
{ \
	long n = _strlen(in); \
	if(n > len) \
	{ \
		len = n; \
		str = _str_alloc(str, n); \
	} \
	_strcpy(str, in); \
	return str; \
}
const_Str1 C12 (const_Str2 in) CXX(str12, len12, str1_alloc, strlen12, strcpy12)
const_Str1 C13 (const_Str3 in) CXX(str13, len13, str1_alloc, strlen3 , strcpy13)
const_Str2 C21 (const_Str1 in) CXX(str21, len21, str2_alloc, strlen21, strcpy21)
const_Str2 C23 (const_Str3 in) CXX(str23, len23, str2_alloc, strlen3 , strcpy23)
const_Str3 C31 (const_Str1 in) CXX(str31, len31, str3_alloc, strlen1 , strcpy31)


/* Temporal Integer to String: count = free memory position */
const_Str2 TIS2 (int count, long n)
{
	static wchar str[21][64];
	assert(0<=count && count<=20);
	if(!(0<=count && count<=20)) count=20;
	Str2 out = str[count];
	intToStr(out, n);
	return out;
}

/***********************************************************************************************************/


#define STRCPY(inEnd, outPtr, outEnd, outChar, inChar, outNext, inNext) \
{ \
	assert(outEnd==NULL); \
	bool getlen = outPtr==NULL; \
	for( ; !inEnd; in = inNext) \
	{ \
		if(getlen) outPtr++; \
		else { \
			outChar = inChar; \
			out = outNext; \
		} \
	} \
	if(!getlen) outChar = '\0'; \
	return out; \
}
Str1 strcpy11 (Str1 out, const_Str1 in) STRCPY (strEnd1(in), out    , NULL   ,  *out     ,  *in     ,  out+1    ,  in+1    )
Str2 strcpy22 (Str2 out, const_Str2 in) STRCPY (strEnd2(in), out    , NULL   ,  *out     ,  *in     ,  out+1    ,  in+1    )
Str1 strcpy13 (Str1 out, const_Str3 in) STRCPY (strEnd3(in), out    , NULL   ,  *out     , sChar(in),  out+1    , sNext(in))
Str2 strcpy23 (Str2 out, const_Str3 in) STRCPY (strEnd3(in), out    , NULL   ,  *out     , sChar(in),  out+1    , sNext(in))
Str3 strcpy31 (Str3 out, const_Str1 in) STRCPY (strEnd1(in), out.ptr, out.end, sChar(out),  *in     , sNext(out),  in+1    )
Str3 strcpy32 (Str3 out, const_Str2 in) STRCPY (strEnd2(in), out.ptr, out.end, sChar(out),  *in     , sNext(out),  in+1    )

Str3 strcpy33 (Str3 out, const_Str3 in)
{
	assert(out.end==NULL); // keep this line
	bool getlen = out.ptr==NULL;
	for( ; !strEnd3(in); in = sNext(in))
	{
		if(getlen) out.ptr++;
		else {
			mChar(out) = mChar(in);
			out = sNext(out);
		}
	}
	if(!getlen)
	{
		if(in.ptr) mChar(out) = mChar(in);
		sChar(out) = '\0';
	}
	return out;
}

Str2 strcpy22S(Str2 out, const_Str2 in, long n)
{
	assert(out!=NULL); // maybe remove this assertion
	memcpy(out, in, n*sizeof(*in));
	if(out) out[n] ='\0';
	return out+n;
}

/***********************************************************************************************************/


Str1 strcpy12 (Str1 out, const_Str2 in)
{
	bool getlen = out==NULL;
	while(!strEnd2(in))
	{
		wchar c = *in++;
		if(getlen)
		{
		     if(c<=0x007F) out += 1;
		else if(c<=0x07FF) out += 2;
		else if(c<=0xFFFF) out += 3;
		else               out += 4;
		}
		else
		{
		     if(c<=0x007F) { *out++ = (char)c; }

		else if(c<=0x07FF) {
			*out++ = 0xC0|((c>>6*1)&0x1F);
			*out++ = 0x80|((c>>6*0)&0x3F); }

		else if(c<=0xFFFF) {
			*out++ = 0xE0|((c>>6*2)&0x0F);
			*out++ = 0x80|((c>>6*1)&0x3F);
			*out++ = 0x80|((c>>6*0)&0x3F); }

		#if __WCHAR_MAX__ > 0x10000
		else {
			if(c>0x10FFFF) c = 0x10FFFF;
			*out++ = 0xF0|((c>>6*3)&0x07);
			*out++ = 0x80|((c>>6*2)&0x3F);
			*out++ = 0x80|((c>>6*1)&0x3F);
			*out++ = 0x80|((c>>6*0)&0x3F); }
		#endif
		}
	}
	if(!getlen) *out = '\0';
	return out;
}

Str2 strcpy21 (Str2 out, const_Str1 in)
{
	bool getlen = out==NULL;
	while(!strEnd1(in))
	{
		wchar c = (unsigned char)*in;
		int i=1, j;
		if(c&0x80) {
			for(j=0x40; (c & j); j>>=1) i++;
		}
		if(getlen) out++;
		else
		{
			if(i==2) c =
				((in[0]&0x1F)<<6*1) |
				((in[1]&0x3F)<<6*0) ;

			else if(i==3) c =
				((in[0]&0x0F)<<6*2) |
				((in[1]&0x3F)<<6*1) |
				((in[2]&0x3F)<<6*0) ;

			#if __WCHAR_MAX__ > 0x10000
			else if(i==4) c =
				((in[0]&0x07)<<6*3) |
				((in[1]&0x3F)<<6*2) |
				((in[2]&0x3F)<<6*1) |
				((in[3]&0x3F)<<6*0) ;
			#endif
			*out++ = c;
		}
		in += i;
	}
	if(!getlen) *out = '\0';
	return out;
}

/***********************************************************************************************************/


#define STRCMP(a_strEnd, b_strEnd, a_strChar, b_strChar, a_strNext, b_strNext) \
{ \
	while(true) \
	{ \
		if(b_strEnd) return a_strEnd ? 0 : +1; \
		if(a_strEnd) return -1; \
		wchar c = a_strChar; \
		wchar d = b_strChar; \
		if(c != d) return (c < d) ? -1 : +1; \
		a_strNext; \
		b_strNext; \
	} \
	return 0; \
}
int strcmp11 (const_Str1 a, const_Str1 b) STRCMP (strEnd1(a), strEnd1(b),  *a     ,  *b     , a++       , b++       )
int strcmp21 (const_Str2 a, const_Str1 b) STRCMP (strEnd2(a), strEnd1(b),  *a     ,  *b     , a++       , b++       )
int strcmp22 (const_Str2 a, const_Str2 b) STRCMP (strEnd2(a), strEnd2(b),  *a     ,  *b     , a++       , b++       )
int strcmp31 (const_Str3 a, const_Str1 b) STRCMP (strEnd3(a), strEnd1(b), sChar(a),  *b     , a=sNext(a), b++       )
int strcmp32 (const_Str3 a, const_Str2 b) STRCMP (strEnd3(a), strEnd2(b), sChar(a),  *b     , a=sNext(a), b++       )
int strcmp33 (const_Str3 a, const_Str3 b) STRCMP (strEnd3(a), strEnd3(b), sChar(a), sChar(b), a=sNext(a), b=sNext(b))


#define STR_EMPTY(strEnd, strChar, strNext) \
{ \
	for( ; !strEnd; strNext) \
	{ \
		wchar c = strChar; \
		if(c == stop) break; \
		if(!isSpace(c)) return false; \
	} \
	return true; \
}
bool strEmpty1 (const_Str1 s, wchar stop) STR_EMPTY (strEnd1(s),  *s     , s++       )
bool strEmpty2 (const_Str2 s, wchar stop) STR_EMPTY (strEnd2(s),  *s     , s++       )
bool strEmpty3 (const_Str3 s, wchar stop) STR_EMPTY (strEnd3(s), sChar(s), s=sNext(s))

/***********************************************************************************************************/


/* trim (remove) leading and trailing spaces */
void strtrim1 (Str1 s)
{
	long i=0, j, len = strlen1(s);

	for(j=0; j<len; j++)            // trim leading spaces
		if(!isSpace(s[j])) break;

	for(; j<len; j++, i++)          // copy middle characters
		s[i] = s[j];

	for(j=i-1; j>=0; j--)           // trim trailing spaces
		if(!isSpace(s[j])) break;

	if(++j < len) s[j]=0;           // end string
}

/* trim (remove) leading and trailing spaces */
void strtrim2 (Str2 s)
{
	long i=0, j, len = strlen2(s);

	for(j=0; j<len; j++)            // trim leading spaces
		if(!isSpace(s[j])) break;

	for(; j<len; j++, i++)          // copy middle characters
		s[i] = s[j];

	for(j=i-1; j>=0; j--)           // trim trailing spaces
		if(!isSpace(s[j])) break;

	if(++j < len) s[j]=0;           // end string
}

/* reverse characters of given string */
void strrev2 (Str2 s)
{
	long i, len = strlen2(s);
	for(i=0; i<len/2; i++)      // from start to middle of string
	{
		wchar c = s[i];         // swap values at position 'i' and 'len-1-i'
		s[i] = s[len-1-i];
		s[len-1-i] = c;
	}
}
