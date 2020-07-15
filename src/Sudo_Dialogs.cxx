// Sudo_Dialogs.cxx

#include "Sudoku.hxx"

// ===========================================================
// stong (AND weak) LINKS dialog
// *****************************
//char szASV[] = "All_Candidates";
BOOL g_bAllSL = TRUE;
//BOOL gChgASV = FALSE;

//char szCLST[] = "Candidate_List";
//char g_szCandList[_MAX_PATH] = { "123456789" };
//BOOL gChgCLST = FALSE;
int g_iSlValues[9] = { 1,2,3,4,5,6,7,8,9 };

//char szIWL[] = "Include_Weak_Links";
//BOOL g_bIWL = FALSE;
//BOOL gChgIWL = FALSE;

//char szIBL[] = "Include_Box_Links";
//BOOL g_bIBL = FALSE;F
//BOOL gChgIBL = FALSE;

bool Show_SL_Value( int setval )
{
    if (!VALUEVALID(setval))
        return false;
    if (g_bAllSL)
        return true;
    if (g_iSlValues[setval - 1])
        return true;
    return false;
}

#ifdef WIN32    // window commctrl dialogs
//////////////////////////////////////////////////////

#ifndef _MSC_VERS
#pragma warning ( disable : 4996 )
#endif

#include "Commctrl.h"
#include <time.h>

extern BOOL bChgDSL2;

#ifndef EOL_CHR
#define EOL_CHR "\r\n"
#endif

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

/* ================================
    List of control classes
ICC_ANIMATE_CLASS Load animate control class. 
ICC_BAR_CLASSES   Load toolbar, status bar, trackbar, and tooltip control classes. 
ICC_COOL_CLASSES  Load rebar control class. 
ICC_DATE_CLASSES  Load date and time picker control class. 
ICC_HOTKEY_CLASS  Load hot key control class. 
ICC_INTERNET_CLASSES Load IP address class. 
ICC_LINK_CLASS       Load a hyperlink control class. 
ICC_LISTVIEW_CLASSES Load list-view and header control classes. 
ICC_NATIVEFNTCTL_CLASS Load a native font control class. 
ICC_PAGESCROLLER_CLASS Load pager control class. 
ICC_PROGRESS_CLASS     Load progress bar control class. 
ICC_STANDARD_CLASSES   Load one of the intrinsic User32 control classes. The user controls include 
button, edit, static, listbox, combobox, and scroll bar. 
ICC_TAB_CLASSES        Load tab and tooltip control classes. 
ICC_TREEVIEW_CLASSES   Load tree-view and tooltip control classes. 
ICC_UPDOWN_CLASS       Load up-down control class. 
ICC_USEREX_CLASSES     Load ComboBoxEx class. 
ICC_WIN95_CLASSES      Load animate control, header, hot key, list-view, progress bar, status bar, 
tab, tooltip, toolbar, trackbar, tree-view, and up-down control classes. 
   ================================ */

BOOL Init_Common_Controls(HWND hWnd)
{
    int cnt = 0;
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.

    ZeroMemory(&icex,sizeof(INITCOMMONCONTROLSEX));
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    if (InitCommonControlsEx(&icex))
        cnt++;

    ZeroMemory(&icex,sizeof(INITCOMMONCONTROLSEX));
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS;
    if (InitCommonControlsEx(&icex))
        cnt++;

    if (cnt == 2)
        return TRUE;

    return FALSE;

}

// Center a CHILD on a PARENT window
// =================================
BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT    rcChild, rcParent;
    int     cxChild, cyChild, cxParent, cyParent;
    int     cxScreen, cyScreen, xNew, yNew;
    HDC     hdc;

    // Get the Height and Width of the child window
    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

    // Get the Height and Width of the parent window
    GetWindowRect(hwndParent, &rcParent);
    cxParent = rcParent.right - rcParent.left;
    cyParent = rcParent.bottom - rcParent.top;

    // Get the display limits
    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rcParent.left + ((cxParent - cxChild) / 2);
    if (xNew < 0)
        xNew = 0;
    else if ((xNew + cxChild) > cxScreen)
        xNew = cxScreen - cxChild;

    // Calculate new Y position, then adjust for screen
    yNew = rcParent.top  + ((cyParent - cyChild) / 2);
    if (yNew < 0)
        yNew = 0;
    else if ((yNew + cyChild) > cyScreen)
        yNew = cyScreen - cyChild;

    // Set it, and return
    return SetWindowPos(hwndChild,
                        NULL,
                        xNew, yNew,
                        0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
}	// end - BOOL CenterWindow(HWND hwndChild, HWND hwndParent)

// ====================== ABOUT BOX ===========================
#define TIMER_ID2 1235
#define TIMER_INTERVAL2 200
UINT uiTimer2;

static char mission[] = "Mission Statement" EOL_CHR EOL_CHR
    "For many, many years, initially my only interest in Soduku" EOL_CHR
    "was to occassionally attempt the puzzle while sitting on the toilet ;=))" EOL_CHR EOL_CHR
    "Then on 18 August 2012 I decided why not write a program to solve " EOL_CHR
    "the puzzle. Of course that program should ONLY use the 'stategies'" EOL_CHR
    "I had developed" EOL_CHR EOL_CHR
    "After an initial command line attempt, I got interested enough" EOL_CHR
    "that I wanted it in a GUI to be able to 'see' what was happening." EOL_CHR EOL_CHR
    "After some progression I decided to try to see what was available" EOL_CHR
    "on internet. I suppose I should NOT have been so surpised that it has" EOL_CHR
    "quite a large internet presence. There are lots of interesting sites." EOL_CHR
    "Just put Sudoku in any search engine." EOL_CHR EOL_CHR
    "While quite a number also enumerated various strategies, I did not" EOL_CHR
    "really find much code. And most of the code I found 'solved' the" EOL_CHR
    "puzzle by brute force. Simply search for the first empty slot," EOL_CHR
    "select a suitable candidate by excluding current row, column and box" EOL_CHR
    "(region) and insert the remainders, and see what works." EOL_CHR EOL_CHR
    "No way! I wanted my program to only use known stategies, and be" EOL_CHR
    "educational in showing the steps it was taking. That is it shows all" EOL_CHR
    "candidates for an empty slot. On the first run through a stategy" EOL_CHR
    "only 'mark' what is is going to change, then in a second run eliminates" EOL_CHR
    "candidates, and in a third run insert values into slots where there" EOL_CHR
    "remains only ONE candidate, then starts over." EOL_CHR EOL_CHR
    "Of course the initial stategies are quite simple, and as each one 'fails'" EOL_CHR
    "to succeed, increase the complexity of the elimination of candidates." EOL_CHR
    "This involves some 'rating' of the strategies, and at the present time" EOL_CHR
    "can solve lots of examples. BUT it sometimes FAILS ;=((" EOL_CHR EOL_CHR
    "So it is a forever search for stategies that can be added. And not only" EOL_CHR
    "that, at this time some implemented strategies incorrectly eliminates" EOL_CHR
    "candidates, and can lead to a blocked path! So this is an ongoing task." EOL_CHR EOL_CHR
    "What I have done so far is released under a GNU GPL 2 licence, so welcome." EOL_CHR
    "others to enhance the code. It is presently targeted to the MS Windows OS," EOL_CHR
    "but I have tried to isolate the logic code, so other GUI interfaces can" EOL_CHR
    "be supported." EOL_CHR EOL_CHR
    "Geoff." EOL_CHR
    "2012/08/27" EOL_CHR;

 
BOOL CALLBACK MyEnumProc( HWND hwnd, LPARAM lParam )
 {
    char szName[256];
    HWND* FoundWindow = (HWND *)lParam; 
    GetClassName( hwnd, szName, 255 ); // Get the class name
    if (stricmp(szName, "SCROLLBAR") == 0 ) // Is it "SCROLLBAR"?
    {
        // Yes
        *FoundWindow = hwnd; // assign to our lparam that we passed in
        return FALSE; // Stop enumeration
    }
    // No
    return TRUE; // Continue to the next child window
}

// Function to return the first scroll bar control found
HWND GetScrollBarHwnd(HWND Parent)
{
    HWND FoundWindow = NULL; // Init to NULL
    EnumChildWindows( Parent, MyEnumProc, (LPARAM)&FoundWindow);
    return FoundWindow; // Return found window or NULL
}

VOID Get_Edit_Control_Text( HWND hdlg )
{
    static unsigned char szBuf[1024];
    WORD cbText;

    *(WORD *)szBuf = sizeof(szBuf); /* Sets the buffer size.     */
    cbText = (WORD) SendDlgItemMessage(hdlg, IDC_EDIT3,
        EM_GETLINE,
        0,                          /* Line number.              */
        (LPARAM) szBuf);     /* Buffer address.           */
    szBuf[cbText] = '\0';           /* Null-terminates the line. */
}

static HWND hScroll = 0;
static HWND hwndEdit;
// EM_GETTHUMB - Get the current position of the 'thumb'
/* ---------------
EM_LINESCROLL

The EM_LINESCROLL message is sent by an application to a multiple-line edit control 
to scroll the text of a multiple-line edit control.

This message is processed only by multiple-line edit controls.

An application specifies the number of lines to scroll vertically in the low-order 
word of the lParam and the number of characters to scroll horizontally in the high-order word.

The edit control will not scroll vertically past the last line of text in the edit control. 
If the current line plus the number of lines specified by the low-order word of the lParam 
parameter exceed the total number of lines in the edit control, the value is adjusted such 
that the last line of the edit control is scrolled to the top of the edit-control window.

The EM_LINESCROLL message can be used to scroll horizontally past the last character of any line.

This message is ignored if the multiple-line edit control was created with either the ES_RIGHT or the ES_CENTER style.

   --------------- */
static time_t seconds;
static bool stop_scroll;
static DWORD lines_scrolled, max_lines;
VOID Do_Text_Scroll(HWND hDlg)
{
    //int i = 0;
    //if ( hwndEdit && !hScroll )
    //    hScroll = GetScrollBarHwnd(hwndEdit);
    //
    //if (hScroll) {
    //    // can play
    //    i = 1;
    //}
    if (!stop_scroll) {
        time_t secs = time(0);
        if (secs != seconds) {
            // Presently scroll one line each second
            // TODO: Would be nice if this could be a pixel smooth scroll
            // rather than the current 'jerky' line scroll
            DWORD res = SendDlgItemMessage(hDlg, IDC_EDIT3, EM_GETTHUMB, 0, 0 );
            if (res >= 100) {
                stop_scroll = true;
            }
            SendDlgItemMessage(hDlg, IDC_EDIT3, EM_LINESCROLL, 0, (LPARAM)1 );
            seconds = secs;
            lines_scrolled++;
            if (lines_scrolled >= max_lines) {
                stop_scroll = true;
            }
        }
    }
}

// Maybe I could add a BITMAP of the current Sudoku with
// HANDLE hBitmap;
// hBitmap = LoadImage(NULL, "aa.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
// HWND hPic = GetDlgItem(hwnd,IDC_PICTURE0);
// SendMessage(hPic, STM_SETIMAGE, IMAGE_BITMAP, LPARAM(hBitmap)); 
// Also SHLoadImageFile can be used get  HBITMAP from bmp, jpeg and gif images.
// see : http://msdn.microsoft.com/en-us/library/aa452250.aspx
BOOL Do_INIT_About(HWND hDlg)
{
    char *tb = GetNxtBuf();
#ifdef SUDO_RC_VALUE
    sprintf(tb,"Sudoku, version %s.%s of %s", SUDO_VERSION, SUDO_RC_VALUE, SUDO_DATE);
#else
    sprintf(tb,"Sudoku, version %s of %s", SUDO_VERSION, SUDO_DATE);
#endif
    SetDlgItemText(hDlg, IDC_LABEL1, tb);

    sprintf(tb,"Copyright (C) Geoff R. McLane, 2012-2015");
    SetDlgItemText(hDlg, IDC_LABEL2, tb);

    sprintf(tb,"Compiled on %s at %s", __DATE__, __TIME__);
    SetDlgItemText(hDlg, IDC_EDIT1, tb);

    sprintf(tb,"Compiled by %s", COMPILER_STR);
    SetDlgItemText(hDlg, IDC_EDIT2, tb);

    sprintf(tb,"Licence GNU GPL version 2 (or later)");
    SetDlgItemText(hDlg, IDC_LABEL3, tb);

    SetDlgItemText(hDlg, IDC_EDIT3, mission);

    // CenterWindow(hDlg, g_hWnd);

    // removed the tab stops, so maybe this is not needed
    //SendDlgItemMessage(hDlg, IDOK, WM_SETFOCUS, NULL, NULL);
   // SetFocus(GetDlgItemHandle(hDlg, IDOK));
    uiTimer2 = SetTimer( hDlg, TIMER_ID2, TIMER_INTERVAL2, NULL);
    if (uiTimer2) {
        // Hmmm this FAILED since it ONLY enumerates the PARENTS -
        // so found class name 'Edit', but NOT the vertical scroll
        // hScroll = GetScrollBarHwnd(hDlg);
        // all fails
        //hwndEdit = GetDlgItem( hDlg, IDC_EDIT3 );
        //if (hwndEdit) {
        //    hScroll = GetScrollBarHwnd(hwndEdit);
        //}

        lines_scrolled = (DWORD)strlen(mission);
        DWORD dwi;
        max_lines = 0;
        for (dwi = 0; dwi < lines_scrolled; dwi++) {
            if (mission[dwi] == '\n')
                max_lines++;
        }
        lines_scrolled = 0;
        if (max_lines > 14)
            max_lines -= 14;
    }


    CenterWindow(hDlg, g_hWnd);
    stop_scroll = false;
	return TRUE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_About(hDlg);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON1)
		{
            if (uiTimer2)
                KillTimer(hDlg, TIMER_ID2);
            hScroll = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		} else {
            // The edit control's parent window receives notification messages through WM_COMMAND messages.
            // The wParam parameter specifies the identifier of the edit control, and the lParam parameter 
            // specifies the handle to the edit control in the low-order word and 
            // the notification code in the high-order word.
            if (LOWORD(lParam) == EN_VSCROLL) {
                if (wParam == IDC_EDIT3) {
                    stop_scroll = true;
                }
            }
        }
		break;
    case WM_TIMER:
        Do_Text_Scroll(hDlg);
        break;
	}
	return (INT_PTR)FALSE;
}

void Do_IDM_ABOUT(HWND hWnd)
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
    if (res == IDC_BUTTON1)
        Post_Command( IDM_EXIT );
}
// ============================================================

// ====================== GENERATE BOX ===========================
static int spot_cnt;
INT_PTR Do_INIT_GENERATE(HWND hDlg)
{
    PABOX2 pb = get_curr_box();
    int spots = 0;
    int row, col;
    for (row = 0; row < 9; row++)
        for (col = 0; col < 9; col++)
            if (pb->line[row].val[col]) spots;

    char *tb = GetNxtBuf();
    sprintf(tb,"The current box has %d spots set." EOL_CHR "Give a number larger than this below" EOL_CHR "to generate a new Sudoku.", spots);
    strcat(tb,EOL_CHR"An attempt will be made to generate" EOL_CHR "a Sudoku with a single unique solution.");
    SetDlgItemText(hDlg, IDC_EDIT1, tb);

    if (spots < g_iMinGiven)
        spots = g_iMinGiven;

    spot_cnt = spots;

    sprintf(tb,"%d", spots);
    SetDlgItemText(hDlg, IDC_EDIT2, tb);
    
    CenterWindow(hDlg, g_hWnd);

    return (INT_PTR)TRUE;
}

bool Do_Generate_OK(HWND hDlg)
{
    char *tb = GetNxtBuf();
    *tb = 0;
    UINT ret = GetDlgItemText(hDlg, IDC_EDIT2, tb, 256);
    if (ret) {
        int spots = atoi(tb);
        if ((spots > 0) && (spots >- spot_cnt)) {
            if (spots != g_iMinGiven) {
                g_iMinGiven = spots;
                gChgSpot = TRUE;
            }
        }
    }
    return true;
}

INT_PTR CALLBACK Generate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_GENERATE(hDlg);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
            if (LOWORD(wParam) == IDOK) {
                if (!Do_Generate_OK(hDlg))
        			return (INT_PTR)FALSE;
            }
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Do_ID_EDIT_GENERATE(HWND hWnd)
{
    int cont = true;
    while(cont) {
        int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_GENERATE), hWnd, Generate);
        if (res == IDOK) {
            PABOX2 pb = get_curr_box();
            CreateProgressDialog(hWnd);
            bool b = Generate_New( pb, g_iMinGiven );
            if (!b) {
                char *tb = GetNxtBuf();
                sprintf(tb,"EEK! Failed to generate a NEW sudoku!" EOL_CHR "with %d spots." EOL_CHR "Click [Yes] to try again!", g_iMinGiven);
                //res = MB2(tb,"Generation Failed");
                //if (res != IDYES) {
                res = Do_MsgBox_YN(tb,"Generation Failed");
                if (res) {
                    cont = false;
                }
            } else {
                cont = false;
            }
        } else {
            cont = false;
        }
    }
}
// ===============================================================
// Save TEMP file
INT_PTR Do_INIT_SAVETEMP(HWND hDlg)
{
    CheckDlgButton(hDlg, IDC_CHECK1,
        (g_dwSvOptBits & sff_NO_LENGTH_CHECK) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK2,
        (g_dwSvOptBits & sff_NO_ADD_ASCII) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK3,
        (g_dwSvOptBits & sff_NO_CHANGE_RESET) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK4,
        (g_dwSvOptBits & sff_NO_RESET_TITLE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK5,
        (g_dwSvOptBits & sff_NO_ADD_CSV) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK6,
        (g_dwSvOptBits & sff_NO_CHANGE_ACTIVE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK7,
        (g_dwSvOptBits & sff_ADD_ASCII_BLOCK) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hDlg, IDC_CHECK8,
        (g_dwSvOptBits & sff_ADD_CANDIDATES) ? BST_CHECKED : BST_UNCHECKED);

    if (g_szSvOptFl[0] == 0) {
        if (Get_LocalData_Path(g_szSvOptFl))
            strcat(g_szSvOptFl, "temptemp.txt");
        else
            strcpy(g_szSvOptFl,"temptemp.txt");
        gChgTSF = TRUE;
    }
    SetDlgItemText(hDlg, IDC_EDIT1, g_szSvOptFl);

    CenterWindow(hDlg, g_hWnd);
    return (INT_PTR)TRUE;
}

#define ISCHKD(a,b,c) { \
    UINT _ui = IsDlgButtonChecked(hDlg, a); \
    if (_ui == BST_CHECKED) b |= c; \
}


bool Do_SaveTemp_OK(HWND hDlg)
{
    char *tb = GetNxtBuf();
    *tb = 0;
    UINT ret = GetDlgItemText(hDlg, IDC_EDIT1, tb, 256);
    if (ret) {
        if (strcmp(g_szSvOptFl,tb)) {
            strcpy(g_szSvOptFl,tb);
            gChgTSF = TRUE;
        }
    }
    DWORD opts = 0;

    ISCHKD( IDC_CHECK1, opts, sff_NO_LENGTH_CHECK );
    ISCHKD( IDC_CHECK2, opts, sff_NO_ADD_ASCII );
    ISCHKD( IDC_CHECK3, opts, sff_NO_CHANGE_RESET );
    ISCHKD( IDC_CHECK4, opts, sff_NO_RESET_TITLE );
    ISCHKD( IDC_CHECK5, opts, sff_NO_ADD_CSV );
    ISCHKD( IDC_CHECK6, opts, sff_NO_CHANGE_ACTIVE );
    ISCHKD( IDC_CHECK7, opts, sff_ADD_ASCII_BLOCK );
    ISCHKD( IDC_CHECK8, opts, sff_ADD_CANDIDATES );
    if (opts != g_dwSvOptBits) {
        g_dwSvOptBits = opts;
        gChgTSO = TRUE;
    }
    return true;
}


INT_PTR CALLBACK SaveTemp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_SAVETEMP(hDlg);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
            if (LOWORD(wParam) == IDOK) {
                if (!Do_SaveTemp_OK(hDlg))
        			return (INT_PTR)FALSE;
            }
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Do_ID_FILE_SAVETEMP(HWND hWnd)
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_SAVETEMP), hWnd, SaveTemp);
    if (res == IDOK) {
        Write_Temp_File();
        set_repaint(false);
    }
}


// Draw Strong Links
INT_PTR Do_INIT_STRONGLINKS(HWND hDlg) // IDD_STRONGLINKS
{
    char *tb = GetNxtBuf();
    int i, val, len, v2, cnt;
    *tb = 0;
    for (i = 0; i < 9; i++)
        g_iSlValues[i] = 0; // clear ALL values
    char *ps = g_szCandList;
    len = (int)strlen(ps);
    if (len) {
        cnt = 0;
        for (i = 0; i < len; i++) {
            val = ps[i];    // extract value
            if (ISDIGIT(val)) {
                v2 = val - '0';
                if (VALUEVALID(v2)) {
                    g_iSlValues[v2 - 1] = v2;
                    cnt++;
                }
            }
        }
        if (!cnt)
            g_iSlValues[0] = 1; // at least set ONE value
    } else {
        g_iSlValues[0] = 1; // at least set ONE value
    }

    for (i = 0; i < 9; i++) {
        val = g_iSlValues[i];
        if (!val) continue;
        sprintf(EndBuf(tb),"%d",val);
    }
    SetDlgItemText(hDlg, IDC_EDIT1, tb);

    CheckDlgButton(hDlg, IDC_CHECK2,
        (g_bIWL) ? BST_CHECKED : BST_UNCHECKED);

    CheckDlgButton(hDlg, IDC_CHECK3,
        (g_bIBL) ? BST_CHECKED : BST_UNCHECKED);

    // char szIASW[] = "Alternating_Strong_Weak";
    CheckDlgButton(hDlg, IDC_CHECK4,
        (g_bASW) ? BST_CHECKED : BST_UNCHECKED);

    // char szExChn[] = "Extend_Link_Chains";
    CheckDlgButton(hDlg, IDC_CHECK5,
        (g_bExtChn) ? BST_CHECKED : BST_UNCHECKED);

    // NOT A GOOD IDEA TO OFFER AN ALL ;=((
    //CheckDlgButton(hDlg, IDC_CHECK1,
    //    (g_bAllSL) ? BST_CHECKED : BST_UNCHECKED);
    // Instead offer to trun on DEBUG output
    // { "Strong Links 2",IDC_CHECK12,&add_debug_sl2, &bChgSL2 },  // 11
    CheckDlgButton(hDlg, IDC_CHECK1,
        (add_debug_sl2) ? BST_CHECKED : BST_UNCHECKED);


    CenterWindow(hDlg, g_hWnd);
    return (INT_PTR)TRUE;
}

void Toggle_Bool( BOOL on, PBOOL pOpt, PBOOL pChg )
{
    if (on) {
        if ( !*pOpt ) {
            *pOpt = on;
            *pChg = TRUE;
        }
    } else {
        if ( *pOpt ) {
            *pOpt = on;
            *pChg = TRUE;
        }
    }
}

void Toggle_CheckBox_Bool( UINT ret, PBOOL pOpt, PBOOL pChg )
{
    Toggle_Bool( ((ret == BST_CHECKED) ? TRUE : FALSE),
        pOpt, pChg );
}



bool Do_STRONGLINKS_OK(HWND hDlg) // IDD_STRONGLINKS
{
    char *tb = GetNxtBuf();
    UINT ret = GetDlgItemText(hDlg, IDC_EDIT1, tb, 256);
    UINT i;
    int val, v2, cnt;
    char *ps = g_szCandList;
    for (i = 0; i < 9; i++)
        g_iSlValues[i] = 0; // clear ALL values

    cnt = 0;
    for (i = 0; i < ret; i++) {
        val = tb[i];    // extract value
        if (ISDIGIT(val)) {
            v2 = val - '0';
            if (VALUEVALID(v2)) {
                g_iSlValues[v2 - 1] = v2;
                cnt++;
            }
        }
    }
    if (!cnt) {
       g_iSlValues[0] = 1; // at lesat set ONE value
    }

#if 0 // NOT a good idea to offer ALL!!!
    ret = IsDlgButtonChecked(hDlg, IDC_CHECK1);
    Toggle_CheckBox_Bool( ret, &g_bAllSL, &gChgASV );
    *tb = 0;
    if (g_bAllSL) {
        // set ALL candidates
        for (i = 0; i < 9; i++) {
            val = i + 1;
            g_iSlValues[i] = val;
            sprintf(EndBuf(tb),"%d",val);
        }
    } else {
        for (i = 0; i < 9; i++) {
            val = g_iSlValues[i];
            if (!val) continue;
            sprintf(EndBuf(tb),"%d",val);
        }
    }
#endif // 0 - bad idea to offer ALL - should be ofr ONE candidate

    // COPY it to INI string
    if (strcmp(tb,ps)) {
        gChgCLST = TRUE;
        strcpy(ps,tb);  // update the STRING
    }

    // include WEAK links
    ret = IsDlgButtonChecked(hDlg, IDC_CHECK2);
    Toggle_CheckBox_Bool( ret, &g_bIWL, &gChgIWL );

    // include BOX links
    ret = IsDlgButtonChecked(hDlg, IDC_CHECK3);
    Toggle_CheckBox_Bool( ret, &g_bIBL, &gChgIBL );

    // char szIASW[] = "Alternating_Strong_Weak";
    ret = IsDlgButtonChecked(hDlg, IDC_CHECK4);
    Toggle_CheckBox_Bool( ret, &g_bASW, &gChgASW );

    // char szExChn[] = "Extend_Link_Chains";
    ret = IsDlgButtonChecked(hDlg, IDC_CHECK5);
    Toggle_CheckBox_Bool( ret, &g_bExtChn, &gChgELC );

    //sprintf(tb,"%s Strong Links. List ", 
    //    (g_bAllSL ? "ALL" : "per list"));
    //ret = Do_Strong_Links(get_curr_box());

    sprintf(tb,"Dlg.OK ID_VIEW_LINKS per list ");
    ret = 0;
    for (i = 0; i < 9; i++) {
        if (g_iSlValues[i]) {
            sprintf(EndBuf(tb),"%d", g_iSlValues[i]);
            ret++;
        }
    }
    if (ret)
        sprintf(EndBuf(tb)," Count %d", ret);
    else
        strcat(tb," None to show!");
    sprtf("%s\n",tb);

    //CheckDlgButton(hDlg, IDC_CHECK1,
    //    (add_debug_sl2) ? BST_CHECKED : BST_UNCHECKED);
    // { "Strong Links 2",IDC_CHECK12,&add_debug_sl2, &bChgSL2 },  // 11
    ret = IsDlgButtonChecked(hDlg, IDC_CHECK1);
    Toggle_CheckBox_Bool( ret, &add_debug_sl2, &bChgDSL2 );

    return true;
}

INT_PTR CALLBACK StrongLinks(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_STRONGLINKS(hDlg);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
            if (LOWORD(wParam) == IDOK) {
                if (!Do_STRONGLINKS_OK(hDlg))
        			return (INT_PTR)FALSE;
            }
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

bool Do_ID_VIEW_STRONGLINKS( HWND hWnd )
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_STRONGLINKS), hWnd, StrongLinks);
    if (res == IDOK) {
        Post_Command(ID_VIEW_LINKS);
        return true;
    }
    return false;
}


BOOL Do_INIT_Progress(HWND hDlg)
{
    // TODO - init the progress IDC_PROGRESS1
    //CheckDlgButton(hwndDlg, ID_ABSREL, fRelative); 
    PBRANGE PBRange;
    DWORD low = SendMessage(hDlg, PBM_GETRANGE, TRUE, (LPARAM)&PBRange );
    DWORD high = SendMessage(hDlg, PBM_GETRANGE, TRUE, (LPARAM)&PBRange );
    
    CenterWindow(hDlg, g_hWnd);
    return TRUE;
}

//int iLine;             // Receives line number.
//BOOL fRelative;        // Receives check box status. 
BOOL CALLBACK ProgressProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    //BOOL fError; 
 
    switch (message) 
    { 
        case WM_INITDIALOG:
            return Do_INIT_Progress(hDlg);
 
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                    DestroyWindow(hDlg); 
                    g_hProgress = NULL; 
                    //fRelative = IsDlgButtonChecked(hwndDlg, ID_ABSREL); 
                    //iLine = GetDlgItemInt(hwndDlg, ID_LINE, &fError, fRelative); 
                    //if (fError) 
                    //{ 
                    //    MessageBox(hwndDlg, SZINVALIDNUMBER, SZGOTOERR, MB_OK); 
                    //    SendDlgItemMessage(hwndDlg, ID_LINE, EM_SETSEL, 0, -1L); 
                    //} 
                    //else 
                    return TRUE; 
 
                case IDCANCEL: 
                    // TODO - Notify the owner window to carry out the task. 
                    DestroyWindow(hDlg); 
                    g_hProgress = NULL; 
                    return TRUE; 
            } 
    } 
    return FALSE; 
} 


void CreateProgressDialog( HWND hWnd )
{
    if (!g_hProgress || !IsWindow(g_hProgress)) 
    {
        g_hProgress = CreateDialog(hInst, 
                                   MAKEINTRESOURCE(IDD_PROGRESS), 
                                   hWnd, 
                                   (DLGPROC)ProgressProc );
        if (g_hProgress)
            ShowWindow(g_hProgress, SW_SHOW);
    }
}

// ====================================================================================
// ------------------------------------------------------------------------------------
// Message Dialog
// see : http://msdn.microsoft.com/en-us/library/windows/desktop/bb775460(v=vs.85).aspx
static HWND hDlgEdit = 0;
static size_t msg_size = 0;
static LRESULT lr_line_count = 0;
static DWORD num_lines = 0;
static BOOL bOnShow = FALSE;
BOOL Get_Dlg_OnShow() { return bOnShow; }
BOOL Set_Dlg_OnShow( BOOL bShw ) { BOOL ret = bOnShow; bOnShow = bShw; return ret; }
BOOL Toggle_Dlg_Show()
{
    if (!g_hMsgDialog || !IsWindow(g_hMsgDialog)) 
        return FALSE;
    // char szShwMsgD[] = "Show_Message_Dialog";
    int nCmdShow = SW_SHOW;
    if (bOnShow) {
        g_bShwMsgD = FALSE;
        nCmdShow = SW_HIDE;
    } else {
        g_bShwMsgD = TRUE;
        nCmdShow = SW_SHOW;
    }
    gChgSMsgD = TRUE;
    ShowWindow( g_hMsgDialog, nCmdShow );
    bOnShow = !bOnShow;
    return bOnShow;
}

/* -----------------
typedef struct tagWINDOWPLACEMENT {     / * wndpl * / 
   UINT length; 
   UINT flags; 
   UINT showCmd; 
   POINT ptMinPosition; 
   POINT ptMaxPosition; 
   RECT rcNormalPosition; // normal position - windows coordinates
} WINDOWPLACEMENT;
  ----------------- */
LRESULT max_chars = 0;
BOOL Do_INIT_MsgDialog(HWND hDlg)
{
    BOOL bRet = TRUE;
    hDlgEdit = GetDlgItem(hDlg,IDC_EDIT1);
    if (hDlgEdit) {
        HFONT hf = Get_Font();
        if (hf) SendMessage(hDlgEdit, WM_SETFONT, (WPARAM)hf, TRUE);
        lr_line_count = SendMessage(hDlgEdit, EM_GETLINECOUNT, 0, 0);
        SendMessage(hDlgEdit, EM_LIMITTEXT, (WPARAM)64000, 0);
        max_chars = SendMessage(hDlgEdit, EM_GETLIMITTEXT, 0, 0 );
        // sprtf("Edit control can hold %u chars\n", (int)max_chars);
    }
    // char szWin2[] = "Window2";
    // char szGOut2[] = "OutSaved2";
    // BOOL  bChgWP2 = FALSE;
    if (gbGotWP2) {
        // WINDOWPLACEMENT   g_sWP2;
        // 1258,331,1738,840
        // sprtf("Dlg WP rect %s\n", Get_Rect_Stg(&g_sWP2.rcNormalPosition));
        if (res_scn_rect) {
            RECT rc = g_sRect;
            int width = g_sWP2.rcNormalPosition.right - g_sWP2.rcNormalPosition.left;
            int height = g_sWP2.rcNormalPosition.bottom - g_sWP2.rcNormalPosition.top;
            if (g_sWP2.rcNormalPosition.left > rc.right - width) {
                g_sWP2.rcNormalPosition.left = rc.right - width;
                if (g_sWP2.rcNormalPosition.left + width > rc.right)
                    g_sWP2.rcNormalPosition.left = 0;
                g_sWP2.rcNormalPosition.right = g_sWP2.rcNormalPosition.left + width;
                bChgWP2 = TRUE;
            }
            if (g_sWP2.rcNormalPosition.top > rc.top + height) {
                g_sWP2.rcNormalPosition.top = rc.top + height;
                if (g_sWP2.rcNormalPosition.top > g_sWP2.rcNormalPosition.bottom)
                    g_sWP2.rcNormalPosition.top = 0;
                g_sWP2.rcNormalPosition.bottom = g_sWP2.rcNormalPosition.top + height;
                bChgWP2 = TRUE;
            }
        }

        MoveWindow( hDlg, 
            g_sWP2.rcNormalPosition.left, //  _In_  int X,
            g_sWP2.rcNormalPosition.top,  //  _In_  int Y,
            g_sWP2.rcNormalPosition.right - g_sWP2.rcNormalPosition.left, // _In_  int nWidth,
            g_sWP2.rcNormalPosition.bottom - g_sWP2.rcNormalPosition.top, // _In_  int nHeight,
            FALSE );    //  _In_  BOOL bRepaint
    }
    return bRet;
}

#ifndef MAX_STRING
#define MAX_STRING 512
#endif
#ifndef MEOL
#define MEOL "\r\n"
#endif

#define TRY_APPEND_STRING

#ifdef TRY_APPEND_STRING

// from : http://social.msdn.microsoft.com/Forums/en/vcgeneral/thread/899e8e4f-d8cb-46dc-b3a0-82781cb5f796
// found this snippet - in fact the author did NOT want it to scroll to the bottom each time
// but I DO ;=)) AND IT WORKS GREAT, even on my Simple Edit Control - but maybe I should
// usa a Rich Edit Control, and ADD some color (some day).
//SendMessage(hRichEdit, EM_SETSEL, -1, EOF);
//SendMessage(hRichEdit, EM_REPLACESEL, 0, LPARAM(str));
//SendMessage(hRichEdit, EM_SETSEL, x, y); //Set the caret's position to last position in control
LRESULT total_chars = 0;
LRESULT grand_total = 0;
// OVERFLOW - When the character count gets close to MAXIMUM
// tried two methods to adjust the text - starting at half
// but neither worked correctly
// Maybe if I seet up a new paint, and exit, and put the balance 
// of text next paint time...
#define TRY_TEXT_REPLACE
#define TRY_EXIT_REPAINT

VOID Do_Msg_Paint(HWND hDlg)
{
    char buf[MAX_STRING+8];
    vSTG *vlp = Get_Screen_Lines();
    size_t max = vlp->size();
    size_t i = msg_size;
    char *ps = buf;
    string s;
    if ((max > i) && hDlgEdit) {
        lr_line_count = SendMessage(hDlgEdit, EM_GETLINECOUNT, 0, 0);
        SendMessage(hDlgEdit, EM_LINESCROLL, 0, (LPARAM)lr_line_count);
        for (; i < max; i++) {
            s = vlp->at(i);  // get string
            strncpy(ps,s.c_str(),MAX_STRING);
            ps[MAX_STRING-1] = 0;   // ensure zero terminated
            int len = (int)strlen(ps);
            if (len) {
                while(len--) {
                    if (buf[len] > ' ') {
                        len++;
                        break;
                    }
                    buf[len] = 0;
                }
                if (len > 0) {
                    strcat(buf,MEOL);
                    total_chars += len + 2;
                    grand_total += len + 2;
                    num_lines++;
                    SendMessage(hDlgEdit, EM_SETSEL, -1, EOF);
                    SendMessage(hDlgEdit, EM_REPLACESEL, 0, LPARAM(buf));
                }
            }
            if (total_chars > 60000)
            {

#ifdef TRY_TEXT_REPLACE
                LRESULT tlen = SendMessage(hDlgEdit, WM_GETTEXTLENGTH, 0, 0);
                LRESULT i, j;
                if (tlen && (tlen > 60000)) {
                    int c;
                    char *tbuf = (char *)malloc( tlen + 4 );
                    CHKMEM(tbuf);
                    tlen = SendMessage(hDlgEdit, WM_GETTEXT, (WPARAM)(tlen + 2), (LPARAM)tbuf);
                    j = 0;
                    for (i = tlen - 1; i >= 0; i--) {
                        c = tbuf[i];
                        j++;
                        if (j > 30000) {
                            if (c == '\n') {
                                i++;
                                break;
                            }
                        }
                    }
                    SendMessage(hDlgEdit, WM_SETTEXT, 0, (LPARAM)&tbuf[i]);
                    total_chars -= i;
                    free(tbuf);
                    lr_line_count = SendMessage(hDlgEdit, EM_GETLINECOUNT, 0, 0);
                    SendMessage(hDlgEdit, EM_LINESCROLL, 0, (LPARAM)lr_line_count);
                }
#else
                SendMessage(hDlgEdit, EM_SETSEL, 0, 30000);
                buf[0] = 0;
                SendMessage(hDlgEdit, EM_REPLACESEL, 0, LPARAM(buf));
                total_chars -= 30000;
                lr_line_count = SendMessage(hDlgEdit, EM_GETLINECOUNT, 0, 0);
                //SendMessage(hDlgEdit, EM_SETSEL, -1, EOF);
                SendMessage(hDlgEdit, EM_LINESCROLL, 0, (LPARAM)lr_line_count);
#endif // TRY_TEXT_REPLACE
#ifdef TRY_EXIT_REPAINT
                // hmm, try exit and set up another repaint...
                set_repaint2();
                max = i;
                break;
#endif 
            }
        }
        lr_line_count = SendMessage(hDlgEdit, EM_GETLINECOUNT, 0, 0);
        sprintf(ps,"Tot lns %d, chs %d",(int)lr_line_count,(int)total_chars);
        if (total_chars < grand_total) {
            sprintf(EndBuf(ps)," of %d, tl %d", (int)grand_total, num_lines);
        }
        SetDlgItemText(g_hMsgDialog, IDC_EDIT2, ps);
        SendMessage(hDlgEdit, EM_LINESCROLL, 0, (LPARAM)lr_line_count);
        SendMessage(hDlgEdit, EM_EMPTYUNDOBUFFER, 0, 0);
    }
    msg_size = max;
}

#else // !TRY_APPEND_STRING

// Maybe keep an ALL string, rather than gettting it each time
static string all_text;
#define USE_ALL_TXT
VOID Do_Msg_Paint(HWND hDlg)
{
    char buf[MAX_STRING+2];
    vSTG *vlp = Get_Screen_Lines();
    size_t max = vlp->size();
    size_t i = msg_size;
#ifndef USE_ALL_TXT
    string all;
#endif
    if ((max > i) && hDlgEdit) {
#ifndef USE_ALL_TXT
        LRESULT tlen = SendMessage(hDlgEdit, WM_GETTEXTLENGTH, 0, 0);
        if (tlen) {
            char *tbuf = (char *)malloc( tlen + 4 );
            CHKMEM(tbuf);
            tlen = SendMessage(hDlgEdit, WM_GETTEXT, (WPARAM)(tlen + 2), (LPARAM)tbuf);
            all = tbuf;
            free(tbuf);
        }
#endif // #ifndef USE_ALL_TXT
        for (; i < max; i++) {
            string s = vlp->at(i);  // get string
            char *ps = buf;
            strncpy(ps,s.c_str(),MAX_STRING);
            ps[MAX_STRING-1] = 0;   // ensure zero terminated
            int len = (int)strlen(ps);
            if (len) {
                while(len--) {
                    if (buf[len] > ' ') {
                        len++;
                        break;
                    }
                    buf[len] = 0;
                }
                if (len > 0) {
                    strcat(buf,MEOL);
                    num_lines++;
#ifndef USE_ALL_TXT
                    all += buf;
#else
                    all_text += buf;
#endif
                }
            }
        }
#ifndef USE_ALL_TXT
        SendMessage(hDlgEdit, WM_SETTEXT, 0, (LPARAM)all.c_str());
#else
        SendMessage(hDlgEdit, WM_SETTEXT, 0, (LPARAM)all_text.c_str());
#endif
        SendMessage(hDlgEdit, EM_EMPTYUNDOBUFFER, 0, 0);
        SendMessage(hDlgEdit, EM_LINESCROLL, (WPARAM)num_lines, 0);
    }
    msg_size = max;

}
#endif // TRY_APPEND_STRING

VOID Do_Msg_WP(HWND hDlg)
{
    UpdateWP2(hDlg);
}


BOOL CALLBACK MsgDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
            return Do_INIT_MsgDialog(hDlg);
        case WM_PAINT:
            Do_Msg_Paint(hDlg);
            break;
       case WM_SIZE:
       case WM_MOVE:
            Do_Msg_WP(hDlg);
            break;
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                    g_bShwMsgD = FALSE;
                    gChgSMsgD = TRUE;
                    Set_Dlg_OnShow(FALSE);
                    ShowWindow(hDlg, SW_HIDE);
                    break;
                case IDC_BUTTON1:
                    Post_Command(IDM_EXIT);
                    break;
            } 
    } 
    return FALSE; 
} 

BOOL CreateMessageDialog( HWND hWnd )
{
    BOOL bRet = FALSE;
    if (!g_hMsgDialog || !IsWindow(g_hMsgDialog)) 
    {
        g_hMsgDialog = CreateDialog(hInst, 
                                   MAKEINTRESOURCE(IDD_EDITBOX), 
                                   hWnd, 
                                   (DLGPROC)MsgDialogProc );
        if (g_hMsgDialog) {
            // char szShwMsgD[] = "Show_Message_Dialog";
            if (g_bShwMsgD) {
                ShowWindow(g_hMsgDialog, SW_SHOW);
                Set_Dlg_OnShow(TRUE);
            } else {
                ShowWindow(g_hMsgDialog, SW_HIDE);
                Set_Dlg_OnShow(FALSE);
            }
            bRet = TRUE;
        }
    }
    return bRet;
}


#if 0 // 0000000000000000 NOT USED 00000000000000000000000
// ------------------------------------------------------------------------------------
// ====================================================================================
// ====================================
// build your own template in memory
// ====================================
#define ID_HELP   150
#define ID_TEXT   200

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG: 
            return TRUE; 
 
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                case IDCANCEL: 
                    DestroyWindow(hwndDlg); 
                    return TRUE; 
            } 
    } 
    return FALSE; 
} 

LPWORD lpwAlign(LPWORD lpIn)
{
#if defined(WIN64) || defined(_WIN64)
    uint64_t u = (uint64_t)lpIn;
    u++;
    u >>= 1;
    u <<= 1;
    return (LPWORD)u;
#else
    ULONG ul;

    ul = (ULONG)lpIn;
    ul ++;
    ul >>=1;
    ul <<=1;
    return (LPWORD)ul;
#endif
}

LRESULT DisplayMyMessage(HINSTANCE hinst, HWND hwndOwner, LPSTR lpszMessage)
{
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    LRESULT ret;
    int nchar;

    hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
    if (!hgbl)
        return -1;
 
    lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);
 
    // Define a dialog box.
    lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
    lpdt->cdit = 3;         // Number of controls
    lpdt->x  = 10;  lpdt->y  = 10;
    lpdt->cx = 100; lpdt->cy = 100;

    lpw = (LPWORD)(lpdt + 1);
    *lpw++ = 0;             // No menu
    *lpw++ = 0;             // Predefined dialog box class (by default)

    lpwsz = (LPWSTR)lpw;
    nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "My Dialog", -1, lpwsz, 50);
    lpw += nchar;

    //-----------------------
    // Define an OK button.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10; lpdit->y  = 70;
    lpdit->cx = 80; lpdit->cy = 20;
    lpdit->id = IDOK;       // OK button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;        // Button class

    lpwsz = (LPWSTR)lpw;
    nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "OK", -1, lpwsz, 50);
    lpw += nchar;
    *lpw++ = 0;             // No creation data

    //-----------------------
    // Define a Help button.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 55; lpdit->y  = 10;
    lpdit->cx = 40; lpdit->cy = 20;
    lpdit->id = ID_HELP;    // Help button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;        // Button class atom

    lpwsz = (LPWSTR)lpw;
    nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "Help", -1, lpwsz, 50);
    lpw += nchar;
    *lpw++ = 0;             // No creation data

    //-----------------------
    // Define a static text control.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10; lpdit->y  = 10;
    lpdit->cx = 40; lpdit->cy = 20;
    lpdit->id = ID_TEXT;    // Text identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;        // Static class

    for (lpwsz = (LPWSTR)lpw; *lpwsz++ = (WCHAR)*lpszMessage++;);
    lpw = (LPWORD)lpwsz;
    *lpw++ = 0;             // No creation data

    GlobalUnlock(hgbl); 
    ret = DialogBoxIndirect(hInst, 
                           (LPDLGTEMPLATE)hgbl, 
                           hwndOwner, 
                           (DLGPROC)DialogProc); 
    GlobalFree(hgbl); 
    return ret; 
}

#endif // 0000000000000000000000000000000000000000000000000000000000

// ======================================================================
// Import a SUDOKU
static char curr_sudo[128];
static HWND hImpDlg = 0;
//////////////////////////////////////////////////////////////////////////////
// return FALSE to STAY in DIALOG
// return TRUE to CLOSE DIALOG
// TODO - Has a memory leak if aborted - the COPY is NOT deleted
BOOL Do_Import_OK(HWND hDlg)
{
    int spots;
    char *tb = GetNxtBuf();
    int len = GetDlgItemText(hDlg,IDC_EDIT1,tb,256);
    if (len < 81) {
        sprintf(tb,"Oops, only got %d characters!" MEOL
            "Need 81, 9x9..." MEOL
            "Click [No] to abort" MEOL, len );
        strcat(tb,"Click [Yes] to redo" MEOL);
        //len = MB2(tb,"NOT ENOUGH CHARACTERS");
        //if (len == IDYES)
        //    return FALSE; // stay and redo
        //else
        //    return TRUE; // forget it - close DIALOG
        len = Do_MsgBox_YN(tb,"NOT ENOUGH CHARACTERS");
        if (len)
            return TRUE;
        else
            return FALSE;
    }
    if (len > 81) {
        sprintf(tb,"Oops, TOO many charcters! Got %d!" MEOL
            "Need 81, 9x9..." MEOL
            "Click [No] to abort" MEOL, len );
        strcat(tb,"Click [Yes] to redo" MEOL);
        //len = MB2(tb,"TOO MANY CHARACTERS");
        //if (len == IDYES)
        //    return FALSE; // stay and redo input
        //else
        //    return TRUE; // forget it - close dialog
        len = Do_MsgBox_YN(tb,"TOO MANY CHARACTERS");
        if (len)
            return TRUE; // forget it
        else
            return FALSE; // stay and redo
    }

    int i, c;
    PABOX2 pb = get_curr_box(); // get BOX
    PABOX2 pb2 = Copy_Box2(pb); // get COPY
    int row = 0;
    int col = 0;
    bool bad = false;
    invalidate_box(pb2); // invalidate COPY
    // read imput as a string of DIGITS
    for (i = 0; i < len; i++) {
        c = tb[i];
        if (ISDIGIT(c)) {
            c -= '0';
        } else {
            c = 0;
        }
        pb2->line[row].val[col++] = c;
        if (col >= 9) {
            row++;
            col = 0;
            if (row >= 9) {
                bad = true;
                break;
            }
        }
    }

    // get SPOT count
    spots = Get_Spots(pb2);
    c = validate_box(pb2);
    if (c) {
        sprintf(tb,"Invalid Sudoku! Got %d spots, but" MEOL, spots);
        switch(c) {
        case 1:   // contains an invalid value = NO PAINT
            sprintf(EndBuf(tb),"Found cell with invalid value!" MEOL);
            break;
        case 2: // duplicate value in a ROW
            sprintf(EndBuf(tb),"Found duplicated value in a ROW!" MEOL);
            break;
        case 3: // duplicate value in a COLUMN
            sprintf(EndBuf(tb),"Found duplicated value in a COLUMN!" MEOL);
            break;
        case 4: // duplicate value in a SQUARE
            sprintf(EndBuf(tb),"Found duplicated value in a BOX!" MEOL);
            break;
        default:
            sprintf(EndBuf(tb),"Found some uncase error! (%d)!" MEOL, c);
            break;
        }
        strcat(tb,get_last_box_error());
        strcat(tb,MEOL);
        strcat(tb,"Click [No] to abort" MEOL);
        strcat(tb,"Click [Yes] to redo" MEOL);
        //len = MB2(tb,"INVALID Sudoku");
        //if (len == IDYES)
        //    return FALSE; // stay and redo IMPORT
        //else
        //    return TRUE;  // ABORT import - close DIALOG
        len = Do_MsgBox_YN(tb,"INVALID Sudoku");
        if (!len)
            return FALSE; // stay and redo IMPORT
        else
            return TRUE;  // ABORT import - close DIALOG

    } else if (spots < g_iMinGiven) { // was 18, but now 17 - see : http://en.wikipedia.org/wiki/Suduko
        char *head;
        if (spots) {
            sprintf(tb,"Sudoku is valid, but only got %d spots?" MEOL, spots);
            head = "MINIMAL SPOTS";
            strcat(tb,"Click [Cancel] to abort" MEOL);
            strcat(tb,"Click [Yes] to continue with import" MEOL);
            strcat(tb,"Click [No] to redo import" MEOL);
            //len = MB3(tb,head);
            len = Do_MsgBox_YNC2(tb,head);
            if (len == IDYES) {
                // continue and use this imported Sudoku
            } else if (len == IDNO)
                return FALSE; // stay and REDO this import
            else
                return TRUE; // ABORT - close DIALOG
        } else {
            sprintf(tb,"Sudoku is valid, but BLANK! No Spots?" MEOL);
            head = "NO SPOTS";
            strcat(tb,"Click [No] to abort" MEOL);
            strcat(tb,"Click [Yes] to redo" MEOL);
            //len = MB2(tb,head);
            //if (len == IDYES)
            //    return FALSE; // stay an REDO import
            //else
            //    return TRUE;  // ABORT and close DIALOG
            len = Do_MsgBox_YN(tb,head);
            if (!len)
                return FALSE; // stay an REDO import
            else
                return TRUE;  // ABORT and close DIALOG
        }

    }

    // Use this IMPORTED box
    set_curr_box(pb2);
    Reset_Active_File(g_pSpecial);
    File_Reset();
    sprintf(tb,"Set Sudoku per import with %d spots.", spots );
    sprtf("%s\n",tb);

    return TRUE;
}

VOID Do_Import_Help(HWND hDlg)
{
    if (!hDlg) {
        if (!hImpDlg)
            return;
        hDlg = hImpDlg;
    }
    char *tb = GetNxtBuf();
    int len = GetDlgItemText(hDlg,IDC_EDIT1,tb,256);
    sprintf(tb,"Got %d chars", len);
    SetDlgItemText(hDlg,IDC_EDIT2,tb);
}


INT_PTR Do_INIT_Import(HWND hDlg)
{
    char *tb = curr_sudo;
    PABOX2 pb = get_curr_box();
    int row, col, val;
    *tb = 0;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            sprintf(EndBuf(tb),"%d",val);
        }
    }

    SetDlgItemText(hDlg, IDC_EDIT1, tb);
    
    //SendMessage(GetDlgItem(hDlg,IDC_EDIT1), EM_SETSEL, 0, EOF);
    //SendDlgItemMessage(hDlg, IDC_EDIT1, WM_SETFOCUS, NULL, NULL);
    CenterWindow(hDlg, g_hWnd);
    hImpDlg = hDlg;
    return (INT_PTR)TRUE;
}

INT_PTR CALLBACK ImportProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_Import(hDlg);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            if (LOWORD(wParam) == IDOK) {
                if (!Do_Import_OK(hDlg))
        			return (INT_PTR)TRUE;
            }
            hImpDlg = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		} else {
            // The wParam parameter specifies the identifier of the edit control, and the lParam parameter 
            // specifies the handle to the edit control in the low-order word and 
            // the notification code in the high-order word.
            if (wParam == IDC_EDIT1) {
                // want the CHANGE event
                if (HIWORD(wParam) == EN_CHANGE) {
                    Do_Import_Help(hDlg);
                }
            }
        }
		break;
	}
	return (INT_PTR)FALSE;
}

VOID Do_ID_FILE_IMPORT(HWND hWnd)
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_IMPORT), hWnd, ImportProc);
    if (res == IDOK) {

    }
}

// =====================================================================
// =====================================================================
// Debug Options - ID_OPTIONS_DEBUGOPTIONS
INT_PTR CALLBACK DebugOptionsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT cmd = LOWORD(wParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_DebugOptions(hDlg);
	case WM_COMMAND:
        switch (cmd)
        {
        case IDOK:
        case IDCANCEL:
            {
                if (cmd == IDOK) {
                    if (!Do_DebugOptions_OK(hDlg))
                        return (INT_PTR)TRUE;
                }
			    EndDialog(hDlg, cmd);
            }
			return (INT_PTR)TRUE;
        case IDC_BUTTON1:
            Do_ALL_Dbg_ON(hDlg);
            break;
        case IDC_BUTTON2:
            Do_ALL_Dbg_OFF(hDlg);
            break;
        }
		break;
	}
	return (INT_PTR)FALSE;
}

VOID Do_ID_OPTIONS_DEBUGOPTIONS(HWND hWnd)
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_DEBUG), hWnd, DebugOptionsProc);
    if (res == IDOK) {

    }
}

// ====================================================================
// ID_OPTIONS_ENABLESTRATEGIES - IDD_STRATEGIES
BOOL Do_INIT_Strategies(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    while (plist->pcaption) {
        if (plist->penab) {
            CheckDlgButton(hDlg, plist->conrol,
                *plist->penab ? BST_CHECKED : BST_UNCHECKED);
        }
        plist++;
    }
    CenterWindow(hDlg, g_hWnd);
    return TRUE;
}

BOOL Do_Strategies_OK(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    UINT res;
    char *ponoff;

    while (plist->pcaption) {
        if (plist->penab) {
            res = IsDlgButtonChecked(hDlg, (int)plist->conrol);
            if (res == BST_CHECKED)
                ponoff = "ON";
            else
                ponoff = "OFF";
            sprtf("STRATEGY: [%s] is %s\n", plist->pcaption, ponoff);
            Toggle_CheckBox_Bool( res, plist->penab, plist->pchgd );
        }
        plist++;
    }
    return TRUE;
}

VOID Do_ALL_Strat_ON(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    while (plist->pcaption) {
        if (plist->penab)
            CheckDlgButton(hDlg, plist->conrol, BST_CHECKED);
        plist++;
    }
}
VOID Do_ALL_Strat_OFF(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    while (plist->pcaption) {
        if (plist->penab)
            CheckDlgButton(hDlg, plist->conrol, BST_UNCHECKED);
        plist++;
    }
}


INT_PTR CALLBACK StrategiesProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT cmd = LOWORD(wParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_Strategies(hDlg);
	case WM_COMMAND:
        switch (cmd)
        {
        case IDOK:
        case IDCANCEL:
            {
                if (cmd == IDOK) {
                    if (!Do_Strategies_OK(hDlg))
                        return (INT_PTR)TRUE;
                }
			    EndDialog(hDlg, cmd);
            }
			return (INT_PTR)TRUE;
        case IDC_BUTTON1:
            Do_ALL_Strat_ON(hDlg);
            break;
        case IDC_BUTTON2:
            Do_ALL_Strat_OFF(hDlg);
            break;
        }
		break;
	}
	return (INT_PTR)FALSE;
}

VOID Do_ID_OPTIONS_ENABLESTRATEGIES(HWND hWnd)
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_STRATEGIES), hWnd, StrategiesProc);
    if (res == IDOK) {
        // all done...
    }
}

// ====================================================================
// ID_OPTIONS_MISCOPTIONS - IDD_MISCOPTIONS
BOOL Do_INIT_MiscOptions(HWND hDlg)
{
    char *tb = double_to_stg(g_AutoDelay);
    SetDlgItemText(hDlg,IDC_EDIT1,tb);

    CenterWindow(hDlg, g_hWnd);
    return TRUE;
}
BOOL Do_MiscOptions_OK(HWND hDlg)
{
    char *tb = GetNxtBuf();
    *tb = 0;
    int len = GetDlgItemText(hDlg,IDC_EDIT1,tb,264);
    double d = atof(tb);
    if (d >= 0.0) {
        if ((d > g_AutoDelay) || (d < g_AutoDelay)) {
            sprtf("Changed AutoDelay from %s to %s seconds\n", double_to_stg(g_AutoDelay), double_to_stg(d) );
            set_repaint2();
            g_AutoDelay = d;
            gChgASD = TRUE;
        }
    }

    return TRUE;
}
INT_PTR CALLBACK MiscOptionsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT cmd = LOWORD(wParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_MiscOptions(hDlg);
	case WM_COMMAND:
        switch (cmd)
        {
        case IDOK:
        case IDCANCEL:
            {
                if (cmd == IDOK) {
                    if (!Do_MiscOptions_OK(hDlg))
                        return (INT_PTR)TRUE;
                }
			    EndDialog(hDlg, cmd);
            }
			return (INT_PTR)TRUE;
        //case IDC_BUTTON1:
        //    Do_ALL_Strat_ON(hDlg);
        //    break;
        //case IDC_BUTTON2:
        //    Do_ALL_Strat_OFF(hDlg);
        //    break;
        }
		break;
	}
	return (INT_PTR)FALSE;
}


VOID Do_ID_OPTIONS_MISCOPTIONS(HWND hWnd)
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_MISCOPTIONS), hWnd, MiscOptionsProc);
    if (res == IDOK) {
        // all done...
    }
}

//////////////////////////////////////////////////////
#else // !#ifdef WIN32 - TODO: Need alternative dialogs
//////////////////////////////////////////////////////
VOID Do_ID_OPTIONS_ENABLESTRATEGIES(HWND hWnd)
{
    sprtf("Do_ID_OPTIONS_ENABLESTRATEGIES not yet implemented in unix!\n");
}

VOID Do_ID_OPTIONS_MISCOPTIONS(HWND hWnd)
{
    sprtf("Do_ID_OPTIONS_MISCOPTIONS not yet implemented in unix!\n");
}

//////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n - option dialogs
//////////////////////////////////////////////////////

// ====================================================================
// eof - Sudo_Dialogs.cxx

