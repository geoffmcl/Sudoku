// Sudo-Menu.cxx
// Handle file menu

#include "Sudoku.hxx"

DWORD gdwMaxFiles = IDM_FILEMax - IDM_FILE1;

#ifdef WIN32    // Windows MENU handling
//////////////////////////////////////////////////////////////////////////////////////////////
#include "Sudo_Paint.hxx"
#include "Sudo_Menu.hxx"
#include "Sudo_Ini.hxx"
#include "Sudo_File.hxx"

DWORD gdwlMenuFlag = 0;
#define M_Fil           1
DWORD gdwMenuMax = 20;  // Limit MRU menu size
BOOL gfAddSep = FALSE;  // add separator only once
DWORD gdwLastMax = 0;

// =====================================
// MENU actions *** KEEP IN SYNC WITH Sudocu.rc ***
#define  MENU_FILE      0
#define  MENU_EDIT      1
#define  MENU_VIEW      2
#define  MENU_OPTION    3
#define  MENU_ABOUT     4

#define  mf_Enab     (MF_BYCOMMAND | MF_ENABLED)
#define  mf_Disab    (MF_BYCOMMAND | MF_DISABLED | MF_GRAYED)
#define  EMI(a,b) EnableMenuItem(hMenu, a, ((b) ? mf_Enab : mf_Disab))
#define  CMI(a,b) CheckMenuItem(hMenu,  a, ((b) ? MF_CHECKED : MF_UNCHECKED) )

bool Selection_Has_Value()
{
    bool bret = false;
    int row, col;
    if (Is_Selected_Valid()) {
        PABOX2 pb = get_curr_box();
        get_curr_selRC( &row, &col );
        if (VALIDRC(row,col)) {
            int val = pb->line[row].val[col];
            if (val) {
                bret = true;
            }
        }
    }
    return bret;
}

bool Have_Single_Choice()
{
    //if (one_value_count && (NO_value_count == 0))
        return true;
    //return false;
}

// menu initialisation
long  Do_WM_INITMENUPOPUP( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    long lRet = 0;
    HMENU    hMenu = (HMENU)wParam;
    DWORD    dwPos = LOWORD(lParam);
    BOOL     bSys  = HIWORD(lParam);
    char *cp = 0;
    char *cp2 = 0;
    if( bSys ) {
        // is the windows (system) menu
        lRet = 1;   // use default processing
    } else {

        if (!gdwlMenuFlag)
            Add_INI_Files_to_Menu();

       switch( dwPos )
       {
       case MENU_FILE:
           // check current loaded file 
           cp = Get_First_INI_File();
           cp2 = Get_Act_File();
           if (cp && cp2 && (strcmp(cp,cp2) == 0)) {
               CMI( IDM_FILE1, true );
           }
           // if current active file is 'Untitled', disable reload
           // but could actually reload from tempgen.txt, the LAST entry
           if (cp2 && (strcmp(cp2,g_pSpecial) == 0)) {
               EMI(ID_FILE_RE,false);
           }
           break;
       case MENU_EDIT:
           EMI( ID_EDIT_UNDO, Got_Change_Stack() );
           EMI( ID_EDIT_REDO, Got_Redo_Stack() );
           break;

       case MENU_VIEW:
           if (!g_hMsgDialog || !IsWindow(g_hMsgDialog)) {
               EMI(ID_VIEW_TOGGLESHOWMESSAGES, FALSE);
           } else {
               CMI(ID_VIEW_TOGGLESHOWMESSAGES, Get_Dlg_OnShow());
           }
           break;

       case MENU_OPTION:
           CMI( ID_OPTIONS_SHOWHINTS, g_bShowHints);
           CMI( ID_OPTIONS_AUTOLOAD, g_bAutoLoad);
           EMI( ID_OPTIONS_DELETECELL, Selection_Has_Value() );
           EMI( ID_OPTIONS_AUTOCOMPLETE, Have_Single_Choice() );    // actually only take ONE step
           if (Have_Single_Choice()) {
               EMI( ID_OPTIONS_SOLVE, TRUE );   // take each step, until none available
               CMI( ID_OPTIONS_SOLVE, g_bAutoSolve );
           } else {
               EMI( ID_OPTIONS_SOLVE, FALSE );   // take each step, until none available
           }
           CMI( ID_OPTIONS_SHOWSELECTION, g_bShwSel );
           CMI( ID_OPTIONS_MARK, g_bMrkNM );  // also mark NON-members if ONE setval used
           CMI( ID_OPTIONS_PAINTCHAINNUMBER, g_bPntChN ); // paint 'chain_number' instead of sequence number
           break;
       }
   }
   return lRet;
}


/* -----------------------------------------------------------------------
	Remove File Name from the 'File' menu pop-up
	for the MAX_FILS configured
   ----------------------------------------------------------------------- */
DWORD DeleteMRU( HMENU hSMenu )
{
	DWORD	    cyc, cnt;
	DWORDLONG	bflg = M_Fil ;		/* Init BIT flag - Max 15 shifts left */
	UINT	    fMsg = IDM_FILE1 ;	/* Start IDM_FILE[n] ... */
    DWORD       max = gdwMaxFiles;

    if (max > gdwMenuMax)
        max = gdwMenuMax;

    cnt = 0;
	for( cyc = 0; cyc < max; cyc++ )
	{
		if( gdwlMenuFlag & bflg )
		{
			DeleteMenu( hSMenu, fMsg, MF_BYCOMMAND );
			gdwlMenuFlag &= ~bflg;
         cnt++;
		}
		fMsg++;  // bump to next
		bflg = bflg << 1 ;
	}
   return cnt;
}

/* -----------------------------------------------------------------------
	Add the File Names from the User's preferences buffer -
	Loaded from the ...INI file - to the 'File' Pop-up
	Menu section
   ----------------------------------------------------------------------- */
DWORD AppendMRU( HMENU hSMenu, vSTG vs )
{
    DWORD	    cnt;
	LPTSTR      lps;
	DWORDLONG	bflg;
	UINT	    fMsg;
    vSTGi       ii;
    DWORD       max = gdwMaxFiles;

    if (max > gdwMenuMax)
        max = gdwMenuMax;
	cnt  = 0;

	// NOTE: Should be upper max of say 64 or 128 max!!!
	if( vs.size() )
	{
        bflg = M_Fil;
        fMsg = IDM_FILE1 ;	// Init IDM_FILE[n] - Max 20-64 reserved
        for (ii = vs.begin(); ii != vs.end(); ii++)
		{
            string s;
            s = *ii;
            // no structured menu list here
            //  pmwl = (PMWL)pNext;
            // if( pmwl->wl_dwFlag & flg_DELETE )  // if it has any DELETE flags
            // continue;
            lps  = (LPTSTR) s.c_str();
			if( !gfAddSep ) {
				AppendMenu( hSMenu, MF_SEPARATOR, 0, 0 ) ;
				gfAddSep = TRUE;
			}
			AppendMenu( hSMenu,
				MF_ENABLED | MF_STRING,
				fMsg,
				lps );
            // Here ONLY one file loaded at a time, so no need for this...
            // if( pmwl->wl_dwFlag & flg_IsLoaded ) {
            // CheckMenuItem( hSMenu, fMsg, /* CHECK/UNCHECK THE FILE! */
            //   ( MF_BYCOMMAND | MF_CHECKED ) ); // show as loaded
            // }

			gdwlMenuFlag |= bflg;
			bflg = bflg << 1;
			fMsg += 1;
			cnt++;
			if( cnt >= max )
				break;
        }

        // set the count ADDED to the DROPDOWN MENU ITEM
        gdwLastMax = cnt;	// Keep the COUNT added
        // =============================================
	}
   return cnt;
}

/* -----------------------------------------------------------------------
   DWORD SetFileMRU( HWND hwnd, PLIST_ENTRY pHead )

	Handle 'File' POP-UP changes
	(a) Remove any existing file names
	(b) Add any new list of names
	(c) Check (or Uncheck) the loaded (not loaded) file
   ----------------------------------------------------------------------- */
DWORD SetFileMRU( HWND hwnd, vSTG vs )
{
    DWORD iRet   = 0;
	HMENU	hMenu, hSMenu;
	if( ( hMenu  = GetMenu( hwnd )                ) &&
       ( hSMenu = GetSubMenu( hMenu, MENU_FILE ) ) )
    {
        if( gdwlMenuFlag )	/* If there are previous items */
            DeleteMRU( hSMenu ) ;	/* Remove them all NOW */
        iRet = AppendMRU( hSMenu, vs );
   }
   return iRet;
}

#define  AM(a,b)  Append_2_Menu(hMenu,uFlags,a,b)
#define  AM_SEP   AppendMenu(hMenu,MF_SEPARATOR,0,0)

BOOL Append_2_Menu( HMENU hMenu, UINT uFlags, UINT uCmd, PTSTR lpb )
{
   BOOL  bRet = FALSE;
   //if( Not_Yet_Added( uCmd ) )
      bRet = AppendMenu( hMenu, uFlags, uCmd, lpb );
   return bRet;
}

bool bFromContext = false;
int cont_Row, cont_Col;

void Do_ID_OPTIONS_DELETECELL( HWND hWnd )
{
    PABOX2 pb = get_curr_box();
    int row, col, val;
    if (Is_Selected_Valid() || bFromContext) {
        if (bFromContext) {
            row = cont_Row;
            col = cont_Col;
        } else {
            get_curr_selRC( &row, &col );
        }
        if (VALIDRC(row,col)) {
            val = pb->line[row].val[col];
            if (val) {
                Change_Box(pb,row,col,-val);
            }
        }
    }
}

// Handler for
// WM_CONTEXTMENU -  WPARAM wParam; LPARAM lParam;
// wParam 
// Handle to the window in which the user right-clicked the mouse.
// This can be a child window of the window receiving the message. 
// For more information about processing this message, see the Remarks section. 

// lParam 
// The low-order word specifies the horizontal position of the cursor,
// in screen coordinates, at the time of the mouse click. 
// The high-order word specifies the vertical position of the cursor, 
// in screen coordinates, at the time of the mouse click. 
VOID Do_WM_CONTEXTMENU( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    POINT pnt;
    HMENU hMenu;
    HWND  hChild = (HWND)wParam;   // get PARENT handle
    char * cp; //= GetNxtBuf();
    int cnt  = 0;
    // PPolyGlobs ppg = get_poly_globs();

    bFromContext = false;
    if ( hChild != g_hWnd )
        return;

    pnt.x = LOWORD(lParam);
    pnt.y = HIWORD(lParam);
    hMenu = CreatePopupMenu(); // create a POPUP Menu
    if ( !hMenu )
        return;

    PABOX2 pb = get_curr_box();
    UINT uFlags = MF_STRING | MF_BYCOMMAND;
    UINT uOpts  = TPM_LEFTALIGN | TPM_TOPALIGN;

    char *tb = GetNxtBuf();

    // Top of Context Menu
    AM(ID_OPTIONS_ONESTEP,"One Step");
    AM( ID_OPTIONS_SOLVE, "Solve!" );

    AM_SEP;

    // ==============================================
    // Add MENU items
     sprintf(tb,"View Links IB %s IW %s Alt %s Ext %s for ",
        g_bIBL ? "On" : "Off",
        g_bIWL ? "On" : "Off",
        g_bASW ? "On" : "Off",
        g_bExtChn ? "On" : "Off" );
    cp = g_szCandList;
    int len = (int)strlen(cp);
    int i, c, val;
    for (i = 0; i < len; i++) {
        c = cp[i];
        if ( !ISDIGIT(c) ) continue;
        val = c - '0';
        if ( !val) continue;
        sprintf(EndBuf(tb),"%d",val);
    }
    if (len)
        AM(ID_VIEW_LINKS, tb);
    AM(ID_VIEW_STRONGLINKS, "View Strong Links...");
    AM(ID_VIEW_UNIQUERECTANGLES, "View Unique Rect.");
    AM(ID_VIEW_NAKEDTRIPLE, "View Naked Triple");
    AM(ID_VIEW_LOCKEDEXCLUSION, "View Locked Excl.");
    AM(ID_VIEW_XWING, "View X-Wing Scan");
    AM(ID_VIEW_XCYCLESSCAN, "View X-Cycle Scan");
    AM(ID_VIEW_HIDDENTRIPLE, "View Hidden Triple"); 
    AM(ID_VIEW_AICSCAN,"View AIC Scan");
    AM(ID_VIEW_XYZWING,"View XYZ-Wing Scan");

    AM_SEP;

    cnt = 0;
    if (Got_Change_Stack()) {
        AM(ID_EDIT_UNDO, "Undo one step");
        cnt++;
    }
    if (Got_Redo_Stack()) {
        AM(ID_EDIT_REDO, "Redo one step");
        cnt++;
    }
    if (cnt)
        AM_SEP;

    if (Is_Selected_Valid()) {
        int row, col;
        int val = 0;
        get_curr_selRC( &row, &col );
        if ( VALIDRC(row,col) )
            val = pb->line[row].val[col];
        if (val) {
            cont_Row = row;
            cont_Col = col;
            bFromContext = true;
            cp = GetNxtBuf();
            sprintf(cp,"Delete Value %d",val);
            AM( ID_OPTIONS_DELETECELL, cp );
        }
    }
    //if (Have_Single_Choice()) {
    //    AM( ID_OPTIONS_AUTOCOMPLETE, "Take Step" );
    //    AM( ID_OPTIONS_SOLVE, "Solve!" );
    //}

    cp = GetNxtBuf();
    sprintf(cp,"Toggle Hints %s", g_bShowHints ? "Off" : "On");
    AM( ID_OPTIONS_SHOWHINTS, cp);

    sprintf(cp,"Toggle Show Selection %s", g_bShwSel ? "Off" : "On");
    AM( ID_OPTIONS_SHOWSELECTION, cp );

    sprintf(cp,"Toggle Show Marked %s", g_bMrkNM ? "Off" : "On");
    AM( ID_OPTIONS_MARK, cp );  // also mark NON-members if ONE setval used

    sprintf(cp,"Toggle Add Chain Numbers %s", g_bPntChN ? "Off" : "On");
    AM( ID_OPTIONS_PAINTCHAINNUMBER, cp ); // paint 'chain_number' instead of sequence number

    if (g_hMsgDialog) {
        AM_SEP;
        sprintf(cp,"Toggle Message Show %s", Get_Dlg_OnShow() ? "Off" : "On");
        AM(ID_VIEW_TOGGLESHOWMESSAGES, cp); // Toggle_Dlg_Show();
    }

    AM_SEP;

    AM( IDM_EXIT,  "Exit Application [ESC]!" );
    AM( ID_FILE_EXITWITHOUTSAVE, "Exit Without Save!" );

    TrackPopupMenu( hMenu,         // handle to shortcut menu
        uOpts,   // TPM_CENTERALIGN | TPM_VCENTERALIGN, // options
        pnt.x,   // horizontal position
        pnt.y,   // vertical position
        0,       // reserved, must be zero
        hWnd,    // handle to owner window
        0 );     // ignored

    DestroyMenu(hMenu);

}

//////////////////////////////////////////////////////////////////////////////////////////////
#else // !#ifdef WIN32 - TODO: menu handling
//////////////////////////////////////////////////////////////////////////////////////////////

DWORD SetFileMRU( HWND hwnd, vSTG vs )
{
    DWORD iRet = 0;
    // TODO: When we have a menu...
    return iRet;
}


//////////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n - menu handling

// eof - Sudo_Menu.cxx
