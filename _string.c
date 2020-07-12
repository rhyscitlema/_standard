/*
	_string.c
*/

#include "_math.h"
#include "_texts.h"
#include "_stdio.h"
#include "_string.h"


static value get_strings (value y, const_value n, const wchar* out[], int count)
{
	uint32_t a = *n;
	if(VTYPE(a)==VALUE_VECTOR){
		a = VECTOR_LEN(a);
		n += 2; // skip vector header
	} else a=1;

	if(a == count){
		for(a=0; a<count; a++) {
			if(!isStr2(n)) break;
			out[a] = getStr2(n);
			n = vNext(n);
		}
	}
	if(a != count){
		const_Str2 argv[3] = {
			L"Call argument%s must be %s string%|1s.",
			(count==1 ? L"" : L"s"),
			TIS2(0,count),
		};
		return setMessage(y, 0, 3, argv);
	}
	return NULL;
}

#define _init_n \
	value y = vPrev(v); \
	const_value n = vGet(y); \
	uint32_t a = value_type(n); \
	if(a==VALUE_MESSAGE) return v; \

#define _init_s(count) \
	_init_n \
	const wchar* s[2] = {0}; \
	value e = get_strings(y, n, s, count); \
	if(e) return e; \

#define _init_str \
	_init_s(1) \
	const_Str2 str = s[0]; \
	const_Str2 end = str + strlen2(str); \


/*********************************************/
// chr => character
// pcn => partly code number
// is_fcn => is fully code number

static bool isfcn[0x10000] = {false};

static void initial_SetIsFcn ()
{
	isfcn[0x0001] = true;
	isfcn[0xFFFF] = true;
}

Str2 wchar_to_wstr (Str2 out, wchar c)
{
	if(out==NULL) return NULL;
	*out++ = '\\';
	int i, u = (c<=9 ? 1 : c<=99 ? 2 : c<=0xFFFF ? 4 : 8);
	if(u>=4) *out++ = (u==4 ? 'u' : 'U');
	out += u;
	*out = 0;
	for(i=0; i<u; i++)
	{
		wchar a;
		if(u<4){
			a = c % 10;
			c = c / 10;
		}
		else{
			a = c % 16;
			c = c / 16;
		}
		*--out = (a<10) ? ('0'+a) : ('A'+a-10);
	}
	out += u;
	assert(*out==0);
	return out;
}

int wstr_to_wchar (wchar *out, const_Str2 str, const_Str2 end)
{
	assert(str && end);
	const wchar *s = str;
	wchar c=0;
	do{
		if(s==end) { end=0; break; } // if s = ''
		c = *s++;
		if(c != '\\') break;
		if(s==end) { end=0; break; } // if s = '\'
		c = *s++;

		     if(c=='t') c = '\t';
		else if(c=='r') c = '\r';
		else if(c=='n') c = '\n';
		else if(c=='u' || c=='U')
		{
			if(s==end) { end=0; break; } // if s = '\u'
			uint32_t i=0, j = (c=='u'?4:8);
			while(true)
			{
				c = *s;
				     if('0'<=c && c<='9') c-='0';
				else if('A'<=c && c<='F') c-='A'-10;
				else if('a'<=c && c<='f') c-='a'-10;
				else break;
				i = i*16 + c;
				s++;
				if(--j==0) break;
				if(s==end) break;
			}
			if(j) end=0; // if incomplete character code
			else c=i;
		}
		else if('0'<=c && c<='9')
		{
			uint32_t i=0, j=3;
			s--;
			while(true)
			{
				c = *s;
				if('0'<=c && c<='9') c-='0';
				else break;
				i = i*10 + c;
				s++;
				if(--j==0) break;
				if(s==end) break;
			}
			c=i;
		}
	}while(0);
	if(out) *out=c;
	int r = s - str;
	return end ? r : -r;
}


value PcnToChr (value v)
{
	_init_str
	if(str==end) return v;

	wchar* out = (wchar*)v;
	while(str < end)
	{
		int i = wstr_to_wchar(out, str, end);
		if(i>0){
			out += 1;
			str += i;
		}
		else{
			i = -i;
			strcpy22S(out, str, i);
			out += i;
			str += i;
		}
	}
	*out=0;
	return setStr22(y, (wchar*)v);
}

static value ChrToXcn (value v, bool fcn)
{
	_init_str

	// 6 = sizeof("\uXXXX") TODO: maybe use 10 for UTF32 instead?
	long size = (end-str)*6;
	if(size==0) return v;

	initial_SetIsFcn();
	wchar* out = (wchar*)v;

	for( ; str < end; str++)
	{
		if(fcn || isfcn[*str])
			out = wchar_to_wstr (out, *str);
		else
		switch(*str)
		{
		case '\\': *out++ = '\\'; *out++ = '\\'; break;
		case '\t': *out++ = '\\'; *out++ = 't'; break;
		case '\r': *out++ = '\\'; *out++ = 'r'; break;
		case '\n': *out++ = '\\'; *out++ = 'n'; break;
		default: *out++ = *str; break;
		}
	}
	*out = 0;
	return setStr22(y, (wchar*)v);
}

value ChrToPcn (value v) { return ChrToXcn(v, 0); }
value ChrToFcn (value v) { return ChrToXcn(v, 1); }
value SetIsFcn (value v)
{
	_init_str
	bool found=false;
	wchar c;
	for( ; str < end; str++)
	{
		if(str[0]!='\\') continue;
		c = str[1];
		if(c!='u' && c!='U' && !isDigit(c))
			continue;
		int i = wstr_to_wchar(&c, str, end);
		if(i>0)
		{
			isfcn[c] = true;
			found = true;
			str += i-1; // -1 due to str++ of for loop
		}
	}
	if(found) v = setBool(y, true);
	else v = setError(y, L"No code number found in given text.");
	return v;
}


/******************************************************************************************/

value oper_alert   (value v)
{
	const_value y = vPrev(v);
	VstToStr(setRef(v, y), 0);
	user_alert (L"Alert", getStr2(v));
	return v;
}

value oper_confirm (value v)
{
	_init_s(1)
	bool b = user_confirm (L"Confirm", s[0]);
	return setBool(y, b);
}

value oper_prompt  (value v)
{
	_init_s(2)
	const wchar* out = user_prompt (L"User Entry", s[0], s[1]);
	return setStr22(y, out);
}

value oper_read    (value v)
{
	#ifdef LOCAL_USER
	_init_s(1)
	long len = strlen2(s[0]);
	wchar name[len+1];     // +1 for '\0'
	strcpy22 (name, s[0]); // first save the file name
	return FileOpen2 (name, y);
	#else
	value y = vPrev(v);
	return setError(y, L"Access denied.");
	#endif
}

value oper_write   (value v)
{
	#ifdef LOCAL_USER
	_init_s(2)
	n = vNext(n+2); // +2 so to skip vector header
	return vPrevCopy(y, FileSave2 (s[0], setRef(v,n)));
	#else
	value y = vPrev(v);
	return setError(y, L"Access denied.");
	#endif
}

/******************************************************************************************/


typedef struct { uint16_t n, offset; } FromGetInt;

static FromGetInt getInt (const wchar* str)
{
	int i=0, n=0;
	wchar c = str[i];
	while('0'<=c && c<='9')
	{
		n = n*10 + (c-'0');
		c = str[++i];
	}
	FromGetInt r = {n, i};
	return r;
}

ToStrInfo getToStrInfo (const wchar* str) // see documentation in _math.h
{
	const wchar* start = str;
	ToStrInfo info = {0,0};
	wchar c = *str++;
	if(c == '%')
	{
		while(true)
		{
			c = *str++;
			if(c=='_') continue;
			if(c=='s') { info.info |= TOSTR_s; break; }
			if(c=='d') { info.info |= TOSTR_d; break; }
			if(c=='x') { info.info |= TOSTR_x; break; }
			if(c=='X') { info.info |= TOSTR_X; break; }
			if(c=='o') { info.info |= TOSTR_o; break; }
			if(c=='b') { info.info |= TOSTR_b; break; }
			if(c=='E') { info.info |= TOSTR_E; break; }
			if(c=='%') { info.info |= TOSTR_PP; break; }

			if('0'<=c && c<='9')
			{
				FromGetInt n = getInt(--str);
				str += n.offset;
				if(n.n > TOSTR_MAX_WIDTH) { c=0; break; } // if error
				info.info = TOSTR_SET_WIDTH(info.info, n.n);
			}
			else if(c=='.')
			{
				c = *str;
				if('0'<=c && c<='9')
				{
					FromGetInt n = getInt(str);
					str += n.offset;
					if(n.n > TOSTR_MAX_PREC) { c=0; break; } // if error
					info.info = TOSTR_SET_PREC(info.info, n.n);
					info.info |= TOSTR_EXACT_PREC;
				}
			}
			else if(c=='|')
			{
				c = *str;
				if('0'<=c && c<='9')
				{
					FromGetInt n = getInt(str);
					str += n.offset;
					if(n.n > TOSTR_MAX_INDEX) { c=0; break; } // if error
					info.info = TOSTR_SET_INDEX(info.info, n.n);
				}
			}
			else if(c=='-') info.info |= TOSTR_ALIGN_LEFT;
			else if(c=='=') info.info |= TOSTR_ALIGN_CENT;

			else if(0==memcmp(str-1, L"Cate", 8)) { str += 3; info.info |= TOSTR_CATEGORY; }
			else if(0==memcmp(str-1, L"Type", 8)) { str += 3; info.info |= TOSTR_CATEGORY | TOSTR_VAL_TYPE; }
			else if(0==memcmp(str-1, L"Esca", 8)) { str += 3; info.info |= TOSTR_ESCAPE; }
			else if(0==memcmp(str-1, L"Line", 8)) { str += 3; info.info |= TOSTR_NEWLINE; }
			else if(0==memcmp(str-1, L"Zero", 8)) { str += 3; info.info |= TOSTR_PAD_ZERO; }
			else if(0==memcmp(str-1, L"Maxi", 8)) { str += 3; info.info |= TOSTR_CHARS_MAXI; }

			else { c=0; break; } // if error
		}
	}
	if(!c) info.info = ~0;
	info.length = str - start;
	return info;
}


static Str2 do_set_message (Str2 output, int argc, const_Str2* argv, const_Str3 str)
{
	long len = 0;
	bool getlen = output==NULL;
	if(!getlen) *output=0;
	if(argc<1 || argv==NULL) return output;

	const_Str2 format = argv[0];
	if(!format) return output;

	int i=0;
	while(true)
	{
		if(getlen && output)
		{
			len += output - (Str2)0;
			output = NULL;
		}
		if(!*format) break;

		ToStrInfo info = getToStrInfo(format);
		int oper = TOSTR_GET_OPER(info.info);
		bool skip = false;

		if((info.info==~0 || info.length<=1)
		|| (oper!=TOSTR_s && oper!=TOSTR_PP))
			skip = true;

		else if(oper == TOSTR_PP) // if a "%%"
		{
			if(output){
				output[0] = '%';
				output[1] = '\0';
			} output++;
		}
		else{
			int j = TOSTR_GET_INDEX(info.info); // get the target index
			i = j ? j : i+1; // use i+1 if target is the default j==0

			if(str.ptr==NULL){
				if(i >= argc) skip = true; // if error
				else output = strcpy22(output, argv[i]);
			}
			else if(i==1) output = strcpy23(output, str);
			else if(i==2) output = intToStr(output, sLine(str));
			else if(i==3) output = intToStr(output, sColn(str));
			else if(i==4) output = strcpy22(output, lchar_get_source(str));
			else if(i-4 >= argc) skip = true;
			else output = strcpy22(output, argv[i-4]);
		}
		if(skip) output = strcpy22S(output, format, info.length);
		format += info.length;
	}
	if(getlen) output += len;
	return output;
}

Str2 sprintf2 (Str2 output, int argc, const_Str2* argv)
{ return do_set_message (output, argc, argv, C37(NULL)); }

Str2 set_message (Str2 output, int argc, const_Str2* argv, const_Str3 str)
{
	assert(str.ptr); if(!str.ptr) return output;
	return do_set_message (output, argc, argv, str);
}


const_Str2 skipComment2 (const_Str2 str)
{
	while(true)
	{
		if(!str || *str != '#') break;
		str++;
		if(*str != '{') // if a single line comment
		{
			// skip till '\n' is found, excluding it
			while(*str && *str != '\n') str++;
			break;
		}
		// else skip till "}#" is found, including it
		wchar a, b;
		int level=1;
		str++;
		b = *str;
		while(true)
		{
			str++;
			if(!*str) break;
			a = b;
			b = *str;
			if(a=='#' && b=='{') level++;
			if(a=='}' && b=='#')
			{
				if(--level==0)
				{ str++; break; }
			}
		}
	}
	return str;
}


const_Str3 skipComment3 (const_Str3 str)
{
	while(true)
	{
		if(strEnd3(str)) break;
		if(sChar(str) != '#') break;
		str = sNext(str);
		if(strEnd3(str)) break;
		if(sChar(str) != '{')
		{
			// skip till '\n' is found, excluding it
			while(!strEnd3(str)
			   && sChar(str) != '\n')
				str = sNext(str);
			break;
		}
		// else: skip till "}#" is found, including it
		wchar a, b;
		int level=1;
		str = sNext(str);
		b = sChar(str);
		while(true)
		{
			str = sNext(str);
			if(strEnd3(str)) break;
			a = b;
			b = sChar(str);
			if(a=='#' && b=='{') level++;
			if(a=='}' && b=='#')
			{
				if(--level==0)
				{ str = sNext(str); break; }
			}
		}
	}
	return str;
}


/*const_String lchar_goto (const_String str, int offset)
{
	if(StrNULL(str)) return str;
	if(offset < 0)
	{
		for( ; offset != 0; offset++)
		{
			str = sPrev(str);
			if(StrEnd(str)) break;
		}
	}
	else if(offset > 0)
	{
		for( ; offset != 0; offset--)
		{
			if(StrEnd(str)) break;
			str = sNext(str);
		}
	}
	return str;
}*/

/******************************************************************************************/


static Str2 names[1000] = {NULL};
static uint16_t count = 1; // keep [0]==NULL

static uint16_t submitSourceName (const_Str2 name)
{
	uint16_t i;
	for(i=0; i<count; i++)
		if(0==strcmp22(names[i], name))
			break;
	if(i==count)
	{
		astrcpy22(&names[i], name);
		if(count+1u < SIZEOF(names)) count++;
	}
	return i;
}

const_Str2 lchar_get_source (const_Str3 str)
{
	uint16_t sourceID = mChar(str).source;
	assert(sourceID < count);
	if(sourceID >= count) return NULL;
	else return names[sourceID];
}

void set_line_coln_source (Str3 str, uint32_t line, uint32_t coln, const_Str2 source)
{
	if(str.ptr==NULL) return;
	uint16_t sourceID = submitSourceName(source);
	while(true)
	{
		sLine(str) = line;
		sColn(str) = coln;
		if(source) mChar(str).source = sourceID;
		if(strEnd3(str)) break;

		if(sChar(str) == '\n')
		{ coln=1; line++; }
		else coln++;
		str = sNext(str);
	}
}


void string_clean()
{
	uint16_t i;
	for(i=0; i<count; i++)
		names[i] = str2_free(names[i]);
	CXX_clean();
}


/*
#include <avl.h>
AVLT soureNames={0};

static int avl_compare_name (const void* key1, const void* key2, const void* arg)
{ return 0==strcmp22( 1+(const wchar*)key1 , 1+(const wchar*)key2 ); }

static uint16_t submitSourceName (const wchar* name)
{
	void* node = avl_do(AVL_FIND, &sourceNames, name-1, 0, 0, avl_compare_name);
	if(node) return *(uint16_t*)node;

	node = avl_new(NULL, (1+strlen2(name)+1)*sizeof(wchar));

	uint16_t sourceID = sourceNames.size + 1;
	*(uint16_t*)node = sourceID;
	strcpy22( 1+(wchar*)node, name);

	avl_do(AVL_PUT, &sourceNames, node, 0, 0, avl_compare_name);
	return sourceID;
}

const wchar* lchar_get_source (const_String str)
{
	uint16_t sourceID = mChar(str).source;
	void* node = avl_min(&sourceNames);
	for( ; node; node = avl_next(node))
	{
		if(*(uint16_t*)node == sourceID)
			return 1+(const wchar*)node;
	}
	assert(node!=NULL);
	return NULL;
}
*/
