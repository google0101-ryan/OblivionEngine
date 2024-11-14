#include <layer0/Lexer.h>
#include <cassert>
#include <cctype>
#include <string.h>

CLexer::CLexer(const char *pSource, int line)
{
    m_pCurPtr = pSource;
    m_pPrevPtr = pSource;
    m_line = line;
}

static const char* punctuation[] =
{
    // Binary shift
    ">>=",
    "<<=",
    //
    "...",
    // logical operators
    "&&",
    "||",
    ">=",
    "<=",
    "==",
    "!=",
    // arithmetic operators
    "*=",
    "/=",
    "%=",
    "+=",
    "-=",
    "++",
    "--",
    // binary operators
    "&=",
    "|=",
    "^=",
    ">>",
    "<<",
    // reference operators
    "->",
    // C++
    "::",
    ".*",
    // arithmetic
    "*",
    "/",
    "%",
    "+",
    "-",
    "=",
    // binary
    "&",
    "|",
    "^",
    "~",
    // logic
    "!",
    ">",
    "<",
    // reference
    ".",
    // seperators
    ",",
    ";",
    // label
    ":",
    // if statement
    "?",
    // braces
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",
    //
    "\\",
    // precompiler stuff
    "#",
    "$",
    nullptr
};

#define puncTableSize ((sizeof(punctuation)) / (sizeof(punctuation[0])))

bool CLexer::GetToken(SToken *pToken)
{
    pToken->m_value.clear();
    if (!SkipWhitespace())
        return false;
    
    m_pPrevPtr = m_pCurPtr;
    if (isdigit(*m_pCurPtr))
    {
        if (*m_pCurPtr == '0')
        {
            assert(0);
        }
        else
        {
            // Read until space, then convert to int
            ReadName(pToken);
            if (strstr(pToken->m_value.c_str(), ".") != nullptr)
            {
                // Floating point
                assert(0);
            }
            else
            {
                pToken->m_tokenType = TT_NUMBER;
                pToken->m_iValue = atoi(pToken->m_value.c_str());
            }
        }
    }
    else if (*m_pCurPtr == '"')
    {
        assert(0);
    }
    else if (int len = IsPunctuation(); len != -1)
    {
        pToken->m_tokenType = TT_PUNCTUATION;
        m_pCurPtr -= len;
        ReadPunctuation(pToken);
    }
    else
    {
        ReadName(pToken);
    }

    return true;
}

bool CLexer::SkipWhitespace()
{
    while (1)
    {
        while (*m_pCurPtr <= ' ')
        {
            if (!*m_pCurPtr)
                return false;
            if (*m_pCurPtr == '\n')
                m_line++;
            m_pCurPtr++;
        }
        if (*m_pCurPtr == '/')
        {
            if (*(m_pCurPtr+1) == '/')
            {
                m_pCurPtr++;
                do
                {
                    m_pCurPtr++;
                    if (!*m_pCurPtr)
                        return false;
                } while (*m_pCurPtr != '\n');
                m_line++;
                m_pCurPtr++;
                if (!*m_pCurPtr)
                    return false;
                continue;
            }
            else if (*(m_pCurPtr+1) == '*')
            {
                m_pCurPtr++;
                while (1)
                {
                    if (!*m_pCurPtr)
                        return false;
                    if (*m_pCurPtr == '\n')
                        m_line++;
                    else if (*m_pCurPtr == '/')
                    {
                        if (*(m_pCurPtr-1) == '*')
                            break;
                    }
                }
                m_pCurPtr++;
                if (!*m_pCurPtr)
                    return false;
                m_pCurPtr++;
                if (!*m_pCurPtr)
                    return false;
                continue;
            }
        }
        break;
    }
    return true;
}

int CLexer::IsPunctuation()
{
    for (int i = 0; i < puncTableSize-1; i++)
    {
        int len = strlen(punctuation[i]);
        int j = 0;
        for (j = 0; punctuation[i][j] && m_pCurPtr[i]; j++)
        {
            if (punctuation[i][j] != m_pCurPtr[i])
                break;
        }
        if (!punctuation[i][j])
        {
            m_pCurPtr += len;
            return len;
        }
    }

    return -1;
}

int CLexer::ReadPunctuation(SToken* pToken)
{
    char c;

    do
    {
        pToken->m_value.push_back(*m_pCurPtr++);
        c = *m_pCurPtr;
    } while (!isspace(c));
    pToken->m_subtype = pToken->m_value.size();

    return 1;
}

int CLexer::ReadName(SToken *pToken)
{
    char c;

    pToken->m_tokenType = TT_NAME;
    do
    {
        pToken->m_value.push_back(*m_pCurPtr++);
        c = *m_pCurPtr;
    } while (!isspace(c));
    pToken->m_subtype = pToken->m_value.size();

    return 1;
}
