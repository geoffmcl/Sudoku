// Sudo_Locked.cxx

#include "Sudoku.hxx"

#define le_elim cf_LKE
#define le_mark cf_LK
#define le_cell cl_MCC
#define le_on   0
#define le_off  1

int add_debug_ape = 1;
#define OUTITLK(tb) if (*tb && add_debug_lk) OUTIT(tb);
#define OUTITAPE(tb) if (*tb && add_debug_ape) OUTIT(tb);

int only_one_lk = 1;
int test_aligned_pair = 0;  // just for testing ONE particular BOX, like 6 in examples\locked-05.txt

// Locked Exclusion
// ================
// Perhaps an acronym for Aligned Pair Exclusion
// or perhaps // 1.3 - Block and Column/Row Interactions

// from : http://sudoku.com.au/sudokutips.aspx?Go=B1-6-1992
// If a candidate is already excluded from two of three boxes in any one row, 
// then that candidate is locked into that row within the remaining box. 
// Therefor, the candidate can be safely excluded from the other rows within that box.
// If a candidate is already excluded from two of three boxes in any one column, 
// then that candidate is locked into that column within the remaining box. Therefor, 
// the candidate can be safely excluded from the other columns within that box.
// If a candidate exists only in one row within a box, then it is locked into that box 
// with respect to that row. Therefor, it can safely be excluded from the locations 
// in that row outside of the box.
// If a candidate exists only in one column within a box, then it is locked into that 
// box with respect to that column. Therefor, it can safely be excluded from the locations 
// in that column outside of the box.

// from : http://www.sudokuessentials.com/sudoku-strategy.html
// See : examples\locked-01-2.txt
// But this is also done in Naked_Triple scan

// from : http://sudoku.ironmonger.com/howto/alignedPairExclusion/docs.tpl?setreferrertoself=true
// Aligned Pair Exclusion

typedef struct tagAPESTR {
    PABOX2 pb;
    vRC *pelims;
    vRC *pelimby;
    PRCRCB prcrcbep;
    PRCRCB prcrcbeo;
    vRC *pvrcep;
    vRC *pvrceo;
}APESTR, *PAPESTR;



typedef struct tagCANDCOMB {
    int val1;
    int val2;
    int flag;
}CANDCOMB, *PCANDCOMB;

static int cand_comb_cnt;
static CANDCOMB candcomb[82];

#define GRCS Get_RC_setval_RC_Stg

static int Check_for_APE_Elims( PAPESTR pape, PROWCOL prc1, PROWCOL prc2 )
{
    int count = 0;
    int setvals1[9];
    int setvals2[9];
    int cnt1, cnt2, off, i, j;
    PSET ps1, ps2;
    int val1, val2, cnt;
    PCANDCOMB pcc;
    SET comm;
    char *tb = GetNxtBuf();

    if (prc1->box != prc2->box) {
        sprintf(tb,"UGH! Call to set_Cand_Comb do NOT have SAME BOX! %s %s ", GRCS(prc1), GRCS(prc2));
        OUTIT(tb);
        return 0;
    }
    ps1 = &prc1->set;
    ps2 = &prc2->set;
    ZERO_SET(&comm);
    cnt = Get_Shared_SET( ps1, ps2, &comm ); // get the COMMON set
    if (cnt == 0) return 0; // NO common candidate to check

    cnt1 = Get_Set_Cnt2( ps1, setvals1 );
    cnt2 = Get_Set_Cnt2( ps2, setvals2 );
    off = 0;
    sprintf(tb,"All combs: ");
    for (i = 0; i < cnt1; i++) {
        val1 = setvals1[i];
        for (j = 0; j < cnt2; j++) {
            val2 = setvals2[j];
            if (val1 == val2) continue; // Rule 1 - can NOT have same value in unit - in this case SAME BOX
            pcc = &candcomb[off++];
            pcc->flag = 0;  // clear the FLAG
            pcc->val1 = val1;
            pcc->val2 = val2;
            sprintf(EndBuf(tb),"%d-%d ", val1, val2 );
        }
    }
    OUTITAPE(tb);

    // Ok got LIST of POSSIBLE combinations of candiddates
    cand_comb_cnt = off;
    sprintf(tb,"Set up %d candidate combination for %s and %s ", off, GRCS(prc1), GRCS(prc2));
    OUTITAPE(tb);

    PABOX2 pb       = pape->pb;
    vRC *pelims     = pape->pelims;
    vRC *pelimby    = pape->pelimby;
    PRCRCB prcrcbep = pape->prcrcbep;
    PRCRCB prcrcbeo = pape->prcrcbeo;
    vRC *pvrcep     = pape->pvrcep;
    vRC *pvrceo     = pape->pvrceo;

    vRC *pvrc = &prcrcbep->vboxs[prc1->box];    // get set of cell with just PAIRS in this BOX
    size_t max = pvrc->size();
    size_t ii;
    PROWCOL prc;
    PSET ps;
    SET set, set2;
    int setvals[9];
    ROWCOL rce;
    vRC vrc;
    pcc = &candcomb[0];
    // DO the BOXES
    // ============
    for (ii = 0; ii < max; ii++) {
        prc = &pvrc->at(ii);
        // sprintf(EndBuf(tb),"SB %s ", Get_RC_setval_RC_Stg(prc));
        ps = &prc->set;
        set = *ps;
        // src-unchanged dst-modified - returns ELIMINATED count
        //cnt2 = Elim_SET_in_SET( &comm, &set );
        cnt2 = Elim_SET_in_SET( ps1, &set );
        cnt2 += Elim_SET_in_SET( ps2, &set );
        // we know the set had TWO - it is a Buddy pair
        // sprintf(EndBuf(tb),"sh %d ",cnt2);
        if (cnt2 != 2) continue;
        // get the SET, which we KNOW is 2
        cnt = Get_Set_Cnt2(ps,setvals);
        if (cnt != 2) {
            sprintf(tb,"Goodness! A candidate pair is NOT 2!! It is %d!!!\n",cnt);
            EXPLODE(tb);
            continue; // just a PROTECTION - would be an ERROR if NOT 2!!!
        }
        val1 = setvals[0];
        val2 = setvals[1];
        // Now eliminate combination that match this
        for (i = 0; i < off; i++) {
            if ( (pcc[i].val1 == val1) && (pcc[i].val2 == val2) ) {
                rce.row = prc->row;
                rce.col = prc->col;
                rce.box = GETBOX(rce.row,rce.col);
                COPY_SET(&rce.set,ps);
                rce.cnum = -1;
                if (!prc_in_chain(&rce, &vrc)) 
                    vrc.push_back(rce);
                pcc[i].flag |= 1;
            }
            if ( (pcc[i].val1 == val2) && (pcc[i].val2 == val1) ) {
                rce.row = prc->row;
                rce.col = prc->col;
                rce.box = GETBOX(rce.row,rce.col);
                COPY_SET(&rce.set,ps);
                rce.cnum = -1;
                if (!prc_in_chain(&rce, &vrc)) 
                    vrc.push_back(rce);
                pcc[i].flag |= 2;
            }
        }
    }
    if (prc1->row == prc2->row) {
        // check against cand pairs in this ROW
        pvrc = &prcrcbep->vrows[prc1->row];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc->box == prc1->box) continue; // skip pairs in SAME box - already done above
            //sprintf(EndBuf(tb),"SR %s ", Get_RC_setval_RC_Stg(prcp));
            ps = &prc->set;
            // Will the COMMON kill this pair?
            set = *ps;
            //          src-unchanged dst-modified - returns ELIMINATED count
            //cnt2 = Elim_SET_in_SET( &comm, &set );
            cnt2 = Elim_SET_in_SET( ps1, &set );
            cnt2 += Elim_SET_in_SET( ps2, &set );
            //sprintf(EndBuf(tb),"sh %d ",cnt2);
            // we know the set had TWO - it is a Buddy pair
            if (cnt2 != 2) continue;
            // YES they WERE
            // get the SET, which we KNOW is 2
            cnt = Get_Set_Cnt2(ps,setvals);
            if (cnt != 2) {
                sprintf(tb,"Goodness! A Candidate pair is NOT 2!! It is %d!!!\n",cnt);
                EXPLODE(tb);
                continue; // just a PROTECTION - would be an ERROR if NOT 2!!!
            }
            val1 = setvals[0];
            val2 = setvals[1];
            // Eliminate the cominations that would KILL the 2 values in this PAIR
            for (i = 0; i < off; i++) {
                if ( (pcc[i].val1 == val1) && (pcc[i].val2 == val2) ) {
                    rce.row = prc->row;
                    rce.col = prc->col;
                    rce.box = GETBOX(rce.row,rce.col);
                    COPY_SET(&rce.set,ps);
                    rce.cnum = -1;
                    if (!prc_in_chain(&rce, &vrc)) 
                        vrc.push_back(rce);
                    pcc[i].flag |= 1;
                }
                if ( (pcc[i].val1 == val2) && (pcc[i].val2 == val1) ) {
                    rce.row = prc->row;
                    rce.col = prc->col;
                    rce.box = GETBOX(rce.row,rce.col);
                    COPY_SET(&rce.set,ps);
                    rce.cnum = -1;
                    if (!prc_in_chain(&rce, &vrc)) 
                        vrc.push_back(rce);
                    pcc[i].flag |= 2;
                }
            }
        }
    } else if (prc1->col == prc2->col) {
        // check against cand pairs in this ROW
        pvrc = &prcrcbep->vcols[prc1->col];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc->box == prc1->box) continue; // skip pairs in SAME box - already done above
            //sprintf(EndBuf(tb),"SR %s ", Get_RC_setval_RC_Stg(prcp));
            ps = &prc->set;
            // Will the COMMON kill this pair?
            set = *ps;
            //          src-unchanged dst-modified - returns ELIMINATED count
            //cnt2 = Elim_SET_in_SET( &comm, &set );
            cnt2 = Elim_SET_in_SET( ps1, &set );
            cnt2 += Elim_SET_in_SET( ps2, &set );
            //sprintf(EndBuf(tb),"sh %d ",cnt2);
            // we know the set had TWO - it is a Buddy pair
            if (cnt2 != 2) continue;
            // YES they WERE
            // get the SET, which we KNOW is 2
            cnt = Get_Set_Cnt2(ps,setvals);
            if (cnt != 2) {
                sprintf(tb,"Goodness! A Candidate pair is NOT 2!! It is %d!!!\n",cnt);
                EXPLODE(tb);
                continue; // just a PROTECTION - would be an ERROR if NOT 2!!!
            }
            val1 = setvals[0];
            val2 = setvals[1];
            // Eliminate the cominations that would KILL the 2 values in this PAIR
            for (i = 0; i < off; i++) {
                if ( (pcc[i].val1 == val1) && (pcc[i].val2 == val2) ) {
                    rce.row = prc->row;
                    rce.col = prc->col;
                    rce.box = GETBOX(rce.row,rce.col);
                    COPY_SET(&rce.set,ps);
                    rce.cnum = -1;
                    if (!prc_in_chain(&rce, &vrc)) 
                        vrc.push_back(rce);
                    pcc[i].flag |= 1;
                }
                if ( (pcc[i].val1 == val2) && (pcc[i].val2 == val1) ) {
                    rce.row = prc->row;
                    rce.col = prc->col;
                    rce.box = GETBOX(rce.row,rce.col);
                    COPY_SET(&rce.set,ps);
                    rce.cnum = -1;
                    if (!prc_in_chain(&rce, &vrc)) 
                        vrc.push_back(rce);
                    pcc[i].flag |= 2;
                }
            }
        }
    }
    // all that remains is to check what candidates can NOT exist, and we have an ELIMINATION
    // This is checked against setval1[9] from prc1, and setval2[9] from prc2, through 'comm'
    ZERO_SET(&set);
    ZERO_SET(&set2);
    sprintf(tb,"Valid combs: ");
    for (i = 0; i < off; i++) {
        if (pcc[i].flag) continue;
        val1 = pcc[i].val1;
        val2 = pcc[i].val2;
        sprintf(EndBuf(tb),"%d-%d ", val1, val2);
        // build up a non excluded possible candidate sets
        set.val[val1 - 1] = val1;
        set2.val[val2 - 1] = val2;
    }
    OUTITAPE(tb);
    // compare original SET with valid SET
    sprintf(tb,"APE.Elim: ");
    for (i = 0; i < 9; i++) {
        val1 = ps1->val[i]; // get a SET1 value
        if (!val1) continue; // no candidate
        if (set.val[val1 - 1]) continue; // remains a possibility
        // FOUND a VALUE to ELIMINATE
        sprintf(EndBuf(tb),"%s ", GRCS(prc1,val1));
        pb->line[prc1->row].set[prc1->col].flag[val1-1] |= le_elim;
        rce.row = prc1->row;
        rce.col = prc1->col;
        rce.box = GETBOX(prc1->row,prc1->col);
        COPY_SET(&rce.set,ps1);
        rce.cnum = val1;
        pelims->push_back(rce);
        count++;
    }
    for (i = 0; i < 9; i++) {
        val2 = ps2->val[i]; // get a SET1 value
        if (!val2) continue; // no candidate
        if (set2.val[val2 - 1]) continue; // remains a possibility
        // FOUND a VALUE to ELIMINATE
        sprintf(EndBuf(tb),"%s ", GRCS(prc2,val2));
        pb->line[prc2->row].set[prc2->col].flag[val2-1] |= le_elim;
        rce.row = prc2->row;
        rce.col = prc2->col;
        rce.box = GETBOX(prc2->row,prc2->col);
        COPY_SET(&rce.set,ps2);
        rce.cnum = val2;
        pelims->push_back(rce);
        count++;
    }
    if (count) {
        for (ii = 0; ii < vrc.size(); ii++) {
            prc = &vrc[ii];
            pelimby->push_back(*prc);
            ps = &prc->set;
            cnt = Get_Set_Cnt2(ps,setvals);
            for (i = 0; i < cnt; i++) {
                val1 = setvals[i];
                pb->line[prc->row].set[prc->col].flag[val1-1] |= le_mark;
                pb->line[prc->row].set[prc->col].cellflg |= le_cell;
                pb->line[prc->row].set[prc->col].uval = le_off;
            }
        }
        pb->line[prc1->row].set[prc1->col].cellflg |= le_cell;
        pb->line[prc1->row].set[prc1->col].uval = le_on;
        pb->line[prc2->row].set[prc2->col].cellflg |= le_cell;
        pb->line[prc2->row].set[prc2->col].uval = le_on;
    } else
        strcat(tb,"None");
    OUTITAPE(tb);
    return count;
}

static APESTR _s_apestr;
int Do_Aligned_Pair_Excl2( PABOX2 pb, vRC *pelims, vRC *pelimby )
{
    int count = 0;
    vRC epairs, eothers;
    RCRCB rcrcb, rcrcb2;
    PRCRCB prcrcb = &rcrcb;
    PRCRCB prcrcb2 = &rcrcb2;
    vRC *pvrcep = &epairs;
    vRC *pvrceo = &eothers;
    vRC *prow, *pcol, *pbox;
    vRC *pbox2;
    SET set;
    int i;
    size_t maxb, maxr, maxc;
    size_t maxb2;
    size_t ii, ii2, ii3;
    PROWCOL prc1, prc2, prcp;
    PSET ps1, ps2, psp;
    SET comm, comm2, comm3;
    int setvals1[9];
    int setvals2[9];
    int cnt = Get_Empty_Cells_per_cand_count( pb, pvrcep, 2, prcrcb );
    if (!cnt) return 0;
    int cnt2 = Get_Empty_Cells_per_not_cand_count( pb, pvrceo, 2, prcrcb2 );
    if (!cnt2) return 0;
    char *tb = GetNxtBuf();
    PAPESTR pape = &_s_apestr;


    pape->pb       = pb;
    pape->pelims   = pelims;
    pape->pelimby  = pelimby;
    pape->prcrcbep = prcrcb;
    pape->prcrcbeo = prcrcb2;
    pape->pvrcep   = pvrcep;
    pape->pvrceo   = pvrceo;

    // work box by box
    for (i = 0; i < 9; i++) {
        if (test_aligned_pair && ((i + 1) != test_aligned_pair)) continue; // DIAG: Get to TEST one particular BOX
        pbox = &prcrcb->vboxs[i];
        pbox2 = &prcrcb2->vboxs[i];
        maxb = pbox->size();    // empty pairs in this BOX
        maxb2 = pbox2->size();  // empty others in this BOX
        if (maxb2 < 2) continue;    // need min of 2 others
        // take as pairs
        sprintf(tb,"Box %d: pairs %d, others %d ", (i + 1), (int)maxb, (int)maxb2);
        OUTITAPE(tb);
        for (ii2 = 0; ii2 < maxb2; ii2++) {
            prc1 = &pbox2->at(ii2); // check this CELL, with EACH following
            for (ii3 = ii2 + 1; ii3 < maxb2; ii3++) {
                prc2 = &pbox2->at(ii3);
                sprintf(tb,"Cells %s %s ", Get_RC_setval_RC_Stg(prc1), Get_RC_setval_RC_Stg(prc2));
                ps1 = &prc1->set;
                ps2 = &prc2->set;
                ZERO_SET(&comm);
                cnt = Get_Shared_SET( ps1, ps2, &comm ); // get the COMMON set
                if (cnt == 0) continue; // NO common candidate to check
                sprintf(EndBuf(tb),"Share [%d] ", cnt);
                AddSet2Buf(tb,&comm);
                ADDSP(tb);
                // will this common set KILL any PAIRS in the same BOX, ROW or COL
                // check pairs in BOX
                ZERO_SET(&comm2); // empty this set
                ZERO_SET(&comm3); // empty this set
                for (ii = 0; ii < maxb; ii++) {
                    prcp = &pbox->at(ii);
                    // sprintf(EndBuf(tb),"SB %s ", Get_RC_setval_RC_Stg(prcp));
                    psp = &prcp->set;
                    // Will the COMMON kill this pair?
                    set = *psp;
                    //          src-unchanged dst-modified - returns ELIMINATED count
                    cnt2 = Elim_SET_in_SET( &comm, &set );
                    // we know the set had TWO - it is a Buddy pair
                    // sprintf(EndBuf(tb),"sh %d ",cnt2);
                    if (cnt2 != 2) continue;
                    // YES they WERE
                    sprintf(EndBuf(tb),"SB %s ", Get_RC_setval_RC_Stg(prcp));
                    sprintf(EndBuf(tb),"sh %d ",cnt2);
                    Add_SET_to_SET( psp, &comm2 ); // This COMBINATION from this Aligned Pair would KILL a pair
                }
                // Now if this Aligned Pair share a ROW or COL
                if (prc1->row == prc2->row) {
                    // check against cand pairs in this ROW
                    prow = &prcrcb->vrows[prc1->row];
                    maxr = prow->size();
                    for (ii = 0; ii < maxr; ii++) {
                        prcp = &prow->at(ii);
                        if (prcp->box == prc1->box) continue; // skip pairs in SAME box - already done above
                        //sprintf(EndBuf(tb),"SR %s ", Get_RC_setval_RC_Stg(prcp));
                        psp = &prcp->set;
                        // Will the COMMON kill this pair?
                        set = *psp;
                        //          src-unchanged dst-modified - retruns ELIMINATED count
                        cnt2 = Elim_SET_in_SET( &comm, &set );
                        //sprintf(EndBuf(tb),"sh %d ",cnt2);
                        // we know the set had TWO - it is a Buddy pair
                        if (cnt2 != 2) continue;
                        // YES they WERE
                         Add_SET_to_SET( psp, &comm3 ); // This COMBINATION from this Aligned Pair would KILL a pair
                        sprintf(EndBuf(tb),"SR %s ", Get_RC_setval_RC_Stg(prcp));
                        sprintf(EndBuf(tb),"sh %d ",cnt2);
                    }
                } else if (prc1->col == prc2->col) {
                    // check against cand pairs in this COL
                    pcol = &prcrcb->vcols[prc1->col];
                    maxc = pcol->size();
                    for (ii = 0; ii < maxc; ii++) {
                        prcp = &pcol->at(ii);
                        if (prcp->box == prc1->box) continue; // skip pairs in SAME box - already done above
                        // sprintf(EndBuf(tb),"SC %s ", Get_RC_setval_RC_Stg(prcp));
                        psp = &prcp->set;
                        // Will the COMMON kill this pair?
                        set = *psp;
                        //          src-unchanged dst-modified - retruns ELIMINATED count
                        cnt2 = Elim_SET_in_SET( &comm, &set );
                        // sprintf(EndBuf(tb),"sh %d ",cnt2);
                        // we know the set had TWO - it is a Buddy pair
                        if (cnt2 != 2) continue;
                        // YES they WERE
                        Add_SET_to_SET( psp, &comm3 ); // This COMBINATION from this Aligned Pair would KILL a pair
                        sprintf(EndBuf(tb),"SC %s ", Get_RC_setval_RC_Stg(prcp));
                        sprintf(EndBuf(tb),"sh %d ",cnt2);
                    }
                }
                cnt = Get_Set_Cnt2( &comm2, setvals1 );
                cnt2 = Get_Set_Cnt2( &comm3, setvals2 );
                sprintf(EndBuf(tb),"Bx %d c[", cnt);
                AddSet2Buf(tb,&comm2);
                sprintf(EndBuf(tb),"] RC %d c[", cnt2);
                AddSet2Buf(tb,&comm3);
                strcat(tb,"]");
                ZERO_SET(&set);
                cnt = Get_Shared_SET( &comm2, &comm3, &set ); // get the COMMON set
                sprintf(EndBuf(tb),"Cm %d ", cnt);
                AddSet2Buf(tb,&set);
                OUTITAPE(tb);
                count += Check_for_APE_Elims( pape, prc1, prc2 );

            }   // for (ii3 = ii2 + 1; ii3 < maxb2; ii3++) // get SECOND ot OTHERS         
        }   // for (ii2 = 0; ii2 < maxb2; ii2++) // get FIST of OTHERS
    }   // for (i = 0; i < 9; i++) // for each BOX
    return count;
}

// **********************************************************************************************
// Locked 2
// Work the ROWS and COLS
// Does any ROW or COL have any particular candidate, that is ONLY in ONE of the 3 possible Boxed
// Other slots in this BOX with this candidate can be eliminated
// Should I work Box by Box - yes I think so...
// int add_debug_lk = 1;

//  MENUITEM "&Locked Exclusion", ID_VIEW_LOCKEDEXCLUSION - Mark_Locked_Excl(get_curr_box());

int Do_Locked_Excl2( PABOX2 pb, vRC *pelims, vRC *pelimby )
{
    int count = 0;
    vRC empty;
    RCRCB rcrcb;
    int cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    int j;
    size_t i, max;
    vRC *pvrc;
    PROWCOL prc;
    int row, col, box, val, scnt;
    int trow, tcol, tbox;
    SET tset, tset2, set3;
    PSET ps, ps2;
    int setvals[9];
    int r, c, rw, cl;
    ROWCOL rce;
	char *tb = GetNxtBuf();
    // work Box by Box
    for (j = 0; j < 9; j++) {
        pvrc = &rcrcb.vboxs[j];   // get rc set for each BOX
        max = pvrc->size();
        if (!max) continue;
        // got a Box with empty slots
        for (i = 0; i < max; i++) {
            prc = &pvrc->at(i); // do EACH for each set for this BOX
            trow = prc->row;
            tcol = prc->col;
            tbox = j;
            ps = &prc->set;
            COPY_SET(&tset,ps);
			sprintf(tb,"Box %d %s COL [%d] elim ", (int)(i + 1), Get_RC_RCBS_Stg(prc), tcol + 1);
            // Now does any candidate(s) for this slot ONLY exist in this ROW in THIS BOX
			// and it MUST exist twice in this BOX
			ZERO_SET(&tset2);
			for (row = 0; row < 9; row++) {
                box = GETBOX(row,tcol);
                val = pb->line[row].val[tcol];
                if (val) continue;
                ps2 = &pb->line[row].set[tcol];
                if (box == tbox) {
					if (row == trow) continue; // same spot
                    // make sure the cand occurs twice or more in COL in BOX
                    Add_SET_to_SET(ps2,&tset2); // add cands to tset2
                    continue; // this is MY BOX
				}
				AddSet2Buf(tb,ps2);
				ADDSP(tb);
                Elim_SET_in_SET( ps2, &tset );
            }
            Elim_SET_not_in_SET( &tset2, &tset );
            // Any candidate remaining?
            scnt = Get_Set_Cnt2(&tset, setvals);
			sprintf(EndBuf(tb),"left [%d] ", scnt);
			AddSet2Buf(tb,&tset);
            cnt = 0;
            if (scnt) { // were ALL cand eliminates in outside box scan of this ROW
				OUTITLK(tb);
                // Yeek we have candidate(s) in this Box that are NOT in the rest of the ROW
                // This/These candidates can be ELIMINATED in other SLOTS in this BOX
                // They are NEEDED to complete this ROW, so MUST be in this ROW
                // BOX ELIMINATION, except for this COL
                r = (trow / 3) * 3;
                c = (tcol / 3) * 3;
                row = r;
                col = c;
                box = GETBOX(row,col);
                sprintf(tb,"Box %d ", box + 1);
                for (rw = 0; rw < 3; rw++) {
                    for (cl = 0; cl < 3; cl++) {
                        row = r + rw;
                        col = c + cl;
                        if (col == tcol) continue;
                        val = pb->line[row].val[col];
                        if (val) continue;
                        ps2 = &pb->line[row].set[col];
                        //                      to   from flag (not all)
                        int tmp = Mark_SET_in_SET( ps2, &tset, le_elim ); // Do_Locked_Excl2()
                        cnt += tmp;
                        if (tmp) {
                            ZERO_SET(&set3);
                            //               from to    on flag
                            Set_SET_per_flag(ps2,  &set3, le_elim);
                            rce.row = row;
                            rce.col = col;
                            rce.box = GETBOX(row,col);
                            COPY_SET(&rce.set,ps2);
                            sprintf(EndBuf(tb),"mk %s ", Get_RC_RCBS_Stg(&rce));
                            Get_Set_Cnt(&set3, &rce.cnum); // TODO - This misses if nore than ONE value elim'ed, but maybe only one always
                            pelims->push_back(rce); // store ELIMINATED candidate
                        }
                    }   // for (cl = 0; cl < 3; cl++)
                }   // for (rw = 0; rw < 3; rw++)
                OUTITLK(tb);
                if (cnt) {
                    ps2 = &pb->line[trow].set[tcol];
                    //              to   from flag (not all)
                    Mark_SET_in_SET( ps2, &tset, le_mark ); // Do_Locked_Excl2()
                    count += cnt;
                    rce.row = trow;
                    rce.col = tcol;
                    rce.box = GETBOX(trow,tcol);
                    COPY_SET(&rce.set,ps2);
                    rce.cnum = -1;
                    pelimby->push_back(rce); // keep eliminated by RC
                }
            }
            if (!cnt) {
                cnt = 0;
				sprintf(tb,"Box %d %s ROW [%d] elim ", (int)(i + 1), Get_RC_RCBS_Stg(prc), trow + 1);
                COPY_SET(&tset,&ps); // get the set of cand back
                // Now does any candidate(s) for this slot ONLY exist in this ROW in THIS BOX
    			// and it MUST exist twice in this BOX
	    		ZERO_SET(&tset2);
                for (col = 0; col < 9; col++) {
                    box = GETBOX(trow,col);
 					if (row == trow) continue; // same spot
                    // make sure the cand occurs twice or more in COL in BOX
                    Add_SET_to_SET(ps2,&tset2); // add cands to tset2
                    if (box == tbox) {
    					if (col == tcol) continue; // same spot
                        // make sure the cand occurs twice or more in ROW in BOX
                        Add_SET_to_SET(ps2,&tset2); // add cands to tset2
                        continue; // this is MY BOX
                    }
					AddSet2Buf(tb,ps2);
					ADDSP(tb);
                    Elim_SET_in_SET( ps2, &tset );
                }
                Elim_SET_not_in_SET( &tset2, &tset );
                // Any candidate remaining?
                scnt = Get_Set_Cnt2(&tset, setvals);
				sprintf(EndBuf(tb),"left [%d] ", scnt);
				AddSet2Buf(tb,&tset);
                if (scnt) { // were ALL cand eliminates in outside box scan of this COL
					OUTITLK(tb);
                    // Yeek we have candidate(s) in this Box that are NOT in the rest of the COL
                    // This/These candidates can be ELIMINATED in other SLOTS in this BOX
                    // They are NEEDED to complete this COL, so MUST be in this COL in this BOX
                    // BOX ELIMINATION, except for this COL in this BOX
                    r = (trow / 3) * 3;
                    c = (tcol / 3) * 3;
                    for (rw = 0; rw < 3; rw++) {
                        for (cl = 0; cl < 3; cl++) {
                            row = r + rw;
                            col = c + cl;
                            if (col == tcol) continue;
                            val = pb->line[row].val[col];
                            if (val) continue;
                            ps2 = &pb->line[row].set[col];
                            //                      to   from flag (not all)
                            //cnt += Mark_SET_in_SET( ps2, ps, le_elim ); // Do_Locked_Excl2()
                            cnt += Mark_SET_in_SET( ps2, &tset, le_elim ); // Do_Locked_Excl2()
                            ZERO_SET(&set3);
                            //                from  to     on flag
                            Set_SET_per_flag( ps2,  &set3, le_elim);
                            rce.row = row;
                            rce.col = col;
                            rce.box = GETBOX(row,col);
                            COPY_SET(&rce.set,ps2);
                            Get_Set_Cnt(&set3, &rce.cnum);
                            pelims->push_back(rce); // store elim candidate
                        }
                    }
                    if (cnt) {
                        ps2 = &pb->line[trow].set[tcol];
                        //              to   from flag  (not all)
                        Mark_SET_in_SET( ps2, ps, le_mark ); // Do_Locked_Excl2()
                        count += cnt;
                        rce.row = trow;
                        rce.col = tcol;
                        rce.box = GETBOX(trow,tcol);
                        COPY_SET(&rce.set,ps2);
                        rce.cnum = -1;
                        pelimby->push_back(rce); // store eliminated by
                    }
                }
            }
            if (count && only_one_lk)
                break;
        }   // for (i = 0; i < max; i++) // for EACH empty cell in box
        if (count && only_one_lk)
            break;
    }   // for (j = 0; j < 9; j++)  // for EACH BOX
    return count;
}

// Locked 1
// Work Box (Region) by Box (Region)
int Do_Locked_Excl1( PABOX2 pb, vRC *pelims, vRC *pelimby )
{
    int count = 0;
    if (!pb)
        pb = get_curr_box();
    int row, col, val;
    int r, c, rw, cl, i;
    int cnt, cl2, cnt2, ecnt;
    int tots[3][3];
    int bad;
    PSET ps, ps2;
    ROWCOL rce;
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
                continue;   // try next Box/Region
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
            if (bad) continue;
            // Have a BLANK row - get to blank row
            for (rw = 0; rw < 3; rw++) {
                if (tots[rw][0]) continue;
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
                        if (val) continue;
                        ps = &pb->line[row].set[cl2];
                        sprintf(EndBuf(tb)," r%d c%d from ", row + 1, cl2 + 1);
                        AddSet2Buf(tb,ps);
                        ecnt = Mark_SET_in_SET( ps, ps2, le_elim, false ); // Do_Locked_Excl()
                        cnt2 += ecnt;
                        strcat(tb," to ");
                        AddSet2BufExFlag(tb,ps,le_elim);
                        //               from to    on flag
                        Set_SET_per_flag(ps,  &set3, le_elim);
                        if (ecnt) {
                            rce.row = row;
                            rce.col = cl2;
                            rce.box = GETBOX(row,cl2);
                            COPY_SET(&rce.set,ps);
                            Get_Set_Cnt(&set3, &rce.cnum);
                            pelims->push_back(rce);
                        }
                    }
                }
                count += cnt2;
                ADDSP(tb);
                sprintf(EndBuf(tb), "Elim [%d] ",cnt2);
                OUTITLK(tb);
                sprintf(tb,"Mrk lkd using ");
                AddSet2Buf(tb,&set3);
                if (cnt2) {
                    for (cl = 0; cl < 3; cl++) {
                        col = (c * 3) + cl;
                        ps = &pb->line[row].set[col];
                        sprintf(EndBuf(tb)," r%d c%d ", row +1, col + 1);
                        AddSet2Buf(tb,ps);
                        cnt2 += Mark_SET_in_SET( ps, &set3, le_mark ); // Do_Locked_Excl()
                        rce.row = row;
                        rce.col = col;
                        rce.box = GETBOX(row,col);
                        COPY_SET(&rce.set,ps);
                        Get_Set_Cnt(&set3, &rce.cnum);
                        pelimby->push_back(rce);
                    }
                }
                OUTITLK(tb);
                break;
            }
        }
    }
    if (!count)
        count = Do_Locked_Excl2(pb, pelims, pelimby);
    return count;
}

// Locked Exclusion FILL
int Do_Fill_Locked( PABOX2 pb )
{
    int count =  Do_Fill_By_Flags( pb, le_elim, le_mark, "LE", "Lock.Excl", le_cell, true );
    char *tb = GetNxtBuf();
    if (count) {
        sprintf(tb,"Slx: Filled Locked Excl %d - To Bgn", count);
        pb->iStage = sg_Begin;
    } else {
        sprintf(tb,"Slx: No Locked Excl filled - No Chg");
    }
    OUTIT(tb);
    return count;
}

int Do_Clear_Locked( PABOX2 pb )
{
    int count =  Do_Fill_By_Flags( pb, le_elim, le_mark, "LE", "Lock.Excl", le_cell, false );
    return count;
}

int Do_Locked_Excl( PABOX2 pb )
{
    if (!pb)
        pb = get_curr_box();
    vRC elims, elimby;
    int count = Do_Locked_Excl1( pb, &elims, &elimby );
    char *tb = GetNxtBuf();
    if (count) {
        size_t max, ii;
        PROWCOL prc;
        max = elims.size();
        sprintf(tb,"S%d: LE.Elim %d ", pb->iStage, (int)max);
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc, prc->cnum));
        }
        max = elimby.size();
        for (ii = 0; ii < max; ii++) {
            prc = &elimby[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc));
        }
        OUTIT(tb);
        sprintf(tb,"Sle: Marked Locked %d ", count);
        strcat(tb,"To Fill");
        pb->iStage = sg_Fill_Locked;
    } else {
        sprintf(tb,"Sle: No Locked marked ");
        //strcat(tb, "To begin");
        pb->iStage = sg_Begin;
    }
    OUTIT(tb);
    return count;
}

int Mark_Locked_Excl( PABOX2 pb )
{
    if (!pb)
        pb = get_curr_box();
    vRC elims, elimby;
    int one = only_one_lk;
    only_one_lk = 0;    // clear ONLY ONE flag
    int count = Do_Locked_Excl1( pb, &elims, &elimby );
    if (!count)
        count = Do_Aligned_Pair_Excl2( pb, &elims, &elimby );
    only_one_lk = one;  // restore ONLY ONE flag
    char *tb = GetNxtBuf();
    if (count) {
        size_t max, ii;
        PROWCOL prc;
        max = elims.size();
        sprintf(tb,"LE.Elim %d ", (int)max);
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc, prc->cnum));
        }
        max = elimby.size();
        for (ii = 0; ii < max; ii++) {
            prc = &elimby[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc));
        }
        OUTIT(tb);
        sprintf(tb,"Sle: Marked Locked %d", count);
        Stack_Fill(Do_Clear_Locked);
    } else {
        sprintf(tb,"Sle: No Locked cleared");
    }
    OUTIT(tb);
    return count;
}


// eof - Sudo_Locked.cxx
