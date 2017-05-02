/*
    value.c
*/

#include "_value.h"
#include "_string.h"
#include "_malloc.h"
#include "_math.h"


//---------------------------------------------------------------

value* value_alloc (long size)
{
    memory_alloc("value");
    return (value*)_malloc((size)*sizeof(value));
}

void value_free (value* vst)
{
    if(!vst) return;
    value *v, *end = vst + VST_LEN(vst);
    for(v=vst; v!=end; v++)
    {
        switch(getType(*v))
        {
            case aString: lchar_free(getString(*v)); break;
            default: break;
        }
    }
    //if(!avl_do(AVL_DEL, &allocated, &vst, sizeof(vst), avl_compare))
    //{ printf("Software Error in value_free(): pointer=%p does not exists.\n", vst); wait(); }
    /*int i = *((int*)vst-1);
    *(int**)vst = freevst[i];
    freevst[i] = (int*)vst;*/
    _free(vst);
    memory_freed("value");
}

void vst_copy (value* out, const value* in, long len)
{
    // note: len == VST_LEN(in)
    lchar *lou, *lin;
    for( ; len--; in++, out++)
    {
        switch(getType(*in))
        {
        case aString:
            lou = NULL;
            lin = getString(*in);
            astrcpy33(&lou, lin);
            *out = setString(lou);
            break;
        default: *out = *in; break;
        }
    }
}

//---------------------------------------------------------------

const char* vst_to_str (const value* vst)
{
    static char tstr[100];
    static char _str[1000];
    char* str = _str;
    long i, len;
    value v;
    int j;

    str[0]=0;
    if(vst==NULL) return _str;
    len = VST_LEN(vst);

    for(i=0; i<len; i++)
    {
        v = *vst;
        j = getType(v);
        switch((int)j)
        {
        case aPoiter: str += sprintf(str, "ptr(%d,%p)", j, (void*)getPoiter(v)); break;
        case aSeptor: str += sprintf(str, "sep(%d,%ld,%ld)", j, getSeptor(v).len, getSeptor(v).cols); break;
        case aString: strcpy13(tstr, getString(v)); str += sprintf(str, "t(%d,%s)", j, tstr); break;
        case aSmaInt: str += sprintf(str, "si(%d,%ld)", j, (long)getSmaInt(v)); break;
        case aSmaRat: str += sprintf(str, "sr(%d,%ld/%ld)", j, (long)getSmaRat(v).nume, (long)getSmaRat(v).deno); break;
        case aSmaFlt: str += sprintf(str, "sf(%d,%lf)", j, getSmaFlt(v)); break;
        case aSmaCom: str += sprintf(str, "sc(%d,~)", j); break;
        default:      str += sprintf(str, "Type=%d", j); break;
        }
        vst++;
    }
    return _str;
}



int valueSt_compare (const value* in1, const value* in2)
{
    int r=0;
    long i1=0, i2=0, len1, len2;
    char v1, v2;

    len1 = VST_LEN(in1);
    len2 = VST_LEN(in2);

    while(1)
    {
        if(i1==len1)
        { if(i2!=len2) r=7; break;
        } if(i2==len2) { r=7; break; }

        v1 = getType(*in1);
        v2 = getType(*in2);

        if(v1==v2)
        {
            i1+=1; in1+=1;
            i2+=1; in2+=1;
        }
        else if(v1==aSeptor)
        {
            i1+=VST_LEN(in1); in1+=VST_LEN(in1);
            i2+=1;   r|=1;    in2+=1;
        }
        else if(v2==aSeptor)
        {
            i1+=1;   r|=2;    in1+=1;
            i2+=VST_LEN(in2); in2+=VST_LEN(in2);
        }
        else
        {
            i1+=1; in1+=1;
            i2+=1; in2+=1;
        }
    }
    return r;
}



int value_compare (const value* in1, const value* in2)
{
    long i, j, size;
    char v1, v2;

    i = VST_LEN(in1);
    j = VST_LEN(in2);
    if(i<j) return -1;
    if(i>j) return +1;
    size=i;

    for(i=0; i<size; i++)
    {
        v1 = getType(*in1);
        v2 = getType(*in2);
        while(1) // not a loop
        {
            if(v1==aSeptor)
            { if(v2==aSeptor) break;
              else return -1;
            } if(v2==aSeptor) return +1;

            if(v1==aString)
            { if(v2==aString)
              {
                j = strcmp33 (getString(*in1), getString(*in2));
                if(j==0) break; else return (int)j;
              }
              else return -1;
            } if(v2==aString) return +1;

            if(!getSmaInt(equalTo(*in1, *in2))) return -1;

        break;
        }
        in1+=1;
        in2+=1;
    }
    return 0;
}



bool valueSt_matrix_getSize (const value* vst, int *rows, int *cols)
{
    int i, j, r, c;
    if(!vst) return false;

    r = VST_LEN(vst);
    if(r==0 || r==1) { *rows=r; *cols=r; return true; }

    r--; // skip Septor
    vst++;
    c = VST_LEN(vst);
    if(r%c) return false;
    r /= c;
    j = r;

    // check if a valid vector or matrix
    if(c==1) { j--;        { i=j; vst++; while(i--) { if(isSeptor(*vst)) return false; vst++; } } }
    else { c--; while(j--) { i=c; vst++; while(i--) { if(isSeptor(*vst)) return false; vst++; } } }

    *rows = r;
    *cols = c;
    return true;
}



void valueSt_matrix_setSize (value* vst, int rows, int cols)
{
    int i;
    value* A;
    long colslen;
    if(rows==1 && cols==1) return;

    colslen = (cols==1) ? 1 : (1+cols);
    *vst = setSepto2(1+rows*colslen, rows);

    if(cols!=1)
    {
        for(i=0; i < rows; i++)
        {
            A = MVE(vst,i,cols);
            *A = setSepto2(colslen, cols);
        }
    }
}



void valueSt_from_floats (value* vst, int rows, int cols, const SmaFlt* floats)
{
    int i, j;
    valueSt_matrix_setSize (vst, rows, cols);

    if(rows==1 && cols==1) { *vst = setSmaFlt(floats[0]); }
    else
    {
        for(i=0; i<rows; i++)
        for(j=0; j<cols; j++)
        {
            value v = setSmaFlt(floats[i*cols+j]);
            MVA(vst,i,j,cols) = v;
        }
    }
}



bool valueSt_get_position (int* position, const value* vst, const lchar* lstr)
{
    int j, index[20];
    const value* end[20];
    if(vst==NULL) return false;
    j=0;
    while(1)
    {
        if(isString(*vst)) // if leave node is reached
        {
            if(0==strcmp33(lstr, getString(*vst)))
            {
                index[0] = j;
                memcpy(position, index, sizeof(index));
                return true;
            }

            vst++;
            while(1)
            {
                if(j==0) break;
                if(vst==end[j]) j--;
                else { index[j]++; break; }
            }
            if(j==0) break;
        }
        else // if not leave node
        {
            ++j;  index[j] = 0;
            end[j] = vst + VST_LEN(vst);
            vst++;
        }
    }
    return false;
}



const value *VST11=0,
            *VST21,
            *VST31,
            *VST41,
            *VST61,
            *VST33;
void SET_VSTXX ()
{
    static value _VST11[0+1];
    static value _VST21[1+2];
    static value _VST31[1+3];
    static value _VST41[1+4];
    static value _VST61[1+6];
    static value _VST33[1+3*(1+3)];
    SmaFlt v[10] = {0};

    if(VST11) return;
    VST11 = _VST11;
    VST21 = _VST21;
    VST31 = _VST31;
    VST41 = _VST41;
    VST61 = _VST61;
    VST33 = _VST33;

    valueSt_from_floats (_VST11, 1, 1, v);
    valueSt_from_floats (_VST21, 2, 1, v);
    valueSt_from_floats (_VST31, 3, 1, v);
    valueSt_from_floats (_VST41, 4, 1, v);
    valueSt_from_floats (_VST61, 6, 1, v);
    valueSt_from_floats (_VST33, 3, 3, v);
}

