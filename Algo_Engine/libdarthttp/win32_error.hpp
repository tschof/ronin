#ifndef __WIN32_ERROR_HPP__
#define __WIN32_ERROR_HPP__

#include <windows.h>
#include <string>

void win32_perror(DWORD);
std::string win32_strerror(DWORD);

#endif
