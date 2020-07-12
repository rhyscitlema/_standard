/*
	test.c

	To test the lib_std _math.h functions.
*/
#include <stdio.h>
#include "_math.h"
#include "_texts.h"
#include "_stdio.h"
#include "_string.h"

/* get next command line argument */
const char* GetWord1 (char* out, const char* in)
{
	bool escape=0;
	int onLoad=0;
	while(in && *in)
	{
		wchar c = *in;
		if(!escape && c=='"')
		{
			if(onLoad){
				if(out) *out++ = c;
				in++; break;
			}else onLoad=2;
		}
		if(!escape && isSpace(c))
			{ if(onLoad==1) break; }
		else if(onLoad==0) { onLoad=1; }
		escape = !escape && (c=='\\');
		if(onLoad && out) *out++ = c;
		in++;
	}
	if(out) *out=0;
	return in;
}

static bool equal (const char* a, const char* b) { return 0==strcmp11(a, b); }


int evaluate (int count, char** input)
{
	uint32_t stack[1000];
	texts_load_twsf(stack, RFET_ENGLISH);
	texts_load_twst(stack, RFET_ENGLISH);

	memset(stack,0,8*sizeof(*stack));
	const_Str1 in = input[1];
	const_Str2 argv[3];
	value P=stack, try, v;
	try = P;
	v = P+8;

	while(true)
	{
		char w[100];
		in = GetWord1(w, in);

		if(strEnd1(w)) // return from function call
		{
			value p = P;
			memcpy(&in, p+4, sizeof(in));
			try = p-p[3];
			P   = p-p[2];
			p   = p-p[1];
			v = vPrevCopy(p, v);
			if(in==NULL) break;
			else continue;
		}
		else if(isDigit(w[0])
		|| (w[0]=='-' && isDigit(w[1]))
		|| (w[0]=='\'') // if a character
		|| (w[0]=='"')) // if a string
			v = StrToVal(v, C21(w));

		else if(w[0]=='p' && w[1]=='_') // if a parameter
		{
			uint32_t i = w[2] - '0';
			if(i<P[0])
				v = vCopy(v, P - *(P-1-i));
			else {
				argv[0] = L"Request for parameter %s not in range [0, %s).";
				argv[1] = TIS2(0,i);
				argv[2] = TIS2(0,P[0]);
				v = setMessage(v, 0, 3, argv);
			}
		}
		else if(w[0]=='f' && w[1]=='_') // if a function
		{
			int f = w[2] - '0'; assert(0<=f && f<=9);
			int C = w[4] - '0'; assert(0<=C && C<=9);
			value p = v + C;

			const_value n = v;
			if(C) { // get to function arguments
				v = vPrev(v);
				n = vGet(v);
			}
			// push values to be recovered on return
			// see struct OperEval in expression.h
			p[0] = C;
			p[1] = p-v;
			p[2] = p-P;
			p[3] = p-try;
			memcpy(p+4, &in, sizeof(in));

			int i;
			if(!C) i=0; // if a variable instead.
			else if(VTYPE(*n)==VALUE_VECTOR){
				i = *n & 0x0FFFFFFF;
				n += 2;
			} else i=1;

			if(i!=C)
			{
				argv[0] = L"Expected %s not %s function arguments.";
				argv[1] = TIS2(0,C);
				argv[2] = TIS2(1,i);
				v = setMessage(v, 0, 3, argv);
			}
			else if(f >= count-2)
			{
				argv[0] = L"Request for function %s not in range [0, %s).";
				argv[1] = TIS2(0,f);
				argv[2] = TIS2(0,count-2);
				v = setMessage(v, 0, 3, argv);
			}
			else
			{
				for(i=0; i<C; i++)
				{
					*(p-1-i) = p-n;
					n = vNext(n);
				}
				P = p;
				try = P;
				v = P+8;
				in = input[2+f];
				continue;
			}
		}

		else if(w[0]==',') // if separator
		{
			int count = strToInt(C21(w+1), v);
			v = tovector(v, count);
		}

		else if(w[0]=='?')      // if condition_ask
		{
			v = vPrev(v);
			const_value n = vGet(v);
			if(!isBool(n)) // if result is not boolean
			{
				assert(VTYPE(*n)==VALUE_VECTOR);
				argv[0] = TWST(Condition_IsNot_Single);
				v = setMessage(v, 0, 1, argv);
			}
			else
			{
				int count=1;
				if(*n & 1)
				while(true)
				{
					in = GetWord1(w, in);
					if(strEnd1(in)) break;
					if(w[0]=='?') count++;
					if(w[0]==':') count--;
					if(count==0) break;
				}
				printf("{%s %s}\r\n", w, (*n&1)?"false":"true");
				continue;
			}
		}
		else if(w[0]==':') // if condition_choose
		{
			int count=1;
			while(true)
			{
				in = GetWord1(w, in);
				if(strEnd1(in)) break;
				if(w[0]=='?') count++;
				if(w[0]==';') count--;
				if(count==0) break;
			}
			continue;
		}
		else if(w[0]==';') continue; // if condition_end

		else if(equal(w, "try"))
		{
			if(try==P) { try=v; continue; }
			else
			{
				argv[0] = L"Nested try-catch is not allowed.";
				v = setMessage(v, 0, 1, argv);
				while(!*in) in++;
			}
		}
		else if(equal(w, "that"))
		{
			while(true)
			{
				in = GetWord1(w, in);
				if(strEnd1(in)) break;
				if(equal(w, "catch")) { try=P; break; }
			}
			continue;
		}
		else if(equal(w, "catch")) { try=P; continue; }

		else if(equal(w, "+ve"  )) v = _pos(v);
		else if(equal(w, "-ve"  )) v = _neg(v);
		else if(equal(w, "*"    )) v = __mul(v);
		else if(equal(w, "/"    )) v = __div(v);
		else if(equal(w, "^"    )) v = power(v);

		else if(equal(w, "+"    )) v = _add(v);
		else if(equal(w, "-"    )) v = _sub(v);
		else if(equal(w, ".*"   )) v = _mul(v);
		else if(equal(w, "./"   )) v = _div(v);
		else if(equal(w, ".^"   )) v = _pow(v);

		else if(equal(w, "=="   )) v = equalTo(v);
		else if(equal(w, "==="  )) v = sameAs(v);
		else if(equal(w, "!=="  )) v = notSame(v);
		else if(equal(w, "!="   )) v = notEqual(v);
		else if(equal(w, "<"    )) v = lessThan(v);
		else if(equal(w, ">"    )) v = greaterThan(v);
		else if(equal(w, "<="   )) v = lessOrEqual(v);
		else if(equal(w, ">="   )) v = greaterOrEqual(v);

		else if(equal(w, "<<"   )) v = shift_left(v);
		else if(equal(w, ">>"   )) v = shift_right(v);
		else if(equal(w, "^|"   )) v = bitwise_xor(v);
		else if(equal(w, "|"    )) v = bitwise_or(v);
		else if(equal(w, "&"    )) v = bitwise_and(v);
		else if(equal(w, "~"    )) v = bitwise_not(v);

		else if(equal(w, ".,"   )) v = combine(v);
		else if(equal(w, "[]"   )) v = indexing(v);
		else if(equal(w, "^T"   )) v = transpose(v);
		else if(equal(w, "or"   )) v = logical_or(v);
		else if(equal(w, "and"  )) v = logical_and(v);
		else if(equal(w, "not"  )) v = logical_not(v);

		else if(equal(w, "//"   )) v = _idiv(v);
		else if(equal(w, "mod"  )) v = _mod(v);
		else if(equal(w, "gcd"  )) v = _gcd(v);
		else if(equal(w, "ilog" )) v = _ilog(v);
		else if(equal(w, "isqrt")) v = _isqrt(v);
		else if(equal(w, "floor")) v = _floor(v);
		else if(equal(w, "ceil" )) v = _ceil(v);

		else if(equal(w, "facto" )) v = factorial(v);
		else if(equal(w, "ffloor")) v = fullfloor(v);
		else if(equal(w, "primes")) v = getprimes(v);
		else if(equal(w, "vector")) v = _vector(v);
		else if(equal(w, "range" )) v = _range(v);

		else if(equal(w, "sqrt" )) v = _sqrt(v);
		else if(equal(w, "cbrt" )) v = _cbrt(v);
		else if(equal(w, "exp"  )) v = _exp(v);
		else if(equal(w, "log"  )) v = _log(v);

		else if(equal(w, "cos"  )) v = _cos(v);
		else if(equal(w, "sin"  )) v = _sin(v);
		else if(equal(w, "tan"  )) v = _tan(v);
		else if(equal(w, "acos" )) v = _acos(v);
		else if(equal(w, "asin" )) v = _asin(v);
		else if(equal(w, "atan" )) v = _atan(v);

		else if(equal(w, "cosh" )) v = _cosh(v);
		else if(equal(w, "sinh" )) v = _sinh(v);
		else if(equal(w, "tanh" )) v = _tanh(v);
		else if(equal(w, "acosh")) v = _acosh(v);
		else if(equal(w, "asinh")) v = _asinh(v);
		else if(equal(w, "atanh")) v = _atanh(v);

		else if(equal(w, "cabs" )) v = _cabs(v);
		else if(equal(w, "carg" )) v = _carg(v);
		else if(equal(w, "real" )) v = _real(v);
		else if(equal(w, "imag" )) v = _imag(v);
		else if(equal(w, "conj" )) v = _conj(v);
		else if(equal(w, "proj" )) v = _proj(v);

		else if(equal(w, "true" )) v = setBool(v, true);
		else if(equal(w, "false")) v = setBool(v, false);
		else if(equal(w, "pi"   )) v = _acos(setSmaInt(v,-1));
		else if(equal(w, "e"    )) v = _exp (setSmaInt(v, 1));
		else if(equal(w, "i"    )) v = _sqrt(setSmaInt(v,-1));

		else if(equal(w, "size" )) v = _size(v);
		else if(equal(w, "span" )) v = _span(v);
		else if(equal(w, "sum"  )) v = _sum(v);
		else if(equal(w, "max"  )) v = _max(v);
		else if(equal(w, "min"  )) v = _min(v);

		else if(equal(w, "tostr" )) v = toStr(v);
		else if(equal(w, "tonum" )) v = toNum(v);
		else if(equal(w, "torat" )) v = toRat(v);
		else if(equal(w, "toflt" )) v = toFlt(v);

		else {
			argv[0] = L"Error: unknown operation [%s].";
			argv[1] = C21(w);
			v = setMessage(v, 0, 2, argv);
		}

		const_Str2 out;
		if(VERROR(v)) out = getMessage(vGetPrev(v));
		else out = getStr2(vGetPrev(VstToStr(setRef(v, vPrev(v)), TOSTR_NEWLINE)));
		printf("[%s] ", w);
		puts2(out);

		if(VERROR(v))
		{
			while(true)
			{
				in = GetWord1(w, in);
				if(strEnd1(in)) break;
				if(equal(w, "that")) { v=try; break; }
				if(equal(w, "catch")) { while(!*in) in++; break; }
			}
			if(v==try) continue;
		}
	}
	return 0;
}


int main (int argc, char** argv)
{
	if(argc>1) return evaluate(argc, argv);

	printf("Provide prefix expression. Examples:\r\n");
	printf("    <program> \"1 1 +\"\r\n");
	printf("    <program> \"5 6 2 / - 3 4 * +\"\r\n");
	printf("    <program> \"0x10 4 *  \\\"string\\\" +\"\r\n");
	printf("    <program> \"2 3 4 ,3 20 30 40 ,3 +\"\r\n");
	printf("Supported operations:       \r\n");
	printf("    +ve -ve  *   /   ^   ===\r\n");
	printf("    +   -   .*  ./  .^   !==\r\n");
	printf("    ==  !=   <   >   <=  >= \r\n");
	printf("    <<  >>  ^|   |   &   ~  \r\n");
	printf("    .,  []  ^T  or  and  not\r\n");
	printf("    //    mod   gcd   ilog  \r\n");
	printf("    isqrt floor ceil  facto \r\n");
	printf("    sqrt  cbrt  exp   log   \r\n");
	printf("    cos   sin   tan   acos  asin  atan  \r\n");
	printf("    cosh  sinh  tan   acosh asinh atanh \r\n");
	printf("    cabs  carg  real  imag  conj  proj  \r\n");
	printf("    true  false pi    e      i          \r\n");
	printf("    size  len   sum   max    min        \r\n");
	printf("    range vector primes ffloor          \r\n");
	printf("    tostr tonum  torat  toflt           \r\n");
	return 0;
}


void user_alert (const wchar* title, const wchar* message)
{
	#ifdef LOCAL_USER
	printf("\r\n>>> "); puts2(title); puts2(message);
	printf("Press Enter to continue..."); getchar();
	#endif
}

bool user_confirm (const wchar* title, const wchar* message)
{
	#ifdef LOCAL_USER
	char buffer[10];
	printf("\r\n>>> "); puts2(title); puts2(message);
	printf("Enter ok / <anything> : ");
	if(!fgets(buffer, sizeof(buffer), stdin)) return false;
	return 0==strcmp(buffer, "ok\n");
	#else
	return false;
	#endif
}
