// Sudo_SET.cxx

#include "Sudoku.hxx"

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
    comparing getting common, nt common, etc, etc, etc,...
    ================================================================================== */

int Get_Common_SET( PSET ps1, PSET pscomm )
{
    int count = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps1->val[i];
        if (!val) continue;
        if (pscomm->val[val - 1]) continue; // already IN common set
        pscomm->val[val - 1] = val;
        count++;
    }
    return count;
}

int Add_SET_to_SET(PSET pssrc, PSET psdst)
{
    int count = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = pssrc->val[i];
        if (!val) continue; // no value here
        if (psdst->val[i]) continue; // dst already has this
        psdst->val[i] = val; // SET VALUE
        count++; // bump count
    }
    return count;
}

int Elim_SET_not_in_SET( PSET pssrc, PSET psdst )
{
    int count = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = psdst->val[i];
        if (!val) continue;
        if (pssrc->val[i]) continue; // source HAS this value
        psdst->val[i] = 0;  // DELETE if NOT IN SOURCE
        count++;
    }

    return count;
}

// Extract from the 'from' SET candidate WITH this flag
// and ADD them to the 'to' SET
// Return COUNT copied
int Set_SET_per_flag(PSET from, PSET to, uint64_t flag )
{
    int i, cnt,val;
    cnt = 0;
    for (i = 0; i < 9; i++) {
        val = from->val[i];
        if (!val) continue; // no candidate
        if ( !( from->flag[i] & flag ) ) continue; // no flag
        to->val[val - 1] = val;
        cnt++;
    }
    return cnt;
}

void AddSet2BufExFlag( char *tb, PSET ps, uint64_t flag )
{
    int i, cnt,val;
    cnt = 0;
    for (i = 0; i < 9; i++) {
        val = ps->val[i];
        if (val) {
            if ( !ps->flag[i] & flag ) {
                sprintf(EndBuf(tb),"%d",val);
                cnt++;
            }
        }
    }
    if (!cnt) {
        strcat(tb, "nul");
    }
}

void Add_Set2Buf( char *tb, PSET ps )
{
    int i, cnt,val;
    cnt = 0;
    for (i = 0; i < 9; i++) {
        val = ps->val[i];
        if (val) {
            sprintf(EndBuf(tb),"%d",val);
            cnt++;
        }
    }
    if (!cnt) {
        strcat(tb, "nul");
    }
}

void Fill_SET( PSET ps )
{
    int i;
    for ( i = 0; i < 9; i++ ) {
        ps->val[i] = i + 1;
    }
}

void Zero_SET( PSET ps )
{
    int i;
    for ( i = 0; i < 9; i++ ) {
        ps->val[i] = 0;
    }
}

// #define Equal_SET Same_SET
bool Same_SET(PSET ps1, PSET ps2)
{
    int i;
    for (i = 0; i < 9; i++) {
        if (ps1->val[i] != ps2->val[i])
            return false;
    }
    return true;
}

/*\
 * Get remaining potential candidates for this cell,
 * returning count, and if a ptr given, the first value.
 * Use Get_Set_Cnt2(PSET,int *[9]) to get all values.
\*/
int Get_Set_Cnt( PSET ps, int *pnv )
{
    int i, cnt, val, nval;
    cnt = 0;
    nval = 0;
    for ( i = 0; i < 9; i++) {
        val = ps->val[i];
        if (val) {
            if (nval == 0)
                nval = val;
            cnt++;
        }
    }
    if (pnv)
        *pnv = nval;    // return first pot value found
    return cnt;
}

// return an array of values - pnv MUST be an array of min 9 ints
int Get_Set_Cnt2( PSET ps, int *pnv )
{
    int i, cnt, val;
    cnt = 0;
    for ( i = 0; i < 9; i++) {
        val = ps->val[i];
        if (val) {
            pnv[cnt++] = val;
        }
    }
    return cnt;
}

// given two PSET, return the COMMON candidates in a PSET, and count.
int Get_Common_SET_Cnt( PSET ps1, PSET ps2, PSET comm )
{
    int cnt = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps1->val[i];
        if (val && ps2->val[i]) {
            cnt++;  // count a SHARED candidate
            comm->val[i] = val;
        }
    }
    return cnt;
}

// Eliminate SET values from a SET
//                   src-unchanged dst-modified
int Elim_SET_in_SET( PSET pssrc, PSET psdst )
{
    int count = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = pssrc->val[i];    // extract a SOURCE
        if (!val) continue;     // no value
        if (psdst->val[val - 1]) {
            psdst->val[val - 1] = 0; // eliminate value
            count++;
        }
    }
    return count;
}

//                        to          from            flag      all?
int Mark_SET_in_SET( PSET psdst, PSET pssrc, uint64_t flag, bool all )
{
    int i, val, count;
    count =0;
    for (i = 0; i < 9; i++) {
        val = pssrc->val[i]; // get a SOURCE
        if (!val) continue;
        if (!psdst->val[i]) continue;
        // do NOT count repeated marks, unless ASKED
        if ( all || !(psdst->flag[i] & flag) ) {
            psdst->flag[i] |= flag;
            count++;
        }
    }
    return count;
}

bool Value_In_SET( PSET ps, int nval )
{
    int i;
    for (i = 0; i < 9; i++) {
        if (ps->val[i] == nval)
            return true;
    }
    return false;
}


bool Value_NOT_in_SET( int val, PSET ps )
{
    return !Value_in_SET(val,ps);
}

int Mark_SET_NOT_in_SET( PSET ps, PSET ps2, int flag )
{
    int i, val, count;
    count =0;
    for (i = 0; i < 9; i++) {
        val = ps->val[i];  // Mark values NOT in SET
        if (val) {
            // got a VALUE
            if (Value_NOT_in_SET( val, ps2 )) {
                ps->flag[i] |= flag;
                count++;
            }
        }
    }
    return count;
}


int Get_Shared_SET( PSET ps1, PSET ps2, PSET comm, bool all )
{
    int cnt = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps1->val[i];
        if (val && ps2->val[i]) {
            if (all)
                cnt++;  // count a SHARED candidate
            else if (!comm->val[i])
                cnt++;  // only count NEW shared candidates
            comm->val[i] = val;
        }
    }
    return cnt;
}




// eof - Sudo_SET.cxx
