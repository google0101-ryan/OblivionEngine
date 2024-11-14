#pragma once

#include "Token.h"

class CLexer
{
public:
    CLexer(const char* pSource, int line = 1);

    bool GetToken(SToken* pToken);
private:
    bool SkipWhitespace();
    int IsPunctuation();
    int ReadPunctuation(SToken* pToken);
    int ReadName(SToken* pToken);

    const char* m_pCurPtr;
    const char* m_pPrevPtr;
    int m_line;
};