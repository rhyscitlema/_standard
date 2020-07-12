/*
    _texts.c

    TWSF = Text With Space False
    TWSF = Text With Space True

    NOTE: The code (mainly due to the algorithm) in this source file
    is quite hard-coded. It is essentially based on the important
    assumption of the structure in which Identities are.
*/

#include "_string.h"
#include "_math.h"
#include "_texts.h"


static const_Str2 default_twsf (enum RFET_LANGUAGE language);
static const_Str2 default_twst (enum RFET_LANGUAGE language);

static wchar loaded_TWSF[ID_TWSF_STOP][100 +1];
static wchar loaded_TWST[ID_TWST_STOP][1000+1];

const_Str2 TWSF (enum ID_TWSF ID) { return loaded_TWSF[ID]; }
const_Str2 TWST (enum ID_TWST ID) { return loaded_TWST[ID]; }


bool texts_load_twst (value stack, enum RFET_LANGUAGE language)
{
    int ID;
    const_Str2 str;
    const_Str2 data = default_twst(language);

    const_Str2 ending = L"[[]]";
    long endingSize = strlen2(ending)*sizeof(*ending);

    for( ; !strEnd2(data); data++)
    {
        // Load the ID number
        str = data;
        while(!strEnd2(data) && isDigit(*data)) data++;
        if(str == data) continue; // if no ID

        // Check if the number is space-enclosed
        if(!isSpace(*data) || !isSpace(*(str-1))) continue;

        // Get the ID number
        int n = data-str;
        wchar wstr[n+1];
        strcpy22S(wstr, str, n);
        ID = strToInt(wstr, stack);

        // If ID = 0 then ignore entire line
        if(ID==0)
        {
            while(!strEnd2(data) && *data!='\n') data++;
            continue;
        }

        // If ID number is invalid then ignore it
        if(!IS_TWST_ID(ID)) continue;

        // Check if ID was already loaded
        if(!strEnd2(loaded_TWST[ID])) continue;

        // Get to the first newline character
        while(!strEnd2(data) && *data!='\n') data++;
        if(strEnd2(data)) break;
        data++; // skip '\n'

        // Load message from after the newline, until when the ending sequence of
        // non-space characters is detected, or until when the end of data is detected.
        str = data;
        for( ; !strEnd2(data); data++)
            if(0==memcmp(data, ending, endingSize)) break;

        n = data-str;
        if(n > 1000)
        {
            const_Str2 argv[2];
            argv[0] = L"Error on loading TWST ID %s, text length must be <= 1000.";
            argv[1] = TIS2(0,ID);
            setMessage(stack, 0, 2, argv);
            return false;
        }
        // Set loaded message to the corresponding ID
        strcpy22S(loaded_TWST[ID], str, n);
    }
    return true;
}


bool texts_load_twsf (value stack, enum RFET_LANGUAGE language)
{
    int ID;
    const_Str2 str;
    const_Str2 data = default_twsf(language);

    for( ; !strEnd2(data); data++)
    {
        // Load the ID number
        str = data;
        while(!strEnd2(data) && isDigit(*data)) data++;
        if(str == data) continue; // if no ID

        // Check if the number is space-enclosed
        if(!isSpace(*data) || !isSpace(*(str-1))) continue;

        // Get the ID number
        int n = data-str;
        wchar wstr[n+1];
        strcpy22S(wstr, str, n);
        ID = strToInt(wstr, stack);

        // If ID = 0 then ignore entire line
        if(ID==0)
        {
            while(!strEnd2(data) && *data!='\n') data++;
            continue;
        }

        // If ID number is invalid then ignore it
        if(!IS_TWSF_ID(ID)) continue;

        // Check if ID was already loaded
        if(!strEnd2(loaded_TWSF[ID])) continue;

        // Get to the first non-space character
        while(!strEnd2(data) && isSpace(*data)) data++;
        if(strEnd2(data)) break;

        // Load the sequence of non-space character that follows
        str = data;
        while(!strEnd2(data) && !isSpace(*data)) data++;

        n = data-str;
        if(n > 100)
        {
            const_Str2 argv[2];
            argv[0] = L"Error on loading TWSF ID %s, text length must be <= 100.";
            argv[1] = TIS2(0,ID);
            setMessage(stack, 0, 2, argv);
            return false;
        }
        // Set loaded message to the corresponding ID
        strcpy22S(loaded_TWSF[ID], str, n);
    }
    return true;
}


bool IS_TWSF_ID (int ID)
{
    switch(ID)
    {
    case OpenedBracket1:
    case OpenedBracket2:
    case OpenedBracket3:
    case ClosedBracket1:
    case ClosedBracket2:
    case ClosedBracket3:

    case EndOfStatement:
    case CommaSeparator:
    case Concatenate:
    case Replacement:
    case ConditionAsk:
    case ConditionChoose:
    case DecimalPoint:
    case DoubleQuote:

    case Logical_OR:
    case Logical_AND:
    case Logical_NOT:
    case Oper_mod:

    case Assignment:
    case EqualTo:
    case SameAs:
    case NotSame:
    case NotEqual:
    case LessThan:
    case GreaterThan:
    case LessOrEqual:
    case GreaterOrEqual:

    case Oper_pos:
    case Oper_neg:
    case Oper_add:
    case Oper_sub:

    case Oper_mul1:
    case Oper_mul2:
    case Oper_div1:
    case Oper_divI:
    case Oper_div2:
    case Oper_pow1:
    case Oper_pow2:

    case Shift_Left:
    case Shift_Right:
    case Bitwise_XOR:
    case Bitwise_OR:
    case Bitwise_AND:
    case Bitwise_NOT:

    case Oper_dotproduct:
    case Oper_transpose:

    case Constant_this:
    case Constant_true:
    case Constant_false:
    case Constant_e_2_718_:
    case Constant_pi_3_141_:
    case SQRT_of_Neg_One:

    case Function_factorial:
    case Function_fullfloor:
    case Function_getprimes:
    case Function_srand:
    case Function_rand:

    case Function_gcd:
    case Function_ilog:
    case Function_isqrt:
    case Function_floor:
    case Function_ceil:

    case Function_sqrt:
    case Function_cbrt:
    case Function_exp:
    case Function_log:

    case Function_cos:
    case Function_sin:
    case Function_tan:
    case Function_acos:
    case Function_asin:
    case Function_atan:

    case Function_cosh:
    case Function_sinh:
    case Function_tanh:
    case Function_acosh:
    case Function_asinh:
    case Function_atanh:

    case Function_cabs:
    case Function_carg:
    case Function_real:
    case Function_imag:
    case Function_conj:
    case Function_proj:

    case Function_size:
    case Function_span:
    case Function_sum:
    case Function_max:
    case Function_min:

    case Function_vector:
    case Function_range:
    case Function_try:

    case Function_tostr:
    case Function_tonum:
    case Function_toint:
    case Function_torat:
    case Function_toflt:

    case Function_eval:
    case Function_call:
    case Function_print:
    case Function_strlen: break;

    default: return false;
    }
    return true;
}


bool IS_TWST_ID (int ID)
{
    switch(ID)
    {
    case Cannot_Find_Component:
    case Component_Already_Defined:

    case IsNot_ValueStructure:
    case Invalid_Expression_End:
    case Invalid_Expression_Syntax:

    case Bracket_Match_Invalid:
    case Bracket_Match_None:
    case Lacking_Bracket:

    case Operands_DoNot_Match:
    case Operand_IsNot_Matrix:
    case Argument_vs_Parameter:
    case ResultSt_vs_Expected:

    case Index_OutOf_Range:
    case Vector_Middle_Value:
    case Vector_Length_Invalid:

    case Left_IsNot_Matrix:
    case Right_IsNot_Matrix:
    case MatrixMult_IsInvalid:
    case Matrix_IsNot_NbyN:
    case Left_IsNot_Single:
    case Right_IsNot_Single:

    case Expect_Question_Before:
    case Expect_Choice_After:
    case Condition_IsNot_Single:

    case Division_By_Zero:
    case Operand_Not_Supported:
    case Only_On_Integer_Number:
    case Argument_OutOf_Domain: break;

    default: return false;
    }
    return true;
}


/*
0 Below are texts 'with space allowed' that are used by the software.
0
0 They are structured as:
0
0    <Identity Number>    <ignored... can be used for small comments>
0<target text> <Ending sequence of non-space characters> <ignored until next ID>
0
0
0 They are loaded as follows:
0
0 1) Any content is ignored until a space-enclosed number is detected.
0
0 2) If the detected number is 0 then the line that follows is ignored.
0
0 3) If the detected number is 1 then the first sequence
0    of non-space characters which follows is loaded.
0    Then we go to step 1 and detect the next number.
0
0 4) Any content until the first encounter of a newline is ignored.
0
0 5) If the detected number is a valid Identity Number
0    as specified by the software provider, then
0    extraction starts from just after the newline.
0    If not then we go to step 1 and detect the next number.
0
0 5) Extraction stops when the End_Of_Text sequence of non-space
0    characters, as specified by Indenty Number 1, is detected.
0    Then we go to step 1 and detect the next number.
0
0 Note: The place-holders are specified using: \<number>.
0       For example \1 means: give special meaning to the number 1.
0       This special meaning is: the place-holder for the first argument.
0       Here the 1st, 2nd and 3rd arguments are the file, line and column.
0
0 Note: If an Identity Number is not found then the default setting,
0       as built in the software, is used. This setting is actually
0       the original version of this file as given by the software.
*/
static const wchar default_twst_ENGLISH[] =
   L"\r\n"
    " 10  Cannot_Find_Component\r\n"
    "Error on '%1' at (%2,%3) in %4:\r\n"
    "Cannot find component from %s.[[]]\r\n"
    "\r\n"
    " 11  Component_Already_Defined\r\n"
    "Error on '%1' at (%2,%3) in %4:\r\n"
    "Component is already defined.[[]]\r\n"
    "\r\n"
    "\r\n"
    " 21  IsNot_ValueStructure\r\n"
    "Error on '%1' at (%2,%3) in %4:\r\n"
    "Must be a bracket, comma or name.[[]]\r\n"
    "\r\n"
    " 22  Invalid_Expression_End\r\n"
    "Error at (%2,%3) in %4:\r\n"
    "Invalid end of expression.[[]]\r\n"
    "\r\n"
    " 23  Invalid_Expression_Syntax\r\n"
    "Error at (%2,%3) in %4:\r\n"
    "Invalid syntax due to '%1'.[[]]\r\n"
    "\r\n"
    "\r\n"
    " 30  Bracket_Match_Invalid\r\n"
    "Error on '%1' at (%2,%3) in %4:\r\n"
    "Expected '%5' instead.[[]]\r\n"
    "\r\n"
    " 31  Bracket_Match_None\r\n"
    "Error on '%1' at (%2,%3) in %4:\r\n"
    "Does not have a matching '%5'.[[]]\r\n"
    "\r\n"
    " 32  Lacking_Bracket\r\n"
    "Error at (%2,%3) in %4:\r\n"
    "Expect %s closed bracket%s at end of expression.[[]]\r\n"
    "\r\n"
    "\r\n"
    " 51  Operands_DoNot_Match\r\n"
    "Left and right operands do not match.[[]]\r\n"
    "\r\n"
    " 52  Operand_IsNot_Matrix\r\n"
    "Operand is not a vector nor a matrix.[[]]\r\n"
    "\r\n"
    " 53  Argument_vs_Parameter\r\n"
    "Argument structure is different from parameter structure.\r\n"
    "Argument structure is %s instead of %s.[[]]\r\n"
    "\r\n"
    " 54  ResultSt_vs_Expected\r\n"
    "Result structure is different from expected.\r\n"
    "Result structure is %s not %s.[[]]\r\n"
    "\r\n"
    "\r\n"
    " 55  Index_OutOf_Range\r\n"
    "Indexing number = %s is not in range [0,%s].[[]]\r\n"
    "\r\n"
    " 56  Vector_Middle_Value\r\n"
    "The sum of the middle argument must be non-zero.[[]]\r\n"
    "\r\n"
    " 57  Vector_Length_Invalid\r\n"
    "Vector length must be a single integer > 0.[[]]\r\n"
    "\r\n"
    "\r\n"
    " 70  Left_IsNot_Matrix\r\n"
    "Left operand is not a vector nor a matrix.\r\n"
    "If per-value operation then use '%s' instead.[[]]\r\n"
    "\r\n"
    " 71  Right_IsNot_Matrix\r\n"
    "Right operand is not a vector nor a matrix.\r\n"
    "If per-value operation then use '%s' instead.[[]]\r\n"
    "\r\n"
    " 72  MatrixMult_IsInvalid\r\n"
    "Matrix multiplication (%s by %s)(%s by %s) is invalid.[[]]\r\n"
    "\r\n"
    " 73  Matrix_IsNot_NbyN\r\n"
    "Operand is a (%s by %s) matrix, not an (n by n) matrix.[[]]\r\n"
    "\r\n"
    " 74  Left_IsNot_Single\r\n"
    "Left operand must be a single value.[[]]\r\n"
    "\r\n"
    " 75  Right_IsNot_Single\r\n"
    "Right operand must be a single value.[[]]\r\n"
    "\r\n"
    "\r\n"
    " 80  Expect_Question_Before\r\n"
    "Error on '%1' at (%2,%3) in %4:\r\n"
    "Expected '?' before it.[[]]\r\n"
    "\r\n"
    " 81  Expect_Choice_After\r\n"
    "Error on '%1' at (%2,%3) in %4:\r\n"
    "Expected ':' after it.[[]]\r\n"
    "\r\n"
    " 82 Condition_IsNot_Single\r\n"
    "Condition must evalute to a single value.[[]]\r\n"
    "\r\n"
    "\r\n"
    " 90  Division_By_Zero\r\n"
    "Division by zero.[[]]\r\n"
    "\r\n"
    " 91  Operand_Not_Supported\r\n"
    "Operand value type is not supported.[[]]\r\n"
    "\r\n"
    " 92  Only_On_Integer_Number\r\n"
    "Operand must be an integer.[[]]\r\n"
    "\r\n"
    " 93  Argument_OutOf_Domain\r\n"
    "Argument is out of domain.[[]]\r\n"
    "\r\n";

/*
0 Below are texts 'with no space' that are used by the software.
0
0 They are structured as:
0
0 <ID Number> <non-space-separated characters> <ignored until next ID>
0
0
0 They are loaded as follows:
0
0 1) Any content is ignored until a space-enclosed number is detected.
0
0 2) If the detected number is 0 then the line that follows is ignored.
0
0 3) If the detected number is a valid Identity Number,
0    as specified by the software provider, then the first
0    sequence of non-space characters that follows is loaded.
0
0 4) Then we go to step 1 and detect the next number.
0
0 Note: Some characters like the decimal point '.' are
0       specified to be (and must remain) single characters.
0
0 Note: The names that have alphabet characters, like "sin"
0       must not have non-alphabet characters, like "=+*"
0
0 Note: If an Identity Number is not found then the default setting,
0       as built in the software, is used. This setting is actually
0       the original version of this file as given by the software.
*/
static const wchar default_twsf_ENGLISH[] =
   L"\r\n"
    " 1 (        OpenedBracket1 , do not change this\r\n"
    " 2 {        OpenedBracket2 , do not change this\r\n"
    " 3 [        OpenedBracket3 , do not change this\r\n"
    " 4 )        ClosedBracket1 , do not change this\r\n"
    " 5 }        ClosedBracket2 , do not change this\r\n"
    " 6 ]        ClosedBracket3 , do not change this\r\n"
    "\r\n"
    " 10 ;       EndOfStatement , do not change this\r\n"
    " 11 ,       CommaSeparator , do not change this\r\n"
    " 12 .,      Concatenate    , do not change this\r\n"
    " 13 :=      Replacement    , do not change this\r\n"
    " 14 ?       ConditionAsk   , do not change this\r\n"
    " 15 :       ConditionChoose, do not change this\r\n"
    " 16 .       DecimalPoint   , do not change this\r\n"
    " 17 \"       DoubleQuote    , do not change this\r\n"
    "\r\n"
    " 20 or      Logical_OR\r\n"
    " 21 and     Logical_AND\r\n"
    " 22 not     Logical_NOT\r\n"
    " 23 mod     Oper_mod\r\n"
    "\r\n"
    " 30 =       Assignment     , do not change this\r\n"
    " 31 ==      EqualTo\r\n"
    " 32 ===     SameAs\r\n"
    " 33 !==     NotSame\r\n"
    " 34 !=      NotEqual\r\n"
    " 35 <       LessThan\r\n"
    " 36 >       GreaterThan\r\n"
    " 37 <=      LessOrEqual\r\n"
    " 38 >=      GreaterOrEqual\r\n"
    "\r\n"
    " 40 +       Oper_pos   , do not change this\r\n"
    " 41 -       Oper_neg   , do not change this\r\n"
    " 42 +       Oper_add   , do not change this\r\n"
    " 43 -       Oper_sub   , do not change this\r\n"
    "\r\n"
    " 50 *       Oper_mul1\r\n"
    " 51 .*      Oper_mul2\r\n"
    " 52 /       Oper_div1\r\n"
    " 53 //      Oper_divI\r\n"
    " 54 ./      Oper_div2\r\n"
    " 55 ^       Oper_pow1\r\n"
    " 56 .^      Oper_pow2\r\n"
    "\r\n"
    " 60 |       Bitwise_OR\r\n"
    " 61 ^|      Bitwise_XOR\r\n"
    " 62 &       Bitwise_AND\r\n"
    " 63 ~       Bitwise_NOT\r\n"
    " 64 >>      Shift_Right\r\n"
    " 65 <<      Shift_Left\r\n"
    "\r\n"
    " 70 •       Oper_dotproduct \\u2022\r\n"
    " 71 ^T      Oper_transpose\r\n"
    "\r\n"
    " 79 this        \r\n"
    " 80 true        \r\n"
    " 81 false       \r\n"
    " 82 e           \r\n"
    " 83 pi          \r\n"
    " 84 i           \r\n"
    "\r\n"
    " 90 factorial   \r\n"
    " 91 fullfloor   \r\n"
    " 92 getprimes   \r\n"
    " 93 srand       \r\n"
    " 94 rand        \r\n"
    "\r\n"
    " 95 gcd         \r\n"
    " 96 ilog        \r\n"
    " 97 isqrt       \r\n"
    " 98 floor       \r\n"
    " 99 ceil        \r\n"
    "\r\n"
    " 100 sqrt       \r\n"
    " 101 cbrt       \r\n"
    " 102 exp        \r\n"
    " 103 log        \r\n"
    "\r\n"
    " 110 cos        \r\n"
    " 111 sin        \r\n"
    " 112 tan        \r\n"
    " 113 acos       \r\n"
    " 114 asin       \r\n"
    " 115 atan       \r\n"
    "\r\n"
    " 116 cosh       \r\n"
    " 117 sinh       \r\n"
    " 118 tanh       \r\n"
    " 119 acosh      \r\n"
    " 120 asinh      \r\n"
    " 121 atanh      \r\n"
    "\r\n"
    " 122 cabs       \r\n"
    " 123 carg       \r\n"
    " 124 real       \r\n"
    " 125 imag       \r\n"
    " 126 conj       \r\n"
    " 127 proj       \r\n"
    "\r\n"
    " 140 size       \r\n"
    " 141 span       \r\n"
    " 142 sum        \r\n"
    " 143 max        \r\n"
    " 144 min        \r\n"
    "\r\n"
    " 150 vector     \r\n"
    " 151 range      \r\n"
    " 152 try        \r\n"
    "\r\n"
    " 160 tostr      \r\n"
    " 161 tonum      \r\n"
    " 162 toint      \r\n"
    " 163 torat      \r\n"
    " 164 toflt      \r\n"
    "\r\n"
    " 170 eval       \r\n"
    " 171 call       \r\n"
    " 172 print      \r\n"
    " 173 strlen     \r\n"
    "\r\n";

static const wchar default_twst_FRENCH[] = L"";
static const wchar default_twsf_FRENCH[] = L"";


static const_Str2 default_twsf (enum RFET_LANGUAGE language)
{
    const wchar* str;
    switch(language)
    {
        case RFET_FRENCH: str = default_twsf_FRENCH; break;
        default:     str = default_twsf_ENGLISH; break;
    }
    return str;
}

static const_Str2 default_twst (enum RFET_LANGUAGE language)
{
    const wchar* str;
    switch(language)
    {
        case RFET_FRENCH: str = default_twst_FRENCH; break;
        default:     str = default_twst_ENGLISH; break;
    }
    return str;
}

