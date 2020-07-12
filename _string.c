/*
    _string.c
*/

#include "_math.h"
#include "_texts.h"
#include "_stdio.h"
#include "_string.h"


/*********************************************/
// chr => character
// pcn => partly code number
// pifcn => partly is fully code number
// 6 = sizeof("\uXXXX")

static bool pifcn[0x10000] = {false};

static void initial_set_pif_cn ()
{
    pifcn[0x0001] = true;
    pifcn[0xFFFF] = true;
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
    while(s!=NULL) // not a loop
    {
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
                     if('0'<=c && c<='9') c=c-'0';
                else if('A'<=c && c<='F') c=c-'A'+10;
                else if('a'<=c && c<='f') c=c-'a'+10;
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
                if('0'<=c && c<='9') c=c-'0';
                else break;
                i = i*10 + c;
                s++;
                if(--j==0) break;
                if(s==end) break;
            }
            c=i;
        }
        break;
    }
    if(out) *out=c;
    int r = s - str;
    return end ? r : -r;
}

value pcn_to_chr (value v)
{
    value y = vPrev(v);
    const_Str2 str = getStr2(vGet(y));
    const_Str2 end = str+strlen2(str);
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

static value chr_to_ (value v, bool fcn)
{
    value y = vPrev(v);
    const_Str2 in = getStr2(vGet(y));

    long size = strlen2(in)*6;
    if(size==0) return v;

    initial_set_pif_cn();
    wchar* out = (wchar*)v;

    for( ; !strEnd2(in); in++)
    {
        if(fcn || pifcn[*in])
            out = wchar_to_wstr (out, *in);
        else
        switch(*in)
        {
        case '\\': *out++ = '\\'; *out++ = '\\'; break;
        case '\t': *out++ = '\\'; *out++ = 't'; break;
        case '\r': *out++ = '\\'; *out++ = 'r'; break;
        case '\n': *out++ = '\\'; *out++ = 'n'; break;
        default: *out++ = *in; break;
        }
    }
    *out = 0;
    return setStr22(y, (wchar*)v);
}

value chr_to_pcn (value v) { return chr_to_(v, 0); }
value chr_to_fcn (value v) { return chr_to_(v, 1); }

value set_pif_cn (value v)
{
    value y = vPrev(v);
    const_Str2 in = getStr2(vGet(y));
    const_Str2 end= in + strlen2(in);
    bool found=false;
    wchar c;
    for( ; in && *in; in++)
    {
        if(in[0]!='\\') continue;
        c = in[1];
        if(c!='u' && c!='U' && !isDigit(c)) continue;
        int i = wstr_to_wchar(&c, in, end);
        if(i>0)
        {
            pifcn[c] = true;
            found = true;
            in += i-1; // -1 due to in++ in for loop
        }
    }
    if(found) v = setBool(y, true);
    else v = setError(y, L"No code number found in given text.");
    return v;
}

/******************************************************************************************/


Str2 sprintf2 (Str2 output, int argc, const_Str2* argv)
{
    long len = 0;
    bool getlen = output==NULL;
    if(!getlen) *output=0;
    if(argc<1 || argv==NULL) return output;

    const_Str2 format = argv[0];
    wchar s[3];
    int i=0;
    while(true)
    {
        if(getlen && output)
        {   len += output - (Str2)0;
            output = NULL;
        }
        if(strEnd2(format)) break;
        wchar c = *format++;
        if(c != '%')
        {
            s[0]=*(format-1); s[1]=0;
            output = strcpy22(output, s);
            continue;
        }
        if(!strEnd2(format))
            c = *format++;
        if(c == '%')
        {
            s[0]=*(format-1); s[1]=0;
            output = strcpy22(output, s);
            continue;
        }
        if(c == '0') { i++; continue; }
        else if(c == 's') i++;
        else if('1'<=c && c<='9') i = c-'0';
        else if('A'<=c && c<='Z') i = c-'A'+10;
        else
        {
            s[0]=*(format-2); s[1]=*(format-1); s[2]=0;
            output = strcpy22(output, s);
            continue;
        }
        if(i < argc) output = strcpy22(output, argv[i]);
    }
    if(getlen) output += len;
    return output;
}



Str2 set_message (Str2 output, int argc, const_Str2* argv, const_Str3 str)
{
    long len = 0;
    bool getlen = output==NULL;
    if(!getlen) *output=0;
    if(argc<=0 || argv==NULL) return output;
    assert(str.ptr); if(!str.ptr) return output;

    const_Str2 format = argv[0];
    wchar s[3];
    int i=0;
    while(true)
    {
        if(getlen && output)
        {   len += output - (Str2)0;
            output = NULL;
        }
        if(strEnd2(format)) break;
        wchar c = *format++;
        if(c != '%')
        {
            s[0]=*(format-1); s[1]=0;
            output = strcpy22(output, s);
            continue;
        }
        if(!strEnd2(format))
            c = *format++;
        if(c == '%')
        {
            s[0]=*(format-1); s[1]=0;
            output = strcpy22(output, s);
            continue;
        }
        if(c == '0') { i++; continue; }
        else if(c == 's') i++;
        else if('1'<=c && c<='9') i = c-'0';
        else if('A'<=c && c<='Z') i = c-'A'+10;
        else
        {
            s[0]=*(format-2); s[1]=*(format-1); s[2]=0;
            output = strcpy22(output, s);
            continue;
        }
             if(i==1) output = strcpy23(output, str);
        else if(i==2) output = intToStr(output, sLine(str));
        else if(i==3) output = intToStr(output, sColn(str));
        else if(i==4) output = strcpy22(output, lchar_get_source(str));
        else if(i-4 < argc) output = strcpy22(output, argv[i-4]);
    }
    if(getlen) output += len;
    return output;
}



const_Str2 strNext2 (const_Str2 str)
{
    while(true) // not a loop
    {
        if(strEnd2(str)) break;
        if(*str != '#')
        {
            str++;
            if(strEnd2(str)) break;
            if(*str != '#') break;
        }
        str++;
        if(strEnd2(str)) break;
        if(*str != '{')
        {
            // skip till '\n' is found, excluding it
            while(!strEnd2(str)
              && *str != '\n')
                str++;
            break;
        }
        // else: skip till "}#" is found, including it
        wchar a, b;
        int level=1;
        str++;
        b = *str;
        while(true)
        {
            str++;
            if(strEnd2(str)) break;
            a = b;
            b = *str;
            if(a=='#' && b=='{') level++;
            if(a=='}' && b=='#')
            {
                if(--level==0)
                { str++; break; }
            }
        }
        break;
    }
    return str;
}

const_Str3 strNext3 (const_Str3 str)
{
    while(true) // not a loop
    {
        if(strEnd3(str)) break;
        if(sChar(str) != '#')
        {
            str = sNext(str);
            if(strEnd3(str)) break;
            if(sChar(str) != '#') break;
        }
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
        break;
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
    {   astrcpy22(&names[i], name);
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
