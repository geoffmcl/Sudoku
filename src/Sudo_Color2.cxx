// Sudo_Color2.cxx

#include "Sudoku.hxx"
// Reading MORE
// from : http://www.sudokuonline.us/coloring.php
// Colouring is a technique similar to forcing chains in that it 
// looks for chains of connected cells. But while forcing chains consider 
// cells with only two candidates that are connected by sharing a candidate, 
// colouring considers cells where a particular candidate occurs for only two 
// cells in a unit (row, column or block.)
// see example : color1.jpg
// or color1.png from : http://www.sadmansoftware.com/sudoku/colouring.htm
// (Note: The 1s in brackets {1} mark the cells where 1 appears as a candidate 
// (along with other candidates) - they do not show the cell contents.) This example 
// contains two separate conjugate chains - marked by orange/green and pink/blue. 
// We're only interested in part of the orange/green chain, as colouring can show 
// that r8c4 cannot be a 1, by considering the chain r8c3 => r4c3 => r5c1 => r5c4. 
// The logic goes like this:
// if r8c3 is 1 then  r8c4 cannot be 1 as both cells occupy the same row.
// if r8c3 is not 1 then
// r4c3 must be 1 (because column three must contain a 1 somewhere), so
// r5c1 cannot be 1, as it is in the same block, so
// r5c4 must be 1 (because row five must contain a 1 somewhere), so
// r8c4 cannot be 1.
// So, in either case, we've shown that r8c4 cannot be 1, we've eliminated a candidate.
// The conjugate chain for the above example is r8c3 => r4c3 => r5c1 => r5c4. Since this 
// is a chain of conjugate cells, the links have alternate truth states - indicated by 
// the green and orange shading in the above example, either the green cells must be true, 
// that is, hold a 1, or the orange cells, but not both. Any other cells in the grid that 
// share a unit with both a link having a true state, and a link having a false state, 
// cannot hold the candidate and that candidate can be eliminated. This is how 1 was 
// eliminated for r8c4 above.



// Another attempt at this - new approach - using chaining
// see image : coloring-missed.bmp using examples\extreme-01-2.txt
// The candidate 7 by forming a chain say
// 7@A2 == 7@B2 == 7@B9 == (1) 7@C7 & (2) 7@E5 
// (1) 7@C7 == (3) 7@C4 & (4) 7@H7
// (2) 7@E5 == 7@E8 == 7@I9 == 7@H7 == (1) 7@C7 & (5) 7@H3
// (3) 7@C4 == (6) 7@A5 & (7) 7@I4
// (4) 7@H7 == (8) 7@H3 & (9) 7@I8
// Eliminations 7@A5 7@B2 7@C7 7@E9 7@G3 7@H3 7@I4 7@I8
// per SINGLES CHAINS (RULE 2): Two 7s in a single unit (in Col 3) have the 
// same color in cells G3 H3 - all yellow coloured candidates can be removed
// ====================================================
// #define FOLLOW_EACH_AS_FOUND

// Keep the chains formed for PAINTING
// ===================================
vRCP color_chain2;
int chain_number2;
bool chain_valid2 = false;
int chain_value2;
bool Is_Chain_Valid2() { return chain_valid2; }
vRCP *Get_Color_Chain2() { return &color_chain2; }
void Set_Chain_Invalid2() { chain_valid2 = false; }
int Get_Chain_Value2() { return chain_value2; }
// ====================================


typedef struct tagCLRSTR {
    PABOX2 pb;
    vRC members, marked;
    vRCP rcpairs;
    PROWCOL prcFirst;
    int bgn_off;
    int nxt_off;
    int setval;
    time_t ch_num;
    uint64_t flag;
    PROWCOL prc;
#ifndef FOLLOW_EACH_AS_FOUND
    ROWCOL rcset[9*9];
#endif
} CLRSTR, *PCLRSTR;

#define OUTITCS2(tb) if (add_debug_cs2 && *tb) OUTIT(tb)

static SET clr_set;
static CLRSTR clrstr[9];

int do_color_test2 = 0;  // start with value 1-9
vRC *Get_Color2_Members(int setval)
{
    if (!VALUEVALID(setval))
        return 0;
    if ( !(clr_set.val[setval - 1]) )
        return 0;
    return &clrstr[setval - 1].members;
}
vRCP *Get_Color2_Pairs(int setval)
{
    if (!VALUEVALID(setval))
        return 0;
    if ( !(clr_set.val[setval - 1]) )
        return 0;
    return &clrstr[setval - 1].rcpairs;
}

PROWCOL Get_Color2_RC( int row, int col, int setval )
{
    static ROWCOL rc;
    vRCP *pvrc = Get_Color2_Pairs(setval);
    if (!pvrc) return 0;
    size_t max = pvrc->size();
    size_t i;
    RCPAIR rcpair;
    for (i = 0; i < max; i++) {
        rcpair = pvrc->at(i);
        rc = rcpair.rowcol[0];
        if ((rc.row == row)&&(rc.col == col))
            return &rc;
        rc = rcpair.rowcol[1];
        if ((rc.row == row)&&(rc.col == col))
            return &rc;
    }
    return 0;
}

PROWCOL Get_Color2_RC_members( int row, int col, int setval )
{
    static ROWCOL rc;
    vRC *pvrc = Get_Color2_Members(setval);
    if (!pvrc) return 0;
    size_t max = pvrc->size();
    size_t i;
    for (i = 0; i < max; i++) {
        rc = pvrc->at(i);
        if ((rc.row == row)&&(rc.col == col))
            return &rc;
    }
    return 0;
}


void clear_color_str( PCLRSTR pclrstr )
{
    pclrstr->members.clear();
    pclrstr->rcpairs.clear();
    pclrstr->marked.clear();
    pclrstr->nxt_off = 0;
    pclrstr->bgn_off = 0;
    ZERO_SET(&clr_set);
}

#ifdef FOLLOW_EACH_AS_FOUND

#define DO_BOX_FIRST

int Follow_Color_Chain( PCLRSTR pclrstr )
{
    int count = 0;
    PABOX2 pb = pclrstr->pb;
    int row, col, box, val;
    int setval = pclrstr->setval;
    ROWCOL rc, rc2;
    int row2, col2, box2;
    PSET ps2;
    uint64_t cflg;
    int r, c, rw, cl;
    char *tb = GetNxtBuf();
    uint64_t flag = pclrstr->flag; // get the FLAG
    RCPAIR rcpair;
    // process candidates - take each off the stack
    rc = *pclrstr->prc;
    row = rc.row;
    col = rc.col;
    box = GETBOX(row,col);
    cflg = (rc.set.flag[setval - 1] & cf_XCA) ? cf_XCB : cf_XCA;
    //sprintf(tb,"join %s ", setval, Get_RC_setval_RC_Stg(&rc,setval));
    //sprintf(tb,"join %s ", Get_RC_RCB_Stg(&rc));
#ifdef DO_BOX_FIRST
    r = (row / 3) * 3;
    c = (col / 3) * 3;
    rw = 0;
    if (flag == cl_LBB)
        rw = 3; // skip BOX search
    for (; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row2 = r + rw;
            col2 = c + cl;
            box2 = GETBOX(row2,col2);
            if (Row_Col_in_RC_Vector( pclrstr->members, row2, col2 )) continue;
            val = pb->line[row2].val[col2];
            if (val) continue;
            ps2 = &pb->line[row2].set[col2];
            if ( !( ps2->val[setval - 1] ) ) continue;

            rc2.row = row2;
            rc2.col = col2;
            COPY_SET(&rc2.set,ps2);
            rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
            rc2.set.tm = rc.set.tm;

            pclrstr->members.push_back(rc2);
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pclrstr->rcpairs.push_back(rcpair);

            sprintf(tb,"join %s ", Get_RC_RCB_Stg(&rc));
            sprintf(EndBuf(tb),"B %s ", Get_RC_RCB_Stg(&rc2));
            OUTITCS2(tb);

            pclrstr->flag = cl_LBB; // last link by BOX
            pclrstr->prc = &rc2;
            Follow_Color_Chain( pclrstr );
        }
    }
#endif

    // search for NEXT
    row2 = 0;
    if (flag == cl_LBC)
        row2 = 9; // skip ROW search of this COL
    for (; row2 < 9; row2++) {
        box2 = GETBOX(row2,col);
        if (row2 == row) continue;
        if (Row_Col_in_RC_Vector( pclrstr->members, row2, col )) continue;
        val = pb->line[row2].val[col];
        if (val) continue;
        ps2 = &pb->line[row2].set[col];
        if ( !( ps2->val[setval - 1] ) ) continue;
        if (flag == cl_LBB) {
            if (box2 == box) continue;
        }

        rc2.row = row2;
        rc2.col = col;

        COPY_SET(&rc2.set,ps2);
        rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
        rc2.set.tm = rc.set.tm;

        pclrstr->members.push_back(rc2);
        rcpair.rowcol[0] = rc;
        rcpair.rowcol[1] = rc2;
        pclrstr->rcpairs.push_back(rcpair);

        sprintf(tb,"join %s ", Get_RC_RCB_Stg(&rc));
        sprintf(EndBuf(tb),"C %s ", Get_RC_RCB_Stg(&rc2));
        OUTITCS2(tb);

        pclrstr->flag = cl_LBC; // last link by COL
        pclrstr->prc = &rc2;
        Follow_Color_Chain( pclrstr );
    }
    col2 = 0;
    if (flag == cl_LBR)
        col2 = 9; // skip COL search of this ROW
    for (; col2 < 9; col2++) {
        box2 = GETBOX(row,col2);
        if (col2 == col) continue;
        if (Row_Col_in_RC_Vector( pclrstr->members, row, col2 )) continue;
        val = pb->line[row].val[col2];
        if (val) continue;
        ps2 = &pb->line[row].set[col2];
        if ( !( ps2->val[setval - 1] ) ) continue;
        if (flag == cl_LBB) {
            if (box2 == box) continue;
        }

        rc2.row = row;
        rc2.col = col2;
        COPY_SET(&rc2.set,ps2);
        rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
        rc2.set.tm = rc.set.tm;

        pclrstr->members.push_back(rc2);
        rcpair.rowcol[0] = rc;
        rcpair.rowcol[1] = rc2;
        pclrstr->rcpairs.push_back(rcpair);

        sprintf(tb,"join %s ", Get_RC_RCB_Stg(&rc));
        sprintf(EndBuf(tb),"R %s ", Get_RC_RCB_Stg(&rc2));
        OUTITCS2(tb);

        pclrstr->flag = cl_LBR; // last link by ROW
        pclrstr->prc = &rc2;
        Follow_Color_Chain( pclrstr );
    }
#ifndef DO_BOX_FIRST
    r = (row / 3) * 3;
    c = (col / 3) * 3;
    rw = 0;
    if (flag == cl_LBB)
        rw = 3; // skip BOX search
    for (; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row2 = r + rw;
            col2 = c + cl;
            box2 = GETBOX(row2,col2);
            if (Row_Col_in_RC_Vector( pclrstr->members, row2, col2 )) continue;
            val = pb->line[row2].val[col2];
            if (val) continue;
            ps2 = &pb->line[row2].set[col2];
            if ( !( ps2->val[setval - 1] ) ) continue;

            rc2.row = row2;
            rc2.col = col2;
            COPY_SET(&rc2.set,ps2);
            rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
            rc2.set.tm = rc.set.tm;

            pclrstr->members.push_back(rc2);
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pclrstr->rcpairs.push_back(rcpair);

            sprintf(tb,"join %s ", Get_RC_RCB_Stg(&rc));
            sprintf(EndBuf(tb),"B %s ", Get_RC_RCB_Stg(&rc2));
            OUTITCS2(tb);

            pclrstr->flag = cl_LBB; // last link by BOX
            pclrstr->prc = &rc2;
            Follow_Color_Chain( pclrstr );
        }
    }
#endif // #ifndef DO_BOX_FIRST
    return -1;
}

#else // !#ifdef FOLLOW_EACH_AS_FOUND - ie follow at end

int Follow_Color_Chain( PCLRSTR pclrstr )
{
    int count = 0;
    PABOX2 pb = pclrstr->pb;
    int row, col, val;
    int bgn_off = pclrstr->bgn_off;
    int nxt_off = pclrstr->nxt_off;
    int setval = pclrstr->setval;
    ROWCOL rc, rc2;
    int row2, col2;
    PSET ps2;
    uint64_t cflg;
    int off = nxt_off;
    int r, c, rw, cl;
    RCPAIR rcpair;

    char *tb = GetNxtBuf();
    // process candidates - take each off the stack
    for (; bgn_off < nxt_off; bgn_off++) {
        rc = pclrstr->rcset[bgn_off];
        row = rc.row;
        col = rc.col;
        cflg = (rc.set.flag[setval - 1] & cf_XCA) ? cf_XCB : cf_XCA;
        //sprintf(tb,"join %s ", setval, Get_RC_setval_RC_Stg(&rc,setval));
        sprintf(tb,"join %s ", Get_RC_RCB_Stg(&rc));
        // search for NEXT
        for (row2 = 0; row2 < 9; row2++) {
            if (row2 == row) continue;
            if (Row_Col_in_RC_Vector( pclrstr->members, row2, col )) continue;
            val = pb->line[row2].val[col];
            if (val) continue;
            ps2 = &pb->line[row2].set[col];
            if ( !( ps2->val[setval - 1] ) ) continue;

            rc2.row = row2;
            rc2.col = col;
            COPY_SET(&rc2.set,ps2);
            rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
            rc2.set.tm = rc.set.tm;

            pclrstr->members.push_back(rc2);
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pclrstr->rcpairs.push_back(rcpair);

            pclrstr->rcset[off++] = rc2;
            sprintf(EndBuf(tb),"C %s ", Get_RC_RCB_Stg(&rc2));
        }
        for (col2 = 0; col2 < 9; col2++) {
            if (col2 == col) continue;
            if (Row_Col_in_RC_Vector( pclrstr->members, row, col2 )) continue;
            val = pb->line[row].val[col2];
            if (val) continue;
            ps2 = &pb->line[row].set[col2];
            if ( !( ps2->val[setval - 1] ) ) continue;

            rc2.row = row;
            rc2.col = col2;
            COPY_SET(&rc2.set,ps2);
            rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
            rc2.set.tm = rc.set.tm;

            pclrstr->members.push_back(rc2);
            pclrstr->rcset[off++] = rc2;
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pclrstr->rcpairs.push_back(rcpair);

            sprintf(EndBuf(tb),"R %s ", Get_RC_RCB_Stg(&rc2));
        }
        r = (row / 3) * 3;
        c = (col / 3) * 3;
        for (rw = 0; rw < 3; rw++) {
            for (cl = 0; cl < 3; cl++) {
                row2 = r + rw;
                col2 = c + cl;
                if (Row_Col_in_RC_Vector( pclrstr->members, row2, col2 )) continue;
                val = pb->line[row2].val[col2];
                if (val) continue;
                ps2 = &pb->line[row2].set[col2];
                if ( !( ps2->val[setval - 1] ) ) continue;

                rc2.row = row2;
                rc2.col = col2;
                COPY_SET(&rc2.set,ps2);
                rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
                rc2.set.tm = rc.set.tm;

                pclrstr->members.push_back(rc2);
                rcpair.rowcol[0] = rc;
                rcpair.rowcol[1] = rc2;
                pclrstr->rcpairs.push_back(rcpair);
                pclrstr->rcset[off++] = rc2;
                sprintf(EndBuf(tb),"B %s ", Get_RC_RCB_Stg(&rc2));
            }
        }
        if (off > nxt_off) {
            OUTITCS2(tb);
            pclrstr->bgn_off = nxt_off; // begin from here
            pclrstr->nxt_off = off;     // next
            Follow_Color_Chain( pclrstr );
            off = pclrstr->nxt_off;
        }

    }
    return count;
}
#endif // #ifdef FOLLOW_EACH_AS_FOUND y/n - ie follow at end

int first_pair_not_box = 0;

// ============================================================
// YET ANOTHER TRY!!!
// ============================================================


// find the first STRONG LINKED PAIR for each candidate
int pair_not_in_same_box = 1;   // ROW COL pairs NOT in SAME BOX
int check_rcset = 0;
int check_strong_link2 = 1;
int add_extra_debug2 = 0;

bool In_rcset( PCLRSTR pclrstr, int row, int col, int cnt ) // already in SET TO DO
{
    int i;
    ROWCOL rc;
    for (i = 0; i < cnt; i++) {
        rc = pclrstr->rcset[i];
        if ((rc.row == row) && (rc.col == col))
            return true;
    }
    return false;
}

int Mark_Color_Chain( PCLRSTR pclrstr )
{
    int count = 0;
    PABOX2 pb = pclrstr->pb;
    int row, col, box, val;
    int bgn_off = pclrstr->bgn_off;
    int nxt_off = pclrstr->nxt_off;
    int setval = pclrstr->setval;
    ROWCOL rc, rc2;
    int row2, col2, box2;
    PSET ps2;
    uint64_t cflg;
    int off = nxt_off;
    int r, c, rw, cl, setcnt;
    RCPAIR rcpair;
    PROWCOL prc = pclrstr->prc;
    char *tb = GetNxtBuf();
    bgn_off = off;
    // process candidate
    rc = *prc;      // get cell to follow
    row = rc.row;
    col = rc.col;
    if (RC_in_RC_Vector( pclrstr->marked, &rc )) return 0;
    pclrstr->marked.push_back(rc);
    box = GETBOX(row,col);
    cflg = (rc.set.flag[setval - 1] & cf_XCA) ? cf_XCB : cf_XCA;
    //sprintf(tb,"join %s ", setval, Get_RC_setval_RC_Stg(&rc,setval));
    sprintf(tb,"mark %s %s to ", Get_RC_RCB_Stg(&rc),
        ((cflg & cf_XCA) ? "B" : "A"));
    // search for NEXT
    for (row2 = 0; row2 < 9; row2++) {
        if (row2 == row) continue;
        if (Row_Col_in_RC_Vector( pclrstr->members, row2, col )) continue;
        val = pb->line[row2].val[col];
        if (val) continue;
        ps2 = &pb->line[row2].set[col];
        if ( !( ps2->val[setval - 1] ) ) continue;
        box2 = GETBOX(row2,col);
        if (pair_not_in_same_box && (box2 == box)) continue; // will be marked in BOX
        if (check_rcset && In_rcset( pclrstr, row2, col, off )) continue; // already in SET TO DO
        setcnt = Count_setval_in_Col(pb, row, col, setval );
        if (check_strong_link2 && (setcnt != 2)) continue;

        rc2.row = row2;
        rc2.col = col;
        COPY_SET(&rc2.set,ps2);
        rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
        rc2.set.tm = rc.set.tm;

        //pclrstr->members.push_back(rc2);
        rcpair.rowcol[0] = rc;
        rcpair.rowcol[1] = rc2;
        pclrstr->rcpairs.push_back(rcpair);

        if (setcnt == 2) { // ONLY if STRONG LINK this column
            pclrstr->rcset[off++] = rc2;
        }
        sprintf(EndBuf(tb),"C %s %s ", Get_RC_RCB_Stg(&rc2), ((setcnt == 2) ? "SL" : "WL") );
    }
    for (col2 = 0; col2 < 9; col2++) {
        if (col2 == col) continue;
        if (Row_Col_in_RC_Vector( pclrstr->members, row, col2 )) continue;
        val = pb->line[row].val[col2];
        if (val) continue;
        ps2 = &pb->line[row].set[col2];
        if ( !( ps2->val[setval - 1] ) ) continue;
        box2 = GETBOX(row,col2);
        if (pair_not_in_same_box && (box2 == box)) continue; // will be marked in BOX
        if (check_rcset && In_rcset( pclrstr, row, col2, off )) continue; // already in SET TO DO
        setcnt = Count_setval_in_Row(pb, row, col, setval );
        if (check_strong_link2 && (setcnt != 2)) continue;

        rc2.row = row;
        rc2.col = col2;
        COPY_SET(&rc2.set,ps2);
        rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
        rc2.set.tm = rc.set.tm;

        //pclrstr->members.push_back(rc2);
        pclrstr->rcset[off++] = rc2;
        rcpair.rowcol[0] = rc;
        rcpair.rowcol[1] = rc2;
        pclrstr->rcpairs.push_back(rcpair);

        if (setcnt == 2) { // ONLY if STRONG LINK this row
            pclrstr->rcset[off++] = rc2;
        }
        sprintf(EndBuf(tb),"R %s %s ", Get_RC_RCB_Stg(&rc2), ((setcnt == 2) ? "SL" : "WL") );
    }
    r = (row / 3) * 3;
    c = (col / 3) * 3;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row2 = r + rw;
            col2 = c + cl;
            if (Row_Col_in_RC_Vector( pclrstr->members, row2, col2 )) continue;
            val = pb->line[row2].val[col2];
            if (val) continue;
            ps2 = &pb->line[row2].set[col2];
            if ( !( ps2->val[setval - 1] ) ) continue;
            if (check_rcset && In_rcset( pclrstr, row2, col2, off )) continue; // already in SET TO DO
            setcnt = Count_setval_in_Box(pb, row, col, setval );
            if (check_strong_link2 && (setcnt != 2)) continue;

            rc2.row = row2;
            rc2.col = col2;
            COPY_SET(&rc2.set,ps2);
            rc2.set.flag[setval - 1] |= cflg; // set COLOR opposite
            rc2.set.tm = rc.set.tm;

            //pclrstr->members.push_back(rc2);
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pclrstr->rcpairs.push_back(rcpair);

            if (setcnt == 2) { // ONLY if STRONG LINK this column
                pclrstr->rcset[off++] = rc2;
            }
            sprintf(EndBuf(tb),"B %s %s ", Get_RC_RCB_Stg(&rc2), ((setcnt == 2) ? "SL" : "WL") );
        }
    }
    int cnt = off - bgn_off;
    if (cnt || add_extra_debug2) {
        sprintf(EndBuf(tb),"cnt %d", off - bgn_off);
        OUTITCS2(tb);
    }
    pclrstr->bgn_off = bgn_off; // from here
    pclrstr->nxt_off = off;     // to here
    return count;
}


int Mark_Color_Rule_1( PABOX2 pb, int setval, vRC &empty, vRC &members, vRC &nonmembers,
                            RCRCB &memrcb )
{
    int count = 0;
    // RULE 1 - Any candidate that can be seens by BOTH A and B colors
    // TODO...
    // Search for empty, with this cand exposed to opposite colors by members of the chain
    int lval = setval - 1;
    ROWCOL rc, rc1, rc2;
    size_t k, max3 = empty.size();
    size_t max4, m4, max5, m5;
    vRC *pvrow;
    vRC *pvcol;
    vRC *pvbox;
    uint64_t flg;
    uint64_t flag2;
    char *clrtype;
    char *tb = GetNxtBuf();
    char *tb2 = GetNxtBuf();

    sprintf(tb,"Elim1 ");
    for (k = 0; k < max3; k++) {
        rc = empty[k]; // get an empty cell
        if ( !(rc.set.val[lval]) )
            continue; // FIX20120921 - does NOT have current candidate
        if (RC_in_RC_Vector(members,&rc)) continue; // skip - member of chain
        if (RC_in_RC_Vector(nonmembers,&rc)) continue; // NOT SURE - skip - member of other chains
        // got a cell with setval, NOT part of a chain - is it exposed to
        // two members of this chain with opposing colors?
        pvrow = &memrcb.vrows[rc.row];  // get chain members in SAME ROW as this cell
        pvcol = &memrcb.vcols[rc.col];  // get chain members in SAME COL as this cell
        pvbox = &memrcb.vboxs[GETBOX(rc.row,rc.col)]; // do I need BOX???
        max4 = pvrow->size();
        max5 = pvcol->size();
        flg = (rc.set.flag[lval] & (cf_XCA|cf_XCB));
        clrtype = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
        sprintf(tb2,"Check %s%s ", Get_RC_setval_RC_Stg( &rc, setval ), clrtype);
        for (m4 = 0; m4 < max4; m4++) {
            rc1 = pvrow->at(m4);
            sprintf(EndBuf(tb2),"- %s ", Get_RC_setval_RC_Stg( &rc1, setval ));
            flg = 0;
            flag2 = rc1.set.flag[lval];
            if (flag2 & cf_XCA) {
                flg = cf_XCB;   // exposed to A - set OPPOSITE
            } else if (flag2 & cf_XCB) {
                flg = cf_XCA;   // exposed to B - set OPPOSITE
            }
            // flg contain the OTHER COLOR, so for this cell add opp
            strcat(tb2,((flg & cf_XCA) ? "B " : (flg & cf_XCB) ? "A " : "?!? "));
            if (flg) {
                for (m5 = 0; m5 < max5; m5++) {
                    rc2 = pvcol->at(m5);
                    flag2 = rc2.set.flag[lval];
                    sprintf(EndBuf(tb2),"- %s ", Get_RC_setval_RC_Stg( &rc2, setval ));
                    strcat(tb2,((flag2 & cf_XCA) ? "A " : (flag2 & cf_XCB) ? "B " : "?!? "));
                    if (flag2 & flg) {
                        // YEEK, exposed to BOTH - eliminate
                        if ( !(pb->line[rc.row].set[rc.col].flag[lval] & cf_CCE) ) {
                            pb->line[rc.row].set[rc.col].flag[lval] |= cf_CCE; // Color2 R1 elim
                            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg( &rc, setval ));
                            count++;
                            sprtf("ELIM: Color2 R1 %s\n", Get_RC_setval_RC_Stg( &rc, setval ));
                        }
                        // cf_CC
                        // prc->set.flag[lval] |= cf_CC; // should MARK this
                        pb->line[rc1.row].set[rc1.col].flag[lval] |= cf_CC;
                        pb->line[rc2.row].set[rc2.col].flag[lval] |= cf_CC;
                    }
                }
            }
        }
        OUTITCS2(tb2);
    }
    if (count) OUTITCS2(tb);

    return count;
}

int Mark_Color_Rule_2( PABOX2 pb, int setval, vRC &members, RCRCB &memrcb )
{
    int count = 0;
    int lval = setval - 1;
    int rcnta[9];
    int rcntb[9];
    int ccnta[9];
    int ccntb[9];
    int bcnta[9];
    int bcntb[9];
    uint64_t flag[9];
    uint64_t flg;
    size_t k, max3;
    int i;
    vRC *pvrow;
    vRC *pvcol;
    vRC *pvbox;
    ROWCOL rc;
    char *tb = GetNxtBuf();

    //int cntr[2][3][9];
    //memset(cntr,0,sizeof(cntr));
    for (i = 0; i < 9; i++) { // clear counters
        rcnta[i] = 0; rcntb[i] = 0; ccnta[i] = 0; ccntb[i] = 0; bcnta[i] = 0; bcntb[i] = 0;
        flag[i] = 0; // clear flag
    }

    // RULE 2 - Any candidate in one unit having the SAME color
    for (i = 0; i < 9; i++) {
        pvrow = &memrcb.vrows[i];
        pvcol = &memrcb.vcols[i];
        pvbox = &memrcb.vboxs[i];
        max3 = pvrow->size(); // examine the ROW
        if (max3) {
            sprintf(tb,"Examine ROW %d for %d cells", i + 1, max3);
            OUTITCS2(tb);
        }
        *tb = 0;
        for (k = 0; k < max3; k++) {
            rc = pvrow->at(k);
            if (rc.set.flag[lval] & cf_XCA) {
                rcnta[i]++;
                sprintf(EndBuf(tb),"%s A ", Get_RC_RCB_Stg(&rc));
            } else if (rc.set.flag[lval] & cf_XCB) {
                rcntb[i]++;
                sprintf(EndBuf(tb),"%s B ", Get_RC_RCB_Stg(&rc));
            }
        }
        OUTITCS2(tb);
        max3 = pvcol->size(); 
        if (max3) {
            sprintf(tb,"Examine COL %d for %d cells", i + 1, max3);
            OUTITCS2(tb);
        }
        *tb = 0;
        for (k = 0; k < max3; k++) {
            rc = pvcol->at(k);
            if (rc.set.flag[lval] & cf_XCA) {
                ccnta[i]++;
                sprintf(EndBuf(tb),"%s A ", Get_RC_RCB_Stg(&rc));
            } else if (rc.set.flag[lval] & cf_XCB) {
                ccntb[i]++;
                sprintf(EndBuf(tb),"%s B ", Get_RC_RCB_Stg(&rc));
            }
        }
        max3 = pvbox->size();
        if (max3) {
            sprintf(tb,"Examine BOX %d for %d cells", i + 1, max3);
            OUTITCS2(tb);
        }
        for (k = 0; k < max3; k++) {
            rc = pvbox->at(k);
            if (rc.set.flag[lval] & cf_XCA) {
                bcnta[i]++;
                sprintf(EndBuf(tb),"%s A ", Get_RC_RCB_Stg(&rc));
            } else if (rc.set.flag[lval] & cf_XCB) {
                bcntb[i]++;
                sprintf(EndBuf(tb),"%s B ", Get_RC_RCB_Stg(&rc));
            }
        }
        OUTITCS2(tb);
    }
    // RULE 2 - Any candidate in one unit having the SAME color
    // BUT if 2 or more are found with 2 or more, except if 
    // say same row or col in a box, then assume error, 
    // and DO NOTHING
    int cnt2;
    cnt2 = 0;
    sprintf(tb,"Counts : ");
    for (i = 0; i < 9; i++) {
        if (rcnta[i] >= 2) {
            flag[i] |= cf_XCA;
            sprintf(EndBuf(tb),"R%d %d A ", i+1, rcnta[i]);
        }
        if (rcntb[i] >= 2) {
            flag[i] |= cf_XCB;
            sprintf(EndBuf(tb),"R%d %d B ", i+1, rcntb[i]);
        }
        if (ccnta[i] >= 2) {
            flag[i] |= cf_XCA;
            sprintf(EndBuf(tb),"C%d %d A ", i+1, ccnta[i]);
        }
        if (ccntb[i] >= 2) {
            flag[i] |= cf_XCB;
            sprintf(EndBuf(tb),"C%d %d B ", i+1, ccntb[i]);
        }
        if (bcnta[i] >= 2) {
            flag[i] |= cf_XCA;
            sprintf(EndBuf(tb),"B%d %d A ", i+1, bcnta[i]);
        }
        if (bcntb[i] >= 2) {
            flag[i] |= cf_XCB;
            sprintf(EndBuf(tb),"B%d %d B ", i+1, ccnta[i]);
        }
    }
    OUTITCS2(tb);
        
    flg = 0; // combine FLAGS into ONE
    for (i = 0; i < 9; i++) {
        flg |= flag[i];
    }
    if (flg) {
        max3 = members.size();
        if ((flg & (cf_XCA|cf_XCB)) == (cf_XCA|cf_XCB)) {
            // will consider this an ERROR
            sprtf("Color Rule 2 aborted - errors\n");
        } else {
            // eliminate members
            sprintf(tb,"Elim2 ");
            for (k = 0; k < max3; k++) {
                rc = members[k];
                if (rc.set.flag[lval] & flg) {
                    if ( !(pb->line[rc.row].set[rc.col].flag[lval] & cf_CCE) ) {
                        pb->line[rc.row].set[rc.col].flag[lval] |= cf_CCE; // Color2 R2 elim
                        sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg( &rc, setval ));
                        count++;
                        sprtf("ELIM: Color2 R2 %s\n", Get_RC_setval_RC_Stg( &rc, setval ));
                    }
                }
            }
            OUTITCS2(tb);
        }
    }
    return count;
}

bool RCP_in_RCP_Vector(vRCP &color_chain2, RCPAIR &rcpair_in)
{
    PROWCOL prc1_in, prc2_in;
    PROWCOL prc1, prc2;
    size_t max = color_chain2.size();
    size_t i;
    RCPAIR rcpair;
    prc1_in = &rcpair_in.rowcol[0];
    prc2_in = &rcpair_in.rowcol[1];
    for (i = 0; i < max; i++) {
        rcpair = color_chain2[i];
        prc1 = &rcpair.rowcol[0];
        prc2 = &rcpair.rowcol[1];
        if (SAMERCCELL(prc1_in,prc1) && SAMERCCELL(prc2_in,prc2))
            return true;
        if (SAMERCCELL(prc2_in,prc1) && SAMERCCELL(prc1_in,prc2))
            return true;
    }

    return false;
}

void Mark_Color_Members( PABOX2 pb, vRCP &pairs, int setval )
{
    size_t max = pairs.size();
    size_t i;
    PROWCOL prc1,prc2;
    RCPAIR rcpair;
    int lval = setval - 1;
    for (i = 0; i < max; i++) {
        rcpair = pairs[i];
        prc1 = &rcpair.rowcol[0];
        prc2 = &rcpair.rowcol[1];
        pb->line[prc1->row].set[prc1->col].cellflg |= cl_MCC;   // Member of color club
        pb->line[prc1->row].set[prc1->col].uval = (prc1->set.flag[lval] & cf_XCA) ? 0 : 1;  // get/set COLOR
        pb->line[prc2->row].set[prc2->col].cellflg |= cl_MCC;   // Member of color club
        pb->line[prc2->row].set[prc2->col].uval = (prc2->set.flag[lval] & cf_XCA) ? 0 : 1;  // get/set COLOR
    }
}

int only_one_cs2 = 1;
int Mark_Color2_Elims ( PCLRSTR pclrstr ) // all the information is in this structure
{
    int count = 0;
    PABOX2 pb = pclrstr->pb;
    RCPAIR rcpair;
    int setval = pclrstr->setval;
    size_t i, jj, max, max2, max4, i4;
    ROWCOL rc1, rc2;
    time_t ch_num;
    vTIMET vtt;
    vRC members, nonmembers;
    //RCRCB rcrcb;
    RCRCB memrcb;
    vRC empty;
    PROWCOL prc;
    char *tb = GetNxtBuf();
    //int cnt = Get_Empty_Cells_of_setval( pb, empty, setval, &rcrcb );
    int cnt = Get_Empty_Cells_of_setval( pb, empty, setval );
    int lval = setval - 1;   // logical 0-8 value

    max = pclrstr->rcpairs.size();  // get number of PAIRS
    for (i = 0; i < max; i++) {
        rcpair = pclrstr->rcpairs.at(i);
        rc1 = rcpair.rowcol[0];
        //rc2 = rcpair.rowcol[1];
        ch_num = rc1.set.tm;
        if (Is_Value_in_vTIMET(ch_num,vtt)) continue;
        vtt.push_back(ch_num);
    }
    // OK, have the list of chain number
    max2 = vtt.size();  // get number of CHAINS
    sprintf(tb,"For cand %d got %d conjugate chains, %d pairs, empty %d", setval, (int)max2, (int)max, cnt);
    OUTITCS2(tb);

    // Deal CHAIN by CHAIN
    for (jj = 0; jj < max2; jj++) {
        ch_num = vtt[jj]; // do this chain
        chain_number2 = (int)ch_num;
        chain_valid2 = false;
        chain_value2 = setval;
        members.clear();
        nonmembers.clear();
        color_chain2.clear();
        for (i = 0; i < max; i++) {
            rcpair = pclrstr->rcpairs.at(i);
            rc1 = rcpair.rowcol[0];
            rc2 = rcpair.rowcol[1];
            if (rc1.set.tm != ch_num) {
                if (!RC_in_RC_Vector(nonmembers,&rc1))
                    nonmembers.push_back(rc1);
                if (!RC_in_RC_Vector(nonmembers,&rc1))
                    nonmembers.push_back(rc2);
                continue;
            }
            if (!RC_in_RC_Vector(members,&rc1))
                members.push_back(rc1);
            if (!RC_in_RC_Vector(members,&rc2))
                members.push_back(rc2);
            if (!RCP_in_RCP_Vector(color_chain2, rcpair))
                color_chain2.push_back(rcpair);
        }
        // got list of members for this CHAIN
        max4 = members.size();
        if (max4 < 4) continue; // no interest in small chains
        // got a CHAIN of members... - and nonmembers to skip
        // got candidate set up by ROW, COL and BOX
        // =========================================
        sprintf(tb,"Chain # %d, with %d members, %d non", (int)jj + 1,
            (int) max4, (int)nonmembers.size() );
        OUTITCS2(tb);
        sprintf(tb,"List ");
        for (i4 = 0; i4 < max4; i4++) {
            prc = &members[i4];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc,setval));
        }
        OUTITCS2(tb);


        // get members of the chain into ROW, COL and BOX
        RC_Vector_to_RCB( members, &memrcb );

        count += Mark_Color_Rule_1( pb, setval, empty, members, nonmembers, memrcb );
        if (count) {
            Mark_Color_Members( pb, color_chain2, setval );
            chain_valid2 = true;
        } else {
            count += Mark_Color_Rule_2( pb, setval, members, memrcb );
            if (count) {
                Mark_Color_Members( pb, color_chain2, setval );
                chain_valid2 = true;
            }
        }
        if (count && only_one_cs2)
            break;
    }   // Deal CHAIN by CHAIN for (j = 0; j < max2; j++)

    return count;
}


int Do_Color_Scan2( PABOX2 pb )
{
    int count = 0;
    int row, col, box, val, i, setval;
    int row2, col2, box2;
    ROWCOL rc, rc2;
    PSET ps, ps2;
    int r, c, rw, cl;
    PCLRSTR pclrstr;
    if (do_color_test2) sprtf("Doing Color Scan2 for - test2 = %d\n", do_color_test2);
    for (i = 0; i < 9; i++) {
        pclrstr = &clrstr[i];
        clear_color_str( pclrstr );
    }
    int bgn_off, off, setcnt;
    RCPAIR rcpair;
    char *tb = GetNxtBuf();
    if (add_debug_cs2) sprtf("Do_Color_Scan2: Begin with DEBUG on!\n");
    for (i = 0; i < 9; i++) {
        setval = i + 1;
        if (do_color_test2 && (setval != do_color_test2)) continue; // do for just this value
        pclrstr = &clrstr[i];
        pclrstr->setval = setval; // value being chained
        clr_set.val[i] = setval;  // done this setval
        off = bgn_off = 0; // start accumulation
        pclrstr->pb = pb;
        pclrstr->ch_num = 1;
        pclrstr->bgn_off = 0;
        pclrstr->nxt_off = 0;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = pb->line[row].val[col];
                if (val) continue;
                ps = &pb->line[row].set[col];
                if ( !( ps->val[setval - 1] ) ) continue;
                if (Row_Col_in_RC_Vector( pclrstr->members, row, col )) continue;
                // got FIRST with this value - form CHAIN
                rc.row = row;
                rc.col = col;
                box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.set.flag[setval - 1] |= cf_XCA; // set COLOR A
                rc.set.tm = pclrstr->ch_num;
                pclrstr->prcFirst = &rc;
                sprintf(tb,"Join %s to ",Get_RC_RCB_Stg(&rc));
                pclrstr->members.push_back(rc);
                // Seek PAIRS with STRONG links on this candidate
                for (row2 = 0; row2 < 9; row2++) {
                    if (row2 == row) continue;
                    if (Row_Col_in_RC_Vector( pclrstr->members, row2, col )) continue;
                    val = pb->line[row2].val[col];
                    if (val) continue;
                    ps2 = &pb->line[row2].set[col];
                    if ( !( ps2->val[setval - 1] ) ) continue;
                    box2 = GETBOX(row2,col);
                    if (pair_not_in_same_box && (box2 == box)) continue;
                    setcnt = Count_setval_in_Col(pb, row2, col, setval );
                    if (check_strong_link2 && (setcnt != 2)) continue;

                    rc2.row = row2;
                    rc2.col = col;
                    COPY_SET(&rc2.set,ps2);
                    rc2.set.flag[setval - 1] |= cf_XCB; // set COLOR B
                    rc2.set.tm = pclrstr->ch_num;
                    pclrstr->members.push_back(rc2);
                    rcpair.rowcol[0] = rc;
                    rcpair.rowcol[1] = rc2;
                    pclrstr->rcpairs.push_back(rcpair);
                    sprintf(EndBuf(tb),"C %s ", Get_RC_RCB_Stg(&rc2));
                    if (setcnt == 2) { // ONLY if STRONG LINK this column
                        pclrstr->rcset[off++] = rc2; // Store for LATER onward scanning
                    }
                }
                for (col2 = 0; col2 < 9; col2++) {
                    if (col2 == col) continue;
                    if (Row_Col_in_RC_Vector( pclrstr->members, row, col2 )) continue;
                    val = pb->line[row].val[col2];
                    if (val) continue;
                    ps2 = &pb->line[row].set[col2];
                    if ( !( ps2->val[setval - 1] ) ) continue;
                    box2 = GETBOX(row,col2);
                    if (pair_not_in_same_box && (box2 == box)) continue;
                    setcnt = Count_setval_in_Row(pb, row, col2, setval );
                    if (check_strong_link2 && (setcnt != 2)) continue;

                    rc2.row = row;
                    rc2.col = col2;
                    COPY_SET(&rc2.set,ps2);
                    rc2.set.flag[setval - 1] |= cf_XCB; // set COLOR B
                    rc2.set.tm = pclrstr->ch_num;
                    pclrstr->members.push_back(rc2);
                    rcpair.rowcol[0] = rc;
                    rcpair.rowcol[1] = rc2;
                    pclrstr->rcpairs.push_back(rcpair);
                    sprintf(EndBuf(tb),"R %s ", Get_RC_RCB_Stg(&rc2));

                    if (setcnt == 2) { // ONLY if STRONG LINK this row
                        pclrstr->rcset[off++] = rc2; // store
                    }
                }
                //if (!first_pair_not_box) {
                {
                    r = (row / 3) * 3;
                    c = (col / 3) * 3;
                    for (rw = 0; rw < 3; rw++) {
                        for (cl = 0; cl < 3; cl++) {
                            row2 = r + rw;
                            col2 = c + cl;
                            if (Row_Col_in_RC_Vector( pclrstr->members, row2, col2 )) continue;
                            val = pb->line[row2].val[col2];
                            if (val) continue;
                            ps2 = &pb->line[row2].set[col2];
                            if ( !( ps2->val[setval - 1] ) ) continue;
                            setcnt = Count_setval_in_Box(pb, row2, col2, setval );
                            if (check_strong_link2 && (setcnt != 2)) continue;

                            rc2.row = row2;
                            rc2.col = col2;
                            COPY_SET(&rc2.set,ps2);
                            rc2.set.flag[setval - 1] |= cf_XCB; // set COLOR B
                            rc2.set.tm = pclrstr->ch_num;
                            pclrstr->members.push_back(rc2);
                            rcpair.rowcol[0] = rc;
                            rcpair.rowcol[1] = rc2;
                            pclrstr->rcpairs.push_back(rcpair);

                            sprintf(EndBuf(tb),"B %s ", Get_RC_RCB_Stg(&rc2));
                            if (setcnt == 2) { // ONLY if STRONG LINK this box
                                pclrstr->rcset[off++] = rc2;
                            }
                        }
                    }
                }

                if (off <= bgn_off) {
                    pclrstr->members.pop_back(); // remove last pushed
                    continue;
                }
                sprintf(EndBuf(tb),"cnt %d ", off - bgn_off);
                OUTITCS2(tb);
                pclrstr->nxt_off = off; // store from here upwards
                if (add_extra_debug2) {
                    sprintf(tb,"Process ");
                    for (rw = bgn_off; rw < off; rw++) {
                        rc = pclrstr->rcset[rw];
                        sprintf(EndBuf(tb),"%s ", Get_RC_RCB_Stg(&rc));
                    }
                    OUTITCS2(tb);
                }
                for ( ; bgn_off < off; bgn_off++) {
                    rc = pclrstr->rcset[bgn_off];
                    //if (Row_Col_in_RC_Vector( pclrstr->members, rc.row, rc.col )) continue;
                    pclrstr->prc = &rc;
                    pclrstr->members.push_back(rc); // do NOT find self
                    Mark_Color_Chain( pclrstr );
                    // potentially added more
                    if (pclrstr->nxt_off > off) {
                        // this ADDED new members
                        if (add_extra_debug2) {
                            sprintf(tb,"Plus %d ", pclrstr->nxt_off - off);
                            for (rw = off; rw < pclrstr->nxt_off; rw++) {
                                rc = pclrstr->rcset[rw];
                                sprintf(EndBuf(tb),"%s ", Get_RC_RCB_Stg(&rc));
                            }
                            OUTITCS2(tb);
                        }
                    } else {
                        // maybe not needed - in fact can REMOVE a member for coloring
                        // unless change the paint coloring to use the pairs vector
                        pclrstr->members.pop_back(); // remove last pushed
                    }
                    off = pclrstr->nxt_off;
                }   // for ( ; bgn_off < off; bgn_off++)
                pclrstr->ch_num++;
            }   // for (col = 0; col < 9; col++)
        }   // for (row = 0; row < 9; row++)
        count = (int)pclrstr->members.size();
        sprintf(tb,"For cand %d got %d links, %d chains", setval, count, (int)(pclrstr->ch_num - 1));
        OUTITCS2(tb);
        sprintf(tb,"That is %d linked pairs.", (int) pclrstr->rcpairs.size());
        OUTITCS2(tb);
        // LOOKS GOOD - Only need to use INFO to try to ELIMINATE a candidate
        // That is a candidate that can be 'seen' by 2 of the linked pairs that have opposite color
        // Now to see if it ALSO works on extreme-01-2.txt - LOOKS GOOD - same outline
        // Time to try to find ELIMINATIONS
        count = Mark_Color2_Elims ( pclrstr ); // all the information is in this structure

        if (count && only_one_cs2) {
            break;
        }
    }
    if (add_debug_cs2) sprtf("Do_Color_Scan2: End with DEBUG on!\n");
    if (count) {
        sprtf("S2f: Color2 scan elimin %d. To fill\n", count);
        pb->iStage = sg_Fill_Color;
    } else {
        Set_Chain_Invalid2();
        sprtf("S2f: Color2 scan. No elims. To begin.\n");
        pb->iStage = sg_Begin;
    }

    return count;
}

int Do_Fill_Color2(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_CCE, cf_CC, "CC", "Color", cl_MCC );
    Set_Chain_Invalid2();
    return count;
}

// eof - Sudo_Color2.cxx
