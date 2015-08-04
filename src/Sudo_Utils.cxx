// Sudo_Utils.cxx

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include "Sudoku.hxx"
#ifdef WIN32 // windows - include 'shlwapi.h'
#include <shlwapi.h>
#endif // WIN32 'shlwapi.h'

typedef vector<FILLSERV> vFSS;

vFSS vFillServ;

void Stack_Fill( FILLSERV fs ) { vFillServ.push_back(fs); }
void Do_Fill_Stack(PABOX2 pb) 
{
    FILLSERV fs;
    size_t i;
    for (i = 0; i < vFillServ.size(); i++) {
        fs = vFillServ[i];
        fs(pb);
    }
    vFillServ.clear();
}


int Count_setval_in_Box( PABOX2 pb, int irow, int icol, int setval )
{
    int count = 0;
    int r = (irow / 3) * 3;
    int c = (icol / 3) * 3;
    int rw, cl, row, col, val;
    PSET ps;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            if ( !(ps->val[setval - 1]) ) continue;
            count++;
        }
    }
    return count;
}

int Count_setval_in_Col( PABOX2 pb, int irow, int icol, int setval )
{
    int count = 0;
    int row, col, val;
    PSET ps;
    col = icol;
    for (row = 0; row < 9; row++) {
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        if ( !(ps->val[setval - 1]) ) continue;
        count++;
    }
    return count;
}

int Count_setval_in_Row( PABOX2 pb, int irow, int icol, int setval )
{
    int count = 0;
    int row, col, val;
    PSET ps;
    row = irow;
    for (col = 0; col < 9; col++) {
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        if ( !(ps->val[setval - 1]) ) continue;
        count++;
    }
    return count;
}

bool prc_in_chain(PROWCOL prc, vRC *pchain)
{
    size_t max = pchain->size();
    size_t ii;
    PROWCOL prc2;
    for (ii = 0; ii < max; ii++) {
        prc2 = &pchain->at(ii);
        if (SAMERCCELL(prc,prc2))
            return true;
    }
    return false;
}


bool Row_in_RC_Vector( vRC & vrc, int row, int col )
{
    ROWCOL rc;
    vRCi ii = vrc.begin();
    for ( ; ii != vrc.end(); ii++) {
        rc = *ii;
        if (rc.row == row)
            return true;
    }
    return false;
}

bool Col_in_RC_Vector( vRC & vrc, int row, int col )
{
    ROWCOL rc;
    vRCi ii = vrc.begin();
    for ( ; ii != vrc.end(); ii++) {
        rc = *ii;
        if (rc.col == col)
            return true;
    }
    return false;
}


#ifdef WIN32 // windows - get DLL version
///////////////////////////////////////////////////////////////////////
#define PACKVERSION(major,minor) MAKELONG(minor,major)

DWORD GetVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;
    // For security purposes, LoadLibrary should be provided with a fully qualified 
    // path to the DLL. The lpszDllName variable should be tested to ensure that it 
    // is a fully qualified path before it is used. 
    hinstDll = LoadLibrary(lpszDllName);
    if (hinstDll )
    {
        DLLGETVERSIONPROC pDllGetVersion;
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");

        // Because some DLLs might not implement this function, you must test for 
        // it explicitly. Depending on the particular DLL, the lack of a DllGetVersion 
        // function can be a useful indicator of the version. 
        if (pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;

            ZeroMemory(&dvi, sizeof(DLLVERSIONINFO));
            dvi.cbSize = sizeof(DLLVERSIONINFO);

            hr = (*pDllGetVersion)(&dvi);

            if(SUCCEEDED(hr))
            {
               dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}

BOOL Got_Comm_Ctrl6()
{
    LPCTSTR lpszDllName = _T("C:\\Windows\\System32\\comctl32.dll");
    DWORD dwVer = GetVersion(lpszDllName);
    DWORD dwTarget = PACKVERSION(6,0);
    if(dwVer >= dwTarget)
    {
        // This version of ComCtl32.dll is version 6.0 or later.
        return TRUE;
    }
    // Proceed knowing that version 6.0 or later additions are not available.
    // Use an alternate approach for older the DLL version.
    return FALSE;
}
///////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 - windows - get dll version

#ifdef WIN32    // M_IS_DIR macro = _S_IFDIR
#define M_IS_DIR    _S_IFDIR
#else           // !WIN32 - M_IS_DIR macro = S_IFDIR
#define M_IS_DIR    S_IFDIR
#endif          // WIN32 - M_IS_DIR macro

static struct stat _s_buf;
struct stat *Get_Stat_Buf() { return &_s_buf; }

int is_file_or_directory( char * file )
{
    struct stat *psb = Get_Stat_Buf();
    if (stat(file,psb) == 0) {
        if (psb->st_mode & M_IS_DIR)
            return 2;
        return 1;
    }
    return 0;
}
size_t get_last_file_size() 
{
    struct stat *psb = Get_Stat_Buf();
    return psb->st_size;
}

char *Get_Rect_Stg(PRECT pr)
{
    char *cp = GetNxtBuf();
    sprintf(cp,"%d,%d,%d,%d",
        (int)pr->left, (int)pr->top, (int)pr->right, (int)pr->bottom);
    return cp;
}

// **********************************************************
// Utility Functions - mostly for DEBUG
// ==========================================================
const char *stglnk = "==";
const char *weklnk = "--";
const char *unklnk = "??";
const char *bthlnk = "!!";

char *Get_RC_RC_Stg( PROWCOL prc )
{
    char *tb = GetNxtBuf();
    sprintf(tb,"%c%d", (char) (prc->row + 'A'), prc->col + 1);
    return tb;
}

void Append_RC_setval_RC_Stg( char *tb, PROWCOL prc, int setval ) // return 7@A5, 345@B8, etc...
{
    if (VALUEVALID(setval))
        sprintf(EndBuf(tb),"%d@%c%d", setval, (char) (prc->row + 'A'), prc->col + 1);
    else {
        AddSet2Buf(tb, &prc->set);
        sprintf(EndBuf(tb),"@%c%d", (char) (prc->row + 'A'), prc->col + 1);
    }
}

char *Get_RC_setval_RC_Stg( PROWCOL prc, int setval ) // return 7@A5, 345@B8, etc...
{
    char *tb = GetNxtBuf();
    *tb = 0;
    Append_RC_setval_RC_Stg( tb, prc, setval );
    return tb;
}

char *Get_RC_RCB_Stg( PROWCOL prc )
{
    char *tb = GetNxtBuf();
    sprintf(tb,"R%dC%dB%d", prc->row + 1, prc->col + 1,
        GETBOX(prc->row,prc->col) + 1);
    return tb;
}
char *Get_RC_RCBS_Stg( PROWCOL prc )
{
    char *tb = GetNxtBuf();
    sprintf(tb,"R%dC%dB%d ", prc->row + 1, prc->col + 1,
        GETBOX(prc->row,prc->col) + 1);
	AddSet2Buf(tb,&prc->set);
    return tb;
}
char *Get_RC_RCBC_Stg( PROWCOL prc )
{
    char *tb = GetNxtBuf();
    sprintf(tb,"R%dC%dB%d %s", prc->row + 1, prc->col + 1,
        GETBOX(prc->row,prc->col) + 1,
        CLRSTG(prc->set.uval));
    return tb;
}

char *Get_PB_RC_RCBS_Stg( PABOX2 pb, int row, int col )
{
    ROWCOL rc;
    rc.row = row;
    rc.col = col;
    COPY_SET(&rc.set,&pb->line[row].set[col]);
    return Get_RC_RCBS_Stg(&rc);
}


bool Same_RC_Pair( PROWCOL prc1, PROWCOL prc2 )
{
    if ((prc1->row == prc2->row)&&(prc1->col == prc2->col))
        return true;
    return false;
}

bool Row_Col_in_RC_Vector( vRC & vrc, int row, int col )
{
    ROWCOL rc;
    vRCi ii = vrc.begin();
    for ( ; ii != vrc.end(); ii++) {
        rc = *ii;
        if ((rc.row == row) && (rc.col == col))
            return true;
    }
    return false;
}

bool RC_in_pvrc( vRC *pvrc, int row, int col )
{
    ROWCOL rc;
    vRCi ii = pvrc->begin();
    for ( ; ii != pvrc->end(); ii++) {
        rc = *ii;
        if ((rc.row == row) && (rc.col == col))
            return true;
    }
    return false;
}

//typedef vector<int> vINT;
//typedef vINT::iterator vINTi;

bool Is_Value_in_vINT( int i, vINT & vi )
{
    vINTi ii;
    for (ii = vi.begin(); ii != vi.end(); ii++) {
        if ( i == *ii )
            return true;
    }
    return false;
}

bool Is_Value_in_vSIZET( size_t val, vSIZET & vst )
{
    size_t max = vst.size();
    size_t i;
    for (i = 0; i < max; i++) {
        if (vst[i] == val)
            return true;
    }
    return false;
}

bool Is_Value_in_vTIMET( time_t val, vTIMET & vtt )
{
    size_t max = vtt.size();
    size_t i;
    for (i = 0; i < max; i++) {
        if (vtt[i] == val)
            return true;
    }
    return false;
}

bool Int_in_iVector( int tval, vINT *pvi)
{
    size_t ii;
    int val;
    if (!pvi) return false;
    size_t max = pvi->size();
    for (ii = 0; ii < max; ii++) {
        val = pvi->at(ii);
        if (val == tval)
            return true;
    }
    return false;
}

// Check the SETTINGS of an RCPAIR per a setval
void Append_RCPAIR_Settings( char *tb, PRCPAIR prcpair, int setval )
{
    uint64_t flg, lflg;
    const char *lnktyp;
    const char *clrtype1;
    const char *clrtype2;
    flg = prcpair->rowcol[0].set.flag[setval - 1];
    lflg = (flg & cl_SLA); // isolate LINK type - same both ends
    lnktyp = (lflg & cl_SLA) ? stglnk : weklnk;
    flg = (flg & (cf_XCA|cf_XCB)); // isolate COLOR flags
    clrtype1 = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
    flg = prcpair->rowcol[1].set.flag[setval - 1];
    flg = (flg & (cf_XCA|cf_XCB)); // isolate COLOR flags
    clrtype2 = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
    sprintf(EndBuf(tb)," %s%s%s%s%s ",
        Get_RC_setval_RC_Stg(&prcpair->rowcol[0],setval),clrtype1,
        lnktyp,
        Get_RC_setval_RC_Stg(&prcpair->rowcol[1],setval), clrtype2 );
}

const char *Get_Link_Type( uint64_t flag )
{
    const char *lnktyp = weklnk;
    if (flag & cl_SLA)
        lnktyp = stglnk;
    return lnktyp;
}

const char *Get_Color_Type( uint64_t flag )
{
    uint64_t flg = (flag & cf_XAB); // isolate COLOR
    const char *clrtype = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
    if (add_debug_aic && !flg)
        debug_stop2();
    return clrtype;
}

void Append_RCPAIR_Stg( char *tb, PRCPAIR prcpair, int setval, bool all )
{
    uint64_t flg;
    const char *lnktyp;
    const char *clrtype1;
    const char *clrtype2;
    flg = prcpair->rowcol[0].set.flag[setval - 1];
    lnktyp = Get_Link_Type(flg); // LINK type - should be same both ends!
    clrtype1 = Get_Color_Type(flg);
    flg = prcpair->rowcol[1].set.flag[setval - 1];
    clrtype2 = Get_Color_Type(flg);
    if (all) setval = 0;
    sprintf(EndBuf(tb),"%s%s%s%s%s",
        Get_RC_setval_RC_Stg(&prcpair->rowcol[0],setval), clrtype1,
        lnktyp,
        Get_RC_setval_RC_Stg(&prcpair->rowcol[1],setval), clrtype2 );
}

char *Get_RCPAIR_Stg( PRCPAIR prcpair, int setval, bool all )
{
    char *tb = GetNxtBuf();
    *tb = 0;
    Append_RCPAIR_Stg( tb, prcpair, setval, all );
    return tb;
}

void Append_RC_Stg( char *tb, PROWCOL prc, int setval )
{
    uint64_t flg;
    const char *clrtyp;
    int i;
    if (VALUEVALID(setval)) {
        flg = prc->set.flag[setval - 1];
    } else {
        flg = 0;
        for (i = 0; i < 9; i++)
            flg |= prc->set.flag[i];
        setval = 0;
    }
    clrtyp = Get_Color_Type(flg);
    sprintf(EndBuf(tb),"%s%s", Get_RC_setval_RC_Stg(prc,setval), clrtyp);
}

char *Get_RC_Stg( PROWCOL prc, int setval )
{
    char *tb = GetNxtBuf();
    *tb = 0;
    Append_RC_Stg( tb, prc, setval );
    return tb;
}

void Append_RC_Settings( char *tb, PROWCOL prc, int setval )
{
    uint64_t flg, lflg;
    const char *clrtype;
    if (VALUEVALID(setval)) {
        flg = prc->set.flag[setval - 1];
        lflg = (flg & cl_SLA); // isolate LINK type - same both ends
        flg = (flg & (cf_XCA|cf_XCB)); // isolate COLOR flags
        clrtype = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
    } else {
        clrtype = "";
    }
    sprintf(EndBuf(tb),"%s%s",
        Get_RC_setval_RC_Stg(prc,setval),clrtype);
}

char *Get_RC_setval_color_Stg( PROWCOL prc, int setval )
{
    char *tb = GetNxtBuf();
    uint64_t flg, lflg;
    const char *clrtype;
    if (VALUEVALID(setval)) {
        flg = prc->set.flag[setval - 1];
        lflg = (flg & cl_SLA); // isolate LINK type - same both ends
        flg = (flg & (cf_XCA|cf_XCB)); // isolate COLOR flags
        clrtype = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
    } else {
        clrtype = "";
    }
    sprintf(tb,"%s%s",
        Get_RC_setval_RC_Stg(prc,setval),clrtype);
    return tb;
}


int Get_Empty_Cells_of_setval( PABOX2 pb, vRC &empty, int setval, PRCRCB prcrcb, bool clear )
{
    int count = 0;
    int row, col, val, box, lval;
    ROWCOL rc;
    PSET ps;
    if (prcrcb && clear) {
        for (row = 0; row < 9; row++) {
            prcrcb->vrows[row].clear();
            prcrcb->vcols[row].clear();
            prcrcb->vboxs[row].clear();
        }
    }
    lval = setval - 1; // FIX20120924 - Changed to using lval
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;  // NOT empty
            ps = &pb->line[row].set[col];
            if ( !( ps->val[lval] ) ) continue; // NOT with this value
            rc.row = row;
            rc.col = col;
            rc.box = GETBOX(row,col);
            COPY_SET(&rc.set,ps);
            empty.push_back(rc);
            if (prcrcb) {
                box = GETBOX(row,col);
                prcrcb->vrows[row].push_back(rc);
                prcrcb->vcols[col].push_back(rc);
                prcrcb->vboxs[box].push_back(rc);
            }
            count++;
        }
    }
    return count;
}

bool RC_in_RC_Vector( vRC &vrc, PROWCOL prc )
{
    size_t max = vrc.size();
    size_t i;
    ROWCOL rc;
    for (i = 0; i < max; i++) {
        rc = vrc[i];
        if ((rc.row == prc->row)&&(rc.col == prc->col))
            return true;
    }
    return false;
}

int RC_Vector_to_RCB( vRC &members, PRCRCB prcrcb, bool clear )
{
    int count = 0;
    size_t max = members.size();
    size_t i;
    if (clear) {
        for (i = 0; i < 9; i++) {
            prcrcb->vrows[i].clear();
            prcrcb->vcols[i].clear();
            prcrcb->vboxs[i].clear();
        }
    }
    ROWCOL rc;
    int box;
    vRC vdone;
    for (i = 0; i < max; i++) {
        rc = members[i];
        if (RC_in_RC_Vector(vdone,&rc)) continue;
        vdone.push_back(rc);
        box = GETBOX(rc.row,rc.col);
        prcrcb->vrows[rc.row].push_back(rc);
        prcrcb->vcols[rc.col].push_back(rc);
        prcrcb->vboxs[box].push_back(rc);
        count++;
    }
    return count;
}

uint64_t Get_RC_Strong_Link_Flag( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, int setval,
                                  bool inc_box )
{
    uint64_t slflg = 0;
    int box1 = GETBOX(prc1->row,prc1->col);
    int box2 = GETBOX(prc2->row,prc2->col);
    int cnt;
    if (SAMERCCELL(prc1,prc2)) {
        return 0;
    }

    if (inc_box) {
        if (box1 == box2) {
            if (Count_setval_in_Box(pb,prc1->row,prc1->col,setval) == 2)
                slflg |= cl_SLB; // SAME BOX
        }
    }

    if (prc1->row == prc2->row) {
        cnt = Count_setval_in_Row(pb,prc1->row,prc1->col,setval);
        if (cnt == 2) {
           slflg |= cl_SLR; // SAME ROW, not same BOX
        }
    } else if (prc1->col == prc2->col) {
        cnt = Count_setval_in_Col(pb,prc1->row,prc1->col,setval);
        if (cnt == 2) {
           slflg |= cl_SLC; // SAME COL
        }
    }
    return slflg;
}

bool equal_rcp( RCPAIR &rcp1, RCPAIR &rcp2 )
{
    ROWCOL rc11 = rcp1.rowcol[0];
    ROWCOL rc12 = rcp1.rowcol[1];
    ROWCOL rc21 = rcp2.rowcol[0];
    ROWCOL rc22 = rcp2.rowcol[1];
    if ((rc11.row == rc21.row) && (rc11.col == rc21.col) &&
        (rc12.row == rc22.row) && (rc12.col == rc22.col) )
        return true;
    if ((rc12.row == rc21.row) && (rc12.col == rc21.col) &&
        (rc11.row == rc22.row) && (rc11.col == rc22.col))
        return true;

    return false;
}

bool prcp_in_vrcp( PRCPAIR prcp, vRCP &vrcp )
{
    size_t i;
    RCPAIR rcp = *prcp;
    for (i = 0; i < vrcp.size(); i++) {
        if (equal_rcp(rcp,vrcp[i]))
            return true;
    }
    return false;
}

bool prcp_in_pvrcp( PRCPAIR prcp, vRCP *pvrcp )
{
    size_t i;
    RCPAIR rcp = *prcp;
    for (i = 0; i < pvrcp->size(); i++) {
        if (equal_rcp(rcp,pvrcp->at(i)) )
            return true;
    }
    return false;
}

bool Same_RC_Pair( PRCPAIR pp1, PRCPAIR pp2 )
{
    PROWCOL prcA, prcB;
    PROWCOL prc1, prc2;
    prcA = &pp1->rowcol[0];
    prcB = &pp1->rowcol[1];
    prc1 = &pp2->rowcol[0];
    prc2 = &pp2->rowcol[1];
    if (SAMERCCELL(prcA,prc1) && SAMERCCELL(prcB,prc2))
            return true;
    if (SAMERCCELL(prcA,prc2) && SAMERCCELL(prcB,prc1))
            return true;
    return false;
}

char *Get_RCPair_Stg( PRCPAIR prcp, int setval )
{
    char *tb = GetNxtBuf();
    *tb = 0;
    Append_RCPAIR_Settings(tb, prcp, setval );
    return tb;
}

void Explode( const char *msg, const char *file, const char *func, int line )
{
    char *tb = GetNxtBuf();
    sprtf("%s\n",msg);
    sprintf(tb,
        "CRITICAL CODE PROBLEM:\r\n%s\r\nFile: %s,\r\nFunction %s,\r\nLine %d\r\nCAN ONLY EXIT on OK!!\r\n",
        msg, file, func, line );
    Do_MsgBox_OK(tb);
    exit(1);
}

int Get_Pairs_of_setval( PABOX2 pb, int setval, vRC *pp, PRCRCB prcrcb, vRCP *pvrcp, bool debug )
{
    int count = 0;
    size_t ii;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    size_t max = pvrcp->size();
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pvrcp->at(ii); // get OUT a pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        if (!prc_in_chain(prc1,pp)) {
            pp->push_back(*prc1);
            prcrcb->vrows[prc1->row].push_back(*prc1);
            prcrcb->vcols[prc1->col].push_back(*prc1);
            prcrcb->vboxs[GETBOX(prc1->row,prc1->col)].push_back(*prc1);
            count++;
        }
        if (!prc_in_chain(prc2,pp)) {
            pp->push_back(*prc2);
            prcrcb->vrows[prc2->row].push_back(*prc2);
            prcrcb->vcols[prc2->col].push_back(*prc2);
            prcrcb->vboxs[GETBOX(prc2->row,prc2->col)].push_back(*prc2);
            count++;
        }
    }
    if (debug) {
        char *tb = GetNxtBuf();
        sprintf(tb,"%d pairs: ", (int)max);
        int i, cnt;
        for (i = 0; i < 9; i++) {
            cnt = prcrcb->vrows[i].size();
            if (!cnt) continue;
            sprintf(EndBuf(tb),"R%d=%d ", i + 1, cnt);
        }
        for (i = 0; i < 9; i++) {
            cnt = prcrcb->vcols[i].size();
            if (!cnt) continue;
            sprintf(EndBuf(tb),"C%d=%d ", i + 1, cnt);
        }
        for (i = 0; i < 9; i++) {
            cnt = prcrcb->vboxs[i].size();
            if (!cnt) continue;
            sprintf(EndBuf(tb),"B%d=%d ", i + 1, cnt);
        }
        OUTIT(tb);
    }
    return count;
}

bool Value_in_SET( int val, PSET ps )
{
 	if (!VALUEVALID(val)) // is value 1-9 ONLY
 		return false;	// value NOT valid so NOT in SET
    if (ps->val[val - 1])
        return true;   // value is IN SET
    return false; // Value is NOT in SET
}

bool prc_in_pvrcp( PROWCOL prc, vRCP *pp, int count )
{
    size_t max = pp->size();
    size_t ii;
    PRCPAIR prcp;
    PROWCOL prc1, prc2;
    int cnt = 0;
    for (ii = 0; ii < max; ii++) {
        prcp = &pp->at(ii);
        prc1 = &prcp->rowcol[0];
        prc2 = &prcp->rowcol[1];
        if (SAMERCCELL(prc1,prc))
            cnt++;
        if (SAMERCCELL(prc2,prc))
            cnt++;
        if (cnt > count)
            return true;
    }
    return false;
}

// DIAGNOSTIC show of RCRCB arrays
void Show_PRCB(PRCRCB prcrcb)
{
    vRC *pvrc;
    PROWCOL prc;
    int i;
    size_t max, ii, total;
    char *tb = GetNxtBuf();
    sprtf("Diagnostic display of current RCRCB\n");
    total = 0;
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vrows[i];
        max = pvrc->size();
        total += max;
        sprintf(tb,"Row %d: %d ", i + 1, (int)max);
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            Append_RC_setval_RC_Stg(tb,prc);
            ADDSP(tb);
        }
        OUTIT(tb);
        pvrc = &prcrcb->vcols[i];
        max = pvrc->size();
        total += max;
        sprintf(tb,"Col %d: %d ", i + 1, (int)max);
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            Append_RC_setval_RC_Stg(tb,prc);
            ADDSP(tb);
        }
        OUTIT(tb);
        pvrc = &prcrcb->vboxs[i];
        max = pvrc->size();
        total += max;
        sprintf(tb,"Box %d: %d ", i + 1, (int)max);
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            Append_RC_setval_RC_Stg(tb,prc);
            ADDSP(tb);
        }
        OUTIT(tb);
    }
    sprtf("Shown %d Cells by RCB (%d empties)\n", (int)total, (int)(total / 3));
}

void Clear_PRCRCB( PRCRCB prcrcb )
{
    int i;
    for (i = 0; i < 9; i++) {
        prcrcb->vrows[i].clear();
        prcrcb->vcols[i].clear();
        prcrcb->vboxs[i].clear();
    }
}

// Get ALL empty cells in vRC empty, 
// AND if given prcrcb, into ROW, COL, BOX vRC
int Get_Empty_Cells( PABOX2 pb, vRC &empty, PRCRCB prcrcb, bool clear )
{
    int count = 0;
    int row, col, val, box;
    ROWCOL rc;
    PSET ps;
    char *tb = GetNxtBuf();
    if (prcrcb && clear) {
        Clear_PRCRCB( prcrcb );
    }

    // process whole sudoku
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            rc.row = row;
            rc.col = col;
            rc.box = box = GETBOX(row,col);
            COPY_SET(&rc.set,ps);
            empty.push_back(rc);
            if (prcrcb) {
                prcrcb->vrows[row].push_back(rc);
                prcrcb->vcols[col].push_back(rc);
                prcrcb->vboxs[box].push_back(rc);
            }
            count++;
        }
    }
    if (add_debug_empty)
        Show_Cells_Collected( pb, prcrcb );
    return count;
}

int Get_Empty_Cells_per_cand_count( PABOX2 pb, vRC *pempty, int scnt, PRCRCB prcrcb, bool clear )
{
    int count = 0;
    int row, col, box, val;
    ROWCOL rc;
    PSET ps;
    if (prcrcb && clear) {
        Clear_PRCRCB(prcrcb);
    }
    if (clear)
        pempty->clear();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            if (Get_Set_Cnt(ps) != scnt) continue;
            rc.row = row;
            rc.col = col;
            rc.box = box = GETBOX(row,col);
            COPY_SET(&rc.set,ps);
            pempty->push_back(rc);
            if (prcrcb) {
                prcrcb->vrows[row].push_back(rc);
                prcrcb->vcols[col].push_back(rc);
                prcrcb->vboxs[box].push_back(rc);
            }
            count++;
        }
    }
    return count;
}

int Get_Empty_Cells_per_not_cand_count( PABOX2 pb, vRC *pempty, int scnt, PRCRCB prcrcb, bool clear )
{
    int count = 0;
    int row, col, box, val;
    ROWCOL rc;
    PSET ps;
    if (prcrcb && clear) {
        for (val = 0; val < 9; val++) {
            prcrcb->vrows[val].clear();
            prcrcb->vcols[val].clear();
            prcrcb->vboxs[val].clear();
        }
    }
    if (clear)
        pempty->clear();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            if (Get_Set_Cnt(ps) == scnt) continue;
            rc.row = row;
            rc.col = col;
            rc.box = box = GETBOX(row,col);
            COPY_SET(&rc.set,ps);
            pempty->push_back(rc);
            if (prcrcb) {
                prcrcb->vrows[row].push_back(rc);
                prcrcb->vcols[col].push_back(rc);
                prcrcb->vboxs[box].push_back(rc);
            }
            count++;
        }
    }
    return count;
}

void Show_RC_Vector( vRC & vrc )
{
    int cnt, box;
    vRCi ii;
    ROWCOL rc, rcp;
    char *tb = GetNxtBuf();
    SET comm;
    sprintf(tb,"List of %d vectors...", (int)vrc.size());
    OUTIT(tb);
    cnt = 0;
    for (ii = vrc.begin(); ii != vrc.end(); ii++) {
        rc = *ii;
        cnt++;
        box = GETBOX(rc.row,rc.col);
        sprintf(tb,"%2d: R%dC%dB%d %s [%d] ", cnt, rc.row + 1, rc.col + 1, box + 1,
            CLRSTG(rc.set.uval), Get_Set_Cnt( &rc.set ));
        AddSet2Buf(tb,&rc.set);
        if (cnt > 1) {
            int scnt = Get_Set_Cnt( &rc.set );
            while (scnt < 6) {
                strcat(tb," ");
                scnt++;
            }
            Zero_SET(&comm);
            int ccnt = Get_Shared_SET( &rc.set, &rcp.set, &comm );
            if (ccnt) {
                sprintf(EndBuf(tb)," shr [%d] ", ccnt);
                AddSet2Buf(tb,&comm);
            } else {
                strcat(tb, " no shr - new?");
            }
        }
        OUTIT(tb);
        rcp = rc;
    }
}

#ifndef STRLEN
#define STRLEN strlen
#endif

void trim_float_buf( char *pb )
{
   size_t len = STRLEN(pb);
   size_t i, dot, zcnt;
   for( i = 0; i < len; i++ )
   {
      if( pb[i] == '.' )
         break;
   }
   dot = i + 1; // keep position of decimal point (a DOT)
   zcnt = 0;
   for( i = dot; i < len; i++ )
   {
      if( pb[i] != '0' )
      {
         i++;  // skip past first
         if( i < len )  // if there are more chars
         {
            i++;  // skip past second char
            if( i < len )
            {
               size_t i2 = i + 1;
               if( i2 < len )
               {
                  if ( pb[i2] >= '5' )
                  {
                     if( pb[i-1] < '9' )
                     {
                        pb[i-1]++;
                     }
                  }
               }
            }
         }
         pb[i] = 0;
         break;
      }
      zcnt++;     // count ZEROS after DOT
   }
   if( zcnt == (len - dot) )
   {
      // it was ALL zeros
      pb[dot - 1] = 0;
   }
}

char *double_to_stg( double d ) // get_double_stg trimmed...
{
    char *cp = GetNxtBuf();
    sprintf(cp,"%lf",d);
    trim_float_buf(cp);
    return cp;
}

/////////////////////////////////////////////////////////////////////////
// value conversions to a string
/////////////////////////////////////////////////////////////////////////
char *get_I64i_Stg( long long val )
{
    char *cp = GetNxtBuf();
#ifdef WIN32
    sprintf(cp,"%I64d",val);
#else
    sprintf(cp,"%lld",val);
#endif
    return cp;
}

char *get_I64u_Stg( unsigned long long val )
{
    char *cp = GetNxtBuf();
#ifdef WIN32
    sprintf(cp,"%I64u",val);
#else
    sprintf(cp,"%llu",val);
#endif
    return cp;
}

char *get_I64x_Stg( unsigned long long val )
{
    char *cp = GetNxtBuf();
#ifdef WIN32
    sprintf(cp,"%I64x",val);
#else
    sprintf(cp,"%llx",val);
#endif
    return cp;
}

// eof - Sudo_Utils.cxx
