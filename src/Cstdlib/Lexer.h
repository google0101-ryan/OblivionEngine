#pragma once

#include <string>

enum TokenType
{
    TT_STRING,
    TT_NUMBER,
    TT_PUNCTUATION,
    TT_NAME
};

enum TokenSubtype
{
    TT_SUB_INTEGER,
    TT_SUB_FLOATING_POINT
};

struct Token
{
    TokenType m_type;
    std::string m_pszValue;
    int m_iValue;
    float m_fValue;
    int m_subType;
};

class Lexer
{
public:
    Lexer(const char* src);

    bool GetNextToken(Token* token);
    bool ExpectName(const char* str);
private:
    bool SkipWhitespace();
    int IsPunctuation();

    int ReadPunctuation(Token* token);
    int ReadName(Token* token);
private:
    const char* m_pSrc;
    const char* m_pCurrent;
    const char* m_pPrev;
    int m_iLine;
};