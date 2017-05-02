#ifndef __STRFUN_H
#define __STRFUN_H
/*
    strfun.h
*/

#include "_stddef.h"


 char*  char_alloc ( char*  char_ptr, long size);
wchar* mchar_alloc (wchar* mchar_ptr, long size);
lchar* lchar_alloc (lchar* lchar_ptr, long size);

void  char_free ( char*  char_ptr);
void mchar_free (wchar* mchar_ptr);
void lchar_free (lchar* lchar_ptr);

// note: memory for NULL terminator is not implicitly assumed
static inline void set_lchar_array (lchar* lchar_ptr, long size)
{   long i;
    for(i=0; i<size; i++)
    {
        lchar_ptr[i].next = (i+1<size) ? &lchar_ptr[i+1] : NULL;
        lchar_ptr[i].prev = (i-1>= 0 ) ? &lchar_ptr[i-1] : NULL;
    }
}


/* Convert String Temporarily */
const  char* CST12 (const wchar* input);
const  char* CST13 (const lchar* input);
const wchar* CST21 (const  char* input);
const wchar* CST23 (const lchar* input);
const lchar* CST31 (const  char* input);
const lchar* CST32 (const wchar* input);
void CST_clean(); // to free memory, best called upon software exit

/* Temporarily convert Integer to String */
const wchar* TIS2 (unsigned char count, long integer);


long strlen1 (const  char* string);
long strlen2 (const wchar* string);
long strlen3 (const lchar* string);
long strlen12(const wchar* string);
long strlen21(const  char* string);

static inline long strlen3S (const lchar* str1, const lchar* str2)
{
    long size=0;
    if(str1) { for( ; str1 != str2 && str1->wchr!=0; str1 = str1->next) size++; }
    return size;
}

/* check for isSpace(str[i]) until character 'stop' or '\0' is found */
bool isEmpty1 (const  char* str, wchar stop);
bool isEmpty2 (const wchar* str, wchar stop);
bool isEmpty3 (const lchar* str, wchar stop);

void strtrim1 ( char* string);
void strtrim2 (wchar* string);

void strrev1 ( char* string, long length);
void strrev2 (wchar* string, long length);


 char* strcpy11 ( char* output, const  char* input);
 char* strcpy12 ( char* output, const wchar* input);
 char* strcpy13 ( char* output, const lchar* input);
wchar* strcpy21 (wchar* output, const  char* input);
wchar* strcpy22 (wchar* output, const wchar* input);
wchar* strcpy23 (wchar* output, const lchar* input);
lchar* strcpy31 (lchar* output, const  char* input);
lchar* strcpy32 (lchar* output, const wchar* input);
lchar* strcpy33 (lchar* output, const lchar* input);

 char* strcpy12S ( char* output, const wchar* input, long size);
wchar* strcpy21S (wchar* output, const  char* input, long size);
wchar* strcpy22S (wchar* output, const wchar* input, long size);
wchar* strcpy23S (wchar* output, const lchar* input, long size);
lchar* strcpy32S (lchar* output, const wchar* input, long size);
lchar* strcpy33S (lchar* output, const lchar* input, long size);


static inline void astrcpy11 ( char** outp, const  char* in) { if(outp && *outp!=in) { *outp =  char_alloc(*outp, strlen1 (in)); strcpy11(*outp, in); }}
static inline void astrcpy12 ( char** outp, const wchar* in) { if(outp             ) { *outp =  char_alloc(*outp, strlen12(in)); strcpy12(*outp, in); }}
static inline void astrcpy13 ( char** outp, const lchar* in) { if(outp             ) { *outp =  char_alloc(*outp, strlen3 (in)); strcpy13(*outp, in); }}
static inline void astrcpy21 (wchar** outp, const  char* in) { if(outp             ) { *outp = mchar_alloc(*outp, strlen21(in)); strcpy21(*outp, in); }}
static inline void astrcpy22 (wchar** outp, const wchar* in) { if(outp && *outp!=in) { *outp = mchar_alloc(*outp, strlen2 (in)); strcpy22(*outp, in); }}
static inline void astrcpy23 (wchar** outp, const lchar* in) { if(outp             ) { *outp = mchar_alloc(*outp, strlen3 (in)); strcpy23(*outp, in); }}
static inline void astrcpy31 (lchar** outp, const  char* in) { if(outp             ) { *outp = lchar_alloc(*outp, strlen1 (in)); strcpy31(*outp, in); }}
static inline void astrcpy32 (lchar** outp, const wchar* in) { if(outp             ) { *outp = lchar_alloc(*outp, strlen2 (in)); strcpy32(*outp, in); }}
static inline void astrcpy33 (lchar** outp, const lchar* in) { if(outp && *outp!=in) { *outp = lchar_alloc(*outp, strlen3 (in)); strcpy33(*outp, in); }}

static inline void astrcpy22S (wchar** outp, const wchar* in, long size) { if(outp && *outp!=in) { *outp = mchar_alloc(*outp, size); strcpy22S(*outp, in, size); }}
static inline void astrcpy23S (wchar** outp, const lchar* in, long size) { if(outp             ) { *outp = mchar_alloc(*outp, size); strcpy23S(*outp, in, size); }}
static inline void astrcpy32S (lchar** outp, const wchar* in, long size) { if(outp             ) { *outp = lchar_alloc(*outp, size); strcpy32S(*outp, in, size); }}
static inline void astrcpy33S (lchar** outp, const lchar* in, long size) { if(outp && *outp!=in) { *outp = lchar_alloc(*outp, size); strcpy33S(*outp, in, size); }}

static inline void strcat21 (wchar* output, const  char* input) { strcpy21( output+strlen2(output), input); }
static inline void strcat22 (wchar* output, const wchar* input) { strcpy22( output+strlen2(output), input); }


int strcmp21 (const wchar* str1, const  char* str2);
int strcmp22 (const wchar* str1, const wchar* str2);
int strcmp23 (const wchar* str1, const lchar* str2);
int strcmp31 (const lchar* str1, const  char* str2);
int strcmp32 (const lchar* str1, const wchar* str2);
int strcmp33 (const lchar* str1, const lchar* str2);

int strcmp22S (const wchar* str1, const wchar* str2, long str2size);
int strcmp23S (const wchar* str1, const lchar* str2, long str2size);
int strcmp33S (const lchar* str1, const lchar* str2, long str2size);


#endif
