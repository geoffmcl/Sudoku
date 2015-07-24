// Sudo_Color.cxx
// Choosing PAINT colors for app

#include "Sudoku.hxx"

// extern COLORREF crSLnk, crWLnk, crOddBox, crEvnBox, crElimBk, crElimFg;
COLORREF dcrSLnk   = RGB(255,0,0);
COLORREF dcrWLnk   = RGB(0,0,255);

COLORREF dcrOddBox = RGB(250,250,250); // g_bOddBox - g_rcRegions[9] - g_bOddBox
COLORREF dcrEvnBox = RGB(255,255,255); // Standard background

COLORREF dcrElimBk = RGB(255,255,192);
COLORREF dcrElimFg = RGB(128,0,0);

COLORREF dcrOn = RGB(255,192,255);
COLORREF dcrOff = RGB(0xc0,255,255);
//char szOn[] = "On_Color";
//BOOL bChgOn = FALSE;
//char szOff[] = "Off_Color";
//BOOL bChgOff = FALSE;

COLORREF dcrClr1 = RGB(238,170,85);  // Light Brown
COLORREF dcrClr2 = RGB(170,255,170); // Light Green
COLORREF dcrClr3 = RGB(170,170,255); // Purple
COLORREF dcrClr4 = RGB(201,236,250); // Soft Blue



typedef struct tagCLRSTR {
    char *text;
    UINT uRed,uGrn,uBlue;
    UINT uSel,uDef;
    COLORREF *pcr;
    COLORREF *pdcr;
    PBOOL pb;
}CLRSTR, *PCLRSTR;

CLRSTR clrstr[] = {
    { "Eliminate candidate (Bk):",IDC_EDIT1,IDC_EDIT2,IDC_EDIT3,IDC_BUTTON1,IDC_BUTTON2,&crElimBk,&dcrElimBk,&bChgElimBk }, // 0
    { "Eliminate candidate (Fg):",IDC_EDIT4,IDC_EDIT5,IDC_EDIT6,IDC_BUTTON3,IDC_BUTTON4,&crElimFg,&dcrElimFg,&bChgElimFg }, // 1
    { "Strong Link Lines:",IDC_EDIT7,IDC_EDIT8,IDC_EDIT9,IDC_BUTTON5,IDC_BUTTON6,&crSLnk,&dcrSLnk,&bChgSlnk }, // 2
    { "Weak Link Lines:",IDC_EDIT10,IDC_EDIT11,IDC_EDIT12,IDC_BUTTON7,IDC_BUTTON8,&crWLnk,&dcrWLnk,&bChgWlnk }, // 3
    { "ON Background:",IDC_EDIT13,IDC_EDIT14,IDC_EDIT15,IDC_BUTTON9,IDC_BUTTON10,&crOn,&dcrOn,&bChgOn }, // 4
    { "OFF Background:",IDC_EDIT16,IDC_EDIT17,IDC_EDIT18,IDC_BUTTON11,IDC_BUTTON12,&crOff,&dcrOff,&bChgOff }, // 5
    { "Cell Color 1:",IDC_EDIT19,IDC_EDIT20,IDC_EDIT21,IDC_BUTTON13,IDC_BUTTON14,&crClr1,&dcrClr1,&bChgClr1 }, // 6
    { "Cell Color 2:",IDC_EDIT22,IDC_EDIT23,IDC_EDIT24,IDC_BUTTON15,IDC_BUTTON16,&crClr2,&dcrClr2,&bChgClr2 }, // 7
    { "Cell Color 3:",IDC_EDIT25,IDC_EDIT26,IDC_EDIT27,IDC_BUTTON17,IDC_BUTTON18,&crClr3,&dcrClr3,&bChgClr3 }, // 8
    { "Cell Color 4:",IDC_EDIT28,IDC_EDIT29,IDC_EDIT30,IDC_BUTTON19,IDC_BUTTON20,&crClr4,&dcrClr4,&bChgClr4 }, // 9

    // LAST in LIST
    { 0, 0, 0, 0, 0, 0 }
};

#define MX_TBL_OFF (sizeof(clrstr) / sizeof(CLRSTR)) - 1

// Standard MS ChooseColor function
/* ===============================

typedef struct {
  DWORD        lStructSize;
  HWND         hwndOwner;
  HWND         hInstance;
  COLORREF     rgbResult;
  COLORREF     *lpCustColors;
  DWORD        Flags;
  LPARAM       lCustData;
  LPCCHOOKPROC lpfnHook;
  LPCTSTR      lpTemplateName;
} CHOOSECOLOR, *LPCHOOSECOLOR;

  ================================ */

static CHOOSECOLOR cc;
static COLORREF crCustom[16] = {0};

BOOL ChoosNewColor( HWND hWnd, COLORREF crCurrent, COLORREF *pcrChoice )
{
    DWORD flag = CC_RGBINIT | CC_FULLOPEN;
    LPCHOOSECOLOR pcc = &cc;

    pcc->lStructSize    = sizeof(CHOOSECOLOR);
    pcc->hwndOwner      = hWnd;
    pcc->hInstance      = (HWND)hInst;
    pcc->rgbResult      = crCurrent;
    pcc->lpCustColors   = crCustom;
    pcc->Flags          = flag;
    pcc->lCustData      = 0;
    pcc->lpfnHook       = 0;
    pcc->lpTemplateName = 0;
#ifdef WIN32
    BOOL bRet = ChooseColor( pcc );
    if (bRet) {
        *pcrChoice = pcc->rgbResult;
    }

    return bRet;
#else // #ifdef WIN32
    sprtf("ChoosNewColor not yet implemented in unix\n");
    return 0;
#endif // WIN32 y.n
}

#ifdef WIN32
INT_PTR Do_INIT_SelectColor(HWND hDlg)
{
    PCLRSTR pcs = &clrstr[0];
    COLORREF *pcr;
    int r, g, b;
    while (pcs->text) {
        pcr = pcs->pcr;
        r = GetRValue(*pcr);
        g = GetGValue(*pcr);
        b = GetBValue(*pcr);
        SetDlgItemInt(hDlg,pcs->uRed,r,FALSE);
        SetDlgItemInt(hDlg,pcs->uGrn,g,FALSE);
        SetDlgItemInt(hDlg,pcs->uBlue,b,FALSE);

        pcs++;
    }
    return (INT_PTR)TRUE;
}

BOOL Do_SelectColor_OK(HWND hDlg)
{
    PCLRSTR pcs = &clrstr[0];
    COLORREF *pcr;
    COLORREF cr;
    BOOL trans = FALSE;
    int r, g, b;
    int rn, gn, bn;
    while (pcs->text) {
        // get TABLE value
        pcr = pcs->pcr;
        r = GetRValue(*pcr);
        g = GetGValue(*pcr);
        b = GetBValue(*pcr);
        // get EDIT values
        rn = GetDlgItemInt(hDlg,pcs->uRed,&trans,false) & 255;
        gn = GetDlgItemInt(hDlg,pcs->uGrn,&trans,false) & 255;
        bn = GetDlgItemInt(hDlg,pcs->uBlue,&trans,false) & 255;
        // has anything CHANGED
        if ((r != rn)||(b != bn)||(g != gn)) {
            cr = RGB(rn,gn,bn);
            *pcr = cr;  // update for PAINT, and INI save
            *pcs->pb = TRUE; // mark change
        }

        pcs++;
    }
    return TRUE;
}

VOID Select_Color_pcs(HWND hDlg, PCLRSTR pcs)
{
    COLORREF cr = *pcs->pcr;
    COLORREF crn = 0;
    BOOL trans;
    int rn, gn, bn;
    // get current EDIT values
    rn = GetDlgItemInt(hDlg,pcs->uRed,&trans,false) & 255;
    gn = GetDlgItemInt(hDlg,pcs->uGrn,&trans,false) & 255;
    bn = GetDlgItemInt(hDlg,pcs->uBlue,&trans,false) & 255;
    cr = RGB(rn,gn,bn);
    if ( ChoosNewColor( hDlg, cr, &crn ) ) {
        if (cr != crn) {
            int r,g,b;
            r = GetRValue(crn);
            g = GetGValue(crn);
            b = GetBValue(crn);
            // set EDIT values
            SetDlgItemInt(hDlg,pcs->uRed,r,FALSE);
            SetDlgItemInt(hDlg,pcs->uGrn,g,FALSE);
            SetDlgItemInt(hDlg,pcs->uBlue,b,FALSE);
        }
    }
}

VOID Reset_Color_pcs(HWND hDlg, PCLRSTR pcs)
{
    // get DEFAULT color ptr
    COLORREF crn = *pcs->pdcr;
    // extract RGB
    int r,g,b;
    r = GetRValue(crn);
    g = GetGValue(crn);
    b = GetBValue(crn);
    // set EDIT values
    SetDlgItemInt(hDlg,pcs->uRed,r,FALSE);
    SetDlgItemInt(hDlg,pcs->uGrn,g,FALSE);
    SetDlgItemInt(hDlg,pcs->uBlue,b,FALSE);
}

VOID Select_Elim_Color(HWND hDlg, int off)
{
    if (off < MX_TBL_OFF) {
        PCLRSTR pcs = &clrstr[off];
        Select_Color_pcs(hDlg,pcs);
    }
}

VOID Reset_Elim_Color(HWND hDlg, int off)
{
    if (off < MX_TBL_OFF) {
        PCLRSTR pcs = &clrstr[off];
        Reset_Color_pcs(hDlg,pcs);
    }
}


// Select Color Option - ID_OPTIONS_SELECTCOLORS
INT_PTR CALLBACK SelectColorProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT cmd = LOWORD(wParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_SelectColor(hDlg);
	case WM_COMMAND:
        switch (cmd)
        {
        case IDOK:
        case IDCANCEL:
            {
                if (cmd == IDOK) {
                    if (!Do_SelectColor_OK(hDlg))
                        return (INT_PTR)TRUE;
                }
			    EndDialog(hDlg, cmd);
            }
			return (INT_PTR)TRUE;
        case IDC_BUTTON1:
            Select_Elim_Color(hDlg,0);
            break;
        case IDC_BUTTON2:
            Reset_Elim_Color(hDlg,0);
            break;
        case IDC_BUTTON3:
            Select_Elim_Color(hDlg,1);
            break;
        case IDC_BUTTON4:
            Reset_Elim_Color(hDlg,1);
            break;
        case IDC_BUTTON5:
            Select_Elim_Color(hDlg,2);
            break;
        case IDC_BUTTON6:
            Reset_Elim_Color(hDlg,2);
            break;
        case IDC_BUTTON7:
            Select_Elim_Color(hDlg,3);
            break;
        case IDC_BUTTON8:
            Reset_Elim_Color(hDlg,3);
            break;
        case IDC_BUTTON9:
            Select_Elim_Color(hDlg,4);
            break;
        case IDC_BUTTON10:
            Reset_Elim_Color(hDlg,4);
            break;
        case IDC_BUTTON11:
            Select_Elim_Color(hDlg,5);
            break;
        case IDC_BUTTON12:
            Reset_Elim_Color(hDlg,5);
            break;
        }
		break;
	}
	return (INT_PTR)FALSE;
}


VOID Do_ID_OPTIONS_SELECTCOLORS(HWND hWnd)
{
    int res = DialogBox(hInst, MAKEINTRESOURCE(IDD_CHOOSECOLOR), hWnd, SelectColorProc);
    if (res == IDOK) {

    }


}

#else // #ifdef WIN32

VOID Do_ID_OPTIONS_SELECTCOLORS(HWND hWnd)
{
    sprtf("Select color not implmented in uxix\n");
}

#endif // #ifdef WIN32 y/n



// eof - Sudo_Color.cxx
