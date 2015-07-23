// Sudo_URect2.cxx

#include "Sudoku.hxx"

// ============================================================================
// SECOND ATTEMPT TO GET THIS RIGHT
// ================================
// Defining Unque Rectangles
// If we have 2 value say 57 found in 4 cells covering EXCACTLY two rows, two
// columns AND two blocks. If these were the ONLY solutions for those 4 cells,
// then we would have a NON-UNIQUE solution. We could just swap 5 & 7.
// from : http://www.learn-sudoku.com/unique-rectangle.html
// There is just one catch I must stress! This only works when four cells 
// fall into exactly two blocks.  

// Type 1:
// So when we have 4 such cells where ONE cell has ONE other value, then 
// the 57 can be eliminated from that cell.
// See examples\uniqrect5.txt
// Type 2:
// Similarly, if we have 4 such cells sharing 2 value, and two other 
// cells also sharing those two values, plus ONE other value. In this 
// case that additionla value MUST be in one of those cells, otherwise 
// again we wold have a swappable pair creating a non-unique solution.
// Since that additional value is thus LOCKED to one or the other,
// that value can be eliminated in the same ROW, COL or BOX
// Type 3:
// Similar to the above, except it can be more that just 1 additional
// value, but creates a LOCKED pairs of cells with oen or the other 
// of those additional values, thus those values can be eliminated 
// in the same ROW, COL, or BOX
// 

// from : 
// Unique Rectangles is an advanced technique. Its name comes from a collection 
// of two locked pairs that form a rectangle. Every Sudoku has to have a unique solution, 
// we are not allowed to create a situation where this rule is violated. We need to look 
// for four squares that share exactly two rows, two columns and two blocks, if we had 
// a Sudoku with a non-unique solution, then we could swap the numbers and still have 
// a valid Sudoku with no duplicates. The examples below are a subset of the logic which 
// can be deduced from the requirement of uniqueness, if you come across a situation which 
// looks similar, then carefully think about the implications of the Sudoku being left 
// in an invalid state.
// NOTE: the squares have to share exactly two rows, two columns and two blocks.
// see : examples\uniqrect5.txt


#define ur_elim2    cf_URE

#define OUTITUR2(tb) if (add_debug_ur2) OUTIT(tb)

#define lnkC 0x01000
#define lnkD 0x02000
#define lnkCD (lnkC | lnkD)
#define lnkRow 0x00100
#define lnkCol 0x00200

// Sometimes better/easier to pass around a structure
typedef struct tagURSTR {
    PABOX2 pb;
    int ur_type;    // Type 1 2 and 4 presently
    int elim_count; // count of eliminations by Unique Rectangels
    int scnt1, scnt2, scnt3, scnt4;
    int setcnts[9];
    SET set, set2;
    vRCR *pvrcr;
    RCRECT rcrect;
    ROWCOL rc1, rc2, rc3, rc4;
    int sumsets, socnt, slcnt;
    PROWCOL prcelim1, prcelim2;
    PROWCOL prcC, prcD;
    RCPAIR rcpair1, rcpair2, rcpair3, rcpair4;
    int lnkage, setval;
    vRC *pelims;
    vRC *pelimby;
}URSTR, *PURSTR;

static URSTR _s_urstr = {0};

static vRCR *pur_rects2 = 0;
static int ur_rect_cnt = 0;
vRCR *Get_UR_Rects2_Ptr()
{
    if (!pur_rects2)
        pur_rects2 = new vRCR;
    return pur_rects2;
}
int Get_UR_Cnt() { return ur_rect_cnt; }
void Set_UR_invalid() { ur_rect_cnt = 0; }
void Kill_UR2()
{
    PURSTR pur = &_s_urstr;
    if (pur_rects2) {
        pur_rects2->clear();
        delete pur_rects2;
    }
    pur_rects2 = 0;
    if (pur->pelims) {
        pur->pelims->clear();
        delete pur->pelims;
    }
    pur->pelims = 0;
    if (pur->pelimby) {
        pur->pelimby->clear();
        delete pur->pelimby;
    }
    pur->pelimby = 0;
}

// Just OUTPUT the RECTANGLE
char *Get_RCRECT_Stg( PRCRECT prcrect )
{
    char *tb = GetNxtBuf();
    PSET pset = &prcrect->set;
    int i, scnt, setval;
    //int j;
    PRCPAIR prcp;
    int setvals[9];
    scnt = Get_Set_Cnt2(pset, setvals);
    sprintf(tb,"RCR-SET : ");
    AddSet2Buf(tb,pset);
    ADDSP(tb);
    setval = setvals[0];
    for (i = 0; i < 4; i++) {
        prcp = &prcrect->rcpair[i];
        //for (j = 0; j < scnt; j++) {
        //    setval = setvals[j];
            sprintf(EndBuf(tb),"%s ", Get_RCPAIR_Stg(prcp,setval,true));
        //}
    }
    return tb;
}

void Show_RCRECT( PRCRECT prcrect, char *pmsg = 0 );
void Show_RCRECT( PRCRECT prcrect, char *pmsg )
{
    char *tb = GetNxtBuf();
    sprintf(tb,"%s %s", Get_RCRECT_Stg(prcrect),
        ((pmsg && *pmsg) ? pmsg : "") );
    OUTIT(tb);
}

int Add_Common_Cands( PSET pdst, PSET ps1, PSET ps2, PSET ps3, PSET ps4 )
{
    int count = 0;
    int i, setval;
    for (i = 0; i < 9; i++) {
        setval = ps1->val[i];
        if (!setval) continue;
        if ( !(ps2->val[i]) ) continue;
        if ( !(ps3->val[i]) ) continue;
        if ( !(ps4->val[i]) ) continue;
        // ok, a SHARED candidate
        pdst->val[setval - 1] = setval;
        count++;
    }
    return count;
}

int Add_Not_Shared_1( PSET pset, PSET pset2, PSET ps1 )
{
    int count = 0;
    int i, setval;
    for (i = 0; i < 9; i++) {
        setval = ps1->val[i];
        if (!setval) continue;
        if (pset->val[i]) continue;
        if (pset2->val[i]) continue; // Already has this
        pset2->val[i] = setval;
        count++;
    }
    return count;
}


int Add_Not_Shared_4( PSET pset, PSET pset2, PSET ps1, PSET ps2, PSET ps3, PSET ps4 )
{
    int count = 0;
    count += Add_Not_Shared_1( pset, pset2, ps1 );
    count += Add_Not_Shared_1( pset, pset2, ps2 );
    count += Add_Not_Shared_1( pset, pset2, ps3 );
    count += Add_Not_Shared_1( pset, pset2, ps4 );
    return count;
}

uint64_t Mark_SL_for_setval_in_rcpair( PABOX2 pb, int setval, PRCPAIR prcp )
{
    uint64_t flag = 0;
    PROWCOL prc1, prc2;
    int box1, box2, setcnt;
    char *tb = GetNxtBuf();

    prc1 = &prcp->rowcol[0];
    prc2 = &prcp->rowcol[1];
    box1 = GETBOX(prc1->row,prc1->col);
    box2 = GETBOX(prc2->row,prc2->col);

    sprintf(tb,"Mk setval %d SL rcpair %s %s ", setval, Get_RC_setval_RC_Stg(prc1,setval),
        Get_RC_setval_RC_Stg(prc2,setval));

    if (prc1->row == prc2->row) {
        setcnt = Count_setval_in_Row( pb, prc1->row, prc1->col, setval );
        if (setcnt == 2) {
            flag = cl_SLR;
            prc1->set.flag[setval - 1] |= flag; // mark strong link in Row
            prc2->set.flag[setval - 1] |= flag; // mark strong link in Row
            strcat(tb,"cl_SLR ");
        }
    } else if (prc1->col == prc2->col) {
        setcnt = Count_setval_in_Row( pb, prc1->row, prc1->col, setval );
        if (setcnt == 2) {
            flag = cl_SLC;
            prc1->set.flag[setval - 1] |= flag; // mark strong link in Col
            prc2->set.flag[setval - 1] |= flag; // mark strong link in Col
            strcat(tb,"cl_SLC ");
        }
    } else if (box1 == box2) {
        setcnt = Count_setval_in_Box( pb, prc1->row, prc1->col, setval );
        if (setcnt == 2) {
            flag = cl_SLB;
            prc1->set.flag[setval - 1] |= flag; // mark strong link in Box
            prc2->set.flag[setval - 1] |= flag; // mark strong link in Box
            strcat(tb,"cl_SLB ");
        }
    } else {
        char *tb = GetNxtBuf();
        sprintf(tb,"YIKES: A CODE ERROR: Mark_SL_for_setval_in_rcpair called, but" MEOL
            "the RCAPIR do NOT share a ROW, COL or BOX - %s %s! FIX ME!!",
            Get_RC_setval_RC_Stg(prc1), Get_RC_setval_RC_Stg(prc2));
        OUTIT(tb);
    }
    if (!flag) 
        strcat(tb,"NONE!");
    OUTITUR2(tb);
    return flag;
}



bool same_rcrect( PRCRECT prcrA, PRCRECT prcrB )
{
    PRCPAIR prcpA, prcpB;
    PROWCOL prcA1, prcA2, prcB1, prcB2;
    int i, j, matched, matchcnt;

    matchcnt = 0;
    for (i = 0; i < 4; i++) {
        prcpA = &prcrA->rcpair[i];
        prcA1 = &prcpA->rowcol[0];
        prcA2 = &prcpA->rowcol[1];
        matched = 0;
        for (j = 0; j < 4; j++) {
            prcpB = &prcrB->rcpair[j];
            prcB1 = &prcpB->rowcol[0];
            prcB2 = &prcpB->rowcol[1];
            if (SAMERCCELL(prcA1,prcB1) && SAMERCCELL(prcA2,prcB2)) {
                matched = 1;
                matchcnt++;
                break;
            } else if (SAMERCCELL(prcA1,prcB2) && SAMERCCELL(prcA2,prcB1)) {
                matched = 1;
                matchcnt++;
                break;
            }
        }
        if (!matched)
            return false;
    }
    if (matchcnt == 4)
        return true;
    return false;
}

#undef ADD_DEBUG_COMPARE
bool prcrect_in_pvrcr( PRCRECT prcrect, vRCR *pvrcr )
{
    size_t ii;
    size_t max = pvrcr->size();
    PRCRECT prcr;
#ifdef ADD_DEBUG_COMPARE
    char *tb1 = Get_RCRECT_Stg(prcrect);
    char *tb2;
#endif
    for (ii = 0; ii < max; ii++) {
        prcr = &pvrcr->at(ii);
#ifdef ADD_DEBUG_COMPARE
        tb2 = Get_RCRECT_Stg(prcr);
#endif
        if (same_rcrect( prcr, prcrect ))
            return true;
    }
    return false;
}

// Type 4 Unique Rectangles
// A type 4 unique rectangle is a situation where 2 cells of a rectangle contain only a pair of 
// possibilities a and b, the other cells C and D contain a and b plus any number of additional 
// values, and there is a strong link between a@C and a@D. In that case, selecting b@C would 
// force a@D, creating a non-unique rectangle; ditto b@D. So b can be eliminated from C and D. 
// The next example is a type-4 unique rectangle at D5, H5, D6 and H6: Value 2 must go in one 
// of D6 and H6. So we must eliminate 5 from D6 and H6.
//  examples\uniqrect4.txt
// 010230090029018300038479021073000109965341782201090063150984030306100900090063010
// UR-Elim: 5@D6 by 25@D5 25@H5 256@D6 257@H6 SC-Type-4
// UR-Elim: 5@H6 by 25@D5 25@H5 256@D6 257@H6 SC-Type-4
// Agrees EXACTLY to text

int Set_Link_Flag( int setval, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4 )
{
    int lnked = 0;
    uint64_t flg1, flg2, flg3, flg4;
    int lval = setval  - 1;


    flg1 = prc1->set.flag[lval] & cf_XAB;
    flg2 = prc2->set.flag[lval] & cf_XAB;
    flg3 = prc3->set.flag[lval] & cf_XAB;
    flg4 = prc4->set.flag[lval] & cf_XAB;

    if (SAMERCCELL(prc1,prc3)) {
        lnked = 13;
        prc1->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        prc3->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        if (flg1 && flg3) { // should also be SAME
            // both already COLORED
            if (flg2 && flg4) {
                // ALL marked
            } else if (flg2) {
                // 3 are colored
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else {
                // only 1 & 3 colored
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            }
        } else if (flg1) {
            flg3 = flg1;
            prc3->set.flag[lval] |= flg3;
            if (flg2 && flg4) {
                // ALL marked
            } else if (flg2) {
                // 3 are colored
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else {
                // only 1 & 3 colored
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            }
        } else if (flg3) {
            flg1 = flg3;
            prc1->set.flag[lval] |= flg1;
            if (flg2 && flg4) {
                // ALL marked
            } else if (flg2) {
                // 3 are colored
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else {
                // only 1 & 3 colored
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            }
        } else {
            // neither 1 or 3 marked
            if (flg2 && flg4) {
                // but both others are, so use their color
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
                flg3 = FLIP_COLOR(flg4);
                prc3->set.flag[lval] |= flg3;
            } else if (flg2) {
                // 3 are NOT colored - ONLY 2
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
                flg3 = flg1;  // joined pair are the SAME
                prc3->set.flag[lval] |= flg3;
                flg4 = flg2;
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                flg3 = FLIP_COLOR(flg4);
                prc3->set.flag[lval] |= flg3;
                flg1 = flg3;
                prc1->set.flag[lval] |= flg1;
                flg2 = flg4;
                prc2->set.flag[lval] |= flg1;
            } else {
                // nor 2 & 4 - aribtray
                flg1 = cf_XCA;
                flg2 = cf_XCB;
                flg3 = flg1; // joined 
                flg4 = flg2;
                prc1->set.flag[lval] |= flg1;
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
                prc4->set.flag[lval] |= flg4;
            }
        }
    } else if (SAMERCCELL(prc1,prc4)) {
        prc1->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        prc4->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        lnked = 14;
        if (flg1 && flg4) { // should also be SAME
            // both already COLORED
            if (flg2 && flg3) {
                // ALL marked
            } else if (flg2) {
                // 3 missing
                flg3 = FLIP_COLOR(flg4);
                prc3->set.flag[lval] |= flg3;
            } else if (flg3) {
                // 2 missing
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else {
                // both 2 & 3 missing
                flg2 = FLIP_COLOR(flg1);
                flg3 = FLIP_COLOR(flg4);
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
            }
        } else if (flg1) {
            // 4 missing
            flg4 = flg1;
            prc4->set.flag[lval] |= flg4;
            if (flg2 && flg3) {
                // ALL marked
            } else if (flg2) {
                // 3 missing
                flg3 = FLIP_COLOR(flg4);
                prc3->set.flag[lval] |= flg3;
            } else if (flg3) {
                // 2 missing
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else {
                // both 2 & 3 missing
                flg2 = FLIP_COLOR(flg1);
                flg3 = FLIP_COLOR(flg4);
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
            }
        } else if (flg4) {
            // 1 missing
            flg1 = flg4;
            prc1->set.flag[lval] |= flg1;
            if (flg2 && flg3) {
                // ALL marked
            } else if (flg2) {
                // 3 missing
                flg3 = FLIP_COLOR(flg4);
                prc3->set.flag[lval] |= flg3;
            } else if (flg3) {
                // 2 missing
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else {
                // both 2 & 3 missing
                flg2 = FLIP_COLOR(flg1);
                flg3 = FLIP_COLOR(flg4);
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
            }
        } else {
            // neither 1 nor 4
            if (flg2 && flg3) {
                flg1 = FLIP_COLOR(flg2);
                flg4 = FLIP_COLOR(flg3);
                prc1->set.flag[lval] |= flg1;
                prc4->set.flag[lval] |= flg4;
            } else if (flg2) {
                // neither 1 3 or 4
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
                flg4 = flg2;
                prc4->set.flag[lval] |= flg4;
                flg3 = flg1;
                prc3->set.flag[lval] |= flg3;
            } else if (flg3) {
                // neither 1 2 or 4
                flg1 = FLIP_COLOR(flg3);
                prc1->set.flag[lval] |= flg1;
                flg2 = flg3;
                prc2->set.flag[lval] |= flg2;
                flg4 = flg1;
                prc4->set.flag[lval] |= flg4;
            } else {
                // none none
                // nor 2 & 4 - aribtray
                flg1 = cf_XCA;
                flg2 = cf_XCB;
                flg4 = flg1;    // joined
                flg3 = flg2;
                prc1->set.flag[lval] |= flg1;
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
                prc4->set.flag[lval] |= flg4;
            }
        }
    } else if (SAMERCCELL(prc2,prc3)) {
        lnked = 23;
        prc2->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        prc3->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        if (flg2 && flg3) { // should also be SAME
            // both already COLORED
            if (flg1 && flg4) {
                // ALL marked
            } else if (flg1) {
                // 4 missing
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                // 1 missing
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
            } else {
                // got 2 & 3 - no 1 & 4
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
            }
        } else if (flg2) {
            // 3 missing
            flg3 = flg2;
            prc3->set.flag[lval] |= flg3;
            if (flg1 && flg4) {
                // got other 2
            } else if (flg1) {
                flg4 = flg1;
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                flg1 = flg4;
                prc1->set.flag[lval] |= flg1;
            } else {
                // no 1 or 4
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
                flg4 = flg1;
                prc4->set.flag[lval] |= flg4;
            }
        } else if (flg3) {
            // 2 missing
            flg2 = flg3;
            prc2->set.flag[lval] |= flg2;
            if (flg1 && flg4) {
                // got other 2
            } else if (flg1) {
                flg4 = flg1;
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                flg1 = flg4;
                prc1->set.flag[lval] |= flg1;
            } else {
                // no 1 or 4
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
                flg4 = flg1;
                prc4->set.flag[lval] |= flg4;
            }
        } else {
            // not 2 or 3
            if (flg1 && flg4) {
                // got other 2
                flg2 = FLIP_COLOR(flg1);
                flg3 = flg2;
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
            } else if (flg1) {
                flg4 = flg1;
                prc4->set.flag[lval] |= flg4;
            } else if (flg4) {
                flg1 = flg4;
                prc1->set.flag[lval] |= flg1;
            } else {
                // no 2 or 3
                // no 1 or 4
                flg1 = cf_XCA;
                flg2 = cf_XCB;
                flg4 = flg1;
                flg3 = flg2; // joined
                prc1->set.flag[lval] |= flg1;
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
                prc4->set.flag[lval] |= flg4;
            }
        }
    } else if (SAMERCCELL(prc2,prc4)) {
        lnked = 24;
        prc2->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        prc4->set.flag[lval] |= cl_LSS; // Add SAMECELL link
        if (flg2 && flg4) { // should also be SAME
            // both already COLORED
            if (flg1 && flg3) {
                // ALL marked
            } else if (flg1) {
                // got 1 2 4 - 3 = not 4
                flg3 = FLIP_COLOR(flg4);
                prc3->set.flag[lval] |= flg3;
            } else if (flg3) {
                // got 2 3 4 - 1 = not 3
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
            } else {
                // got 2 & 4, no 1 & 3 - 1 = not 2, 3 = not 4
                flg1 = FLIP_COLOR(flg2);
                flg3 = FLIP_COLOR(flg4);
                prc1->set.flag[lval] |= flg1;
                prc3->set.flag[lval] |= flg3;
            }
        } else if (flg2) {
            // no 4
            if (flg1 && flg3) {
                // got 1 2 3
                flg4 = FLIP_COLOR(flg3);
                prc4->set.flag[lval] |= flg4;
            } else if (flg1) {
                // got 1 2, no 3 4 - 4 = 2 joined - 3 = noy 4
                flg4 = flg2; // joined
                prc4->set.flag[lval] |= flg4;
                flg3 = FLIP_COLOR(flg4);
                prc3->set.flag[lval] |= flg3;
            } else if (flg3) {
                // got 2 3, no 1 4 - 4 = 2 joined 1 = not 2
                flg4 = flg2; // joined pair
                flg1 = FLIP_COLOR(flg2);
                prc4->set.flag[lval] |= flg4;
                prc1->set.flag[lval] |= flg1;
            } else {
                // got 2 no 4, no 1 & 3
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
                flg4 = flg2; // joined
                prc4->set.flag[lval] |= flg4;
                flg3 = flg1;
                prc3->set.flag[lval] |= flg3;
            }
        } else if (flg4) {
            // got 4, no 2
            if (flg1 && flg3) {
                // ALL marked
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else if (flg1) {
                // no 2 3
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
                flg3 = flg1;
                prc3->set.flag[lval] |= flg3;
            } else if (flg3) {
                // got 3 4, no 1 2 = 2 & 4 linked
                flg2 = flg4;
                flg1 = flg3;
                prc2->set.flag[lval] |= flg2;
                prc1->set.flag[lval] |= flg1;
            } else {
                // got 2 no 4, no 1 & 3
                flg1 = FLIP_COLOR(flg2);
                flg4 = flg2;
                flg3 = flg1;
                prc1->set.flag[lval] |= flg1;
                prc3->set.flag[lval] |= flg3;
                prc4->set.flag[lval] |= flg4;
            }
        } else {
            // no 2 or 4
            if (flg1 && flg3) {
                // ALL marked
                flg2 = FLIP_COLOR(flg1);
                flg4 = FLIP_COLOR(flg4);
                prc2->set.flag[lval] |= flg2;
                prc4->set.flag[lval] |= flg4;
            } else if (flg1) {
                // NO 2 3 4
                flg2 = FLIP_COLOR(flg1);
                flg3 = flg1;
                flg4 = flg2; // joined
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
                prc4->set.flag[lval] |= flg4;
            } else if (flg3) {
                // no 1 2 4
                flg4 = FLIP_COLOR(flg3);
                flg2 = flg4; // joined
                flg1 = flg3;
                prc1->set.flag[lval] |= flg1;
                prc2->set.flag[lval] |= flg2;
                prc4->set.flag[lval] |= flg4;
            } else {
                // no 2 & 4, no 1 & 3
                flg1 = cf_XCA;
                flg2 = cf_XCB;
                flg4 = flg2; // joined
                flg3 = flg1;
                prc1->set.flag[lval] |= flg1;
                prc2->set.flag[lval] |= flg2;
                prc3->set.flag[lval] |= flg3;
                prc4->set.flag[lval] |= flg4;
            }
        }    
    }
    return lnked;
}


// Add SL flags for each candidate, between each side of the RECT
int Mark_UR_Strong_Links_for_cands( PURSTR pur)
{
    int count = 0;
    PABOX2 pb = pur->pb;
    PRCRECT prcrect = &pur->rcrect;
    int i, setval, j, j2, lval;
    PSET pset = &prcrect->set;
    PRCPAIR prcp, prcp2;
    PROWCOL prc1, prc2, prc3, prc4;
    PSET ps1, ps2;
    uint64_t flag;

    // clear set flag of strong links, and COLOR
    for (i = 0; i < 9; i++) {
        pset->flag[i] &= ~(cl_SLA|cf_XAB);
    }
    for (j = 0; j < 4; j++) {
        prcp = &prcrect->rcpair[j]; // each PAIR
        prc1 = &prcp->rowcol[0];
        prc2 = &prcp->rowcol[1];
        ps1 = &prc1->set;
        ps2 = &prc2->set;
        for (i = 0; i < 9; i++) {
            ps1->flag[i] &= ~(cl_SLA|cf_XAB);
            ps2->flag[i] &= ~(cl_SLA|cf_XAB);
        }
    }

    for (i = 0; i < 9; i++) {
        setval = pset->val[i];
        if (!setval) continue;
        lval = setval - 1;
        // do 4 pairs for this SHARED setval
        for (j = 0; j < 4; j++) {
            prcp = &prcrect->rcpair[j]; // each PAIR
            flag = Mark_SL_for_setval_in_rcpair( pb, setval, prcp );
            if (flag) {
                count++;
                pset->flag[i] |= flag; // add flag to SET also
            }
        }
    }
    for (i = 0; i < 9; i++) {
        setval = pset->val[i];
        if (!setval) continue;
        lval = setval - 1;
        // do 4 pairs for this SHARED setval
        for (j = 0; j < 4; j++) {
            prcp = &prcrect->rcpair[j]; // each PAIR
            prc1 = &prcp->rowcol[0];
            prc2 = &prcp->rowcol[1];
            for (j2 = 0; j2 < 4; j2++) { // with each other pair
                if ( j2 == j ) continue;
                prcp2 = &prcrect->rcpair[j2]; // each PAIR
                prc3 = &prcp2->rowcol[0];
                prc4 = &prcp2->rowcol[1];
                if (SAMERCCELL(prc1,prc3)) {
                    Set_Link_Flag( setval, prc1, prc2, prc3, prc4 );
                } else if (SAMERCCELL(prc1,prc4)) {
                    Set_Link_Flag( setval, prc1, prc2, prc3, prc4 );
                } else if (SAMERCCELL(prc2,prc3)) {
                    Set_Link_Flag( setval, prc1, prc2, prc3, prc4 );
                } else if (SAMERCCELL(prc2,prc4)) {
                    Set_Link_Flag( setval, prc1, prc2, prc3, prc4 );
                }
            }
        }
    }
    return count;
}

bool Set_UR_Type( PURSTR pur )
{
    char *tb = GetNxtBuf();
    PABOX2 pb = pur->pb;
    int i;
    string s;
    PROWCOL prc1, prc2, prc3, prc4;

    pur->ur_type = 0;
    prc1 = &pur->rc1;
    prc2 = &pur->rc2;
    prc3 = &pur->rc3;
    prc4 = &pur->rc4;

    // ok 4 cells, in a RECT sharing 2 candidates
    sprintf(tb,"UR: %s %s %s %s ",
        Get_RC_setval_RC_Stg(prc1),
        Get_RC_setval_RC_Stg(prc2),
        Get_RC_setval_RC_Stg(prc3),
        Get_RC_setval_RC_Stg(prc4));
    OUTITUR2(tb);

    memset(&pur->setcnts[0],0,sizeof(pur->setcnts));
    pur->setcnts[pur->scnt1]++;
    pur->setcnts[pur->scnt2]++;
    pur->setcnts[pur->scnt3]++;
    pur->setcnts[pur->scnt4]++;
    // Type 1 Unique Rectangles 2 2 2 3+
    pur->sumsets = 0;
    for (i = 3; i < 9; i++) {
        pur->sumsets += pur->setcnts[i];
    }

    // Hmmm - add restrictionS
    if ( !(pur->setcnts[2] >= 2) ) {
        sprintf(tb,"UR: SKIPPED - pair count %d NOT >= 2!", pur->setcnts[2]);
        OUTITUR2(tb);
        return false;
    }
    if ( !((pur->setcnts[0] == 0) && (pur->setcnts[1] == 0)) ) {
        sprintf(tb,"UR: SKIPPED - counts 0=%d 1=%d NOT == 0!", pur->setcnts[0], pur->setcnts[1]);
        OUTITUR2(tb);
        return false;
    }

    sprintf(tb,"Chk set counts %d%d%d%d! shrd ", 
        pur->scnt1, pur->scnt2, pur->scnt3, pur->scnt4 );
    AddSet2Buf(tb,&pur->set);
    sprintf(EndBuf(tb),", others [%d] ", pur->socnt );
    AddSet2Buf(tb,&pur->set2);
    sprintf(EndBuf(tb),", sum %d ", pur->sumsets);
    OUTITUR2(tb);

    pur->slcnt = Mark_UR_Strong_Links_for_cands(pur);

    pur->prcelim1 = 0;
    pur->prcelim2 = 0;
    if ((pur->setcnts[2] == 3) && (pur->setcnts[3] == 1) && (pur->socnt == 1)) {
        s = "1 - 2223 ";
        if (pur->scnt1 > 2) {
            s += "elim in rc1";
            pur->prcelim1 = prc1;
        } else if (pur->scnt2 > 2) {
            s += "elim in rc2";
            pur->prcelim1 = prc2;
        } else if (pur->scnt3 > 2) {
            s += "elim in rc3";
            pur->prcelim1 = prc3;
        } else if (pur->scnt4 > 2) {
            s += "elim in rc4";
            pur->prcelim1 = prc4;
        } else {
            sprintf(tb,"CODE ERROR: Expected 2223 got %d%d%d%d!",
                pur->scnt1, pur->scnt2, pur->scnt3, pur->scnt4 );
            EXPLODE(tb);
        }
        sprintf(tb,"Type %s - elim ", s.c_str());
        AddSet2Buf(tb,&pur->set);
        sprintf(EndBuf(tb), " from %s ",
            Get_RC_setval_RC_Stg(pur->prcelim1));
        OUTITUR2(tb);
        pur->ur_type = 1;
        return true;
    } else if ((pur->setcnts[2] == 2) && (pur->setcnts[3] == 2) && (pur->socnt == 1)) {
        // // Type 2 Unique Rectangles
        // A type 2 unique rectangle occurs when 2 cells of a rectangle contain 
        // only a pair of possibilities a and b and the other two cells C and D 
        // contain a and b plus one additional possibility c. In that case, one 
        // of C and D must contain c, so c can be eliminated from any cells sharing 
        // a group with C and D.
        s = "2 - 2233 ";
        // Get Cells with 3 chandidates
        if (pur->scnt1 > 2) {
            s += "elim per rc1";
            if (pur->prcelim1)
                pur->prcelim2 = prc1;
            else
                pur->prcelim1 = prc1;
        }
        if (pur->scnt2 > 2) {
            s += "elim per rc2";
            if (pur->prcelim1)
                pur->prcelim2 = prc2;
            else
                pur->prcelim1 = prc2;
        }
        if (pur->scnt3 > 2) {
            s += "elim per rc3";
            if (pur->prcelim1)
                pur->prcelim2 = prc3;
            else
                pur->prcelim1 = prc3;
        }
        if (pur->scnt4 > 2) {
            s += "elim per rc4";
            if (pur->prcelim1)
                pur->prcelim2 = prc4;
            else
                pur->prcelim1 = prc4;
        }
        sprintf(tb,"Type %s - elim ", s.c_str());
        AddSet2Buf(tb,&pur->set2);
        if (!pur->prcelim1 || !pur->prcelim2) {
            sprintf(EndBuf(tb),"CODE ERROR: One or other elims is NULL");
            EXPLODE(tb);
        }
        sprintf(EndBuf(tb), " from SRCB as %s & %s",
            Get_RC_setval_RC_Stg(pur->prcelim1),
            Get_RC_setval_RC_Stg(pur->prcelim2));
        OUTITUR2(tb);
        pur->ur_type = 2;

        return true;
    } else if ((pur->setcnts[2] == 2) && (pur->socnt == 2) && (pur->sumsets == 2)) {
        // Type 3 Unique Rectangles 2234 - Similar to above
        // like - Chk set counts 3242! shrd 79, others [2] 18 sum 2
        //

    }

    // ***************************************************************************
    // others depend on STRONG LINKS
    // ***************************************************************************
    if (!pur->slcnt) {
        sprintf(tb,"UR: SKIPPED - no strong links");
        OUTITUR2(tb);
        return false;
    }

    // but need to know specifically WHICH are the STRONG links
    // A type 4 unique rectangle is a situation where 2 cells of a rectangle contain only a pair of 
    // possibilities a and b, the other cells C and D contain a and b plus any number of additional 
    // values, and there is a strong link between a@C and a@D. In that case, selecting b@C would 
    // force a@D, creating a non-unique rectangle; ditto b@D. So b can be eliminated from C and D. 
    // The candidate a & b are in pur->set
    if (pur->setcnts[2] == 2) {
        s = "4 22?? ";
        if (pur->scnt1 == 2) {
            s += "rc1 ";
            if (pur->prcelim1)
                pur->prcelim2 = prc1;
            else
                pur->prcelim1 = prc1;
            prc1 = 0;
        }
        if (pur->scnt2 == 2) {
            s += "rc2 ";
            if (pur->prcelim1)
                pur->prcelim2 = prc2;
            else
                pur->prcelim1 = prc2;
            prc2 = 0;
        }
        if (pur->scnt3 == 2) {
            s += "rc3 ";
            if (pur->prcelim1)
                pur->prcelim2 = prc3;
            else
                pur->prcelim1 = prc3;
            prc3 = 0;
        }
        if (pur->scnt4 == 2) {
            s += "rc4 ";
            if (pur->prcelim1)
                pur->prcelim2 = prc4;
            else
                pur->prcelim1 = prc4;
            prc4 = 0;
        }
        sprintf(tb,"Type %s ", s.c_str());
        if (!(pur->prcelim1) || !(pur->prcelim2)) {
            sprintf(EndBuf(tb),"CODE ERROR: One or other elims is NULL");
            EXPLODE(tb);
        }
        // Now we know the ENDS (cells) that contain the ab pair (in set)
        // Get alternates
        strcat(tb,"sets ab ");
        AddSet2Buf(tb,&pur->prcelim1->set);
        ADDSP(tb);
        AddSet2Buf(tb,&pur->prcelim2->set);
        ADDSP(tb);

        pur->prcC = 0;
        pur->prcD = 0;
        int setcnt;
        if (prc1) {
            if (pur->prcC)
                pur->prcD = prc1;
            else
                pur->prcC = prc1;
        }
        if (prc2) {
            if (pur->prcC)
                pur->prcD = prc2;
            else
                pur->prcC = prc2;
        }
        if (prc3) {
            if (pur->prcC)
                pur->prcD = prc3;
            else
                pur->prcC = prc3;
        }
        if (prc4) {
            if (pur->prcC)
                pur->prcD = prc4;
            else
                pur->prcC = prc4;
        }
        if (!pur->prcC || !pur->prcD) {
            sprintf(EndBuf(tb),"CODE ERROR: One or other alternates is NULL");
            EXPLODE(tb);
        }

        strcat(tb,"CD ");
        AddSet2Buf(tb,&pur->prcC->set);
        ADDSP(tb);
        AddSet2Buf(tb,&pur->prcD->set);
        ADDSP(tb);
        // Now have a 2 ab cells, and 2 CD cells
        // Check for STRONG LINKS - a@C & a@D or b@C & b@D
        int lval;
        for (i = 0; i < 9; i++) {
            pur->setval = pur->set.val[i];
            if (!pur->setval) continue;
            pur->lnkage = 0;
            lval = pur->setval - 1;
            // Check a@C
            if (pur->prcelim1->row == pur->prcC->row) {
                // is prc1 have a SL to prcC on this setval
                setcnt = Count_setval_in_Row( pb, pur->prcC->row, pur->prcC->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkC | lnkRow | 0x1C);
                    pur->prcelim1->set.flag[lval] |= cl_SLR;
                    pur->prcC->set.flag[lval] |= cl_SLR;
                    pur->rcpair1.rowcol[0] = *pur->prcelim1;
                    pur->rcpair1.rowcol[1] = *pur->prcC;
                    pur->rcpair2.rowcol[0] = *pur->prcelim2;
                    pur->rcpair2.rowcol[1] = *pur->prcD;
                }
            } else if (pur->prcelim1->col == pur->prcC->col) {
                setcnt = Count_setval_in_Col( pb, pur->prcC->row, pur->prcC->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkC | lnkCol | 0x1C);
                    pur->prcelim1->set.flag[lval] |= cl_SLC;
                    pur->prcC->set.flag[lval] |= cl_SLC;
                    pur->rcpair1.rowcol[0] = *pur->prcelim1;
                    pur->rcpair1.rowcol[1] = *pur->prcC;
                    pur->rcpair2.rowcol[0] = *pur->prcelim2;
                    pur->rcpair2.rowcol[1] = *pur->prcD;
                }
            } else if (pur->prcelim2->col == pur->prcC->col) {
                setcnt = Count_setval_in_Col( pb, pur->prcC->row, pur->prcC->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkC | lnkCol | 0x2C);
                    pur->prcelim2->set.flag[lval] |= cl_SLC;
                    pur->prcC->set.flag[lval] |= cl_SLC;
                    pur->rcpair1.rowcol[0] = *pur->prcelim2;
                    pur->rcpair1.rowcol[1] = *pur->prcC;
                    pur->rcpair2.rowcol[0] = *pur->prcelim1;
                    pur->rcpair2.rowcol[1] = *pur->prcD;
                }
            } else if (pur->prcelim2->row == pur->prcC->row) {
                setcnt = Count_setval_in_Row( pb, pur->prcC->row, pur->prcC->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkC | lnkRow | 0x2C);
                    pur->prcelim2->set.flag[lval] |= cl_SLR;
                    pur->prcC->set.flag[lval] |= cl_SLR;
                    pur->rcpair1.rowcol[0] = *pur->prcelim2;
                    pur->rcpair1.rowcol[1] = *pur->prcC;
                    pur->rcpair2.rowcol[0] = *pur->prcelim1;
                    pur->rcpair2.rowcol[1] = *pur->prcD;
                }
            }

            // Chk a@D
            if (pur->prcelim1->row == pur->prcD->row) {
                // is prc1 have a SL to prcC on this setval
                setcnt = Count_setval_in_Row( pb, pur->prcD->row, pur->prcD->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkD | lnkRow | 0x2D);
                    pur->prcelim1->set.flag[lval] |= cl_SLR;
                    pur->prcD->set.flag[lval] |= cl_SLR;
                    pur->rcpair3.rowcol[0] = *pur->prcelim1;
                    pur->rcpair3.rowcol[1] = *pur->prcD;
                    pur->rcpair4.rowcol[0] = *pur->prcelim2;
                    pur->rcpair4.rowcol[1] = *pur->prcC;
                }
            } else if (pur->prcelim1->col == pur->prcD->col) {
                setcnt = Count_setval_in_Col( pb, pur->prcD->row, pur->prcD->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkD | lnkCol | 0x1D);
                    pur->prcelim1->set.flag[lval] |= cl_SLC;
                    pur->prcD->set.flag[lval] |= cl_SLC;
                    pur->rcpair3.rowcol[0] = *pur->prcelim1;
                    pur->rcpair3.rowcol[1] = *pur->prcD;
                    pur->rcpair4.rowcol[0] = *pur->prcelim2;
                    pur->rcpair4.rowcol[1] = *pur->prcC;
                }
            } else if (pur->prcelim2->col == pur->prcD->col) {
                setcnt = Count_setval_in_Col( pb, pur->prcD->row, pur->prcD->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkD | lnkCol | 0x2D);
                    pur->prcelim2->set.flag[lval] |= cl_SLC;
                    pur->prcD->set.flag[lval] |= cl_SLC;
                    pur->rcpair3.rowcol[0] = *pur->prcelim2;
                    pur->rcpair3.rowcol[1] = *pur->prcD;
                    pur->rcpair4.rowcol[0] = *pur->prcelim1;
                    pur->rcpair4.rowcol[1] = *pur->prcC;
                }
            } else if (pur->prcelim2->row == pur->prcD->row) {
                setcnt = Count_setval_in_Row( pb, pur->prcD->row, pur->prcD->col, pur->setval );
                if (setcnt == 2) {
                    pur->lnkage |= (lnkD | lnkRow | 0x2D);
                    pur->prcelim2->set.flag[lval] |= cl_SLR;
                    pur->prcD->set.flag[lval] |= cl_SLR;
                    pur->rcpair3.rowcol[0] = *pur->prcelim2;
                    pur->rcpair3.rowcol[1] = *pur->prcD;
                    pur->rcpair4.rowcol[0] = *pur->prcelim1;
                    pur->rcpair4.rowcol[1] = *pur->prcC;
                }
            }

            if ((pur->lnkage & lnkCD) == lnkCD) {
                sprintf(EndBuf(tb),"Pairs: %s & %s and %s & %s ",
                    Get_RCPAIR_Stg( &pur->rcpair1, pur->setval, true ),
                    Get_RCPAIR_Stg( &pur->rcpair2, pur->setval, true ),
                    Get_RCPAIR_Stg( &pur->rcpair3, pur->setval, true ),
                    Get_RCPAIR_Stg( &pur->rcpair4, pur->setval, true ));
                OUTITUR2(tb);
                pur->ur_type = 4;
                return true;
            }
        }
    }
    return false;
}



int Mark_Same_Row_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims = 0 );
int Mark_Same_Col_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims = 0 );
int Mark_Same_Box_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims = 0 );
int Mark_Same_RCB_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims = 0 );

int Mark_Same_Row_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims )
{
    int count = 0;
    int row, col, val;
    int col1;
    int lval = setval - 1;
    ROWCOL rc;
    PSET ps;

    row = prc->row;
    col = prc->col;

    rc.row = row; // keep ROW constant
    for (col1 = 0; col1 < 9; col1++) {
        val = pb->line[row].val[col1];
        if (val) continue;
        ps = &pb->line[row].set[col1];
        if ( !(ps->val[lval]) ) continue; // does NOT have this candidate
        rc.col = col1;
        if (prc_in_chain(&rc,pvrc)) continue; // skip members of RECTANGLE
        if ( ps->flag[lval] & eflg ) continue; // already MARKED
        ps->flag[lval] |= eflg;
        count++;
        if (pelims) {
            COPY_SET(&rc.set,ps);
            pelims->push_back(rc);
        }
    }

    return count;
}

int Mark_Same_Col_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims )
{
    int count = 0;
    int row, col, val;
    int row1;
    int lval = setval - 1;
    ROWCOL rc;
    PSET ps;

    row = prc->row;
    col = prc->col;

    rc.col = col; // keep COL constant
    for (row1 = 0; row1 < 9; row1++) {
        val = pb->line[row1].val[col];
        if (val) continue;
        ps = &pb->line[row1].set[col];
        if ( !(ps->val[lval]) ) continue; // does NOT have this candidate
        rc.row = row1;
        if (prc_in_chain(&rc,pvrc)) continue; // skip members of RECTANGLE
        if ( ps->flag[lval] & eflg ) continue; // already MARKED
        ps->flag[lval] |= eflg;
        count++;
        if (pelims) {
            COPY_SET(&rc.set,ps);
            pelims->push_back(rc);
        }
    }

    return count;
}

int Mark_Same_Box_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims )
{
    int count = 0;
    int row, col, val;
    int lval = setval - 1;
    ROWCOL rc;
    PSET ps;

    row = prc->row;
    col = prc->col;

    int r = (row / 3) * 3;
    int c = (col / 3) * 3;
    int rw, cl;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            rc.row = r + rw;
            rc.col = c + cl;
            val = pb->line[rc.row].val[rc.col];
            if (val) continue;
            ps = &pb->line[rc.row].set[rc.col];
            if ( !(ps->val[lval]) ) continue; // does NOT have this candidate
            if (prc_in_chain(&rc,pvrc)) continue; // skip members of RECTANGLE
            if ( ps->flag[lval] & eflg ) continue; // already MARKED
            ps->flag[lval] |= eflg;
            count++;
            if (pelims) {
                COPY_SET(&rc.set,ps);
                pelims->push_back(rc);
            }
        }
    }

    return count;
}



int Mark_Same_RCB_not_in_vector( PABOX2 pb, int setval, PROWCOL prc, vRC *pvrc, uint64_t eflg, vRC *pelims )
{
    int count = 0;
    count += Mark_Same_Row_not_in_vector( pb, setval, prc, pvrc, eflg, pelims );
    count += Mark_Same_Col_not_in_vector( pb, setval, prc, pvrc, eflg, pelims );
    count += Mark_Same_Box_not_in_vector( pb, setval, prc, pvrc, eflg, pelims );
    return count;
}

int Mark_UR_Eliminations( PURSTR pur )
{
    int count = 0;
    PABOX2 pb = pur->pb;
    PRCRECT prcrect = &pur->rcrect;
    int type = pur->ur_type;
    PROWCOL prc, prc2;
    PSET ps, ps2;
    int i, setval, val;
    ROWCOL rc;
    switch (type)
    {
    case 1: // 2223 - Eliminate 2 candidate from the cell with the 3rd candidate
        // Set_UR_Type() set the value of pur->prcelim1 to this cell
        // pur->set contains the candidates to eliminate
        prc = pur->prcelim1;
        if (prc) {
            ps = &pur->set;
            for (i = 0; i < 9; i++) {
                setval = ps->val[i];
                if (!setval) continue;
                val = pb->line[prc->row].val[prc->col];
                if (val) continue; // this would be an ERROR
                ps2 = &pb->line[prc->row].set[prc->col];
                if ( !(ps2->val[i]) ) continue; // also would be an ERROR
                if (ps2->flag[i] & ur_elim2 ) continue; // cf_URE already marked - do not count twice
                ps2->flag[i] |= ur_elim2; // cf_URE mark elimination
                count++;    // bump count
                rc.row = prc->row;
                rc.col = prc->col;
                COPY_SET(&rc.set,ps2);
                pur->pelims->push_back(rc);
                if (pur->pelimby) {
                    pur->pelimby->push_back(pur->rc1);
                    pur->pelimby->push_back(pur->rc2);
                    pur->pelimby->push_back(pur->rc3);
                    pur->pelimby->push_back(pur->rc4);
                }
            }
        }
        break;
    case 2: // 2233 - Eliminate the 3rd candidate form SAME ROW, COL, BOX
        // Set_UR_Type() set the value of pur->prcelim1 and pur->prcelim2 to these cells
        // pur->set2 contains the candidates to eliminate
        // Only MARK those in the SAME ROW, COL, BOX of this PAIR
        prc = pur->prcelim1;
        prc2 = pur->prcelim2;
        if (prc && prc2) {
            ps = &pur->set2;
            i = Get_Set_Cnt(ps,&setval);    // would be ERROR if i was NOT 1, but not checked
            // Mark same ROW, COL and BOX, but NOT if member of RECTANGLE
            vRC vrc; // But avoid marking the RECTANGLE members
            vrc.push_back(pur->rc1);
            vrc.push_back(pur->rc2);
            vrc.push_back(pur->rc3);
            vrc.push_back(pur->rc4);
            if (prc->row == prc2->row) {
                count += Mark_Same_Row_not_in_vector( pb, setval, prc, &vrc, ur_elim2, pur->pelims );
            } else if (prc->col == prc2->col) {
                count += Mark_Same_Col_not_in_vector( pb, setval, prc, &vrc, ur_elim2, pur->pelims );
            }
            if (GETBOX(prc->row,prc->col) == GETBOX(prc2->row,prc2->col)) {
                count += Mark_Same_Box_not_in_vector( pb, setval, prc, &vrc, ur_elim2, pur->pelims );
            }
            if (count && pur->pelims && pur->pelimby) {
                pur->pelimby->push_back(pur->rc1);
                pur->pelimby->push_back(pur->rc2);
                pur->pelimby->push_back(pur->rc3);
                pur->pelimby->push_back(pur->rc4);
            }
        }
        break;
    case 4: // 22?? - And STRONG LINKS
        // Set_UR_Type() set the value of pur->prcelim1 and pur->prcelim2 to the cells with 2
        // And pur->prcC and prcD for the other two with which there is a strong link
        // The strong link pairs copied into pur->rcpair1, pair2, pair3, pair4
        // Type 4 22?? rc1 rc3  sets ab 25 25 CD 256 257 
        // Pairs: 25@D5U==256@D6U & 25@H5U--257@H6U and 25@H5U==257@H6U & 25@D5U==256@D6U 
        // HOW to work out to eliminate 5@D6 and 5@H6????
        // In the example a=2@D5 b=5@H5 - C=2@D6 D=5@H6
        // so a@C = 2@D5===2@D6  a@D = 2@D5===2@D6 then by elimination
        // Selecting 5@D6->2@D5->5@H5->2@H6
        if (pur->prcelim1 && pur->prcelim2 && pur->prcC && pur->prcD) {

        }
        break;
    }
    pur->elim_count += count;
    return count;
}

void Show_UR_Elims2( PURSTR pur )
{
    vRC *pelims = pur->pelims;
    if (!pelims) return;
    vRC *pelimby = pur->pelimby;
    size_t max1 = pelims->size();
    if (!max1)
        return;
    size_t max2 = 0;
    if (pelimby)
        max2 = pelimby->size();
    size_t ii, i2;
    PROWCOL prc;
    int i, val;
    char *tb = GetNxtBuf();

    sprintf(tb,"UR2-Elims: %d ", (int)max1);

    for (ii = 0; ii < max1; ii++) {
        prc = &pelims->at(ii);
        for (i = 0; i < 9; i++) {
            val = prc->set.val[i];
            if (!val) continue;
            if ( !( prc->set.flag[i] & ur_elim2 ) ) continue;
            sprintf(EndBuf(tb),"%d", val);
        }
        sprintf(EndBuf(tb),"@%c%d ", (char)(prc->row + 'A'), prc->col + 1);
    }

    // show cells involved, if available
    if (pelimby && max2) {
        strcat(tb, " by ");
        for (i2 = 0; i2 < max2; i2++) {
            prc = &pelimby->at(i2);
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc));
        }
    }
    OUTIT(tb);
}


int Do_Clear_UniqRect2( PABOX2 pb )
{
    int count = Do_Fill_By_Flags( pb, cf_URE, cf_UR, "UR2", "Uniq.Rect2", cl_UNR );
    Set_Rect_Link_Cnt(0);   // stop showing unique rectange links
    return count;
}

int Process_Unique_Rectanges2( PURSTR pur )
{
    int count = 0;
    PABOX2 pb;
    int row1, col1, val;
    int row2, col2, fcol;
    int row3, col3, scnt;
    int boxes[9];
    int boxcnt;
    int box1, box2, box3, box4, i;
    PSET ps1, ps2, ps3, ps4;
    PROWCOL prc1, prc2, prc3, prc4;
    char *tb = GetNxtBuf();


    pb   = pur->pb;
    prc1 = &pur->rc1;
    prc2 = &pur->rc2;
    prc3 = &pur->rc3;
    prc4 = &pur->rc4;

    for (row1 = 0; row1 < 9; row1++) {
        for (col1 = 0; col1 < 9; col1++) {
            val = pb->line[row1].val[col1];
            if (val) continue;
            ps1 = &pb->line[row1].set[col1];
            prc1->row = row1;
            prc1->col = col1;
            prc1->box = box1 = GETBOX(row1,col1);
            COPY_SET(&prc1->set,ps1);
            pur->scnt1 = Get_Set_Cnt(ps1);
            fcol = col1 + 1;
            for (row2 = 0; row2 < 9; row2++) {
                for (col2 = fcol; col2 < 9; col2++) {
                    val = pb->line[row2].val[col2];
                    if (val) continue;
                    ps2 = &pb->line[row2].set[col2];
                    prc2->row = row2;
                    prc2->col = col2;
                    prc2->box = box2 = GETBOX(row2,col2);
                    COPY_SET(&prc2->set,ps2);
                    pur->scnt2 = Get_Set_Cnt(ps2);
                    if (SAMERCCELL(prc1,prc2)) continue;
                    // GOT TWO CELLS - Do we have a RECTANGE
                    if (prc1->row == prc2->row) {
                        // they share a ROW, search ROW onwards, using fixed COLS - col1 and col2
                        pur->rcrect.rcpair[0].rowcol[0] = *prc1;
                        pur->rcrect.rcpair[0].rowcol[1] = *prc2;
                        prc3->col = prc1->col;
                        prc4->col = prc2->col;
                        for (row3 = row1 + 1; row3 < 9; row3++) {
                            val = pb->line[row3].val[col1];
                            if (val) continue;
                            val = pb->line[row3].val[col2];
                            if (val) continue;
                            ps3 = &pb->line[row3].set[col1];
                            prc3->row = row3;
                            COPY_SET(&prc3->set,ps3);
                            ps4 = &pb->line[row3].set[col2];
                            prc4->row = row3;
                            COPY_SET(&prc4->set,ps4);
                            if (SAMERCCELL(prc3,prc4)) continue;
                            if (SAMERCCELL(prc1,prc3)) continue;
                            if (SAMERCCELL(prc1,prc4)) continue;
                            if (SAMERCCELL(prc2,prc3)) continue;
                            if (SAMERCCELL(prc2,prc4)) continue;
                            // got FOUR different CELL in RECTANGLE FORM
                            pur->scnt3 = Get_Set_Cnt(ps3);
                            pur->scnt4 = Get_Set_Cnt(ps4);
                            prc3->box  = box3 = GETBOX(prc3->row,prc3->col);
                            prc4->box  = box4 = GETBOX(prc4->row,prc4->col);

                            // skip if ALL in SAME box
                            if ((box1 == box2) && (box2 == box3) && (box3 == box4)) continue;
                            // skip if NOT exactly 2 boxes
                            memset(&boxes[0],0,sizeof(boxes));
                            boxes[box1]++;
                            boxes[box2]++;
                            boxes[box3]++;
                            boxes[box4]++;
                            boxcnt = 0;
                            for (i = 0; i < 9; i++) {
                                if (boxes[i])
                                    boxcnt++;
                            }
                            if ( !(boxcnt == 2) ) continue;

                            // 3 & 4 share a ROW - row3
                            pur->rcrect.rcpair[2].rowcol[0] = *prc3;
                            pur->rcrect.rcpair[2].rowcol[1] = *prc4;
                            // All that remains is do they SHARE at least TWO candidates
                            // AND is there a STRONG LINK on either of the TWO candidates
                            ZERO_SET(&pur->set);
                            scnt = Add_Common_Cands( &pur->set, ps1, ps2, ps3, ps4 );
                            if (scnt != 2) continue;
                            ZERO_SET(&pur->set2);
                            pur->socnt = Add_Not_Shared_4( &pur->set, &pur->set2, ps1, ps2, ps3, ps4 );
                            // Must line these up
                            if ((prc2->col == prc3->col)||(prc2->row == prc3->row)) {
                                pur->rcrect.rcpair[1].rowcol[0] = *prc2;
                                pur->rcrect.rcpair[1].rowcol[1] = *prc3;
                                pur->rcrect.rcpair[3].rowcol[0] = *prc4;
                                pur->rcrect.rcpair[3].rowcol[1] = *prc1;
                            } else if ((prc2->col == prc4->col)||(prc2->row == prc4->row)) {
                                pur->rcrect.rcpair[1].rowcol[0] = *prc2;
                                pur->rcrect.rcpair[1].rowcol[1] = *prc4;
                                pur->rcrect.rcpair[3].rowcol[0] = *prc3;
                                pur->rcrect.rcpair[3].rowcol[1] = *prc1;
                            } else {
                                // WTF!!!
                                sprintf(tb,"CODE ERROR: Expected either ROW or COL match" MEOL);
                                sprintf(EndBuf(tb),"UR: %s %s %s %s ",
                                    Get_RC_setval_RC_Stg(prc1),
                                    Get_RC_setval_RC_Stg(prc2),
                                    Get_RC_setval_RC_Stg(prc3),
                                    Get_RC_setval_RC_Stg(prc4));
                                EXPLODE(tb);
                                continue;
                            }

                            if (prcrect_in_pvrcr( &pur->rcrect, pur->pvrcr )) continue;

                            COPY_SET(&pur->rcrect.set,&pur->set);

                            if ( !Set_UR_Type( pur ) ) {
                                sprintf(tb,"Rect FAILED Set_UR_Type()!");
                                OUTITUR2(tb);
                                continue;
                            }

                            // RECT == 4 PAIRS
                            Mark_UR_Eliminations(pur);

                            if (add_debug_ur2)
                                Show_RCRECT( &pur->rcrect, "ADDED SR" );
                            pur->pvrcr->push_back(pur->rcrect);
                        }
                    } else if (prc1->col == prc2->col) {
                        // they share a COL, search COL onwards, using fixed ROWS - row1 and row2
                        prc3->row = prc1->row;
                        prc4->row = prc2->row;
                        pur->rcrect.rcpair[0].rowcol[0] = *prc1;
                        pur->rcrect.rcpair[0].rowcol[1] = *prc2;
                        for (col3 = col1 + 1; col3 < 9; col3++) {
                            val = pb->line[row1].val[col3];
                            if (val) continue;
                            ps3 = &pb->line[row1].set[col3];
                            prc3->col = col3;
                            COPY_SET(&prc3->set,ps3);
                            val = pb->line[row2].val[col3];
                            if (val) continue;
                            ps4 = &pb->line[row2].set[col3];
                            prc4->col = col3;
                            COPY_SET(&prc4->set,ps4);
                            pur->scnt3 = Get_Set_Cnt(ps3);
                            pur->scnt4 = Get_Set_Cnt(ps4);
                            // 3 & 4 share a COL - col3
                            prc3->box = box3 = GETBOX(prc3->row,prc3->col);
                            prc4->box = box4 = GETBOX(prc4->row,prc4->col);

                            // skip if ALL in SAME box
                            if ((box1 == box2) && (box2 == box3) && (box3 == box4)) continue;
                            // skip if NOT exactly 2 boxes
                            memset(&boxes[0],0,sizeof(boxes));
                            boxes[box1]++;
                            boxes[box2]++;
                            boxes[box3]++;
                            boxes[box4]++;
                            boxcnt = 0;
                            for (i = 0; i < 9; i++) {
                                if (boxes[i])
                                    boxcnt++;
                            }
                            if ( !(boxcnt == 2) ) continue;

                            pur->rcrect.rcpair[2].rowcol[0] = *prc3;
                            pur->rcrect.rcpair[2].rowcol[1] = *prc4;
                            // All that remains is do they SHARE at least TWO candidates
                            // AND is there a STRONG LINK on either of the TWO candidates
                            ZERO_SET(&pur->set);
                            scnt = Add_Common_Cands( &pur->set, ps1, ps2, ps3, ps4 );
                            if (scnt != 2) continue;
                            ZERO_SET(&pur->set2);
                            pur->socnt = Add_Not_Shared_4( &pur->set, &pur->set2, ps1, ps2, ps3, ps4 );
                            // Must line these up
                            if ((prc2->col == prc3->col)||(prc2->row == prc3->row)) {
                                pur->rcrect.rcpair[1].rowcol[0] = *prc2;
                                pur->rcrect.rcpair[1].rowcol[1] = *prc3;
                                pur->rcrect.rcpair[3].rowcol[0] = *prc4;
                                pur->rcrect.rcpair[3].rowcol[1] = *prc1;
                            } else if ((prc2->col == prc4->col)||(prc2->row == prc4->row)) {
                                pur->rcrect.rcpair[1].rowcol[0] = *prc2;
                                pur->rcrect.rcpair[1].rowcol[1] = *prc4;
                                pur->rcrect.rcpair[3].rowcol[0] = *prc3;
                                pur->rcrect.rcpair[3].rowcol[1] = *prc1;
                            } else {
                                // WTF!!!
                                sprintf(tb,"CODE ERROR: Expected either ROW or COL match" MEOL);
                                sprintf(EndBuf(tb),"UR: %s %s %s %s ",
                                    Get_RC_setval_RC_Stg(prc1),
                                    Get_RC_setval_RC_Stg(prc2),
                                    Get_RC_setval_RC_Stg(prc3),
                                    Get_RC_setval_RC_Stg(prc4));
                                EXPLODE(tb);
                                continue;
                            }
                            if (prcrect_in_pvrcr( &pur->rcrect, pur->pvrcr )) continue;
                            // ok 4 cells, in a RECT sharing 2 candidates
                            COPY_SET(&pur->rcrect.set,&pur->set);

                            if ( !Set_UR_Type( pur ) ) {
                                sprintf(tb,"Rect FAILED Set_UR_Type!");
                                OUTITUR2(tb);
                                continue;
                            }

                            // RECT == 4 PAIRS
                            Mark_UR_Eliminations(pur);

                            if (add_debug_ur2)
                                Show_RCRECT( &pur->rcrect, "ADDED SC" );
                            pur->pvrcr->push_back(pur->rcrect);
                        }
                    } else
                        continue; // Do NOT share a ROW or COL
                }
                fcol = 0;
            }
        }
    }
    count = pur->elim_count;
    return count;
}

// For DIAGNOSTIC and HELP simply MARK/DRAW all Unique Rectanges
// Get an two empty cells, and see if a Unique Rectangle can be formed
int Mark_Unique_Rectanges2( PABOX2 pb )
{
    int count = 0;

    PURSTR pur = &_s_urstr;
    char *tb = GetNxtBuf();

    pur->pvrcr      = Get_UR_Rects2_Ptr();
    pur->pb         = pb;
    pur->pvrcr->clear();
    ur_rect_cnt = 0;
    pur->elim_count = 0; // count of eliminations by Unique Rectangels
    if (pur->pelims)
        pur->pelims->clear();
    else
        pur->pelims = new vRC;
    if (pur->pelimby)
        pur->pelimby->clear();
    else
        pur->pelimby = new vRC;

    sprintf(tb,"Mark Unique Rectangles 2.");
    OUTIT(tb);

    count = Process_Unique_Rectanges2(pur);

    ur_rect_cnt = (int)pur->pvrcr->size();
    if (ur_rect_cnt) {
        if (pur->elim_count) {
            Show_UR_Elims2( pur );
            sprintf(tb,"Marked %d Unique Rectangles, elim %d for painting.", ur_rect_cnt, pur->elim_count);
            count = pur->elim_count;
            Stack_Fill(Do_Clear_UniqRect2);
        } else {
            sprintf(tb,"Marked %d Unique Rectangles for painting.", ur_rect_cnt);
            count = ur_rect_cnt;
        }
    } else {
        sprintf(tb,"No Unique Rectangles FOUND!");
    }
    OUTITUR2(tb);
    return count;
}


int Do_UniqRect2_Scan( PABOX2 pb )
{
    int count = 0;
    return count;
}

// eof - Sudo_URect2.cxx
