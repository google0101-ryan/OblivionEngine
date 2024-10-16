#include <Util/Logger.h>
#include <Framework/Engine/Engine.h>

#include <cstdarg>
#include <iostream>

const char* LogLevelToString(int iLevel)
{
    switch (iLevel)
    {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::FATAL:
        return "FATAL";
    default:
        return "INFO";
    }
}

void Logger::Log(int iLevel, const char *pszFmt, ...)
{
    if (iLevel < LOG_LEVEL_MAX)
        return;
    
    std::cout << "[" << LogLevelToString(iLevel) << "]: ";

    static char printBuf[1024];

    va_list args;
    va_start(args, pszFmt);
    
    vsnprintf(printBuf, 2048, pszFmt, args);

    va_end(args);

    std::cout << printBuf << "\n";

    if (iLevel == LogLevel::FATAL) [[unlikely]]
        g_pEngine->Exit(-1);
}