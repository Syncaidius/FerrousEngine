#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <io.h>
#include <fstream>
#include <fcntl.h>
#include <stdio.h>

void RedirectToConsole(void);