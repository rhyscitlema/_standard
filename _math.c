/*
    _math.c
*/

#include <math.h>
#include <complex.h>
#include "_string.h"
#include "_texts.h"
#include "_math.h"



typedef double complex SmaCom;

#ifdef USE_GMP
#include <gmp.h>
typedef mpz_t BigInt;
typedef mpq_t BigRat;
typedef mpf_t BigFlt;
#else
typedef SmaInt BigInt;
typedef SmaRat BigRat;
typedef SmaFlt BigFlt;
#endif
typedef SmaCom BigCom;



#ifndef COMPLEX
#define COMPLEX 0

#define csqrt
#define cpow(x,y) (x+y)
#define cexp
#define clog

#define ccos
#define csin
#define ctan
#define cacos
#define casin
#define catan

#define ccosh
#define csinh
#define ctanh
#define cacosh
#define casinh
#define catanh

inline SmaFlt cabs (SmaCom x)
{ SmaFlt r = creal(x),
         i = cimag(x);
  return sqrt(r*r+i*i);
}
/*
#define carg
#define creal
#define cimag
#define conj
*/
#define cproj

#endif

static SmaCom clog10 (SmaCom x)
{ return clog(x)/clog(10); }



static inline SmaCom getSmaCom(value v) { return (*((SmaCom*)(&(v.sc)))); }

static inline value setSmaCom(SmaCom sc ) { value v; v.type = aSmaCom; (*((SmaCom*)(&(v.sc)))) = sc ; return v; }

value setSmaCo2 (SmaFlt Re, SmaFlt Im) { return setSmaCom(Re+Im*I); }

#define FltSmaRat(n) (getSmaRat(n).nume / (SmaFlt)getSmaRat(n).deno)

#define returnY if(!getType(y)) y = setNotval(TWST(Not_On_Complex_Number)); return y;

#define initY value y={0};



static SmaInt get_gcd (SmaInt a, SmaInt b)
{
    SmaInt t;
    if(b==0) return 1;
    while(a != 0)
    { t = a; a = b%a; b = t; }
    return b;
}

static inline SmaRat SimRat (SmaRat sr) // Simplify SmaRat
{
    const SmaInt LIM = 0xFFFF;
    if(sr.deno==0) { if(sr.nume!=0) sr.nume=1; return sr; }
    if(-LIM <= sr.nume && LIM <= sr.nume
    && -LIM <= sr.deno && LIM <= sr.deno) return sr;
    SmaInt si = get_gcd (sr.nume, sr.deno);
    sr.nume /= si;
    sr.deno /= si;
    return sr;
}

static value toInt (value n)
{
    SmaInt si;
    SmaFlt sf;
    SmaCom sc;
    switch(getType(n))
    {
    case aSmaRat: if(getSmaRat(n).deno==1)
                    n = setSmaInt(getSmaRat(n).nume);
                  break;

    case aSmaFlt: sf = getSmaFlt(n);
                  si = (SmaInt)sf;
                  if(si==sf) n = setSmaInt(si);
                  break;

    case aSmaCom: sc = getSmaCom(n);
                  if(cimag(sc)==0)
                  {   sf = creal(sc);
                      si = (SmaInt)sf;
                      if(si==sf) n = setSmaInt(si);
                      else       n = setSmaFlt(sf);
                  } break;
    default: break;
    }
    return n;
}



value positive (value n) { return n; }

value negative (value n)
{
    initY
    switch(getType(n))
    {
        case aSmaInt: y = setSmaInt(-getSmaInt(n)); break;
        case aSmaRat: y = setSmaRa2(-getSmaRat(n).nume, getSmaRat(n).deno); break;
        case aSmaFlt: y = setSmaFlt(-getSmaFlt(n)); break;
        case aSmaCom: y = setSmaCom(-getSmaCom(n)); break;
        default: break;
    }
    returnY
}



value add (value n, value m)
{
    SmaRat nsr, msr;
    initY
    switch(MIX(getType(n), getType(m)))
    {
        case MIX(aSmaInt, aSmaInt): y = setSmaInt(getSmaInt(n) + getSmaInt(m)); break;
        case MIX(aSmaInt, aSmaRat): msr = getSmaRat(m); msr.nume = getSmaInt(n)*msr.deno + msr.nume; y = setSmaRat(SimRat(msr)); break;
        case MIX(aSmaInt, aSmaFlt): y = setSmaFlt(getSmaInt(n) + getSmaFlt(m)); break;
        case MIX(aSmaInt, aSmaCom): y = setSmaCom(getSmaInt(n) + getSmaCom(m)); break;

        case MIX(aSmaRat, aSmaInt): nsr = getSmaRat(n); nsr.nume = nsr.nume + nsr.deno*getSmaInt(m); y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaRat): nsr = getSmaRat(n); msr = getSmaRat(m); nsr.nume = nsr.nume*msr.deno + nsr.deno*msr.nume; nsr.deno = nsr.deno*msr.deno; y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaFlt): y = setSmaFlt(FltSmaRat(n) + getSmaFlt(m)); break;
        case MIX(aSmaRat, aSmaCom): y = setSmaCom(FltSmaRat(n) + getSmaCom(m)); break;

        case MIX(aSmaFlt, aSmaInt): y = setSmaFlt(getSmaFlt(n) + getSmaInt(m)); break;
        case MIX(aSmaFlt, aSmaRat): y = setSmaFlt(getSmaFlt(n) + FltSmaRat(m)); break;
        case MIX(aSmaFlt, aSmaFlt): y = setSmaFlt(getSmaFlt(n) + getSmaFlt(m)); break;
        case MIX(aSmaFlt, aSmaCom): y = setSmaCom(getSmaFlt(n) + getSmaCom(m)); break;

        case MIX(aSmaCom, aSmaInt): y = setSmaCom(getSmaCom(n) + getSmaInt(m)); break;
        case MIX(aSmaCom, aSmaRat): y = setSmaCom(getSmaCom(n) + FltSmaRat(m)); break;
        case MIX(aSmaCom, aSmaFlt): y = setSmaCom(getSmaCom(n) + getSmaFlt(m)); break;
        case MIX(aSmaCom, aSmaCom): y = setSmaCom(getSmaCom(n) + getSmaCom(m)); break;
    }
    returnY
}



value subtract (value n, value m)
{
    SmaRat nsr, msr;
    initY
    switch(MIX(getType(n), getType(m)))
    {
        case MIX(aSmaInt, aSmaInt): y = setSmaInt(getSmaInt(n) - getSmaInt(m)); break;
        case MIX(aSmaInt, aSmaRat): msr = getSmaRat(m); msr.nume = getSmaInt(n)*msr.deno - msr.nume; y = setSmaRat(SimRat(msr)); break;
        case MIX(aSmaInt, aSmaFlt): y = setSmaFlt(getSmaInt(n) - getSmaFlt(m)); break;
        case MIX(aSmaInt, aSmaCom): y = setSmaCom(getSmaInt(n) - getSmaCom(m)); break;

        case MIX(aSmaRat, aSmaInt): nsr = getSmaRat(n); nsr.nume = nsr.nume - nsr.deno*getSmaInt(m); y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaRat): nsr = getSmaRat(n); msr = getSmaRat(m); nsr.nume = nsr.nume*msr.deno - nsr.deno*msr.nume; nsr.deno = nsr.deno*msr.deno; y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaFlt): y = setSmaFlt(FltSmaRat(n) - getSmaFlt(m)); break;
        case MIX(aSmaRat, aSmaCom): y = setSmaCom(FltSmaRat(n) - getSmaCom(m)); break;

        case MIX(aSmaFlt, aSmaInt): y = setSmaFlt(getSmaFlt(n) - getSmaInt(m)); break;
        case MIX(aSmaFlt, aSmaRat): y = setSmaFlt(getSmaFlt(n) - FltSmaRat(m)); break;
        case MIX(aSmaFlt, aSmaFlt): y = setSmaFlt(getSmaFlt(n) - getSmaFlt(m)); break;
        case MIX(aSmaFlt, aSmaCom): y = setSmaCom(getSmaFlt(n) - getSmaCom(m)); break;

        case MIX(aSmaCom, aSmaInt): y = setSmaCom(getSmaCom(n) - getSmaInt(m)); break;
        case MIX(aSmaCom, aSmaRat): y = setSmaCom(getSmaCom(n) - FltSmaRat(m)); break;
        case MIX(aSmaCom, aSmaFlt): y = setSmaCom(getSmaCom(n) - getSmaFlt(m)); break;
        case MIX(aSmaCom, aSmaCom): y = setSmaCom(getSmaCom(n) - getSmaCom(m)); break;
    }
    returnY
}



value multiply (value n, value m)
{
    SmaRat nsr, msr;
    initY
    switch(MIX(getType(n), getType(m)))
    {
        case MIX(aSmaInt, aSmaInt): y = setSmaInt(getSmaInt(n) * getSmaInt(m)); break;
        case MIX(aSmaInt, aSmaRat): msr = getSmaRat(m); msr.nume *= getSmaInt(n); y = setSmaRat(SimRat(msr)); break;
        case MIX(aSmaInt, aSmaFlt): y = setSmaFlt(getSmaInt(n) * getSmaFlt(m)); break;
        case MIX(aSmaInt, aSmaCom): y = setSmaCom(getSmaInt(n) * getSmaCom(m)); break;

        case MIX(aSmaRat, aSmaInt): nsr = getSmaRat(n); nsr.nume *= getSmaInt(m); y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaRat): nsr = getSmaRat(n); msr = getSmaRat(m); nsr.nume *= msr.nume; nsr.deno *= msr.deno; y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaFlt): y = setSmaFlt(FltSmaRat(n) * getSmaFlt(m)); break;
        case MIX(aSmaRat, aSmaCom): y = setSmaCom(FltSmaRat(n) * getSmaCom(m)); break;

        case MIX(aSmaFlt, aSmaInt): y = setSmaFlt(getSmaFlt(n) * getSmaInt(m)); break;
        case MIX(aSmaFlt, aSmaRat): y = setSmaFlt(getSmaFlt(n) * FltSmaRat(m)); break;
        case MIX(aSmaFlt, aSmaFlt): y = setSmaFlt(getSmaFlt(n) * getSmaFlt(m)); break;
        case MIX(aSmaFlt, aSmaCom): y = setSmaCom(getSmaFlt(n) * getSmaCom(m)); break;

        case MIX(aSmaCom, aSmaInt): y = setSmaCom(getSmaCom(n) * getSmaInt(m)); break;
        case MIX(aSmaCom, aSmaRat): y = setSmaCom(getSmaCom(n) * FltSmaRat(m)); break;
        case MIX(aSmaCom, aSmaFlt): y = setSmaCom(getSmaCom(n) * getSmaFlt(m)); break;
        case MIX(aSmaCom, aSmaCom): y = setSmaCom(getSmaCom(n) * getSmaCom(m)); break;
    }
    returnY
}



value divide   (value n, value m)
{
    SmaInt nsi, msi;
    SmaRat nsr, msr;
    SmaFlt nsf, msf;
    SmaCom nsc, msc;
    initY
    switch(MIX(getType(n), getType(m)))
    {
        case MIX(aSmaInt, aSmaInt): nsi = getSmaInt(n); msi = getSmaInt(m); y = setSmaRa2(nsi, msi); break;
        case MIX(aSmaInt, aSmaRat): nsi = getSmaInt(n); msr = getSmaRat(m); nsr.nume = nsi * msr.deno; nsr.deno = msr.nume; y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaInt, aSmaFlt): nsi = getSmaInt(n); msf = getSmaFlt(m); y = setSmaFlt(nsi / msf); break;
        case MIX(aSmaInt, aSmaCom): nsi = getSmaInt(n); msc = getSmaCom(m); y = setSmaCom(nsi / msc); break;

        case MIX(aSmaRat, aSmaInt): nsr = getSmaRat(n); msi = getSmaInt(m); nsr.deno *= msi; y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaRat): nsr = getSmaRat(n); msr = getSmaRat(m); nsr.nume *= msr.deno; nsr.deno *= msr.nume; y = setSmaRat(SimRat(nsr)); break;
        case MIX(aSmaRat, aSmaFlt): nsf = FltSmaRat(n); msf = getSmaFlt(m); y = setSmaFlt(nsf / msf); break;
        case MIX(aSmaRat, aSmaCom): nsf = FltSmaRat(n); msc = getSmaCom(m); y = setSmaCom(nsf / msc); break;

        case MIX(aSmaFlt, aSmaInt): nsf = getSmaFlt(n); msi = getSmaInt(m); y = setSmaFlt(nsf / msi); break;
        case MIX(aSmaFlt, aSmaRat): nsf = getSmaFlt(n); msf = FltSmaRat(m); y = setSmaFlt(nsf / msf); break;
        case MIX(aSmaFlt, aSmaFlt): nsf = getSmaFlt(n); msf = getSmaFlt(m); y = setSmaFlt(nsf / msf); break;
        case MIX(aSmaFlt, aSmaCom): nsf = getSmaFlt(n); msc = getSmaCom(m); y = setSmaCom(nsf / msc); break;

        case MIX(aSmaCom, aSmaInt): nsc = getSmaCom(n); msi = getSmaInt(m); y = setSmaCom(nsc / msi); break;
        case MIX(aSmaCom, aSmaRat): nsc = getSmaCom(n); msf = FltSmaRat(m); y = setSmaCom(nsc / msf); break;
        case MIX(aSmaCom, aSmaFlt): nsc = getSmaCom(n); msf = getSmaFlt(m); y = setSmaCom(nsc / msf); break;
        case MIX(aSmaCom, aSmaCom): nsc = getSmaCom(n); msc = getSmaCom(m); y = setSmaCom(nsc / msc); break;
    }
    returnY
}

value idivide  (value n, value m)
{ return _floor(divide(n,m)); }



value _ceil (value n)
{
    initY
    switch(getType(n))
    {
        case aSmaInt: y=n; break;
        case aSmaRat: y = setSmaInt(ceil(FltSmaRat(n))); break;
        case aSmaFlt: y = setSmaInt(ceil(getSmaFlt(n))); break;
        default: break;
    }
    returnY
}

value _floor (value n)
{
    initY
    bool f=0;
    SmaFlt sf;
    switch(getType(n))
    {
        case aSmaInt: y=n; break;
        case aSmaRat: sf=FltSmaRat(n); f=1; break;
        case aSmaFlt: sf=getSmaFlt(n); f=1; break;
        default: break;
    }
    if(f)
    {   if(sf>0) sf += 0.000001;
        y = setSmaInt(floor(sf));
    }
    returnY
}



/* --------- Exponent and Logarithm Functions ----------- */

value _sqrt (value n)
{
    bool b=0;
    SmaFlt sf=0;
    initY
    switch(getType(n))
    {
        case aSmaInt: sf = getSmaInt(n); b=1; break;
        case aSmaRat: sf = FltSmaRat(n); b=1; break;
        case aSmaFlt: sf = getSmaFlt(n); b=1; break;
        case aSmaCom: if(COMPLEX) { y = setSmaCom(csqrt(getSmaCom(n))); } break;
        default: break;
    }
    if(b)
    {   if(sf>=0) y = setSmaFlt(sqrt(sf));
        else y = setSmaCo2(0, sqrt(-sf));
    }
    returnY
}

value _isqrt  (value n)
{ return _floor(_sqrt(n)); }



#define DO_LOG(n, SF_CALL, SC_CALL) \
{ \
    int b=0; \
    SmaFlt sf=0; \
    SmaCom sc=0; \
    initY \
    switch(getType(n)) \
    { \
        case aSmaInt: sf = getSmaInt(n); b=1; break; \
        case aSmaRat: sf = FltSmaRat(n); b=1; break; \
        case aSmaFlt: sf = getSmaFlt(n); b=1; break; \
        case aSmaCom: if(COMPLEX) { sc = getSmaCom(n); b=2; } break; \
        default: break; \
    } \
    if(b==1) \
    {   if(sf>0) y = setSmaFlt(SF_CALL(sf)); \
        else y = setNotval(TWST(Argument_OutOf_Domain)); \
    } \
    else if(b==2) \
    {   if(cabs(sc)>0) y = setSmaCom(SC_CALL(sc)); \
        else y = setNotval(TWST(Argument_OutOf_Domain)); \
    } \
    returnY \
}

value _log_10(value n) DO_LOG (n, log10, clog10)
value _log_e (value n) DO_LOG (n, log, clog)



/* --------- Trigonometric and Hyperbolic Functions ---- */

#define TYPE1_OPR(n, SF_CALL, SC_CALL) \
{ \
    initY \
    switch(getType(n)) \
    { \
        case aSmaInt: y = setSmaFlt(SF_CALL(getSmaInt(n))); break; \
        case aSmaRat: y = setSmaFlt(SF_CALL(FltSmaRat(n))); break; \
        case aSmaFlt: y = setSmaFlt(SF_CALL(getSmaFlt(n))); break; \
        case aSmaCom: if(COMPLEX) { y = setSmaCom(SC_CALL(getSmaCom(n))); } break; \
        default: break; \
    } \
    returnY \
}

#define INV_TRIGO(n, SF_CALL, SC_CALL) \
{ \
    int b=0; \
    SmaFlt sf=0; \
    SmaCom sc=0; \
    initY \
    switch(getType(n)) \
    { \
        case aSmaInt: sf = getSmaInt(n); b=1; break; \
        case aSmaRat: sf = FltSmaRat(n); b=1; break; \
        case aSmaFlt: sf = getSmaFlt(n); b=1; break; \
        case aSmaCom: if(COMPLEX) { sc = getSmaCom(n); b=2; } break; \
        default: break; \
    } \
    if(b) \
    {   if(sf < -1 || sf > 1) y = setNotval(TWST(Argument_OutOf_Domain)); \
        else y = setSmaFlt(SF_CALL(sf)); \
    } \
    else if(b==2) \
    {   sf = cabs(sc); \
        if(sf < -1 || sf > 1) y = setSmaCom(SC_CALL(sc)); \
        else y = setNotval(TWST(Argument_OutOf_Domain)); \
    } \
    returnY \
}

#ifndef acosh
#define acosh(x) log((x) + sqrt((x)*(x) - 1))
#define asinh(x) log((x) + sqrt((x)*(x) + 1))
#define atanh(x) (log(1+(x)) - log(1-(x)))/2.0
#endif

value _exp_e (value n) TYPE1_OPR (n, exp, cexp)

value _cos   (value n) TYPE1_OPR (n, cos, ccos)
value _sin   (value n) TYPE1_OPR (n, sin, csin)
value _tan   (value n) TYPE1_OPR (n, tan, ctan)

value _cosh  (value n) TYPE1_OPR (n, cosh, ccosh)
value _sinh  (value n) TYPE1_OPR (n, sinh, csinh)
value _tanh  (value n) TYPE1_OPR (n, tanh, ctanh)

value _acos  (value n) INV_TRIGO (n, acos, cacos)
value _asin  (value n) INV_TRIGO (n, asin, casin)
value _atan  (value n) TYPE1_OPR (n, atan, catan)

value _acosh (value n) TYPE1_OPR (n, acosh, cacosh)
value _asinh (value n) TYPE1_OPR (n, asinh, casinh)
value _atanh (value n) TYPE1_OPR (n, atanh, catanh)



/* --------- Mainly For Complex Numbers ---------------- */

#define COMPL_OPR(n, SR_CALL, SF_CALL, SC_CALL) \
{ \
    initY \
    switch(getType(n)) \
    { \
        case aSmaInt: y = setSmaInt(SF_CALL(getSmaInt(n))); break; \
        case aSmaRat: y = setSmaRat(SR_CALL(getSmaRat(n))); break; \
        case aSmaFlt: y = setSmaFlt(SF_CALL(getSmaFlt(n))); break; \
        case aSmaCom: y = setSmaFlt(SC_CALL(getSmaCom(n))); break; \
        /* here there is conversion from complex to float */ \
        default: break; \
    } \
    returnY \
}

#define TYPE1_COM(n, SF_CALL, SC_CALL) \
{ \
    initY \
    switch(getType(n)) \
    { \
        case aSmaInt: y = setSmaFlt(SF_CALL(getSmaInt(n))); break; \
        case aSmaRat: y = setSmaFlt(SF_CALL(FltSmaRat(n))); break; \
        case aSmaFlt: y = setSmaFlt(SF_CALL(getSmaFlt(n))); break; \
        case aSmaCom: y = setSmaCom(SC_CALL(getSmaCom(n))); break; \
        default: break; \
    } \
    returnY \
}

#define sf_abs(n) (((n)>=0) ? (n) : -(n))
#define sf_arg(n)  0
#define sf_real(n) n
#define sf_imag(n) 0
#define sf_conj(n) n
#define sf_proj(n) n

static inline SmaRat sr_abs(SmaRat n) { if(n.nume<0) n.nume = -n.nume; return n; }
static const SmaRat sr0; // sr0=0
#define sr_arg(n)  sr0
#define sr_real(n) n
#define sr_imag(n) sr0
#define sr_conj(n) n
#define sr_proj(n) n

value __abs (value n) COMPL_OPR (n, sr_abs , sf_abs , cabs)
value _carg (value n) COMPL_OPR (n, sr_arg , sf_arg , carg)
value _real (value n) COMPL_OPR (n, sr_real, sf_real, creal)
value _imag (value n) COMPL_OPR (n, sr_imag, sf_imag, cimag)
value _conj (value n) TYPE1_COM (n, sf_conj, conj)
value _proj (value n) TYPE1_COM (n, sf_proj, cproj)



/* ----------- Integer operations ------------------ */

value factorial (value n)
{
    initY
    SmaInt si, osi;
    SmaFlt sf;
    n = toInt(n);

    switch(getType(n))
    {
        case aSmaInt: si = getSmaInt(n);
            if(si <= -1) y = setNotval(TWST(Argument_OutOf_Domain));
            else { for(osi=1; si != 0; si--) osi *= si; y = setSmaInt(osi); }
            break;

        case aSmaRat:
        case aSmaFlt:
            if(getType(n)==aSmaRat)
                 sf = FltSmaRat(n);
            else sf = getSmaFlt(n);
            if(sf <= -1) y = setNotval(TWST(Argument_OutOf_Domain));
            else y = setSmaFlt(tgamma(sf+1));
            break;
        default: break;
    }
    returnY
}


value modulo (value n, value m)
{
    initY
    SmaInt N, M;
    n = toInt(n);
    m = toInt(m);

    if(isSmaInt(n) && isSmaInt(m))
    {
        N = getSmaInt(n); M = getSmaInt(m);
        if(M==0) y = setNotval(TWST(Division_By_Zero));
        else y = setSmaInt(N % M);
    }
    else y = setNotval(TWST(Only_On_Integer_Number));
    return y;
}

value find_gcd (value n, value m)
{
    initY
    SmaInt N, M;
    n = toInt(n);
    m = toInt(m);

    if(isSmaInt(n) && isSmaInt(m))
    {
        N = getSmaInt(n); M = getSmaInt(m);
        if(M==0) y = setNotval(TWST(Division_By_Zero));
        else y = setSmaInt(get_gcd(N, M));
    }
    else y = setNotval(TWST(Only_On_Integer_Number));
    return y;
}



/* --------- Bitwise Operations ------------------------ */

#define BWISE_OPR(n, m, SI_CALL) \
{ \
    initY \
    SmaInt N, M; \
    n = toInt(n); \
    m = toInt(m); \
 \
    if(isSmaInt(n) && isSmaInt(m)) \
    { \
        N = getSmaInt(n); M = getSmaInt(m); \
        y = setSmaInt(SI_CALL(N, M)); \
    } \
    else y = setNotval(TWST(Only_On_Integer_Number)); \
    return y; \
}

#define si_shift_right(n,m) (n >> m)
#define si_shift_left(n,m)  (n << m)
#define si_bitwise_or(n,m)  (n |  m)
#define si_bitwise_xor(n,m) (n ^  m)
#define si_bitwise_and(n,m) (n &  m)

value shift_right (value n, value m) BWISE_OPR (n, m, si_shift_right)
value shift_left  (value n, value m) BWISE_OPR (n, m, si_shift_left)
value bitwise_or  (value n, value m) BWISE_OPR (n, m, si_bitwise_or)
value bitwise_xor (value n, value m) BWISE_OPR (n, m, si_bitwise_xor)
value bitwise_and (value n, value m) BWISE_OPR (n, m, si_bitwise_and)

value bitwise_not (value n)
{
    initY
    n = toInt(n);
    if(isSmaInt(n)) y = setSmaInt(~getSmaInt(n));
    else y = setNotval(TWST(Only_On_Integer_Number));
    return y;
}



/* --------- Comparison Operations --------------------- */

#define EQUAL_OPR(n, m, SX_CALL) \
{ \
    initY \
    switch(MIX(getType(n), getType(m))) \
    { \
        case MIX(aSmaInt, aSmaInt): y = setSmaInt(SX_CALL(getSmaInt(n), getSmaInt(m))); break; \
        case MIX(aSmaInt, aSmaRat): y = setSmaInt(SX_CALL(getSmaInt(n), FltSmaRat(m))); break; \
        case MIX(aSmaInt, aSmaFlt): y = setSmaInt(SX_CALL(getSmaInt(n), getSmaFlt(m))); break; \
        case MIX(aSmaInt, aSmaCom): y = setSmaInt(SX_CALL(getSmaInt(n), getSmaCom(m))); break; \
 \
        case MIX(aSmaRat, aSmaInt): y = setSmaInt(SX_CALL(FltSmaRat(n), getSmaInt(m))); break; \
        case MIX(aSmaRat, aSmaRat): y = setSmaInt(SX_CALL(FltSmaRat(n), FltSmaRat(m))); break; \
        case MIX(aSmaRat, aSmaFlt): y = setSmaInt(SX_CALL(FltSmaRat(n), getSmaFlt(m))); break; \
        case MIX(aSmaRat, aSmaCom): y = setSmaInt(SX_CALL(FltSmaRat(n), getSmaCom(m))); break; \
 \
        case MIX(aSmaFlt, aSmaInt): y = setSmaInt(SX_CALL(getSmaFlt(n), getSmaInt(m))); break; \
        case MIX(aSmaFlt, aSmaRat): y = setSmaInt(SX_CALL(getSmaFlt(n), FltSmaRat(m))); break; \
        case MIX(aSmaFlt, aSmaFlt): y = setSmaInt(SX_CALL(getSmaFlt(n), getSmaFlt(m))); break; \
        case MIX(aSmaFlt, aSmaCom): y = setSmaInt(SX_CALL(getSmaFlt(n), getSmaCom(m))); break; \
 \
        case MIX(aSmaCom, aSmaInt): y = setSmaInt(SX_CALL(getSmaCom(n), getSmaInt(m))); break; \
        case MIX(aSmaCom, aSmaRat): y = setSmaInt(SX_CALL(getSmaCom(n), FltSmaRat(m))); break; \
        case MIX(aSmaCom, aSmaFlt): y = setSmaInt(SX_CALL(getSmaCom(n), getSmaFlt(m))); break; \
        case MIX(aSmaCom, aSmaCom): y = setSmaInt(SX_CALL(getSmaCom(n), getSmaCom(m))); break; \
    } \
    returnY \
}

#define COMPA_OPR(n, m, SX_CALL) \
{ \
    initY \
    switch(MIX(getType(n), getType(m))) \
    { \
        case MIX(aSmaInt, aSmaInt): y = setSmaInt(SX_CALL(getSmaInt(n), getSmaInt(m))); break; \
        case MIX(aSmaInt, aSmaRat): y = setSmaInt(SX_CALL(getSmaInt(n), FltSmaRat(m))); break; \
        case MIX(aSmaInt, aSmaFlt): y = setSmaInt(SX_CALL(getSmaInt(n), getSmaFlt(m))); break; \
 \
        case MIX(aSmaRat, aSmaInt): y = setSmaInt(SX_CALL(FltSmaRat(n), getSmaInt(m))); break; \
        case MIX(aSmaRat, aSmaRat): y = setSmaInt(SX_CALL(FltSmaRat(n), FltSmaRat(m))); break; \
        case MIX(aSmaRat, aSmaFlt): y = setSmaInt(SX_CALL(FltSmaRat(n), getSmaFlt(m))); break; \
 \
        case MIX(aSmaFlt, aSmaInt): y = setSmaInt(SX_CALL(getSmaFlt(n), getSmaInt(m))); break; \
        case MIX(aSmaFlt, aSmaRat): y = setSmaInt(SX_CALL(getSmaFlt(n), FltSmaRat(m))); break; \
        case MIX(aSmaFlt, aSmaFlt): y = setSmaInt(SX_CALL(getSmaFlt(n), getSmaFlt(m))); break; \
    } \
    returnY \
}

#define sx_Equal(n,m) ((n==m) ? 1 : 0)
#define sx_NotEq(n,m) ((n!=m) ? 1 : 0)
#define sx_Leser(n,m) ((n< m) ? 1 : 0)
#define sx_Grter(n,m) ((n> m) ? 1 : 0)
#define sx_LesEq(n,m) ((n<=m) ? 1 : 0)
#define sx_GreEq(n,m) ((n>=m) ? 1 : 0)

value equalTo        (value n, value m) EQUAL_OPR (n, m, sx_Equal)
value notEqual       (value n, value m) EQUAL_OPR (n, m, sx_NotEq)
value lessThan       (value n, value m) COMPA_OPR (n, m, sx_Leser)
value greaterThan    (value n, value m) COMPA_OPR (n, m, sx_Grter)
value lessOrEqual    (value n, value m) COMPA_OPR (n, m, sx_LesEq)
value greaterOrEqual (value n, value m) COMPA_OPR (n, m, sx_GreEq)



/* --------- Logical Operations ------------------------ */

#define sx_logical_or(n,m)  (((n) != 0 || (m) != 0) ? 1 : 0)
#define sx_logical_and(n,m) (((n) != 0 && (m) != 0) ? 1 : 0)

value logical_or  (value n, value m) EQUAL_OPR (n, m, sx_logical_or)
value logical_and (value n, value m) EQUAL_OPR (n, m, sx_logical_and)

value logical_not (value n)
{
    initY
    switch(getType(n))
    {
        case aString: y = setSmaInt(getString(n)==0); break;
        case aSmaInt: y = setSmaInt(getSmaInt(n)==0); break;
        case aSmaRat: y = setSmaInt(getSmaRat(n).nume==0); break;
        case aSmaFlt: y = setSmaInt(getSmaFlt(n)==0); break;
        case aSmaCom: y = setSmaInt(getSmaCom(n)==0); break;
        default: break;
    }
    return y;
}



/*-----------------------------------------------------------*/

value power (value n, value m)
{
    initY
    SmaInt nsi, msi, osi, vsi, wsi, prev;
    SmaFlt nsf, msf;
    SmaCom nsc, msc;
    n = toInt(n);
    m = toInt(m);

    switch(MIX(getType(n), getType(m)))
    {
        case MIX(aSmaInt, aSmaInt):
            nsi = getSmaInt(n);
            msi = getSmaInt(m);
            vsi = (nsi>=0) ? +nsi : -nsi;
            wsi = (msi>=0) ? +msi : -msi;

            for(prev=0, osi=1; wsi; wsi--)
            { osi *= vsi; if(osi<prev) break; else prev=osi; }

            if(wsi) // if result too large
            {
                if(msi>=0) y = setSmaRa2((((nsi<0) && (msi&1))?-1:+1),0);
                else y = setSmaInt(0);
            }
            else if((nsi<0) && (msi&1)) // if result should be negative
            {
                if(msi>=0) y = setSmaInt(-osi);
                else       y = setSmaRa2(-1, osi);
            }
            else // if result should be positive
            {
                if(msi>=0) y = setSmaInt(osi);
                else       y = setSmaRa2(1, osi);
            }
            break;

        case MIX(aSmaInt, aSmaRat):
            nsf = getSmaInt(n);
            msf = FltSmaRat(m);
            if(nsf>=0) y = setSmaFlt(pow(nsf, msf));
            else if(COMPLEX) y = setSmaCom(cpow(nsf,msf));
            break;

        case MIX(aSmaInt, aSmaFlt):
            nsf = getSmaInt(n);
            msf = getSmaFlt(m);
            if(nsf>=0) y = setSmaFlt(pow(nsf, msf));
            else if(COMPLEX) y = setSmaCom(cpow(nsf,msf));
            break;

        case MIX(aSmaInt, aSmaCom):
            if(COMPLEX)
            {   nsf = getSmaInt(n);
                msc = getSmaCom(m);
                y = setSmaCom(cpow(nsf, msc));
            }
            break;


        case MIX(aSmaRat, aSmaInt):
            nsf = FltSmaRat(n);
            msi = getSmaInt(m);
            msf=1; if(msi>=0) { while(msi--) msf *= nsf; y = setSmaFlt(msf); }
            else { msi = -msi;  while(msi--) msf *= nsf; y = setSmaFlt(1/msf); }
            break;

        case MIX(aSmaRat, aSmaRat):
            nsf = FltSmaRat(n);
            msf = FltSmaRat(m);
            if(nsf>=0) y = setSmaFlt(pow(nsf, msf));
            else if(COMPLEX) y = setSmaCom(cpow(nsf,msf));
            break;

        case MIX(aSmaRat, aSmaFlt):
            nsf = FltSmaRat(n);
            msf = getSmaFlt(m);
            if(nsf>=0) y = setSmaFlt(pow(nsf, msf));
            else if(COMPLEX) y = setSmaCom(cpow(nsf,msf));
            break;

        case MIX(aSmaRat, aSmaCom):
            if(COMPLEX)
            {   nsf = FltSmaRat(n);
                msc = getSmaCom(m);
                y = setSmaCom(cpow(nsf, msc));
            }
            break;


        case MIX(aSmaFlt, aSmaInt):
            nsf = getSmaFlt(n);
            msi = getSmaInt(m);
            msf=1; if(msi>=0) { while(msi--) msf *= nsf; y = setSmaFlt(msf); }
            else { msi = -msi;  while(msi--) msf *= nsf; y = setSmaFlt(1/msf); }
            break;

        case MIX(aSmaFlt, aSmaRat):
            nsf = getSmaFlt(n);
            msf = FltSmaRat(m);
            if(nsf>=0) y = setSmaFlt(pow(nsf, msf));
            else if(COMPLEX) y = setSmaCom(cpow(nsf,msf));
            break;

        case MIX(aSmaFlt, aSmaFlt):
            nsf = getSmaFlt(n);
            msf = getSmaFlt(m);
            if(nsf>=0) y = setSmaFlt(pow(nsf, msf));
            else if(COMPLEX) y = setSmaCom(cpow(nsf,msf));
            break;

        case MIX(aSmaFlt, aSmaCom):
            if(COMPLEX)
            {   nsf = getSmaFlt(n);
                msc = getSmaCom(m);
                y = setSmaCom(cpow(nsf, msc));
            }
            break;


        case MIX(aSmaCom, aSmaInt):
            nsc = getSmaCom(n);
            msi = getSmaInt(m);
            msc=1; if(msi>=0) { while(msi--) msc *= nsc; y = setSmaCom(msc); }
            else { msi = -msi;  while(msi--) msc *= nsc; y = setSmaCom(1/msc); }
            break;

        case MIX(aSmaCom, aSmaRat):
            if(COMPLEX)
            {   nsc = getSmaCom(n);
                msf = FltSmaRat(m);
                y = setSmaCom(cpow(nsc, msf));
            }
            break;

        case MIX(aSmaCom, aSmaFlt):
            if(COMPLEX)
            {   nsc = getSmaCom(n);
                msf = getSmaFlt(m);
                y = setSmaCom(cpow(nsc, msf));
            }
            break;

        case MIX(aSmaCom, aSmaCom):
            if(COMPLEX)
            {   nsc = getSmaCom(n);
                msc = getSmaCom(m);
                y = setSmaCom(cpow(nsc, msc));
            }
            break;
    }
    returnY
}



/* --------- Convert in-between number value types --------------------- */

static SmaRat RatSmaFlt (value v)
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
}

value toRat (value n)
{
    initY
    switch(getType(n))
    {
    case aSmaInt: y = setSmaRa2(getSmaInt(n),1); break;
    case aSmaRat: y = n; break;
    case aSmaFlt: y = setSmaRat(RatSmaFlt(n)); break;
    default: break;
    }
    returnY;
}

value toFlt (value n)
{
    initY
    switch(getType(n))
    {
    case aSmaInt: y = setSmaFlt(getSmaInt(n)); break;
    case aSmaRat: y = setSmaFlt(FltSmaRat(n)); break;
    case aSmaFlt: y = n; break;
    default: break;
    }
    returnY;
}



/*----------------- get array from value structure ----------------------*/

bool floatFromVst(SmaFlt* output, const value* input, int count, wchar* errormessage, const char* name)
{
    bool fail=false;
    int i;
    for(i=0; i<count; input++)
    {
        if(isSeptor(*input)) continue;
        value v = toFlt(*input);
        if(!IsSmaFlt(getType(v))) fail=true;
        else if(output) *output = getSmaFlt(v);
        i++; output++;
    }
    if(fail)
    {   char* str = (char*)ErrStr0;
        if(!name) name = "result";
        if(count==1) sprintf1(str, "Error: %s must be a real number.", name);
        else         sprintf1(str, "Error: %s must be %d real numbers.", name, count);
        strcpy21(errormessage, str);
    }
    return i==count;
}

bool integerFromVst (int* output, const value* input, int count, wchar* errormessage, const char* name)
{
    bool fail=false;
    int i;
    for(i=0; i<count; input++)
    {
        if(isSeptor(*input)) continue;
        value v = _floor(*input);
        if(!IsSmaInt(getType(v))) fail=true;
        else if(output) *output = getSmaInt(v);
        i++; output++;
    }
    if(fail)
    {   char* str = (char*)ErrStr0;
        if(!name) name = "result";
        if(count==1) sprintf1(str, "Error: %s must be an integer.", name);
        else         sprintf1(str, "Error: %s must be %d integers.", name, count); // TODO: test this part of the code
        strcpy21(errormessage, str);
    }
    return i==count;
}



/******************************************************************************************/

#define StrToValGet(base, a, b, d) \
    else if(c>=a && c<=b) \
    { \
        if(dp) deno *= base; \
        nume = nume*base + d + (int)(c-a); \
        if(nume<prev) { dp=4; break; } else prev=nume; \
    }

value StrToVal (const wchar* str) // single output value
{
    initY
    int i=0, dp=0, neg=0, length;
    SmaInt prev=0, nume=0, deno=1;
    wchar c=0;
    wchar dpt = *TWSF(DecimalPoint);

    length = strlen2(str);
    if(length<=0) return setNotval(L"String to number conversion: given string is empty.");

    if(str[i] == *TWSF(Negative)) { neg=1; i++; }

    if(str[i]=='0' && str[i+1]=='x')
    {
        for(i+=2; i < length; i++)
        {
            c = str[i];
            if(c == dpt) { dp++; if(dp==2) break; }
            StrToValGet(16, '0', '9', 0)
            StrToValGet(16, 'A', 'F', 10)
            StrToValGet(16, 'a', 'f', 10)
            else { dp=3; break; }
        }
    }
    else if(str[i]=='0' && str[i+1]=='b')
    {
        for(i+=2; i < length; i++) { c = str[i]; if(c == dpt) { dp++; if(dp==2) break; } StrToValGet(2, '0', '1', 0) else { dp=3; break; } }
    }
    else if(str[i]=='0' && str[i+1]=='o')
    {
        for(i+=2; i < length; i++) { c = str[i]; if(c == dpt) { dp++; if(dp==2) break; } StrToValGet(8, '0', '7', 0) else { dp=3; break; } }
    }
    else if(str[i] != *TWSF(DecimalPoint))
    {
        for(    ; i < length; i++) { c = str[i]; if(c == dpt) { dp++; if(dp==2) break; } StrToValGet(10, '0', '9', 0) else { dp=3; break; } }
    }

    else return setNotval(L"Number must not begin with a decimal point.");
    if(c==dpt
    && dp==1) return setNotval(L"Number must not end with a decimal point.");
    if(dp==2) return setNotval(L"Number has more than one decimal point.");
    if(dp==3) return setNotval(L"Number contains an invalid digit.");
    if(dp==4) return setNotval(L"Number has too many digits; BigNum is not yet supported.");

    if(neg) nume = -nume;
    if(deno==1) y = setSmaInt(nume);
    else
    {   SmaInt si = get_gcd(nume, deno);
        y = setSmaRa2(nume/si, deno/si);
    }
    return y;
}



static wchar* IntToStrGet (wchar* str, SmaInt n, char base)
{
    int len=0, off=0;

    if(n==(SmaInt)0x8000000000000000) return strcpy21(str, "inf");
    if(n<0)     // if number is negative
    {   n=-n;
        if(n<0) return strcpy21(str, "inf");
        str[len++] = *TWSF(Negative);
        off=1;  // offset for string-reverse()
    }

    switch(base)
    {
    case 16:
        off+=2; str[len++]='0'; str[len++]='x';
        while(1)
        {
            str[len++] = (wchar)(((n%16)<10) ? ((n%16)+'0') : ((n%16)-10+'A'));
            n /= 16; if(n==0) break;
        }
        break;
    case 2:
        off+=2; str[len++]='0'; str[len++]='b';
        while(1) { str[len++] = (wchar)(n%2) + '0'; n /= 2; if(n==0) break; }
        break;
    case 8:
        off+=2; str[len++]='0'; str[len++]='o';
        while(1) { str[len++] = (wchar)(n%8) + '0'; n /= 8; if(n==0) break; }
        break;
    default:
        while(1) { str[len++] = (wchar)(n%10) + '0'; n /= 10; if(n==0) break; }
        break;
    }
    str[len]=0;
    strrev2 (str+off, len-off);     // reverse the digits
    return str+len;
}



static wchar* FltToStrGet (wchar* output, SmaFlt n, char base)
{
    int i;
    SmaInt w, d;

    const SmaInt exp7 =
        (base== 2) ? 2*2*2*2*2*2*2 :
        (base== 8) ? 8*8*8*8*8*8*8 :
        (base==16) ? 0x10000000 : 10000000;

    if(n<0) { n = -n; *output++ = *TWSF(Negative); }    // make n positive
    w = (SmaInt)n;                          // get whole part of n
    if(w<0) return strcpy21(output, "inf"); // if n is too large
    n = n-w;                                // get decimal part of n
    d = (SmaInt)((1.0+n)*exp7);             // scale and get it into integer
    if(d%base >= base/2) d += base/2;       // round the last decimal digit

    if(d >= 2*exp7) { w++; d=exp7; }    // check if decimal part is close to 1
    output = IntToStrGet(output, w, base);
    if(d==exp7) return output;          // check if decimal part is 0

    *output++ = *TWSF(DecimalPoint);         // put decimal point
    IntToStrGet(output, d/base, base);      // remove last digit of d
    if(d==0) return ++output;

    i = 1 + (base==10 ? 0 : 2);
    while(output[i]!=0)         // shift so to remove
    {   *output = output[i];    // the characters 0x1
        output++;
    }
    // remove trailing zeros
    while(*(output-1)=='0') output--;
    *output = 0;
    return output;
}



static wchar* ValToStr (value n, wchar* output, char base, char wplaces, short dplaces)
{
    int i;
    SmaFlt sf;
    wchar temp[100];
    if(output==NULL) return NULL;
    if(base!=2 && base!=8 && base!=16) base=10;
    i = base==10 ? 0 : 2;

    switch(getType(n))
    {
        case aSmaInt: return IntToStrGet (output, getSmaInt(n), base);
        case aSmaRat: return FltToStrGet (output, FltSmaRat(n), base);
        case aSmaFlt: return FltToStrGet (output, getSmaFlt(n), base);
        case aSmaCom: break;
        default: *output=0; return output;
    }

    *output=0;
    FltToStrGet (output, creal(getSmaCom(n)), base);

    if((                  output[i+0]=='0' && output[i+1]==0)
    || (output[0]=='-' && output[i+1]=='0' && output[i+2]==0))
    {
        *output = 0; // if n == 0+bi

        sf = cimag(getSmaCom(n));
        if(sf<0)
        {   sf = -sf;
            strcpy22(output, TWSF(Negative));
        }
        temp[i]=0;
        FltToStrGet (temp, sf, base);

        output += strlen2(output);
        if(!(temp[i]=='1' && temp[i+1]==0)) // if n != 0+1i
            output = strcpy22(output, temp);
        output = strcpy22(output, TWSF(SQRT_of_Negative_One));
    }
    else
    {
        output += strlen2(output); // if n == a+bi

        sf = cimag(getSmaCom(n));
        if(sf<0)
        {   sf = -sf;
            strcpy22(output, TWSF(Minus));
        } else strcpy22(output, TWSF(Plus));

        temp[i]=0;
        FltToStrGet (temp, sf, base);

        output += strlen2(output);
        if(!(temp[i]=='1' && temp[i+1]==0)) // if n != a+1i
            output = strcpy22(output, temp);
        output = strcpy22(output, TWSF(SQRT_of_Negative_One));
    }
    *output = 0;
    return output;
}



static wchar* VstToStrGet (const value** vst_ptr, wchar* result, int info,
                           char base, char wplaces, short dplaces)
{
    const value* vst = *vst_ptr;
    const value* end = vst + VST_LEN(vst);

    if(info&4)
    {   while(1)
        {   vst++;
            if(vst>=end) { info &= ~4; break; }
            if(isSeptor(*vst) || isPoiter(*vst)) break;
        }
    }
    vst = *vst_ptr;
    (*vst_ptr)++;

    if(isPoiter(*vst))
    {   vst = getPoiter(*vst);
        result = VstToStrGet (&vst, result, info, base, wplaces, dplaces);
    }
    else if(!isSeptor(*vst))
    {
        char c = info&3;
        if(c==1 || c==3)
        {
            const char* s;
            switch(getType(*vst))
            {
            case aPoiter: s = "p"; break;
            case aString: s = "t"; break;
            case aSmaInt: s = c==1 ? "n" : "si"; break;
            case aSmaRat: s = c==1 ? "n" : "sr"; break;
            case aSmaFlt: s = c==1 ? "n" : "sf"; break;
            case aSmaCom: s = c==1 ? "n" : "sc"; break;
            case aBigInt: s = c==1 ? "n" : "bi"; break;
            case aBigRat: s = c==1 ? "n" : "br"; break;
            case aBigFlt: s = c==1 ? "n" : "bf"; break;
            case aBigCom: s = c==1 ? "n" : "bc"; break;
            default: s="?"; break;
            }
            while(*s) *result++ = *s++;
        }
        else if(isString(*vst))
        {
            const lchar* lstr = getString(*vst);
            if(c==0)
            {   strcpy23(result, lstr);
                result += strlen2(result);
            }
            else // else c==3
            {   *result++ = *TWSF(DoubleQuote);
                for( ; lstr->wchr!=0; lstr = lstr->next)
                {
                    wchar mc = lstr->wchr;
                         if(mc=='\\') { *result++ = '\\'; *result++ = '\\'; }
                    else if(mc== '"') { *result++ = '\\'; *result++ =  '"'; }
                    else *result++ = mc;
                }
                *result++ = *TWSF(DoubleQuote);
            }
        }
        else if(isPoiter(*vst))
        {
            const value* tvst = getPoiter(*vst);
            result = VstToStrGet (&tvst, result, info, base, wplaces, dplaces);
        }
        else result = ValToStr(*vst, result, base, wplaces, dplaces);
    }
    else
    {
        *result++ = *TWSF(Opened_Bracket_1);

        int info2 = info & ~4; // remove the put-newline mask
        result = VstToStrGet (vst_ptr, result, info2, base, wplaces, dplaces);

        while(*vst_ptr < end)
        {
            *result++ = *TWSF(CommaSeparator);
            if(info&4) { *result++ = '\r'; *result++ = '\n'; }
            *result++ = ' ';
            result = VstToStrGet (vst_ptr, result, info2, base, wplaces, dplaces);
        }
        *result++ = *TWSF(Closed_Bracket_1);
    }
    *result = 0;
    return result;
}

wchar* VstToStr (const value* vst, wchar* result, char info, // see function declaration in
                 char base, char wplaces, short dplaces)     // _math.h for explanation
{
    if(vst==NULL || result==NULL) return result;
    return VstToStrGet (&vst, result, info, base, wplaces, dplaces);
}

