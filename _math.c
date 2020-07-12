/*
    _math.c
    TODO: before introducing arrays of integers and the likes, first do something about the very confusing "string as an array of characters". Maybe treat these as special arrays, by treating some operations like addition and logical_not differently. Then provide a string-only array-to-vector operation.
*/

#include <math.h>
#include <complex.h>
#include "_string.h"
#include "_stdio.h"
#include "_texts.h"
#include "_math.h"



typedef double complex SmaCom;

#ifndef COMPLEX
#define COMPLEX 0

static SmaCom _cpow   (SmaCom n, SmaCom m) { return n+m; }
static SmaCom _csqrt  (SmaCom n) { return n; }
static SmaCom _cexp   (SmaCom n) { return n; }
static SmaCom _clog   (SmaCom n) { return n; }

static SmaCom _ccos   (SmaCom n) { return n; }
static SmaCom _csin   (SmaCom n) { return n; }
static SmaCom _ctan   (SmaCom n) { return n; }
static SmaCom _cacos  (SmaCom n) { return n; }
static SmaCom _casin  (SmaCom n) { return n; }
static SmaCom _catan  (SmaCom n) { return n; }

static SmaCom _ccosh  (SmaCom n) { return n; }
static SmaCom _csinh  (SmaCom n) { return n; }
static SmaCom _ctanh  (SmaCom n) { return n; }
static SmaCom _cacosh (SmaCom n) { return n; }
static SmaCom _casinh (SmaCom n) { return n; }
static SmaCom _catanh (SmaCom n) { return n; }

static SmaCom _cproj  (SmaCom n) { return n; }

static SmaFlt ccabs (SmaCom x)
{ SmaFlt r = creal(x),
         i = cimag(x);
  return sqrt(r*r+i*i);
}

#else

#define _cpow   cpow
#define _csqrt  csqrt
#define _cexp   cexp
#define _clog   clog

#define _ccos   ccos
#define _csin   csin
#define _ctan   ctan
#define _cacos  cacos
#define _casin  casin
#define _catan  catan

#define _ccosh  ccosh
#define _csinh  csinh
#define _ctanh  ctanh
#define _cacosh cacosh
#define _casinh casinh
#define _catanh catanh

#define _cproj  cproj
#define ccabs   cabs

#endif



static inline SmaCom getSmaCom (const_value v) { return *(SmaCom*)(v+1); }
value setSmaCom (value v, SmaCom si)
{
    assert(v!=NULL);
    v[0] = (VAL_NUMBER<<28) | 3;
    *(SmaCom*)(v+1) = si;
    v[5] = (VAL_OFFSET<<28) | 5;
    return v+6;
}
value setSmaCom2 (value v, SmaFlt Re, SmaFlt Im) { return setSmaCom(v, Re+Im*I); }

#define OPER_FAIL_(code) v = setMessage(y, code, 0, 0)
#define OPER_FAIL OPER_FAIL_(Operand_Not_Supported);

#define NOT_AVAILABLE { return setError(vPrev(v), L"Operation not yet available!"); }


#define _init_n \
    value y = vPrev(v); \
    const_value n = vGet(y); \
    uint32_t a = value_type(n); \
    if(a==VAL_MESSAGE) return v; \

#define _init_n_m \
    value y = vPrev(v); \
    const_value m = vGet(y); \
    uint32_t b = value_type(m); \
    if(b==VAL_MESSAGE) return v; \
    value e = y; \
    y = vPrev(y); \
    const_value n = vGet(y); \
    uint32_t a = value_type(n); \
    if(a==VAL_MESSAGE) return e; \

#define init_n(call) _init_n \
    if(a==VAL_VECTOR) return vcopy(y, OPER_TYPE1(v,n,call)); \

#define init_n_m(call) _init_n_m \
    if(a==VAL_VECTOR || b==VAL_VECTOR) return vcopy(y, OPER_TYPE2(v,n,m,call)); \

// mix value types, used by switch statements
#define MIX(n,m) ((n<<8)|m)


static value OPER_TYPE1 (value v, const_value n, value (*CALL)(value v))
{
    assert((*n>>28)==VAL_VECTOR);
    uint32_t i, count = VEC_LEN(*n);
    n += 2; // skip vector header
    value y = v;
    v += 2; // reserve space for vector header
    for(i=0; i<count; i++)
    {
        CALL(setRef(v, n));
        if(*v>>28) v = vNEXT(v);
        else return v;
        n = vNext(n);
    }
    setVector(y, count, v-y-2);
    return y;
}


static value OPER_TYPE2 (value v, const_value n, const_value m, value (*CALL)(value v))
{
    uint32_t i, count;
    uint32_t a=*n, b=*m;

    if((a>>28)==VAL_VECTOR){
         count = VEC_LEN(a);
         if((b>>28)==VAL_VECTOR){
             if((VEC_LEN(b)) != count){
                setMessage(v, Operands_DoNot_Match, 0, 0);
                return v;
             }
         }
         else b=0;
    }
    else if((b>>28)==VAL_VECTOR){
        count = VEC_LEN(b);
        a=0;
    }
    else { assert(false); return v; }

    if(a) n += 2; // skip vector header
    if(b) m += 2;
    value y = v;
    v += 2; // reserve space for vector header

    for(i=0; i<count; i++)
    {
        CALL(setRef(setRef(v, n), m));
        if(*v>>28) v = vNEXT(v);
        else return v;
        if(a) n = vNext(n);
        if(b) m = vNext(m);
    }
    setVector(y, count, v-y-2);
    return y;
}



//---------------------------------------------------------------

value combine (value v)
{
    _init_n_m
    long sn = vSize(n);
    long sm = vSize(m);
    if(a==VAL_VECTOR) { a = VEC_LEN(*n); n+=2; sn-=2; } else a=1;
    if(b==VAL_VECTOR) { b = VEC_LEN(*m); m+=2; sm-=2; } else b=1;
    memcpy(v+2   , n, sn*sizeof(*n));
    memcpy(v+2+sn, m, sm*sizeof(*m));
    setVector(v, a+b, sn+sm);
    return vcopy(y, v);
}


value _size (value v)
{
    _init_n
    uint32_t rows=1, cols=1;

    if(a==VAL_ARRRAY){ // if an array but not a string
        rows = *n & 0x0FFFFFFF;
        assert(a!=VAL_ARRRAY); // not allowed for now
    }
    else if(a==VAL_VECTOR)
    {
        rows = VEC_LEN(*n);
        n += 2; // skip vector header

        if(rows==0) cols=0;
        else{
            uint32_t j = *vGet(n);
            cols = ((j>>28)==VAL_VECTOR) ? VEC_LEN(j) : 1;
            n = vNext(n);

            uint32_t i;
            for(i=1; i<rows; i++)
            {
                j = *vGet(n);
                j = ((j>>28)==VAL_VECTOR) ? VEC_LEN(j) : 1;
                if(j != cols) { cols=0; break; }
                n = vNext(n);
            }
        }
    }
    y = setSmaInt(y, rows);
    y = setSmaInt(y, cols);
    return tovector(y, 2);
}


static uint32_t get_span (const_value n)
{
    uint32_t c = *n;
    if((c>>28)==VAL_VECTOR){
        c = VEC_LEN(c); // get vector length
        n += 2; // skip vector header
    } else c=1;

    uint32_t r = 0;
    while(c--)
    {
        const_value t = vGet(n);
        if((*t>>28)==VAL_VECTOR)
            r += get_span(t); // recursive call
        else r++;
        n = vNext(n);
    }
    return r;
}
value _span (value v) { _init_n return setSmaInt(y, get_span(n)); }


static value get_sum (const_value n, value v)
{
    uint32_t c = *n;
    if((c>>28)==VAL_VECTOR){
        c = VEC_LEN(c); // get vector length
        if(!c && !*(v-1)) return vcopy(v-1, n);
        n += 2; // skip vector header
    } else c=1;

    while(c--)
    {
        const_value t = vGet(n);
        if((*t>>28)==VAL_VECTOR)
            v = get_sum(t, v); // recursive call
        else if(*(v-1)==0) // if first time
            v = vcopy(v-1, t); // then just copy
        else v = _add(setRef(v, t));
        n = vNext(n);
    }
    return v;
}
value _sum (value v) { _init_n *v++=0; return vpcopy(y, get_sum(n, v)); }


static const_value _max_min (const_value n, value v, value (*CALL)(value v))
{
    uint32_t c = *n;
    if((c>>28)==VAL_VECTOR){
        c = VEC_LEN(c); // get vector length
        n += 2; // skip vector header
    } else c=1;

    const_value w, r = NULL;
    while(c--)
    {
        w = vGet(n);
        n = vNext(n);
        if((*w>>28)==VAL_VECTOR)
            w = _max_min(w, v, CALL); // recursive call
        if(r==NULL) r=w;
        else if(w)
        {
            CALL(setRef(setRef(v, r), w));
            if(isBool(v) && (*v & 1)) r = w;
        }
    }
    return r;
}
value _max (value v) { _init_n return vcopy(y, _max_min(n, v, lessThan   )); }
value _min (value v) { _init_n return vcopy(y, _max_min(n, v, greaterThan)); }



//---------------------------------------------------------------

value vStrLen (value v)
{
    init_n(vStrLen)
    if(a!=aString) OPER_FAIL
    else v = setSmaInt(y, VEC_LEN(*n)-1); // -1 to exclude '\0'
    return v;
}

value toStr (value v)
{
    _init_n
    value e = check_arguments(y,*n,2); if(e) return e;
    n += 2; // skip vector header
    const_value m = vNext(n);

    int info_base=0;
    int t_places=-1;
    int d_places=-1;

    n = vGet(n);
    a = *n;
    if((a>>28)==VAL_VECTOR){
        a = VEC_LEN(a); // get vector length
        n += 2; // skip vector header
    } else a=1;

    while(true)
    {
        if(value_type(n)!=aSmaInt) break;
        info_base = (int)getSmaInt(n);
        if(--a==0) break;

        n = vNext(n);
        if(value_type(n)!=aSmaInt) break;
        t_places = (int)getSmaInt(n);
        if(--a==0) break;

        n = vNext(n);
        if(value_type(n)!=aSmaInt) break;
        d_places = (int)getSmaInt(n);
        --a; break;
    }
    if(a)
    {
        const_Str2 out =
            L"First argument must have integers only:\r\n"
            L" (<base> [, <total_places> [, <decimal_places>]]).";
        v = setMessage(y, 0, 1, &out);
    }
    else v = vpcopy(y, VstToStr(setRef(v,m), info_base, t_places, d_places));
    return v;
}


value toNum (value v)
{
    NOT_AVAILABLE
}


value toRat (value v)
{
    NOT_AVAILABLE
}

value toFlt (value v)
{
    init_n(toFlt)
    switch(a)
    {
        case aSmaInt: v = setSmaFlt(y, getSmaInt(n)); break;
        case aSmaFlt: v = setSmaFlt(y, getSmaFlt(n)); break; // TODO: this line is due to setRef() in OPER_TYPE1
        case aSmaCom: v = setSmaFlt(y, creal(getSmaCom(n))); break;
        default: OPER_FAIL break;
    }
    return v;
}


static inline int aMODb (int a, int b) { a = a % b; if(a<0) a+=b; return a; }

value indexing (value v)
{
    _init_n_m

    if(b==VAL_VECTOR){
        b = VEC_LEN(*m); // get count of vector elements
        m += 2; // skip vector header
    } else b=1;

    int i=b, start=0, stop=0, incre=0;
    do{
        if(value_type(m)!=aSmaInt) break;
        start = (int)getSmaInt(m);
        if(--i==0) break;

        m = vNext(m);
        if(value_type(m)!=aSmaInt) break;
        stop = (int)getSmaInt(m);
        if(--i==0) break;
        else incre=stop;

        m = vNext(m);
        if(value_type(m)!=aSmaInt) break;
        stop = (int)getSmaInt(m);
        --i; break;
    }while(0);

    do{
        if(i) { v = setError(y, L"Indexing argument must be at most 3 integers."); break; }

        i = a==aString;
        if(i || a==VAL_VECTOR){
            a = VEC_LEN(*n); // get vector length
            n += 2; // skip vector header
        } else a=1;
        if(i) a-=1; // -1 so to skip '\0'

        start = aMODb(start, a+1);
        stop  = aMODb(stop , a+1);
        if(b==1)
        {
            if(i) // if indexing a null-terminated array of characters
                v = setCharac(y, WCHAR(n)[start]);
            else if(start==a) // if index == length of vector
            {
                setVector(y, 0, 0);
                v = setOffset(y+2, 2);
            }
            else{
                while(start--) n = vNext(n);
                v = vcopy(y, vGet(n));
            }
            break;
        }
        if(b==2) // if two indexing argument provided
            incre = 1; // then set default increment
        if(incre==0)
        { v = setError(y, L"The second argument used for incrementing is invalid."); break; }

        if(i) // if indexing a null-terminated array of characters
        {
            wchar* w = (wchar*)(v+2);
            while(true)
            {
                if(incre>0) { if(start>stop) break; }
                else { if(start<stop) break; }
                *w++ = WCHAR(n)[start];
                start += incre;
            }
            *w = '\0';
            v = vpcopy(y, onSetStr2(v, w));
        }
        else if(incre>0)
        {
            int r;
            r = 0; // = number of rows of result vector
            i = 0; // current position in source vector
            v = y+2;
            while(start<=stop)
            {
                if(start!=a)
                {
                    while(i != start)
                    { n = vNext(n); i++; }
                    const_value t = vGet(n);
                    long size = vSize(t);
                    memmove(v, t, size*sizeof(*t));
                    v += size; r++;
                }
                start += incre;
            }
            setVector(y, r, v-y-2);
            v = setOffset(v, v-y);
        }
        else{
            v = setError(y, L"Negative incrementation not yet available.");
            // TODO: NOTE: no vectored indexing, must use range() then...?!
        }
    }while(0);
    return v;
}


value _range (value v)
{
    _init_n
    value e = check_arguments(y,*n,3); if(e) return e;
    n += 2; // skip vector header

    const_value start = n;          // get 1st element
    n = vNext(n);                   // get 2nd element
    const_value stop = vNext(n);    // get 3rd element

    n = vGet(n);
    uint32_t b = *n; // n points at the incrementing value
    if((b>>28)==VAL_VECTOR){ // if a vector
        b = VEC_LEN(b); // get vector length
        n += 2; // skip vector header
    } else b=1;
    if(b==0) return vcopy(y,start);

    if((size_t)v % sizeof(void*)) v++; // get to aligned void**
    const_value* incre = (const_value*)v;
    v = (value)(incre+b);

    incre[0] = start;
    v = setRef(v, start);
    for(a=1; a<b; a++)
    {
        incre[a] = v;
        v = _add(setRef(setRef(v, incre[a-1]), n));
        n = vNext(n);
    }
    const_value sum = v;
    v = _add(setRef(setRef(v, incre[a-1]), n));
    v = _sub(setRef(v, start));
    if(VERROR(v)) return vpcopy(y,v);
    value w = v;

    lessThan(setSmaInt(setRef(v, sum), 0));
    if(!isBool(v)) v=0; // if result is not a boolean
    else{
        value (*check)(value) = (*v & 1) ? lessThan : greaterThan;

        v += 2; // reserve space for vector header
        v = vcopy(v, start); // deal with case a=0
        for(a=1; ; a++)
        {
            v = unOffset(v);
            if(a>100000 && !wait_for_confirmation(L"Warning",
                L"Vector length is greater than 100000. Continue?"))
                return setError(y, L"Vector length is greater than 100000.");
            e=v;

            v = setRef(v, incre[a%b]);
            v = setSmaInt(v, a/b);
            v = setRef(v, sum);
            v = _add(_mul(v));

            check(setRef(setRef(v, e), stop));
            if(!isBool(v)) { v=0; break; }
            if(*v & 1) { v=e; break; }
        }
    }
    if(v==0) v = setError(y, L"At least one argument is not comparable.");
    else{
        setVector(w, a, v-w-2);
        v = vcopy(y, w);
    }
    return v;
}


value _vector (value v)
{
    _init_n
    value e = check_arguments(y,*n,3); if(e) return e;
    n += 2; // skip vector header

    const_value start = n;          // get 1st element
    n = vNext(n);                   // get 2nd element
    const_value stop = vNext(n);    // get 3rd element

    n = vGet(n);
    uint32_t b = *n; // n points at the incrementing value
    if((b>>28)==VAL_VECTOR){ // if a vector
        b = VEC_LEN(b); // get vector length
        n += 2; // skip vector header
    } else b=1;
    if(b==0) return vcopy(y,start);

    if((size_t)v % sizeof(void*)) v++; // get to aligned void**
    const_value* incre = (const_value*)v;
    v = (value)(incre+b);

    incre[0] = start;
    v = setRef(v, start);
    for(a=1; a<b; a++)
    {
        incre[a] = v;
        v = _add(setRef(setRef(v, incre[a-1]), n));
        n = vNext(n);
    }
    const_value sum = v;
    v = _add(setRef(setRef(v, incre[a-1]), n));
    v = _sub(setRef(v, start));
    if(VERROR(v)) return vpcopy(y,v);
    value w = v;

    long count=0;
    if(value_type(stop)!=aSmaInt || (count = getSmaInt(stop))<=0)
        v = setError(y, TWST(Vector_Length_Invalid));

    else if(count>100000 && !wait_for_confirmation(L"Warning",
        L"Vector length is greater than 100000. Continue?"))
        return setError(y, L"Vector length is greater than 100000.");
    else
    {
        v += 2; // reserve space for vector header
        v = vcopy(v, start); // deal with case a=0
        for(a=1; a<count; a++)
        {
            v = unOffset(v);
            v = setRef(v, incre[a%b]);
            v = setSmaInt(v, a/b);
            v = setRef(v, sum);
            v = _add(_mul(v));
        }
        assert(!VERROR(v));
        v = unOffset(v);
        setVector(w, a, v-w-2);
        v = vcopy(y, w);
    }
    return v;
}


value fullfloor (value v)
{
    NOT_AVAILABLE
/*
{
    long i, n, m, len;
    const value *N, *A;
    value y, sum, numerator;

    Expression* expression = eca.expression;
    eca.expression = expression->headChild;
    if(!expression_evaluate(eca)) return 0;

    Str2 errmsg = eca.garg->message;
    if(!check_first_level(eca.stack, 2, errmsg, expression->name)) return 0;

    N = eca.stack+1;          // get vector of values of n
    len = VST_SIZE(N);

    A = N + len;          // get the vector A
    m = VST_SIZE(A);
    if(m>1) { m--; A++; } // skip ',' separator
    //TODO: check if A is a single value or vector

    sum = setSmaInt(0);
    for(i=0; i<m; i++) sum = add(sum, A[i]);

    if(getSmaInt(logical_not(sum))) // if sum==0
    {
        for(n=len; n!=0; n--, N++, eca.stack++)
        {
            y = *N;
            if(!isVector(y))
                y = setSmaInt((SmaInt)0x8000000000000000);
            *eca.stack = y;
        }
    }
    else
    {
        for(n=len; n!=0; n--, N++, eca.stack++)
        {
            y = *N;
            if(!isVector(y))
            {
                numerator = y;
                y = setSmaInt(0);
                for(i=1; i<=m; i++)
                {
                    y = add(y, idivide(numerator, sum));
                    numerator = add (numerator, A[m-i]);
                }
            }
            *eca.stack = y;
        }
    }
    return 1;
}
*/
}


value getprimes (value v)
{
    NOT_AVAILABLE
}


value _srand (value v)
{
    NOT_AVAILABLE
}


value _rand (value v)
{
    NOT_AVAILABLE
}


value dotproduct (value v)
{
    NOT_AVAILABLE
}


//---------------------------------------------------------------

value _pos (value v)
{
    init_n(_neg)
    switch(a)
    {
        case aSmaInt: v = setSmaInt(y, +getSmaInt(n)); break;
        case aSmaFlt: v = setSmaFlt(y, +getSmaFlt(n)); break;
        case aSmaCom: v = setSmaCom(y, +getSmaCom(n)); break;
        default: OPER_FAIL break;
    }
    return v;
}

value _neg (value v)
{
    init_n(_neg)
    switch(a)
    {
        case aSmaInt: v = setSmaInt(y, -getSmaInt(n)); break;
        case aSmaFlt: v = setSmaFlt(y, -getSmaFlt(n)); break;
        case aSmaCom: v = setSmaCom(y, -getSmaCom(n)); break;
        default: OPER_FAIL break;
    }
    return v;
}


static const_value* set_matrix_pointers (value v, const_value n, int rows, int cols)
{
    if((size_t)v % sizeof(void*)) v++; // get to aligned void**
    const_value* p = (const_value*)v;
    int i, j;
    n += 2; // skip vector header
    for(i=0; i<rows; i++)
    {
        const_value m = vGet(n);
        if(cols>1) m += 2; // skip vector header
        for(j=0; j<cols; j++)
        {
            *p++ = m;
            m = vNext(m);
        }
        n = vNext(n);
    }
    return (const_value*)v;
}


value transpose (value v)
{
    _init_n
    if(a!=VAL_VECTOR) return v;
    int i, j, rows, cols;
    const_value t;

    _size(setRef(v, n));
    t = v+2; // +2 to skip vector header
    rows = getSmaInt(t);
    t = vNext(t);
    cols = getSmaInt(t);
    if(cols==0) return OPER_FAIL_(Operand_IsNot_Matrix);

    const_value* p = set_matrix_pointers(v,n,rows,cols);
    v = (value)(p + rows*cols);
    value w = v+2;
    for(j=0; j<cols; j++)
    {
        value u=0;
        if(rows>1)
        {
            u=w;
            w+=2;
        }
        for(i=0; i<rows; i++)
            w = unOffset(vcopy(w, p[i*cols+j]));
        if(rows>1)
            setVector(u, rows, w-u-2);
    }
    setVector(v, cols, w-v-2);
    return vcopy(y, v);
}


value __mul (value v)
{
    _init_n_m
    if(a!=VAL_VECTOR || b!=VAL_VECTOR) return _mul(v);
    int i, j, k, rowN, colN, rowM, colM;
    const_value t;

    _size(setRef(v, n));
    t = v+2; // +2 to skip vector header
    rowN = getSmaInt(t);
    t = vNext(t);
    colN = getSmaInt(t);

    _size(setRef(v, m));
    t = v+2; // +2 to skip vector header
    rowM = getSmaInt(t);
    t = vNext(t);
    colM = getSmaInt(t);

    const_Str2 argv[5];
    enum ID_TWST error=0;

    int argc=1;
    argv[argc++] = TWSF(Oper_mul2);
         if(colN==0) error = Left_IsNot_Matrix;
    else if(colM==0) error = Right_IsNot_Matrix;

    else if(rowN==3 && colN==1 && rowM==3 && colM==1)
    {
        // do vector multiplication
        const_value _n[3], _m[3];
        n+=2; _n[0]=n; for(i=1; i<3; i++) { n = vNext(n); _n[i]=n; }
        m+=2; _m[0]=m; for(j=1; j<3; j++) { m = vNext(m); _m[j]=m; }

        v = _mul(setRef(setRef(v, _n[1]), _m[2]));
        v = _mul(setRef(setRef(v, _n[2]), _m[1]));
        v = _sub(v);
        v = _mul(setRef(setRef(v, _n[2]), _m[0]));
        v = _mul(setRef(setRef(v, _n[0]), _m[2]));
        v = _sub(v);
        v = _mul(setRef(setRef(v, _n[0]), _m[1]));
        v = _mul(setRef(setRef(v, _n[1]), _m[0]));
        v = _sub(v);

        v = tovector(v,3);
        v = vpcopy(y, v);
    }
    else if(colN == rowM) // do matrix multiplication
    {
        const_value* pn = set_matrix_pointers(v,n,rowN,colN);
        v = (value)(pn + rowN*colN);
        const_value* pm = set_matrix_pointers(v,m,rowM,colM);
        v = (value)(pm + rowM*colM);
        value w = v+2;
        for(i=0; i<rowN; i++)
        {
            value u = w;
            if(colM!=1) w+=2;
            for(j=0; j<colM; j++)
            {
                w = setSmaInt(w,0);
                for(k=0; k<colN; k++)
                {
                    w = setRef(w, pn[i*colN+k]);
                    w = setRef(w, pm[k*colM+j]);
                    w = _add(_mul(w));
                }
                w = unOffset(w);
            }
            if(colM!=1) setVector(u, colM, w-u-2);
        }
        if(rowN==1 && colM==1) v+=2;
        else setVector(v, rowN, w-v-2);
        v = vcopy(y, v);
    }
    else // invalid matrix multiplication
    {
        argc=1;
        argv[argc++] = TIS2(0, rowN);
        argv[argc++] = TIS2(1, colN);
        argv[argc++] = TIS2(2, rowM);
        argv[argc++] = TIS2(3, colM);
        error = MatrixMult_IsInvalid;
    }

    if(error){
        argv[0] = TWST(error);
        v = setMessage(y, error, argc, argv);
    }
    return v;
}


value __div (value v)
{
    _init_n_m
    if(!a) return v; // just to remove unused variable warning
    if(b==VAL_VECTOR) { OPER_FAIL_(Right_IsNot_Single); return v; }
    return _div(v);
}

value power (value v)
{
    _init_n_m
    if(a==VAL_VECTOR) { OPER_FAIL_(Left_IsNot_Single); return v; }
    if(b==VAL_VECTOR) { OPER_FAIL_(Right_IsNot_Single); return v; }
    return _pow(v);
}



//---------------------------------------------------------------

value _add (value v)
{
    _init_n_m

    if(a==aString || b==aString)
    {
        v = VstToStr(setRef(v,n), 0,-1,-1); n = vGetPrev(v);
        v = VstToStr(setRef(v,m), 0,-1,-1); m = vGetPrev(v);
        a = (*n & 0x0FFFFFFF)-1; n+=2;
        b = (*m & 0x0FFFFFFF)-1; m+=2; // -1 to exclude '\0'
        wchar* w = (wchar*)(y+2);
        memcpy(w  , n, a*sizeof(wchar));
        memcpy(w+a, m, b*sizeof(wchar));
        *(w+a+b) = '\0';
        return onSetStr2(y, w+a+b);
    }
    if(a==VAL_VECTOR || b==VAL_VECTOR) return vcopy(y,OPER_TYPE2(v,n,m,_add));
    if(b==aSmaInt && (*n>>28)==VAL_CHARAC) return setCharac(y, (wchar)(getSmaInt(n) + getSmaInt(m)));
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt): v = setSmaInt(y, getSmaInt(n) + getSmaInt(m)); break;
        case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, getSmaInt(n) + getSmaFlt(m)); break;
        case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, getSmaInt(n) + getSmaCom(m)); break;

        case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, getSmaFlt(n) + getSmaInt(m)); break;
        case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, getSmaFlt(n) + getSmaFlt(m)); break;
        case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, getSmaFlt(n) + getSmaCom(m)); break;

        case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, getSmaCom(n) + getSmaInt(m)); break;
        case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, getSmaCom(n) + getSmaFlt(m)); break;
        case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, getSmaCom(n) + getSmaCom(m)); break;

        default: OPER_FAIL break;
    }
    return v;
}


value _sub (value v)
{
    _init_n_m
    if(a==VAL_VECTOR || b==VAL_VECTOR) return vcopy(y,OPER_TYPE2(v,n,m,_sub));
    if(b==aSmaInt && (*n>>28)==VAL_CHARAC) return setCharac(y, (wchar)(getSmaInt(n) - getSmaInt(m)));
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt): v = setSmaInt(y, getSmaInt(n) - getSmaInt(m)); break;
        case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, getSmaInt(n) - getSmaFlt(m)); break;
        case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, getSmaInt(n) - getSmaCom(m)); break;

        case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, getSmaFlt(n) - getSmaInt(m)); break;
        case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, getSmaFlt(n) - getSmaFlt(m)); break;
        case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, getSmaFlt(n) - getSmaCom(m)); break;

        case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, getSmaCom(n) - getSmaInt(m)); break;
        case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, getSmaCom(n) - getSmaFlt(m)); break;
        case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, getSmaCom(n) - getSmaCom(m)); break;

        default: OPER_FAIL break;
    }
    return v;
}


value _mul (value v)
{
    init_n_m(_mul)
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt): v = setSmaInt(y, getSmaInt(n) * getSmaInt(m)); break;
        case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, getSmaInt(n) * getSmaFlt(m)); break;
        case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, getSmaInt(n) * getSmaCom(m)); break;

        case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, getSmaFlt(n) * getSmaInt(m)); break;
        case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, getSmaFlt(n) * getSmaFlt(m)); break;
        case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, getSmaFlt(n) * getSmaCom(m)); break;

        case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, getSmaCom(n) * getSmaInt(m)); break;
        case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, getSmaCom(n) * getSmaFlt(m)); break;
        case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, getSmaCom(n) * getSmaCom(m)); break;

        default: OPER_FAIL break;
    }
    return v;
}


value _div (value v)
{
    init_n_m(_div)
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt): v = setSmaFlt(y, getSmaInt(n) / (SmaFlt)getSmaInt(m)); break;
        case MIX(aSmaInt, aSmaFlt): v = setSmaFlt(y, getSmaInt(n) / getSmaFlt(m)); break;
        case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, getSmaInt(n) / getSmaCom(m)); break;

        case MIX(aSmaFlt, aSmaInt): v = setSmaFlt(y, getSmaFlt(n) / getSmaInt(m)); break;
        case MIX(aSmaFlt, aSmaFlt): v = setSmaFlt(y, getSmaFlt(n) / getSmaFlt(m)); break;
        case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, getSmaFlt(n) / getSmaCom(m)); break;

        case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, getSmaCom(n) / getSmaInt(m)); break;
        case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, getSmaCom(n) / getSmaFlt(m)); break;
        case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, getSmaCom(n) / getSmaCom(m)); break;

        default: OPER_FAIL break;
    }
    return v;
}



/* --------- Exponent and Logarithm Functions ----------- */

value _cbrt (value v)
{
    init_n(_cbrt)
    switch(a)
    {
        case aSmaInt: v = setSmaFlt(y, pow(getSmaInt(n), 1/3.0)); break;
        case aSmaFlt: v = setSmaFlt(v, pow(getSmaFlt(n), 1/3.0)); break;
        case aSmaCom: if(COMPLEX) { v = setSmaCom(y, _cpow(getSmaCom(n), 1/3.0)); break; }
        default: OPER_FAIL break;
    }
    return v;
}


value _sqrt (value v)
{
    int b=0;
    SmaFlt sf=0;
    init_n(_sqrt)
    switch(a)
    {
        case aSmaInt: sf = getSmaInt(n); b=1; break;
        case aSmaFlt: sf = getSmaFlt(n); b=1; break;
        case aSmaCom: if(COMPLEX) { v = setSmaCom(y, _csqrt(getSmaCom(n))); break; }
        default: OPER_FAIL break;
    }
    if(b){
        if(sf>=0) v = setSmaFlt(y, sqrt(sf));
        else v = setSmaCom(y, sqrt(-sf)*I);
    }
    return v;
}


value _log (value v)
{
    int b=0;
    SmaFlt sf=0;
    SmaCom sc=0;
    init_n(_log)
    switch(a)
    {
        case aSmaInt: sf = getSmaInt(n); b=1; break;
        case aSmaFlt: sf = getSmaFlt(n); b=1; break;
        case aSmaCom: if(COMPLEX) { sc = getSmaCom(n); b=2; break; }
        default: OPER_FAIL break;
    }
    if(b==1)
    {
        if(sf>0) v = setSmaFlt(y, log(sf));
        else OPER_FAIL_(Argument_OutOf_Domain);
    }
    else if(b==2)
    {
        if(ccabs(sc)>0) v = setSmaCom(y, _clog(sc));
        else OPER_FAIL_(Argument_OutOf_Domain);
    }
    return v;
}



/* --------- Trigonometric and Hyperbolic Functions ---- */

static value TYPE1_OPR (value v,
                        value (*CALL)(value v),
                        SmaFlt (*SF_CALL)(SmaFlt x),
                        SmaCom (*SC_CALL)(SmaCom x) )
{
    init_n(CALL)
    switch(a)
    {
        case aSmaInt: v = setSmaFlt(y, SF_CALL(getSmaInt(n))); break;
        case aSmaFlt: v = setSmaFlt(y, SF_CALL(getSmaFlt(n))); break;
        case aSmaCom: if(COMPLEX) { v = setSmaCom(y, SC_CALL(getSmaCom(n))); break; }
        default: OPER_FAIL break;
    }
    return v;
}

static value INV_TRIGO (value v,
                        value (*CALL)(value v),
                        SmaFlt (*SF_CALL)(SmaFlt x),
                        SmaCom (*SC_CALL)(SmaCom x) )
{
    int b=0;
    SmaFlt sf=0;
    SmaCom sc=0;
    init_n(CALL)
    switch(a)
    {
        case aSmaInt: sf = getSmaInt(n); b=1; break;
        case aSmaFlt: sf = getSmaFlt(n); b=1; break;
        case aSmaCom: if(COMPLEX) { sc = getSmaCom(n); b=2; break; }
        default: OPER_FAIL break;
    }
    if(b==1)
    {
        if(-1 <= sf && sf <= 1) v = setSmaFlt(y, SF_CALL(sf));
        else OPER_FAIL_(Argument_OutOf_Domain);
    }
    else if(b==2)
    {
        sf = ccabs(sc);
        if(-1 <= sf && sf <= 1) v = setSmaCom(y, SC_CALL(sc));
        else OPER_FAIL_(Argument_OutOf_Domain);
    }
    return v;
}

/*#ifndef acosh
static SmaFlt acosh(SmaFlt x) { return log(x + sqrt(x*x - 1)); }
static SmaFlt asinh(SmaFlt x) { return log(x + sqrt(x*x + 1)); }
static SmaFlt atanh(SmaFlt x) { return (log(1+x) - log(1-x))/2; }
#endif*/

value _exp   (value v) { return TYPE1_OPR (v, _exp, exp, _cexp); }

value _cos   (value v) { return TYPE1_OPR (v, _cos, cos, _ccos); }
value _sin   (value v) { return TYPE1_OPR (v, _sin, sin, _csin); }
value _tan   (value v) { return TYPE1_OPR (v, _tan, tan, _ctan); }

value _cosh  (value v) { return TYPE1_OPR (v, _cosh, cosh, _ccosh); }
value _sinh  (value v) { return TYPE1_OPR (v, _sinh, sinh, _csinh); }
value _tanh  (value v) { return TYPE1_OPR (v, _tanh, tanh, _ctanh); }

value _acos  (value v) { return INV_TRIGO (v, _acos, acos, _cacos); }
value _asin  (value v) { return INV_TRIGO (v, _asin, asin, _casin); }
value _atan  (value v) { return TYPE1_OPR (v, _atan, atan, _catan); }

value _acosh (value v) { return TYPE1_OPR (v, _acosh, acosh, _cacosh); }
value _asinh (value v) { return TYPE1_OPR (v, _asinh, asinh, _casinh); }
value _atanh (value v) { return TYPE1_OPR (v, _atanh, atanh, _catanh); }



/* --------- Mainly For Complex Numbers ---------------- */

#define COMPL_OPR(v, CALL, SR_CALL, SF_CALL, SC_CALL) \
{ \
    init_n(CALL) \
    switch(a) \
    { \
        case aSmaInt: v = setSmaInt(y, SF_CALL(getSmaInt(n))); break; \
        case aSmaFlt: v = setSmaFlt(y, SF_CALL(getSmaFlt(n))); break; \
        case aSmaCom: v = setSmaFlt(y, SC_CALL(getSmaCom(n))); break; \
        /* line above has conversion from complex to float */ \
        default: OPER_FAIL break; \
    } \
    return v; \
}

#define TYPE1_COM(v, CALL, SF_CALL, SC_CALL) \
{ \
    init_n(CALL) \
    switch(a) \
    { \
        case aSmaInt: v = setSmaFlt(y, SF_CALL(getSmaInt(n))); break; \
        case aSmaFlt: v = setSmaFlt(y, SF_CALL(getSmaFlt(n))); break; \
        case aSmaCom: v = setSmaCom(y, SC_CALL(getSmaCom(n))); break; \
        default: OPER_FAIL break; \
    } \
    return v; \
}

#define sf_abs(n) (((n)>=0) ? (n) : -(n))
#define sf_arg(n)  0
#define sf_real(n) n
#define sf_imag(n) 0
#define sf_conj(n) n
#define sf_proj(n) n

value _cabs (value v) COMPL_OPR (v, _cabs, sr_abs , sf_abs , ccabs)
value _carg (value v) COMPL_OPR (v, _carg, sr_arg , sf_arg , carg)
value _real (value v) COMPL_OPR (v, _real, sr_real, sf_real, creal)
value _imag (value v) COMPL_OPR (v, _imag, sr_imag, sf_imag, cimag)
value _conj (value v) TYPE1_COM (v, _conj, sf_conj, conj)
value _proj (value v) TYPE1_COM (v, _proj, sf_proj, _cproj)



/* ----------- Integer operations ------------------ */

value _idiv (value v)
{ return toInt(_div(v)); }

value _isqrt  (value v)
{ return toInt(_sqrt(v)); }

value _ceil (value v)
{
    init_n(_ceil)
    switch(a)
    {
        case aSmaInt: v = setSmaInt(y, getSmaInt(n)); break; // TODO: this line is due to setRef() in OPER_TYPE1
        case aSmaFlt: v = setSmaFlt(y, ceil(getSmaFlt(n))); break;
        default: OPER_FAIL break;
    }
    return v;
}

value _floor (value v)
{
    init_n(_floor)
    switch(a)
    {
        case aSmaInt: v = setSmaInt(y, getSmaInt(n)); break;
        case aSmaFlt: v = setSmaFlt(y, floor(getSmaFlt(n))); break;
        default: OPER_FAIL break;
    }
    return v;
}

value toInt (value v)
{
    init_n(toInt)
    switch(a)
    {
        case aSmaInt: v = setSmaInt(y, getSmaInt(n)); break;
        case aSmaFlt: v = setSmaInt(y, FltToInt(getSmaFlt(n))); break;
        default: OPER_FAIL break;
    }
    return v;
}


value factorial (value v)
{
    SmaInt si, osi;
    SmaFlt sf;
    init_n(factorial)
    switch(a)
    {
        case aSmaInt: si = getSmaInt(n);
            if(si <= -1) OPER_FAIL_(Argument_OutOf_Domain);
            else { for(osi=1; si != 0; si--) osi *= si; v = setSmaInt(y, osi); }
            break;

        case aSmaFlt:
            sf = getSmaFlt(n);
            if(sf <= -1) OPER_FAIL_(Argument_OutOf_Domain);
            else v = setSmaFlt(y, tgamma(sf+1));
            break;

        default: OPER_FAIL break;
    }
    return v;
}


value _mod (value v)
{
    init_n_m(_mod)
    if(a==aSmaInt && b==aSmaInt)
    {
        SmaInt N = getSmaInt(n);
        SmaInt M = getSmaInt(m);
        if(M) v = setSmaInt(y, N % M);
        else OPER_FAIL_(Division_By_Zero);
    }
    else OPER_FAIL_(Only_On_Integer_Number);
    return v;
}


static SmaInt get_gcd (SmaInt a, SmaInt b)
{
    SmaInt t;
    if(b==0) return 1;
    while(a != 0)
    { t = a; a = b%a; b = t; }
    return b;
}

value _gcd (value v)
{
    _init_n
    value e = check_arguments(y,*n,2); if(e) return e;
    n += 2; // skip vector header
    const_value m = vNext(n);
    n = vGet(n);
    m = vGet(m);
    a = value_type(n);
    int b = value_type(m);
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt): {
            SmaInt N = getSmaInt(n);
            SmaInt M = getSmaInt(m);
            if(M) v = setSmaInt(y, get_gcd(N, M));
            else OPER_FAIL_(Division_By_Zero);
            break; }
        default: OPER_FAIL_(Only_On_Integer_Number); break;
    }
    return v;
}


value _ilog (value v)
{
    _init_n
    value e = check_arguments(y,*n,2); if(e) return e;
    n += 2; // skip vector header
    const_value m = vNext(n);
    n = vGet(n);
    m = vGet(m);
    a = value_type(n);
    int b = value_type(m);
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt):
            v = _log(setRef(v,m));
            v = _log(setRef(v,n));
            v = toInt(_div(v));
            v = vpcopy(y, v);
            break;
        default: OPER_FAIL_(Only_On_Integer_Number); break;
    }
    return v;
}



/* --------- Bitwise Operations ------------------------ */

#define BWISE_OPR(v, CALL, SI_CALL) \
{ \
    SmaInt N, M; \
    init_n_m(CALL) \
    switch(MIX(a,b)) \
    { \
        case MIX(aSmaInt, aSmaInt): \
            N = getSmaInt(n); \
            M = getSmaInt(m); \
            v = setSmaInt(y, SI_CALL(N, M)); \
            break; \
        default: OPER_FAIL_(Only_On_Integer_Number); break; \
    } \
    return v; \
}

#define si_shift_left(n,m)  (n << m)
#define si_shift_right(n,m) (n >> m)
#define si_bitwise_xor(n,m) (n ^  m)
#define si_bitwise_or(n,m)  (n |  m)
#define si_bitwise_and(n,m) (n &  m)

value shift_left  (value v) BWISE_OPR (v, shift_left , si_shift_left)
value shift_right (value v) BWISE_OPR (v, shift_right, si_shift_right)
value bitwise_xor (value v) BWISE_OPR (v, bitwise_xor, si_bitwise_xor)
value bitwise_or  (value v) BWISE_OPR (v, bitwise_or , si_bitwise_or)
value bitwise_and (value v) BWISE_OPR (v, bitwise_and, si_bitwise_and)

value bitwise_not (value v)
{
    init_n(bitwise_not)
    switch(a)
    {
        case aSmaInt: v = setSmaInt(y, ~getSmaInt(n)); break;
        default: OPER_FAIL_(Only_On_Integer_Number); break;
    }
    return v;
}



/* --------- Comparison Operations --------------------- */

value lessThan (value v)
{
    init_n_m(lessThan)
    bool r = false;
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt): r = getSmaInt(n) < getSmaInt(m); break;
        case MIX(aSmaInt, aSmaFlt): r = getSmaInt(n) < getSmaFlt(m); break;
        case MIX(aSmaFlt, aSmaInt): r = getSmaFlt(n) < getSmaInt(m); break;
        case MIX(aSmaFlt, aSmaFlt): r = getSmaFlt(n) < getSmaFlt(m); break;
        case MIX(aString, aString): r = -1==strcmp22(getStr2(n), getStr2(m)); break;
        default: { OPER_FAIL return v; }
    }
    return setBool(y, r);
}

value greaterThan (value v)
{
    _init_n_m
    if(!a) return v; // just to remove unused variable warning
    // swap n and m positions, since n>m = m<n
    v = lessThan(setRef(setRef(v,m),n));
    return vpcopy(y, v);
}


value equalTo (value v)
{
    init_n_m(equalTo)
    bool r = false;
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt): r = getSmaInt(n) == getSmaInt(m); break;
        case MIX(aSmaInt, aSmaFlt): r = getSmaInt(n) == getSmaFlt(m); break;
        case MIX(aSmaInt, aSmaCom): r = getSmaInt(n) == getSmaCom(m); break;

        case MIX(aSmaFlt, aSmaInt): r = getSmaFlt(n) == getSmaInt(m); break;
        case MIX(aSmaFlt, aSmaFlt): r = getSmaFlt(n) == getSmaFlt(m); break;
        case MIX(aSmaFlt, aSmaCom): r = getSmaFlt(n) == getSmaCom(m); break;

        case MIX(aSmaCom, aSmaInt): r = getSmaCom(n) == getSmaInt(m); break;
        case MIX(aSmaCom, aSmaFlt): r = getSmaCom(n) == getSmaFlt(m); break;
        case MIX(aSmaCom, aSmaCom): r = getSmaCom(n) == getSmaCom(m); break;

        case MIX(aString, aString): r = 0==strcmp22(getStr2(n), getStr2(m)); break;
        default: break;
    }
    return setBool(y, r);
}

value sameAs (value v)
{
    v = equalTo(v);
    const_value n = vGetPrev(v);
    if(!isBool(n))
    {
        assert((*n >> 28)==VAL_VECTOR);
        v = _not(_sum(_not(v)));
    }
    return v;
}



/* --------- Logical Operations ------------------------ */

#define lenString(n) ((*n & 0x0FFFFFFF)-1) // get string length

#define OR_AND(CALL, opr) \
{ \
    init_n_m(CALL) \
    switch(a) \
    { \
        case aString: a = lenString(n)!=0; break; \
        case aSmaInt: a = getSmaInt(n)!=0; break; \
        case aSmaFlt: a = getSmaFlt(n)!=0; break; \
        case aSmaCom: a = getSmaCom(n)!=0; break; \
        default: a=false; break; \
    } \
    switch(b) \
    { \
        case aString: b = lenString(m)!=0; break; \
        case aSmaInt: b = getSmaInt(m)!=0; break; \
        case aSmaFlt: b = getSmaFlt(m)!=0; break; \
        case aSmaCom: b = getSmaCom(m)!=0; break; \
        default: b=false; break; \
    } \
    return setBool(y, a opr b); \
}

value logical_or  (value v) OR_AND(logical_or , ||)
value logical_and (value v) OR_AND(logical_and, &&)
value logical_not (value v)
{
    init_n(logical_not)
    switch(a)
    {
        case aString: a = lenString(n)!=0; break;
        case aSmaInt: a = getSmaInt(n)!=0; break;
        case aSmaFlt: a = getSmaFlt(n)!=0; break;
        case aSmaCom: a = getSmaCom(n)!=0; break;
        default: a=false; break;
    }
    return setBool(y, !a);
}



/*-----------------------------------------------------------*/

value _pow (value v)
{
    init_n_m(_pow)
    SmaInt nsi, msi;
    SmaFlt nsf, msf;
    SmaCom nsc, msc;
    msc=nsc=msf=nsf=msi=nsi=0;

    switch(a) {
        case aSmaInt: nsi = getSmaInt(n); break;
        case aSmaFlt: nsf = getSmaFlt(n); break;
        case aSmaCom: nsc = getSmaCom(n); break;
    }
    switch(b) {
        case aSmaInt: msi = getSmaInt(m); break;
        case aSmaFlt: msf = getSmaFlt(m); break;
        case aSmaCom: msc = getSmaCom(m); break;
    }
    switch(MIX(a,b))
    {
        case MIX(aSmaInt, aSmaInt):
            msf=1; if(msi>=0) { while(msi--) msf *= nsi; msi = msf; }
            else { msi = -msi;  while(msi--) msf *= nsi; msf = 1/msf; }
            if(msi==msf) v = setSmaInt(y, msi);
            else         v = setSmaFlt(y, msf);
            break;

        case MIX(aSmaFlt, aSmaInt):
            msf=1; if(msi>=0) { while(msi--) msf *= nsf; }
            else { msi = -msi;  while(msi--) msf *= nsf; msf = 1/msf; }
            v = setSmaFlt(y, msf);
            break;

        case MIX(aSmaInt, aSmaFlt):
            if(nsi>=0 || floor(msf)==msf) v = setSmaFlt(y, pow(nsi, msf));
            else if(COMPLEX) v = setSmaCom(y, _cpow(nsi,msf));
            break;

        case MIX(aSmaFlt, aSmaFlt):
            if(nsf>=0 || floor(msf)==msf) v = setSmaFlt(y, pow(nsf, msf));
            else if(COMPLEX) v = setSmaCom(y, _cpow(nsf,msf));
            break;

        #ifdef COMPLEX
        case MIX(aSmaInt, aSmaCom): v = setSmaCom(y, _cpow(nsi, msc)); break;
        case MIX(aSmaFlt, aSmaCom): v = setSmaCom(y, _cpow(nsf, msc)); break;
        case MIX(aSmaCom, aSmaInt): v = setSmaCom(y, _cpow(nsc, msi)); break;
        case MIX(aSmaCom, aSmaFlt): v = setSmaCom(y, _cpow(nsc, msf)); break;
        case MIX(aSmaCom, aSmaCom): v = setSmaCom(y, _cpow(nsc, msc)); break;
        #endif

        default: OPER_FAIL break;
    }
    return v;
}



/*static SmaRat RatSmaFlt (value v)
{
    SmaFlt n = getSmaFlt(v);
    int i, j, len=0;
    bool neg;
    SmaFlt t;
    SmaRat sr = {0,0x1000000000000000};
    SmaInt g, w, a, b, c, d, BB;
    const int B=2*5;
    int digits[30];

    if(n<0) { n=-n; neg=1; } else neg=0;
    w = (SmaInt)n;
    if(w<0) { a=w; b=neg; } // if n = inf
    else
    {
        n -= w;
        BB=B;
        while(1)
        {
            g = (SmaInt)(n*BB); // get next decimal digit
            digits[len++] = (int)(g%B);
            if(BB>=1000000000)
            {
                c = get_gcd(g, BB); a = g/c; b = BB/c;
                if(b<sr.deno) { sr.nume = a; sr.deno = b; }
            }

            j=0;
            for(i=len-1-2; (i+1)*2 >= len; i--)
            {
                for(j=0; j<len-1-i; j++)
                { if(*(digits+len-1-j) != *(digits+i-j)) break; }
                if(j==len-1-i) { j=len; break; }
            }

            if(j==len) // if a repetition has been found
            {
                a=0; b=1;
                for(j=0; j <= 2*i-(len-1); j++)
                { a = a*B + digits[j]; b*=B; }

                c=0; d=1;
                for( ; j <= i; j++)
                { c = c*B + digits[j]; d*=B; } d--;

                a = a*d+c;
                b = b*d;
                t = a/(SmaFlt)b - n;
                if(-1E-08 <= t && t <= 1E-08) break; // if t is close to zero
            }
            BB *= B;
            if(BB<0) { a = sr.nume; b = sr.deno; break; }
        }
        c = get_gcd(a, b); a /= c; b /= c;
        a += w*b; // add whole part
    }
    if(neg) a = -a;
    sr.nume = a;
    sr.deno = b;
    return sr;
}*/


/******************************************************************************************/

#define StrToIntGet(base, a, b, d) \
    else if(c>=a && c<=b) \
    { \
        if(dp) deno *= base; \
        nume = nume*base + d + (int)(c-a); \
        if(nume<prev) { dp=4; break; } else prev=nume; \
    }

value StrToVal (value out, const_Str2 str)  // single output value
{
    const_Str2 end;
    const_Str2 argv[2];
    *argv=NULL;

    wchar c = str[0];
    long len = strlen2(str);
    if(len==0)
    {
        out = setError(out, L"Given constant is empty.");
    }
    else if(c=='\'') // if a character
    {
        if(len<2 || str[len-1]!=c)
        {
            out = setError(out, L"Given character constant must end with ' .");
        }
        else
        {
            str += 1; // skip the starting and ending ''
            end = str + len-2;
            int i = wstr_to_wchar(&c, str, end);
            if(i>0 && str[i]=='\'') // if end of character string reached
                out = setCharac(out, c);
            else
                out = setError(out, L"Given character constant is invalid.");
        }
    }
    else if(c=='"') // if a string // TODO: use strNext2()
    {
        if(len<2 || str[len-1]!=c)
        {
            out = setError(out, L"Given string constant must end with \" .");
        }
        else
        {
            int i=0;
            wchar* w = (wchar*)(out+2);
            str += 1; // skip the starting and ending ""
            end = str + len-2;
            while(str < end)
            {
                i = wstr_to_wchar(&c, str, end);
                if(i<=0) break;
                *w++ = c;
                str += i;
            }
            if(i<0)
            {
                i = -i;
                wchar w[i+1];
                strcpy22S(w, str, i);
                argv[1] = w;
                argv[0] = L"Given string has invalid character code '%s'.";
                out = setMessage(out, 0, 2, argv);
            }
            else
            {
                *w = '\0';
                out = onSetStr2(out, w);
            }
        }
    }
    else if(!(c=='-' || c=='.' || isDigit(c)))
        out = setStr22(out, str);
    else
    {
        const wchar* s = str;
        int i=0, dp=0, neg=0;
        SmaInt prev=0, nume=0, deno=1;

        if(s[i] == '-') { neg=1; i++; }

        if(s[i]=='0' && s[i+1]=='x')
        {
            for(i+=2; i<len; i++)
            {
                c = s[i];
                if(c=='_') continue;
                if(c=='.') { if(++dp==2) break; }
                StrToIntGet(16, '0', '9', 0)
                StrToIntGet(16, 'A', 'F', 10)
                StrToIntGet(16, 'a', 'f', 10)
                else { dp=3; break; }
            }
        }
        else if(s[i]=='0' && s[i+1]=='b')
        {
            for(i+=2; i < len; i++) { c = s[i]; if(c=='_') continue; if(c=='.') { if(++dp==2) break; } StrToIntGet(2, '0', '1', 0) else { dp=3; break; } }
        }
        else if(s[i]=='0' && s[i+1]=='o')
        {
            for(i+=2; i < len; i++) { c = s[i]; if(c=='_') continue; if(c=='.') { if(++dp==2) break; } StrToIntGet(8, '0', '7', 0) else { dp=3; break; } }
        }
        else if(s[i] != '.')
        {
            for(    ; i < len; i++) { c = s[i]; if(c=='_') continue; if(c=='.') { if(++dp==2) break; } StrToIntGet(10, '0', '9', 0) else { dp=3; break; } }
        }
        else *argv = L"Number must not begin with a decimal point.";
        if(c=='.'
        && dp==1) *argv = L"Number must not end with a decimal point.";
        if(dp==2) *argv = L"Number has more than one decimal point.";
        if(dp==3) *argv = L"Number contains an invalid digit.";
        if(dp==4) *argv = L"Number has too many digits; BigNum is not yet supported.";

        if(*argv) out = setMessage(out, 0, 1, argv);
        else{
            if(neg) nume = -nume;
            if(deno==1) out = setSmaInt(out, nume);
            else out = setSmaFlt(out, nume/(SmaFlt)deno);
        }
    }
    return out;
}



static wchar* IntToStrGet (wchar* out, SmaInt n, int base)
{
    wchar* str=out;
    int off=0;

    if(n==(SmaInt)0x8000000000000000L) // TODO: why casting?
        return strcpy21(out, "inf");

    if(n<0){    // if number is negative
        n=-n;
        if(n<0) return strcpy21(out, "inf");
        *out++ = '-';
        off=1;  // offset for string-reverse()
    }
    switch(base)
    {
    case  2: off+=2; *out++='0'; *out++='b'; break;
    case  8: off+=2; *out++='0'; *out++='o'; break;
    case 16: off+=2; *out++='0'; *out++='x'; break;
    default: break;
    }
    while(true)
    {
        wchar a;
        a = n % base;
        n = n / base;
        *out++ = (a<10) ? ('0'+a) : ('A'+a-10);
        if(n==0) break; // n could initially be 0
    }
    *out=0;
    strrev2(str+off); // reverse the digits
    return out;
}



static wchar* FltToStrGet (wchar* out, SmaFlt n, int base)
{
    int i;
    SmaInt w, d;

    const SmaInt exp7 =
        (base== 2) ? 2*2*2*2*2*2*2 :
        (base== 8) ? 8*8*8*8*8*8*8 :
        (base==16) ? 0x10000000 : 10000000;

    if(n<0) { n=-n; *out++ = '-'; }         // make n positive
    w = (SmaInt)n;                          // get whole part of n
    if(w<0) return strcpy21(out, "inf");    // if n is too large

    n = n-w;                                // get decimal part of n
    d = (SmaInt)((1.0+n)*exp7);             // scale and get it into integer
    if(d%base >= base/2) d += base/2;       // round the last decimal digit

    if(d >= 2*exp7) { w++; d=exp7; }    // check if decimal part is close to 1
    out = IntToStrGet(out, w, base);
    if(d==exp7) return out;             // check if decimal part is 0

    *out++ = '.';                       // put decimal point
    IntToStrGet(out, d/base, base);     // remove last digit of d
    if(d==0) return ++out;

    i = 1 + (base==10 ? 0 : 2);
    while(out[i]!=0){           // shift so to remove
        *out = out[i];          // the characters 0x1
        out++;
    }
    // remove trailing zeros
    while(*(out-1)=='0') out--;
    *out = 0;
    return out;
}



static Str2 ValToStr (const_value n, Str2 output, int base, int t_places, int d_places)
{
    wchar temp[100];
    wchar str[1000] = {0};
    wchar* out = str;
    *out=0;

    enum ValueType type = value_type(n);
    if(base!=2 && base!=8 && base!=16)
        base = type==VAL_POINTER ? 16 : 10;
    int i = base==10 ? 0 : 2;

    /*if(type==VAL_POINTER) IntToStrGet (out, (intptr_t)getPointer(n), base);
  else*/ if(type==aSmaInt) IntToStrGet (out, getSmaInt(n), base);
    else if(type==aSmaFlt) FltToStrGet (out, getSmaFlt(n), base);
    else if(type!=aSmaCom) strcpy21(str, "Error in ValToStr().");
    else{
        FltToStrGet (out, creal(getSmaCom(n)), base);
        SmaFlt sf = cimag(getSmaCom(n));

        if((out[i]=='0' && out[i+1]==0)
        || (out[0]=='-' && out[i+1]=='0' && out[i+2]==0))
        {
            *out = 0; // if n == 0+bi

            if(sf<0){
                sf = -sf;
                *out++ = '-';
            }
            temp[i]=0;
            FltToStrGet (temp, sf, base);

            if(!(temp[i]=='1' && temp[i+1]==0)) // if n != 0+1i
                out = strcpy22(out, temp);

            out = strcpy22(out, TWSF(SQRT_of_Neg_One));
        }
        else
        {
            out += strlen2(out); // if n == a+bi

            if(sf<0){
                sf = -sf;
                *out++ = '-';
            } else *out++ = '+';

            temp[i]=0;
            FltToStrGet (temp, sf, base);

            if(!(temp[i]=='1' && temp[i+1]==0)) // if n != a+1i
                out = strcpy22(out, temp);

            out = strcpy22(out, TWSF(SQRT_of_Neg_One));
        }
    }
    return strcpy22(output, str);
}



static Str2 VstToStrGet (const_value v, Str2 out, int info_base, int t_places, int d_places)
{
    v = vGet(v);
    enum ValueType type = *v >> 28;

    uint32_t count;
    if(type==VAL_VECTOR){
        count = VEC_LEN(*v); // get vector length
        v += 2; // skip vector header
    } else count=1;

    int info = info_base & 0xF00;
    int base = info_base & 0x0FF;

    if((info & PUT_NEWLINE) && type==VAL_VECTOR)
    {
        const_value n = v;
        int c = count;
        while(true)
        {
            if(c==0) { info &= ~PUT_NEWLINE; break; }
            uint32_t a = *vGet(n);
            if((a>>28)==VAL_VECTOR && VEC_LEN(a)) break;
            n = vNext(n); c--;
        }
    }

    if(!(info & PUT_VAL_TYPE)) // do a number of checks for software error
    {
        assert(type != VAL_MESSAGE);
        assert(type != VAL_OFFSET);
        assert(type != VAL_POINTER || (*v & 0x0F000000)==0);
    }
    if(type!=VAL_VECTOR)
    {
        if(info & PUT_CATEGORY)
            out = strcpy21(out, "v");

        else if(info & PUT_VAL_TYPE)
        {
            const char* s;
            switch(type)
            {
            case VAL_MESSAGE: s = "error"  ; break;
            case VAL_OFFSET : s = "offset" ; break;
            case VAL_ENUMER : s = "bool"   ; break;
            case VAL_CHARAC : s = "wchar"  ; break;
            case VAL_POINTER: s = "pointer"; break;
            default:
                switch(value_type(v))
                {
                case aSmaInt    : s = "integer"; break;
                case aSmaFlt    : s = "float"  ; break;
                case aSmaRat    : s = "ratio"  ; break;
                case aSmaCom    : s = "complex"; break;
                case aString    : s = "string" ; break;
                default: s="???"; break;
                }
            }
            out = strcpy21(out,s);
        }
        else if(type==VAL_MESSAGE)
        {
            out = strcpy22(out, getMessage(v));
        }
        else if(type==VAL_CHARAC)
        {
            wchar c = (wchar)(*v & 0x0FFFFFFF);
            if(info & PUT_ESCAPE)
            {
                *out++ = '\'';
                if(c=='\\' || c=='\'') { *out++ = '\\'; *out++ =  c ; }
                else if(isPrintableASCII(c)) *out++ = c;
                else out = wchar_to_wstr(out, c);
                *out++ = '\'';
            }
            else *out++ = c;
            *out = 0;
        }
        else if(type==VAL_ENUMER)
        {
            const char* s = NULL;
            uint32_t a = *v;
            if(isBool(v)) s = (a&1) ? "true" : "false";
            else out = IntToStrGet(out, (a & 0xFFFF), 8); // 8 for no reason!
            if(s) out = strcpy21(out,s);
        }
        else if(type==VAL_ARRRAY)
        {
            if(info & PUT_ESCAPE)
            {
                *out++ = '"';
                if(isStr2(v))
                {
                    const wchar* s = getStr2(v);
                    for(; *s; s++)
                    {
                        wchar c = *s;
                        if(c=='\\' || c=='"') { *out++ = '\\'; *out++ =  c ; }
                        else if(isPrintableASCII(c)) *out++ = c;
                        else out = wchar_to_wstr(out, c);
                    }
                }
                else assert(false);
                *out++ = '"';
                *out = 0;
            }
            else if(isStr1(v)) out = strcpy21(out, getStr1(v));
            else if(isStr2(v)) out = strcpy22(out, getStr2(v));
            else assert(false);
        }
        else out = ValToStr(v, out, base, t_places, d_places);
    }
    else if(count)
    {
        out = strcpy21(out,"(");

        int info2 = info & ~PUT_NEWLINE; // remove the put-newline mask
        out = VstToStrGet (v, out, info2|base, t_places, d_places);
        count--;
        v = vNext(v);

        while(count)
        {
            out = strcpy21(out,",");
            if(info & PUT_NEWLINE) out = strcpy21(out,"\r\n");
            out = strcpy21(out," ");
            out = VstToStrGet (v, out, info2|base, t_places, d_places);
            count--;
            v = vNext(v);
        }
        out = strcpy21(out,")");
    }
    return out;
}

value VstToStr (value v, int info_base, int t_places, int d_places)
{
    assert(v!=NULL); if(!v) return v;
    _init_n
    wchar* out = (wchar*)(v+2);
    bool b = (info_base & PUT_CATEGORY) && ((*n>>28)!=VAL_VECTOR || VEC_LEN(*n)==0);
    if(b) *out++ = '(';
    out = VstToStrGet(n, out, info_base, t_places, d_places);
    if(b) *out++ = ')';
    *out = 0;
    return vpcopy(y, onSetStr2(v, out));
}

