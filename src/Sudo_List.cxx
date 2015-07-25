// Sudo_List.cxx

#include "Sudoku.hxx"

#ifdef WIN32 // windows LISTVIEW control
#include <CommCtrl.h>

#ifdef ADD_LISTVIEW

HWND g_hListView = 0;
#define SUB_CLASS_ID 0x1234

typedef LRESULT ( CALLBACK *SUBCLASSPROC)(
  HWND hWnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam,
  UINT_PTR uIdSubclass,
  DWORD_PTR dwRefData
);

SUBCLASSPROC pSubClass = 0;

LRESULT CALLBACK LVCallBack( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
    switch (uMsg)
    {
    case WM_CHAR:
        Do_WM_CHAR( g_hWnd, wParam );
        break;
    }
    return 0;
}

/**************************************************************************
   LVSwitchView()
**************************************************************************/
void LVSwitchView(HWND hwndListView, DWORD dwView)
{
	DWORD dwStyle = GetWindowLong(hwndListView, GWL_STYLE);
	SetWindowLong(hwndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	// LVResize(hwndListView, GetParent(hwndListView));
}


/**************************************************************************
   LVRemoveExStyle()
**************************************************************************/
void LVRemoveExStyle(HWND hwndListView, DWORD dwNewStyle)
{
	DWORD dwStyle = SendMessage(hwndListView, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	dwStyle &= ~dwNewStyle;
	SendMessage(hwndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);
}


// LVCreateListView: Creates a list-view control in report view.
// Returns the handle to the new control
// TO DO:  The calling procedure should determine whether the handle is NULL, in case 
// of an error in creation.
//
// HINST hInst: The global handle to the applicadtion instance.
// HWND  hWndParent: The handle to the control's parent window. 
//
HWND LVCreateListView (HWND hwndParent) 
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    ZeroMemory(&icex,sizeof(INITCOMMONCONTROLSEX));
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rc;                       // The parent window's client area.
    GetClientRect (hwndParent, &rc);
    rc.left = (rc.right / 4) * 3;

    //DWORD style = WS_CHILD | LVS_LIST | WS_BORDER | WS_VISIBLE;
    DWORD style = WS_CHILD | LVS_REPORT | WS_BORDER | WS_VISIBLE;
    style |= LVS_NOSORTHEADER | LVS_SINGLESEL;

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindow(WC_LISTVIEW, 
                                     TEXT(""),
                                     style, // WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
                                     rc.left, rc.top,       // x,y position
                                     rc.right - rc.left,    // cx width
                                     rc.bottom - rc.top,    // cy height
                                     hwndParent,
                                     (HMENU)IDC_LISTVIEW,   //IDM_CODE_SAMPLES,
                                     hInst,
                                     NULL); 

    if (hWndListView) {
        // LVS_EX_FULLROWSELECT
        LVAddExStyle(hWndListView, LVS_EX_FULLROWSELECT);
    }

    return (hWndListView);
}

int LVSize( HWND hWnd, PRECT pr )
{
    int i = -1;
    if (g_hListView) {
        i = MoveWindow( g_hListView,
			pr->left,              // x
			pr->top,               // y
			(pr->right  - pr->left), // cx
			(pr->bottom - pr->top),  // cy
			TRUE );
    }
    return i;
}

#define MX_TX_SIZE 256
BOOL LVInitColumns( int cols, char **pstgs) 
{ 
    TCHAR szText[MX_TX_SIZE+2];     // Temporary buffer.
    LVCOLUMN lvc;
    int iCol;
    RECT rc;
    int width;

    GetClientRect( g_hListView, &rc );
    width = rc.right - rc.left;
    width /= cols;

    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text,
    // and subitem members of the structure are valid.
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    if (cols > 1)
        lvc.mask |= LVCF_SUBITEM;

    // Add the columns.
    for (iCol = 0; iCol < cols; iCol++)
    {
        // set text
        strncpy(szText,pstgs[iCol], MX_TX_SIZE);
        szText[MX_TX_SIZE] = 0;

        lvc.iSubItem = iCol;
        lvc.pszText = szText;
        lvc.cx = width;               // Width of column in pixels.

        if ( iCol < 2 )
            lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
        else
            lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

        // Insert the columns into the list view.
        if (ListView_InsertColumn(g_hListView, iCol, &lvc) == -1)
            return FALSE;
    }

    //if ( !SetWindowSubclass( g_hListView, LVCallBack, SUB_CLASS_ID, 0)) {
    //    return FALSE;
    //}
    
    return TRUE;
} 

BOOL LVInsertItems(int cItems, char **pstgs)
{
    LVITEM lvI;

    // Initialize LVITEM members that are common to all items.
    // lvI.pszText   = pstg; // LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.mask      = LVIF_TEXT; // | LVIF_STATE; // | LVIF_IMAGE 
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    // Initialize LVITEM members that are different for each item.
    for (int index = 0; index < cItems; index++)
    {
        lvI.iItem  = index;
        lvI.iImage = index;
        lvI.pszText = pstgs[index];
        // Insert items into the list.
        if (ListView_InsertItem(g_hListView, &lvI) == -1)
            return FALSE;
    }

    return TRUE;
}

BOOL LVInsertItem(char *pstg)
{
    LVITEM lvI = {0};

    if (!pstg)
        return FALSE;

    size_t len = strlen(pstg);
    if (!len)
        return FALSE;

    char *ps = _strdup(pstg);
    if (!ps) return FALSE;    // forget it
    len = strlen(ps);
    while(len--) {
        if (ps[len] > ' ') {
            len++;
            break;
        }
        ps[len] = 0;
    }
    if (len <= 0) return FALSE;

    for(size_t i = 0; i < len; i++) {
        if (ps[i] < ' ')
            ps[i] = ' ';
    }

    // Initialize LVITEM members that are common to all items.
    lvI.mask       = LVIF_TEXT; // | LVIF_STATE; // | LVIF_IMAGE 
    lvI.pszText    = ps; // LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.cchTextMax = len;
    if (g_hListView) {
        lvI.iItem      = SendMessage(g_hListView, LVM_GETITEMCOUNT, 0, 0);
        if (ListView_InsertItem(g_hListView, &lvI) == -1)
            return FALSE;
        ListView_EnsureVisible(g_hListView, lvI.iItem, 0);
    }
    return TRUE;
}

// case WM_NOTIFY:
/* 
typedef struct tagNMCHAR {
  NMHDR hdr;
  UINT  ch;
  DWORD dwItemPrev;
  DWORD dwItemNext;
} NMCHAR, *LPNMCHAR;

*/


#endif // #ifdef ADD_LISTVIEW

#ifndef MX_TC_SIZE
#define MX_TX_SIZE 256
#endif

BOOL LV_InitColumns(HWND hListView, int cols, char **pstgs) 
{ 
    TCHAR szText[MX_TX_SIZE+2];     // Temporary buffer.
    LVCOLUMN lvc;
    int iCol;
    RECT rc;
    int width;

    GetClientRect( hListView, &rc );
    width = rc.right - rc.left;
    width /= cols;

    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text,
    // and subitem members of the structure are valid.
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    if (cols > 1)
        lvc.mask |= LVCF_SUBITEM;

    // Add the columns.
    for (iCol = 0; iCol < cols; iCol++)
    {
        // set text
        strncpy(szText,pstgs[iCol], MX_TX_SIZE);
        szText[MX_TX_SIZE] = 0;

        lvc.iSubItem = iCol;
        lvc.pszText = szText;
        lvc.cx = width;               // Width of column in pixels.

        if ( iCol < 2 )
            lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
        else
            lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

        // Insert the columns into the list view.
        if (ListView_InsertColumn(hListView, iCol, &lvc) == -1)
            return FALSE;
    }

    //if ( !SetWindowSubclass( hListView, LV_CallBack, SUB_CLASS_ID, 0)) {
    //    return FALSE;
    //}
    
    return TRUE;
} 


BOOL LV_InsertItem(HWND hListView, char *pstg)
{
    LVITEM lvI = {0};

    if (!pstg)
        return FALSE;

    size_t len = strlen(pstg);
    if (!len)
        return FALSE;

    char *ps = _strdup(pstg);
    if (!ps) return FALSE;    // forget it
    len = strlen(ps);
    while(len--) {
        if (ps[len] > ' ') {
            len++;
            break;
        }
        ps[len] = 0;
    }
    if (len <= 0) return FALSE;

    for(size_t i = 0; i < len; i++) {
        if (ps[i] < ' ')
            ps[i] = ' ';
    }

    // Initialize LVITEM members that are common to all items.
    lvI.mask       = LVIF_TEXT; // | LVIF_STATE; // | LVIF_IMAGE 
    lvI.pszText    = ps; // LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.cchTextMax = len;
    if (hListView) {
        lvI.iItem      = SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);
        if (ListView_InsertItem(hListView, &lvI) == -1)
            return FALSE;
        ListView_EnsureVisible(hListView, lvI.iItem, 0);
    }
    return TRUE;
}

VOID Do_WM_NOTIFY( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    char buf[256];
    LPNMHDR pnh = (LPNMHDR)lParam;
    sprintf(buf,"WM_NOTIFY: Code %d - ID %d", pnh->code, pnh->idFrom );
    //switch (pnh->code)
#ifdef ADD_LISTVIEW
    if (g_hListView && g_bDoneInit) {
        if (pnh->idFrom == IDC_LISTVIEW) {
            if (pnh->code == NM_CHAR) {
                LPNMCHAR pnc = (LPNMCHAR)lParam;
                Do_WM_CHAR( g_hWnd, (WPARAM)pnc->ch );
                //int prev = add_list_out(0);
                //sprtf("%s\n",buf);
                //add_list_out(prev);
            } else if (pnh->code == NM_SETFOCUS) {
                SetFocus(g_hWnd);
            }
        }
    }
#endif // ADD_LISTVIEW
}

/**************************************************************************
   LVAddExStyle()
**************************************************************************/
void LVAddExStyle(HWND hwndListView, DWORD dwNewStyle)
{
	DWORD dwStyle = SendMessage(hwndListView, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	dwStyle |= dwNewStyle;
	SendMessage(hwndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);

}

#endif // #ifdef WIN32 // windows LISTVIEW control


// eof - Sudo_List.cxx
