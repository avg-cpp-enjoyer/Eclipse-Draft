#pragma once

#include <comdef.h>
#include <string>

#define HR_LOG(expr)                                                                 \
    do {                                                                             \
        long hr__ = (expr);                                                          \
        if (FAILED(hr__)) {                                                          \
            _com_error err(hr__);                                                    \
            std::wstring msg = L"[HR FAILED] " + std::wstring(err.ErrorMessage()) +  \
                L"\n  Expression: " + _CRT_WIDE(#expr) +                             \
                L"\n  File: " + _CRT_WIDE(__FILE__) +                                \
                L"\n  Line: " + std::to_wstring(__LINE__) + L"\n\n";                 \
            OutputDebugString(msg.c_str());                                          \
        }                                                                            \
    } while(0)

#define HR_LOG_AND_RETURN(expr, value)                                               \
 do {                                                                                \
        long hr__ = (expr);                                                          \
        if (FAILED(hr__)) {                                                          \
            _com_error err(hr__);                                                    \
            std::wstring msg = L"[HR FAILED] " + std::wstring(err.ErrorMessage()) +  \
                L"\n  Expression: " + _CRT_WIDE(#expr) +                             \
                L"\n  File: " + _CRT_WIDE(__FILE__) +                                \
                L"\n  Line: " + std::to_wstring(__LINE__) + L"\n\n";                 \
            OutputDebugString(msg.c_str());                                          \
        }                                                                            \
 } while(0);                                                                         \
 return value

#define HR_THROW(expr)                      \
    do {                                    \
        HRESULT _hr__ = (expr);             \
        if (FAILED(_hr__)) {                \
            throw std::exception();         \
        }                                   \
    } while (0)