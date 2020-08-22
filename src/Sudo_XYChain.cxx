// Sudo_XYChain.txt

#include "Sudoku.hxx"

#define OUTITXY(tb) if (add_debug_xy) OUTIT(tb)

int debug_ONE_setval = 0;   // to turn ON debug just for ONE setval


BOOL g_b1BxLnk = TRUE;
BOOL g_bExBgn  = TRUE;
BOOL g_bNoRpt  = FALSE; // TRUE; // perhaps this should be NO REPEAT in SAME ROW, COL or BOX
// BUT now I have found an example xychain3.txt where the CHAIN is
// 8@B2-2@B2--2@D8-4@D8--4@D3-6@D3--6@D3-2@D6--2@D3-8@C6
// MAYBE I can ALLOW this, BUT take good care of the OPPOSITE coloring
// ie Start with A color, 
// A8@B2-B2@B2--A2@D8-B4@D8--A4@D3-B6@D3--A6@D3-B2@D6--A2@D3-B8@C6
// But would not this ALWAYS be the CASE since the flip is in each cell
// Maybe I just miss this ELIMINATION due to
// A8@B2 is in BOX 3, and B8@C6 is in the SAME ROW as Elim 8@B9???

// Another EXAMPLE
// examples\xyxhain4.txt - evolved from diablical4.txt
// Chain: 3@E7-9@E7--9@F9-6@F9--6@G9-1@G9--1@I9-3@I9
// Elim : 3@I7 due seen by ends 3@E7 and 3@I9
// I get
// Start XY 3@E7-9@E7<->9@F9-6@F9
// Cont 6@G9-1@G9 = good
// Cont 1@G6-8@G6
// Cont 8@F6-9@F6
// Cont 9@E4-5@E4
// Drop 9@E4-5@E4
// Drop 8@F6-9@F6
// Drop 1@G6-8@G6
// Drop 6@G9-1@G9
// But never get 1@I9-3@I9 EOC - WHY
// Need MORE, BETTER diags!!!
#define DO_IMMEDIATE_ELIM  // FIX20120924 - Do NOT fall back when chain closes, but CHECK elims IMMEDIATELY
// only FALL BACK if elims are found and only_one_mark is ON - FIXED THE ABOVE

static const char *stgstg = "s2h";
#define END_OF_CHAIN -1
int min_in_chain = 3; // minimum in chain, before seeking END OF CHAIN

// last_lnk_flg = LINK by BOX, COL, ROW
// #define cl_LBB  0x0080000000000000
// #define cl_LBC  0x0040000000000000
// #define cl_LBR  0x0020000000000000
// #define cl_LBA  (cl_LBB|cl_LBC|cl_LBR)

typedef struct tagXYSTR {
    PABOX2 pb;
    int elim_count;
    int bgn_setval;
    int nxt_setval;
    int prev_setval;
    vRC *pempty;
    vRC *pelims;
    PRCRCB prcrcb;
    PRCRCB prcrcball;
    vRC chain;
    int count;
    PROWCOL prcfirst;
    vRCP *prcpairs; // avoid doing a chain twice
    uint64_t last_lnk_flg;
    vINT vsetvals;
    int level;
    //char dbg_buf[1024]; // UGH! Found buffer OVERRUN, so remove it
}XYSTR, *PXYSTR;

static vRC curr_act_chain;
static XYSTR xystr;
static int xy_chain_elims = 0;
int only_one_mark = 1;
int test_xy_chain = 0;

vRC *Get_XY_Act_Chain() { return &curr_act_chain; }
void Set_XY_Elim_Count(int cnt) { xy_chain_elims = cnt; }
void Set_XY_setval(int setval) { xystr.bgn_setval = setval; }
int Get_XY_setval() { return xystr.bgn_setval; }
int Get_XY_Elim_Count() { return xy_chain_elims; }

vRC *Get_XY_Chain()
{
    if (!xy_chain_elims)
        return 0;
    vRC *pvrc = Get_XY_Act_Chain();
    size_t max = pvrc->size();
    if (max < 4)
        return 0;
    return pvrc;
}
int Get_XY_Bgn_Val() { return xystr.bgn_setval; }
void Set_XYChain_Invalid() { Set_XY_Elim_Count(0); }

bool PRCP_in_RCP_Vector(vRCP *plist, PRCPAIR prcp)
{
    size_t ii;
    PRCPAIR prcp2;
    PROWCOL prc1_in, prc2_in, prc1, prc2;
    size_t max = plist->size();
    if (!max)   // quick out if nothing in list
        return false;
    // get the TWO RC pointer of the PAIR
    prc1_in = &prcp->rowcol[0];
    prc2_in = &prcp->rowcol[1];
    // dance with the LIST, trying to find the SAME RC pair
    for (ii = 0; ii < max; ii++) {
        prcp2 = &plist->at(ii);
        prc1 = &prcp2->rowcol[0];
        prc2 = &prcp2->rowcol[1];
        if (SAMERCCELL(prc1_in,prc1) && SAMERCCELL(prc2_in,prc2))
            return true;
        if (SAMERCCELL(prc2_in,prc1) && SAMERCCELL(prc1_in,prc2))
            return true;
    }
    return false;
}


int Show_XY_Chain(PXYSTR pxystr)
{
    int count = 0;
    int bgn_setval = pxystr->bgn_setval;
    vRC *pchain    = &pxystr->chain;
    size_t max     = pchain->size();
    size_t ii;
    PROWCOL prc;
    PSET ps;
    int nxtval, bgnval, cnt;
    int setvals[9];
    char *tb = GetNxtBuf();
    char *tb2 = GetNxtBuf();
    char *tb3 = GetNxtBuf();
    bool onoff;
    nxtval = bgnval = bgn_setval;

    sprintf(tb,"XY for %d cnt %d ", bgnval, (int) max);
    *tb2 = 0;
    onoff = true;
    const char *on;
    const char *off;
    char crow;
    sprintf(tb3,"XY-Cells: ");
    for (ii = 0; ii < max; ii++) {
        prc = &pchain->at(ii);
        ps = &prc->set;
        cnt = Get_Set_Cnt2(ps,setvals);
        if (cnt != 2) // this would be an ERROR
            continue;
        if (setvals[0] == bgnval) {
            nxtval = setvals[1];
        } else {
            nxtval = setvals[0];
        }

        sprintf(EndBuf(tb),"%s-%s", Get_RC_setval_RC_Stg(prc, bgnval),
            Get_RC_setval_RC_Stg(prc, nxtval) );
        sprintf(EndBuf(tb3),"%s ",Get_RC_setval_RC_Stg(prc));

        off = (onoff ? "-" : "+");
        on  = (onoff ? "+" : "-"),
        crow = (char)(prc->row + 'A');

        // start off
        sprintf(EndBuf(tb2),"%s%d[%c%d]%s%d[%c%d]",
            off, bgnval, crow, prc->col + 1,
            on,  nxtval, crow, prc->col + 1 );
        // start on
        //sprintf(EndBuf(tb3),"%s%d[%c%d]%s%d[%c%d]",
        //    on,  bgnval, crow, prc->col + 1,
        //    off, nxtval, crow, prc->col + 1 );

        bgnval = nxtval;
        // onoff = !onoff;

        if ((ii + 1) < max)
            strcat(tb,"<->");
    }
    OUTIT(tb);
    OUTITXY(tb2);
    OUTITXY(tb3);
    return count;
}

int Mark_XY_Chain_setvals(PXYSTR pxystr)
{
    int count = 0;
    PABOX2 pb      = pxystr->pb;
    int bgn_setval = pxystr->bgn_setval;
    vRC *pchain    = &pxystr->chain;
    size_t max     = pchain->size();
    char *tb = GetNxtBuf();
    size_t ii;
    PROWCOL prc;
    PSET ps;
    int nxtval, bgnval, cnt;
    int setvals[9];
    uint32_t flg, flg1, flg2;
    nxtval = bgnval = bgn_setval;
    sprintf(tb,"MarkXY %d ", (int) max);
    flg1 = cf_XCA;
    flg2 = cf_XCB;
    for (ii = 0; ii < max; ii++) {
        prc = &pchain->at(ii);
        ps = &prc->set;
        cnt = Get_Set_Cnt2(ps,setvals);
        if (cnt != 2) // this would be an ERROR
            continue;
        if (setvals[0] == bgnval) {
            nxtval = bgnval;
            sprintf(EndBuf(tb),"%s-%s", Get_RC_setval_RC_Stg(prc, bgnval),
                Get_RC_setval_RC_Stg(prc, setvals[1]) );
            bgnval = setvals[1];
        } else {
            nxtval = bgnval;
            sprintf(EndBuf(tb),"%s-%s", Get_RC_setval_RC_Stg(prc, bgnval),
                Get_RC_setval_RC_Stg(prc, setvals[0]) );
            bgnval = setvals[0];
        }
        pb->line[prc->row].set[prc->col].flag[nxtval - 1] |= (cf_XY | flg1);
        pb->line[prc->row].set[prc->col].flag[bgnval - 1] |= (cf_XY | flg2);
        // switch the COLOR flags
        flg = flg1;
        flg1 = flg2;
        flg2 = flg;

        if ((ii + 1) < max)
            strcat(tb,"<->");
    }
    OUTITXY(tb);
    //if (add_debug_xy) {
    //    sprintf(tb,"XYC: ");
    //    for (ii = 0; ii < max; ii++) {
    //        prc = &pchain->at(ii);
    //        sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc));
    //    }
    //    OUTITXY(tb);
    //}
    return count;
}

int Test_XY_Chain_for_Elims(PXYSTR pxystr)
{
    int count = 0;
    PABOX2 pb      = pxystr->pb;
    int bgn_setval = pxystr->bgn_setval;
    int nxt_setval = pxystr->nxt_setval;
    vRC *pempty    = pxystr->pempty;
    size_t max     = pempty->size();
    int ccnt       = pxystr->count;
    PRCRCB all     = pxystr->prcrcball;
    vRC *pelims    = pxystr->pelims;
    vRC *pchain    = &pxystr->chain;

    size_t ii;
    PROWCOL prc, prc1, prc2;
    PSET ps;
    vRC *pvrc;
    int lval = bgn_setval - 1;
    int box, box2;
    RCPAIR rcpair;
    vRCP *prcpair = pxystr->prcpairs;
    uint64_t flag = 0;
    char *tb = GetNxtBuf();

    ii = pchain->size();
    prc1 = &pchain->at(0);                // get FIRST
    prc2 = &pchain->at(ii - 1); // get LAST
    rcpair.rowcol[0] = *prc1;
    rcpair.rowcol[1] = *prc2;

    if (PRCP_in_RCP_Vector(prcpair, &rcpair))
        return 0;
    prcpair->push_back(rcpair); // do this NOW

    size_t max_bgn = pelims->size();
    // if (add_debug_xy) Show_XY_Chain( pxystr );
    // if first and last share a ROW
    if (prc1->row == prc2->row) {
        flag |= cl_LBR; // linked by ROW
        pvrc = &all->vrows[prc1->row];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc_in_chain(prc,pchain)) continue;
            ps = &prc->set;
            if ( !(ps->val[lval]) ) continue;
            if (prc_in_chain(prc,pelims)) continue;
            pelims->push_back(*prc);
        }
    }

    // if first and last share a COL
    if (prc1->col == prc2->col) {
        flag |= cl_LBC; // LINKED by COL
        pvrc = &all->vcols[prc1->col];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc_in_chain(prc,pchain)) continue;
            ps = &prc->set;
            if ( !(ps->val[lval]) ) continue;
            if (prc_in_chain(prc,pelims)) continue;
            pelims->push_back(*prc);
        }
    }
    // and check the BOXES
    box = GETBOX(prc1->row,prc1->col);
    box2 = GETBOX(prc2->row,prc2->col);
    if (box == box2)
    {
        flag |= cl_LBB; // linked by BOX
        pvrc = &all->vboxs[box];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc_in_chain(prc,pchain)) continue;
            ps = &prc->set;
            if ( !(ps->val[lval]) ) continue;
            if (prc_in_chain(prc,pelims)) continue;
            pelims->push_back(*prc);
        }
    }
#if 0
    if (box != box2) {
        pvrc = &all->vboxs[box2];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc_in_chain(prc,pchain)) continue;
            ps = &prc->set;
            if ( !(ps->val[lval]) ) continue;
            if (prc_in_chain(prc,pelims)) continue;
            pelims->push_back(*prc);
        }
    }
#endif // 0
    if (flag == 0) {
        // hmmm, first and LAST NOT linked by ROW, COL or BOX
        // Candidate 'seen' by both are under threat
        // Say candidates in SAME box, on SAME ROW COL as other
        pvrc = &all->vboxs[box];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc_in_chain(prc,pchain)) continue;
            ps = &prc->set;
            if ( !(ps->val[lval]) ) continue;
            if (prc_in_chain(prc,pelims)) continue;
            // IFF seen by OTHER - prc2
            if ((prc->row == prc2->row)||(prc->col == prc2->col))
                pelims->push_back(*prc);
        }
        pvrc = &all->vboxs[box2];
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc = &pvrc->at(ii);
            if (prc_in_chain(prc,pchain)) continue;
            ps = &prc->set;
            if ( !(ps->val[lval]) ) continue;
            if (prc_in_chain(prc,pelims)) continue;
            // IFF seen by OTHER - prc1
            if ((prc->row == prc1->row)||(prc->col == prc1->col))
                pelims->push_back(*prc);
        }
    }

    max = pelims->size();
    if (max > max_bgn) {
        sprintf(tb,"XY_Chain2: First %s Last %s of %d ", Get_RC_setval_RC_Stg(prc1), Get_RC_setval_RC_Stg(prc2),
            (int) (max - max_bgn) );
        OUTITXY(tb);
        if (add_debug_xy) Show_XY_Chain( pxystr );
        sprintf(tb,"XYC-ELIM: %d x %d ", bgn_setval, (int)(max - max_bgn));
        for (ii = max_bgn; ii < max; ii++) {
            prc = &pelims->at(ii);
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc, bgn_setval));
            pb->line[prc->row].set[prc->col].flag[bgn_setval - 1] |= cf_XYE;
            count++; // bump the ELIM count
        }
        Mark_XY_Chain_setvals(pxystr);
        if (count) {
            OUTITXY(tb);
            vRC *pact_chain = Get_XY_Act_Chain();
            pact_chain->clear();
            xy_chain_elims = count;
            pchain = &pxystr->chain;
            max = pchain->size();
            sprintf(tb,"XYC: ");
            for (ii = 0; ii < max; ii++) {
                prc = &pchain->at(ii);
                pact_chain->push_back(*prc);
                sprintf(EndBuf(tb),"%s ",Get_RC_setval_RC_Stg(prc));
            }
            sprintf(EndBuf(tb),"Trans %d to act_chain.", (int)max);
            OUTITXY(tb);
        }
        pxystr->elim_count += count;
    //} else {
    //    strcat(tb,"NO elims");
    //    OUTITXY(tb);
    }
    return count;
}

// Follow the CHAIN until END = Has same second as first
// =====================================================
int Follow_XY_Chain(PXYSTR pxystr)
{
    int count = 0;
    int bgn_setval = pxystr->bgn_setval;
    int nxt_setval = pxystr->nxt_setval;
    vRC *pempty    = pxystr->pempty;
    size_t max     = pempty->size();
    vRC *pchain    = &pxystr->chain;
    int ccnt       = pxystr->count;
    vINT *pvsv     = &pxystr->vsetvals;
    int level      = pxystr->level;
    // char *tsb      = &pxystr->dbg_buf[0];
    size_t ii;
    PROWCOL prc;
    PSET ps;
    int cnt, ret, row, col, box, box2, nxtval;
    int setvals[9];
    prc = &pchain->at(pchain->size()-1);
    row = prc->row;
    col = prc->col;
    box = GETBOX(row,col);
    ret = 0;
    char *tb = GetNxtBuf();
    for (ii = 0; ii < max; ii++) {
        prc = &pempty->at(ii);
        box2 = GETBOX(prc->row,prc->col);
        if ( !((box == box2)||(prc->row == row)||(prc->col == col)) )
            continue;   // MUST share a ROW, COL or BOX with LAST to form chain
        if (g_b1BxLnk && (box == box2) && (pxystr->last_lnk_flg & cl_LBB))
            continue; // NO two BOX links allowed
        if (prc_in_chain(prc,pchain)) continue; // skip if ALREADY in this CHAIN
        ps = &prc->set;
        cnt = Get_Set_Cnt2(ps,setvals);
        if (cnt != 2) continue; // we KNOW it has TWO values, but
        if (setvals[0] == nxt_setval) {
            nxtval = setvals[1];
        } else if (setvals[1] == nxt_setval) {
            nxtval = setvals[0];
        } else
            continue;
        if (g_bExBgn && (ccnt < min_in_chain) && (nxtval == bgn_setval))
            continue;
        if (g_bNoRpt && (Int_in_iVector( nxtval, pvsv))) continue;

        if (box == box2)
            pxystr->last_lnk_flg = cl_LBB;
        else if (prc->row == row)
            pxystr->last_lnk_flg = cl_LBR;
        else if (prc->col == col)
            pxystr->last_lnk_flg = cl_LBC;
        else
            pxystr->last_lnk_flg = 0; // THIS IS A CODE ERROR
        pxystr->prev_setval = pxystr->nxt_setval; 
        pxystr->nxt_setval = nxtval;
        pxystr->count++;
        pchain->push_back(*prc); // ADD TO CHAIN
        pvsv->push_back(nxtval);
        if ((ccnt >= min_in_chain)&&(nxtval == bgn_setval)) {
            sprintf(tb,"Cont %d %s-%s ", level + 1,
                Get_RC_setval_RC_Stg(prc, nxt_setval),
                Get_RC_setval_RC_Stg(prc,nxtval) );
            //strcat(tsb,tb);
            sprintf(EndBuf(tb)," EOC %d", level + 1);
#ifdef DO_IMMEDIATE_ELIM
            OUTITXY(tb);
            count += Test_XY_Chain_for_Elims( pxystr );
            if (count && only_one_mark)
                ret = END_OF_CHAIN; // FALL BACK - got an ELIM
#else // !DO_IMMEDIATE_ELIM
            ret = END_OF_CHAIN;
#endif // DO_IMMEDIATE_ELIM
        } else {
            sprintf(tb,"Cont %d %s-%s ", level + 1,
                Get_RC_setval_RC_Stg(prc, nxt_setval),
                Get_RC_setval_RC_Stg(prc,nxtval) );
            //strcat(tsb,tb);
            OUTITXY(tb);
            pxystr->level = level + 1;
            ret = Follow_XY_Chain( pxystr );
        }
        if (ret == END_OF_CHAIN) {
            if (*tb)
                strcat(tb," EOC");
            //if (*tsb)
            //    sprintf(EndBuf(tsb)," EOC %d", level + 1);
            OUTITXY(tb);
            //OUTITXY(tsb);
            return END_OF_CHAIN;
        } else {
            pxystr->nxt_setval = pxystr->prev_setval;
            pxystr->count--;     // FIX20120928 - BACK UP COUNT
            // get LAST pushed back OFF chain
            pchain->pop_back(); // FIX20120928 - KICK OFF LAST
            pvsv->pop_back();   // FIX20120923 - KICK off LAST
            sprintf(tb,"Drop %s-%s ", Get_RC_setval_RC_Stg(prc, nxt_setval),
                    Get_RC_setval_RC_Stg(prc,nxtval) );
            //sprintf(EndBuf(tsb),"%s %d ", tb, level + 1);
            OUTITXY(tb);
        }
    }

    //sprintf(tb,"Stopped");
    //OUTITXY(tb);
    return count;
}

int Show_XY_Chain_OK(PXYSTR pxystr)
{
    int count = 0;
    int bgn_setval = pxystr->bgn_setval;
    vRC *pchain    = &pxystr->chain;
    size_t max     = pchain->size();
    char *tb = GetNxtBuf();
    size_t ii;
    PROWCOL prc;
    PSET ps;
    int nxtval, bgnval, cnt;
    int setvals[9];
    nxtval = bgnval = bgn_setval;
    sprintf(tb,"XYChain %d ", (int) max);
    for (ii = 0; ii < max; ii++) {
        prc = &pchain->at(ii);
        ps = &prc->set;
        cnt = Get_Set_Cnt2(ps,setvals);
        if (cnt != 2) // this would be an ERROR
            continue;
        if (setvals[0] == bgnval) {
            sprintf(EndBuf(tb),"%s-%s", Get_RC_setval_RC_Stg(prc, bgnval),
                Get_RC_setval_RC_Stg(prc, setvals[1]) );
            bgnval = setvals[1];
        } else {
            sprintf(EndBuf(tb),"%s-%s", Get_RC_setval_RC_Stg(prc, bgnval),
                Get_RC_setval_RC_Stg(prc, setvals[0]) );
            bgnval = setvals[0];
        }
        if ((ii + 1) < max)
            strcat(tb,"<->");
    }
    OUTIT(tb);
    return count;
}


// Since CHAINS a bi-directional, will always find the reverse of a chain
// which will only eliminate the members a second time, so keep a list of
// first-last PAIRS, and dump a chain that is JUST the SAME PAIR (in reverse of course)
int Mark_XY_Chain( PABOX2 pb, int setval, vRC &empty, PRCRCB prcrcb, PRCRCB prcrcball )
{
    int count = 0;
    int lval = setval - 1;
    size_t max = empty.size();
    size_t ii, max2, jj;
    PROWCOL prc, prc2;
    PSET ps, ps2;
    int cnt, cnt2, ret, nxtval, lval2, box1, box2;
    int setvals[9];
    int setvals2[9];
    vRC *pvrc;
    bool fnd;
    vRC elimins;
    xystr.pb = pb;
    xystr.pempty = &empty;
    xystr.prcrcb = prcrcb;
    xystr.prcrcball = prcrcball;
    xystr.pelims = &elimins;
    char *tb = GetNxtBuf();
    if (debug_ONE_setval) {
        add_debug_xy = (setval == debug_ONE_setval) ? 1 : 0;
    }
    if (add_debug_xy) sprtf("Mark XY-Chains for setval %d\n", setval);
    vRCP rcpairs;
    xystr.prcpairs = &rcpairs;
    xystr.bgn_setval = setval;  // BEGIN chain with this VALUE
    xystr.elim_count = 0;       // keep COUNT of ELIMS
    for (ii = 0; ii < max; ii++) {
        prc = &empty[ii];
        ps = &prc->set;
        if ( !(ps->val[lval]) ) continue; // start chain ONLY with setval given
        cnt = Get_Set_Cnt2(ps,setvals);
        if (cnt != 2) continue; // we KNOW it has TWO values, but
        if (setvals[0] == setval)
            nxtval = setvals[1];
        else if (setvals[1] == setval)
            nxtval = setvals[0];
        else
            continue; // NEVER BE HERE - IS AN ERROR IF SO
        lval2 = nxtval - 1;
        pvrc = &prcrcb->vrows[prc->row];
        max2 = pvrc->size();
        xystr.prcfirst = prc;
        xystr.vsetvals.clear();
        xystr.vsetvals.push_back(nxtval);
        // for (jj = 0; jj < max2; jj++) {
        //    prc2 = &pvrc->at(jj);
        //    if (prc2->col == prc->col) continue; // skip SELF
        // scan everywhere - same row, col, box for next sharing one
        box1 = GETBOX(prc->row,prc->col);
        for (jj = 0; jj < max; jj++) {
            if (jj == ii) continue;
            prc2 = &empty[jj];
            if ((prc2->row == prc->row) && (prc2->col == prc->col)) continue; // skip SELF
            box2 = GETBOX(prc2->row,prc2->col);
            if ( !((prc2->row == prc->row) || (prc2->col == prc->col) || (box1 == box2)) )
                continue; // MUST share a ROW, COL or BOX
            ps2 = &prc2->set;
            if ( !(ps2->val[lval2]) ) continue; // does NOT contain continuation setval
            cnt2 = Get_Set_Cnt2(ps2,setvals2);
            if (cnt2 != 2) continue; // we KNOW it has TWO values, but
            fnd = false;
            if (nxtval == setvals2[0]) {
                if (setvals2[1] == setval) continue; // 2nd must NOT contain bgn_setval
                // got our first link - start value is setval, chain on val setvals2[1]
                xystr.nxt_setval = setvals2[1];
                fnd = true;
            } else if (nxtval == setvals2[1]) {
                if (setvals2[0] == setval) continue; // 2nd must NOT contain bgn_setval
                // got our first link - start value is setval, chain val setvals2[0]
                xystr.nxt_setval = setvals2[0];
                fnd = true;
            } else {
                // would be a CODE ERROR since this cell must contain this nxtval
            }
            if (fnd) {
                if (box1 == box2)
                    xystr.last_lnk_flg = cl_LBB;
                else if (prc2->row == prc->row)
                    xystr.last_lnk_flg = cl_LBR;
                else if (prc2->col == prc->col)
                    xystr.last_lnk_flg = cl_LBC;
                else    // NEVER HERE except as a coding error
                    xystr.last_lnk_flg = 0;
                xystr.chain.clear();
                xystr.chain.push_back(*prc);
                xystr.chain.push_back(*prc2);
                xystr.count = 2;
                xystr.vsetvals.push_back(xystr.nxt_setval);
                sprintf(tb,"Start XY %s-%s", Get_RC_setval_RC_Stg(prc, setval),
                    Get_RC_setval_RC_Stg(prc,nxtval) );
                sprintf(EndBuf(tb),"<->%s-%s ", Get_RC_setval_RC_Stg(prc2, nxtval),
                    Get_RC_setval_RC_Stg(prc2,xystr.nxt_setval) );
                //strcpy(xystr.dbg_buf,tb); // Being the DIAG string
                OUTITXY(tb);
                xystr.level = 0; // start the LEVEL counter
                ret = Follow_XY_Chain(&xystr);
                count += xystr.elim_count;
                if (count && only_one_mark)
                    break;
                xystr.elim_count = 0;
#ifdef DO_IMMEDIATE_ELIM
                xystr.chain.clear(); // clear CHAIN for a NEW start
#else // !DO_IMMEDIATE_ELIM
                if (ret == END_OF_CHAIN) {
                    count += Test_XY_Chain_for_Elims( &xystr );
                } else {
                    // remove ALL NOW
                    xystr.chain.clear();
                }
#endif // DO_IMMEDIATE_ELIM y/n
            }   // fnd
            if (count && only_one_mark)
                break;
        }   // for (jj = 0; jj < max; jj++)
        if (count && only_one_mark)
            break;
    }   // for (ii = 0; ii < max; ii++)
    return count;
}

int Do_Clear_XYChains(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_XYE, cf_XY, "XY", "XY-Chains", 0, false );
    xy_chain_elims = 0;
    if (count) {
        sprtf("%s: Clear %d candidates\n", stgstg, count);
    } else {
        sprtf("%s: Clear NO candidates\n", stgstg, count);
    }
    return count;
}

int Do_Fill_XYChains(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_XYE, cf_XY, "XY", "XY-Chains", 0, true );
    xy_chain_elims = 0;
    char *tb = GetNxtBuf();
    if (count) {
        sprtf("%s: Eliminated %d candidates - to bgn\n", stgstg, count);
        pb->iStage = sg_Begin;
    } else {
        sprtf("%s: Eliminated NO candidates\n", stgstg, count);
    }
    return count;
}


int Mark_XY_Chains( PABOX2 pb )
{
    int count = 0;
    vRC empty, emptyall;
    RCRCB rcrcb, rcrcball;
    int i;
    int ecnt = Get_Empty_Cells_per_cand_count( pb, &empty, 2, &rcrcb );
    xy_chain_elims = 0;
    if (!ecnt || (ecnt >= 81))
        return 0;
    int acnt = Get_Empty_Cells( pb, emptyall, &rcrcball );
    for (i = 0; i < 9; i++) {
        if (test_xy_chain && ((i + 1) != test_xy_chain)) continue; // for test limit search
        count += Mark_XY_Chain( pb, i+1, empty, &rcrcb, &rcrcball );
        if (count && only_one_mark)
            break;
    }
    if (count) {
        Stack_Fill(Do_Clear_XYChains);
        sprtf("Done XY-Chains with %d cells marked\n", count);
    }

    return count;
}

int Do_XY_Chains( PABOX2 pb )
{
    int count = 0;
    vRC empty, emptyall;
    RCRCB rcrcb, rcrcball;
    int i;
    int ecnt = Get_Empty_Cells_per_cand_count( pb, &empty, 2, &rcrcb );
    xy_chain_elims = 0;
    if (!ecnt || (ecnt >= 81))
        return 0;
    int acnt = Get_Empty_Cells( pb, emptyall, &rcrcball );
    for (i = 0; i < 9; i++) {
        if (test_xy_chain && ((i + 1) != test_xy_chain)) continue; // for test limit search
        count += Mark_XY_Chain( pb, i+1, empty, &rcrcb, &rcrcball );
        if (count && only_one_mark)
            break;
    }
    if (count) {
        sprtf("%s: XY-Chains %d - to fill %d\n", stgstg, count, sg_Fill_XYChains);
        pb->iStage = sg_Fill_XYChains;
    } else {
        sprtf("%s: XY-Chains - to bgn\n", stgstg, count);
        pb->iStage = sg_Begin;
    }
    return count;
}

// eof - Sudo_XYChain.cxx

