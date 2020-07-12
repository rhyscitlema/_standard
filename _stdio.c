/*
    _stdio.c
*/

#include "_stdio.h"
#include "_value.h"
#include "_string.h"


/*******************************************************************************************/


#ifndef USE_CUSTOM_STDIO_H

#include <stdio.h>
#include <wchar.h>


void wait() { printf("press enter... "); getchar(); }


// TODO: using putwchar() makes printf() [and even putchar()] to not work. Despite that this putwchar() does not even work.
void putc2 (wchar chr) { putchar(chr); }


value FileOpen1 (const_Str1 filename, value stack)
{
    const_Str2 argv[2];
    *argv=NULL;

    FILE* file = fopen(filename, "rb");
    if(file==NULL)
        argv[0] = L"Error: cannot open the file \"%s\".";
    else
    {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // if content size is invalid
        if(size == 0x7FFFFFFF)
            argv[0] = L"Error: invalid size of the file \"%s\".";
        else
        {
            char* out = (char*)(stack+2);
            if(fread(out, 1, size, file) == size)
            {
                *(out+size) = '\0';
                stack = onSetStr1(stack, out+size);
            }
            else argv[0] = L"Error while reading the file \"%s\".";
        }
        fclose(file);
    }
    if(*argv){
        argv[1] = C21(filename);
        stack = setMessage(stack, 0, 2, argv);
    }
    return stack;
}


value FileSave1 (const_Str1 filename, value stack)
{
    value y = vPrev(stack);
    FILE* file = fopen(filename, "wb");
    if(file==NULL)
    {
        const_Str2 argv[2];
        argv[0] = L"Error: cannot save the file \"%s\".";
        argv[1] = C21(filename);
        return setMessage(y, 0, 2, argv);
    }
    const_value n = vGet(y);
    const_Str1 str = getStr1(n);

    // strlen1(str) not used due to inner '\0', but exclude last '\0'
    long size = (*n & 0x0FFFFFFF)-1;

    fwrite(str, 1, size, file);
    fclose(file);
    return setBool(y, true);
}


#include <sys/stat.h>
#include <time.h>
//#include <utime.h>

bool file_modified (const_Str2 filename)
{
    static time_t FileTime[100]={0};
    static Str2   FileName[100]={0};
    static int    count=0;

    int i, err;
    struct stat file_stat;

    wchar wstr[MAX_PATH_LEN+1];
    const char* pathname = C12(add_path_to_file_name(filename, wstr));

    err = stat (pathname, &file_stat);
    if(err!=0)
    {   //printf("In file_modified(): error on calling stat().\r\n");
        return false;
    }

    for(i=0; i<count; i++) if(0==strcmp22(FileName[i], filename)) break;
    if(i==count) astrcpy22(&FileName[i], filename); // TODO: memory in file_modified() not freed

    if(file_stat.st_mtime <= FileTime[i])
    {   //printf("In file_modified(): On '%s': file_stat.mtime <= FileTime[%d]\r\n", C52(fileName), i);
        return false;
    }

    FileTime[i] = file_stat.st_mtime;
    return true;
}


#else

/* Nothing */

#endif


/*******************************************************************************************/


void puts1 (const_Str1 s) { while(!strEnd1(s)) { putc2(*s); s++; } putc2('\n'); }

void puts2 (const_Str2 s) { while(!strEnd2(s)) { putc2(*s); s++; } putc2('\n'); }

void puts3 (const_Str3 s) { while(!strEnd3(s)) { putc2(sChar(s)); s=sNext(s); } putc2('\n'); }

void puts3LC (const_Str3 s)
{
    while(!strEnd3(s))
    {
        wchar c = sChar(s);
        if(c=='\r') { putc2('\\'); putc2('r'); }
        else if(c=='\n') { putc2('\\'); putc2('n'); }
        else putc2(c);
        printf(":%d:%d ", sLine(s), sColn(s));
        s = sNext(s);
    }
    putc2('\n');
}

/*******************************************************************************************/


const_Str2 get_extension_from_name (const_Str2 filename)
{
    long i, len = strlen2(filename);
    for(i=len; i>0; i--)
    {
        if(len-i > 10) { i=0; break; }
        wchar c = filename[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
        if(c=='.') break;
    }
    if(i) filename += i;
    else filename = NULL;
    return filename;
}


const_Str2 get_name_from_path_name (const_Str2 pathname)
{
    long i, len = strlen2(pathname);
    for(i=len; i>0; i--)
    {
        wchar c = pathname[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    pathname += i;
    return pathname;
}


const_Str2 get_path_from_path_name (const_Str2 pathname, Str2 out)
{
    assert(out!=NULL);
    long i, len = strlen2(pathname);
    for(i=len; i>0; i--)
    {
        wchar c = pathname[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    strcpy22S(out, pathname, i);
    return out;
}


const_Str2 add_path_to_file_name   (const_Str2 filename, Str2 out)
{
    assert(out!=NULL);
    if(out) *out = 0;
    if(strEnd2(filename)) return out;

    int i;
    for(i=0; filename[i]; i++)
        if(filename[i]==':')
            break;
    if(filename[i]) i++; // skip the ':' found
    else i=0;
    wchar c = filename[i];

    if(!(c=='/' || c=='|' || c=='\\'))  // if not an absolute path
        strcpy22(out, default_file_path());

    strcat22(out, filename);
    return out;
}


static wchar dfp[MAX_PATH_LEN+1];
Str2 default_file_path() { return dfp; }


/*******************************************************************************************/


// g_type records the opened file's encoding:
// * if(g_type==1) then is UTF-8 encoding
// * if(g_type==2) then is UTF-16 little-endian
// * if(g_type==3) then is UTF-16 big-endian
static int g_type = 1; // assume UTF-8 encoding

static void docopy (wchar* output, const wchar* input, long size)
{
    if(input==output && g_type!=3) return; // avoid redundant copy
    while(size-- > 0)
    {
        wchar c = *input++;
        if(g_type==3) c = (((c&0xFF)<<8) | ((c>>8)&0xFF));
        *output++ = c;
    }
}


value FileOpen2 (const_Str2 filename, value stack)
{
    wchar wstr[MAX_PATH_LEN+1];
    const_Str1 filename1 = C12(add_path_to_file_name(filename, wstr));

    value v = FileOpen1(filename1, stack);
    if(VERROR(v)) return v;

    const_Str1 in = getStr1(stack);
    long size = (*stack & 0x0FFFFFFF)-1;
    Str2 out = (wchar*)(stack+2);

    unsigned char* s = (unsigned char*)in;
    long n=size;
         if(n>=2 && (s[0]==0xFF && s[1]==0xFE)) { g_type=2; n=2; } // check for UTF-16-BOM little-endian
    else if(n>=2 && (s[0]==0xFE && s[1]==0xFF)) { g_type=3; n=2; } // check for UTF-16-BOM big-endian
    else if(n>=4 && (s[0]==0x00 || s[2]==0x00)) { g_type=3; n=0; } // if yet a UTF-16 big-endian
    else if(n>=4 && (s[1]==0x00 || s[3]==0x00)) { g_type=2; n=0; } // if yet a UTF-16 little-endian
    else
    {   g_type = 1; // assume UTF-8 encoding

        if(s[0]==0xEF
        && s[1]==0xBB
        && s[2]==0xBF)
        { in+=3; } // skip the BOM of UTF-8

        strcpy21((wchar*)v, in);
        out = strcpy22(out, (wchar*)v);
    }
    if(g_type != 1) // assume UTF-16 encoding
    {
        in += n; // skip the BOM of UTF-16
        size = (size-n)/2; // get content size
        docopy(out, (const wchar*)in, size);
        out+=size; *out=0;
    }
    return onSetStr2(stack, out);
}


value FileSave2 (const_Str2 filename, value stack)
{
    if(VERROR(stack)) return stack;
    value y = vPrev(stack);
    const_value n = vGet(y);
    const_Str2 content = getStr2(n);

    long size=0;
    Str1 out = (char*)(stack+2);

    if(g_type==1)
        out = strcpy12(out, content);
    else
    {
        size = 1+ (*n & 0x0FFFFFFF)-1; // 1+ for the BOM, -1 for the '\0'
        out[0] = (char)0xFF; // assume UTF-16 little-endian
        out[1] = (char)0xFE; // and provide the BOM of it
        docopy((wchar*)out, content, size); // convert if(g_type==3)
        size *= 2; // get the Str1 size excluding '\0'
        out+=size; *out=0;
    }
    stack = onSetStr1(stack, out);

    wchar wstr[MAX_PATH_LEN+1];
    const_Str1 filename1 = C12(add_path_to_file_name(filename, wstr));

    return vpcopy(y, FileSave1(filename1, stack));
}

