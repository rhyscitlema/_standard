#ifndef __STDIO_H
#define __STDIO_H
/*
    _stdio.h
*/

#include "_stddef.h"

#define MAX_PATH_SIZE 300 /* = maximum filename size */


void wait(); /* wait for user to press Enter on command-line-interface */

/* these 2 are explicitly provided by the platform-dependent code */
void wait_for_user_first   (const wchar* title, const wchar* message);
bool wait_for_confirmation (const wchar* title, const wchar* message);


/* put character in default standard output */
void putc2 (wchar chr);

/* put string using calls to putc2(chr) */
void puts1 (const  char* str);
void puts2 (const wchar* str);
void puts3 (const lchar* str);
void puts2S (const wchar* str, size_t length);
void puts3LC (const lchar* str);


const wchar* get_extension_from_name (wchar* extension, const wchar* file_name);
const wchar* get_name_from_path_name (wchar* name, const wchar* path_name);
const wchar* get_path_from_path_name (wchar* path, const wchar* path_name);
const wchar* add_path_to_file_name (const wchar* file_path, const wchar* file_name);
extern wchar default_file_path[];


/* Important: 'old' must be set to = {0} if very first time */
Array1 FileOpen1 (const  char* filename1, Array1 old1);
Array2 FileOpen2 (const wchar* filename2, Array2 old2);

int FileSave1 (const  char* filename1, const_Array1 content1);
int FileSave2 (const wchar* filename2, const_Array2 content2);

bool file_is_modified (const wchar* fileName);


#endif

