// Sudo_Debug.hxx
#ifndef _SUDO_DEBUG_HXX_
#define _SUDO_DEBUG_HXX_


// in Sudo_Coloring.cxx
extern int add_debug_cs;    // used in Sud_Coloring.cxx

// in Sudo_Logic.cxx and others...
extern int add_debug_np;   // debug np, like // Seek_Same_Pair_In_Row()...
extern int add_debug_PP2;  // Another try at Pointing Pairs
extern int add_debug_pp;   // original pointing pairs
extern int add_debug_lk;   // Do_Locked_Excl()
extern int add_debug_xw;   // debug X-Wings
extern int add_debug_yw;   // debug Y-Wings
extern int add_debug_ur;   // debug Unique Rectangles
extern int add_debug_ur2;  // debug Unique Rectangles 2
extern int add_debug_ff;   // debug fill by flags
extern int add_debug_empty; // display list of empty cells collected
extern int add_debug_xc;   // debug for X-Cycles
extern int add_debug_xc3;  // debug for X-Cycles - another try
extern int add_debug_cs2;  // debug color scan 2
extern int add_debug_ht;   // debug hidden triple search
extern int add_debug_sl;   // debug strong link search
extern int add_debug_xy;   // debug xy-chain search
extern int add_debug_sl2;  // extension of strong links
extern int add_debug_sl3;  // extension of strong links
extern int add_debug_aic;  // debug AIC - Alternating Inference Chains
extern int add_debug_xyz;  // debug xyz wing scan

// in Sudo_Paint.cxx
extern int add_debug_paint;    // add debug to paint output
extern int add_debug_paint_set;
extern int add_debug_pfr;  // add debug to FillRect()
extern int add_debug_pxy2; // debug output when painting XY-Chains 2
extern int add_debug_paic;  // debug paint AIC - Alternating Inference Chains

extern void Out_Debug_Set();
extern const char *Flag2Name( uint64_t flag );
extern void AddFlags2Buf(char *tb, uint64_t flag);
extern void Debug_Flag_Set(uint64_t flag);

extern BOOL Do_INIT_DebugOptions(HWND hDlg);
extern BOOL Do_DebugOptions_OK(HWND hDlg);
extern VOID Do_ALL_Dbg_ON(HWND hDlg);
extern VOID Do_ALL_Dbg_OFF(HWND hDlg);

typedef struct tagDBGLIST  {
    const char *pcaption;
    UINT        conrol;
    int *       pdebug;
    PBOOL       pchg;
    STRATSERV   serv;
    PBOOL       penab;
    PBOOL       pchgd;
} DBGLIST, *PDBGLIST;
extern DBGLIST DbgList[];

extern void run_test3(PABOX2 pb); // run color scan

extern int on_xcycles_test;

extern void debug_stop2();

extern void debug_stop( PROWCOL prc );
// DBGRC(prc,7,9);
#define DBGRC(prc,r,c) if ((prc->row == (r - 1)) && (prc->col == (c - 1))) debug_stop(prc)
extern void debug_step( int row, int col );
#define DBGRC2(r1,c1,r2,c2) if (((r2 + 1) == r1) && ((c2 + 1) == c1)) debug_step(r1,c1)

#define SAME_BOX(a,b) (GETBOX(a->row,a->col) == GETBOX(b->row,b->col))

//    { "Naked Pairs",IDC_CHECK1, &add_debug_np, &bChgDNP      , Do_Naked_Pairs, 
extern BOOL Do_NP;   //, &Chg_NP          },  // 0
//    { "Color Scan",IDC_CHECK2,  &add_debug_cs2, &bChgDCS2    , Do_Color_Scan2, 
extern BOOL Do_CS2; //, &Chg_CS2        },  // 1
//    { "Pointing Pairs",IDC_CHECK3,&add_debug_PP2, &bChgDPP2  , Do_Pointing_Pairs, &
extern BOOL Do_PP; //, &Chg_PP       },  // 2
//    { "Locked Exclusion",IDC_CHECK4,&add_debug_lk, &bChgDLK  , Do_Locked_Excl, &
extern BOOL Do_LE; //, &Chg_LE          },  // 3
//    { "X-Wings",IDC_CHECK5,&add_debug_xw, &bChgDXW           , Do_XWing_Scan, &
extern BOOL Do_XW;  //, &Chg_XW           },  // 4
//    { "Y-Wings",IDC_CHECK6,&add_debug_yw, &bChgDYW           , Do_YWing_Scan, &
extern BOOL Do_YW; //, &Chg_YW           },  // 5
//    { "Unique Rectangle",IDC_CHECK7,&add_debug_ur, &bChgDUR  , Do_UniqRect_Scan, &
extern BOOL Do_UR; //, &Chg_UR        },  // 6
//    { "X-Cycles",IDC_CHECK8,&add_debug_xc, &bChgDXC          , Do_XCycles_Scan, &
extern BOOL Do_XC; //, &Chg_XC         },  // 7
//    { "Hidden Triples",IDC_CHECK9,&add_debug_ht, &bChgDHT    , Do_Hidden_Triple_Scan, &
extern BOOL Do_HT; //, &Chg_HT   },  // 8
//    { "XY-Chains",IDC_CHECK10,&add_debug_xy, &bChgDXY        , Do_XY_Chains, &
extern BOOL Do_XY; //, &Chg_XY            },  // 9
//    { "Strong Links",IDC_CHECK11,&add_debug_sl, &bChgDSL     , Do_Strong_Links_Scan, &
extern BOOL Do_SL; //, &Chg_SL    },  // 10
//    { "Strong Links 2",IDC_CHECK12,&add_debug_sl2, &bChgDSL2 , Do_Strong_Links2_Scan, &
extern BOOL Do_SL2; //, &Chg_SL2 },  // 11
//    { "Fill by Flags",IDC_CHECK13,&add_debug_ff, &bChgDFF    , 0, 0, 0                                  },  // 12
//    { "Paint Rect. Fill",IDC_CHECK14,&add_debug_pfr,&bChgPFR , 0, 0, 0                                  },  // 13
//    { "X-Cycles 3",IDC_CHECK15,&add_debug_xc3, &bChgDXC3     , Do_XCycles_Scan3, &
extern BOOL Do_XC3; //, &Chg_XC3      },  // 14
//    { "Paint XY-Chains 2",IDC_CHECK16,&add_debug_pxy2, &bChgPXY2, 0, 0, 0                               },  // 15
//    { "AIC",IDC_CHECK17,&add_debug_aic, &bChgDAIC            , Do_AIC_Scans, &
extern BOOL Do_AIC; //, &Chg_AIC          },  // 16
//    { "Paint AIC",IDC_CHECK18,&add_debug_paic, &bChgPAIC     , 0, 0, 0                                  },  // 17
//    { "XYZ-Scan",IDC_CHECK19,&add_debug_xyz, &bChgDXYZ       , Do_XYZ_Wings, &
extern BOOL Do_XYZ; //, &Chg_XYZ          },  // 18
//    { "Strong Links 3",IDC_CHECK20,&add_debug_sl3, &bChgDSL3 , Do_Strong_Link3_Scan, &
extern BOOL Do_SL3; //, &Chg_SL3  },  // 19

#endif // #ifndef _SUDO_DEBUG_HXX_
// eof - Sudo_Debug.hxx

