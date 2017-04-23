/*
    _stdio.c
*/

#include "_stdio.h"
#include "_string.h"
#include "_malloc.h"


/*******************************************************************************************/


#ifndef CUSTOM_STDIO_H

#include <stdio.h>


void wait() { printf("press enter... "); getchar(); }


void putc2 (mchar chr) { putchar((char)chr); }


bool file_to_array (const  char* filename,  char** filecontent_ptr, size_t *contentSize_ptr)
{
    size_t size;
    char* filecontent;

    FILE* file = fopen (filename, "rb");
    if(file==NULL) return false;

    fseek (file, 0, SEEK_END);
    size = ftell (file);
    fseek (file, 0, SEEK_SET);
    if(size==0x7FFFFFFF) { fclose(file); return false; }

    if(contentSize_ptr!=NULL) *contentSize_ptr = size;
    if(filecontent_ptr!=NULL)
    {
        filecontent = (char*) _realloc (*filecontent_ptr, size+1);
        *filecontent_ptr = filecontent;

        if(fread (filecontent, 1, size, file) != size)
        { fclose(file); return false; }
        filecontent[size]=0;
    }
    fclose(file);
    return true;
}


bool array_to_file (const char* filename, const char* filecontent, size_t contentSize)
{
    FILE* file = fopen(filename, "wb");
    if(file==NULL) return 0;
    fwrite(filecontent, 1, contentSize, file);
    fclose(file);
    return 1;
}


#include <sys/stat.h>
#include <time.h>
//#include <utime.h>

bool file_is_modified (const mchar* fileName)
{
    static time_t FileTime[100]={0};
    static mchar* FileName[100]={0};
    static int count=0;

    int i, err;
    struct stat file_stat;
    const char* path_name = CST12(add_path_to_file_name(NULL, fileName));

    err = stat (path_name, &file_stat);
    if(err!=0) { /*printf("In file_is_modified(): error on calling stat().\r\n");*/ return false; }

    for(i=0; i<count; i++) if(0==strcmp22(FileName[i], fileName)) break;
    if(i==count) astrcpy22(&FileName[i], fileName);

    if(file_stat.st_mtime <= FileTime[i])
    { /*printf("In file_is_modified(): On '%s': file_stat.mtime <= FileTime[%d]\r\n", CST12(fileName), i);*/ return false; }

    FileTime[i] = file_stat.st_mtime;
    //printf("In file_is_modified():   FileName[%d] = %s   FileTime[%d] = %lld\r\n", i, CST12(FileName[i]), i, (long long)FileTime[i]);
    return true;
}


#else

/* Nothing */

#endif


/*******************************************************************************************/


void puts1 (const  char* str) { if(str) { while(*str) { putc2((mchar)*str); str++; } putc2('\n'); } }

void puts2 (const mchar* str) { if(str) { while(*str) { putc2(*str); str++; } putc2('\n'); } }

void puts3 (const lchar* str) { if(str) { while(str->mchr) { putc2(str->mchr); str = str->next; } putc2('\n'); } }

void puts2S (const mchar* str, size_t length)
{
    const mchar* str_end = str + length;
    if(!str) return;
    if(length < 0) { puts2(str); return; }
    while(str != str_end)
    { putc2(*str); str++; }
    putc2('\n');
}

void puts3LC (const lchar* str)
{
    if(str) while(str->mchr)
    {
        if(str->mchr=='\r') { putc2('\\'); putc2('r'); }
        else if(str->mchr=='\n') { putc2('\\'); putc2('n'); }
        else putc2(str->mchr);
        printf(":%d:%d ", str->line, str->coln);
        str = str->next;
    }
    putc2('\n');
}


/*******************************************************************************************/


const mchar* get_extension_from_name (mchar* file_extension, const mchar* file_name)
{
    static mchar extension[10]; // 10 also used below
    size_t i, len;

    if(file_extension==NULL) file_extension = extension;
    file_extension[0]=0;

    if(file_name==NULL) return NULL;

    len = strlen2(file_name);
    for(i=len; i>0; i--)
    {
        mchar c = file_name[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
        if(c=='.')
        {   if(len-i <= 10)
                strcpy22( file_extension, file_name+i);
            break;
        }
    }
    return file_extension;
}


const mchar* get_name_from_path_name (mchar* file_name, const mchar* file_path_name)
{
    static mchar name[MAX_PATH_SIZE];
    size_t i, len;

    if(file_name==NULL) file_name = name;
    file_name[0]=0;

    if(file_path_name==NULL) return NULL;

    len = strlen2(file_path_name);
    for(i=len; i>0; i--)
    {
        mchar c = file_path_name[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    strcpy22( file_name, file_path_name + i);
    return file_name;
}


const mchar* get_path_from_path_name (mchar* file_path, const mchar* file_path_name)
{
    static mchar path[MAX_PATH_SIZE];
    size_t i, len;

    if(file_path==NULL) file_path = path;
    file_path[0]=0;

    if(file_path_name==NULL) return NULL;

    len = strlen2(file_path_name);
    for(i=len; i>0; i--)
    {
        mchar c = file_path_name[i-1];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    strcpy22S( file_path, file_path_name, i);
    return file_path;
}


const mchar* add_path_to_file_name (const mchar* file_path, const mchar* file_name)
{
    static mchar fileName[MAX_PATH_SIZE];
    fileName[0]=0;
    size_t i;

    if(file_name==NULL || file_name[0]==0) return NULL;

    if(file_path==NULL) file_path = default_file_path;

    for(i=0; file_name[i]!=0; i++)
    {
        mchar c = file_name[i];
        if(c=='/' || c=='|' || c=='\\') break;
    }
    if(file_name[i]==0 && file_name[1]!=':') // for Windows' C: or D: or E:
        strcpy22(fileName, file_path);

    strcpy22(fileName+strlen2(fileName), file_name);
    return fileName;
}


mchar default_file_path[MAX_PATH_SIZE];


/*******************************************************************************************/


// to record whether opened file is of 1-byte or 2-bytes character
static int type = 1;

static void docopy (mchar* output, const mchar* input, size_t size)
{
    while(size--)
    {
        mchar c = *input++;
        if(type==3) c = (((c&0xFF)<<8) | ((c>>8)&0xFF));
        *output++ = c;
    }
    *output = 0;
}


bool Openfile (const mchar* fileName, mchar** fileContent_ptr, size_t *contentSize_ptr)
{
    size_t n=0, size;
    unsigned char* str;
    char* filecontent=NULL;
    mchar* fileContent;
    const char* filename;

    filename = CST12(add_path_to_file_name(NULL, fileName));
    if(!file_to_array( filename, &filecontent, &size)) return false;

    str = (unsigned char*)filecontent;
         if(size>=2 && (str[0]==0xFF && str[1]==0xFE)) { type = 2; n = 2; } // check for UTF-16-BOM little-endian
    else if(size>=2 && (str[0]==0xFE && str[1]==0xFF)) { type = 3; n = 2; } // check for UTF-16-BOM big-endian
    else if(size>=4 && (str[0]==0x00 || str[2]==0x00)) { type = 3; n = 0; } // if yet a UTF-16 big-endian
    else if(size>=4 && (str[1]==0x00 || str[3]==0x00)) { type = 2; n = 0; } // if yet a UTF-16 little-endian
    else
    {   type = 1; // assume UTF-8 encoding

        if(str[0]==0xEF
        && str[1]==0xBB
        && str[2]==0xBF)
        { str+=3; size-=3; } // skip the BOM of UTF-8

        if(fileContent_ptr!=NULL)
        {
            fileContent = mchar_alloc (*fileContent_ptr, size);
            *fileContent_ptr = fileContent;

            size = (strcpy21S(fileContent, (char*)str, size) - fileContent);
        }
    }
    if(type != 1)
    {
        if(fileContent_ptr!=NULL)
        {
            fileContent = mchar_alloc (*fileContent_ptr, size);
            *fileContent_ptr = fileContent;

            str += n; // skip the BOM of UTF-16
            size -= n;
            size /= 2;
            docopy(fileContent, (mchar*)str, size);
        }
    }
    if(contentSize_ptr!=NULL) *contentSize_ptr = size;
    _free(filecontent);
    return true;
}



bool Savefile (const mchar* fileName, const mchar* fileContent, size_t contentSize)
{
    size_t size;
    bool success;
    char *filecontent;
    const char* filename;

    if(!fileName || !fileContent) return false;
    if(contentSize==-1) contentSize = strlen2(fileContent);

    if(type==1)
    {
        size = 3*contentSize;
        filecontent = char_alloc(NULL, size);
        size = (strcpy12S(filecontent, fileContent, contentSize) - filecontent);
    }
    else
    {
        size = 2 + contentSize*sizeof(mchar);
        filecontent = char_alloc(NULL, size);

        if(type==2)       // if is little-endian
        {   filecontent[0] = 0xFF;
            filecontent[1] = 0xFE;
        }
        else              // else is big-endian
        {   filecontent[0] = 0xFE;
            filecontent[1] = 0xFF;
        }
        docopy((mchar*)(filecontent+2), fileContent, contentSize);
    }

    filename = CST12(add_path_to_file_name(NULL, fileName));
    success = array_to_file( filename, filecontent, size);

    char_free(filecontent);
    return success;
}

