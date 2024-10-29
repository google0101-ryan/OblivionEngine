#pragma once

class CString
{
public:
    CString();
    CString( const char* pData );
    CString( CString& text );

    ~CString();

    void operator=( CString text );

    // Convert any nasty backslashes to the much better forward slash
    // Used for FS stuff
    void BackslashToSlash();

    void Append( CString& other );
    void Append( const char* pData );

    const char* GetCStr() const;
    float AsFloat() const;
    int AsInt() const;

    int Length() const;
private:
    // Expand, if necessary, to fit the new string
    void Expand( int length );
private:
    static const int MAX_STATIC_STRING = 32;

    int m_allocSize;
    int m_length;
    bool m_bIsStatic;
    char* m_pData;
    char m_staticData[MAX_STATIC_STRING];
};

CString operator+( const char* pStr, CString& bStr );
CString operator+( CString& aStr, const char* pStr );