// Sudo_Ini.cxx
#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif

#include "Sudoku.hxx"
#include "Sudo_Paint.hxx"
#include "Sudo_Menu.hxx"
#include "Sudo_Ini.hxx"
#ifdef WIN32    // include 'direct.h' for _getcwd()
#include <direct.h> // for _getcwd()
#endif  // WIN32 'direct.h'
#include <fstream>

#ifndef STRCMPFIL
#ifdef WIN32    // windows STRCMPFIL macro = strcmpi
#define STRCMPFIL  strcmpi
#else   // !WIN32 - STRCMPFIL macro = strcmp
#define STRCMPFIL  strcmp
#endif  // WIN32 y/n - compare file macro
#endif
#ifndef STRCMPI
#ifdef WIN32    // windows STRCMPFIL macro = strcmpi
#define STRCMPI  strcmpi
#else   // !WIN32 - STRCMPFIL macro = strcmp
#define STRCMPI  strcasecmp
#endif  // WIN32 y/n - compare file macro
#endif

int add_debug_ini = 0;

/////////////////////////////////////////////////////////////////////////////
#ifdef WIN32    // windows formats and INI read/write
/////////////////////////////////////////////////////////////////////////////
#define MY3VALS "%d,%d,%d"
#define MY4VALS "%d,%d,%d,%d"

#define	GetStg( a, b )	\
	m_ReadProf( a, b, &szBlk[0], lpb, 256, lpini )

#undef WritePrivateProfileString
#define WritePrivateProfileString m_WriteProf

/////////////////////////////////////////////////////////////////////////////
#else // !#ifdef WIN32    // unix formats and INI read/write
/////////////////////////////////////////////////////////////////////////////
#include <errno.h>
/* ************************************************************************
   *** port of INI file handling ***
   =================================

#define	GetStg( a, b )	\
	GetPrivateProfileString( a, b, &szBlk[0], lpb, 256, lpini )

SYNTAX:
DWORD WINAPI GetPrivateProfileString(
  _In_  LPCTSTR lpAppName,
  _In_  LPCTSTR lpKeyName,
  _In_  LPCTSTR lpDefault,
  _Out_ LPTSTR  lpReturnedString,
  _In_  DWORD   nSize,
  _In_  LPCTSTR lpFileName
);

BOOL WINAPI WritePrivateProfileString(
  _In_ LPCTSTR lpAppName,
  _In_ LPCTSTR lpKeyName,
  _In_ LPCTSTR lpString,
  _In_ LPCTSTR lpFileName
);

==================================================================
*/

#define MY3VALS "%ld,%ld,%ld"
#define MY4VALS "%ld,%ld,%ld,%ld"

#define	GetStg( a, b )	\
	m_ReadProf( a, b, &szBlk[0], lpb, 256, lpini )

#define GetPrivateProfileString m_ReadProf
#define WritePrivateProfileString m_WriteProf
#define EqualRect m_EqualRect
#define GetWindowPlacement m_GetWindowPlacement

/////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n   // formats and INI read/write


DWORD m_ReadProf(LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR  lpReturnedString,
                                DWORD   nSize,  LPCTSTR lpFileName)
{
    // GetStg( pSect, szShow ); // = "ShowCmd";
    std::ifstream file(lpFileName);
    std::string line, key, val; 
    size_t off;
    bool insect = false;
    while (std::getline(file, line))
    {
        // Process line
        if (line[0] == '[') {
            off = line.find(']');
            if (off > 0) {
                val = line.substr(1,off-1);
                insect = strcmp(lpSecName,val.c_str()) ? false : true;
            }
        } else if ((off = line.find('=')) > 0) {
            if (insect) {
                key = line.substr(0,off);
                if (strcmp(lpKeyName, key.c_str()) == 0) {
                    val = line.substr(off+1);
                    if ((DWORD)val.size() < nSize) {
                        strcpy(lpReturnedString, val.c_str());
                        file.close();
                        return (DWORD) val.size();
                    }
                }
            }
        }
    }
    file.close();
    strcpy(lpReturnedString, lpDefault);
    return 0;
}

BOOL get_to_section(std::ifstream &file, LPCTSTR lpSecName, vSTG &vs)
{
    std::string line, key, val; 
    size_t off;
    bool insect = false;
    while (std::getline(file, line))
    {
        vs.push_back(line);     // Process line
        if (line[0] == '[') {
            off = line.find(']');
            if (off > 0) {
                val = line.substr(1,off-1);
                insect = strcmp(lpSecName,val.c_str()) ? false : true;
                if (insect)
                    return TRUE;
            }
        }
    }
    return FALSE;
}

int get_to_key(std::ifstream &file, LPCTSTR lpKeyName, vSTG &vs, size_t *poff)
{
    std::string line, key; 
    size_t off;
    bool insect = false;
    off = (size_t)file.tellg();
    while (std::getline(file, line))
    {
        if (line[0] == '[') {
            if (poff)
                *poff = off;
            return 2;
        }
        if ((off = line.find('=')) > 0) {
            key = line.substr(0,off);
            if (strcmp(lpKeyName, key.c_str()) == 0) {
                if (poff)
                    *poff = off;
                return 1;
            }
        }
        off = (size_t)file.tellg();
        vs.push_back(line);
    }
    return 0;
}

const char *get_file_name( const char *file )
{
    const char *name = file;
    size_t ii, len = strlen(file);
    int c;
    for (ii = 0; ii < len; ii++) {
        c = file[ii];
        if (( c == '\\' ) || ( c == '/' )) {
            name = &file[ii+1];
        }
    }
    return name;
}

BOOL m_WriteProf(LPCTSTR lpSecName,LPCTSTR lpKeyName,LPCTSTR lpString,LPCTSTR lpFileName)
{
    std::ifstream file(lpFileName);
    vSTG vs;
    std::string line, ofile;
    std::ofstream out;
    int res;
    size_t off, ii;
    if (get_to_section(file, lpSecName, vs)) {
        if (lpKeyName && *lpKeyName) {
            res = get_to_key(file, lpKeyName, vs, &off);
            if (res) {
                if (res == 2) {
                    // reached next section, without finding the key! backup one
                    file.seekg(off);    // back to BEFORE this section
                } else {
                    // could check if there is a change...
                }
            }
            // insert new line
            line  = lpKeyName;
            line += "=";
            line += lpString;
            vs.push_back(line);
            while (std::getline(file, line)) {
                vs.push_back(line);
            }
        } else {
            // delete this SECTION
            while (std::getline(file, line)) {
                if (line[0] == '[') {
                    // got next section
                    vs.push_back(line);
                    break;
                }
            }
            while (std::getline(file, line)) {
                vs.push_back(line);
            }
        }
        file.close();

        // write NEW file
        ofile = lpFileName;
        ofile += ".old";
        out.open(ofile.c_str(), ios_base::out);
        off = vs.size();
        for (ii = 0; ii < off; ii++) {
            line = vs[ii];
             out << line << std::endl;
        }
        out.close();
        ///////////////////////////////////////////////////////////////
        // if successful write operation, delete previous, and ren .old
        ///////////////////////////////////////////////////////////////
        int rc;
        rc = unlink(lpFileName);
        if(rc) {
            SPRTF("Error: m_WriteProf: Failed to 'delete' file '%s' (%d)\n", lpFileName, errno);
        }

        rc = std::rename(ofile.c_str(), lpFileName); 
        if(rc) {
            const char *nm = get_file_name(lpFileName);
            rc = std::rename(ofile.c_str(), nm); 
            if (rc) {
                SPRTF("Error: m_WriteProf: Failed to 'rename' file '%s' to '%s'! or %s! (%d)\n", ofile.c_str(), lpFileName, nm, errno);
            }
        }
    } else {
         file.close();
         // Append new section, and write value
         if (lpString) {
             out.open(lpFileName, std::ios::app);
             line = "[";
             line += lpSecName;
             line += "]";
             out << line << std::endl;
             line  = lpKeyName;
             line += "=";
             line += lpString;
             out << line << std::endl;
             out.close();
         }
    }
    return TRUE;
}

BOOL m_EqualRect( PRECT pr1, PRECT pr2 )
{
    if ((pr1->bottom == pr2->bottom) &&
        (pr1->left   == pr2->left  ) &&
        (pr1->right  == pr2->right ) &&
        (pr1->top    == pr2->top   )) {
        return TRUE;
    }
    return FALSE;
}

BOOL m_GetWindowPlacement(HWND hwnd, PWINDOWPLACEMENT pwp)
{
    return FALSE;
}


static char m_szTmpBuf[1024];
static char g_szDefIni[] = "Sudoku.ini";
char g_szIni[264] = { "\0" };
char szBlk[] = "\0";
// Sections
char szVer[] = "Version";
char szWin[] = "Window";
char szFil[] = "FileList";
char szOpt[] = "Options";
char szClr[] = "Colors";

char szDt[] = "Version-Date";
char szCVer[] = SUDO_VERS_DATE;
int bChgAll = 0;

// section [Window]
// char szWin[] = "Window";
char szShow[] = "ShowCmd";
char szMaxX[] = "MaxX";
char szMaxY[] = "MaxY";
char szMinX[] = "MinX";
char szMinY[] = "MinY";
char szLeft[] = "NormLeft";
char szTop[]  = "NormTop";
char szRite[] = "NormRight";
char szBot[]  = "NormBottom";

char szGOut[] = "OutSaved";
BOOL  gbGotWP = FALSE;
WINDOWPLACEMENT   g_sWP;
BOOL  bChgWP = FALSE;

char szWin2[] = "Window2";
char szGOut2[] = "OutSaved2";
BOOL  gbGotWP2 = FALSE;
WINDOWPLACEMENT   g_sWP2;
BOOL  bChgWP2 = FALSE;

// section char szOpt[] = "Options";
char szAuto[] = "ShowHints";
// BOOL g_bShowHints = TRUE;
// BOOL gChgHint = FALSE;

char szALoad[] = "AutoLoad";
BOOL g_bAutoLoad = TRUE;
BOOL g_bALChg = FALSE;

char szATab[] = "AutoTab";
BOOL g_bAutoTab = TRUE;  // on entry of a number, skip to next cell
BOOL g_bChgAT = FALSE;

char szWarnOBV[] = "Warn_On_Bad_Value";
BOOL g_bWarnOBV = TRUE; // Entered a NON-VALID number from keyboard
BOOL g_bChgOBV = FALSE;

char szShwSel[] = "Show_Selection"; // When a cell with val selected, color all empty with this canadidate
BOOL g_bShwSel = FALSE;
BOOL g_bChgSS = FALSE;

char szShwMsgD[] = "Show_Message_Dialog";
BOOL g_bShwMsgD = TRUE;
BOOL gChgSMsgD = FALSE;

vSTG load_files;
char szFile[] = "File%d";
BOOL gChgFiles = FALSE;

char szSpots[] = "DefaultSpots";
int g_iMinUsr = 17; // was 30; but see : http://en.wikipedia.org/wiki/Suduko
BOOL gChgSpot = FALSE;

// *** Manifest Constant ***
int g_iMinGiven = 17; // was 30; but see : http://en.wikipedia.org/wiki/Suduko - CONSTANT

char szTmpSvOp[] = "TempSaveOptionBits";
DWORD g_dwSvOptBits = sff_TEMP_FILE;
BOOL gChgTSO = FALSE;

char szTmpSvFl[] = "TempSaveFile";
char g_szSvOptFl[_MAX_PATH] = {"\0"};
BOOL gChgTSF = FALSE;

char szPntChN[] = "Paint_CHain_Number";
// BOOL g_bPntChN = TRUE;  // paint 'chain_number' instead of sequence number
BOOL g_bChgPChN = FALSE;

char szASV[] = "All_Candidates";
// BOOL g_bAllSL = TRUE;
BOOL gChgASV = FALSE;

char szCLST[] = "Candidate_List";
char g_szCandList[_MAX_PATH] = { "1" };
BOOL gChgCLST = FALSE;

char szIWL[] = "Include_Weak_Links";
BOOL g_bIWL = TRUE;
BOOL gChgIWL = FALSE;

char szIBL[] = "Include_Box_Links";
BOOL g_bIBL = TRUE;
BOOL gChgIBL = FALSE;

char szIASW[] = "Alternating_Strong_Weak";
BOOL g_bASW = TRUE;
BOOL gChgASW = FALSE;

char szExChn[] = "Extend_Link_Chains";
BOOL g_bExtChn = TRUE;
BOOL gChgELC = FALSE;

char szASD[] = "Auto_Solve_Delay_Seconds_as_float";
// extern double g_AutoDelay;
BOOL gChgASD = FALSE;

char szTAICWAE[] = "Treat_AIC_Warn_As_Error";
extern int g_bTAICWAE; // was treat_aic_warn_as_error = 1;
BOOL gChgAICWAE = FALSE;

// Colors SECTION
// char szClr[] = "Colors"; = it_Color
// Text Colors By Type
char szOrgVal[] = "Original_Values";
//extern COLORREF crOrgVal;
BOOL gChgOVal = FALSE;
char szNewVal[] = "New_Values";
//extern COLORREF crNewVal;
BOOL gChgNVal = FALSE;
char szBadVal[] = "Bad_Values";
//extern COLORREF crBadVal;
BOOL gChgBVal = FALSE;
char szSelVal[] = "Selected_Value";
//extern COLORREF crSelVal;
BOOL gChgSVal = FALSE;

char szElimBk[] = "Eliminate_Candidate_Bk";
// COLORREF crElimBk;
BOOL bChgElimBk = FALSE;
char szElimFg[] = "Eliminate_Candidate_Fg";
// COLORREF crElimFg;
BOOL bChgElimFg = FALSE;
// extern , crWLnk, crOddBox, crEvnBox, crElimBk, crElimFg;
char szSLnk[] = "Strong_Link_Lines";
// COLORREF crSLnk;
BOOL bChgSlnk = FALSE;
char szWLnk[] = "Weak_Links_Lines";
// COLORREF crWLnk;
BOOL bChgWlnk = FALSE;
char szOn[] = "On_Color_Bk";
// COLORREF crOn;
BOOL bChgOn = FALSE;
char szOff[] = "Off_Color_Bk";
// COLORREF crOff;
BOOL bChgOff = FALSE;

char szClr1[] = "Cell_Color_1";
// COLORREF crClr1;
BOOL bChgClr1 = FALSE;
char szClr2[] = "Cell_Color_2";
// COLORREF crClr2;
BOOL bChgClr2 = FALSE;
char szClr3[] = "Cell_Color_3";
// COLORREF crClr3;
BOOL bChgClr3 = FALSE;
char szClr4[] = "Cell_Color_4";
// COLORREF crClr4;
BOOL bChgClr4 = FALSE;


char szShwSLnks[] = "Show_Strong_Links";
BOOL g_bShwStLnks = FALSE;
BOOL gChgStLnks = FALSE;

char szMrkNM[] = "mark_non_color_members";
BOOL g_bMrkNM = TRUE;   // also mark NON-members if ONE setval used
BOOL gChgMNM = FALSE;

char szLIndOpn[] = "LastOpnFilterIndex";
int g_iLIndOpn = 1; // 0=custom-filter, else 1, 2, ... for each pair in filter
BOOL gChgLIO = FALSE;

char szLPath[] = "LastPath";
char g_szLastPath[264] = {0}; // LAST PATH
BOOL gChgLPath = FALSE;

char szDbg[] = "Debug";
char szStr[] = "Strategies";

char *Get_INI_File( int off )
{
    int cnt = 0;
    vSTG *vsp = &load_files;
    vSTGi ii;
    for (ii = vsp->begin() ; ii != vsp->end(); ii++ ) {
        if (cnt == off) {
            string s = *ii;
            char *cp = GetNxtBuf();
            strcpy(cp,s.c_str());
            return cp;
        }
        cnt++;
    }
    return 0;
}

char *Get_First_INI_File() { return Get_INI_File(0); }


DWORD Add_INI_Files_to_Menu()
{
    DWORD res = SetFileMRU( g_hWnd, load_files );
    return res;
}

void Add_to_INI_File_List( char * pfile )
{
    vSTG *vsp = &load_files;
    string s = pfile;
    string s2;
    vSTGi ii;
    DWORD count = 0;    // have no NEW list
    bool isfirst = false;
    for (ii = vsp->begin(); ii != vsp->end(); ii++) {
        s2 = *ii;
        if (STRCMPFIL(s.c_str(),s2.c_str()) == 0 ) {
            isfirst = true;
        }
        break; // only testing the FIRST entry
    }
    if (isfirst) {
        return;
    }
    // last file is ALWAYS put to TOP of list
    vSTG nlist;
    nlist.push_back(s);
    count = 1;    // have ONE in list
    for (ii = vsp->begin(); ii != vsp->end(); ii++) {
        s2 = *ii;
        if (STRCMPFIL(s.c_str(),s2.c_str()) == 0 )
            continue; // was already in list
        nlist.push_back(s2);
        count++;
        if (count >= gdwMaxFiles)
            break;
    }
    gChgFiles = TRUE;
    vsp->clear();
    for (ii = nlist.begin(); ii != nlist.end(); ii++)
    {
        s = *ii;
        vsp->push_back(s);
    }
    nlist.clear();
    Add_INI_Files_to_Menu();    // reset the MENU LIST MRU
}

void Remove_INI_File( char *pf )
{
    vSTG *vsp = &load_files;
    string s;
    vSTGi ii;
    for (ii = vsp->begin(); ii != vsp->end(); ii++) {
        s = *ii;
        if (strcmp(pf,s.c_str()) == 0) {
            vsp->erase(ii);
        }
    }
    Add_INI_Files_to_Menu();    // reset the MENU LIST MRU
}

//      pSect = plst->i_Sect;
//      pItem = plst->i_Item;
//      pDef  = plst->i_Deft;
//      bchg  = *(plst->i_Chg);
//	  i_Sect,  i_Item, i_Type,  char *	i_Deft, int *	   i_Chg, i_Void, gs_double
INILST sIniLst[] = {
   { szVer, szDt,   it_Version,          szCVer,        &bChgAll, 0, 0 },  // CHANGE ALL!
   // section "Window"
   { szWin, szGOut,   it_WinSize,   (char *)&g_sWP,        &bChgWP, (PVOID)&gbGotWP, 0 },
   { szWin2, szGOut2, it_WinSize,   (char *)&g_sWP2,       &bChgWP2, (PVOID)&gbGotWP2, 0 },
   // section "Options";
   { szOpt, szAuto, it_Bool,      (char *)&g_bShowHints, &gChgHint, 0,  0 },
   { szOpt, szALoad, it_Bool,     (char *)&g_bAutoLoad,  &g_bALChg, 0,  0 },
   { szOpt, szSpots, it_Int,      (char *)&g_iMinUsr,    &gChgSpot, 0,  0 },
   { szOpt, szTmpSvOp, it_Int,    (char *)&g_dwSvOptBits,&gChgTSO,  0,  0 },
   { szOpt, szTmpSvFl, it_String,         g_szSvOptFl,   &gChgTSF,  0,  0 },
   { szOpt, szShwSel,  it_Bool,   (char *)&g_bShwSel,    &g_bChgSS, 0,  0 },
   { szOpt, szPntChN,  it_Bool,   (char *)&g_bPntChN,    &g_bChgPChN, 0, 0 },
   // not sure I like this yet - not very  useful - very messy when LOTS of links - DEF = OFF
//   { szOpt, szShwSLnks,it_Bool,   (char *)&g_bShwStLnks, &gChgStLnks, 0, 0 },
   { szOpt, szShwMsgD, it_Bool,  (char *)&g_bShwMsgD,    &gChgSMsgD, 0, 0 },
   { szOpt, szASV,     it_Bool,  (char *)&g_bAllSL,      &gChgASV,   0, 0 },
   { szOpt, szCLST,   it_String,         g_szCandList,   &gChgCLST, 0, 0 },
   { szOpt, szIWL,     it_Bool,  (char *)&g_bIWL,        &gChgIWL,  0, 0 },
   { szOpt, szIBL,     it_Bool,  (char *)&g_bIBL,        &gChgIBL,  0, 0 },
   { szOpt, szIASW,    it_Bool,  (char *)&g_bASW,        &gChgASW,  0, 0 },
   { szOpt, szExChn,   it_Bool,  (char *)&g_bExtChn,     &gChgELC,  0, 0 },
   { szOpt, szLIndOpn, it_Int,   (char *)&g_iLIndOpn,    &gChgLIO,  0, 0 },
   { szOpt, szLPath,   it_String,        g_szLastPath,   &gChgLPath, 0, 0 },
   { szOpt, szASD,     it_Double,(char *)&g_AutoDelay,   &gChgASD,   0, 0 },
   { szOpt, szTAICWAE, it_Bool,   (char*)&g_bTAICWAE,    &gChgAICWAE, 0, 0 },

   // Colors SECTION
   { szClr, szOrgVal,  it_Color, (char *)&crOrgVal,      &gChgOVal, 0,  0 },
   { szClr, szNewVal,  it_Color, (char *)&crNewVal,      &gChgNVal, 0,  0 },
   { szClr, szBadVal,  it_Color, (char *)&crBadVal,      &gChgBVal, 0,  0 },
   { szClr, szSelVal,  it_Color, (char *)&crSelVal,      &gChgSVal, 0,  0 },
   { szClr, szElimBk,  it_Color, (char *)&crElimBk,      &bChgElimBk, 0, 0 },
   { szClr, szElimFg,  it_Color, (char *)&crElimFg,      &bChgElimFg, 0, 0 },
   { szClr, szSLnk,    it_Color, (char *)&crSLnk,        &bChgSlnk,   0, 0 },
   { szClr, szWLnk,    it_Color, (char *)&crWLnk,        &bChgWlnk,   0, 0 },
   { szClr, szOn,      it_Color, (char *)&crOn,          &bChgOn,     0, 0 },
   { szClr, szOff,     it_Color, (char *)&crOff,         &bChgOff,    0, 0 },
   { szClr, szClr1,    it_Color, (char *)&crClr1,        &bChgClr1,   0, 0 },
   { szClr, szClr2,    it_Color, (char *)&crClr2,        &bChgClr2,   0, 0 },
   { szClr, szClr3,    it_Color, (char *)&crClr3,        &bChgClr3,   0, 0 },
   { szClr, szClr4,    it_Color, (char *)&crClr4,        &bChgClr4,   0, 0 },

   // section "FileList";
   { szFil, szFile, it_Files,     (char *)&load_files,   &gChgFiles, 0, 0 },

   // section Debug - CARE this must match the items in Sudo_Debug.cxx - DbgList structure
   { szDbg, (char *)DbgList[0].pcaption, it_Bool, (char *)DbgList[0].pdebug, DbgList[0].pchg, 0, 0 },
   { szDbg, (char *)DbgList[1].pcaption, it_Bool, (char *)DbgList[1].pdebug, DbgList[1].pchg, 0, 0 },
   { szDbg, (char *)DbgList[2].pcaption, it_Bool, (char *)DbgList[2].pdebug, DbgList[2].pchg, 0, 0 },
   { szDbg, (char *)DbgList[3].pcaption, it_Bool, (char *)DbgList[3].pdebug, DbgList[3].pchg, 0, 0 },
   { szDbg, (char *)DbgList[4].pcaption, it_Bool, (char *)DbgList[4].pdebug, DbgList[4].pchg, 0, 0 },
   { szDbg, (char *)DbgList[5].pcaption, it_Bool, (char *)DbgList[5].pdebug, DbgList[5].pchg, 0, 0 },
   { szDbg, (char *)DbgList[6].pcaption, it_Bool, (char *)DbgList[6].pdebug, DbgList[6].pchg, 0, 0 },
   { szDbg, (char *)DbgList[7].pcaption, it_Bool, (char *)DbgList[7].pdebug, DbgList[7].pchg, 0, 0 },
   { szDbg, (char *)DbgList[8].pcaption, it_Bool, (char *)DbgList[8].pdebug, DbgList[8].pchg, 0, 0 },
   { szDbg, (char *)DbgList[9].pcaption, it_Bool, (char *)DbgList[9].pdebug, DbgList[9].pchg, 0, 0 },
   { szDbg, (char *)DbgList[10].pcaption, it_Bool, (char *)DbgList[10].pdebug, DbgList[10].pchg, 0, 0 },
   { szDbg, (char *)DbgList[11].pcaption, it_Bool, (char *)DbgList[11].pdebug, DbgList[11].pchg, 0, 0 },
   { szDbg, (char *)DbgList[12].pcaption, it_Bool, (char *)DbgList[12].pdebug, DbgList[12].pchg, 0, 0 },
   { szDbg, (char *)DbgList[13].pcaption, it_Bool, (char *)DbgList[13].pdebug, DbgList[13].pchg, 0, 0 },
   { szDbg, (char *)DbgList[14].pcaption, it_Bool, (char *)DbgList[14].pdebug, DbgList[14].pchg, 0, 0 },
   { szDbg, (char *)DbgList[15].pcaption, it_Bool, (char *)DbgList[15].pdebug, DbgList[15].pchg, 0, 0 },
   { szDbg, (char *)DbgList[16].pcaption, it_Bool, (char *)DbgList[16].pdebug, DbgList[16].pchg, 0, 0 },
   { szDbg, (char *)DbgList[17].pcaption, it_Bool, (char *)DbgList[17].pdebug, DbgList[17].pchg, 0, 0 },
   { szDbg, (char *)DbgList[18].pcaption, it_Bool, (char *)DbgList[18].pdebug, DbgList[18].pchg, 0, 0 },
   { szDbg, (char *)DbgList[19].pcaption, it_Bool, (char *)DbgList[19].pdebug, DbgList[19].pchg, 0, 0 },
   { szDbg, (char *)DbgList[20].pcaption, it_Bool, (char *)DbgList[20].pdebug, DbgList[20].pchg, 0, 0 },

   // section Strategies - CARE this must match the items in Sudo_Debug.cxx - DbgList structure
   { szStr, (char *)DbgList[0].pcaption, it_Bool, (char *)DbgList[0].penab, DbgList[0].pchgd, 0, 0 },
   { szStr, (char *)DbgList[1].pcaption, it_Bool, (char *)DbgList[1].penab, DbgList[1].pchgd, 0, 0 },
   { szStr, (char *)DbgList[2].pcaption, it_Bool, (char *)DbgList[2].penab, DbgList[2].pchgd, 0, 0 },
   { szStr, (char *)DbgList[3].pcaption, it_Bool, (char *)DbgList[3].penab, DbgList[3].pchgd, 0, 0 },
   { szStr, (char *)DbgList[4].pcaption, it_Bool, (char *)DbgList[4].penab, DbgList[4].pchgd, 0, 0 },
   { szStr, (char *)DbgList[5].pcaption, it_Bool, (char *)DbgList[5].penab, DbgList[5].pchgd, 0, 0 },
   { szStr, (char *)DbgList[6].pcaption, it_Bool, (char *)DbgList[6].penab, DbgList[6].pchgd, 0, 0 },
   { szStr, (char *)DbgList[7].pcaption, it_Bool, (char *)DbgList[7].penab, DbgList[7].pchgd, 0, 0 },
   { szStr, (char *)DbgList[8].pcaption, it_Bool, (char *)DbgList[8].penab, DbgList[8].pchgd, 0, 0 },
   { szStr, (char *)DbgList[9].pcaption, it_Bool, (char *)DbgList[9].penab, DbgList[9].pchgd, 0, 0 },
   { szStr, (char *)DbgList[10].pcaption, it_Bool, (char *)DbgList[10].penab, DbgList[10].pchgd, 0, 0 },
   { szStr, (char *)DbgList[11].pcaption, it_Bool, (char *)DbgList[11].penab, DbgList[11].pchgd, 0, 0 },
   //{ szStr, (char *)DbgList[12].pcaption, it_Bool, (char *)DbgList[12].penab, DbgList[12].pchgd, 0, 0 },
   //{ szStr, (char *)DbgList[13].pcaption, it_Bool, (char *)DbgList[13].penab, DbgList[13].pchgd, 0, 0 },
   { szStr, (char *)DbgList[14].pcaption, it_Bool, (char *)DbgList[14].penab, DbgList[14].pchgd, 0, 0 },
   //{ szStr, (char *)DbgList[15].pcaption, it_Bool, (char *)DbgList[15].penab, DbgList[15].pchgd, 0, 0 },
   { szStr, (char *)DbgList[16].pcaption, it_Bool, (char *)DbgList[16].penab, DbgList[16].pchgd, 0, 0 },
   //{ szStr, (char *)DbgList[17].pcaption, it_Bool, (char *)DbgList[17].penab, DbgList[17].pchgd, 0, 0 },
   { szStr, (char *)DbgList[18].pcaption, it_Bool, (char *)DbgList[18].penab, DbgList[18].pchgd, 0, 0 },
   { szStr, (char *)DbgList[19].pcaption, it_Bool, (char *)DbgList[19].penab, DbgList[19].pchgd, 0, 0 },
   { szStr, (char *)DbgList[20].pcaption, it_Bool, (char *)DbgList[20].penab, DbgList[20].pchgd, 0, 0 },

   // last entry
   { 0,  0,  it_None, 0, 0, 0, 0 }
};

PINILST Find_an_Options(const char* popt)
{
    PINILST plist = &sIniLst[0];
    while (plist->i_Sect) {
        if (strcmp(plist->i_Sect, szOpt) == 0) {
            if (strcmp(plist->i_Item, popt) == 0)
                return plist;
        }
        plist++; // bump list pointer
    }
    return NULL;
}

//typedef struct tagDBGLIST  {
//    const char *pcaption;
//    UINT        conrol;
//    int *       pdebug;
//    PBOOL       pchg;
//} DBGLIST, *PDBGLIST;
//DBGLIST DbgList[] = {

int Chk4Debug( char * lpd )
{
   int     bret = FALSE;
   char * ptmp = &m_szTmpBuf[0];
   char *   p;
   int  dwi;

   strcpy(ptmp, lpd);
   dwi = (int)strlen(ptmp);
   while(dwi--)
   {
      if(ptmp[dwi] == '\\')
      {
         ptmp[dwi] = 0;
         p = strrchr(ptmp, '\\');
         if(p)
         {
            p++;
            if( STRCMPFIL(p, "DEBUG") == 0 )
            {
               *p = 0;
               strcpy(lpd,ptmp);    // use this
               bret = TRUE;
               break;
            }
         }
      }
   }
   return bret;
}

#ifndef WIN32
/* SYNTAX:

DWORD WINAPI GetModuleFileName(
  _In_opt_ HMODULE hModule,
  _Out_    LPTSTR  lpFilename,
  _In_     DWORD   nSize
);

 */

DWORD GetModuleFileName(HMODULE hModule, LPTSTR  lpFilename, DWORD   nSize)
{
    char id[256];
    memset(lpFilename,0,nSize);
    sprintf(id,"/proc/%d/exe", getpid());
    int ch = readlink(id,lpFilename,nSize);
    if ((ch != -1) && lpFilename[0]) {
        lpFilename[ch] = 0;
        std::string path = lpFilename;
        std::string::size_type t = path.find_last_of("/");
        if (t > 0) {
            path = path.substr(0,t+1);
            strcpy(lpFilename,path.c_str());
        }
    }
    return (DWORD)strlen(lpFilename);
}

#endif

void  GetModulePath( char * lpb )
{
   char *   p;
   GetModuleFileName( NULL, lpb, 256 );
   p = strrchr( lpb, '\\' );
   if( p )
      p++;
   else
      p = lpb;
   *p = 0;
#ifndef  NDEBUG
   Chk4Debug( lpb );
#endif   // !NDEBUG

}

bool Get_LocalData_Path( char *path )
{
    bool isok = false;
#ifdef WIN32 // use getenv("LOCALAPPDATA")\sudoku
    char *cp = getenv("LOCALAPPDATA");
    if (cp) {
        std::string s = cp;
        s += "\\sudoku";
        if (is_file_or_directory((char *)s.c_str()) != 2) {
            _mkdir(s.c_str()); // create a directory
        }
        if (is_file_or_directory((char *)s.c_str()) == 2) {
            strcpy(path,s.c_str());
            strcat(path,"\\");
            isok = true;
        }
    }
#else        // !#ifdef WIN32 - Try for $HOME/.config/sudoku 
    char *cp = getenv("HOME");
    if (cp) {
        std::string s = cp;
        s += "/.config";
        if (is_file_or_directory((char *)s.c_str()) != 2) {
            mkdir(s.c_str(), 0700); // create a directory
        }
        if (is_file_or_directory((char *)s.c_str()) == 2) {
            s += "/sudoku";
            if (is_file_or_directory((char *)s.c_str()) != 2) {
                mkdir(s.c_str(), 0700); // create a directory
            }
            if (is_file_or_directory((char *)s.c_str()) == 2) {
                strcpy(path,s.c_str());
                strcat(path,"/");
                isok = true;
            }
        }
    }
#endif      // #ifdef WIN32 y/n - OS path for INI file
   return isok;
}

char * GetINIFile(void) 
{
    char * lpini = g_szIni;
    if( *lpini == 0 ) {
        bool isok = Get_LocalData_Path(lpini);
        if (!isok)
            GetModulePath( lpini );    // does   GetModuleFileName( NULL, lpini, 256 );
        strcat(lpini, g_szDefIni);
   }
   return lpini;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsYes
// Return type: BOOL 
// Argument   : LPTSTR lpb
// Description: Return TRUE if the string given is "Yes" OR "On"
///////////////////////////////////////////////////////////////////////////////
int  IsYes( char * lpb )
{
   int  bRet = FALSE;
   if( ( STRCMPI(lpb, "yes") == 0 ) ||
       ( STRCMPI(lpb, "on" ) == 0 ) )
       bRet = TRUE;
   return bRet;
}

int  IsNo( char * lpb )
{
   int  bRet = FALSE;
   if( ( STRCMPI(lpb, "no" ) == 0 ) ||
       ( STRCMPI(lpb, "off") == 0 ) )
       bRet = TRUE;
   return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsRectOk
// Return type: BOOL 
// Argument   : PRECT pr
// Description: A rough verification that a RECTANGLE 'looks' OK
//              NOTE: Thre must be SOME value, and they must all
// be positive (or zero, but not all).
///////////////////////////////////////////////////////////////////////////////
BOOL  IsRectOk( PRECT pr )
{
   BOOL  bRet = FALSE;
   if( pr->left || pr->top || pr->right || pr->bottom )
   {
      // good start - some value is NOT zero
      // here I am ONLY accepting POSITIVE values
      if( ( pr->left >= 0 ) &&
          ( pr->top >= 0 ) &&
          ( pr->right >= 0 ) &&
          ( pr->bottom >= 0 ) )
      {
         bRet = TRUE;
      }
      else if(( pr->right  > 0 ) &&
              ( pr->bottom > 0 ) )
      {
         // this is a POSSIBLE candidate,
         // but limit neg x,y to say 5 pixels
         if(( pr->left > -5 ) &&
            ( pr->top  > -5 ) )
         {
            bRet = TRUE;
         }
      }
   }
   return bRet;
}

BOOL  ValidShowCmd( UINT ui )
{
   BOOL  bRet = FALSE;
   if( ( ui == SW_HIDE ) ||   //Hides the window and activates another window. 
       ( ui == SW_MAXIMIZE ) ||  //Maximizes the specified window. 
       ( ui == SW_MINIMIZE ) ||  //Minimizes the specified window and activates the next top-level window in the Z order. 
       ( ui == SW_RESTORE ) ||   //Activates and displays the window. If the window is minimized or maximized, the system restores it to its original size and position. An application should specify this flag when restoring a minimized window. 
       ( ui == SW_SHOW ) || //Activates the window and displays it in its current size and position.  
       ( ui == SW_SHOWMAXIMIZED ) || //Activates the window and displays it as a maximized window. 
       ( ui == SW_SHOWMINIMIZED ) || //Activates the window and displays it as a minimized window. 
       ( ui == SW_SHOWMINNOACTIVE ) ||  //Displays the window as a minimized window. 
       ( ui == SW_SHOWNA ) || //Displays the window in its current size and position. 
       ( ui == SW_SHOWNOACTIVATE ) ||  //Displays a window in its most recent size and position. 
       ( ui == SW_SHOWNORMAL ) )
       bRet = TRUE;
   return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GotWP
// Return type: BOOL 
// Arguments  : LPTSTR pSect
//            : LPTSTR pDef
//            : LPTSTR lpb
//            : LPTSTR lpini
// Description: Read in a special BLOCK of window placement items from
//              the INI file. This is in its own [section].
///////////////////////////////////////////////////////////////////////////////
BOOL  GotWP( PTSTR pSect, PTSTR pDef, PTSTR lpb, PTSTR lpini )
{
   BOOL  bRet = FALSE;
   WINDOWPLACEMENT   wp;
   WINDOWPLACEMENT * pwp = (WINDOWPLACEMENT *)pDef;
   if( !pwp )
      return FALSE;

   *lpb = 0;
   GetStg( pSect, szShow ); // = "ShowCmd";
   if( *lpb == 0 )
      return FALSE;
   wp.showCmd = m_atoi(lpb);
   if( !ValidShowCmd( wp.showCmd ) )
      return FALSE;

   *lpb = 0;
   GetStg( pSect, szMaxX );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMaxPosition.x = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szMaxY );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMaxPosition.y = m_atoi(lpb);

   *lpb = 0;
   GetStg( pSect, szMinX );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMinPosition.x = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szMinY );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMinPosition.y = m_atoi(lpb);

   *lpb = 0;
   GetStg( pSect, szLeft );   // = "NormLeft";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.left = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szTop ); // = "NormTop";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.top = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szRite );   // = "NormRight";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.right = m_atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szBot ); //  = "NormBottom";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.bottom = m_atoi(lpb);

   wp.flags = 0;
   wp.length = sizeof(WINDOWPLACEMENT);

   memcpy( pwp, &wp, sizeof(WINDOWPLACEMENT) );
   bRet = TRUE;
   return bRet;
}

BOOL  ChangedWP( WINDOWPLACEMENT * pw1, WINDOWPLACEMENT * pw2 )
{
   BOOL  bChg = FALSE;
   if( ( pw1->length != sizeof(WINDOWPLACEMENT) ) ||
       ( pw2->length != sizeof(WINDOWPLACEMENT) ) ||
       ( pw1->showCmd != pw2->showCmd ) ||
       ( pw1->ptMaxPosition.x != pw2->ptMaxPosition.x ) ||
       ( pw1->ptMaxPosition.y != pw2->ptMaxPosition.y ) ||
       ( !EqualRect( &pw1->rcNormalPosition, &pw2->rcNormalPosition ) ) )
   {
      bChg = TRUE;
   }
   return bChg;
}

BOOL Update_PWP( HWND hwnd, WINDOWPLACEMENT *pwp )
{
    BOOL bRet = FALSE;
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    if( GetWindowPlacement(hwnd,&wp) )
    {
        if( ChangedWP( &wp, pwp ) ) {
            memcpy( pwp, &wp, sizeof(WINDOWPLACEMENT) );
            pwp->length = sizeof(WINDOWPLACEMENT);
            bRet = TRUE;
        }
    }
    return bRet;
}

VOID UpdateWP( HWND hwnd )
{
    WINDOWPLACEMENT *pwp = &g_sWP;
    if (Update_PWP(hwnd,pwp))
        bChgWP = TRUE;
}

VOID UpdateWP2( HWND hwnd )
{
    WINDOWPLACEMENT *pwp = &g_sWP2;
    if (Update_PWP(hwnd,pwp))
        bChgWP2 = TRUE;
}

void Post_INI_Read() 
{
    // a chance to FIX anything missing
    // char szLPath[] = "LastPath";
    // char g_szLastPath[264] = {0}; // LAST PATH
    // BOOL gChgLPath = FALSE;
    if (g_szLastPath[0] == 0) {
        if (getcwd( g_szLastPath, 256 )) 
            gChgLPath = TRUE;
    }

}

void ReadINI( void )
{
   PINILST  plst = &sIniLst[0];
   char *   lpb = &m_szTmpBuf[0];
   unsigned int dwt;
   char *   pSect, * pItem, * pDef;
   int *    pb, * pbd;
   int *     pi;
   int      i, icnt;
   double * pdbl;
   double   dbl;
   PRECT    pr, pr1;

   char * lpini = GetINIFile();
   icnt = 0;
    // if (add_debug_ini) 20180121 - always SHOW INI being read
    sprtf("Reading INI file [%s]\n", lpini);
    while( ( dwt = plst->i_Type ) != it_None )
    {
        pSect = plst->i_Sect;   // pointer to [section] name
        pItem = plst->i_Item;   // pointer to item in section
        pDef  = plst->i_Deft;   // pointer to destination
        pb    = plst->i_Chg;    // pointer to CHANGE flag
        if (dwt == it_Files) {
            vSTG *vsp = (vSTG *)pDef;
            DWORD j;
            string s;
            for( j = 0; j < gdwMaxFiles; j++ ) {
                // Always do FULL LIST, in case of gaps
                sprintf( lpb, pItem, (j+1) );
			    GetStg( pSect, lpb );
                if(( i = strlen( lpb ) ) != 0 ) { // If we GOT a FILE NAME
                    s = lpb;
                    vsp->push_back(s);
                }
            }
            plst++;
            continue;
        }
        GetStg( pSect, pItem );
        if( *lpb ) 
        {
         switch(dwt)
         {
         case it_Version:
            if( strcmp( pDef, lpb ) )
               *pb = TRUE;
            break;
         case it_String:
            if( strcmp( pDef, lpb ) )
               strcpy( pDef, lpb );
            break;
         case it_Int:   // also doubles as a DWORD in WIN32
            pi = (int *)pDef;
            i = (int)atoi(lpb);
            *pi = i;
            break;
         case it_Double:   // floating point number
            pdbl = (double *)pDef;
            dbl = atof(lpb);
            *pdbl = dbl;
            break;
         case it_Bool:
            pbd = (int *)pDef;
            if( IsYes(lpb) )
               *pbd = TRUE;
            else if( IsNo(lpb) )
               *pbd = FALSE;
            else
               *pb = TRUE;
            break;
         case it_WinSize:     // special WINDOWPLACEMENT
            if( ( IsYes(lpb) ) &&
                ( GotWP( pSect, pDef, lpb, lpini ) ) )
            {
               // only if SAVED is yes, AND then success
               pb = (PBOOL) plst->i_Void;
               *pb = TRUE; // set that we have a (valid!) WINDOWPLACEMENT
            }
            else
               *pb = FALSE; // set this only if a windows placment is done (TRUE);
            break;
         case it_Rect:
            pr = (PRECT) &lpb[ strlen(lpb) + 2 ];
            pr->left = 0;
            pr->top = 0;
            pr->right = 0;
            pr->bottom = 0;
            if( ( m_sscanf(lpb, MY4VALS, &pr->left, &pr->top, &pr->right, &pr->bottom ) == 4 ) &&
                ( IsRectOk( pr ) ) )
            {
               pr1 = (PRECT)pDef;
               pr1->left = pr->left;
               pr1->top  = pr->top;
               pr1->right = pr->right;
               pr1->bottom = pr->bottom;
               pb = (PBOOL) plst->i_Void;
               if(pb)
                  *pb = TRUE;
            }
            break;
         case it_Color:
            pr = (PRECT) &lpb[ strlen(lpb) + 2 ];
            pr->left = 0;
            pr->top = 0;
            pr->right = 0;
            pr->bottom = 0;
            if( m_sscanf(lpb, MY3VALS, &pr->left, &pr->top, &pr->right ) == 3 )
            {
               COLORREF * pcr = (COLORREF *)pDef;
               *pcr = RGB( pr->left, pr->top, pr->right );
            }
            break;
         case it_Files:
             break;
         }
      } else {
         *pb = TRUE; // parameter does not EXIST in file
         // mark it as changed
      }
      plst++;
   }
   Post_INI_Read();
}

void trim_trailing_zeros( char * lpb )
{
   int len = strlen(lpb);
   while(len--) {
      if( lpb[len] == '0' )
         lpb[len] = 0;
      else
         break;
      if(len < 2)
         break;
   }
}


#define  WI( a, b )\
   {  sprintf(lpb, "%d", (int)b );\
      WritePrivateProfileString(pSect, a, lpb, lpini ); }

void WriteINI( void )
{
   char *   lpb   = &m_szTmpBuf[0];
   PINILST  plst  = &sIniLst[0];
   int    dwt;
   char * pSect, * pItem, * pDef;
   int     ball = *(plst->i_Chg);
   int     bchg;
   int *   pint;
   int *    pb;
   double * pdbl;
   WINDOWPLACEMENT * pwp;
   PRECT    pr;
   char *  lpini = GetINIFile();
   int chgcount = 0;

   if (add_debug_ini) sprtf("Writting INI file [%s]\n", lpini);
   // Pre_INI_Write();
   if( ball )
   {
      // clear down the current INI file
      while( ( dwt = plst->i_Type ) != it_None )
      {
         pSect = plst->i_Sect;
         WritePrivateProfileString( pSect,		// Section
            NULL,    // Res.Word
            NULL,    // String to write
            lpini );	// File Name
         plst++;
      }
   }
   plst = &sIniLst[0];  // start of LIST

   while( ( dwt = plst->i_Type ) != it_None )
   {
      pSect = plst->i_Sect;
      pItem = plst->i_Item;
      pDef  = plst->i_Deft;
      bchg  = *(plst->i_Chg);
      if( ball || bchg )
      {
         *lpb = 0;
         switch(dwt)
         {
         case it_Version:
            strcpy(lpb, pDef);
            break;
         case it_String:
            strcpy(lpb, pDef);
            break;
         case it_Int:
            pint = (int *)pDef;
            sprintf(lpb, "%d", *pint );
            break;
         case it_Double:   // floating point number
            pdbl = (double *)pDef;
            sprintf(lpb, "%0.12f", *pdbl);
            trim_trailing_zeros(lpb);
            break;
         case it_Bool:
            pb = (int *)pDef;
            if( *pb )
               strcpy(lpb, "Yes");
            else
               strcpy(lpb, "No");
            break;
         case it_WinSize:     // special WINDOWPLACEMENT
            pb = (PBOOL)plst->i_Void;
            pwp = (WINDOWPLACEMENT *)pDef;
            if( ( pwp->length == sizeof(WINDOWPLACEMENT) ) &&
                ( ValidShowCmd( pwp->showCmd ) ) )
            {
               WI( szShow, pwp->showCmd );
               WI( szMaxX, pwp->ptMaxPosition.x );
               WI( szMaxY, pwp->ptMaxPosition.y );
               WI( szMinX, pwp->ptMinPosition.x );
               WI( szMinY, pwp->ptMinPosition.y );
               WI( szLeft, pwp->rcNormalPosition.left );
               WI( szTop,  pwp->rcNormalPosition.top  );
               WI( szRite, pwp->rcNormalPosition.right);
               WI( szBot,  pwp->rcNormalPosition.bottom);
               strcpy(lpb, "Yes");
            }
            else
               strcpy(lpb, "No");
            break;
         case it_Rect:
            pr = (PRECT)pDef;
            sprintf(lpb, MY4VALS, pr->left, pr->top, pr->right, pr->bottom );
            break;
         case it_Color:
            {
               COLORREF * pcr = (COLORREF *)pDef;
               sprintf(lpb, "%d,%d,%d",
                  GetRValue(*pcr), GetGValue(*pcr), GetBValue(*pcr) );
            }
            break;
#ifdef   ADD_LINKED_LIST
         case it_SList:
            {
               PLE      plh = (PLE)pDef;
               PLE      pln;
               dwt = 0;
               Traverse_List( plh, pln )
               {
                  PFILLST pfl = (PFILLST)pln;
                  wsprintf(lpb, pItem, (dwt+1)); // build the reserved word
                  WritePrivateProfileString(
						   pSect,		// Section
						   lpb,		// Res.Word
                     &pfl->file[0],		// String to write
                     lpini );	// File Name
                  dwt++;      // bump to next LIST number

                  if( dwt >= plst->i_Res1 )  // do NOT exceed maximum
                     break;
               }
               *lpb = 0;   // ALL DONE - list is written
            }
            break;
#endif // #ifdef   ADD_LINKED_LIST
         case it_Files:
             vSTG *vsp = (vSTG *)pDef;
             vSTGi ii;
             string s;
             int cnt = 0;
             for (ii = vsp->begin(); ii != vsp->end(); ii++) {
                 s = *ii; 
                 cnt++;
                 sprintf(lpb,pItem,cnt);
                 WritePrivateProfileString(
                     pSect,     // Section
                     lpb,       // Res.word
                     s.c_str(), // name
                     lpini );   // ini file name
                 chgcount++;
             }
             *lpb = 0;  // ALL Done - List written
         }

         *(plst->i_Chg) = FALSE;

         if( *lpb )
         {
            WritePrivateProfileString(
						pSect,		// Section
						pItem,		// Res.Word
						lpb,		// String to write
						lpini );	// File Name
            chgcount++;
         }
      }
      plst++;
   }
   if (add_debug_ini) {
        if (chgcount) {
            sprtf("Written INI file with %d changes.\n", chgcount);
        } else {
            sprtf("No INI file changes...\n");
        }
   }
}

// eof - Sudo_Ini.cxx
