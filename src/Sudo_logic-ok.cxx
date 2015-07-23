// Sudo_Logic.cxx
// Where the analysis happens ;=))

#include "Sudoku.hxx"
#include <map>
#include <utility>

bool done_count_warning = false;

int use_elim_on_square = 1; // this service seems SUSPECT - need to check MORE ;=(( MAYBE OK
#define AddSet2Buf Add_Set2Buf

#define ADDSP(a) strcat(a," ")
#define OUTIT(a) sprtf("%s\n",a); *a = 0

//#define COPY_SET(d,s) memcpy(d,s,sizeof(SET))
#define COPY_SET(d,s) memmove(d,s,sizeof(SET))
#define ZERO_SET(s) memset(s,0,sizeof(SET))

int Set_SET_per_flag(PSET from, PSET to, int flag )
{
    int i, cnt,val;
    cnt = 0;
    for (i = 0; i < 9; i++) {
        val = from->val[i];
        if (val) {
            if ( from->flag[i] & flag ) {
                to->val[val - 1] = val;
                cnt++;
            }
        }
    }
    return cnt;
}

void AddSet2BufExFlag( char *tb, PSET ps, int flag )
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

#define Equal_SET Same_SET
bool Same_SET(PSET ps1, PSET ps2)
{
    int i;
    for (i = 0; i < 9; i++) {
        if (ps1->val[i] != ps2->val[i])
            return false;
    }
    return true;
}

void Set_SET_flag( PSET ps2, int flag )
{
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps2->val[i];
        if (val)
            ps2->flag[i] |= flag;
    }
}

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

int Elim_SET_in_SET( PSET ps, PSET ps2, bool del = false );

int Elim_SET_in_SET( PSET ps, PSET ps2, bool del )
{
    int i, val, count;
    count =0;
    for (i = 0; i < 9; i++) {
        val = ps2->val[i];
        if (val) {
            if (ps->val[i]) {
                if (del) 
                    ps->val[i] = 0; // Eliminate
                ps->flag[i] |= cf_NPE;
                count++;
            }
        }
    }
    return count;
}

int Mark_SET_in_SET( PSET ps, PSET ps2, int flag, bool all = true );
//                      to      from      flag      all?
int Mark_SET_in_SET( PSET ps, PSET ps2, int flag, bool all )
{
    int i, val, count;
    count =0;
    for (i = 0; i < 9; i++) {
        val = ps2->val[i];
        if (val) {
            if (ps->val[i]) {
                // do NOT count repeated marks
                if ( all || !(ps->flag[i] & flag) ) {
                    ps->flag[i] |= flag;
                    count++;
                }
            }
        }
    }
    return count;
}

bool Value_in_SET( int val, PSET ps )
{
    int i;
    for (i = 0; i < 9; i++) {
        if (ps->val[val - 1])
            return true;   // value is IN SET
    }
    return false; // Value is NOT in SET
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

#define INVALID_VAL 0

void invalidate_box(PABOX2 pb)
{
    int row, col;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            pb->line[row].val[col] = INVALID_VAL;
            Fill_SET( &pb->line[row].set[col] );
            // clear ALL potential value flags
            memset(&pb->line[row].set[col].flag,0,(sizeof(int) * 9));
            pb->line[row].set[col].uval = 0;    // remove UNIQUE
            pb->line[row].set[col].cellflg = 0;    // CLEAR ALL FLAGS
            pb->line[row].set[col].tm = 0;  // no TIME
        }
    }
    pb->iStage = 0;

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

#define VALUEVALID(v) ((v >= 1) && (v <= 9))

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
                    if (val != INVALID_VAL) {
                        sprintf(tb,"VAL ERR: R %d C %d INVALID [%d]",
                            (row + 1), (col + 1), val);
                        set_last_box_error(tb);
                    }
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
    // Do a SQUARES check
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
    if (count && (count < 18)) {
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
        prc->cnt = Get_Set_Cnt( &prc->set );
        sprintf(tb,"%d: row %d col %d has %d vals ", i+1, prc->row + 1, prc->col + 1, prc->cnt );
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
        sprintf(tb,"THIS: row %d col %d has %d vals ", prc->row + 1, prc->col + 1, prc->cnt );
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
            prc->cnt = Elim_All( crow, col2, pb, ps );
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

void Do_Simple_Scan(PABOX2 pb = 0);
void Do_Stage_Two(PABOX2 pb = 0);
void Do_Fill_Unique(PABOX2 pb = 0);
int Do_Locked_Excl( PABOX2 pb = 0);
int Do_XWing_Scan( PABOX2 pb = 0);
int Do_Pointer_Pairs(PABOX2 pb = 0);

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



bool Value_In_SET( PSET ps, int nval )
{
    int i;
    for (i = 0; i < 9; i++) {
        if (ps->val[i] == nval)
            return true;
    }
    return false;
}

bool Unique_In_Row( PABOX2 pb, int crow, int ccol, int nval )
{
    int col, val;
    PSET ps;
    for (col = 0; col < 9; col++) {
        if (col == ccol) continue; // skip SELF
        val = pb->line[crow].val[col];
        if (!val) {
            ps = &pb->line[crow].set[col];
            if ( Value_In_SET( ps, nval ) ) {
                return false;
            }
        }
    }
    return true;
}
bool Unique_In_Col( PABOX2 pb, int crow, int ccol, int nval )
{
    int row, val;
    PSET ps;
    for (row = 0; row < 9; row++) {
        if (row == crow) continue; // skip SELF
        val = pb->line[row].val[ccol];
        if (!val) {
            ps = &pb->line[row].set[ccol];
            if ( Value_In_SET( ps, nval ) )
                return false;
        }
    }
    return true;
}

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
            val = pb->line[row].val[ccol];
            if (!val) {
                ps = &pb->line[row].set[ccol];
                if ( Value_In_SET( ps, nval ) )
                    return false;
            }
        }
    }
    return true;
}

int SPI_debug = 0;

int Elim_SET( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del = false );

int Elim_SET_in_Row( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del = false );
int Elim_SET_in_Row( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del )
{
    int col, val, cnt;
    PSET ps;
    int count = 0;
    for (col = 0; col < 9; col++) {
        if ((ccol1 == col)||(ccol2 == col)) continue; // skip PAIR
        val = pb->line[crow1].val[col];
        if (!val) {
            ps = &pb->line[crow1].set[col];
            char *tb = GetNxtBuf();
            sprintf(tb," COLS row %d col %d vals ", crow1+1, col+1);
            Add_Set2Buf(tb, ps);
            cnt = Elim_SET_in_SET( ps, ps2, del );
            if (cnt) {
                sprintf(EndBuf(tb)," ae %d ",cnt);
                Add_Set2Buf(tb, ps);
            } else {
                strcat(tb, " none");
            }
            if (SPI_debug) sprtf("%s\n",tb);
            count += cnt;
        }
    }
    return count;
}

int Elim_SET_in_Col( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del = false );
int Elim_SET_in_Col( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del )
{
    int row, val, cnt;
    PSET ps;
    int count = 0;
    // elim same COLUMN
    for (row = 0; row < 9; row++) {
        if ((crow1 == row)||(crow2 == row)) continue; // skip PAIR
        val = pb->line[row].val[ccol1];
        if (!val) {
            ps = &pb->line[row].set[ccol1];
            char *tb = GetNxtBuf();
            sprintf(tb," ROWS row %d col %d vals ", row+1, ccol1+1);
            Add_Set2Buf(tb, ps);
            cnt = Elim_SET_in_SET( ps, ps2, del );
            if (cnt) {
                sprintf(EndBuf(tb)," ae %d ",cnt);
                Add_Set2Buf(tb, ps);
            } else {
                strcat(tb, " none");
            }
            if (SPI_debug) sprtf("%s\n",tb);
            count += cnt;
        }
    }
    return count;
}

int Elim_SET_in_Box( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del = false );
int Elim_SET_in_Box( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del )
{
    int row, col, val, cnt;
    PSET ps;
    int count = 0;
    int r, c, rw, cl;
    r = (crow1 / 3) * 3;
    c = (ccol1 / 3) * 3;
    char *tb = GetNxtBuf();
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((crow1 == row) && (ccol1 == col)) continue; // skip this PAIR
            if ((crow2 == row) && (ccol2 == col)) continue; // skip this PAIR
            val = pb->line[row].val[col];
            if (!val) {
                ps = &pb->line[row].set[col];
                sprintf(tb," BOX1 row %d col %d vals ", row+1, col+1);
                Add_Set2Buf(tb, ps);
                cnt = Elim_SET_in_SET( ps, ps2, del );
                count += cnt;
                if (cnt) {
                    sprintf(EndBuf(tb)," ae %d ",cnt);
                    Add_Set2Buf(tb, ps);
                } else {
                    strcat(tb, " none");
                }
                if (SPI_debug) sprtf("%s\n",tb);
            }
        }
    }
    if ( (((crow2 / 3) * 3) != r) ||
         (((ccol2 / 3) * 3) != c) )
    {
        // in a DIFFERENT BOX
        r = (crow2 / 3) * 3;
        c = (ccol2 / 3) * 3;
        for (rw = 0; rw < 3; rw++) {
            for (cl = 0; cl < 3; cl++) {
                row = r + rw;
                col = c + cl;
                if ((crow1 == row) && (ccol1 == col)) continue; // skip this PAIR
                if ((crow2 == row) && (ccol2 == col)) continue; // skip this PAIR
                val = pb->line[row].val[col];
                if (!val) {
                    ps = &pb->line[row].set[col];
                sprintf(tb," BOX2 row %d col %d vals ", row+1, col+1);
                Add_Set2Buf(tb, ps);
                cnt = Elim_SET_in_SET( ps, ps2, del );
                if (cnt) {
                    sprintf(EndBuf(tb)," ae %d ",cnt);
                    Add_Set2Buf(tb, ps);
                } else {
                    strcat(tb, " none");
                }
                if (SPI_debug) sprtf("%s\n",tb);
                count += cnt;
                }
            }
        }
    }
    return count;
}

int Elim_SET_in_ALL( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, bool del )
{
    int row, col, val, count, cnt;
    PSET ps;
    char *tb;

    // elim in same ROW
    tb = GetNxtBuf();
    strcpy(tb," Elim ");
    Add_Set2Buf(tb, ps2);
    sprintf(tb," excluding: row %d col %d row %d col %d vals ",
        crow1 + 1, ccol1 + 1,
        crow2 + 1, ccol2 + 1);
    sprtf("%s\n",tb);

    count = 0;
    for (col = 0; col < 9; col++) {
        if ((ccol1 == col)||(ccol2 == col)) continue; // skip PAIR
        val = pb->line[crow1].val[col];
        if (!val) {
            ps = &pb->line[crow1].set[col];
            sprintf(tb," COLS row %d col %d vals ", crow1+1, col+1);
            Add_Set2Buf(tb, ps);
            cnt = Elim_SET_in_SET( ps, ps2, del );
            if (cnt) {
                sprintf(EndBuf(tb)," ae %d ",cnt);
                Add_Set2Buf(tb, ps);
            } else {
                strcat(tb, " none");
            }
            sprtf("%s\n",tb);
            count += cnt;
        }
    }
    // elim same COLUMN
    for (row = 0; row < 9; row++) {
        if ((crow1 == row)||(crow2 == row)) continue; // skip PAIR
        val = pb->line[row].val[ccol1];
        if (!val) {
            ps = &pb->line[row].set[ccol1];
            sprintf(tb," ROWS row %d col %d vals ", row+1, ccol1+1);
            Add_Set2Buf(tb, ps);
            cnt = Elim_SET_in_SET( ps, ps2, del );
            if (cnt) {
                sprintf(EndBuf(tb)," ae %d ",cnt);
                Add_Set2Buf(tb, ps);
            } else {
                strcat(tb, " none");
            }
            sprtf("%s\n",tb);
            count += cnt;
        }
    }
    // eliminate in Box
    int r, c, rw, cl;
    r = (crow1 / 3) * 3;
    c = (ccol1 / 3) * 3;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((crow1 == row) && (ccol1 == col)) continue; // skip this PAIR
            if ((crow2 == row) && (ccol2 == col)) continue; // skip this PAIR
            val = pb->line[row].val[col];
            if (!val) {
                ps = &pb->line[row].set[col];
                sprintf(tb," BOX1 row %d col %d vals ", row+1, col+1);
                Add_Set2Buf(tb, ps);
                cnt = Elim_SET_in_SET( ps, ps2, del );
                count += cnt;
                if (cnt) {
                    sprintf(EndBuf(tb)," ae %d ",cnt);
                    Add_Set2Buf(tb, ps);
                } else {
                    strcat(tb, " none");
                }
                sprtf("%s\n",tb);
            }
        }
    }
    if ( (((crow2 / 3) * 3) != r) ||
         (((ccol2 / 3) * 3) != c) )
    {
        // in a DIFFERENT BOX
        r = (crow2 / 3) * 3;
        c = (ccol2 / 3) * 3;
        for (rw = 0; rw < 3; rw++) {
            for (cl = 0; cl < 3; cl++) {
                row = r + rw;
                col = c + cl;
                if ((crow1 == row) && (ccol1 == col)) continue; // skip this PAIR
                if ((crow2 == row) && (ccol2 == col)) continue; // skip this PAIR
                val = pb->line[row].val[col];
                if (!val) {
                    ps = &pb->line[row].set[col];
                sprintf(tb," BOX2 row %d col %d vals ", row+1, col+1);
                Add_Set2Buf(tb, ps);
                cnt = Elim_SET_in_SET( ps, ps2, del );
                if (cnt) {
                    sprintf(EndBuf(tb)," ae %d ",cnt);
                    Add_Set2Buf(tb, ps);
                } else {
                    strcat(tb, " none");
                }
                sprtf("%s\n",tb);
                count += cnt;
                }
            }
        }
    }
    return count;
}

int Seek_Same_Pair_In_Row( PABOX2 pb, int crow, int ccol, PSET ps2 )
{
    int col, val, count, fnd, cnt;
    PSET ps;
    char *tb = GetNxtBuf();
    sprintf(tb,"SPIROw: row %d col %d vals ", crow + 1, ccol + 1);
    Add_Set2Buf(tb, ps2);
    if (SPI_debug) sprtf("%s\n",tb);
    count = 0;
    fnd = 0;
    for (col = 0; col < 9; col++) {
        if (col == ccol) continue; // skip SELF
        val = pb->line[crow].val[col];
        if (!val) {
            ps = &pb->line[crow].set[col];
            if ( Equal_SET( ps, ps2 ) ) {
                tb = GetNxtBuf();
                sprintf(tb,"SPIRow: row %d col %d vals ", crow + 1, col + 1);
                Add_Set2Buf(tb, ps);
                strcat(tb," Found");
                if (SPI_debug) sprtf("%s\n",tb);
                cnt = Elim_SET_in_Row( pb, crow, ccol, crow, col, ps );
                if (cnt)
                    Set_SET_flag( ps2, cf_NP );
                fnd++;
            }
        }
    }
    if (!fnd) {
        tb = GetNxtBuf();
        sprintf(tb,"SPIROw: row %d col %d vals ", crow + 1, ccol + 1);
        Add_Set2Buf(tb, ps2);
        strcat(tb," No Finds.");
        if (SPI_debug) sprtf("%s\n",tb);

    }
    return count;
}

int Seek_Same_Pair_In_Col( PABOX2 pb, int crow, int ccol, PSET ps2 )
{
    int row, val, count, fnd, cnt;
    PSET ps;
    char *tb = GetNxtBuf();
    sprintf(tb,"SPICol: row %d col %d vals ", crow + 1, ccol + 1);
    Add_Set2Buf(tb, ps2);
    if (SPI_debug) sprtf("%s\n",tb);
    count = 0;
    fnd = 0;
    for (row = 0; row < 9; row++) {
        if (row == crow) continue; // skip SELF
        val = pb->line[row].val[ccol];
        if (!val) {
            ps = &pb->line[row].set[ccol];
            if ( Equal_SET( ps, ps2 ) ) {
                tb = GetNxtBuf();
                sprintf(tb,"SPICol: row %d col %d vals ", row + 1, ccol + 1);
                Add_Set2Buf(tb, ps);
                strcat(tb," Found");
                if (SPI_debug) sprtf("%s\n",tb);
                cnt = Elim_SET_in_Col( pb, crow, ccol, row, ccol, ps );
                count += cnt;
                if (cnt)
                    Set_SET_flag( ps2, cf_NP );
                fnd++;
            }
        }
    }
    if (!fnd) {
        tb = GetNxtBuf();
        sprintf(tb,"SPICol: row %d col %d vals ", crow + 1, ccol + 1);
        Add_Set2Buf(tb, ps2);
        strcat(tb," No finds");
        if (SPI_debug) sprtf("%s\n",tb);

    }
    return count;
}

int Seek_Same_Pair_In_Box( PABOX2 pb, int crow, int ccol, PSET ps2 )
{
    int row, col, val, count, fnd, cnt;
    PSET ps;
    char *tb = GetNxtBuf();
    sprintf(tb,"SPIBox: row %d col %d vals ", crow + 1, ccol + 1);
    Add_Set2Buf(tb, ps2);
    if (SPI_debug) sprtf("%s\n",tb);
    int r = (crow / 3) * 3;
    int c = (ccol / 3) * 3;
    int rw, cl;
    fnd = 0;
    count = 0;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = (r + rw);
            col = (c + cl);
            if ((crow == row) && (ccol == col)) continue; // Skip SELF in BOX
            val = pb->line[row].val[col];
            if (!val) {
                ps = &pb->line[row].set[col];
                if ( Equal_SET( ps, ps2 ) ) {
                    tb = GetNxtBuf();
                    sprintf(tb,"SPIBox: row %d col %d vals ", row + 1, col + 1);
                    Add_Set2Buf(tb, ps);
                    strcat(tb," Found");
                    if (SPI_debug) sprtf("%s\n",tb);
                    cnt = Elim_SET_in_Box( pb, crow, ccol, row, col, ps );
                    count += cnt;
                    if (cnt)
                        Set_SET_flag( ps2, cf_NP );
                    fnd++;
                }
            }
        }
    }
    if (!fnd) {
        tb = GetNxtBuf();
        sprintf(tb,"SPIBox: row %d col %d vals ", crow + 1, ccol + 1);
        Add_Set2Buf(tb, ps2);
        strcat(tb, " No finds.");
        if (SPI_debug) sprtf("%s\n",tb);

    }
    return count;
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

#define SAMECELL(r1,c1,r2,c2) ((r1 == r2)&&(c1 == c2))

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
typedef vector<int> vINT;
typedef vINT::iterator vINTi;

bool Is_Value_in_vINT( int i, vINT & vi )
{
    vINTi ii;
    for (ii = vi.begin(); ii != vi.end(); ii++) {
        if ( i == *ii )
            return true;
    }
    return false;
}

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
    sprintf(tb,"S2d: Map LIST ALL %d pairs", vp.size());
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
        sprintf(tb,"%3d: key %d+%d has %d pairs", cnt, val1, val2, pvp->size());
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
                sprintf(tb,"WARNING: %3d: key %d+%d has %d pairs - VALUE INVALID!", cnt, val1, val2, pvp->size());
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
                                    rc.cnt = Get_Set_Cnt(ps);
                                    memcpy(&rc.set,ps,sizeof(SET));
                                    vrc.push_back(rc);
                                }
                            }
                        }
                    }
                    if (vrc.size() == 2) {
                        // YOW, we have TWO empty cells in this BOX, NOT eliminated
                        // WE can APPLY the Naked Pair Strategy
                        sprintf(tb,"%3d:%d: key %d+%d has %d pairs, remainer 2", cnt, box, val1, val2, pvp->size());
                        sprtf("%s\n",tb);
                        *tb = 0;
                        rc = vrc[0];
                        ps = &pb->line[rc.row].set[rc.col];
                        cnt2 = Mark_SET_NOT_in_SET( ps, ps2, cf_HPE );

                        sprintf(EndBuf(tb)," Box %d: r %d c %d has %d ", box, rc.row, rc.col, rc.cnt);
                        AddSet2Buf(tb, ps);
                        sprintf(EndBuf(tb), " elim %d ", cnt2);
                        count += cnt2;
                        rc2 = vrc[1];
                        ps = &pb->line[rc2.row].set[rc2.col];
                        cnt2 = Mark_SET_NOT_in_SET( ps, ps2, cf_HPE );
                        sprintf(EndBuf(tb)," r %d c %d has %d ",rc2.row, rc2.col, rc2.cnt);
                        AddSet2Buf(tb, ps);
                        sprintf(EndBuf(tb), " elim %d ", cnt2);
                        count += cnt2;
                        // hmmm wanted to find the 3 pairs resposible, but difficult
                        //for (ii = pvp->begin(); ii != pvp->end(); ii++) {
                        //    pair = *ii;
                        //}
                    } else {
                        sprintf(tb,"%3d:%d: key %d+%d has %d pairs", cnt, box, val1, val2, pvp->size());
                        sprintf(EndBuf(tb)," found %d blanks", vrc.size());
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


    // on the wya OUT
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
    return count;
}


int Do_Naked_Pairs(PABOX2 pb)
{
    // SO try for the so called Naked Pair
    if (SPI_debug) sprtf("S2c: Try to find Naked Pairs\n");
    // Find 2 cells in the sme row, vol, or box, having the same PAIR of number
    // Since this is the only solution for these two cells, one will have one, and the other the other,
    // and we hope to find whihc way later.
    // But for now can ELIMINATE all others in the same Row, Col, or Box
    int row, col, val, cnt, count;
    PSET ps;
    count = 0;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (!val) {
                ps = &pb->line[row].set[col];
                cnt = Get_Set_Cnt(ps);
                if (cnt == 2) {
                    // ok, got a PAIR/TRIPLET
                    count += Seek_Same_Pair_In_Row( pb, row, col, ps );
                    count += Seek_Same_Pair_In_Col( pb, row, col, ps );
                    count += Seek_Same_Pair_In_Box( pb, row, col, ps );
                }
            }
        }
    }
    if (count) {
        sprtf("S2c: Found Naked Pairs. Elim %d To %d\n", count, sg_Fill_Naked);
        pb->iStage = sg_Fill_Naked;
    } else {
        sprtf("S2c: NO Naked Pairs FOUND.\n");
    }
    return count;
}

int Get_Shared_SET( PSET ps1, PSET ps2, PSET comm, bool all = true );

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

#define PEB2(a,b) sprintf(EndBuf(tb),a,b);

int debug_pp = 0;

int Do_Pointer_Pairs(PABOX2 pb)
{
    int count = 0;
    int row, col, val;
    int br, bc, rw, cl;
    PSET rsets[3];
    int ccnt;
    SET comm;
    PSET ps;
    int i;
    char *tb = GetNxtBuf();
    for (br = 0; br < 3; br++) {
        for (bc = 0; bc < 3; bc++) {
            for (rw = 0; rw < 3; rw++) {
                ccnt = 0;
                row = (br * 3) + rw;    // row constant
                sprintf(tb,"R%d B%d Cols ", row + 1, (br * 3) + bc);
                for (cl = 0; cl < 3; cl++) {
                    col = (bc * 3) + cl;    // compute column
                    val = pb->line[row].val[col];
                    if (!val) {
                        ps = &pb->line[row].set[col];
                        rsets[ccnt++] = ps;
                        sprintf(EndBuf(tb),"%d", col + 1);
                    }
                }
                if (ccnt >= 2) {
                    // have 2 or more SETs in this ROW
                    ZERO_SET(&comm);
                    int i1, i2;
                    int comcnt = 0;
                    for (i1 = 0; i1 < ccnt; i1++) {
                        for (i2 = 0; i2 < ccnt; i2++) {
                            if (i1 == i2) continue;
                            comcnt += Get_Shared_SET( rsets[i1], rsets[i2], &comm );
                        }
                    }
                    strcat(tb," vals ");
                    AddSet2Buf(tb,&comm);
                    if (comcnt) {
                        // have 1 or more SHARED candidates
                        // but ONLY interested if NOT in any other ROW of this cell
                        int r2, c2;
                        bool bad = false;
                        for (r2 = 0; r2 < 3; r2++) {
                            if (r2 == rw) continue;
                            for (c2 = 0; c2 < 3; c2++) {
                                row = (br * 3) + r2;
                                col = (bc * 3) + c2;
                                val = pb->line[row].val[col];
                                if (!val) {
                                    ps = &pb->line[row].set[col];
                                    // eliminate candidates here from the common set
                                    for (i = 0; i < 9; i++) {
                                        val = comm.val[i];  // get one of common values
                                        if (val && Value_in_SET( val, ps )) {
                                            sprintf(EndBuf(tb)," E%d",val);
                                            comm.val[val - 1] = 0;
                                            if (Get_Set_Cnt(&comm) == 0) {
                                                // no more candidates
                                                bad = true;
                                                strcat(tb," ALL");
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (bad)
                                    break;
                            }
                            if (bad)
                                break;
                        }
                        if (!bad) {
                            // OK, have common candidate(s), NOT in any other ROW
                            // can MARK candidates in this ROW, OUTSIDE this BOX for elimination
                            // rw is the ROW with the pointed pair, SET comm is candidates to elim
                            // br and bc identifies this BOX, not to be touched
                            row = (br * 3) + rw;    // ROW stays constant
                            int c;
                            int elim = 0;
                            int elim2 = 0;
                            for (col = 0; col < 9; col++) {
                                c = col / 3;
                                if (c == bc) continue; // NOT this BOX
                                val = pb->line[row].val[col];
                                if (!val) {
                                    ps = &pb->line[row].set[col];
                                    for (i = 0; i < 9; i++) {
                                        val = ps->val[i];
                                        if (val && Value_in_SET(val, &comm)) {
                                            elim++;
                                            if ( !(ps->flag[i] & cf_PPE) )
                                                elim2++;
                                            ps->flag[i] |= cf_PPE;
                                            sprintf(EndBuf(tb)," C%dM%d", col + 1, val);
                                        }
                                    }
                                }
                            }
                            count += elim2;
                            row = (br * 3) + rw;    // ROW constant
                            if (elim) {
                                int cl2;
                                // if the pair or triplet caused some ROW eliminations, mark them
                                for (cl2 = 0; cl2 < 3; cl2++) {
                                    col = (bc * 3) + cl2; // compute BOX column
                                    val = pb->line[row].val[col];
                                    if (!val) {
                                        ps = &pb->line[row].set[col];
                                        for (i = 0; i < 9; i++) {
                                            val = ps->val[i];
                                            if (val && Value_in_SET(val, &comm)) {
                                                ps->flag[i] |= cf_PP;
                                                sprintf(EndBuf(tb)," C%dP%d", col + 1, val);
                                            }
                                        }
                                    }
                                }
                            } else {
                                sprintf(EndBuf(tb)," No elims on Row %d", row + 1);
                            }
                        } else {
                            strcat(tb," bad");
                        }
                    } else {
                        strcat(tb," No shared");
                    }
                } else {
                    //sprintf(EndBuf(tb)," comm=%d", ccnt);
                    PEB2(" common %d", ccnt);
                }
                if (debug_pp) OUTIT(tb);
            }
        }
    }
    // NOW, should do COLUMNS the same way
    for (bc = 0; bc < 3; bc++) {
        for (br = 0; br < 3; br++) {
            for (cl = 0; cl < 3; cl++) {
                ccnt = 0;
                col = (bc * 3) + cl;    // column constant
                sprintf(tb,"C%d B%d Rows ", col + 1, (br * 3) + bc);
                for (rw = 0; rw < 3; rw++) {
                    row = (br * 3) + rw;    // compute row
                    val = pb->line[row].val[col];
                    if (!val) {
                        ps = &pb->line[row].set[col];
                        rsets[ccnt++] = ps;
                        sprintf(EndBuf(tb),"%d", row + 1);
                    }
                }
                if (ccnt >= 2) {
                    // have 2 or more SETs in this ROW
                    ZERO_SET(&comm);
                    int i1, i2;
                    int comcnt = 0;
                    for (i1 = 0; i1 < ccnt; i1++) {
                        for (i2 = 0; i2 < ccnt; i2++) {
                            if (i1 == i2) continue;
                            comcnt += Get_Shared_SET( rsets[i1], rsets[i2], &comm );
                        }
                    }
                    strcat(tb," vals ");
                    AddSet2Buf(tb,&comm);
                    if (comcnt) {
                        // have 1 or more SHARED candidates
                        // but ONLY interested if NOT in any other COL of this cell
                        int r2, c2;
                        bool bad = false;
                        for (c2 = 0; c2 < 3; c2++) {
                            if (c2 == cl) continue;
                            for (r2 = 0; r2 < 3; r2++) {
                                row = (br * 3) + r2;
                                col = (bc * 3) + c2;
                                val = pb->line[row].val[col];
                                if (!val) {
                                    ps = &pb->line[row].set[col];
                                    // eliminate candidates here from the common set
                                    for (i = 0; i < 9; i++) {
                                        val = comm.val[i];  // get one of common values
                                        if (val && Value_in_SET( val, ps )) {
                                            sprintf(EndBuf(tb)," E%d",val);
                                            comm.val[val - 1] = 0;
                                            if (Get_Set_Cnt(&comm) == 0) {
                                                // no more candidates
                                                bad = true;
                                                strcat(tb," ALL");
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (bad)
                                    break;
                            }
                            if (bad)
                                break;
                        }
                        col = (bc * 3) + cl;    // COL stays constant
                        if (!bad) {
                            // OK, have common candidate(s), NOT in any other COL
                            // can MARK candidates in this COL, OUTSIDE this BOX for elimination
                            // cl is the COL with the pointed pair, SET comm is candidates to elim
                            // br and bc identifies this BOX, not to be touched
                            int r;
                            int elim = 0;
                            int elim2 = 0;
                            for (row = 0; row < 9; row++) {
                                r = row / 3;
                                if (r == br) continue; // NOT this BOX
                                val = pb->line[row].val[col];
                                if (!val) {
                                    ps = &pb->line[row].set[col];
                                    for (i = 0; i < 9; i++) {
                                        val = ps->val[i];
                                        if (val && Value_in_SET(val, &comm)) {
                                            elim++;
                                            if ( !(ps->flag[i] & cf_PPE) )
                                                elim2++;
                                            ps->flag[i] |= cf_PPE;
                                            sprintf(EndBuf(tb)," C%dM%d", col + 1, val);
                                        }
                                    }
                                }
                            }
                            count += elim2;
                            col = (bc * 3) + cl;    // COL constant
                            if (elim) {
                                // if the pair or triplet caused some COL eliminations, mark them
                                int rw2;
                                for (rw2 = 0; rw2 < 3; rw2++) {
                                    row = (br * 3) + rw2; // compute BOX column
                                    val = pb->line[row].val[col];
                                    if (!val) {
                                        ps = &pb->line[row].set[col];
                                        for (i = 0; i < 9; i++) {
                                            val = ps->val[i];
                                            if (val && Value_in_SET(val, &comm)) {
                                                ps->flag[i] |= cf_PP;
                                                sprintf(EndBuf(tb)," C%dP%d", col + 1, val);
                                            }
                                        }
                                    }
                                }
                            } else {
                                sprintf(EndBuf(tb)," No elims on Row %d", row + 1);
                            }
                        } else {
                            strcat(tb," bad");
                        }
                    } else {
                        strcat(tb," No shared");
                    }
                } else {
                    //sprintf(EndBuf(tb)," comm=%d", ccnt);
                    PEB2(" common %d", ccnt);
                }
                if (debug_pp) OUTIT(tb);
            }
        }
    }

    if (count) {
        sprtf("S2d: Found %d Pointers. To %d\n", count, sg_Fill_Pointers);
        pb->iStage = sg_Fill_Pointers;
    } else {
        sprtf("S2d: NO Pointers FOUND.\n");
    }
    return count;
}

void Do_Fill_Unique(PABOX2 pb)
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
                if (ps->flag[i] & cf_Unique) {
                    Change_Box(pb, row, col, nval);
                    count++;
                    ps->flag[i] &= !cf_Unique;
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
    // set_repaint();
}

// Delete Naked Pair items
void Do_Fill_Naked(PABOX2 pb)
{
    int row, col, val, cnt, count, nval, i;
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
                        if (ps->flag[i] & cf_NPE) {
                            // Deleted by Naked Pair stategy
                            count++;
                            ps->val[i] = 0; // remove potential value
                        }
                        ps->flag[i] &= !(cf_NP | cf_NPE);   // CLEAR these flags
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
    // set_repaint();
}

// Delete Hidden Pair items
void Do_Fill_Hidden(PABOX2 pb)
{
    int row, col, val, cnt, count, nval, i;
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
                        ps->flag[i] &= !cf_HPE;   // CLEAR this flag
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
    // set_repaint();
}

int Do_Fill_Locked( PABOX2 pb )
{
    int row, col, val, i, count, flag;
    PSET ps;
    count = 0;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (!val) {
                ps = &pb->line[row].set[col];
                for (i = 0; i < 9; i++) {
                    val = ps->val[i];
                    if (val) {
                        flag = ps->flag[i];
                        if (flag & cf_LKE) {
                            ps->val[i] = 0;
                            ps->flag[i] &= !cf_LKE;
                            count++;
                        }
                        if (flag & cf_LK) {
                            ps->flag[i] &= !cf_LK;
                        }
                    }
                }
            }
        }
    }
    char *tb = GetNxtBuf();
    if (count) {
        sprintf(tb,"S%d: Cleared %d Locked Excl ", pb->iStage, count);
    } else {
        sprintf(tb,"S%d: No Locked Excl found ", pb->iStage);
    }
    strcat(tb," To begin");
    pb->iStage = sg_Begin;
    OUTIT(tb);
    return count;
}


int Do_Fill_XWing( PABOX2 pb )
{
    int row, col, val, i, count, flag;
    PSET ps;
    count = 0;
    char *tb = GetNxtBuf();
    strcpy(tb,"XW ");
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            pb->line[row].set[col].cellflg &= !(cf_XW | cf_XWR);
            if (!val) {
                ps = &pb->line[row].set[col];
                for (i = 0; i < 9; i++) {
                    val = ps->val[i];
                    if (val) {
                        flag = ps->flag[i];
                        if (flag & cf_XWE) {
                            sprintf(EndBuf(tb),"R%dC%d %d ", row + 1, col + 1, val);
                            ps->val[i] = 0;
                            ps->flag[i] &= !cf_XWE;
                            count++;
                        }
                        if (flag & cf_XW) {
                            ps->flag[i] &= !cf_XW;
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

int debug_locked_excl = 1;
int Do_Locked_Excl( PABOX2 pb )
{
    int count = 0;
    if (!pb)
        pb = get_curr_box();
    // Locked 1
    // Work Box (Region) by Box (Region)
    // 
    int row, col, val;
    int r, c, rw, cl, i;
    int cnt, cl2, cnt2;
    int tots[3][3];
    int bad;
    PSET ps, ps2;
    SET set;
    ps2 = &set;
    char *tb = GetNxtBuf();
    count = 0;
    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++) {
            // Process a Region/Box
            // Get values for the region
            int minrow = (r * 3);
            int maxrow = minrow + 2;
            int mincol = (c * 3);
            int maxcol = mincol + 2;
            int sum = 0;
            for (rw = 0; rw < 3; rw++) {
                for (cl = 0; cl < 3; cl++) {
                    row = (r * 3) + rw;
                    col = (c * 3) + cl;
                    val = pb->line[row].val[col];
                    tots[rw][cl] = val;
                    if (val)
                        sum++;
                }
            }
            // Study totals
            // Seek only
            // 111 = 3
            // 111 = 3
            // 000 = 0 = one blank row
            // or
            //  110
            //  110
            //  110
            //  330 = one blank column
            if (sum != 6)   // have to have 6 out of 9, to even have a chance of a blank row or column
                continue;   // trry next Box/Region
            // Ok, now do we have the above - two rows filled, and one blank row
            for (rw = 0; rw < 3; rw++) {
                bad = 0;
                for (cl = 0; cl < 3; cl++) {
                    if (cl == 0) {
                        val = tots[rw][cl];
                    } else {
                        if (val) {
                            // 1st has value
                            if (!tots[rw][cl]) {
                                bad = 1;    // BAD if other have zero
                                break;
                            }
                        } else {
                            // 1st has no value
                            if (tots[rw][cl]) {
                                bad = 1;    // BAD if others have a value
                                break;
                            }
                        }
                    }
                }
                if (bad)
                    break;
            }
            if (!bad) {
                // Have a BLANK row - get to blank row
                for (rw = 0; rw < 3; rw++) {
                    if (!tots[rw][0]) {
                        // get potential values that MUST go in this ROW
                        Zero_SET(ps2);
                        *tb = 0;
                        row = (r * 3) + rw;
                        for (cl = 0; cl < 3; cl++) {
                            col = (c * 3) + cl;
                            ps = &pb->line[row].set[col];
                            sprintf(EndBuf(tb),"r%d c%d ", row + 1, col + 1);
                            AddSet2Buf(tb,ps);
                            strcat(tb," ");
                            for (i = 0; i < 9; i++) {
                                val = ps->val[i];
                                if (val)
                                    ps2->val[val - 1] = val;
                            }
                        }
                        cnt = Get_Set_Cnt(ps2);
                        sprintf(EndBuf(tb), "Cum [%d] ",cnt);
                        AddSet2Buf(tb,ps2);
                        // Clear this SET from others in the ROW
                        cnt2 = 0;
                        SET set3;
                        Zero_SET(&set3);
                        for (cl2 = 0; cl2 < 9; cl2++) {
                            // EXCLUDING this Region
                            if ((cl2 < mincol) || (cl2 > maxcol)) {
                                val = pb->line[row].val[cl2];
                                if (!val) {
                                    ps = &pb->line[row].set[cl2];
                                    sprintf(EndBuf(tb)," r%d c%d from ", row + 1, cl2 + 1);
                                    AddSet2Buf(tb,ps);
                                    cnt2 += Mark_SET_in_SET( ps, ps2, cf_LKE, false );
                                    strcat(tb," to ");
                                    AddSet2BufExFlag(tb,ps,cf_LKE);
                                    //               from to    on flag
                                    Set_SET_per_flag(ps,  &set3, cf_LKE);
                                }
                            }
                        }
                        count += cnt2;
                        ADDSP(tb);
                        sprintf(EndBuf(tb), "Elim [%d] ",cnt2);
                        if (debug_locked_excl) OUTIT(tb);
                        sprintf(tb,"Mrk lkd using ");
                        AddSet2Buf(tb,&set3);
                        if (cnt2) {
                            for (cl = 0; cl < 3; cl++) {
                                col = (c * 3) + cl;
                                ps = &pb->line[row].set[col];
                                sprintf(EndBuf(tb)," r%d c%d ", row +1, col + 1);
                                AddSet2Buf(tb,ps);
                                cnt2 += Mark_SET_in_SET( ps, &set3, cf_LK );
                            }
                        }
                        if (debug_locked_excl) OUTIT(tb);
                        break;
                    }
                }
            }
        }
    }
    if (count) {
        sprintf(tb,"S%d: Marked %d Locked ", pb->iStage, count);
        strcat(tb,"To Lock Fill");
        pb->iStage = sg_Fill_Locked;
    } else {
        sprintf(tb,"S%d: No Locked marked ", pb->iStage);
        //strcat(tb, "To begin");
        pb->iStage = sg_Begin;
    }
    OUTIT(tb);
    // set_repaint();
    return count;
}



void Do_ID_OPTIONS_SHOWHINT()
{
    // if there is a SELECTION, show possible values
    int row = g_sel_Row;
    int col = g_sel_Col;
    int val, cnt;
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
int add_debug_xw = 0;
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


int Do_XWing_Scan( PABOX2 pb )
{
    int count;
    if (!pb)
        pb = get_curr_box();
    int row, col, val, ccnt;
    PSET psx1, psx2, psx3, psx4, ps5;
    int *pi1, *pi2, *pi3, *pi4;
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
            if (!val) {
                // got top-left blank - seek top-right blank
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
                                    // both ar blank
                                    psx3 = &pb->line[row2].set[col];
                                    pi3  = &pb->line[row2].set[col].cellflg;
                                    psx4 = &pb->line[row2].set[col2];
                                    pi4  = &pb->line[row2].set[col2].cellflg;
                                    Zero_SET(&comm2);
                                    ccnt2 = Get_Common_SET_Cnt( psx3, psx4, &comm2 );
                                    if (ccnt2) {
                                        Zero_SET( &comm3 );
                                        ccnt3 = Get_Common_SET_Cnt( &comm, &comm2, &comm3 );
                                        if (ccnt3) {
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
                                                if (!val) {
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
                                            }
                                            if (!bad) {
                                                // ok have a common value NOT eliminated, and NOT elswhere in ROW
                                                // test the 2nd column
                                                memcpy(&comm4,&comm3,sizeof(SET)); // keep original of common
                                                for (col3 = 0; col3 < 9; col3++) {
                                                    if (col3 == col) continue; // skip first self
                                                    if (col3 == col2) continue; // skip second self
                                                    val = pb->line[row2].val[col3];
                                                    if (!val) {
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
                                                    if (!val) {
                                                        ps5 = &pb->line[row3].set[col];
                                                        // set elimination flag
                                                        cnt3 = Mark_SET_in_SET( ps5, &comm3, cf_XWE, false );
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
                                                    rc.cnt = Get_Set_Cnt(psx1);
                                                    memcpy(&rc.set,psx1,sizeof(SET));
                                                    vXWrc.push_back(rc);
                                                    rc.row = row;
                                                    rc.col = col2;
                                                    rc.cnt = Get_Set_Cnt(psx2);
                                                    memcpy(&rc.set,psx2,sizeof(SET));
                                                    vXWrc.push_back(rc);
                                                    rc.row = row2;
                                                    rc.col = col;
                                                    rc.cnt = Get_Set_Cnt(psx3);
                                                    memcpy(&rc.set,psx3,sizeof(SET));
                                                    vXWrc.push_back(rc);
                                                    rc.row = row2;
                                                    rc.col = col2;
                                                    rc.cnt = Get_Set_Cnt(psx4);
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
                                                            sprintf(EndBuf(tb),"r%c% ", row2 + 1, col3 + 1);
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
                                                    cnt2 += Mark_SET_in_SET( psx4, &comm3, cf_XW, false );

                                                    // set the cell flags of the corners
                                                    *pi1 |= cf_XW; // | cf_XWR;
                                                    *pi2 |= cf_XW; // | cf_XWR;
                                                    *pi3 |= cf_XW; // | cf_XWR;
                                                    *pi4 |= cf_XW; // | cf_XWR;

                                                    rc.row = row;
                                                    rc.col = col;
                                                    rc.cnt = Get_Set_Cnt(psx1);
                                                    memcpy(&rc.set,psx1,sizeof(SET));
                                                    vXWrc.push_back(rc);
                                                    rc.row = row;
                                                    rc.col = col2;
                                                    rc.cnt = Get_Set_Cnt(psx2);
                                                    memcpy(&rc.set,psx2,sizeof(SET));
                                                    vXWrc.push_back(rc);
                                                    rc.row = row2;
                                                    rc.col = col;
                                                    rc.cnt = Get_Set_Cnt(psx3);
                                                    memcpy(&rc.set,psx3,sizeof(SET));
                                                    vXWrc.push_back(rc);
                                                    rc.row = row2;
                                                    rc.col = col2;
                                                    rc.cnt = Get_Set_Cnt(psx4);
                                                    memcpy(&rc.set,psx4,sizeof(SET));
                                                    vXWrc.push_back(rc);
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

int Do_Fill_Simple(PABOX2 pb)
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
                            ps->flag[i] &= !cf_SSE;
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

int Do_Fill_By_Flags( PABOX2 pb, int eflg, int mflg, char *smsg, char *type, int cflg = 0 );

int Do_Fill_By_Flags( PABOX2 pb, int eflg, int mflg, char *smsg, char *type, int cflg )
{
    int row, col, val, i, count, flag;
    PSET ps;
    count = 0;
    char *tb = GetNxtBuf();
    sprintf(tb,"%s ",smsg);
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            ps = &pb->line[row].set[col];
            if (!val) {
                for (i = 0; i < 9; i++) {
                    val = ps->val[i];
                    if (val) {
                        flag = ps->flag[i];
                        if (flag & eflg) {
                            sprintf(EndBuf(tb),"R%dC%d %d ", row + 1, col + 1, val);
                            ps->val[i] = 0;
                            ps->flag[i] &= !eflg;
                            count++;
                        }
                        if (flag & mflg) {
                            ps->flag[i] &= !mflg;
                        }
                    }
                }
            }
            if (cflg)
                ps->cellflg &= !cflg;
        }
    }
    if (count) {
        OUTIT(tb);
        sprintf(tb,"S%d: Cleared [%d] %s ", pb->iStage, count, type);
    } else {
        sprintf(tb,"S%d: No %s found ", pb->iStage, type);
    }
    strcat(tb," To begin");
    pb->iStage = sg_Begin;
    OUTIT(tb);
    // set_repaint();
    return count;

}

int Do_Fill_Pointers(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_PPE, cf_PP, "PP", "Pointed Pairs" );
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


int only_one_Y = 1; // for the moment only do ONE Y-Wing elimination per step
static int xwing_scans = 0;

int Do_YWing_Scan(PABOX2 pb)
{
    int count = 0;
    int row, col, val;
    int row2, col2, scnt;
    PSET ps, ps2, ps3, ps4;
    SET comm, comm2, comm3;
    char *tb = GetNxtBuf();
    xwing_scans++;
    sprtf("Doing YWing scan %d\n", xwing_scans);
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
    int br, bc;
    int brw, bcl;
    int i;
    int setcnt = 0;
    int box, box2;
    ROWCOL rc[9+2];
    PROWCOL prc;
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
                            prc->cnt = 2;
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
                    OUTIT(tb);
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
                                            prc->cnt = 2;
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
            OUTIT(tb);
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

void Do_Stage_Two(PABOX2 pb)
{
    int row, col, val, cnt, count, nval;
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
                if (cnt == 1) {
                    Change_Box(pb, row, col, nval);
                    count++;
                }
            }
        }
    }
    if (count) {
        sprtf("S2a: Set SINGLE values. %d - To begin\n", count);
        pb->iStage = sg_Begin;  // reset to do more eliminations
    } else {
        int i;
        int result;
        // Mark SINGLES
        // Unqiue in Row, Column or Box, or a combination
        count = 0;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                ps = &pb->line[row].set[col];
                val = pb->line[row].val[col];
                if (!val) {
                    cnt = Get_Set_Cnt(ps);
                    if (cnt) {
                        // This count is NOT one, else would have done that above
                        for (i = 0; i < 9; i++) {
                            nval = ps->val[i];
                            if (!nval) continue; // forget zero values
                            // is this nval UNIQUE in this ROW
                            result = 0;
                            if ( Unique_In_Row( pb, row, col, nval ) )
                                result |= cf_Unique_Row;
                            if ( Unique_In_Col( pb, row, col, nval ) )
                                result |= cf_Unique_Col;
                            if ( Unique_In_Box( pb, row, col, nval ) )
                                result |= cf_Unique_Box;
                            if (result) {
                                pb->line[row].set[col].flag[i] |= result;
                                pb->line[row].set[col].cellflg |= result;
                                count++;
                            }
                        }
                    } else {
                        // BAD BOX!!!
                    }
                }
            }
        }
        if (count) {
            sprtf("S2b: %d Marked SINGLE. To %d\n", count, sg_Fill_Unique);
            pb->iStage = sg_Fill_Unique; // set stage 3 to enter these Marked unique values
        } else {
            // So try for the so called Naked Pairs
            sprtf("S2b: No vals marked SINGLE.\n", count);
            count = Do_Naked_Pairs(pb);
            if (!count) {
#if 0 // this is NOT working properly yet
                count = Do_Hidden_Pairs(pb);
                if (count) {
                    sprtf("S2d: Hidden Pairs, elim %d vals. To %d\n", count,
                        sg_Fill_Hidden);
                    pb->iStage = sg_Fill_Hidden;
                } else {
                    sprtf("S2d: Hidden Pairs. No elims. To begin.\n");
                    pb->iStage = sg_Begin;
                }
#endif // 0
            }
            if (!count) {
                count = Do_Locked_Excl(pb);
            }
            if (!count) {
                count = Do_XWing_Scan(pb);
            }
            if (!count) {
                count = Do_Pointer_Pairs(pb);
            }
            if (!count) {
                count = Do_YWing_Scan(pb);
            }
        }
    }
    // set_repaint();
}

void Do_Simple_Scan(PABOX2 pb)
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
            pb->line[row].set[col].cellflg &= !cl_Perm;  // clear all except some PERSISTANT flags
            memset(&pb->line[row].set[col].flag,0,(sizeof(int) * 9)); // DO I want to do THIS????
            // like pb->line[row].set[col].flag[i] &= !cf_KEEP;, but for NOW clear
            pb->line[row].set[col].uval = 0;
            count += Mark_Same_Col( pb, col, ps, cf_SSE );
            count += Mark_Same_Row( pb, row, ps, cf_SSE );
            count += Mark_Same_Box( row, col, pb, ps, cf_SSE );
        }
    }
    if (count) {
        sprtf("S%d: Elim same row,col,box [%d] To %d\n", sg_One, count, sg_Fill_Simple);
        pb->iStage = sg_Fill_Simple;
    } else {
        sprtf("S%d: None elim in simple scan To %d\n", sg_One, count, sg_Two);
        pb->iStage = sg_Two;
    }
    // set_repaint();
}

VOID Do_ID_OPTIONS_ONESTEP( HWND hWnd )
{
    PABOX2 pb = get_curr_box();
    switch( pb->iStage )
    {
    case sg_Begin:
    case sg_One:
        Do_Simple_Scan(pb);
        break;
    case sg_Two:
        Do_Stage_Two(pb);
        break;
    case sg_Fill_Unique:
        Do_Fill_Unique(pb); // Put items marked 'unique' into Box
        break;
    case sg_Fill_Naked:
        Do_Fill_Naked(pb);
        break;
    case sg_Fill_Hidden:
        Do_Fill_Hidden(pb);
        break;
    case sg_Fill_Locked:
        Do_Fill_Locked(pb);
        break;
    case sg_Fill_XWing:
        Do_Fill_XWing(pb);
        break;
    case sg_Fill_Simple:
        Do_Fill_Simple(pb);
        break;
    case sg_Fill_Pointers:
        Do_Fill_Pointers(pb);
        break;
    case sg_Fill_YWing:
        Do_Fill_YWing(pb);
        break;
    }
    set_repaint();
}


// eof - Sudo_Logic.cxx
