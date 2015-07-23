// Sudo_List.cxx

#include "Sudoku.hxx"
#include <Windows.h>
#include <shlwapi.h>
#include <Commctrl.h>

#ifndef MEOR
#define MEOR "\r\n"
#endif

#define PACKVERSION(major,minor) MAKELONG(minor,major)
#define NO_IMAGE_LISTS
#define IDC_LISTVIEW	1000

#define  VLV(a)      ( a && IsWindow(a) )

#define	UseComCtrls(a)	{\
	INITCOMMONCONTROLSEX _iccex;\
	_iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);\
	_iccex.dwICC = a;\
	InitCommonControlsEx(&_iccex);\
}

BOOL g_bComCtl4 = FALSE;
HWND g_hListView = 0;  // handle, of the LISTVIEW window
BOOL g_bLVOff = FALSE;
int g_iLVColCnt;

typedef struct tagLISTHEADERS {
   LPTSTR   lh_pTitle;  // title at top of column
   DWORD    lh_dwWidth; // suggested WIDTH of the column
   DWORD    lh_dwFlag;  // flag of bits - used by list
}LISTHEADERS, * PLISTHEADERS;

#define  LV_DEF_COLS    1     // presently 7 columns

typedef  struct LVOPTIONS {
   int  lvo_iType;    // LVS_REPORT, LIST or ICONS = DEF_LV_TYPE;
   int	lvo_iChgType;
   int	lvo_iAddTrack; // case IDM_HOVERSELECT:
   int	lvo_iChgTrack;
   int	lvo_iAddCheck;	// LVS_EX_CHECKBOXES case IDM_CHECKBOXES:
   int	lvo_iChgCheck;
   int	lvo_iAddGrid;	// LVS_EX_GRIDLINES  case IDM_GRIDLINES:
   int	lvo_iChgGrid;
   int	lvo_iAddFull;	// LVS_EX_FULLROWSELECT case IDM_FULLROWSELECT:
   int	lvo_iChgFull;
   int	lvo_iAddHDrag;	// LVS_EX_HEADERDRAGDROP case IDM_HEADERDRAGDROP:
   int	lvo_iChgHDrag;
   int	lvo_iAddISub;	// LVS_EX_SUBITEMIMAGES case IDM_SUBITEMIMAGES:
   int	lvo_iChgISub;
}LVOPTIONS, * PLVOPTIONS;

LVOPTIONS   sLVOptions = {
    LVS_LIST, 0,
	0, 0,    //   case IDM_HOVERSELECT:
   0, 0, 	//   case IDM_CHECKBOXES: // LVS_EX_CHECKBOXES
	0, 0,    //   case IDM_GRIDLINES:	// LVS_EX_GRIDLINES
   1, 0,    //case IDM_FULLROWSELECT:	// LVS_EX_FULLROWSELECT
   0, 0,    //case IDM_HEADERDRAGDROP: // LVS_EX_HEADERDRAGDROP
   0, 0,    //case IDM_SUBITEMIMAGES: // LVS_EX_SUBITEMIMAGES
};

LISTHEADERS sOutlineList[LV_DEF_COLS] = {
   { "Sudoku",   -1, 0 },
};

#define  nOutlineCols   (sizeof(sOutlineList) / sizeof(LISTHEADERS)) // = 1

DWORD LVcolumncount( VOID )
{
   return( nOutlineCols );
}

BOOL  LVInit( VOID )
{
	UseComCtrls(ICC_LISTVIEW_CLASSES);
    return TRUE;
}

// forward ref
void LVResize(HWND hwndListView, HWND hwndParent);

/**************************************************************************
   LVSwitchView()
**************************************************************************/
void LVSwitchView(HWND hwndListView, DWORD dwView)
{
	DWORD dwStyle = GetWindowLong(hwndListView, GWL_STYLE);
	SetWindowLong(hwndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	LVResize(hwndListView, GetParent(hwndListView));
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

/**************************************************************************
   LVRemoveExStyle()
**************************************************************************/
void LVRemoveExStyle(HWND hwndListView, DWORD dwNewStyle)
{
	DWORD dwStyle = SendMessage(hwndListView, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	dwStyle &= ~dwNewStyle;
	SendMessage(hwndListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);
}


/* =============================
// and an APPLICATION RC DEPENDANT menu items -
void	LVSetViewType( HWND hwndListView, int iType ) {
	DWORD	dwt;
	int		i = 0;
	switch(iType) {
	case IDM_LARGEICONS: dwt = LVS_ICON; i++; break;
	case IDM_SMALLICONS: dwt = LVS_SMALLICON; i++; break;
   	case IDM_LIST: dwt = LVS_LIST; i++; break;
	case IDM_REPORT: dwt = LVS_REPORT; i++; break;
	}
	if( i )
		LVSwitchView( hwndListView, dwt );
}
  =================================== */

#define	lvStyle(a,b)	\
	if( *a )\
	{\
		if( !( dwf & b ) )\
			LVAddExStyle(hwndListView,b);\
	}else{\
		if( dwf & b )\
			LVRemoveExStyle(hwndListView,b);\
	}

void	LVSetDefault2(HWND hwndListView, PLVOPTIONS plvo)
{
	DWORD	dwf = 0;

	//LVSetViewType( hwndListView, plvo->lvo_iType ); // g_iType );
	LVSwitchView( hwndListView, LVS_LIST ); // g_iType );

	dwf = (DWORD)SendMessage( hwndListView,
		LVM_GETEXTENDEDLISTVIEWSTYLE,
		0, 
		0 );

	//   case IDM_HOVERSELECT:
	lvStyle( &plvo->lvo_iAddTrack, LVS_EX_TRACKSELECT );
	//   case IDM_CHECKBOXES:
	lvStyle( &plvo->lvo_iAddCheck, LVS_EX_CHECKBOXES );
	//   case IDM_GRIDLINES:
	lvStyle( &plvo->lvo_iAddGrid,  LVS_EX_GRIDLINES  );
	//	 case IDM_FULLROWSELECT:
	lvStyle( &plvo->lvo_iAddFull,  LVS_EX_FULLROWSELECT );
	//	 case IDM_HEADERDRAGDROP:
	lvStyle( &plvo->lvo_iAddHDrag, LVS_EX_HEADERDRAGDROP );
	//	 case IDM_SUBITEMIMAGES:
	lvStyle( &plvo->lvo_iAddISub,  LVS_EX_SUBITEMIMAGES );

//	InvalidateRect( hwndListView, NULL, TRUE );
//	UpdateWindow( hwndListView );

}

BOOL LVInitColumns2(HWND hwndListView, PLISTHEADERS plhs, INT cnt, PLVOPTIONS plvo)
{
	LV_COLUMN   lvColumn;
	INT         i, j;
	//initialize the columns
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//lvColumn.cx = 100;
	for(i = 0; i < cnt; i++)
	{
		//make the secondary columns smaller
      //if(i) lvColumn.cx = 50;
        lvColumn.cx       = plhs[i].lh_dwWidth;
		lvColumn.pszText  = plhs[i].lh_pTitle;  // szString[i];
        lvColumn.iSubItem = i;

		//SendMessage(hwndListView, LVM_INSERTCOLUMN, (WPARAM)i, (LPARAM)&lvColumn);
        j = ListView_InsertColumn(hwndListView, i, &lvColumn);
        if( j == -1 )
           return FALSE;
	}

//	InsertListViewItems( hwndListView );

	// set listview default view
	LVSetDefault2( hwndListView, plvo );

    g_iLVColCnt = cnt;

	return TRUE;

}

BOOL  setcolumns( HWND hLV )
{
   PLVOPTIONS plvo = &sLVOptions;
   return( LVInitColumns2( hLV, &sOutlineList[0], LVcolumncount(), plvo ) );
   // &sLVOptions ) );  // = nOutlineCols
   // return TRUE;
}

int ResizeLV( HWND hLV, LPRECT lpr )
{
	int	i = 0;
	if(( hLV           ) &&
		( IsWindow(hLV) ) &&
		( lpr           ) )
	{
		i = MoveWindow( hLV,
			lpr->left,              // x
			lpr->top,               // y
			(lpr->right  - lpr->left), // cx
			(lpr->bottom - lpr->top),  // cy
			TRUE );
	}
	return i;
}

/******************************************************************************
   LVResize
******************************************************************************/
void LVResize(HWND hwndListView, HWND hwndParent)
{
	RECT  rc;

	GetClientRect(hwndParent, &rc);

   rc.top = rc.bottom / 2;
   
	ResizeLV( hwndListView, &rc );
//	MoveWindow( hwndListView,
//		rc.left, rc.top,
//		rc.right - rc.left,
//		rc.bottom - rc.top,
//		TRUE );

}


static HWND _s_hListView = 0;
BOOL  Set_LV_ON(VOID)
{
   if( _s_hListView )
   {
       g_bLVOff = FALSE;
//      g_bChgLVOff = TRUE;
      g_hListView = _s_hListView;
      //view_update_lv( current_view );  // ensure the soon-to-be-visible lv is up-to-date
      ShowWindow(_s_hListView, SW_SHOW);  // let the PAINT go
      _s_hListView = 0; // kill store of handle
      return TRUE;
   }
   return FALSE;
}

BOOL  Set_LV_OFF(VOID)
{
   if(g_hListView)
   {
      g_bLVOff = TRUE;
//      g_bChgLVOff = TRUE;
      _s_hListView = g_hListView;
      g_hListView  = 0;
      ShowWindow(_s_hListView, SW_HIDE);
      return TRUE;
   }
   return FALSE;
}


DWORD GetDllVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;
    hinstDll = LoadLibrary(lpszDllName);
	if(hinstDll) {
        DLLGETVERSIONPROC pDllGetVersion;
        pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");
        /* Because some DLLs may not implement this function, you
         * must test for it explicitly. Depending on the particular 
         * DLL, the lack of a DllGetVersion function may
         * be a useful indicator of the version.
        */
        if(pDllGetVersion) {
            DLLVERSIONINFO dvi;
            HRESULT hr;
            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
            hr = (*pDllGetVersion)(&dvi);
            if(SUCCEEDED(hr)) {
                dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}

BOOL  IsComCtl32400( VOID )
{
   BOOL  bRet = FALSE;
//   if( GetDllVersion( TEXT("comctl32.dll") ) >= PACKVERSION(4,00) )
   if( GetDllVersion( TEXT("comctl32.dll") ) >= PACKVERSION(4,70) ) {
      bRet = TRUE;
   }
   return bRet;
}



HWND LVCreateWindow(HINSTANCE hInstance, HWND hwndParent)
{

	DWORD       dwStyle;
	HWND        hwndListView;
#ifndef  NO_IMAGE_LISTS
	HIMAGELIST  himlSmall;
	HIMAGELIST  himlLarge;
	SHFILEINFO  sfi;
#endif   // #ifndef  NO_IMAGE_LISTS
	BOOL        bSuccess = TRUE;

#ifdef  NO_IMAGE_LISTS
	dwStyle =   WS_TABSTOP | 
            WS_CHILD | 
            WS_BORDER | 
            // LVS_AUTOARRANGE |
            // LVS_REPORT | 
            LVS_LIST |
            WS_VISIBLE;
#else    // #ifndef  NO_IMAGE_LISTS
	dwStyle =   WS_TABSTOP | 
            WS_CHILD | 
            WS_BORDER | 
            LVS_AUTOARRANGE |
            LVS_REPORT | 
            LVS_EDITLABELS |
            LVS_SHAREIMAGELISTS |
            WS_VISIBLE;
#endif   // #ifndef  NO_IMAGE_LISTS

    LVInit();   // ensure common control ocx is loaded

	hwndListView = CreateWindowEx( WS_EX_CLIENTEDGE,	// ex style
		WC_LISTVIEW,			// class name - defined in commctrl.h
		NULL,					// window text
		dwStyle,				// style
		0,						// x position
		0,						// y position
		0,						// width
		0,						// height
		hwndParent,				// parent
		(HMENU)IDC_LISTVIEW,	// ID
		hInst,				// instance
		NULL );					// no extra data
   sprtf( "CREATED: Listview window (class=WC_LISTVIEW) hwnd=%#x."MEOR,
           hwndListView );

	if(!hwndListView)
		return NULL;

	LVResize(hwndListView, hwndParent);

#ifndef  NO_IMAGE_LISTS
	// ====================================================
	// set the large and small icon image lists
	himlSmall = (HIMAGELIST)SHGetFileInfo( TEXT("C:\\"),
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON );

	himlLarge = (HIMAGELIST)SHGetFileInfo( TEXT("C:\\"),
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | SHGFI_LARGEICON);

	if(( himlSmall ) &&
		( himlLarge ) )
	{
      // we have the LARGE and SMALL image lists
		SendMessage(hwndListView, LVM_SETIMAGELIST, (WPARAM)LVSIL_SMALL, (LPARAM)himlSmall );
		SendMessage(hwndListView, LVM_SETIMAGELIST, (WPARAM)LVSIL_NORMAL, (LPARAM)himlLarge);
	}
	// ====================================================
#endif   // #ifndef  NO_IMAGE_LISTS

   {
      HWND  hwnd = hwndListView;
      HFONT hfont = 0;

//      if( g_hFF8bold )
//         hfont = g_hFF8bold;

//      if( g_hfCN8 && !hfont )
//         hfont = g_hfCN8;

//      if( g_hFixedFont8 && !hfont )
//         hfont = g_hFixedFont8;

      if(hfont)
         SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, TRUE);

   }

	return hwndListView;
}


LRESULT  LVInsertItem( char *pstg )
{
    LVITEM   lvItem;
    LRESULT  nIndex = -1;

    ZeroMemory( &lvItem, sizeof(LVITEM) );

    lvItem.mask = LVIF_TEXT;   // add TEXT
    lvItem.pszText = pstg;

    if (VLV(g_hListView)) {
        lvItem.iItem = SendMessage(g_hListView, LVM_GETITEMCOUNT, 0, 0);
        lvItem.iItem++;
        nIndex = SendMessage( g_hListView,
				LVM_INSERTITEM,
				(WPARAM)0,
				(LPARAM)&lvItem);
    }
    return nIndex;
}

// =================================
// SAMPLE CODE
/******************************************************************************
   LVCreateWindow
******************************************************************************/
// CreateListView: Creates a list-view control in report view.
// Returns the handle to the new control
// TO DO:  The calling procedure should determine whether the handle is NULL, in case 
// of an error in creation.
//
// HINST hInst: The global handle to the applicadtion instance.
// HWND  hWndParent: The handle to the control's parent window. 
//
HWND CreateListView (HWND hwndParent) 
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rcClient;                       // The parent window's client area.

    GetClientRect (hwndParent, &rcClient); 

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindow(WC_LISTVIEW, 
                                     TEXT(""),
                                     WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
                                     0, 0,
                                     rcClient.right - rcClient.left,
                                     rcClient.bottom - rcClient.top,
                                     hwndParent,
                                     (HMENU)0, //IDM_CODE_SAMPLES,
                                     hInst,
                                     NULL); 

    return (hWndListView);
}

// SetView: Sets a list-view's window style to change the view.
// hWndListView: A handle to the list-view control. 
// dwView:       A value specifying the new view style.
//
VOID SetView(HWND hWndListView, DWORD dwView) 
{ 
    // Retrieve the current window style. 
    DWORD dwStyle = GetWindowLong(hWndListView, GWL_STYLE); 
    
    // Set the window style only if the view bits changed.
    if ((dwStyle & LVS_TYPEMASK) != dwView) 
    {
        SetWindowLong(hWndListView,
                      GWL_STYLE,
                      (dwStyle & ~LVS_TYPEMASK) | dwView);
    }               // Logical OR'ing of dwView with the result of 
}                   // a bitwise AND between dwStyle and 
                    // the Unary complenent of LVS_TYPEMASK.

// InitListViewColumns: Adds columns to a list-view control.
// hWndListView:        Handle to the list-view control. 
// Returns TRUE if successful, and FALSE otherwise. 
BOOL InitListViewColumns(HWND hWndListView, int C_COLUMNS, char **pstgs) 
{ 
    TCHAR szText[256];     // Temporary buffer.
    LVCOLUMN lvc;
    int iCol;

    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text,
    // and subitem members of the structure are valid.
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    // Add the columns.
    for (iCol = 0; iCol < C_COLUMNS; iCol++)
    {
        lvc.iSubItem = iCol;
        lvc.pszText = szText;
        lvc.cx = 100;               // Width of column in pixels.

        if ( iCol < 2 )
            lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
        else
            lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

        // Load the names of the column headings from the string resources.
        //LoadString(g_hInst,
        //           IDS_FIRSTCOLUMN + iCol,
        //           szText,
        //           sizeof(szText)/sizeof(szText[0]));
        strcpy(szText,pstgs[iCol]); 
        // Insert the columns into the list view.
        if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
            return FALSE;
    }
    
    return TRUE;
} 


// InsertListViewItems: Inserts items into a list view. 
// hWndListView:        Handle to the list-view control.
// cItems:              Number of items to insert.
// Returns TRUE if successful, and FALSE otherwise.
BOOL InsertListViewItems(HWND hWndListView, int cItems, char *pstg)
{
    LVITEM lvI;

    // Initialize LVITEM members that are common to all items.
    lvI.pszText   = pstg; // LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.mask      = LVIF_TEXT; // | LVIF_STATE; // | LVIF_IMAGE 
    lvI.stateMask = 0;
    lvI.iSubItem  = 0;
    lvI.state     = 0;

    // Initialize LVITEM members that are different for each item.
    for (int index = 0; index < cItems; index++)
    {
        lvI.iItem  = index;
        lvI.iImage = index;
    
        // Insert items into the list.
        if (ListView_InsertItem(hWndListView, &lvI) == -1)
            return FALSE;
    }

    return TRUE;
}

char *Get_LV_Text()
{
    return "";
}

// HandleWM_NOTIFY - Handles the LVN_GETDISPINFO notification code that is 
//         sent in a WM_NOTIFY to the list view parent window. The function 
//        provides display strings for list view items and subitems.
//
// lParam - The LPARAM parameter passed with the WM_NOTIFY message.
// rgPetInfo - A global array of structures, defined as follows:
//         struct PETINFO
//        {
//            TCHAR szKind[10];
//            TCHAR szBreed[50];
//            TCHAR szPrice[20];
//        };
//
//        PETINFO rgPetInfo[ ] = 
//        {
//            {TEXT("Dog"),  TEXT("Poodle"),     TEXT("$300.00")},
//            {TEXT("Cat"),  TEXT("Siamese"),    TEXT("$100.00")},
//            {TEXT("Fish"), TEXT("Angel Fish"), TEXT("$10.00")},
//            {TEXT("Bird"), TEXT("Parakeet"),   TEXT("$5.00")},
//            {TEXT("Toad"), TEXT("Woodhouse"),  TEXT("$0.25")},
//        };
//
void HandleWM_NOTIFY(LPARAM lParam)
{
    NMLVDISPINFO* plvdi;

    switch (((LPNMHDR) lParam)->code)
    {
        case LVN_GETDISPINFO:

            plvdi = (NMLVDISPINFO*)lParam;

            switch (plvdi->item.iSubItem)
            {
                case 0:
                    plvdi->item.pszText = Get_LV_Text(); // rgPetInfo[plvdi->item.iItem].szKind;
                    break;
                      
                //case 1:
                //    plvdi->item.pszText = rgPetInfo[plvdi->item.iItem].szBreed;
                //    break;
                
                //case 2:
                //    plvdi->item.pszText = rgPetInfo[plvdi->item.iItem].szPrice;
                //    break;
                
                default:
                    break;
            }

        break;

    }
    // NOTE: In addition to setting pszText to point to the item text, you could 
    // copy the item text into pszText using StringCchCopy. For example:
    //
    // StringCchCopy(plvdi->item.pszText, 
    //                         plvdi->item.cchTextMax, 
    //                         rgPetInfo[plvdi->item.iItem].szKind);

    return;
}

// =================================
// eof - Sudo_List.cxx
