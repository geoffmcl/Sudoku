// Sudo_Paint.cxx
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma comment ( lib, "Msimg32" )
#include <vector>
#include "Sudoku.hxx"
#ifdef WIN32
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Sudo_Paint.hxx"
#include "sprtf.hxx"
#include "Sudo_File.hxx"

using namespace std;

//#define ADD_TEXT_BLOCK  // add text to right side
//#undef ADD_TEXT_BLOCK

#ifndef MAX_STRING
#define MAX_STRING 512
#endif

#define OUTITPFR(tb) if (add_debug_pfr) OUTIT(tb)

BOOL g_bLineSB = FALSE;
BOOL g_bSkipAll = TRUE; // when adding mark to cell with selected candidate, SKIP cells with ALL on
BOOL g_bPntCtr = FALSE; // just center to center or from candidate to candidate - this seem best
BOOL g_bPntSeq = TRUE;  // add sequence or chain_number to square
BOOL g_bPntChN = TRUE;  // paint 'chain_number' instead of sequence number
BOOL g_bPntChns = TRUE; // join only those with same 'chain_number' and same Row, Col, or Box

BOOL g_bOddBox = TRUE;  // Fill (logical) Odd Boxes (Regions) with another color crOddBox

BOOL g_bUseChNum = FALSE; // for on_xcycles_test, paint chain number instead of candidate

BOOL g_bDbgPntRects = FALSE; // Just to PAINT rectangle ONLY

BOOL g_bCopyHDC = TRUE;

// NOT USED HERE!
typedef struct tagPPSET {
   int done_init;
   HDC hdc;
   PRECT prc;
   COLORREF cr;
   RECT mm;
   HPEN  hp, hpOld;
   HBRUSH hb, hbOld;
}PPSET, * PPPSET;


// forward reference
void Paint_Box( HDC hdc );
void get_text_size_len_hdc( PSIZE psz, char * ps, int len, HDC hdc );
void Paint_Selected( HDC hdc );
HBRUSH Get_Solid_Brush( COLORREF cr ); // { return CreateSolidBrush(cr); }
void Clear_BkBr();

static PPSET _s_act_set;
PPPSET get_act_paint_set(void) { return &_s_act_set; }

// current mouse HOVERING
RECT g_rcHover = { 0, 0, 0, 0 };
int g_hov_Row, g_hov_Col;
// current SELECTED cell
static RECT g_rcSelected = { 0, 0, 0, 0 };
int g_sel_Row, g_sel_Col;

PRECT get_curr_sel() { return &g_rcSelected; }
void get_curr_selRC( int *pr, int *pc ) { *pr = g_sel_Row; *pc = g_sel_Col; }
void set_curr_selRC( int row, int col ) { g_sel_Row = row; g_sel_Col = col; }

// BOOL g_bShwSel in INI - was int highlight_cells = 1;    // if cell with value is SELECTED, mark ALL blank cells with that candidate value

int curr_HRemainder = -1;
int curr_VRemainder = -1;
SET g_hSet;
SET g_vSet;

#ifdef ADD_MOUSE_RECT
RECT g_rcMouse;
#endif // #ifdef ADD_MOUSE_RECT

int clear_hover = 1;    // if mouse pos NOT found, still clear last hover
int add_bottom_legend = 1; // if enough space, add legend also to bottom

#ifdef ADD_VERT_DIVIDE
//int d_x1,d_y1,d_x2,d_y2;
RECT g_rcVertDiv;
int vert_div_width = 2; // two pixel divide
#endif // #ifdef ADD_VERT_DIVIDE

int sq_x1, sq_y1;
int sq_x2, sq_y2;
// int sq_x3, sq_y3;
int sq_x4, sq_y4;
//int g_hl_margin = 30;
int g_hr_margin = 10;
//int g_vt_margin = 40;
int g_vb_margin = 10;

int g_leg_height = 40;  // should be set to height of text being painted
int g_leg_width  = 30;  // set duing Set_SIZES()

// sq_x1,sq_y1       sq_x4,sq_y4
// sq_x2,sq_y2       sq_x3,sq_y3
int sudo_x, sudo_y; // units for ONE box
int min_width = 20;
int min_height = 20;

RECT g_rcFullClient;    // this is a FULL client rectangle

RECT g_rcClient;        // same or lesser rectange
RECT rcSquares[9][9];
RECT g_rcSudoku;    // Sudoku rect
RECT g_rcSudokuLeg;    // Sudoku rect + legend text
RECT g_rcRegions[9];    // each REGION (BOX)

int add_Legend_rect = 1;    // add rectangle around this legend text (for diagnostics)
RECT g_rcHLegT[9];  // legend across top - 1-9
RECT g_rcHLegB[9];  // legend across bottom - 1-9
RECT g_rcVLegL[9]; // left side vertical legend rect - A-I
RECT g_rcVLegR[9]; // right side vertical legend rect - 1-9

enum tagCorners {
    cnr_TL = 0,     // top left
    cnr_TR = 1,     // top right
    cnr_BL = 2,     // bottom left
    cnr_BR = 3,     // bottom right
    cnr_CNT = 4
};
RECT g_rcCorners[cnr_CNT]; // top left, top right, bottom left and bottom right corners

#ifdef ADD_LISTVIEW
RECT g_rcListView;
#endif // ADD_LISTVIEW
#ifdef ADD_TEXT_BLOCK
RECT g_rcText;      // output text block
#endif // #ifdef ADD_TEXT_BLOCK


#ifdef ADD_HINT_TEXT
// TODO - These should be calulated on the text size
//int g_hint_wdef = 200;  // has to contain |123456789|
//int g_hint_hdef = 120;  // has to contain 3 rows of |XXX|
// These should be CALCULATED
int g_hint_width = 230;  // has to contain |123456789|
// has to contain |123|
//                |456|
//                |789|
int g_hint_height = 120;

#define MCA_SZ 16
char cH1[MCA_SZ], cH2[MCA_SZ], cH3[MCA_SZ], cH4[MCA_SZ], cH5[MCA_SZ], cH6[MCA_SZ], cH7[MCA_SZ], cH8[MCA_SZ], cH9[MCA_SZ];
char cV1[MCA_SZ], cV2[MCA_SZ], cV3[MCA_SZ], cV4[MCA_SZ], cV5[MCA_SZ], cV6[MCA_SZ], cV7[MCA_SZ], cV8[MCA_SZ], cV9[MCA_SZ];
// ROWS
char *cpHWork[9] = { cH1, cH2, cH3, cH4, cH5, cH6, cH7, cH8, cH9 };
// COLUMNS
char *cpVWork[9] = { cV1, cV2, cV3, cV4, cV5, cV6, cV7, cV8, cV9 };
int paint_hint_rectangles = 1;
int try_higher_horiz = 1;
RECT rcHHint[9]; // ROW 0-9 - horiz hint text
int try_wider_verts = 1;
RECT rcVHint[9];    // rect for painting hints in a column across bottom
#endif // #ifdef ADD_HINT_TEXT

int done_paint = 0;
int try_cn18_font = 1;
int curr_xPos, curr_yPos; // current mouse position

//typedef vector<string> vSTG;
//typedef vSTG::iterator vSTGi;

vSTG vSLines;

vSTG * Get_Screen_Lines() { return &vSLines; }

PRECT get_rcSquares(int row, int col) { return &rcSquares[row][col]; }

COLORREF crBlack = RGB(0,0,0);
COLORREF crWhite = RGB(255,255,255);
COLORREF crRed = RGB(255,0,0);
COLORREF crGreen = RGB(0,255,0);
COLORREF crBlue = RGB(0,0,255);
COLORREF crYellow = RGB(255,255,0);
COLORREF crPink = RGB(255,0,255);
COLORREF crCyan = RGB(0,255,255);

//COLORREF crDRed = RGB(128,0,0);
COLORREF crDBlue = RGB(0,0,128);
COLORREF crDGreen = RGB(0,128,0);

COLORREF crGrid = RGB(90,90,90);
COLORREF crBox = RGB(190,190,190);
COLORREF crGray = RGB(90,90,90);
COLORREF crText = RGB(90,90,90);
COLORREF crLGray = RGB(190,190,190);
COLORREF crCntr = RGB(255,0,0);
COLORREF crTop = RGB(255,0,0);
COLORREF crPurp = RGB(128,0,255);
COLORREF crLBrown = RGB(192,128,32);
COLORREF crLBrown2 = RGB(217,202,167);
COLORREF crLGreen = RGB(144,238,144);
//COLORREF crDBlue = RGB(0,0,128);
COLORREF crLBlue = RGB(194,209,244); // cf_XC
//COLORREF crPBlue = RGB(200,200,255); // good for square background - highlight_cells
//COLORREF crPBlue = RGB(0x60,0xC0,255); // good for square background - highlight_cells
COLORREF crPBlue = RGB(0xc0,255,255); // good for square background - highlight_cells
//COLORREF crLYellow = RGB(255,255,192);
COLORREF crBYellow = RGB(248,252,121);  // Unique Rectangle
COLORREF crLOrange = RGB(255,192,255); // = RGB(0xff,0xc0,0x60); // also square background
COLORREF crLOrange2 = RGB(255,238,221);
COLORREF crLPink = RGB(255,192,255);

// Colors By Type
COLORREF crNewVal = RGB(128,0,0);
COLORREF crOrgVal = RGB(0,0,128);
COLORREF crBadVal = RGB(128,0,0);
COLORREF crSelVal = RGB(0xc0,255,255); // good for square background - highlight_cells

COLORREF crSLnk   = RGB(255,0,0);
COLORREF crWLnk   = RGB(0,0,255);

COLORREF crOddBox = RGB(250,250,250); // g_bOddBox - g_rcRegions[9] - g_bOddBox
COLORREF crEvnBox = RGB(255,255,255); // Standard background

COLORREF crElimBk = RGB(255,255,192);
COLORREF crElimFg = RGB(128,0,0);

COLORREF crOn = RGB(255,192,255);
COLORREF crOff = RGB(0xc0,255,255);

COLORREF crClr1 = RGB(238,170,85);  // Light Brown
COLORREF crClr2 = RGB(170,255,170); // Light Green
COLORREF crClr3 = RGB(170,170,255); // Purple
COLORREF crClr4 = RGB(201,236,250); // Soft Blue

HPEN hpSLnk = NULL;
HPEN hpWLnk = NULL;
HPEN  hpGrid = NULL;
HPEN  hpGrid2 = NULL;
HFONT hfCN8  = NULL;
HFONT hfCN8b  = NULL;
HFONT hfCN24b = NULL;
HFONT hfCN18b = NULL;
HPEN  hpBox  = NULL;
HPEN  hpGray = NULL;
HPEN  hpPurp = NULL;
HPEN  hpLBrown_1 = NULL;
HPEN  hpLBrown_2 = NULL;
HPEN  hpLBrown_4 = NULL;
HPEN  hpLGreen = NULL;
HPEN  hpLBlue = NULL;
HPEN  hpRed = NULL;
HPEN hpWhite = NULL;
HPEN hpWhite_2 = NULL;
HPEN hpBlue_2 = NULL;
HPEN hpGreen_2 = NULL;
HPEN hpText = NULL;
HPEN hpPink_2 = NULL;

HBRUSH hbSel    = NULL; // Get_Solid_Brush(crPBlue);
HBRUSH hbOff    = NULL; // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
HBRUSH hbOn     = NULL; // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
HBRUSH hbUnRect = NULL; // byell = Get_Solid_Brush(crBYellow); // = RG(248,252,121);
HBRUSH hbOdd    = NULL;

#define MX_COLOR_BR  4
static HBRUSH hbcolors[MX_COLOR_BR] = {0};


// Special vertical font
HFONT hfVert = NULL;
int hfVpoints = 0;
SIZE vrcSize = {0};
int Log_Pixels = 0;
int Last_Points = 0;
SIZE fntSz = {0};

// Idea - When the BACKGROUND color is set, then DrawText() fill the TEXT space with the 
// background color, BUT I want to FILL the WHOLE rectangle, so I replace a call SETBkColor(hdc,cr)
// with a SETBkColor(hdc,cr) which will not only SET the backgound color, but return a BRUSH
// that can be used in FillRect.
typedef struct tagBKBRUSH {
    HBRUSH hb;
    COLORREF cr;
}BKBRUSH, *PBKBRUSH;
typedef vector<BKBRUSH> vBKBR;

vBKBR vBkBr;
BKBRUSH act_bkbr;
BOOL g_bHvBkBrush = FALSE;
BOOL g_bAddRect = FALSE;

COLORREF SETBkColor( HDC hdc, COLORREF cr )
{
    size_t max = vBkBr.size();
    size_t ii;
    COLORREF old = SetBkColor(hdc,cr);
    g_bHvBkBrush = TRUE;
    for (ii = 0; ii < max; ii++) {
        act_bkbr = vBkBr[ii];
        if (act_bkbr.cr == cr)
            return old;
    }
    act_bkbr.cr = cr;
    act_bkbr.hb = Get_Solid_Brush(cr);
    vBkBr.push_back(act_bkbr);
    return old;
}

void Clear_BkBr()
{
    size_t max = vBkBr.size();
    size_t ii;
    for (ii = 0; ii < max; ii++) {
        act_bkbr = vBkBr[ii];
        DeleteObject(act_bkbr.hb);
    }
    vBkBr.clear();
}

HBRUSH Get_Clr1_Brush() { return Get_Solid_Brush(crClr1); }
HBRUSH Get_Clr2_Brush() { return Get_Solid_Brush(crClr2); }
HBRUSH Get_Clr3_Brush() { return Get_Solid_Brush(crClr3); }
HBRUSH Get_Clr4_Brush() { return Get_Solid_Brush(crClr4); }

HBRUSH *Get_Color_Brushes()
{
    if (!hbcolors[0]) {
        hbcolors[0] = Get_Clr1_Brush();
        hbcolors[1] = Get_Clr2_Brush();
        hbcolors[2] = Get_Clr3_Brush();
        hbcolors[3] = Get_Clr4_Brush();
    }
    return hbcolors;
}

int m_color_cnt = 5;
COLORREF GetColorPerCount(int len)
{
    if (len == 1)
        return crRed;
    else if (len == 2)
        return crLBrown;
    else if (len == 3)
        return crBlue;
    else if (len == 4)
        return crLGreen;
    return crGray;
}

//int join_poly_points = 1;
//int join_first_to_last = 1;
//int g_paint_bounding_box = 0; // paint_bounding_box( hdc, &mm ); can overwrite points!
//int g_paint_airports = 1;     // only 1 or 2 selected - could load apt.dat...
// paint_airports( hdc, g_c_lat, g_c_lon, map_size, map_zoom, &rc );

int done_CN8_text = 0;
SIZE g_szCN8Text = { 14, 14 };

#define  MX_COLORS   12
typedef struct tagColorSet {
   COLORREF cr;
   char * desc;
   int   style;
   HPEN  hp;
   HBRUSH hb;
}ColorSet, * PColorSet;

ColorSet colors[MX_COLORS] = {
   { RGB(255,0,0), "red", HS_BDIAGONAL, 0, 0 }, // 45-degree upward left-to-right hatch 
   { RGB(0,0,255), "blue", HS_CROSS, 0, 0 },    // Horizontal and vertical crosshatch
   { RGB(0,255,0), "green", HS_DIAGCROSS, 0, 0 }, // 45-degree crosshatch
   { RGB(255,255,0), "redgreen", HS_FDIAGONAL, 0, 0 }, // 45-degree downward left-to-right hatch 
   { RGB(255,0,255), "redblue", HS_HORIZONTAL, 0, 0 }, // Horizontal hatch
   { RGB(0,255,255), "greenblue", HS_VERTICAL, 0, 0 }, // Vertical hatch
   { RGB(125,0,0), "halfred", HS_BDIAGONAL, 0, 0 },
   { RGB(0,0,125), "halfblue", HS_CROSS, 0, 0 },
   { RGB(0,125,0), "halfgreen", HS_DIAGCROSS, 0, 0 },
   { RGB(125,125,0), "halfredgreen", HS_FDIAGONAL, 0, 0 },
   { RGB(125,0,125), "halfredblue", HS_HORIZONTAL, 0, 0 },
   { RGB(0,125,125), "halfgreenblue", HS_VERTICAL, 0, 0 }
};

#define DELOBJ(o) if (o) DeleteObject(o); o = NULL

void clean_up_paint_tools(void)
{
   int i;
   PColorSet pcs = &colors[0];
   for(i = 0; i < MX_COLORS; i++, pcs++)
   {
      if(pcs->hp)
         DeleteObject(pcs->hp);
      pcs->hp = 0;
      if(pcs->hb)
         DeleteObject(pcs->hb);
      pcs->hb = 0;
   }
   if(hpGrid)
      DeleteObject(hpGrid);
   hpGrid = NULL;
   if(hpGrid2)
      DeleteObject(hpGrid2);
   hpGrid2 = NULL;
   if(hpBox)
      DeleteObject(hpBox);
   hpBox = NULL;
   if(hpGray)
      DeleteObject(hpGray);
   hpGray = NULL;
   if(hpPurp)
      DeleteObject(hpPurp);
   hpPurp = NULL;

   if(hpLBrown_1)
      DeleteObject(hpLBrown_1);
   hpLBrown_1 = NULL;
   if(hpLBrown_2)
      DeleteObject(hpLBrown_2);
   hpLBrown_2 = NULL;
   if(hpLBrown_4)
      DeleteObject(hpLBrown_4);
   hpLBrown_4 = NULL;

   if(hpLGreen)
      DeleteObject(hpLGreen);
   hpLGreen = NULL;
   if (hpLBlue)
       DeleteObject(hpLBlue);
   hpLBlue = NULL;
   if(hfCN8)
      DeleteObject(hfCN8);
   hfCN8 = NULL;
   if(hfCN24b)
      DeleteObject(hfCN24b);
   hfCN24b = NULL;
   if(hfCN18b)
      DeleteObject(hfCN18b);
   hfCN18b = NULL;
   if (hpRed)
       DeleteObject(hpRed);
   hpRed = NULL;
   if (hpWhite)
       DeleteObject(hpWhite);
   hpWhite = NULL;
   if (hpWhite_2)
       DeleteObject(hpWhite_2);
   hpWhite_2 = NULL;
   if (hpBlue_2)
       DeleteObject(hpBlue_2);
   hpBlue_2 = NULL;
   if (hpGreen_2)
       DeleteObject(hpGreen_2);
   hpGreen_2 = NULL;
   if (hpText)
       DeleteObject(hpText);
   hpText = NULL;
   if (hfVert)
       DeleteObject(hfVert);
   hfVert = NULL;

   if (hpPink_2)
       DeleteObject(hpPink_2);
   hpPink_2 = NULL;

    DELOBJ(hbSel); // Get_Solid_Brush(crPBlue);
    DELOBJ(hbOff); // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    DELOBJ(hbOn); // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
    DELOBJ(hbUnRect); // byell = Get_Solid_Brush(crBYellow); // = RG(248,252,121);

    DELOBJ(hpSLnk);
    DELOBJ(hpWLnk);

    DELOBJ(hbOdd);
    DELOBJ(hfCN8b);
   //clear_painted_points();
   //clear_paintPoints();
   //delete_color_list();
    Clear_BkBr();

    // #define MX_COLOR_BR  4
    //static HBRUSH hbcolors[MX_COLOR_BR] = {0};
    for (i = 0; i < MX_COLOR_BR; i++) {
        DELOBJ(hbcolors[i]);
    }

    Set_Paint_Chains_Invalid(); // STOP PAINTING PAIRS, etc 

}

HBRUSH CreateStyledBrush( int fnStyle, COLORREF clrref )
{
   return CreateHatchBrush( fnStyle,      // hatch style
      clrref );   // foreground color
}

HFONT Create_Font_of_PointSize( HDC hdc, int pointsize, bool bold )
{
   LOGFONT lf;
   LOGFONT * plf = &lf;
   ZeroMemory( plf, sizeof(LOGFONT) );
   plf->lfHeight = -MulDiv( pointsize, GetDeviceCaps(hdc, LOGPIXELSY), 72 );
   strcpy( &plf->lfFaceName[0], "Courier New" );
   if (bold)
       plf->lfWeight = FW_BOLD;   // set BOLD font
   HFONT hf = CreateFontIndirect( plf );
   return hf;
}

HFONT Get_CN8b_Font( HDC hdc )
{
   if (!hfCN8b) 
       hfCN8b = Create_Font_of_PointSize( hdc, 8, true );
   return hfCN8b;
}

HFONT Get_CN24b_Font( HDC hdc )
{
   HFONT hf = hfCN24b;
   if(hf)
      return hf;
   hf = Create_Font_of_PointSize( hdc, 24, true );
   hfCN24b = hf;
   return hf;
}

HFONT Get_CN18b_Font( HDC hdc )
{
    if (!hfCN18b)
       hfCN18b = Create_Font_of_PointSize( hdc, 18, true );
   return hfCN18b;
}


HFONT Get_Font_For_Vertical_Boxes( HDC hdc, PRECT prc, SIZE *psz, char *cpc )
{
    SIZE sz;
    sz.cx = prc->right - prc->left;
    sz.cy = prc->bottom - prc->top;
    if (hfVert && fntSz.cx && fntSz.cy ) {
        SIZE sz1;
        SIZE sz2;
        int marg = 1;
        sz1.cx = sz.cx - marg;
        sz1.cy = sz.cy - marg;
        sz2.cx = sz.cx + marg;
        sz2.cy = sz.cy + marg;
        if ((vrcSize.cx >= sz1.cx) && (vrcSize.cy >= sz1.cy) &&
            (vrcSize.cy <= sz2.cy) && (vrcSize.cy <= sz2.cy)) {
            *psz = fntSz;
            return hfVert;
        }
    }

    if (Log_Pixels == 0)
        Log_Pixels = GetDeviceCaps(hdc, LOGPIXELSY);

    int len = (int)strlen(cpc);
    if (!len)
        return 0;

    //char *tb = GetNxtBuf();
    //HFONT hf = Get_CN24b_Font(hdc);
    //int off = 0;
    int points = ((sz.cx / 3) * Log_Pixels) / 96;
    if (points >= 24) {
        if (hfVert)
           DeleteObject(hfVert);
        hfVert = 0;
        return 0;
    }
    if (points < 8)
        points = 8;
    if (hfVert) {
        if ( hfVpoints == points )
            return hfVert;  // use this one
       DeleteObject(hfVert);
    }

    hfVpoints = points;
    hfVert = Create_Font_of_PointSize( hdc, points, true );
    fntSz.cx = 0;
    fntSz.cy = 0;
    if (hfVert) {
        HFONT fto = (HFONT)SelectObject(hdc,hfVert);
        get_text_size_len_hdc( &fntSz, "9", 1, hdc );
        SelectObject(hdc,fto);
    }
    if (Last_Points != points) {
        sprtf("Sug pt sz %d, sz x/y %d/%d %d/%d\n",points, psz->cx, psz->cy, fntSz.cx, fntSz.cy);
    }
    Last_Points = points;
    *psz = fntSz;
    vrcSize = sz;

    return hfVert;
}


HFONT Get_CN8_Font( HDC hdc )
{
   LOGFONT lf;
   LOGFONT * plf = &lf;
   HFONT hf = hfCN8;
   if(hf)
      return hf;
   ZeroMemory( plf, sizeof(LOGFONT) );
   plf->lfHeight = -MulDiv( 8, GetDeviceCaps(hdc, LOGPIXELSY), 72 );
   strcpy( &plf->lfFaceName[0], "Courier New" );
   //plf->lfWeight = FW_BOLD;   // set BOLD font
   hf = CreateFontIndirect( plf );
   hfCN8 = hf;
   return hf;
}

HFONT Get_Font()
{
    HDC hdc = GetDC(g_hWnd);
    HFONT hf = Get_CN8_Font(hdc);
    ReleaseDC(g_hWnd,hdc);
    return hf;
}

void get_text_size_len_hdc( PSIZE psz, char * ps, int len, HDC hdc )
{
   GetTextExtentPoint32( hdc, ps, len, psz );
}

void get_text_size_len( PSIZE psz, char * ps, int len )
{
   PPPSET pps = get_act_paint_set();
   get_text_size_len_hdc( psz, ps, len, pps->hdc );
}

void get_text_size( PSIZE psz, char * ps )
{
   get_text_size_len( psz, ps, strlen(ps) );
}

void get_CN8_Text_Size( HDC hdc, PSIZE psz )
{
   if( !done_CN8_text ) {
       HFONT hf = Get_CN8_Font( hdc );
       HFONT hfOld = 0;
       if(hf) hfOld = (HFONT)SelectObject(hdc, hf);
       get_text_size_len_hdc( &g_szCN8Text, "A", 1, hdc );
       if(hfOld) SelectObject(hdc, hfOld);
       done_CN8_text = 1;   // only do this once
   }
   psz->cx = g_szCN8Text.cx;
   psz->cy = g_szCN8Text.cy;
}

HPEN Get_Solid_Pen_1( COLORREF cr )  { return CreatePen( PS_SOLID, 1, cr ); }
HPEN Get_Solid_Pen_2( COLORREF cr )  { return CreatePen( PS_SOLID, 2, cr ); }
HPEN Get_Solid_Pen_4( COLORREF cr )  { return CreatePen( PS_SOLID, 4, cr ); }
HPEN Get_Dotted_Pen_1( COLORREF cr ) { return CreatePen( PS_DOT, 1, cr );   }
// brush color value
HBRUSH Get_Solid_Brush( COLORREF cr ){ return CreateSolidBrush(cr); }

HPEN Get_RED_Pen_1( void )
{
   if( !hpRed )
       hpRed = Get_Solid_Pen_1( crRed );   // COLORREF crColor
   return hpRed;
}
HPEN Get_WHITE_Pen_1( void )
{
   if( !hpWhite )
       hpWhite = Get_Solid_Pen_1( crWhite );   // COLORREF crColor
   return hpWhite;
}

HPEN Get_WHITE_Pen_2( void )
{
   if( !hpWhite_2 )
       hpWhite_2 = Get_Solid_Pen_2( crWhite );   // COLORREF crColor
   return hpWhite_2;
}


HPEN Get_GRAY_Pen_1( void )
{
   if( hpGray )
      return hpGray;
   hpGray = Get_Solid_Pen_1( crGray );   // COLORREF crColor
   return hpGray;
}

HPEN Get_GRID_Pen( void )
{
   if( hpGrid )
      return hpGrid;
   hpGrid = Get_Solid_Pen_1( crGrid );   // COLORREF crColor - Get_GRID_Pen
   return hpGrid;
}

HPEN Get_GRID2_Pen( void )
{
   if( hpGrid2 )
      return hpGrid2;
   hpGrid2 = Get_Dotted_Pen_1( crGrid );   // COLORREF crColor
   return hpGrid2;
}

HPEN Get_Purp_Pen( void )
{
   if( hpPurp )
      return hpPurp;
   hpPurp = Get_Dotted_Pen_1( crPurp );   // COLORREF crColor
   return hpPurp;
}
HPEN Get_LBrown_Pen_1( void )
{
   if( hpLBrown_1 )
      return hpLBrown_1;
   hpLBrown_1 = Get_Solid_Pen_1( crLBrown );   // COLORREF crColor
   return hpLBrown_1;
}
HPEN Get_LBrown_Pen_2( void )
{
   if( hpLBrown_2 )
      return hpLBrown_2;
   hpLBrown_2 = Get_Solid_Pen_2( crLBrown );   // COLORREF crColor
   return hpLBrown_2;
}
HPEN Get_LBrown_Pen_4( void )
{
   if( hpLBrown_4 )
      return hpLBrown_4;
   hpLBrown_4 = Get_Solid_Pen_4( crLBrown );   // COLORREF crColor
   return hpLBrown_4;
}

HPEN Get_LGreen_Pen( void )
{
   if( hpLGreen )
      return hpLGreen;
   hpLGreen = Get_Solid_Pen_1( crLGreen );   // COLORREF crColor
   return hpLGreen;
}
HPEN Get_LBlue_Pen( void )
{
   if( !hpLBlue )
       hpLBlue = Get_Solid_Pen_1( crLBlue );   // COLORREF crColor
   return hpLBlue;
}
HPEN Get_Blue_Pen_2(void)
{
    if (!hpBlue_2)
        hpBlue_2 = Get_Solid_Pen_2( crBlue );
    return hpBlue_2;
}
HPEN Get_Green_Pen_2(void)
{
    if (!hpGreen_2)
        hpGreen_2 = Get_Solid_Pen_2( crGreen );
    return hpGreen_2;
}

HPEN Get_BOX_Pen( void )
{
   if( hpBox )
      return hpBox;
   hpBox = CreatePen( PS_DASH, // int fnPenStyle, 
      1, // int nWidth, 
      crBox );   // COLORREF crColor
   return hpBox;
}

HPEN Get_Text_Pen( void )
{
   if( !hpText )
       hpText = Get_Solid_Pen_1( crText );
   return hpText;
}

HPEN Get_hpPink_2()
{
    if (!hpPink_2)
        hpPink_2 = Get_Solid_Pen_2( crPink );
    return hpPink_2;
}

HBRUSH Get_Sel_Brush()
{
    if (!hbSel) hbSel = Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    return hbSel;
}
HBRUSH Get_Off_Brush()
{
    if (!hbOff) hbOff = Get_Solid_Brush(crOff); // crPBlue);
    return hbOff;
}
HBRUSH Get_On_Brush()
{
    if (!hbOn) hbOn  = Get_Solid_Brush(crOn); // crLPink);    // = RGB(255,192,255);
    return hbOn;
}
HBRUSH Get_UnRect_Brush()
{
    if (!hbUnRect) hbUnRect = Get_Solid_Brush(crBYellow); // = RG(248,252,121);
    return hbUnRect;
}

HBRUSH Get_Odd_Brush()
{
    if (!hbOdd) hbOdd = Get_Solid_Brush(crOddBox); // = RGB(200,200,200);
    return hbOdd;
}


HPEN Get_SLnk_Pen( void )
{
   if( !hpSLnk )
       hpSLnk = Get_Solid_Pen_1( crSLnk );   // COLORREF crColor
   return hpSLnk;
}
HPEN Get_WLnk_Pen( void )
{
   if( !hpWLnk )
       hpWLnk = Get_Solid_Pen_1( crWLnk );   // COLORREF crColor
   return hpWLnk;
}


int   g_map_next_color = 0;
int   curr_act_color;
char * g_act_color = "none";
PColorSet ps_act_pcs = NULL;
HPEN Get_Act_Pen(void)
{
   if(ps_act_pcs)
      return ps_act_pcs->hp;
   return NULL;
}
HBRUSH get_act_brush(void)
{
   if(ps_act_pcs)
      return ps_act_pcs->hb;
   return NULL;
}

COLORREF get_next_color( void )
{
   curr_act_color = g_map_next_color;
   ps_act_pcs = &colors[curr_act_color];
   COLORREF cr = ps_act_pcs->cr;
   g_act_color = ps_act_pcs->desc;
   if( !ps_act_pcs->hp )
      ps_act_pcs->hp = Get_Solid_Pen_1(cr);
   if( !ps_act_pcs->hb )
      ps_act_pcs->hb = CreateStyledBrush( ps_act_pcs->style, cr );

   // if changing colors, bump tot next, wrapping
   //if( get_poly_globs()->g_paint_colors ) {
   //   g_map_next_color++;  // bump to NEXT
   //   if( g_map_next_color >= MX_COLORS )
   //      g_map_next_color = 0;
   //}
   return cr;  // return color to use
}

COLORREF get_std_text_color(void) { return crText; }

COLORREF Get_LBrown_Color(void) { return crLBrown; }
COLORREF Get_LGreen_Color(void) { return crLGreen; }
COLORREF Get_Cntr_Color(void)   { return crCntr;   }
COLORREF Get_Top_Color(void)    { return crTop;   }

void draw_line( HDC hdc, int x1, int y1, int x2, int y2 )
{
   MoveToEx( hdc, x1, y1, NULL ); // start
   LineTo( hdc, x2, y2 ); // to center bottom
}

void draw_line( HDC hdc, PPOINT a, PPOINT b )
{
    draw_line( hdc, a->x, a->y, b->x, b->y );
}

void draw_rectangle( HDC hdc, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )
{
    draw_line( hdc, x1, y1, x2, y2 );
    draw_line( hdc, x2, y2, x3, y3 );
    draw_line( hdc, x3, y3, x4, y4 );
    draw_line( hdc, x4, y4, x1, y1 );
}

void Draw_Rectangle( HDC hdc, PRECT pr, int offset = 0 );
void Draw_Rectangle( HDC hdc, PRECT pr, int offset )
{
    draw_rectangle( hdc, pr->left + offset, pr->top + offset,
                         pr->left + offset, pr->bottom - offset, 
                         pr->right - offset, pr->bottom - offset,
                         pr->right - offset, pr->top + offset);
}

void draw_X( HDC hdc, PRECT pr )
{
    draw_line( hdc, pr->left, pr->top, pr->right, pr->bottom );
    draw_line( hdc, pr->left, pr->bottom, pr->right, pr->top );
}

void draw_Y( HDC hdc, PRECT pr )
{
    POINT p;
    p.x = (pr->left + pr->right) / 2;   // center
    p.y = ((pr->top + pr->bottom) / 2); // center * 2;
    draw_line( hdc, pr->left, pr->top, p.x, p.y );
    draw_line( hdc, pr->right, pr->top, p.x, p.y );
    draw_line( hdc, p.x, p.y, p.x, pr->bottom );
}


void Add_String( char * ps )
{
    string s = ps;
    vSTG *vlp = Get_Screen_Lines();
    vlp->push_back(s);
}

void set_repaint2( bool erase)
{
    if (g_hMsgDialog && IsWindow(g_hMsgDialog)) 
        InvalidateRect(g_hMsgDialog,NULL,erase);
}


void set_repaint( bool erase)
{
    InvalidateRect(g_hWnd,NULL,erase);
    set_repaint2(erase);
}

#ifdef ADD_TEXT_BLOCK
VOID Draw_Top_Text( HDC hdc )
{
    HFONT hf = Get_CN8_Font( hdc );
    HFONT hfOld = (HFONT)SelectObject(hdc,hf);
    char *ps = _T("Sudoku Step Output");
    int len = (int)strlen(ps);
    RECT rc = g_rcText;        // get a working copy
    PRECT prc = &rc;        // pointer to working copy
    UINT format = DT_CENTER;
    SIZE sz;
    char buf[MAX_STRING+2];
    vSTG * vlp = Get_Screen_Lines();

    // get text sizes
    get_CN8_Text_Size( hdc, &sz );

    size_t count = vlp->size();     // get number of lines to paint 
    size_t i;
    int height = prc->bottom - prc->top - sz.cy;    // height - header
    height -= sz.cy;    // subtract header line
    size_t lncnt = height / sz.cy;  // lines that will fit
    if (lncnt > count)
        i = 0;  // start at beginning
    else
        i = count - lncnt;  // start at an offset into list

    // Paint the HEADING
    prc->bottom = prc->top + sz.cy;
    DrawText( hdc, // handle to DC
        ps,         // text to draw
        len,        // text length
        prc,        // formatting dimensions
        format );   // text-drawing options

    format = DT_LEFT;
    for (; i < count; i++) {
        string s = vlp->at(i);  // get string
        ps = buf;
        strncpy(ps,s.c_str(),MAX_STRING);
        ps[MAX_STRING-1] = 0;   // ensure zero terminated
        len = (int)strlen(ps);
        if (len) {
            while(len--) {
                if (buf[len] > ' ') {
                    len++;
                    break;
                }
                buf[len] = 0;
            }
            if (len > 0) {
                DrawText( hdc, // handle to DC
                    ps,         // text to draw
                    len,        // text length
                    prc,        // formatting dimensions
                    format );   // text-drawing options
            }
        }
        prc->top += sz.cy;
        prc->bottom = prc->top + sz.cy;
    }

    if(hfOld)
        SelectObject(hdc,hfOld);

}
#endif // #ifdef ADD_TEXT_BLOCK

int one_value_count = 0;
int NO_value_count = 0;
int total_empty_count = -1;
ROWCOL ones[9*9];
ROWCOL all_anal[9*9];
PROWCOL Get_Copy_All_Anal()
{
    PROWCOL all = (PROWCOL)malloc(sizeof(all_anal));
    if (!all) {
        MB("Copy a all_anal FAILED!");
        exit(1);
    }
    memcpy(all,all_anal,sizeof(all_anal));
    return all;
}


PROWCOL Get_First_Ones()
{
    if (one_value_count &&
        (NO_value_count == 0))
        return &ones[0];
    return NULL;
}

int ht_margin = 2;
int vt_margin = 2;
// sometime numbers occluded a little
//int ht_margin = 1;
//int vt_margin = 1;

void Paint_Set(HDC hdc, PRECT prcCell, PSET ps, PABOX2 pab, int row, int col)
{
    SIZE sz;
    int height, width;
    RECT rct = *prcCell;
    char *tb = GetNxtBuf();
    // Adjust for margins
    rct.left   += ht_margin;
    rct.right  -= ht_margin;
    rct.top    += vt_margin;
    rct.bottom -= vt_margin;

    height = rct.bottom - rct.top;
    width  = rct.right  - rct.left;
    get_CN8_Text_Size( hdc, &sz );
    if ( !( (( sz.cy * 3 ) < height) && ((sz.cx * 3) < width) ) ) {
        rct = *prcCell;
        height = rct.bottom - rct.top;
        width  = rct.right  - rct.left;
        if ( !( (( sz.cy * 3 ) < height) && ((sz.cx * 3) < width) ) ) {
            static bool dn_warn =false;
            if (!dn_warn) {
                dn_warn = true;
                sprtf("TODO - Need to generate a SMALLER font than 8!\n");
            }
        }
    }
    if ( (( sz.cy * 3 ) < height) && ((sz.cx * 3) < width) ) {
        int i, val, len, cnt;
        char buf[16];
        char *pb = buf;
        UINT format = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
        HFONT hf = Get_CN8_Font(hdc);
        HFONT hfb = Get_CN8b_Font(hdc);
        //HFONT hf24 = Get_CN24b_Font(hdc);
        HFONT hf18 = Get_CN18b_Font(hdc);
        HPEN hp  = Get_Text_Pen();
        HFONT hfold = (HFONT)SelectObject(hdc,hf);
        HPEN hpold = (HPEN)SelectObject(hdc,hp);
        COLORREF crold = SetTextColor(hdc,crText);
        COLORREF crbkold, crtold;
        int scnt = Get_Set_Cnt(ps);
        RECT rc = rct;
        int h3 = height / 3;
        int w3 = width / 3;
        uint64_t flag = 0;
        uint64_t cellflg = 0;
        uint64_t cummflag = 0;
        RECT rcsave;

        if ( pab && VALIDRC(row,col) ) {
            cellflg = pab->line[row].set[col].cellflg; // get the FLAG for the cell
        }
        rc.right = rc.left + w3;
        rc.bottom = rc.top + h3;
        cnt = 0;
        if (add_debug_paint_set) sprintf(tb,"%c%d ", (char)(row + 'A'), col + 1);
        for (i = 0; i < 9; i++) {
            if ( i && ((i % 3) == 0) ) {
                // go down a line
                cnt++;
                rc = rct;
                rc.top += (h3 * cnt);
                rc.bottom = rc.top + h3;
                rc.right = rc.left + w3;
            }
            val = ps->val[i];
            flag = ps->flag[i]; // extract state flag for this potential value
            cummflag |= flag;
            if (val) {
                len = sprintf(pb,"%d",val);
                if (add_debug_paint_set && flag) { sprintf(EndBuf(tb),"%d-",val); AddFlags2Buf(tb,flag); }
                // uses the device context's selected font, text color, and background color to draw the text
                if (scnt == 1) {
                    //SelectObject(hdc,hf24);
                    SelectObject(hdc,hf18);
                    crbkold = SETBkColor( hdc, crLGreen );
                    rcsave = rc;
                    rc = rct;
                } // else 
                if (flag & cf_ELIM) {
                    crbkold = SETBkColor( hdc, crElimBk );
                    crtold = SetTextColor(hdc,crElimFg);
                    if (scnt != 1) {
                        SelectObject(hdc,hfb);
                    }
                } else if (flag & cf_UNIQ) {
                    crbkold = SETBkColor( hdc, crLGreen );
                // for each in cf_PRIM ( cf_HP  | cf_HT  | | cf_SS  | cf_XC...
                } else if (flag & cf_NP) {
                    crbkold = SETBkColor( hdc, crCyan );
                } else if (flag & cf_LK) {
                    crbkold = SETBkColor( hdc, crLPink );
                } else if (flag & cf_XW) {
                    crbkold = SETBkColor( hdc, crLOrange );
                } else if (flag & cf_PP) {
                    crbkold = SETBkColor( hdc, crCyan );    // TODO - need another color
                } else if (flag & cf_YW) {
                    crbkold = SETBkColor( hdc, crLGreen );  // TODO - need another color
                } else if (flag & cf_UR) {
                    crbkold = SETBkColor( hdc, crLOrange ); // TODO - need another color
                } else if (flag & cf_NT) {
                    crbkold = SETBkColor( hdc, crLPink );   // TODO - need another color
                } else if (flag & cf_HP) {
                    crbkold = SETBkColor( hdc, crCyan );   // TODO - need another color
                } else if (flag & cf_HT) {
                    crbkold = SETBkColor( hdc, crLGreen );    // TODO - need another color
                } else if (flag & cf_SS) {
                    crbkold = SETBkColor( hdc, crLGreen );  // TODO - need another color
                } else if (flag & cf_XC) {
                    crbkold = SETBkColor( hdc, crLBlue ); // TODO - need another color
                } else if (flag & cf_CC) {
                    if (flag & cf_XCA) {
                        crbkold = SETBkColor( hdc, crLBlue ); // TODO - need another color
                    } else if (flag & cf_XCB) {
                        crbkold = SETBkColor( hdc, crLGreen );  // TODO - need another color
                    } else {
                        crbkold = SETBkColor( hdc, crLOrange ); // TODO - need another color
                    }
                } else if (flag & cf_XY) {
                    if (flag & cf_XCA) {
                        crbkold = SETBkColor( hdc, crLBlue ); // TODO - need another color
                    } else if (flag & cf_XCB) {
                        crbkold = SETBkColor( hdc, crLGreen );  // TODO - need another color
                    } else {
                        crbkold = SETBkColor( hdc, crLOrange ); // TODO - need another color
                    }
                } else if (flag & cf_SL) {
                    if (flag & cf_XCA) {
                        crbkold = SETBkColor( hdc, crLBlue ); // TODO - need another color
                    } else if (flag & cf_XCB) {
                        crbkold = SETBkColor( hdc, crLGreen );  // TODO - need another color
                    } else {
                        crbkold = SETBkColor( hdc, crLOrange ); // TODO - need another color
                    }
                }

                if (on_xcycles_test && ((on_xcycles_test - 1 ) == i)) {
                    PROWCOL prc = get_XCycles_RC( row, col, i + 1 );
                    if (prc) {
                        if (g_bUseChNum) // use chain number
                            len = sprintf(pb,"%d",(int)prc->set.tm);
                        if (prc->set.flag[i] & cf_XCA) // get the COLOR for the candidate
                            crbkold = SETBkColor( hdc, crLGreen );
                        else
                            crbkold = SETBkColor( hdc, crCyan );
                    } else {
                        crbkold = SETBkColor( hdc, crLPink );   // TODO - need another color
                    }
                }

                if (do_color_test2 && ((do_color_test2 - 1) == i)) {
                    PROWCOL prc = Get_Color2_RC( row, col, i + 1 );
                    if (prc) {
                        if ( (prc->set.flag[i] & (cf_XCA|cf_XCB)) == (cf_XCA|cf_XCB) ) // BOTH COLORS ON - eek
                            crbkold = SETBkColor( hdc, crLOrange);   // TODO - need another color
                        else if (prc->set.flag[i] & cf_XCA) // get the COLOR for the candidate
                            crbkold = SETBkColor( hdc, crLGreen );
                        else if (prc->set.flag[i] & cf_XCB)
                            crbkold = SETBkColor( hdc, crCyan );
                        else
                            crbkold = SETBkColor( hdc, crLPink );   // TODO - need another color
                    } else {
                        crbkold = SETBkColor( hdc, crLPink );   // TODO - need another color
                    }
                }
                // ************************************************************
                if (g_bAddRect) Draw_Rectangle( hdc, &rc );
                // *************************************************************
                if (g_bHvBkBrush && act_bkbr.hb) FillRect(hdc,&rc,act_bkbr.hb);
                // *************************************************************
                DrawText( hdc, // handle to DC
                    pb,         // text to draw
                    len,        // text length
                    &rc,        // formatting dimensions
                    format );   // text-drawing options
                // *************************************************************
                if ((scnt == 1) || (flag & (cf_UNIQ | cf_ELIM | cf_PRIM))) {
                    SETBkColor( hdc, crbkold ); // put background color back
                    if (scnt == 1) {
                        SelectObject(hdc,hf);
                        rc = rcsave;
                    }
                }
                if (flag & cf_ELIM) {
                    SetTextColor(hdc,crtold);
                    SelectObject(hdc,hf);
                }
                if (on_xcycles_test && ((on_xcycles_test - 1) == i)) {
                    SETBkColor( hdc, crbkold ); // put background color back
                }
                if (do_color_test2 && ((do_color_test2 - 1) == i)) {
                    SETBkColor( hdc, crbkold ); // put background color back
                }
            }
            rc.left  += w3;
            rc.right += w3;
        }
        if (add_debug_paint_set && cummflag) OUTIT(tb);
        SetTextColor(hdc,crold);
        SelectObject(hdc,hfold);
        SelectObject(hdc,hpold);
    }

}

HFONT Get_Legend_Font(HDC hdc)
{
    //HFONT hf = Get_CN24b_Font( hdc );
    HFONT hf = Get_CN18b_Font( hdc );
    return hf;
}

void Paint_Legend_Text( HDC hdc )
{
    int row, col, len;
    UINT format = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
    PRECT prc;
    char *tb = GetNxtBuf();
    HFONT hf = Get_Legend_Font( hdc );
    HFONT hfold = (HFONT)SelectObject(hdc,hf);
    //COLORREF crold = SetTextColor(hdc,crText);
    COLORREF crold = SetTextColor(hdc,crLGray);
    RECT rc;
    // paint the vertical legend
    for (row = 0; row < 9; row++) {
        prc = &g_rcVLegL[row]; // get for PAINTING ROW letter
        // Draw_Rectangle(hdc,pr);
        len = sprintf(tb,"%c", (char)row + 'A');
        DrawText( hdc, // handle to DC
            tb,         // text to draw
            len,        // text length
            prc,        // formatting dimensions
            format );   // text-drawing options
        prc = &g_rcVLegR[row]; // get for PAINTING ROW - vertical right number legend
        // Draw_Rectangle(hdc,pr);
        len = sprintf(tb,"%d", row + 1);
        DrawText( hdc, // handle to DC
            tb,         // text to draw
            len,        // text length
            prc,        // formatting dimensions
            format );   // text-drawing options
    }

    // paint across the TOP 1 2 3 ...
    for (col = 0; col < 9; col++) {
        prc = &g_rcHLegT[col];  // PAINT of top text in Paint_Legend_Text()
        //Draw_Rectangle(hdc,pr);
        rc = *prc;
        len = sprintf(tb,"%d", col + 1);
        rc.bottom -= 2;
        DrawText( hdc, // handle to DC
            tb,         // text to draw
            len,        // text length
            &rc,        // formatting dimensions
            format );   // text-drawing options
       if (add_bottom_legend) {
            prc = &g_rcHLegB[col];  // PAINT of bottom text in Paint_Legend_Text()
            rc = *prc;
            DrawText( hdc, // handle to DC
                tb,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format );   // text-drawing options
       }
    }
    SetTextColor(hdc,crold);
    SelectObject(hdc,hfold);
}


void Paint_XW_X( HDC hdc, PRECT prc, uint64_t flag )
{
    HPEN hp = Get_hpPink_2();
    if (hp) {
        HPEN hpold = (HPEN)SelectObject(hdc,hp);
        draw_X(hdc,prc);
        POINT a,b;
        if (flag & cf_XWR) {
            a.x = prc->left;
            a.y = (prc->top + prc->bottom) / 2;
            b.x = prc->right;
            b.y = a.y;
        }
        else
        {
            a.x = (prc->left + prc->right) / 2;
            b.x = a.x;
            a.y = prc->top;
            b.y = prc->bottom;
        }
        draw_line(hdc, &a, &b );
        if (hpold)
            SelectObject(hdc,hpold);
    }
}

void Paint_YW_Y( HDC hdc, PRECT prc, uint64_t flag )
{
    HPEN hp = Get_hpPink_2();
    if (hp) {
        HPEN hpold = (HPEN)SelectObject(hdc,hp);
        draw_Y(hdc,prc);
        if (hpold)
            SelectObject(hdc,hpold);
    }
}

bool RC_Same_Box(PROWCOL prc1, PROWCOL prc2)
{
    if (GETBOX(prc1->row,prc1->col) == GETBOX(prc2->row,prc2->col))
        return true;
    return false;
}
bool RC_Same_RCB(PROWCOL prc1, PROWCOL prc2)
{
    if (prc1->row == prc2->row)
        return true;
    if (prc1->col == prc2->col)
        return true;
    if (RC_Same_Box(prc1,prc2))
        return true;
    return false;
}

bool rcp_in_vrcp( RCPAIR &rcp, vRCP &vrcp )
{
    size_t i;
    for (i = 0; i < vrcp.size(); i++) {
        if (equal_rcp(rcp,vrcp[i]))
            return true;
    }
    return false;
}


void Paint_Box( HDC hdc )
{
    int row, col, val;
    uint64_t cellflg;
    PABOX2 pb = get_curr_box();
    HFONT hf, hfold;
    PRECT prc;
    char buf[MAX_STRING];
    char *tb = buf;
    int len;
    UINT format = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
    UINT format2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
    SET set;
    int offset = 2;
#ifdef ADD_HINT_TEXT
    RECT rc;
    char *cp;
    char *cp2;
#endif // #ifdef ADD_HINT_TEXT
    PSET ps;
    COLORREF crtold;
    COLORREF crbold;
    HFONT hf18 = 0;
    HFONT hf18old = 0;

    if (try_cn18_font)
        hf18 = Get_CN18b_Font(hdc);
    // TODO - How to decide the SIZE
    //hf = Get_CN8_Font( hdc );
    hf = Get_CN24b_Font( hdc );
    hfold = (HFONT)SelectObject(hdc,hf);
    one_value_count = 0;
    NO_value_count = 0;
    total_empty_count = 0;
    // Keep list of TWO types of CELLS with NO VALUE
    // 1. Cells that have NO potential possibilities - this means the puzzle is LOCKED, BAD, FAILING
    // 2. Cells that have just ONE potential value - maybe have key to fill these in...
    for (row = 0; row < 9; row++) {
        Fill_SET(&set); // start with ALL values ON for this ROW
        // process COL by COL
        for (col = 0; col < 9; col++) {
            if (g_bOddBox) {
                if (GETBOX(row,col) & 1)
                    crbold = SETBkColor(hdc, crOddBox);
                else
                    crbold = SETBkColor(hdc, crEvnBox);
            }
            val = pb->line[row].val[col];
            ps = &pb->line[row].set[col];
            cellflg = pb->line[row].set[col].cellflg;
            prc = &rcSquares[row][col];
            if (val) {
                // PAINT A VALUE
                if (cellflg & cl_Bad) {
                    crtold = SetTextColor(hdc, crBadVal);
                } else if (cellflg & cl_New) {
                    crtold = SetTextColor(hdc, crNewVal);
                } else {
                    crtold = SetTextColor(hdc, crOrgVal);
                }
                len = sprintf(tb,"%d",val);
                DrawText( hdc, // handle to DC
                    tb,         // text to draw
                    len,        // text length
                    prc,        // formatting dimensions
                    format );   // text-drawing options
                set.val[val - 1] = 0;   // clear potential values
                Zero_SET(ps);           // MAYBE should NOT do this, but for now...
                SetTextColor(hdc, crtold);
            } else {
                total_empty_count++;

                if (cellflg & cf_XW)
                    Paint_XW_X( hdc, prc, cellflg );
                else if (cellflg & cf_YWC)
                    Paint_YW_Y( hdc, prc, cellflg );

                Paint_Set(hdc, prc, ps, pb, row, col ); // paint candidates (with color)
            }
        }   // for (col = 0; col < 9; col++)
        // Done all columns
        if (try_cn18_font)
            hf18old = (HFONT)SelectObject(hdc,hf18);

#ifdef ADD_HINT_TEXT
        // PAINT HINT candidates
        // =====================
        // set now contains potential for this row
        prc = &rcHHint[row]; // get for PAINTING - ROW 0-9 - horiz hint text
        rc = *prc;
        rc.left += offset;  // bump text in from drawn margin
        *tb = 0;
        for (col = 0; col < 9; col++) {
            val = set.val[col]; // started full set of 9 value, cleared of current ROW
            if (val)
                sprintf(EndBuf(tb),"%d",val);
        }
        len = (int)strlen(tb);
        cp = cpHWork[row];  // ROWS
        if (len < MCA_SZ)
            strcpy(cp,tb);  // copy the potential value string
        else {
            MB("ERROR: String length greater than buffer. Increase MCA_SZ and recompile!");
            exit(1);
        }
        if (len) {
            DrawText( hdc, // handle to DC
                tb,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format2 );   // text-drawing options
        }

        // Now on a ONE BY ONE, do further eliminations, column by colum, for this whole ROW
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            prc = &rcSquares[row][col];
            if (!val) {
                SET set2;
                if ((row == 7)&&(col == 7))
                    val = 1;
                memcpy(&set2,&set,sizeof(SET));
                Eliminate_Same_Col( pb, col, &set2 );
                //  already done - Eliminate_Same_Row( PABOX2 pb, int row, PSET ps )
                Eliminate_Same_Box( row, col, pb, &set2 );
                if (use_elim_on_square) Eliminate_on_Square( row, col, pb, &set2 ); // Eliminate_Square_Values
                len = Get_Set_Cnt( &set2, &val );
                if ( !(( len == 0 ) || ( len == 1 )) ) {
                    Eliminate_Unique_In_Row( row, col, pb, &set2 );
                    len = Get_Set_Cnt( &set2, &val );
                }
                {
                    int off = (row * 9) + col;
                    PROWCOL prc = &all_anal[off];
                    prc->row = row;
                    prc->col = col;
                    prc->cnum = len;
                    memcpy(&prc->set, &set2, sizeof(SET));
                }

                if (( len == 0 ) || ( len == 1 )) {
                    RECT rc = *prc;
                    int offset = 5;
                    rc.left += offset;
                    rc.bottom -= offset;
                    rc.top += offset;
                    rc.right -= offset;
                    if (len == 0) {
                        PatBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, BLACKNESS);
                        NO_value_count++;
                    } else {
                        if (g_bShowHints) {
                            HPEN hp = Get_Purp_Pen();
                            HPEN hpold = (HPEN)SelectObject(hdc,hp);
                            Draw_Rectangle(hdc,&rc);
                            SelectObject(hdc,hpold);
                        }
                        PROWCOL prc = &ones[one_value_count];
                        prc->row = row;
                        prc->col = col;
                        prc->cnum = 1;
                        memcpy(&prc->set, &set2, sizeof(SET));
                        one_value_count++;
                    }
                }
            }
        }   // for each column
#endif // #ifdef ADD_HINT_TEXT
        if (try_cn18_font)
            SelectObject(hdc,hf18old);
    }   // for each row

#ifdef ADD_HINT_TEXT
    // Now the columns
    SIZE sz;
    get_text_size_len_hdc( &sz, "9", 1, hdc );
    //format2 = DT_WORDBREAK | DT_LEFT | DT_TOP;
    format2 = DT_LEFT | DT_TOP;
    //format2 = DT_EDITCONTROL;
    int width;
    for (col = 0; col < 9; col++) {
        for (row = 0; row < 9; row++) {
            set.val[row] = row + 1; // fill a full set of 9 value
        }
        prc = &rcVHint[col];    // get for PAINT
        rc = *prc;
        rc.left += offset;  // bump text in from drawn margin
        rc.top += offset;
        for (row = 0; row < 9; row++) {
            val = pb->line[row].val[col];
            if (val) 
                set.val[val - 1] = 0;
        }
        *tb = 0;
        for (row = 0; row < 9; row++) {
            val = set.val[row];
            if (val) {
                sprintf(EndBuf(tb),"%d",val);
            }
        }
        len = (int)strlen(tb);
        cp = cpVWork[col];  // COLUMNS
        if (len < MCA_SZ)
            strcpy(cp,tb);
        else {
            MB("ERROR: String length greater than buffer. Increase MCA_SZ and recompile!");
            exit(1);
        }
        if (len) {
            // adjust text 
            int off = 0;
            int len2;
            width = rc.right - rc.left - offset;
            hf = Get_Font_For_Vertical_Boxes( hdc, &rc, &sz, tb );
            if (hf)
                SelectObject(hdc,hf);
            cp = &buf[len+1]; // use upper part of buffer
            cp2 = cp; // moving pointer
            for (row = 0; row < len; row++) {
                cp2[off++] = tb[row];    // move a character
                cp2[off] = 0;
                len2 = (int)strlen(cp2);
                if (((len2 + 1) * sz.cx) > width) {
                    cp2[off++] = '\n'; // add break
                    cp2[off] = 0;       // zero terminate
                    cp2 = &cp2[off];    // move pointer to next line...
                    off = 0;            // restart offset on this line
                }
            }
            len = (int)strlen(cp);
            DrawText( hdc, // handle to DC
                cp,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format2 );   // text-drawing options
        }
    }
#endif // #ifdef ADD_HINT_TEXT

    SelectObject(hdc,hfold);

}

void Set_SIZES(HWND hWnd)
{
    int width, height;
#ifdef ADD_TEXT_BLOCK    
    int wid3;
#endif // #ifdef ADD_TEXT_BLOCK    

    HDC hdc = GetDC(hWnd);
    RECT rc;
    int row, col;
    int x1, x2, y1, y2;
#ifdef ADD_HINT_TEXT
    PRECT pr;
#endif // #ifdef ADD_HINT_TEXT
    PRECT pr2, pr3;

    GetClientRect(hWnd,&g_rcFullClient);
    // copy to user rectange
    g_rcClient = g_rcFullClient;

    width = g_rcFullClient.right - g_rcFullClient.left;
    height = g_rcFullClient.bottom - g_rcFullClient.top;
    if ((width > 760) && (height > 600)) {
        // add a gradient border
        rc = g_rcClient;
        int offset = 20;
        InflateRect(&rc,-offset,-offset);
        //rc.top += offset;
        //rc.bottom -= offset;
        //rc.left += offset;
        //rc.right -= offset;
        g_rcClient = rc;
    }


#ifdef ADD_HINT_TEXT
    if (try_cn18_font) {
        g_hint_width = 174;  // has to contain |123456789|
        g_hint_height = 110;
    }
#endif // #ifdef ADD_HINT_TEXT

    rc = g_rcClient;   // get usable client rectangle
    width = rc.right - rc.left;
    height = rc.bottom - rc.top;

    HFONT hfLeg = Get_Legend_Font(hdc);
    HFONT hfold = (HFONT)SelectObject(hdc,hfLeg);
    SIZE szleg;
    get_text_size_len_hdc( &szleg, "9", 1, hdc );
    if (try_cn18_font) {
        SIZE szf18;
        SelectObject(hdc,Get_CN18b_Font(hdc));
        get_text_size_len_hdc( &szf18, " 123456789 ", 11, hdc );
#ifdef ADD_HINT_TEXT
        g_hint_width = szf18.cx + 20;
#endif // #ifdef ADD_HINT_TEXT
    }
    SelectObject(hdc,hfold);
    g_leg_height = szleg.cy + 4;
    g_leg_width  = szleg.cx + 8; // look a bit SQEEZED at 6
#ifdef ADD_HINT_TEXT
    sprtf("Set legend h/w %d/%d hint h/w %d/%d\n", g_leg_height, g_leg_width, g_hint_height, g_hint_width);
#endif // #ifdef ADD_HINT_TEXT

#ifdef ADD_TEXT_BOX
    // take 2/3 for main rectangle, 1/3 for text output
    wid3 = width / 3;
#endif // #ifdef ADD_TEXT_BOX

    SIZE sz = g_szCN8Text;
    get_CN8_Text_Size( hdc, &sz );

#ifdef ADD_HINT_TEXT
    sudo_x = ((wid3 * 2) - g_hint_width) / 9;                         // one sudo x width
    if (add_bottom_legend) {
        sudo_y = (height - (2 * g_leg_height) - g_hint_height) / 9;        // one sudo y height
    } else {
        sudo_y = (height - (g_leg_height + g_vb_margin) - g_hint_height) / 9;        // one sudo y height
    }
#else
#ifdef ADD_TEXT_BLOCK
    sudo_x = (wid3 * 2) / 9;                         // one sudo x width
#else //#ifdef ADD_TEXT_BLOCK
    sudo_x = (width - (2 * g_leg_width)) / 9;
#endif // #ifdef ADD_TEXT_BLOCK
    if (add_bottom_legend) {
        sudo_y = (height - (2 * g_leg_height)) / 9;        // one sudo y height
    } else {
        sudo_y = (height - (g_leg_height + g_vb_margin)) / 9;        // one sudo y height
    }
#endif // #ifdef ADD_HINT_TEXT

    //if (wid3 > 50) {
    if ((sudo_x > min_width) && (sudo_y > min_height)) {

#ifdef ADD_HINT_TEXT
        //wid3 = (g_hl_margin + g_hr_margin) + (sudo_x * 9) + g_hint_width;
        wid3 = (2 * g_leg_width) + (sudo_x * 9) + g_hint_width;
#else // #ifdef ADD_HINT_TEXT
#ifdef ADD_TEXT_BLOCK
        wid3 = (2 * g_leg_width) + (sudo_x * 9) + g_leg_width;
#endif // #ifdef ADD_TEXT_BLOCK
#endif // #ifdef ADD_HINT_TEXT

#ifdef ADD_VERT_DIVIDE
        // vertical divide between main square and text block
        //d_x1 = wid3;
        //d_x2 = d_x1;
        //d_y1 = rc.top;
        //d_y2 = rc.bottom;
        g_rcVertDiv.left = wid3;
        g_rcVertDiv.right = g_rcVertDiv.left + vert_div_width;
        g_rcVertDiv.top = rc.top;
        g_rcVertDiv.bottom = rc.bottom;
#endif // #ifdef ADD_VERT_DIVIDE

        // pr->left   = sq_x4 + g_hl_margin;
        // pr->right  = pr->left + g_hint_width - g_hl_margin;
        // sq_x1 = rc.left + g_hl_margin;
        sq_x1 = rc.left + g_leg_width;
        sq_x2 = sq_x1;
        //sq_x3 = d_x1 - g_hr_margin - g_hint_width;
        //sq_x3 = g_rcVertDiv.left - g_hr_margin - g_hint_width;

#ifdef ADD_HINT_TEXT
        sq_x3 = g_rcVertDiv.left - g_leg_width - g_hint_width;
#else // #ifdef ADD_HINT_TEXT
        //sq_x3 = g_rcVertDiv.left - g_leg_width;
#endif // #ifdef ADD_HINT_TEXT

        sq_x4 = sq_x1 + (sudo_x * 9); //sq_x3; // + g_hl_margin;

        sq_y1 = rc.top + g_leg_height; // was g_vt_margin;
        sq_y2 = sq_y1 + (sudo_y * 9);
        //sq_y3 = sq_y2;
        sq_y4 = sq_y1;
        // d_x1 = wid3;
        // d_x2 = d_x1;
        // d_y1 = rc.top;
        // d_y2 = rc.bottom;
        // actual SUDOKU square (rectangle)
        g_rcSudoku.left = sq_x1;
        g_rcSudoku.top = sq_y1;
        g_rcSudoku.right = sq_x4;
        g_rcSudoku.bottom = sq_y2;

        // g_rcRegions[9]; // set of regions (boxes)
        int i;
        PRECT prc;
        for (i = 0; i < 9; i++) {
            prc = &g_rcRegions[i]; // SETTING regions - in Set_SIZES()
            prc->left   = sq_x1 + (((i % 3) % 3) * (sudo_x * 3));
            prc->top    = sq_y1 + ((i / 3) * (sudo_y * 3));
            prc->right  = prc->left + (sudo_x * 3);
            prc->bottom = prc->top  + (sudo_y * 3);
        }

        // now WITH legend
        g_rcSudokuLeg = g_rcSudoku;
        g_rcSudokuLeg.left   -= g_leg_width;
        g_rcSudokuLeg.right  += g_leg_width;
        g_rcSudokuLeg.top    -= g_leg_height;
        g_rcSudokuLeg.bottom += g_leg_height;

        // 4 corners of the display - TODO - needs to be fixed!!!
        prc = &g_rcCorners[cnr_TL];  // top LEFT
        prc->left   = g_rcSudokuLeg.left;
        prc->right  = prc->left + g_leg_width - 2;
        prc->top    = g_rcSudokuLeg.top;
        prc->bottom = prc->top + g_leg_height - 2;
        prc = &g_rcCorners[cnr_TR];  // top RIGHT
        prc->left   = g_rcSudokuLeg.right - g_leg_width + 2;
        prc->right  = prc->left + g_leg_width - 2;
        prc->top    = g_rcSudokuLeg.top;
        prc->bottom = prc->top + g_leg_height - 2;
        prc = &g_rcCorners[cnr_BL];  // bottom LEFT
        prc->left   = g_rcSudokuLeg.left;
        prc->right  = prc->left + g_leg_width - 2;
        prc->top    = g_rcSudokuLeg.bottom - g_leg_height + 2;
        prc->bottom = prc->top + g_leg_height + 2;
        prc = &g_rcCorners[cnr_BR];  // bottom RIGHT
        prc->left   = g_rcSudokuLeg.right - g_leg_width + 2;
        prc->right  = prc->left + g_leg_width - 2;
        prc->top    = g_rcSudokuLeg.bottom - g_leg_height + 2;
        prc->bottom = prc->top + g_leg_height + 2;

#ifdef ADD_MOUSE_RECT
        // Just for a LINE of mouse output
        // g_rcMouse.left   = d_x1 + g_hl_margin;
        g_rcMouse.left   = g_rcVertDiv.right + 10; // g_hl_margin;
        g_rcMouse.right  = g_rcClient.right  - 10; // g_hr_margin;
        g_rcMouse.bottom = g_rcClient.bottom - g_vb_margin;
        g_rcMouse.top    = rc.bottom - g_vb_margin - sz.cy;
#endif // #ifdef ADD_MOUSE_RECT

#ifdef ADD_LISTVIEW
        // position for LISTVIEW if USED
        g_rcListView.left  = wid3; // + h_margin;      // set LEFT
        g_rcListView.right = rc.right; // - h_margin; // set RIGHT
        g_rcListView.top   = rc.top; // + v_margin;
#ifdef ADD_MOUSE_RECT
        g_rcListView.bottom = g_rcMouse.top - g_vb_margin;
#else // #ifdef ADD_MOUSE_RECT
        g_rcListView.bottom = rc.bottom - g_vb_margin;
#endif // #ifdef ADD_MOUSE_RECT
#endif // #ifdef ADD_LISTVIEW

#ifdef ADD_TEXT_BLOCK
        // position for TEXT BLOCK, if no LISTVIEW
        g_rcText.left  = wid3 + g_hr_margin; // plus a small margin
        g_rcText.right = rc.right - g_hr_margin; // minus a small margin
        g_rcText.top   = rc.top + g_vb_margin; // plus a small margin
        //rcText.bottom = rcText.top + sz.cy;
        // this is actually the g_rcMouse.top!
        g_rcText.bottom = rc.bottom - g_vb_margin - sz.cy;
#endif // #ifdef ADD_TEXT_BLOCK

        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                x1 = sq_x1 + (sudo_x * col);
                x2 = sq_x1 + (sudo_x * (col+1));
                y1 = sq_y1 + (sudo_y * row);
                y2 = sq_y1 + (sudo_y * (row+1));
                pr2 = &rcSquares[row][col];
                pr2->top    = y1;
                pr2->bottom = y2;
                pr2->left   = x1;
                pr2->right  = x2;
            }
        }

        // set rectangle for paint horizontal lines
        x1 = sq_x1;
#ifdef ADD_TEXT_BLOCK
        x2 = wid3;  // sq_x3;
#else // #ifdef ADD_TEXT_BLOCK
        x2 = rc.right - g_leg_width;
#endif // #ifdef ADD_TEXT_BLOCK
        for (row = 0; row < 9; row++) {
            y1 = sq_y1 + (sudo_y * row);
            y2 = sq_y1 + (sudo_y * row);
#ifdef ADD_HINT_TEXT
            // fill in HORIZONTAL work pad rect - hints
            pr = &rcHHint[row]; // get for SETTING, in Set_SIZES(), by ROW 0-9 - horiz hint text
            //pr->left   = sq_x4 + g_hl_margin;
            //pr->right  = pr->left + g_hint_width - g_hr_margin - g_hl_margin;
            //pr->right  = pr->left + g_hint_width - g_hl_margin - g_hr_margin;
            pr->left   = sq_x4 + g_leg_width;
            pr->right  = pr->left + g_hint_width - (2 * g_leg_width);
            if (try_higher_horiz) {
                pr->top    = y1 + 1;
                pr->bottom = pr->top + sudo_y - 2;
            } else {
                pr->top    = y1 + g_vb_margin;
                pr->bottom = pr->top + sudo_y - (2 * g_vb_margin);
            }
#endif // #ifdef ADD_HINT_TEXT

            pr2 = &g_rcVLegL[row];  // get for SETTING, in Set_SIZES();
            pr2->left = g_rcClient.left;
            //pr2->right = pr2->left + g_hl_margin;
            pr2->right = pr2->left + g_leg_width;
            pr2->top = y1;
            pr2->bottom = pr2->top + sudo_y;
            //sprtf("%d: V1 RECT %s\n", row + 1, Get_Rect_Stg(pr2));
            pr3 = &g_rcVLegR[row]; // get for SETTING, in Set_SIZES() - vertical right number legend
            pr3->left = sq_x4;  // starts at right legend rectangle
            //pr3->right = pr3->left + g_hl_margin;
            pr3->right = pr3->left + g_leg_width;
            pr3->top = pr2->top;
            pr3->bottom = pr2->bottom;
            // sprtf("%d: V2 RECT %s\n", row + 1, Get_Rect_Stg(pr3));
        }

        // set rectangles for paint vertical lines
        y1 = sq_y1;
        y2 = sq_y2;
        for (col = 0; col < 9; col++) {
            x1 = sq_x1 + (sudo_x * col);
            x2 = sq_x1 + (sudo_x * col);

#ifdef ADD_HINT_TEXT
            // fill in VERTICAL work pad rect
            pr = &rcVHint[col]; // get for SETTING in Set_SIZES
            if (try_wider_verts) {
                pr->left   = x1 + 1; // left is left + margin
                pr->right  = pr->left + sudo_x - 2; // right is left + 1 sudo_x - 2 * margin
            } else {
                pr->left   = x1 + g_hr_margin; // left is left + margin
                pr->right  = pr->left + sudo_x - (2 * g_hr_margin); // right is left + 1 sudo_x - 2 * margin
            }
            if (add_bottom_legend) {
                pr->top    = y2 + g_leg_height; // top is bottom + legend heightn
                pr->bottom = g_rcClient.bottom - 1;
            } else {
                pr->top    = y2 + g_vb_margin; // top is bottom + margin
                pr->bottom = pr->top + g_hint_height - g_vb_margin; // bot is top + work - margin
            }
#endif // #ifdef ADD_HINT_TEXT

            // paint across the TOP 1 2 3 ...
            pr2 = &g_rcHLegT[col];  // SETTING of legend text top position in WM_SIZE
            pr2->left = x1;         // set LEFT
            pr2->right = pr2->left + sudo_x;
            pr2->top   = g_rcClient.top + 1;
            pr2->bottom = pr2->top + g_leg_height; // was g_vt_margin
            // and maybe paint across the BOTTOM 1 2 3 ...
            pr3 = &g_rcHLegB[col];  // SETTING of legend text bottom position in WM_SIZE
            pr3->left   = x1;
            pr3->right  = pr3->left + sudo_x;
            pr2 = &rcSquares[8][0];
            pr3->top    = pr2->bottom + 1;
            pr3->bottom = pr3->top + g_leg_height; // was g_vt_margin
        }

    }
    if (hfVert)
        DeleteObject(hfVert);
    hfVert = 0;

    if (hdc)
        ReleaseDC(hWnd,hdc);
}


// WM_SIZE
VOID Do_WM_SIZE( HWND hWnd, LPARAM lParam )
{
   //g_wm_size.cx = LOWORD(lParam);
   //g_wm_size.cy = HIWORD(lParam);   // width and height
   //SetScrollRanges(hWnd, LOWORD(lParam), HIWORD(lParam));
   //GetClientRect( hWnd, &g_rcClient );
   //int win_size = ((g_rcClient.right > g_rcClient.bottom) ?
   //   g_rcClient.right : g_rcClient.bottom );
   UpdateWP( hWnd );
   //double clon, clat;
   //get_g_center( &clon, &clat );
   //sprtf("WM_SIZE: set size = %d. center = %0.6f,%0.06f\n", win_size, clon, clat );
   //Do_ResizeStatus( hWnd );
   Set_SIZES(hWnd);

#ifdef ADD_LISTVIEW
   LVSize( hWnd, &g_rcListView );
#endif
}

//  fill border between g_rcClient g_rcFullClient;
void Paint_Border(HDC hdc)
{
    TRIVERTEX vertex[2] ;
    GRADIENT_RECT gRect;
    PRECT pro = &g_rcFullClient;
    PRECT pri = &g_rcClient;
    if (EqualRect(pro,pri))
        return;
    if ((pri->left <= pro->left) || (pri->top <= pro->top) ||
        (pri->right >= pro->right) || (pri->bottom >= pro->bottom))
        return;

    // Create an array of TRIVERTEX structures that describe 
    // positional and color values for each vertex. For a rectangle, 
    // only two vertices need to be defined: upper-left and lower-right. 

    // set up left vertical column
    vertex[0].x     = pro->left;
    vertex[0].y     = pro->top;
    vertex[0].Red   = 0x0000;
    vertex[0].Green = 0x8000;
    vertex[0].Blue  = 0x8000;
    vertex[0].Alpha = 0x0000;

    vertex[1].x     = pri->left;
    vertex[1].y     = pro->bottom; 
    vertex[1].Red   = 0x0000;
    vertex[1].Green = 0xd000;
    vertex[1].Blue  = 0xd000;
    vertex[1].Alpha = 0x0000;

    // Create a GRADIENT_RECT structure that 
    // references the TRIVERTEX vertices. 
    gRect.UpperLeft  = 0;
    gRect.LowerRight = 1;

    // Draw left vertical column as a shaded rectangle. 
    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

    // set up top horizontal row
    vertex[0].x     = pri->left;
    vertex[0].y     = pro->top;
    vertex[1].x     = pri->right;
    vertex[1].y     = pri->top; 
    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);

    // set up right vertical column
    vertex[0].x     = pri->right;
    vertex[0].y     = pro->top;
    vertex[0].Red   = 0x0000;
    vertex[0].Green = 0xd000;
    vertex[0].Blue  = 0xd000;
    vertex[0].Alpha = 0x0000;
    vertex[1].x     = pro->right;
    vertex[1].y     = pro->bottom; 
    vertex[1].Red   = 0x0000;
    vertex[1].Green = 0x8000;
    vertex[1].Blue  = 0x8000;
    vertex[1].Alpha = 0x0000;
    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

    // set up bottom horizontal row
    vertex[0].x     = pri->left;
    vertex[0].y     = pri->bottom;
    vertex[1].x     = pri->right;
    vertex[1].y     = pro->bottom; 
    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);


}

// COLORREF crPBlue = RGB(194,209,244); // good for square background - highlight_cells
void Paint_Highlights(HDC hdc)
{
    //if (!g_bShwSel)
    //    return;
    int row, col, val;
    PABOX2 pb = get_curr_box();
    HBRUSH hbsel = Get_Sel_Brush(); // Get_Solid_Brush(crPBlue);
    HBRUSH hboff = Get_Off_Brush();  // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    HBRUSH hbon  = Get_On_Brush();  // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
    HBRUSH hbunrect = Get_UnRect_Brush(); // Get_Solid_Brush(crBYellow); // = RG(248,252,121);
    HBRUSH *phbcolors = Get_Color_Brushes();
    HBRUSH hbfill = NULL;
    uint64_t cflag;
    char *tb = GetNxtBuf();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            cflag = pb->line[row].set[col].cellflg; // extract FLAG
            hbfill = 0;
            *tb = 0;
            if (g_bShwSel && (cflag & cl_CSC)) {
                hbfill = hbsel;
                strcat(tb,"Fill cl_CSC select ");
            }
            if (cflag & (cl_MCC|cl_CLRS)) {
                if (cflag & cl_CLRS) {
                    if (cflag & cl_CLR1) {
                        hbfill = phbcolors[0];
                        strcat(tb,"Fill cl_CLR1 ");
                    } else if (cflag & cl_CLR2) {
                        hbfill = phbcolors[1];
                        strcat(tb,"Fill cl_CLR2 ");
                    } else if (cflag & cl_CLR3) {
                        hbfill = phbcolors[2];
                        strcat(tb,"Fill cl_CLR3 ");
                    } else { // if (cflag & cl_CLR4) 
                        hbfill = phbcolors[3];
                        strcat(tb,"Fill cl_CLR4 ");
                    }
                } else {
                    if (pb->line[row].set[col].uval) {
                        hbfill = hboff;
                        strcat(tb,"Fill cl_MCC uvql off ");
                    } else {
                        hbfill = hbon;
                        strcat(tb,"Fill cl_MCC uvql on ");
                    }
                }
            }
            if (cflag & cl_UNR) {
                hbfill = hbunrect;
            }
            if (hbfill) {
                // Cell Contains Selected Candidate
                // OR is member of COLOR CLUB
                RECT rc = rcSquares[row][col];
                rc.left += 1;
                rc.right -= 1;
                rc.top += 1;
                rc.bottom -= 1;
                FillRect(hdc,&rc,hbfill); // TO Pant_Highlights() into rcSquares[row][col] OUTITPFR
                sprintf(EndBuf(tb),"rect %s ",Get_Rect_Stg(&rc));
                OUTITPFR(tb); // Paint_Highlights() PFR
            }
        }
    }
}


void Paint_Link_Colors(HDC hdc)
{
    if (!Is_SLP_Valid()) return;
    vRCP *pvrcp = Get_SL_Pairs(); // TO Paint_Link_Colors()
    size_t max = pvrcp->size();
    if (!max) return;
    size_t ii;
    PRCPAIR prcpair;
    PROWCOL prc1, prc2;
    uint64_t flg1, flg2;
    int setval;
    char *tb = GetNxtBuf();
    HBRUSH hbsel = Get_Sel_Brush(); // Get_Solid_Brush(crPBlue);
    HBRUSH hboff = Get_Off_Brush();  // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    HBRUSH hbon  = Get_On_Brush();  // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
    HBRUSH hbunrect = Get_UnRect_Brush(); // Get_Solid_Brush(crBYellow); // = RG(248,252,121);
    HBRUSH hbfill = NULL;
    RECT rc;
    vRC vdone;
    for (ii = 0; ii < max; ii++)
    {
        prcpair = &pvrcp->at(ii);
        prc1 = &prcpair->rowcol[0];
        prc2 = &prcpair->rowcol[1];
        setval = prc1->set.uval;
        flg1 = prc1->set.flag[setval - 1] & (cf_XCA|cf_XCB);
        //setval = prc2->set.uval;
        flg2 = prc2->set.flag[setval - 1] & (cf_XCA|cf_XCB);
        //if ( (prc->set.flag[i] & (cf_XCA|cf_XCB)) == (cf_XCA|cf_XCB) ) // BOTH COLORS ON - eek
        //   crbkold = SETBkColor( hdc, crLOrange);   // TODO - need another color
        //else if (prc->set.flag[i] & cf_XCA) // get the COLOR for the candidate
        //   crbkold = SETBkColor( hdc, crLGreen );
        //else if (prc->set.flag[i] & cf_XCB)
        //   crbkold = SETBkColor( hdc, crCyan );
        //else
        //   crbkold = SETBkColor( hdc, crLPink );   // TODO - need another color
        hbfill = 0;
        sprintf(tb,"Paint_Link_Colors:1: ");
        if (flg1) {
            if ( flg1 == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
                hbfill = hbsel;
                strcat(tb,"BOTH eek ");
            } else if (flg1 & cf_XCA) {
                hbfill = hbon;
                strcat(tb,"Clr A ");
            } else if (flg1 & cf_XCB) {
                hbfill = hboff;
                strcat(tb,"Clr B ");
            } else {
                strcat(tb,"Clr UNK ");
            }
        } else {
            hbfill = hbunrect;
            strcat(tb,"NF ");
        }
        if (hbfill && !prc_in_chain(prc1,&vdone)) {
            rc = rcSquares[prc1->row][prc1->col];
            rc.left += 1;
            rc.right -= 1;
            rc.top += 1;
            rc.bottom -= 1;
            FillRect(hdc,&rc,hbfill); // TO Paint_Link_Colors() into rcSquares[row][col] - OUTITPFR
            sprintf(EndBuf(tb),"Rect %s ", Get_Rect_Stg(&rc));
            OUTITPFR(tb);
        }
        hbfill = 0;
        sprintf(tb,"Paint_Link_Colors:2: ");
        if (flg2) {
            if ( flg2 == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
                hbfill = hbsel;
                strcat(tb,"BOTH eek ");
            } else if (flg2 & cf_XCA) {
                hbfill = hbon;
                strcat(tb,"Clr A ");
            } else if (flg2 & cf_XCB) {
                hbfill = hboff;
                strcat(tb,"Clr B ");
            } else {
                strcat(tb,"Clr UNK ");
            }
        } else {
            hbfill = hbunrect;
            strcat(tb,"NF ");
        }
        if (hbfill && !prc_in_chain(prc2,&vdone)) {
            rc = rcSquares[prc2->row][prc2->col];
            rc.left += 1;
            rc.right -= 1;
            rc.top += 1;
            rc.bottom -= 1;
            FillRect(hdc,&rc,hbfill); // TO Paint_Highlights() into rcSquares[row][col] OUTITPFR
            sprintf(EndBuf(tb),"Rect %s ", Get_Rect_Stg(&rc));
            OUTITPFR(tb);
        }
        vdone.push_back(*prc1);
        vdone.push_back(*prc2);
    }
}


void DEBUG_paint_rects(HDC hdc)
{
    int row, col, val;
    uint64_t cellflg;
    PABOX2 pb = get_curr_box();
    HFONT hf, hfold;
    PRECT prc;
    char buf[MAX_STRING];
    char *tb = buf;
    int len;
    UINT format = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
    UINT format2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
    SET set;
    int offset = 2;
    PSET ps;
    COLORREF crtold;
    COLORREF crbold;
    HFONT hf18 = 0;
    HFONT hf18old = 0;

    if (try_cn18_font)
        hf18 = Get_CN18b_Font(hdc);
    // TODO - How to decide the SIZE
    //hf = Get_CN8_Font( hdc );
    hf = Get_CN24b_Font( hdc );
    hfold = (HFONT)SelectObject(hdc,hf);
    one_value_count = 0;
    NO_value_count = 0;
    total_empty_count = 0;
    // Keep list of TWO types of CELLS with NO VALUE
    // 1. Cells that have NO potential possibilities - this means the puzzle is LOCKED, BAD, FAILING
    // 2. Cells that have just ONE potential value - maybe have key to fill these in...
    for (row = 0; row < 9; row++) {
        Fill_SET(&set); // start with ALL values ON for this ROW
        // process COL by COL
        for (col = 0; col < 9; col++) {
            if (g_bOddBox) {
                if (GETBOX(row,col) & 1)
                    crbold = SETBkColor(hdc, crOddBox);
                else
                    crbold = SETBkColor(hdc, crEvnBox);
            }
            val = pb->line[row].val[col];
            ps = &pb->line[row].set[col];
            cellflg = pb->line[row].set[col].cellflg;
            prc = &rcSquares[row][col];
            if (val) {
                // PAINT A VALUE
                if (cellflg & cl_Bad) {
                    crtold = SetTextColor(hdc, crBadVal);
                } else if (cellflg & cl_New) {
                    crtold = SetTextColor(hdc, crNewVal);
                } else {
                    crtold = SetTextColor(hdc, crOrgVal);
                }
                len = sprintf(tb,"%d",val);
                DrawText( hdc, // handle to DC
                    tb,         // text to draw
                    len,        // text length
                    prc,        // formatting dimensions
                    format );   // text-drawing options
                set.val[val - 1] = 0;   // clear potential values
                Zero_SET(ps);           // MAYBE should NOT do this, but for now...
                SetTextColor(hdc, crtold);
            } else {
                total_empty_count++;

                if (cellflg & cf_XW)
                    Paint_XW_X( hdc, prc, cellflg );
                else if (cellflg & cf_YWC)
                    Paint_YW_Y( hdc, prc, cellflg );

                Paint_Set(hdc, prc, ps, pb, row, col ); // paint candidates (with color)
            }
        }   // for (col = 0; col < 9; col++)
        // Done all columns
        if (try_cn18_font)
            hf18old = (HFONT)SelectObject(hdc,hf18);

        if (try_cn18_font)
            SelectObject(hdc,hf18old);
    }   // for each row

    SelectObject(hdc,hfold);

}

void Paint_Corners( HDC hdc )
{
    // HBRUSH hbodd = Get_Odd_Brush(); // if (!hbOdd) hbOdd = Get_Solid_Brush(cfOddBox); // = RGB(200,200,200);
    HBRUSH hbodd = Get_Sel_Brush();
    PRECT prc;
    prc = &g_rcCorners[cnr_TL]; // top left
    FillRect(hdc,prc,hbodd);
    prc = &g_rcCorners[cnr_TR]; // top right
    FillRect(hdc,prc,hbodd);
    prc = &g_rcCorners[cnr_BL]; // bottom left
    FillRect(hdc,prc,hbodd);
    prc = &g_rcCorners[cnr_BR]; // bottom right
    FillRect(hdc,prc,hbodd);
}

int test_sudo_rect = 0;
VOID Do_WM_PAINT( HWND hWnd )
{
	PAINTSTRUCT ps;
	HDC hdc, hdcORG;
    int row, col;
    int x1,y1,x2,y2;
    PRECT pr;
    bool use_copy = false;
    HBITMAP bitmap = 0;
    int width = g_rcFullClient.right - g_rcFullClient.left;
    int height = g_rcFullClient.bottom - g_rcFullClient.top;
    HBRUSH hbodd = Get_Odd_Brush(); // if (!hbOdd) hbOdd = Get_Solid_Brush(cfOddBox); // = RGB(200,200,200);

    if (g_bCopyHDC) {
    	hdcORG = BeginPaint(hWnd, &ps);
        hdc = CreateCompatibleDC(hdcORG);
        if( !hdc ) {
            hdc = hdcORG;
        } else {
            bitmap = CreateCompatibleBitmap( hdcORG, width, height );
            if( !bitmap ) {
                DeleteDC(hdc);
                hdc = hdcORG;
            } else {
                SelectObject(hdc,bitmap);
                // BitBlt(hdc,0,0,width,height,hdcORG,0,0,SRCCOPY);
                // 	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
                FillRect(hdc,&g_rcFullClient,(HBRUSH)(COLOR_WINDOW+1));
                use_copy = true;
            }
        }
    } else {
      	hdc = BeginPaint(hWnd, &ps);
    }

    Paint_Border(hdc);

    if ((sudo_x > min_width) && (sudo_y > min_height)) {

        HPEN hbp4 = Get_LBrown_Pen_4(); // Border and box borders
        HPEN hbp1 = Get_LBrown_Pen_1(); // intermediate lines
        HPEN hpOld = 0;

        if (hbp4)
            hpOld = (HPEN)SelectObject(hdc, hbp4);

#ifdef ADD_VERT_DIVIDE
        // draw vertical divide
        draw_line( hdc, g_rcVertDiv.left, g_rcVertDiv.top, g_rcVertDiv.left, g_rcVertDiv.bottom );
#endif // #ifdef ADD_VERT_DIVIDE
#ifdef ADD_MOUSE_RECT
        // horizontal top of mouse position box
        draw_line( hdc, g_rcVertDiv.right, g_rcMouse.top - g_vb_margin, g_rcClient.right, g_rcMouse.top - g_vb_margin ); 
#endif // #ifdef ADD_MOUSE_RECT

        if (g_bOddBox) {
            // Fill the regions with a backgound color
            // g_rcRegions[9]; // set of regions (boxes)
            int i;
            PRECT prc;
            for (i = 0; i < 9; i++) {
                prc = &g_rcRegions[i];
                if (i & 1) {
                    FillRect(hdc,prc,hbodd); // TO Do_WM_PAINT() fill ODD g_rcRegions[i] (BOXES) with soft gray
                }
            }
        }

        // paint horizontal lines
        x1 = sq_x1;
        //x2 = sq_x3;
        x2 = sq_x4;
        for (row = 0; row <= 9; row++) {
            y1 = sq_y1 + (sudo_y * row);
            y2 = sq_y1 + (sudo_y * row);
            if (row % 3)
                SelectObject(hdc,hbp1);
            else
                SelectObject(hdc,hbp4);
            draw_line( hdc, x1, y1, x2, y2 );
        }
        
        // paint vertical lines
        y1 = sq_y1;
        y2 = sq_y2;
        for (col = 0; col <= 9; col++) {
            x1 = sq_x1 + (sudo_x * col);
            x2 = sq_x1 + (sudo_x * col);
            if (col % 3)
                SelectObject(hdc,hbp1);
            else
                SelectObject(hdc,hbp4);
            draw_line( hdc, x1, y1, x2, y2 );
        }

        if (test_sudo_rect) {
            HPEN hpo2 = (HPEN)SelectObject(hdc,Get_Purp_Pen());
            Draw_Rectangle( hdc, &g_rcSudoku, -2);
            SelectObject(hdc,hpo2);
        }

        if (g_bDbgPntRects) {
            DEBUG_paint_rects(hdc);
            goto End_Paint;
        }

        // COLORREF crPBlue = RGB(194,209,244); // good for square background - highlight_cells
        Paint_Highlights(hdc);
        Paint_Link_Colors(hdc); // of Get_SL_Pairs()

        hbp1 = Get_LBlue_Pen();
        SelectObject(hdc,hbp1);
        for (row = 0; row < 9; row++) {
#ifdef ADD_HINT_TEXT
            pr = &rcHHint[row]; // get for PAINTING rect by ROW 0-9
            if (paint_hint_rectangles)
                Draw_Rectangle(hdc,pr);
#endif // #ifdef ADD_HINT_TEXT
            if (add_Legend_rect) {
                pr = &g_rcHLegT[row];   // ADD legend rect, if (add_Legend_rect)
                Draw_Rectangle(hdc,pr);
                if (add_bottom_legend) {
                    pr = &g_rcHLegB[row];   // ADD legend rect, if (add_Legend_rect && add_bottom_legend)
                    Draw_Rectangle(hdc,pr);
                }
            }
        }
        for (col = 0; col < 9; col++) {
#ifdef ADD_HINT_TEXT
            pr = &rcVHint[col]; // get for PAINTING rect by COL 0-9 - hint text
            if (paint_hint_rectangles)
                Draw_Rectangle(hdc,pr);
#endif // #ifdef ADD_HINT_TEXT
            if (add_Legend_rect) {
                pr = &g_rcVLegL[col]; // get for PAINTING rect, by COL 0-9 - vertical left letter legend
                Draw_Rectangle(hdc,pr);
                pr = &g_rcVLegR[col]; // get for PAINTING rect, by COL 0-9 - vertical right number legend
                Draw_Rectangle(hdc,pr);
            }
        }

        if (hpOld)
            SelectObject(hdc,hpOld);

        Paint_Corners(hdc);

#ifdef ADD_TEXT_BLOCK
        Draw_Top_Text(hdc);
#endif // #endif // #ifdef ADD_TEXT_BLOCK

        if (is_curr_box_valid_for_paint()) {

            Paint_Colors(hdc);

            Paint_Box(hdc); // paint in current values
        }
    }

End_Paint:

    Paint_Selected(hdc);
    Paint_Legend_Text(hdc);
    if (use_copy) {
        //int wid = ps.rcPaint.right - ps.rcPaint.left;
        //int hgt = ps.rcPaint.bottom - ps.rcPaint.top;
        BitBlt(hdcORG,0,0,width,height,hdc,0,0,SRCCOPY);
        DeleteDC(hdc);
    }
    if (bitmap)
        DeleteObject(bitmap);
	EndPaint(hWnd, &ps);
    done_paint = 1;
}

#ifdef ADD_HINT_TEXT

void Trim_Work_Strings( HDC hdc, int row, int col )
{
    PABOX2 pb = get_curr_box();
    int val = pb->line[row].val[col];
    char buf[MAX_STRING];
    char *ps = buf;
    int offset = 2;
    UINT format2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
    if (!val) {
        PRECT prsqr = &rcSquares[row][col];
        char *cpr = cpHWork[row];  // ROWS
        char *cpc = cpVWork[col];  // COLUMNS
        SET hset, vset;
        int len,i,c,velim,helim;
        len = (int)strlen(cpr);
        curr_HRemainder = len;  // this is BEFORE elimination
        for (i = 0; i < 9; i++) {
            hset.val[i] = 0;
        }
        for (i = 0; i < len; i++) {
            c = cpr[i];
            val = c - '0';
            hset.val[val - 1] = val;
        }
        len = (int)strlen(cpc);
        curr_VRemainder = len;  // this is BEFORE elimination
        for (i = 0; i < 9; i++) {
            vset.val[i] = 0;
        }
        for (i = 0; i < len; i++) {
            c = cpc[i];
            val = c - '0';
            vset.val[val - 1] = val;
        }
        helim = 0;
        helim += Eliminate_Same_Col( pb, col, &hset );
        helim += Eliminate_Same_Row( pb, row, &hset );
        helim += Eliminate_Same_Box( row, col, pb, &hset );
        velim = 0;
        velim += Eliminate_Same_Col( pb, col, &vset );
        velim += Eliminate_Same_Row( pb, row, &vset );
        velim += Eliminate_Same_Box( row, col, pb, &vset );
        // this should ALWAYS be TRUE, shouldn't it???
        if (Same_SET(&hset,&vset) && use_elim_on_square) {
            if (Eliminate_on_Square( row, col, pb, &vset, &hset )) {  // use_elim_on_square AND Eliminate_Square_Values
                helim++;
                velim++;
            }
            if ( !(Get_Set_Cnt(&vset) < 2) ) {
                if (Eliminate_Unique_In_Row( row, col, pb, &vset, &hset )) {  // use_elim_on_square AND Eliminate_Square_Values
                    helim++;
                    velim++;
                }

            }
        }
        memcpy(&g_hSet,&hset,sizeof(SET));
        memcpy(&g_vSet,&vset,sizeof(SET));
        if (helim || velim || (curr_HRemainder < m_color_cnt) || (curr_VRemainder < m_color_cnt)) {
            HFONT hf;
            if (try_cn18_font)
                hf = Get_CN18b_Font(hdc);
            else
                hf = Get_CN24b_Font(hdc);
            HFONT hfold = (HFONT)SelectObject(hdc,hf);
            PRECT prc;
            RECT  rc;
            COLORREF cr;
            SIZE sz;
            char buf[MAX_STRING];
            int off, len2, width;
            char *cp; // use upper part of buffer
            char *cp2; // moving pointer

            get_text_size_len_hdc( &sz, "9", 1, hdc );
            if (helim || (curr_HRemainder < m_color_cnt)) {

                prc = &rcHHint[row]; // get for PAINTING hint values trimmed - Trim_Work_Strings on hover
                rc = *prc;
                rc.left += offset;  // bump text in from drawn margin
                *ps = 0;
                len = (int)strlen(cpr);
                while(len--) strcat(ps," ");
                len = (int)strlen(ps);
                if (len) {
                    DrawText( hdc, // handle to DC
                        ps,         // text to draw
                        len,        // text length
                        &rc,        // formatting dimensions
                        format2 );   // text-drawing options
                }
                *ps = 0;
                for (i = 0; i < 9; i++) {
                    val = hset.val[i]; // final eliminated set
                    if (val) {
                        sprintf(EndBuf(ps),"%d",val);
                    }
                }
                len = (int)strlen(ps);
                curr_HRemainder = len;
                if (len) {
                    cr = GetColorPerCount(len);
                    cr = SetTextColor(hdc,cr);
                    DrawText( hdc, // handle to DC
                        ps,         // text to draw
                        len,        // text length
                        &rc,        // formatting dimensions
                        format2 );   // text-drawing options
                    if (cr != CLR_INVALID)
                        SetTextColor(hdc,cr);
                }
            }

            if (velim || (curr_VRemainder < m_color_cnt)) {
                prc = &rcVHint[col]; // get for PAINTING 'trimmed' text, by COLUMN
                rc = *prc;
                rc.left += offset;  // bump text in from drawn margin
                rc.top  += offset;
                len = (int)strlen(cpc);
                hf = Get_Font_For_Vertical_Boxes( hdc, &rc, &sz, cpc );
                if (hf)
                    SelectObject(hdc,hf);
                *ps = 0;
                len = (int)strlen(cpc);
                while(len--) strcat(ps," ");
                len = (int)strlen(ps);
                if (len) {
                    // adjust text 
                    off = 0;
                    width = rc.right - rc.left - offset;
                    cp = &buf[len+1]; // use upper part of buffer
                    cp2 = cp; // moving pointer
                    format2 = DT_LEFT | DT_TOP;
                    for (i = 0; i < len; i++) {
                        cp2[off++] = ps[i];    // move a character
                        cp2[off] = 0;
                        len2 = (int)strlen(cp2);
                        if (((len2 + 1) * sz.cx) > width) {
                            cp2[off++] = '\n'; // add break
                            cp2[off] = 0;       // zero terminate
                            cp2 = &cp2[off];    // move pointer to next line...
                            off = 0;            // restart offset on this line
                        }
                    }
                    len = (int)strlen(cp);
                    DrawText( hdc, // handle to DC
                        cp,         // text to draw
                        len,        // text length
                        &rc,        // formatting dimensions
                        format2 );   // text-drawing options
                }
                *ps = 0;
                for (i = 0; i < 9; i++) {
                    val = vset.val[i]; // was fill a full set of 9 value and eliminated non-valid
                    if (val) {
                        sprintf(EndBuf(ps),"%d",val);
                    }
                }
                len = (int)strlen(ps);
                curr_VRemainder = len;
                if (len) {
                    cr = GetColorPerCount(len);
                    // adjust text 
                    off = 0;
                    width = rc.right - rc.left - offset;
                    cp = &buf[len+1]; // use upper part of buffer
                    cp2 = cp; // moving pointer
                    format2 = DT_LEFT | DT_TOP;
                    for (i = 0; i < len; i++) {
                        cp2[off++] = ps[i];    // move a character
                        cp2[off] = 0;
                        len2 = (int)strlen(cp2);
                        if (((len2 + 1) * sz.cx) > width) {
                            cp2[off++] = '\n'; // add break
                            cp2[off] = 0;       // zero terminate
                            cp2 = &cp2[off];    // move pointer to next line...
                            off = 0;            // restart offset on this line
                        }
                    }
                    len = (int)strlen(cp);
                    cr = SetTextColor(hdc,cr);
                    DrawText( hdc, // handle to DC
                        cp,         // text to draw
                        len,        // text length
                        &rc,        // formatting dimensions
                        format2 );   // text-drawing options
                    if (cr != CLR_INVALID)
                        SetTextColor(hdc,cr);
                }
            }

            if (hfold) SelectObject(hdc,hfold);
        }
    }
}

void Paint_Row_Col_Text( HDC hdc, int row, int col )
{
    PRECT prc;
    RECT rc;
    int offset = 2;
    char *ps;
    int len;
    UINT format2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
    if (VALIDRC(row,col)) {
        HFONT hf;
        if (try_cn18_font) {
            hf = Get_CN18b_Font(hdc);
        } else {
            hf = Get_CN24b_Font( hdc );
        }
        HFONT hfold = (HFONT)SelectObject(hdc,hf);
        // // ROWS
        prc = &rcHHint[row]; // get for PAINTING hint values normal values
        rc = *prc;
        rc.left += offset;  // bump text in from drawn margin
        ps = cpHWork[row];  // ROWS
        len = (int)strlen(ps);
        if (len) {
            DrawText( hdc, // handle to DC
                ps,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format2 );   // text-drawing options
        }
        // // COLUMNS
        prc = &rcVHint[col];    // get for PAINTING in Paint_Row_Col_Text()
        rc = *prc;
        rc.left += offset;  // bump text in from drawn margin
        rc.top += offset;
        ps = cpVWork[col];  // COLUMNS
        len = (int)strlen(ps);
        if (len) {
            // adjust text 
            char buf[MAX_STRING];
            int off = 0;
            int len2;
            int width = rc.right - rc.left - offset;
            char *cp = &buf[len+1]; // use upper part of buffer
            char *cp2 = cp; // moving pointer
            SIZE sz;
            get_text_size_len_hdc( &sz, "9", 1, hdc );
            hf = Get_Font_For_Vertical_Boxes( hdc, &rc, &sz, ps );
            if (hf)
                SelectObject(hdc,hf);
            format2 = DT_LEFT | DT_TOP; // change format
            for (row = 0; row < len; row++) {
                cp2[off++] = ps[row];    // move a character
                cp2[off] = 0;
                len2 = (int)strlen(cp2);
                if (((len2 + 1) * sz.cx) > width) {
                    cp2[off++] = '\n'; // add break
                    cp2[off] = 0;       // zero terminate
                    cp2 = &cp2[off];    // move pointer to next line...
                    off = 0;            // restart offset on this line
                }
            }
            len = (int)strlen(cp);
            DrawText( hdc, // handle to DC
                cp,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format2 );   // text-drawing options
        }
        if(hfold) SelectObject(hdc,hfold);
    }
}

#endif // #ifdef ADD_HINT_TEXT

bool Is_Selected_Valid()
{
    if (IsRectEmpty(get_curr_sel()))
        return false;
    return true;
}
bool Is_Hover_Valid()
{
    if (IsRectEmpty(&g_rcHover))
        return false;
    return true;
}

void Paint_Outline(HWND hWnd, PRECT prt, bool found, int row, int col)
{
    PRECT prh = &g_rcHover;
    if (found && EqualRect( prt, prh ))
        return;
    HDC hdc = GetDC(hWnd);
    if (hdc) {
        HPEN hp;
        HPEN hpOld = 0;
        if ( Is_Hover_Valid() ) {
            hp = Get_WHITE_Pen_1();
            if (hp) {
                hpOld = (HPEN)SelectObject(hdc,hp);
                Draw_Rectangle(hdc,prh);
                if (hpOld) SelectObject(hdc,hpOld);
            }
            SetRectEmpty(prh);
            hpOld = 0;
#ifdef ADD_HINT_TEXT
            // should also put back the text
            Paint_Row_Col_Text( hdc, g_hov_Row, g_hov_Col );
#endif // #ifdef ADD_HINT_TEXT

        }
        if (found) {
            PABOX2 pb = get_curr_box();
            hp = 0;
            if (VALIDRC(row,col) && pb->line[row].val[col])
                hp = Get_LGreen_Pen();
            else
                hp = Get_RED_Pen_1();
            if (hp) {
                if (hpOld)
                    SelectObject(hdc,hp);
                else
                    hpOld = (HPEN)SelectObject(hdc,hp);
                Draw_Rectangle(hdc,prt);
                // keep what is drawn
                // ==================
                *prh = *prt;
                g_hov_Row = row;
                g_hov_Col = col;
                // ==================
            }
        }
        if (hpOld)
            SelectObject(hdc,hpOld);

#ifdef ADD_HINT_TEXT
        if (found && VALIDRC(row,col)) {
            Trim_Work_Strings( hdc, row, col );
        }
#endif // #ifdef ADD_HINT_TEXT

        ReleaseDC(hWnd,hdc);
    }
}

void Paint_Selected( HDC hdc )
{
    PRECT prs = get_curr_sel(); // &g_rcSelected;  // To PAINT slected
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        HPEN hp, hpold = 0;
        PABOX2 pb = get_curr_box();
        if (pb->line[row].val[col])
            hp = Get_Green_Pen_2();
        else
            hp = Get_Blue_Pen_2();
        if (hp)
            hpold = (HPEN)SelectObject(hdc,hp);
        Draw_Rectangle(hdc,prs);
        if (hpold) SelectObject(hdc,hpold);
    }
}

void Clear_Cell_Flag( PABOX2 pb, uint64_t flag )
{
    int row, col;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            pb->line[row].set[col].cellflg &= ~flag;  // clear ALL color selection flag
        }
    }
}

int Mark_HighLight_Cells(bool outlog)
{
    int count = 0;
    int sval = 0;
    int tested = 0;
    int full = 0;
    // if (g_bShwSel && Is_Selected_Valid()) {
    if (Is_Selected_Valid()) {
        PSET ps;
        int srow, scol;
        PABOX2 pb = get_curr_box();
        get_curr_selRC( &srow, &scol );
        if (VALIDRC(srow,scol))
            sval = pb->line[srow].val[scol];
        if (sval) {
            int row, col, val, i;
            // clear Cell Contains Candidate
            Clear_Cell_Flag(pb,cl_CSC);
            for (row = 0; row < 9; row++) {
                for (col = 0; col < 9; col++) {
                    val = pb->line[row].val[col];
                    if (val) continue; // got a VALUE - skip it
                    tested++;
                    ps = &pb->line[row].set[col];
                    if (Get_Set_Cnt(ps) == 9) {
                        full++;
                        if (g_bSkipAll) {
                            continue;
                        } else {
                            pb->line[row].set[col].cellflg |= cl_CSC; // Mark_HighLight_Cells
                            count++;
                        }
                    } else {
                        for (i = 0; i < 9; i++) {
                            val = ps->val[i];
                            if (val == sval) {
                                pb->line[row].set[col].cellflg |= cl_CSC; // Mark_HighLight_Cells
                                count++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    //if (outlog) {
    //    if (g_bSkipAll && full && (full == tested)) {
    //        sprtf("Sel %d all on %d\n",sval, full);
    //    } else if (sval || count ) {
    //        sprtf("Sel %d set on %d cells\n",sval, count);
    //    }
    //}
    set_repaint();
    return count;
}


void Set_New_Selected(HWND hWnd, PRECT prt, bool found, int row, int col)
{
    PRECT prs = get_curr_sel(); // &g_rcSelected; // to SET new SELECTION
    int srow = g_sel_Row;
    int scol = g_sel_Col;
    if (found && EqualRect( prt, prs ))
        return;
    HDC hdc = GetDC(hWnd);
    if (hdc) {
        HPEN hp;
        HPEN hpOld = 0;
        if ( Is_Selected_Valid() ) {
            hp = Get_WHITE_Pen_2();
            if (hp) {
                hpOld = (HPEN)SelectObject(hdc,hp);
                Draw_Rectangle(hdc,prs);
                if (hpOld) SelectObject(hdc,hpOld);
            }
            SetRectEmpty(prs);
            hpOld = 0;
#ifdef ADD_HINT_TEXT
            // should also put back the text
            if (VALIDRC(srow,scol))
                Paint_Row_Col_Text( hdc, srow, scol );
#endif // #ifdef ADD_HINT_TEXT

        }
        if (found) {
            *prs = *prt;
            set_curr_selRC( row, col );
            Paint_Selected(hdc);
        }
        if (hpOld)
            SelectObject(hdc,hpOld);

#ifdef ADD_HINT_TEXT
        if (found && VALIDRC(row,col)) {
            Trim_Work_Strings( hdc, row, col );
        }
#endif // #ifdef ADD_HINT_TEXT

        ReleaseDC(hWnd,hdc);
    }
    Mark_HighLight_Cells(); // mark hightlight - cell had candidate same as selection
}

void Move_Selection(HWND hWnd, int row, int col)
{
    if (VALIDRC(row,col)) {
        PRECT prs = &rcSquares[row][col];
        int offset = 4;
        RECT rc = *prs;
        rc.left += offset;
        rc.bottom -= offset;
        rc.top += offset;
        rc.right -= offset;
        Paint_Outline(hWnd, &rc, true, row, col);
        rc.left += offset;
        rc.bottom -= offset;
        rc.top += offset;
        rc.right -= offset;
        Set_New_Selected(hWnd, &rc, true, row, col);
    }
}

// Maybe left, with ROW change also
int do_move_wrap = 1;   // change both row and col if required
int sel_if_not_sel = 1; // on move request make selection if none

void Move_Selection_Left(HWND hWnd)
{
    PRECT prs = get_curr_sel(); // &g_rcSelected; // to MOVE selection LEFT
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        if (col) {
            col--;
        } else {
            col = 8;
            if (do_move_wrap) {
                if (row)
                    row--;  // going left - go up
                else
                    row = 8;
            }
        }
        Move_Selection(hWnd, row, col);
    } else if (sel_if_not_sel) {
        // select bottom right
        row = 8;
        col = 8;
        Move_Selection(hWnd, row, col);
    }
}
void Move_Selection_Up(HWND hWnd)
{
    PRECT prs = get_curr_sel(); // &g_rcSelected; // to MOVE selection UP
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        if (row) {
            row--;
        } else {
            row = 8;
            if (do_move_wrap) {
                if (col)
                    col--;
                else
                    col = 8;
            }
        }
        Move_Selection(hWnd, row, col);
    } else if (sel_if_not_sel) {
        // got up and NO slection, select bottom right
        row = 8;
        col = 8;
        Move_Selection(hWnd, row, col);
    }
}
void Move_Selection_Down(HWND hWnd)
{
    PRECT prs = get_curr_sel(); // &g_rcSelected; // to MOVE selection DOWN
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        row++;
        if (row >= 9) {
            row = 0;
            if (do_move_wrap) {
                col++;  // got to next column
                if (col >= 9)
                    col = 0;
            }
        }
        Move_Selection(hWnd, row, col);
    } else if (sel_if_not_sel) {
        // got down and NO slection, select top left
        row = 0;
        col = 0;
        Move_Selection(hWnd, row, col);
    }
}

void Move_Selection_Right(HWND hWnd)
{
    PRECT prs = get_curr_sel(); // &g_rcSelected; // to MOVE slection RIGHT
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        col++;
        if (col >= 9) {
            col = 0;
            if (do_move_wrap) {
                row++;
                if (row >= 9)
                    row = 0;
            }
        }
        Move_Selection(hWnd, row, col);
    } else if (sel_if_not_sel) {
        // got right and NO slection, select top left
        row = 0;
        col = 0;
        Move_Selection(hWnd, row, col);
    }
}


ROWCOL sAct_RC;
PRECT  prHover = NULL;
bool Check_Curr_Position()
{
    int xPos = curr_xPos;
    int yPos = curr_yPos;
    int row, col;
    bool found = false;
    PRECT prt;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            prt = &rcSquares[row][col];
            if ((xPos >= prt->left) &&
                (xPos <= prt->right) &&
                (yPos >= prt->top) &&
                (yPos <= prt->bottom)) {
                found = true;
                sAct_RC.row = row;
                sAct_RC.col = col;
                prHover = prt;
                break;  // end inner COLUMNS
            }
        }
        if (found)
            break;  // end outer ROWS
    }
    return found;
}

bool Check_Hover_Position( HWND hWnd )
{
    bool found = Check_Curr_Position();
    PRECT prt = prHover;
    int row = sAct_RC.row;
    int col = sAct_RC.col;
    if ((found || clear_hover) && prt && VALIDRC(row,col) ) {
        RECT rc = *prt;
        int offset = 4;
        rc.left += offset;
        rc.bottom -= offset;
        rc.top += offset;
        rc.right -= offset;
        Paint_Outline(hWnd, &rc, found, row, col);
        if (g_bLBDown && found) {
            rc.left += offset;
            rc.bottom -= offset;
            rc.top += offset;
            rc.right -= offset;
            Set_New_Selected(hWnd, &rc, found, row, col);
        }
    }
    return found;
}

int Check_For_Insertion()
{
    int iret = 0;
    int row = g_sel_Row;
    int col = g_sel_Col;
    if (Is_Selected_Valid() && VALIDRC(row,col)) {
        int nval1 = 0;
        int nval2 = 0;
        int cnt1 = 0;
        int cnt2 = 0;
        if ((curr_HRemainder == 1) && (curr_VRemainder == 1) &&
            Same_SET(&g_hSet,&g_vSet) &&
            ((cnt1 = Get_Set_Cnt(&g_hSet,&nval1)) == 1)&&
            ((cnt2 = Get_Set_Cnt(&g_vSet,&nval2)) == 1)&&
            nval1 && (nval1 == nval2))
        {
            PABOX2 pb = get_curr_box();
            int val = pb->line[row].val[col];
            if (!val) {
                Change_Box(pb,row,col,nval1);
                // sprintf(EndBuf(ps), " set %d", nval1);
                iret = 1;
            }
        }
    }
    return iret;
}

void Paint_Mouse_Pos(HWND hWnd)
{
    bool found = Check_Hover_Position(hWnd);
#ifdef ADD_MOUSE_RECT
    HDC hdc = GetDC(hWnd);
    if (!hdc)
        return;
    char buf[MAX_STRING];
    char *ps = buf;
    int len;
    HFONT hf = Get_CN8_Font( hdc );
    HFONT hfOld = (HFONT)SelectObject(hdc,hf);
    SIZE sz;
    int offset = 2;
    RECT rc = g_rcMouse;
    int row, col;
    PRECT pfr = &g_rcFullClient;

    UINT format2 = DT_SINGLELINE | DT_VCENTER | DT_LEFT;
    get_CN8_Text_Size( hdc, &sz );

    if (rc.left < rc.right) {
        len = rc.right - rc.left;
        *ps = 0;
        while(len--) strcat(ps," ");
        len = (int)strlen(ps);
        DrawText( hdc, // handle to DC
                ps,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format2 );   // text-drawing options
        sprintf(ps, "Mouse %d,%d", curr_xPos, curr_yPos);
        if (g_bAShiftDown) strcat(ps, " SHFT");
        if (g_bCtrlDown) strcat(ps, " CTRL");
        if (g_bAltDown) strcat(ps, " ALT");
        if (g_bLBDown) strcat(ps, " LBTN");
        // Add Row and Column if over a cell
        row = sAct_RC.row;
        col = sAct_RC.col;
        if (found) {
            sprintf(EndBuf(ps), ", cell r%dc%d", row + 1, col + 1);
            if (VALIDRC(row,col)) {
                PABOX2 pb = get_curr_box();
                int val = pb->line[row].val[col];
                if (val) {
                    sprintf(EndBuf(ps)," v %d",val);
                } else {
                    strcat(ps," blank");
                }
            }
        }

        // hmmm, this seems like a BAD way to enter a value - just on shift key
        // need to re-think this !!! TODO - maybe via a double clikc???
        //if (g_bAShiftDown) {
        //    Check_For_Insertion();
        //    g_bAShiftDown = FALSE;
        //}

        sprintf(EndBuf(ps), " on %dx%d", pfr->right - pfr->left, pfr->bottom - pfr->top);
        len = (int)strlen(ps);
        DrawText( hdc, // handle to DC
                ps,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format2 );   // text-drawing options
    }

    if (hfOld) SelectObject(hdc,hfOld);
    ReleaseDC(hWnd,hdc);
#endif // #ifdef ADD_MOUSE_RECT
}


void Do_WM_MOUSEMOVE( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    curr_xPos = LOWORD(lParam); // GET_X_LPARAM(lParam);
    curr_yPos = HIWORD(lParam); // GET_Y_LPARAM(lParam);
    Paint_Mouse_Pos(hWnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#else // #ifdef WIN32
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32

// eof - Sudo_Paint.cxx
