// This is free and unencumbered software released into the public domain.
#include <fcntl.h>
#include <io.h>

#define CP_UTF8 65001

/*
WINBASEAPI WINBOOL WINAPI SetConsoleCP(UINT code_page_id);
WINBASEAPI WINBOOL WINAPI SetConsoleOutputCP(UINT code_page_id);
*/

#define W32(r) __declspec(dllimport) r __stdcall

W32(int) SetConsoleCP(unsigned int);
W32(int) SetConsoleOutputCP(unsigned int);

__attribute__((constructor)) void winsane_init(void) {
    _setmode(0, _O_BINARY);
    _setmode(1, _O_BINARY);
    _setmode(2, _O_BINARY);
    SetConsoleCP(CP_UTF8); // maybe will work someday
    SetConsoleOutputCP(CP_UTF8);
}
