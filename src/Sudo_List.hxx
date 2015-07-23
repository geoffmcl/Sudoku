// Sudo_List.hxx
#ifndef _SUDO_LIST_HXX_
#define _SUDO_LIST_HXX_

#pragma comment ( lib, "COMCTL32" )

#ifdef ADD_LISTVIEW
#define IDC_LISTVIEW	9876
extern HWND LVCreateListView (HWND hwndParent);
extern int LVSize( HWND hWnd, PRECT pr );
extern BOOL LVInitColumns( int cols, char **pstgs);
extern BOOL LVInsertItem(char *pstg);
extern BOOL LVInsertItems(int cItems, char **pstgs);
extern HWND g_hListView;
#endif

extern void LVAddExStyle(HWND hwndListView, DWORD dwNewStyle);

extern VOID Do_WM_NOTIFY( HWND hWnd, WPARAM wParam, LPARAM lParam );

extern BOOL LV_InitColumns(HWND hListView, int cols, char **pstgs);
extern BOOL LV_InsertItem(HWND hListView, char *pstg);


#endif // #ifndef _SUDO_LIST_HXX_
// eof - Sudo_List.hxx
