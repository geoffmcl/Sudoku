/*\
 * Sudoku.cxx  : Defines the entry point for the application.
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
/* ==================================================
   Sudoku project
   |S|U| |  Created: (C) Geoff R. McLane - Aug 2012
   | |D|O|  <reports .at. geoffair .dot. info>
   |K|U| |  License: GNU GPL v.2 (or later)
   This program is free software. You can redistribute it
   and/or modify it under the terms of the GNU General 
   Public Licence as published by the Free Software Foundation.
   This program is distributed in the hope it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty 
   of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
   the GNU General Public License for more details.
   ================================================== */

#include "Sudoku.hxx"

// Global Variables:
BOOL        g_bChanged = FALSE;
HWND        g_hWnd = 0;
HINSTANCE   hInst = 0;			// current instance

#define MAX_LOADSTRING 100
#ifndef EOL_CHR
#define EOL_CHR "\r\n"
#endif

// Global Variables:
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

BOOL        g_bAShiftDown = FALSE;   // shift key DOWN (VK_SHIFT)
BOOL        g_bCtrlDown   = FALSE;    // CTRL key (VK_CONTROL)
BOOL        g_bAltDown    = FALSE;     // ALT key  (VK_MENU)
BOOL        g_bLBDown     = FALSE;
BOOL        g_bShowHints  = TRUE;
BOOL        gChgHint      = FALSE;
BOOL        g_bComCtl6    = FALSE;
BOOL        g_bDoneInit   = FALSE;

HWND g_hProgress = 0;
HWND g_hMsgDialog = 0;

RECT g_sRect;
BOOL res_scn_rect = FALSE;

#ifdef ADD_LIST_VIEW
HWND g_hListBox = 0;
#endif

////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
////////////////////////////////////////////////////////////////////////////////////////////

// add a timer message
#define ADD_TIMER1
#define TIMER_ID1 1234
#define TIMER_INTERVAL1 200
UINT uiTimer1;
// ====================

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
long                Do_WM_INITMENUPOPUP( HWND hWnd, WPARAM wParam, LPARAM lParam );


#define MMX_OPTS    64

int ParseArgs(int argc, char **argv);

// 20120917 - Orig '#' started comment line, but add
// '//' and ';' as comment lines

int Load_Input_File(char *file, struct stat *psb)
{
    if (psb->st_size == 0)
        return 0; // quietly IGNORE a zero length input file
    FILE *fp = fopen(file,"r");
    if (!fp)
        return 1;
    int size = psb->st_size + 2;
    char *pb = (char *)malloc( size + (sizeof(char *) * MMX_OPTS));
    CHKMEM(pb);
    int res = fread(pb,1,psb->st_size,fp);
    if (res == 0) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    int i, c;
    char *bgn = pb;
    int off = 0;
    char **argv = (char **)&pb[size];
    int argc = 0;
    argv[argc++] = "dummy";
    for (i = 0; i < res; i++) {
        c = pb[i];
        if (off == 0) {
            if (c == '\n') continue;
            if (c == '\r') continue;
            if ((c <= ' ')&&(off == 0)) continue;
            if ((c == '#')||( c == ';')) {
                i++;
                for (; i < res; i++) {
                    c = pb[i];
                    if (c == '\n') break;
                    if (c == '\r') break;
                }
                continue;
            } else if ((c == '/')&&((i + 1) < res)&&(pb[i+1] == '/')) {
                // skip until END OF LINE
                i++;
                for (; i < res; i++) {
                    c = pb[i];
                    if (c == '\n') break;
                    if (c == '\r') break;
                }
                continue;
            }
        }
        // accumulate
        if (( c == '\n') || (c == '\r') || (c == '#')) {
            // back off any white space
            while(off--) {
                if (bgn[off] > ' ')
                    break;
                bgn[off] = 0;
            }
            if (c == '#') {
                i++;
                for (; i < res; i++) {
                    c = pb[i];
                    if (c == '\n') break;
                    if (c == '\r') break;
                }
            }
            off++;
            argv[argc++] = bgn; // set begin of arg
            bgn += off; // bump buffer
            off = 0;    // restart offset
        } else {
            bgn[off++] = (char)c;
            bgn[off] = 0;
        }
    }

    // catch any last entry in file, which does NOT have a CR or LF trainig!!!
    if (off) {
        while(off--) {
            if (bgn[off] > ' ')
                break;
            bgn[off] = 0;
        }
        if (off)
            argv[argc++] = bgn;
    }

    off = 0;
    if (argc > 1) {
        // quietly ignore no arguments
        off = ParseArgs(argc, argv);
    }
    free(pb);
    return off;
}

int ParseArgs(int argc, char **argv) 
{
    int i, res;
    char *tb = GetNxtBuf();
    for (i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (*arg == '-') {
            sprintf(tb,"ERROR: Unknow command\n%s\nAt present there are NO options\nWill abort on OK",arg);
            MB(tb);
            return 1;
        } else if (*arg == '@') {
            res = 1;
            if (is_file_or_directory(&arg[1]) == 1) {
                res = Load_Input_File(&arg[1], Get_Stat_Buf());
            }
            if (res) {
                sprintf(tb,"ERROR: Unable to load input file\n%s\nCheck file name and location\nWill abort on OK",&arg[1]);
                MB(tb);
                return 1;
            }
        } else {
            // bare argument - assume a file to load
            if (is_file_or_directory(arg) == 1)
                Reset_Active_File(arg);
            else {
                sprintf(tb,"ERROR: Unable to 'stat' file\n%s\nCheck file name and location\nWill abort on OK",arg);
                MB(tb);
                return 1;
            }
        }
    }
    return 0;
}

void add_app_begin()
{
    int curr = add_sys_time(1);
    sprtf("Begin Application\n");
    add_sys_time(curr);
    if (res_scn_rect) {
        sprtf("Screen Workarea [%s]\n", Get_Rect_Stg(&g_sRect));
    } else {
        sprtf("Get workarea FAILED!\n");
    }
}

void add_app_end()
{
    int curr = add_sys_time(1);
    sprtf("End Application\n");
    add_sys_time(curr);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

    res_scn_rect = SystemParametersInfo(
        SPI_GETWORKAREA, // UINT uiAction,
        0,               // UINT uiParam,
        &g_sRect,        // PVOID pvParam,
        0 );             // UINT fWinIni

    ReadINI();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SUDOKU, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

    //g_bComCtl4 = IsComCtl32400(); // now seeks min 4.70 for LISTVIEW
    if ( ParseArgs(__argc, __argv) )
    {
        return FALSE;
    }

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

#ifdef	ADD_TIMER1
   if( !(uiTimer1 = SetTimer( g_hWnd, TIMER_ID1, TIMER_INTERVAL1, NULL)) ) {
      MB( "Failed in get Timer! Aborting ..." );
      return FALSE;
	}
#endif	/* ADD_TIMER1 */

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SUDOKU));

    g_bDoneInit = TRUE;
    add_app_begin();
#ifndef NDEBUG
    Debug_Flag_Set(cf_ELIM|cf_PRIM|cl_Perm|cl_Others);
#endif
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
        // skip dialog box messages
        if (g_hProgress && IsWindow(g_hProgress) && IsDialogMessage(g_hProgress, &msg))
            continue;
#ifdef ADD_LISTVIEW
        if (g_hListBox && IsWindow(g_hListBox) && IsDialogMessage(g_hListBox, &msg))
            continue;
#endif
        if (g_hMsgDialog && IsWindow(g_hMsgDialog) && IsDialogMessage(g_hMsgDialog, &msg))
            continue;

		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    //WriteINI();
    //clean_up_paint_tools();
    //Free_Boxes2();
    //Kill_Change();

    add_app_end();
    close_log_file();
	return (int) msg.wParam;
}

void Post_Command( WPARAM cmd )
{
    PostMessage(g_hWnd, WM_COMMAND, cmd, 0);
}

int Get_Spots(PABOX2 pb)
{
    int spots = 0;
    int row, col, val;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val)
                spots++;
        }
    }
    return spots;
}


int Get_Spot_Count()
{
    int spots = 0;
    PABOX2 pb = get_curr_box();
    return Get_Spots(pb);
}

bool Check_Clear() 
{
    int res;
    int spots = Get_Spot_Count();
    if (g_bChanged && spots) { // total_empty_count
        char *tb = GetNxtBuf();
        sprintf(tb,"New will clears all %d current cells.", spots);
        strcat(tb,EOL_CHR"But current Sudoku box has been changed."EOL_CHR"Do you want to SAVE these results?"EOL_CHR"Click [Yes] to save this change."EOL_CHR"Click [ NO ] to clear all spots.");
        res = MB2(tb,"Save on New?");
        if (res == IDYES) {
            Do_ID_FILE_SAVE(g_hWnd);
            return false;
        }
    }
    return true;
}

VOID Make_First_Selection()
{
    PRECT prs = get_curr_sel(); // &g_rcSelected; // to MOVE selection LEFT
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( IsRectEmpty(prs) || !VALIDRC(row,col) || !Is_Selected_Valid() ) {
        Move_Selection(g_hWnd, 0, 0);
    }
}

VOID Do_ID_FILE_NEW()
{
    Check_Clear();
    Reset_Active_File(g_pSpecial);
    invalidate_box(get_curr_box());
    set_solution_valid(0);
    File_Reset();
    Make_First_Selection();
    set_repaint();
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SUDOKU));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SUDOKU);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SUDOKU2));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

    if (!Init_Common_Controls(g_hWnd))
    {
        return FALSE;
    }

    g_bComCtl6 = Got_Comm_Ctrl6();  // we have comctl32.dll 6.0, or above

#ifdef ADD_LISTVIEW
   g_hListView = LVCreateListView (g_hWnd);
   if (!g_hListView) 
   {
       return FALSE;
   }

   HFONT hf = Get_Font();
   if (hf) SendMessage(g_hListView, WM_SETFONT, (WPARAM)hf, TRUE);

   char *hdg = "Output Strings";
   char *stgs[1];
   stgs[0] = hdg;
   if ( LVInitColumns( 1, stgs) ) {
       ShowWindow(g_hListView, SW_SHOW);  // let the PAINT go
   } else {
       return FALSE;
   }

#endif // ADD_LISTVIEW

#ifdef ADD_MESSAGE_DIALOG
    CreateMessageDialog(g_hWnd);
#endif // #ifdef ADD_MESSAGE_DIALOG

   if (gbGotWP)
       SetWindowPlacement(g_hWnd,&g_sWP);
   else
       ShowWindow(g_hWnd, nCmdShow);

   UpdateWindow(g_hWnd);

   return TRUE;
}

void DoFileCommand( HWND hWnd, int wmId )
{
    char *tb;
    int res;
	if(( wmId >= IDM_FILE1   ) &&
       ( wmId <= IDM_FILEMax ) )
    {
        int off = wmId - IDM_FILE1;
        char *cp = Get_INI_File( off );
        if (cp) {
            char * cp2 = _strdup(cp);
            if (cp2) cp = cp2;
            char *cp3 = Get_Act_File();
            if (cp3 && (strcmp(cp,cp3) == 0)) {
                // hmmm, already loaded - what to do???
            } else {
                if (g_bChanged) {
                    // need to ASK to Save or not, BEFORE loading a different file
                    tb = GetNxtBuf();
                    sprintf(tb,"Warning, currently loaded file\n%s\nhas been changed!\nDo you want to SAVE this first?\n",
                        (cp3 ? cp3 : "Untitled") );
                    res = MB2(tb,"Save Current Changed File");
                    if (res == IDYES) {
                        Do_ID_FILE_SAVE(hWnd);
                    }
                }
                // Now we can LOAD this NEW file
                if (!Load_a_file( hWnd, cp )) {
                    tb = GetNxtBuf();
                    sprintf(tb,"Warning, attempt to load file\n%s\nFAILED!\nRemove from MRU list?\n",
                        cp );
                    res = MB2(tb,"Remove From MRU List");
                    if (res == IDYES) {
                        Remove_INI_File(cp);
                    }
                }
            }
            if (cp2) free(cp2);
        }
    }
}


LRESULT Do_WM_COMMAND(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
	int wmId, wmEvent;
	wmId    = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	// Parse the menu selections:
    //	if( (wParam >= IDM_FILE1) && (wParam <= IDM_FILEMax) )
	if(( wmId >= IDM_FILE1   ) &&
       ( wmId <= IDM_FILEMax ) ) {
        DoFileCommand( hWnd, wmId );
        return lRes;
    }

	switch (wmId)
	{
	case IDM_ABOUT:
        Do_IDM_ABOUT(hWnd);
		break;
	case IDM_EXIT:
        if (Check_Exit()) {
            DestroyWindow(hWnd);
        }
		break;

    case ID_FILE_EXITWITHOUTSAVE:
        g_bChanged = FALSE;
        DestroyWindow(hWnd);
        break;
    case ID_VIEW_UNIQUERECTANGLES:
        Mark_Unique_Rectanges2(get_curr_box());
        set_repaint(false);
        break;
    case ID_VIEW_LINKS: // calls Mark_Links(get_curr_box());
        Mark_Links(get_curr_box());
        set_repaint();
        break;
    case ID_VIEW_STRONGLINKS:
        g_bShwStLnks = FALSE;
        set_repaint();
        if (!Do_ID_VIEW_STRONGLINKS( hWnd ))
            break;
        //g_bShwStLnks = TRUE;
        gChgStLnks = TRUE;
        break;
    case ID_VIEW_NAKEDTRIPLE:
        Do_Naked_Triple_Scan(get_curr_box());
        set_repaint();
        break;
    case ID_VIEW_LOCKEDEXCLUSION:
        Mark_Locked_Excl(get_curr_box());
        set_repaint();
        break;
    case ID_VIEW_XWING:
        Do_Fill_XWing(get_curr_box());
        set_repaint();
        break;
    case ID_VIEW_XCYCLESSCAN:
        //Mark_Links(get_curr_box());
        //set_repaint();
        break;
    case ID_VIEW_HIDDENTRIPLE:
        Mark_Hidden_Triple(get_curr_box());
        set_repaint();
        break;
    case ID_VIEW_AICSCAN:
        Mark_AIC_Scans(get_curr_box());
        set_repaint();
        break;
    case ID_VIEW_XYZWING:
        Mark_XYZ_Wings(get_curr_box());
        set_repaint();
        break;
    case ID_VIEW_TOGGLESHOWMESSAGES:
        Toggle_Dlg_Show();
        break;
    case ID_OPTIONS_DELETECELL:
        Do_ID_OPTIONS_DELETECELL( hWnd );
        break;
    case ID_FILE_NEW:
        Do_ID_FILE_NEW();
        break;
    case ID_FILE_OPEN:
        Do_ID_FILE_OPEN(hWnd);
        break;
    case ID_FILE_SAVE:
        Do_ID_FILE_SAVE(hWnd);
        break;
    case ID_FILE_SAVETEMP:
        Do_ID_FILE_SAVETEMP(hWnd);
        break;
    case ID_FILE_RE:
        Do_ID_FILE_RE(hWnd);
        break;
    case ID_FILE_PRINT:
        Do_ID_FILE_PRINT(hWnd);
        break;
    case ID_FILE_IMPORT:
        if (g_bCtrlDown)
            Do_ID_FILE_IMPORT(hWnd);
        //else // appears it is a TAB???
        //    Move_Selection_Right(hWnd);
        break;
    case ID_FILE_OPENDIRECTORY:
        Do_ID_FILE_OPENDIRECTORY(hWnd);
        break;

    case ID_OPTIONS_SHOWHINTS:
        g_bShowHints = !g_bShowHints;
        gChgHint = TRUE;
        set_repaint();
        break;
    case ID_OPTIONS_AUTOCOMPLETE:
        g_bAutoComplete = true; // actually take ONE STEP
        break;
    case ID_OPTIONS_SOLVE:
        Start_Auto_Solve();
        break;
    case ID_OPTIONS_AUTOLOAD:
        g_bAutoLoad = !g_bAutoLoad;
        g_bALChg = TRUE;
        break;
    case ID_OPTIONS_ONESTEP:
        if (total_empty_count == 0) {
            Do_Solved_Message();
        } else {
            Do_ID_OPTIONS_ONESTEP(hWnd);
        }
        break;
    case ID_OPTIONS_MARK:
        g_bMrkNM = !g_bMrkNM;   // also mark NON-members if ONE setval used
        gChgMNM = TRUE;
        set_repaint();
        break;
    case ID_OPTIONS_PAINTCHAINNUMBER:
        //char szPntChN[] = "Paint_Chain_Number";
        g_bPntChN = !g_bPntChN;  // paint 'chain_number' instead of sequence number
        g_bChgPChN = TRUE;
        set_repaint();
        break;
    case ID_EDIT_UNDO:
        Do_ID_EDIT_UNDO(hWnd);
        break;
    case ID_EDIT_REDO:
        Do_ID_EDIT_REDO(hWnd);
        break;
    case ID_EDIT_TOGGLELISTVIEW:
        // TODO
        break;
    case ID_EDIT_GENERATE:
        Do_ID_EDIT_GENERATE(hWnd);
        break;
    case ID_OPTIONS_RUNTEST:
        //run_test(get_curr_box());
        run_test3(get_curr_box());
        break;
    case ID_OPTIONS_SHOWHINT:
        Do_ID_OPTIONS_SHOWHINT();
        break;
    case ID_OPTIONS_SHOWSELECTION:
        g_bShwSel = !g_bShwSel;
        g_bChgSS = TRUE;
        set_repaint();  // paint new state
        break;
    // TODO case ID_OPTIONS_OVERWRITE: // allow overwrite of a value
    //    break;
    case ID_OPTIONS_DEBUGOPTIONS:
        Do_ID_OPTIONS_DEBUGOPTIONS(hWnd);
        break;
    case ID_OPTIONS_ENABLESTRATEGIES:
        Do_ID_OPTIONS_ENABLESTRATEGIES(hWnd);
        break;

    case ID_OPTIONS_MISCOPTIONS:
        Do_ID_OPTIONS_MISCOPTIONS(hWnd);
        break;

    case ID_OPTIONS_SELECTCOLORS:
        Do_ID_OPTIONS_SELECTCOLORS(hWnd);
        break;

    case IDC_EDIT1:
        if (HIWORD(wParam) == EN_CHANGE) {
            Do_Import_Help(0);
        }
        break;
	default:
		lRes = DefWindowProc(hWnd, message, wParam, lParam);
	}
    return lRes;
}

VOID Do_WM_LBUTTONDOWN( HWND hWnd, LPARAM lParam )
{
    // lParam 
    // The low-order word specifies the x-coordinate of the cursor. 
    // The coordinate is relative to the upper-left corner of the client area. 
    // The high-order word specifies the y-coordinate of the cursor. 
    // The coordinate is relative to the upper-left corner of the client area.
    curr_xPos = LOWORD(lParam); // GET_X_LPARAM(lParam);
    curr_yPos = HIWORD(lParam); // GET_Y_LPARAM(lParam);

   if( g_bAShiftDown ) {
      //TrackMouse( hWnd, LOWORD(lParam), HIWORD(lParam) );
   } else {
      // show distance and 
      //TrackMouse2( hWnd, LOWORD(lParam), HIWORD(lParam) );

   }
}


//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//  PURPOSE:  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
	switch (message)
	{
     case WM_INITMENUPOPUP:
        lRes = Do_WM_INITMENUPOPUP( hWnd, wParam, lParam );
        break;
	case WM_COMMAND:
        lRes = Do_WM_COMMAND(hWnd,message,wParam,lParam);
		break;
	case WM_PAINT:
        Do_WM_PAINT(hWnd);
		break;
	case WM_DESTROY:
        WriteINI();
        clean_up_paint_tools();
        Free_Boxes2();
        // Kill_Change();
		PostQuitMessage(0);
		break;
    case WM_CHAR:
        Do_WM_CHAR( hWnd, wParam );
        break;
    case WM_KEYDOWN:
       Do_WM_KEYDOWN( hWnd, wParam, lParam );
       Paint_Mouse_Pos(hWnd);
       break;
    case WM_KEYUP:
       Do_WM_KEYUP( hWnd, wParam, lParam );
       Paint_Mouse_Pos(hWnd);
       break;
    case WM_TIMER:
       Do_WM_TIMER( hWnd );
       break;
    case WM_MOUSEMOVE:
       Do_WM_MOUSEMOVE( hWnd, wParam, lParam );
       break;
    case WM_LBUTTONDOWN:
        g_bLBDown = TRUE;
        Do_WM_LBUTTONDOWN( hWnd, lParam );
        Paint_Mouse_Pos(hWnd);
       break;
    case WM_LBUTTONUP:
        g_bLBDown = FALSE;
        Paint_Mouse_Pos(hWnd);
       break;
   case WM_CONTEXTMENU:
       Do_WM_CONTEXTMENU( hWnd, wParam, lParam );
       break;
   case WM_SIZE:
        Do_WM_SIZE( hWnd, lParam );
        break;
   case WM_MOVE:
       UpdateWP( hWnd );
       break;
   case WM_NOTIFY:
       Do_WM_NOTIFY( hWnd, wParam, lParam );
       break;
   case WM_ERASEBKGND:
        if (g_bCopyHDC) { // if TRUE handle this message
            lRes = TRUE;
        }
        break;
	default:
		lRes = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return lRes;
}

////////////////////////////////////////////////////////////////////////////////////////////
#else // #ifdef WIN32
////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
    printf("Only run in Windows for now...\n");
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32
// eof - Sudoku.cxx
