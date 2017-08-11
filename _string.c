/*
    _string.c
*/

#include "_math.h"
#include "_texts.h"
#include "_stdio.h"
#include "_stdarg.h"
#include "_string.h"



/******************************************************************************************/

bool isOpenedBracket (wchar c)
{
    if(c == *TWSF(Opened_Bracket_1)) return true;
    if(c == *TWSF(Opened_Bracket_2)) return true;
    if(c == *TWSF(Opened_Bracket_3)) return true;
    return false;
}

bool isClosedBracket (wchar c)
{
    if(c == *TWSF(Closed_Bracket_1)) return true;
    if(c == *TWSF(Closed_Bracket_2)) return true;
    if(c == *TWSF(Closed_Bracket_3)) return true;
    return false;
}

wchar OpenedToClosedBracket (wchar c)
{
    if(c == *TWSF(Opened_Bracket_1)) return *TWSF(Closed_Bracket_1);
    if(c == *TWSF(Opened_Bracket_2)) return *TWSF(Closed_Bracket_2);
    if(c == *TWSF(Opened_Bracket_3)) return *TWSF(Closed_Bracket_3);
    return 0;
}

wchar ClosedToOpenedBracket (wchar c)
{
    if(c == *TWSF(Closed_Bracket_1)) return *TWSF(Opened_Bracket_1);
    if(c == *TWSF(Closed_Bracket_2)) return *TWSF(Opened_Bracket_2);
    if(c == *TWSF(Closed_Bracket_3)) return *TWSF(Opened_Bracket_3);
    return 0;
}



/******************************************************************************************/
// chr => character
// pcn => partly code number
// pifcn => partly is fully code number
// 6 = sizeof("\uXXXX")

static wchar* buffer = NULL;

static bool pifcn[0x10000] = {false};

static void initial_set_pif_cn ()
{
    pifcn[0x0001] = true;
    pifcn[0xFFFF] = true;
}

static bool mstr_to_wchar (wchar *c, const wchar* str)
{
    wchar i, t, n=0;
    if(c==NULL) return false;
    for(i=0; i<4; i++)
    {
        t = CHRtoHEX(str[i]);
        if(t<0 || t>0xF) break;
        n = (n << 4) + t;
    }
    if(i<4) return false;
    else { *c = n; return true; }
}

static wchar* wchar_to_mstr (wchar* str, wchar c)
{
    if(str==NULL) return NULL;
    *str++ = '\\';
    *str++ = 'u';
    *str++ = HEXtoCHR( (c>>(4*3)) & 0xF );
    *str++ = HEXtoCHR( (c>>(4*2)) & 0xF );
    *str++ = HEXtoCHR( (c>>(4*1)) & 0xF );
    *str++ = HEXtoCHR( (c>>(4*0)) & 0xF );
    *str = 0;
    return str;
}

const wchar* pcn_to_chr_22 (wchar** output_ptr, const wchar* input)
{
    long size = strlen2(input);
    if(size==0) return NULL;

    buffer = wchar_alloc (buffer, size);
    wchar* output = buffer;

    for( ; *input!=0; input++)
    {
        if(*input=='\\')
        {
            input++;
            switch(*input)
            {
            case '\\': *output++ = '\\'; break;
            case 't' : *output++ = '\t'; break;
            case 'r' : *output++ = '\r'; break;
            case 'n' : *output++ = '\n'; break;
            case 'u' : // if \uXXXX found
                if(mstr_to_wchar(output, input+1) && (*output!=0))
                {   output++;
                    input += 4; // skip the \uXXXX
                    break;
                }
            default: { *output++= '\\'; *output++ = *input; }
            }
        }
        else *output++ = *input;
    }
    *output=0;
    astrcpy22(output_ptr, buffer);
    return buffer;
}

const wchar* chr_to_pcn_22 (wchar** output_ptr, const wchar* input)
{
    long size = strlen2(input);
    if(size==0) return NULL;

    buffer = wchar_alloc (buffer, size*6);
    wchar* output = buffer;

    initial_set_pif_cn();
    for( ; *input!=0; input++)
    {
        if(pifcn[*input])
        { output = wchar_to_mstr (output, *input); continue; }

        switch(*input)
        {
        case '\\': *output++ = '\\'; *output++ = '\\'; break;
        case '\t': *output++ = '\\'; *output++ = 't'; break;
        case '\r': *output++ = '\\'; *output++ = 'r'; break;
        case '\n': *output++ = '\\'; *output++ = 'n'; break;
        default: *output++ = *input; break;
        }
    }
    *output = 0;
    astrcpy22(output_ptr, buffer);
    return buffer;
}

const wchar* chr_to_fcn_22 (wchar** output_ptr, const wchar* input)
{
    long size = strlen2(input);
    if(size==0) return NULL;

    buffer = wchar_alloc (buffer, size*6);
    wchar* output = buffer;

    while(*input!=0) output = wchar_to_mstr (output, *input++);
    astrcpy22(output_ptr, buffer);
    return buffer;
}

bool set_pif_cn (const wchar* str)
{
    bool found=false;
    wchar c;
    int i;
    for(i=0; str[i] != 0; i++)
    {
        if((str[i+0] == '\\')
        && (str[i+1] == 'u')
        && mstr_to_wchar(&c, &str[i+2]))
        { pifcn[c] = true; found=true; i+=6-1; }
    }
    if(!found) strcpy21(errorMessage(), "No code number found in given text.");
    return found;
}



/******************************************************************************************/

static const wchar placeHolder[] = {'%','s', 0};

wchar* sprintf2 (wchar* output, const wchar* format, ...)
{
    wchar *str;
    int pHSize;

    va_list varg;           // start of variable argument list
    va_start(varg, format); // ...not sure how these things works...!

    pHSize = strlen2(placeHolder);

    while(*format != 0)
    {
        if(0!=strcmp22S (placeHolder, format, pHSize))
            *output++ = *format++;
        else
        {
            str = va_arg (varg, wchar*); // get next argument as an wchar* (string)
            if(str) while(*str) *output++ = *str++;
            format += pHSize;
        }
    }
    va_end(varg);   // end variable argument list
    *output = 0;
    return output;
}



void set_message (wchar* output, const wchar* format, const lchar* lstr, ...)
{
    int id, pHSize, args=0;
    wchar temp[50];
    const wchar* argv[10];

    va_list varg;
    if(lstr==NULL) return;
    va_start(varg, lstr);   // start variable argument list

    while(*format != 0)
    {
        if(*format != '\\')
            *output++ = *format++;
        else
        {
            pHSize=1;   // get ready to collect the place holder id
            while(isDigit(format[pHSize])) pHSize++;

            if(pHSize==1 || pHSize>3) // if no place holder then copy '\' and continue
            { *output++ = *format++; continue; }

            strcpy22S (temp, format+1, pHSize-1);
            id=0; strToInt (temp, &id);

                 if(id==1) strcpy22 (output, lchar_get_source(*lstr));
            else if(id==2) strcpy22 (output, intToStr(temp,lstr->line));
            else if(id==3) strcpy22 (output, intToStr(temp,lstr->coln));
            else if(id==4) strcpy23 (output, lstr);
            else
            {
                id -= 5;
                if(id<0 || id>=10) { *output++ = *format++; continue; }

                while(args<=id) // get arguments after 'lstr'
                    argv[args++] = va_arg (varg, const wchar*);

                strcpy22 (output, argv[id]);
            }
            output += strlen2(output);
            format += pHSize;
        }
    }
    va_end(varg);   // end variable argument list
    *output = 0;
}



/* Example inputs to deal with are:
   1) abc def ghi   // 3 blocks: 'abc' 'def' 'ghi'
   2) "abc def ghi" // 1 block : 'abc def ghi'
   3) abc "def" ghi // 3 blocks: 'abc' 'def' 'ghi'
   4) abc"def "ghi  // 3 blocks: 'abc' 'def ' 'ghi'
*/
const wchar* sgets2 (const wchar* input, wchar** output)
{
    const wchar* start=NULL; // _char
    int onLoad=0;
    int size=0;
    wchar c;
    if(input==NULL) return NULL;
    while(1)
    {
        c = *input; // inGet
        if(c==0) break;
        if(c=='"')
        {   input++; // inNext
            if(onLoad) break;
            else { onLoad=2; start=input; }
        }
        if(isSpace(c)) { if(onLoad==1) break; }
        else if(onLoad==0) { onLoad=1; start=input; }
        if(onLoad) size++;
        input++; // inNext
    }
    if(output!=NULL) astrcpy22S (output, start, size); // _astrcpy
    return input;
}



const lchar* sgets3 (const lchar* input, lchar** output)
{
    const lchar* start=NULL; // _char
    int onLoad=0;
    int size=0;
    wchar c;
    if(input==NULL) return NULL;
    while(1)
    {
        c = input->wchr; // inGet
        if(c==0) break;
        if(c=='"')
        {   input = input->next; // inNext
            if(onLoad) break;
            else { onLoad=2; start=input; }
        }
        if(isSpace(c)) { if(onLoad==1) break; }
        else if(onLoad==0) { onLoad=1; start=input; }
        if(onLoad) size++;
        input = input->next; // inNext
    }
    if(output!=NULL) astrcpy33S (output, start, size); // _astrcpy
    return input;
}



/******************************************************************************************/

const lchar* lchar_next (const lchar* lstr)
{
    while(1) // not a loop
    {
        if(lstr->wchr != '#')
        { lstr = lstr->next;
          if(lstr->wchr != '#') break; }

        lstr = lstr->next;
        if(lstr->wchr != '{') // skip till '\n' is found, excluding it
        { while(lstr->wchr != 0 && lstr->wchr != '\n') lstr = lstr->next; break; }

        // else: skip till '}#' is found, including it
        wchar a, b;
        int level=1;
        lstr = lstr->next;
        b = lstr->wchr;
        while(b!=0)
        {
            lstr = lstr->next;
            a = b;
            b = lstr->wchr;
            if(a=='#' && b=='{') level++;
            if(a=='}' && b=='#')
            { if(--level==0) { lstr = lstr->next; break; } }
        }
        break;
    }
    return lstr;
}



const lchar* lchar_goto (const lchar* lstr, int offset)
{
    if(!lstr) return lstr;
    if(offset < 0)
    {
        for( ; offset != 0; offset++)
        {
            if(lstr->prev==0 || lstr->prev->wchr==0) break;
            if(lstr->prev) lstr = lstr->prev;
        }
    }
    else if(offset > 0)
    {
        for( ; offset != 0; offset--)
        {
            if(lstr->wchr==0) break;
            if(lstr->next) lstr = lstr->next;
        }
    }
    return lstr;
}



static wchar* names[1000] = {NULL};
static unsigned short count = 1; // keep [0]==NULL
static unsigned short submitSourceName (const wchar* name)
{
    unsigned short i;
    for(i=0; i<count; i++)
        if(0==strcmp22(names[i], name))
            break;
    if(i==count)
    {   astrcpy22(&names[i], name);
        if(count+1 < SIZEOF(names)) count++;
    }
    return i;
}
const wchar* lchar_get_source (lchar lchr) { return names[lchr.source]; }

void set_line_coln_source (lchar* lstr, int line, int coln, const wchar* sourceName)
{
    if(!lstr) return;
    int sourceID = submitSourceName(sourceName);
    while(true)
    {
        lstr->line = line;
        lstr->coln = coln;
        if(sourceName) lstr->source = sourceID;
        if(lstr->wchr == 0) break;
        if(lstr->wchr == '\n')
        { line++; coln=1; }
        else coln++;
        lstr = lstr->next;
    }
}


void string_c_clean()
{
    wchar_free(buffer); buffer=NULL;
    int i;
    for(i=0; i<count; i++)
    {
        wchar_free(names[i]);
        names[i] = NULL;
    }
}

/******************************************************************************************/

#ifdef CUSTOM_STDIO_H


/* get integer from string (decimal digits) */
// THIS CODE IS PROBABLY WRONG!!!
int strtoint (const char* str)
{
    int ans=0;
    bool neg=false;
    if(*str=='-')                   // if number is negative
    { neg=true; str++; }            // that is first character is '-'
    do
    {   ans *= 10;
        if(*str<'0' || *str>'9')    // if not a decimal digit, then
            return 0x7FFFFFFF;      // error, return maximum integer type.
        ans += (int)(*str - '0');   // else add decimal digit.
        str++;
    } while(*str!=0);               // while not end of string
    if(neg) return -ans;
    else return ans;
}

/* get integer from string (hexadecimal digits) */
int strtohex (const char* str)
{
    int ans=0;
    return ans;
}

/* get integer from string (binary digits) */
int strtobin (const char* str)
{
    int ans=0;
    return ans;
}


void flttostr (float n, char* str)
{
}

void eflttostr (float n, char* str)
{
}


/* get string from integer (decimal digits) */
char* inttostr (int n, char* str)
{
    unsigned int len=0, neg=0;

    if(n<0)                         // if number is negative
    {   n=-n;                       // then put first character '-'
        str[len++]='-';
        neg=1;                      // offset for strreverse()
    }
    do{
        str[len++] = (char)(n%10) + '0';    // get and put new decimal digit
    } while ((n /= 10) > 0);
    str[len] = '\0';                // end string

    strrev1(str+neg, len-neg);   // reverse the digits
    return str;
}

/* get string from integer (decimal digits) */
char* uinttostr (unsigned int n, char* str)
{
    unsigned int len=0;

    do{
        str[len++] = (char)(n%10) + '0';    // get and put new decimal digit
    } while ((n /= 10) > 0);
    str[len] = '\0';                // end string

    strrev1(str, len);   // reverse the digits
    return str;
}

/* get string from integer (hexadecimal digits) */
char* hextostr (long long n, char* str)
{
    char digits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    int len=0, neg=0;

    if(n<0)                         // similar procedure as that
    {   n=-n;                       // used in inttostr()
        str[len++]='-';
        neg=1;
    }
    do{
        str[len++] = digits[n & 0xF]; // get and put new hexadecimal digit
    } while((n=n>>4)>0);
    str[len] = '\0';                // end string

    strrev1(str+neg, len-neg);
    return str;
}

/* get string from pointer) */
char* ptrtostr (char* p, char* str)
{
    return hextostr ((long long)p, str);
}

/* get string from integer (binary digits) */
char* bintostr (int n, char* str)
{
    int len=0, neg=0;

    if(n<0)                         // similar procedure as that
    {   n=-n;                       // used in inttostr()
        str[len++]='-';
        neg=1;
    }
    do{
        str[len++] = (char)(n & 1) + '0'; // get and put new binary digit
    } while((n=n>>1)>0);
    str[len] = '\0';                // end string

    strrev1(str+neg, len-neg);
    return str;
}



void sprintf1 (char* outputString, const char* format, ...)
{
    char to_str[64];                    // integer to string, maximum of 64 digits
    char chr, *str;
    float f;
    int n;

    va_list varg;                       // start of variable argument list
    va_start(varg, format);             // ...not sure how these things works...!

    to_str[0] = 0;

    for(; *format != 0; format++)
    {
        if(*format!='%')
            *outputString++ = *format;
        else
        {
            format++;
            switch(*format)
            {
            case 'c':
                chr = (char)(va_arg(varg, int)); // get next argument as a char (was passed as an integer)
                *outputString++ = chr;
                break;
            case 's':
                str = va_arg(varg, char*);      // get next argument as a char* (string)
                while(*str) *outputString++ = *str++;
                break;
            case 'd':
                n = va_arg(varg, int);          // get next argument as an integer in decimal
                inttostr(n, to_str);
                str = to_str;
                while(*str) *outputString++ = *str++;
                break;
            case 'u':
                n = va_arg(varg, int);          // get next argument as an integer in decimal
                uinttostr(n, to_str);
                str = to_str;
                while(*str) *outputString++ = *str++;
                break;
            case 'x':
                n = va_arg(varg, int);          // get next argument as an integer in hexadecimal
                hextostr(n, to_str);
                str = to_str;
                while(*str) *outputString++ = *str++;
                break;
            case 'b':
                n = va_arg(varg, int);          // get next argument as an integer in binary
                bintostr(n, to_str);
                str = to_str;
                while(*str) *outputString++ = *str++;
                break;
            case 'p':
                *outputString++ = '0';
                *outputString++ = 'x';
                str = va_arg(varg, void*);      // get next argument as a pointer
                ptrtostr(str, to_str);
                str = to_str;
                while(*str) *outputString++ = *str++;
                break;
            case 'f':
                f = va_arg(varg, float);        // get next argument as a float
                flttostr(f, to_str);
                str = to_str;
                while(*str) *outputString++ = *str++;
                break;
            case 'e':
                f = va_arg(varg, float);        // get next argument as a float in standard notation
                eflttostr(f, to_str);
                str = to_str;
                while(*str) *outputString++ = *str++;
                break;
            default:
                *outputString++ = '%';
                *outputString++ = *format;
                break;
            }
        }
    }
    va_end(varg);   // end variable argument list
    *outputString++ = '\0';
}

#endif // end #ifdef CUSTOM_STDIO_H


