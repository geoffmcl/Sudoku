// Sudo_XCycles.hxx
#ifndef _SUDO_XCYCLES_HXX_
#define _SUDO_XCYCLES_HXX_

extern int Do_XCycles_Scan( PABOX2 pb = 0 );
extern vRC *get_XCycles_pvrc( int val );
extern PROWCOL get_XCycles_RC( int row, int col, int setval );

// DIAGNOSTIC for STRONG LINK PAIRS
// ==============================================================
extern int *Get_Act_Setval();   // { return act_setval; }
extern vRCP *Get_SL_Pairs();   // { return &sl_pairs;  }
extern bool Is_SLP_Valid();    // { return sl_pairs_valid; }
extern void Invalidate_SLP();  // { sl_pairs_valid = false; }
extern void Set_SLP_setval( int val );
extern bool Set_SLP_Valid(bool b);

extern int Mark_Links( PABOX2 pb );
// ==============================================================

// STRONG LINK CHAIN
// ================================
extern int Get_SL_setval(); // { return curr_SL_setval }
extern int Set_SL_invalid(int val); // 1-9 valid otherwise INVLAID 
extern vRC *Get_SL_Chain_for_setval( int setval );
//extern int Scan_Strong_Links_by_setval( PABOX2 pb, int setval );
extern int Mark_Strong_Link_for_setval( PABOX2 pb, int setval );
extern vRCP* Get_SL_FL_Pairs_for_setval(int setval); // return NULL if not init_SLink();

// =======================================

#endif // #ifndef _SUDO_XCYCLES_HXX_
// eof - Sudo_XCycles.hxx


