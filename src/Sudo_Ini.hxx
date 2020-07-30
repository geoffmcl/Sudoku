// Sudo_Ini.hxx
#ifndef _SUDO_INI_HXX
#define _SUDO_INI_HXX
#ifdef WIN32    // Windows.h include
#include <Windows.h>
#endif

#define  it_None        0     // end of list
#define  it_Version     1
#define  it_String      2
#define  it_Int         3
#define  it_Bool        4
#define  it_WinSize     5     // special WINDOWPLACEMENT
#define  it_Rect        6
#define  it_Color       7
#define  it_Double      8
#define  it_Files       9   // vector<string> if files

typedef double (*GS_DOUBLE) (void);

typedef struct	tagINILST {	/* i */
	char *	i_Sect;
	char *	i_Item;
	int	   i_Type;
	char *	i_Deft;
	int *	   i_Chg;
	void *   i_Void;
	GS_DOUBLE   gs_double;
} INILST, * PINILST;

#define  SUDO_VERS_DATE "2020-07-15"
// SUDO_VERS_DATE "2012-08-18"

extern void ReadINI( void );
extern void WriteINI( void );
extern int  IsYes( char * pb );
extern int  IsNo( char * pb );
extern VOID UpdateWP( HWND hwnd );
extern VOID UpdateWP2( HWND hwnd );
extern void Add_to_INI_File_List( char * pfile );
extern DWORD Add_INI_Files_to_Menu();
extern char *Get_First_INI_File();
extern char *Get_INI_File( int off );
extern void Remove_INI_File( char *pf );

extern PINILST Find_an_Options(const char* popt);


#define  m_atoi   atoi
#define  m_sscanf sscanf

// char szALoad[] = "AutoLoad";
extern BOOL g_bAutoLoad, g_bALChg;

// char szGOut[] = "OutSaved";
extern BOOL  gbGotWP;
extern WINDOWPLACEMENT g_sWP;

// char szWin2[] = "Window2";
// char szGOut2[] = "OutSaved2";
extern BOOL  gbGotWP2;
extern WINDOWPLACEMENT g_sWP2;
extern BOOL bChgWP2;


// char szSpots[] = "DefaultSpots";
extern int g_iMinGiven;
extern BOOL gChgSpot;

// char szTmpSvOp[] = "TempSaveOptionBits";
extern DWORD g_dwSvOptBits;
extern BOOL gChgTSO;

// char szTmpSvFl[] = "TempSaveFile";
extern char g_szSvOptFl[]; // = {"\0"};
extern BOOL gChgTSF;

// char szATab[] = "AutoTab";
extern BOOL g_bAutoTab;  // on entry of a number, skip to next cell
extern BOOL g_bChgAT;

// char szWarnOBV[] = "Warn_On_Bad_Value";
extern BOOL g_bWarnOBV; // Entered a NON-VALID number from keyboard
extern BOOL g_bChgOBV;

//char szShwSel[] = "Show_Selection"; // When a cell with val selected, color all empty with this canadidate
extern BOOL g_bShwSel, g_bChgSS;

// char szMrkNM[] = "mark_non_color_members";
extern BOOL g_bMrkNM;   // also mark NON-members if ONE setval used
extern BOOL gChgMNM;

//char szSelVal[] = "Selected_Value";
//extern COLORREF crSelVal;
extern BOOL gChgSVal;

//char szPntChN[] = "Paint_Chain_Number";
// BOOL g_bPntChN = TRUE;  // paint 'chain_number' instead of sequence number
extern BOOL g_bChgPChN;

//char szShwSLnks[] = "Show_Strong_Links";
extern BOOL g_bShwStLnks;
extern BOOL gChgStLnks;

// char szShwMsgD[] = "Show_Message_Dialog";
extern BOOL g_bShwMsgD;
extern BOOL gChgSMsgD;

// char szASV[] = "All_Candidates";
// BOOL g_bAllSL = TRUE;
extern BOOL gChgASV;

//char szCLST[] = "Candidate_List";
extern char g_szCandList[]; // = { "123456789" };
extern BOOL gChgCLST;

// char szIWL[] = "Include_Weak_Links";
extern BOOL g_bIWL, gChgIWL;

// char szIBL[] = "Include_Box_Links";
extern BOOL g_bIBL, gChgIBL;

// char szIASW[] = "Alternating_Strong_Weak";
extern BOOL g_bASW, gChgASW;

//char szExChn[] = "Extend_Link_Chains";
extern BOOL g_bExtChn, gChgELC;

//char szLIndOpn[] = "LastOpnFilterIndex";
extern int g_iLIndOpn; // 0=custom-filter, else 1, 2, ... for each pair in filter
extern BOOL gChgLIO;

//char szLPath[] = "LastPath";
extern char g_szLastPath[]; // LAST PATH
extern BOOL gChgLPath;

// char szElimBk[] = "Eliminate_Candidate_Bkg";
// COLORREF crElimBk;
extern BOOL bChgElimBk;
// char szElimFg[] = "Eliminate_Candidate_Fg";
// COLORREF crElimFg;
extern BOOL bChgElimFg;
//char szSLnk[] = "Strong_Links";
// COLORREF crSLnk;
extern BOOL bChgSlnk;
//char szWLnk[] = "Weak_Links";
// COLORREF crWLnk;
extern BOOL bChgWlnk;
//char szOn[] = "On_Color";
// COLORREF crOn;
extern BOOL bChgOn;
// char szOff[] = "Off_Color";
// COLORREF crOff;
extern BOOL bChgOff;

// char szClr1[] = "Cell_Color_1";
// COLORREF crClr1;
extern BOOL bChgClr1;
// char szClr2[] = "Cell_Color_2";
// COLORREF crClr2;
extern BOOL bChgClr2;
// char szClr3[] = "Cell_Color_3";
// COLORREF crClr3;
extern BOOL bChgClr3;
// char szClr4[] = "Cell_Color_4";
// COLORREF crClr4;
extern BOOL bChgClr4;

// char szASD[] = "Auto_Solve_Delay_Seconds_as_float";
// extern double g_AutoDelay;
extern BOOL gChgASD;

extern char * GetINIFile(void); // get DEFAULT name of INI file in use

extern bool Get_LocalData_Path( char *path ); // WIN32=%LOCALAPPDATA%\sudoku, unix=$HOME/.config/sudoku, in buf with trailing path sep

#endif // #ifndef _SUDO_INI_HXX
// eof - Sudo_Ini.hxx

