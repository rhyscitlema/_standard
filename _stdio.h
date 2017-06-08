#ifndef __STDIO_H
#define __STDIO_H
/*
    _stdio.h
*/

#include "_stddef.h"

#define MAX_PATH_SIZE 300 /* also maximum file name */


void wait(); /* wait for user to press Enter on CLI */

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


const wchar* get_extension_from_name (wchar* file_extension, const wchar* file_name);
const wchar* get_name_from_path_name (wchar* file_name, const wchar* file_path_name);
const wchar* get_path_from_path_name (wchar* file_path, const wchar* file_path_name);
const wchar* add_path_to_file_name (const wchar* file_path, const wchar* file_name);
extern wchar default_file_path[];


/* Note: none of the below provide an error message on failure. TODO: is this fine? */
/* Important, the call: { *filecontent_ptr = realloc(*filecontent_ptr, contentSize); } is used */

bool file_to_array (const  char* filename,  char** filecontent_ptr, size_t *contentSize_ptr);
bool Openfile      (const wchar* fileName, wchar** fileContent_ptr, size_t *contentSize_ptr);

bool array_to_file (const  char* filename, const  char* filecontent, size_t contentSize);
bool Savefile      (const wchar* fileName, const wchar* fileContent, size_t contentSize);

bool file_is_modified (const wchar* fileName);


#endif

