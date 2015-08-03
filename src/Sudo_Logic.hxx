// Sudo_Logic.hxx
#ifndef _SUDO_LOGIC_HXX_
#define _SUDO_LOGIC_HXX_

typedef int (*STRATSERV) (PABOX2);

enum Stages {
    sg_Begin,
    sg_One,
    sg_Two,
    sg_Fill_Singles,
    sg_Fill_Unique,
    sg_Fill_Naked,
    sg_Fill_Hidden,
    sg_Six,
    sg_Seven,
    sg_Eight,
    sg_Fill_Locked,
    sg_Fill_XWing,
    sg_Fill_Simple,
    sg_Fill_Pointers,
    sg_Fill_YWing,
    sg_Fill_Color,
    sg_Fill_Rect,
    sg_Fill_NakedTrip,
    sg_Fill_NakedQuad,
    sg_Fill_HidTrip,
    sg_Fill_XYChains,
    sg_Fill_SLChains,
    sg_Fill_SLPairs,
    sg_Fill_SLPairs3,
    sg_Fill_AIC,
    sg_Fill_XYZWing

};

extern bool Eliminate_on_Square( int crow, int ccol, PABOX2 pb, PSET ps_1, PSET ps_2nd = 0 );
extern bool Eliminate_Unique_In_Row( int crow, int ccol, PABOX2 pb, PSET ps_1, PSET ps_2nd = 0 );

extern int use_elim_on_square; // this service seems SUSPECT - need to check MORE ;=(( MAYBE OK

extern void invalidate_box(PABOX2 pb); // fill with 0!!!
extern int validate_box(PABOX2 pb);
extern char *last_box_error;
extern char *get_last_box_error();

extern int Do_ID_OPTIONS_ONESTEP( HWND hWnd );
extern void Do_ID_OPTIONS_SHOWHINT();

extern bool done_count_warning;

extern void Reset_Candidates( bool clr_flag = true ); // RESET ALL candiates for blank cells

extern int Do_Fill_By_Flags( PABOX2 pb, uint64_t eflg, uint64_t mflg, char *smsg, char *type,
                             uint64_t cflg = 0,
                             bool do_delete = true );

#define ADDSP(a) strcat(a," ")
#define OUTIT(a) if (*a) sprtf("%s\n",a); *a = 0
#define GETBOX(r,c) ((r / 3) * 3 ) + (c / 3)
#define CHKMEM(a) if (!a) { sprtf("ERROR: MEMORY FAILED!\n"); exit(1); }
#define CLRSTG(a)   ((a == -1) ? "I" : (a) ? "B" : "A")
#define VALUEVALID(v) ((v >= 1) && (v <= 9))
#define SAMECELL(r1,c1,r2,c2) ((r1 == r2)&&(c1 == c2))
#define SAMERCCELL(a,b) SAMECELL(a->row,a->col,b->row,b->col)

// DBGSTOP2(row,4,col,5,row2,8,col2,5);
#define DBGSTOP2(rw1,r1,cl1,c1,rw2,r2,cl2,c2) \
    if (((rw1 == (r1 - 1)) && (cl1 == (c1 - 1))) && ((rw2 == (r2 - 1)) && (cl2 == (c2 - 1)))) debug_stop2()

extern int Do_Simple_Scan(PABOX2 pb = 0);
extern int Do_Stage_Two(PABOX2 pb = 0);
extern int Do_Fill_Unique(PABOX2 pb = 0);
extern int Do_XWing_Scan( PABOX2 pb = 0);
extern int Do_Fill_XWing( PABOX2 pb = 0);
extern int Do_Strong_Links( PABOX2 pb );
extern int Do_Unique_Scan(PABOX2 pb );
extern int Do_Pointing_Pairs( PABOX2 pb );
extern int Do_YWing_Scan(PABOX2 pb);
extern int Do_Hidden_Triple_Scan(PABOX2 pb);
extern int Do_Hidden_Pairs(PABOX2 pb);

extern vRC *Get_Strong_Links(); // { return &vStrong_Links; }

extern void Show_Cells_Collected( PABOX2 pb, PRCRCB prcrcb, bool add_color = false );

extern int Mark_Hidden_Triple(PABOX2 pb);

#define Equal_SET Same_SET
extern bool Same_SET(PSET ps1, PSET ps2);

extern const char *stage_to_text(int stage);    // get stage text mainly for debug


#endif // #ifndef _SUDO_LOGIC_HXX_
// Sudo_Logic.hxx


