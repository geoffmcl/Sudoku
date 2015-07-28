// Sudo_Debug.cxx

#include "Sudoku.hxx"

// Should be the ONLY place for DEBUG flags

int add_debug_cs2 = 0;  // debug for a new color scan - MAYBE FINISHED and OK
int add_debug_cs = 0;   // used in Sudo_Coloring.cxx - Abandoned
BOOL bChgDCS2 = FALSE;

// int Seek_Same_Pair_In_Row( PABOX2 pb, int crow, int ccol, PSET ps2 )
int add_debug_np = 0;   // debug naked pairs
BOOL bChgDNP = FALSE;

int add_debug_PP2 = 0;  // Another try at Pointing Pairs
int add_debug_pp = 0;   // original pointing pairs
BOOL bChgDPP2 = FALSE;

int add_debug_lk = 0; // Do_Locked_Excl()
BOOL bChgDLK = FALSE;

int add_debug_xw = 0;   // debug X-Wings
BOOL bChgDXW = FALSE;

int add_debug_yw = 0;   // debug Y-Wings
BOOL bChgDYW = FALSE;

int add_debug_ur = 0;    // debug Unique Rectangles
BOOL bChgDUR = FALSE;
int add_debug_ur2 = 0;  // debug Unique Rectangle 3
BOOL bChgDUR2 = FALSE;


int add_debug_ff = 0;    // debug fill by flags
BOOL bChgDFF = FALSE;

int add_debug_empty = 0;    // display list of empty cells collected

int add_debug_xc = 0;   // debug for X-Cycles - NOT FINISHED
BOOL bChgDXC = FALSE;
int add_debug_xc3 = 0;   // debug for X-Cycles - another try
BOOL bChgDXC3 = FALSE;

int add_debug_ht = 0;    // debug hidden triple search
BOOL bChgDHT = FALSE;

int add_debug_sl = 0;    // debug strong link search
BOOL bChgDSL = FALSE;
int add_debug_sl2 = 0;  // extension of strong links
BOOL bChgDSL2 = FALSE;
int add_debug_sl3 = 0;  // extension of strong links
BOOL bChgDSL3 = FALSE;

int add_debug_xy = 0;    // debug xy-chain search - 20120922 - LOOKS GOOD
BOOL bChgDXY = FALSE;

int add_debug_aic = 0;  // debug AIC - Alternating Inference Chains
BOOL bChgDAIC = FALSE;

int add_debug_xyz = 0;  // debug XYZ scan
BOOL bChgDXYZ = FALSE;

// in Sudo_Paint.cxx
// ===================================================================
int add_debug_paint = 0;    // add debug to paint output
int add_debug_paint_set = 0;
int add_debug_pfr = 0;  // add debug to FillRect()
BOOL bChgPFR = FALSE;
int add_debug_paic = 0;  // debug paint AIC - Alternating Inference Chains
BOOL bChgPAIC = FALSE;
int add_debug_pxy2 = 0; // debug output when painting XY-Chains 2
BOOL bChgPXY2 = FALSE;
// ===================================================================

BOOL Do_US = TRUE;
BOOL Do_NP = TRUE;
#if 0 // this is NOT working properly yet
BOOL Do_HP = FALSE;
#endif // 0
BOOL Do_LE = TRUE;
BOOL Do_PP = TRUE;
BOOL Do_XW = TRUE;
BOOL Do_YW = TRUE;
#ifdef USE_COLOR2_SCAN
BOOL Do_CS2 = TRUE;
#else // !USE_COLOR2_SCAN
BOOL Do_CS = FALSE;
#endif // USE_COLOR2_SCAN y/n
BOOL Do_UR = TRUE;
BOOL Do_HT = TRUE;
BOOL Do_XY = TRUE;
BOOL Do_SL2 = TRUE;
BOOL Do_SL3 = TRUE;
BOOL Do_SL = TRUE;
BOOL Do_AIC = TRUE;
BOOL Do_XYZ = TRUE;
BOOL Do_XC = TRUE;
BOOL Do_XC3 = TRUE;
BOOL Do_UR2 = TRUE;

BOOL Chg_US = FALSE;
BOOL Chg_NP = FALSE;
#if 0 // this is NOT working properly yet
BOOL Chg_HP = FALSE;
#endif // 0
BOOL Chg_LE = FALSE;
BOOL Chg_PP = FALSE;
BOOL Chg_XW = FALSE;
BOOL Chg_YW = FALSE;
#ifdef USE_COLOR2_SCAN
BOOL Chg_CS2 = FALSE;
#else // !USE_COLOR2_SCAN
BOOL Chg_CS = FALSE;
#endif // USE_COLOR2_SCAN y/n
BOOL Chg_UR = FALSE;
BOOL Chg_HT = FALSE;
BOOL Chg_XY = FALSE;
BOOL Chg_SL2 = FALSE;
BOOL Chg_SL3 = FALSE;
BOOL Chg_SL = FALSE;
BOOL Chg_AIC = FALSE;
BOOL Chg_XYZ = FALSE;
BOOL Chg_XC = FALSE;
BOOL Chg_XC3 = FALSE;
BOOL Chg_UR2 = FALSE;

typedef struct tagSTRATLIST {
    STRATSERV serv;
    PBOOL penab;
    PBOOL pchgd;
    int off;
}STRATLIST, *PSTRATLIST;

STRATLIST StratServers[] = {
    { Do_Unique_Scan, &Do_US, &Chg_US,          -1 },
    { Do_Naked_Pairs, &Do_NP, &Chg_NP,           0 },
#if 0 // this is NOT working properly yet
    { Do_Hidden_Pairs, &Do_HP, &Chg_HP,         -1 },
#endif // 0
    { Do_Locked_Excl, &Do_LE, &Chg_LE,           3 },
    { Do_Pointing_Pairs, &Do_PP, &Chg_PP,        2 },
    { Do_XWing_Scan, &Do_XW, &Chg_XW,            4 },
    { Do_YWing_Scan, &Do_YW, &Chg_YW,            5 },
#ifdef USE_COLOR2_SCAN
    { Do_Color_Scan2, &Do_CS2, &Chg_CS2,         1 },
#else // !USE_COLOR2_SCAN
    { Do_Color_Scan, &Do_CS, &Chg_CS,           -1 },
#endif // USE_COLOR2_SCAN y/n
    { Do_UniqRect_Scan, &Do_UR, &Chg_UR,         6 },
    { Do_Hidden_Triple_Scan, &Do_HT, &Chg_HT,    8 },
    { Do_XY_Chains, &Do_XY, &Chg_XY,             9 },
    { Do_Strong_Links2_Scan, &Do_SL2, &Chg_SL2, 11 },
    { Do_Strong_Link3_Scan, &Do_SL3, &Chg_SL3,  -1 },
    { Do_Strong_Links_Scan, &Do_SL, &Chg_SL,    10 },
    { Do_AIC_Scans, &Do_AIC, &Chg_AIC,          16 },
    { Do_XYZ_Wings, &Do_XYZ, &Chg_XYZ,          18 },
    { Do_XCycles_Scan, &Do_XC, &Chg_XC,          7 },
    { Do_XCycles_Scan3, &Do_XC3, &Chg_XC3,      14 },

    // LAST ENTRY
    { 0, 0, 0, -1 }
};


DBGLIST DbgList[] = {
    { "Naked Pairs",IDC_CHECK1, &add_debug_np, &bChgDNP      , Do_Naked_Pairs, &Do_NP, &Chg_NP          },  // 0
    { "Color Scan",IDC_CHECK2,  &add_debug_cs2, &bChgDCS2    , Do_Color_Scan2, &Do_CS2, &Chg_CS2        },  // 1
    { "Pointing Pairs",IDC_CHECK3,&add_debug_PP2, &bChgDPP2  , Do_Pointing_Pairs, &Do_PP, &Chg_PP       },  // 2
    { "Locked Exclusion",IDC_CHECK4,&add_debug_lk, &bChgDLK  , Do_Locked_Excl, &Do_LE, &Chg_LE          },  // 3
    { "X-Wings",IDC_CHECK5,&add_debug_xw, &bChgDXW           , Do_XWing_Scan, &Do_XW, &Chg_XW           },  // 4
    { "Y-Wings",IDC_CHECK6,&add_debug_yw, &bChgDYW           , Do_YWing_Scan, &Do_YW, &Chg_YW           },  // 5
    { "Unique Rectangle",IDC_CHECK7,&add_debug_ur, &bChgDUR  , Do_UniqRect_Scan, &Do_UR, &Chg_UR        },  // 6
    { "X-Cycles",IDC_CHECK8,&add_debug_xc, &bChgDXC          , Do_XCycles_Scan, &Do_XC, &Chg_XC         },  // 7
    { "Hidden Triples",IDC_CHECK9,&add_debug_ht, &bChgDHT    , Do_Hidden_Triple_Scan, &Do_HT, &Chg_HT   },  // 8
    { "XY-Chains",IDC_CHECK10,&add_debug_xy, &bChgDXY        , Do_XY_Chains, &Do_XY, &Chg_XY            },  // 9
    { "Strong Links",IDC_CHECK11,&add_debug_sl, &bChgDSL     , Do_Strong_Links_Scan, &Do_SL, &Chg_SL    },  // 10
    { "Strong Links 2",IDC_CHECK12,&add_debug_sl2, &bChgDSL2 , Do_Strong_Links2_Scan, &Do_SL2, &Chg_SL2 },  // 11
    { "Fill by Flags",IDC_CHECK13,&add_debug_ff, &bChgDFF    , 0, 0, 0                                  },  // 12
    { "Paint Rect. Fill",IDC_CHECK14,&add_debug_pfr,&bChgPFR , 0, 0, 0                                  },  // 13
    { "X-Cycles 3",IDC_CHECK15,&add_debug_xc3, &bChgDXC3     , Do_XCycles_Scan3, &Do_XC3, &Chg_XC3      },  // 14
    { "Paint XY-Chains 2",IDC_CHECK16,&add_debug_pxy2, &bChgPXY2, 0, 0, 0                               },  // 15
    { "AIC",IDC_CHECK17,&add_debug_aic, &bChgDAIC            , Do_AIC_Scans, &Do_AIC, &Chg_AIC          },  // 16
    { "Paint AIC",IDC_CHECK18,&add_debug_paic, &bChgPAIC     , 0, 0, 0                                  },  // 17
    { "XYZ-Scan",IDC_CHECK19,&add_debug_xyz, &bChgDXYZ       , Do_XYZ_Wings, &Do_XYZ, &Chg_XYZ          },  // 18
    { "Strong Links 3",IDC_CHECK20,&add_debug_sl3, &bChgDSL3 , Do_Strong_Link3_Scan, &Do_SL3, &Chg_SL3  },  // 19
    { "Unique Rects 2",IDC_CHECK21,&add_debug_ur2, &bChgDUR2 , Do_UniqRect2_Scan, &Do_UR2, &Chg_UR2     },  // 20

    // *** MUST be LAST ***
    { 0, 0, 0, 0, 0, 0, 0 }
};

#ifdef WIN32    // windows select debug options dialog

BOOL Do_INIT_DebugOptions(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    while (plist->pcaption) {
        CheckDlgButton(hDlg, plist->conrol,
            *plist->pdebug ? BST_CHECKED : BST_UNCHECKED);
        plist++;
    }
    CenterWindow(hDlg, g_hWnd);
    return TRUE;
}

BOOL Do_DebugOptions_OK(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    UINT res;
    while (plist->pcaption) {
        res = IsDlgButtonChecked(hDlg, (int)plist->conrol);
        if (res == BST_CHECKED)
            sprtf("DEBUG: [%s] is ON\n", plist->pcaption);
        Toggle_CheckBox_Bool( res, plist->pdebug, plist->pchg );
        plist++;
    }
    return TRUE;
}

VOID Do_ALL_Dbg_ON(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    while (plist->pcaption) {
        CheckDlgButton(hDlg, plist->conrol, BST_CHECKED);
        plist++;
    }
}
VOID Do_ALL_Dbg_OFF(HWND hDlg)
{
    PDBGLIST plist = &DbgList[0];
    while (plist->pcaption) {
        CheckDlgButton(hDlg, plist->conrol, BST_UNCHECKED);
        plist++;
    }
}

#endif // #ifdef WIN32 - select debug option dialog

void Out_Debug_Set()
{
    // show what DEBUG flags are ON, maybe...
}

typedef struct tagFLG2NM {
    uint64_t flag;
    const char *name;
} FLG2NM, *PFLG2NM;

FLG2NM flg2nm[] = {
    {cf_URF,"cf_URF"},
    {cf_UCF,"cf_UCF"},
    {cf_UBF,"cf_UBF"},
    {cf_NP, "cf_NP"},
    {cf_NPE,"cf_NPE"},
    {cf_HP, "cf_HP"},
    {cf_HPE,"cf_HPE"},
    {cf_HT, "cf_HT"},
    {cf_HTE,"cf_HTE"},
    {cf_LK,"cf_LK"},
    {cf_LKE,"cf_LKE"},
    {cf_XW,"cf_XW"},
    {cf_XWE,"cf_XWE"},
    {cf_XWR,"cf_XWR"},
    {cf_SS,"cf_SS"},
    {cf_SSE,"cf_SSE"},
    {cf_PP,"cf_PP"},
    {cf_PPE,"cf_PPE"},
    {cf_YW,"cf_YW"},
    {cf_YWE,"cf_YWE"},
    {cf_YWC,"cf_YWC"},
    {cf_CC,"cf_CC"},
    {cf_CCE,"cf_CCE"},
    {cf_UR,"cf_UR"},
    {cf_URE,"cf_URE"},
    {cf_NT, "cf_NT"},
    {cf_NTE,"cf_NPT"},
    {cf_XC,"cf_XC"},
    {cf_XCE,"cf_XCE"},
    {cf_XCA,"cf_XCA"},
    {cf_XCB,"cf_XCB"},
    {cf_XY,"cf_XY"},
    {cf_XYE,"cf_XYE"},
    {cf_SL,"cf_SL"},
    {cf_SLE,"cf_SLE"},

    // Upper set - while there is bit space
    {cl_New,"cl_New"},
    {cl_CSC,"cl_CSC"},
    {cl_Bad,"cl_Bad"},
    {cl_MCC,"cl_MCC"},
    {cl_UNR,"cl_UNR"},
    {cl_SLB,"cl_SLB"},
    {cl_SLC,"cl_SLC"},
    {cl_SLR,"cl_SLR"},
    {cl_LBB,"cl_LBB"},
    {cl_LBC,"cl_LBC"},
    {cl_LBR,"cl_LBR"},
    {cl_LSS,"cl_LSS"},

    // must be LAST entry
    {0, 0 }
};

static const char *NoFlag = "none";
const char *Flag2Name( uint64_t flag )
{
    PFLG2NM pf2n = &flg2nm[0];
    while (pf2n->name) {
        if (pf2n->flag & flag)
            return pf2n->name;
        pf2n++;
    }
    return NoFlag;
}

void AddFlags2Buf(char *tb, uint64_t flag)
{
    if (!flag) return;
    uint64_t test = 1;
    int i;
    for (i = 0; i < 64; i++) {
        if (flag & test)
            sprintf(EndBuf(tb),"%s ", Flag2Name(test));
        // test = (test << 1);
        test <<= 1;
    }
}

void Debug_Flag_Set(uint64_t flag)
{
    if (!flag) return;
    uint64_t test = 1;
    int i;
    char *tb = GetNxtBuf();
    const char *pfn;
    *tb = 0;
    for (i = 0; i < 64; i++) {
        if (flag & test) {
            pfn = Flag2Name(test);
            if (strcmp(pfn,NoFlag) == 0) {
#ifdef WIN32    // format for 64-bit value - I64
                sprintf(EndBuf(tb),"0x%I64X ",test);
#else           // !WIN32 - use PRIx64      
                sprintf(EndBuf(tb),"%" PRIx64 " ",test);
#endif          // WIN32 y/n
            }
        }
        test <<= 1;
    }
    if (*tb) {
        sprtf("WARNING: Missing Flag(s) %s\n", tb);
    }
}




void debug_stop2()
{
    int i;
    i = 0;
}

void run_test3_2( PABOX2 pb )
{
    //do_color_test = 1;
    //on_color_test = 1;
    // examples\coloring3.txt
    //do_color_test = 5;
    //on_color_test = 5;
    // examples\coloring6.txt
    //do_color_test = 3;
    //on_color_test = 3;
    // examples\coloring7.txt
    //do_color_test = 7;
    //on_color_test = 7;
    //Do_Color_Scan(pb);

#ifndef USE_COLOR2_SCAN
    // examples\diabolical4.txt
    do_color_test = 1;
    on_color_test = 1;
    int prev = add_debug_cs;
    add_debug_cs = 1;
    Do_Color_Scan(pb);
    add_debug_cs = prev;
    //Do_Pointing_Pairs(pb);
    set_repaint();
#endif // !USE_COLOR2_SCAN

}

int on_xcycles_test = 0;
void run_test3_XC( PABOX2 pb )
{
    if (on_xcycles_test) {
        on_xcycles_test++;
        if (on_xcycles_test >= 10) {
            sprtf("End XCycles test\n");
            on_xcycles_test = 0;
        }
    } else {
        on_xcycles_test = 1;
        Do_XCycles_Scan(pb);
    }
    set_repaint();
}

int test_xc_scan3( PABOX2 pb );

void run_test3( PABOX2 pb )
{
    test_xc_scan3(pb);
    set_repaint();
}


void run_test3_3( PABOX2 pb )
{
    if (do_color_test2) {
        sprtf("Setting Color Test OFF\n");
        do_color_test2 = 0;
    } else {
        //do_color_test2 = 7; // for extreme-01-2.txt - LOOKS GOOD - same outline...
        do_color_test2 = 1;     // for color1.txt
        sprtf("Doing Color Test for candidate %d\n", do_color_test2);
        Do_Color_Scan2(pb);
    }
    set_repaint();
}

void run_test3_4( PABOX2 pb )
{
    if ( test_xy_chain ) {
        sprtf("Setting XY-Chain Test OFF\n");
        test_xy_chain = 0;
    } else {
         test_xy_chain = 5;
         sprtf("Doing XY-CHain Test for candidate %d\n", test_xy_chain);
         Mark_XY_Chains(get_curr_box());
    }
    set_repaint();
}

int test_sl_scan = 0;
int test_sl_setval;
void run_test3_5( PABOX2 pb )
{
    Set_SL_invalid(0);
    if (test_sl_scan) {
        Set_SL_invalid(0);
        sprtf("Clear SL view\n");
    } else {
        test_sl_setval = 6;
        //int count = Scan_Strong_Links_by_setval(get_curr_box(), test_sl_setval );
        int count = Mark_Strong_Link_for_setval( pb, test_sl_setval );
        sprtf("Done SL scan for cand %d, got count %d\n", test_sl_setval, count);
        //if (count)
        //    Set_SL_invalid(test_sl_setval);
    }
    set_repaint();
}

void debug_stop( PROWCOL prc )
{
    debug_stop2();
    char *tb = Get_RC_RCBC_Stg(prc);
    int box;
    box = GETBOX(prc->row,prc->col);
}
void debug_step( int row, int col )
{
    debug_stop2();
    int i = 0;
    char buf[32];
    sprintf(buf,"R%dC%d", row + 1, col + 1);
    i = 1;
}

// eof - Sudo_Debug.cxx

