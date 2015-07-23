// Sudo_Paint.cxx
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <vector>
#include "Sudoku.hxx"
#include "Sudo_Paint.hxx"
#include "sprtf.hxx"
#include "Sudo_File.hxx"

using namespace std;

#ifndef MAX_STRING
#define MAX_STRING 512
#endif

typedef struct tagPPSET {
   int done_init;
   HDC hdc;
   double c_lon;
   double c_lat;
   double map_size;
   double map_zoom;
   PRECT prc;
   double dlat, dlon, dx, dy;
   COLORREF cr;
   int   spread_x, pen_size;
   int x, y, first_x, first_y, cnt, last_x, last_y;
   //PPolyGlobs ppg; // pointer to some globals
   RECT mm;
   HPEN  hp, hpOld;
   HBRUSH hb, hbOld;
   double ps_min_slat, ps_max_slat, ps_min_slon, ps_max_slon;
   double ps_max_north, ps_max_south, ps_max_west, ps_max_east;
}PPSET, * PPPSET;


// forward reference
void Paint_Box( HDC hdc );
void get_text_size_len_hdc( PSIZE psz, char * ps, int len, HDC hdc );
void Paint_Selected( HDC hdc );

static PPSET _s_act_set;
PPPSET get_act_paint_set(void) { return &_s_act_set; }

// current mouse HOVERING
RECT g_rcHover = { 0, 0, 0, 0 };
int g_hov_Row, g_hov_Col;
// current SELECTED cell
RECT g_rcSelected = { 0, 0, 0, 0 };
int g_sel_Row, g_sel_Col;

int curr_HRemainder = -1;
int curr_VRemainder = -1;
SET g_hSet;
SET g_vSet;

RECT g_rcMouse;

int clear_hover = 1;    // if mouse pos NOT found, still clear last hover

int d_x1,d_y1,d_x2,d_y2;

int sq_x1, sq_y1;
int sq_x2, sq_y2;
int sq_x3, sq_y3;
int sq_x4, sq_y4;
int g_hl_margin = 30;
int g_hr_margin = 10;
//int h_margin = 10;
int g_vt_margin = 40;
int g_vb_margin = 10;
//int v_margin = 10;

int h_workmrg = 200;
int v_workmrg = 120;
// sq_x1,sq_y1       sq_x4,sq_y4
// sq_x2,sq_y2       sq_x3,sq_y3
int sudo_x, sudo_y; // units for ONE box
int min_width = 20;
int min_height = 20;

RECT g_rcClient;
RECT g_rcListView;
RECT rcText;
RECT rcSquares[9][9];
RECT rcHWork[9];
RECT rcVWork[9];

int add_Legend_rect = 0;    // add rectangle around this legend text (for diagnostics)
RECT g_rcHText[9];
RECT g_rcVTextL[9];
RECT g_rcVTextR[9];

#define MCA_SZ 16

char cH1[MCA_SZ], cH2[MCA_SZ], cH3[MCA_SZ], cH4[MCA_SZ], cH5[MCA_SZ], cH6[MCA_SZ], cH7[MCA_SZ], cH8[MCA_SZ], cH9[MCA_SZ];
char cV1[MCA_SZ], cV2[MCA_SZ], cV3[MCA_SZ], cV4[MCA_SZ], cV5[MCA_SZ], cV6[MCA_SZ], cV7[MCA_SZ], cV8[MCA_SZ], cV9[MCA_SZ];

// ROWS
char *cpHWork[9] = { cH1, cH2, cH3, cH4, cH5, cH6, cH7, cH8, cH9 };
// COLUMNS
char *cpVWork[9] = { cV1, cV2, cV3, cV4, cV5, cV6, cV7, cV8, cV9 };

int done_paint = 0;
int debug_paint = 0;

int curr_xPos, curr_yPos; // current mouse position

//typedef vector<string> vSTG;
//typedef vSTG::iterator vSTGi;

vSTG vLines;


COLORREF crBlack = RGB(0,0,0);
COLORREF crWhite = RGB(255,255,255);
COLORREF crRed = RGB(255,0,0);
COLORREF crGreen = RGB(0,255,0);
COLORREF crBlue = RGB(0,0,255);

COLORREF crGrid = RGB(90,90,90);
COLORREF crBox = RGB(190,190,190);
COLORREF crGray = RGB(90,90,90);
COLORREF crText = RGB(90,90,90);
COLORREF crLGray = RGB(190,190,190);
COLORREF crCntr = RGB(255,0,0);
COLORREF crTop = RGB(255,0,0);
COLORREF crPurp = RGB(128,0,255);
COLORREF crLBrown = RGB(192,128,32);
COLORREF crLGreen = RGB(144,238,144);
COLORREF crLBlue = RGB(0,0,128);
COLORREF crYellow = RGB(255,255,0);
COLORREF crLYellow = RGB(255,255,0xc0);
COLORREF crCyan = RGB(0,0xff,0xff);
COLORREF crLOrange = RGB(0xff,0xc0,0x60);
COLORREF crLPink = RGB(0xff,0xc0,0xff);
COLORREF crPink = RGB(0xff,0,0xff);

HPEN  hpGrid = NULL;
HPEN  hpGrid2 = NULL;
HFONT hfCN8  = NULL;
HFONT hfCN24b = NULL;
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

// Special vertical font
HFONT hfVert = NULL;
SIZE vrcSize = {0};
int Log_Pixels = 0;
int Last_Points = 0;
SIZE fntSz = {0};

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

   //clear_painted_points();
   //clear_paintPoints();
   //delete_color_list();
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

HFONT Get_CN24b_Font( HDC hdc )
{
   HFONT hf = hfCN24b;
   if(hf)
      return hf;
   hf = Create_Font_of_PointSize( hdc, 24, true );
   hfCN24b = hf;
   return hf;
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
    if (hfVert)
       DeleteObject(hfVert);
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
    vLines.push_back(s);
}

void set_repaint( bool erase)
{
    InvalidateRect(g_hWnd,NULL,erase);
}

VOID Draw_Top_Text( HDC hdc )
{
    HFONT hf = Get_CN8_Font( hdc );
    HFONT hfOld = (HFONT)SelectObject(hdc,hf);
    char *ps = _T("Sudoku");
    int len = (int)strlen(ps);
    PRECT prc = &rcText;
    UINT format = DT_CENTER;
    SIZE sz;
    char buf[MAX_STRING];

    get_CN8_Text_Size( hdc, &sz );

    DrawText( hdc, // handle to DC
        ps,         // text to draw
        len,        // text length
        prc,        // formatting dimensions
        format );   // text-drawing options

    vSTGi ii = vLines.begin();
    format = DT_LEFT;
    for (ii = vLines.begin(); ii != vLines.end(); ii++)
    {
        string s = *ii;
        ps = buf;
        strncpy(ps,s.c_str(),MAX_STRING);
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
                prc->top += sz.cy;
                prc->bottom = prc->top + sz.cy;
                DrawText( hdc, // handle to DC
                    ps,         // text to draw
                    len,        // text length
                    prc,        // formatting dimensions
                    format );   // text-drawing options
            }
        }
    }

    if(hfOld)
        SelectObject(hdc,hfOld);

}

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

void Paint_Set(HDC hdc, PRECT prcCell, PSET ps, PABOX2 pab, int row, int col)
{
    SIZE sz;
    int height, width;
    RECT rct = *prcCell;

    // Adjust for margins
    rct.left   += ht_margin;
    rct.right  -= ht_margin;
    rct.top    += vt_margin;
    rct.bottom -= vt_margin;

    height = rct.bottom - rct.top;
    width  = rct.right  - rct.left;
    get_CN8_Text_Size(hdc, &sz );
    if ( (( sz.cy * 3 ) < height) && ((sz.cx * 3) < width) ) {
        int i, val, len, cnt;
        char buf[16];
        char *pb = buf;
        UINT format = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
        HFONT hf = Get_CN8_Font(hdc);
        HFONT hf24 = Get_CN24b_Font(hdc);
        HPEN hp  = Get_Text_Pen();
        HFONT hfold = (HFONT)SelectObject(hdc,hf);
        HPEN hpold = (HPEN)SelectObject(hdc,hp);
        COLORREF crold = SetTextColor(hdc,crText);
        COLORREF crbkold;
        int scnt = Get_Set_Cnt(ps);
        RECT rc = rct;
        int h3 = height / 3;
        int w3 = width / 3;
        int flag = 0;
        int cellflg = 0;
        RECT rcsave;

        if ( pb && VALIDRC(row,col) )
            cellflg = pab->line[row].set[col].cellflg; // get the FLAG for the cell

        rc.right = rc.left + w3;
        rc.bottom = rc.top + h3;
        cnt = 0;
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
            if (val) {
                len = sprintf(pb,"%d",val);
                // uses the device context's selected font, text color, and background color to draw the text
                if (scnt == 1) {
                    SelectObject(hdc,hf24);
                    crbkold = SetBkColor( hdc, crLGreen );
                    rcsave = rc;
                    rc = rct;
                } else if (flag & cf_Unique) {
                    crbkold = SetBkColor( hdc, crLGreen );
                } else if (flag & cf_NP) {
                    crbkold = SetBkColor( hdc, crCyan );
                } else if (flag & cf_LK) {
                    crbkold = SetBkColor( hdc, crLPink );
                } else if (flag & cf_XW) {
                    crbkold = SetBkColor( hdc, crLOrange );
                } else if (flag & cf_ELIM) {
                    crbkold = SetBkColor( hdc, crLYellow );
                } else if (flag & cf_PP) {
                    crbkold = SetBkColor( hdc, crCyan );    // TODO - need another color
                } else if (flag & cf_YW) {
                    crbkold = SetBkColor( hdc, crLGreen );  // TODO - need another color
                }

                DrawText( hdc, // handle to DC
                    pb,         // text to draw
                    len,        // text length
                    &rc,        // formatting dimensions
                    format );   // text-drawing options
                if ((scnt == 1) || (flag & (cf_Unique | cf_ELIM | cf_PRIM))) {
                    SetBkColor( hdc, crbkold ); // put color back
                    if (scnt == 1) {
                        SelectObject(hdc,hf);
                        rc = rcsave;
                    }
                }

            }
            rc.left  += w3;
            rc.right += w3;
        }
        SetTextColor(hdc,crold);
        SelectObject(hdc,hfold);
        SelectObject(hdc,hpold);
    }

}

void Paint_Legend_Text( HDC hdc )
{
    int row, col, len;
    UINT format = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
    PRECT prc;
    char *tb = GetNxtBuf();
    HFONT hf = Get_CN24b_Font( hdc );
    HFONT hfold = (HFONT)SelectObject(hdc,hf);
    //COLORREF crold = SetTextColor(hdc,crText);
    COLORREF crold = SetTextColor(hdc,crLGray);
    RECT rc;
    // paint the vertical legend
    for (row = 0; row < 9; row++) {
        prc = &g_rcVTextL[row];
        // Draw_Rectangle(hdc,pr);
        len = sprintf(tb,"%c", (char)row + 'A');
        DrawText( hdc, // handle to DC
            tb,         // text to draw
            len,        // text length
            prc,        // formatting dimensions
            format );   // text-drawing options
        prc = &g_rcVTextR[row];
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
        prc = &g_rcHText[col];  // PAINT of top text in Paint_Legend_Text()
        //Draw_Rectangle(hdc,pr);
        rc = *prc;
        len = sprintf(tb,"%d", col + 1);
        rc.bottom -= 2;
        DrawText( hdc, // handle to DC
            tb,         // text to draw
            len,        // text length
            &rc,        // formatting dimensions
            format );   // text-drawing options
    }
    SetTextColor(hdc,crold);
    SelectObject(hdc,hfold);
}


void Paint_XW_X( HDC hdc, PRECT prc, int flag )
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

void Paint_YW_Y( HDC hdc, PRECT prc, int flag )
{
    HPEN hp = Get_hpPink_2();
    if (hp) {
        HPEN hpold = (HPEN)SelectObject(hdc,hp);
        draw_Y(hdc,prc);
        if (hpold)
            SelectObject(hdc,hpold);
    }
}


void Paint_Box( HDC hdc )
{
    int row, col, val, cellflg;
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
    RECT rc;
    char *cp;
    char *cp2;
    PSET ps;
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
        for (col = 0; col < 9; col++) {
            set.val[col] = col + 1; // fill a full set of 9 value
        }
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            ps = &pb->line[row].set[col];
            cellflg = pb->line[row].set[col].cellflg;

            prc = &rcSquares[row][col];
            if (val) {
                len = sprintf(tb,"%d",val);
                DrawText( hdc, // handle to DC
                    tb,         // text to draw
                    len,        // text length
                    prc,        // formatting dimensions
                    format );   // text-drawing options
                set.val[val - 1] = 0;   // clear potential values
                Zero_SET(ps);
            } else {
                total_empty_count++;

                if (cellflg & cf_XW)
                    Paint_XW_X( hdc, prc, cellflg );
                else if (cellflg & cf_YWC)
                    Paint_YW_Y( hdc, prc, cellflg );

                Paint_Set(hdc, prc, ps, pb, row, col ); // paint candidates (with color)
            }
        }

        // set now contains potential for this row
        prc = &rcHWork[row];
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
                    prc->cnt = len;
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
                        prc->cnt = 1;
                        memcpy(&prc->set, &set2, sizeof(SET));
                        one_value_count++;
                    }
                }
            }
        }
    }

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
        prc = &rcVWork[col];
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

    SelectObject(hdc,hfold);

}

void Set_SIZES(HWND hWnd)
{
    GetClientRect(hWnd,&g_rcClient);

    RECT rc = g_rcClient;
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int log = width / 3;
    SIZE sz = g_szCN8Text;

    sudo_x = ((log * 2) - h_workmrg) / 9;                         // one sudo x width
    sudo_y = (height - (g_vt_margin + g_vb_margin) - v_workmrg) / 9;        // one sudo y height

    //if (log > 50) {
    if ((sudo_x > min_width) && (sudo_y > min_height)) {

        log = (g_hl_margin + g_hr_margin) + (sudo_x * 9) + h_workmrg;

        rcText.left  = log + g_hl_margin;
        rcText.right = rc.right - g_hr_margin;
        rcText.top   = rc.top + g_vb_margin;
        rcText.bottom = rcText.top + sz.cy;

        d_x1 = log;
        d_x2 = d_x1;
        d_y1 = rc.top;
        d_y2 = rc.bottom;

        sq_x1 = rc.left + g_hl_margin;
        sq_x2 = sq_x1;
        sq_x3 = d_x1 - g_hr_margin - h_workmrg;
        sq_x4 = sq_x3;

        sq_y1 = rc.top + g_vt_margin;
        sq_y2 = sq_y1 + (sudo_y * 9);
        sq_y3 = sq_y2;
        sq_y4 = sq_y1;
        // d_x1 = log;
        // d_x2 = d_x1;
        // d_y1 = rc.top;
        // d_y2 = rc.bottom;
        g_rcMouse.left   = d_x1 + g_hl_margin;
        g_rcMouse.right  = g_rcClient.right - g_hr_margin;
        g_rcMouse.bottom = g_rcClient.bottom - g_vb_margin;
        g_rcMouse.top    = rc.bottom - g_vb_margin - sz.cy;

        g_rcListView.left  = log; // + h_margin;      // set LEFT
        g_rcListView.right = rc.right; // - h_margin; // set RIGHT
        g_rcListView.top   = rc.top; // + v_margin;
        g_rcListView.bottom = g_rcMouse.top - g_vb_margin;

        int row, col;
        int x1, x2, y1, y2;
        PRECT pr, pr2, pr3;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                x1 = sq_x1 + (sudo_x * col);
                x2 = sq_x1 + (sudo_x * (col+1));
                y1 = sq_y1 + (sudo_y * row);
                y2 = sq_y1 + (sudo_y * (row+1));
                pr = &rcSquares[row][col];
                pr->top    = y1;
                pr->bottom = y2;
                pr->left   = x1;
                pr->right  = x2;
            }
        }
        //RECT g_rcVTextL[9];
        //RECT g_rcVTextR[9];
        // set rectangle for paint horizontal lines
        x1 = sq_x1;
        x2 = sq_x3;
        for (row = 0; row < 9; row++) {
            y1 = sq_y1 + (sudo_y * row);
            y2 = sq_y1 + (sudo_y * row);
            // fill in HORIZONTAL work pad rect
            // sq_x1,sq_y1       sq_x4,sq_y4
            // sq_x2,sq_y2       sq_x3,sq_y3
            pr = &rcHWork[row];
            pr->left   = sq_x4 + g_hl_margin;
            //pr->right  = pr->left + h_workmrg - g_hr_margin - g_hl_margin;
            pr->right  = pr->left + h_workmrg - g_hl_margin;
            pr->top    = y1 + g_vb_margin;
            pr->bottom = pr->top + sudo_y - (2 * g_vb_margin);
            pr2 = &g_rcVTextL[row];
            pr2->left = g_rcClient.left;
            pr2->right = pr2->left + g_hl_margin;
            pr2->top = y1;
            pr2->bottom = pr2->top + sudo_y;
            //sprtf("%d: V1 RECT %s\n", row + 1, Get_Rect_Stg(pr2));
            pr3 = &g_rcVTextR[row];
            pr3->left = sq_x4;
            pr3->right = pr3->left + g_hl_margin;
            pr3->top = pr2->top;
            pr3->bottom = pr2->bottom;
            // sprtf("%d: V2 RECT %s\n", row + 1, Get_Rect_Stg(pr3));
        }
        // RECT g_rcHText[9]; - set TOP rects in WM_SIZE
        // set rectangles for paint vertical lines
        y1 = sq_y1;
        y2 = sq_y2;
        for (col = 0; col < 9; col++) {
            x1 = sq_x1 + (sudo_x * col);
            x2 = sq_x1 + (sudo_x * col);
            // fill in VERTICAL work pad rect
            pr = &rcVWork[col];
            pr->left   = x1 + g_hr_margin; // left is left + margin
            pr->right  = pr->left + sudo_x - (2 * g_hr_margin); // right is left + 1 sudo_x - 2 * margin
            pr->top    = y2 + g_vb_margin; // top is bottom + margin
            pr->bottom = pr->top + v_workmrg - g_vb_margin; // bot is top + work - margin
            // paint across the TOP 1 2 3 ...
            pr2 = &g_rcHText[col];  // SETTING of text position in WM_SIZE
            pr2->left = x1;
            pr2->right = pr2->left + sudo_x;
            pr2->top   = g_rcClient.top + 1;
            pr2->bottom = pr2->top + g_vt_margin;
        }
    }
    if (hfVert)
        DeleteObject(hfVert);
    hfVert = 0;
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


VOID Do_WM_PAINT( HWND hWnd )
{
	PAINTSTRUCT ps;
	HDC hdc;
//    RECT rc;
//    int width, height;
//    SIZE sz;
    int log;
    int row, col;
    int x1,y1,x2,y2;
    PRECT pr;

	hdc = BeginPaint(hWnd, &ps);

    //if (log > 50) {
    if ((sudo_x > min_width) && (sudo_y > min_height)) {

        log = (g_hl_margin + g_hr_margin) + (sudo_x * 9) + h_workmrg;

        if (debug_paint && !done_paint)
            sprtf("So (h_margin * 2) + (sudo_x * 9) = %d\n", log);

        HPEN hbp4 = Get_LBrown_Pen_4();
        HPEN hbp1 = Get_LBrown_Pen_1();
        HPEN hpOld = 0;

        if (hbp4)
            hpOld = (HPEN)SelectObject(hdc, hbp4);

        draw_line( hdc, d_x1, d_y1, d_x2, d_y2 );

        draw_line( hdc, d_x1, g_rcMouse.top - g_vb_margin, g_rcClient.right, g_rcMouse.top - g_vb_margin ); 
        //draw_rectangle( hdc, sq_x1, sq_y1, sq_x2, sq_y2, sq_x3, sq_y3, sq_x4, sq_y4 );
        //draw_line( hdc, sq_x2, sq_y2, sq_x3, sq_y3);

        // paint horizontal lines
        x1 = sq_x1;
        x2 = sq_x3;
        for (row = 0; row <= 9; row++) {
            y1 = sq_y1 + (sudo_y * row);
            y2 = sq_y1 + (sudo_y * row);
            if (row % 3)
                SelectObject(hdc,hbp1);
            else
                SelectObject(hdc,hbp4);
            draw_line( hdc, x1, y1, x2, y2 );
            //if (row < 9) {
                // fill in HORIZONTAL work pad rect
                // sq_x1,sq_y1       sq_x4,sq_y4
                // sq_x2,sq_y2       sq_x3,sq_y3
                //pr = &rcHWork[row];
                //pr->left   = sq_x4 + g_hl_margin;
                //pr->right  = pr->left + h_workmrg - g_hr_margin;
                //pr->top    = y1 + g_vb_margin;
                //pr->bottom = pr->top + sudo_y - (2 * g_vb_margin);
            //}
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
            //if (col < 9) {
            //    // fill in VERTICAL work pad rect
            //    pr = &rcVWork[col];
            //    pr->left   = x1 + g_hr_margin; // left is left + margin
            //    pr->right  = pr->left + sudo_x - (2 * g_hr_margin); // right is left + 1 sudo_x - 2 * margin
            //    pr->top    = y2 + g_vb_margin; // top is bottom + margin
            //    pr->bottom = pr->top + v_workmrg - g_vb_margin; // bot is top + work - margin
            //}
        }

        hbp1 = Get_LBlue_Pen();

        SelectObject(hdc,hbp1);

        for (col = 0; col < 9; col++) {
            pr = &rcHWork[col];
            Draw_Rectangle(hdc,pr);
            if (add_Legend_rect) {
                pr = &g_rcHText[col];   // ADD legend rectangles, if (add_Legend_rect)
                Draw_Rectangle(hdc,pr);
            }
        }
        for (row = 0; row < 9; row++) {
            pr = &rcVWork[row];
            Draw_Rectangle(hdc,pr);
            if (add_Legend_rect) {
                pr = &g_rcVTextL[row];
                Draw_Rectangle(hdc,pr);
                pr = &g_rcVTextR[row];
                Draw_Rectangle(hdc,pr);
            }
        }

        if (hpOld)
            SelectObject(hdc,hpOld);

        Draw_Top_Text(hdc);

        if (is_curr_box_valid_for_paint()) {
            Paint_Box(hdc); // paint in current values
        }
    }

    Paint_Selected(hdc);
    Paint_Legend_Text(hdc);

	EndPaint(hWnd, &ps);
    done_paint = 1;
}

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
            HFONT hf = Get_CN24b_Font( hdc );
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
                prc = &rcHWork[row];
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
                prc = &rcVWork[col];
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
        HFONT hf = Get_CN24b_Font( hdc );
        HFONT hfold = (HFONT)SelectObject(hdc,hf);
        // // ROWS
        prc = &rcHWork[row];
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
        prc = &rcVWork[col];
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

bool Is_Selected_Valid()
{
    if (IsRectEmpty(&g_rcSelected))
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
            // should also put back the text
            Paint_Row_Col_Text( hdc, g_hov_Row, g_hov_Col );
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

        if (found && VALIDRC(row,col)) {
            Trim_Work_Strings( hdc, row, col );
        }
        ReleaseDC(hWnd,hdc);
    }
}

void Paint_Selected( HDC hdc )
{
    PRECT prs = &g_rcSelected;
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


void Set_New_Selected(HWND hWnd, PRECT prt, bool found, int row, int col)
{
    PRECT prs = &g_rcSelected;
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
            // should also put back the text
            if (VALIDRC(srow,scol))
                Paint_Row_Col_Text( hdc, srow, scol );
        }
        if (found) {
            *prs = *prt;
            g_sel_Row = row;
            g_sel_Col = col;
            Paint_Selected(hdc);
        }
        if (hpOld)
            SelectObject(hdc,hpOld);

        if (found && VALIDRC(row,col)) {
            Trim_Work_Strings( hdc, row, col );
        }
        ReleaseDC(hWnd,hdc);
    }
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

void Move_Selection_Left(HWND hWnd)
{
    PRECT prs = &g_rcSelected;
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        if (col) {
            col--;
        } else {
            col = 8;
        }
        Move_Selection(hWnd, row, col);
    }
}
void Move_Selection_Up(HWND hWnd)
{
    PRECT prs = &g_rcSelected;
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        if (row) {
            row--;
        } else {
            row = 8;
        }
        Move_Selection(hWnd, row, col);
    }
}
void Move_Selection_Down(HWND hWnd)
{
    PRECT prs = &g_rcSelected;
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        row++;
        if (row >= 9)
            row = 0;
        Move_Selection(hWnd, row, col);
    }
}
void Move_Selection_Right(HWND hWnd)
{
    PRECT prs = &g_rcSelected;
    int row = g_sel_Row;
    int col = g_sel_Col;
    if ( !IsRectEmpty(prs) && VALIDRC(row,col) ) {
        col++;
        if (col >= 9)
            col = 0;
        Move_Selection(hWnd, row, col);
    }
}


vCHG vChgs;
vCHG vChgsUndone;
CHANGE chg; // last change is here

int change_rank = 0;
int undo_rank = 0;
void Kill_Change()
{
    CHANGE chg;
    vCHGi ii;
    if (vChgs.size())
        sprtf("Killing %d changes...\n", vChgs.size());
    for (ii = vChgs.begin(); ii != vChgs.end(); ii++) {
        chg = *ii;
        if (chg.vals)
            free(chg.vals);
        chg.vals = 0;
    }
    vChgs.clear();
}

bool Got_Change_Stack()
{
    return vChgs.size() ? true : false;
}
bool Got_Redo_Stack()
{
    return (undo_rank < change_rank) ? true : false;
}

// NOTE: A NEGATIVE nval denote DELETED
void Add_2_Change(int Row, int Col, int nval, int flag)
{
    int cnt;
    char *ps;
    // find if this has been changed before
    vCHGi ii;
    change_rank++;
    if (flag == 0) {
        undo_rank = change_rank;
    }
    for (ii = vChgs.begin(); ii != vChgs.end(); ii++) {
        chg = *ii;
        if ((chg.row == Row) && (chg.col == Col)) {
            // found existing
            // now may have had this value before, so check - no, no check
            cnt = chg.cnt;
            if ((cnt + 1) >= chg.max) {
                // need to allocate MORE
                chg.max *= 2;
                chg.vals = (int *)realloc(chg.vals,chg.max);
                chg.order = (int *)realloc(chg.order,chg.max);
                if (!chg.vals || !chg.order) {
                    ps = "ERROR: Change memory re-allocation FAILED!\n";
                    sprtf("%s\n");
                    MB(ps);
                    exit(1);
                }
            }
            // NOTE: negative means DELETED
            chg.val = nval;
            chg.vals[cnt] = nval;
            chg.order[cnt] = change_rank;
            cnt++;
            chg.cnt = cnt;
            *ii = chg;
            sprtf("Re-set Row %d Col %d to value %d %s\n",
                (Row + 1), (Col + 1), nval,
                ((flag == 1) ? "UNDO" : (flag == 2) ? "REDO" : "") );
            return;
        }
    }

    sprtf("Setting Row %d Col %d to value %d %s\n",
        (Row + 1), (Col + 1), nval,
        ((flag == 1) ? "UNDO" : (flag == 2) ? "REDO" : "") );
    memset(&chg,0,sizeof(CHANGE));
    chg.row = Row;
    chg.col = Col;
    chg.val = nval;
    chg.max = 64;
    chg.vals = (int *)malloc(sizeof(int) * chg.max);
    chg.order = (int *)malloc(sizeof(int) * chg.max);
    if (!chg.vals || !chg.order) {
        ps = "ERROR: Change memory allocation FAILED!\n";
        sprtf("%s\n");
        MB(ps);
        exit(1);
    }
    cnt = 0;
    chg.vals[cnt] = nval;
    chg.order[cnt] = change_rank;
    cnt++;
    chg.cnt = cnt;
    vChgs.push_back(chg);
}

// NOTE: A NEGATIVE nval denote DELETED
void Change_Box(PABOX2 pb, int row, int col, int val, int flag)
{
    Add_Box2(pb);   // stack the BOX, ready for BACKUP
    if (val < 0) {
        pb->line[row].val[col] = 0;
    } else {
        pb->line[row].val[col] = val;
    }
    Add_2_Change(row, col, val, flag);
    g_bChanged = TRUE;
    Set_Window_Title();
    pb->iStage = sg_Begin;
    if (val > 0) {
        int val2 = get_solution_value(row,col);
        if (val2 && (val2 != val)) {
            sprtf("Change R%dC%d: Value %d NOT %d\n", row + 1, col + 1, val2, val); 
        }
    }
    set_repaint();
}

void Do_ID_EDIT_UNDO(HWND hWnd)
{
    CHANGE chg;
    vCHGi ii;
    int cnt, undone_rank, i;
    int row, col, nval, val, fnd;
    if (Got_Change_Stack()) {
        undone_rank = undo_rank;
        fnd = 0;
        for (ii = vChgs.begin(); ii != vChgs.end(); ii++) {
            chg = *ii;
            cnt = chg.cnt;
            for (i = 0; i < cnt; i++) {
                if (chg.order[i] == undone_rank) {
                    // found undo command
                    fnd = 1;
                    nval = chg.vals[i];
                    row  = chg.row;
                    col  = chg.col;
                    PABOX2 pb = get_curr_box();
                    if (VALIDRC(row,col)) {
                        val = pb->line[row].val[col];
                        // forget current value 
                        // if previous was negative, reverse to add it
                        // if it was positive, reverse to delete it
                        Change_Box(pb, row, col, -nval, 1);
                        if (undo_rank)
                            undo_rank--;    // back up to undo next to last
                    }
                    break;
                }
            } // for cnt
            if (fnd)
                break;
        }
    }
}

void Do_ID_EDIT_REDO(HWND hWnd)
{
    CHANGE chg;
    vCHGi ii;
    int cnt, undone_rank, i;
    int row, col, nval, val, fnd;
    if (Got_Change_Stack() && (undo_rank < change_rank)) {
        undone_rank = undo_rank + 1;
        fnd = 0;
        for (ii = vChgs.begin(); ii != vChgs.end(); ii++) {
            chg = *ii;
            cnt = chg.cnt;
            for (i = 0; i < cnt; i++) {
                if (chg.order[i] == undone_rank) {
                    // found undo command
                    fnd = 1;
                    nval = chg.vals[i];
                    row  = chg.row;
                    col  = chg.col;
                    PABOX2 pb = get_curr_box();
                    if (VALIDRC(row,col)) {
                        val = pb->line[row].val[col];
                        // forget current value 
                        // if previous was negative, reverse to add it
                        // if it was positive, reverse to delete it
                        Change_Box(pb, row, col, -nval, 2);
                        undo_rank++;    // move up to redo/undo next
                    }
                    break;
                }
            } // for cnt
            if (fnd)
                break;
        }
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

void Paint_Mouse_Pos(HWND hWnd)
{
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
    bool found = Check_Hover_Position(hWnd);
    int row, col;

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
        sprintf(ps, "Mouse at %d,%d", curr_xPos, curr_yPos);
        if (g_bAShiftDown) strcat(ps, " SHFT");
        if (g_bCtrlDown) strcat(ps, " CTRL");
        if (g_bAltDown) strcat(ps, " ALT");
        if (g_bLBDown) strcat(ps, " LBTN");
        // Add Row and Column if over a cell
        row = sAct_RC.row;
        col = sAct_RC.col;
        if (found) {
            sprintf(EndBuf(ps), ", cell row %d col %d", row + 1, col + 1);
            if (VALIDRC(row,col)) {
                PABOX2 pb = get_curr_box();
                int val = pb->line[row].val[col];
                if (val) {
                    sprintf(EndBuf(ps),", val %d",val);
                } else {
                    strcat(ps," blank.");
                }
            }
        }
        //if (g_bLBDown && VALIDRC(g_act_Row,g_act_Col)) {
        row = g_sel_Row;
        col = g_sel_Col;
        if (g_bAShiftDown && Is_Selected_Valid() && VALIDRC(row,col)) {
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
                    sprintf(EndBuf(ps), " set %d", nval1);
                }
            }
            //g_bLBDown = FALSE;  // only ONCE through here
            g_bAShiftDown = FALSE;
        }

        len = (int)strlen(ps);
        DrawText( hdc, // handle to DC
                ps,         // text to draw
                len,        // text length
                &rc,        // formatting dimensions
                format2 );   // text-drawing options
        //if (g_bCtrlDown) {
        //    PABOX2 pb = get_curr_box();
        //    PABOX3 pb3 = get_curr_box3();
        //    Copy_BOX2_to_BOX3( pb, pb3 );
        //    g_bCtrlDown = FALSE;
        //}
    }

    if (hfOld) SelectObject(hdc,hfOld);
    ReleaseDC(hWnd,hdc);
}


void Do_WM_MOUSEMOVE( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    curr_xPos = LOWORD(lParam); // GET_X_LPARAM(lParam);
    curr_yPos = HIWORD(lParam); // GET_Y_LPARAM(lParam);
    Paint_Mouse_Pos(hWnd);
}


// eof - Sudo_Paint.cxx
