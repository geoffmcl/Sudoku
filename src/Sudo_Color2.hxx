// Sudo_Color2.hxx
#ifndef _SUDO_COLOR2_HXX_
#define _SUDO_COLOR2_HXX_

extern int Do_Color_Scan2( PABOX2 pb );
extern int do_color_test2;  // start with value 1-9
extern vRC *Get_Color2_Members(int setval);
extern vRCP *Get_Color2_Pairs(int setval);
extern PROWCOL Get_Color2_RC( int row, int col, int setval );

extern bool Is_Chain_Valid2(); // { return chain_valid2; }
extern vRCP *Get_Color_Chain2(); // { return &color_chain2; }
extern void Set_Chain_Invalid2(); // { chain_valid2 = false; }
extern int Get_Chain_Value2(); // { return chain_value2; }
extern int Do_Fill_Color2(PABOX2 pb);

#endif // #ifndef _SUDO_COLOR2_HXX_
// eof - Sudo_Color2.hxx



