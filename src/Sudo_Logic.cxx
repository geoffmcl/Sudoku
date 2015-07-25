// Sudo_Logic.cxx
// Where the analysis happens ;=))

#include "Sudoku.hxx"
#include <map>
#include <utility>
#include <assert.h>

#ifndef MEOL
#define MEOL "\r\n"
#endif

bool done_count_warning = false;

int use_elim_on_square = 0; // this service seems SUSPECT - need to check MORE ;=(( MAYBE OK

// forward refs
int Just_in_Rows( PABOX2 pb, int mrow, int mcol, int setval, vRC &pairs );
int Just_in_Cols( PABOX2 pb, int mrow, int mcol, int setval, vRC &pairs );
int Just_in_Box( PABOX2 pb, int mrow, int mcol, int setval, vRC &pairs );

void invalidate_box(PABOX2 pb)
{
    memset(pb,0,sizeof(ABOX2));
    int row, col;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            Fill_SET( &pb->line[row].set[col] );
        }
    }
}

char *last_box_error = 0;
void set_last_box_error( char *err )
{
    size_t len;
    if (err && ((len = strlen(err)) != 0)) {
        if (last_box_error)
            free(last_box_error);
        last_box_error = (char *)malloc( len + 2 );
        strcpy(last_box_error,err);
        sprtf("%s\n",err);
    }
}

char *get_last_box_error()
{
    if (last_box_error)
        return last_box_error;
    return "no error";
}

// #define INVALID_VAL -1

int validate_box(PABOX2 pb)
{
    char *tb = GetNxtBuf();
    int row, col, val, count, i;
    bool error = false;
    count = 0;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) {
                if (VALUEVALID(val))
                    count++;
                else {
                    //if (val != INVALID_VAL) {
                        sprintf(tb,"VAL ERR: R %d C %d INVALID [%d]",
                            (row + 1), (col + 1), val);
                        set_last_box_error(tb);
                    //}
                    return 1;   // contains an invalid value = NO PAINT
                }
            }
        }
    }

    SET set;
    // do a row check
    for (row = 0; row < 9; row++) {
        Zero_SET(&set); // blank set for this ROW
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) {
                if ( set.val[val - 1] ) {
                    sprintf(tb,"ROW ERR: R %d C %d DUPED [%d]!",
                        (row + 1), (col + 1), val);
                    for(i = 0; i < col; i++) {
                        if (pb->line[row].val[i] == val) {
                            sprintf(EndBuf(tb), " See column %d", (i + 1));
                            break;
                        }
                    }
                    set_last_box_error(tb);
                    return 2; // duplicate value in a ROW
                }
                set.val[val - 1] = val;
            }
        }
    }
    // do a column check
    for (col = 0; col < 9; col++) {
        Zero_SET(&set); // blank set for this COLUMN
        for (row = 0; row < 9; row++) {
            val = pb->line[row].val[col];
            if (val) {
                if ( set.val[val - 1] ) {
                    sprintf(tb,"COL ERR: R %d C %d DUPED [%d]!",
                        (row + 1), (col + 1), val);
                    for(i = 0; i < row; i++) {
                        if (pb->line[i].val[col] == val) {
                            sprintf(EndBuf(tb), " See r %d", (i + 1));
                            break;
                        }
                    }
                    set_last_box_error(tb);
                    return 3; // duplicate value in a COLUMN
                }
                set.val[val - 1] = val;
            }
        }
    }
    // Do a BOX SQUARE check
    int r, c, rw, cl;
    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++) {
            Zero_SET(&set); // blank set for this SQUARE
            for (rw = 0; rw < 3; rw++) {
                for (cl = 0; cl < 3; cl++) {
                    row = ((r * 3) + rw);
                    col = ((c * 3) + cl);
                    if (!VALIDRC(row,col)) {
                        sprintf(tb,"Int ERR in R %d C %d!\n", row + 1, col + 1);
                        sprtf("%s\n",tb);
                        return 1;
                    }
                    val = pb->line[row].val[col];
                    if (val) {
                        if ( set.val[val - 1] ) {
                            int rw2, cl2;
                            sprintf(tb,"BOX ERR: R %d C %d DUPED [%d]!",
                                (row + 1), (col + 1), val);
                            // back up and find the duplicae
                            for (rw2 = 0; rw2 < rw; rw2++) {
                                for (cl2 = 0; cl2 < cl; cl2++) {
                                    row = ((r * 3) + rw2);
                                    col = ((c * 3) + cl2);
                                    if (pb->line[row].val[col] == val) {
                                        sprintf(EndBuf(tb), " See r%d c%d", (row + 1), (col + 1));
                                        error = true;
                                        break;
                                    }
                                }
                                if (error)
                                    break;
                            }
                            set_last_box_error(tb);
                            return 4; // duplicate value in a SQUARE
                        }
                        set.val[val - 1] = val;
                    }
                }
            }
        }
    }

    // NO WARNING if ZERO - just a NEW box
    if (count && (count < g_iMinGiven)) { // was 18, now 17 for unique solution
        if (!done_count_warning) {
            done_count_warning = true;
            sprintf(tb,"WARNING: Box has %d cells. Is error?", count);
            set_last_box_error(tb);
            //return 5;   // contains too few value cells - is this really an ERROR???
        }
    }

    return 0;   // box looks GOOD
}


// try to code for in this square of nine, if there are 3 blanks, and 
// the options for the 3 are say 489, 89, 89, then there is ONLY one 
// place for the '4' by elimination...
// Can do better - Rule is get potential values for each cell in a square
// Compare them, and if ONE value ONLY fits in ONE place, then
// we have an ONLY ONLY value

bool elim_on_sq_debug = false;

bool Eliminate_on_Square( int crow, int ccol, PABOX2 pb, PSET ps_1, PSET ps_2nd )
{
    bool bret = false;
    if (!VALIDRC(crow,ccol))
        return bret;
    int r = (crow / 3) * 3;
    int c = (ccol / 3) * 3;
    int row, col, val;
    int rr, cc, cnt, i, fnd;
    ROWCOL rcset[9];    // up to 9 ROWCOL sets
    PROWCOL prc;
    char *tb = GetNxtBuf();
    PROWCOL prc2;
    int j, k, l, val2;
    SET set;
    bool bad = false;
    cnt = 0;
    if (elim_on_sq_debug) sprtf("Checking cell row %d col %d\n", (crow + 1), (ccol + 1));
    //if ((crow == 2) && (ccol == 4))
    //    fnd = 0;
    fnd = 0;    // would NOT be doing this if the curr cell, crow,ccol had a value,
    // but will check that again here to not waste time
    for (rr = 0; rr < 3; rr++) {
        for (cc = 0; cc < 3; cc++) {
            row = rr + r;
            col = cc + c;
            val = pb->line[row].val[col];
            if (!val) {
                prc = &rcset[cnt++];
                prc->row = row;
                prc->col = col;
                Fill_SET( &prc->set );
                if ((crow == row) && (ccol == col))
                    fnd = 1;
            }
        }
    }
    if (!fnd) {
        sprtf("WARNING: Current cell row %d col %d NOT empty!\n", (crow + 1), (ccol + 1));
        return bret; // SHOULD NEVER HAPPEN
    }
    // Eliminate the easy ones
    for (i = 0; i < cnt; i++) {
        prc = &rcset[i];
        Eliminate_Same_Col( pb, prc->col, &prc->set );
        Eliminate_Same_Row( pb, prc->row, &prc->set );
        Eliminate_Same_Box( prc->row, prc->col, pb, &prc->set );
        //prc->cnt = Get_Set_Cnt( &prc->set );
        sprintf(tb,"%d: row %d col %d has %d vals ", i+1, prc->row + 1, prc->col + 1,
            Get_Set_Cnt( &prc->set ) );
        Add_Set2Buf(tb, &prc->set );
        if (( prc->row == crow ) && ( prc->col == ccol ))
            strcat(tb, " THIS");
        if (elim_on_sq_debug) sprtf("%s\n",tb);
    }

    // Of each eliminated set, seek one that has a unique val
    // like 489 89 89, 389 89 89, and more complicated
    // 61249 24 14 1245 247 1479 12459 - the 6 is ALONE
    // 5349 468 38 46 349 389 - the 5 is ALONE
    // SO for each value in a SET, just check if it exists in any other set
    Zero_SET(&set);
    for (i = 0; i < cnt; i++) {
        prc = &rcset[i];
        // now ONLY really interested in checking for THIS specific cell, in this square
        if ((prc->row != crow) || (prc->col != ccol))
            continue;   // get to the CELL of interest
        fnd = 0;
        sprintf(tb,"THIS: row %d col %d has %d vals ", prc->row + 1, prc->col + 1, Get_Set_Cnt( &prc->set ) );
        Add_Set2Buf(tb, &prc->set );
        for ( j = 0; j < 9; j++ ) {
            val = prc->set.val[j];
            if (val) {
                // Check if it is an any other cell in the square, other than itself...
                sprintf(EndBuf(tb)," chk [%d] ",val);
                fnd = 0;
                for ( k = 0; k < cnt; k++ ) {
                    if ( i == k )
                        continue; // skip self
                    prc2 = &rcset[k];
                    sprintf(EndBuf(tb), " set %d: ", (k + 1));
                    Add_Set2Buf(tb, &prc2->set );
                    for ( l = 0; l < 9; l++ ) {
                        val2 = prc2->set.val[l];
                        if (val2 == val) {
                            // found in another
                            fnd = 1;
                            sprintf(EndBuf(tb)," Fnd");
                            break;
                        }
                    }
                    if (fnd)
                        break;
                    strcat(tb," NF");
                }
                if (!fnd) {
                    // What oh, this value NOT in any OTHER
                    if (set.val[val-1])
                        bad = true; // found it a second time HOW COME
                    set.val[val -1] = val; // store the UNIQUE entry
                    sprintf(EndBuf(tb)," NF %d", val);
                }
            }
        }
        if (elim_on_sq_debug) sprtf("%s\n",tb);
        if (bad)
            break;
    }
    if (bad) {
        return bret;
    }
    int nval;
    i = Get_Set_Cnt( &set, &nval );
    sprintf(tb,"Unique cnt %d - ", i);
    Add_Set2Buf(tb, &set );
    if (elim_on_sq_debug) sprtf("%s\n",tb);
    if ( i == 1 ) {
        for ( i = 0; i < 9; i++ ) {
            val = ps_1->val[i];
            if (val != nval)
                ps_1->val[i] = 0;
            if (ps_2nd) {
                val = ps_2nd->val[i];
                if (val != nval)
                    ps_2nd->val[i] = 0;
            }
        }
        bret = true;
    }
    return bret;
}

int Elim_All( int row, int col, PABOX2 pb, PSET ps )
{
    int cnt = 0;
    cnt += Eliminate_Same_Col( pb, col, ps );
    cnt += Eliminate_Same_Row( pb, row, ps );
    cnt += Eliminate_Same_Box( row, col, pb, ps );
    if (Eliminate_on_Square( row, col, pb, ps ))
        cnt++;
    cnt = Get_Set_Cnt(ps);
    return cnt;
}

bool Eliminate_Unique_In_Row( int crow, int ccol, PABOX2 pb, PSET ps_1, PSET ps_2nd )
{
    bool bret = false;
    int col2, val, cnt, i2;
    ROWCOL rcset[9];    // up to 9 ROWCOL sets
    PROWCOL prc;
    PSET ps;
    PROWCOL prc_this = 0;
    bool dbg = false;
    char *tb = GetNxtBuf();
    if ((crow == 3) && (ccol == 5))
        dbg = true;
    cnt = 0;
    for (col2 = 0; col2 < 9; col2++) {
        val = pb->line[crow].val[col2];    // get the value, if any
        if (!val) {
            prc = &rcset[cnt++];
            prc->row = crow;
            prc->col = col2;
            ps = &prc->set;
            Fill_SET(ps);
            Elim_All( crow, col2, pb, ps );
            //prc->cnt = Elim_All( crow, col2, pb, ps );
            if (col2 == ccol) {
                prc_this = prc;
                i2 = cnt - 1;
            }
        }
    }
    if (prc_this) {
        int i, j, k, fnd, val2;
        SET set;
        PSET ps2;
        Zero_SET(&set);
        ps = &prc_this->set;
        for (i = 0; i < 9; i++) {
            val = ps->val[i];
            if (val) {
                // see if this val is unique
                fnd = 0;
                for (j = 0; j < cnt; j++) {
                    if (j == i2) continue; // avoid SELF
                    prc = &rcset[j];
                    ps2 = &prc->set;
                    for (k = 0; k < 9; k++) {
                        val2 = ps2->val[k];
                        if (val2 == val) {
                            fnd = 1;
                            break;
                        }
                    }
                    if (fnd)
                        break;
                }
                if (!fnd) {
                    // store potentially UNIQUE value in all the eliminated ROW sets
                    set.val[val - 1] = val;
                }
            }
        }
        cnt = Get_Set_Cnt(&set,&val2);
        if (cnt == 1) {
            // have SINGLE value for this Row/Col
            for ( i = 0; i < 9; i++ ) {
                val = ps_1->val[i];
                if (val != val2)
                    ps_1->val[i] = 0;
                if (ps_2nd) {
                    val = ps_2nd->val[i];
                    if (val != val2)
                        ps_2nd->val[i] = 0;
                }
            }
            bret = true;
        }
    }
    return bret;
}

// Basic - Mark in SAME Row, Col, Box
int Mark_Same_Col( PABOX2 pb, int col, PSET ps, int flag )
{
    int row2, val, cnt;
    cnt = 0;
    for (row2 = 0; row2 < 9; row2++) {
        val = pb->line[row2].val[col];    // get the value, if any
        if (val) {
            if ( ps->val[val -1] ) {
                //ps->val[val - 1] = 0;   // remove value found in vert COLUMN search
                if ( !(ps->flag[val - 1] & flag) ) {
                    ps->flag[val - 1] |= flag;   // MARK value found in vert COLUMN search
                    cnt++;
                }
            }
        }
    }
    return cnt;
}

int Mark_Same_Row( PABOX2 pb, int row, PSET ps, int flag )
{
    int col2, val, cnt;
    cnt = 0;
    for (col2 = 0; col2 < 9; col2++) {
        val = pb->line[row].val[col2];    // get the value, if any
        if (val) {
            if ( ps->val[val -1] ) {
                //ps->val[val - 1] = 0;   // remove value found in vert COLUMN search
                if ( !(ps->flag[val - 1] & flag) ) {
                    ps->flag[val - 1] |= flag;   // MARK value found in vert COLUMN search
                    cnt++;
                }
            }
        }
    }
    return cnt;
}

int Mark_Same_Box( int crow, int ccol, PABOX2 pb, PSET ps, int flag )
{
    int r = crow / 3;
    int c = ccol / 3;
    int rw, cl, val, cnt;
    r *= 3;
    c *= 3;
    cnt = 0;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            val = pb->line[r + rw].val[c + cl];    // get the value, if any
            if (val) {
                if (ps->val[val - 1] ) {
                    if ( !(ps->flag[val - 1] & flag) ) {
                        ps->flag[val - 1] |= flag;   // MARK value found in vert COLUMN search
                        cnt++;
                    }
                }
            }
        }
    }
    return cnt;
}

// Fixed ROW, scan each column
bool Unique_In_Row( PABOX2 pb, int crow, int ccol, int nval )
{
    int col, val;
    PSET ps;
    for (col = 0; col < 9; col++) {
        if (col == ccol) continue; // skip SELF
        val = pb->line[crow].val[col];
        if (val) continue; // skip if has value
        ps = &pb->line[crow].set[col];
        if ( Value_In_SET( ps, nval ) )
            return false; // found in another COL
    }
    return true;
}

// Fixed COL, scan each ROW
bool Unique_In_Col( PABOX2 pb, int crow, int ccol, int nval )
{
    int row, val;
    PSET ps;
    for (row = 0; row < 9; row++) {
        if (row == crow) continue; // skip SELF
        val = pb->line[row].val[ccol];
        if (val) continue; // skip if has value
        ps = &pb->line[row].set[ccol]; // get SET pointer
        if ( Value_In_SET( ps, nval ) )
            return false; // found in another ROW
    }
    return true;    // NOT found in any other ROW
}

// 20120806 - Fixed BUG in this service - was using ccol instead of compued col !!!
bool Unique_In_Box( PABOX2 pb, int crow, int ccol, int nval )
{
    int r = (crow / 3) * 3;
    int c = (ccol / 3) * 3;
    int row, col, rw, cl, val;
    PSET ps;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((crow == row) && (ccol == col)) continue; // skip SELF
            val = pb->line[row].val[col];
            if (val) continue; // skip cells with VALUE already
            ps = &pb->line[row].set[col]; // get the SET of candidates
            if ( Value_In_SET( ps, nval ) )
                return false;   // FOUND, in OTHER THAN SELF == NOT UNIQUE in BOX!
        }
    }
    return true; // candidate nval NOT found in other than SELF == UNIQUE in BOX
}

// Where value in other rows, column eliminate possibilities
//   1 2 3 4 5 6 7 8 9
// A  | | | | | | | |  1
// B  | | | | | | | |  2
// C  | | | | | | | |  3
// D  | | | | | | | |  4
// E  | | | | | | | |  5
// F  | | | | | | | |  6
// G  | | | | | | | |  7
// H  | | | | | | | |  8
// I  | | | | | | | |  9

bool Same_Pair( PAIR *p1, PAIR *p2 )
{
    if (memcmp(p1,p2,sizeof(PAIR)) == 0)
        return true;

    int val11, val12, val21, val22, tmp;
    val11 = p1->val1;
    val12 = p1->val2;
    // ensure least value first
    if (val12 < val11) {
        tmp = val11;
        val11 = val12;
        val12 = tmp;
    }
    val21 = p2->val1;
    val22 = p2->val2;
    if (val22 < val21) {
        tmp = val21;
        val21 = val22;
        val22 = tmp;
    }

    if ((val11 == val21)&&
        (val12 == val22))
    {
        // EEEK, have same pair of values - check are NOT SAME cells
        if (SAMECELL(p1->row1,p1->col1,p2->row1,p2->col1)&&
            SAMECELL(p1->row2,p1->col2,p2->row2,p2->col2))
            return true;

        if (SAMECELL(p1->row2,p1->col2,p2->row1,p2->col1)&&
            SAMECELL(p1->row2,p1->col2,p2->row1,p2->col1))
            return true;
    }

    return false;
}


bool Pair_Not_In_Vector( PAIR &p, vPAIR &vp )
{
    vPAIRi ii;
    PAIR pair;
    for (ii = vp.begin(); ii != vp.end(); ii++) {
        pair = *ii;
        if (Same_Pair(&p,&pair))
            return false;   // IT IS IN VECTOR ALREADY
    }
    return true;
}

int Find_Same_Pair( PABOX2 pb, int cval1, int crow1, int ccol1, int cval2, int crow2, int ccol2,
        vPAIR & vp)
{
    int row, col, val, count;
    int row2, col2, val2;
    count = 0;
    PAIR pair;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            if (SAMECELL(row,col,crow1,ccol1)) continue;
            if (SAMECELL(row,col,crow2,ccol2)) continue;
            val = pb->line[row].val[col];
            if (val == cval1) {
                // seek cval2 in this ROW or COLUMN
                // in ROW
                for (col2 = 0; col2 < 9; col2++) {
                    val2 = pb->line[row].val[col2]; // get ROW value
                    if (val2 == cval2) {
                        // FOUND - put lowest column first
                        if (col < col2) {
                            pair.row1 = row;
                            pair.col1 = col;
                            pair.val1 = val;
                            pair.row2 = row;
                            pair.col2 = col2;
                            pair.val2 = val2;
                        } else {
                            pair.row1 = row;
                            pair.col1 = col2;
                            pair.val1 = val2;
                            pair.row2 = row;
                            pair.col2 = col;
                            pair.val2 = val;
                        }
                        if (Pair_Not_In_Vector( pair, vp )) {
                            vp.push_back(pair);
                            count++;
                        }
                    }
                }
                // in COLUMN
                for (row2 = 0; row2 < 9; row2++) {
                    val2 = pb->line[row2].val[col]; // get COLUMN value
                    if (val2 == cval2) {
                        // FOUND put lowest row first just for easy compare
                        if (row < row2) {
                            pair.row1 = row;
                            pair.col1 = col;
                            pair.val1 = val;
                            pair.row2 = row2;
                            pair.col2 = col;
                            pair.val2 = val2;
                        } else {
                            pair.row1 = row2;
                            pair.col1 = col;
                            pair.val1 = val2;
                            pair.row2 = row;
                            pair.col2 = col;
                            pair.val2 = val;
                        }
                        if (Pair_Not_In_Vector( pair, vp )) {
                            vp.push_back(pair);
                            count++;
                        }
                    }
                }
            } else if (val == cval2) {
                // seek cval1 in this ROW or COLUMN
                // in ROW
                for (col2 = 0; col2 < 9; col2++) {
                    val2 = pb->line[row].val[col2]; // get ROW value
                    if (val2 == cval1) {
                        // FOUND
                        if (col < col2) {
                            pair.row1 = row;
                            pair.col1 = col;
                            pair.val1 = val2;
                            pair.row2 = row;
                            pair.col2 = col2;
                            pair.val2 = val;
                        } else {
                            pair.row1 = row;
                            pair.col1 = col2;
                            pair.val1 = val;
                            pair.row2 = row;
                            pair.col2 = col;
                            pair.val2 = val2;
                        }
                        if (Pair_Not_In_Vector( pair, vp )) {
                            vp.push_back(pair);
                            count++;
                        }
                    }
                }
                // in COLUMN
                for (row2 = 0; row2 < 9; row2++) {
                    val2 = pb->line[row2].val[col]; // get COLUMN value
                    if (val2 == cval1) {
                        // FOUND
                        if (row < row2) {
                            pair.row1 = row;
                            pair.col1 = col;
                            pair.val1 = val2;
                            pair.row2 = row2;
                            pair.col2 = col;
                            pair.val2 = val;
                        } else {
                            pair.row1 = row2;
                            pair.col1 = col;
                            pair.val1 = val;
                            pair.row2 = row;
                            pair.col2 = col;
                            pair.val2 = val2;
                        }
                        if (Pair_Not_In_Vector( pair, vp )) {
                            vp.push_back(pair);
                            count++;
                        }
                    }
                }
            }
        }
    }
    return count;
}

typedef map<int,vPAIR *> mPAIR;
typedef mPAIR::iterator mPAIRi;

int  Sort_Hidden_Pairs( PABOX2 pb, vPAIR & vp )
{
    vPAIRi ii;
    PAIR pair;
    int cnt, cval;
    int val1, val2, tmp;
    int count = 0;
    cnt = 0;
    vPAIR *pvp;
    mPAIR mMap;
    int prev = add_list_out(0);
    char *tb = GetNxtBuf();
    sprintf(tb,"S2d: Map LIST ALL %d pairs", (int)vp.size());
    sprtf("%s\n",tb);
    for (ii = vp.begin(); ii != vp.end(); ii++) {
        cnt++;
        pair = *ii;
        val1 = pair.val1;
        val2 = pair.val2;
        if (val2 < val1) {
            tmp = val1;
            val1 = val2;
            val2 = tmp;
        }
        cval = (val1 * 10) + val2;
        if (!mMap[cval])
            mMap[cval] = new vPAIR;
        pvp = mMap[cval];
        pvp->push_back(pair);
        // FULL LIST OF ALL PAIRS
        sprintf(tb,"%3d: pair %d%d - r %d c %d & r %d c %d - %s (%d)", cnt,
            val1, val2,
            pair.row1 + 1,
            pair.col1 + 1,
            pair.row2 + 1,
            pair.col2 + 1,
            ((pair.col1 == pair.col2) ? "COLUMN" : "ROW"),
            cval );
        sprtf("%s\n",tb);
    }
    sprtf("S2d: Got map of %d pairs\n", mMap.size());
    cnt = 0;
    mPAIRi mi;
    for ( mi = mMap.begin(); mi != mMap.end(); mi++ ) {
        cnt++;
        cval = (*mi).first;
        pvp = (*mi).second;
        val1 = cval / 10;
        val2 = cval % 10;
        sprintf(tb,"%3d: key %d+%d has %d pairs", cnt, val1, val2, (int)pvp->size());
        sprtf("%s\n",tb);
    }
    // Delete element of NO interest
    bool haddel = true;
    while (haddel) {
        haddel = false;
        for ( mi = mMap.begin(); mi != mMap.end(); mi++ ) {
            cnt++;
            cval = (*mi).first;
            pvp = (*mi).second;
            if (pvp->size() < 3) {
                pvp->clear();
                delete pvp;
                mMap.erase(mi);
                haddel = true;
                break;
            }
        }
    }
    sprtf("S2d: After del LT 3, got map of %d pairs\n", mMap.size());
    cnt = 0;
    haddel = true;
    int cnt2;
    PSET ps, ps2;
    while(haddel) {
        haddel = false;
        for ( mi = mMap.begin(); mi != mMap.end(); mi++ ) {
            cnt++;
            cval = (*mi).first;
            pvp = (*mi).second;
            val1 = cval / 10;
            val2 = cval % 10;
            if (!VALUEVALID(val1) || !VALUEVALID(val2)) {
                sprintf(tb,"WARNING: %3d: key %d+%d has %d pairs - VALUE INVALID!", cnt, val1, val2, (int)pvp->size());
                sprtf("%s\n",tb);
                continue;
            }
            // sprintf(tb,"%3d: key %d+%d has %d pairs", cnt, val1, val2, pvp->size());
            SET set;
            Zero_SET(&set);
            set.val[val1 - 1] = val1;
            set.val[val2 - 1] = val2;
            ps2 = &set; // these are the value to eliminate
            bool dodel = false;
            vINT cols;
            vINT rows;
            // get ROWS and COLUMNS of the pairs
            for (ii = pvp->begin(); ii != pvp->end(); ii++) {
                pair = *ii;
                if (pair.row1 == pair.row2) {
                    // share a ROW
                    rows.push_back(pair.row1);
                } else if (pair.col1 == pair.col2) {
                    // share a COLUMN
                    cols.push_back(pair.col1);
                }
            }
            // Got a set of ROWS and COLS where this PAIR of values can NOT be...
            // Now check each BOX to see if just either -
            // (a) 2 ROWS and 1 COLUMN, or (b) 2 COLUMNS and 1 ROW are excluded,
            // which would leave 2 cells in that BOX where this PAIR can be ELIMINATED
            int box = 0;
            int r, c, rw, cl;
            // Process BOX by BOX
            for (r = 0; r < 3; r++) {
                for (c = 0; c < 3; c++) {
                    // processing a BOX
                    box++;
                    int row, col, val;
                    vRC vrc;
                    ROWCOL rc, rc2;
                    vrc.clear();
                    // Now each CELL in the BOX
                    for (rw = 0; rw < 3; rw++) {
                        for (cl = 0; cl < 3; cl++) {
                            row = (r * 3) + rw;
                            col = (c * 3) + cl;
                            val = pb->line[row].val[col];
                            if (!val) {
                                ps = &pb->line[row].set[col];
                                if (!Is_Value_in_vINT( row, rows )&&
                                    !Is_Value_in_vINT( col, cols )&&
                                    Value_in_SET(val1,ps) &&
                                    Value_in_SET(val2,ps) )
                                {
                                    rc.row = row;
                                    rc.col = col;
                                    //rc.cnt = Get_Set_Cnt(ps);
                                    memcpy(&rc.set,ps,sizeof(SET));
                                    vrc.push_back(rc);
                                }
                            }
                        }
                    }
                    if (vrc.size() == 2) {
                        // YOW, we have TWO empty cells in this BOX, NOT eliminated
                        // WE can APPLY the Naked Pair Strategy
                        sprintf(tb,"%3d:%d: key %d+%d has %d pairs, remainer 2", cnt, box, val1, val2, (int)pvp->size());
                        sprtf("%s\n",tb);
                        *tb = 0;
                        rc = vrc[0];
                        ps = &pb->line[rc.row].set[rc.col];
                        cnt2 = Mark_SET_NOT_in_SET( ps, ps2, cf_HPE );

                        sprintf(EndBuf(tb)," Box %d: r %d c %d has %d ", box, rc.row, rc.col,
                            Get_Set_Cnt(ps) ); // rc.cnt);
                        AddSet2Buf(tb, ps);
                        sprintf(EndBuf(tb), " elim %d ", cnt2);
                        count += cnt2;
                        rc2 = vrc[1];
                        ps = &pb->line[rc2.row].set[rc2.col];
                        cnt2 = Mark_SET_NOT_in_SET( ps, ps2, cf_HPE );
                        sprintf(EndBuf(tb)," r %d c %d has %d ",rc2.row, rc2.col,
                            Get_Set_Cnt(ps) ); // rc2.cnt);
                        AddSet2Buf(tb, ps);
                        sprintf(EndBuf(tb), " elim %d ", cnt2);
                        count += cnt2;
                        // hmmm wanted to find the 3 pairs resposible, but difficult
                        //for (ii = pvp->begin(); ii != pvp->end(); ii++) {
                        //    pair = *ii;
                        //}
                    } else {
                        sprintf(tb,"%3d:%d: key %d+%d has %d pairs", cnt, box, val1, val2, (int)pvp->size());
                        sprintf(EndBuf(tb)," found %d blanks", (int)vrc.size());
                    }
                    sprtf("%s\n",tb);
                    *tb = 0;
                }
            }
            if (dodel) {
                pvp->clear();
                delete pvp;
                mMap.erase(mi);
                haddel = true;
                break;
            }
        }
    }


    // on the way OUT
    add_list_out(prev);
    for ( mi = mMap.begin(); mi != mMap.end(); mi++ ) {
        pvp = (*mi).second;
        pvp->clear();
        delete pvp;
    }
    return count;
}

int Do_Hidden_Pairs(PABOX2 pb)
{
    int row, col, val, cnt, count;
    int col2, val2, tmp;
    PSET ps;
    vPAIR vp;
    PAIR pair;
    count = 0;
    sprtf("S2d: Find Hidden Pairs\n");
    char *tb = GetNxtBuf();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) {
                sprintf(tb,"Found %d at r %d c %d", val, row + 1, col + 1);
                if (val == 6)
                    tmp = 1;
                // got one value, find second in this ROW
                for (col2 = col + 1; col2 < 9; col2++) {
                    val2 = pb->line[row].val[col2];
                    if (val2) {
                        // got a PAIR of values
                        sprintf(tb,"Pair %d%d at r %d c %d + c %d", val, val2, row + 1, col + 1, col2 + 1);
                        if (val2 == 7)
                            tmp = 2;
                        cnt = Find_Same_Pair( pb, val, row, col, val2, row, col2, vp );
                        count += cnt;
                        if (cnt) {  // Found this pair in other places, so ADD this PAIR
                            // put lower column value as FIRST pair
                            if (col < col2) {
                                pair.row1 = row;
                                pair.col1 = col;
                                pair.val1 = val2;
                                pair.row2 = row;
                                pair.col2 = col2;
                                pair.val2 = val;
                            } else {
                                pair.row1 = row;
                                pair.col1 = col2;
                                pair.val1 = val;
                                pair.row2 = row;
                                pair.col2 = col;
                                pair.val2 = val2;
                            }
                            if (Pair_Not_In_Vector( pair, vp )) {
                                vp.push_back(pair);
                                count++;
                            }
                        }
                    }
                }
            } else {
                ps = &pb->line[row].set[col];
                cnt = Get_Set_Cnt(ps);
            }
        }
    }

    sprtf("S2d: Found %d Pairs (%d)\n", vp.size(), count);
    count = Sort_Hidden_Pairs( pb, vp );
    if (count) {
        sprtf("S2d: Hidden Pairs, elim %d vals. To %d\n", count,
            sg_Fill_Hidden);
        pb->iStage = sg_Fill_Hidden;
    } else {
        sprtf("S2d: Hidden Pairs. No elims. To begin.\n");
        pb->iStage = sg_Begin;
    }
    return count;
}

// examples\hidden-triple.txt
// This tough puzzle has two Hidden Triples: the first, marked in red, 
// is in row A. Cell A4 contains [2,5,6], A7 has [2,6] and cell A9 
// contains [2,5]. These three cells are the last remaining cells 
// in row A which can contain 2, 5 and 6 so those numbers must go 
// in those cells. Therefore we can remove the other candidates.

#define OUTITHT(tb) if (add_debug_ht) OUTIT(tb)

int Do_Clear_HTrip(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_HTE, cf_HT, "HT", "HId.Trip", 0, false );
    return count;
}

// So find 3 cells, sharing 3 values, that are NOT in others in the ROW
// Each cell does NOT have to have each of the 3 values, and can have
// other candidates...
int Get_Sum_of_Cands_vRC( vRC *pvrc, PSET ps_in )
{
    int count = 0;
    size_t max = pvrc->size();
    size_t i;
    PROWCOL prc;
    PSET ps;
    int v;
    for (i = 0; i < max; i++) {
        prc = &pvrc->at(i);
        ps = &prc->set;
        for (v = 0; v < 9; v++) {
            if ( !(ps->val[v]) ) continue;
            if ( ps_in->val[v] ) continue;
            ps_in->val[v] = v + 1;
            count++;
        }
    }
    return count;
}

int Get_Sum_of_Cands_pset( PSET pssrc, PSET psdst )
{
    int count = 0;
    int i;
    for (i = 0; i < 9; i++) {
        if ( !(pssrc->val[i]) ) continue; // skip - no candidate in SOURCE
        if (  (psdst->val[i]) ) continue; // skip - DEST already has candidate
        psdst->val[i] = i + 1;  // ADD new candidate
        count++;    // bump count
    }
    return count;
}


int Get_Counts_of_Cands( vRC *pvrc, int *setvals )
{
    int count = 0;
    size_t max = pvrc->size();
    size_t i;
    PROWCOL prc;
    PSET ps;
    int v;
    for (i = 0; i < max; i++) {
        prc = &pvrc->at(i);
        ps = &prc->set;
        for (v = 0; v < 9; v++) {
            if ( !(ps->val[v]) ) continue;
            setvals[v]++;
            count++;
        }
    }
    return count;
}

int Mark_Hidden_Triple_pvrc(PABOX2 pb, vRC *pvrc)
{
    int count = 0;
    int count2 = 0;
    int cnt;
    size_t max, ii, i2, i3, i4;
    SET set[9];
    int setvals[9];
    int setvaldone[9];
    int v, v2, setval, val, scnt;
    PSET ps, ps1, ps2, ps3;
    PROWCOL prc1, prc2, prc3, prc4;
    uint64_t *pi64;
    char *tb = GetNxtBuf();

    max = pvrc->size();
    memset(setvals,0,sizeof(setvals));
    Get_Counts_of_Cands( pvrc, setvals );
    // First try - find a candidate that only EXIST in 3 cells
    for (v = 0; v < 9; v++) {
        if ( !(setvals[v] == 3) ) continue;
        setval = v + 1;
        // get the 3 cells with this candidate
        for (ii = 0; ii < max; ii++) {
            prc1 = &pvrc->at(ii);
            if ( !(prc1->set.val[v]) ) continue;
            ps1 = &prc1->set;
            for (i2 = (ii + 1); i2 < max; i2++) {
                prc2 = &pvrc->at(i2);
                if ( !(prc2->set.val[v]) ) continue;
                for (i3 = (i2 + 1); i3 < max; i3++) {
                    prc3 = &pvrc->at(i3);
                    if ( !(prc3->set.val[v]) ) continue;
                    // GOT the 3 cells - Do the SHARE 1 or 2 other candidate asside from the setval
                    // that are NOT in any others of this ROW
                    ps1 = &set[0];
                    ps2 = &set[1];
                    ps3 = &set[2];
                    // copy SETS to a neutral place so cands can be eliminated
                    COPY_SET(ps1, &prc1->set);
                    COPY_SET(ps2, &prc2->set);
                    COPY_SET(ps3, &prc3->set);
                    for (i4 = 0; i4 < max; i4++) {
                        if (i4 == ii) continue; // skip cell 1
                        if (i4 == i2) continue; // skip cell 2
                        if (i4 == i3) continue; // skip cell 3
                        prc4 = &pvrc->at(i4);
                        ps = &prc4->set;
                        // Elim_SET_in_SET( PSET pssrc, PSET psdst )
                        Elim_SET_in_SET( ps, ps1 );
                        Elim_SET_in_SET( ps, ps2 );
                        Elim_SET_in_SET( ps, ps3 );
                    }
                    // done elimination by OTHERS - get SUM of candidates remaining
                    ps = &set[3];
                    ZERO_SET(ps);
                    Get_Sum_of_Cands_pset( ps1, ps );
                    Get_Sum_of_Cands_pset( ps2, ps );
                    Get_Sum_of_Cands_pset( ps3, ps );
                    scnt = Get_Set_Cnt2(ps, setvaldone);
                    if (scnt == 3) {
                        // Three candidate remain
                        // See if there are ANY eliminations possible - get original SETS back
                        COPY_SET(ps1, &prc1->set);
                        COPY_SET(ps2, &prc2->set);
                        COPY_SET(ps3, &prc3->set);
                        // Eliminate per the Hidden Triple set
                        Elim_SET_in_SET( ps, ps1 );
                        Elim_SET_in_SET( ps, ps2 );
                        Elim_SET_in_SET( ps, ps3 );
                        cnt = 0; // See if there is ANYTHING to eliminate
                        cnt += Get_Set_Cnt(ps1);
                        cnt += Get_Set_Cnt(ps2);
                        cnt += Get_Set_Cnt(ps3);
                        if (cnt) {  // YOW, we have Naked Trriple Eliminations
                            sprintf(tb,"Elim %d in %s %s %s cands other than ", cnt,
                                Get_RC_RCB_Stg(prc1), Get_RC_RCB_Stg(prc2), Get_RC_RCB_Stg(prc3));
                            AddSet2Buf(tb, ps);
                            OUTITHT(tb);

                            sprintf(tb,"Elim cf_HTE ");
                            cnt = 0;
                            for (v2 = 0; v2 < 9; v2++) {
                                val = ps1->val[v2];
                                if (!val) continue;
                                sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc1,val));
                                pi64 = &pb->line[prc1->row].set[prc1->col].flag[v2];
                                if (*pi64 & cf_HTE) continue;
                                *pi64 |= cf_HTE;
                                cnt++;
                            }
                            for (v2 = 0; v2 < 9; v2++) {
                                val = ps2->val[v2];
                                if (!val) continue;
                                sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc2,val));
                                pi64 = &pb->line[prc2->row].set[prc2->col].flag[v2];
                                if (*pi64 & cf_HTE) continue;
                                *pi64 |= cf_HTE;
                                cnt++;
                            }
                            for (v2 = 0; v2 < 9; v2++) {
                                val = ps3->val[v2];
                                if (!val) continue;
                                sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc3,val));
                                pi64 = &pb->line[prc3->row].set[prc3->col].flag[v2];
                                if (*pi64 & cf_HTE) continue;
                                *pi64 |= cf_HTE;
                                cnt++;
                            }
                            count += cnt;
                            sprintf(EndBuf(tb),"cnt %d",cnt);
                            OUTITHT(tb);
                            // Finally mark candidates that cause this elimination
                            if (cnt) {
                                cnt = 0;
                                sprintf(tb,"Mark cf_HT ");
                                for (v2 = 0; v2 < scnt; v2++) {
                                    // val = ps->val[v2];
                                    val = setvaldone[v2];
                                    if (!val) continue;
                                    if (pb->line[prc1->row].set[prc1->col].val[val-1]) {
                                        pb->line[prc1->row].set[prc1->col].flag[val-1] |= cf_HT;
                                        sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc1,val));
                                        cnt++;
                                    }
                                    if (pb->line[prc2->row].set[prc2->col].val[val-1]) {
                                        pb->line[prc2->row].set[prc2->col].flag[val-1] |= cf_HT;
                                        sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc2,val));
                                        cnt++;
                                    }
                                    if (pb->line[prc3->row].set[prc3->col].val[val-1]) {
                                        pb->line[prc3->row].set[prc3->col].flag[val-1] |= cf_HT;
                                        sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc3,val));
                                        cnt++;
                                    }
                                }
                                count2 += cnt;
                                sprintf(EndBuf(tb),"cnt %d",cnt);
                                OUTITHT(tb);
                            }
                        }
                    }
                }
            }
        }
    }
    return count;
}

// Missing one - http://www.sudokuwiki.org/sudoku.htm
// In prcessing diabolical4.txt - Box 9 contains - after other elims
// 5678@G7  [2]@G8  16@G9
// 4578@H7  [9]@H8  47@H9
// 368@I7  368@I8  136@I9
// HIDDEN TRIPLE: 4/5/7 in box 9: G7 - 5/6/7/8 -> 5/7
// HIDDEN TRIPLE: 4/5/7 in box 9: H7 - 4/5/7/8 -> 4/5/7
// ELIM: 68@G7 and 8@H7
// --------------------


int Mark_Hidden_Triple(PABOX2 pb)
{
    int count = 0;
    int row, col, box, cnt;
    RCRCB rcrcb;
    vRC empty;
    cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    vRC *pvrc;
    size_t max, ii; //, i2, i3, i4;
    //ROWCOL rc[9];
    SET set[9];
    int setvals[9];
    //int setvaldone[9];
    int v; //, v2, setval, val, scnt;
    PSET ps;
    //PSET ps1, ps2, ps3;
    PROWCOL prc1; //, prc2, prc3, prc4;
    //uint64_t *pi64;
    char *tb = GetNxtBuf();
    sprtf("Mark Hidden Triple\n");

    for (row = 0; row < 9; row++) {
        pvrc = &rcrcb.vrows[row];
        max = pvrc->size();
        ps = &set[row];
        sprintf(tb,"Row %d ", row + 1);
        ZERO_SET(ps);
        Get_Sum_of_Cands_vRC( pvrc, ps );
        AddSet2Buf(tb,ps);
        ADDSP(tb);
        memset(setvals,0,sizeof(setvals));
        Get_Counts_of_Cands( pvrc, setvals );
        for (v = 0; v < 9; v++) {
            if ( !(ps->val[v]) ) continue;
            sprintf(EndBuf(tb),"%d-%d ", ps->val[v], setvals[v]);
        }
        for (ii = 0; ii < max; ii++) {
            prc1 = &pvrc->at(ii);
            strcat(tb,Get_RC_setval_RC_Stg(prc1));
            ADDSP(tb);
        }
        OUTITHT(tb);
        // First try - find a candidate that only EXIST in 3 cells
        count += Mark_Hidden_Triple_pvrc(pb, pvrc);
    }
    for (col = 0; col < 9; col++) {
        pvrc = &rcrcb.vcols[col];
        max = pvrc->size();
        ps = &set[col];
        sprintf(tb,"Col %d ", col + 1);
        ZERO_SET(ps);
        Get_Sum_of_Cands_vRC( pvrc, ps );
        AddSet2Buf(tb,ps);
        ADDSP(tb);
        memset(setvals,0,sizeof(setvals));
        Get_Counts_of_Cands( pvrc, setvals );
        for (v = 0; v < 9; v++) {
            if ( !(ps->val[v]) ) continue;
            sprintf(EndBuf(tb),"%d-%d ", ps->val[v], setvals[v]);
        }
        for (ii = 0; ii < max; ii++) {
            prc1 = &pvrc->at(ii);
            strcat(tb,Get_RC_setval_RC_Stg(prc1));
            ADDSP(tb);
        }
        OUTITHT(tb);
        // First try - find a candidate that only EXIST in 3 cells
        count += Mark_Hidden_Triple_pvrc(pb, pvrc);
    }
    // 20120920 - Add a BOX scan
    for (box = 0; box < 9; box++) {
        pvrc = &rcrcb.vboxs[box];
        max = pvrc->size();
        ps = &set[box];
        sprintf(tb,"Box %d ", box + 1);
        ZERO_SET(ps);
        Get_Sum_of_Cands_vRC( pvrc, ps );
        AddSet2Buf(tb,ps);
        ADDSP(tb);
        memset(setvals,0,sizeof(setvals));
        Get_Counts_of_Cands( pvrc, setvals );
        for (v = 0; v < 9; v++) {
            if ( !(ps->val[v]) ) continue;
            sprintf(EndBuf(tb),"%d-%d ", ps->val[v], setvals[v]);
        }
        for (ii = 0; ii < max; ii++) {
            prc1 = &pvrc->at(ii);
            strcat(tb,Get_RC_setval_RC_Stg(prc1));
            ADDSP(tb);
        }
        OUTITHT(tb);
        // First try - find a candidate that only EXIST in 3 cells
        count += Mark_Hidden_Triple_pvrc(pb, pvrc);
    }
    sprtf("View Hidden Triple %d\n", count);
    if (count)
        Stack_Fill(Do_Clear_HTrip);
    return count;
}

int Do_Hidden_Triple_Scan(PABOX2 pb)
{
    int count = 0;
    RCRCB rcrcb;
    vRC empty;
    int cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    vRC *pvrc;
    int i;
    for (i = 0; i < 9; i++) {
        pvrc = &rcrcb.vrows[i];
        count += Mark_Hidden_Triple_pvrc(pb, pvrc);
    }
    for (i = 0; i < 9; i++) {
        pvrc = &rcrcb.vcols[i];
        count += Mark_Hidden_Triple_pvrc(pb, pvrc);
    }
    // 20120920 - Add BOX scan as well
    for (i = 0; i < 9; i++) {
        pvrc = &rcrcb.vboxs[i];
        count += Mark_Hidden_Triple_pvrc(pb, pvrc);
    }
    if (count) {
        sprtf("s2g: Hidden Triple %d - to fill\n",count);
        pb->iStage = sg_Fill_HidTrip;
    } else {
        sprtf("s2g: No Hidden Triple - to bgn\n",count);
        pb->iStage = sg_Begin;

    }

    return count;
}

int Do_Fill_HidTrip(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_HTE, cf_HT, "HT", "Hid.Trip", 0, true );
    sprtf("Fill Hidden Triple %d\n", count);
    return count;
}


////////////////////////////////////////////////////////////////////////


// Another try at Pointing Pairs
// =============================
// int add_debug_PP2 = 1;
int add_vector_counts2 = 0;

#define OUTITPP(b) if (add_debug_PP2) { OUTIT(b); }

void Show_Cells_Collected( PABOX2 pb, PRCRCB prcrcb, bool add_color )
{
    char *tb = GetNxtBuf();
    size_t jmax, k, j;
    ROWCOL rc;
    vRC *pvrc;
    int i;
    strcpy(tb, "By Row ");
    k = 0;
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vrows[i];
        jmax = pvrc->size();
        sprintf(EndBuf(tb),"%d-%d ", (int)(i + 1), (int)jmax);
        for (j = 0; j < jmax; j++) {
            rc = pvrc->at(j);
            sprintf(EndBuf(tb),"%s ",
                (add_color ? Get_RC_RCBC_Stg(&rc) : Get_RC_RCB_Stg(&rc)) );
        }
        k += jmax;
    }
    if (k)
        sprintf(EndBuf(tb),"tot %d", (int)k);
    else
        strcat(tb,"none");
    OUTIT(tb);

    strcpy(tb, "By Col ");
    k = 0;
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vcols[i];
        jmax = pvrc->size();
        sprintf(EndBuf(tb),"%d-%d ", (int)(i + 1), (int)jmax);
        for (j = 0; j < jmax; j++) {
            rc = pvrc->at(j);
            sprintf(EndBuf(tb),"%s ",
                (add_color ? Get_RC_RCBC_Stg(&rc) : Get_RC_RCB_Stg(&rc)) );
        }
        k += jmax;
    }
    if (k)
        sprintf(EndBuf(tb),"tot %d", (int)k);
    else
        strcat(tb,"none");
    OUTIT(tb);

    strcpy(tb, "By Box ");
    k = 0;
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vboxs[i];
        jmax = pvrc->size();
        sprintf(EndBuf(tb),"%d-%d ", (int)(i + 1), (int)jmax);
        for (j = 0; j < jmax; j++) {
            rc = pvrc->at(j);
            sprintf(EndBuf(tb),"%s ",
                (add_color ? Get_RC_RCBC_Stg(&rc) : Get_RC_RCB_Stg(&rc)) );
        }
        k += jmax;
    }
    if (k)
        sprintf(EndBuf(tb),"tot %d", (int)k);
    else
        strcat(tb,"none");
    OUTIT(tb);

}

void Add_vRC_2_Buf(char *tb, vRC &elim, bool add_set = false);

void Add_vRC_2_Buf(char *tb, vRC &elim, bool add_set)
{
    ROWCOL rc;
    size_t i;
    for (i = 0; i < elim.size(); i++) {
        rc = elim[i];
        sprintf(EndBuf(tb),"%s ",
            (add_set ? Get_RC_RCBS_Stg(&rc) : Get_RC_RCB_Stg(&rc)));
    }
}

/////////////////////////////////////////////////////////////////////////////
// Pointing Pairs
// ==============
// Pointing Pairs
// Relatively simple strategy
// If an any one Box a particular candidate is ONLY in a Row or Column,
// then that candidate eliminates others, outside the box, in the same Row or Col
int Elim_in_vRC_outside_Box( PABOX2 pb, vRC *pvrc, int box, int setval, vRC &elim )
{
    int count = 0;
    ROWCOL rc;
    size_t max = pvrc->size();
    size_t i;
    int box2;
    int val = setval - 1; // get 0 offset
    int row, col;
    PSET ps;
    char *tb = GetNxtBuf();
	if (!VALUEVALID(setval))
		return 0;
    strcpy(tb,"Elim cf_PPE ");
    for (i = 0; i < max; i++) {
        rc = pvrc->at(i);
        row = rc.row;
        col = rc.col;
        box2 = GETBOX(row,col);
        if (box2 == box)
			continue; // not in this BOX
        if (pb->line[row].val[col]) {
			sprtf("UGH: ERROR: %s HAS VALUE %d!\n", Get_RC_RCB_Stg(&rc),
				pb->line[row].val[col] );
			continue; // shud not need to test this - shuld just be EMPTIES
		}
        ps = &pb->line[row].set[col];
        if ( !ps->val[val] )
			continue; // does NOT contain this candidate
        if ( ps->flag[val] & cf_PPE )
			continue; // already MARKED for deletion
        ps->flag[val] |= cf_PPE;
        elim.push_back(rc);
        sprintf(EndBuf(tb),"%s ", Get_RC_RCB_Stg(&rc));
        count++;
    }
    if (count) {
        sprintf(EndBuf(tb),"Cnt=%d",count);
        OUTITPP(tb);
    }
    return count;
}

int PP_Elim_in_Row_outside_Box( PABOX2 pb, int row, int box, PRCRCB prcrcb, int setval,
    vRC &elim)
{
    vRC *pvrc = &prcrcb->vrows[row];   // get rc set for each in this row
    int count = Elim_in_vRC_outside_Box( pb, pvrc, box, setval, elim );
    return count;
}

int PP_Elim_in_Col_outside_Box( PABOX2 pb, int col, int box, PRCRCB prcrcb, int setval,
    vRC &elim)
{
    vRC *pvrc = &prcrcb->vcols[col];   // get rc set for each in this col
    int count = Elim_in_vRC_outside_Box( pb, pvrc, box, setval, elim );
    return count;
}

int g_bSkipPPVals = 0;
int Do_Pointing_Pairs( PABOX2 pb )
{
    int count = 0;
    vRC empty;
    RCRCB rcrcb;
    int i, cnt, ccnt, cc, val, c2, rcount, row, col, box, box2, mkcnt;
    size_t j, max, k;
    vRC *pvrc;
    PRCRCB prcrcb = &rcrcb;
    PROWCOL prc, prc2;
    PSET ps, ps2;
    int setvals[9];
    PROWCOL psets[9];
    vRC elim;
    char *tb = GetNxtBuf();
    //vRC vdone;
    ROWCOL rc, rc2;
    cnt = Get_Empty_Cells( pb, empty, prcrcb );
    for (i = 0; i < 9; i++) { // test ALL 9 boxes for 'pointers'
        pvrc = &prcrcb->vboxs[i];   // get rc set for each box
        max = pvrc->size();
        if (max < 2) continue;  // must have 2 or more members
        for (j = 0; j < max; j++) {
            prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
            rc = *prc;
            ///vdone.push_back(rc);
            row = prc->row;
            col = prc->col;
            box = GETBOX(row,col);
            ps = &prc->set; // pointer to list of candidates
            // got a set of candidates - work on each candidate
            ccnt = Get_Set_Cnt2( ps, setvals ); // get SET of value for this Cell
            for (cc = 0; cc < ccnt; cc++) {
                val = setvals[cc];  // get ONE candidate
                cnt = 0; // foreach candidate,
                psets[cnt++] = prc; // start collection of pointer
                // now to find other empties in this Box, that share this candidate
                for (k = 0; k < max; k++) {
                    if (k == j) continue;
                    prc2 = &pvrc->at(k);
                    rc2 = *prc2;
                    // THIS IS WRONG!!! if (Row_Col_in_RC_Vector(vdone, rc2.row, rc2.col)) continue;
                    box2 = GETBOX(rc2.row,rc2.col);
                    if (box != box2) {
                        continue;   // should already be only per BOX!!!
                    }
                    ps2 = &prc2->set; // pointer to list of candidates
                    if (!Value_in_SET( val, ps2 )) continue;
                    // Yowee, got TWO cells in this BOX that share a candidate
                    psets[cnt++] = prc2;
                }
                if (cnt >= 2) {
                    // YOWEE, got 2 or more cells in this box that share a candidate
                    for (c2 = 1; c2 < cnt; c2++) {
                        prc2 = psets[c2];
                        if (prc->row != prc2->row) break;
                    }
                    if (c2 == cnt) {
                        // Eureka, they SHARE a single ROW
                        sprintf(tb,"Box %d val %d share row %d cnt %d ", box + 1, val, prc->row+1, cnt);
                        elim.clear();
                        rcount = PP_Elim_in_Row_outside_Box( pb, prc->row, GETBOX(prc->row,prc->col),
                            prcrcb, val, elim );
                        count += rcount;
                        if (rcount || elim.size()) {
                            OUTITPP(tb);
                            sprintf(tb,"Elim %d x %d from ", val, rcount);
                            Add_vRC_2_Buf(tb,elim);
                            sprintf(EndBuf(tb)," same ROW as %s ", Get_RC_RCB_Stg(prc) );
                            row = prc->row;
                            col = prc->col;
                            mkcnt = 0;
                            if ( pb->line[row].val[col] ) {
                                sprtf("UGH! ERROR: %s HAS VALUE %d!\n", Get_RC_RCB_Stg(prc),
                                    pb->line[row].val[col] );
                            } else {
                                ps = &pb->line[row].set[col];
                                if ( ps->val[val-1] ) { // does it contain this candiate
                                    if ( !(ps->flag[val-1] & cf_PP) ) {
                                        ps->flag[val-1] |= cf_PP;
                                        mkcnt++;
                                    }
                                } else {
                                    sprtf("UGH! ERROR: %s does NOT have cand %d!\n",
                                        Get_RC_RCB_Stg(prc), val );
                                }
                            }
                            for (c2 = 1; c2 < cnt; c2++) {
                                prc2 = psets[c2];
                                sprintf(EndBuf(tb),"%s ", Get_RC_RCBS_Stg(prc2));
                                row = prc2->row;
                                col = prc2->col;
                                if ( pb->line[row].val[col] ) {
                                    sprtf("UGH! ERROR: %s HAS VALUE %d!\n", Get_RC_RCB_Stg(prc2),
                                        pb->line[row].val[col] );
                                } else {
                                    ps = &pb->line[row].set[col];
                                    if ( ps->val[val-1] ) { // does it contain this candiate
                                        if ( !(ps->flag[val-1] & cf_PP) ) {
                                            ps->flag[val-1] |= cf_PP;
                                            mkcnt++;
                                        }
                                    } else {
                                        sprtf("UGH! ERROR: %s does NOT have cand %d!\n",
                                            Get_RC_RCB_Stg(prc2), val );
                                    }
                                }
                            }
                            sprintf(EndBuf(tb),"mkd %d",mkcnt);
                            OUTITPP(tb);
                        }
                    } else {
                        for (c2 = 1; c2 < cnt; c2++) {
                            prc2 = psets[c2];
                            if (prc->col != prc2->col) break;
                        }
                        if (c2 == cnt) {
                            // Eureka, they SHARE a single COL
                            sprintf(tb,"Box %d val %d share col %d cnt %d ", box + 1, val, prc->col+1, cnt);
                            elim.clear();
                            rcount = PP_Elim_in_Col_outside_Box( pb, prc->col, GETBOX(prc->row,prc->col),
                                prcrcb, val, elim );
                            count += rcount;
                            if (rcount || elim.size()) {
                                OUTITPP(tb);
                                sprintf(tb,"Elim %d x %d from ", val, rcount);
                                Add_vRC_2_Buf(tb,elim);
                                sprintf(EndBuf(tb)," same COL as %s ", Get_RC_RCB_Stg(prc));
                                row = prc->row;
                                col = prc->col;
                                mkcnt = 0;
                                if ( pb->line[row].val[col] ) {
                                        sprtf("UGH! ERROR: %s HAS VALUE %d!\n", Get_RC_RCB_Stg(prc),
                                            pb->line[row].val[col] );
                                } else {
                                    ps = &pb->line[row].set[col];
                                    if ( ps->val[val-1] ) { // does it contain this candiate
                                        if ( !(ps->flag[val-1] & cf_PP) ) {
                                            ps->flag[val-1] |= cf_PP;
                                            mkcnt++;
                                        }
                                    } else {
                                        sprtf("UGH! ERROR: %s does NOT have cand %d!\n",
                                            Get_RC_RCB_Stg(prc), val );
                                    }
                                }
                                for (c2 = 1; c2 < cnt; c2++) {
                                    prc2 = psets[c2];
									sprintf(EndBuf(tb),"%s ", Get_RC_RCBS_Stg(prc2));
                                    row = prc2->row;
                                    col = prc2->col;
                                    if ( pb->line[row].val[col] ) {
                                            sprtf("UGH! ERROR: %s HAS VALUE %d!\n", Get_RC_RCB_Stg(prc2),
                                                pb->line[row].val[col] );
                                    } else {
                                        ps = &pb->line[row].set[col];
                                        if ( ps->val[val-1] ) { // does it contain this candiate
                                            if ( !(ps->flag[val-1] & cf_PP) ) {
                                                ps->flag[val-1] |= cf_PP;
                                                mkcnt++;
                                            }
                                        } else {
                                            sprtf("UGH! ERROR: %s does NOT have cand %d!\n",
                                                Get_RC_RCB_Stg(prc2), val );
                                        }
                                    }
                                }
                                sprintf(EndBuf(tb),"mkd %d", mkcnt);
                                OUTITPP(tb);
                            }
                        }   // if (c2 == cnt) COLS equal
                    }   // if (c2 == cnt) ROWS EQUAL else COLS equal
                }   // if (cnt >= 2) { // YOWEE, got 2 or more cells in this box that share a candidate
            }   // for (cc = 0; cc < ccnt; cc++) { val = setvals[cc];  // get ONE candidate
        }   // for (j = 0; j < max; j++) { prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
    }   // for (i = 0; i < 9; i++) { // test ALL 9 boxes for 'pointers'
    if (count) {
        sprtf("S2d: Found Pointing %d. To Fill\n", count );
        pb->iStage = sg_Fill_Pointers;
    } else {
        sprtf("S2d: NO Pointing FOUND.\n");
    }
    return count;
}

int Do_Fill_Pointing(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_PPE, cf_PP, "PP", "Pointing Pairs" );
    return count;
}

/////////////////////////////////////////////////////////////////////////////

int Do_Fill_Unique(PABOX2 pb)
{
    // Fill in UNIQUE values, and restart
    int row, col, val, count, nval, i;
    PSET ps;
    count = 0;
    if (!pb)
        pb = get_curr_box();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            ps = &pb->line[row].set[col];
            val = pb->line[row].val[col];
            for ( i = 0; i < 9; i++ ) {
                nval = ps->val[i];
                if (!nval) continue; // forget zero
                if (ps->flag[i] & cf_UNIQ) {
                    Change_Box(pb, row, col, nval);
                    count++;
                    ps->flag[i] &= ~cf_UNIQ;  // REMOVE 'unique' flagS
                }
            }
        }
    }

    char *tb = GetNxtBuf();
    sprintf(tb,"S%d: ",pb->iStage);
    if (count) {
        sprintf(EndBuf(tb),"Fill %d UNIQUE vals.", count);
    } else {
        strcat(tb,"NO Fill UNIQUE found.");
    }
    strcat(tb," To begin");
    sprtf("%s\n",tb);
    pb->iStage = sg_Begin;  // reset to do more eliminations if any
    return count;
}

// Delete Naked Pair items
int Do_Fill_Naked(PABOX2 pb)
{
    int count = 0;
    int row, col, val, cnt, nval, i;
    PSET ps;
    if (!pb)
        pb = get_curr_box();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            ps = &pb->line[row].set[col];
            val = pb->line[row].val[col];
            if (!val) {
                cnt = Get_Set_Cnt(ps,&nval);
                for (i = 0; i < 9; i++) {
                    val = ps->val[i];
                    if (val) {
                        if (ps->flag[i] & cf_NPE) {
                            // Deleted by Naked Pair stategy
                            count++;
                            ps->val[i] = 0; // remove potential value
                        }
                        ps->flag[i] &= ~(cf_NP | cf_NPE);   // CLEAR these flags
                    }
                }
            }
        }
    }
    char *tb = GetNxtBuf();
    sprintf(tb,"S%d: Del NAKED ",pb->iStage);
    if (count)
        sprintf(EndBuf(tb),"%d vals.",count);
    else
        strcat(tb,"NONE!");
    strcat(tb," To begin");
    sprtf("%s\n",tb);
    pb->iStage = sg_Begin;
    return count;
}

// Delete Hidden Pair items
int Do_Fill_Hidden(PABOX2 pb)
{
    int count = 0;
    int row, col, val, cnt, nval, i;
    PSET ps;
    count = 0;
    if (!pb)
        pb = get_curr_box();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            ps = &pb->line[row].set[col];
            val = pb->line[row].val[col];
            if (!val) {
                cnt = Get_Set_Cnt(ps,&nval);
                for (i = 0; i < 9; i++) {
                    val = ps->val[i];
                    if (val) {
                        if (ps->flag[i] & cf_HPE) {
                            // Deleted by Hidden Pair stategy
                            count++;
                            ps->val[i] = 0; // remove potential value
                        }
                        ps->flag[i] &= ~cf_HPE;   // CLEAR this flag
                    }
                }
            }
        }
    }
    char *tb = GetNxtBuf();
    if (count)
        sprintf(tb,"S%d: Del %d pot vals",sg_Fill_Hidden, count);
    else
        sprintf(tb,"S%d: NO pot vals del!", sg_Fill_Hidden);

    strcat(tb," To begin");
    sprtf("%s\n",tb);
    pb->iStage = sg_Begin;
    return count;
}


int Do_Fill_XWing( PABOX2 pb )
{
    int row, col, val, i, count;
    uint64_t flag;
    PSET ps;
    if (!pb) pb = get_curr_box();
    count = 0;
    char *tb = GetNxtBuf();
    strcpy(tb,"XW ");
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            pb->line[row].set[col].cellflg &= ~(cf_XW | cf_XWR);
            if (!val) {
                ps = &pb->line[row].set[col];
                for (i = 0; i < 9; i++) {
                    val = ps->val[i];
                    if (val) {
                        flag = ps->flag[i];
                        if (flag & cf_XWE) {
                            sprintf(EndBuf(tb),"R%dC%d %d ", row + 1, col + 1, val);
                            ps->val[i] = 0;
                            ps->flag[i] &= ~cf_XWE;
                            count++;
                        }
                        if (flag & cf_XW) {
                            ps->flag[i] &= ~cf_XW;
                        }
                    }
                }
            }
        }
    }
    if (count) {
        OUTIT(tb);
        sprintf(tb,"S%d: Cleared [%d] X-Wing Excl ", pb->iStage, count);
    } else {
        sprintf(tb,"S%d: No X-Wing Excl found ", pb->iStage);
    }
    strcat(tb," To begin");
    pb->iStage = sg_Begin;
    OUTIT(tb);
    // set_repaint();
    return count;
}


void Do_ID_OPTIONS_SHOWHINT()
{
    // if there is a SELECTION, show possible values
    int row, col;
    int val, cnt;
    get_curr_selRC( &row, &col );
    if (Is_Selected_Valid() && VALIDRC(row,col)) {
        PABOX2 pb = get_curr_box();
        vGRID *pg = Get_Hints(pb);
        if (pg->size()) {
            vGRIDi ii;
            GRID grid;
            SET set;
            Zero_SET(&set);
            cnt = 0;
            for (ii = pg->begin(); ii != pg->end(); ii++) {
                grid = *ii;
                val = grid.v[row][col];
                if (VALUEVALID(val)) {
                    if (!set.val[val - 1]) {
                        set.val[val - 1] = val;
                        cnt++;
                    }
                }
            }
            if (cnt) {
                char *tb = GetNxtBuf();
                sprintf(tb,"%d %s for r%d c%d - ", cnt, 
                    ((cnt == 1) ? "hint" : "hints"),
                    row + 1, col + 1);
                Add_Set2Buf(EndBuf(tb),&set);
                sprtf("%s\n",tb);
            } else {
                sprtf("Found NO hint for r%d c%d!\n", row + 1, col + 1 );
            }
        } else
            sprtf("No hint for row %d col %d!\n", row + 1, col + 1 );

    } else
        sprtf("Make selection to get 'hints'\n");

    set_repaint(false);
}

void Reset_Candidates( bool clr_flag )
{
    int row, col, val, i;
    PABOX2 pb = get_curr_box();
    PSET ps;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (!val) {
                ps = &pb->line[row].set[col];
                Fill_SET(ps);
                if (clr_flag) {
                    for (i = 0; i < 9; i++) {
                        ps->flag[i] = 0;
                    }
                }
            }
        }
    }
    pb->iStage = sg_Begin;
    sprtf("Reset all candidates. To begin\n");

}


//                          mask elim from
int Elim_Shared_SET_values( PSET mask, PSET elim )
{
    int cnt = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = mask->val[i];
        if (val) {
            if (elim->val[i]) {
                cnt++;
                elim->val[i] = 0;
            }
        }
    }
    return cnt;
}


//  takes two rows and finds four empty cells perpendicular to form a rectangle
// The cells must be related to each other using a common value that is held as a candidate for each of the empty cells,
// there must only ever be two empty cells on each of the two rows that house that certain common value
vRC vXWrc;
#define SAMERC(a,b) ((a.row == b.row)&&(a.col == b.col))
int Has_This_Cell_Been_Done( vRC & vrc, int row1, int col1, int row2, int col2, int row3, int col3, int row4, int col4 )
{
    ROWCOL rc[4];
    ROWCOL r1,r2,r3,r4;
    size_t i, j, k, l, m;
    r1.row = row1;
    r1.col = col1;
    r2.row = row2;
    r2.col = col2;
    r3.row = row3;
    r3.col = col3;
    r4.row = row4;
    r4.col = col4;
    if (add_debug_xw ) sprtf("Cell    R%dC%d R%dC%d R%dC%d R%dC%d %d\n",
        row1 + 1, col1 + 1,
        row2 + 1, col2 + 1,
        row3 + 1, col3 + 1,
        row4 + 1, col4 + 1,
        vrc.size() );

    if (add_debug_xw ) sprtf("Testing R%dC%d R%dC%d R%dC%d R%dC%d %d\n",
        r1.row + 1, r1.col + 1,  
        r2.row + 1, r2.col + 1, 
        r3.row + 1, r3.col + 1, 
        r4.row + 1, r4.col + 1,
        vrc.size() );

    for (i = 0; (i + 3) < vrc.size(); i += 4) {
        rc[0] = vrc[i];
        rc[1] = vrc[i+1];
        rc[2] = vrc[i+2];
        rc[3] = vrc[i+3];
        if (add_debug_xw ) sprtf("With    R%dC%d R%dC%d R%dC%d R%dC%d %d\n",
            rc[0].row + 1, rc[0].col,
            rc[1].row + 1, rc[1].col,
            rc[2].row + 1, rc[2].col,
            rc[3].row + 1, rc[3].col,
            i );

        for (j = 0; j < 4; j++) {
            if (SAMERC(&rc[j],&r1)) {
                for (k = 0; k < 4; k++) {
                    if (k == j) continue;
                    if (SAMERC(&rc[k],&r2)) {
                        for (l = 0; l < 4; l++) {
                            if (l == j) continue;
                            if (l == k) continue;
                            if (SAMERC(&rc[l],&r3)) {
                                for (m = 0; m < 4; m++) {
                                    if (m == j) continue;
                                    if (m == k) continue;
                                    if (m == l) continue;
                                    if( SAMERC(&rc[m],&r4) )
                                        return 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int Mark_Val_in_SET( PSET ps, int mval, int flag, bool cnt_all = true );

int Mark_Val_in_SET( PSET ps, int mval, int flag, bool cnt_all )
{
    int cnt = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps->val[i];
        if (val == mval) {
            if (cnt_all) {
                ps->flag[i] |= flag;
                cnt++;
            } else if ( !(ps->flag[i] & flag) ) {
                ps->flag[i] |= flag;
                cnt++;
            }
            break;
        }
    }
    return cnt;
}


int only_one_X = 1; // for the moment only do ONE X-Wing elimination per step

// 2.1 - X-Wing
// This solving method considers four interlinked cells. The 
// technique takes two rows and finds four empty cells perpendicular 
// to form a rectangle. The cells must be related to each other 
// using a common value that is held as a candidate for each of 
// the empty cells, there must only ever be two empty cells on 
// each of the two rows that house that certain common value. 
// Either top-left and bottom-right or top-right and bottom-left of 
// this pattern will hold the true candidates, any other empty cells 
// in the columns connecting the X-Wing can have the common value 
// withdrawn from their candidate list.

// Yet another TRY to get this RIGHT - should be SIMPLE
// The idea this time - scan for first empty, then scan the ROW
// and the COL for others with same candidate(s), different box. When these 3 are found,
// simple check the fourth shares this candidate, and bingo we have a x-wing elimination
// in the ROWS and COLS shared by the 4 members.
// UGH!!! Now it seems TOO AGRESSIVE, and ELIMINATES VALID MEMBERS ;=((
// AH HAH, the X-Wing MUST have a strong link in one direction. hmmm
int Do_XWing_Scan3(PABOX2 pb)
{
    int count = 0;
    int row, col, val, box;
    int row2, col2, box2, scnt, scnt2, scnt3;
    int row3, col3, cnt, cnt1;
    PSET ps, ps2, ps3, ps4;
    SET comm, comm2, comm3, comm4;
    uint64_t *pi1, *pi2, *pi3, *pi4;
    char *tb = GetNxtBuf();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            // got ONE
            pi1 = &pb->line[row].set[col].cellflg;
            ps = &pb->line[row].set[col];
            box = GETBOX(row,col);
            // check the ROWS down this COL for sharer
            for (row2 = row + 1; row2 < 9; row2++) {
                box2 = GETBOX(row2,col);
                if (box2 == box) continue; // skip same BOX
                val = pb->line[row2].val[col];
                if (val) continue;
                ps2 = &pb->line[row2].set[col];
                ZERO_SET(&comm);
                scnt = Get_Shared_SET(ps, ps2, &comm); // get COMMON slot1 and slot 2
                if (!scnt) continue; // no shared here
                // got TWO - with shared in comm
                pi2 = &pb->line[row2].set[col].cellflg;
                // check the COLS across this ROW for sharer
                for (col2 = col + 1; col2 < 9; col2++) {
                    box2 = GETBOX(row,col2);
                    if (box2 == box) continue;
                    val = pb->line[row].val[col2];
                    if (val) continue;
                    ps3 = &pb->line[row].set[col2];
                    ZERO_SET(&comm2);
                    scnt2 = Get_Shared_SET(&comm,ps3,&comm2); // get COMMON slot3
                    if (!scnt2) continue;
                    // got THREE, with shared in comm2
                    pi3 = &pb->line[row].set[col2].cellflg;
                    // check the FORTH member
                    val = pb->line[row2].val[col2];
                    if (val) continue;
                    ps4 = &pb->line[row2].set[col2];
                    ZERO_SET(&comm3);
                    scnt3 = Get_Shared_SET(&comm2,ps4,&comm3); // get COMMON slot4
                    if (!scnt3) continue;
                    // GOT X-WING - 4 empty slots, sharing cands in comm3
                    pi4 = &pb->line[row2].set[col2].cellflg;
                    // NOW as pairs these must form a strong link in one direction
                    // Either slot1 == slot2 and slot3 == slot4, or
                    // slot1 == slot3 and slot2 == slot4, where '==' is a STRONG LINK
                    // That is they are the ONLY slots with the candidate in a ROW or COL
                    // If link is in ROWS, can eliminate in COLS only
                    // If link is in COLS, can eliminate in ROWS only
                    // Else NO ELIMINATION possible!
                    // That is do the candidates in comm3 only exist TWICE either in the 
                    // common ROWS - row and row2 - or in the common COLS - col and col2
                    // check COLS
                    cnt = 0; // count of other sharing this comm3 candidate
                    uint64_t cflag = 0;
                    ZERO_SET(&comm4);
                    for (row3 = 0; row3 < 9; row3++) {
                        if (row3 == row) continue; // skip slot 1
                        if (row3 == row2) continue; // skip slot 2
                        val = pb->line[row3].val[col];
                        if (!val) {
                            // potential elimination
                            ps4 = &pb->line[row3].set[col];
                            cnt += Get_Shared_SET(&comm3,ps4,&comm4); // get COMMON slot4
                        }
                        val = pb->line[row3].val[col2];
                        if (val) continue;
                        // potential elimination
                        ps4 = &pb->line[row3].set[col2];
                        cnt += Get_Shared_SET(&comm3,ps4,&comm4); // get COMMON slot4
                        if (cnt)
                            break; // only need to find 1 to break strong link
                    }
                    if (cnt)
                        cflag |= cf_XWR; // have elimins in ROWS, if COLS are strong linked

                    cnt = 0;
                    // check the ROWS
                    for (col3 = 0; col3 < 9; col3++) {
                        if (col3 == col) continue; // skip slot 1
                        if (col3 == col2) continue; // skip slot 2
                        val = pb->line[row].val[col3];
                        if (!val) {
                            // potential elimination
                            ps4 = &pb->line[row].set[col3];
                            cnt += Get_Shared_SET(&comm3,ps4,&comm4); // get COMMON slot4
                        }
                        val = pb->line[row2].val[col3];
                        if (val) continue;
                        // potential elimination
                        ps4 = &pb->line[row2].set[col3];
                        cnt += Get_Shared_SET(&comm3,ps4,&comm4); // get COMMON slot4
                        if (cnt)
                            break; // only need to find 1 to break strong link
                    }
                    if (cnt)
                        cflag |= cf_XWC; // have elimins in COLS, if ROWS are strong linked

                    if (cflag == 0) continue; // nothing to eliminate in either ROWS or COLS
                    if ((cflag & cf_XWR) && (cflag & cf_XWC)) continue; // NO STRONG LINK in one direction

                    // all that remains is to check the 2 ROWS and 2 COLS for cands to ELIMINATE
                    // ROWS are row/row2, COLS are col/col2
                    cnt = 0;    // count changes made
                    strcpy(tb,"X-Wing elim ");
                    if (cflag & cf_XWR) {
                        // check the COLS
                        for (row3 = 0; row3 < 9; row3++) {
                            if (row3 == row) continue; // skip slot 1
                            if (row3 == row2) continue; // skip slot 2
                            val = pb->line[row3].val[col];
                            if (!val) {
                                // potential elimination
                                ps4 = &pb->line[row3].set[col];
                                cnt1 = Mark_SET_in_SET(ps4,&comm3, cf_XWE);
                                cnt += cnt1;
                                if (cnt1) {
                                    AddSet2Buf(tb,&comm3);
                                    sprintf(EndBuf(tb),"@%c%d ", (char)(row3 + 'A'), col + 1);
                                }
                            }
                            val = pb->line[row3].val[col2];
                            if (val) continue;
                            // potential elimination
                            ps4 = &pb->line[row3].set[col2];
                            cnt1 = Mark_SET_in_SET(ps4,&comm3, cf_XWE);
                            cnt += cnt1;
                            if (cnt1) {
                                AddSet2Buf(tb,&comm3);
                                sprintf(EndBuf(tb),"@%c%d ", (char)(row3 + 'A'), col2 + 1);
                            }
                        }
                    }
                    if (cflag & cf_XWC) {
                        // check the ROWS
                        for (col3 = 0; col3 < 9; col3++) {
                            if (col3 == col) continue; // skip slot 1
                            if (col3 == col2) continue; // skip slot 2
                            val = pb->line[row].val[col3];
                            if (!val) {
                                // potential elimination
                                ps4 = &pb->line[row].set[col3];
                                cnt1 = Mark_SET_in_SET(ps4,&comm3, cf_XWE);
                                cnt += cnt1;
                                if (cnt1) {
                                    AddSet2Buf(tb,&comm3);
                                    sprintf(EndBuf(tb),"@%c%d ", (char)(row + 'A'), col3 + 1);
                                }
                            }
                            val = pb->line[row2].val[col3];
                            if (val) continue;
                            // potential elimination
                            ps4 = &pb->line[row2].set[col3];
                            cnt1 = Mark_SET_in_SET(ps4,&comm3, cf_XWE);
                            cnt += cnt1;
                            if (cnt1) {
                                AddSet2Buf(tb,&comm3);
                                sprintf(EndBuf(tb),"@%c%d ", (char)(row2 + 'A'), col3 + 1);
                            }
                        }
                    }
                    if (cnt) {
                        // cflag already contains the ROW or COL indicator - may have the SENSE wrong, but...
                        cflag |= cf_XW;
                        *pi1 |= cflag; // Mark Cell Flag
                        *pi2 |= cflag;
                        *pi3 |= cflag;
                        *pi4 |= cflag;
                        count += cnt;
                        sprintf(EndBuf(tb)," due %s %s %s %s",
                            Get_PB_RC_RCBS_Stg(pb, row, col),
                            Get_PB_RC_RCBS_Stg(pb, row2, col),
                            Get_PB_RC_RCBS_Stg(pb, row, col2),
                            Get_PB_RC_RCBS_Stg(pb, row2, col2));
                        OUTIT(tb);
                    }
                    if (count && only_one_X)
                        break;
                }   // for (col2 = col + 1; col2 < 9; col2++)
                if (count && only_one_X)
                    break;
            }   // for (row2 = row + 1; row2 < 9; row2++)
            if (count && only_one_X)
                break;
        }   // for (col = 0; col < 9; col++)
        if (count && only_one_X)
            break;
    }   // for (row = 0; row < 9; row++)
    return count;
}

int Do_XWing_Scan2( PABOX2 pb )
{
    int count;
    if (!pb)
        pb = get_curr_box();
    int row, col, val, ccnt;
    PSET psx1, psx2, psx3, psx4;
    uint64_t *pi1, *pi2, *pi3, *pi4;
    int row2, col2, val2, ccnt2, ccnt3;
    int row3, col3;
    SET comm, comm2, comm3;
    int bad = 0;
    char *tb = GetNxtBuf();
    if (add_debug_xw) sprtf("S%d: X-Wings Scan2\n", pb->iStage);
    count = 0;
    //ROWCOL rc;
    //vXWrc.clear();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (!val) {
                // got top-left blank - seek next top-right blank
                psx1 = &pb->line[row].set[col];
                pi1  = &pb->line[row].set[col].cellflg;
                for (col2 = col + 1; col2 < 9; col2++) {
                    val = pb->line[row].val[col2];
                    if (!val) {
                        psx2 = &pb->line[row].set[col2];
                        pi2  = &pb->line[row].set[col2].cellflg;
                        Zero_SET(&comm);
                        ccnt = Get_Common_SET_Cnt( psx1, psx2, &comm );
                        if (ccnt) {
                            // ok, found two cells in this ROW that SHARE a common candidate(s)
                            // Now decend each column to find two others to form the rectangle
                            for (row2 = row + 1; row2 < 9; row2++) {
                                val = pb->line[row2].val[col];
                                val2 = pb->line[row2].val[col2];
                                if ( !val && !val2 ) {
                                    // both are blank
                                    psx3 = &pb->line[row2].set[col];
                                    pi3  = &pb->line[row2].set[col].cellflg;
                                    psx4 = &pb->line[row2].set[col2];
                                    pi4  = &pb->line[row2].set[col2].cellflg;
                                    Zero_SET(&comm2);
                                    ccnt2 = Get_Common_SET_Cnt( psx3, psx4, &comm2 );
                                    if (ccnt2) {
                                        // ok, 3 and 4 share a common value (or more)
                                        Zero_SET(&comm3);
                                        ccnt3 = Get_Common_SET_Cnt( &comm, &comm2, &comm3 );
                                        if (ccnt3) {
                                            // more ok, the common of 1 & 2 have a common value of 3 & 4
                                            // time to check if either the rows OR columns are CLEAR of this/these CANDIDATES
                                            int i;
                                            int cflag = cf_XW;
                                            for (i = 0; i < 9; i++) {
                                                val = comm3.val[i];
                                                if (!val) continue;
                                                int result = 0;
                                                PSET ps2, ps3;
                                                // check COLS for this SHARED value
                                                for (row3 = 0; row3 < 9; row3++) {
                                                    if ((row3 == row)||(row3 == row2)) continue; // skip selves
                                                    val2 = pb->line[row3].val[col];
                                                    if (!val2) {
                                                        ps2 = &pb->line[row3].set[col];
                                                        if (Value_in_SET(val,ps2)) {
                                                            result |= 1; // MARK found in COL
                                                            break;
                                                        }
                                                    }
                                                    val2 = pb->line[row3].val[col2];
                                                    if (!val2) {
                                                        ps2 = &pb->line[row3].set[col2];
                                                        if (Value_in_SET(val,ps2)) {
                                                            result |= 1; // MARK found in COL
                                                            break;
                                                        }
                                                    }
                                                }
                                                // Done COLS, now ROWS
                                                for (col3 = 0; col3 < 9; col3++) {
                                                    if ((col3 == col)||(col3 == col2)) continue; // skip selves
                                                    val2 = pb->line[row].val[col3];
                                                    if (!val2) {
                                                        ps3 = &pb->line[row].set[col3];
                                                        if (Value_in_SET(val,ps3)) {
                                                            result |= 2;    // MARK found in ROW
                                                            break;
                                                        }
                                                    }
                                                    val2 = pb->line[row2].val[col3];
                                                    if (!val2) {
                                                        ps3 = &pb->line[row2].set[col3];
                                                        if (Value_in_SET(val,ps3)) {
                                                            result |= 2;    // MARK found in ROW
                                                            break;
                                                        }
                                                    }
                                                }
                                                // Done ROW and COL check for THIS val
                                                if ((result == 1)||(result == 2)) {
                                                    // TIME TO PARTY
                                                    int cnt3;
                                                    sprintf(tb,"XW2: R%dC%d%d and R%dC%d%d share ",
                                                        row + 1, col + 1, col2 + 1, row2 + 1, col + 1, col2 + 1);
                                                    AddSet2Buf(tb,&comm3);
                                                    if (result == 1) {
                                                        sprintf(EndBuf(tb)," elim %d in COL ", val);
                                                        for (row3 = 0; row3 < 9; row3++) {
                                                            if ((row3 == row)||(row3 == row2)) continue; // skip selves
                                                            val2 = pb->line[row3].val[col];
                                                            if (!val2) {
                                                                ps2 = &pb->line[row3].set[col];
                                                                if (Mark_Val_in_SET( ps2, val, cf_XWE )) { // ELIM by X-Wing
                                                                    count++;
                                                                    sprintf(EndBuf(tb),"R%dC%d ", row3 + 1, col + 1);
                                                                }
                                                            }
                                                            val2 = pb->line[row3].val[col2];
                                                            if (!val2) {
                                                                ps2 = &pb->line[row3].set[col2];
                                                                if (Mark_Val_in_SET( ps2, val, cf_XWE )) { // ELIM by X-Wing
                                                                    count++;
                                                                    sprintf(EndBuf(tb),"R%dC%d ", row3 + 1, col2 + 1);
                                                                }
                                                            }
                                                        }
                                                        cflag = cf_XW;
                                                    } else if (result == 2) {
                                                        sprintf(EndBuf(tb)," elim %d in ROW ", val);
                                                        cnt3 = 0;
                                                        for (col3 = 0; col3 < 9; col3++) {
                                                            if ((col3 == col)||(col3 == col2)) continue; // skip selves
                                                            val2 = pb->line[row].val[col3];
                                                            if (!val2) {
                                                                ps3 = &pb->line[row].set[col3];
                                                                if (Mark_Val_in_SET( ps3, val, cf_XWE )) { // ELIM by X-Wing
                                                                    count++;
                                                                    sprintf(EndBuf(tb),"R%dC%d ", row + 1, col3 + 1);
                                                                }
                                                            }   
                                                            val2 = pb->line[row2].val[col3];
                                                            if (!val2) {
                                                                ps3 = &pb->line[row2].set[col3];
                                                                if (Mark_Val_in_SET( ps3, val, cf_XWE )) { // ELIM by X-Wing
                                                                    count++;
                                                                    sprintf(EndBuf(tb),"R%dC%d ", row2 + 1, col3 + 1);
                                                                }
                                                            }   
                                                        }
                                                        cflag = cf_XW | cf_XWR; // Mark Cell Flag
                                                    }
                                                    *pi1 |= cflag; // Mark Cell Flag
                                                    *pi2 |= cflag;
                                                    *pi3 |= cflag;
                                                    *pi4 |= cflag;
                                                    sprtf("%s\n",tb);
                                                }   // if ((result == 1)||(result == 2))
                                                if ( count && only_one_X )
                                                    break;
                                            }   // for (i = 0; i < 9; i++)
                                        }   // if (ccnt3)
                                    }   // if (ccnt2)
                                }   // if ( !val && !val2 )
                                if ( count && only_one_X )
                                    break;
                            }   // for (row2 = row + 1; row2 < 9; row2++)
                        }   // if (ccnt)
                    }   // if (!val)
                    if ( count && only_one_X )
                        break;
                }   // for (col2 = col + 1; col2 < 9; col2++)
            }   // if (!val)
            if ( count && only_one_X )
                break;
        }   // for (col = 0; col < 9; col++)
        if ( count && only_one_X )
            break;
    }   // for (row = 0; row < 9; row++)

    return count;
}

// TODO - This need a REWRITE - See Do_XWing_scan2() above
int Do_XWing_Scan( PABOX2 pb )
{
    int count;
    if (!pb)
        pb = get_curr_box();
    int row, col, val, ccnt;
    PSET psx1, psx2, psx3, psx4, ps5;
    uint64_t *pi1, *pi2, *pi3, *pi4;
    int row2, col2, val2, ccnt2, ccnt3;
    int row3, col3, cnt2, cnt3;
    SET comm, comm2, comm3, comm4;
    int bad = 0;
    char *tb = GetNxtBuf();
    if (add_debug_xw) sprtf("S%d: X-Wings Scan\n", pb->iStage);
    count = 0;
    ROWCOL rc;
    vXWrc.clear();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            // got top-left blank - seek top-right blank
            psx1 = &pb->line[row].set[col];
            pi1  = &pb->line[row].set[col].cellflg;
            for (col2 = col + 1; col2 < 9; col2++) {
                val = pb->line[row].val[col2];
                if (val) continue;
                psx2 = &pb->line[row].set[col2];
                pi2  = &pb->line[row].set[col2].cellflg;
                Zero_SET(&comm);
                ccnt = Get_Common_SET_Cnt( psx1, psx2, &comm );
                if (!ccnt) continue;
                // ok, found two cells in this ROW that SHARE a common candidate(s)
                // Now decend each column to find two others to form the rectangle
                for (row2 = row + 1; row2 < 9; row2++) {
                    val = pb->line[row2].val[col];
                    val2 = pb->line[row2].val[col2];
                    if ( !( !val && !val2 ) ) continue;
                    // both are blank
                    psx3 = &pb->line[row2].set[col];
                    pi3  = &pb->line[row2].set[col].cellflg;
                    psx4 = &pb->line[row2].set[col2];
                    pi4  = &pb->line[row2].set[col2].cellflg;
                    Zero_SET(&comm2);
                    ccnt2 = Get_Common_SET_Cnt( psx3, psx4, &comm2 );
                    if (!ccnt2) continue;
                    Zero_SET( &comm3 );
                    ccnt3 = Get_Common_SET_Cnt( &comm, &comm2, &comm3 );
                    if (!ccnt3) continue;
                    // getting close
                    // Now must ensure either the two ROW, or the 2 COLUMNS
                    // one or the other do NOT share this common set
                    // first test the columns
                    bad = 0; // reset BAD horizontal or vertical scan
                    memcpy(&comm4,&comm3,sizeof(SET)); // keep original of common
                    // first a ROW check
                    for (col3 = 0; col3 < 9; col3++) {
                        if (col3 == col) continue; // skip first self
                        if (col3 == col2) continue; // skip second self
                        val = pb->line[row].val[col3];
                        if (val) continue;
                        ps5 = &pb->line[row].set[col3];
                        if (Elim_Shared_SET_values( ps5, &comm4 )) {
                            //ccnt = Get_Set_Cnt(&comm4);
                            //if (!ccnt) {
                                // oops ROW has SHARED value
                                // same ROW search FAILED
                                bad = 1;
                                break;
                            //}
                        }
                    }
                    if (!bad) {
                        // ok have a common value NOT eliminated, and NOT elswhere in ROW
                        // test the 2nd column
                        memcpy(&comm4,&comm3,sizeof(SET)); // keep original of common
                        for (col3 = 0; col3 < 9; col3++) {
                            if (col3 == col) continue; // skip first self
                            if (col3 == col2) continue; // skip second self
                            val = pb->line[row2].val[col3];
                            if (val) continue;
                            ps5 = &pb->line[row2].set[col3];
                            if (Elim_Shared_SET_values( ps5, &comm3 )) {
                                //ccnt = Get_Set_Cnt(&comm3);
                                //if (!ccnt) {
                                    // oops everything eliminated
                                    // same column search FAILED
                                    bad = 1;
                                    break;
                                //}
                            }
                        }
                    }
                    if (!bad) {
                        if (add_debug_xw ) sprtf("Check   R%dC%d R%dC%d R%dC%d R%dC%d \n",
                            row + 1, col + 1,
                            row + 1, col2 + 1,
                            row2 + 1, col + 1,
                            row2 + 1, col2 + 1 );
                        bad = Has_This_Cell_Been_Done( vXWrc, row, col, row, col2, row2, col, row2, col2 );
                    }

                    if (!bad) {
                        // have 2 ROWS that have a share candidate, where NO other
                        // members have this same candidate, so can clear the columns
                        // of this candidate(s)
                        sprintf(tb,"X-Wing ROW R%dC%d R%dC%d R%dC%d R%dC%d ",
                            row + 1, col + 1, row + 1, col2 + 1,
                            row2 + 1, col + 1, row2 + 1, col2 + 1);
                        cnt2 = 0;
                        for (row3 = 0; row3 < 9; row3++) {
                            if (row3 == row) continue; // skip first self
                            if (row3 == row2) continue; // skip second self
                            val = pb->line[row3].val[col];
                            if (val)
                                if (!val) {
                                ps5 = &pb->line[row3].set[col];
                                // set elimination flag
                                cnt3 = Mark_SET_in_SET( ps5, &comm3, cf_XWE, false ); // Do_XWing_Scan()
                                cnt2 += cnt3;
                                if (cnt3)
                                    sprintf(EndBuf(tb),"r%dc%d ", row3 + 1, col + 1);
                            }
                            val2 = pb->line[row3].val[col2];
                            if (!val2) {
                                ps5 = &pb->line[row3].set[col2];
                                // set elimination flag
                                cnt3 = Mark_SET_in_SET( ps5, &comm3, cf_XWE, false );
                                cnt2 += cnt3;
                                if (cnt3)
                                    sprintf(EndBuf(tb),"r%dc%d ", row3 + 1, col2 + 1);
                            }
                        }
                        count += cnt2;
                        sprintf(EndBuf(tb),"elim [%d] ",cnt2);
                        AddSet2Buf(tb,&comm3);
                        if (add_debug_xw && cnt2) {
                            OUTIT(tb);
                        }
                        if (cnt2) {
                            cnt2 = 0;
                            // mark candidate flags accordingly
                            cnt2 += Mark_SET_in_SET( psx1, &comm3, cf_XW, false );
                            cnt2 += Mark_SET_in_SET( psx2, &comm3, cf_XW, false );
                            cnt2 += Mark_SET_in_SET( psx3, &comm3, cf_XW, false );
                            cnt2 += Mark_SET_in_SET( psx4, &comm3, cf_XW, false );

                            // set the cell flags of the corners
                            *pi1 |= cf_XW | cf_XWR; // Mark Cell Flag
                            *pi2 |= cf_XW | cf_XWR;
                            *pi3 |= cf_XW | cf_XWR;
                            *pi4 |= cf_XW | cf_XWR;

                            rc.row = row;
                            rc.col = col;
                            // rc.cnt = Get_Set_Cnt(psx1);
                            memcpy(&rc.set,psx1,sizeof(SET));
                            vXWrc.push_back(rc);
                            rc.row = row;
                            rc.col = col2;
                            // rc.cnt = Get_Set_Cnt(psx2);
                            memcpy(&rc.set,psx2,sizeof(SET));
                            vXWrc.push_back(rc);
                            rc.row = row2;
                            rc.col = col;
                            // rc.cnt = Get_Set_Cnt(psx3);
                            memcpy(&rc.set,psx3,sizeof(SET));
                            vXWrc.push_back(rc);
                            rc.row = row2;
                            rc.col = col2;
                            // rc.cnt = Get_Set_Cnt(psx4);
                            memcpy(&rc.set,psx4,sizeof(SET));
                            vXWrc.push_back(rc);
                        }
                    } else {
                        // ROW searching did NOT pan out, so time for a column search
                        memcpy(&comm4,&comm3,sizeof(SET)); // keep original of common
                        for (row3 = 0; row3 < 9; row3++) {
                            if (row3 == row) continue; // skip first self
                            if (row3 == row2) continue; // skip second self
                            val = pb->line[row3].val[col];
                            if (!val) {
                                ps5 = &pb->line[row3].set[col];
                                if (Elim_Shared_SET_values( ps5, &comm4 )) {
                                    //ccnt = Get_Set_Cnt(&comm4);
                                    //if (!ccnt) {
                                        // oops everything eliminated
                                        // same column search FAILED
                                        bad = 1;
                                        break;
                                    //}
                                }
                            }
                            if (!bad) {
                                for (row3 = 0; row3 < 9; row3++) {
                                    if (row3 == row) continue; // skip first self
                                    if (row3 == row2) continue; // skip second self
                                    val = pb->line[row3].val[col2];
                                    if (!val) {
                                        ps5 = &pb->line[row3].set[col2];
                                        if (Elim_Shared_SET_values( ps5, &comm4 )) {
                                            //ccnt = Get_Set_Cnt(&comm4);
                                            //if (!ccnt) {
                                                // oops everything eliminated
                                                // same column search FAILED
                                                bad = 1;
                                                break;
                                            //}
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (!bad) {
                        if (add_debug_xw ) sprtf("Check   R%dC%d R%dC%d R%dC%d R%dC%d \n",
                            row + 1, col + 1,
                            row + 1, col2 + 1,
                            row2 + 1, col + 1,
                            row2 + 1, col2 + 1 );
                        bad = Has_This_Cell_Been_Done( vXWrc, row, col, row, col2, row2, col, row2, col2 );
                    }

                    if (!bad) {
                        // have 2 COLUMNS that have a share candidate, where NO other
                        // members have this same candidate, so can clear the ROWS
                        // of this candidate(s)
                        sprintf(tb,"X-Wing COLS R%dC%d R%dC%d R%dC%d R%dC%d ",
                            row + 1, col + 1, row + 1, col2 + 1,
                            row2 + 1, col + 1, row2 + 1, col2 + 1);
                        cnt2 = 0;
                        for (col3 = 0; col3 < 9; col3++) {
                            if (col3 == col) continue; // skip first self
                            if (col3 == col2) continue; // skip second self
                            val = pb->line[row].val[col3];
                            if (!val) {
                                ps5 = &pb->line[row].set[col3];
                                // set an elimination flag
                                cnt3 = Mark_SET_in_SET( ps5, &comm3, cf_XWE, false );
                                cnt2 += cnt3;
                                if (cnt3)
                                    sprintf(EndBuf(tb),"r%dc%d ", row + 1, col3 + 1);
                            }
                            val2 = pb->line[row2].val[col3];
                            if (!val2) {
                                ps5 = &pb->line[row2].set[col3];
                                // set an elimination flag
                                cnt3 = Mark_SET_in_SET( ps5, &comm3, cf_XWE, false );
                                cnt2 += cnt3;
                                if (cnt3)
                                    sprintf(EndBuf(tb),"r%dc%d ", row2 + 1, col3 + 1);
                            }
                        }
                        count += cnt2;
                        sprintf(EndBuf(tb),"elim [%d] ",cnt2);
                        AddSet2Buf(tb,&comm3);
                        if (add_debug_xw && cnt2) {
                            OUTIT(tb);
                        }
                        if (cnt2) {
                            cnt2 += Mark_SET_in_SET( psx1, &comm3, cf_XW, false );
                            cnt2 += Mark_SET_in_SET( psx2, &comm3, cf_XW, false );
                            cnt2 += Mark_SET_in_SET( psx3, &comm3, cf_XW, false );
                            cnt2 += Mark_SET_in_SET( psx4, &comm3, cf_XW, false ); // Do_XWing_Scan()

                            // set the cell flags of the corners
                            *pi1 |= cf_XW; // | cf_XWR;
                            *pi2 |= cf_XW; // | cf_XWR;
                            *pi3 |= cf_XW; // | cf_XWR;
                            *pi4 |= cf_XW; // | cf_XWR;

                            rc.row = row;
                            rc.col = col;
                            // rc.cnt = Get_Set_Cnt(psx1);
                            memcpy(&rc.set,psx1,sizeof(SET));
                            vXWrc.push_back(rc);
                            rc.row = row;
                            rc.col = col2;
                            // rc.cnt = Get_Set_Cnt(psx2);
                            memcpy(&rc.set,psx2,sizeof(SET));
                            vXWrc.push_back(rc);
                            rc.row = row2;
                            rc.col = col;
                            // rc.cnt = Get_Set_Cnt(psx3);
                            memcpy(&rc.set,psx3,sizeof(SET));
                            vXWrc.push_back(rc);
                            rc.row = row2;
                            rc.col = col2;
                            // rc.cnt = Get_Set_Cnt(psx4);
                            memcpy(&rc.set,psx4,sizeof(SET));
                            vXWrc.push_back(rc);
                        }
                    }
                    if (count & only_one_X)
                        break;
                }   // for (row2 = row + 1; row2 < 9; row2++)
                if (count & only_one_X)
                    break;
            }   // for (col2 = col + 1; col2 < 9; col2++)
            if (count & only_one_X)
                break;
        }   // for (col = 0; col < 9; col++)
        if (count & only_one_X)
            break;
    }   // for (row = 0; row < 9; row++)

    if (!count) // hmmm, try this logic AGAIN
        count = Do_XWing_Scan2(pb);
    if (!count) // and AGAIN
        count = Do_XWing_Scan3(pb);

    if (count) {
        sprtf("S2c: X-Wing scan, elim %d. To %d\n", count,
            sg_Fill_XWing);
        pb->iStage = sg_Fill_XWing;
    } else {
        sprtf("S2d: X-Wing scan. No elims. To begin.\n");
        pb->iStage = sg_Begin;
    }
    // set_repaint();
    return count;
}

int Do_Fill_Simple_OK(PABOX2 pb)
{
    int count = 0;
    int row, col, val, i;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (!val) {
                PSET ps = &pb->line[row].set[col];
                for (i = 0; i < 9; i++) {
                    val = ps->val[i];
                    if (val) {
                        if (ps->flag[i] & cf_SSE ) {
                            ps->val[i] = 0;
                            ps->flag[i] &= ~(cf_SSE);
                            count++;
                        }
                    }
                }
            }
        }
    }
    sprtf("S%d: Del %d simple - To Begin\n",pb->iStage, count);
    pb->iStage = sg_Begin;
    // set_repaint();
    return count;
}

int Do_Fill_Simple(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_SSE, cf_SS, "SF", "Simple", 0, true );
    //if (count) {
    //    sprtf("Sfs: Simple Fill %d - To Bgn\n", count );
    //    pb->iStage = sg_Begin;
    //} else {
    //    sprtf("Sfs: No Simple Fill - No Chg\n" );
    //}
    return count;
}


#define OUTITFF(a) if (add_debug_ff) OUTIT(a)

BOOL g_bChkElims = TRUE;

int Do_Fill_By_Flags( PABOX2 pb, uint64_t eflg, uint64_t mflg, char *smsg, char *type, uint64_t cflg,
                      bool do_delete )
{
    int row, col, val, i, count, mflags, cflags;
    uint64_t flag;
    PSET ps;
    ROWCOL rc;
    vRC elims, elimby;
    count = 0;
    mflags = 0;
    cflags = 0;
    char *tb = GetNxtBuf();
#ifdef WIN32 // format 64-bit value - I64
    sprintf(tb,"%s eflg=%I64x mflg=%I64x cflg=%I64x ",type, eflg, mflg, cflg);
#else   // !WIN32 - use PRIx64    
    sprintf(tb,"%s eflg=%" PRIx64 " mflg=%" PRIx64 " cflg=%" PRIx64 " ",type, eflg, mflg, cflg);
#endif  // WIN32 y/n

    // do whole puzzle
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // has value
            ps = &pb->line[row].set[col];
            for (i = 0; i < 9; i++) {
                val = ps->val[i];
                if (!val) continue; // no candidate
                flag = ps->flag[i];
                if (flag & eflg) {
                    sprintf(EndBuf(tb),"D%d@%c%d ", val, (char)(row + 'A'), col + 1);
                    if (do_delete)
                        ps->val[i] = 0; // CLEAR value marked for elimination
                    ps->flag[i] &= ~eflg;
                    count++;    // count an elimination of a candidate
                    rc.row = row;
                    rc.col = col;
                    rc.box = GETBOX(row,col);
                    COPY_SET(&rc.set,ps);
                    rc.cnum = i + 1;
                    elims.push_back(rc);
                }
                if (flag & mflg) {
                    sprintf(EndBuf(tb),"M%c%d ", (char)(row + 'A'), col + 1);
                    ps->flag[i] &= ~mflg;
                    mflags++;
                    rc.row = row;
                    rc.col = col;
                    rc.box = GETBOX(row,col);
                    COPY_SET(&rc.set,ps);
                    rc.cnum = i + 1;
                    elimby.push_back(rc);
                }
            }
            if (cflg) {
                if (ps->cellflg & cflg) {
                    ps->cellflg &= ~cflg;
                    cflags++;
                    sprintf(EndBuf(tb),"C%c%d ", (char)(row + 'A'), col + 1);
                }
            }
        }
    }

    sprintf(EndBuf(tb),"D=%d M=%d C=%d", count, mflags, cflags);
    OUTITFF(tb);

    // =============================================================
    if (g_bChkElims && (pb->bflag & bf_DnTest)) {
        size_t max = elims.size();
        size_t ii;
        PROWCOL prc;
        int val2;
        *tb = 0;
        cflags = 0;
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            val2 = get_solution_value(prc->row,prc->col);
            if (val2 && (val2 == prc->cnum)) {
                sprintf(EndBuf(tb),"%s ",Get_RC_setval_RC_Stg(prc,val2));
                cflags++;
            }
        }
        if (cflags) {
            sprtf("WARNING: %d %s elim errors! %s\n", cflags, type, tb);
        }
    }
    elims.clear();
    elimby.clear();
    // ================================================================

    if (count) {
        sprintf(tb,"S%d: Cleared [%d] %s ", pb->iStage, count, type);
        strcat(tb," To begin");
        pb->iStage = sg_Begin;
    } else {
        sprintf(tb,"S%d: No %s found - no change", pb->iStage, type);
    }
    OUTIT(tb);
    return count;
}


// In simple terms
// 1 - Classic - Rectangle
// (a) find cell that only has TWO candidates remaining
// (b) Search same ROW for next with 2, sharing one
// (c) Search same COL for next with 2, sharing one
//  AB --- BC
//  ...
//  AC ->  C is eliminated
// If cell 1 is A, then cell 3 is C, so cell 4 can not be, and
// if cell 1 is B, then cell 2 is C, so cell 4 can not be.
// What ever cell 1 is, cell 4 C can be eliminated
// 2 - Staggered Y
// (a) Find 2 cells in a box with only 2 candidates remaining, and they share 1
// (b) In another box, on the same row or column as one of the above, find 
// a cell with 2 candidates, sharing one with that in the same row column
// and the other with the 2nd cell.
// File: examples\y-wing.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || * |  3|1 3|| * | * |1 3||   |   |   ||
// A ||*9*|   |  6||*2*|*4*|   || 5 | 5 | 56|| 1
//   || * | 8 | 8 || * | * |78 ||7  | 8 | 8 ||
//    ---------------------------------------
//   ||   | * |   || * | * |   || * | * | * ||
// B ||4  |*5*|4  ||*6*|*9*|   ||*2*|*3*|*1*|| 2
//   ||78 | * | 8 || * | * |78 || * | * | * ||
//    ---------------------------------------
//   ||1 3| * |1 3||1  | * |1 3||   | * |   ||
// C ||  6|*2*|  6||   |*5*|   ||4  |*9*|4 6|| 3
//   ||78 | * | 8 || 8 | * |78 ||7  | * | 8 ||
//    =======================================
//   ||1  | * |1  || * |1  |   || * | * |   ||
// D ||4 6|*9*|456||*7*|  6|4  ||*3*|*2*|45 || 4
//   || 8 | * | 8 || * |   | 8 || * | * | 8 ||
//    ---------------------------------------
//   ||1  |   | * || * | * | * || * |1  | * ||
// E ||4  |4  |*2*||*9*|*3*|*5*||*6*|4  |*7*|| 5
//   || 8 | 8 | * || * | * | * || * | 8 | * ||
//    ---------------------------------------
//   ||1 3| * |1 3||   |1  | * || * |1  |   ||
// F ||4 6|*7*|456||4  |  6|*2*||*9*|45 |45 || 6
//   || 8 | * | 8 || 8 |   | * || * | 8 | 8 ||
//    =======================================
//   ||   | * | * ||1  | * |1  ||1  | * | * ||
// G ||4  |*6*|*9*||45 |*2*|4  ||45 |*7*|*3*|| 7
//   || 8 | * | * ||   | * |   || 8 | * | * ||
//    ---------------------------------------
//   || * | * |  3||  3| * | * ||   | * | * ||
// H ||*5*|*1*|4  ||4  |*7*|*9*||4  |*6*|*2*|| 8
//   || * | * | 8 ||   | * | * || 8 | * | * ||
//    ---------------------------------------
//   || * |  3| * ||1 3| * | * ||1  |   | * ||
// I ||*2*|4  |*7*||45 |*8*|*6*||45 |45 |*9*|| 9
//   || * |   | * ||   | * | * ||   |   | * ||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// Above, A2 38 & B3 48 in Box 1, share 8
// I2 has 34, on the same COL as A2 and shares 3
// If A2 is 3, then I2 has to be 4, would elim H3 4 - same box 7
// If A2 is 8, then B3 has to be 4, would elim H3 4 - same COL 3
// Hence H3 4 can be eliminated.

// Missing this one - WHY
// File: examples\y-wing.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || * |  3|1 3|| * | * |1 3||   |   |   ||
// A ||*9*|   |  6||*2*|*4*|   || 5 | 5 | 56|| 1
//   || * | 8 | 8 || * | * |78 ||7  | 8 | 8 ||
//    ---------------------------------------
//   ||   | * |   || * | * |   || * | * | * ||
// B ||4  |*5*|4  ||*6*|*9*|   ||*2*|*3*|*1*|| 2
//   ||78 | * | 8 || * | * |78 || * | * | * ||
//    ---------------------------------------
//   ||1 3| * |1 3||1  | * |1 3||   | * |   ||
// C ||  6|*2*|  6||   |*5*|   ||4  |*9*|4 6|| 3
//   ||78 | * | 8 || 8 | * |78 ||7  | * | 8 ||
//    =======================================
//   ||1  | * |1  || * |1  |   || * | * |   ||
// D ||4 6|*9*|456||*7*|  6|4  ||*3*|*2*|45 || 4
//   || 8 | * | 8 || * |   | 8 || * | * | 8 ||
//    ---------------------------------------
//   ||1  |   | * || * | * | * || * |1  | * ||
// E ||4  |4  |*2*||*9*|*3*|*5*||*6*|4  |*7*|| 5
//   || 8 | 8 | * || * | * | * || * | 8 | * ||
//    ---------------------------------------
//   ||1 3| * |1 3||   |1  | * || * |1  |   ||
// F ||4 6|*7*|456||4  |  6|*2*||*9*|45 |45 || 6
//   || 8 | * | 8 || 8 |   | * || * | 8 | 8 ||
//    =======================================
//   ||   | * | * ||1  | * |1  ||1  | * | * ||
// G ||4  |*6*|*9*||45 |*2*|4  ||45 |*7*|*3*|| 7
//   || 8 | * | * ||   | * |   || 8 | * | * ||
//    ---------------------------------------
//   || * | * |  3||  3| * | * ||   | * | * ||
// H ||*5*|*1*|   ||4  |*7*|*9*||4  |*6*|*2*|| 8
//   || * | * | 8 ||   | * | * || 8 | * | * ||
//    ---------------------------------------
//   || * |  3| * ||1 3| * | * ||1  |   | * ||
// I ||*2*|4  |*7*||45 |*8*|*6*||45 |45 |*9*|| 9
//   || * |   | * ||   | * | * ||   |   | * ||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// Box7 has 3 pairs that share - but G1 48 and I2 43
// should MATCH Box1 A2 38, and eliminate B1 and C1 8s!!!
// BUT I initially put a RESTRICTION to only finding 1 PAIR with a share!!!
#define OUTITYW(tb) if (add_debug_yw && *tb) OUTIT(tb)

int OR_Set_cellflg_with_Flag( PABOX2 pb, PSET ps, uint64_t flag, PROWCOL prc = 0 );
int OR_Set_setval_with_flag( PABOX2 pb, PSET ps, int setval, uint64_t flag, PROWCOL prc = 0 );


int OR_Set_cellflg_with_Flag( PABOX2 pb, PSET ps, uint64_t flag, PROWCOL prc )
{
    int iret = 0;
    if (prc) {
       sprtf("Marking prc %s cellflg with %s\n", Get_RC_RCB_Stg(prc), Flag2Name(flag));
    } else if (pb) {
        PSET ps2;
        int row, col;
        int fnd = 0;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                ps2 = &pb->line[row].set[col];
                if (ps == ps2) {
                    fnd = 1;
                    break;
                }
            }
            if (fnd)
                break;
        }
        if (fnd) {
            ROWCOL rc;
            rc.row = row;
            rc.col = col;
            sprtf("Marking est %s cellflg with %s\n", Get_RC_RCB_Stg(&rc), Flag2Name(flag));
        }
    }
    if (ps->cellflg & flag)
        iret = 1;   // already MARKED
    else
        ps->cellflg |= flag;
    return iret;
}

int OR_Set_setval_with_flag( PABOX2 pb, PSET ps, int setval, uint64_t flag, PROWCOL prc )
{
    int iret = 0;
    if (!VALUEVALID(setval)) {
        sprtf("ZUTE: OR_Set_setval_with_flag: INVALID setval %d\n", setval);
        return -1;
    }
    if (prc) {
        sprtf("Mark RC cand %d@%s with %s\n", setval, Get_RC_RC_Stg(prc), Flag2Name(flag));
    } else if (pb) {
        PSET ps2;
        int row, col;
        int fnd = 0;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                ps2 = &pb->line[row].set[col];
                if (ps == ps2) {
                    fnd = 1;
                    break;
                }
            }
            if (fnd)
                break;
        }
        if (fnd) {
            sprtf("Mark es cand %d@%c%d with %s\n", setval, (char)(row + 'A'), col + 1, Flag2Name(flag));
        }
    }

    if (ps->flag[setval - 1] & flag )
        iret = 1;
    else
        ps->flag[setval - 1] |= flag;
    return iret;
}


int only_one_Y = 1; // for the moment only do ONE Y-Wing elimination per step
static int xwing_scans = 0;
int Find_YWing_3rd(PABOX2 pb, PROWCOL prc1 , PROWCOL prc2, int br, int bc)
{
    int count =0;
    int row, col, row2, col2, val, res; 
    PSET psi1, psi2, ps3;
    int scnt;
    SET comm;
    ROWCOL rc9[9];
    PROWCOL prc, prc3;
    bool abort = false;
    ROWCOL trc;
    char *tb = GetNxtBuf();
    // in box got TWO cells each with 2 candidates
    row = prc1->row;
    col = prc1->col;
    row2 = prc2->row;
    col2 = prc2->col;
    psi1 = &pb->line[row].set[col];
    psi2 = &pb->line[row2].set[col2];
    // do they SHARE 1 common candidate
    Zero_SET(&comm);
    scnt = Get_Shared_SET( psi1, psi2, &comm );
    int box = (br * 3) + bc + 1;
    sprintf(tb,"Box%d ",box);
    if ((scnt == 1) && !(row == row2) && !(col == col2)) {
        sprintf(EndBuf(tb),"R%dC%d v ", row + 1, col + 1);
        AddSet2Buf(tb,psi1);
        sprintf(EndBuf(tb)," R%dC%d v ", row2 + 1, col2 + 1);
        AddSet2Buf(tb,psi2);
        strcat(tb," shr ");
        AddSet2Buf(tb,&comm);
        OUTITYW(tb);
        // they shared 1 candidate, and NOT in the same row or column
        // time to try to find a 3rd, in another box, in the same row or col set
        // as these, also has 2 cands, sharing 1, but not same one ad 1 & 2 share
        int br2, bc2;
        // These are the row/col pair
        for (br2 = 0; br2 < 3; br2++) {
            for (bc2 = 0; bc2 < 3; bc2++) {
                if ((br == br2) && (bc == bc2)) continue; // skip current BOX
                // only interested in SAME row or col
                int rw2, cl2;
                int row3, col3;
                int setcnt = 0;
                // test another box
                int box2 = (br2 * 3) + bc2 + 1;
                sprintf(tb,"B%d ", box2);
                for (rw2 = 0; rw2 < 3; rw2++) {
                    for (cl2 = 0; cl2 < 3; cl2++) {
                        row3 = (br2 * 3) + rw2;
                        col3 = (bc2 * 3) + cl2;
                        if ( !((row3 == row) || (row3 == row2) || (col3 == col) || (col3 == col2)) ) continue;
                        // interest - found same row or col
                        val = pb->line[row3].val[col3];
                        if (val) continue;
                        ps3 = &pb->line[row3].set[col3];
                        if ( !(Get_Set_Cnt(ps3) == 2) ) continue;
                        prc = &rc9[setcnt++];
                        prc->row = row3;
                        prc->col = col3;
                        // prc->cnt = 2;
                        COPY_SET(&prc->set,ps3);
                    }
                }
                // done another box
                sprintf(EndBuf(tb)," [%d] ", setcnt - 2);
                if (!setcnt) continue;
                // we got some cells to CHECK
                // each has 2 candidate - but do they share 1 with the 2 primaries
                int scnt1, scnt2;
                int i;
                SET comm2, comm3;
                for (i = 0; i < setcnt; i++) {
                    Zero_SET(&comm2);
                    Zero_SET(&comm3);
                    prc3 = &rc9[i]; // 3rd candidate cell
                    row3 = prc3->row;
                    col3 = prc3->col;
                    ps3 = &pb->line[row3].set[col3];
                    sprintf(EndBuf(tb)," %d: ", i - 1);
                    sprintf(EndBuf(tb),"r%dc%d ", row3 + 1, col3 + 1);
                    AddSet2Buf(tb,ps3);
                    scnt1 = Get_Shared_SET( psi1, ps3, &comm2 );
                    sprintf(EndBuf(tb)," s%d ", scnt1);
                    AddSet2Buf(tb,&comm2);
                    scnt2 = Get_Shared_SET( psi2, ps3, &comm3 );
                    sprintf(EndBuf(tb)," s%d ", scnt2);
                    AddSet2Buf(tb,&comm3);
                    if ( !((scnt1 == 1) && (scnt2 == 1) && !Same_SET( &comm2, &comm3 )) ) continue;
                    // Yowee - have a 3rd cell, in a different Box
                    // sharing 1 each with the two primaries
                    int nval = 0;
                    if (row3 == row) {
                        // eliminate comm3 shared in this BOX
                        Get_Set_Cnt(&comm3,&nval);
                    } else if (row3 == row2) {
                        // eliminate comm2 shared in this BOX
                        Get_Set_Cnt(&comm2,&nval);
                    } else if (col3 == col) {
                        // eliminate comm3 shared in this BOX
                        Get_Set_Cnt(&comm3,&nval);
                    } else if (col3 == col2) {
                        // eliminate comm2 shared in this BOX
                        Get_Set_Cnt(&comm2,&nval);
                    } else {
                        sprtf("YWing LOGIC ERROR - FIX ME!\n");
                    }
                    if (!nval) continue;
                    int row4, col4;
                    PSET ps4;
                    for (rw2 = 0; rw2 < 3; rw2++) {
                        for (cl2 = 0; cl2 < 3; cl2++) {
                            row4 = (br2 * 3) + rw2;
                            col4 = (bc2 * 3) + cl2;
                            if ((row4 == row3) && (col4 == col3)) 
                                continue; // Avoid elim in SELF
                            if ( !((row4 == row) || (col4 == col)||
                                    (row4 == row2)||(col4 == col2)) )
                                continue; // MUST share a ROW or COL
                            val = pb->line[row4].val[col4];
                            if (val) continue; // not an empty cell
                            ps4 = &pb->line[row4].set[col4];
                            if ( !(ps4->val[nval - 1]) ) continue; // does NOT have this cand
                            // FIX20120911 - New Y-Wing RULE - ADDITIONAL RESTRICTION - Basically
                            // if the Y cells share a ROW or a COL, then THIS elimination
                            // CAN NOT BE IN THE SAME ROW OR COL, so
                            if ( ((row4 == prc1->row) && (prc1->row == prc2->row)) ||
                                 ((col4 == prc1->col) && (prc1->col == prc2->col)) ||
                                 ((row4 == prc2->row) && (prc2->row == prc3->row)) ||
                                 ((col4 == prc2->col) && (prc2->col == prc3->col)) ||
                                 ((row4 == prc1->row) && (prc1->row == prc3->row)) ||
                                 ((col4 == prc1->col) && (prc1->col == prc3->col)) ) {
                                strcat(tb," BAD - new YW rule");
                                OUTITYW(tb);
                                continue; // BAD
                            }
                            // ps4->flag[nval - 1] |= cf_YWE;
                            res = OR_Row_Col_SetVal_with_Flag( pb, row4, col4, nval, cf_YWE );
                            if (res) {
                                trc.row = row4;
                                trc.col = col4;
                                COPY_SET(&trc.set,ps4);
                                OUTITYW(tb);
                                strcpy(tb,"Y-Wing Strategy Logic Problem!" MEOL);
                                sprintf(EndBuf(tb),"Marking %d@%s with elimination flag %s" MEOL
                                    "yielded an ERROR number %d!" MEOL, nval,
                                    Get_RC_RC_Stg(&trc),
                                    Flag2Name(cf_YWE),
                                    res);
                                sprintf(EndBuf(tb),"%s" MEOL, get_last_or_setval_error());
                                if (res < SERR_NOCND) {
                                    strcat(tb, "Error critical! Aborting application!" MEOL );
                                    abort = true;
                                } else {
                                    strcat(tb, "Click [Yes] to FORCE elimination." MEOL);
                                    strcat(tb, "Click [No] to abort operation." MEOL );
                                }
                                strcat(tb,"Click [Cancel] (or ESC) to exit application!" MEOL);
                                res = MB3(tb, "Y-Wing Strategy Problem");
                                if (abort || (res == IDCANCEL)) {
                                    Post_Command(IDM_EXIT);
                                    return 0;
                                } else if (!abort && (res == IDYES)) {
                                        res = OR_Row_Col_SetVal_with_Flag( pb, row4, col4, nval, cf_YWE, true );
                                        if (res) return 0;
                                } else {
                                    return 0;
                                }
                                *tb = 0;
                            }
                            count++;
                            OUTITYW(tb);
                            if (add_debug_yw) {
                                OR_Set_cellflg_with_Flag( pb, psi1, cf_YWC, prc1 );
                                OR_Set_cellflg_with_Flag( pb, psi2, cf_YWC, prc2 );
                                OR_Set_cellflg_with_Flag( pb, ps3, cf_YWC );

                                OR_Set_setval_with_flag( pb, psi1, nval, cf_YW, prc1 );
                                OR_Set_setval_with_flag( pb, psi2, nval, cf_YW, prc2 );
                                OR_Set_setval_with_flag( pb, ps3, nval, cf_YW );
                            } else {
                                psi1->cellflg |= cf_YWC;
                                psi2->cellflg |= cf_YWC;
                                ps3->cellflg |= cf_YWC;
                                psi1->flag[nval - 1] |= cf_YW;
                                psi2->flag[nval - 1] |= cf_YW;
                                ps3->flag[nval - 1] |= cf_YW;   
                            }
                        }
                    }
                    if (count && only_one_Y)
                        break;
                }
                if (count && only_one_Y)
                    break;
            }   // on pair process all other boxes - for (c2 = 0; c2 < 3; c2++)
            if (count && only_one_Y)
                break;
        }   // on pair process all other boxes - for (r2 = 0; r2 < 3; r2++)
    }   // if ((scnt == 1) && !(row == row2) && !(col == col2))
    // OUTITYW(tb);
    return count;
}

int Do_YWing_Scan(PABOX2 pb)
{
    int count = 0;
    int row, col, val, res;
    int row2, col2, scnt;
    PSET ps, ps2, ps3, ps4;
    SET comm, comm2, comm3;
    char *tb = GetNxtBuf();
    xwing_scans++;
    ROWCOL trc;
    bool abort = false;
    int br, bc;
    int brw, bcl;
    int i;
    int setcnt = 0;
    int box;
    ROWCOL rc[9+2];
    PROWCOL prc;

    if (add_debug_yw) sprtf("Doing YWing scan %d\n", xwing_scans);
    // classic Y-Wing - rectangle
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // looking for empty cells
            ps = &pb->line[row].set[col];
            if ( !(Get_Set_Cnt(ps) == 2) ) continue;
            // time to look for the 2nd member in same ROW
            for (col2 = col + 1; col2 < 9; col2++) {
                val = pb->line[row].val[col2];
                if (val) continue;
                ps2 = &pb->line[row].set[col2];
                // primary cell - get SHARED into comm
                Zero_SET(&comm);
                scnt = Get_Shared_SET( ps, ps2, &comm );
                if ( !((Get_Set_Cnt(ps2) == 2) && (scnt == 1)) ) continue;
                // time to look for the 3rd member in same COL
                for (row2 = row + 1; row2 < 9; row2++) {
                    val = pb->line[row2].val[col];
                    if (val) continue;
                    ps3 = &pb->line[row2].set[col];
                    Zero_SET(&comm2);
                    scnt = Get_Shared_SET( ps, ps3, &comm2 );
                    if ( !((Get_Set_Cnt(ps3) == 2) && (scnt == 1) &&
                        !Equal_SET(&comm, &comm2) ) ) continue; // 1-2 & 1-3 do NOT hav SAME shared
                    // Yikes getting close
                    Zero_SET(&comm3);
                    scnt = Get_Shared_SET( &comm, &comm2, &comm3 );
                    if ( !(scnt == 1) ) continue; // do NOT share ONE
                    // can eliminate this comm3 at the intersection
                    // of the column of 2, and the row of 3
                    val = pb->line[row2].val[col2];
                    if (val) continue;
                    ps4 = &pb->line[row2].set[col2];
                    Get_Set_Cnt(&comm3,&val);
                    if (!val) continue;
                    // got value to MARK
                    if ( !(ps4->val[val - 1]) ) continue;
                    // ps4->flag[val - 1] |= cf_YWE;
                    res = OR_Row_Col_SetVal_with_Flag( pb, row2, col2, val, cf_YWE );
                    if (res) {
                        trc.row = row2;
                        trc.col = col2;
                        COPY_SET(&trc.set,ps4);
                        strcpy(tb,"Y-Wing Strategy Logic Problem!" MEOL);
                        sprintf(EndBuf(tb),"Marking %d@%s with elimination flag %s" MEOL
                            "yielded an ERROR %d!" MEOL, val,
                            Get_RC_RCBS_Stg(&trc),
                            Flag2Name(cf_YWE),
                            res);

                        sprintf(EndBuf(tb),"%s" MEOL, get_last_or_setval_error());
                        if (res < SERR_NOCND) {
                            strcat(tb, "Error critical! Aborting operation!" MEOL );
                            abort = true;
                        } else {
                            strcat(tb, "Click [Yes] to FORCE elimination." MEOL);
                            strcat(tb, "Click [No] to abort operation." MEOL );
                        }
                        strcat(tb,"Click [Cancel] (or ESC) to exit application!" MEOL);
                        res = MB3(tb, "Y-Wing Strategy Problem");
                        if (res == IDCANCEL) {
                            Post_Command(IDM_EXIT);
                            return 0;
                        } else if (!abort && (res == IDYES)) {
                                OR_Row_Col_SetVal_with_Flag( pb, row2, col2, val, cf_YWE, true );
                        } else {
                            return 0;
                        }
                    }
                    count++;
                    ps->cellflg |= cf_YWC;
                    ps2->cellflg |= cf_YWC;
                    ps3->cellflg |= cf_YWC;
                    ps->flag[val - 1] |= cf_YW;
                    ps2->flag[val - 1] |= cf_YW;
                    ps3->flag[val - 1] |= cf_YW;
                }
            }
        }
    }

    if (count && only_one_Y)
        goto YWing_Exit;

    // Now to do the more generalised case
    // work box by box
    //int r, c, rw, cl, i;
    for (br = 0; br < 3; br++) {
        for (bc = 0; bc < 3; bc++) {
            // Do a BOX
            setcnt = 0;
            box = (br * 3) + bc + 1;
            sprintf(tb,"Box%d ",box);
            for (brw = 0; brw < 3; brw++) {
                for (bcl = 0; bcl < 3; bcl++) {
                    row = (br * 3) + brw;
                    col = (bc * 3) + bcl;
                    val = pb->line[row].val[col];
                    if (val) continue;
                    ps = &pb->line[row].set[col];
                    if ( !(Get_Set_Cnt(ps) == 2) ) continue;
                    prc = &rc[setcnt++];
                    prc->row = row;
                    prc->col = col;
                    // prc->cnt = 2;
                    COPY_SET(&prc->set,ps);
                    // sprintf(EndBuf(tb),"R%dC%d ", row + 1, col + 1);
                }
            }
            // done BOX
            sprintf(EndBuf(tb)," sc %d ",setcnt);
            // UGH this has to be if (setcnt >= 2), and EACH pair must be processed
            // to search for the 3rd in ALL boxes
            if (setcnt >= 2) {
                // in box got TWO, or more cells, each with 2 candidates...
                PROWCOL prc1, prc2;
                for (i = 0; i < setcnt; i += 2) {
                    prc1 = &rc[i];
                    if ((i + 1) < setcnt)
                        prc2 = &rc[i+1];
                    else
                        prc2 = &rc[0];
                    count += Find_YWing_3rd(pb, prc1 , prc2, br, bc);
                    if (count && only_one_Y)
                        break;
                }
            } else {
                if (add_debug_yw) OUTIT(tb);
            }
            if (count && only_one_Y)
                break;
        }   // for each Box col - for (c = 0; c < 3; c++)
        if (count && only_one_Y)
            break;
    }   // for each Box row - for (r = 0; r < 3; r++)


YWing_Exit:
    if (count) {
        sprtf("S2e: Y-Wing scan, elim %d. To %d\n", count,
            sg_Fill_XWing);
        pb->iStage = sg_Fill_YWing;
    } else {
        sprtf("S2e: Y-Wing scan. No elims. To begin.\n");
        pb->iStage = sg_Begin;
    }
    return count;
}

int Do_YWing_Scan_nearly_OK(PABOX2 pb)
{
    int count = 0;
    int row, col, val;
    int row2, col2, scnt;
    PSET ps, ps2, ps3, ps4;
    SET comm, comm2, comm3;
    char *tb = GetNxtBuf();
    int br, bc;
    int brw, bcl;
    int i;
    int setcnt = 0;
    int box, box2;
    ROWCOL rc[9+2];
    PROWCOL prc;

    xwing_scans++;
    if (add_debug_yw) sprtf("Doing YWing scan %d\n", xwing_scans);
    // classic Y-Wing - rectangle
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (!val) {
                ps = &pb->line[row].set[col];
                if (Get_Set_Cnt(ps) == 2) {
                    // time to look for the 2nd member in same ROW
                    for (col2 = col + 1; col2 < 9; col2++) {
                        val = pb->line[row].val[col2];
                        if (!val) {
                            ps2 = &pb->line[row].set[col2];
                            // primary cell - get SHARED into comm
                            Zero_SET(&comm);
                            scnt = Get_Shared_SET( ps, ps2, &comm );
                            if ((Get_Set_Cnt(ps2) == 2) && (scnt == 1)) {
                                // time to look for the 3rd member in same COL
                                for (row2 = row + 1; row2 < 9; row2++) {
                                    val = pb->line[row2].val[col];
                                    if (!val) {
                                        ps3 = &pb->line[row2].set[col];
                                        Zero_SET(&comm2);
                                        scnt = Get_Shared_SET( ps, ps3, &comm2 );
                                        if ((Get_Set_Cnt(ps3) == 2) && (scnt == 1) &&
                                            !Equal_SET(&comm, &comm2) ) { // 1-2 & 1-3 do NOT hav SAME shared
                                            // Yikes getting close
                                            Zero_SET(&comm3);
                                            scnt = Get_Shared_SET( &comm, &comm2, &comm3 );
                                            if (scnt == 1) {
                                                // can eliminate this comm3 at the intersection
                                                // of the column of 2, and the row of 3
                                                val = pb->line[row2].val[col2];
                                                if (!val) {
                                                    ps4 = &pb->line[row2].set[col2];
                                                    Get_Set_Cnt(&comm3,&val);
                                                    if (val) {
                                                        // got value to MARK
                                                        if (ps4->val[val - 1]) {
                                                            ps4->flag[val - 1] |= cf_YWE;
                                                            count++;
                                                            ps->cellflg |= cf_YWC;
                                                            ps2->cellflg |= cf_YWC;
                                                            ps3->cellflg |= cf_YWC;
                                                            ps->flag[val - 1] |= cf_YW;
                                                            ps2->flag[val - 1] |= cf_YW;
                                                            ps3->flag[val - 1] |= cf_YW;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (count && only_one_Y)
        goto YWing_Exit;

    // Now to do the more generalised case
    // work box by box
    //int r, c, rw, cl, i;
    for (br = 0; br < 3; br++) {
        for (bc = 0; bc < 3; bc++) {
            // Do a BOX
            setcnt = 0;
            box = (br * 3) + bc + 1;
            sprintf(tb,"Box%d ",box);
            for (brw = 0; brw < 3; brw++) {
                for (bcl = 0; bcl < 3; bcl++) {
                    row = (br * 3) + brw;
                    col = (bc * 3) + bcl;
                    val = pb->line[row].val[col];
                    ps = &pb->line[row].set[col];
                    if (!val) {
                        if (Get_Set_Cnt(ps) == 2) {
                            prc = &rc[setcnt++];
                            prc->row = row;
                            prc->col = col;
                            // prc->cnt = 2;
                            COPY_SET(&prc->set,ps);
                            // sprintf(EndBuf(tb),"R%dC%d ", row + 1, col + 1);
                        }
                    }
                }
            }
            // done BOX
            sprintf(EndBuf(tb)," sc %d ",setcnt);
            // UGH this has to be if (setcnt >= 2), and EACH pair must be processed
            // to search for the 3rd in ALL boxes
            if (setcnt == 2) {
                // in box got TWO cells each with 2 candidates
                row = rc[0].row;
                col = rc[0].col;
                row2 = rc[1].row;
                col2 = rc[1].col;
                ps = &pb->line[row].set[col];
                ps2 = &pb->line[row2].set[col2];
                // do they SHARE 1 common candidate
                Zero_SET(&comm);
                scnt = Get_Shared_SET( &rc[0].set, &rc[1].set, &comm );
                if ((scnt == 1) && !(row == row2) && !(col == col2)) {
                    sprintf(EndBuf(tb),"R%dC%d v ", row + 1, col + 1);
                    AddSet2Buf(tb,ps);
                    sprintf(EndBuf(tb)," R%dC%d v ", row2 + 1, col2 + 1);
                    AddSet2Buf(tb,ps2);
                    strcat(tb," shr ");
                    AddSet2Buf(tb,&comm);
                    OUTITYW(tb);
                    // they shared 1 candidate, and NOT in the same row or column
                    // time to try to find a 3rd, in another box, in the same row or col set
                    // as these, also has 2 cands, sharing 1, but not same one ad 1 & 2 share
                    int br2, bc2;
                    // These are the row/col pair
                    for (br2 = 0; br2 < 3; br2++) {
                        for (bc2 = 0; bc2 < 3; bc2++) {
                            if ((br == br2) && (bc == bc2)) continue; // skip current BOX
                            // only interested in SAME row or col
                            int rw2, cl2;
                            int row3, col3;
                            setcnt = 2;
                            // test another box
                            box2 = (br2 * 3) + bc2 + 1;
                            sprintf(tb,"B%d ", box2);
                            for (rw2 = 0; rw2 < 3; rw2++) {
                                for (cl2 = 0; cl2 < 3; cl2++) {
                                    row3 = (br2 * 3) + rw2;
                                    col3 = (bc2 * 3) + cl2;
                                    if ((row3 == row) || (row3 == row2) || (col3 == col) || (col3 == col2)) {
                                        // interest - found same row or col
                                        val = pb->line[row3].val[col3];
                                        ps3 = &pb->line[row3].set[col3];
                                        if (!val && (Get_Set_Cnt(ps3) == 2)) {
                                            prc = &rc[setcnt++];
                                            prc->row = row3;
                                            prc->col = col3;
                                            // prc->cnt = 2;
                                            COPY_SET(&prc->set,ps3);
                                        }
                                    }
                                }
                            }
                            // done another box
                            sprintf(EndBuf(tb)," [%d] ", setcnt - 2);
                            if (setcnt > 2) {
                                // we got some cells to CHECK
                                // each has 2 candidate - but do they share 1 with the 2 primaries
                                int scnt1, scnt2;
                                for (i = 2; i < setcnt; i++) {
                                    Zero_SET(&comm2);
                                    Zero_SET(&comm3);
                                    row3 = rc[i].row;
                                    col3 = rc[i].col;
                                    ps3 = &pb->line[row3].set[col3];
                                    sprintf(EndBuf(tb)," %d: ", i - 1);
                                    sprintf(EndBuf(tb),"r%dc%d ", row3 + 1, col3 + 1);
                                    AddSet2Buf(tb,ps3);
                                    scnt1 = Get_Shared_SET( ps, ps3, &comm2 );
                                    sprintf(EndBuf(tb)," s%d ", scnt1);
                                    AddSet2Buf(tb,&comm2);
                                    scnt2 = Get_Shared_SET( ps2, ps3, &comm3 );
                                    sprintf(EndBuf(tb)," s%d ", scnt2);
                                    AddSet2Buf(tb,&comm3);
                                    if ((scnt1 == 1) && (scnt2 == 1) &&
                                        !Same_SET( &comm2, &comm3 )) {
                                        // Yowee - have a 3rd cell, in a different Box
                                        // sharing 1 each with the two primaries
                                        int nval = 0;
                                        if (row3 == row) {
                                            // TODO - check eliminate comm3 shared in this BOX
                                            Get_Set_Cnt(&comm3,&nval);
                                        } else if (row3 == row2) {
                                            // TODO - check eliminate comm2 shared in this BOX
                                            Get_Set_Cnt(&comm2,&nval);
                                        } else if (col3 == col) {
                                            // eliminate comm3 shared in this BOX
                                            Get_Set_Cnt(&comm3,&nval);
                                        } else if (col3 == col2) {
                                            // eliminate comm2 shared in this BOX
                                            Get_Set_Cnt(&comm2,&nval);
                                        } else {
                                            sprtf("YWing LOGIC ERROR - FIX ME!\n");
                                        }
                                        if (nval) {
                                            int row4, col4;
                                            for (rw2 = 0; rw2 < 3; rw2++) {
                                                for (cl2 = 0; cl2 < 3; cl2++) {
                                                    row4 = (br2 * 3) + rw2;
                                                    col4 = (bc2 * 3) + cl2;
                                                    if ((row4 == row3) && (col4 == col3)) 
                                                        continue; // Avoid elim in SELF
                                                    if ( !((row4 == row) || (col4 == col)||
                                                           (row4 == row2)||(col4 == col2)) )
                                                        continue; // MUST share a ROW or COL
                                                    val = pb->line[row4].val[col4];
                                                    ps4 = &pb->line[row4].set[col4];
                                                    if (!val) {
                                                        if (ps4->val[nval - 1]) {
                                                            ps4->flag[nval - 1] |= cf_YWE;
                                                            count++;
                                                            ps->cellflg |= cf_YWC;
                                                            ps2->cellflg |= cf_YWC;
                                                            ps3->cellflg |= cf_YWC;
                                                            ps->flag[nval - 1] |= cf_YW;
                                                            ps2->flag[nval - 1] |= cf_YW;
                                                            ps3->flag[nval - 1] |= cf_YW;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if (count && only_one_Y)
                                        break;
                                }
                            }
                            if (count && only_one_Y)
                               break;
                        }   // on pair process all other boxes - for (c2 = 0; c2 < 3; c2++)
                        if (count && only_one_Y)
                           break;
                    }   // on pair process all other boxes - for (r2 = 0; r2 < 3; r2++)
                }   // if ((scnt == 1) && !(row == row2) && !(col == col2))
            }
            OUTITYW(tb);
            if (count && only_one_Y)
                break;
        }   // for each Box col - for (c = 0; c < 3; c++)
        if (count && only_one_Y)
            break;
    }   // for each Box row - for (r = 0; r < 3; r++)


YWing_Exit:
    if (count) {
        sprtf("S2e: Y-Wing scan, elim %d. To %d\n", count,
            sg_Fill_XWing);
        pb->iStage = sg_Fill_YWing;
    } else {
        sprtf("S2e: Y-Wing scan. No elims. To begin.\n");
        pb->iStage = sg_Begin;
    }
    return count;
}

int Do_Fill_YWing(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_YWE, cf_YW, "YW", "Y-Wing", cf_YWC );
    return count;
}

int UnSet_Cell_Flag( PABOX2 pb, int row, int col, uint64_t flag )
{
    int count = 0;
    if ( pb->line[row].set[col].cellflg & flag ) {
        pb->line[row].set[col].cellflg &= ~flag;
        count++;
    }
    return count;
}

int Do_Fill_UniqRect_NOT_COMPLETE( PABOX2 pb )
{
    int count = 0;
    int row, col;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            count += UnSet_Cell_Flag(pb,row,col, cl_UNR);
        }
    }
    Set_Rect_Link_Cnt(0);
    return count;
}


bool Any_of_SET1_in_SET2 ( PSET ps1, PSET ps2 )
{
    int i, setval;
    for (i = 0; i < 9; i++) {
        setval = ps1->val[i];
        if (!setval) continue;
        if (ps2->val[setval - 1])
            return true; // YES, a SET1 value is in SET2
    }
    return false; // NONE of SET1 in SET2
}

int Add_Strong_Link_Pair( PABOX2 pb, char *tb, PROWCOL prc, PROWCOL prc2,
                          vRC &sl_pairs )
{
    int count = 0;
    int row, col, val;
    bool bad;
    PSET ps;
    if (Get_Set_Cnt(&prc->set) != 2) return 0;
    if (Get_Set_Cnt(&prc2->set) != 2) return 0;
    if (!Same_SET(&prc->set, &prc2->set)) return 0;
    // Established they share the SAME 2 candidates - check if they share a ROW or COL
    bad = true;
    if (prc->row == prc2->row) {
        // same ROW - check others in ROW
        row = prc->row; // fixed ROW
        bad = false;
        for (col = 0; col < 9; col++) {
            // scan COL
            if (col == prc->col) continue; // skip me
            if (col == prc2->col) continue; // and him
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if (Any_of_SET1_in_SET2(ps,&prc->set)) {
                bad = true; // Yow, found another cell sharing a candidate
                break; // not a strong link
            }
        }

    } else if (prc->col == prc2->col) {
        // same COLUM
        col = prc->col; // fixed COL
        bad = false;
        for (row = 0; row < 9; row++) {
            // scan ROW
            if (row == prc->row) continue; // skip me
            if (row == prc2->row) continue; // and him
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if (Any_of_SET1_in_SET2(ps,&prc->set)) {
                bad = true; // Yow, found another cell sharing a candidate
                break; // not a strong link
            }
        }
    }
    if (!bad) {
        AddSet2Buf(tb,&prc->set);
        sprintf(EndBuf(tb),"@%c%d==",
            (char)(prc->row + 'A'), prc->col + 1);
        AddSet2Buf(tb,&prc->set);
        sprintf(EndBuf(tb),"@%c%d ",
            (char)(prc2->row + 'A'), prc2->col + 1);
        // ADD this PAIR to the STRONG LINK pairs
        sl_pairs.push_back( *prc );
        sl_pairs.push_back( *prc2 );
        count++;
    }

    return count;
}

int Add_Strong_Links_for_Pairs( PABOX2 pb, char *tb, PROWCOL prc, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4,
                                vRC &sl_pairs )
{
    int count = 0;
    count += Add_Strong_Link_Pair( pb, tb, prc, prc2, sl_pairs );
    count += Add_Strong_Link_Pair( pb, tb, prc, prc3, sl_pairs );
    count += Add_Strong_Link_Pair( pb, tb, prc, prc4, sl_pairs );
    count += Add_Strong_Link_Pair( pb, tb, prc2, prc3, sl_pairs );
    count += Add_Strong_Link_Pair( pb, tb, prc2, prc4, sl_pairs );
    count += Add_Strong_Link_Pair( pb, tb, prc3, prc4, sl_pairs );
    return count;
}


int Do_Fill_Single( PABOX2 pb )
{
    int count = 0;
    int row, col, val, nval, cnt;
    PSET ps;

    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            ps = &pb->line[row].set[col];
            val = pb->line[row].val[col];
            if (!val) {
                cnt = Get_Set_Cnt(ps,&nval);
                if (cnt == 1) {
                    Change_Box(pb, row, col, nval);
                    count++;
                }
            }
        }
    }
    return count;
}

int Do_Mark_Single( PABOX2 pb )
{
    int count = 0;
    int row, col, val, nval, cnt;
    PSET ps;

    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            ps = &pb->line[row].set[col];
            val = pb->line[row].val[col];
            if (!val) {
                cnt = Get_Set_Cnt(ps,&nval);
                if (cnt == 1) {
                    count++;
                }
            }
        }
    }
    return count;
}

int Do_Single_Scan(PABOX2 pb)
{
    int count = Do_Mark_Single(pb);
    if (count) {
        sprtf("S2a: Marked SINGLE values %d - To Fill\n", count);
        pb->iStage = sg_Fill_Singles;  // reset to do more eliminations
    } else {
        sprtf("S2a: Marked SINGLE values None.\n", count);
    }
    return count;
}

// Mark SINGLES - Unique in Row, Column or Box, or a combination
// Where the previous eliminations have left just ONE unique 
// single candidate in ROW, COL, BOX
int Do_Unique_Scan(PABOX2 pb)
{
    int count = 0;
    int i, row, col, val, cnt, nval;
    uint64_t result;
    PSET ps;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            cnt = Get_Set_Cnt(ps);
            if (!cnt) continue; // this WOULD BE AN ERROR!!!
            // This count is NOT one, else would have done that above
            for (i = 0; i < 9; i++) {
                nval = ps->val[i];  // extract candidate
                if (!nval) continue; // forget zero values
                // QUESTION: is this nval (setval) UNIQUE in this ROW, COL or BOX
                result = 0;
                if ( Unique_In_Row( pb, row, col, nval ) )
                    result |= cf_URF; // is UNIQUE in ROW
                if ( Unique_In_Col( pb, row, col, nval ) )
                    result |= cf_UCF; // is UNIQUE in COL
                if ( Unique_In_Box( pb, row, col, nval ) )
                    result |= cf_UBF; // is UNIQUE in BOX
                if (result) {
                    pb->line[row].set[col].flag[i] |= result;
                    pb->line[row].set[col].cellflg |= result;
                    count++;
                }
            }
        }
    }
    if (count) {
        sprtf("S2b: Marked Unique RCB - %d. To fill\n", count); // , sg_Fill_Unique);
        pb->iStage = sg_Fill_Unique; // set stage 3 to enter these Marked unique values
    } else {
        sprtf("S2b: Marked Unique RCB - None.\n");
    }
    return count;
}


int Do_Simple_Scan(PABOX2 pb)
{
    int row, col, count;
    PSET ps;
    count = 0;
    if (!pb)
        pb = get_curr_box();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            ps = &pb->line[row].set[col];
            // stage ONE also removes any flags
            //pb->line[row].set[col].cellflg &= ~cl_Perm;  // clear all except some PERSISTANT flags
            //memset(&pb->line[row].set[col].flag,0,(sizeof(int) * 9)); // DO I want to do THIS????
            // like pb->line[row].set[col].flag[i] &= ~cf_KEEP;, but for NOW clear
            // pb->line[row].set[col].uval = 0; // WHY clear this here ?????????
            count += Mark_Same_Col( pb, col, ps, cf_SSE );
            count += Mark_Same_Row( pb, row, ps, cf_SSE );
            count += Mark_Same_Box( row, col, pb, ps, cf_SSE );
        }
    }
    if (count) {
        sprtf("S%d: Elim same row,col,box [%d] To Fill\n", sg_One, count, sg_Fill_Simple);
        pb->iStage = sg_Fill_Simple;
    } else {
        sprtf("S%d: NONE Elim same row,col,box To %d\n", sg_One, count, sg_Two);
        pb->iStage = sg_Two;
    }
    return count;
}

/* -----------------------------------------------
    { "Naked Pairs",IDC_CHECK1, &add_debug_np, &bChgDNP      , Do_Naked_Pairs, &Do_NP, &Chg_NP          },  // 0
    { "Color Scan",IDC_CHECK2,  &add_debug_cs2, &bChgDCS2    , Do_Color_Scan2, &Do_CS2, &Chg_CS2        },  // 1
    { "Pointing Pairs",IDC_CHECK3,&add_debug_PP2, &bChgDPP2  , Do_Pointing_Pairs, &Do_PP, &Chg_PP       },  // 2
    { "Locked Exclusion",IDC_CHECK4,&add_debug_lk, &bChgDLK  , Do_Locked_Excl, &Do_LE, &Chg_LE          },  // 3
    { "X-Wings",IDC_CHECK5,&add_debug_xw, &bChgDXW           , Do_XWing_Scan, &Do_XW, &Chg_XW           },  // 4
    { "Y-Wings",IDC_CHECK6,&add_debug_yw, &bChgDYW           , Do_YWing_Scan, &Do_YW, &Chg_YW           },  // 5
    { "Unique Rectangle",IDC_CHECK7,&add_debug_ur, &bChgDUR  , Do_UniqRect_Scan, &Do_UR, &Chg_UR        },  // 6
    { "X-Cycles",IDC_CHECK8,&add_debug_xc, &bChgDXC          , Do_XCycles_Scan, &Do_XC, &Chg_XC         },  // 7
    { "Hidden Triples",IDC_CHECK9,&add_debug_ht, &bChgDHT    , Do_Hidden_Triple_Scan, &Do_HT, &Chg_HT   },  // 8
    { "XY-Chains",IDC_CHECK10,&add_debug_xy, &bChgDXY        , Do_XY_Chains, &Do_XY, &Chg_XY            },  // 9
    { "Strong Links",IDC_CHECK11,&add_debug_sl, &bChgDSL     , Do_Strong_Links_Scan, &Do_SL, &Chg_SL    },  // 10
    { "Strong Links 2",IDC_CHECK12,&add_debug_sl2, &bChgDSL2 , Do_Strong_Links2_Scan, &Do_SL2, &Chg_SL2 },  // 11
    { "Fill by Flags",IDC_CHECK13,&add_debug_ff, &bChgDFF    , 0, 0, 0                                  },  // 12
    { "Paint Rect. Fill",IDC_CHECK14,&add_debug_pfr,&bChgPFR , 0, 0, 0                                  },  // 13
    { "X-Cycles 3",IDC_CHECK15,&add_debug_xc3, &bChgDXC3     , Do_XCycles_Scan3, &Do_XC3, &Chg_XC3      },  // 14
    { "Paint XY-Chains 2",IDC_CHECK16,&add_debug_pxy2, &bChgPXY2, 0, 0, 0                               },  // 15
    { "AIC",IDC_CHECK17,&add_debug_aic, &bChgDAIC            , Do_AIC_Scans, &Do_AIC, &Chg_AIC          },  // 16
    { "Paint AIC",IDC_CHECK18,&add_debug_paic, &bChgPAIC     , 0, 0, 0                                  },  // 17
    { "XYZ-Scan",IDC_CHECK19,&add_debug_xyz, &bChgDXYZ       , Do_XYZ_Wings, &Do_XYZ, &Chg_XYZ          },  // 18
    { "Strong Links 3",IDC_CHECK20,&add_debug_sl3, &bChgDSL3 , Do_Strong_Link3_Scan, &Do_SL3, &Chg_SL3  },  // 19
   ----------------------------------------------- */

// This could be done run per the above TABLE
int Do_Stage_Two(PABOX2 pb)
{
    int count = 0;
    if (!pb)
        pb = get_curr_box();
    count = Do_Single_Scan(pb);
    if (!count) {
        count = Do_Unique_Scan(pb); // Mark SINGLES - Unique in Row, Column or Box, or a combination
    }
    if (!count && Do_NP) {
        count = Do_Naked_Pairs(pb);
    }
#if 0 // this is NOT working properly yet
    if (!count) {
        count = Do_Hidden_Pairs(pb);
    }
#endif // 0
    if (!count && Do_LE) {
        count = Do_Locked_Excl(pb);
    }
    if (!count && Do_PP) {
        count = Do_Pointing_Pairs(pb);
    }
    if (!count && Do_XW) {
        count = Do_XWing_Scan(pb);
    }
    if (!count && Do_YW) {
        count = Do_YWing_Scan(pb);
    }
    if (!count && Do_HT) {
        count = Do_Hidden_Triple_Scan(pb);
    }
    if (!count && Do_XY) {
        count = Do_XY_Chains(pb);
    }
    if (!count && Do_SL2) {
        count = Do_Strong_Links2_Scan(pb);
    }
    if (!count && Do_SL3) {
        count = Do_Strong_Link3_Scan(pb);
    }
    if (!count && Do_SL) {
        count = Do_Strong_Links_Scan(pb);
    }
    if (!count && Do_AIC) {
        count = Do_AIC_Scans(pb);
    }
    if (!count && Do_XYZ) {
        count = Do_XYZ_Wings(pb);
    }
    if (!count && Do_UR) {
        count = Do_UniqRect_Scan(pb);
    }
    if (!count && Do_CS2) {
#ifdef USE_COLOR2_SCAN
        count = Do_Color_Scan2(pb);
#else // !USE_COLOR2_SCAN
        count = Do_Color_Scan(pb);
#endif // USE_COLOR2_SCAN y/n
     }
    if (!count) {
        sprtf("WARNING: NO ELIMINATIONS FOUND!\n");
    }
    return count;
}



int Do_ID_OPTIONS_ONESTEP( HWND hWnd )
{
    int count = 0;
    PABOX2 pb = get_curr_box();
    PABOX2 copy_box = Copy_Box2(pb);    // get copy of BOX
    Do_Fill_Stack(pb);  // clear any stacked items
    Set_Paint_Chains_Invalid(); // STOP PAINTING PAIRS, CHAINS, etc 
    switch( pb->iStage )
    {
    case sg_Begin:
    case sg_One:
        count = Do_Simple_Scan(pb);
        if (!count) {
            Do_Stage_Two(pb);
        }
        break;
    case sg_Two:
        count = Do_Stage_Two(pb);
        break;
    case sg_Fill_Singles:
        count = Do_Fill_Single(pb);
        break;
    case sg_Fill_Unique:
        count = Do_Fill_Unique(pb); // Put items marked 'unique' into Box
        break;
    case sg_Fill_Naked:
        count = Do_Fill_Naked(pb);
        break;
    case sg_Fill_Hidden:
        count = Do_Fill_Hidden(pb);
        break;
    case sg_Fill_Locked:
        count = Do_Fill_Locked(pb);
        break;
    case sg_Fill_XWing:
        count = Do_Fill_XWing(pb);
        break;
    case sg_Fill_Simple:
        count = Do_Fill_Simple(pb);
        break;
    case sg_Fill_Pointers:
        count = Do_Fill_Pointing(pb);
        break;
    case sg_Fill_YWing:
        count = Do_Fill_YWing(pb);
        break;
    case sg_Fill_Color:
        count = Do_Fill_Color2(pb);
        break;
    case sg_Fill_Rect:
        count = Do_Fill_UniqRect(pb);
        break;
    case sg_Fill_NakedTrip:
        count = Do_Fill_NakedTriple(pb);
        break;
    case sg_Fill_NakedQuad:
        count = Do_Fill_NakedQuad(pb);
        break;
    case sg_Fill_HidTrip:
        count = Do_Fill_HidTrip(pb);
        break;
    case sg_Fill_XYChains:
        count = Do_Fill_XYChains(pb);
        break;
    case sg_Fill_SLChains:
        count = Do_Fill_Strong_Links(pb);
        break;
    case sg_Fill_SLPairs:
        count = Do_Fill_Links(pb);
        break;
    case sg_Fill_SLPairs3:
        count = Do_Fill_Links3(pb);
        break;
    case sg_Fill_AIC:
        count = Do_Fill_AIC(pb);
        break;
    case sg_Fill_XYZWing:
        count = Do_Fill_XYZWing(pb);
        break;
    }
    // Mark_HighLight_Cells(false);
    //if (g_bShwStLnks) Do_Strong_Links(get_curr_box());
    if (memcmp(pb,copy_box,sizeof(ABOX2)))
        Stack_Box(copy_box);
    else
        free(copy_box);
    set_repaint();
    return count;
}


// A STRONG link is where a single candidate is ONLY exists twice in 
// any ROW, COL, or BOX - draw a LINK beween these candidates
vRC vStrong_Links;
vRC *Get_Strong_Links() { return &vStrong_Links; }

int Just_in_Rows( PABOX2 pb, int mrow, int mcol, int setval, vRC &pairs )
{
    int count = 0;
    int row, col, val;
    PSET ps;
    ROWCOL rc;
    col = mcol; // fix COL, scan ROWS
    for (row = 0; row < 9; row++) {
        if (row == mrow) continue; // skip me
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        if (!Value_in_SET(setval,ps)) continue;
        rc.row = row;
        rc.col = col;
        ZERO_SET(&rc.set);
        rc.set.val[setval - 1] = setval;
        pairs.push_back(rc);    // save the PAIR
        count++;
    }
    return count;
}

int Just_in_Cols( PABOX2 pb, int mrow, int mcol, int setval, vRC &pairs )
{
    int count = 0;
    int row, col, val;
    PSET ps;
    ROWCOL rc;
    row = mrow; // fix ROW, scan COLS
    for (col = 0; col < 9; col++) {
        if (col == mcol) continue; // skip me
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        if (!Value_in_SET(setval,ps)) continue;
        rc.row = row;
        rc.col = col;
        ZERO_SET(&rc.set);
        rc.set.val[setval - 1] = setval;
        pairs.push_back(rc);    // save the PAIR
        count++;
    }
    return count;
}

int Just_in_Box( PABOX2 pb, int mrow, int mcol, int setval, vRC &pairs )
{
    int count = 0;
    int row, col, val;
    PSET ps;
    int r = (mrow / 3) * 3;
    int c = (mcol / 3) * 3;
    int rw, cl;
    ROWCOL rc;
    // scan BOX
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((col == mcol)&&(row == mrow)) continue; // skip me
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            if (!Value_in_SET(setval,ps)) continue;
            rc.row = row;
            rc.col = col;
            ZERO_SET(&rc.set);
            rc.set.val[setval - 1] = setval;
            pairs.push_back(rc);    // save the PAIR
            count++;
        }
    }
    return count;
}

int Do_Strong_Links( PABOX2 pb )
{
    int count = 0;
    int row, col, val, scnt;
    int i, setval;
    int setvals[9];
    PSET ps;
    vRC *pvsl = Get_Strong_Links();
    vRC pairs;
    ROWCOL rc, rc2;
    size_t j;
    pvsl->clear();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            scnt = Get_Set_Cnt2(ps, setvals);
            for (i = 0; i < scnt; i++) {
                setval = setvals[i];
                if (!Show_SL_Value(setval)) continue;

                pairs.clear();
                rc.row = row;
                rc.col = col;
                ZERO_SET(&rc.set);
                rc.set.val[setval - 1] = setval;
                if (Just_in_Rows( pb, row, col, setval, pairs ) == 1) {
                    // mark this STRONG link
                    rc.set.flag[setval - 1] |= cl_SLR;
                    for (j = 0; j < pairs.size(); j++) {
                        pvsl->push_back(rc);
                        rc2 = pairs[j];
                        rc2.set.flag[setval - 1] |= cl_SLR;
                        pvsl->push_back(rc2);
                        count++;
                    }
                }
                pairs.clear();
                if (Just_in_Cols( pb, row, col, setval, pairs ) == 1) {
                    // mark this
                    rc.set.flag[setval - 1] |= cl_SLC;
                    for (j = 0; j < pairs.size(); j++) {
                        pvsl->push_back(rc);
                        rc2 = pairs[j];
                        rc2.set.flag[setval - 1] |= cl_SLC;
                        pvsl->push_back(rc2);
                        count++;
                    }
                }
                pairs.clear();
                if (Just_in_Box( pb, row, col, setval, pairs ) == 1) {
                    // mark this
                    rc.set.flag[setval - 1] |= cl_SLB;
                    for (j = 0; j < pairs.size(); j++) {
                        pvsl->push_back(rc);
                        rc2 = pairs[j];
                        rc2.set.flag[setval - 1] |= cl_SLB;
                        pvsl->push_back(rc2);
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

// =================================================
// eof - Sudo_Logic.cxx
