#ifndef __STDIO_H
#define __STDIO_H
/*
    _stdio.h
*/

#include "_stddef.h"

#define MAX_PATH_SIZE 300


void wait();

/* these 2 are provided by the platform */
void wait_for_user_first   (const mchar* title, const mchar* message);
bool wait_for_confirmation (const mchar* title, const mchar* message);


/* put character in default standard output */
void putc2 (mchar chr);

/* put string using calls to putc2(chr) */
void puts1 (const  char* str);
void puts2 (const mchar* str);
void puts3 (const lchar* str);
void puts2S (const mchar* str, size_t length);
void puts3LC (const lchar* str);


const mchar* get_extension_from_name (mchar* file_extension, const mchar* file_name);
const mchar* get_name_from_path_name (mchar* file_name, const mchar* file_path_name);
const mchar* get_path_from_path_name (mchar* file_path, const mchar* file_path_name);
const mchar* add_path_to_file_name (const mchar* file_path, const mchar* file_name);
extern mchar default_file_path[];


/* Note: none of the below provide an error message on failure. TODO: is this fine? */
/* Important, the call: { *filecontent_ptr = realloc(*filecontent_ptr, contentSize); } is used */

bool file_to_array (const  char* filename,  char** filecontent_ptr, size_t *contentSize_ptr);
bool Openfile      (const mchar* fileName, mchar** fileContent_ptr, size_t *contentSize_ptr);

bool array_to_file (const  char* filename, const  char* filecontent, size_t contentSize);
bool Savefile      (const mchar* fileName, const mchar* fileContent, size_t contentSize);

bool file_is_modified (const mchar* fileName);


#endif

