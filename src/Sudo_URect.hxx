// Sudo_URect.hxx
#ifndef _SUDO_URECT_HXX_
#define _SUDO_URECT_HXX_

extern int Do_UniqRect_Scan( PABOX2 pb );
extern int Do_Fill_UniqRect( PABOX2 pb );

// for painting
extern int Get_Rect_Link_Cnt(); // { return _s_iSLrect; }
extern int Set_Rect_Link_Cnt(int cnt); // { int i = _s_iSLrect; _s_iSLrect = cnt; return i; }
extern vRC *Get_Rect_Links(); // { return &_s_vSLrect; }

#endif // #ifndef _SUDO_URECT_HXX_
// eof - SUdo_URect.hxx


