#include <layer0/layer0.h>

CString::CString()
{
    m_pData = m_staticData;
    m_bIsStatic = true; // Used until size exceeds 32
    m_allocSize = MAX_STATIC_STRING;
    m_length = 0;
    m_staticData[0] = '\0';
}

CString::CString(const char *pData)
{
    size_t len = strlen( pData );

    // Easy case, just memcpy and done
    if ( len < MAX_STATIC_STRING )
    {
        m_bIsStatic = true;
        m_length = len;
        m_allocSize = MAX_STATIC_STRING;
        memcpy( m_staticData, pData, len );
        m_staticData[len] = '\0';
        m_pData = m_staticData;
        return;
    }

    // More complicated. Allocate string len * 2 to account for future appending
    m_pData = new char[len*2];
    m_length = len;
    m_allocSize = len*2;
    memcpy(m_pData, pData, len);
    m_pData[len] = '\0';
    m_bIsStatic = false;
}

CString::CString(CString &text)
{
    m_length = 0;

    int l = text.Length();
    Expand( l );
    strcpy( m_pData, text.m_pData );
    m_length = l;
}

CString::~CString()
{
    m_length = 0;
    m_allocSize = 0;
    if ( m_pData != m_staticData )
        delete m_pData;
}

void CString::operator=(CString text)
{
    int l;

    l = text.Length();
    Expand( l );
    memcpy( m_pData, text.m_pData, l );
    m_pData[l] = 0;
    m_length = l;
}

void CString::BackslashToSlash()
{
    for (int i = 0; i < m_allocSize; i++)
    {
        if (m_pData[i] == '\\')
            m_pData[i] = '/';
    }
}

void CString::Append(CString &other)
{
    int l = other.Length();

    Expand( m_length + l );
    strcat( m_pData, other.m_pData );
    m_length += l;
    m_pData[m_length] = 0;
}

void CString::Append(const char *pData)
{
    int l = strlen( pData );

    Expand( m_length + l );
    strcat( m_pData, pData );
    m_length += l;
    m_pData[m_length] = 0;
}

const char *CString::GetCStr() const
{
    if (m_bIsStatic)
        return m_staticData;
    else
        return m_pData;
}

float CString::AsFloat() const
{
    return atof( m_pData );
}

int CString::AsInt() const
{
    return atoi( m_pData );
}

int CString::Length() const
{
    return m_allocSize;
}

void CString::Expand(int length)
{
    length += 1; // Account for NULL
    if ( length <= m_length )
        return;
    
    if ( m_bIsStatic && length < MAX_STATIC_STRING )
        return;
    
    // Length is longer than the current string and more than the static string
    char* pNewData = new char[ length*2 ];
    memcpy( pNewData, m_pData, m_length );
    m_allocSize = length*2;
    m_bIsStatic = false;
}

CString operator+(const char *pStr, CString &bStr)
{
    CString str( pStr );
    str.Append( bStr );

    return str;
}

CString operator+(CString &aStr, const char *pStr)
{
    CString str( aStr );
    str.Append( pStr );

    return str;
}

CString operator+(const char *pStr, CString bStr)
{
    CString str( pStr );
    str.Append( bStr );

    return str;
}

CString operator+(CString aStr, const char *pStr)
{
    CString str( aStr );
    str.Append( pStr );

    return str;
}
