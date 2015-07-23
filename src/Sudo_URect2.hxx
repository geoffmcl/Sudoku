// Sudo_URect2.hxx
#ifndef _SUDO_URECT2_HXX_
#define _SUDO_URECT2_HXX_

extern int Mark_Unique_Rectanges2( PABOX2 pb );
extern int Do_UniqRect2_Scan( PABOX2 pb ); // in process

// for Painting
extern vRCR *Get_UR_Rects2_Ptr(); // { return pur_rects2; }
extern int Get_UR_Cnt(); // { return ur_rect_cnt; }
extern void Set_UR_invalid(); //  { ur_rect_cnt = 0; }
extern void Kill_UR2(); // delete the vector

#endif // #ifndef _SUDO_URECT2_HXX_
// eof - SUdo_URect2.hxx


