/*
    _stdio.c
*/

#include "_stdio.h"
#include "_string.h"
#include "_malloc.h"


/*******************************************************************************************/


#ifndef USE_CUSTOM_STDIO_H

#include <stdio.h>
#include <wchar.h>


void wait() { printf("press enter... "); getchar(); }


void putc2 (wchar chr) { putwchar(chr); }


Array1 FileOpen1 (const  char* filename1, Array1 old1)
{
    Array1 content={0};

    FILE* file = fopen (filename1, "rb");
    if(file==NULL) content.size = -1;
    else
    {
        fseek(file, 0, SEEK_END);
        content.size = ftell(file);
        fseek(file, 0, SEEK_SET);

        if(content.size == 0x7FFFFFFF)  // if content size is invalid
            content.size = -2;

        else if(old1.size != -1) // if allowed to get content data as well
        {
            content.data = char_alloc (old1.data, content.size);

            if(fread(content.data, 1, content.size, file) != content.size)
            {
                // if failed to read content data
                char_free(content.data);
                content.data=NULL;
            }
            else content.data[content.size]=0;
        }
        fclose(file);
    }
    return content;
}


int FileSave1 (const  char* filename1, const_Array1 content1)
{
    FILE* file = fopen(filename1, "wb");
    if(file==NULL) return false;
    fwrite(content1.data, 1, content1.size, file);
    fclose(file);
    return true;
}


#include <sys/stat.h>
#include <time.h>
//#include <utime.h>

bool file_is_modified (const wchar* fileName)
{
    static time_t FileTime[100]={0};
    static wchar* FileName[100]={0};
    static int count=0;

    int i, err;
    struct stat file_stat;
    const char* path_name = CST12(add_path_to_file_name(NULL, fileName));

    err = stat (path_name, &file_stat);
    if(err!=0)
    {   //printf("In file_is_modified(): error on calling stat().\r\n");
        return false;
    }

    for(i=0; i<count; i++) if(0==strcmp22(FileName[i], fileName)) break;
    if(i==count) astrcpy22(&FileName[i], fileName);

    if(file_stat.st_mtime <= FileTime[i])
    {   //printf("In file_is_modified(): On '%s': file_stat.mtime <= FileTime[%d]\r\n", CST12(fileName), i);
        return false;
    }

    FileTime[i] = file_stat.st_mtime;
    //printf("In file_is_modified():   FileName[%d] = %s   FileTime[%d] = %lld\r\n", i, CST12(FileName[i]), i, (long long)FileTime[i]);
    return true;
}


#else

/* Nothing */

#endif


/*******************************************************************************************/


void puts1 (const  char* str) { if(str) { while(*str) { putc2((wchar)*str); str++; } putc2('\n'); } }

void puts2 (const wchar* str) { if(str) { while(*str) { putc2(*str); str++; } putc2('\n'); } }

void puts3 (const lchar* str) { if(str) { while(str->wchr) { putc2(str->wchr); str = str->next; } putc2('\n'); } }

void puts2S (const wchar* str, size_t length)
{
    const wchar* str_end = str + length;
    if(!str) return;
    if(length < 0) { puts2(str); return; }
    while(str != str_end)
    { putc2(*str); str++; }
    putc2('\n');
}

void puts3LC (const lchar* str)
{
    if(str) while(str->wchr)
    {
        if(str->wchr=='\r') { putc2('\\'); putc2('r'); }
        else if(str->wchr=='\n') { putc2('\\'); putc2('n'); }
        else putc2(str->wchr);
        printf(":%d:%d ", str->line, str->coln);
        str = str->next;
    }
    putc2('\n');
}


/*******************************************************************************************/


const wchar* get_extension_from_name (wchar* extension, const wchar* file_name)
{
    static wchar _extension[10]; // 10 also used below
    size_t i, len;

    if(extension==NULL) extension = _extension;
    extension[0]=0;
    if(file_name==NULL) return NULL;

    len = strlen2(file_name);
    for(i=len; i>0; i--)
    {
        wchar c = file_name[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
        if(c=='.')
        {   if(len-i <= 10)
                strcpy22(extension, file_name+i);
            break;
        }
    }
    return extension;
}


const wchar* get_name_from_path_name (wchar* name, const wchar* path_name)
{
    static wchar _name[MAX_PATH_SIZE];
    size_t i, len;

    if(name==NULL) name = _name;
    name[0]=0;
    if(path_name==NULL) return name;

    len = strlen2(path_name);
    for(i=len; i>0; i--)
    {
        wchar c = path_name[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    strcpy22(name, path_name+i);
    return name;
}


const wchar* get_path_from_path_name (wchar* path, const wchar* path_name)
{
    static wchar _path[MAX_PATH_SIZE];
    size_t i, len;

    if(path==NULL) path = _path;
    path[0]=0;
    if(path_name==NULL) return NULL;

    len = strlen2(path_name);
    for(i=len; i>0; i--)
    {
        wchar c = path_name[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    strcpy22S(path, path_name, i);
    return path;
}


const wchar* add_path_to_file_name (const wchar* file_path, const wchar* file_name)
{
    static wchar path_name[MAX_PATH_SIZE];
    path_name[0]=0;
    size_t i;

    if(file_name==NULL || file_name[0]==0) return NULL;

    if(file_path==NULL) file_path = default_file_path;

    for(i=0; file_name[i]!=0; i++)
    {
        wchar c = file_name[i];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    if(file_name[i]==0 && file_name[1]!=':') // for Windows's C: or D: or ...
        strcpy22(path_name, file_path);

    strcat22(path_name, file_name);
    return path_name;
}


wchar default_file_path[MAX_PATH_SIZE];


/*******************************************************************************************/


// to record whether opened file is of 1-byte or 2-bytes character
static int g_type = 1; // assume UTF-8 encoding

static void docopy (wchar* output, const wchar* input, long size)
{
    while(size-- > 0)
    {   wchar c = *input++;
        if(g_type==3) c = (((c&0xFF)<<8) | ((c>>8)&0xFF));
        *output++ = c;
    } *output = 0;
}


Array2 FileOpen2 (const wchar* filename2, Array2 old2)
{
    long n=0;
    unsigned char* str;
    const char* filename1;
    Array1 content1={0};
    Array2 content2={0};

    filename1 = CST12(add_path_to_file_name(NULL, filename2));
    if((content1 = FileOpen1(filename1, content1)).size<=0)
    { content2.size = content1.size; return content2; }

    str = (unsigned char*)content1.data;
    n = content1.size;
         if(n>=2 && (str[0]==0xFF && str[1]==0xFE)) { g_type=2; n=2; } // check for UTF-16-BOM little-endian
    else if(n>=2 && (str[0]==0xFE && str[1]==0xFF)) { g_type=3; n=2; } // check for UTF-16-BOM big-endian
    else if(n>=4 && (str[0]==0x00 || str[2]==0x00)) { g_type=3; n=0; } // if yet a UTF-16 big-endian
    else if(n>=4 && (str[1]==0x00 || str[3]==0x00)) { g_type=2; n=0; } // if yet a UTF-16 little-endian
    else
    {   g_type = 1; // assume UTF-8 encoding

        if(str[0]==0xEF
        && str[1]==0xBB
        && str[2]==0xBF)
        { str+=3; content1.size-=3; } // skip the BOM of UTF-8

        content2.size = strlen21(content1.data); // get content size

        if(old2.size!=-1) // if allowed to get content data as well
        {
            content2.data = wchar_alloc (old2.data, content2.size);
            strcpy21S(content2.data, (char*)str, content1.size);
        }
    }
    if(g_type != 1)
    {
        str += n; // skip the BOM of UTF-16
        content1.size -= n;
        content2.size = content1.size / 2; // get content size

        if(old2.size != -1) // if allowed to get content data as well
        {
            content2.data = wchar_alloc (old2.data, content2.size);
            docopy(content2.data, (wchar*)str, content2.size);
        }
    }
    char_free(content1.data);
    return content2;
}



int FileSave2 (const wchar* filename2, const_Array2 content2)
{
    const char* filename1;
    Array1 content1;
    bool success;

    if(!filename2 || !content2.data) return false;
    if(content2.size==-1) content2.size = strlen2(content2.data);

    if(g_type==1)
    {
        content1.size = 3*content2.size;
        content1.data = char_alloc(NULL, content1.size);
        strcpy12S(content1.data, content2.data, content2.size);
        content1.size = strlen1(content1.data);
    }
    else
    {
        content1.size = 2 + content2.size*sizeof(wchar);
        content1.data = char_alloc(NULL, content1.size);

        if(g_type==2)       // if is little-endian
        {   content1.data[0] = (char)0xFF;
            content1.data[1] = (char)0xFE;
        }
        else              // else is big-endian
        {   content1.data[0] = (char)0xFE;
            content1.data[1] = (char)0xFF;
        }
        docopy((wchar*)(content1.data+2), content2.data, content2.size);
    }

    filename1 = CST12(add_path_to_file_name(NULL, filename2));
    success = FileSave1(filename1, ConstArray1(content1));

    char_free(content1.data);
    return success;
}

