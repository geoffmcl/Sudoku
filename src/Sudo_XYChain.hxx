// Sudo_XYChain.hxx
#ifndef _SUDO_XYCHAIN_HXX_
#define _SUDO_XYCHAIN_HXX_

extern int Mark_XY_Chains( PABOX2 pb );
extern int test_xy_chain;
extern int Do_XY_Chains( PABOX2 pb );
extern int Do_Fill_XYChains(PABOX2 pb);
extern int Do_Clear_XYChains(PABOX2 pb);
// for painting
extern vRC *Get_XY_Chain();
extern int Get_XY_Bgn_Val(); // { return xystr.bgn_setval; }
extern void Set_XYChain_Invalid(); // { xy_chain_elims = 0; }

// for painting 2
extern vRC *Get_XY_Act_Chain(); // { return &act_chain; }
extern void Set_XY_Elim_Count(int cnt); // { xy_chain_elims = cnt; }
extern void Set_XY_setval(int setval); // { xystr.bgn_setval = setval; }
extern int Get_XY_setval(); // { return xystr.bgn_setval; }
extern int Get_XY_Elim_Count(); // { return xy_chain_elims; }

#endif // #ifndef _SUDO_XYCHAIN_HXX_
// eof - Sudo_XYChain.hxx



