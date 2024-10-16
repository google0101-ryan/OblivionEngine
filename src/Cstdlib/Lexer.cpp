#include "Lexer.h"
#include <Util/Logger.h>

#include <cstring>
#include <cassert>

Lexer::Lexer(const char *src)
{
    m_pSrc = src;
    m_pCurrent = src;
}

bool Lexer::GetNextToken(Token* token)
{
    m_pPrev = m_pCurrent;

    if (!SkipWhitespace())
        return false;

    token->m_pszValue.clear();
    
    // Figure out what kind of token we're getting
    if (isdigit(*m_pCurrent))
    {
        // Some kind of number
        m_pCurrent++;
        token->m_type = TT_NUMBER;
        if (*m_pCurrent == 'x')
        {
            // Hex number
            assert(0 && "TODO: Hex number");
        }
        else if (*m_pCurrent == 'b')
        {
            // binary number
            assert(0 && "TODO: binary number");
        }
        else if (*(m_pCurrent-1) == '0')
        {
            // Octal
            assert(0 && "TODO: octal number");
        }
        else
        {
            // decimal
            assert(0 && "TODO: Decimal number");
            auto ptr = m_pCurrent;
            while (isdigit(*ptr))
                ptr++;
            // floating point number
            if (*ptr == '.' || *ptr == 'f')
            {

            }
            else
            {

            }
        }
    }
    else if (*m_pCurrent == '"')
    {
        // string
        assert(0 && "TODO: String");
    }
    else if (int len = IsPunctuation(); len != -1)
    {
        // punctatuion
        token->m_type = TT_PUNCTUATION;
        m_pCurrent -= len;
        ReadPunctuation(token);
    }
    else
    {
        // name (used for symbols and the like)
        // i.e. void in C++ would be a name
        ReadName(token);
    }

    return true;
}

bool Lexer::ExpectName(const char *str)
{
    Token tk;
    if (!GetNextToken(&tk))
    {
        Logger::Log(LogLevel::WARNING, "Unexpected name on line %d", m_iLine);
        return false;
    }

    if (tk.m_type != TT_NAME || tk.m_pszValue != str)
    {
        Logger::Log(LogLevel::WARNING, "Unexpected name on line %d: '%s' expected, got '%s' instead", m_iLine, str, tk.m_pszValue.c_str());
        m_pCurrent = m_pPrev;
        return false;
    }

    return true;
}

bool Lexer::SkipWhitespace()
{
    while (1)
    {
        while (*m_pCurrent <= ' ')
        {
            if (!*m_pCurrent)
                return false;
            if (*m_pCurrent == '\n')
                m_iLine++;
            m_pCurrent++;
        }
        if (*m_pCurrent == '/')
        {
            if (*(m_pCurrent+1) == '/')
            {
                m_pCurrent++;
                do
                {
                    m_pCurrent++;
                    if (!*m_pCurrent)
                        return false;
                } while (*m_pCurrent != '\n');
                m_iLine++;
                m_pCurrent++;
                if (!*m_pCurrent)
                    return false;
                continue;
            }
            else if (*(m_pCurrent+1) == '*')
            {
                m_pCurrent++;
                while (1)
                {
                    if (!*m_pCurrent)
                        return false;
                    if (*m_pCurrent == '\n')
                        m_iLine++;
                    else if (*m_pCurrent == '/')
                    {
                        if (*(m_pCurrent-1) == '*')
                            break;
                    }
                }
                m_pCurrent++;
                if (!*m_pCurrent)
                    return false;
                m_pCurrent++;
                if (!*m_pCurrent)
                    return false;
                continue;
            }
        }
        break;
    }
    
    return true;
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
    NULL
};

#define puncTableSize ((sizeof(punctuation)) / (sizeof(punctuation[0])))

int Lexer::IsPunctuation()
{
    for (int i = 0; i < puncTableSize-1; i++)
    {
        int len = strlen(punctuation[i]);
        int j = 0;
        for (j = 0; punctuation[i][j] && m_pCurrent[i]; j++)
        {
            if (punctuation[i][j] != m_pCurrent[i])
                break;
        }
        if (!punctuation[i][j])
        {
            m_pCurrent += len;
            return len;
        }
    }

    return -1;
}

int Lexer::ReadPunctuation(Token *token)
{
    char c;

    do
    {
        token->m_pszValue.push_back(*m_pCurrent++);
        c = *m_pCurrent;
    } while (!isspace(c));
    token->m_subType = token->m_pszValue.size();

    return 1;
}

int Lexer::ReadName(Token *token)
{
    char c;

    token->m_type = TT_NAME;
    do
    {
        token->m_pszValue.push_back(*m_pCurrent++);
        c = *m_pCurrent;
    } while (!isspace(c));
    token->m_subType = token->m_pszValue.size();

    return 1;
}
