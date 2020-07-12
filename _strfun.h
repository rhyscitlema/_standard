#ifndef __STRFUN_H
#define __STRFUN_H
/*
	strfun.h
*/

#include "_stddef.h"


#define mChar(str) ((str).ptr->chr)
#define sChar(str) (mChar(str).c)
#define sLine(str) (mChar(str).line)
#define sColn(str) (mChar(str).coln)
static inline Str3 sNext (Str3 s) { s.ptr = s.ptr->next; return s; }
static inline Str3 sPrev (Str3 s) { s.ptr = s.ptr->prev; return s; }

static inline bool strEnd1 (const_Str1 s) { return !s || !*s ; }
static inline bool strEnd2 (const_Str2 s) { return !s || !*s ; }
static inline bool strEnd3 (const_Str3 s) { return !s.ptr || (s.end ? s.ptr==s.end : sChar(s)=='\0'); }


/* n = length of string, excluding '\0' */
Str1 str1_alloc (Str1 s, long n);
Str2 str2_alloc (Str2 s, long n);
Str3 str3_alloc (Str3 s, long n);
#define str1_free(ptr) str1_alloc(ptr,0)
#define str2_free(ptr) str2_alloc(ptr,0)
#define str3_free(ptr) str3_alloc(ptr,0)
#define wchar_alloc str2_alloc
#define wchar_free str2_free


/* Convert string between different string types.
*  Auto-conversion would have been quite helpful!
*/
const_Str1 C12 (const_Str2 in);
const_Str1 C13 (const_Str3 in); // for ASCII char only
const_Str2 C21 (const_Str1 in);
const_Str2 C23 (const_Str3 in);
const_Str3 C31 (const_Str1 in); // for ASCII char only
void CXX_clean(); // preferably called upon software exit

static inline Str3 C37 (lchar* lstr) { Str3 s = {lstr,NULL}; return s; }

/* Temporal Integer to String - See implementation in _strfun.c */
const_Str2 TIS2 (int count, long n);


/* return output pointing at end of string.
*  if out==NULL return out + length of input
*/
Str1 strcpy11 (Str1 out, const_Str1 in);
Str1 strcpy12 (Str1 out, const_Str2 in);
Str2 strcpy21 (Str2 out, const_Str1 in);
Str2 strcpy22 (Str2 out, const_Str2 in);
Str2 strcpy23 (Str2 out, const_Str3 in);
Str3 strcpy32 (Str3 out, const_Str2 in);
Str3 strcpy33 (Str3 out, const_Str3 in);
Str2 strcpy22S(Str2 out, const_Str2 in, long n);

static inline long strlen1 (const_Str1 in) { Str1 s=0; return strcpy11(s,in) - s; }
static inline long strlen2 (const_Str2 in) { Str2 s=0; return strcpy22(s,in) - s; }
static inline long strlen12(const_Str2 in) { Str1 s=0; return strcpy12(s,in) - s; }
static inline long strlen21(const_Str1 in) { Str2 s=0; return strcpy21(s,in) - s; }
static inline long strlen3 (const_Str3 in) { Str3 s={0,0}; return strcpy33(s,in).ptr - s.ptr; }


static inline Str2 strcat21 (Str2 out, const_Str1 in) { out += strlen2(out); return strcpy21(out,in); }
static inline Str2 strcat22 (Str2 out, const_Str2 in) { out += strlen2(out); return strcpy22(out,in); }

static inline void astrcpy21 (Str2* outp, const_Str1 in) { if(outp){ *outp = str2_alloc(*outp, strlen21(in)); strcpy21(*outp, in); }}
static inline void astrcpy22 (Str2* outp, const_Str2 in) { if(outp){ *outp = str2_alloc(*outp, strlen2 (in)); strcpy22(*outp, in); }}

/* str must point to memory of size = (n = strlen2(in)+1) * sizeof(lchar) */
Str3 set_lchar_array (lchar* str, long n, const_Str2 in, const_Str2 source);
Str3 astrcpy32 (Str3 out, const_Str2 in, const_Str2 source);


int strcmp11 (const_Str1 a, const_Str1 b);
int strcmp21 (const_Str2 a, const_Str1 b); // for ASCII char only
int strcmp22 (const_Str2 a, const_Str2 b);
int strcmp31 (const_Str3 a, const_Str1 b); // for ASCII char only
int strcmp32 (const_Str3 a, const_Str2 b);
int strcmp33 (const_Str3 a, const_Str3 b);

/* check for isSpace(s[i]) until <stop> or '\0' is found */
bool strEmpty1 (const_Str1 s, wchar stop);
bool strEmpty2 (const_Str2 s, wchar stop);
bool strEmpty3 (const_Str3 s, wchar stop);

void strtrim1(Str1 s);
void strtrim2(Str2 s);
void strrev2 (Str2 s);


#endif
