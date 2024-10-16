#pragma once

namespace LogLevel
{
enum
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};
}

namespace Logger
{

#ifdef NDEBUG
#define LOG_LEVEL_MAX LogLevel::INFO
#else
#define LOG_LEVEL_MAX LogLevel::DEBUG
#endif

void Log(int iLevel, const char* pszFmt, ...);

}