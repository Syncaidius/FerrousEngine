#pragma once
#ifdef _WIN32
#    ifdef FERROUS_EXPORTS
#        define FERROUS_API __declspec(dllexport)
#    else
#        define FERROUS_API __declspec(dllimport)
#    endif
#elif
#    define FERROUS_API
#endif