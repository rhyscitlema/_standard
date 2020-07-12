#ifndef __STRING_H
#define __STRING_H
/*
	_string.h
*/

#include <string.h>
#include "_strfun.h"

void string_clean(); // free memory, must call on software exit


/* get next position while skipping comments */
const_Str2 strNext2 (const_Str2 str);
const_Str3 strNext3 (const_Str3 str);

/* if source=={0} then it is ignored, this can be used to preserve its value */
void set_line_coln_source (Str3 str, uint32_t first_line, uint32_t first_coln, const_Str2 source);
const_Str2 lchar_get_source (const_Str3 str);


static inline bool isPrintableASCII (wchar c) { return (0x20 <= c && c <= 0x7E); }

static inline bool isSpace (wchar c) { return (c==' ' || c=='\t' || c=='\r' || c=='\n'); }
static inline bool isDigit (wchar c) { return  ('0'<=c && c<='9'); }
static inline bool isHexad (wchar c) { return (('0'<=c && c<='9') || ('A'<=c && c<='F') || ('a'<=c && c<='f')); }
static inline bool isAlpNu (wchar c) { return (('0'<=c && c<='9') || ('A'<=c && c<='Z') || ('a'<=c && c<='z')); }

static inline bool isOpenedBracket (wchar c) { return c=='(' || c=='{' || c=='['; }
static inline bool isClosedBracket (wchar c) { return c==')' || c=='}' || c==']'; }
static inline wchar OpenedToClosedBracket (wchar c) { return c=='(' ? ')' : c=='{' ? '}' : c=='[' ? ']' : 0; }
static inline wchar ClosedToOpenedBracket (wchar c) { return c==')' ? '(' : c=='}' ? '{' : c==']' ? '[' : 0; }


int wstr_to_wchar (wchar *out, const_Str2 str, const_Str2 end);
Str2 wchar_to_wstr (Str2 out, wchar c);

value pcn_to_chr (value v); // Partly Code Number to Character
value chr_to_pcn (value v); // Character to Partly Code Number
value chr_to_fcn (value v); // Character to Fully Code Number
value set_pif_cn (value v); // Set Partly is Fully Code Number
/* return false if input string is invalid */


Str2 sprintf2 (Str2 output, int argc, const_Str2* argv);

Str2 set_message (Str2 output, int argc, const_Str2* argv, const_Str3 str);

/*
	The below is defined only in librfet.
	It is a once-off evaluation.
	Typical usage is:

	stack = rfet_parse_and_evaluate (stack, rfet_string, NULL, NULL);
	stack = VstToStr(stack, PUT_NEWLINE|0, -1, -1); // see _math.h
	puts2(getStr2(vGetPrev(stack))); // print output string to stdout
*/
value rfet_parse_and_evaluate (
	value stack,
	const_Str2 rfet_string,     /* string to be parsed and evaluated */
	const_Str2 source_name,     /* source of rfet_string, if NULL then "input" is used */
	const_value argument );

#endif
