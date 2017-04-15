#ifndef _VALUE_H
#define _VALUE_H
/*
    value.h
*/

#include "_stddef.h"

// mix value types, used by switch statements
#define MIX(n,m) (((n)<<8)+(m))

static inline enum ValueType getType(value v) { return v.type; }

static inline Notval getNotval(value v) { return v.msg; }
static inline Poiter getPoiter(value v) { return v.ptr; }
static inline Septor getSeptor(value v) { return v.sep; }
static inline String getString(value v) { return v.str; }
static inline SmaInt getSmaInt(value v) { return v.si; }
static inline SmaRat getSmaRat(value v) { return v.sr; }
static inline SmaFlt getSmaFlt(value v) { return v.sf; }

static inline value setNotval(Notval msg) { value v; v.type = aNotval; v.msg = msg; return v; }
static inline value setPoiter(Poiter ptr) { value v; v.type = aPoiter; v.ptr = ptr; return v; }
static inline value setSeptor(Septor sep) { value v; v.type = aSeptor; v.sep = sep; return v; }
static inline value setString(String str) { value v; v.type = aString; v.str = str; return v; }
static inline value setSmaInt(SmaInt si ) { value v; v.type = aSmaInt; v.si  = si ; return v; }
static inline value setSmaRat(SmaRat sr ) { value v; v.type = aSmaRat; v.sr  = sr ; return v; }
static inline value setSmaFlt(SmaFlt sf ) { value v; v.type = aSmaFlt; v.sf  = sf ; return v; }

static inline value setSepto2(long len, long cols) { value v; v.type = aSeptor; v.sep.len = len; v.sep.cols = cols; return v; }
static inline value setSmaRa2(SmaInt nume, SmaInt deno) { value v; v.type = aSmaRat; v.sr.nume = nume; v.sr.deno = deno; return v; }
extern value setSmaCo2(SmaFlt Re, SmaFlt Im);

static inline bool IsSmaInt(enum ValueType t) { return (t==aSmaInt); }
static inline bool IsSmaRat(enum ValueType t) { return (t==aSmaRat || IsSmaInt(t)); }
static inline bool IsSmaFlt(enum ValueType t) { return (t==aSmaFlt || IsSmaRat(t)); }
static inline bool IsSmaCom(enum ValueType t) { return (t==aSmaCom || IsSmaFlt(t)); }
static inline bool IsBigInt(enum ValueType t) { return (t==aBigInt || IsSmaInt(t)); }
static inline bool IsBigRat(enum ValueType t) { return (t==aBigRat || IsSmaRat(t) || IsBigInt(t)); }
static inline bool IsBigFlt(enum ValueType t) { return (t==aBigFlt || IsSmaFlt(t) || IsBigRat(t)); }
static inline bool IsBigCom(enum ValueType t) { return (t==aBigCom || IsSmaCom(t) || IsBigFlt(t)); }
static inline bool IsNumber(enum ValueType t) { return (t==aNumber || IsBigCom(t)); }

#define isPoiter(val) (getType(val)==aPoiter)
#define isSeptor(val) (getType(val)==aSeptor)
#define isString(val) (getType(val)==aString)
#define isSmaInt(val) (getType(val)==aSmaInt) // TODO: remove these

/* count number of total values in value structure */
static inline long VST_LEN(const value* vst)
{
    enum ValueType type;
    value v;
    if(!vst) return 0;
    v = *vst;
    type = getType(v);
    return type==0 ? 0 :
           type!=aSeptor ? 1 :
           getSeptor(v).len ;
}


value* value_alloc (long size);
void value_free (value* vst);

void vst_copy (value* out, const value* in, long len);

static inline value* value_copy (const value* in)
{   long len;
    value* out;
    if(!in) return NULL;
    len = VST_LEN(in);
    out = value_alloc(len);
    vst_copy(out, in, len);
    return out;
}

static inline void avaluecpy (value** out, const value* in)
{ if(out) { value_free(*out); *out = value_copy(in); } }

static inline void vst_shift (register value* out, register const value* in)
{   register const value* end = in + VST_LEN(in);
    while(in!=end) *out++ = *in++;
}


const char* vst_to_str (const value* vst);

int value_compare (const value* in1, const value* in2);


/* valueSt_compare() =
 * 00b if in1 == in2 // one-to-one
 * 01b if in1 >  in2 // many-to-one
 * 10b if in1 <  in2 // one-to-many
 * 11b if in1 <> in2 // many-to-many
 *111b if in1 != in2 // no matching
 */
int valueSt_compare (const value* in1, const value* in2);

bool valueSt_matrix_getSize (const value* vst, int *rows, int *cols);
void valueSt_matrix_setSize (value* vst, int rows, int cols);
void valueSt_from_floats (value* vst, int rows, int cols, const SmaFlt* floats);


#define MVE(A,i,cols) ( A+1 + (i)*((cols==1) ? 1 : (1+cols)) )

#define MVA(A,i,j,cols) *( MVE(A,i,cols) + ((cols==1) ? 0 : (1+j)) )

#define MSIZE(rows, cols) ((cols==1) ? (1+rows) : (1+rows*(1+cols)))


extern const value *VST11,
                   *VST21,
                   *VST31,
                   *VST41,
                   *VST61,
                   *VST33;
void SET_VSTXX ();

bool valueSt_get_position (int* position, const value* parameter, const lchar* lstr);


#endif

