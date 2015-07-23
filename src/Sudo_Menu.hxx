// Sudo_Menu,hxx

#ifndef _SUDO_MENU_HXX_
#define _SUDO_MENU_HXX_
#include "Sudo_Paint.hxx"

#define IDM_FILE1                       39001
#define IDM_FILE2                       39002
#define IDM_FILE3                       39003
#define IDM_FILE4                       39004
#define IDM_FILE5                       39005
#define IDM_FILE6                       39006
#define IDM_FILE7                       39007
#define IDM_FILE8                       39008
#define IDM_FILE9                       39009
#define IDM_FILE10                      39010
#define IDM_FILE11                      39011
#define IDM_FILE12                      39012
#define IDM_FILE13                      39013
#define IDM_FILE14                      39014
#define IDM_FILE15                      39015
#define IDM_FILE16                      39016
#define IDM_FILE17                      39017
#define IDM_FILE18                      39018
#define IDM_FILE19                      39019
// Hmmmm, this just TOO MANY - Makes dropdown TOO BIG
// Perhpas need a DIALOG to handle this BIG LIST, but ONLY put say 20 on MRU Menu
#define IDM_FILEMax                     39064

extern DWORD gdwMaxFiles;
extern DWORD SetFileMRU( HWND hwnd, vSTG vs );
extern VOID Do_WM_CONTEXTMENU( HWND hWnd, WPARAM wParam, LPARAM lParam );
extern void Do_ID_OPTIONS_DELETECELL( HWND hWnd );

#endif 
// eof - Sudo_Menu.hxx
