#pragma once

#include <string>

typedef enum
{
    TT_STRING,
    TT_NUMBER,
    TT_NAME,
    TT_PUNCTUATION
} TokenType_t;

struct SToken
{
    TokenType_t m_tokenType;
    std::string m_value;
    int m_iValue;
    float m_fValue;
    int m_subtype; // string length for TT_STRING and TT_NAME
};