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
#include "Sudo_Time.hxx"

static const char *module = "Sudoku";
#ifndef SPRTF
#define SPRTF printf
#endif

#ifndef SLEEP
#ifdef WIN32    // windows SLEEP macro = Sleep(ms)
#define SLEEP(x) Sleep(x)
#else           // !#ifdef WIN32 SLEEP macro = usleep(1000*ms)
#define SLEEP(x) usleep(1000 * x)
#endif          // #ifdef WIN32 y/n - SLEEP macro
#endif

#ifndef DEF_FILE
#ifdef WIN32    // windows default debug file - TODO: Add to CMakeLists.txt
#define DEF_DIR     "C:\\GTools\\tools\\Sudoku\\examples\\"
#else // !WIN32 // unix default debug file
#define DEF_DIR     "/home/geoff/projects/Sudoku/examples/"
#endif // WIN32 y/n - def dbg file
//#define DEF_FILE    DEF_DIR "y-wing.txt"
#define DEF_FILE    DEF_DIR "diabolical.txt"
#endif

// Global Variables:
BOOL        g_bChanged = FALSE;
HWND        g_hWnd = 0;
HINSTANCE   hInst = 0;			// current instance
int         verbosity = 0;

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
// general services - TODO: Move to UTILS (in library)
////////////////////////////////////////////////////////////////////////////////////////////
int Get_Spots(PABOX2 pb)
{
    if (!pb)
        return 0;
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
    PABOX2 pb = get_curr_box();
    return Get_Spots(pb);
}

// get global total empty count - 0 == solved
// use the above Get_Spot_Count()
int get_empty_count()
{
    int len = Get_Spot_Count();
    total_empty_count = 81 - len;    // set global total empty count - 0 == solved
    return total_empty_count;
}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN and END app services
// ==========================
static Timer InApptmr;
void add_app_begin()
{
    InApptmr.start();
    int curr = add_sys_time(1);
    sprtf("Begin Application\n");
    add_sys_time(curr);
#ifdef BUILD_WIN32_EXE // WIN32 GUI EXE
    if (res_scn_rect) {
        sprtf("Screen Workarea [%s]\n", Get_Rect_Stg(&g_sRect));
    } else {
        sprtf("Get workarea FAILED!\n");
    }
#endif // #ifdef BUILD_WIN32_EXE // WIN32 GUI EXE
}

void add_app_end()
{
    g_dwSvOptBits = sff_TEMP_FILE;
    int count = get_empty_count();
    if (!count) {
        g_dwSvOptBits &= ~(sff_ADD_ASCII_BLOCK | sff_ADD_CANDIDATES);   // if SOLVED, remove the fancy output
    }
    Write_Temp_File();
    int curr = add_sys_time(1); // ensure time added to message output
    InApptmr.stop();
    char *log = get_log_file();
    sprtf("End Application. Ran for %s. log %s\n", InApptmr.getTimeStg(), log);
    add_sys_time(curr);         // retore adding time to message output
}

////////////////////////////////////////////////////////////////////////////////////////////

// Do_Solved_MsgBox(tb,"SUDOKU SOLVED");
VOID Do_Solved_MsgBox(const char *msg, const char *title)
{
    //int res = MB3(tb,"SUDOKU SOLVED");
    int res = MB3(msg,title);
    if (res == IDYES) {
        Post_Command(ID_FILE_OPEN);
    } else if (res == IDCANCEL) {
        Post_Command(IDM_EXIT);
    }
}

// Get_MsgBox_YNC(tb,"Y-Wing Strategy Problem");
int Get_MsgBox_YNC(const char *msg,const char *title, bool abort)
{
    int res = MB3(msg, title);
    if (res == IDCANCEL) {
        Post_Command(IDM_EXIT);
        return 1;
    } else if (!abort && (res == IDYES)) {
        return 0;
    }
    return 1;
}

int Do_MsgBox_YNC2(const char *msg,const char *title)
{
    int res = MB3(msg, title);
    return res;
}


int Do_MsgBox_YN(const char *msg, const char *title)
{
    int res = MB2(msg,title);
    if (res != IDYES) {
        return 1;
    }
    return 0;
}

VOID Do_MsgBox_OK(const char *msg)
{
    MB(msg);
}



/////////////////////////////////////////////////////////////////////////
// 20120917 - Orig '#' started comment line, but add
// '//' and ';' as comment lines
#define MMX_OPTS    64
#ifdef BUILD_WIN32_EXE // WIN32 GUI EXE
int ParseArgs(int, char**);
#else
int parse_args(int, char**);
#endif 

int Load_Input_File(char* file, struct stat* psb, int* pcnt = 0);


int Load_Input_File(char* file, struct stat* psb, int *pcnt)
{
    if (pcnt) {
        *pcnt = 0;
    }
    if (psb->st_size == 0)
        return 0; // quietly IGNORE a zero length input file
    FILE* fp = fopen(file, "rb");
    if (!fp)
        return 1;
    int size = psb->st_size + 2;
    char* pb = (char*)malloc(size + (sizeof(char*) * MMX_OPTS));
    CHKMEM(pb);
    int res = fread(pb, 1, psb->st_size, fp);
    if (res != psb->st_size) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    int i, c;
    char* bgn = pb;
    int off = 0;
    int inquot = 0;
    char* ptmp = pb;
    char** argv = (char**)&pb[size];
    int argc = 0;
    int cnt = 0;
    argv[argc++] = "dummy";
    for (i = 0; i < res; i++) {
        c = pb[i];
        ptmp = &pb[i];
        // remove leading...
        if (off == 0) {
            if (c == '\n') continue;
            if (c == '\r') continue;
            if ((c <= ' ') && (off == 0)) continue;
            if ((c == '#') || (c == ';')) {
                i++;
                for (; i < res; i++) {
                    c = pb[i];
                    if (c == '\n') break;
                    if (c == '\r') break;
                }
                continue;
            }
            else if ((c == '/') && ((i + 1) < res) && (pb[i + 1] == '/')) {
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
        if ((c < ' ') || (c == '#')) {
            // back off any white space
 add_opt:
            if (off) {
                while (off--) {
                    if (bgn[off] > ' ') {
                        off++;
                        break;
                    }
                    bgn[off] = 0;
                }
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
            cnt++;
        }
        else if (c == ' ') {
            if (inquot) {
                bgn[off++] = (char)c;
                bgn[off] = 0;
            }
            else
                goto add_opt;
        }
        else if (c == '"') {
            if (inquot)
                inquot = 0;
            else
                inquot = 1;
        }
        else {
            bgn[off++] = (char)c;
            bgn[off] = 0;
        }
    }

    // catch any last entry in file, which does NOT have a CR or LF trailing!!!
    if (off) {
        while (off--) {
            if (bgn[off] > ' ')
                break;
            bgn[off] = 0;
        }
        if (off) {
            argv[argc++] = bgn;
            cnt++;
        }
    }

    off = 0;
    if (argc > 1) {
        // quietly ignore no arguments
#ifdef BUILD_WIN32_EXE // WIN32 GUI EXE
        off = ParseArgs(argc, argv);
#else
        off = parse_args(argc, argv);
#endif 
    }
    free(pb);
    if (pcnt) {
        *pcnt = cnt;
    }
    return off;
}


////////////////////////////////////////////////////////////////////////////////////////////
#ifdef BUILD_WIN32_EXE // WIN32 GUI EXE
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


int ParseArgs(int argc, char **argv);

/////////////////////////////////////////////////////////////////////////
///////////// Attach Console ///////////////////////////
// add shorter, clearer message of 'type'
typedef struct tagDW2STG {
    DWORD val;
    char *type;
    char *desc;
}DW2STG, *PDW2STG;

static DW2STG ft2stg[] = {
    { FILE_TYPE_CHAR, "FILE_TYPE_CHAR", "is a character file, typically an LPT device or a console." },
    { FILE_TYPE_DISK, "FILE_TYPE_DISK", "is a disk file." },
    { FILE_TYPE_PIPE, "FILE_TYPE_PIPE", "is a socket, a named pipe, or an anonymous pipe." },
    { FILE_TYPE_REMOTE, "FILE_TYPE_REMOTE", "Unused." },
    { FILE_TYPE_UNKNOWN, "FILE_TYPE_UNKNOWN", "Either the type of the specified file is unknown, or the function failed." },

    // LAST
    { 0, 0 }
};

static char *GetFileTypeStg(DWORD dw)
{
    PDW2STG pdw = &ft2stg[0];
    while (pdw->desc) {
        if (pdw->val == dw)
            return pdw->type;   // pdw->desc;
        pdw++;
    }
    return "Unexpected type";
}

// See Reference
static void sendEnterKey(void)
{
    INPUT ip;
    // Set up a generic keyboard event. 
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key 
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    // Send the "Enter" key 
    ip.ki.wVk = 0x0D; // virtual-key code for the "Enter" key 
    ip.ki.dwFlags = 0; // 0 for key press 
    SendInput(1, &ip, sizeof(INPUT));
    // Release the "Enter" key 
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release 
    SendInput(1, &ip, sizeof(INPUT));
}

int attach_console()
{
    int iret = 0;
    DWORD dwMode = 0;
    BOOL bRedON = FALSE;
    // get 'stdout' info
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);  // get stdout handle
    DWORD type = GetFileType(hout);
    char *desc = GetFileTypeStg(type);
    if (hout && !GetConsoleMode(hout, &dwMode))
        bRedON = TRUE;

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        int dnfro1 = 0;
        // selectively use 'freopen', if not already REDIRECTED!
        if ((type != FILE_TYPE_DISK) && !bRedON) {
            freopen("conout$", "w", stdout);
            dnfro1 = 1;
        }
        SPRTF("\nAttachConsole succeeded... stdout: type %s, %s, bRedON=%s\n", desc,
            (dnfro1 ? "done fropen" : "is disk file"),
            (bRedON ? "TRUE" : "FALSE"));
    }
    else {
        DWORD err = GetLastError();
        const char *pmsg = (err == ERROR_ACCESS_DENIED) ? "ERROR_ACCESS_DENIED" :
            (err == ERROR_INVALID_HANDLE) ? "ERROR_INVALID_HANDLE" :
            (err == ERROR_GEN_FAILURE) ? "ERROR_GEN_FAILURE" :
            "UNEXPECTED_ERROR";
        SPRTF("\nAttachConsole FAILED... %s (%d)\n", pmsg, err);
        iret = 1;
    }
    return iret;
}


int ParseArgs(int argc, char **argv) 
{
    int i, res;
    char *tb = GetNxtBuf();
    for (i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (*arg == '-') {
            sprintf(tb,"ERROR: Unknow command\n%s\nAt present there are NO options\nWill abort on OK",arg);
            Do_MsgBox_OK(tb);
            return 1;
        } else if (*arg == '@') {
            res = 1;
            if (is_file_or_directory(&arg[1]) == 1) {
                res = Load_Input_File(&arg[1], Get_Stat_Buf());
            }
            if (res) {
                sprintf(tb,"ERROR: Unable to load input file\n%s\nCheck file name and location\nWill abort on OK",&arg[1]);
                Do_MsgBox_OK(tb);
                return 1;
            }
        } else {
            // bare argument - assume a file to load
            if (is_file_or_directory(arg) == 1)
                Reset_Active_File(arg);
            else {
                sprintf(tb,"ERROR: Unable to 'stat' file\n%s\nCheck file name and location\nWill abort on OK",arg);
                Do_MsgBox_OK(tb);
                return 1;
            }
        }
    }
    return 0;
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
    int cons = attach_console();

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
      Do_MsgBox_OK( "Failed in get Timer! Aborting ..." );
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

    //clean_up_paint_tools();
    //Free_Boxes2();
    //Kill_Change();

    add_app_end();
    WriteINI();
    if (!cons)
        sendEnterKey();
    close_log_file();
	return (int) msg.wParam;
}

void Post_Command( WPARAM cmd )
{
    PostMessage(g_hWnd, WM_COMMAND, cmd, 0);
}

bool Check_Clear() 
{
    int res;
    int spots = Get_Spot_Count();
    if (g_bChanged && spots) { // total_empty_count
        char *tb = GetNxtBuf();
        sprintf(tb,"New will clears all %d current cells.", spots);
        strcat(tb,EOL_CHR"But current Sudoku box has been changed." EOL_CHR "Do you want to SAVE these results?" EOL_CHR "Click [Yes] to save this change." EOL_CHR "Click [ NO ] to clear all spots.");
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

void DoOneStepCommand( HWND hWnd )
{
    PABOX2 pb = get_curr_box();
    int last_stage = pb->iStage;
    int count = Do_ID_OPTIONS_ONESTEP(hWnd);
    if (last_stage == pb->iStage) {
        Do_MsgBox_OK("Warning: One Step FAILED! Logic impaired!!\n"
            "\n"
            "Either unable to find solutions, or\n"
            "perhaps some previous value was wrong.\n"
            "Can only manually change the values,\n"
            "or load a new puzzle.\n"
            "\n"
            "The adventurous could fix the problem\n"
            "and post a PR on\n"
            "https://github.com/geoffmcl/Sudoku/pulls\n");
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
            DoOneStepCommand(hWnd); // Do_ID_OPTIONS_ONESTEP(hWnd);
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
        // WriteINI(); -// too early in close process
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
#else // !#ifdef WIN32 - standard console app using main()
////////////////////////////////////////////////////////////////////////////////////////////

void Post_Command( WPARAM cmd )
{
    // TODO: in alternative GUI code, maybe sprtf("TODO: Post_Command( %d ) to be done...\n", (int)cmd );
}

static const char *usr_input = 0;

// TODO: Not sure I like this #ifdef SUDO_RC_VALUE... but how else to designate a 'Release Candidate'???
void show_version()
{
    printf("\n");
#ifdef SUDO_RC_VALUE
    printf("%s: version %s.%s of %s\n", module, SUDO_VERSION, SUDO_RC_VALUE, SUDO_DATE );
#else // !SUDO_RC_VALUE
    printf("%s: version %s of %s\n", module, SUDO_VERSION, SUDO_DATE );
#endif // SUDO_RC_VALUE y/n
    printf(" Copyright (C) Geoff R. McLane, 2012-2020\n");
    printf(" Compiled on %s at %s\n", __DATE__, __TIME__);
    printf(" Compiled by %s\n", COMPILER_STR);
    printf(" Licence GNU GPL version 2 (or later)\n");
    printf("\n");
}

void give_help( char *name )
{
    printf("\n");
    printf("%s: usage: [options] usr_input\n", module);
    printf("Options:\n");
    printf(" --help   (-h or -?) = This help and exit(2)\n");
    // char szASD[] = "Auto_Solve_Delay_Seconds_as_float";
    printf(" --verb[n]      (-v) = Bump or set verbosity. (def=%d)\n", verbosity);
    printf(" --Version      (-V) = Show the version and exit(2)\n");
    printf(" --delay float  (-d) = Set Auto_Solve_Delay_Seconds_as_float. (def=%lf)\n", g_AutoDelay);
    printf(" --input <file> (-i) = Input command file of space sep. options. # begins comment.\n");
    printf(" --D|S|O 'opt=val'  = Set/Unset Debug|Strategies|Option. The 'opt' is per the INI file. Can be 'all' for D|S.\n");
    printf("  The val for /ON/ can be '1','y','t','yes','true','on', and /OFF/ 1 of '0','n','f','no','false','off', case ignored.\n");
    printf("\n");
    printf(" Given a Sudoku puzzle file input, read and auto solve the Sudoku.\n");
    printf(" The 'solution' will be compared to the 'brute force' solution, and\n");
    printf(" the auto-solve will abort if it finds no solution.\n");
    printf("\n");
    printf(" The 'input' file can be one of several ASCII file formats...\n");
    printf(" '*.txt' - Assumed to be a line of 81 values, 0 = no value, [1-9] the slot value.\n");
    printf(" '*.csv' - Nine rows of nine comma separated values. No value (,,) = no value, else [1-9] slot value.\n");
    printf(" '*.sdk' - Similar to csv without the separator. 9 rows of 9 values. '.' = no value, else [1-9] slot value.\n");
    printf(" '*.xml' - Root 'NewDataSet', with 81 <Table1><spot> values. 0 = no value, else [1-9] slot value.\n");
    printf(" %s tries to 'guess' the type of input by the extension and analysis of ASCII contents.\n", module );
    printf(" See the 'examples' folder for samples. Some support comment lines...\n");
    printf("\n");
    printf(" %s reads and writes an INI file, with lots of parameters. (def=%s)\n", module, GetINIFile() ); // Get_INI_File(0));
    printf(" That INI file contains multiple user parameters, like [Strategies] enabled, some [Debug], etc...\n");
    printf(" Manually adjust its contents to what you desire before running %s.\n", module );
    printf("\n");
    printf(" A big thank you to http://www.sudokuwiki.org/sudoku.htm for a great javascript (?) inspiration!\n");
    printf(" This Sudoku 'wiki' has LOTS of very good 'tutorial' information, strategies, with images, examples...\n");
    printf("\n");
    printf(" This C/C++ code effort is not complete, nor perfect ;=)) Would like some help achieving that.\n");
    printf(" Clone this repo, and present a Pull Request, or a diff/patch file, and it will be considered.\n");
    show_version();
}

typedef std::vector<std::string> vSTR;
vSTR split_string(const string& str, const char* sep, int maxsplit = 0);


vSTR split_string(const string& str, const char* sep, int maxsplit)
{
    //if (sep == 0)
    //    return split_whitespace(str, maxsplit);

    vSTR result;
    size_t n = ::strlen(sep);
    if (n == 0)
    {
        // Error: empty separator string
        return result;
    }
    const char* s = str.c_str();
    string::size_type len = str.length();
    string::size_type i = 0;
    string::size_type j = 0;
    int splitcount = 0;

    while (i + n <= len)
    {
        if (s[i] == sep[0] && (n == 1 || ::memcmp(s + i, sep, n) == 0))
        {
            result.push_back(str.substr(j, i - j));
            i = j = i + n;
            ++splitcount;
            if (maxsplit && (splitcount >= maxsplit))
                break;
        }
        else
        {
            ++i;
        }
    }

    result.push_back(str.substr(j, len - j));
    return result;
}

const char* choices_no[] =
{ "0", "n", "f", "no",  "false", "off", 0 };

const char* choices_yes[] =
{ "1", "y", "t", "yes", "true",  "on", 0 };

int is_a_no(const char* cp)
{
    const char* *list = &choices_no[0];
    while (*list) {
        if (STRICMP(cp,*list) == 0)
            return 1;
        list++;
    }
    return 0;
}
int is_a_yes(const char* cp)
{
    const char** list = &choices_yes[0];
    while (*list) {
        if (STRICMP(cp, *list) == 0)
            return 2;
        list++;
    }
    return 0;
}

int set_degbug_strats(int c, char* arg)
{
    std::string s = arg;
    int onoff = 0;
    size_t pos = s.find('=');
    const char* pSect = "Unknown";
    const char* pAct = "Unknown";

    if (c == 'D')
        pSect = "Debug";
    if (c == 'S')
        pSect = "Strategies";

    if (pos != std::string::npos) {
        vSTR vs = split_string(s, "=");
        if (vs.size() == 2) {
            std::string opt = vs[0];
            std::string of = vs[1];
            int onoff = is_a_yes(of.c_str());
            if (!onoff)
                onoff = is_a_no(of.c_str());
            if (onoff) {
                // set the action ON or OFF
                if (onoff == 2)
                    pAct = "YES";
                if (onoff == 1)
                    pAct = "NO";

                // we have something worth looking at
                if (opt == "all") {
                    if (c == 'D') {
                        if (onoff == 2)
                            Set_ALL_Dbg_ON(true);
                        else
                            Set_ALL_Dbg_OFF(true);
                    }
                    else {
                        if (onoff == 2)
                            Set_ALL_Strat_ON(true);
                        else
                            Set_ALL_Strat_OFF(true);
                    }

                    SPRTF("%s: Sect [%s] item '%s' set %s (%d)\n", module, pSect, opt.c_str(), pAct, onoff);
                    return onoff;
                }
                else {
                    if (Set_One_DS_opt(opt.c_str(), c, &onoff)) {
                        SPRTF("%s: Sect [%s] item '%s' set %s (%d)\n", module, pSect, opt.c_str(), pAct, onoff);
                        return 1;
                    }
                }
            }
        }
    }
    SPRTF("%s: Arg '%s' no split on '='! Or some other prob...\n", module, arg);
    return 0;
}

int set_an_option(char* arg)
{
    std::string s = arg;
    size_t pos = s.find('=');
    if (pos != std::string::npos) {
        vSTR vs = split_string(s, "=");
        if (vs.size() == 2) {
            std::string opt = vs[0];
            std::string of = vs[1];
            PINILST plst = Find_an_Options(opt.c_str());
            if (plst) {
                int i;
                char* lpb = (char*)of.c_str();
                unsigned int dwt = plst->i_Type;
                char *pSect = plst->i_Sect;   // pointer to [section] name
                char *pItem = plst->i_Item;   // pointer to item in section
                char *pDef = plst->i_Deft;   // pointer to destination
                int *pb = plst->i_Chg;    // pointer to CHANGE flag
                int* pi;
                int* pbd;
                vSTG* vsp = 0;
                switch (dwt) {
                case it_String:
                    if (strcmp(pDef, lpb)) {
                        strcpy(pDef, lpb);
                        *pb = TRUE;
                        SPRTF("%s: Sect [Options], set item '%s' to '%s' - set change\n", module, opt.c_str(), lpb);
                    }
                    else {
                        SPRTF("%s: Sect [Options], item '%s' already '%s'\n", module, opt.c_str(), lpb);
                    }
                    return 1;
                    break;
                case it_Int:   // also doubles as a DWORD in WIN32
                    pi = (int*)pDef;
                    i = (int)atoi(lpb);
                    if (*pi != i) {
                        *pi = i;
                        *pb = TRUE;
                        SPRTF("%s: Sect [Options], set item '%s' to '%d' - set change\n", module, opt.c_str(), i);
                    }
                    else {
                        SPRTF("%s: Sect [Options], item '%s' already '%d' - no change\n", module, opt.c_str(), i);
                    }
                    return 1;
                    break;
                case it_Bool:
                    pbd = (int*)pDef;
                    if (is_a_yes(lpb)) {
                        if (*pbd != TRUE) {
                            *pbd = TRUE;
                            *pb = TRUE;
                            SPRTF("%s: Sect [Options], set item '%s' to TRUE - set change\n", module, opt.c_str());
                        }
                        else {
                            SPRTF("%s: Sect [Options], item '%s' already TRUE - no change\n", module, opt.c_str());
                        }
                        return 1;

                    } else if (is_a_no(lpb)) {
                        if (*pbd != FALSE) {
                            *pbd = FALSE;
                            *pb = TRUE;
                            SPRTF("%s: Sect [Options], set item '%s' to FALSE - set change\n", module, opt.c_str());
                        }
                        else {
                            SPRTF("%s: Sect [Options], item '%s' already FALSE - no change\n", module, opt.c_str());
                        }
                        return 1;
                    }
                    break;
                case it_WinSize: // // special WINDOWPLACEMENT
                    break;
                case it_Rect:
                    break;
                case it_Color:
                    break;
                case it_Double:
                    break;
                case it_Files: // 9   // vector<string> if files
                    //vsp = (vSTG*)pDef;
                    //vsp->push_back(of);
                    //return 1;
                    break;
                default:
                    break;
                }
            }

        }
    }

    SPRTF("%s: Arg '%s' no split on '='! or some other prob...\n", module, arg);
    return 0;
}

static int in_input = 0;

int Con_Input_File(char *sarg, int *pir, int *pcnt)
{
    int iret = 0;
    int res = -1;
    if (!sarg || (!*sarg) || !pir || !pcnt) {
        return 0;
    }
    if (is_file_or_directory(sarg) == 1) {
        in_input++;
        res = Load_Input_File(sarg, Get_Stat_Buf(), pcnt);
        in_input--;
        if (res) {
            *pir = res;
        }
        iret = 1;
    }

    return iret;
}


int parse_args( int argc, char **argv )
{
    int iret = 0;
    int i,i2,c;
    char *arg, *sarg;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        i2 = i + 1;
        if (*arg == '-') {
            sarg = &arg[1];
            while (*sarg == '-')
                sarg++;
            c = *sarg;
            if (strcmp(arg, "--version") == 0) {
                show_version();
                return 2;
            }
            switch (c) {
            case 'h':
            case '?':
                give_help(argv[0]);
                return 2;
                break;
            case 'i':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    if (Con_Input_File(sarg, &iret, &i2)) {
                        if (iret) {
                            SPRTF("%s: Loaded Input File '%s %s', %d args, got exit(%d).\n", module, arg, sarg, i2, iret);
                            return iret;
                        }
                        else {
                            SPRTF("%s: Loaded Input File '%s %s', %d args, ok...\n", module, arg, sarg, i2);
                        }
                    }
                    else {
                        SPRTF("%s: Input file load '%s %s', %d args, FAILED!\n", module, arg, sarg, i2);
                        return 1;
                    }
                }
                else {
                    SPRTF("%s: Expected a float value to follow '%s'!\n", module, arg);
                    return 1;
                }
                break;

            case 'd':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    g_AutoDelay = atof(sarg);
                } else {
                    SPRTF("%s: Expected a float value to follow '%s'!\n", module, arg);
                    return 1;
                }
                break;
            case 'v':
                verbosity++;    // bump
                sarg++;         // and get to next
                while (*sarg) { // what have we got
                    if (*sarg == 'v') {
                        verbosity++;    // allow lazy -vvvvvvvvvvvvvvvvvvv to work
                    } else if (ISDIGIT(*sarg)) {
                        verbosity = atoi(sarg); // if a digit, like -v5, set 5
                        break;
                    }
                    sarg++;
                }
                break;
            case 'V':
                show_version();
                return 2;
            case 'D':
            case 'S':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    if (!set_degbug_strats(c, sarg)) {
                        SPRTF("%s: Failed to set the strategy '%s', like 'Naked Pairs=off'!\n", module, sarg);
                        return 1;
                    }
                }
                else {
                    SPRTF("%s: Expected a strategy, like 'Naked Pairs=off' to follow '%s'!\n", module, arg);
                    return 1;
                }
                break;
            case 'O':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    if (!set_an_option(sarg)) {
                        SPRTF("%s: Failed to set the option '%s, like 'ShowHints=off' !\n", module, sarg);

                    }
                }
                else {
                    SPRTF("%s: Expected an option, like 'ShowHints=off' to follow '%s'!\n", module, arg);
                    return 1;
                }
                break;

            // TODO: Other arguments
            default:
                SPRTF("%s: Unknown argument '%s'. Try -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            if (usr_input) {
                SPRTF("%s: Already have input '%s'! What is this '%s'?\n", module, usr_input, arg );
                return 1;
            }
            if (is_file_or_directory(arg) == 1)
                Reset_Active_File(arg);
            else {
                SPRTF("ERROR: Unable to 'stat' file\n'%s'\nCheck file name and location... aborting...",arg);
                return 1;
            }
            usr_input = strdup(arg);
        }
    }

    if (in_input)
        return 0;   // no probs so far
#ifndef NDEBUG
    if (!usr_input) {
        const char *def_file = DEF_FILE;
        if (is_file_or_directory((char *)def_file) == 1) {
            usr_input = strdup(def_file);
        }
    }
#endif
    if (!usr_input) {
        sprtf("%s: No user input found in command!\n", module);
        if (g_bAutoLoad) { // AutoLoad
            char* cp = Get_First_INI_File();
            if (cp) {
                if (is_file_or_directory((char*)cp) == 1) {
                    usr_input = strdup(cp);
                    sprtf("%s: g_bAutoLoad: using first INI file '%s'.\n", module, usr_input);
                }
                else {
                    return 1;
                }

            }
            else {
                return 1;
            }
        }
        else {

            return 1;
        }
    }
    if (VERB9) {
        Set_ALL_Dbg_ON();
    }
    return iret;
}

DWORD msSleep = 55; // a very short time
static int iSolveStage = -1;
typedef std::vector<int> vINT;

#define SHOW_STAGE_LIST

#ifdef SHOW_STAGE_LIST
static vINT vStages;

// order of stageg run, using one-step
void Show_Stage_List()
{
    if (vStages.size() > 0) {
        size_t ii, max = vStages.size();
        int last = add_sys_time(0);
        int i;
        const char *st;
        int restarts = 0;
        SPRTF("Solver: %d stages: ", (int)max);
        if (VERB2) {
            for (ii = 0; ii < max; ii++) {
                i = vStages[ii];
                st = stage_to_text(i);
                if (i == 0) {
                    SPRTF("\n");
                    restarts++;
                }
                SPRTF("%s (%d) ", st, i);
            }
            SPRTF("w/ %d restarts...\n", restarts);
        }
        else {
            SPRTF("Use -v2 to show each...\n");
        }
        last = add_sys_time(last);
    }
}

#endif

static Timer *pAutoTime = 0;
static double last_seconds = -1.0;
static Timer *pSleep = 0;
static double secs_in_sleep = 0.0;
static int steps_taken = 0;

void do_message_block( double elap, const char *why )
{
    static char msg_blocks[4][1024];
    char *tb  = msg_blocks[0];
    char *tb1 = msg_blocks[1];
    char *tb2 = msg_blocks[2];
    char *tb3 = msg_blocks[3];
    *tb = 0;
    *tb1 = 0;
    *tb2 = 0;
    *tb3 = 0;
    pAutoTime->setTimeStg(tb1,elap);
    pSleep->setTimeStg(tb2,secs_in_sleep);
    pSleep->setTimeStg(tb3,g_Secs_in_SPRTF);
    if (VERB5)
        strcpy(tb,"\n");
    sprintf(EndBuf(tb),"%s in %d steps in %s", why, steps_taken, tb1);
    if (secs_in_sleep > 0.0) {
        sprintf(EndBuf(tb),", slept %s (-d %s)", tb2, double_to_stg(g_AutoDelay));
    }
    sprintf(EndBuf(tb),", form/IO %s", tb3);
    SPRTF("%s\n",tb);
    if (total_empty_count == 0) {
        if (!done_end_dialog) {
            done_end_dialog = true;
            Do_Solved_Message();
        }
    }
}

#define IS129(a) ((a >= '1') && (a <= '9'))

// show solution - after solved, or not...
void show_solution()
{
    // show solution (or restart) value
    size_t ii, len;
    char* fb = get_ASCII_81_Stg(get_file_box());
    len = strlen(fb);
    int in_slot_cnt = 0;
    int added_slots = 0;
    int missed_slots = 0;
    for (ii = 0; ii < len; ii++) {
        if (fb[ii] != '0')
            in_slot_cnt++;
    }
    SPRTF("%s - start  %2d slots.\n", fb, in_slot_cnt);  // this is the initial BOX from the file


    char* cp = get_ASCII_81_Stg(get_curr_box());
    char* cp3 = GetNxtBuf();
    strcpy(cp3, cp); // copy, before modification
    len = strlen(cp3);
    for (ii = 0; ii < len; ii++) {
        if ((cp3[ii] != '0') && (cp3[ii] == fb[ii])) {
            cp3[ii] = '=';
        }
        else if (cp3[ii] == '0') {
            cp3[ii] = ' ';
        }
        if (IS129(cp3[ii]))
            added_slots++;

    }
    SPRTF("%s - added  %2d slots.\n", cp3, added_slots);
    PABOX2 pb_con = get_pb_con();
    if (pb_con) {
        char* cp2 = get_ASCII_81_Stg(pb_con); // get the SECRET brute force solution
        size_t len2 = strlen(cp2);
        if (len == len2) {
            for (ii = 0; ii < len2; ii++) {
                if (cp[ii] != '0') {
                    if (cp[ii] == cp2[ii]) {
                        cp2[ii] = '=';
                    }
                }
                if (IS129(cp2[ii])) {
                    missed_slots++;
                }
            }
        }
        if (VERB9)
            SPRTF("%s - missed %2d slots.\n", cp2, missed_slots);
        else
            SPRTF("****************** elevate to -v9, to see brute force solution ****************** - missed %2d slots.\n", missed_slots);
    }
} // show solution - after solved, or not...


// now it is loaded, and analyses, or in the process of doing that,
// we could ID_OPTIONS_ONESTEP, or an outright ID_OPTIONS_SOLVE
// TODO: A user option to choose this, but heer choose one step, since I think this gives more informat
// if (total_empty_count == 0) { Do_Solved_Message(); } else {
//    Do_ID_OPTIONS_ONESTEP(hWnd); }
// OR case ID_OPTIONS_SOLVE: Start_Auto_Solve();
int solve_the_Sudoku()
{
    int iret = 0;
    bool ok = true;
    HWND hWnd = 0;

    if (!pAutoTime)
        pAutoTime = new Timer;
    if (!pSleep)
        pSleep = new Timer;
    pSleep->start();
    pSleep->stop();

    if (VERB5) {
        SPRTF("\n%s: Commencing step by step solution...\n", module);
    }
    pAutoTime->start();
    last_seconds = pAutoTime->getElapsedTime();
    g_bAutoSolve = true;    // repeat steps
    get_empty_count();

    while(ok)
    {
        PABOX2 pb = get_curr_box();
        if (pb->iStage != iSolveStage) {
            iSolveStage = pb->iStage;
            steps_taken++;
#ifdef SHOW_STAGE_LIST
            vStages.push_back(iSolveStage);
            //const char *st = stage_to_text(iSolveStage);
            //SPRTF("Stage %s (%d)\n", st, (int)iSolveStage );
#endif
            // take a STEP
            //////////////////////////////////////////////////////////
            // int savestdout = add_std_out(0);    // set none, well maybe log file...
            // if (VERB5) add_std_out(1);          // -v5 show it all
            Do_ID_OPTIONS_ONESTEP( hWnd );      // take a step
            // add_std_out(savestdout);            // resore stdout io setting
            //////////////////////////////////////////////////////////
            get_empty_count();
            // if no change in 'stage' then failed or finished
            if ( (total_empty_count != 0) && (pb->iStage == iSolveStage) ) {
                g_bAutoSolve = false;
                pAutoTime->stop();
                do_message_block( pAutoTime->getElapsedTime(), "Aborted" );
                ok = false;
            }
        }
        // if stage changed, then maybe solved (or stuck!)
        if ( ok && (total_empty_count == 0) ) {
            pAutoTime->stop();
            double elap = pAutoTime->getElapsedTimeInSec();
            g_bAutoSolve = FALSE;
            g_bAutoComplete = false;
            do_message_block( elap, "Solved" );
            ok = false;
        }

        ok = g_bAutoSolve;
        if (ok) {
            bool wait = (iSolveStage == sg_Begin) ? false : true;
            // could add a user delay, say if user wanted to read the console ouput quietly...
            while (wait) {
                double new_secs = pAutoTime->getElapsedTime();
                double new_time = last_seconds + g_AutoDelay;
                if (new_secs >= new_time) {
                    last_seconds = new_secs;
                    wait = false;
                } else {
                    pSleep->start();
                    SLEEP(msSleep); // waste time
                    pSleep->stop();
                    secs_in_sleep += pSleep->getElapsedTimeInSec();
                }
            }
        }
    }

    show_solution(); // show solution - after solved, or not...


#ifdef SHOW_STAGE_LIST
    Show_Stage_List();
#endif

    return iret;
}


int main( int argc, char **argv )
{
    int iret = 0;
    add_std_out(1);
    set_log_file("tempcon.txt");
    g_AutoDelay = 0.0001;  // was 0.02;
    ReadINI();
    iret = parse_args( argc, argv );
    if (iret) {
        if (iret == 2)
            iret = 0;
        return iret;
    }
    add_app_begin();
    if (VERB2) {
        SPRTF("%s: Will input file '%s', and try to solve the sudoku, using enabled strategies.\n", module, usr_input);
        if (VERB9) {
            SPRTF("%s: Begin a thread to analyse the puzzle by BRUTE FORCE. Try each possible\n"
                "value for each blank cell, and report if the solution is UNIQUE!\n", module);
        }
    }
    add_sys_time(1);
    if (Load_a_file( 0, (LPTSTR)usr_input )) {
        SLEEP(msSleep);
        iret = solve_the_Sudoku(); // action of the app
    } else {
        sprtf("Error: Failed to load '%s'\n", usr_input);
        iret = 1;
    }
    add_app_end();
    WriteINI();
    return iret;
}

////////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n - GUI versus console app

// eof - Sudoku.cxx
