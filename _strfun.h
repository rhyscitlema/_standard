#ifndef __STRFUN_H
#define __STRFUN_H
/*
    strfun.h
*/

#include "_stddef.h"


 char*  char_alloc ( char*  char_ptr, long size);
mchar* mchar_alloc (mchar* mchar_ptr, long size);
lchar* lchar_alloc (lchar* lchar_ptr, long size);

void  char_free ( char*  char_ptr);
void mchar_free (mchar* mchar_ptr);
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
const  char* CST12 (const mchar* input);
const  char* CST13 (const lchar* input);
const mchar* CST21 (const  char* input);
const mchar* CST23 (const lchar* input);
const lchar* CST31 (const  char* input);
const lchar* CST32 (const mchar* input);
void CST_clean(); // to free memory, best called upon software exit

/* Temporarily convert Integer to String */
const mchar* TIS2 (unsigned char count, long integer);


long strlen1 (const  char* string);
long strlen2 (const mchar* string);
long strlen3 (const lchar* string);
long strlen12(const mchar* string);
long strlen21(const  char* string);

static inline long strlen3S (const lchar* str1, const lchar* str2)
{
    long size=0;
    if(str1) { for( ; str1 != str2 && str1->mchr!=0; str1 = str1->next) size++; }
    return size;
}

/* check for isSpace(str[i]) until character 'stop' or '\0' is found */
bool isEmpty1 (const  char* str, mchar stop);
bool isEmpty2 (const mchar* str, mchar stop);
bool isEmpty3 (const lchar* str, mchar stop);

void strtrim1 ( char* string);
void strtrim2 (mchar* string);

void strrev1 ( char* string, long length);
void strrev2 (mchar* string, long length);


 char* strcpy11 ( char* output, const  char* input);
 char* strcpy12 ( char* output, const mchar* input);
 char* strcpy13 ( char* output, const lchar* input);
mchar* strcpy21 (mchar* output, const  char* input);
mchar* strcpy22 (mchar* output, const mchar* input);
mchar* strcpy23 (mchar* output, const lchar* input);
lchar* strcpy31 (lchar* output, const  char* input);
lchar* strcpy32 (lchar* output, const mchar* input);
lchar* strcpy33 (lchar* output, const lchar* input);

 char* strcpy12S ( char* output, const mchar* input, long size);
mchar* strcpy21S (mchar* output, const  char* input, long size);
mchar* strcpy22S (mchar* output, const mchar* input, long size);
mchar* strcpy23S (mchar* output, const lchar* input, long size);
lchar* strcpy32S (lchar* output, const mchar* input, long size);
lchar* strcpy33S (lchar* output, const lchar* input, long size);


static inline void astrcpy11 ( char** outp, const  char* in) { if(outp && *outp!=in) { *outp =  char_alloc(*outp, strlen1 (in)); strcpy11(*outp, in); }}
static inline void astrcpy12 ( char** outp, const mchar* in) { if(outp             ) { *outp =  char_alloc(*outp, strlen12(in)); strcpy12(*outp, in); }}
static inline void astrcpy13 ( char** outp, const lchar* in) { if(outp             ) { *outp =  char_alloc(*outp, strlen3 (in)); strcpy13(*outp, in); }}
static inline void astrcpy21 (mchar** outp, const  char* in) { if(outp             ) { *outp = mchar_alloc(*outp, strlen21(in)); strcpy21(*outp, in); }}
static inline void astrcpy22 (mchar** outp, const mchar* in) { if(outp && *outp!=in) { *outp = mchar_alloc(*outp, strlen2 (in)); strcpy22(*outp, in); }}
static inline void astrcpy23 (mchar** outp, const lchar* in) { if(outp             ) { *outp = mchar_alloc(*outp, strlen3 (in)); strcpy23(*outp, in); }}
static inline void astrcpy31 (lchar** outp, const  char* in) { if(outp             ) { *outp = lchar_alloc(*outp, strlen1 (in)); strcpy31(*outp, in); }}
static inline void astrcpy32 (lchar** outp, const mchar* in) { if(outp             ) { *outp = lchar_alloc(*outp, strlen2 (in)); strcpy32(*outp, in); }}
static inline void astrcpy33 (lchar** outp, const lchar* in) { if(outp && *outp!=in) { *outp = lchar_alloc(*outp, strlen3 (in)); strcpy33(*outp, in); }}

static inline void astrcpy22S (mchar** outp, const mchar* in, long size) { if(outp && *outp!=in) { *outp = mchar_alloc(*outp, size); strcpy22S(*outp, in, size); }}
static inline void astrcpy23S (mchar** outp, const lchar* in, long size) { if(outp             ) { *outp = mchar_alloc(*outp, size); strcpy23S(*outp, in, size); }}
static inline void astrcpy32S (lchar** outp, const mchar* in, long size) { if(outp             ) { *outp = lchar_alloc(*outp, size); strcpy32S(*outp, in, size); }}
static inline void astrcpy33S (lchar** outp, const lchar* in, long size) { if(outp && *outp!=in) { *outp = lchar_alloc(*outp, size); strcpy33S(*outp, in, size); }}

static inline void strcat21 (mchar* output, const  char* input) { strcpy21( output+strlen2(output), input); }
static inline void strcat22 (mchar* output, const mchar* input) { strcpy22( output+strlen2(output), input); }


int strcmp21 (const mchar* str1, const  char* str2);
int strcmp22 (const mchar* str1, const mchar* str2);
int strcmp23 (const mchar* str1, const lchar* str2);
int strcmp31 (const lchar* str1, const  char* str2);
int strcmp32 (const lchar* str1, const mchar* str2);
int strcmp33 (const lchar* str1, const lchar* str2);

int strcmp22S (const mchar* str1, const mchar* str2, long str2size);
int strcmp23S (const mchar* str1, const lchar* str2, long str2size);
int strcmp33S (const lchar* str1, const lchar* str2, long str2size);


#endif
