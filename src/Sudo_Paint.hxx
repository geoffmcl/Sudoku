// Sudo_Paint.hxx
#ifndef _SUDO_PAINT_HXX_
#define _SUDO_PAINT_HXX_
#include <vector>

using namespace std;

#define VALIDRC(r,c) ((r >= 0) && (r < 9) && (c >=0 ) && (c < 9))

extern VOID Do_WM_PAINT( HWND hWnd );
extern void Add_String( char * ps );
extern void set_repaint( bool erase = true );
extern void set_repaint2( bool erase = false );
extern void Do_WM_MOUSEMOVE( HWND hWnd, WPARAM wParam, LPARAM lParam );
extern void clean_up_paint_tools(void);
extern void Paint_Mouse_Pos(HWND hWnd);
extern VOID Do_WM_SIZE( HWND hWnd, LPARAM lParam );

typedef vector<string> vSTG;
typedef vSTG::iterator vSTGi;

typedef vector<int> vINT;
typedef vINT::iterator vINTi;

extern RECT g_rcClient;
extern RECT g_rcHover;
extern RECT g_rcSudoku, g_rcSudokuLeg;
extern int g_hov_Row, g_hov_Col;

extern int curr_xPos, curr_yPos; // current mouse position
extern int one_value_count; // count of cells with only ONE choice, and NO current value
extern int NO_value_count;  // count of cells with no choice values, and NO current value - BAD BAD BAD
extern int total_empty_count;   // SUCCESS when ZERO ;=))
extern PROWCOL Get_First_Ones();

extern void Move_Selection(HWND hWnd, int row, int col);
extern void Move_Selection_Left(HWND hWnd);
extern void Move_Selection_Up(HWND hWnd);
extern void Move_Selection_Down(HWND hWnd);
extern void Move_Selection_Right(HWND hWnd);

extern int Check_For_Insertion();
extern void Clear_Cell_Flag( PABOX2 pb, uint64_t flag = cl_CSC );

extern HFONT Get_Font();

// Colors By Type
extern COLORREF crNewVal, crOrgVal, crBadVal;

extern bool Is_Selected_Valid();
extern void get_curr_selRC( int *pr, int *pc ); // get g_sel_Row, g_sel_Col;
extern PRECT get_curr_sel();
extern int g_sel_Row, g_sel_Col;

extern int Mark_HighLight_Cells(bool outlog = true);

//char szSelVal[] = "Selected_Value";
extern COLORREF crSelVal;
//BOOL gChgSVal = FALSE;

// char szPntChN[] = "Paint_CHain_Number";
extern BOOL g_bPntChN;  // paint 'chain_number' instead of sequence number
// BOOL g_bChgPChN = FALSE;

extern vSTG *Get_Screen_Lines(); // { return &vSLines; }

extern BOOL g_bCopyHDC; // if TRUE handle WM_ERASEBKGND 

// Move Paint_Colors(hdc) into Sudo_PntColors.cxx
extern HPEN Get_SLnk_Pen( void );
extern HPEN Get_WLnk_Pen( void );
extern void draw_link(HDC hdc, PROWCOL prc1, PROWCOL prc2, int setval, bool one = false);
extern HPEN Get_hpPink_2();
extern HBRUSH Get_Sel_Brush();
extern HBRUSH Get_Off_Brush();
extern HBRUSH Get_On_Brush();
extern HBRUSH Get_UnRect_Brush();
extern int ht_margin, vt_margin;
extern void draw_line( HDC hdc, int x1, int y1, int x2, int y2 );
extern void draw_line( HDC hdc, PPOINT a, PPOINT b );
extern PRECT get_rcSquares(int row, int col); // { return &rcSquares[row][col]; }

extern COLORREF crSLnk, crWLnk, crOddBox, crEvnBox, crElimBk, crElimFg, crOn, crOff;
extern COLORREF crClr1, crClr2, crClr3, crClr4;

#endif // #ifndef _SUDO_PAINT_HXX_
// eof - Sudo_Paint.hxx
