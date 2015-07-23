// Sudo_List.hxx
#ifndef _SUDO_LIST_HXX_
#define _SUDO_LIST_HXX_

extern BOOL g_bComCtl4;
extern BOOL IsComCtl32400( VOID );
extern HWND g_hListView;
extern HWND LVCreateWindow(HINSTANCE hInstance, HWND hwndParent);
extern BOOL setcolumns( HWND hLV );
extern BOOL Set_LV_ON(VOID);
extern BOOL Set_LV_OFF(VOID);
extern int ResizeLV( HWND hLV, LPRECT lpr );
extern BOOL g_bLVOff;
extern LRESULT LVInsertItem( char *pstg );

#pragma comment ( lib, "COMCTL32" )

#endif // #ifndef _SUDO_LIST_HXX_
// eof - Sudo_List.hxx
