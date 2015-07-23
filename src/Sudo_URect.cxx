// Sudo_URect.cxx

#include "Sudoku.hxx"

// Unique Rectangles - assumes UNIQUE solution (as it should be for a well formed Sudoku)
// from : http://zitowolf.net/sudoku/strategy.html#xcycles - and down
// Find two cells that have the SAME candidate pair
// Search Row and Col for 2 other cells having at least that same pair of candidates, plus 1 for one of them

// Type 1 Unique Rectangles
// There are several ways to identify such cases. The simplest case, type 1, occurs when 3 
// cells of a rectangle contain only a pair of possibilities ab and the fourth cell contains 
// the same two possibilities plus any number of additional values. In that case, filling 
// the fourth cell with either a or b would create a non-unique rectangle, so possibilities 
// a and b can be eliminated from the fourth cell. The example above is a type-1 unique rectangle.
// See: examples\uniqrect1.txt
// 512080094960005082083920651040209175071508429295700863039072548020850937050493216
// UR-Elim: 6@D1 by 36@D5 36@E5 368@D1 36@E1 SC-Type-4
// UR-Elim: 6@E1 by 36@D5 36@E5 368@D1 36@E1 SC-Type-4
// Hmmmm, text says shuld be Type 1, Elim 3@D1 and 6@D1 ???????
// Yes, there is a STRONG LINK on the 3, but NOT on the 6 ??????

// Type 2 Unique Rectangles
// A type 2 unique rectangle occurs when 2 cells of a rectangle contain only a pair of possibilities 
// a and b and the other two cells C and D contain a and b plus one additional possibility c. In 
// that case, one of C and D must contain c, so c can be eliminated from any cells sharing a group 
// with C and D. This puzzle contains a type-2 unique rectangle at A3, C3, A5 and C5 on values 5 and 7. 
// Value 6 must be assigned to one of A5 and C5 or a non-unique rectangle will be created. This 
// eliminates 6 from A6 and J5.
// examples\uniqrect2.txt
// 290000104001294735340001000000630040002040800450012000000420079724080600000100428
// UR-Elim: 6@I5 by 57@A3 57@C3 567@A5 567@C5 SC-elim-ROW
// UR-Elim: 6@A6 by 57@A3 57@C3 567@A5 567@C5 SB-elim-Box
// Agrees EXACTLY to text

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

// from : http://zitowolf.net/sudoku/strategy.html#xcycles down page...
// Hidden Unique Rectangles
// In the type-4 case we saw how one strong linkage could create 
// a nonunique-rectangle configuration. Generalizing, we define a 
// Hidden Rectangle as a rectangle of four cells containing a common 
// pair of possibilities a and b, with strong links on a and/or b 
// between cells such that selecting a or b at one of the cells 
// forces a non-unique rectangle. An example will help make this 
// more concrete. Here, three cells of a rectangle have possibilities 
// beyond the shared pair, but the strong links 6@G3=6@E3 and 
// 6@E1=6@G1 cause 7@G3 to force a non-unique rectangle. 
// ie ELIM: 7@G3
// Need new code to FIND this...

#define OUTITUR(tb) if (add_debug_ur) OUTIT(tb)

// used void Paint_Strong_Links(HDC hdc)
static int _s_iSLrect = 0;
static vRC _s_vSLrect;
int Get_Rect_Link_Cnt() { return _s_iSLrect; }
int Set_Rect_Link_Cnt(int cnt) { int i = _s_iSLrect; _s_iSLrect = cnt; return i; }
vRC *Get_Rect_Links() { return &_s_vSLrect; }

int only_some_unr = 0;

// ====================================================================
// Services Used
// *************

// Returning TRUE if ALL of SET1 are present in SET2
// FALSE is SET2 is missing any value in SET1
bool Set1_shared_with_Set2(PSET ps1, PSET ps2)
{
    int i, setval;
    for (i = 0; i < 9; i++) {
        setval = ps1->val[i];
        if (!setval) continue;
        if (!ps2->val[setval - 1])
            return false; // oops, SET2 does NOT have this value
    }
    return true;
}


bool Find_Uniq_Rect( PABOX2 pb, PROWCOL prc, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4,
                     vRC &vdone )
{
    bool bret = false;
    int row, col, val, scnt, scnt2;
	int comm[9];
	int comm2[9];
    PSET ps, ps2;
    if (prc->col == prc2->col) {
        row = prc->row; // fix row per 1, search cols
        for (col = 0; col < 9; col++) {
            if (col == prc->col) continue; // thats col of cell 1
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            // check if ALL in SET1 are pesent in SET2
            if (!Set1_shared_with_Set2(&prc->set,ps)) continue; // no 2 shared cands
            // Yeek got a THIRD sharing these 2 candidates
            // Do we need more restriction, like how many more, if more???
			scnt = Get_Set_Cnt2(ps,comm);
			if (scnt > 3) continue; // TOO many SHARED - only accept +1 at MOST!
            prc3->row = row;
            prc3->col = col;
            if (Row_Col_in_RC_Vector(vdone,row,col)) continue;
            COPY_SET(&prc3->set,ps);
            // only remains to test the fourth, which is presribed by current row col set
            row = prc2->row;
            // current colum
            prc4->row = row;
            prc4->col = col;
            if (Row_Col_in_RC_Vector(vdone,row,col)) continue;
            val = pb->line[row].val[col];
            if (val) continue;
            ps2 = &pb->line[row].set[col];
			scnt2 = Get_Set_Cnt2(ps2,comm2);
			if (scnt2 > 3) continue; // TOO many candidates
            COPY_SET(&prc4->set,ps2);
            // check if ALL in SET1 are pesent in SET2
            if (!Set1_shared_with_Set2(&prc->set,ps2)) continue; // no 2 shared cands
            // Yowee, got RECTANGLE, but how many candidates
			// Any MORE restrictions??? Like if 1 is 3, the other MUST be 2
            // Type 1 will be tested later
			// if ( !(((scnt == 2) && (scnt2 == 3))||((scnt == 3)&&(scnt2 == 2))) ) continue;
            //if (GETBOX(prc3->row,prc3->col) != GETBOX(prc4->row,prc4->col)) continue; // NOT in SAME BOX
            bret = true;
            break;
        }
    } else if (prc->row == prc2->row) {
        col = prc->col;
        for (row = 0; row < 9; row++) {
            if (row == prc->row) continue; // thats col of cell 1
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            // check if ALL in SET1 are pesent in SET2
            if (!Set1_shared_with_Set2(&prc->set,ps)) continue; // no 2 shared cands
            // Yeek got a THIRD sharing these 2 candidates
            // Do we need more restriction, like how many more, if more???
			scnt = Get_Set_Cnt2(ps,comm);
			if (scnt > 3) continue; // TOO many SHARED - only accept +1 at MOST!
            prc3->row = row;
            prc3->col = col;
            COPY_SET(&prc3->set,ps);
            // only remains to test the fourth, which is presribed by current row col set
            // current row
            col = prc2->col;
            prc4->row = row;
            prc4->col = col;
            val = pb->line[row].val[col];
            if (val) continue;
            ps2 = &pb->line[row].set[col];
			scnt2 = Get_Set_Cnt2(ps2,comm2);
			if (scnt2 > 3) continue; // TOO many candidates
            COPY_SET(&prc4->set,ps2);
            // check if ALL in SET1 are pesent in SET2
            if (!Set1_shared_with_Set2(&prc->set,ps2)) continue; // no 2 shared cands
            // Yowee, got RECTANGLE
			// Any MORE restrictions??? Like if 1 is 3, the other MUST be 2
            // Type 1 will be tested later
			// if ( !(((scnt == 2) && (scnt2 == 3))||((scnt == 3)&&(scnt2 == 2))) ) continue;
            //if (GETBOX(prc3->row,prc3->col) != GETBOX(prc4->row,prc4->col)) continue; // NOT in SAME BOX
            bret = true;
            break;
        }
    } else {
        // else this would be a BAD call to this service
        sprtf("WARNING: BAD USE Find_uniq_Rect() %s %s do NOT share ROW or COL!\n",
            Get_RC_RCB_Stg(prc), Get_RC_RCB_Stg(prc2));
    }
    return bret;
}

// Given a ROW, count the number of setval in that ROW
int Shares_in_Row( PABOX2 pb, int mrow, int setval, vRC &cells )
{
    int count = 0;
    int col, val;
    PSET ps;
    ROWCOL rc;
    // march across ROW
    for (col = 0; col < 9; col++) {
        val = pb->line[mrow].val[col];
        if (val) continue;
        ps = &pb->line[mrow].set[col];
        if (!Value_in_SET(setval,ps)) continue;
        rc.row = mrow;
        rc.col = col;
        ZERO_SET(&rc.set);
        rc.set.val[setval - 1] = setval;
        cells.push_back(rc);    // save the CELLS with this setval
        count++;
    }
    return count;
}

int Shares_in_Col( PABOX2 pb, int mcol, int setval, vRC &cells )
{
    int count = 0;
    int row, val;
    PSET ps;
    ROWCOL rc;
    // march across ROW
    for (row = 0; row < 9; row++) {
        val = pb->line[row].val[mcol];
        if (val) continue;
        ps = &pb->line[row].set[mcol];
        if (!Value_in_SET(setval,ps)) continue;
        rc.row = row;
        rc.col = mcol;
        ZERO_SET(&rc.set);
        rc.set.val[setval - 1] = setval;
        cells.push_back(rc);    // save the CELLS with this setval
        count++;
    }
    return count;
}


int Get_Strong_Links_for_Pair( PABOX2 pb, char *tb, PROWCOL prc1, PROWCOL prc2, vRC *psl_pairs )
{

    int count = 0;
    if (SAMERCCELL(prc1,prc2)) return 0;
    SET set;
    PSET comm = &set;
    ZERO_SET(comm);
    PSET ps1 = &prc1->set;
    PSET ps2 = &prc2->set;
    int scnt = Get_Shared_SET( ps1, ps2, comm );
    if (!scnt) return 0;
    int i, setval;
    vRC cells;
    for (i = 0; i < 9; i++) {
        setval = comm->val[i];
        if (!setval) continue;
        // is there a strong link on this value
        if ((prc1->row == prc2->row)&&(prc1->col == prc2->col)) {
            sprtf("UGH: Bad Mark_Strong_Links_for_Pair - SAME CELL %s\n", Get_RC_RCB_Stg(prc1));
        } else if (prc1->row == prc2->row) {
            cells.clear();
            if (Shares_in_Row( pb, prc1->row, setval, cells ) == 2) {
                // since this PAIR share this common value, then this is a strong LINK
                prc1->set.flag[setval - 1] |= cl_SLR; // mark strong link in Row
                prc2->set.flag[setval - 1] |= cl_SLR; // mark strong link in Row
                psl_pairs->push_back(*prc1);
                psl_pairs->push_back(*prc2);
                count++;
            }
        } else if (prc1->col == prc2->col) {
            cells.clear();
            if (Shares_in_Col( pb, prc1->col, setval, cells ) == 2) {
                // since this PAIR share this common value, then this is a strong LINK
                prc1->set.flag[setval - 1] |= cl_SLC; // mark strong link in Col
                prc2->set.flag[setval - 1] |= cl_SLC; // mark strong link in Col
                psl_pairs->push_back(*prc1);
                psl_pairs->push_back(*prc2);
                count++;
            }
        } else {
            sprtf("UGH: Bad Mark_Strong_Links_for_Pair - NO RC SHARED CELL %s %s\n",
                Get_RC_RCB_Stg(prc1), Get_RC_RCB_Stg(prc2));
        }
    }
    return count;
}


int Get_Strong_Links_for_Rect( PABOX2 pb, char *tb, PROWCOL prc, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4,
                                vRC *psl_pairs )
{
    int count = 0;
    if ( !SAMERCCELL(prc,prc2) && ((prc->row == prc2->row) || (prc->col == prc2->col)))
        count += Get_Strong_Links_for_Pair( pb, tb, prc, prc2, psl_pairs );

    if ( !SAMERCCELL(prc,prc3) && ((prc->row == prc3->row) || (prc->col == prc3->col)))
        count += Get_Strong_Links_for_Pair( pb, tb, prc, prc3, psl_pairs );

    if ( !SAMERCCELL(prc,prc4) && ((prc->row == prc4->row) || (prc->col == prc4->col)))
        count += Get_Strong_Links_for_Pair( pb, tb, prc, prc4, psl_pairs );

    if ( !SAMERCCELL(prc2,prc3) && ((prc2->row == prc3->row) || (prc2->col == prc3->col)))
        count += Get_Strong_Links_for_Pair( pb, tb, prc2, prc3, psl_pairs );

    if ( !SAMERCCELL(prc2,prc4) && ((prc2->row == prc4->row) || (prc2->col == prc4->col)))
        count += Get_Strong_Links_for_Pair( pb, tb, prc2, prc4, psl_pairs );

    if ( !SAMERCCELL(prc3,prc4) && ((prc3->row == prc4->row) || (prc3->col == prc4->col)))
        count += Get_Strong_Links_for_Pair( pb, tb, prc3, prc4, psl_pairs );

    return count;
}

int Set_Cell_Flag( PABOX2 pb, int row, int col, uint64_t flag )
{
    int count = 0;
    if ( !(pb->line[row].set[col].cellflg & flag) ) {
        pb->line[row].set[col].cellflg |= flag;
        count++;
    }
    return count;
}

#define ur_elim cf_URE

void Show_UR_Elims( PABOX2 pb, vRC *pelims, vRC *pelimby, vSTG *pvstg )
{
    size_t max = pelims->size();
    if (!max)
        return; // nothing to show
    size_t max4 = pelimby->size();
    size_t ii, i4;
    PROWCOL prc;
    int i, val, cnt;
    char *tb = GetNxtBuf();

    for (ii = 0; ii < max; ii++) {
        prc = &pelims->at(ii);
        for (i = 0; i < 9; i++) {
            val = prc->set.val[i];
            if (!val) continue;
            if ( !(prc->set.flag[i] & ur_elim) ) continue;
            sprintf(tb,"UR-Elim: %s by ", Get_RC_setval_RC_Stg(prc, val));
            i4 = ii * 4;
            cnt = 0;
            for (; i4 < max4; i4++) {
                prc = &pelimby->at(i4);
                sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc));
                cnt++;
                if (cnt >= 4)
                    break;
            }
            if (ii < pvstg->size())
                strcat(tb,pvstg->at(ii).c_str());
            OUTIT(tb);
        }
    }
}


int Do_Fill_UniqRect( PABOX2 pb )
{
    int count = Do_Fill_By_Flags( pb, cf_URE, cf_UR, "UR", "Uniq.Rect", cl_UNR );
    Set_Rect_Link_Cnt(0);   // stop showing unique rectange links
    return count;
}

// FIX20120930 - EEK, found a case of a MISTAKE ;=(( Running diabolical4.txt
int Do_UniqRect_Scan( PABOX2 pb )
{
    int count = 0;
    int row, col, val, scnt;
    int row2, col2, scnt2;
    int i, cand1, cand2;
    PSET ps, ps2;
    int cands[9];
    int cands2[9];
    ROWCOL rc, rc2, rc3, rc4, rce;
    vRC vdone, sl_pairs;
    vRC *pslv = Get_Rect_Links();
    vRC elims, elimby;
    vSTG vstg;
    string s;
    bool done = false;
    char *tb = GetNxtBuf();
    int sl_cnt = 0;
    int cnt1, cnt2;
    PSET pse;
    if (!pb) pb = get_curr_box();
    if (add_debug_ur) sprtf("Doing UniqRect Scan...\n");
    Set_Rect_Link_Cnt(0);
    pslv->clear();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            scnt = Get_Set_Cnt2(ps, cands);
            if (scnt != 2) continue;
            // got our first with TWO values
            rc.row = row;
            rc.col = col;
            if (Row_Col_in_RC_Vector(vdone,row,col)) continue;
            COPY_SET(&rc.set,ps);
            // for each PRIMARY PAIR, only find ONE match
            done = false;
            for (row2 = 0; row2 < 9; row2++) {
                for (col2 = 0; col2 < 9; col2++) {
                    // DBGSTOP2(row,4,col,5,row2,8,col2,5);
                    if ((row2 == row) && (col2 == col)) continue; // skip SELF
                    if ( !((row2 == row) || (col2 == col)) ) continue; // does NOT share ROW or COL
                    val = pb->line[row2].val[col2];
                    if (val) continue;
                    ps2 = &pb->line[row2].set[col2];
                    scnt2 = Get_Set_Cnt2(ps2, cands2);
                    if (scnt2 != 2) continue;
                    // ok have TWO cells with 2 candidates, in SAME Row or Col!
                    for (i = 0; i < scnt2; i++) {
                        if (cands[i] != cands2[i])
                            break;
                    }
                    if (i < scnt2) continue; // oops, NOT same candidates
                    // have 2 cells, with 2 of the SAME candidates
                    // time to try to complete rectangle
                    rc2.row = row2;
                    rc2.col = col2;
                    if (Row_Col_in_RC_Vector(vdone,row2,col2)) continue;
                    // FINALLY
                    cand1 = cands[0]; // get the SHARED candidates
                    cand2 = cands[1];
                    COPY_SET(&rc2.set,ps2);
                    sprintf(tb,"Unique.rect %s 1-%s 2-%s ",
                        ((rc.row == rc2.row) ? "SR" : (rc.col == rc2.col) ? "SC" : "??"),
                        Get_RC_RCBS_Stg(&rc), Get_RC_RCBS_Stg(&rc2));
                    if (Find_Uniq_Rect( pb, &rc, &rc2, &rc3, &rc4, vdone )) {
                        if (GETBOX(rc.row,rc.col) != GETBOX(rc2.row,rc2.col)) // SAME BOX?
                            strcat(tb,"SB ");
                        sprintf(EndBuf(tb),"+ %s 3-%s 4-%s ",
                            ((rc3.row == rc4.row) ? "SR" : (rc3.col == rc4.col) ? "SC" : "??"),
                            Get_RC_RCBS_Stg(&rc3), Get_RC_RCBS_Stg(&rc4));
                        if (GETBOX(rc3.row,rc3.col) != GETBOX(rc4.row,rc4.col)) // NOT SAME BOX
                            strcat(tb,"SB ");
                        // Type 4 - Check for STRONG LINKS - That is any PAIR have just 2 candidates equal,
                        // AND they are the ONLY in their ROW, COL, or BOX that have these 2 candidates
                        // ============================================================================
                        // this does nothing, and is not really helpful
                        // Add_Strong_Links_for_Pairs( pb, tb, &rc, &rc2, &rc3, &rc4, sl_pairs );
                        OUTITUR(tb);
                        sl_cnt += Get_Strong_Links_for_Rect( pb, tb, &rc, &rc2, &rc3, &rc4, &sl_pairs );
                        // the rc.set.flag[setval - 1] set with cl_SLR[ow] and/or cl_SLC[ol]... if needed
                        // Decide TYPE
                        // Is it type 2 - occurs when 2 cells of a rectangle contain only a pair of possibilities
                        // as do rc and rc2, a and b and the other two cells, rc3, and rc4, C and D contain 
                        // a and b plus one additional possibility c. 
                        // In that case, one of C and D must contain c, so c can be eliminated from any 
                        // cells sharing a group with C and D.
                        // see examples\uniqrect2.txt
                        // This puzzle contains a type-2 unique rectangle at A3, C3, A5 and C5 on values 
                        // 5 and 7. Value 6 must be assigned to one of A5 and C5 or a non-unique rectangle 
                        // will be created. This eliminates 6 from A6 and J5.
                        int cands3[9];
                        int cands4[9];
                        cands3[2] = -1;
                        cands4[2] = -2;
                        for (i = 0; i < 9; i++) {
                            cands3[i] = -1;
                            cands4[i] = -2;
                        }

                        int scnt3 = Get_Set_Cnt2( &rc3.set, cands3 );
                        int scnt4 = Get_Set_Cnt2( &rc4.set, cands4 );
                        int setval = -1; // if any same ROW, COL or BOX contains this
                        i = -1;
                        cnt1 = 0;
                        if ((scnt3 == 3) && (scnt4 == 3)) {
                            for (i = 0; i < scnt3; i++) {
                                if (cands3[i] != cands4[i])
                                    break;
                                if (cands3[i] == cand1) continue;
                                if (cands3[i] == cand2) continue;
                                setval = cands3[i];
                            }
                        }
                        if ((scnt3 == 3) && (scnt4 == 3) && (i == 3)) {
                            // it can be eliminated
                            int row3, col3;
                            PSET ps3;
                            if (rc3.row == rc4.row) {
                                // they share a ROW - eliminate setval from this COL, except rc3, rc4
                                row3 = rc3.row;
                                for (col3 = 0; col3 < 9; col3++) {
                                    if (col3 == rc3.col) continue; // skip cell 3
                                    if (col3 == rc4.col) continue; // skip cell 4
                                    val = pb->line[row3].val[col3];
                                    if (val) continue; // skip - has a value
                                    ps3 = &pb->line[row3].set[col3];
                                    if (! ps3->val[setval -1] ) continue; // does NOT have the candidate
                                    ps3->flag[setval - 1] |= cf_URE; // elimination
                                    cnt1++;
                                    s = "SR-elim-COL";
                                    rce.row = row3;
                                    rce.col = col3;
                                    COPY_SET(&rce.set,ps3);
                                    if (!prc_in_chain(&rce,&elims)) {
                                        elims.push_back(rce);
                                        vstg.push_back(s);
                                        elimby.push_back(rc);
                                        elimby.push_back(rc2);
                                        elimby.push_back(rc3);
                                        elimby.push_back(rc4);
                                    }
                                }
                            } else if (rc3.col == rc4.col) {
                                // they share a COL - eliminate setval from this ROW, except rc3, rc4
                                col3 = rc3.col;
                                for (row3 = 0; row3 < 9; row3++) {
                                    if (row3 == rc3.row) continue; // skip cell 3
                                    if (row3 == rc4.row) continue; // skip cell 4
                                    val = pb->line[row3].val[col3];
                                    if (val) continue; // skip - has a value
                                    ps3 = &pb->line[row3].set[col3];
                                    if (! ps3->val[setval -1] ) continue; // does NOT have the candidate
                                    ps3->flag[setval - 1] |= cf_URE; // elimination
                                    cnt1++;
                                    s = "SC-elim-ROW";
                                    rce.row = row3;
                                    rce.col = col3;
                                    COPY_SET(&rce.set,ps3);
                                    if (!prc_in_chain(&rce,&elims)) {
                                        elims.push_back(rce);
                                        vstg.push_back(s);
                                        elimby.push_back(rc);
                                        elimby.push_back(rc2);
                                        elimby.push_back(rc3);
                                        elimby.push_back(rc4);
                                    }
                                }
                            }
                            if (GETBOX(rc3.row,rc3.col) == GETBOX(rc4.row,rc4.col)) {
                                // they share a BOX - eliminate setval from this BOX, except rc3, rc4
                                int r = (rc3.row / 3) * 3;
                                int c = (rc3.col / 3) * 3;
                                int rw, cl;
                                for (rw = 0; rw < 3; rw++) {
                                    for (cl = 0; cl < 3; cl++) {
                                        row3 = r + rw;
                                        col3 = c + cl;
                                        if ((row3 == rc.row)&&(col3 == rc.col)) continue; // skip cell 1
                                        if ((row3 == rc2.row)&&(col3 == rc2.col)) continue; // skip cell 2
                                        if ((row3 == rc3.row)&&(col3 == rc3.col)) continue; // skip cell 3
                                        if ((row3 == rc4.row)&&(col3 == rc4.col)) continue; // skip cell 4
                                        val = pb->line[row3].val[col3];
                                        if (val) continue; // skip - has a value
                                        ps3 = &pb->line[row3].set[col3];
                                        if (! ps3->val[setval -1] ) continue; // does NOT have the candidate
                                        ps3->flag[setval - 1] |= cf_URE; // elimination
                                        cnt1++;
                                        s = "SB-elim-Box";
                                        rce.row = row3;
                                        rce.col = col3;
                                        COPY_SET(&rce.set,ps3);
                                        if (!prc_in_chain(&rce,&elims)) {
                                            elims.push_back(rce);
                                            vstg.push_back(s);
                                            elimby.push_back(rc);
                                            elimby.push_back(rc2);
                                            elimby.push_back(rc3);
                                            elimby.push_back(rc4);
                                        }
                                    }
                                }
                            }
                            if (cnt1) {
                                // mark the candidates that caused this elimination
                                pb->line[rc.row].set[rc.col].flag[cand1 - 1] |= cf_UR; // cause
                                pb->line[rc.row].set[rc.col].flag[cand2 - 1] |= cf_UR; // cause

                                pb->line[rc2.row].set[rc2.col].flag[cand1 - 1] |= cf_UR; // cause
                                pb->line[rc2.row].set[rc2.col].flag[cand2 - 1] |= cf_UR; // cause

                                pb->line[rc3.row].set[rc3.col].flag[cand1 - 1] |= cf_UR; // cause
                                pb->line[rc3.row].set[rc3.col].flag[cand2 - 1] |= cf_UR; // cause

                                pb->line[rc4.row].set[rc4.col].flag[cand1 - 1] |= cf_UR; // cause
                                pb->line[rc4.row].set[rc4.col].flag[cand2 - 1] |= cf_UR; // cause
                            }
                            count += cnt1;
                        }

                        // Is it type 4 - ie the other pair who share a pair, plus other cands, 
                        // also have a strong link on one of the two candidates - this would mean 
                        // this value MUST be used in the ROW or COL, thus the 2nd value can be 
                        // eliminated.
                        // It has been set such that rc and rc2 are the pair that share exactly a pair
                        // so the elimination, if any, would be in rc3 and rc4
                        cnt2 = 0;
                        if (rc3.row == rc4.row) {
                            // they share a ROW
                            if (( rc3.set.flag[cand1 - 1] & cl_SLR ) && ( rc4.set.flag[cand1 - 1] & cl_SLR ) &&
                                !( rc3.set.flag[cand2 - 1] & cl_SLR ) && !( rc4.set.flag[cand2 - 1] & cl_SLR )) {
                                // candidate 2 can be elimated from 3 and 4
                                pb->line[rc3.row].set[rc3.col].flag[cand1 - 1] |= cf_UR; // caused elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand1 - 1] |= cf_UR; // caused elimination
                                pb->line[rc3.row].set[rc3.col].flag[cand2 - 1] |= cf_URE; // elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand2 - 1] |= cf_URE; // elimination
                                cnt2++;
                                s = "SR-Type-4";
                                rce.row = rc3.row;
                                rce.col = rc3.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                                rce.row = rc4.row;
                                rce.col = rc4.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                            } else if (!( rc3.set.flag[cand1 - 1] & cl_SLR ) && !( rc4.set.flag[cand1 - 1] & cl_SLR ) &&
                                        ( rc3.set.flag[cand2 - 1] & cl_SLR ) && ( rc4.set.flag[cand2 - 1] & cl_SLR )) {
                                // candidate 1 can be elimated from 3 and 4
                                pb->line[rc3.row].set[rc3.col].flag[cand2 - 1] |= cf_UR; // caused elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand2 - 1] |= cf_UR; // caused elimination
                                pb->line[rc3.row].set[rc3.col].flag[cand1 - 1] |= cf_URE; // elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand1 - 1] |= cf_URE; // elimination
                                cnt2++;
                                s = "NSR-Type-4";
                                rce.row = rc3.row;
                                rce.col = rc3.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                                rce.row = rc4.row;
                                rce.col = rc4.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                            }
                        } else if (rc3.col == rc4.col) {
                            // they share a COL
                            if (( rc3.set.flag[cand1 - 1] & cl_SLC ) && ( rc4.set.flag[cand1 - 1] & cl_SLC ) &&
                                !( rc3.set.flag[cand2 - 1] & cl_SLC ) && !( rc4.set.flag[cand2 - 1] & cl_SLC )) {
                                // candidate 2 can be elimated from 3 and 4
                                pb->line[rc3.row].set[rc3.col].flag[cand1 - 1] |= cf_UR; // caused elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand1 - 1] |= cf_UR; // caused elimination
                                pb->line[rc3.row].set[rc3.col].flag[cand2 - 1] |= cf_URE; // elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand2 - 1] |= cf_URE; // elimination
                                cnt2++;
                                s = "SC-Type-4";
                                rce.row = rc3.row;
                                rce.col = rc3.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                                rce.row = rc4.row;
                                rce.col = rc4.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                            } else if (!( rc3.set.flag[cand1 - 1] & cl_SLC ) && !( rc4.set.flag[cand1 - 1] & cl_SLC ) &&
                                        ( rc3.set.flag[cand2 - 1] & cl_SLC ) && ( rc4.set.flag[cand2 - 1] & cl_SLC )) {
                                // candidate 1 can be elimated from 3 and 4
                                pb->line[rc3.row].set[rc3.col].flag[cand2 - 1] |= cf_UR; // caused elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand2 - 1] |= cf_UR; // caused elimination
                                pb->line[rc3.row].set[rc3.col].flag[cand1 - 1] |= cf_URE; // elimination
                                pb->line[rc4.row].set[rc4.col].flag[cand1 - 1] |= cf_URE; // elimination
                                cnt2++;
                                s = "NSC-Type-4";
                                rce.row = rc3.row;
                                rce.col = rc3.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                                rce.row = rc4.row;
                                rce.col = rc4.col;
                                pse = &pb->line[rce.row].set[rce.col];
                                COPY_SET(&rce.set,pse);
                                if (!prc_in_chain(&rce,&elims)) {
                                    elims.push_back(rce);
                                    vstg.push_back(s);
                                    elimby.push_back(rc);
                                    elimby.push_back(rc2);
                                    elimby.push_back(rc3);
                                    elimby.push_back(rc4);
                                }
                            }
                        }
                        if (cnt2 || add_debug_ur) {
                            row = rc.row;
                            col = rc.col;
                            Set_Cell_Flag( pb, row, col, cl_UNR );
                            row = rc2.row;
                            col = rc2.col;
                            Set_Cell_Flag( pb, row, col, cl_UNR );
                            row = rc3.row;
                            col = rc3.col;
                            Set_Cell_Flag( pb, row, col, cl_UNR );
                            row = rc4.row;
                            col = rc4.col;
                            Set_Cell_Flag( pb, row, col, cl_UNR );
                        }
                        count += cnt2;
                        vdone.push_back(rc);
                        vdone.push_back(rc2);
                        vdone.push_back(rc3);
                        vdone.push_back(rc4);
                        //if (only_some_unr && (count >= only_some_unr))
                        done = true;
                        break;
                    } else {
                        vdone.push_back(rc);
                        vdone.push_back(rc2);
                        strcat(tb,"Failed to find matching pair!");
                        OUTITUR(tb);
                    }
                }
                if (done)
                    break;  // back to find primary pair
                if (only_some_unr && (count >= only_some_unr))
                   break;
            }
            if (only_some_unr && (count >= only_some_unr))
               break;
        }
        if (only_some_unr && (count >= only_some_unr))
           break;
    }
    if (count || add_debug_ur) {
        Set_Rect_Link_Cnt(sl_cnt);
        for (size_t j = 0; j < sl_pairs.size(); j++) {
            rc = sl_pairs[j];
            pslv->push_back(rc);
        }
    }
    Show_UR_Elims( pb, &elims, &elimby, &vstg );
    sl_pairs.clear();
    if (count) {
        sprtf("S2f: Uniq.Rect, marked %d. To %d\n", count, sg_Fill_Rect);
        pb->iStage = sg_Fill_Rect;
    } else {
        // since have MARKED the rectangles for painting, still stack the Undo
        Stack_Fill( Do_Fill_UniqRect );
        sprtf("S2f: Uniq.Rect scan. No marks. To begin.\n");
        pb->iStage = sg_Begin;
    }
    return count;
}

// eof - Sudo_URect.cxx
