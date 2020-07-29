// Sudo-AIC.cxx
// Alternating Inference Chains
// Extension of X-Cycles where any candidate can be used
#include "Sudoku.hxx"

/* ==================================================================
   from : http://www.sudokuwiki.org/Alternating_Inference_Chains

    Alternating Inference Chains

    Chaining strategies now take a new leap with Alternating Inference Chains. These extend X-Cycle 
    into a new dimension - where X-Cycles stuck to a single number, AICs use any candidate number.

    AICs encapsulate all the discussion of chaining strategies so far. It's very useful to split out 
    chain-like strategies into X-Wings, XY-Chains, Forcing Chains, XYZ-Wings, X-Cycles, Nice Loops and 
    so on, since they have special characteristics which make them spottable. But it turns out they are 
    all part of a larger extended family.

    As we saw in the previous chapter, alternation is just what X-Cycles are about. However, you'll remember 
    that X-Cycles are applied only to a single candidate number. AICs, on the other hand, take everything 
    from an X-Cycle and extend the logic to as many different candidate numbers as necessary.

    AICs ask the question "How many ways are there to make a strong or a weak link?" If there is more than 
    one way, we can join them up in an alternating manner and make deductions leading to eliminations. 
    Let's look back on the previous chain-like strategies and note the following:

    - We can link two candidates of the same value in a unit - this is called "bi-location" (X-Cycles).
    - We can link two different candidates in the same cell - this is called "bi-value".

    There are also other ways (see later articles), but for now let's keep it simple and stick to these 
    two dimensions - links between cells and within cells.

   ================================================================== */

#define DO_CELL_VALUES_FIRST
#define MY_MX_CHAINS 512

#define OUTITAIC(tb) if (add_debug_aic) OUTIT(tb)

#define aic_memb    cf_XY       // AIC-chain member (also XY-Chain member)
#define aic_elim    cf_XYE      // XY-Chain candidate elimination
// for cell coloring, use cf_XCA and cf_XCB

int only_one_aic = 1;
int treat_aic_warn_as_error = 1;

// int test_start_cand = 8;
// examples\AIC-scan-01.txt - 
int test_start_cand = 0; // 4;
int reject_chains_of_one_value = 1;
int reject_chains_that_loop_over = 1;   // wow how to make sure this does NOT happen

// In examples\AIC-scan-01.txt looking for 
// AIC (Alternating Inference Chain) Rule 1:
// +4[B7]-4[B2]+7[B2]-7[B5]+6[B5]-6[H5]+4[H5]-4[H7]+4[B7]
// - Off-chain candidate 7 taken off B3 - weak link: B2 to B5
// - Off-chain candidate 6 taken off J5 - weak link: B5 to H5

typedef struct tagAICSTR {
    PABOX2 pb;
    int setval, links;
    int elim_count;
    vRC *pvrc;
    vRCP *pvrcp;
    uint64_t dirflg; // last direction scanned
    uint64_t clrflg; // current color
    PRCRCB prcrcb;
    ROWCOL rcFirst; // stays CONSTANT from beginning
    ROWCOL rcLast;  // always current LAST in CHAIN
    bool exit_scan;
    bool had_error; // AIC elim a candidate needed for the (cheat) solution
    char *scan;
    vINT *pvirow, *pvicol, *pvibox;
}AICSTR, *PAICSTR;


// for PAINTING
// ============
static bool AIC_chains_valid = false;
static int AIC_chain_count = 0;
static vRCP *AIC_chains[MY_MX_CHAINS] = {0};

int Get_AIC_chain_count() { return AIC_chain_count; }
vRCP **Get_AIC_chains() { return AIC_chains; }
bool are_AIC_chains_valid() { return AIC_chains_valid; }
bool validate_AIC_chains( bool b )
{
    bool curr = AIC_chains_valid;
    AIC_chains_valid = b;
    return curr;
}
void Kill_AIC_chains()
{
    int i, max = AIC_chain_count;
    AIC_chain_count = 0;
    AIC_chains_valid = false;
    for (i = 0; i < max; i++) {
        vRCP *pnxt = AIC_chains[i];
        pnxt->clear();
        delete pnxt;
        AIC_chains[i] = 0;
    }
}

#define int_in_vint Int_in_iVector

static int add_debug_aic2 = 0;
static int max_links_in_chain = 20;    // This is a little abitrary
static bool show_all_cands = false;
static bool start_strong_link = true;
static size_t min_candidates = 3;
static BOOL g_bNeverStartSameCell = TRUE;
static BOOL gbSkipSRCB = TRUE; // skip chaining by the same ROW, COL, or BOX

static AICSTR _s_aicstr;

int stop_here()
{
    int i;
    i = 0;
    return i;
}

char *get_AIC_Color_type( uint64_t flag )
{
    uint64_t flg = (flag & cf_XAB);
    if (flg == cf_XAB)
        return "?";
    if (flg & cf_XCA)
        return "+";
    if (flg & cf_XCB)
        return "-";
    return "U";
}


void Append_AIC_RCPAIR_Stg(char *tb, PRCPAIR prcp, int all = 3);
void Append_AIC_RCPAIR_Stg(char *tb, PRCPAIR prcp, int all)
{
    PROWCOL prc1, prc2;
    int setval1, setval2;
    uint64_t flg1, flg2;
    char *clrtyp1, *clrtyp2;
    char *err = "";

    if (all & 1) {
        prc1 = &prcp->rowcol[0];
        setval1 = prc1->cnum;
        // get flag
        flg1 = 0;
        if (VALUEVALID(setval1)) {
            flg1 = prc1->set.flag[setval1 - 1] & cf_XAB;
        } else {
            err = "SV1?";
            strcat(tb,err);
        }
        // get AIC color type
        clrtyp1 = get_AIC_Color_type(flg1);
        sprintf(EndBuf(tb), "%s%d[%c%d]", clrtyp1, setval1, (char)(prc1->row + 'A'), prc1->col + 1);
    }

    if (all & 2) {
        prc2 = &prcp->rowcol[1];
        // get setval
        setval2 = prc2->cnum;
        flg2 = 0;
        if (VALUEVALID(setval2)) {
            flg2 = prc2->set.flag[setval2 - 1] & cf_XAB;
        } else {
            err = "SV2?";
            strcat(tb,err);
        }
        clrtyp2 = get_AIC_Color_type(flg2);
        sprintf(EndBuf(tb), "%s%d[%c%d]", clrtyp2, setval2, (char)(prc2->row + 'A'), prc2->col + 1);
    }

}

char *Get_AIC_RCPAIR_Stg(PRCPAIR prcp)
{
    char *tb = GetNxtBuf();
    *tb = 0;
    Append_AIC_RCPAIR_Stg( tb, prcp );
    return tb;
}

#define RECTWIDTH(r) (r.right - r.left)
#define RECTHEIGHT(r) (r.bottom - r.top)

void Pnt_Chain( vRCP *pvrcp )
{
#ifdef WIN32    // windows HDC painting
    HDC hdc = GetDC(g_hWnd);
    HANDLE bitmap = 0;
    bool use_copy = false;
    if (hdc) {
        RECT rc;
        GetClientRect(g_hWnd,&rc);
        HDC hdcmem = CreateCompatibleDC(hdc);
        if( hdcmem ) {
            bitmap = CreateCompatibleBitmap( hdc, RECTWIDTH(rc), RECTHEIGHT(rc) );
            if( bitmap ) {
                SelectObject(hdcmem,bitmap);
                BitBlt(hdcmem,0,0,RECTWIDTH(rc),RECTHEIGHT(rc),hdc,0,0,SRCCOPY);
                use_copy = true;
            }
        }
        Paint_One_AIC_Chain( hdc, pvrcp, 0, 0 ); // paint to real hdc
        if (use_copy) // restore it
            BitBlt(hdc,0,0,RECTWIDTH(rc),RECTHEIGHT(rc),hdcmem,0,0,SRCCOPY);
        ReleaseDC(g_hWnd,hdc);
    }
#else // !WIN32 - TODO: alternative sceen painting
    // TODO: paint a chain to HDC
#endif // WIN32 y/n
}

void Store_AIC_Chain( PAICSTR paic )
{
    PABOX2 pb = paic->pb;
    vRCP *pvrcp = paic->pvrcp;
    PRCRCB prcrcb = paic->prcrcb;
    int links = paic->links;
    size_t max = pvrcp->size();
    if (!max)
        return;
    if (AIC_chain_count >= MY_MX_CHAINS) {
        sprtf("Chain count exceeds MY_MX_CHAINS %d! RECOMPILE!!\n", MY_MX_CHAINS);
        paic->exit_scan = true;
        return;
    }
    char *tb = GetNxtBuf();
    AIC_chains[AIC_chain_count] = new vRCP;
    vRCP *pnext = AIC_chains[AIC_chain_count];
    size_t ii;
    PRCPAIR prcp;
    int all = 3; // on first show 1st and second
    sprintf(tb,"AIC Chain: %d ", (int)max);
    for (ii = 0; ii < max; ii++) {
        prcp = &pvrcp->at(ii);
        pnext->push_back(*prcp);
        //all = 3;
        //all = (ii == 0) ? 3 : 2;
        Append_AIC_RCPAIR_Stg(tb, prcp, all);
        all = 2; // on 2nd and all other show only 2nd
    }
    OUTITAIC(tb);
    //if (add_debug_aic) Pnt_Chain(pnext);
    AIC_chain_count++;
}


void Check_AIC_Chain( PAICSTR paic )
{
    PABOX2 pb     = paic->pb;
    vRCP *pvrcp   = paic->pvrcp;
    PRCRCB prcrcb = paic->prcrcb;
    size_t max, ii;
    size_t max2, m2;
    PRCPAIR prcp;
    PROWCOL prc, prc1, prc2;
    int setval1, setval2, lval;
    vRC *pvrc;
    vRC elim, elimby;
    SET set;
    char *tb = GetNxtBuf();
    max = pvrcp->size();

    ZERO_SET(&set);
    for (ii = 0; ii < max; ii++) {
        prcp = &pvrcp->at(ii);
        prc1 = &prcp->rowcol[0];
        prc2 = &prcp->rowcol[1];
        setval1 = prc1->cnum;
        setval2 = prc2->cnum;
        if (!VALUEVALID(setval1)) {
            stop_here();
            continue;   // this would be an ERROR
        }
        if (!VALUEVALID(setval2)) {
            stop_here();
            continue;   // this would be an ERROR;
        }

        // fill in the set
        set.val[setval1 - 1] = setval1;
        set.val[setval2 - 1] = setval2;

        if ( !(setval1 == setval2) ) continue;
        lval = setval1 - 1;
        if (prc1->box == prc2->box) {
            pvrc = &prcrcb->vboxs[prc1->box];
            max2 = pvrc->size();
            for (m2 = 0; m2 < max2; m2++) {
                prc = &pvrc->at(m2);
                if (SAMERCCELL(prc,prc1)) continue;
                if (SAMERCCELL(prc,prc2)) continue;
                if ( !(prc->set.val[lval]) ) continue;
                if (prc_in_chain(prc,&elim)) continue;
                prc->cnum = setval1;
                elim.push_back(*prc);
                elimby.push_back(*prc1);
                elimby.push_back(*prc2);
            }
        }
        if (prc1->row == prc2->row) {
            pvrc = &prcrcb->vrows[prc1->row];
            max2 = pvrc->size();
            for (m2 = 0; m2 < max2; m2++) {
                prc = &pvrc->at(m2);
                if (SAMERCCELL(prc,prc1)) continue;
                if (SAMERCCELL(prc,prc2)) continue;
                if ( !(prc->set.val[lval]) ) continue;
                if (prc_in_chain(prc,&elim)) continue;
                prc->cnum = setval1;
                elim.push_back(*prc);
                elimby.push_back(*prc1);
                elimby.push_back(*prc2);
            }
        } else if (prc1->col == prc2->col) {
            pvrc = &prcrcb->vcols[prc1->col];
            max2 = pvrc->size();
            for (m2 = 0; m2 < max2; m2++) {
                prc = &pvrc->at(m2);
                if (SAMERCCELL(prc,prc1)) continue;
                if (SAMERCCELL(prc,prc2)) continue;
                if ( !(prc->set.val[lval]) ) continue;
                if (prc_in_chain(prc,&elim)) continue;
                prc->cnum = setval1;
                elim.push_back(*prc);
                elimby.push_back(*prc1);
                elimby.push_back(*prc2);
            }
        }
    }

    lval = Get_Set_Cnt(&set);
    max = elim.size();
    max2 = elimby.size();
    m2 = 0;
    if (max) {
        if (reject_chains_of_one_value && (lval == 1))
            max = 0;
    }
    for (ii = 0; ii < max; ii++) {
        prc = &elim[ii];
        lval = prc->cnum - 1;
        if (pb->line[prc->row].set[prc->col].flag[lval] & aic_elim) continue;
        pb->line[prc->row].set[prc->col].flag[lval] |= aic_elim;
        m2 = ii * 2;
        prc1 = &elimby[m2 + 0];
        prc2 = &elimby[m2 + 1];
        pb->line[prc1->row].set[prc1->col].flag[lval] |= aic_memb;
        pb->line[prc2->row].set[prc2->col].flag[lval] |= aic_memb;
        paic->elim_count++;
        sprintf(tb,"AIC-Elim: %s by %s & %s ",
            Get_RC_setval_RC_Stg(prc,prc->cnum),
            Get_RC_setval_RC_Stg(prc1,prc->cnum),
            Get_RC_setval_RC_Stg(prc2,prc->cnum) );
        OUTITAIC(tb);
        ///////////////////////////////////////////////////////////////////////////////////////////
        // CHEAT A LITTLE - If we HAVE a UNIQUE solution, check this marked for deletion candidte
        ///////////////////////////////////////////////////////////////////////////////////////////
        int val2 = get_solution_value(prc->row,prc->col);
        if (val2 && (val2 == prc->cnum)) {
            sprtf("Warning: AIC: Candidate %s marked for deletion!\n", Get_RC_setval_RC_Stg(prc,val2));
            if (treat_aic_warn_as_error) {
                paic->exit_scan = true;
                paic->had_error = true;
                paic->elim_count = 0;
                break;
            }
        }
    }

    if (paic->elim_count) {
        Store_AIC_Chain(paic);
        if (only_one_aic)
            paic->exit_scan = true;
    }

}


bool is_valid_set( PSET ps )
{
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps->val[i];
        if (!val) continue;
        if (!VALUEVALID(val))
            return false;
    }
    return true;
}

bool is_valid_prc_setval( PROWCOL prc, int setval )
{
    if (!VALIDRC(prc->row,prc->col))
        return false;
    if (VALUEVALID(setval) && !(prc->set.flag[setval-1] & cf_XAB)) {
        return false; // NO COLOR FLAG
    }
    return is_valid_set( &prc->set );
}

int AIC_Follow_Chain( PAICSTR paic );

// Does the line prcA->row --- prcB->row intersect any current column in chain
// or does line  prcA->col --- prcB->col intersect any current row in the chain
bool prc12_cross_chain(PROWCOL prcA, PROWCOL prcB, vRCP *pvrcp)
{
    size_t ii;
    PRCPAIR prcp;
    PROWCOL prc1, prc2;
    int colA, colB;
    int rowA, rowB;
    size_t max = pvrcp->size();
    if (prcA->row == prcB->row) {
        if (prcA->col < prcB->col) {
            colA = prcA->col;   // get min COL
            colB = prcB->col;
        } else {
            colA = prcB->col;   // get min COL
            colB = prcA->col;
        }
        for (ii = 0; ii < max; ii++) {
            prcp = &pvrcp->at(ii);
            prc1 = &prcp->rowcol[0];
            prc2 = &prcp->rowcol[1];
            if (prc1->col == prc2->col) {
                if ((prc1->col > colA) && (prc1->col < colB))
                    return true;    // they intersect
            }
        }
    } else if (prcA->col == prcB->col) {
        if (prcA->row < prcB->row) {
            rowA = prcA->row;   // get min ROW
            rowB = prcB->row;
        } else {
            rowA = prcB->row;   // get min ROW
            rowB = prcA->row;
        }
        for (ii = 0; ii < max; ii++) {
            prcp = &pvrcp->at(ii);
            prc1 = &prcp->rowcol[0];
            prc2 = &prcp->rowcol[1];
            if (prc1->row == prc2->row) {
                if ((prc1->row > rowA) && (prc2->row < rowB))
                    return true;
            }
        }
    }
    return false;
}

void AIC_Scan_For_Next( PAICSTR paic )
{
    vRC *pvrc;
    size_t max, ii;
    PROWCOL prc2;
    //PROWCOL prclast = paic->prclast;
    PROWCOL prclocal;
    vRCP *pvrcp;
    int setval, lval, links;
    uint64_t nflg, clrflg, flg2, sl_flag;
    PROWCOL prcl = &paic->rcLast;
    ROWCOL rc, rclast;
    RCPAIR rcpair;
    PABOX2 pb = paic->pb;
    PRCRCB prcrcb = paic->prcrcb;
    bool is_box;
    vINT *pvint = 0;
    int row, col, box;
    int itest = 0;

    char *tb = GetNxtBuf();

    if (paic->exit_scan) return;

    links  = paic->links;
    setval = paic->setval;
    pvrcp  = paic->pvrcp;
    clrflg = paic->clrflg;  // color for NEXT
    flg2   = paic->dirflg;  // extract last 'direction' flag
    pvrc   = paic->pvrc;    // vRC * to scan
    lval   = setval - 1;
    is_box = (flg2 & cl_LBB) ? true : false;

    rclast = *prcl;     // copy last to LOCAL memory
    rc = rclast;
    prclocal = &rc;  // ptr to last (local memory) to be modified
    row = rc.row;
    col = rc.col;
    box = rc.box;

    //sprintf(tb,"AIC BgnS: %s lks %d ", Get_RC_Stg(&rclast,setval), links);
    //OUTITAIC(tb);
    if (!is_valid_prc_setval(&rclast,setval)) {
        if (add_debug_aic) {
            sprtf("WARNING: AIC Scan Next called with INVALID PROWCOL! [%s]\n",
                Get_RC_setval_RC_Stg(&rclast));
        }
        return;
    }

    if (flg2 & cl_LBR) {
        pvint = paic->pvirow;
        if ( gbSkipSRCB && int_in_vint(row, pvint) ) {
            if (add_debug_aic2) sprtf("Skip same ROW %d\n", row + 1);
            return;
        }
        itest = row;
    } else if (flg2 & cl_LBC) {
        pvint = paic->pvicol;
        if ( gbSkipSRCB && int_in_vint(col, pvint) ) {
            if (add_debug_aic2) sprtf("Skip same COL %d\n", col + 1);
            return;
        }
        itest = col;
    } else if (flg2 & cl_LBB) {
        pvint = paic->pvibox;
        if ( gbSkipSRCB && int_in_vint(row, pvint) ) {
            if (add_debug_aic2) sprtf("Skip same BOX %d\n", box + 1);
            return;
        }
        itest = box;
    }

    max = pvrc->size();     // get size
    for (ii = 0; ii < max; ii++) {
        prc2 = &pvrc->at(ii);       // get next ROW, COL, or BOX
        if (SAMERCCELL(prc2,prclocal)) continue; // skip LAST
        if (prc_in_pvrcp(prc2,pvrcp)) continue; // skip if already in CHAIN
        if ( !(prc2->set.val[lval]) ) continue; // skip if it does NOT have this value
        if ( !is_box && (prc2->box == prclocal->box)) continue; // let the BOX scan do this
        if ( !is_box && reject_chains_that_loop_over && prc12_cross_chain(prclocal,prc2,pvrcp)) continue;

        *prcl = *prc2;          // set NEW last
        prcl->cnum = setval;
        prcl->box  = GETBOX(prcl->row,prcl->col);

        sl_flag = 0; // clear STRONG LINK flag
        if ( flg2 & (cl_LBC|cl_LBR) ) { // if linked by COL or ROW, exclude BOX
            sl_flag = Get_RC_Strong_Link_Flag( pb, &rc, prcl, setval, false );
        } else if (flg2 & cl_LBB) { // if linked by BOX, exclude ROW COL
            sl_flag = Get_RC_Strong_Link_Flag( pb, &rc, prcl, setval, true );
            sl_flag &= ~(cl_LBC|cl_LBR); // remove COL ROW SL
        }

        // adjust previous last
        rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage - keep color
        rc.set.flag[lval]  |= (flg2 | sl_flag); // add new linkage flag
        
        // adjust new last
        nflg = prcl->set.flag[lval];     // get FLAG
        nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
        nflg |= (flg2 | clrflg | sl_flag);  // add linkage and opp. color and any STRONG LINK
        prcl->set.flag[lval] = nflg;     // set FLAG

        rcpair.rowcol[0] = rc;      // copy previous (adjusted) last
        rcpair.rowcol[1] = *prcl;   // copy new last in chain
        pvrcp->push_back(rcpair);   // and store
        sprintf(tb,"AIC Cont: %s fm %s on sv %d to %s lev %d ",paic->scan, Get_RC_Stg(&rclast,setval),
            setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), links);
        OUTITAIC(tb);

        if (pvint) pvint->push_back(itest); // store this LINK

        paic->links = links + 1;

        AIC_Follow_Chain( paic );

        pvrcp->pop_back();      // kick last off CHAIN
        if (paic->exit_scan) break;
    }   // for (ii = 0; ii < max; ii++)

    //sprintf(tb,"AIC EndS: %s lks %d ", Get_RC_Stg(&rclast,setval), links);
    //OUTITAIC(tb);
}


void AIC_Close_Chain( PAICSTR paic )
{
    uint64_t flg;
    RCPAIR rcpair;
    vRCP *pvrcp = paic->pvrcp;
    ROWCOL rc1  = paic->rcLast;
    ROWCOL rc2  = paic->rcFirst;
    int lval    = paic->setval - 1;
    int links   = paic->links;
    int setval  = paic->setval;
    char *tb = GetNxtBuf();

    rc1.set.flag[lval] &= ~(cl_LBA|cl_SLA); // take OUT linkage
    rc2.set.flag[lval] &= ~(cl_LBA|cl_SLA); // take OUT linkage
    if (rc1.box == rc2.box)
        flg = cl_LBB;
    else if (rc1.row == rc2.row) 
        flg = cl_LBR;
    else if (rc1.col == rc2.col)
        flg = cl_LBC;
    else
        return;
    rc1.set.flag[lval] |= flg; // add linkage
    rc2.set.flag[lval] |= flg; // add linkage

    rcpair.rowcol[0] = rc1;
    rcpair.rowcol[1] = rc2;
    pvrcp->push_back(rcpair);
    paic->links = links + 1;
    sprintf(tb,"AIC Final: close sv %d to %s lev %d ",
            setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), links);
    OUTITAIC(tb);
    Check_AIC_Chain( paic );
    pvrcp->pop_back();  // drop this LAST link
}

static int add_debug_aic3 = 1;
// This scan introduces an extended use of ROWCOL
// where the candidate value is also known... so the cnum member of ROWCOL
// with be used for this...
int AIC_Follow_Chain( PAICSTR paic )
{
    int count = 0;
    size_t max;
    //PRCPAIR prcplast;    // get LAST PAIR in CHAIN
    PROWCOL prcLast;     // ptr to struct rcLast
    int setval;      // get last CANDIDATE
    int lval, scnt;
    uint64_t flag; // and last LINK flag
    uint64_t dir;        // last direction travelled
    uint64_t clr;        // and ON/OFF color
    uint64_t flg, flg2, nflg;
    ROWCOL rc1, rc2, rclast;
    RCPAIR rcpair;
    vRC *pvrow, *pvcol, *pvbox;
    //size_t ii;
    //PRCPAIR prcp2;
    PROWCOL prc2;
    int setval2, lval2;
    int row, col, box;

    PABOX2 pb     = paic->pb;
    vRCP *pvrcp   = paic->pvrcp;
    PRCRCB prcrcb = paic->prcrcb;
    int links     = paic->links;
    char *tb = GetNxtBuf();

    if (paic->exit_scan) return count;

    max = pvrcp->size();
    if (!max) return 0;

    //prcplast = &pvrcp->at(max-1);    // get LAST PAIR in CHAIN
    //prclast = &prcplast->rowcol[1];  // last CELL
    prcLast = &paic->rcLast;        // get LAST in CHAIN
    setval = prcLast->cnum;         // get last CANDIDATE
    rclast = paic->rcLast;          // local copy of LAST
    lval = setval - 1;
    flag = prcLast->set.flag[lval]; // and last LINK flag
    // dir = flag & cl_LBA;        // last direction travelled
    clr = flag & cf_XAB;            // and ON/OFF color
    dir = paic->dirflg;            // last DIRECTION

    if (!is_valid_prc_setval(&rclast,setval)) {
        if (add_debug_aic) {
            sprtf("WARNING: AIC Follow Chain called with INVALID PROWCOL! [%s]\n",
                Get_RC_setval_RC_Stg(&rclast));
        }
        return 0;
    }
    if (links > max_links_in_chain) {
        return 0;
    }
    if (links > 2) {
        // can maybe close this chain back to first
        // prcp2 = &pvrcp->at(0);    // get FIRST PAIR in CHAIN
        // prc2 = &prcp2->rowcol[0];  // last FIRST CELL
        prc2 = &paic->rcFirst;
        setval2 = prc2->cnum;         // get first CANDIDATE
        // can ONLY close if SAME candidate
        if ((setval2 == setval) && 
            ((prc2->row == prcLast->row)||(prc2->col == prcLast->col)||(prc2->box == prcLast->box))) {
            // YOW a CLOSED CHAIN - Now ONLY interest is does it ELIMINATE anything
            AIC_Close_Chain( paic );
            max = pvrcp->size();
            if (!max) return 0;
            return count;       // back out of this CLOSE, and go NEW direction
        }

    }

    if (paic->exit_scan) return count;

    // set OPPOSITE color
    flg = FLIP_COLOR(clr);
    if (add_debug_aic & !flg) {
        sprtf("WARNING: COLOR flag is NULL\n");
    }
    paic->clrflg  = clr;
    paic->links   = links + 1;
    paic->setval  = setval;
    //paic->prclast = prclast;
    scnt          = Get_Set_Cnt(&prcLast->set);
    rc1 = rclast;        // copy last to local memory to modify
    row = rc1.row;
    col = rc1.col;
    box = rc1.box;
    pvrow = &prcrcb->vrows[row];
    pvcol = &prcrcb->vcols[col];
    pvbox = &prcrcb->vboxs[box];

    //sprintf(tb,"AIC Enter %s dep %d ", Get_RC_Stg(&rclast,setval), links);
    //OUTITAIC(tb);

#ifdef DO_CELL_VALUES_FIRST
    // if last CHAIN direction was NOT SAME CELL, 
    // AND this LAST CELL NOT already in CHAIN twice
    // AND only if we can establish an ON/OFF situation - 
    // that is there are only TWO candidates
    if ( !(dir & cl_LSS) && (scnt == 2) && !prc_in_pvrcp(prcLast,pvrcp,1) ) {
        int i, cnt, setvals[9];
        cnt = Get_Set_Cnt2(&rc1.set,setvals);
        flg2 = cl_LSS;
        for (i = 0; i < cnt; i++) {
            setval2 = setvals[i];
            if (!VALUEVALID(setval2)) continue; // This would be an ERROR
            if (setval2 == setval) continue;
            lval2 = setval2 - 1;
            //rc2.row = prcLast->row;
            //rc2.col = prcLast->col;
            //COPY_SET(&rc2.set,&prcLast->set);
            rc2 = rclast;
            rc2.cnum = setval2; // set NEW setval
            rc2.box  = GETBOX(rc2.row,rc2.col);
            // fix flags for this new PAIR, with DIFFERENT setvals
            rc1.set.flag[lval] &= ~(cl_LBA|cl_SLA); // take OUT linkage
            rc1.set.flag[lval] |= flg2;    // add in new linkage
            // rc keep color
            nflg = rc2.set.flag[lval2];
            nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg2 | flg); // add same cell linkage and OPPOSITE color
            rc2.set.flag[lval2] = nflg; // add OPPOSITE color
            rcpair.rowcol[0] = rc1;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            sprintf(tb,"AIC Cont: SS fm %s sv %d to ",Get_RC_Stg(&rclast,setval), setval);
            Append_RC_Stg(tb,&rc1,setval);
            strcat(tb,"-");
            Append_RC_Stg(tb,&rc2,setval2);
            sprintf(EndBuf(tb)," depth %d", links);
            OUTITAIC(tb);
            paic->links  = links + 1;
            paic->dirflg = flg2;
            paic->clrflg = flg; // set NEXT color
            paic->setval = setval2;
            //if (setval2 == 9)
            //    stop_here();
            paic->rcLast = rc2; // update LAST
            AIC_Follow_Chain( paic );
            if (add_debug_aic3) {
                rcpair = pvrcp->back();
                //sprintf(tb,"AIC Drop: SS ",Get_RC_Stg(&rcpair.rowcol[0],setval), setval);
                strcpy(tb,"AIC Drop: SS ");
                Append_RC_Stg(tb,&rcpair.rowcol[0],setval);
                strcat(tb,"-");
                Append_RC_Stg(tb,&rcpair.rowcol[1],setval2);
                sprintf(EndBuf(tb)," depth %d", links);
                OUTITAIC(tb);
            }
            pvrcp->pop_back();
            paic->rcLast = rclast; // restore LAST start
            if (paic->exit_scan) return count;
        }
        rc1 = rclast;        // retore last to local memory to modify
    }

#endif // DO_CELL_VALUES_FIRST
    paic->setval = setval;
    if (dir & cl_LBR) {
        // can now try COL and BOX
        // COL scan
        if ( gbSkipSRCB && int_in_vint(col, paic->pvicol) ) {
            if (add_debug_aic2) sprtf("Skipped same COL %d R\n", col + 1);
        } else {
            sprintf(tb,"R-SC%d", col + 1);
            paic->dirflg = cl_LBC;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvcol;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
        // BOX scan
        if ( gbSkipSRCB && int_in_vint(box, paic->pvibox) ) {
            if (add_debug_aic2) sprtf("Skipped same BOX %d R\n", box + 1);
        } else {
            sprintf(tb,"R-SB%d", box + 1);
            paic->dirflg = cl_LBB;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvbox;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
    } else if (dir & cl_LBC) {
        // can now try ROW and BOX
        // ROW scan
        if ( gbSkipSRCB && int_in_vint(row, paic->pvirow) ) {
            if (add_debug_aic2) sprtf("Skipped same ROW %d C\n", row + 1);
        } else {
            sprintf(tb,"C-SR%d", row + 1);
            paic->dirflg = cl_LBR;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvrow;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
        // BOX scan
        if ( gbSkipSRCB && int_in_vint(box, paic->pvibox) ) {
            if (add_debug_aic2) sprtf("Skipped same BOX %d C\n", box + 1);
        } else {
            sprintf(tb,"C-SB%d", box + 1);
            paic->dirflg = cl_LBB;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvbox;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
    } else if (dir & cl_LBB) {
        // can now try ROW and COL
        // ROW scan
        if ( !(gbSkipSRCB && int_in_vint(row, paic->pvirow)) ) {
            sprintf(tb,"B-SR%d", row + 1);
            paic->dirflg = cl_LBR;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvrow;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
        // COL scan
        if ( !(gbSkipSRCB && int_in_vint(col, paic->pvicol)) ) {
            sprintf(tb,"B-SC%d", col + 1);
            paic->dirflg = cl_LBC;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvcol;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
    } else if (dir & cl_LSS) {
        // can now try ROW, COL or BOX
        // ROW scan
        if ( gbSkipSRCB && int_in_vint(row, paic->pvirow) ) {
            if (add_debug_aic2) sprtf("Skipped same ROW %d S\n", row + 1);
        } else {
            sprintf(tb,"C-SR%d", row + 1);
            paic->dirflg = cl_LBR;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvrow;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
        // COL scan
        if ( gbSkipSRCB && int_in_vint(col, paic->pvicol) ) {
            if (add_debug_aic2) sprtf("Skipped same COL %d S\n", col + 1);
        } else {
            sprintf(tb,"C-SC%d", col + 1);
            paic->dirflg = cl_LBC;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvcol;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
        // BOX scan
        if ( gbSkipSRCB && int_in_vint(box, paic->pvibox) ) {
            if (add_debug_aic2) sprtf("Skipped same BOX %d S\n", box + 1);
        } else {
            sprintf(tb,"C-SB%d", box + 1);
            paic->dirflg = cl_LBB;
            paic->clrflg = flg; // set NEXT color
            paic->pvrc   = pvbox;
            paic->scan   = tb;
            AIC_Scan_For_Next( paic );
            paic->rcLast = rclast; // restore LAST start
        }
        if (paic->exit_scan) return count;
    }

    if (paic->exit_scan) return count;

    // and always can try OTHER candidates in this cell
    // if LAST linkage was NOT SAME CELL
    // ================================================
#ifndef DO_CELL_VALUES_FIRST
    // if last CHAIN directorion was NOT SAME CELL, 
    // AND this LAST CELL NOT already in CHAIN twice
    if ( !(dir & cl_LSS) && (scnt == 2) && !prc_in_pvrcp(prclast,pvrcp,1) ) {
        int i, cnt, setvals[9];
        cnt = Get_Set_Cnt2(&rc1.set,setvals);
        flg2 = cl_LSS;
        for (i = 0; i < cnt; i++) {
            setval2 = setvals[i];
            if (!VALUEVALID(setval2)) continue; // This would be an ERROR
            if (setval2 == setval) continue;
            lval2 = setval2 - 1;
            rc1 = *prclast;
            rc2.row = prclast->row;
            rc2.col = prclast->col;
            COPY_SET(&rc2.set,&prclast->set);
            rc2.cnum = setval2; // set NEW setval
            rc2.box  = GETBOX(rc2.row,rc2.col);
            // fix flags for this new PAIR, with DIFFERENT setvals
            rc1.set.flag[lval] &= ~(cl_LBA|cl_SLA); // take OUT linkage
            rc1.set.flag[lval] |= flg2;    // add in new linkage
            // rc keep color
            nflg = rc2.set.flag[lval2];
            nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg2 | flg); // add same cell linkage and OPPOSITE color
            rc2.set.flag[lval2] = nflg; // add OPPOSITE color
            rcpair.rowcol[0] = rc1;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            //sprintf(tb,"AIC Cont: SS %d %s %d %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), setval2, links);
            sprintf(tb,"AIC Cont: SS setval %d ",setval);
            Append_RC_Stg(tb,&rc1,setval);
            strcat(tb,"-");
            Append_RC_Stg(tb,&rc2,setval2);
            sprintf(EndBuf(tb)," depth %d", links);
            OUTITAIC(tb);
            paic->links = links + 1;
            paic->dirflg = flg2;
            paic->rcLast = rc2;
            AIC_Follow_Chain( paic );
            pvrcp->pop_back();
            paic->rcLast = rclast; // restore LAST start
            if (paic->exit_scan) return count;
        }
    }
#endif // #ifndef DO_CELL_VALUES_FIRST

    //sprintf(tb,"AIC Exit  %s dep %d ", Get_RC_Stg(&rclast,setval), links);
    //OUTITAIC(tb);

    return count;
}

int Do_Clear_AIC(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, aic_elim, aic_memb, "AIC", "AIC-Scan", 0, false );
    AIC_chains_valid = false;
    return count;
}

int Do_Fill_AIC(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, aic_elim, aic_memb, "AIC", "AIC-Scan", 0, true );
    if (count)
        pb->iStage = sg_Begin;
    AIC_chains_valid = false;
    return count;
}


#define CLRVINTS { virow.clear(); vicol.clear(); vibox.clear(); }
#define CLRPVINTS { paic->pvirow->clear(); paic->pvicol->clear(); paic->pvibox->clear(); }

int Process_AIC_Box( PAICSTR paic )
{
    int count = 0;
    int row, col, box, val;
    PABOX2 pb = paic->pb;
    int cnt, lval, i;
    int setvals1[9];
    PSET ps, ps2;
    vRC *pvrow, *pvcol, *pvbox;
    size_t max, ii, max2;
    PROWCOL prc;
    uint64_t sl_flag;
    PROWCOL prcf = &paic->rcFirst;
    PROWCOL prcl = &paic->rcLast;
    PRCRCB prcrcb = paic->prcrcb;
    vRCP *pvrcp = paic->pvrcp;
    RCPAIR rcp;
    PRCPAIR prcp;
    int i2, val2;
    char *tb = GetNxtBuf();

    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            // got a set of candidates to chain from...
            box = GETBOX(row,col);
            prcf->row = row;
            prcf->col = col;
            prcf->box = box;
            COPY_SET(&prcf->set,ps);
            cnt = Get_Set_Cnt2(ps,setvals1);
            for (i = 0; i < cnt; i++) {
                val = setvals1[i];
                prcf->cnum = val;   // set setval of first
                lval = val - 1;
                // got FIRST member of CHAIN
                if (paic->exit_scan) break;

                if (test_start_cand && (val != test_start_cand)) continue; // DIAG - Start with THIS candidate

                // c@RC - reach out in SAME ROW
                pvrow = &prcrcb->vrows[prcf->row];
                max = pvrow->size();
                if (!start_strong_link && min_candidates && (max < min_candidates)) goto Do_Cols; // first LINK must have 3 in unit
                for (ii = 0; ii < max; ii++) {
                    prc = &pvrow->at(ii);
                    if (prc->col == col) continue; // skip self
                    ps2 = &pb->line[row].set[prc->col];
                    if ( !(ps2->val[lval]) ) continue; // does NOT hold candidate
                    // got 2nd member in chain - with a strong or weak link to 1st
                    prcl->row = row;
                    prcl->col = prc->col;
                    prcl->box = GETBOX(row,prc->col);
                    if (prcf->box == prcl->box) continue; // let BOX scan do this

                    sl_flag = Get_RC_Strong_Link_Flag( pb, prcf, prcl, val, false ); // exclude BOX
                    if (start_strong_link && !sl_flag) continue;

                    COPY_SET(&prcl->set,ps2);
                    prcl->cnum = val;   // set setval of second

                    // got a PAIR, linked by ROW
                    prcf->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color
                    prcl->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color

                    prcf->set.flag[lval]  |= (cf_XCA | cl_LBR | sl_flag);
                    prcl->set.flag[lval]  |= (cf_XCB | cl_LBR | sl_flag);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = *prcl;
                    pvrcp->clear();
                    pvrcp->push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = pvrcp->size();
                    prcp = &pvrcp->at(max2 - 1);
                    sprintf(tb,"AIC Begin: SR %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LBR;
                    paic->pvirow->push_back(row);
                    AIC_Follow_Chain( paic );
                    pvrcp->clear();
                    CLRPVINTS;
                    if (paic->exit_scan) break;
                }   // for (ii = 0; ii < max; ii++) 

Do_Cols:
                if (paic->exit_scan) break;
                // c@RC - reach out in SAME COL
                pvcol = &prcrcb->vcols[prcf->col];
                max = pvcol->size();
                if (!start_strong_link && min_candidates && (max < min_candidates)) goto Do_Box; // first LINK must have 3 in unit
                for (ii = 0; ii < max; ii++) {
                    prc = &pvcol->at(ii);
                    if (prc->row == row) continue; // skip self
                    ps2 = &pb->line[prc->row].set[col];
                    if ( !(ps2->val[lval]) ) continue; // does NOT hold candidate
                    // got 2nd member in chain - with a strong or weak link to 1st
                    prcl->row = prc->row;
                    prcl->col = col;
                    prcl->box = GETBOX(prc->row,col);
                    if (prcf->box == prcl->box) continue; // let BOX scan do this

                    sl_flag = Get_RC_Strong_Link_Flag( pb, prcf, prcl, val, false ); // exclude BOX
                    if (start_strong_link && !sl_flag) continue;

                    COPY_SET(&prcl->set,ps2);
                    prcl->cnum = val;
                    // got a PAIR, linked by ROW
                    prcf->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color
                    prcl->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color
                    prcf->set.flag[lval]  |= (cf_XCA | cl_LBC | sl_flag);
                    prcl->set.flag[lval]  |= (cf_XCB | cl_LBC | sl_flag);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = *prcl;
                    pvrcp->clear();
                    pvrcp->push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = pvrcp->size();
                    prcp = &pvrcp->at(max2 - 1);
                    sprintf(tb,"AIC Begin: SC %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LBC;
                    paic->pvicol->push_back(col);
                    AIC_Follow_Chain( paic );
                    pvrcp->clear();
                    CLRPVINTS;
                    if (paic->exit_scan) break;
                }   // for (ii = 0; ii < max; ii++) 

Do_Box:
                if (paic->exit_scan) break;
                // c@RC - reach out in SAME BOX
                pvbox = &prcrcb->vboxs[prcf->box];
                max = pvbox->size();
                if (!start_strong_link && min_candidates && (max < min_candidates)) goto Do_SCell; // first LINK must have 3 in unit

                for (ii = 0; ii < max; ii++) {
                    prc = &pvbox->at(ii);
                    if ((prc->col == col)&&(prc->row == row)) continue; // skip self
                    ps2 = &pb->line[prc->row].set[prc->col];
                    if ( !(ps2->val[lval]) ) continue; // does NOT hold candidate
                    // got 2nd member in chain - with a strong or weak link to 1st
                    prcl->row = prc->row;
                    prcl->col = prc->col;
                    prcl->box = GETBOX(prc->row,prc->col);

                    sl_flag = Get_RC_Strong_Link_Flag( pb, prcf, prcl, val );
                    sl_flag &= ~(cl_SLR | cl_SLC); // remove COL and ROW SL
                    if (start_strong_link && !sl_flag) continue;

                    COPY_SET(&prcl->set,ps2);
                    prcl->cnum = val;
                    // got a PAIR, linked by ROW
                    prcf->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color
                    prcl->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color
                    prcf->set.flag[lval] |= (cf_XCA | cl_LBB | sl_flag);
                    prcl->set.flag[lval] |= (cf_XCB | cl_LBB | sl_flag);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = *prcl;
                    pvrcp->clear();
                    pvrcp->push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = pvrcp->size();
                    prcp = &pvrcp->at(max2 - 1);
                    sprintf(tb,"AIC Begin: SB %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LBB;
                    paic->pvibox->push_back(box);
                    AIC_Follow_Chain( paic );
                    pvrcp->clear();
                    CLRPVINTS;
                    if (paic->exit_scan) break;
                }   // for (ii = 0; ii < max; ii++) 

Do_SCell:
                if (paic->exit_scan) break;
                // c@RC - reach out in SAME CELL, another candidate
                if (g_bNeverStartSameCell) continue;

                for (i2 = 0; i2 < cnt; i2++) {
                    val2 = setvals1[i2];
                    if (val2 == val) continue;
                    prcl->row = prc->row;
                    prcl->col = prc->col;
                    prcl->box = GETBOX(prc->row,prc->col);

                    sl_flag = 0; // NO strong link for SAME CELL different setval
                    // if (start_strong_link && !sl_flag) continue;

                    COPY_SET(&prcl->set,ps2);
                    prcl->cnum = val;
                    // got a PAIR, linked by CELL
                    prcf->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color
                    prcl->set.flag[lval]  &= ~(cl_LBA|cl_SLA|cf_XAB); // take OUT linkage and color
                    prcf->set.flag[lval]  |= (cf_XCA | cl_LSS);
                    prcl->set.flag[lval]  |= (cf_XCB | cl_LSS);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = *prcl;
                    pvrcp->push_back(rcp); // store first PAIR A & B ON/OFF linked by CELL
                    max2 = pvrcp->size();
                    prcp = &pvrcp->at(max2 - 1);    // get pointer to LAST
                    sprintf(tb,"AIC Begin: SS %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LSS;
                    AIC_Follow_Chain( paic );
                    pvrcp->clear();
                    CLRPVINTS;
                    if (paic->exit_scan) break;
                }   // for (i2 = 0; i2 < cnt; i2++)
                if (paic->exit_scan) break;
            }   // for (i = 0; i < cnt; i++) {
            if (paic->exit_scan) break;
        }   // for (col = 0; col < 9; col++)
        if (paic->exit_scan) break;
    }   // for (row = 0; row < 9; row++) 
    return count;
}


// This scan introduces an extended use of ROWCOL
// where the candidate value is also known... so the cnum member of ROWCOL
// with be used for this...
int Mark_AIC_Scans( PABOX2 pb )
{
    int count = 0;
    vRC empty;
    RCRCB rcrcb;
    vRCP vrcp;
    PAICSTR paic = &_s_aicstr;
    vINT virow, vicol, vibox;
    int scnout = 1;
    int cnt;

    char *tb = GetNxtBuf();
    cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    if (cnt < 4)
        return 0;
    Kill_AIC_chains(); // remove any previous chains

    // setup structure
    paic->pb        = pb;
    paic->elim_count = 0;
    paic->pvrcp     = &vrcp;
    paic->prcrcb    = &rcrcb;
    paic->exit_scan = false;
    paic->had_error = false;
    paic->pvirow    = &virow;
    paic->pvicol    = &vicol;
    paic->pvibox    = &vibox;
    CLRVINTS;

    sprtf("Doing AIC scan on %d empty cells.\n",cnt);
    if (add_debug_aic) {
        scnout = add_screen_out(0);
        Show_PRCB(&rcrcb);
    }

    count = Process_AIC_Box(paic);

    if (add_debug_aic) {
         add_screen_out(scnout);
    }

    count = paic->elim_count;
    if (count) {
        AIC_chains_valid = true;
        Stack_Fill(Do_Clear_AIC);
    } else if (paic->had_error) {
        Kill_AIC_chains(); // remove any previous chains
    }
    return count;
}

int Do_AIC_Scans( PABOX2 pb )
{
    int count = 0;
    vRC empty;
    RCRCB rcrcb;
    vRCP vrcp;
    PAICSTR paic = &_s_aicstr;
    vINT virow, vicol, vibox;
    int scnout = 1;
    int cnt;

    char *tb = GetNxtBuf();
    cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    if (cnt < 4)
        return 0;
    Kill_AIC_chains(); // remove any previous chains

    // setup structure
    paic->pb        = pb;
    paic->elim_count = 0;
    paic->pvrcp     = &vrcp;
    paic->prcrcb    = &rcrcb;
    paic->exit_scan = false;
    paic->pvirow    = &virow;
    paic->pvicol    = &vicol;
    paic->pvibox    = &vibox;
    CLRVINTS;

    if (add_debug_aic) {
        sprtf("Do AIC scan on %d empty cells.\n",cnt);
        scnout = add_screen_out(0);
        Show_PRCB(&rcrcb);
    }

    count = Process_AIC_Box(paic);

    if (add_debug_aic) {
         add_screen_out(scnout);
    }
    count = paic->elim_count;

    tb = GetNxtBuf();
    if (count) {
        AIC_chains_valid = true;
        sprintf(tb,"S%d:  Elim AIC [%d] - To Fill.", pb->iStage, count);
        pb->iStage = sg_Fill_AIC;
    } else {
        sprintf(tb,"S%d:  None elim by AIC.", pb->iStage);
        if (paic->had_error) 
            sprintf(EndBuf(tb)," Note errors");
        sprintf(EndBuf(tb)," - To bgn");
        pb->iStage = sg_Begin;
    }
    OUTIT(tb);
    return count;
}

// eof - Sudo_AIC.cxx
