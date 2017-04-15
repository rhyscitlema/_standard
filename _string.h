#ifndef __STRING_H
#define __STRING_H
/*
    _string.h
*/

#include <string.h>
#include "_strfun.h"

mchar* errorMessage();

#define ErrStr0 (errormessage+1000)
#define ErrStr1 (errormessage+2000)
#define ErrStr2 (errormessage+3000)



const mchar* sgets2 (const mchar* input, mchar** output);
const lchar* sgets3 (const lchar* input, lchar** output);

const lchar* lchar_next (const lchar* lstr);
const lchar* lchar_goto (const lchar* lstr, int offset);

/* if file==NULL then the file lchar structure member is ignored */
void set_line_coln_file (lchar* lstr, int first_line, int first_coln, const mchar* file);

void string_c_clean(); // to free memory, best called upon software exit



#define CHRtoHEX(c) ((c>='0' && c<='9') ? ((c)-'0'+0x0) : \
                     (c>='A' && c<='F') ? ((c)-'A'+0xA) : \
                     (c>='a' && c<='f') ? ((c)-'a'+0xa) : -1)

#define HEXtoCHR(c) (((c)>=0x0 && (c)<=0x9) ? (c)-0x0+'0' : \
                     ((c)>=0xA && (c)<=0xF) ? (c)-0xA+'A' : 0)

static inline bool isSpace (mchar c) { return  (c==' ' || c=='\t' || c=='\r' || c=='\n'); }
static inline bool isDigit (mchar c) { return  (c>='0' && c<='9'); }
static inline bool isHexad (mchar c) { return ((c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f')); }
static inline bool isAlpNu (mchar c) { return ((c>='0' && c<='9') || (c>='A' && c<='Z') || (c>='a' && c<='z')); }

bool isOpenedBracket (mchar c);
bool isClosedBracket (mchar c);
mchar OpenedToClosedBracket (mchar c);
mchar ClosedToOpenedBracket (mchar c);



const mchar* pcn_to_chr_22 (mchar** output_ptr, const mchar* input); // Partly Code Number to Character
const mchar* chr_to_pcn_22 (mchar** output_ptr, const mchar* input); // Character to Partly Code Number
const mchar* chr_to_fcn_22 (mchar** output_ptr, const mchar* input); // Character to Fully Code Number
bool set_pif_cn (const mchar* str); // Set Partly is Fully Code Number
/* return false if input string is invalid */



// IMPORTANT: The OS crashes on a call
// to sprintf(), so use sprintf1() instead.
#ifndef sprintf1
#include <stdio.h>
#define sprintf1 sprintf
#endif

mchar* sprintf2 (mchar* output, const mchar* format, ...);

void set_message (mchar* output, const mchar* format, const lchar* lstr, ...);



/*
    The below is defined only in libmfet.
    It is a once-off evaluation.
    Returns non-NULL on success.
    Typical usage is:

    const value* result = mfet_parse_and_evaluate(input_string, NULL);

    if(result==NULL) puts2(errorMessage());

    else if(VstToStr(result, output_string, 4, -1, -1, -1)) // see _math.h

         puts2(output_string); // output_string contains the VstToStr

    else puts2(output_string); // output_string contains the error message
*/
extern const value* mfet_parse_and_evaluate (
    const mchar* mfet_string,   /* string to be parsed and evaluated */
    const mchar* source_name,   /* source of mfet_string, if NULL then "input" is used */
    const value* result_str);   /* the 'expected' result structure, NULL for any */

#endif
