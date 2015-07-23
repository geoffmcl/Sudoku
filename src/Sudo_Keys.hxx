// Sudo_Keys.hxx
#ifndef _SUDO_KEYS_HXX_
#define _SUDO_KEYS_HXX_

extern VOID Do_WM_KEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam );
extern VOID Do_WM_KEYUP( HWND hWnd, WPARAM wParam, LPARAM lParam );
extern void Do_WM_CHAR( HWND hWnd, WPARAM wParam );

extern bool Check_Exit();

#endif // #ifndef _SUDO_KEYS_HXX_
// eof - Sudo_Keys.hxx
