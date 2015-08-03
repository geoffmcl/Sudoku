// Sudo_SET.hxx
#ifndef _SUDO_SET_HXX_
#define _SUDO_SET_HXX_
/* ==================================================================================
   Every EMPTY CELL in the Sudoku is represented by a SET

    typedef struct tagASET {
        int val[9];     // value when valid, zero otherwise
        uint64_t flag[9]; // state flag for EVERY potential value
        uint64_t cellflg; // general flag for cell
        time_t tm;
        int uval;       // unique value, if any
    }SET, *PSET;

    which holds the candidate values remaining for that CELL,
    and various FLAGS indicating the 'disposition' of that candidate 1 - 9
    When a candidate is eliminated the int val[9] for that candidate will be ZERO

    The cellflag, tm, and uval have VARIOUS uses - That is they are NOT yet solidly determined
    ==========================================================================================

    This file contains a bunch of services for maniplating the SET... filling, copying, 
    comparing getting common, not common, etc, etc, etc,...
    ================================================================================== */

extern int Get_Common_SET( PSET ps1, PSET pscomm );
extern int Add_SET_to_SET(PSET pssrc, PSET psdst);
extern int Elim_SET_not_in_SET( PSET pssrc, PSET psdst );
extern int Set_SET_per_flag(PSET from, PSET to, uint64_t flag );
extern void AddSet2BufExFlag( char *tb, PSET ps, uint64_t flag );

#define AddSet2Buf Add_Set2Buf
extern void Add_Set2Buf( char *tb, PSET ps );

#define Equal_SET Same_SET
extern bool Same_SET(PSET ps1, PSET ps2);

extern int Get_Common_SET_Cnt( PSET ps1, PSET ps2, PSET comm );

// Eliminate SET values from a SET
//                   src-unchanged dst-modified
extern int Elim_SET_in_SET( PSET pssrc, PSET psdst );
//                        to          from            flag       all?

extern int Mark_SET_in_SET( PSET psdst, PSET pssrc, uint64_t flag, bool all = false );
extern bool Value_In_SET( PSET ps, int nval );
extern bool Value_NOT_in_SET( int val, PSET ps );
extern int Mark_SET_NOT_in_SET( PSET ps, PSET ps2, int flag );
extern int Mark_SET_NOT_in_SET2( PABOX2 pb, PROWCOL prc, PSET ps2, int flag, bool *had_err );

extern void Fill_SET( PSET ps );
extern void Zero_SET( PSET ps );

extern int Get_Shared_SET( PSET ps1, PSET ps2, PSET comm, bool all = true );

extern int Get_Set_Cnt( PSET ps, int *pnv = 0 );
// return an array of values - pnv MUST be an array of min 9 ints
extern int Get_Set_Cnt2( PSET ps, int *pnv = 0 );

#define COPY_SET(d,s) memcpy((void *)d,(const void *)s,sizeof(SET))
// #define COPY_SET(d,s) memmove(d,s,sizeof(SET))
#define ZERO_SET(s) memset(s,0,sizeof(SET))

typedef vector<SET> vSET;   // a vector to HOLD sets

#endif // #ifndef _SUDO_SET_HXX_
// eof - Sudu_SET.hxx

