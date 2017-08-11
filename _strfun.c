/*
    _strfun.c
*/
#include "_string.h"
#include "_malloc.h"
#include "_math.h"


static inline lchar* lchar_new ()
{
    lchar* lchar_ptr = (lchar*)_malloc(sizeof(lchar));
    memset(lchar_ptr, 0, sizeof(lchar));
    memory_alloc("lchar");
    return lchar_ptr;
}

/***********************************************************************************************************/

void  char_free ( char*  char_ptr) { if( char_ptr) memory_freed(" char"); _free( char_ptr); }
void wchar_free (wchar* wchar_ptr) { if(wchar_ptr) memory_freed("wchar"); _free(wchar_ptr); }
void lchar_free (lchar* lchar_ptr)
{
    while(lchar_ptr)
    {
        lchar* lchr = lchar_ptr;
        lchar_ptr = lchar_ptr->next;
        memset(lchr, 0, sizeof(lchar));
        _free(lchr);
        memory_freed("lchar");
    }
}


 char*  char_alloc ( char*  char_ptr, long size) { if(size==0) {  char_free( char_ptr); return NULL; } if(! char_ptr) memory_alloc(" char"); return ( char*) _realloc ( char_ptr, (size+1)*sizeof( char)); }
wchar* wchar_alloc (wchar* wchar_ptr, long size) { if(size==0) { wchar_free(wchar_ptr); return NULL; } if(!wchar_ptr) memory_alloc("wchar"); return (wchar*) _realloc (wchar_ptr, (size+1)*sizeof(wchar)); }
lchar* lchar_alloc (lchar* lchar_ptr, long size)
{
    lchar *lchr1, *lchr2;

    lchar_free (lchar_ptr);
    if(size<=0) return NULL;

    lchr1 = lchar_new();
    while(size--)
    {
        lchr2 = lchar_new();
        lchr2->next = lchr1;
        lchr1->prev = lchr2;
        lchr1 = lchr2;
    }
    return lchr1;
    // The end-of-string character is
    // also added here implicitly specified
}


/***********************************************************************************************************/

static  char* str12=NULL;
static  char* str13=NULL;
static wchar* str21=NULL;
static wchar* str23=NULL;
static lchar* str31=NULL;
static lchar* str32=NULL;

static long size12=0,
            size13=0,
            size21=0,
            size23=0,
            size31=0,
            size32=0;

void CST_clean()
{
     char_free(str12); str12=NULL; size12=0;
     char_free(str13); str13=NULL; size13=0;
    wchar_free(str21); str21=NULL; size21=0;
    wchar_free(str23); str23=NULL; size23=0;
    lchar_free(str31); str31=NULL; size31=0;
    lchar_free(str32); str32=NULL; size32=0;
}

#define CST(str, size, _char_alloc, _strlen, _strcpy) \
{ \
    long len = 1+_strlen(input); \
    if(len > size) \
    {   size = len; \
        str = _char_alloc(str,size); \
    } \
    _strcpy(str,input); \
    return str; \
}
const  char* CST12 (const wchar* input) CST (str12, size12,  char_alloc, strlen12, strcpy12)
const  char* CST13 (const lchar* input) CST (str13, size13,  char_alloc, strlen3 , strcpy13)
const wchar* CST21 (const  char* input) CST (str21, size21, wchar_alloc, strlen21, strcpy21)
const wchar* CST23 (const lchar* input) CST (str23, size23, wchar_alloc, strlen3 , strcpy23)
const lchar* CST31 (const  char* input) CST (str31, size31, lchar_alloc, strlen1 , strcpy31)
const lchar* CST32 (const wchar* input) CST (str32, size32, lchar_alloc, strlen2 , strcpy32)


/* Temporal Integer to String */
const wchar* TIS2 (unsigned char count, long integer)
{
    static wchar str[20][64];
    if(count >= 20) return NULL;
    intToStr (str[count], integer);
    return str[count];
}


/***********************************************************************************************************/

#define STRCPY(outType, outGet, outNext, inGet, inNext) \
{ \
    if(out == NULL) return NULL; \
    if(in != NULL) \
        for( ; inGet != 0; inNext, outNext) \
            outGet = (outType) inGet; \
    outGet = 0; \
    return out; \
}
 char* strcpy11 ( char* out, const  char* in) STRCPY ( char, *out, out++, *in, in++)
wchar* strcpy22 (wchar* out, const wchar* in) STRCPY (wchar, *out, out++, *in, in++)
 char* strcpy13 ( char* out, const lchar* in) STRCPY ( char, *out, out++, in->wchr, in = in->next)
wchar* strcpy23 (wchar* out, const lchar* in) STRCPY (wchar, *out, out++, in->wchr, in = in->next)
lchar* strcpy31 (lchar* out, const  char* in) STRCPY (wchar, out->wchr, out = out->next, *in, in++)
lchar* strcpy32 (lchar* out, const wchar* in) STRCPY (wchar, out->wchr, out = out->next, *in, in++)
lchar* strcpy33 (lchar* out, const lchar* in)
{
    if(out == NULL) return NULL;
    if(in == NULL) out->wchr=0;
    else while(1)
    {
        out->wchr = in->wchr;
        lchar_copy(out, in);
        if(in->wchr == 0) break;
        out = out->next;
        in = in->next;
    }
    return out;
}

 char* strcpy12 ( char* out, const wchar* in) { return strcpy12S (out, in, strlen2(in)); }
wchar* strcpy21 (wchar* out, const  char* in) { return strcpy21S (out, in, strlen1(in)); }

 char* strcpy12S ( char* output, const wchar* input, long size)
{
    if(!output) return NULL;
    //assert(input!=NULL || size==0);
    const wchar* end = input+size;
    wchar c;
    while(input<end)
    {
        c = *input++;
        if(0x0000<= c && c <=0x007F) { *output++ = (char)c; }

        if(0x0080<= c && c <=0x07FF) { *output++ = 0xC0|((c>> 6)&0x1F);
                                       *output++ = 0x80|((c    )&0x3F); }

        if(0x0800<= c && c <=0xFFFF) { *output++ = 0xE0|((c>>12)&0x0F);
                                       *output++ = 0x80|((c>> 6)&0x3F);
                                       *output++ = 0x80|((c    )&0x3F); }
    }
    *output = 0;
    return output;
}

wchar* strcpy21S (wchar* output, const  char* input, long size)
{
    if(!output) return NULL;
    //assert(input!=NULL || size==0);
    const char* end = input+size;
    unsigned char i, j;
    wchar c;
    while(input<end)
    {
        c = (unsigned char)*input;
        i=1; if(c&0x80) { for(j=0x40; (c & j); j>>=1, i++); }
        if(i==2) c = ((input[0]&0x1F)<< 6) |  (input[1]&0x3F) ;
        if(i==3) c = ((input[0]&0x0F)<<12) | ((input[1]&0x3F)<<6) | (input[2]&0x3F) ;
        input += i;
        *output++ = c;
    }
    *output = 0;
    return output;
}


#define STRCPYS(outGet, outNext, inGet, inNext) \
{ \
    if(out == NULL) return NULL; \
    if(in != NULL) \
        while(size--) \
        { \
            outGet = inGet; \
            outNext; \
            inNext; \
        } \
    outGet = 0; \
    return out; \
}
wchar* strcpy22S (wchar* out, const wchar* in, long size) STRCPYS (*out, out++, *in, in++)
wchar* strcpy23S (wchar* out, const lchar* in, long size) STRCPYS (*out, out++, in->wchr, in = in->next)
lchar* strcpy32S (lchar* out, const wchar* in, long size) STRCPYS (out->wchr, out = out->next, *in, in++)
lchar* strcpy33S (lchar* out, const lchar* in, long size)
{
    if(out == NULL) return NULL;
    if(in == NULL) out->wchr=0;
    else
    {   while(size--)
        {   out->wchr = in->wchr;
            lchar_copy(out, in);
            out = out->next;
            in = in->next;
        }
        if(in==NULL) in = out->prev;
        lchar_copy(out, in);
        out->wchr=0;
    }
    return out;
}

/***********************************************************************************************************/


#define STRCMP(str1Get, str1Next, str2Get, str2Next) \
{ \
    if(str1==NULL) \
    {   if(str2==NULL) return 0; \
        else return -1; \
    } else if (str2==NULL) return 1; \
 \
    while (str1Get == str2Get \
        && str1Get != 0) \
        { str1Next; str2Next; } \
 \
    if(str1Get < str2Get) return -1; \
    if(str1Get > str2Get) return  1; \
    return 0; \
}
int strcmp11 (const  char* str1, const  char* str2) STRCMP (*str1, str1++, *str2, str2++)
int strcmp12 (const  char* str1, const wchar* str2) STRCMP (*str1, str1++, *str2, str2++)
int strcmp21 (const wchar* str1, const  char* str2) STRCMP (*str1, str1++, *str2, str2++)
int strcmp22 (const wchar* str1, const wchar* str2) STRCMP (*str1, str1++, *str2, str2++)
int strcmp13 (const  char* str1, const lchar* str2) STRCMP (*str1, str1++, str2->wchr, str2 = str2->next)
int strcmp23 (const wchar* str1, const lchar* str2) STRCMP (*str1, str1++, str2->wchr, str2 = str2->next)
int strcmp31 (const lchar* str1, const  char* str2) STRCMP (str1->wchr, str1 = str1->next, *str2, str2++)
int strcmp32 (const lchar* str1, const wchar* str2) STRCMP (str1->wchr, str1 = str1->next, *str2, str2++)
int strcmp33 (const lchar* str1, const lchar* str2) STRCMP (str1->wchr, str1 = str1->next, str2->wchr, str2 = str2->next)


#define STRCMPS(strcmp, str1Get, str1Next, str2Get, str2Next) \
{ \
    if(str1==NULL) return -1; \
    if(str2==NULL) return 1; \
    if(str2size<0) return strcmp (str1, str2); \
    while(1) \
    { \
        if(str2size-- == 0) return (str1Get==0) ? 0 : 1; \
        if(str1Get == 0) return -1; \
        if(str1Get != str2Get) return (str1Get < str2Get) ? -1 : 1; \
        str1Next; \
        str2Next; \
    } \
    return 0; \
}
int strcmp22S (const wchar* str1, const wchar* str2, long str2size) STRCMPS (strcmp22, *str1, str1++, *str2, str2++)
int strcmp23S (const wchar* str1, const lchar* str2, long str2size) STRCMPS (strcmp23, *str1, str1++, str2->wchr, str2 = str2->next)
int strcmp33S (const lchar* str1, const lchar* str2, long str2size) STRCMPS (strcmp33, str1->wchr, str1 = str1->next, str2->wchr, str2 = str2->next)


/***********************************************************************************************************/

#define STRLEN(stringGet, stringNext) \
{ \
    long len=0; \
    if(string != NULL) \
        while(stringGet != 0) \
        { stringNext; len++; } \
    return len; \
}
long strlen1 (const  char* string) STRLEN (*string, string++)
long strlen2 (const wchar* string) STRLEN (*string, string++)
long strlen3 (const lchar* string) STRLEN (string->wchr, string = string->next)

long strlen12 (const wchar* str)
{
    long len=0;
    wchar c;
    if(str)
    {   while(true)
        {
            c = *str++;
            if(!c) break;
            if(0x0000<= c && c <=0x007F) len+=1;
            if(0x0080<= c && c <=0x07FF) len+=2;
            if(0x0800<= c && c <=0xFFFF) len+=3;
            //if(0x10000<= c <=0x10FFFF) len+=4;
        }
    }
    return len;
}

long strlen21 (const  char* str)
{
    long len=0;
    unsigned char i, j;
    wchar c;
    if(str)
    {   while(true)
        {
            c = (unsigned char)*str;
            if(!c) break;
            i=1; if(c&0x80) { for(j=0x40; (c & j); j>>=1, i++); }
            str += i;
            len++;
        }
    }
    return len;
}


#define IS_EMPTY(strGet, strNext) \
{ \
    if(str==NULL) return true; \
    while(isSpace(strGet)) strNext; \
    return (strGet==stop || strGet=='\0'); \
}
bool isEmpty1 (const  char* str, wchar stop) IS_EMPTY (*str, str++)
bool isEmpty2 (const wchar* str, wchar stop) IS_EMPTY (*str, str++)
bool isEmpty3 (const lchar* str, wchar stop) IS_EMPTY (str->wchr, str = str->next)



/* trim (remove) leading and trailing spaces */
void strtrim1 (char* str)
{
    long i=0, j;
    for(j=0; str[j]==' '; j++);     // trim leading spaces

    for(; str[j]!=0; j++)           // copy middle characters
        str[i++] = str[j];
    for(j=i-1; str[j]==' '; j--)    // trim trailing spaces
        i--;
    str[i]=0;                       // end string
}

/* trim (remove) leading and trailing spaces */
void strtrim2 (wchar* str)
{
    long i=0, j;
    for(j=0; str[j]==' '; j++);     // trim leading spaces

    for(; str[j]!=0; j++)           // copy middle characters
        str[i++] = str[j];
    for(j=i-1; str[j]==' '; j--)    // trim trailing spaces
        i--;

    str[i]=0;                       // end string
}



/* reverse string of size 'len' */
void strrev1 (char* str, long len)
{
    char temp;                      // used for swapping
    long i;
    for(i=0; i<len/2; i++)          // from start to middle of string
    {
        temp = str[i];              // swap values at position 'i' and 'len-1-i'
        str[i] = str[len-1-i];
        str[len-1-i] = temp;
    }
}

/* reverse string of length 'len' */
void strrev2 (wchar* str, long len)
{
    wchar temp;                     // used for swapping
    long i;
    for(i=0; i<len/2; i++)          // from start to middle of string
    {
        temp = str[i];              // swap values at position 'i' and 'len-1-i'
        str[i] = str[len-1-i];
        str[len-1-i] = temp;
    }
}
