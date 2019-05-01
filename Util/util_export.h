#pragma once
#ifdef _WIN32
#    ifdef FERROUS_UTIL_EXPORTS
#        define FERROUS_UTIL_API __declspec(dllexport)
#    else
#        define FERROUS_UTIL_API __declspec(dllimport)
#    endif
#elif
#    define FERROUS_UTIL_API
#endif