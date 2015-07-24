// Sudo_File.cxx
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif

#include "Sudoku.hxx"

#ifdef WIN32
#define STRICMP stricmp
#else
#define STRICMP strcasecmp
#endif

#define EOL_CHRS "\r\n"
#define NUL_VAL "0"

#ifndef MAX_STRING
#define MAX_STRING 1024
#endif

static char _s_buf[MAX_STRING*16];

char *g_pAct_File = 0;          // current ACTIVE file
char *g_pSpecial = "Untitled";  // when a box is say generated

// ****************************
// Reset the SINGLE ACTIVE FILE
void Reset_Active_File(char *file)
{
    if (g_pAct_File) {
        free(g_pAct_File);
    }
    g_pAct_File = (char *)malloc( strlen(file) + 8 );
    if (!g_pAct_File) {
        MB("Memory allocation for file name FAILED!");
        exit(1);
    }
    strcpy(g_pAct_File,file);
}

char *Get_Act_File() { return g_pAct_File; }


///////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
///////////////////////////////////////////////////////////////////////////////////////////
#include <CommCtrl.h>

using namespace std;

static ABOX2 _s_box;

int debug_file_read = 0;

enum FileType {
    ft_Unknown,
    ft_CSV,
    ft_XML,
    ft_STG,
    ft_SDK
};


// TCHAR g_filter[_MAX_PATH] = _T("CSV Files\0*.csv\0XML Files\0*.xml\0TXT FIles\0*.txt\0All Files\0*.*\0\0");
TCHAR g_filter[_MAX_PATH] = _T("CSV Files\0*.csv\0XML Files\0*.xml\0TXT Files\0*.txt\0SDK Files\0*.sdk\0All Files\0*.*\0\0");

//char *g_pAct_File = 0;          // current ACTIVE file
//char *g_pSpecial = "Untitled";  // when a box is say generated

// forward reference
int Test_Extract_CSV_Info(PABOX2 pb, char *pbuf, int len);
int Test_Extract_ASCII_Info( PABOX2 pb, char *pbuf, int len );
int Test_Extract_SDK_Info( PABOX2 pb, char *pbuf, int len );
int Extract_SDK_Info( char *pbuf, int len );
bool Save_SDK_File( HWND hWnd, char *pf, int flag = 0 );


void Set_Window_Title()
{
    char *tb = GetNxtBuf();
    char *cp = Get_Act_File();
    if (cp) {
        sprintf(tb,"%s - %s", szTitle, cp);
        if (g_bChanged)
            strcat(tb,"*");
    } else {
        strcpy(tb,szTitle);
    }
    SetWindowText(g_hWnd,tb);
}

void File_Reset()
{
    Set_Window_Title();
    char *cp = Get_Act_File();
    if (cp && strcmp(g_pSpecial,cp)) {
        sprtf("Loaded [%s]\n",cp);
        Add_to_INI_File_List(cp);
    }
    done_end_dialog = false;
    // Kill_Change();
    // Free_Boxes2();
    g_bChanged = false;
    done_count_warning = false;
    Set_Paint_Chains_Invalid(); // STOP PAINTING PAIRS, CHAINS, etc 
    set_solution_valid(0);
    if (Get_Spot_Count() >= g_iMinGiven)
        StartThread();
    // if (g_bShwStLnks) Do_Strong_Links(get_curr_box());
    set_repaint();
}

/* ---------------------------
typedef struct tagOFN {
  DWORD         lStructSize;
  HWND          hwndOwner;
  HINSTANCE     hInstance;
  LPCTSTR       lpstrFilter;
  LPTSTR        lpstrCustomFilter;
  DWORD         nMaxCustFilter;
  DWORD         nFilterIndex;
  LPTSTR        lpstrFile;
  DWORD         nMaxFile;
  LPTSTR        lpstrFileTitle;
  DWORD         nMaxFileTitle;
  LPCTSTR       lpstrInitialDir;
  LPCTSTR       lpstrTitle;
  DWORD         Flags;
  WORD          nFileOffset;
  WORD          nFileExtension;
  LPCTSTR       lpstrDefExt;
  LPARAM        lCustData;
  LPOFNHOOKPROC lpfnHook;
  LPCTSTR       lpTemplateName;
#if (_WIN32_WINNT >= 0x0500)
  void          *pvReserved;
  DWORD         dwReserved;
  DWORD         FlagsEx;
#endif 
} OPENFILENAME, *LPOPENFILENAME;


   --------------------------- */


int Extract_CSV_Info(char *pbuf, int len)
{
    int i, c, val, j;
    int row = 0;
    int col = 0;
    int count;
    bool hadc = true;
    PABOX2 pb = get_curr_box();
    char *lines[11];
    char *cp;
    row = 0;
    val = 0;
    char *tb = GetNxtBuf();
    size_t cvssize = 18;
    //sprtf("Loading file [%s]\n",file);
    cp = &pbuf[0];

    for(i = 0; i < len; i++) {
        c = pbuf[i];
        if (( c == '\r' )||( c == '\n' )) {
            lines[row++] = cp;
            pbuf[i] = 0;
            i++;
            for(; i < len; i++) {
                c = pbuf[i];
                if ( c > ' ' ) {
                    j = (int)strlen(cp);
                    if (debug_file_read) sprtf("Row %d [%s] len %d\n",row,cp,j);
                    val = i;
                    cp = &pbuf[val];
                    i--;
                    break;
                }
                pbuf[i] = 0;
            }
            if (row > 9)
                break;
        }
    }
    lines[row++] = cp;
    if (row < 9) {
        sprtf("ERROR: Not enough lines in the file! Only %d rows\n", row);
        return 1;   // not enough lines
    }
    invalidate_box(pb);
    count = 0;
    for (row = 0; row < 9; row++) {
        cp = lines[row];
        len = strlen(cp); // minimum line case ,,,,,,,, = 8
        if (debug_file_read) sprtf("Doing row %d [%s] len %d\n", (row+1), cp, len);
        SET set;
        for (col = 0; col < 9; col++) {
            set.val[col] = col + 1; // fill a full set of 9 value
        }
        col = 0;
        hadc = true;   // begin with - had a comma
        *tb = 0;        
        for (j = 0; j < len; j++) {
            c = cp[j];
            if ( c == ',' ) {
                if (hadc)
                    pb->line[row].val[col++] = 0;
                hadc = true;
                strcat(tb,",");
            } else if ( c <= ' ' ) {
                // skip
            } else if ( ISDIGIT(c) ) {
                if (hadc) {
                    val = c - '0';
                    pb->line[row].val[col++] = val;
                    if (val) {
                        count++;
                        set.val[val - 1] = 0;
                    }
                } else
                    sprtf("Found digit %c without comma separation!\n", (char)c );
                hadc = 0;
                sprintf(EndBuf(tb), "%c", (char)c);
            } else {
                sprtf("Found unknown value [%c]!\n",c);
            }
            if (col >= 9)
                break;
        }
        if (hadc) {
            if (col < 9)
                pb->line[row].val[col++] = 0;
        }
        if (col < 9) {
            sprtf("ERROR: Doing row %d, only found %d columns!\n", (row+1), col);
            return 1;
        }
        // get missing values for this row
        while (strlen(tb) < cvssize) strcat(tb," ");

        sprintf(EndBuf(tb), " - row %d has %d values. Missing ", (row + 1), count);
        for (col = 0; col < 9; col++) {
            val = set.val[col]; // get value
            if (val)
                sprintf(EndBuf(tb), "%d",val);
        }
        // got to next row
        if (debug_file_read) sprtf("%s\n",tb);
    }   // for each ROW
    if (count < g_iMinGiven) {  // was 18, now 17 - see : http://en.wikipedia.org/wiki/Suduko
        sprtf("WARNING: Too few values for unique solution found! Only %d values\n", count);
    }
    return 0;
}

#define EOLCHAR(c) (( c == '\n' )||( c == '\r' ))
#define EXTDIGIT(c) ISDIGIT(c) || (c == '.') || (c == '-') || (c == ',') || (c == '_') || (c == ' ')

// TODO - In this buffer search return the FIRST type identified
FileType TODO_Guess_File_Type( char *pbuf, int len )
{
    FileType ft = ft_Unknown;
    int i, c, pc, nc, i2;
    int lncnt = 0;
    int colcnt = 0;
    int commacnt = 0;
    PABOX2 pb = &_s_box;
    invalidate_box(pb);
    c = 0;
    for (i = 0; i < len; i++) {
        pc = c;
        c = pbuf[i];
        i2 = i + 1;
        nc = (i2 < len) ? pbuf[i2] : 0;
        if (EOLCHAR(c)) {
            i++;
            for (; i < len; i++) {
                pc = c;
                c = pbuf[i];
                if (!EOLCHAR(c))
                    break;
            }
            lncnt++;
        } else if (( c == '#') || ((c == '/') && (nc == '/'))) {
            // assume a COMMENT line - skip the line
            i++;
            for (; i < len; i++) {
                pc = c;
                c = pbuf[i];
                if (!EOLCHAR(c)) {
                    i--; // back up 1 to find this again
                    break;
                }
            }
        } else if (colcnt == 0) {
            // not EOL - immediately check for a string of EXTDIGIT(c) chars
            // and count the COLUMNS
            if (EXTDIGIT(c)) {
                i2 = i + 1;
                for (; i2 < len; i2++) {
                    pc = c;
                    c = pbuf[i2];
                    if (EOLCHAR(c)) {
                        break;
                    } else if (!EXTDIGIT(c)) {
                        break;
                    } else if (c == ',') {
                        commacnt++;
                    }
                    colcnt++;
                }
                if ((lncnt == 0) & (colcnt >= 81)) {
                    // got at least 81 'extended' DIGITS on first line
                    // ASSUME is ft_STG
                    if ( (Test_Extract_ASCII_Info( pb, pbuf, len ) == 0) &&
                         ( validate_box(pb) == 0 ) )
                         return ft_STG;
                }
                if (colcnt >= 9) {
                    // Check SDK or CSV
                    if (commacnt == 8) {
                        // starting to look like CSV
                    } else {

                    }

                }
            }
        }
    } 

    return ft;
}

// Get_Line_Count - get line count
// but does a LOT move
int Count_Lines_In_Buffer( char *pbuf, int len, int *max = 0, int *cvs = 0 );
int Count_Lines_In_Buffer( char *pbuf, int len, int *max, int *cvs )
{
    int c;
    int i;
    int lines = 0;
    int mcols = 0;
    int cols = 0;
    int cvslines = 0;
    int comma_cnt = 0;
    for (i = 0; i < len; i++) {
        c = pbuf[i];
        if (EOLCHAR(c)) {
            i++;
            for (; i < len; i++) {
                c = pbuf[i];
                if (!EOLCHAR(c)) {
                    i--;    // back up to count this char on next row
                    break;
                }
            }
            if ((lines == 0) && (cols > mcols))
                mcols = cols;   // max columns is ONLY the first LINE

            lines++;    // count a line
            if ((lines == 1) && (mcols >= 81) && max) {
                // assume first line is ASCII line
                break;  // no more scan needed
            }

            if ((cols >= 8) && (comma_cnt >= 8))
                cvslines++; // think of it as a CVS line

            cols = 0;       // RESTART column count
            comma_cnt = 0;  // and restart comma count
        } else {
            cols++;
            if (c == ',')
                comma_cnt++;
        }
    }
    if ((cols >= 8) && (comma_cnt >= 8))
       cvslines++; // think of it as a CVS line
    if (max)
        *max = mcols;
    if (cvs)
        *cvs = cvslines;

    return lines;
}

#define ISDIGIT1(c) (( c >= '1' )&&( c <= '9' ))

int Extract_ASCII_Info( char *pbuf, int len )
{
    int row = 0;
    int col = 0;
    int val;
    int c, i;
    int count = 0;
    PABOX2 pb = get_curr_box();

    invalidate_box(pb);
    for (i = 0; i < len; i++) {
        c = pbuf[i];
        if (ISDIGIT1(c)) {
            val = c - '0';
            pb->line[row].val[col++] = val;
            count++;
        } else {
            pb->line[row].val[col++] = 0;
        }
        if (col >= 9) {
            row++;
            col = 0;
            if (row >= 9)
                break;
        }
    }
    if (count < g_iMinGiven) { // was 18, now 17 - see: http://en.wikipedia.org/wiki/Suduko
        sprtf("WARNING: Only %d values. Require %d unique solution!\n", count, g_iMinGiven);
    }
    return 0;
}

int Process_Buffer( char *file, char * pbuf, int len )
{
    int max_len, cvs_cnt, res;
    PABOX2 pb = &_s_box;
    int lines = Count_Lines_In_Buffer( pbuf, len,
        &max_len, // line with maximum length, if FIRST line
        &cvs_cnt ); // lines with 8 or grater chars = cvs lines, like ,,,,,,,,
    if ( (lines >= 9) && (cvs_cnt >= 9) ) {
        return Extract_CSV_Info( pbuf, len );   // treat as a CVS file
    }
    // Hmmm, if we had a line with 81 (9x9) chars, then maybe just simple ASCII line
    if (max_len >= 81) {
        return Extract_ASCII_Info( pbuf, len );
    }
    invalidate_box(pb);
    res = Test_Extract_SDK_Info( pb, pbuf, len );
    if ((res == 0) && (validate_box(pb) == 0)) {
        return Extract_SDK_Info( pbuf, len );
    }
        
    // other formats??????
    sprtf("File [%s] unhandled format!\n", file);
    set_repaint2();
    return -1;
}

/* expect
  <Table1>
    <spot>0</spot>
  </Table1>
  <Table1>
    <spot>1</spot>
  </Table1>
 ============== */
#ifndef VALIDVAL
#define VALIDVAL(a) (( a >= 0 ) && ( a <= 9 ))
#endif

bool Load_XML_Buffer( char *buf, int len )
{
    bool ok = false;
    int i, c;
    char tmp[MAX_STRING];
    GRID grid;
    int row = 0;
    int col = 0;
    char *pt = tmp;
    int off = 0;
    int inquot = 0;
    int val;
    bool bad = false;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            grid.v[row][col] = -1;
        }
    }
    row = 0;
    col = 0;
    for (i = 0; i < len; i++) {
        c = buf[i];
        if (c == '<') {
            i++;
            off = 0;
            for (; i < len; i++) {
                c = buf[i];
                if (inquot) {
                    pt[off++] = (char)c;
                    if ( c == '"' )
                        inquot = 0;
                } else {
                    if ( c == '"' ) {
                        inquot = 1;
                        continue;
                    }
                    if ( c == '>' ) {
                        pt[off] = 0;
                        if (strcmp(pt,"spot") == 0)
                        {
                            i++;
                            off = 0;
                            for (; i < len; i++) {
                                c = buf[i];
                                if ( ISDIGIT(c) ) {
                                    pt[off++] = (char)c;
                                } else if ( c == '<' ) {
                                    break;
                                }
                            }
                            pt[off] = 0;
                            if (off) {
                                val = atoi(pt);
                                if (VALIDVAL(val)) {
                                    grid.v[row][col++];
                                    if (col >= 9) {
                                        row++;
                                        col = 0;
                                    }
                                } else {
                                    bad = true;
                                }
                            }
                            break;
                        }
                    } else {
                        pt[off++] = (char)c; // collect token
                    }
                }
                if (bad)
                    break;
            }
        }
        if (bad)
            break;
        if (row >= 9)
            break;
    }
    if (!bad) {
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = grid.v[row][col];
                if (!VALIDVAL(val)) {
                    bad = true;
                    break;
                }
            }
            if (bad == true)
                break;
        }
    }
    if (!bad) {
        PABOX2 pb = get_curr_box();
        invalidate_box(pb);
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = grid.v[row][col];
                pb->line[row].val[col] = val;
            }
        }
    }
    return ok;
}

bool Load_XML_File( HWND hWnd, char *file )
{
    bool ok = false;
    //size_t len = _tcslen(file);
    char *pf2 = _strdup(file);
    if (pf2)
        file = pf2;
    HANDLE hfile = CreateFile(file,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL );
    if (VFH(hfile)) {
        DWORD size_hi = 0;
        DWORD size_low = GetFileSize(hfile, &size_hi);
        if (size_hi == 0) {
            char *buf = (char *)malloc(size_low+16);
            if (buf) {
                if (ReadFile(hfile,buf,size_low,&size_hi,NULL)&&
                    (size_low == size_hi)) {
                        buf[size_low] = 0;
                    Load_XML_Buffer( buf, size_low );
                    if (is_curr_box_valid_for_paint()) {
                        ok = true;
                    }
                }
                free(buf);
            }
        } else {
            sprtf("ERROR: File TOO large!\n");
        }
        CloseHandle(hfile);
    }
    if (ok) {
        Reset_Active_File(file);
        File_Reset();
    } else {
        sprtf("Loaded of [%s] FAILED\n",file);
    }
    if (pf2)
        free(pf2);
    return ok;
}

bool Load_a_file( HWND hWnd, char *file )
{
    bool ok = false;
    //size_t len = _tcslen(file);
    char *pf2 = _strdup(file);
    if (pf2)
        file = pf2;
    HANDLE hfile = CreateFile(file,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL );
    if (VFH(hfile)) {
        DWORD size_hi = 0;
        DWORD size_low = GetFileSize(hfile, &size_hi);
        if (size_hi == 0) {
            char *buf = (char *)malloc(size_low+16);
            if (buf) {
                if (ReadFile(hfile,buf,size_low,&size_hi,NULL)&&
                    (size_low == size_hi)) {
                        buf[size_low] = 0;
                    Process_Buffer(file,buf,size_low);
                    if (is_curr_box_valid_for_paint()) {
                        ok = true;
                    }
                }
                free(buf);
            }
        } else {
            sprtf("ERROR: File TOO large!\n");
        }
        CloseHandle(hfile);
    }
    if (ok) {
        Reset_Active_File(file);
        File_Reset();
    } else {
        sprtf("Loaded of [%s] FAILED\n",file);
    }
    if (pf2)
        free(pf2);
    return ok;
}

char *Get_Active_File_Dupe()
{
    char *pf = 0;
    char *cp = Get_Act_File();
    if (cp)
        pf = _strdup(cp);
    else {
        cp = Get_First_INI_File();
        if (cp)
            pf = _strdup(cp);
    }
    return pf;
}

void Auto_Load_file(HWND hWnd)
{
    LPTSTR lpstrFile = Get_Active_File_Dupe();
    if (lpstrFile) {
        Load_a_file(hWnd, lpstrFile);
        free(lpstrFile);
    }
}

VOID Do_ID_FILE_RE(HWND hwnd)
{
    char *pf = Get_Active_File_Dupe();
    if (pf) {
        Load_a_file(hwnd, pf);
        free(pf);
    }
}

//char szLIndOpn[] = "LastOpnFilterIndex";
//int g_iLIndOpn = 1; // 0=custom-filter, else 1, 2, ... for each pair in filter
//BOOL gChgLIO = FALSE;

BOOL g_bUseFullFN = TRUE;
VOID Do_ID_FILE_OPEN(HWND hWnd)
{
    OPENFILENAME ofn;
    char _buf[_MAX_PATH+16];
    char *cp = Get_Act_File();
    LPTSTR        lpstrFile = _buf;
    LPCTSTR       lpstrFilter = g_filter;
    bool          ok = false;
    LPTSTR lp = 0;
    char *tb = GetNxtBuf();
    DWORD dwi;

    *lpstrFile = 0;
    if (cp) 
        strcpy(lpstrFile,cp);

    memset(&ofn,0,sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFilter = lpstrFilter; // filter
    ofn.nFilterIndex = g_iLIndOpn;
    ofn.lpstrFile = lpstrFile;      // file name or blank
    ofn.nMaxFile = _MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName( &ofn )) {
        dwi = 0;
        if (g_bUseFullFN) {
            dwi = GetFullPathName(lpstrFile, //  _In_   LPCTSTR lpFileName,
                264, // _In_   DWORD nBufferLength,
                tb,  // _Out_  LPTSTR lpBuffer,
                &lp );  // _Out_  LPTSTR *lpFilePart
            
        }
        size_t len = strlen(lpstrFile);
        if ((len > 4) && (STRICMP(&lpstrFile[len-4],".xml") == 0)) {
            ok = Load_XML_File(hWnd, lpstrFile);
        } else {
            ok = Load_a_file(hWnd, lpstrFile);
        }
        if (ok) {
            // successful LOAD of a FILE
            // When the user selects a file, nFilterIndex returns the index of the currently displayed filter. 
            if (ofn.nFilterIndex != g_iLIndOpn) {
                // User CHANGED the index of the FILTER
                g_iLIndOpn = ofn.nFilterIndex;
                gChgLIO = TRUE;
            }
            //char szLPath[] = "LastPath";
            if (dwi && g_bUseFullFN && lp) {
                char *tb2 = GetNxtBuf();
                dwi = lp - tb;
                strcpy(tb2,tb);
                tb2[dwi] = 0;
                if (dwi && strcmp(tb2, g_szLastPath)) {
                    strcpy(g_szLastPath,tb2);
                    gChgLPath = TRUE;
                }
            }
        }
    }
}

// Add ASCII block is like -
// Filled with possibilities
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   ||123|123|123||123|123|123||123|123|123||  
// A ||456|456|456||456|456|456||456|456|456|| 1
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// B ||456|456|456||456|456|456||456|456|456|| 2
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// C ||456|456|456||456|456|456||456|456|456|| 3
//   ||789|789|789||789|789|789||789|789|789||  
//    =======================================
//   ||123|123|123||123|123|123||123|123|123||  
// D ||456|456|456||456|456|456||456|456|456|| 4
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// E ||456|456|456||456|456|456||456|456|456|| 5
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// F ||456|456|456||456|456|456||456|456|456|| 6
//   ||789|789|789||789|789|789||789|789|789||  
//    =======================================
//   ||123|123|123||123|123|123||123|123|123||  
// G ||456|456|456||456|456|456||456|456|456|| 7
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// H ||456|456|456||456|456|456||456|456|456|| 8
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// I ||456|456|456||456|456|456||456|456|456|| 9
//   ||789|789|789||789|789|789||789|789|789||  
//    =======================================
//      1   2   3    4   5   6    7   8   9
HANDLE OpenSaveFile( char *pf )
{
    HANDLE hfile = CreateFile(pf,
        GENERIC_READ|GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL );
    return hfile;
}

bool Save_a_File( HWND hWnd, char *pf, int flag )
{
    bool ok = false;
    // size_t len = _tcslen(lpstrFile);
    HANDLE hfile = OpenSaveFile(pf);
    if (VFH(hfile)) {
        size_t st = strlen(pf);
        if ((st < 4) || stricmp(&pf[st-4],".csv")) {
            // NO CSV output if NOT .csv extension
            flag |= sff_NO_ADD_CSV;
        }
        PABOX2 pb = get_curr_box();
        int row, col, val;
        DWORD wtn,len;
        char *ps = _s_buf;
        *ps = 0;
        if ( !(flag & sff_NO_ADD_CSV) ) {
            for (row = 0; row < 9; row++) {
                for ( col = 0; col < 9; col++ ) {
                    val = pb->line[row].val[col];
                    if (val) {
                        sprintf(EndBuf(ps),"%d",val);
                    }
                    if ((col + 1) < 9)
                        strcat(ps,",");
                }
                strcat(ps,EOL_CHRS);
            }
            strcat(ps,EOL_CHRS);
        }

        if ( !(flag & sff_NO_ADD_ASCII) ) {
            for (row = 0; row < 9; row++) {
                for ( col = 0; col < 9; col++ ) {
                    val = pb->line[row].val[col];
                    if (val)
                        sprintf(EndBuf(ps),"%d",val);
                    else
                        strcat(ps,NUL_VAL);
                }
            }
            strcat(ps,EOL_CHRS);
        }
        strcat(ps,EOL_CHRS);

        len = (DWORD)strlen(ps);
        WriteFile(hfile,ps,len,&wtn,NULL);

        if (flag & sff_ADD_ASCII_BLOCK) {
            int rc = 0;
            int cc = 0;
            PSET ps2;
            char *pfv = "*";
            char *cp = Get_Act_File();
            *ps = 0;
            if (cp)
                sprintf(ps,"// File: %s"EOL_CHRS, cp);
            strcat(ps,"//      1   2   3    4   5   6    7   8   9"EOL_CHRS);
            strcat(ps,"//    ======================================="EOL_CHRS);
            for (row = 0; row < 9; row++) {     // 9 ROWS
                //if (((row + 1) & 3) == 0)
                //    strcat(ps,"//    ======================================="EOL_CHRS);
                for (rc = 0; rc < 3; rc++) {    // each row in 3 rows
                    // commence a ROW
                    switch(rc) {
                    case 0:
                        strcat(ps,"//   ||");
                        break;
                    case 1:
                        sprintf(EndBuf(ps),"// %c ||", (row + 'A'));
                        break;
                    case 2:
                        strcat(ps,"//   ||");
                        break;
                    }
                    for (col = 0; col < 9; col++ ) {   // 9 COLS
                        val = pb->line[row].val[col];
                        ps2 = &pb->line[row].set[col];
                        for (cc = 0; cc < 3; cc++) {      // has col in 3 columns
                            if (val) {
                                switch(rc) {
                                case 0:
                                case 2:
                                    //   ||===|123|===||===|123|===||   | 23|  3||  
                                    switch(cc) {
                                    case 0:
                                        strcat(ps," ");
                                        break;
                                    case 1:
                                        strcat(ps,pfv);
                                        //strcat(ps," ");
                                        break;
                                    case 2:
                                        strcat(ps," ");
                                        break;
                                    }
                                    break;
                                case 1:
                                    // B ||=9=|   |=4=||=6=| 5 |=7=|| 5 | 5 | 5 || 2
                                    switch(cc) {
                                    case 0:
                                        strcat(ps,pfv);
                                        //strcat(ps,"[");
                                        break;
                                    case 1:
                                        sprintf(EndBuf(ps),"%d",val);
                                        break;
                                    case 2:
                                        strcat(ps,pfv);
                                        //strcat(ps,"]");
                                        break;
                                    }
                                    break;
                                }
                            } else {
                                // NO VALUE - fill in possibilities/candidates
                                if (flag & sff_ADD_CANDIDATES) {
                                    switch(rc) {
                                    case 0:
                                        // "//   ||123|123|123||123|123|123||123|123|123||  
                                        switch(cc) {
                                        case 0:
                                            if (ps2->val[0])
                                                sprintf(EndBuf(ps),"%d",ps2->val[0]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 1:
                                            if (ps2->val[1])
                                                sprintf(EndBuf(ps),"%d",ps2->val[1]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 2:
                                            if (ps2->val[2])
                                                sprintf(EndBuf(ps),"%d",ps2->val[2]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        }
                                        break;
                                    case 1:
                                        // // A ||456|456|456||456|456|456||456|456|456|| 1
                                        switch(cc) {
                                        case 0:
                                            if (ps2->val[3])
                                                sprintf(EndBuf(ps),"%d",ps2->val[3]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 1:
                                            if (ps2->val[4])
                                                sprintf(EndBuf(ps),"%d",ps2->val[4]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 2:
                                            if (ps2->val[5])
                                                sprintf(EndBuf(ps),"%d",ps2->val[5]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        }
                                        break;
                                    case 2:
                                        // //   ||789|789|789||789|789|789||789|789|789||  
                                        switch(cc) {
                                        case 0:
                                            if (ps2->val[6])
                                                sprintf(EndBuf(ps),"%d",ps2->val[6]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 1:
                                            if (ps2->val[7])
                                                sprintf(EndBuf(ps),"%d",ps2->val[7]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 2:
                                            if (ps2->val[8])
                                                sprintf(EndBuf(ps),"%d",ps2->val[8]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        }
                                        break;
                                    }
                                } else {
                                    strcat(ps," ");
                                }
                            }
                        }
                        // END OF CELL
                        strcat(ps,"|");
                        //if (col && ((col % 3) == 0))
                        if (((col + 1) % 3) == 0)
                            strcat(ps,"|");
                    }
                    // END OF EACH ROW
                    switch(rc) {
                    case 0:
                        strcat(ps,EOL_CHRS);
                        break;
                    case 1:
                        sprintf(EndBuf(ps)," %d"EOL_CHRS, (row + 1));
                        break;
                    case 2:
                        strcat(ps,EOL_CHRS);
                        break;
                    }
                }
                if ( ( (row + 1) % 3 ) == 0 )
                    strcat(ps,"//    ======================================="EOL_CHRS);
                else
                    strcat(ps,"//    ---------------------------------------"EOL_CHRS);
            }
            strcat(ps,"//      1   2   3    4   5   6    7   8   9"EOL_CHRS);
            sprintf(EndBuf(ps),EOL_CHRS"// eof - generated %s"EOL_CHRS, get_date_time_stg());
            len = (DWORD)strlen(ps);
            WriteFile(hfile,ps,len,&wtn,NULL);
        }

        CloseHandle(hfile);
        if ((len != wtn) && !(flag & sff_NO_LENGTH_CHECK)) {
            sprintf(ps,"WARNING: Writing to file [%s] FAILED!\nRequested write %d, but written %d\nCheck the FILE!\n",
                pf, len, wtn);
            sprtf("%s\n",ps);
            MB2(ps,"File Write Failed");
        } else {
            ok = true;
        }

        if ( !(flag & sff_NO_CHANGE_RESET) )
            g_bChanged = FALSE;

        if ( !(flag & sff_NO_CHANGE_ACTIVE) ) {
            char *cp = Get_Act_File();
            if ( !cp || strcmp(cp,pf) ) {
                // must CHANGE the active file name
                Reset_Active_File(pf);
            }
            cp = Get_Act_File();
            if (cp) {
                sprtf("Saved [%s]\n",cp);
                Add_to_INI_File_List(cp);   // reset the MENU to reflect this change
            }
        }

        if ( !(flag & sff_NO_RESET_TITLE) )
            Set_Window_Title(); // clear change from title
    }
    return ok;
}

bool Save_SDK_File( HWND hWnd, char *pf, int flag )
{
    bool ok = false;
    // size_t len = _tcslen(lpstrFile);
    HANDLE hfile = OpenSaveFile(pf);
    if (VFH(hfile)) {
        size_t st = strlen(pf);
        PABOX2 pb = get_curr_box();
        int row, col, val;
        DWORD wtn,len;
        char *ps = _s_buf;
        *ps = 0;
        for (row = 0; row < 9; row++) {
            for ( col = 0; col < 9; col++ ) {
                val = pb->line[row].val[col];
                if (val) {
                    sprintf(EndBuf(ps),"%d",val);
                } else {
                    strcat(ps,".");
                }
            }
            strcat(ps,EOL_CHRS);
        }
        strcat(ps,EOL_CHRS);

        len = (DWORD)strlen(ps);
        WriteFile(hfile,ps,len,&wtn,NULL);

        CloseHandle(hfile);
        if ((len != wtn) && !(flag & sff_NO_LENGTH_CHECK)) {
            sprintf(ps,"WARNING: Writing to file [%s] FAILED!\nRequested write %d, but written %d\nCheck the FILE!\n",
                pf, len, wtn);
            sprtf("%s\n",ps);
            MB2(ps,"File Write Failed");
        } else {
            ok = true;
        }

        if ( !(flag & sff_NO_CHANGE_RESET) )
            g_bChanged = FALSE;

        if ( !(flag & sff_NO_CHANGE_ACTIVE) ) {
            char *cp = Get_Act_File();
            if ( !cp || strcmp(cp,pf) ) {
                // must CHANGE the active file name
                Reset_Active_File(pf);
            }
            cp = Get_Act_File();
            if (cp) {
                sprtf("Saved [%s]\n",cp);
                Add_to_INI_File_List(cp);   // reset the MENU to reflect this change
            }
        }

        if ( !(flag & sff_NO_RESET_TITLE) )
            Set_Window_Title(); // clear change from title
    }
    return ok;
}


void Write_Temp_File()
{
    char *pf = g_szSvOptFl;
    if (*pf == 0) strcpy(pf,"temptemp.txt");
    DWORD opts = g_dwSvOptBits;
    bool ok = Save_a_File( NULL, pf, opts );
    size_t st = strlen(pf);
    if ((st < 4) || stricmp(&pf[st-4],".csv")) {
        // NO CSV output if NOT .csv extension
        opts |= sff_NO_ADD_CSV;
    }
    if (ok) {
        sprtf("Current written to [%s] opts %d\n",pf,opts);
    } else {
        sprtf("Failed to create [%s] opts %d\n",pf,opts);
    }
}

char xml_head[] = "<NewDataSet>\n"
"  <xs:schema id=\"NewDataSet\" xmlns=\"\" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:msdata=\"urn:schemas-microsoft-com:xml-msdata\">\n"
"    <xs:element name=\"NewDataSet\" msdata:IsDataSet=\"true\" msdata:UseCurrentLocale=\"true\">\n"
"      <xs:complexType>\n"
"        <xs:choice minOccurs=\"0\" maxOccurs=\"unbounded\">\n"
"          <xs:element name=\"Table1\">\n"
"            <xs:complexType>\n"
"              <xs:sequence>\n"
"                <xs:element name=\"spot\" type=\"xs:unsignedByte\" minOccurs=\"0\" />\n"
"              </xs:sequence>\n"
"            </xs:complexType>\n"
"          </xs:element>\n"
"        </xs:choice>\n"
"      </xs:complexType>\n"
"    </xs:element>\n"
"  </xs:schema>\n";

bool Save_XML_File(HWND hWnd, char *pf, int flag = 0);

bool Save_XML_File(HWND hWnd, char *pf, int flag)
{
    bool ok = false;
    HANDLE hfile = OpenSaveFile(pf);
    if (!VFH(hfile)) {
        char *tb = GetNxtBuf();
        sprintf(tb,"ERROR: Unable to create file [%s]!\n",pf);
        MB(tb);
        return false;
    }
    DWORD len, wtn;
    len = (DWORD)strlen(xml_head);
    WriteFile(hfile,xml_head,len,&wtn,NULL);
    PABOX2 pb = get_curr_box();
    int row, col, val;
    char *ps = _s_buf;
    *ps = 0;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            sprintf(EndBuf(ps),"  <Table1>\n    <spot>%d</spot>\n  </Table1>\n",val);

        }
    }
    strcat(ps,"</NewDataSet>\n");
    len = (DWORD)strlen(ps);
    WriteFile(hfile,ps,len,&wtn,NULL);
    CloseHandle(hfile);
    if ((len != wtn) && !(flag & sff_NO_LENGTH_CHECK)) {
        sprintf(ps,"WARNING: Writing to file [%s] FAILED!\nRequested write %d, but written %d\nCheck the FILE!\n",
            pf, len, wtn);
        sprtf("%s\n",ps);
        MB(ps);
    } else {
        ok = true;
    }

    if ( !(flag & sff_NO_CHANGE_RESET) )
        g_bChanged = FALSE;

    if ( !(flag & sff_NO_CHANGE_ACTIVE) ) {
        char *cp = Get_Act_File();
        if ( !cp || strcmp(cp,pf) ) {
            // must CHANGE the active file name
            Reset_Active_File(pf);
        }
        cp = Get_Act_File();
        if (cp) {
            sprtf("Saved [%s]\n",cp);
            Add_to_INI_File_List(cp);   // reset the MENU to reflect this change
        }
    }

    if ( !(flag & sff_NO_RESET_TITLE) )
        Set_Window_Title(); // clear change from title

    return ok;
}


VOID Do_ID_FILE_SAVE(HWND hWnd)
{
    OPENFILENAME ofn;
    char _buf[_MAX_PATH+16];
    LPTSTR        lpstrFile = _buf;
    LPCTSTR       lpstrFilter = g_filter;
    char *cp = Get_Act_File();

    *lpstrFile = 0;
    if (cp)
        strcpy(lpstrFile,cp);

    memset(&ofn,0,sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFilter = lpstrFilter;
    ofn.nFilterIndex = g_iLIndOpn;  // TODO - Separate Open and Write FILTER indexes
    ofn.lpstrFile = lpstrFile;
    ofn.nMaxFile = _MAX_PATH;
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 
    ofn.lpstrTitle = _T("Sudoku Save File");

    bool ok = false;
    LPTSTR lp = 0;
    char *tb = GetNxtBuf();
    DWORD dwi;
    if (GetSaveFileName( &ofn )) {
        // ==============================================================
        dwi = 0;
        if (g_bUseFullFN) {
            *tb = 0;
            dwi = GetFullPathName(lpstrFile, //  _In_   LPCTSTR lpFileName,
                264, // _In_   DWORD nBufferLength,
                tb,  // _Out_  LPTSTR lpBuffer,
                &lp );  // _Out_  LPTSTR *lpFilePart
            if (dwi && *tb && strcmp(lpstrFile,tb)) {
                lpstrFile = tb; // switch to using FULL PATH
            }
            
        }
        // ==============================================================
        size_t len = strlen(lpstrFile);
        if ((len > 4) && (stricmp(&lpstrFile[len-4],".xml") == 0)) {
            ok = Save_XML_File(hWnd, lpstrFile);
        } if ((len > 4) && (stricmp(&lpstrFile[len-4],".sdk") == 0)) {
            ok = Save_SDK_File(hWnd, lpstrFile);
        } else {
            ok = Save_a_File(hWnd, lpstrFile);
        }
        if (ok) {
            // SUCCESSFUL WRITE - Reset FILE NAME, etc
            // When the user selects a file, nFilterIndex returns the index of the currently displayed filter. 
            if (ofn.nFilterIndex != g_iLIndOpn) {
                // User CHANGED the index of the FILTER
                g_iLIndOpn = ofn.nFilterIndex;
                gChgLIO = TRUE;
            }
            //char szLPath[] = "LastPath";  // TODO - separate last path read and write
            if (dwi && g_bUseFullFN && lp) {
                char *tb2 = GetNxtBuf();
                dwi = lp - tb;
                strcpy(tb2,tb);
                tb2[dwi] = 0;
                if (dwi && strcmp(tb2, g_szLastPath)) {
                    strcpy(g_szLastPath,tb2);
                    gChgLPath = TRUE;
                }
            }
            Reset_Active_File(lpstrFile);
            File_Reset();
        } else {
            // WRITE FAILED - Offer to TRY AGAIN

        }
    }

}

// Browse for folder

// (a) lpIDList = SHBrowseForFolder(tBrowseInfo)
#if 0 // This FAILED - missing the definition OFN_PROJECT
// (b) GetOpenFileName()/GetSaveFileName() using OFN_PROJECT 
BOOL Get_Directory_Name(HWND hWnd, char *lpstrFile)
{
    BOOL bRet = FALSE;
    OPENFILENAME ofn;
    memset(&ofn,0,sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    // ofn.lpstrFilter = lpstrFilter;
    ofn.lpstrFile = lpstrFile;
    ofn.nMaxFile = _MAX_PATH;
    ofn.Flags = OFN_PROJECT; // OFN_PROJECT;    // OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 
    ofn.lpstrTitle = _T("Sudoku Open Directory");

    //if (GetSaveFileName( &ofn )) {
    //    bRet = TRUE;
    //}
    if (GetOpenFileName( &ofn )) {
        bRet = TRUE;
    }
    return bRet;
}
#endif // 0 - missing a definition

/* ====================================
typedef struct _browseinfo {
  HWND              hwndOwner;
  PCIDLIST_ABSOLUTE pidlRoot;
  LPTSTR            pszDisplayName;
  LPCTSTR           lpszTitle;
  UINT              ulFlags;
  BFFCALLBACK       lpfn;
  LPARAM            lParam;
  int               iImage;
} BROWSEINFO, *PBROWSEINFO, *LPBROWSEINFO;
   ==================================== */
// For Windows Vista or later, it is recommended that you use IFileDialog with 
// the FOS_PICKFOLDERS option rather than the SHBrowseForFolder function. 
// This uses the Open Files dialog in pick folders mode and is the preferred 
// implementation.
#if 0 // This worked FINE, but no intial folder - quite important
#include <Shlobj.h>
static BROWSEINFO sBI;
bool dn_one = false;
bool dn_coinit = false;
BOOL Get_Directory_Name(HWND hWnd, char *lpstrFile)
{
    BOOL bRet = FALSE;
    if (!dn_coinit) {
        DWORD dwCoInit = COINIT_APARTMENTTHREADED;
        HRESULT res = CoInitializeEx( NULL, dwCoInit );
        if (!SUCCEEDED(res)) {
            sprtf("WARNING: CoInitializEx(COINIT_APARTMENTTHREADED) FAILED\n");
        }
        dn_coinit = true;
    }
    if (!dn_one) {
        memset(&sBI,0,sizeof(BROWSEINFO));
        dn_one = true;
    }

    sBI.hwndOwner = hWnd;
    sBI.pszDisplayName = lpstrFile;
    sBI.lpszTitle = "Select a Directory";
    sBI.ulFlags = BIF_NONEWFOLDERBUTTON | BIF_USENEWUI;

    PIDLIST_ABSOLUTE lpIDList = SHBrowseForFolder(&sBI);
    if (lpIDList) {
        bRet = TRUE;
       // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( lpIDList );
            imalloc->Release ( );
        }
    }
    return bRet;
}
#endif // 0

BOOL Get_Directory_Name(HWND hWnd, char *lpstrFile)
{
    char *tb = GetNxtBuf();
    BOOL bRet = XBrowseForFolder( hWnd,
					  lpstrFile,
                      "Select Folder to Scan for Sudoku files",
                      tb,
                      264 );
    if (bRet) {
        strcpy(lpstrFile,tb);

    }
    return bRet;
}

int Test_Extract_ASCII_Info( PABOX2 pb, char *pbuf, int len )
{
    int row = 0;
    int col = 0;
    int val;
    int c, i;
    int count = 0;
    for (i = 0; i < len; i++) {
        c = pbuf[i];
        if (ISDIGIT1(c)) {
            val = c - '0';
            pb->line[row].val[col++] = val;
            count++;
        } else {
            pb->line[row].val[col++] = 0;
        }
        if (col >= 9) {
            row++;
            col = 0;
            if (row >= 9)
                break;
        }
    }
    return 0;
}

int Test_Extract_CSV_Info(PABOX2 pb, char *pbuf, int len)
{
    int i, c, val, j;
    int row = 0;
    int col = 0;
    int count;
    bool hadc = true;
    char *lines[11];
    char *cp;
    row = 0;
    val = 0;
    size_t cvssize = 18;
    cp = &pbuf[0];

    for(i = 0; i < len; i++) {
        c = pbuf[i];
        if (( c == '\r' )||( c == '\n' )) {
            lines[row++] = cp;
            pbuf[i] = 0;
            i++;
            for(; i < len; i++) {
                c = pbuf[i];
                if ( c > ' ' ) {
                    j = (int)strlen(cp);
                    val = i;
                    cp = &pbuf[val];
                    i--;
                    break;
                }
                pbuf[i] = 0;
            }
            if (row > 9)
                break;
        }
    }
    lines[row++] = cp;
    if (row < 9) {
        return 1;   // not enough lines
    }
    count = 0;
    for (row = 0; row < 9; row++) {
        cp = lines[row];
        len = strlen(cp); // minimum line case ,,,,,,,, = 8
        SET set;
        for (col = 0; col < 9; col++) {
            set.val[col] = col + 1; // fill a full set of 9 value
        }
        col = 0;
        hadc = true;   // begin with - had a comma
        for (j = 0; j < len; j++) {
            c = cp[j];
            if ( c == ',' ) {
                if (hadc)
                    pb->line[row].val[col++] = 0;
                hadc = true;
            } else if ( c <= ' ' ) {
                // skip
            } else if ( ISDIGIT(c) ) {
                if (hadc) {
                    val = c - '0';
                    pb->line[row].val[col++] = val;
                    if (val) {
                        count++;
                        set.val[val - 1] = 0;
                    }
                }
                hadc = 0;
            }
            if (col >= 9)
                break;
        }
        if (hadc) {
            if (col < 9)
                pb->line[row].val[col++] = 0;
        }
        if (col < 9) {
            return 1;
        }
        // got to next row
    }   // for each ROW
    return 0;
}

int Test_Extract_XML_Info( PABOX2 pb, char *buf, int len )
{
    int ok = -1;
    int i, c;
    char tmp[MAX_STRING];
    GRID grid;
    int row = 0;
    int col = 0;
    char *pt = tmp;
    int off = 0;
    int inquot = 0;
    int val;
    bool bad = false;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            grid.v[row][col] = -1;
        }
    }
    row = 0;
    col = 0;
    for (i = 0; i < len; i++) {
        c = buf[i];
        if (c == '<') {
            i++;
            off = 0;
            for (; i < len; i++) {
                c = buf[i];
                if (inquot) {
                    pt[off++] = (char)c;
                    if ( c == '"' )
                        inquot = 0;
                } else {
                    if ( c == '"' ) {
                        inquot = 1;
                        continue;
                    }
                    if ( c == '>' ) {
                        pt[off] = 0;
                        if (strcmp(pt,"spot") == 0)
                        {
                            i++;
                            off = 0;
                            for (; i < len; i++) {
                                c = buf[i];
                                if ( ISDIGIT(c) ) {
                                    pt[off++] = (char)c;
                                } else if ( c == '<' ) {
                                    break;
                                }
                            }
                            pt[off] = 0;
                            if (off) {
                                val = atoi(pt);
                                if (VALIDVAL(val)) {
                                    grid.v[row][col++];
                                    if (col >= 9) {
                                        row++;
                                        col = 0;
                                    }
                                } else {
                                    bad = true;
                                }
                            }
                            break;
                        }
                    } else {
                        pt[off++] = (char)c; // collect token
                    }
                }
                if (bad)
                    break;
            }
        }
        if (bad)
            break;
        if (row >= 9)
            break;
    }
    if (!bad) {
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = grid.v[row][col];
                if (!VALIDVAL(val)) {
                    bad = true;
                    break;
                }
            }
            if (bad == true)
                break;
        }
    }
    if (!bad) {
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = grid.v[row][col];
                pb->line[row].val[col] = val;
            }
        }
        ok = 0;
    }
    return ok;
}


void Invalidate_GRID( PGRID pg )
{
    int row, col;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            pg->v[row][col] = -1;
        }
    }
}

bool Is_GRID_Valid( PGRID pg )
{
    int row, col, val;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pg->v[row][col];
            if (!val) continue;
            if (!VALUEVALID(val))
                return false;
        }
    }
    return true;
}



// Looking for 9 lines like 
// 6.......7
// ....9..2.
// 3.1..259.
// 8....7.13
// ....8....
// 76.3....8
// .782..1.6
// .5..3....
// 2.......9
int Test_Extract_SDK_Info( PABOX2 pb, char *pbuf, int len )
{
    int ok = -1;
    int i, c, row, col, i2;
    GRID g;
    bool bad;
    char *cp;
    for (i = 0; i < len; i++) {
        cp = &pbuf[i];
        c = pbuf[i];
        if (ISDIGIT(c) || (c == '.')) {
            Invalidate_GRID(&g);
            row = col = 0;
            bad = false;
            for (i2 = i; i2 < len; i2++) {
                c = pbuf[i2];
                if (c == ' ') continue;
                if (EOLCHAR(c)) {
                    if (col < 9) {
                        bad = true;
                        break;
                    }
                    col = 0;
                    row++;
                    if (row >= 9)
                        break;
                    i2++;
                    for (; i2 < len; i2++) {
                        c = pbuf[i2];
                        if (EOLCHAR(c)) continue;
                        if (ISDIGIT(c) || (c == '.')) {
                            i2--;
                            break;
                        } else if ( c == ' ' )
                            continue;
                        else {
                            bad = true;
                            break;
                        }
                    }
                    if (bad) break;
                    continue;
                }
                if (ISDIGIT(c))
                    c -= '0';
                else if (c == '.')
                    c = 0;
                g.v[row][col] = c;
                col++;
                if (col > 9) {
                    bad = true;
                    break;
                }
            }
            if (!bad && Is_GRID_Valid(&g))
                return 0;

        } else {
            i++;
            for (; i < len; i++) {
                c = pbuf[i];
                if (EOLCHAR(c)) {
                    break;
                }
            }
            for (; i < len; i++) {
                c = pbuf[i];
                if (!EOLCHAR(c)) {
                    i--;
                    break;
                }
            }
        }
    }

    return ok;
}

int Extract_SDK_Info( char *pbuf, int len )
{
    int ok = -1;
    int i, c, row, col, i2;
    GRID g;
    bool bad;
    for (i = 0; i < len; i++) {
        c = pbuf[i];
        if (ISDIGIT(c) || (c == '.')) {
            Invalidate_GRID(&g);
            row = col = 0;
            bad = false;
            for (i2 = i; i2 < len; i2++) {
                c = pbuf[i2];
                if (c == ' ') continue;
                if (EOLCHAR(c)) {
                    if (col < 9) {
                        bad = true;
                        break;
                    }
                    col = 0;
                    row++;
                    if (row >= 9)
                        break;
                    i2++;
                    for (; i2 < len; i2++) {
                        c = pbuf[i2];
                        if (EOLCHAR(c)) continue;
                        if (ISDIGIT(c) || (c == '.')) {
                            i2--;
                            break;
                        } else if ( c == ' ' )
                            continue;
                        else {
                            bad = true;
                            break;
                        }
                    }
                    if (bad) break;
                    continue;
                }
                if (ISDIGIT(c))
                    c -= '0';
                else if (c == '.')
                    c = 0;
                g.v[row][col] = c;
                col++;
                if (col > 9) {
                    bad = true;
                    break;
                }
            }
            if (!bad && Is_GRID_Valid(&g)) {
                PABOX2 pb = get_curr_box();
                invalidate_box(pb);
                int val;
                for (row = 0; row < 9; row++) {
                    for (col = 0; col < 9; col++) {
                        val = g.v[row][col];
                        pb->line[row].val[col] = val;
                    }
                }
                return 0;
            }

        } else {
            i++;
            for (; i < len; i++) {
                c = pbuf[i];
                if (EOLCHAR(c)) {
                    break;
                }
            }
            for (; i < len; i++) {
                c = pbuf[i];
                if (!EOLCHAR(c)) {
                    i--;
                    break;
                }
            }
        }
    }

    return ok;
}


int Test_Process_Buffer(PABOX2 pb, char *file, char * pbuf, int len )
{
    int max_len, cvs_cnt, ret;
    int lines = Count_Lines_In_Buffer( pbuf, len,
        &max_len, // line with maximum length, if FIRST line
        &cvs_cnt ); // lines with 8 or greater chars = cvs lines, like ,,,,,,,,

    if ( (lines >= 9) && (cvs_cnt >= 9) ) {
        invalidate_box(pb);
        ret = Test_Extract_CSV_Info( pb, pbuf, len );   // treat as a CVS file
        if ((ret == 0) && (validate_box(pb) == 0))
            return 0;
    }
    // Hmmm, if we had a line with 81 (9x9) chars, then maybe just simple ASCII line
    if (max_len >= 81) {
        invalidate_box(pb);
        ret =Test_Extract_ASCII_Info( pb, pbuf, len );
        if ((ret == 0) && (validate_box(pb) == 0))
            return 0;
    }
    invalidate_box(pb);
    ret = Test_Extract_XML_Info( pb, pbuf, len );
    if ((ret == 0) && (validate_box(pb) == 0))
        return 0;

    invalidate_box(pb);
    ret = Test_Extract_SDK_Info( pb, pbuf, len );
    if ((ret == 0) && (validate_box(pb) == 0))
        return 0;

    // other formats??????
    return -1;
}


bool Test_Sudoku_Load(PABOX2 pb, char *pfile)
{
    bool ok = false;
    char *pf2 = _strdup(pfile);
    if (pf2)
        pfile = pf2;
    HANDLE hfile = CreateFile(pfile,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL );
    if (VFH(hfile)) {
        DWORD size_hi = 0;
        DWORD size_low = GetFileSize(hfile, &size_hi);
        if (size_hi == 0) {
            char *buf = (char *)malloc(size_low+16);
            if (buf) {
                if (ReadFile(hfile,buf,size_low,&size_hi,NULL)&&
                    (size_low == size_hi))
                {
                    buf[size_low] = 0;
                    if ((Test_Process_Buffer(pb,pfile,buf,size_low) == 0) &&
                        ( validate_box(pb) == 0 ) )
                    {
                        ok = true;
                    }
                }
                free(buf);
            }
        }
        CloseHandle(hfile);
    }
    if (pf2)
        free(pf2);
    return ok;
}

int Get_File_List( char *pdir, vSTG *vstg )
{
    int count = 0;
    int c;
    size_t len = strlen(pdir);
    if (!len) return 0;

    PABOX2 pb = new ABOX2;
    if (!pb) return 0;

    char *tb = GetNxtBuf();
    char *tb2 = GetNxtBuf();
    char *tb3 = GetNxtBuf();

    strcpy(tb,pdir);
    c = tb[len-1];
    if ( !((c == '/')||(c == '\\')) )
        strcat(tb,"\\");
    strcpy(tb2,tb);
    strcat(tb,"*.*");

    WIN32_FIND_DATA fd;
    HANDLE hFind;
    string s;

    hFind = FindFirstFile( tb, &fd );
    if (hFind && (hFind != INVALID_HANDLE_VALUE)) {
        do {
            if ( !( (strcmp( fd.cFileName, "." ) == 0) || (strcmp( fd.cFileName, ".." ) == 0) ) ) {
                strcpy(tb3,tb2);
                strcat(tb3,fd.cFileName);
                if (Test_Sudoku_Load(pb, tb3)) {
                    s = tb3;
                    vstg->push_back(s);
                    count++;
                }
            }
        } while (FindNextFile(hFind,&fd));

    }
    return count;
}

BOOL InitListTab(HWND hListTab, int count, char **tabString )
{
	TCITEM tie;
	tie.mask = TCIF_TEXT; // | TCIF_IMAGE;
	tie.iImage = -1;
	for(int i = 0; i < count; i++)
	{
		tie.pszText = tabString[i];
		SendMessage(hListTab, TCM_INSERTITEM, 0, (LPARAM)&tie);
	}
    return TRUE;
}


BOOL Fill_List_Box(HWND hDlg, HWND hList, vSTG *pvstg)
{
    size_t max = pvstg->size();
    size_t ii;
    string s;

    HFONT hf = Get_Font();
    if (hf) SendMessage(hList, WM_SETFONT, (WPARAM)hf, TRUE);
    char *hdg = "Sudoku Files";
    char *stgs[1];
    stgs[0] = hdg;
    //if ( !InitListTab( hList, 1, stgs) ) {
    if ( !LV_InitColumns( hList, 1, stgs) ) {
        return FALSE;
    }
    for (ii = 0; ii < max; ii++) {
        s = pvstg->at(ii);
        if ( !LV_InsertItem( hList, (char *)s.c_str()) )
            return FALSE;

    }
    LVAddExStyle(hList, LVS_EX_FULLROWSELECT);
    return TRUE;
}

HWND hTableList = 0;
TCHAR itemSelected[MAX_PATH];

void GetItemText( HWND hDlg, HWND hList, const int &iSel, TCHAR * text )
{
    TCHAR item[MAX_PATH] = {0};
	LVITEM listItem;
	listItem.mask = LVIF_TEXT;
	listItem.iItem = iSel;
	listItem.pszText = item;
	listItem.cchTextMax = MAX_PATH;
    int i = 0;
    // if there were MULTIPLE columns, but here is just 1
	//for(i = 0; i<= 6; i++)
	//{
		listItem.iSubItem = i;
		SendMessage(hList, LVM_GETITEMTEXT, iSel, (LPARAM)&listItem);
		strcat(text, item);
	//}
}


INT_PTR Do_INIT_FileList(HWND hDlg, LPARAM lParam)
{
    vSTG *pvstg = (vSTG *)lParam;
    if (!pvstg || (pvstg->size() == 0))
        return (INT_PTR)FALSE;
    HWND hList = GetDlgItem(hDlg, IDC_LIST3); // IDC_LIST1);
    if (!hList)
        return (INT_PTR)FALSE;
    hTableList = hList;
    if ( !Fill_List_Box(hDlg, hList, pvstg) )
        return (INT_PTR)FALSE;

    // disable OK button until a SELECTION is made
    EnableWindow( GetDlgItem( hDlg, IDOK), FALSE );

    CenterWindow(hDlg, g_hWnd);
    return (INT_PTR)TRUE;
}


// Message handler for about box.
INT_PTR CALLBACK View_File_List(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        return Do_INIT_FileList(hDlg,lParam);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
        }
		break;
	case WM_NOTIFY:
       	LPNMLISTVIEW pnm;
      	int iIndex;
		switch(LOWORD(wParam))
		{
		case IDC_LIST3:
			pnm = (LPNMLISTVIEW)lParam;
			if(((LPNMHDR)lParam)->code == NM_CLICK)
			{
				// get current selection
				iIndex = (int)SendMessage(hTableList, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
				if(iIndex == -1)
					return FALSE;
                itemSelected[0] = 0;
				GetItemText(hDlg, hTableList, iIndex, itemSelected);
                if (is_file_or_directory(itemSelected) == 1) {
                    // show the SELECTION
                    SetDlgItemText(hDlg,IDC_EDIT1,itemSelected);
                    // enable OK button now a SELECTION valid file made
                    EnableWindow( GetDlgItem( hDlg, IDOK), TRUE );
                }
				return FALSE;
			}
			break;
        }
	}
	return (INT_PTR)FALSE;
}

BOOL Load_Selected_File( HWND hWnd, char *pfile )
{
    BOOL bRet = FALSE;
    char *tb;
    int res;
    char *cp3 = Get_Act_File();
    if (cp3 && (strcmp(pfile,cp3) == 0)) {
        // hmmm, already loaded - what to do???
        bRet = TRUE;
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
        if (Load_a_file( hWnd, pfile )) {
            bRet = TRUE;
        }
    }
    return bRet;
}


VOID Do_VIEW_FILELIST( HWND hWnd, vSTG *plist )
{
    bool done = false;
    while (!done) {
        done = true;
        INT_PTR res = DialogBoxParam(
            hInst, // _In_opt_  HINSTANCE hInstance,
            MAKEINTRESOURCE(IDD_LISTBOX), // _In_      LPCTSTR lpTemplateName,
            hWnd,   // _In_opt_  HWND hWndParent,
            View_File_List, // _In_opt_  DLGPROC lpDialogFunc,
            (LPARAM)plist ); //  _In_      LPARAM dwInitParam
        if (res == IDOK) {
            char *pfile = itemSelected;
            sprtf("Selected [%s]\n", pfile);
            if (is_file_or_directory(pfile) == 1) {
                if ( !Load_Selected_File( hWnd, pfile ) ) {
                    char *tb = GetNxtBuf();
                    sprintf(tb,"Warning, attempt to load file" MEOL
                        "%s" MEOL
                        "FAILED!" MEOL
                        "Click OK to Select again" MEOL,
                        pfile );
                    res = MB2(tb,"Advice of Failure");
                    if (res == IDYES) {
                        done = false; // back for another selection
                    }
                }
            }
        }
    }
}

VOID Do_ID_FILE_OPENDIRECTORY(HWND hWnd)
{
    DWORD dwi;
    char *tb = GetNxtBuf();
    char *tb2 = GetNxtBuf();
    char *tb3 = Get_Act_File();

    *tb = 0;
    if (tb3) {
        dwi = GetFullPathName( tb3, 264, tb2, 0 );
        if (dwi)
            strcpy(tb,tb2);
        else
            strcpy(tb,tb3);
        size_t len = strlen(tb);
        size_t ii;
        if (len) {
            int c;
            for (ii = len - 1; ii > 0; ii--) {
                c = tb[ii];
                if ((c == '\\')||(c == '/')) {
                    tb[ii] = 0;
                    break;
                }
            }
        }
    }
    if ((tb[0] == 0) && g_szLastPath[0]) {
        strcpy(tb,g_szLastPath);
    }

    if (Get_Directory_Name(hWnd,tb)) {
        vSTG list;
        sprtf("Got DIR: [%s]\n", tb);
        dwi = GetFullPathName( tb, 264, tb2, 0 ); 
        if (dwi && strcmp(tb,tb2)) {
            tb = tb2;
            sprtf("Got DIR: [%s]\n", tb);
        }
        int cnt = Get_File_List( tb, &list );
        if (cnt && list.size()) {
            sprtf("Found %d valid files...(%d)\n",cnt, (int)list.size());
            Do_VIEW_FILELIST( hWnd, &list );
        }
    }
    set_repaint();
}

// *********************************************

///////////////////////////////////////////////////////////////////////////////////////////
#else // !#ifdef WIN32
///////////////////////////////////////////////////////////////////////////////////////////
VOID Do_ID_FILE_OPEN(HWND hWnd)
{
    sprtf("Do_ID_FILE_OPEN not yet implemented in unix\n");
}
VOID Do_ID_FILE_OPENDIRECTORY(HWND hWnd)
{
    sprtf("Do_ID_FILE_OPENDIRECTORY not yet implemented in unix\n");
}
VOID Do_VIEW_FILELIST( HWND hWnd, vSTG *plist )
{
    sprtf("Do_VIEW_FILELIST not yet implemented in unix\n");

}
void Set_Window_Title()
{

}

bool Save_a_File2( HWND hWnd, char *pf, int flag );

void Write_Temp_File()
{
    char *pf = g_szSvOptFl;
    if (*pf == 0) strcpy(pf,"temptemp.txt");
    DWORD opts = g_dwSvOptBits;
    bool ok = Save_a_File2( NULL, pf, opts );
    size_t st = strlen(pf);
    if ((st < 4) || STRICMP(&pf[st-4],".csv")) {
        // NO CSV output if NOT .csv extension
        opts |= sff_NO_ADD_CSV;
    }
    if (ok) {
        sprtf("Current written to [%s] opts %d\n",pf,opts);
    } else {
        sprtf("Failed to create [%s] opts %d\n",pf,opts);
    }
}

VOID Do_ID_FILE_SAVE(HWND hWnd)
{
    sprtf("TODO: Do_ID_FILE_SAVE not yet ported!\n");
}

///////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n

bool Save_a_File2( HWND hWnd, char *pf, int flag )
{
    bool ok = false;
    FILE *hfile = fopen(pf,"w");
    DWORD wtn = 1;
    DWORD len = 2;
    if (hfile) {
        size_t st = strlen(pf);
        if ((st < 4) || STRICMP(&pf[st-4],".csv")) {
            // NO CSV output if NOT .csv extension
            flag |= sff_NO_ADD_CSV;
        }
        PABOX2 pb = get_curr_box();
        int row, col, val;
        char *ps = _s_buf;
        *ps = 0;
        if ( !(flag & sff_NO_ADD_CSV) ) {
            for (row = 0; row < 9; row++) {
                for ( col = 0; col < 9; col++ ) {
                    val = pb->line[row].val[col];
                    if (val) {
                        sprintf(EndBuf(ps),"%d",val);
                    }
                    if ((col + 1) < 9)
                        strcat(ps,",");
                }
                strcat(ps,EOL_CHRS);
            }
            strcat(ps,EOL_CHRS);
        }

        if ( !(flag & sff_NO_ADD_ASCII) ) {
            for (row = 0; row < 9; row++) {
                for ( col = 0; col < 9; col++ ) {
                    val = pb->line[row].val[col];
                    if (val)
                        sprintf(EndBuf(ps),"%d",val);
                    else
                        strcat(ps,NUL_VAL);
                }
            }
            strcat(ps,EOL_CHRS);
        }
        strcat(ps,EOL_CHRS);

        len = (DWORD)strlen(ps);
        // WriteFile(hfile,ps,len,&wtn,NULL);
        wtn = fwrite(ps,1,len,hfile);
        if (wtn != len) {
            fclose(hfile);
            return false;
        }

        if (flag & sff_ADD_ASCII_BLOCK) {
            int rc = 0;
            int cc = 0;
            PSET ps2;
            char *pfv = "*";
            char *cp = Get_Act_File();
            *ps = 0;
            if (cp)
                sprintf(ps,"// File: %s"EOL_CHRS, cp);
            strcat(ps,"//      1   2   3    4   5   6    7   8   9"EOL_CHRS);
            strcat(ps,"//    ======================================="EOL_CHRS);
            for (row = 0; row < 9; row++) {     // 9 ROWS
                //if (((row + 1) & 3) == 0)
                //    strcat(ps,"//    ======================================="EOL_CHRS);
                for (rc = 0; rc < 3; rc++) {    // each row in 3 rows
                    // commence a ROW
                    switch(rc) {
                    case 0:
                        strcat(ps,"//   ||");
                        break;
                    case 1:
                        sprintf(EndBuf(ps),"// %c ||", (row + 'A'));
                        break;
                    case 2:
                        strcat(ps,"//   ||");
                        break;
                    }
                    for (col = 0; col < 9; col++ ) {   // 9 COLS
                        val = pb->line[row].val[col];
                        ps2 = &pb->line[row].set[col];
                        for (cc = 0; cc < 3; cc++) {      // has col in 3 columns
                            if (val) {
                                switch(rc) {
                                case 0:
                                case 2:
                                    //   ||===|123|===||===|123|===||   | 23|  3||  
                                    switch(cc) {
                                    case 0:
                                        strcat(ps," ");
                                        break;
                                    case 1:
                                        strcat(ps,pfv);
                                        //strcat(ps," ");
                                        break;
                                    case 2:
                                        strcat(ps," ");
                                        break;
                                    }
                                    break;
                                case 1:
                                    // B ||=9=|   |=4=||=6=| 5 |=7=|| 5 | 5 | 5 || 2
                                    switch(cc) {
                                    case 0:
                                        strcat(ps,pfv);
                                        //strcat(ps,"[");
                                        break;
                                    case 1:
                                        sprintf(EndBuf(ps),"%d",val);
                                        break;
                                    case 2:
                                        strcat(ps,pfv);
                                        //strcat(ps,"]");
                                        break;
                                    }
                                    break;
                                }
                            } else {
                                // NO VALUE - fill in possibilities/candidates
                                if (flag & sff_ADD_CANDIDATES) {
                                    switch(rc) {
                                    case 0:
                                        // "//   ||123|123|123||123|123|123||123|123|123||  
                                        switch(cc) {
                                        case 0:
                                            if (ps2->val[0])
                                                sprintf(EndBuf(ps),"%d",ps2->val[0]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 1:
                                            if (ps2->val[1])
                                                sprintf(EndBuf(ps),"%d",ps2->val[1]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 2:
                                            if (ps2->val[2])
                                                sprintf(EndBuf(ps),"%d",ps2->val[2]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        }
                                        break;
                                    case 1:
                                        // // A ||456|456|456||456|456|456||456|456|456|| 1
                                        switch(cc) {
                                        case 0:
                                            if (ps2->val[3])
                                                sprintf(EndBuf(ps),"%d",ps2->val[3]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 1:
                                            if (ps2->val[4])
                                                sprintf(EndBuf(ps),"%d",ps2->val[4]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 2:
                                            if (ps2->val[5])
                                                sprintf(EndBuf(ps),"%d",ps2->val[5]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        }
                                        break;
                                    case 2:
                                        // //   ||789|789|789||789|789|789||789|789|789||  
                                        switch(cc) {
                                        case 0:
                                            if (ps2->val[6])
                                                sprintf(EndBuf(ps),"%d",ps2->val[6]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 1:
                                            if (ps2->val[7])
                                                sprintf(EndBuf(ps),"%d",ps2->val[7]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        case 2:
                                            if (ps2->val[8])
                                                sprintf(EndBuf(ps),"%d",ps2->val[8]);
                                            else
                                                strcat(ps," ");
                                            break;
                                        }
                                        break;
                                    }
                                } else {
                                    strcat(ps," ");
                                }
                            }
                        }
                        // END OF CELL
                        strcat(ps,"|");
                        //if (col && ((col % 3) == 0))
                        if (((col + 1) % 3) == 0)
                            strcat(ps,"|");
                    }
                    // END OF EACH ROW
                    switch(rc) {
                    case 0:
                        strcat(ps,EOL_CHRS);
                        break;
                    case 1:
                        sprintf(EndBuf(ps)," %d"EOL_CHRS, (row + 1));
                        break;
                    case 2:
                        strcat(ps,EOL_CHRS);
                        break;
                    }
                }
                if ( ( (row + 1) % 3 ) == 0 )
                    strcat(ps,"//    ======================================="EOL_CHRS);
                else
                    strcat(ps,"//    ---------------------------------------"EOL_CHRS);
            }
            strcat(ps,"//      1   2   3    4   5   6    7   8   9"EOL_CHRS);
            sprintf(EndBuf(ps),EOL_CHRS"// eof - generated %s"EOL_CHRS, get_date_time_stg());
            len = (DWORD)strlen(ps);
            //WriteFile(hfile,ps,len,&wtn,NULL);
            wtn = fwrite(ps,1,len,hfile);
            //if (wtn != len) {
            //    fclose(hfile);
            //    return false;
            //}
        }

        //CloseHandle(hfile);
        fclose(hfile);
        if ((len != wtn) && !(flag & sff_NO_LENGTH_CHECK)) {
            sprintf(ps,"WARNING: Writing to file [%s] FAILED!\nRequested write %d, but written %d\nCheck the FILE!\n",
                pf, len, wtn);
            sprtf("%s\n",ps);
            MB2(ps,"File Write Failed");
        } else {
            ok = true;
        }

        if ( !(flag & sff_NO_CHANGE_RESET) )
            g_bChanged = FALSE;

        if ( !(flag & sff_NO_CHANGE_ACTIVE) ) {
            char *cp = Get_Act_File();
            if ( !cp || strcmp(cp,pf) ) {
                // must CHANGE the active file name
                Reset_Active_File(pf);
            }
            cp = Get_Act_File();
            if (cp) {
                sprtf("Saved [%s]\n",cp);
                Add_to_INI_File_List(cp);   // reset the MENU to reflect this change
            }
        }

        if ( !(flag & sff_NO_RESET_TITLE) )
            Set_Window_Title(); // clear change from title
    }
    return ok;
}

// eof - Sudo_File.cxx
