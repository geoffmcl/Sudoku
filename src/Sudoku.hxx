/*\
 * Sudoku.hxx  : Main header for the application.
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
// Sudoku.hxx
#ifndef _SUDOKU_HXX_
#define _SUDOKU_HXX_

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "targetver.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include "resource.h"
// #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#endif

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <vector>
#ifdef WIN32
#include <tchar.h>
#include "unistd.h"
#else
#include <unistd.h>
#endif

using namespace std;

// Cell Flags - 64-bits
#define cf_URF      0x0000000000000001
#define cf_UCF      0x0000000000000002
#define cf_UBF      0x0000000000000004
#define cf_UNIQ     (cf_URF | cf_UCF | cf_UBF) // combined

// Candidate flag - only valid when there is a candidate
#define cf_NP       0x0000000000000008  // Is a NAKED PAIR
#define cf_NPE      0x0000000000000010  // eliminated by a NAKED PAIR

#define cf_HP       0x0000000000000020  // Is a 'Hidden' pair
#define cf_HPE      0x0000000000000040  // eliminated by a HIDDEN PAIR

#define cf_HT       0x0000000000000080  // Is a 'Hidden' pair
#define cf_HTE      0x0000000000000100  // eliminated by a HIDDEN TRIPLE

#define cf_XW       0x0000000000000200  // four corners of X-Wing rect
#define cf_XWE      0x0000000000000400  // candidates eliminated by X-Wing scan
#define cf_XWR      0x0000000000000800  // X-Wings on ROW basis
#define cf_XWC      0x0000000000001000  // X-Wings on COL basis

#define cf_PP       0x0000000000004000  // is a member of the pointed pair club
#define cf_PPE      0x0000000000008000  // candidate eliminated by pointed pair

#define cf_YW       0x0000000000010000  // is a member of the Y-Wing club
#define cf_YWE      0x0000000000020000  // candidate eliminated by Y-Wing
#define cf_YWC      0x0000000000040000  // is a member of the Y-Wing set

#define cf_CC       0x0000000000080000  // is member of color club
#define cf_CCE      0x0000000000100000  // candidate eliminated by color scan

#define cf_UR       0x0000000000200000  // is member of unique rectangle club
#define cf_URE      0x0000000000400000  // candidate eliminated by unique rectangle scan

#define cf_NT       0x0000000000800000  // Is a NAKED TRIPLE
#define cf_NTE      0x0000000001000000  // eliminated by a NAKED TRIPLE

#define cf_SS       0x0000000002000000  // PRESENTLY NOT USED
#define cf_SSE      0x0000000004000000  // Eliminated by Row, Column or Box (Region) search

#define cf_XC       0x0000000010000000  // X-Cycles member
#define cf_XCE      0x0000000020000000  // X-Cycles candidate elimination
#define cf_XCA      0x0000000040000000  // X-Cycles candidate COLOR A
#define cf_XCB      0x0000000080000000  // X-Cycles candidate COLOR B
#define cf_XAB      (cf_XCA | cf_XCB)   // combined COLOR flag
#define FLIP_COLOR(a) ( a & cf_XCA ) ? cf_XCB : cf_XCA

#define cf_LK       0x0000000100000000  // used to eliminate Locked values
#define cf_LKE      0x0000000200000000  // eliminate by Locked values

#define cf_XY       0x0000000400000000  // XY-Chain member
#define cf_XYE      0x0000000800000000  // XY-Chain candidate elimination
// for cell coloring, use cf_XCA and cf_XCB

#define cf_SL       0x0000001000000000  // Strong Link member
#define cf_SLE      0x0000002000000000  // Eliminated by Strong Links
// for cell coloring, use cf_XCA and cf_XCB


#define cf_ELIM (cf_NPE|cf_HPE|cf_HTE|cf_LKE|cf_XWE|cf_SSE|cf_PPE|cf_YWE|cf_CCE|cf_URE|cf_NTE|cf_XCE|cf_XYE|cf_SLE)
#define cf_PRIM (cf_NP |cf_HP |cf_HT |cf_LK |cf_XW |cf_SS |cf_PP |cf_YW |cf_CC |cf_UR |cf_NT |cf_XC |cf_XY |cf_SL )

// ====================================
// Cell Flags
// start at the OTHER end of the scale for a uint64_t ==  unsigned __int64 
#define cl_CLR4     0x0008000000000000
#define cl_CLR3     0x0004000000000000
#define cl_CLR2     0x0002000000000000
#define cl_CLR1     0x0001000000000000
#define cl_CLRS (cl_CLR1|cl_CLR2|cl_CLR3|cl_CLR4)

// LINK by BOX, COL, ROW, SAMECELL
#define cl_LBB      0x0080000000000000
#define cl_LBC      0x0040000000000000
#define cl_LBR      0x0020000000000000
#define cl_LSS      0x0010000000000000  // is SAME CELL
#define cl_LBA  (cl_LBB|cl_LBC|cl_LBR|cl_LSS)

// strong LINKS between individual candidates
#define cl_SLB      0x0400000000000000
#define cl_SLC      0x0200000000000000
#define cl_SLR      0x0100000000000000
#define cl_SLA  (cl_SLB | cl_SLC | cl_SLR) // strong link Any or All

#define cl_New      0x8000000000000000
#define cl_CSC      0x4000000000000000  // cell Containing Selected Candidate ie equals current SELECTION
#define cl_Bad      0x2000000000000000

#define cl_MCC      0x1000000000000000  // cell is Member of the Color Club
#define cl_UNR      0x0800000000000000  // Cell is Member of Unique Rectange set (of 4)



// flags to keep
#define cl_Perm    (cl_New | cl_CSC | cl_Bad)
#define cl_Others  (cl_MCC | cl_UNR | cl_SLA | cl_LBA)

typedef struct tagASET {
    int val[9];     // value when valid, zero otherwise
    uint64_t flag[9]; // state flag for EVERY potential value
    uint64_t cellflg; // general flag for cell
    time_t tm;
    int uval;       // unique value, if any
}SET, *PSET;

typedef struct tagALINE2 {
    int val[9]; // value when known, zero otherwise
    SET set[9]; // potential value, if zero
}ALINE2, *PALINE2;

#define bf_DnTest   0x0000000000000001  // have run SOLVER
#define bf_Unique   0x0000000000000002  // is UNIQUE solution

typedef struct tagABOX2 {
    int iStage;
    uint64_t bflag;
    ALINE2 line[9];  // just nine lines
}ABOX2, *PABOX2;

typedef struct tagROWCOL {
    int row, col, box;   // absolute row/col offset
    int cnum;
    SET set;        // set of value for this sudo
}ROWCOL, *PROWCOL;

typedef struct tagPAIR {
    int row1,col1,val1;
    int row2,col2,val2;
}PAIR, *PPAIR;

typedef vector<ROWCOL> vRC;
typedef vRC::iterator vRCi;
typedef vector<PAIR> vPAIR;
typedef vPAIR::iterator vPAIRi;

typedef struct tagRCSQUARE {
    ROWCOL rowcol[9];
}RCSQUARE, *PRCSQUARE;

typedef struct tagRCQUAD {
    SET set;
    ROWCOL rowcol[4];
}RCQUAD, *PRCQUAD;


typedef struct tagRCPAIR {
    ROWCOL rowcol[2];
}RCPAIR, *PRCPAIR;

// rectangle == 4 pairs
typedef struct tagRCRECT {
    SET set;
    RCPAIR rcpair[4];
}RCRECT, *PRCRECT;

// A simple 9x9 grid, to run 'test' - trial and error - solutions
typedef struct tagGRID {
    int v[9][9];
}GRID, *PGRID;
typedef vector<GRID> vGRID;
typedef vGRID::iterator vGRIDi;

typedef vector<int> vINT;
typedef vector<RCPAIR> vRCP;
typedef vector<RCRECT> vRCR;

typedef struct tagRCRCB {
    vRC vrows[9];
    vRC vcols[9];
    vRC vboxs[9];
}RCRCB, *PRCRCB;

#define ISDIGIT(a) (( a >= '0' )&&( a <= '9'))
#define VFH(a)  ( a && ( a != INVALID_HANDLE_VALUE ))

extern HWND g_hWnd;         // parent handle
extern HINSTANCE hInst;		// current instance
extern TCHAR szTitle[]; 	// The title bar text

#define  MB(a) MessageBox( g_hWnd, a, "CRITICAL ERROR", MB_OK | MB_ICONINFORMATION )
#define  MB2(a,b) MessageBox( g_hWnd, a, b, MB_ICONINFORMATION | MB_YESNO )
#define  MB3(a,b) MessageBox( g_hWnd, a, b, MB_ICONINFORMATION | MB_YESNOCANCEL )

extern BOOL g_bAShiftDown, g_bCtrlDown, g_bAltDown, g_bLBDown, g_bChanged, g_bShowHints, gChgHint;
extern BOOL g_bDoneInit, g_bComCtl6;

extern void Post_Command( WPARAM cmd );

extern void Do_WM_CHAR( HWND hWnd, WPARAM wParam );

extern HWND g_hProgress, g_hListBox, g_hMsgDialog;

extern int Get_Spot_Count();
extern int Get_Spots(PABOX2 pb);

extern RECT g_sRect;
extern BOOL res_scn_rect;

// local header files
#include "sprtf.hxx"
#include "Sudo_File.hxx"
#include "Sudo_Paint.hxx"
#include "Sudo_Ini.hxx"
#include "Sudo_Menu.hxx"
#include "Sudo_Timer.hxx"
#include "Sudo_Logic.hxx"
#include "Sudo_List.hxx"
#include "Sudo_Utils.hxx"
#include "Sudo_Strat.hxx"
#include "Sudo_Dialogs.hxx"
#include "Sudo_Thread.hxx"
#include "Sudo_Change.hxx"
#include "Sudo_Color2.hxx"
#include "Sudo_BMP.hxx"
#include "Sudo_Print.hxx"
#include "Sudo_Debug.hxx"
#include "Sudo_Keys.hxx"
#include "Sudo_Locked.hxx"
#include "Sudo_NakedPTQ.hxx"
#include "Sudo_XCycles.hxx"
#include "Sudo_XYChain.hxx"
#include "Sudo_SLinks.hxx"
#include "Sudo_SLink2.hxx"
#include "Sudo_SLink3.hxx"
#include "Sudo_XCycle3.hxx"
#include "Sudo_URect.hxx"
#include "Sudo_URect2.hxx"
#include "Sudo_PntColors.hxx"
#include "Sudo_Scraps.hxx"
#include "Sudo_XBrowse.hxx"
#include "Sudo_Color.hxx"
#include "Sudo_AIC.hxx"
#include "Sudo_SET.hxx"
#include "Sudo_XYZWings.hxx"
#include "Sudo_Paint.hxx"

#ifndef MEOL
#define MEOL "\r\n"
#endif

#endif // #ifndef _SUDOKU_HXX_
// eof - Sudoku.hxx

