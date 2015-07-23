// Sudo_Scraps.hxx
#ifndef _SUDO_SCRAPS_HXX_
#define _SUDO_SCRAPS_HXX_

extern void run_test(PABOX2 pb);
extern vGRID *get_vector_grid();
extern vGRID *Get_Hints(PABOX2 pb);
extern int Test_Solve( PABOX2 pb );
extern int Get_Solution( PABOX2 pb, bool test_unique = false );

extern bool Generate_New( PABOX2 pb, int spots );

#endif // #ifndef _SUDO_SCRAPS_HXX_
// eof - SUdo_Scraps.hxx



