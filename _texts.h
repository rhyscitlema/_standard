#ifndef __TEXTS_H
#define __TEXTS_H
/*
    _texts.h
*/

#include "_stddef.h"


enum ID_TWSF
{
    ID_TWSF_START       = 0,

    Opened_Bracket_1    = 1,
    Opened_Bracket_2    = 2,
    Opened_Bracket_3    = 3,
    Closed_Bracket_1    = 4,
    Closed_Bracket_2    = 5,
    Closed_Bracket_3    = 6,

    DecimalPoint        = 10,
    CommaSeparator      = 11,
    EndOfStatement      = 12,
    DoubleQuote         = 13,
    Assignment          = 14,

    Concatenate         = 20,
    Replacement         = 21,
    ConditionAsk        = 22,
    ConditionChoose     = 23,

    Equal_1             = 30,
    Equal_2             = 31,
    NotEqual_1          = 32,
    NotEqual_2          = 33,
    LessThan_1          = 34,
    LessThan_2          = 35,
    GreaterThan_1       = 36,
    GreaterThan_2       = 37,
    LessOrEqual_1       = 38,
    LessOrEqual_2       = 39,
    GreaterOrEqual_1    = 40,
    GreaterOrEqual_2    = 41,

    Positive            = 50,
    Negative            = 51,
    Plus                = 52,
    Minus               = 53,
    Times_1             = 54,
    Times_2             = 55,
    Divide_1            = 56,
    Divide_2            = 57,
    ToPower_1           = 58,
    ToPower_2           = 59,
    DotProduct          = 60,
    Factorial           = 61,
    Transpose           = 62,
    Indexing            = 63,

    Ari_Shift_Right     = 64,
    Ari_Shift_Left      = 65,
    Bitwise_OR          = 66,
    Bitwise_XOR         = 67,
    Bitwise_AND         = 68,
    Bitwise_NOT         = 69,

    Logical_OR          = 70,
    Logical_AND         = 71,
    Logical_NOT         = 72,
    Modulo_Remainder    = 73,

    Constant_E_2_718_               = 80,
    Constant_PI_3_141_              = 81,
    SQRT_of_Negative_One            = 82,

    Function_Power_of_e             = 91,
    Function_Logarithm_Base_e       = 92,
    Function_Logarithm_Base_10      = 93,
    Function_Square_Root            = 94,
    Function_Math_Ceil              = 95,
    Function_Math_Floor             = 96,
    Function_FullFloor              = 97,
    Function_GetPrimes              = 98,

    Function_Absolute_Value         = 100,
    Function_Argument_Angle         = 101,
    Function_Real_Part              = 102,
    Function_Imag_Part              = 103,
    Function_Complex_Conjugate      = 104,
    Function_Complex_Projection     = 105,

    Function_aRandom_Number         = 109,
    Function_sRandom_Number         = 110,
    Function_Summation              = 111,
    Function_Maximum                = 112,
    Function_Minimum                = 113,

    Trigonometric_Sine              = 117,
    Trigonometric_Cosine            = 118,
    Trigonometric_Tangent           = 119,
    Trigonometric_Sine_Inverse      = 120,
    Trigonometric_Cosine_Inverse    = 121,
    Trigonometric_Tangent_Inverse   = 122,
    Hyperbolic_Sine                 = 123,
    Hyperbolic_Cosine               = 124,
    Hyperbolic_Tangent              = 125,
    Hyperbolic_Sine_Inverse         = 126,
    Hyperbolic_Cosine_Inverse       = 127,
    Hyperbolic_Tangent_Inverse      = 128,

    Generate_Range                  = 131,
    Generate_Vector                 = 132,
    Length_of_Value                 = 133,
    Size_of_Value                   = 134,
    Try_And_Catch                   = 135,
    Print_Value                     = 136,

    Convert_To_Str                  = 140,
    Convert_To_Num                  = 141,
    Convert_To_Rat                  = 142,
    Convert_To_Flt                  = 143,

    SET_NUMBER                      = 160,
    SET_STRING                      = 161,
    SET_VARIABLE                    = 162,
    SET_FUNCTION                    = 163,
    SET_PARAMETER                   = 164,
    SET_OUTSIDER                    = 165,
    SET_CURRENT                     = 166,
    SET_CONTCALL                    = 167,
    SET_CONSTANT                    = 168,

    ID_TWSF_STOP                    = 170
};


enum ID_TWST
{
    ID_TWST_START               = 0,

    End_Of_Text                 = 1,

    // inside component.c
    Component_Already_Defined   = 10,
    Parameter_Vst_Is_Invalid    = 11,
    Result_Vst_Is_Invalid       = 12,

    // inside expression.c
    IsNot_ValueStructure        = 21,
    Invalid_Expression_End      = 22,
    Invalid_Expression_Syntax   = 23,

    Bracket_Match_Invalid       = 30,
    Bracket_Match_None          = 31,
    Lacking_Bracket             = 32,
    Lacking_Brackets            = 33,

    IsNot_Number                = 40,
    IsNot_Component             = 41,
    Deadlock_Found              = 42,

    // inside operations*.c
    Invalid_Operand             = 50,
    Operands_DoNot_Match        = 51,
    Operand_IsNot_Matrix        = 52,
    Argument_vs_Parameter       = 53,

    Indexing_Single_Number      = 54,
    Index_OutOf_Range           = 55,
    Vector_Starting_Value       = 56,
    Vector_Stopping_Value       = 57,
    Vector_Middle_Value         = 58,
    Vector_Length_Invalid       = 59,

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
    Not_On_Complex_Number       = 91,
    Only_On_Integer_Number      = 92,
    Argument_OutOf_Domain       = 93,

    ID_TWST_STOP                = 100
};


const wchar* TWSF (enum ID_TWSF ID); // get loaded Text-With-Space-False
const wchar* TWST (enum ID_TWST ID); // get loaded Text-With-Space-True

bool texts_load_twsf (const wchar* fileContent);
bool texts_load_twst (const wchar* fileContent);

enum LANGUAGE { ENGLISH, FRENCH };
const wchar* default_twsf (enum LANGUAGE language);
const wchar* default_twst (enum LANGUAGE language);

bool is_TWSF_ID (int ID);
bool is_TWST_ID (int ID);


#endif
