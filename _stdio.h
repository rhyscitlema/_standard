#ifndef __STDIO_H
#define __STDIO_H
/*
	_stdio.h
*/

#include "_stddef.h"

#define MAX_NAME_LEN  255 /* = maximum size of file name */
#define MAX_PATH_LEN 4095 /* = maximum size of file path */


extern void wait(); /* wait for user to press Enter on Command Line Interface */

/* the below are explicitly provided by the platform-dependent code */
extern void         user_alert   (const_Str2 title, const_Str2 message);
extern bool         user_confirm (const_Str2 title, const_Str2 message);
extern const wchar* user_prompt  (const_Str2 title, const_Str2 message, const wchar* entry);


/* print character to default standard output */
void putc2 (wchar chr);

/* put string using calls to putc2(chr) */
void puts1 (const_Str1 s);
void puts2 (const_Str2 s);
void puts3 (const_Str3 s);
void puts3LC (const_Str3 s);


/* return beginning of result string */
const_Str2 get_extension_from_name (const_Str2 filename);
const_Str2 get_name_from_path_name (const_Str2 pathname);
const_Str2 get_path_from_path_name (const_Str2 pathname, Str2 out);
const_Str2 add_path_to_file_name   (const_Str2 filename, Str2 out);
extern Str2 default_file_path();


/* On success, return file content as VT_ARRRAY
*  On failure, return error message as VT_MESSAGE
*/
value FileOpen1 (const_Str1 filename, value stack);
value FileOpen2 (const_Str2 filename, value stack);


/* File content to save = y = vPrev(stack)
*  On success, return setBool(y, true)
*  On failure, return setError(y, message)
*/
value FileSave1 (const_Str1 filename, value stack);
value FileSave2 (const_Str2 filename, value stack);


bool file_modified (const_Str2 filename);

#endif

