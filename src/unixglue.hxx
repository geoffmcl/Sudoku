/*\
 * unixglue.hxx
 *
 * Copyright (c) 2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#ifndef _UNIXGLUE_HXX_
#define _UNIXGLUE_HXX_

typedef void * HWND;
typedef void * HINSTANCE;
typedef char TCHAR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int BOOL;
typedef BOOL * PBOOL;
typedef long WPARAM;
typedef void VOID;
typedef char * PTSTR;
typedef char * LPTSTR;
typedef const char * LPCTSTR;
typedef const char * LPCSTR;
typedef long LPARAM;
typedef void * HPEN;
typedef void * HBRUSH;
typedef void * HDC;
typedef void * HFONT;
typedef void * HANDLE;
typedef void * HMENU;
typedef void * HPALETTE;
typedef void * HBITMAP;
typedef void * PVOID;
typedef void * HMODULE;
typedef unsigned char byte;

typedef unsigned long COLORREF;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef int INT;

typedef struct tagRECT {
    long left;
    long top;
    long right;
    long bottom;
} RECT, *PRECT;

typedef struct tagPOINT {
    long x;
    long y;
}POINT, *PPOINT;

typedef struct tagWINDOWPLACEMENT {
    unsigned int length;
    unsigned int flags;
    unsigned int showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT rcNormalPosition;
}WINDOWPLACEMENT, *PWINDOWPLACEMENT;

typedef struct tagCHOOSECOLOR {
    DWORD   lStructSize;
    HWND    hwndOwner;
    HWND    hInstance;
    COLORREF rgbResult;
    COLORREF *lpCustColors;
    DWORD   Flags;
    LPARAM  lCustData;
    void *  lpfnHook;
    LPCSTR  lpTemplateName;
}CHOOSECOLOR, *LPCHOOSECOLOR;

#define CC_RGBINIT               0x00000001
#define CC_FULLOPEN              0x00000002
#define CC_PREVENTFULLOPEN       0x00000004
#define CC_SHOWHELP              0x00000008
#define CC_ENABLEHOOK            0x00000010
#define CC_ENABLETEMPLATE        0x00000020
#define CC_ENABLETEMPLATEHANDLE  0x00000040
// #if(WINVER >= 0x0400)
#define CC_SOLIDCOLOR            0x00000080
#define CC_ANYCOLOR              0x00000100
// #endif /* WINVER >= 0x0400 */

#define _cdecl
#define FALSE false
#define TRUE   true

/*
 * Dialog Box Command IDS
 */
#define IDOK        1
#define IDCANCEL    2
#define IDABORT     3
#define IDRETRY     4
#define IDIGNORE    5
#define IDYES       6
#define IDNO        7

#define RGB(r,g,b) (COLORREF)(r & 0xff) | ((g << 8) & 0xff00) | ((b << 16) & 0xff0000)

#ifndef _MAX_PATH
#define _MAX_PATH   264
#endif

/*
 * ShowWindow() Commands
 */
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11

#define LOBYTE(w) ((BYTE)((w) & 0xff))
#define GetRValue(rgb)  (LOBYTE(rgb))
#define GetGValue(rgb)  (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)  (LOBYTE((rgb)>>16))

#endif // #ifndef _UNIXGLUE_HXX_
// eof - unixglue.hxx

