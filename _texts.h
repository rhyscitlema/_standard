#ifndef __TEXTS_H
#define __TEXTS_H
/*
    _texts.h

    TWSF = Text With Space False
    TWSF = Text With Space True
*/

#include "_stddef.h"


enum ID_TWSF
{
    ID_TWSF_START       = 0,

    OpenedBracket1      = 1,
    OpenedBracket2      = 2,
    OpenedBracket3      = 3,
    ClosedBracket1      = 4,
    ClosedBracket2      = 5,
    ClosedBracket3      = 6,

    EndOfStatement      = 10,
    CommaSeparator      = 11,
    Concatenate         = 12,
    Replacement         = 13,
    ConditionAsk        = 14,
    ConditionChoose     = 15,
    DecimalPoint        = 16,
    DoubleQuote         = 17,

    ReplaceRecord       = 18,
    OperJustJump        = 19,

    Logical_OR          = 20,
    Logical_AND         = 21,
    Logical_NOT         = 22,
    Oper_mod            = 23,

    Assignment          = 30,
    EqualTo             = 31,
    SameAs              = 32,
    NotSame             = 33,
    NotEqual            = 34,
    LessThan            = 35,
    GreaterThan         = 36,
    LessOrEqual         = 37,
    GreaterOrEqual      = 38,

    Oper_pos            = 40,
    Oper_neg            = 41,
    Oper_add            = 42,
    Oper_sub            = 43,

    Oper_mul0           = 49,
    Oper_mul1           = 50,
    Oper_mul2           = 51,
    Oper_div1           = 52,
    Oper_divI           = 53,
    Oper_div2           = 54,
    Oper_pow1           = 55,
    Oper_pow2           = 56,

    Bitwise_OR          = 60,
    Bitwise_XOR         = 61,
    Bitwise_AND         = 62,
    Bitwise_NOT         = 63,
    Shift_Right         = 64,
    Shift_Left          = 65,

    Oper_dotproduct     = 70,
    Oper_transpose      = 71,
    Oper_indexing       = 72,

    Constant_this       = 79,
    Constant_true       = 80,
    Constant_false      = 81,
    Constant_e_2_718_   = 82,
    Constant_pi_3_141_  = 83,
    SQRT_of_Neg_One     = 84,

    Function_factorial  = 90,
    Function_fullfloor  = 91,
    Function_getprimes  = 92,
    Function_srand      = 93,
    Function_rand       = 94,

    Function_gcd        = 95,
    Function_ilog       = 96,
    Function_isqrt      = 97,
    Function_floor      = 98,
    Function_ceil       = 99,

    Function_sqrt       = 100,
    Function_cbrt       = 101,
    Function_exp        = 102,
    Function_log        = 103,

    Function_cos        = 110,
    Function_sin        = 111,
    Function_tan        = 112,
    Function_acos       = 113,
    Function_asin       = 114,
    Function_atan       = 115,

    Function_cosh       = 116,
    Function_sinh       = 117,
    Function_tanh       = 118,
    Function_acosh      = 119,
    Function_asinh      = 120,
    Function_atanh      = 121,

    Function_cabs       = 122,
    Function_carg       = 123,
    Function_real       = 124,
    Function_imag       = 125,
    Function_conj       = 126,
    Function_proj       = 127,

    Function_size       = 140,
    Function_span       = 141,
    Function_sum        = 142,
    Function_max        = 143,
    Function_min        = 144,

    Function_vector     = 150,
    Function_range      = 151,
    Function_try        = 152,
    Function_try_that   = 153,
    Function_try_catch  = 154,

    Function_tostr      = 160,
    Function_tonum      = 161,
    Function_toint      = 162,
    Function_torat      = 163,
    Function_toflt      = 164,

    Function_eval       = 170,
    Function_call       = 171,
    Function_print      = 172,
    Function_strlen     = 173,

    ID_TWSF_STOP        = 179,

    SET_CONSTANT        = 180,
    SET_COMPNAME        = 181,
    SET_OUTSIDER        = 182,
    SET_REPL_LHS        = 183,
    SET_PARAMTER        = 184,
    SET_VAR_FUNC        = 185,
    SET_DOT_CALL        = 186,
    SET_DOT_FUNC        = 187
};


enum ID_TWST
{
    ID_TWST_START               = 0,

    // inside component.c
    Cannot_Find_Component       = 10,
    Component_Already_Defined   = 11,

    // inside expression.c
    IsNot_ValueStructure        = 21,
    Invalid_Expression_End      = 22,
    Invalid_Expression_Syntax   = 23,

    Bracket_Match_Invalid       = 30,
    Bracket_Match_None          = 31,
    Lacking_Bracket             = 32,

    // inside operations*.c
    Operands_DoNot_Match        = 51,
    Operand_IsNot_Matrix        = 52,
    Argument_vs_Parameter       = 53,
    ResultSt_vs_Expected        = 54,

    Index_OutOf_Range           = 55,
    Vector_Middle_Value         = 56,
    Vector_Length_Invalid       = 57,

    // inside operations_specific.c
    Left_IsNot_Matrix           = 70,
    Right_IsNot_Matrix          = 71,
    MatrixMult_IsInvalid        = 72,
    Matrix_IsNot_NbyN           = 73,
    Left_IsNot_Single           = 74,
    Right_IsNot_Single          = 75,

    Expect_Question_Before      = 80,
    Expect_Choice_After         = 81,
    Condition_IsNot_Single      = 82,

    // evaluation errors
    Division_By_Zero            = 90,
    Operand_Not_Supported       = 91,
    Only_On_Integer_Number      = 92,
    Argument_OutOf_Domain       = 93,

    ID_TWST_STOP                = 100
};


bool IS_TWSF_ID (int ID);
bool IS_TWST_ID (int ID);

const_Str2 TWSF (enum ID_TWSF ID); // get loaded string of
const_Str2 TWST (enum ID_TWST ID); // the corresponding ID

enum RFET_LANGUAGE { RFET_ENGLISH, RFET_FRENCH };

bool texts_load_twsf (value stack, enum RFET_LANGUAGE language);
bool texts_load_twst (value stack, enum RFET_LANGUAGE language);


#endif
