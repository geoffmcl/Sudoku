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
typedef bool BOOL;
typedef bool * PBOOL;
typedef long WPARAM;
typedef void VOID;
typedef char * LPTSTR;
typedef char * LPCTSTR;
typedef char * LPCSTR;
typedef long LPARAM;
typedef void * HPEN;
typedef void * HBRUSH;
typedef void * HDC;
typedef void * HFONT;
typedef void * HANDLE;
typedef void * HMENU;
typedef void * HPALETTE;
typedef void * HBITMAP;
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
    RECT rcNormalPositon;
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

#endif // #ifndef _UNIXGLUE_HXX_
// eof - unixglue.hxx

