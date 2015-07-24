// Sudo_Utils.hxx
#ifndef _SUDO_UTILS_HXX_
#define _SUDO_UTILS_HXX_
#include <sys/types.h>
#include <sys/stat.h>

typedef vector<int> vINT;
typedef vINT::iterator vINTi;
extern bool Is_Value_in_vINT( int i, vINT & vi );
typedef vector<size_t> vSIZET;
typedef vSIZET::iterator vSIZETi;
extern bool Is_Value_in_vSIZET( size_t i, vSIZET & vst );
typedef vector<time_t> vTIMET;
typedef vTIMET::iterator vTIMETi;
extern bool Is_Value_in_vTIMET( time_t i, vTIMET & vtt );

extern int is_file_or_directory( char * file ); // 1 = file, 2 = dir, 0 = neither!

extern char *Get_Rect_Stg(PRECT pr);
extern BOOL Got_Comm_Ctrl6();

// Helper - convert value to string
#define G_STRINGIZE(X) G_DO_STRINGIZE(X)
#define G_DO_STRINGIZE(X) #X
#ifdef _MSC_VER
#define COMPILER_STR "Microsoft Visual C++ version " G_STRINGIZE(_MSC_VER)
#else
#define COMPILER_STR "Unknown Compiler"
#endif

extern struct stat *Get_Stat_Buf();

extern char *Get_RC_RC_Stg( PROWCOL prc ); // return A5, B8, etc...
extern void Append_RC_setval_RC_Stg( char *tb, PROWCOL prc, int setval = 0 ); // return 7@A5, 345@B8, etc...
extern char *Get_RC_setval_RC_Stg( PROWCOL prc, int setval = 0 ); // return 1@A5, 368@B8, etc...
extern char *Get_RC_RCB_Stg( PROWCOL prc );
extern char *Get_RC_RCBC_Stg( PROWCOL prc );
extern char *Get_RC_RCBS_Stg( PROWCOL prc ); // add set values

extern char *Get_PB_RC_RCBS_Stg( PABOX2 pb, int row, int col );
extern void Append_RCPAIR_Stg( char *tb, PRCPAIR prcpair, int setval, bool all = false );
extern char *Get_RCPAIR_Stg( PRCPAIR prcpair, int setval, bool all = false );
extern void Append_RC_Stg( char *tb, PROWCOL prc, int setval );
extern char *Get_RC_Stg( PROWCOL prc, int setval = 0 ); // return 1@RCA 137@RCB etc...

extern bool Same_RC_Pair( PROWCOL prc1, PROWCOL prc2 );
extern bool Row_Col_in_RC_Vector( vRC & vrc, int row, int col );
extern bool RC_in_pvrc( vRC *pvrc, int row, int col );

extern const char *stglnk; // = "==";
extern const char *weklnk; // = "--";
extern const char *unklnk; // = "??";
extern const char *bthlnk; // = "!!";

// Check the SETTINGS of an RCPAIR per a setval
extern void Append_RCPAIR_Settings( char *tb, PRCPAIR prcpair, int setval );

typedef int (*FILLSERV) (PABOX2);
typedef vector<FILLSERV> vFSS;
// vFSS vFillServ;
extern void Stack_Fill( FILLSERV fs );  // { vFillServ.push_back(fs); }
extern void Do_Fill_Stack(PABOX2 pb) ;

extern int Count_setval_in_Box( PABOX2 pb, int irow, int icol, int setval );
extern int Count_setval_in_Col( PABOX2 pb, int irow, int icol, int setval );
extern int Count_setval_in_Row( PABOX2 pb, int irow, int icol, int setval );

extern bool Row_in_RC_Vector( vRC & vrc, int row, int col );
extern bool Col_in_RC_Vector( vRC & vrc, int row, int col );

extern bool prc_in_chain(PROWCOL prc, vRC *pchain);
extern bool Int_in_iVector( int tval, vINT *pvi);

extern void Append_RC_Settings( char *tb, PROWCOL prc, int setval = 0 );
extern char *Get_RC_setval_color_Stg( PROWCOL prc, int setval );

extern int RC_Vector_to_RCB( vRC &members, PRCRCB prcrcb, bool clear = true );
extern bool RC_in_RC_Vector( vRC &vrc, PROWCOL prc );

extern uint64_t Get_RC_Strong_Link_Flag( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, int setval,
                                  bool inc_box = true );

extern bool equal_rcp( RCPAIR &rcp1, RCPAIR &rcp2 );
extern bool prcp_in_vrcp( PRCPAIR prcp, vRCP &vrcp );
extern bool prcp_in_pvrcp( PRCPAIR prcp, vRCP *pvrcp );
extern bool Same_RC_Pair( PRCPAIR pp1, PRCPAIR pp2 );

extern char *Get_RCPair_Stg( PRCPAIR prcp, int setval );

extern void Explode( const char *msg, const char *file, const char *func, int line );
#define EXPLODE(m) Explode( m, __FILE__, __FUNCTION__, __LINE__ )

extern int Get_Pairs_of_setval( PABOX2 pb, int setval, vRC *pp, PRCRCB prcrcb, vRCP *pvrcp, bool debug = false );

extern bool Value_in_SET( int val, PSET ps );

extern bool prc_in_pvrcp( PROWCOL prc, vRCP *pp, int count = 0 );

extern void Show_PRCB(PRCRCB prcrcb);

extern int Get_Empty_Cells( PABOX2 pb, vRC &empty, PRCRCB prcrcb, bool clear = true );
extern int Get_Empty_Cells_of_setval( PABOX2 pb, vRC &empty, int setval, PRCRCB prcrcb = 0, bool clear = true );
extern int Get_Empty_Cells_per_cand_count( PABOX2 pb, vRC *pempty, int ccnt, PRCRCB prcrcb = 0, bool clear = true );
extern int Get_Empty_Cells_per_not_cand_count( PABOX2 pb, vRC *pempty, int scnt, PRCRCB prcrcb = 0, bool clear = true );
extern void Clear_PRCRCB( PRCRCB prcrcb );

extern void Show_RC_Vector( vRC & vrc );

extern char *double_to_stg( double d );

#endif // #ifndef _SUDO_UTILS_HXX_
// eof - Sudo_Utils.hxx


