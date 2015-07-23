// Sudo_SLinks.cxx

#include "Sudoku.hxx"

// STRONG LINKS - That is where a candidate (setval) only occurs TWICE in a ROW COL or BOX
// NOTE: Some STRONG LINK scanning is also in X-Cycles... Sudo_XCycles.cxx
// This was a 2nd attempt to get STRONG LINKS right ;=))
// and has been added to the normal scan logic
// Painting is by -
/* ======================================
void Paint_Strong_Links3(HDC hdc)
{
    int setval = Get_SL_setval();
    if (!VALUEVALID(setval)) return;
    vRC *pvrc = Get_SL_Chain_for_setval(setval);
    if (!pvrc) return;
    size_t max = pvrc->size();
    if (max < 2) return;
    ...
   ======================================= */
// =======================================================================================
static int only_one_sl = 1;

#define OUTITSL2(tb) if (add_debug_sl2) OUTIT(tb)

static char *stgstg = (char *)"ssl";

static vRC SL_Scans2[9];
static vRCP SL_FL_Pairs[9];
static int curr_SL_setval = 0;
int Get_SL_setval() { return curr_SL_setval; }
int Set_SL_invalid(int val) 
{
    int curr = curr_SL_setval;
    curr_SL_setval = val;
    return curr;
}
vRC *Get_SL_Chain_for_setval( int setval )
{
    if (!VALUEVALID(setval))
        return 0;
    return &SL_Scans2[setval - 1];
}
vRCP *Get_SL_FL_Pairs_for_setval( int setval )
{
    if (!VALUEVALID(setval))
        return 0;
    return &SL_FL_Pairs[setval - 1];
}

void Show_RC_SL_Chain2(vRC * vprc, int setval, char *type)
{
    size_t ii;
    size_t max = vprc->size();
    PROWCOL prc;
    bool onoff;
    char *tb = GetNxtBuf();
    const char *lnktyp;
    uint64_t flg, lflg;
    sprintf(tb,"%s: ",type);
    onoff = true;
    for (ii = 0; ii < max; ii++) {
        prc = &vprc->at(ii);
        Append_RC_Settings( tb, prc, setval );
        flg = prc->set.flag[setval - 1];
        lflg = (flg & cl_SLA); // isolate LINK type - same both ends
        lnktyp = (lflg & cl_SLA) ? stglnk : weklnk;
        if ((ii + 1) < max)
            strcat(tb,lnktyp);
    }
    OUTIT(tb);
}

int Check_SL_for_Elims(PABOX2 pb, vRC *pvrc, int setval)
{
    int count = 0;
    size_t max = pvrc->size();
    PROWCOL prc, prcf, prcl;
    time_t ch_num;
    size_t ii, jj, max2, max3;
    int i, val, elims;
    vINT vint;
    vRC chain;
    ROWCOL rc;
    PSET ps;
    uint64_t flg;
    elims = 0;
    char *tb = GetNxtBuf();
    sprintf(tb,"Check elims for chain ");
    for (ii = 0; ii < max; ii++) {
        prc = &pvrc->at(ii);
        ch_num = prc->set.tm;
        i = (int)ch_num;
        if (Int_in_iVector( i, &vint )) continue;
        vint.push_back(i);
        sprintf(EndBuf(tb),"%d ", i);
    }

    // must process CHAIN by CHAIN
    max2 = vint.size();
    sprintf(EndBuf(tb),"Total %d chains", (int)max2);
    OUTITSL2(tb);
    SL_FL_Pairs[setval-1].clear();
    for (jj = 0; jj < max2; jj++) {
        i = vint[jj];
        ch_num = (time_t)i;
        chain.clear();
        prcf = 0;
        prcl = 0;
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc->set.tm != ch_num) continue;
            chain.push_back(*prc);
            if (prcf)
                prcl = prc;
            else
                prcf = prc;
        }

        // ------------------------------------------------------
        max3 = chain.size(); // do we have LENGTH, and is it EVEN
        //if (add_debug_sl2) {
        //    sprintf(tb,"Chain %d, cnt %d ", (int)ch_num, (int)max3);
        //    Show_RC_SL_Chain2(&chain, setval, tb);
        //}
        // OUTITSL2(tb);
        // *****************************************************
        if ((max3 <= 2) || (max3 & 1)) continue;
        // *****************************************************
        // Ok got a EVEN STRONG LINK CHAIN
        // ------------------------------------------
        sprintf(tb,"First ");
        Append_RC_Settings( tb, prcf, setval );
        strcat(tb,", Last ");
        Append_RC_Settings( tb, prcl, setval );
        prcf = &chain[0];
        prcl = &chain[max3 - 1];
        strcat(tb," (or ");
        Append_RC_Settings( tb, prcf, setval );
        strcat(tb," & ");
        Append_RC_Settings( tb, prcl, setval );
        strcat(tb,")");
        OUTITSL2(tb);
        // ------------------------------------------
        // got FIRST and LAST of an EVEN STRONG LINK CHAIN
        // Any candidate see by BOTH end must die
        sprintf(tb, "SL Elims: ");
        elims = 0;
        rc.row = prcf->row;
        rc.col = prcl->col;
        val = pb->line[rc.row].val[rc.col];
        if (!val ) {
            ps = &pb->line[rc.row].set[rc.col];
            if (ps->val[setval - 1]) {
                ps->flag[setval - 1] |= cf_SLE;
                elims++;
                sprintf(EndBuf(tb),"%d@%c%d ", setval, (char)(rc.row + 'A'), rc.col + 1);
            }
        }
        rc.row = prcl->row;
        rc.col = prcf->col;
        val = pb->line[rc.row].val[rc.col];
        if (!val ) {
            ps = &pb->line[rc.row].set[rc.col];
            if (ps->val[setval - 1]) {
                ps->flag[setval - 1] |= cf_SLE;
                elims++;
                sprintf(EndBuf(tb),"%d@%c%d ", setval, (char)(rc.row + 'A'), rc.col + 1);
            }
        }
        if (elims) {
            flg = cf_XCA;
            for (ii = 0; ii < max3; ii++) {
                prc = &chain[ii]; // Mark ALL the members in the CHAIN
                pb->line[prc->row].set[prc->col].flag[setval - 1] |= (cf_SL | flg);
                flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
            }
            //pb->line[prcf->row].set[prcf->col].cellflg |= cf_XCA;
            //pb->line[prcl->row].set[prcl->col].cellflg |= cf_XCB;
            RCPAIR rcpair;
            rcpair.rowcol[0] = *prcf;
            rcpair.rowcol[1] = *prcl;
            SL_FL_Pairs[setval-1].push_back(rcpair);
        } else {
            strcat(tb,"None");
        }
        OUTITSL2(tb);
        count += elims;
    }
    return count;
}


int Follow_SL_Chain( PABOX2 pb, vRC *pvrc, int setval, time_t ch_num, uint64_t flg )
{
    int count = 0;
    int lval, row, col, val;
    ROWCOL rc;
    PSET ps;
    uint64_t flag;
    size_t max = pvrc->size();
    PROWCOL prclast = &pvrc->at(max - 1);
    lval = setval - 1;
    char *tb = GetNxtBuf();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if ( !(ps->val[lval]) ) continue;
            rc.row = row;
            rc.col = col;
            COPY_SET(&rc.set,ps);
            if (prc_in_chain( &rc, pvrc )) continue;
            flag = Get_RC_Strong_Link_Flag( pb, prclast, &rc, setval );
            if (!flag) continue;
            rc.set.tm = ch_num;
            rc.set.cellflg = flag;
            rc.set.flag[lval] = (flag | flg);
            pvrc->push_back(rc);
            sprintf(tb,"Continue with ");
            Append_RC_Settings( tb, &rc, setval );
            OUTITSL2(tb);
            count++;
            flg = (flg & cf_XCA) ? cf_XCB : cf_XCA; // next OPPOSITE
            count += Follow_SL_Chain( pb, pvrc, setval, ch_num, flg );
        }
    }
    return count;
}

// Sort into ordered chains
// count the links each cell has - cells with just one link 
// are the beginning and ends of the chain
// put them into the destination as contiguous chains
void Sort_RC_SL_Chain2( PABOX2 pb, vRC *psrc, vRC *pdst, int setval )
{
    size_t max = psrc->size();
    size_t ii, jj;
    PROWCOL prc1, prc2, prcnxt;
    int box1, box2;
    int lnkcnt;
    char *tb = GetNxtBuf();
    time_t ch_num = 0;

    pdst->clear();
    sprintf(tb, "Sort of %d cells...", (int)max);
    OUTITSL2(tb);
    for (ii = 0; ii < max; ii++) {
        prc1 = &psrc->at(ii);
        if (prc_in_chain(prc1,pdst)) continue;  // skip done
        box1 = GETBOX(prc1->row,prc1->col);
        lnkcnt = 0;
        sprintf(tb,"From ");
        Append_RC_Settings( tb, prc1, setval );
        for (jj = 0; jj < max; jj++) {
            if (jj == ii) continue;
            prc2 = &psrc->at(jj);
            if (prc_in_chain(prc2,pdst)) continue;  // skip done
            box2 = GETBOX(prc2->row,prc2->col);
            if ( !((box1 == box2)||(prc1->row == prc2->row)||(prc1->col == prc2->col)) ) continue;
            lnkcnt++;
            prcnxt = prc2;
            strcat(tb, " to ");
            Append_RC_Settings( tb, prc2, setval );
            if (lnkcnt > 1) {
                OUTITSL2(tb);
                break;
            }
        }
        if (lnkcnt == 1) {
            ch_num++;
            // prc1 is BEGIN or END of a CHAIN, and prcnxt is only connection
            prc1->set.tm = ch_num;
            prcnxt->set.tm = ch_num;
            pdst->push_back(*prc1);     // START of CHAIN
            pdst->push_back(*prcnxt);   // next in chain
            // follow the CHAIN
            box1 = GETBOX(prcnxt->row,prcnxt->col);
            for (jj = 0; jj < max; jj++) {
                if (jj == ii) continue;
                prc2 = &psrc->at(jj);
                if (prc_in_chain(prc2,pdst)) continue;  // skip done
                box2 = GETBOX(prc2->row,prc2->col);
                if ( !((box1 == box2)||(prcnxt->row == prc2->row)||(prcnxt->col == prc2->col)) ) continue;
                prc2->set.tm = ch_num;
                pdst->push_back(*prc2);   // next in chain
                prcnxt = prc2;
                box1 = GETBOX(prcnxt->row,prcnxt->col);
                jj = -1; // restart scan
                strcat(tb, " to ");
                Append_RC_Settings( tb, prc2, setval );
            }
            OUTITSL2(tb);
        }
    }

    ii = pdst->size();
    sprintf(tb,"Transferred %d of %d in %d chains", (int)ii, (int)max, (int)ch_num);
    OUTITSL2(tb);
    if (ii < max) {
        sprintf(tb,"Skipped %d ", (int)(max - ii));
        jj = 0;
        for (ii = 0; ii < max; ii++) {
            prc1 = &psrc->at(ii);
            if (prc_in_chain(prc1,pdst)) continue;  // skip done
            Append_RC_Settings( tb, prc1, setval );
            jj++;
        }
        if (!jj)
            strcat(tb,"DUPLICATE");
        OUTITSL2(tb);
    }

}

int Count_set_flag_by_Flag( PABOX2 pb, uint64_t flag, vRC *pvrc )
{
    int count = 0;
    int row, col, val, i;
    PSET ps;
    uint64_t flg;
    ROWCOL rc;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            for (i = 0; i < 9; i++) {
                if ( !(ps->val[i]) ) continue;
                flg = ps->flag[i];
                if ( !(flg & flag) ) continue;
                if (pvrc) {
                    rc.row = row;
                    rc.col = col;
                    COPY_SET(&rc.set,ps);
                    pvrc->push_back(rc);
                }
                count++;
            }
        }
    }
    return count;
}

int Check_SL_Elim( PABOX2 pb )
{
    vRC vrc;
    int count = Count_set_flag_by_Flag(pb, cf_SLE, &vrc);
    char *tb = GetNxtBuf();
    sprintf(tb,"SL Check: cnt %d ", count);
    size_t max = vrc.size();
    size_t ii;
    PROWCOL prc;
    for (ii = 0; ii < max; ii++) {
        prc = &vrc[ii];
        Append_RC_Settings(tb, prc);
        ADDSP(tb);
    }
    OUTIT(tb);
    return count;
}



// CALLED BY: int Mark_Strong_Link_for_setval( PABOX2 pb, int setval ) - for just MARKING 1 setval
// CALLED BY: int Do_Strong_Links2_Scan( PABOX2 pb ) - do all setvals - PART OF LOGIC CHAIN
// BOTH CALL: int Mark_Strong_Links2( PABOX2 pb ) which calls here
// Scans by a single setval
// Get a PAIR sharing this setval and test for a STRONG LINK
// calls : flag = Get_RC_Strong_Link_Flag( pb, &rc1, &rc2, setval );
// and includes BOX links - bool inc_box = true );
// Marks ends A and B, and stores the two SL cell in a vRC vector
// and calls :  count += Follow_SL_Chain( pb, pvrc, setval, ch_num, cf_XCA ); // next is A
// if (add_debug_sl2) { Show_RC_SL_Chain2(pvrc, setval,"SL-Chain: before sort"); }
// Since the process just starts with the first pair found, and add all following
// the chain is then SORTED and put in static vRC SL_Scans2[9];
// through vRC *pvrc2 = Get_SL_Chain_for_setval(setval);
//  Sort into contin   SRC -> DST Sort_RC_SL_Chain2( pb, pvrc, pvrc2, setval );
// That 'sorted' chain is then scanned for elims - val = Check_SL_for_Elims(pb, pvrc2, setval);
int Scan_Strong_Links_by_setval( PABOX2 pb, int setval )
{
    int count = 0;
    int lval, row, col, val;
    int row2, col2;
    ROWCOL rc1, rc2;
    PSET ps, ps2;
    uint64_t flag;
    vRC vrc;
    vRC *pvrc = &vrc;
    time_t ch_num = 0; // initialise CHAIN number
    lval = setval - 1;
    char *tb = GetNxtBuf();
    pvrc->clear();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if ( !(ps->val[lval]) ) continue;
            rc1.row = row;
            rc1.col = col;
            if (prc_in_chain( &rc1, pvrc )) continue;
            for (row2 = 0; row2 < 9; row2++) {
                for (col2 = 0; col2 < 9; col2++) {
                    if ((col2 == col)&&(row2 == row)) continue;
                    val = pb->line[row2].val[col2];
                    if (val) continue; // has a value
                    ps2 = &pb->line[row2].set[col2];
                    if ( !(ps2->val[lval]) ) continue;
                    rc2.row = row2;
                    rc2.col = col2;
                    if (prc_in_chain( &rc2, pvrc )) continue;
                    flag = Get_RC_Strong_Link_Flag( pb, &rc1, &rc2, setval );
                    if (!flag) continue;
                    COPY_SET(&rc1.set,ps);
                    COPY_SET(&rc2.set,ps2);
                    ch_num++; // bump CHAIN number
                    rc1.set.tm = ch_num;
                    rc2.set.tm = ch_num;
                    rc1.set.flag[lval] = (flag | cf_XCA); // start A
                    rc2.set.flag[lval] = (flag | cf_XCB); // next B
                    pvrc->push_back(rc1);
                    pvrc->push_back(rc2);
                    sprintf(tb,"Chn %d bgn ", (int) ch_num);
                    Append_RC_Settings( tb, &rc1, setval );
                    strcat(tb," to ");
                    Append_RC_Settings( tb, &rc2, setval );
                    OUTITSL2(tb);
                    count += 2;
                    count += Follow_SL_Chain( pb, pvrc, setval, ch_num, cf_XCA ); // next is A
                }
            }
        }
    }
    if (add_debug_sl2) {
        sprtf("Have SL %d chains, with %d cells\n", (int)ch_num, count);
        Show_RC_SL_Chain2(pvrc, setval,"SL-Chain: before sort");
    }
    vRC *pvrc2 = Get_SL_Chain_for_setval(setval);
    //  Sort into contin   SRC   DST
    Sort_RC_SL_Chain2( pb, pvrc, pvrc2, setval );
    // Got CHAIN in correct ORDER
    if (add_debug_sl2) Show_RC_SL_Chain2(pvrc2, setval,"SL-Chain: after sort");
    val = Check_SL_for_Elims(pb, pvrc2, setval);
    if (val) {
        sprtf("SL Scan elim %d\n",val);
        int val2 = Check_SL_Elim(pb);
    }
    count = val;
    return count;
}



int Do_Clear_Strong_Links(PABOX2 pb)
{
    int dbg_ff = add_debug_ff;
    if (add_debug_sl2)
        add_debug_ff = 1;

    int count = Do_Fill_By_Flags( pb, cf_SLE, cf_SL, "SL", "S-Links", 0, false );
    if (count) {
        sprtf("%s: Clear %d candidates\n", stgstg, count);
    } else {
        sprtf("%s: Clear NO candidates\n", stgstg, count);
    }

    if (add_debug_sl2)
        add_debug_ff = dbg_ff;

    Set_SL_invalid(0);
    set_repaint();
    return count;
}

int Do_Fill_Strong_Links(PABOX2 pb)
{
    int dbg_ff = add_debug_ff;
    if (add_debug_sl2)
        add_debug_ff = 1;

    int count = Do_Fill_By_Flags( pb, cf_SLE, cf_SL, "SL", "S-Links", 0, true );
    if (count) {
        sprtf("%s: Eliminated %d candidates - to bgn\n", stgstg, count);
        pb->iStage = sg_Begin;
    } else {
        sprtf("%s: Eliminated NO candidates\n", stgstg, count);
    }

    if (add_debug_sl2)
        add_debug_ff = dbg_ff;

    Set_SL_invalid(0);
    set_repaint();
    return count;
}

int Mark_Strong_Links2( PABOX2 pb )
{
    int count = 0;
    int lval, setval;
    for (lval = 0; lval < 9; lval++) {
        setval = lval + 1;
        if (curr_SL_setval && (curr_SL_setval != setval)) continue;
        count += Scan_Strong_Links_by_setval(pb,setval);
        if (count && only_one_sl) {
            curr_SL_setval = setval;
            break;
        }
    }
    return count;
}

int Mark_Strong_Link_for_setval( PABOX2 pb, int setval )
{
    curr_SL_setval = setval;
    int count = Mark_Strong_Links2( pb );
    if (count) {
        Stack_Fill(Do_Clear_Strong_Links);
    } else {
        curr_SL_setval = 0;
    }
    return count;
}

int Do_Strong_Links2_Scan( PABOX2 pb )
{
    int count = 0;
    curr_SL_setval = 0;
    count = Mark_Strong_Links2( pb );
    if (count) {
        sprtf("%s: SL-Chains %d - to fill\n", stgstg, count);
        pb->iStage = sg_Fill_SLChains;
    } else {
        sprtf("%s: SL-Chains - to bgn\n", stgstg, count);
        pb->iStage = sg_Begin;
    }

    return count;
}

// eof - Sudo_SLinks.cxx
