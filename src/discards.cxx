
// this is NOT compiled - contains code replaced by NEW code
// eventually should be DELETED!!!

#if 0 // old code - to be removed when new is working file
// ******************************************************
int Mark_Unique_Rectanges2_OK( PABOX2 pb )
{
    int count = 0;
    int row1, col1, val;
    int row2, col2, fcol;
    int row3, col3, scnt;
    int boxes[9];
    int boxcnt;
    int box1, box2, box3, box4, i;
    PSET ps1, ps2, ps3, ps4;

    PURSTR pur = &_s_urstr;
    PROWCOL prc1, prc2, prc3, prc4;
    char *tb = GetNxtBuf();

    pur->pvrcr = Get_UR_Rects2_Ptr();
    prc1 = &pur->rc1;
    prc2 = &pur->rc2;
    prc3 = &pur->rc3;
    prc4 = &pur->rc4;
    pur->pb = pb;
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

    for (row1 = 0; row1 < 9; row1++) {
        for (col1 = 0; col1 < 9; col1++) {
            val = pb->line[row1].val[col1];
            if (val) continue;
            ps1 = &pb->line[row1].set[col1];
            prc1->row = row1;
            prc1->col = col1;
            box1 = GETBOX(row1,col1);
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
                    box2 = GETBOX(row2,col2);
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
                            box3 = GETBOX(prc3->row,prc3->col);
                            box4 = GETBOX(prc4->row,prc4->col);

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
                            box3 = GETBOX(prc3->row,prc3->col);
                            box4 = GETBOX(prc4->row,prc4->col);

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



// ******************************************************
void Do_WM_TIMER2( HWND hWnd )
{
    if (!Done_Auto) {
        if (g_bAutoLoad)
            Auto_Load_file(hWnd);
    }
    Done_Auto = true;

    if (g_bAutoComplete) {
        PROWCOL prc = Get_First_Ones();
        if (prc) {
            PABOX2 pb = get_curr_box();
            int i = one_value_count;
            while(i--) {
                int val;
                int row = prc->row;
                int col = prc->col;
                int len = Get_Set_Cnt( &prc->set, &val );
                if (len == 1) {
                    Change_Box(pb, row, col, val);
                }
                prc++;
            }
        } else {
            sprtf("No more autocomplete items!\n");
        }
        g_bAutoComplete = false;
    }

    if (!g_bAutoComplete && g_bAutoSolve) {
        //if (iSolveStage == 0) {
        //    bgn_box = get_curr_box();
        //    iSolveStage++;
        //}
        if (Get_First_Ones()) {
            g_bAutoComplete = true;
        } else if (total_empty_count) {
            sprtf("WARNING: Auto Solver can NOT continue!\n");
            g_bAutoSolve = FALSE;
        } else {
            g_bAutoSolve = FALSE;
        }
    }
    if (total_empty_count == 0) {
        g_bAutoSolve = FALSE;
        g_bAutoComplete = false;
        if (!done_end_dialog) {
            done_end_dialog = true;
            Do_Solved_Message();
        }
    }
}

// ******************************************************
// Sudo_AIC2.cxx

#include "Sudoku.hxx"

#define OUTITAIC(tb) if (add_debug_aic) OUTIT(tb)

// int test_start_cand = 8;
// examples\AIC-scan-01.txt - 
static int test_start_cand = 4;
#define int_in_vint Int_in_iVector

static int add_debug_aic2 = 0;
static int max_links_in_chain = 20;    // This is a little abitrary
static bool show_all_cands = false;
static bool start_strong_link = true;
static size_t min_candidates = 3;
static BOOL g_bNeverStartSameCell = TRUE;
static BOOL gbSkipSRCB = TRUE; // skip chaining by the same ROW, COL, or BOX

// In examples\AIC-scan-01.txt looking for 
// AIC (Alternating Inference Chain) Rule 1:
// +4[B7]-4[B2]+7[B2]-7[B5]+6[B5]-6[H5]+4[H5]-4[H7]+4[B7]
// - Off-chain candidate 7 taken off B3 - weak link: B2 to B5
// - Off-chain candidate 6 taken off J5 - weak link: B5 to H5

typedef struct tagAICSTR {
    PABOX2 pb;
    int setval, links;
    vRC *pvrc;
    vRCP *pvrcp;
    PROWCOL prclast;
    uint64_t flg2;
    uint64_t flg;
    PRCRCB prcrcb;
    ROWCOL rcFirst; // stay CONSTANT
    ROWCOL rcLast;  // always current LAST in CHAIN
    bool exit_scan;
    char *scan;
    vINT *pvirow, *pvicol, *pvibox;
}AICSTR, *PAICSTR;

static AICSTR _s_aicstr;

static int AIC_Follow_Chain( PAICSTR paic )
{
    int count = 0;
    ROWCOL rc1, rc2, rclast;
    int setval, lval, scnt;
    uint64_t flag,clr,dir,flg,flg2,nflg;
    int setvals[9];
    vRCP *pvrcp;
    int i, setval2, lval2, links;
    RCPAIR rcpair;
    char *tb = GetNxtBuf();

    pvrcp = paic->pvrcp;        // pairs in chain already
    rclast = paic->rcLast;      // get LAST in CHAIN
    setval = rclast.cnum;         // get last CANDIDATE
    lval = setval - 1;
    flag = rclast.set.flag[lval]; // and last LINK flag
    // dir = flag & cl_LBA;        // last direction travelled
    clr = flag & cf_XAB;            // and ON/OFF color
    dir = paic->flg2;               // last DIRECTION
    flg = FLIP_COLOR(clr);
    scnt = Get_Set_Cnt2(&rclast.set,setvals);
    links = paic->links;

    rc1 = rclast;   // start with LAST
    if ( !(dir & cl_LSS) && (scnt == 2) && !prc_in_pvrcp(&rclast,pvrcp,1) ) {
        flg2 = cl_LSS;
        for (i = 0; i < scnt; i++) {
            setval2 = setvals[i];
            if (!VALUEVALID(setval2)) continue; // This would be an ERROR
            if (setval2 == setval) continue;
            lval2 = setval2 - 1;
            rc2.row = rclast.row;
            rc2.col = rclast.col;
            COPY_SET(&rc2.set,&rclast.set);
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
            paic->links = links + 1;
            paic->flg2 = flg2;
            AIC_Follow_Chain( paic );
            pvrcp->pop_back();
            paic->rcLast = rclast; // restore LAST start
            if (paic->exit_scan) return count;
        }
    }
    paic->rcLast = rclast; // restore LAST start

    return count;
}



#define CLRVINTS { virow.clear(); vicol.clear(); vibox.clear(); }
// This scan introduces an extended use of ROWCOL
// where the candidate value is also known... so the cnum member of ROWCOL
// with be used for this...
int Mark_AIC_Scans2( PABOX2 pb )
{
    int count = 0;
    int row, col, box, val, val2, i, i2, cnt, lval;
    PSET ps, ps2;
    vRC empty;
    RCRCB rcrcb;
    vRC *pvrow, *pvcol, *pvbox;
    size_t max, ii, max2;
    PROWCOL prc, prcf, prcl;
    PRCPAIR prcp;
    int setvals1[9];
    RCPAIR rcp;
    vRCP vrcp;
    PAICSTR paic = &_s_aicstr;
    uint64_t sl_flag;
    vINT virow, vicol, vibox;
    int scnout;

    char *tb = GetNxtBuf();
    cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    if (cnt < 4)
        return 0;
    Kill_AIC_chains(); // remove any previous chains
    prcf = &paic->rcFirst;
    prcl = &paic->rcLast;
    paic->pb   = pb;
    paic->pvrcp  = &vrcp;
    paic->prcrcb = &rcrcb;
    paic->exit_scan = false;
    paic->pvirow = &virow;
    paic->pvicol = &vicol;
    paic->pvibox = &vibox;
    CLRVINTS;

    sprtf("Doing AIC scan on %d empty cells.\n",cnt);
    if (add_debug_aic) {
        scnout = add_screen_out(0);
        Show_PRCB(&rcrcb);
    }

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
                pvrow = &rcrcb.vrows[prcf->row];
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
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SR %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links = 1;
                    paic->flg2  = cl_LBR;
                    virow.push_back(row);
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
                    if (paic->exit_scan) break;
                }   // for (ii = 0; ii < max; ii++) 

Do_Cols:
                if (paic->exit_scan) break;
                // c@RC - reach out in SAME COL
                pvcol = &rcrcb.vcols[prcf->col];
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
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SC %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links = 1;
                    paic->flg2  = cl_LBC;
                    vicol.push_back(col);
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
                    if (paic->exit_scan) break;
                }   // for (ii = 0; ii < max; ii++) 

Do_Box:
                if (paic->exit_scan) break;
                // c@RC - reach out in SAME BOX
                pvbox = &rcrcb.vboxs[prcf->box];
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
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SB %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->flg2  = cl_LBB;
                    vibox.push_back(box);
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
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
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by CELL
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];    // get pointer to LAST
                    sprintf(tb,"AIC Begin: SS %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->flg2  = cl_LSS;
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
                    if (paic->exit_scan) break;
                }   // for (i2 = 0; i2 < cnt; i2++)
                if (paic->exit_scan) break;
            }   // for (i = 0; i < cnt; i++) {
            if (paic->exit_scan) break;
        }   // for (col = 0; col < 9; col++)
        if (paic->exit_scan) break;
    }   // for (row = 0; row < 9; row++) 

    count = Get_AIC_chain_count(); // { return AIC_chain_count; }
    if (count) validate_AIC_chains(true);

    if (add_debug_aic) {
         add_screen_out(scnout);
    }
    return count;
}

// eof - Sudo_AIC2.cxx

// ******************************************************
int Mark_AIC_Scans_OK( PABOX2 pb )
{
    int count = 0;
    int row, col, box, val, val2, i, i2, cnt, lval;
    PSET ps, ps2;
    vRC empty;
    RCRCB rcrcb;
    vRC *pvrow, *pvcol, *pvbox;
    size_t max, ii, max2;
    PROWCOL prc, prcf, prcl;
    PRCPAIR prcp;
    int setvals1[9];
    RCPAIR rcp;
    vRCP vrcp;
    PAICSTR paic = &_s_aicstr;
    uint64_t sl_flag;
    vINT virow, vicol, vibox;
    int scnout;

    char *tb = GetNxtBuf();
    cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    if (cnt < 4)
        return 0;
    Kill_AIC_chains(); // remove any previous chains
    prcf = &paic->rcFirst;
    prcl = &paic->rcLast;
    paic->pb        = pb;
    paic->elim_count = 0;
    paic->pvrcp     = &vrcp;
    paic->prcrcb    = &rcrcb;
    paic->exit_scan = false;
    paic->pvirow    = &virow;
    paic->pvicol    = &vicol;
    paic->pvibox    = &vibox;
    CLRVINTS;

    sprtf("Doing AIC scan on %d empty cells.\n",cnt);
    if (add_debug_aic) {
        scnout = add_screen_out(0);
        Show_PRCB(&rcrcb);
    }

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
                pvrow = &rcrcb.vrows[prcf->row];
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
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SR %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LBR;
                    virow.push_back(row);
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
                    if (paic->exit_scan) break;
                }   // for (ii = 0; ii < max; ii++) 

Do_Cols:
                if (paic->exit_scan) break;
                // c@RC - reach out in SAME COL
                pvcol = &rcrcb.vcols[prcf->col];
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
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SC %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LBC;
                    vicol.push_back(col);
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
                    if (paic->exit_scan) break;
                }   // for (ii = 0; ii < max; ii++) 

Do_Box:
                if (paic->exit_scan) break;
                // c@RC - reach out in SAME BOX
                pvbox = &rcrcb.vboxs[prcf->box];
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
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SB %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LBB;
                    vibox.push_back(box);
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
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
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by CELL
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];    // get pointer to LAST
                    sprintf(tb,"AIC Begin: SS %s ",Get_RCPAIR_Stg(prcp,val,show_all_cands));
                    OUTITAIC(tb);
                    paic->links  = 1;
                    paic->clrflg = cf_XCA;
                    paic->dirflg = cl_LSS;
                    AIC_Follow_Chain( paic );
                    vrcp.clear();
                    CLRVINTS;
                    if (paic->exit_scan) break;
                }   // for (i2 = 0; i2 < cnt; i2++)
                if (paic->exit_scan) break;
            }   // for (i = 0; i < cnt; i++) {
            if (paic->exit_scan) break;
        }   // for (col = 0; col < 9; col++)
        if (paic->exit_scan) break;
    }   // for (row = 0; row < 9; row++) 

    if (add_debug_aic) {
         add_screen_out(scnout);
    }
    //count = Get_AIC_chain_count(); // { return AIC_chain_count; }
    count = paic->elim_count;
    if (count) {
        AIC_chains_valid = true;
        Stack_Fill(Do_Clear_AIC);
    }
    return count;
}


int AIC_Follow_Chain( PABOX2 pb, vRCP *pvrcp, PRCRCB prcrcb, int links );

void Store_AIC_Chain( PABOX2 pb, vRCP *pvrcp, PRCRCB prcrcb, int links )
{
    size_t max = pvrcp->size();
    if (!max)
        return;
    if (AIC_chain_count >= MY_MX_CHAINS) {
        sprtf("Chain count exceeds MY_MX_CHAINS %d! RECOMPILE!!\n", MY_MX_CHAINS);
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
    AIC_chain_count++;
}


int AIC_Follow_Chain( PABOX2 pb, vRCP *pvrcp, PRCRCB prcrcb, int links )
{
    int count = 0;
    size_t max;
    PRCPAIR prcplast;    // get LAST PAIR in CHAIN
    PROWCOL prclast;     // last CELL
    int setval;      // get last CANDIDATE
    int lval;
    uint64_t flag; // and last LINK flag
    uint64_t dir;        // last direction travelled
    uint64_t clr;        // and ON/OFF color
    uint64_t flg, flg2, nflg;
    ROWCOL rc, rc2;
    RCPAIR rcpair;
    vRC *pvrow, *pvcol, *pvbox, *pvrc;
    size_t ii;
    PRCPAIR prcp2;
    PROWCOL prc2;
    int setval2, lval2;
    char *tb = GetNxtBuf();

    max = pvrcp->size();
    if (!max) return 0;
    prcplast = &pvrcp->at(max-1);    // get LAST PAIR in CHAIN
    prclast = &prcplast->rowcol[1];      // last CELL
    setval = prclast->cnum;          // get last CANDIDATE
    lval = setval - 1;
    flag = prclast->set.flag[lval];  // and last LINK flag
    dir = flag & cl_LBA;         // last direction travelled
    clr = flag & cf_XAB;         // and ON/OFF color

    pvrow = &prcrcb->vrows[prclast->row];
    pvcol = &prcrcb->vcols[prclast->col];
    pvbox = &prcrcb->vrows[prclast->box];

    if (!is_valid_prc(prclast)) {
        if (add_debug_aic) {
            sprtf("WARNING: AIC_Follow_Chain called with INVALID PROWCOL! [%s]\n",
                Get_RC_setval_RC_Stg(prclast));
        }
        return 0;
    }
    if (links > max_links_in_chain) {
        return 0;
    }
    if (links > 2) {
        // can maybe close this chain back to first
        prcp2 = &pvrcp->at(0);    // get FIRST PAIR in CHAIN
        prc2 = &prcp2->rowcol[0];  // last FIRST CELL
        setval2 = prc2->cnum;         // get first CANDIDATE
        // can ONLY close if SAME candidate
        if ((setval2 == setval) && 
            ((prc2->row == prclast->row)||(prc2->col == prclast->col)||(prc2->box == prclast->box))) {
            // YOW a CLOSED CHAIN - Now ONLY interest is does it ELIMINATE anything
            rc = *prclast;
            rc2 = *prc2;
            rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage
            rc2.set.flag[lval] &= ~(cl_LBA|cl_SLA); // take OUT linkage
            flg = cl_LBB;
            if (prc2->row == prclast->row) 
                flg = cl_LBR;
            else if (prc2->col == prclast->col)
                flg = cl_LBC;
            rc.set.flag[lval]  |= flg; // add linkage
            rc2.set.flag[lval] |= flg; // add linkage
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            Store_AIC_Chain( pb, pvrcp, prcrcb, links+1 );
            pvrcp->pop_back();  // drop this LAST link

            // should NOT be required, but if seem memory reallocation IS DONE
            max = pvrcp->size();
            if (!max) return 0;
            prcplast = &pvrcp->at(max-1);    // get LAST PAIR in CHAIN
            prclast = &prcplast->rowcol[1];      // last CELL
            setval = prclast->cnum;          // get last CANDIDATE
            lval = setval - 1;
            flag = prclast->set.flag[lval];  // and last LINK flag
            dir = flag & cl_LBA;         // last direction travelled
            clr = flag & cf_XAB;         // and ON/OFF color

            pvrow = &prcrcb->vrows[prclast->row];
            pvcol = &prcrcb->vcols[prclast->col];
            pvbox = &prcrcb->vrows[prclast->box];

        }

    }
    // set OPPOSITE color
    flg = FLIP_COLOR(clr);
    if (add_debug_aic & !flg) {
        sprtf("WARNING: COLOR flag is NULL\n");
    }
    if (dir & cl_LBR) {
        // can now try COL and BOX
        // COL scan
        pvrc = pvcol;
        flg2 = cl_LBC;
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc2 = &pvrc->at(ii);
            if (SAMERCCELL(prc2,prclast)) continue;
            if (prc_in_pvrcp(prc2,pvrcp)) continue;
            if ( !(prc2->set.val[lval]) ) continue;
            if (prc2->box == prclast->box) continue; // let the BOX scan do this
            rc  = *prclast;
            rc2 = *prc2;
            rc2.cnum = setval;
            rc2.box  = GETBOX(rc2.row,rc2.col);
            rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage - keep color
            nflg = rc2.set.flag[lval]; // get FLAG
            nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg | flg2); // add same COL linkage, 
            rc2.set.flag[lval] = nflg;  // and opp COLOR
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            sprintf(tb,"AIC Cont: R-SC %d %s %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), links);
            OUTITAIC(tb);
            AIC_Follow_Chain( pb, pvrcp, prcrcb, (links+1) );
            pvrcp->pop_back();
        }
        // BOX scan
        pvrc = pvbox;
        flg2 = cl_LBB;
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc2 = &pvrc->at(ii);
            if (SAMERCCELL(prc2,prclast)) continue;
            if (prc_in_pvrcp(prc2,pvrcp)) continue;
            if ( !(prc2->set.val[lval]) ) continue;
            rc = *prclast;
            rc2 = *prc2;
            rc2.cnum = setval;
            rc2.box  = GETBOX(rc2.row,rc2.col);
            rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage - keep color
            nflg = rc2.set.flag[lval];
            nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg2 | flg); // add back linkage and colr
            rc2.set.flag[lval] = nflg; // add back linkage and color
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            sprintf(tb,"AIC Cont: R-SB %d %s %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), links);
            OUTITAIC(tb);
            AIC_Follow_Chain( pb, pvrcp, prcrcb, (links+1) );
            pvrcp->pop_back();
        }
    } else if (dir & cl_LBC) {
        // can now try ROW and BOX
        // ROW scan
        pvrc = pvrow;
        flg2 = cl_LBR;
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc2 = &pvrc->at(ii);
            if (SAMERCCELL(prc2,prclast)) continue;
            if (prc_in_pvrcp(prc2,pvrcp)) continue;
            if ( !(prc2->set.val[lval]) ) continue;
            if (prc2->box == prclast->box) continue; // let the BOX scan do this
            rc = *prclast;
            rc2 = *prc2;
            rc2.cnum = setval;
            rc2.box  = GETBOX(rc2.row,rc2.col);
            rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage - keep color
            nflg = rc2.set.flag[lval];
            nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg2 | flg); // add back linkage and color
            rc2.set.flag[lval] = nflg; // set
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            sprintf(tb,"AIC Cont: C-SR %d %s %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), links);
            OUTITAIC(tb);
            AIC_Follow_Chain( pb, pvrcp, prcrcb, (links+1) );
            pvrcp->pop_back();
        }
        // BOX scan
        pvrc = pvbox;
        flg2 = cl_LBB;
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc2 = &pvrc->at(ii);
            if (SAMERCCELL(prc2,prclast)) continue;
            if (prc_in_pvrcp(prc2,pvrcp)) continue;
            if ( !(prc2->set.val[lval]) ) continue;
            rc = *prclast;
            rc2 = *prc2;
            rc2.cnum = setval;
            rc2.box  = GETBOX(rc2.row,rc2.col);
            rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage - keep color
            nflg = rc2.set.flag[lval];
            nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg2 | flg); // add back linkage and color
            rc2.set.flag[lval] = nflg; // set
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            sprintf(tb,"AIC Cont: C-SB %d %s %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), links);
            OUTITAIC(tb);
            AIC_Follow_Chain( pb, pvrcp, prcrcb, (links+1) );
            pvrcp->pop_back();
        }

    } else if (dir & cl_LBB) {
        // can now try ROW and COL
        // ROW scan
        pvrc = pvrow;
        flg2 = cl_LBR;
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc2 = &pvrc->at(ii);
            if (SAMERCCELL(prc2,prclast)) continue;
            if (prc_in_pvrcp(prc2,pvrcp)) continue;
            if ( !(prc2->set.val[lval]) ) continue;
            if (prc2->box == prclast->box) continue; // let the BOX scan do this
            rc = *prclast;
            rc2 = *prc2;
            rc2.cnum = setval;
            rc2.box  = GETBOX(rc2.row,rc2.col);
            rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage - keep color
            nflg = rc2.set.flag[lval];
            nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg2 | flg); // add back linkage and color
            rc2.set.flag[lval] = nflg; // set
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            sprintf(tb,"AIC Cont: B-SR %d %s %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), links);
            OUTITAIC(tb);
            AIC_Follow_Chain( pb, pvrcp, prcrcb, (links+1) );
            pvrcp->pop_back();
        }
        // COL scan
        pvrc = pvcol;
        flg2 = cl_LBC;
        max = pvrc->size();
        for (ii = 0; ii < max; ii++) {
            prc2 = &pvrc->at(ii);
            if (SAMERCCELL(prc2,prclast)) continue;
            if (prc_in_pvrcp(prc2,pvrcp)) continue;
            if ( !(prc2->set.val[lval]) ) continue;
            if (prc2->box == prclast->box) continue; // let the BOX scan do this
            rc = *prclast;
            rc2 = *prc2;
            rc2.cnum = setval;
            rc2.box  = GETBOX(rc2.row,rc2.col);
            rc.set.flag[lval]  &= ~(cl_LBA|cl_SLA); // take OUT linkage - keep color
            nflg = rc2.set.flag[lval]; // get flag
            nflg  &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
            nflg |= (flg2 | flg); // add same COL linkage
            rc2.set.flag[lval] = nflg;
            rcpair.rowcol[0] = rc;
            rcpair.rowcol[1] = rc2;
            pvrcp->push_back(rcpair);
            sprintf(tb,"AIC Cont: B-SC %d %s %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands),links);
            OUTITAIC(tb);
            AIC_Follow_Chain( pb, pvrcp, prcrcb, (links+1) );
            pvrcp->pop_back();
        }
    }

    // and always can try OTHER candidates in this cell
    // ================================================
    int i, cnt, setvals[9];
    cnt = Get_Set_Cnt2(&prclast->set,setvals);
    flg2 = cl_LSS;
    for (i = 0; i < cnt; i++) {
        setval2 = setvals[i];
        if (!VALUEVALID(setval2)) continue; // This would be an ERROR
        if (setval2 == setval) continue;
        lval2 = setval2 - 1;
        rc = *prclast;
        rc2.row = prclast->row;
        rc2.col = prclast->col;
        COPY_SET(&rc2.set,&prclast->set);
        rc2.cnum = setval2; // set NEW setval
        rc2.box  = GETBOX(rc2.row,rc2.col);
        // fix flags for this new PAIR, with DIFFERENT setvals
        rc.set.flag[lval] &= ~(cl_LBA|cl_SLA); // take OUT linkage
        rc.set.flag[lval] |= flg2;    // add in new linkage
        // rc keep color
        nflg = rc2.set.flag[lval2];
        nflg &= ~(cl_LBA|cl_SLA|cf_XAB); // take out linkage and color ON/OFF
        nflg |= (flg2 | flg); // add same cell linkage and OPPOSITE color
        rc2.set.flag[lval2] = nflg; // add OPPOSITE color
        rcpair.rowcol[0] = rc;
        rcpair.rowcol[1] = rc2;
        pvrcp->push_back(rcpair);
        sprintf(tb,"AIC Cont: SS %d %s %d %d ",setval, Get_RCPAIR_Stg(&rcpair,setval,show_all_cands), setval2, links);
        OUTITAIC(tb);
        AIC_Follow_Chain( pb, pvrcp, prcrcb, (links+1) );
        pvrcp->pop_back();
    }

    return count;
}


int Mark_AIC_Scans2( PABOX2 pb )
{
    int count = 0;
    int row, col, val, val2, i, i2, cnt, lval;
    PSET ps, ps2;
    //ROWCOL rc;
    ROWCOL rc2;
    vRC empty;
    RCRCB rcrcb;
    vRC *pvrow, *pvcol, *pvbox;
    size_t max, ii, max2;
    PROWCOL prc, prcf;
    PRCPAIR prcp;
    int setvals1[9];
    RCPAIR rcp;
    vRCP vrcp;
    PAICSTR paic = &_s_aicstr;
    char *tb = GetNxtBuf();
    cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    if (cnt < 4)
        return 0;
    Kill_AIC_chains(); // remove any previous chains
    prcf = &paic->rcFirst;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            // got a set of candidates to chain from...
            prcf->row = row;
            prcf->col = col;
            prcf->box = GETBOX(row,col);
            COPY_SET(&prcf->set,ps);
            cnt = Get_Set_Cnt2(ps,setvals1);
            for (i = 0; i < cnt; i++) {
                val = setvals1[i];
                prcf->cnum = val;
                lval = val - 1;
                // got FIRST member of CHAIN
                // c@RC - reach out in SAME ROW
                pvrow = &rcrcb.vrows[prcf->row];
                max = pvrow->size();
                if (min_candidates && (max < min_candidates)) continue; // first LINK must have 3 in unit
                for (ii = 0; ii < max; ii++) {
                    prc = &pvrow->at(ii);
                    if (prc->col == col) continue; // skip self
                    ps2 = &pb->line[row].set[prc->col];
                    if ( !(ps2->val[lval]) ) continue; // does NOT hold candidate
                    // got 2nd member in chain - with a strong or weak link to 1st
                    rc2.row = row;
                    rc2.col = prc->col;
                    rc2.box = GETBOX(row,prc->col);
                    COPY_SET(&rc2.set,ps2);
                    rc2.cnum = val;
                    // got a PAIR, linked by ROW
                    prcf->set.flag[lval]  |= (cf_XCA | cl_LBR);
                    rc2.set.flag[lval] |= (cf_XCB | cl_LBR);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = rc2;
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SR %s ",Get_RCPAIR_Stg(prcp,val,true));
                    OUTITAIC(tb);
                    AIC_Follow_Chain( pb, &vrcp, &rcrcb, 1 );
                    vrcp.clear();
                }
                // c@RC - reach out in SAME COL
                pvcol = &rcrcb.vcols[prcf->col];
                max = pvcol->size();
                if (min_candidates && (max < min_candidates)) continue; // first LINK must have 3 in unit
                for (ii = 0; ii < max; ii++) {
                    prc = &pvcol->at(ii);
                    if (prc->row == row) continue; // skip self
                    ps2 = &pb->line[prc->row].set[col];
                    if ( !(ps2->val[lval]) ) continue; // does NOT hold candidate
                    // got 2nd member in chain - with a strong or weak link to 1st
                    rc2.row = prc->row;
                    rc2.col = col;
                    rc2.box = GETBOX(prc->row,col);
                    COPY_SET(&rc2.set,ps2);
                    rc2.cnum = val;
                    // got a PAIR, linked by ROW
                    prcf->set.flag[lval]  |= (cf_XCA | cl_LBC);
                    rc2.set.flag[lval] |= (cf_XCB | cl_LBC);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = rc2;
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SC %s ",Get_RCPAIR_Stg(prcp,val,true));
                    OUTITAIC(tb);
                    AIC_Follow_Chain( pb, &vrcp, &rcrcb, 1 );
                    vrcp.clear();
                }
                // c@RC - reach out in SAME BOX
                pvbox = &rcrcb.vboxs[prcf->col];
                max = pvbox->size();
                if (min_candidates && (max < min_candidates)) continue; // first LINK must have 3 in unit
                for (ii = 0; ii < max; ii++) {
                    prc = &pvbox->at(ii);
                    if ((prc->col == col)&&(prc->row == row)) continue; // skip self
                    ps2 = &pb->line[prc->row].set[prc->col];
                    if ( !(ps2->val[lval]) ) continue; // does NOT hold candidate
                    // got 2nd member in chain - with a strong or weak link to 1st
                    rc2.row = prc->row;
                    rc2.col = prc->col;
                    rc2.box = GETBOX(prc->row,prc->col);
                    COPY_SET(&rc2.set,ps2);
                    rc2.cnum = val;
                    // got a PAIR, linked by ROW
                    prcf->set.flag[lval]  |= (cf_XCA | cl_LBB);
                    rc2.set.flag[lval] |= (cf_XCB | cl_LBB);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = rc2;
                    vrcp.clear();
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by ROW
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];
                    sprintf(tb,"AIC Begin: SB %s ",Get_RCPAIR_Stg(prcp,val,true));
                    OUTITAIC(tb);
                    AIC_Follow_Chain( pb, &vrcp, &rcrcb, 1 );
                    vrcp.clear();
                }
                // c@RC - reach out in SAME CELL, another candidate
                if (g_bNeverStartSameCell) continue;
                for (i2 = 0; i2 < cnt; i2++) {
                    val2 = setvals1[i2];
                    if (val2 == val) continue;
                    rc2.row = prc->row;
                    rc2.col = prc->col;
                    rc2.box = GETBOX(prc->row,prc->col);
                    COPY_SET(&rc2.set,ps2);
                    rc2.cnum = val;
                    // got a PAIR, linked by CELL
                    prcf->set.flag[lval]  |= (cf_XCA | cl_LSS);
                    rc2.set.flag[lval] |= (cf_XCB | cl_LSS);
                    rcp.rowcol[0] = *prcf;
                    rcp.rowcol[1] = rc2;
                    vrcp.push_back(rcp); // store first PAIR A & B ON/OFF linked by CELL
                    max2 = vrcp.size();
                    prcp = &vrcp[max2 - 1];    // get pointer to LAST
                    sprintf(tb,"AIC Begin: SS %s ",Get_RCPAIR_Stg(prcp,val,true));
                    OUTITAIC(tb);
                    AIC_Follow_Chain( pb, &vrcp, &rcrcb, 1 );
                    vrcp.clear();
                }
            }
        }
    }
    return count;
}


// ******************************************************
int Do_Locked_Excl2_ABANDONED( PABOX2 pb )
{
    int count = 0;
    vRC empty;
    RCRCB rcrcb;
    int cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    int row, col, box;
    vRC *pvrc;
    size_t max, i;
    SET setcomm;
    PROWCOL prc;
    ROWCOL rc;
    // get common set per BOX (region)
    SET setpbox[9];
    int boxes[9];
    int j, setval, k;
    int r, c, rw, cl;
    for (j = 0; j < 9; j++) {
        pvrc = &rcrcb.vboxs[j];   // get rc set for each BOX
        ZERO_SET(&setpbox[j]);
        max = pvrc->size(); // empty cells in this BOX (region)
        for (i = 0; i < max; i++) {
            prc = &pvrc->at(i); // do EACH for each set for this BOX
            Get_Common_SET( &prc->set, &setpbox[j] ); 
        }
    }
    // GOT common set for EACH BOX
    // Now scan the ROWS
    for (j = 0; j < 9; j++) {
        pvrc = &rcrcb.vrows[j];   // get rc set for each ROW
        max = pvrc->size();
        if (!max) continue; // no empties in ROW
        ZERO_SET(&setcomm);
        memset(boxes,0,(sizeof(int)*9));
        cnt = 0;
        for (i = 0; i < max; i++) {
            prc = &pvrc->at(i); // do EACH empty cell in the ROW
            rc = *prc;
            row = prc->row;
            col = prc->col;
            box = GETBOX(row,col);
            cnt += Get_Common_SET( &prc->set, &setcomm); 
            boxes[box] = box + 1;
        }
        r = (row / 3) * 3;
        c = (col / 3) * 3;
        // have the COMMON set for the empty cells in this ROW
        // take each common candidate in this ROW
        for (k = 0; k < 9; k++) {
            setval = setcomm.val[k];
            if (!setval) continue;
            // does this candidate occupy each of the remaining cells, slots, in any Box
            // and ONLY exist in that BOX
            // Any particular ROW would always only cover 3 boxes
            // Any Boxes with empty slots whould be marked in boxes[]
            // Process the BOXES
            for (rw = 0; rw < 3; rw++) {
                for (cl = 0; cl < 3; cl++) {
                    row = r + rw;
                    col = c + cl;
                    box = GETBOX(row,col);
                    if (!boxes[box]) continue;
                    // Ok, have a BOX, crossed by this ROW, that has empty cells

                }
            }
        }
    }
    for (col = 0; col < 9; col++) {

    }

    return count;
}


// ******************************************************
TCHAR file11[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-11.csv");
TCHAR file2[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-02.csv");
TCHAR file3[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-05.csv");
TCHAR file4[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-03.csv");
TCHAR file5[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-04.csv");
TCHAR file6[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-06.csv");
TCHAR file7[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-01.csv");
TCHAR file8[_MAX_PATH] = _T("C:\\GTools\\tools\\Sudoku\\Sudoku-07.csv");


// ******************************************************
// Aborted BOX3 attempt
// ==================================================
// Another approach - doing whole Sudoku anaysis
typedef struct tagASET3 {
    int count;    // count of values
    int value[9]; // value when valid, zero otherwise
}SET3, *PSET3;

typedef struct tagALINE3 {
    int value[9]; // value when known, zero otherwise
    SET3 sets[9];
}ALINE3, *PALINE3;

typedef struct tagABOX3 {
    ALINE3 lines[9];  // just nine lines, with a SET for EACH cell
}ABOX3, *PABOX3;

typedef vector<SET3> vSET3;
typedef vSET3::iterator vSET3i;

// move onto another structure, more like the first ;=()
extern int Elim_Row_Col_Square3( PABOX3 pb3, int in_row, int in_col, PSET3 ps3 );
extern void Show_Box3_Counts( PABOX3 pb3, int flag = 0 );
extern void Set_BOX3_Sets( PABOX3 pb3, int flag = 0 );
extern void Copy_BOX2_to_BOX3( PABOX2 pb, PABOX3 pb3, int flag = 0 );
// extern void Copy_BOX3_to_BOX2( PABOX2 pb, PABOX3 pb3, int flag = 0 );
extern PABOX3 get_curr_box3();

PABOX3 pBox3 = 0;
PABOX3 get_curr_box3()
{
    if (!pBox3) {
        pBox3 = (PABOX3)malloc(sizeof(ABOX3));
        if (!pBox3) {
            MB("PABOX3 Memory FAILED!");
            exit(1);
        }
    }
    return pBox3;
}
PABOX3 Copy_Box3( PABOX3 pb_in )
{
    PABOX3 pb = (PABOX3)malloc( sizeof(ABOX3) );
    if (!pb) {
        MB("ERROR: Memory allocation FAILED!\n");
        exit(1);
    }
    memcpy(pb,pb_in,sizeof(ABOX3));
    return pb;
}

int Elim_Row_Col_Square3( PABOX3 pb3, int in_row, int in_col, PSET3 ps3 )
{
    int row, col, val, cnt, i;
    // keep in row, and scan columns
    for (col = 0; col < 9; col++) {
        val = pb3->lines[in_row].value[col];
        if (val)
            ps3->value[val - 1] = 0;    // clear this value
    }
    // keep in column, and scan rows
    for (row = 0; row < 9; row++) {
        val = pb3->lines[row].value[in_col];
        if (val)
            ps3->value[val - 1] = 0;    // clear this value
    }
    // scan the current square
    int r = in_row / 3;
    int c = in_col / 3;
    r *= 3;
    c *= 3;
    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            val = pb3->lines[r + row].value[c + col];
            if (val)
                ps3->value[val - 1] = 0;    // clear this value
        }
    }
    // set potential values count
    cnt = 0;
    for (i = 0; i < 9; i++) {
        val = ps3->value[i];
        if (val)
            cnt++;
    }
    ps3->count = cnt;   // set count of potential values
    return cnt;
}

void Show_Box3_Counts( PABOX3 pb3, int flag )
{
    int row, col, val, i;
    PSET3 ps3;
    int so = add_screen_out(flag & flg_ADDSCREEN);
    char buf[MAX_STRING];
    char *ps = buf;
    vSTG vs;
    SET3 set3;
    vSET3 vset3;
    string s;
    int push_cnt = 0;
    *ps = 0;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb3->lines[row].value[col];
            ps3 = &pb3->lines[row].sets[col];
            sprintf(ps,"Row %d Col %d ", (row + 1), (col + 1));
            if (val) {
                sprintf(EndBuf(ps),"has value %d",val);
                set3.count = -1;
                if ( !(flag & flg_SHOWVALS) )
                    continue;
            } else {
                sprintf(EndBuf(ps),"has %d possible ",ps3->count);
                set3.count = ps3->count;
                for (i = 0; i < 9; i++) {
                    val = ps3->value[i];
                    if (val)
                        sprintf(EndBuf(ps),"%d",val);
                    set3.value[i] = val;
                }
            }
            s = ps;
            vs.push_back(s);
            //sprtf("%s\n",ps);
            vset3.push_back(set3);
            push_cnt++;
        }
    }
    vSTGi ii;
    sprtf("List of %d cells, and their potential values %s\n", push_cnt,
        ((flag & flg_NOSORT) ? "in cell order" : "sorted by counts"));

    push_cnt = 0;
    if (flag & flg_NOSORT) {
        for (ii = vs.begin(); ii != vs.end(); ii++) {
            s = *ii;
            sprtf("%s\n",s.c_str());
            push_cnt++;
        }
    } else {
        // ok want to sort them... vset3 contains set count
        int cnt = 0;
        push_cnt = 0;
        vSET3i is;
        for (is = vset3.begin(); is != vset3.end(); is++) {
            set3 = *is;
            if (set3.count == -1) {
                s = vs[cnt];
                sprtf("%s\n",s.c_str());
                push_cnt++;
            }
            cnt++;
        }
        for (val = 0; val <= 9; val++) {
            cnt = 0;
            for (is = vset3.begin(); is != vset3.end(); is++) {
                set3 = *is;
                if (set3.count == val) {
                    s = vs[cnt];
                    sprtf("%s\n",s.c_str());
                    push_cnt++;
                }
                cnt++;
            }
        }
    }
    sprtf("End of list of %d cells...\n", push_cnt);
    add_screen_out(so);
}


void Set_BOX3_Sets( PABOX3 pb3, int flag )
{
    int row, col, val, cnt, i;
    PSET3 ps3;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb3->lines[row].value[col];
            ps3 = &pb3->lines[row].sets[col];
            if (val) {
                ps3->count = -1;    // SIGNAL no values since cell has value
                for (i = 0; i < 9; i++)
                    ps3->value[i] = 0;
            } else {
                // intially can have ALL values
                for (i = 0; i < 9; i++)
                    ps3->value[i] = i + 1;  // set ALL values
                // Eliminate same row, column and already in this square
                cnt = Elim_Row_Col_Square3( pb3, row, col, ps3 );
            }
        }
    }
    if ( !(flag & flg_NOPUTPUT) )
        Show_Box3_Counts( pb3, flag );
}

void Copy_BOX2_to_BOX3( PABOX2 pb, PABOX3 pb3, int flag )
{
    int row, col, val;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            pb3->lines[row].value[col] = val;
        }
    }
    Set_BOX3_Sets(pb3, flag);
}

// discard CHANGE structre
typedef struct tagCHANGE {
    int row,col;    // aboslute row, col;
    int val;        // last change
    int cnt;        // count of changes to this cell
    int max;        // amount of change memory
    int *vals;      // changes made to this, in order
    int *order;     // order/rank of change
}CHANGE, *PCHANGE;

typedef vector<CHANGE> vCHG;
typedef vCHG::iterator vCHGi;


// spearate some paint items
void paint_xcycles_OK_BUT( HDC hdc, int setval )
{
    vRC *pvrc;
    int i;
    size_t max, j, k;
    ROWCOL rc, rc2;
    int count = 0;
    int box, box2;
    i = setval - 1;
    pvrc = get_XCycles_pvrc(i);
    max = pvrc->size();
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    for (j = 0; j < max - 1; j++) {
        k = j + 1;
        rc = pvrc->at(j);
        rc2 = pvrc->at(k);
        box = GETBOX(rc.row,rc.col);
        box2 = GETBOX(rc2.row,rc2.col);

        if ( !((rc.row == rc2.row)||(rc.col == rc2.col)||(box2 == box)) ) continue;
        //if (rc.set.tm != rc2.set.tm)
        //    continue; // ONLY join those in SAME chain

        if ((rc.set.flag[i] & cl_SLA) && (rc2.set.flag[i] & cl_SLA))
            SelectObject(hdc,hpslnk);
        else
            SelectObject(hdc,hpwlnk);

        draw_link(hdc, &rc, &rc2, setval);
        count++;
    }
    sprtf("Painted %d XCycles links\n", count);
    set_repaint2(false);
}

void paint_xcycles_PREV( HDC hdc, int setval )
{
    vRC *pvrc;
    int i;
    time_t ch_num, ch1;
    size_t max, j, k;
    ROWCOL rc, rc2;
    int count = 0;
    i = setval - 1;
    pvrc = get_XCycles_pvrc(i);
    max = pvrc->size();
    int row, col, box;
    int row2, col2, box2;
    PABOX2 pb = get_curr_box();
    vRC vdone;
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    int bcnt, rcnt, ccnt;
    vRCP vrcp;
    RCPAIR rcp;
    for (j = 0; j < max; j++) {
        rc = pvrc->at(j);
        ch_num = rc.set.tm;
        ch1 = ch_num + 1;
        row = rc.row;
        col = rc.col;
        box = GETBOX(row,col);
        vdone.push_back(rc);
        for (k = 0; k < max; k++) {
            if (k == j) continue;
            rc2 = pvrc->at(k);
            if (rc.set.tm != rc2.set.tm)
                continue; // ONLY join those in SAME chain
            row2 = rc2.row;
            col2 = rc2.col;
            box2 = GETBOX(row2,col2);
            // if (rc2.set.tm != ch1) continue;
            if ( !((row2 == row)||(col2 == col)||(box2 == box)) ) continue;
            if (Row_Col_in_RC_Vector(vdone,row2,col2))
                continue;
            // This is sometimes WRONG
            //if ((rc.set.flag[i] & cl_SLA) && (rc2.set.flag[i] & cl_SLA))
            //    SelectObject(hdc,hpslnk);
            //else
            //    SelectObject(hdc,hpwlnk);
            bcnt = 0;
            ccnt = 0;
            rcnt = 0;
            SelectObject(hdc,hpwlnk); // set weak link
            if (box2 == box) {
                // SAME box
                bcnt = Count_setval_in_Box(pb,row,col, setval );
            } else {
                // not in SAME box
                if (col2 == col) {
                    ccnt = Count_setval_in_Col(pb, row, col, setval );
                    if (ccnt == 2)
                        SelectObject(hdc,hpslnk);
                } else if (row2 == row) {
                    rcnt = Count_setval_in_Row(pb, row, col, setval );
                    if (rcnt == 2)
                        SelectObject(hdc,hpslnk);
                }
            }

            draw_link(hdc, &rc, &rc2, i + 1);
            count++;
            rcp.rowcol[0] = rc;
            rcp.rowcol[1] = rc2;
            vrcp.push_back(rcp);
        }
    }

    ROWCOL rcfirst, rclast;
    vINT vints;
    size_t v;
    int ichn, cnt;
    for (j = 0; j < max; j++) {
        rc = pvrc->at(j);
        ichn = (int)rc.set.tm;
        for (v = 0; v < vints.size(); v++) {
            if (vints[v] == ichn)
                break;
        }
        if (v == vints.size())
            vints.push_back(ichn);
    }
    for (v = 0; v < vints.size(); v++) {
        ichn = vints[v];
        cnt = 0;
        for (j = 0; j < max; j++) {
            rc = pvrc->at(j);
            if ((int)rc.set.tm == ichn) {
                if (cnt == 0)
                    rcfirst = rc;
                else
                    rclast = rc;
                cnt++;
            }
        }
        if (cnt > 2) {
            row = rcfirst.row;
            col = rcfirst.col;
            box = GETBOX(row,col);
            row2 = rclast.row;
            col2 = rclast.col;
            box2 = GETBOX(row2,col2);
            rcp.rowcol[0] = rcfirst;
            rcp.rowcol[1] = rclast;
            if (((row2 == row)||(col2 == col)||(box2 == box)) &&
                !rcp_in_vrcp( rcp, vrcp )) {
                bcnt = 0;
                ccnt = 0;
                rcnt = 0;
                SelectObject(hdc,hpwlnk); // set weak link
                if (box2 == box) {
                    // SAME box
                    bcnt = Count_setval_in_Box(pb,row,col, setval );
                } else {
                    // not in SAME box
                    if (col2 == col) {
                        ccnt = Count_setval_in_Col(pb, row, col, setval );
                        if (ccnt == 2)
                            SelectObject(hdc,hpslnk);
                    } else if (row2 == row) {
                        rcnt = Count_setval_in_Row(pb, row, col, setval );
                        if (rcnt == 2)
                            SelectObject(hdc,hpslnk);
                    }
                }
                draw_link(hdc, &rc, &rc2, i + 1);
                count++;
            }
        }
    }

    sprtf("Painted %d XCycles links\n", count);
    set_repaint2(false);
}





// ******************************************************
// Discarded X-Cycles code
//int Do_XCycles_Chain( PABOX2 pb, PROWCOL prc, int setval, vRC &members, time_t ch_num,
//                      int dep, int *pcount )
int Do_XCycles_Chain_NO( PCHNSTR pch, vRC &members )
{
    int count = 0;
    PABOX2 pb = pch->pb;
    PROWCOL prc = pch->prc;
    int setval = pch->setval;
    time_t ch_num = pch->ch_num;
    int dep = pch->dep;
    int row, col, box, val, cnt;
    bool last_was_row = pch->last_was_row;
    int sl_count = pch->sl_count;
    PSET ps;
    ROWCOL rc;
    uint64_t slflg;
    int mbox = GETBOX(prc->row, prc->col);
    uint64_t cflg = (prc->set.flag[setval - 1] & cf_XCA) ? cf_XCB : cf_XCA;
    char *tb = GetNxtBuf();
    if (last_was_row)
        cnt = Count_setval_in_Col(pb, prc->row, prc->col, setval);
    else
        cnt = Count_setval_in_Row(pb, prc->row, prc->col, setval);
    sprintf(tb,"Scanning %s with %d cands %s", (last_was_row ? "COL" : "ROW"), cnt, (cflg & cf_XCA ? "A" : "B") );
    OUTITXC(tb);
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if (!ps->val[setval - 1]) continue; // does NOT have the candidate
            if (Row_Col_in_RC_Vector(members, row, col)) continue; // alrad in chain
            box = GETBOX(row,col);
            if (in_debug_set) {
                DBGSTOP(8,9);
            }
            if (last_was_row) {
                if ( !(col == prc->col) ) continue;
            } else {
                if ( !(row == prc->row) ) continue;
            }
            //if ( !((box == mbox)||(row == prc->row)||(col == prc->col)) ) continue;
            rc.row = row;
            rc.col = col;
            slflg = Get_RC_Strong_Link_Flag(pb, &rc, prc, setval);
            //if (dep & 1) { // looking for weak link
            //    if (slflg) continue;
            //} else { // need a STRONG link
            //    if ( !(slflg & (cl_SLR | cl_SLC)) ) continue;
            //}
            // got another member
            COPY_SET(&rc.set,ps);
            rc.set.flag[setval - 1] |= (cflg | slflg);   // set A color on this candidate
            members.push_back(rc);
            if (slflg & (cl_SLR | cl_SLC))
                pch->sl_count++;
            count = pch->count;
            cnt = count;
            cnt++;
            pch->count = cnt;
            pch->dep++;
            pch->last_was_row = last_was_row ? false : true;
            val = Do_XCycles_Chain(pch, members);
            //val = Do_XCycles_Chain(pb, &rc, setval, members, ch_num, dep+1, pcount);
            cnt = pch->count;
            sprintf(tb,"scan from %s %s ",
                Get_RC_RCB_Stg(prc), ((prc->set.flag[setval - 1] & cf_XCA) ? "A" : "B") );
            sprintf(EndBuf(tb),"to %s %s ",
                Get_RC_RCB_Stg(&rc),
                ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"));
            sprintf(EndBuf(tb),"ret %d count %d", val, cnt );
            OUTITXC(tb);
            if (val < 0) {
                if ((cnt < min_chain_members) || (pch->sl_count < min_sl_count)) {
                    pch->count = count;     // restart counter
                    pch->dep = dep;         // back up depth
                    pch->sl_count = sl_count; // back up SL count
                    members.pop_back();     // remove last member
                    continue;
                }
            }
            return 0;
        }
    }
    return -1; // no new member found on this trail ;=((
}

int Do_XCycles_Chain_STRONG( PCHNSTR pch, vRC &members )
{
    int count = 0;
    PABOX2 pb = pch->pb;
    PROWCOL prc = pch->prc;
    int setval = pch->setval;
    time_t ch_num = pch->ch_num;
    int dep = pch->dep;
    int row, col, box, val, cnt;
    PSET ps;
    ROWCOL rc;
    uint64_t slflg;
    int mbox = GETBOX(prc->row, prc->col);
    uint64_t cflg = (prc->set.flag[setval - 1] & cf_XCA) ? cf_XCB : cf_XCA;
    char *tb = GetNxtBuf();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if (!ps->val[setval - 1]) continue; // does NOT have the candidate
            if (Row_Col_in_RC_Vector(members, row, col)) continue; // alrad in chain
            box = GETBOX(row,col);
            if ( !((box == mbox)||(row == prc->row)||(col == prc->col)) ) continue;
            rc.row = row;
            rc.col = col;
            slflg = Get_RC_Strong_Link_Flag(pb, &rc, prc, setval);
            if (dep & 1) { // looking for weak link
                if (slflg) continue;
            } else { // need a STRONG link
                if ( !(slflg & (cl_SLR | cl_SLC)) ) continue;
            }
            // got another member
            COPY_SET(&rc.set,ps);
            rc.set.flag[setval - 1] |= (cflg | slflg);   // set A color on this candidate
            members.push_back(rc);
            count = pch->count;
            cnt = count;
            cnt++;
            pch->count = cnt;
            pch->dep++;
            val = Do_XCycles_Chain(pch, members);
            //val = Do_XCycles_Chain(pb, &rc, setval, members, ch_num, dep+1, pcount);
            cnt = pch->count;
            sprintf(tb,"scan from %s %s ",
                Get_RC_RCB_Stg(prc), ((prc->set.flag[setval - 1] & cf_XCA) ? "A" : "B") );
            sprintf(EndBuf(tb),"to %s %s ",
                Get_RC_RCB_Stg(&rc),
                ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"));
            sprintf(EndBuf(tb),"ret %d count %d", val, cnt );
            OUTITXC(tb);
            if (val < 0) {
                if (cnt < min_chain_members) {
                    pch->count = count;     // restart counter
                    pch->dep = dep;         // back up depth
                    members.pop_back();     // remove last member
                    continue;
                }
            }
            return 0;
        }
    }
    return -1; // no new member found on this trail ;=((
}

int Process_SW_Chains_NO_GO(PABOX2 pb)
{
    int count = 0;
    size_t max = sl_pairs.size();
    size_t ii;
    vRCP sla;   // Alternating S->W->S->... CHAIN
    vRCP wla;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    int setval, lval;
    bool findweak;
    count = (int)max;
    for (lval = 0; lval < 9; lval++) {
        setval = lval + 1;
        for (ii = 0; ii < max; ii++) {
            prcp1 = &sl_pairs[ii]; // get OUT a pair
            prc1 = &prcp1->rowcol[0];
            if ( !(prc1->set.uval == setval) ) continue;
            prc2 = &prcp1->rowcol[1];
            if ( !((prc1->set.flag[lval] & cl_SLA)&&(prc2->set.flag[lval] & cl_SLA)) ) continue;
            sla.push_back(*prcp1);
            findweak = true;
            wla.clear();
            Find_Next_Link( sla, wla, setval, prc1, prc2, findweak );
            while (wla.size()) {
                size_t max2 = wla.size();
                size_t jj;
                vRCP wla2;
                findweak = !findweak;
                wla2.clear();
                for (jj = 0; jj < max2; jj++) {
                    prcp1 = &wla[jj]; // get OUT a pair
                    prc1 = &prcp1->rowcol[0];
                    prc2 = &prcp1->rowcol[1];
                    Find_Next_Link( sla, wla2, setval, prc1, prc2, findweak );
                }
                wla.clear();
                max2 = wla2.size();
                for (jj = 0; jj < max2; jj++) {
                    wla.push_back(wla2[jj]);
                }
            }
        }
    }
    max = sla.size();
    if (max) {
        // if we FOUND alternating then pass back that
        sl_pairs.clear();
        for (ii = 0; ii < max; ii++)
            sl_pairs.push_back(sla[ii]);
        count = (int)max;
    }
    return count;
}

int Find_Next_Link_FAILED( vRCP &sla, vRCP &wla, int setval, PROWCOL prc1, PROWCOL prc2, bool findweak )
{
    int count = 0;
    int lval = setval - 1;
    size_t max = sl_pairs.size();
    size_t jj;
    PRCPAIR prcp2;
    PROWCOL prc3, prc4;
    bool fnd;
    for (jj = 0; jj < max; jj++) {
        prcp2 = &sl_pairs[jj]; // get OUT a pair
        if (prcp_in_vrcp(prcp2,sla)) continue;
        prc3 = &prcp2->rowcol[0];
        if ( !(prc3->set.uval == setval) ) continue;
        prc4 = &prcp2->rowcol[1];
        if (findweak) {
            if ((prc3->set.flag[lval] & cl_SLA)||(prc4->set.flag[lval] & cl_SLA)) continue;
        } else {
            if ( !((prc3->set.flag[lval] & cl_SLA)&&(prc4->set.flag[lval] & cl_SLA)) ) continue;
        }
        fnd = false;
        if ((prc1->row == prc3->row)&&(prc1->col == prc3->col))
            fnd = true;
        else if ((prc1->row == prc4->row)&&(prc1->col == prc4->col))
            fnd = true;
        else if ((prc2->row == prc3->row)&&(prc2->col == prc3->col))
            fnd = true;
        else if ((prc2->row == prc4->row)&&(prc2->col == prc4->col))
            fnd = true;
        if (fnd) {
            sla.push_back(*prcp2);
            bool ws = (findweak ? false : true);
            vRCP wla2;
            int cnt = Find_Next_Link_FAILED( sla, wla2, setval, prc3, prc4, ws );
            if (!cnt) {
                sla.pop_back();
            } else {
                count++;
                wla.push_back(*prcp2);
            }
        }
    }
    return count;
}

// Process EACH Candidate, find an EMPTY cell with this candidate
// then find a 2nd with a STRONG link to this cell
// then scan for others with this candidate
// BUT this scans for ALL links - NOT WHAT X-Cycles IS
int Do_XCycles_Scan_ALL_NOT_USED( PABOX2 pb )
{
    int count = 0;
    int row, col, box, val, i, setval, cnt, fcol;
    int row2, col2, box2;
    int scnt;
    ROWCOL rc, rc2;
    PSET ps, ps2;
    vRC *pvrc;
    bool good;
    uint64_t slflg;
    char *tb = GetNxtBuf();
    // for EACH candidate
    time_t ch_num;
    int ccnt;
#ifndef DO_IMMED_SCAN
    vRC todo;
#endif
    for (i = 0; i < 9; i++) {
        pvrc = get_XCycles_pvrc(i);
        pvrc->clear();
        setval = i + 1;
        // find a cell (spot)
        ch_num = 1; // start chain number for this candidate
        sprintf(tb,"Processing %d ",setval);
        ccnt = 0;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = pb->line[row].val[col];
                if (val) continue;
                ps = &pb->line[row].set[col];
                if (!ps->val[i]) continue; // does NOT have candidate
                rc.row = row;
                rc.col = col;
                //  if (i == 0) DBGSTOP(4,4);
                box = GETBOX(row,col);
                // now find a SECOND
                good = false; // may NEVER find one
                slflg = 0;
#ifndef DO_IMMED_SCAN
                todo.clear();
#endif
                // continue onwards finding 2nds...
                fcol = col + 1;
                cnt = 0;
                for (row2 = row; row2 < 9; row2++) {
                    for (col2 = fcol; col2 < 9; col2++) {
                        // continue scan for a SL on this candidate
                        val = pb->line[row2].val[col2];
                        if (val) continue;
                        ps2 = &pb->line[row2].set[col2];
                        if (!ps2->val[i]) continue; // does NOT have this candidate
                        // FOUND a second with this candidate
                        // DBGSTOPRC(row2,8,col2,4);
                        box2 = GETBOX(row2,col2);
                        if ( !((box2 == box)||(row2 == row)||(col2 == col)) ) continue;
                        // they share a BOX ROW COL
                        rc2.row = row2;
                        rc2.col = col2;
                        slflg = Get_RC_Strong_Link_Flag(pb,&rc,&rc2,setval);
                        if (!slflg) continue;
                        if (!good) {
                            good = true;
                            // start of a CHAIN
                            COPY_SET((void *)&rc.set,ps);
                            rc.set.tm = ch_num;  // chain number
                            rc.set.flag[i] |= (cf_XCA | slflg);   // set A color on this candidate
                        }
                        COPY_SET(&rc2.set,ps2);
                        rc2.set.tm = ch_num;  // chain number
                        rc2.set.flag[i] |= (cf_XCB | slflg);   // set A color on this candidate
                        // stack these so they will NOT be found again
                        pvrc->push_back(rc);
                        pvrc->push_back(rc2);
                        count++;
                        ccnt++;
                        // got FIRST in chain - get chain from this
                        sprintf(tb,"Join %d:%d:0: %s A to %s B SL", setval, (int)ch_num,
                            Get_RC_RCB_Stg(&rc),
                            Get_RC_RCB_Stg(&rc2) );
                        OUTITXC(tb);
#ifdef DO_IMMED_SCAN
                        uint64_t cflg = rc.set.flag[setval - 1]; 
                        scnt = Do_XCycles_Scan2(pb, &rc, setval, pvrc, ch_num, 1);
                        sprintf(tb,"Scan from %s %s got %d ", Get_RC_RCB_Stg(&rc),
                                ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"),
                                scnt );
                        OUTITXC(tb);
                        count += scnt;
                        cnt++;
#else
                        todo.push_back(rc2);    // keep second, to follow chain from here
#endif // DO_IMMED_SCAN
                    }   // for (col2 = col + 1; col2 < 9; col2++
                    fcol = 0;
                }   // for (row2 = row; row2 < 9; row2++) // each row onwards
#ifndef DO_IMMED_SCAN
                size_t max = todo.size();
                size_t j;
                cnt = 0;
                for (j = 0; j < max; j++) {
                    rc = todo[j];
                    uint64_t cflg = rc.set.flag[setval - 1]; 
                    scnt = Do_XCycles_Scan2(pb, &rc, setval, pvrc, ch_num, 1);
                    sprintf(tb,"Scan from %s %s got %d ", Get_RC_RCB_Stg(&rc),
                            ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"),
                            scnt );
                    OUTITXC(tb);
                    cnt += scnt;
                }
                count += cnt;
                if (max)
                    ch_num++;   // bump chain number
#else
                if (cnt)
                    ch_num++; // bump chain number
#endif
            }   // for (col = 0; col < 9; col++) // for each COL
        }   // for (row = 0; row < 9; row++) // for each ROW
    }   // for (i = 0; i < 9; i++) // for each candidate
    return count;
}





// ******************************************************
// Add a Hidden Triple Scan
int Mark_Hidden_Triple_OK(PABOX2 pb)
{
    int count = 0;
    int count2 = 0;
    int row, col, cnt;
    RCRCB rcrcb;
    vRC empty;
    cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    vRC *pvrc;
    size_t max, ii, i2, i3, i4;
    //ROWCOL rc[9];
    SET set[9];
    int setvals[9];
    int setvaldone[9];
    int v, v2, setval, val, scnt;
    PSET ps, ps1, ps2, ps3;
    PROWCOL prc1, prc2, prc3, prc4;
    uint64_t *pi64;
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

    }
    sprtf("View Hidden Triple %d\n", count);
    if (count || count2)
        Stack_Fill(Do_Clear_HTrip);
    return count;
}



// ===================================================
// another try at color scanning
int Do_Color_Scan2_NEARLY( PABOX2 pb )
{
    int count = 0;
    int row, col, box, val, i, setval;
    int row2, col2, box2;
    ROWCOL rc, rc2;
    PSET ps, ps2;
    int r, c, rw, cl;
    PCLRSTR pclrstr;
    sprtf("Doing Color Scan 2 for %d\n", do_color_test2);
    for (i = 0; i < 9; i++) {
        pclrstr = &clrstr[i];
        clear_color_str( pclrstr );
    }
    int bgn_off, off;
    RCPAIR rcpair;
    char *tb = GetNxtBuf();
    for (i = 0; i < 9; i++) {
        setval = i + 1;
        if (do_color_test2 && (setval != do_color_test2)) continue; // do for just this value
        pclrstr = &clrstr[i];
        pclrstr->setval = setval; // value being chained
        clr_set.val[i] = setval;  // done this setval
        off = bgn_off = 0; // start accumulation
        pclrstr->pb = pb;
        pclrstr->ch_num = 1;
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
                pclrstr->members.push_back(rc);
                pclrstr->prcFirst = &rc;
#ifndef FOLLOW_EACH_AS_FOUND
                sprintf(tb,"Join %s ", Get_RC_RCB_Stg(&rc));
#endif
                for (row2 = 0; row2 < 9; row2++) {
                    if (row2 == row) continue;
                    if (Row_Col_in_RC_Vector( pclrstr->members, row2, col )) continue;
                    val = pb->line[row2].val[col];
                    if (val) continue;
                    ps2 = &pb->line[row2].set[col];
                    if ( !( ps2->val[setval - 1] ) ) continue;
                    box2 = GETBOX(row2,col);
                    if (first_pair_not_box && (box2 == box)) continue;
                    rc2.row = row2;
                    rc2.col = col;
                    COPY_SET(&rc2.set,ps2);
                    rc2.set.flag[setval - 1] |= cf_XCB; // set COLOR B
                    rc2.set.tm = pclrstr->ch_num;
                    pclrstr->members.push_back(rc2);
                    rcpair.rowcol[0] = rc;
                    rcpair.rowcol[1] = rc2;
                    pclrstr->rcpairs.push_back(rcpair);
#ifdef FOLLOW_EACH_AS_FOUND
                    sprintf(tb,"Join %s ", Get_RC_RCB_Stg(&rc));
                    sprintf(EndBuf(tb),"R %s ", Get_RC_RCB_Stg(&rc2));
                    OUTITCS2(tb);

                    pclrstr->flag = cl_LBR; // last link in ROW
                    pclrstr->prc = &rc2;
                    Follow_Color_Chain( pclrstr );
                    pclrstr->ch_num++;      // BUMP chain number
#else
                    sprintf(EndBuf(tb),"C %s ", Get_RC_RCB_Stg(&rc2));
                    pclrstr->rcset[off++] = rc2;
#endif
                }
                for (col2 = 0; col2 < 9; col2++) {
                    if (col2 == col) continue;
                    if (Row_Col_in_RC_Vector( pclrstr->members, row, col2 )) continue;
                    val = pb->line[row].val[col2];
                    if (val) continue;
                    ps2 = &pb->line[row].set[col2];
                    if ( !( ps2->val[setval - 1] ) ) continue;
                    box2 = GETBOX(row,col2);
                    if (first_pair_not_box && (box2 == box)) continue;
                    rc2.row = row;
                    rc2.col = col2;
                    COPY_SET(&rc2.set,ps2);
                    rc2.set.flag[setval - 1] |= cf_XCB; // set COLOR B
                    rc2.set.tm = pclrstr->ch_num;
                    pclrstr->members.push_back(rc2);
                    rcpair.rowcol[0] = rc;
                    rcpair.rowcol[1] = rc2;
                    pclrstr->rcpairs.push_back(rcpair);
#ifdef FOLLOW_EACH_AS_FOUND
                    sprintf(tb,"Join %s ", Get_RC_RCB_Stg(&rc));
                    sprintf(EndBuf(tb),"R %s ", Get_RC_RCB_Stg(&rc2));
                    OUTITCS2(tb);

                    pclrstr->flag = cl_LBC; // last link in COL
                    pclrstr->prc = &rc2;
                    Follow_Color_Chain( pclrstr );
                    pclrstr->ch_num++;      // BUMP chain number
#else
                    sprintf(EndBuf(tb),"R %s ", Get_RC_RCB_Stg(&rc2));
                    pclrstr->rcset[off++] = rc2;
#endif
                }
                if (!first_pair_not_box) {
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
                            rc2.set.flag[setval - 1] |= cf_XCB; // set COLOR B
                            rc2.set.tm = pclrstr->ch_num;
                            pclrstr->members.push_back(rc2);
                            rcpair.rowcol[0] = rc;
                            rcpair.rowcol[1] = rc2;
                            pclrstr->rcpairs.push_back(rcpair);

#ifdef FOLLOW_EACH_AS_FOUND
                            sprintf(tb,"Join %s ", Get_RC_RCB_Stg(&rc));
                            sprintf(EndBuf(tb),"B %s ", Get_RC_RCB_Stg(&rc2));
                            OUTITCS2(tb);
                            pclrstr->flag = cl_LBB; // last link in BOX
                            pclrstr->prc = &rc2;
                            Follow_Color_Chain( pclrstr );
                            pclrstr->ch_num++;      // BUMP chain number
#else
                            sprintf(EndBuf(tb),"B %s ", Get_RC_RCB_Stg(&rc2));
                            pclrstr->rcset[off++] = rc2;
#endif
                        }
                    }
                }

#ifndef FOLLOW_EACH_AS_FOUND
                if (off <= bgn_off) continue;
                sprintf(EndBuf(tb),"cnt %d ", off - bgn_off);
                OUTITCS2(tb);
                pclrstr->bgn_off = bgn_off; // from here
                pclrstr->nxt_off = off;     // to here
                Follow_Color_Chain( pclrstr );
                off = pclrstr->nxt_off;     // get next begin
                bgn_off = off;              // and set it
                pclrstr->ch_num++;      // BUMP chain number
#endif
            }
            //if (off)
            //    break;
        }
        count += (int)pclrstr->members.size();
        sprtf("For setval %d created %d links\n", setval, count);
        if (count)
            set_repaint();
    }
    return count;

}


void paint_color_test2_PREV( HDC hdc, int setval ) // if (do_color_test2)
{
    int i = setval - 1;
    size_t max, j, k;
    vRC *pvrc = Get_Color2_Members(setval); // ONLY in paint_color_test2_PREV()
    if (!pvrc)
        return;
    max = pvrc->size();
    if (max < 4)
        return;

    ROWCOL rc, rc2;
    int count = 0;
    int row, col, box;
    int row2, col2, box2;
    PROWCOL prc1,prc2;
    PABOX2 pb = get_curr_box();
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    int bcnt, rcnt, ccnt;
    for (j = 0; j < max - 1; j++) {
        k = j + 1;
        rc = pvrc->at(j);
        rc2 = pvrc->at(k);
        prc1 = &rc;
        prc2 = &rc2;
        if (rc2.set.tm != rc.set.tm) continue;
        if (!SAMERCB(prc1,prc2)) continue;

        row = rc.row;
        col = rc.col;
        box = GETBOX(row,col);
        row2 = rc2.row;
        col2 = rc2.col;
        box2 = GETBOX(row2,col2);

        bcnt = 0;
        ccnt = 0;
        rcnt = 0;
        SelectObject(hdc,hpwlnk); // set weak link
        if (box2 == box) {
            // SAME box
            // if (setval == 7) DBGSTOP2(row,1,col,5,row2,3,col2,4);
            bcnt = Count_setval_in_Box(pb,row,col, setval );
            if (bcnt == 2)
                SelectObject(hdc,hpslnk);
        } else {
            // not in SAME box
            if (col2 == col) {
                ccnt = Count_setval_in_Col(pb, row, col, setval );
                if (ccnt == 2)
                    SelectObject(hdc,hpslnk);
            } else if (row2 == row) {
                rcnt = Count_setval_in_Row(pb, row, col, setval );
                if (rcnt == 2)
                    SelectObject(hdc,hpslnk);
            }
        }
        draw_link(hdc, &rc, &rc2, i + 1);
        count++;
    }
    sprtf("Painted %d Color links\n", count);
}

int Mark_Color2_Elims_OK ( PCLRSTR pclrstr ) // all the information is in this structure
{
    int count = 0;
    PABOX2 pb = pclrstr->pb;
    RCPAIR rcpair;
    int setval = pclrstr->setval;
    size_t i, jj, max, max2, k, max3;
    ROWCOL rc, rc1, rc2;
    time_t ch_num;
    vTIMET vtt;
    vRC members, nonmembers;
    //RCRCB rcrcb;
    RCRCB memrcb;
    vRC empty;
    vRC *pvrow;
    vRC *pvcol;
    vRC *pvbox;
    uint64_t flg;
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
        members.clear();
        nonmembers.clear();
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
        }
        // got list of members for this CHAIN
        if (members.size() < 4) continue; // no interest in small chains
        // got a CHAIN of members... - and nonmembers to skip
        // got candidate set up by ROW, COL and BOX
        // =========================================
        sprintf(tb,"Processing chain %d, with %d members, %d non", (int)jj + 1,
            (int) members.size(), (int)nonmembers.size() );
        OUTITCS2(tb);

        // get members of the chain into ROW, COL and BOX
        RC_Vector_to_RCB( members, &memrcb );
        //int cntr[2][3][9];
        //memset(cntr,0,sizeof(cntr));
        int rcnta[9];
        int rcntb[9];
        int ccnta[9];
        int ccntb[9];
        int bcnta[9];
        int bcntb[9];
        uint64_t flag[9];

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
                            pb->line[rc.row].set[rc.col].flag[lval] |= cf_CCE;
                            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg( &rc, setval ));
                            count++;
                        }
                    }
                }
                OUTITCS2(tb);
            }
        }

        if (!count) {
            // RULE 1 - Any candidate that can be seens by BOTH A and B colors
            // TODO...
            // Search for empty, with this cand exposed to opposite colors by members of the chain
            max3 = empty.size();
            size_t max4, m4, max5, m5;
            sprintf(tb,"Elim1 ");
            char *tb2 = GetNxtBuf();
            uint64_t flag2;
            for (k = 0; k < max3; k++) {
                rc = empty[k]; // get an empty cell
                if (RC_in_RC_Vector(members,&rc)) continue; // skip - member of chain
                if (RC_in_RC_Vector(nonmembers,&rc)) continue; // NOT SURE - skip - member of other chains
                // got a cell with setval, NOT part of a chain - is it exposed to
                // two members of this chain with opposing colors?
                pvrow = &memrcb.vrows[rc.row];  // get chain members in SAME ROW as this cell
                pvcol = &memrcb.vcols[rc.col];  // get chain members in SAME COL as this cell
                pvbox = &memrcb.vboxs[GETBOX(rc.row,rc.col)]; // do I need BOX???
                max4 = pvrow->size();
                max5 = pvcol->size();
                sprintf(tb2,"Chk %s ", Get_RC_setval_RC_Stg( &rc, setval ));
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
                                    pb->line[rc.row].set[rc.col].flag[lval] |= cf_CCE;
                                    sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg( &rc, setval ));
                                    count++;
                                }
                            }
                        }
                    }
                }
                OUTITCS2(tb2);
            }
            if (count) OUTITCS2(tb);

        }   // no count from RULE 2
    }   // Deal CHAIN by CHAIN for (j = 0; j < max2; j++)


    return count;
}




// ===================================================


// Maybe put all the empty cells in a vector list by set value?

int Do_XCycles_Scan_IDEA3( PABOX2 pb )
{
    int count = 0;
    int row, col, val, i, setval, box;
    ROWCOL rc;
    PSET ps;
    vRC by_setval[9];
    RCRCB rcrcb;
    vRC *pvrc;
    size_t max, j;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            rc.row = row;
            rc.col = col;
            box = GETBOX(row,col);
            COPY_SET(&rc.set,ps);
            rc.set.uval = -1;    // set NO COLOR
            rcrcb.vboxs[box].push_back(rc);
            rcrcb.vrows[row].push_back(rc);
            rcrcb.vcols[col].push_back(rc);
            for (i = 0; i < 9; i++) {
                setval = ps->val[i];
                if (!setval) continue;
                val = setval - 1;
                by_setval[val].push_back(rc);
            }
        }
    }
    // got by ROW, COL, BOX, and by setval
    for (i= 0; i < 9; i++) {
        pvrc = &by_setval[i]; // get SETS with this setval
        max = pvrc->size();
        if (max < 4) continue;
        for (j = 0; j < max; j++) {
            rc = pvrc->at(j);
            row = rc.row;
            col = rc.col;
            box = GETBOX(row,col);

        }
    }
    return count;
}


int Do_XCycles_Scan2_GETTING_CLOSE( PABOX2 pb, PROWCOL prc, int setval, vRC *pvrc, time_t ch_num )
{
    int count = 0;
    int row, col, val, box2, cnt;
    ROWCOL rc;
    PSET ps;
    size_t min = pvrc->size();
    int box = GETBOX(prc->row,prc->col);
    bool isaclr = (prc->set.flag[setval - 1] & cf_XCA) ? true : false;
    uint64_t cflg = isaclr ? cf_XCB : cf_XCA;
    char *tb = GetNxtBuf();
    uint64_t slflg;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            box2 = GETBOX(row,col);
            ps = &pb->line[row].set[col];
            if ( !(ps->val[setval - 1]) ) continue;
            if (RC_in_pvrc(pvrc, row, col)) continue;
            if ( !((row == prc->row)||(col == prc->col)||(box2 == box)) ) continue;
            // is SAME ROW, COL or BOX - add to chain
            // IF A STRONG LINK
            if (check_strong_link) {
                if (box2 == box) {
                    cnt = Count_setval_in_Box( pb, row, col, setval );
                    if ( !(cnt == 2) ) continue;
                }
                if (row == prc->row) {
                    cnt = Count_setval_in_Row( pb, row, col, setval );
                    if ( !(cnt == 2) ) continue;
                }
                if (col == prc->col) {
                    cnt = Count_setval_in_Col( pb, row, col, setval );
                    if ( !(cnt == 2) ) continue;
                }
            }
            rc.row = row;
            rc.col = col;
            COPY_SET((void *)&rc.set,(const void *)ps);
            // COLOR set per candidate as a flag!!! rc.set.uval = prc->set.uval ? 0 : 1;    // set COLOR
            slflg = Get_RC_Strong_Link_Flag(pb,prc,&rc,setval);
            rc.set.flag[setval -1] |= (cflg & slflg);
            if (slflg)
                prc->set.flag[setval - 1] |= slflg;
            rc.set.tm = ch_num;
            sprintf(tb,"Join %d: %s %s to %s %s %s", setval,
                Get_RC_RCB_Stg(prc), (isaclr ? "A" : "B"),
                Get_RC_RCB_Stg(&rc), (isaclr ? "B" : "A"),
                (slflg ? "SL" : "WL") );
            OUTITXC(tb);
            pvrc->push_back(rc);
            count++;
        }
    }
    // now follow the chain - much like simple coloring
    size_t max = pvrc->size();
    size_t j;
    for (j = min; j < max; j++) {
        rc = pvrc->at(j);
        cnt = Do_XCycles_Scan2( pb, &rc, setval, pvrc, ch_num );
    }
    return count;
}
int Do_XCycles_Scan2_MAYBE( PABOX2 pb, PROWCOL prc, int setval, vRC *pvrc, time_t ch_num )
{
    int count = 0;
    int row, col, val, box2, cnt;
    ROWCOL rc;
    PSET ps;
    int box = GETBOX(prc->row,prc->col);
    bool isaclr = (prc->set.flag[setval - 1] & cf_XCA) ? true : false;
    uint64_t cflg = isaclr ? cf_XCB : cf_XCA;
    char *tb = GetNxtBuf();
    uint64_t slflg;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            box2 = GETBOX(row,col);
            ps = &pb->line[row].set[col];
            if ( !(ps->val[setval - 1]) ) continue;
            if (RC_in_pvrc(pvrc, row, col)) continue;
            if ( !((row == prc->row)||(col == prc->col)||(box2 == box)) ) continue;
            // is SAME ROW, COL or BOX - add to chain
            // IF A STRONG LINK
            if (check_strong_link) {
                if (box2 == box) {
                    cnt = Count_setval_in_Box( pb, row, col, setval );
                    if ( !(cnt == 2) ) continue;
                }
                if (row == prc->row) {
                    cnt = Count_setval_in_Row( pb, row, col, setval );
                    if ( !(cnt == 2) ) continue;
                }
                if (col == prc->col) {
                    cnt = Count_setval_in_Col( pb, row, col, setval );
                    if ( !(cnt == 2) ) continue;
                }
            }
            rc.row = row;
            rc.col = col;
            COPY_SET((void *)&rc.set,(const void *)ps);
            // COLOR set per candidate as a flag!!! rc.set.uval = prc->set.uval ? 0 : 1;    // set COLOR
            slflg = Get_RC_Strong_Link_Flag(pb,prc,&rc,setval);
            rc.set.flag[setval -1] |= (cflg & slflg);
            if (slflg)
                prc->set.flag[setval - 1] |= slflg;
            rc.set.tm = ch_num;
            sprintf(tb,"Join %d: %s %s to %s %s %s", setval,
                Get_RC_RCB_Stg(prc), (isaclr ? "A" : "B"),
                Get_RC_RCB_Stg(&rc), (isaclr ? "B" : "A"),
                (slflg ? "SL" : "WL") );
            OUTITXC(tb);
            pvrc->push_back(rc);
            count++;
            count += Do_XCycles_Scan2( pb, &rc, setval, pvrc, ch_num );
        }
    }
    return count;
}

// Find an EMPTY cell, and process its candidates
int Do_XCycles_Scan_IDEA_1( PABOX2 pb )
{
    int count = 0;
    int row, col, val, i, setval, cnt;
    ROWCOL rc;
    PSET ps;
    vRC *pvrc;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            rc.row = row;
            rc.col = col;
            COPY_SET(&rc.set,ps);
            rc.set.uval = 0;    // set COLOR
            rc.set.tm = 0;  // chain number
            for (i = 0; i < 9; i++) {
                setval = ps->val[i];
                if (!setval) continue;
                rc.set.flag[i] |= cf_XCA;   // set A color on this candidate
                val = setval - 1;
                pvrc = get_XCycles_pvrc(val);
                pvrc->push_back(rc);
                // got FIRST in chain - get chain from this
                cnt = Do_XCycles_Scan2(pb, &rc, setval, pvrc, 1);
                // Now how to SORT this
            }
        }
    }
    return count;
}

int Do_XCycles_Scan_MAYBE2( PABOX2 pb )
{
    int count = 0;
    int row, col, box, val, i, setval, cnt;
    int row2, col2, box2, fcol;
    ROWCOL rc, rc2;
    PSET ps, ps2;
    vRC *pvrc;
    bool good;
    uint64_t slflg;
    char *tb = GetNxtBuf();
    // for EACH candidate
    time_t ch_num;
    for (i = 0; i < 9; i++) {
        setval = i + 1;
        // find a cell (spot)
        ch_num = 0;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = pb->line[row].val[col];
                if (val) continue;
                ps = &pb->line[row].set[col];
                if (!ps->val[i]) continue; // does NOT have candidate
                rc.row = row;
                rc.col = col;
                if (i == 0) DBGSTOP(4,4);
                box = GETBOX(row,col);
                // now find a SECOND
                good = false; // may NEVER find one
                slflg = 0;
                fcol = col + 1;
                for (row2 = row; row2 < 9; row2++) {
                    for (col2 = fcol; col2 < 9; col2++) {
                        // continue scan for a SL on this candidate
                        val = pb->line[row2].val[col2];
                        if (val) continue;
                        ps2 = &pb->line[row2].set[col2];
                        if (!ps2->val[i]) continue; // does NOT have this candidate
                        // FOUND a second with this candidate
                        box2 = GETBOX(row2,col2);
                        if ( !((box2 == box)||(row2 == row)||(col2 == col)) ) continue;
                        // they share a BOX ROW COL
                        rc2.row = row2;
                        rc2.col = col2;
                        slflg = Get_RC_Strong_Link_Flag(pb,&rc,&rc2,setval);
                        if (!slflg) continue;
                        good = true;
                        break;
                    }
                    fcol = 0; // reset start column
                    if (good)
                        break;
                }
                if (!good) continue; // back to search for FIRST with this candidate
                // start of a CHAIN
                ch_num++;
                COPY_SET((void *)&rc.set,ps);
                rc.set.tm = ch_num;  // chain number
                rc.set.flag[i] |= (cf_XCA | slflg);   // set A color on this candidate
                COPY_SET(&rc2.set,ps2);
                rc.set.tm = ch_num;  // chain number
                rc2.set.flag[i] |= (cf_XCB | slflg);   // set A color on this candidate

                val = setval - 1;
                // stack these so they will NOT be found again
                pvrc = get_XCycles_pvrc(val);
                pvrc->push_back(rc);
                pvrc->push_back(rc2);
                // got FIRST in chain - get chain from this
                sprintf(tb,"Join 1st %d: %s A to %s B SL", setval,
                    Get_RC_RCB_Stg(&rc),
                    Get_RC_RCB_Stg(&rc2) );
                OUTITXC(tb);
                cnt = Do_XCycles_Scan2(pb, &rc, setval, pvrc, ch_num);
            }
        }
    }
    return count;
}




uint64_t Get_RC_Strong_Link_Flag_OK_BUT( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, int setval )
{
    if (SAMERCCELL(prc1,prc2)) {
        return 0;
    } else if (GETBOX(prc1->row,prc1->col) == GETBOX(prc2->row,prc2->col)) {
        // SAME BOX
        if (Count_setval_in_Box(pb,prc1->row,prc1->col,setval) == 2)
            return cl_SLB;
    } else if (prc1->row == prc2->row) {
        // SAME ROW
        if (Count_setval_in_Row(pb,prc1->row,prc1->col,setval) == 2)
            return cl_SLR;
    } else if (prc1->col == prc2->col) {
        // SAME COL
        if (Count_setval_in_Col(pb,prc1->row,prc1->col,setval) == 2)
            return cl_SLC;
    }
    return 0;
}


// ===================================================


// ===============================================
int Elim_SET_in_SET_NOT_USED( PSET ps, PSET ps2, bool del = false );

int Elim_SET_in_SET_NOT_USED( PSET ps, PSET ps2, bool del )
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

// ====================================================================================
// ------------------------------------------------------------------------------------
static HWND hDlgList = 0;
static size_t msg_count = 0;
BOOL Do_INIT_MsgDialog2(HWND hDlg)
{
    BOOL bRet = TRUE;
    hDlgList = GetDlgItem(hDlg,IDC_LIST1);
    if (hDlgList) {
        HFONT hf = Get_Font();
        if (hf) SendMessage(hDlgList, WM_SETFONT, (WPARAM)hf, TRUE);

        char *hdg = "Output Strings";
        char *stgs[1];
        stgs[0] = hdg;
        if ( LV_InitColumns( hDlgList, 1, stgs) ) {
            bRet = TRUE;
        }
    }
    return bRet;
}

#ifndef MAX_STRING
#define MAX_STRING 512
#endif

VOID Do_Msg_Paint2(HWND hDlg)
{
    char buf[MAX_STRING+2];
    vSTG *vlp = Get_Screen_Lines();
    size_t max = vlp->size();
    size_t i = msg_count;
    if ((max > i) && hDlgList) {
        for (; i < max; i++) {
            string s = vlp->at(i);  // get string
            char *ps = buf;
            strncpy(ps,s.c_str(),MAX_STRING);
            ps[MAX_STRING-1] = 0;   // ensure zero terminated
            int len = (int)strlen(ps);
            if (len) {
                while(len--) {
                    if (buf[len] > ' ') {
                        len++;
                        break;
                    }
                    buf[len] = 0;
                }
                if (len > 0) {
                    LV_InsertItem(hDlgList, ps);
                }
            }
        }
    }
    msg_count = max;

}

BOOL CALLBACK MsgDialogProc2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
    switch (message) 
    { 
        case WM_INITDIALOG:
            return Do_INIT_MsgDialog2(hDlg);
        case WM_PAINT:
            Do_Msg_Paint(hDlg);
            break;
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case IDOK: 
                    g_bShwMsgD = FALSE;
                    gChgSMsgD = TRUE;
                    Set_Dlg_OnShow(FALSE);
                    ShowWindow(hDlg, SW_HIDE);
                    break;
            } 
    } 
    return FALSE; 
} 

void CreateMessageDialog2( HWND hWnd )
{
    if (!g_hMsgDialog || !IsWindow(g_hMsgDialog)) 
    {
        g_hMsgDialog = CreateDialog(hInst, 
                                   MAKEINTRESOURCE(IDD_LISTBOX), 
                                   hWnd, 
                                   (DLGPROC)MsgDialogProc2 );
        if (g_hMsgDialog)
            ShowWindow(g_hMsgDialog, SW_SHOW);
    }
}


// ------------------------------------------------------------------------------------
// ====================================================================================


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


// ===============================================

// ===============================================
#define PEB2(a,b) sprintf(EndBuf(tb),a,b);
#define OUTITpp(tb) if (add_debug_pp) OUTIT(tb);

int Do_Pointer_Pairs_NOT_USED(PABOX2 pb)
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
                OUTITpp(tb);
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
                OUTITpp(tb);
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


// ===============================================
// ===============================================
// all about color strategy
bool Row_Col_in_Vector( vRC & vrc, int row, int col )
{
    ROWCOL rc;
    vRCi ii = vrc.begin();
    for ( ; ii != vrc.end(); ii++) {
        rc = *ii;
        if ((rc.row == row) && (rc.col == col))
            return true;
    }
    return false;
}

// Eliminate a cells which either
// (1) Do NOT have a 'sharer' of setval in the same ROW, COL or BOX, or
// (2) Have more than ONE 'sharer' of setval in the same ROW, COL, or BOX
int debug_just_2 = 1;
bool Just_2_in_RCB_With_Val(PABOX2 pb, vRC &vrc, int srow, int scol, int setval)
{
    int flag, box, box2;
    ROWCOL rc, rc2;
    size_t max = vrc.size();
    size_t i, j;
    int sbox = GETBOX(srow,scol);
    char *tb = GetNxtBuf();
    // char *tb2 = GetNxtBuf();
    flag = 0;
    // start with one of each has this value, namely the test cell
    int rowcnt = 1;
    int colcnt = 1;
    int boxcnt = 1;
    sprintf(tb,"%d R%dC%dB%d ", setval, srow + 1, scol + 1, sbox + 1);
    for (i = 0; i < max; i++) {
        rc = vrc[i];
        if ((rc.row == srow) && (rc.col == scol)) continue; // skip test cell
        // count how many with setval are in the same COL, ROW or BOX
        box = GETBOX(rc.row,rc.col);
        if ( !((rc.row == srow) || (rc.col == scol) || (box == sbox)) )
                    continue;   // NOT same ROW, COL or BOX as test
        if ( !Value_in_SET( setval, &rc.set )) continue; // does NOT have this candidate
        // have a cell which shares this candidate, and is same R, C, or B as test
        sprintf(EndBuf(tb),"vs R%dC%dB%d ", rc.row + 1, rc.col + 1, box + 1);
        // SHARES a ROW COL or BOX with test
        if (rc.row == srow) {   // SAME ROW as test
            // how many in this ROW with steval - want to find 2
            rowcnt++;   // bump ROW count with this value, asside from test
            // strcat(tb, "SR ");
            for (j = 0; j < max; j++) {
                if (j == i) continue;   // skip this
                rc2 = vrc[j];
                if (rc2.row != srow) continue; // checking only ROW
                // same ROW
                if (rc2.col == scol) continue; // skip test cell
                // same ROW, not cell to be check, not this
                if ( !Value_in_SET( setval, &rc2.set ) ) continue; // does NOT have setval
                rowcnt++;
                if (rowcnt > 2) {
                    if ( !(flag & 1) )
                        strcat(tb,"F ROW >2 ");
                    flag |= 1;  // FAILED on SAME ROW TEST - more than 2
                    break;
                }
            }
        }
        if (rc.col == scol) {
            // how many in this COL with steval
            colcnt++;   // bump COL count with this value, asside from test
            // strcat(tb, "SC ");
            for (j = 0; j < max; j++) {
                if (j == i) continue;   // skip this
                rc2 = vrc[j];
                // box2 = GETBOX(rc2.row,rc2.col);   // get the BOX
                // sprintf(tb2,"R%dC%dB%d", rc2.row + 1, rc2.col + 1, box2 + 1);
                if (rc2.col != scol) continue; // not same COL
                // same COL
                if (rc2.row == srow) continue; // is test cell
                // same COL, not cell to be check, not this
                if ( !Value_in_SET( setval, &rc2.set ) ) continue; // no setval
                colcnt++;
                if (colcnt > 2) {
                    if ( !(flag & 2) )
                        strcat(tb,"F COL >2 ");
                    flag |= 2;  // FAILED on SAME COL TEST - more than 2
                    break;
                }
            }
        }
        if (box == sbox) {
            // how many in this BOX with steval
            boxcnt++;   // bump BOX count with this value, asside from test
            // strcat(tb, "SB ");
            for (j = 0; j < max; j++) {
                if (j == i) continue;   // skip this
                rc2 = vrc[j];
                if ((rc2.row == srow) && (rc2.col == scol)) continue; // skip test cell
                box2 = GETBOX(rc2.row,rc2.col);   // get the BOX
                if (sbox != box2) continue;  // not same BOX
                // same BOX, not cell to be check, not this
                if ( !Value_in_SET( setval, &rc2.set ) ) continue; // no setval
                boxcnt++;
                if (boxcnt > 2) {
                    if ( !(flag & 4) )
                        strcat(tb,"F BOX >2 ");
                    flag |= 4;  // FAILED on SAME BOX TEST - more than 2
                    break; // no need to check moe
                }
            }
        }
        if (flag == (1 | 2 | 4)) {
            strcat(tb," FAILED ALL");
            if (debug_just_2) OUTIT(tb);
            return false;   // FAILED on ALL ROW, COL and BOX tests
        }
    }

    if (rowcnt < 2) {
        strcat(tb,"F ROW <2 ");
        flag |= 1;  // FAILED on SAME ROW TEST - less than 2
    }
    if (colcnt < 2) {
        strcat(tb,"F COL <2 ");
        flag |= 2;  // FAILED on SAME COL TEST - less than 2
    }
    if (boxcnt < 2) {
        strcat(tb,"F BOX <2 ");
        flag |= 4;  // FAILED on SAME BOX TEST - less than 2
    }
    if (flag == (1 | 2 | 4)) {
        strcat(tb," FAILED ALL");
        if (debug_just_2) OUTIT(tb);
        return false;   // FAILED on ALL ROW, COL and BOX tests
    }
    // SUCCESS
    strcat(tb," VIABLE");
    if (debug_just_2) OUTIT(tb);
    return true; // PASSED ROW, COL and BOX test - is viable candidate for COLOR scan
}

int Find_Sharer_Sets_RCB( PABOX2 pb, vRC & vrc, int srow, int scol, int color )
{
    static ROWCOL cumrc[9*9];
    int count = 0;
    int row, col, val;
    int ccnt;
    PSET ps = &pb->line[srow].set[scol];
    SET comm;
    ROWCOL rc;
    PSET ps2;
    int rccnt = 0;
    PROWCOL pcrc;

    // find in COL - fix column - scan rows
    for (row = 0; row < 9; row++) {
        if (row == srow) continue; // skipping self
        val = pb->line[row].val[scol];
        if (val) continue; // only interested in empty cells
        if (Row_Col_in_Vector( vrc, row, scol )) continue; // already have this cell in list
        ps2 = &pb->line[row].set[scol]; // get candidates
        Zero_SET(&comm); // zero set
        ccnt = Get_Shared_SET( ps, ps2, &comm ); // get common
        if (!ccnt) continue; // no shared - skip it
        // save this cell
        rc.row = row;
        rc.col = scol;
        rc.cnt = color;
        COPY_SET( &rc.set, ps2 );
        vrc.push_back(rc);
        pcrc = &cumrc[rccnt++]; // get storage
        *pcrc = rc; // copy into store
        count++;
    }
    // find in ROW - fix ROW, scan cols
    for (col = 0; col < 9; col++) {
        if (col == scol) continue; // skip self
        val = pb->line[srow].val[col];
        if (val) continue; // only empty cells
        if (Row_Col_in_Vector( vrc, srow, col )) continue;
        ps2 = &pb->line[srow].set[col];
        Zero_SET(&comm);
        ccnt = Get_Shared_SET( ps, ps2, &comm );
        if (!ccnt) continue;
        // save this cell
        rc.row = srow;
        rc.col = col;
        rc.cnt = color;
        COPY_SET( &rc.set, ps2 );
        vrc.push_back(rc); // into vector
        count++;
        pcrc = &cumrc[rccnt++];
        *pcrc = rc; // int storeage
    }
    // find in BOX
    int r = (srow / 3) * 3; // get BOX offset
    int c = (scol / 3) * 3;
    int rw, cl;
    // throuhg each cell in BOX 3x3
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw; // compute cell
            col = c + cl;
            if ((rw == srow) && (cl == scol)) continue; // skip self
            val = pb->line[row].val[col];
            if (val) continue; // only wnat empty
            if (Row_Col_in_Vector( vrc, row, col )) continue; // not already stored
            ps2 = &pb->line[row].set[col];
            Zero_SET(&comm); // zero common
            ccnt = Get_Shared_SET( ps, ps2, &comm ); // get common count
            if (!ccnt) continue; // skip nothing in common
            // add cell to vector and storeage
            rc.row = row;
            rc.col = col;
            rc.cnt = color;
            COPY_SET( &rc.set, ps2 );
            vrc.push_back(rc);
            count++;
            pcrc = &cumrc[rccnt++];
            *pcrc = rc;
        }
    }
    // interate through the storeage collected
    for (c = 0; c < rccnt; c++) {
        rc = cumrc[c]; // get storeage
        count += Find_Sharer_Sets_RCB( pb, vrc, rc.row, rc.col,
            (color ? 0 : 1)); // call self...
    }
    return count;
}

int Find_ALL_Sharer_Sets( PABOX2 pb, vRC & vrc, PSET ps, int color );
int Find_ALL_Sharer_Sets( PABOX2 pb, vRC & vrc, PSET ps, int color )
{
    int count = 0;
    int row, col, val;
    int ccnt;
    PSET ps2;
    SET comm;
    ROWCOL rc;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            if (Row_Col_in_Vector( vrc, row, col )) continue;
            ps2 = &pb->line[row].set[col];
            Zero_SET(&comm);
            ccnt = Get_Shared_SET( ps, ps2, &comm );
            if (!ccnt) continue;
            rc.row = row;
            rc.col = col;
            rc.cnt = color;
            COPY_SET( &rc.set, ps2 );
            vrc.push_back(rc);
            count++;
            // count += Find_Sharer_Sets(pb,vrc,ps2,(color ? 0 : 1)); // Find OPPOSITE color
            count += Find_ALL_Sharer_Sets(pb,vrc,ps,color); // Find SAME color
            count += Find_ALL_Sharer_Sets(pb,vrc,ps2,(color ? 0 : 1)); // Find OPPOSITE color
        }
    }
    return count;
}

void Show_Vector( vRC & vrc )
{
    int cnt, box;
    vRCi ii;
    ROWCOL rc, rcp;
    char *tb = GetNxtBuf();
    SET comm;
    //*tb = 0;
    //OUTIT(tb);
    sprintf(tb,"List of %d vectors...", vrc.size());
    OUTIT(tb);
    cnt = 0;
    for (ii = vrc.begin(); ii != vrc.end(); ii++) {
        rc = *ii;
        cnt++;
        box = GETBOX(rc.row,rc.col);
        sprintf(tb,"%2d: R%dC%dB%d %s [%d] ", cnt, rc.row + 1, rc.col + 1, box + 1,
            (rc.cnt ? "A" : "B"), Get_Set_Cnt( &rc.set ));
        AddSet2Buf(tb,&rc.set);
        if (cnt > 1) {
            int scnt = Get_Set_Cnt( &rc.set );
            while (scnt < 6) {
                strcat(tb," ");
                scnt++;
            }
            Zero_SET(&comm);
            int ccnt = Get_Shared_SET( &rc.set, &rcp.set, &comm );
            if (ccnt) {
                sprintf(EndBuf(tb)," shr [%d] ", ccnt);
                AddSet2Buf(tb,&comm);
            } else {
                strcat(tb, " no shr - new?");
            }
        }
        OUTIT(tb);
        rcp = rc;
    }
}

int add_debug_cs = 1;
int only_one_cs = 1;

// seek in ROW, COL, BOX an empty cell, NOT already a member of the chain,
// which shares ONE value, setval, with prcm(aster)...
int Get_Next_Chain_Members( PABOX2 pb, vRC &vrc, PROWCOL prcm, vRC &members, int setval, int color )
{
    static ROWCOL found[9*9];
    int count = 0;
    ROWCOL rc, rcf, rc3;
    size_t i, k;
    int row, col, box, flag;
    rcf = *prcm;
    row = prcm->row;
    col = prcm->col;
    box = prcm->set.uval;
    int mfound = 0;
    // char *tb = GetNxtBuf();
    char *tb = (char *)malloc(1204);
    CHKMEM(tb);
    char tmp[16];
    tmp[0] = 0;
    for (i = 0; i < vrc.size(); i++ ) {
        rc = vrc[i];
        if ((rc.row == row) && (rc.col == col)) continue; // is ME
        if (Row_Col_in_Vector( members, rc.row, rc.col )) continue; // already member
        if ( !((rc.row == row) || (rc.col == col) || (rc.set.uval == box)) )
                    continue;   // NOT same ROW, COL or BOX
        if (!Value_in_SET( setval, &rc.set )) continue;    // does NOT share value
        // AND they MUST just be a PAIR in this ROW, COL, or BOX
        if (!Just_2_in_RCB_With_Val(pb, vrc, rc.row, rc.col, setval)) continue;

        flag = 0;
        if (rc.row == rcf.row) {
            strcat(tmp,"SR ");
            // seek other 'sharers' of this comm with the same ROW
            for (k = 0; k < vrc.size(); k++) {
                if (k == i) continue;
                rc3 = vrc[k];
                if (rc.row != rc3.row) continue;
                if (!Value_in_SET( setval, &rc3.set )) continue;    // does NOT share value
                // THEY are NOT a ROW PAIR
                flag |= 1;  // mark this
                break; // no need to search more
            }
        }
        if (rc.col == rcf.col) {
            strcat(tmp,"SC ");
            // seek other 'sharers' of this val with the same COL
            for (k = 0; k < vrc.size(); k++) {
                if (k == i) continue;
                rc3 = vrc[k];
                if (rc.col != rc3.col) continue;
                if (!Value_in_SET( setval, &rc3.set )) continue;    // does NOT share value
                // THEY are NOT a ROW PAIR
                flag |= 2;  // mark this
                break; // no need to search more
            }
        }

        if (rc.set.uval == rcf.set.uval) {
            // seek other 'sharers' of this val with the same COL
            strcat(tmp,"SB ");
            for (k = 0; k < vrc.size(); k++) {
                if (k == i) continue;
                rc3 = vrc[k];
                if (rc.set.uval != rc3.set.uval) continue;
                if (!Value_in_SET( setval, &rc3.set )) continue;    // does NOT share value
                // THEY are NOT a BOX PAIR
                flag |= 4;  // mark this
                break; // no need to search more
            }
        }
        if ( flag == (1 | 2 | 4) ) continue;    // oops is NOT just a PAIR in a ROW,COL or BOX
        // WOW, got NEXT member in the CHAIN (I think!)
        rc.cnt = color;
        sprintf_s(tb,1024,"from R%dC%dB%d ",
            (row + 1), (col + 1), (box + 1));
        sprintf(EndBuf(tb),"%s ", (rcf.cnt ? "B" : "A"));
        sprintf(EndBuf(tb),"Next R%dC%dB%d %s ",
            rc.row + 1, rc.col + 1, GETBOX(rc.row,rc.col) + 1,
            (rc.cnt ? "B" : "A"));
        //strcat(tb,tmp);
        OUTIT(tb);
        members.push_back(rc);  // store as a member
        found[mfound] = rc;   // and store this color set
        mfound++;
        count++;
        
    }

    // now seek opposite of each found
    sprtf("Get_Next_Chain_Members: Processing %d found\n", mfound);
    for (row = 0; row < mfound; row++) {
        rc = found[row];
        sprintf(tb,"From R%dC%dB%d %s",
            rc.row + 1, rc.col + 1, GETBOX(rc.row,rc.col) + 1,
            (rc.cnt ? "B" : "A"));
        OUTIT(tb);
        count += Get_Next_Chain_Members( pb, vrc, &rc, members, setval,
            (color ? 0 : 1) );
    }
    free(tb);
    return count;
}

void Mark_Color_Members( PABOX2 pb, vRC & members )
{
    size_t max = members.size();
    size_t i;
    ROWCOL rc;
    int row,col;
    for (i = 0; i < max; i++) {
        rc = members[i];
        row = rc.row;
        col = rc.col;
        pb->line[row].set[col].cellflg |= cl_MCC;   // Member of color club
        pb->line[row].set[col].uval = rc.cnt;       // get COLOR
    }
}

int Find_Pairs_in_RCB( PABOX2 pb, vRC &vrc2, PROWCOL prc, int setval, vRC &dest )
{
    int count = 0;
    int row, col, val, box;
    int row2, col2;
    PSET ps, ps2;
    int srow = prc->row;
    int scol = prc->col;
    int sbox = GETBOX(srow,scol);
    bool bad;
    ROWCOL rc;
    // test in COL
    bad = false;
    char *tb = GetNxtBuf();
    sprintf(tb,"R%dC%dB%d val %d ", srow + 1, scol + 1, sbox + 1, setval);
    for (row = 0; row < 9; row++) {
        if (row == srow) continue; // skip self
        val = pb->line[row].val[scol];
        if (val) continue;
        ps = &pb->line[row].set[scol];
        if ( !Value_in_SET( setval, ps )) continue;
        box = GETBOX(row,scol);
        if (box == sbox) continue;
        // found a sharing partner in same ROW, outside src box
        // but must now check there are no more in this ROW
        bad = false;
        for (row2 = 0; row2 < 9; row2++) {
            if (row2 == srow) continue; // skip loc of 1st
            if (row2 == row) continue;  // skip loc of 2nd
            val = pb->line[row2].val[scol];
            if (val) continue;
            ps2 = &pb->line[row2].set[scol];
            if ( Value_in_SET( setval, ps2 )) {
                // oops, there is a 3rd in this COLUMN
                bad = true;
                break;
            }
        }
        if (!bad) {
            rc.row = row;
            rc.col = scol;
            COPY_SET(&rc.set,ps);
            dest.push_back(rc);
            count++;
            sprintf(EndBuf(tb),"Cpair R%dC%dB%d ", rc.row + 1, rc.col + 1,
                GETBOX(rc.row,rc.col) + 1);
        }
        break;
    }
    // test in ROW
    for (col = 0; col < 9; col++) {
        if (col == scol) continue; // skip self
        val = pb->line[srow].val[col];
        if (val) continue;
        ps = &pb->line[srow].set[col];
        if ( !Value_in_SET( setval, ps )) continue;
        box = GETBOX(srow,col);
        if (box == sbox) continue;
        // found a sharing partner in same COL outside BOX
        // but must now check there are no more in this COL
        bad = false;
        for (col2 = 0; col2 < 9; col2++) {
            if (col2 == scol) continue; // skip loc of 1st
            if (col2 == col) continue;  // skip loc of 2nd
            val = pb->line[srow].val[col2];
            if (val) continue;
            ps2 = &pb->line[srow].set[col2];
            if ( Value_in_SET( setval, ps2 )) {
                // oops, there is a 3rd
                bad = true;
                break;
            }
        }
        if (!bad) {
            rc.row = srow;
            rc.col = col;
            COPY_SET( &rc.set, ps );
            dest.push_back(rc);
            count++;
            sprintf(EndBuf(tb),"Rpair R%dC%dB%d ", rc.row + 1, rc.col + 1,
                GETBOX(rc.row,rc.col) + 1);
        }
        break; // found 1 or none in this COL
    }
    // in same BOX
    int r = (srow / 3) * 3;
    int c = (scol / 3) * 3;
    int rw, cl;
    bad = true;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((row == srow) && (col == scol)) continue;
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            if ( !Value_in_SET( setval, ps )) continue;
            // found a sharing partner in same BOX
            // but must now check there are no more
            bad = false;
            break;
        }
    }
    if (!bad) {
        for (rw = 0; rw < 3; rw++) {
            for (cl = 0; cl < 3; cl++) {
                row2 = r + rw;
                col2 = c + cl;
                if ((row2 == srow) && (col2 == scol)) continue; // skip first
                if ((row2 ==  row) && (col2 ==  col)) continue; // skip second
                val = pb->line[row2].val[col2];
                if (val) continue;
                ps2 = &pb->line[row2].set[col2];
                if ( Value_in_SET( setval, ps2 ) ) {
                    // oops, found a 3rd in the BOX
                    bad = true;
                    break;
                }
            }
            if (bad)
                break;
        }
    }
    if (!bad) {
        rc.row = row;
        rc.col = col;
        COPY_SET( &rc.set, ps );
        dest.push_back(rc);
        sprintf(EndBuf(tb),"Bpair R%dC%dB%d ", rc.row + 1, rc.col + 1,
            GETBOX(rc.row,rc.col) + 1);
        count++;
    }
    sprintf(EndBuf(tb),"cnt %d", count);
    OUTIT(tb);
    return count;
}


int Process_Color_vectors( PABOX2 pb, vRC &vrc2, int setval, int color, vRC &members)
{
    int count = 0;
    vRC vrc3;
    size_t i;
    size_t max = vrc2.size();
    ROWCOL rc;
    for (i = 0; i < max; i++) {
        rc = vrc2[i];
        if (Row_Col_in_Vector( members, rc.row, rc.col )) continue;
        vrc3.clear();
        if (!Find_Pairs_in_RCB( pb, vrc2, &rc, setval, vrc3 )) continue;
        // is 'connected' to those in vrc3
        rc.cnt = color; // set color
        members.push_back(rc);
        count++;
        ROWCOL rc2;
        int color2 = (color ? 0 : 1);
        size_t max2 = vrc3.size();
        size_t i2;
        for (i2 = 0; i2 < max2; i2++) {
            rc2 = vrc3[i2];
            rc2.cnt = color2;  // flip the color
            members.push_back(rc2);
            count++;
        }
        for (i2 = 0; i2 < max2; i2++) {
            rc2 = vrc3[i2];
            count += Process_Color_vectors( pb, vrc2, setval, color2, members );
        }
    }
    return count;
}

int Work_With_Color_Vector( PABOX2 pb, vRC & vrc )
{
    int count = 0;
    if (add_debug_cs) {
        sprtf("Color scan - found %d 'sharers'\n",vrc.size());
        Show_Vector(vrc); // just for diag...
    }
    // Maybe try by value
    size_t i;
    int setval, cnt;
    vRC vrc2;
    vRC members;
    ROWCOL rc;
    char *tb = GetNxtBuf();
    // set the rc.set.uval == the BOX number, for quick comparison
    for ( i = 0; i < vrc.size(); i++ ) {
        rc = vrc[i];
        rc.set.uval = (rc.row / 3) + (rc.col / 3);
    }
    // for (setval = 1; setval <= 9; setval++) {
    for (setval = 5; setval <= 9; setval++) {
        vrc2.clear();
        // cull from the all 'sharers', taking ONLY 'sharers' with this setval,
        // and who are not 'broken' for other reasons.
        for ( i = 0; i < vrc.size(); i++ ) {
            rc = vrc[i];
            if (!Value_in_SET( setval, &rc.set )) continue;
            // also could avoid adding this, 
            // IF there is NOT a SINGLE other cell in a ROW, COL, or BOX with this value
            sprintf(tb,"%2d: R%dC%dB%d [%d] ", i + 1, rc.row + 1, rc.col + 1,
                (rc.row / 3) + (rc.col / 3) + 1,
                Get_Set_Cnt( &rc.set ));
            AddSet2Buf(tb,&rc.set);
            if (!Just_2_in_RCB_With_Val(pb, vrc, rc.row, rc.col, setval)) continue;
            vrc2.push_back(rc);
        }
        if (!vrc2.size()) continue;
        sprtf("Val %d: Stored %d of %d cells\n", setval, (int)vrc2.size(), (int)vrc.size() );
        Show_Vector(vrc2);
        // We KNOW each of these SHARE the value val - try for color chain
        members.clear();
        cnt = Process_Color_vectors(pb, vrc2, setval, 0, members);
        count += cnt;
        Mark_Color_Members( pb, members );
        if (count && only_one_cs)
            break;

    }
    return count;
}



int Work_With_Color_Vector_OK( PABOX2 pb, vRC & vrc )
{
    int count = 0;
    if (add_debug_cs) {
        sprtf("Color scan - found %d 'sharers'\n",vrc.size());
        Show_Vector(vrc); // just for diag...
    }
    // Maybe try by value
    size_t i, j, k, setval, flag;
    vRC vrc2;
    vRC members;
    ROWCOL rc, rc2, rc3;
    bool done;  // done a set value
    char *tb = GetNxtBuf();
    // set the rc.set.uval == the BOX number, for quick comparison
    for ( i = 0; i < vrc.size(); i++ ) {
        rc = vrc[i];
        rc.set.uval = (rc.row / 3) + (rc.col / 3);
    }
    // for (setval = 1; setval <= 9; setval++) {
    for (setval = 5; setval <= 9; setval++) {
        vrc2.clear();
        // cull from the all 'sharers', taking ONLY 'sharers' with this setval,
        // and who are not 'broken' for other reasons.
        for ( i = 0; i < vrc.size(); i++ ) {
            rc = vrc[i];
            if (!Value_in_SET( setval, &rc.set )) continue;
            // also could avoid adding this, 
            // IF there is NOT a SINGLE other cell in a ROW, COL, or BOX with this value
            sprintf(tb,"%2d: R%dC%dB%d [%d] ", i + 1, rc.row + 1, rc.col + 1,
                (rc.row / 3) + (rc.col / 3) + 1,
                Get_Set_Cnt( &rc.set ));
            AddSet2Buf(tb,&rc.set);
            if (!Just_2_in_RCB_With_Val(pb, vrc, rc.row, rc.col, setval)) continue;
            vrc2.push_back(rc);
        }
        if (!vrc2.size()) continue;
        sprtf("Val %d: Stored %d of %d cells\n", setval, (int)vrc2.size(), (int)vrc.size() );
        Show_Vector(vrc2);
        // We KNOW each of these SHARE the value val - try for color chain
        done = false;
        for (i = 0; i < vrc2.size(); i++) {
            rc = vrc2[i];

            // is there a 'sharer' of this val in the SAME ROW, COL, BOX
            for (j = 0; j < vrc2.size(); j++) {
                if (i == j) continue; // skip self
                rc2 = vrc2[j];
                if ( !((rc.row == rc2.row) || (rc.col == rc2.col) || (rc.set.uval == rc2.set.uval)) )
                    continue;   // NOT same ROW, COL or BOX
                // BUT also MUST just 2 cells in this ROW,COL,BOX to continue
                flag = 0;
                if (rc.row == rc2.row) {
                    // seek other 'sharers' of this val with the same ROW
                    for (k = 0; k < vrc2.size(); k++) {
                        if (k == i) continue;
                        if (k == j) continue;
                        rc3 = vrc2[k];
                        if (rc.row == rc3.row) {
                            // THEY are NOT a ROW PAIR
                            flag |= 1;  // mark this
                            break; // no need to search more
                        }
                    }
                }
                if (rc.col == rc2.col) {
                    // seek other 'sharers' of this val with the same COL
                    for (k = 0; k < vrc2.size(); k++) {
                        if (k == i) continue;
                        if (k == j) continue;
                        rc3 = vrc2[k];
                        if (rc.col == rc3.col) {
                            // THEY are NOT a ROW PAIR
                            flag |= 2;  // mark this
                            break; // no need to search more
                        }
                    }
                }

                if (rc.set.uval == rc2.set.uval) {
                    // seek other 'sharers' of this val with the same COL
                    for (k = 0; k < vrc2.size(); k++) {
                        if (k == i) continue;
                        if (k == j) continue;
                        rc3 = vrc2[k];
                        if (rc.set.uval == rc3.set.uval) {
                            // THEY are NOT a ROW PAIR
                            flag |= 4;  // mark this
                            break; // no need to search more
                        }
                    }
                }
                if ( flag == (1 | 2 | 4) ) continue;    // oops is NOT just a PAIR in a ROW,COL or BOX
                rc.cnt = 0;     // set opposite COLORS
                rc2.cnt = 1;
                // Now continue the chain with cell 2
                members.clear();
                members.push_back(rc);
                members.push_back(rc2);
                sprintf(tb,"Start R%dC%dB%d %s, next R%dC%dB%d %s",
                    rc.row + 1, rc.col + 1, GETBOX(rc.row,rc.col) + 1,
                    (rc.cnt ? "B" : "A"),
                    rc2.row + 1, rc2.col + 1, GETBOX(rc2.row,rc2.col) + 1,
                    (rc2.cnt ? "B" : "A") );
                OUTIT(tb);
                count += Get_Next_Chain_Members( pb, vrc2, &rc2, members, setval, 0 );
                if (add_debug_cs) {
                    sprtf("Color scan - val %d - found %d 'members'\n",setval, members.size());
                    Show_Vector(members); // just for diag...
                }
                Mark_Color_Members( pb, members );
                done = true;
                break;  // DONE this set value
            }
            if (done)
                break;
        }
        if (count && only_one_cs)
            break;
    }
    return count;
}

// Color Scan
int Do_Color_Scan( PABOX2 pb )
{
    int count = 0;
    int row, col, val;
    int color;
    ROWCOL rc;
    vRC vrc;
    vrc.clear(); // clear vector
    // do whole 9x9
    int cnt = 0;
    if (add_debug_cs) sprtf("Doing Color scan...\n");
    int totempty = 0;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // only empty cells
            totempty++; // total EMPTY cells
            if (Row_Col_in_Vector( vrc, row, col )) continue; // not already stored
            PSET ps = &pb->line[row].set[col];
            // seek a partner sharing at least one common candidate
            rc.row = row;
            rc.col = col;
            rc.cnt = 0; // color 0
            COPY_SET(&rc.set,ps);
            vrc.push_back(rc);
            color = 1;
            cnt += Find_Sharer_Sets_RCB(pb,vrc,row,col,color);
        }
    }
    //assert( cnt == (int) vrc.size() );
    if (vrc.size()) {
        sprtf("Passed %d of %d to Work_With_Color_Vector\n", vrc.size(), totempty);
        count = Work_With_Color_Vector( pb, vrc );
    }
    if (count) {
        sprtf("S2f: Color scan, elim %d. To %d\n", count,
            sg_Fill_Color);
        pb->iStage = sg_Fill_Color;
    } else {
        sprtf("S2f: Color scan. No elims. To begin.\n");
        pb->iStage = sg_Begin;
    }
    return count;
}

// end of color strategy
// ===============================================
void Set_SIZES_OK(HWND hWnd)
{
    GetClientRect(hWnd,&g_rcClient);

    RECT rc = g_rcClient;
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int log = width / 3;
    SIZE sz = g_szCN8Text;

    sudo_x = ((log * 2) - h_workmrg) / 9;                         // one sudo x width
    sudo_y = (height - (g_vt_margin + g_vb_margin) - v_workmrg) / 9;        // one sudo y height

    //if (log > 50) {
    if ((sudo_x > min_width) && (sudo_y > min_height)) {

        log = (g_hl_margin + g_hr_margin) + (sudo_x * 9) + h_workmrg;

        g_rcText.left  = log + g_hr_margin; // plus a small margin
        g_rcText.right = rc.right - g_hr_margin; // minus a small margin
        g_rcText.top   = rc.top + g_vb_margin; // plus a small margin
        //rcText.bottom = rcText.top + sz.cy;
        // this is actually the g_rcMouse.top!
        g_rcText.bottom = rc.bottom - g_vb_margin - sz.cy;


        d_x1 = log;
        d_x2 = d_x1;
        d_y1 = rc.top;
        d_y2 = rc.bottom;

        sq_x1 = rc.left + g_hl_margin;
        sq_x2 = sq_x1;
        sq_x3 = d_x1 - g_hr_margin - h_workmrg;
        sq_x4 = sq_x3;

        sq_y1 = rc.top + g_vt_margin;
        sq_y2 = sq_y1 + (sudo_y * 9);
        sq_y3 = sq_y2;
        sq_y4 = sq_y1;
        // d_x1 = log;
        // d_x2 = d_x1;
        // d_y1 = rc.top;
        // d_y2 = rc.bottom;
        g_rcMouse.left   = d_x1 + g_hl_margin;
        g_rcMouse.right  = g_rcClient.right - g_hr_margin;
        g_rcMouse.bottom = g_rcClient.bottom - g_vb_margin;
        g_rcMouse.top    = rc.bottom - g_vb_margin - sz.cy;

        g_rcListView.left  = log; // + h_margin;      // set LEFT
        g_rcListView.right = rc.right; // - h_margin; // set RIGHT
        g_rcListView.top   = rc.top; // + v_margin;
        g_rcListView.bottom = g_rcMouse.top - g_vb_margin;

        int row, col;
        int x1, x2, y1, y2;
        PRECT pr, pr2, pr3;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                x1 = sq_x1 + (sudo_x * col);
                x2 = sq_x1 + (sudo_x * (col+1));
                y1 = sq_y1 + (sudo_y * row);
                y2 = sq_y1 + (sudo_y * (row+1));
                pr = &rcSquares[row][col];
                pr->top    = y1;
                pr->bottom = y2;
                pr->left   = x1;
                pr->right  = x2;
            }
        }

        // set rectangle for paint horizontal lines
        x1 = sq_x1;
        x2 = sq_x3;
        for (row = 0; row < 9; row++) {
            y1 = sq_y1 + (sudo_y * row);
            y2 = sq_y1 + (sudo_y * row);
            // fill in HORIZONTAL work pad rect
            // sq_x1,sq_y1       sq_x4,sq_y4
            // sq_x2,sq_y2       sq_x3,sq_y3
            pr = &rcHWork[row];
            pr->left   = sq_x4 + g_hl_margin;
            //pr->right  = pr->left + h_workmrg - g_hr_margin - g_hl_margin;
            pr->right  = pr->left + h_workmrg - g_hl_margin;
            pr->top    = y1 + g_vb_margin;
            pr->bottom = pr->top + sudo_y - (2 * g_vb_margin);
            pr2 = &g_rcVTextL[row];
            pr2->left = g_rcClient.left;
            pr2->right = pr2->left + g_hl_margin;
            pr2->top = y1;
            pr2->bottom = pr2->top + sudo_y;
            //sprtf("%d: V1 RECT %s\n", row + 1, Get_Rect_Stg(pr2));
            pr3 = &g_rcVTextR[row];
            pr3->left = sq_x4;
            pr3->right = pr3->left + g_hl_margin;
            pr3->top = pr2->top;
            pr3->bottom = pr2->bottom;
            // sprtf("%d: V2 RECT %s\n", row + 1, Get_Rect_Stg(pr3));
        }
        // set rectangles for paint vertical lines
        y1 = sq_y1;
        y2 = sq_y2;
        for (col = 0; col < 9; col++) {
            x1 = sq_x1 + (sudo_x * col);
            x2 = sq_x1 + (sudo_x * col);
            // fill in VERTICAL work pad rect
            pr = &rcVWork[col]; // get for SETTING - Set_SIZES_OK()
            pr->left   = x1 + g_hr_margin; // left is left + margin
            pr->right  = pr->left + sudo_x - (2 * g_hr_margin); // right is left + 1 sudo_x - 2 * margin
            pr->top    = y2 + g_vb_margin; // top is bottom + margin
            pr->bottom = pr->top + v_workmrg - g_vb_margin; // bot is top + work - margin
            // paint across the TOP 1 2 3 ...
            pr2 = &g_rcHText[col];  // SETTING of text position in WM_SIZE
            pr2->left = x1;
            pr2->right = pr2->left + sudo_x;
            pr2->top   = g_rcClient.top + 1;
            pr2->bottom = pr2->top + g_vt_margin;
        }
    }
    if (hfVert)
        DeleteObject(hfVert);
    hfVert = 0;
}

// ===================================================
// An abandonned COLORING module
// Sudo_Coloring.hxx
#ifndef _SUDO_COLORING_HXX_
#define _SUDO_COLORING_HXX_

#ifndef USE_COLOR2_SCAN

extern int Do_Fill_Color(PABOX2 pb);
extern int Do_Color_Scan( PABOX2 pb );

extern int do_color_test;  // start with value 1-9
extern int on_color_test; // = 0;

extern vRC *Get_Color_Chain();
extern bool Is_Chain_Valid();
extern int Get_Chain_Value();
extern void Set_Chain_Invalid();

#endif // #ifndef USE_COLOR2_SCAN

#endif // #ifndef _SUDO_COLORING_HXX_
// eof - Sudo_Coloring.hxx


// Sudo_Coloring.cxx

#include "Sudoku.hxx"

#ifndef USE_COLOR2_SCAN
// ==========================================================
// BEGIN COLOR STRATEGY
// ====================
int only_one_cs = 1;

#define OUTITCS(a) if (add_debug_cs) OUTIT(a)

int do_color_test = 1;  // start with value 1-9
int on_color_test = 0;

BOOL g_bMrkNM = TRUE;   // also mark NON-members if ONE setval used
BOOL b_bDoSameChain = TRUE; // if nothing elim outside marked, check does same chain elim anything
BOOL g_bElimConf = FALSE;

// Keep the chains formed for PAINTING
// ===================================
vRC color_chain;
int chain_number;
bool chain_valid = false;
int chain_value;
bool Is_Chain_Valid()
{
#ifdef USE_COLOR2_SCAN
    return Is_Chain_Valid2();
#else // #ifdef USE_COLOR2_SCAN
    return chain_valid; 
#endif // #ifdef USE_COLOR2_SCAN
}
vRC *Get_Color_Chain() { return &color_chain; }
void Set_Chain_Invalid()
{ 
    chain_valid = false; 
    Set_Chain_Invalid2();
}
int Get_Chain_Value() 
{
#ifdef USE_COLOR2_SCAN
    return Get_Chain_Value2();
#else // #ifdef USE_COLOR2_SCAN
    return chain_value;
#endif // #ifdef USE_COLOR2_SCAN
}
// ====================================
int g_bTfrALL = FALSE; // only transfer to FINAL if members of chain per 'chain_number' GT 2

void Mark_Color_Members( PABOX2 pb, vRC & members )
{
    size_t max = members.size();
    size_t i;
    ROWCOL rc;
    int row,col;

    if (add_debug_cs) Show_RC_Vector(members);

    for (i = 0; i < max; i++) {
        rc = members[i];
        row = rc.row;
        col = rc.col;
        pb->line[row].set[col].cellflg |= cl_MCC;   // Member of color club
        pb->line[row].set[col].uval = rc.set.uval;  // get/set COLOR
    }
    if (max)
        chain_valid = true;
}

int Mark_Color_Non_Members( PABOX2 pb, vRC & members, int setval, vRC &vrc )
{
    int count = 0;
    size_t max = vrc.size();
    size_t i;
    ROWCOL rc;
    int row,col;
    PSET ps;
    int val = setval - 1;
    if (!VALUEVALID(setval)) return 0;

    for (i = 0; i < max; i++) {
        rc = vrc[i];
        row = rc.row;
        col = rc.col;
        if (Row_Col_in_RC_Vector( members, row, col )) continue;
        ps = &pb->line[row].set[col];  // get SET
        if ( !ps->val[val] ) continue;
        pb->line[row].set[col].cellflg |= cl_CSC;   // selected, like Mark_HighLight_Cells
        count++;
    }
    return count;
}

// ==========================================================
// **********************************************************

// RULES OF TRANSFER
// If g_bTfrALL is ON, trasfer ALL, else
// ONLY transfer chains of 3 or more members!
// That is they form a 'L' whihc can be used in an ON/OFF sense to 
// potentially ELIMINATE other candidates...
int Transfer_Color_Members( PABOX2 pb, vRC &members, vRC &final_members )
{
    int count = 0;
    size_t max = members.size();
    size_t i, j;
    int cnt, off, k, dn;
    ROWCOL rc, rc2;
    if (g_bTfrALL) {
        for (i = 0; i < max; i++) {
            rc = members[i];
            final_members.push_back(rc);
            count++;
        }
        return count;
    }
    char *tb = GetNxtBuf();
    size_t *pi = new size_t[max];
    size_t *done = new size_t[max];
    off = 0;
    for (i = 0; i < max; i++) {
        dn = 0;
        for (k = 0; k < off; k++) {
            if (done[k] == i) {
                dn = 1;
                break;
            }
        }
        if (dn) continue;
        rc = members[i];
        done[off++] = i;    // doing this now
        sprintf(tb,"Chain num %d %s ", rc.cnum + 1, Get_RC_RCB_Stg(&rc));
        cnt = 0;    // GET COUNT WITH THIS CHAIN
        for (j = 0; j < members.size(); j++) {
            dn = 0;
            for (k = 0; k < off; k++) {
                if (done[k] == j) {
                    dn = 1;
                    break;
                }
            }
            if (dn) continue;
            rc2 = members[j];
            if (rc2.cnum == rc.cnum) {
                // same chain number
                sprintf(EndBuf(tb),"+%s ", Get_RC_RCB_Stg(&rc2));
                pi[cnt++] = j; // same group number
                done[off++] = j;
            }
        }
        sprintf(EndBuf(tb),"count %d", cnt + 1);
        OUTITCS(tb);
        if (cnt > 1) {
            final_members.push_back(rc);
            count++;
            for (k = 0; k < cnt; k++) {
                rc2 = members[pi[k]];
                final_members.push_back(rc2);
                count++;
            }
        }
    }

    delete pi;
    delete done;
    return count;
}




//pb->line[row].set[col].cellflg |= cl_MCC;   // Member of color club
//pb->line[row].set[col].uval = rc.set.uval;  // get/set COLOR
int skip_box_elims = 1;
int add_vector_counts = 0;
// oops, this can MARK members - NOT possible - should ONLY mark NON members!!!
int Eliminate_per_vRC_Val_Color( PABOX2 pb, vRC &members, int setval, int color )
{
    int count = 0;
    size_t max = members.size();
    if (!max)
        return 0;
    if (!VALUEVALID(setval))
        return 0;
    int val = setval - 1;
    ROWCOL rc;
    size_t i;
    for (i = 0; i < max; i++) {
        rc = members[i];
        if ( !rc.set.uval == color ) continue;
        OR_Row_Col_SetVal_with_Flag( pb, rc.row, rc.col, setval, cf_CCE );
        // pb->line[rc.row].set[rc.col].flag[val] |= cf_CCE;
        count++;
    }
    return count;
}

void Set_RCB_Structure( vRC & members, PRCRCB prcrcb )
{
    size_t i, max;
    ROWCOL rc;
    int row, col, box;
    vRC *pvrc;
    vRC *pvrow;
    vRC *pvcol;
    vRC *pvbox;

    max = members.size();
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vrows[i];
        pvrc->clear();
    }
    for (i = 0; i < max; i++) {
        rc = members[i];
        row = rc.row;
        col = rc.col;
        box = GETBOX(row,col);
        pvrow = &prcrcb->vrows[row];
        pvcol = &prcrcb->vcols[col];
        pvbox = &prcrcb->vboxs[box];
        pvrow->push_back(rc);
        pvcol->push_back(rc);
        pvbox->push_back(rc);
    }
    if (!add_debug_cs)
        return; // all done

    char *tb = GetNxtBuf();
    size_t jmax, k, j, j2;
    ROWCOL rc2;
    bool add_stg;
    int conflics[4];
    int cfcnt, a;

    strcpy(tb, "Row pairs ");
    k = 0;
    cfcnt = 0;
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vrows[i];
        jmax = pvrc->size();
        if (jmax) {
            if (add_vector_counts)
                sprintf(EndBuf(tb),"%d-%d ", (int)(i + 1), (int)jmax);
            if (jmax > 1) {
                for (j = 0; j < jmax; j++) {
                    j2 = j + 1;
                    if (j2 == jmax)
                        j2 = 0;
                    rc = pvrc->at(j);
                    rc2 = pvrc->at(j2);
                    if ((rc.set.uval == rc2.set.uval) && !Same_RC_Pair(&rc,&rc2)) {
                        // SAME ROW and SAME COLOR
                        add_stg = 0;
                        if (cfcnt) {
                            add_stg = 0;
                            for (a = 0; a < cfcnt; a++) {
                                if (conflics[a] == rc.set.uval)
                                    break;
                            }
                            if (a == cfcnt) {
                                conflics[cfcnt++] = rc.set.uval;
                                add_stg = 1;
                            }
                        } else {
                            conflics[cfcnt++] = rc.set.uval;
                            add_stg = 1;
                        }
                        if (add_stg) {
                           if (add_vector_counts)
                               sprintf(EndBuf(tb),"R%dC %s%s ", (int)(i + 1), CLRSTG(rc.set.uval), CLRSTG(rc2.set.uval));
                           else
                               sprintf(EndBuf(tb),"r%d: %s & %s ", (int)(i + 1), Get_RC_RCBC_Stg(&rc), Get_RC_RCBC_Stg(&rc2));
                        }
                    }
                }
            }
        }
        k += jmax;
    }
    if (k)
        sprintf(EndBuf(tb),"tot %d", (int)k);
    else
        strcat(tb,"none");
    OUTITCS(tb);

    strcpy(tb, "Col pairs ");
    k = 0;
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vcols[i];
        jmax = pvrc->size();
        if (jmax) {
            if (add_vector_counts)
                sprintf(EndBuf(tb),"%d-%d ", (int)(i + 1), (int)jmax);
            if (jmax > 1) {
                for (j = 0; j < jmax; j++) {
                    j2 = j + 1;
                    if (j2 == jmax)
                        j2 = 0;
                    rc = pvrc->at(j);
                    rc2 = pvrc->at(j2);
                    if ((rc.set.uval == rc2.set.uval) && !Same_RC_Pair(&rc,&rc2)) {
                        // SAME COL and SAME COLOR
                        add_stg = 0;
                        if (cfcnt) {
                            for (a = 0; a < cfcnt; a++) {
                                if (conflics[a] == rc.set.uval)
                                    break;
                            }
                            if (a == cfcnt) {
                                conflics[cfcnt++] = rc.set.uval;
                                add_stg = 1;
                            }
                        } else {
                            conflics[cfcnt++] = rc.set.uval;
                            add_stg = 1;
                        }
                        if (add_stg) {
                            if (add_vector_counts)
                                sprintf(EndBuf(tb),"C%dC %s%s ", (int)(i + 1), CLRSTG(rc.set.uval), CLRSTG(rc2.set.uval));
                            else
                                sprintf(EndBuf(tb),"c%d: %s & %s ", (int)(i + 1), Get_RC_RCBC_Stg(&rc), Get_RC_RCBC_Stg(&rc2));
                        }
                    }
                }
            }
        }
        k += jmax;
    }
    if (k)
        sprintf(EndBuf(tb),"tot %d", (int)k);
    else
        strcat(tb,"none");
    OUTITCS(tb);

    strcpy(tb, "Box pairs ");
    k = 0;
    for (i = 0; i < 9; i++) {
        pvrc = &prcrcb->vboxs[i];
        jmax = pvrc->size();
        if (jmax) {
            if (add_vector_counts)
                sprintf(EndBuf(tb),"%d-%d ", (int)(i + 1), (int)jmax);
            if (jmax > 1) {
                for (j = 0; j < jmax; j++) {
                    j2 = j + 1;
                    if (j2 == jmax)
                        j2 = 0;
                    rc = pvrc->at(j);
                    rc2 = pvrc->at(j2);
                    if ((rc.set.uval == rc2.set.uval) && !Same_RC_Pair(&rc,&rc2)) {
                        // SAME BOX and SAME COLOR
                        add_stg = 0;
                        if (cfcnt) {
                            for (a = 0; a < cfcnt; a++) {
                                if (conflics[a] == rc.set.uval)
                                    break;
                            }
                            if (a == cfcnt) {
                                conflics[cfcnt++] = rc.set.uval;
                                add_stg = 1;
                            }
                        } else {
                            conflics[cfcnt++] = rc.set.uval;
                            add_stg = 1;
                        }
                        if (add_stg) {
                            if (add_vector_counts)
                                sprintf(EndBuf(tb),"B%dC %s%s ", (int)(i + 1), CLRSTG(rc.set.uval), CLRSTG(rc2.set.uval));
                            else
                                sprintf(EndBuf(tb),"b%d: %s & %s ", (int)(i + 1), Get_RC_RCBC_Stg(&rc), Get_RC_RCBC_Stg(&rc2));
                        }
                    }
                }
            }
        }
        k += jmax;
    }
    if (k)
        sprintf(EndBuf(tb),"tot %d", (int)k);
    else
        strcat(tb,"none");
    OUTITCS(tb);

}


int Check_Color_Members( PABOX2 pb, vRC &members_in, vRC &vrc, int setval )
{
    int count = 0;
    // Do any color members ELIMINATE a value in the main list
    size_t i, j, k;
    ROWCOL rc, rc2;
    ROWCOL rcrow, rccol;
    int row,col,cnt,box;
    int a, b;
    RCRCB rcrcb;
    size_t max, mvrc;
    int val0 = setval - 1;
    vRC *pvrc, *pvrows, *pvcols;
    ROWCOL rcpairs[9];
    bool done;
    //int conflics[4];
    //int cfcnt;
    char *tb = GetNxtBuf();
    vRC members;

    Transfer_Color_Members( pb, members_in, members );
    max = members.size();
    mvrc = vrc.size();

    // if (add_debug_cs) Show_RC_Vector(members);
    // sort members into ROW COL and BOX
    sprintf(tb,"Checking %d of %d color members, on %d total",
        (int)members.size(), (int)members_in.size(), (int)vrc.size());
    OUTITCS(tb);

    //cfcnt = 0;
    //conflics[cfcnt] = 0;
    Set_RCB_Structure( members, &rcrcb );

#if 0 // 0 - need to re-think this
    if (cfcnt && g_bElimConf) {
        if (cfcnt == 1) {
            count += Eliminate_per_vRC_Val_Color( pb, members, setval, conflics[0] );
            sprtf("CS: Elim. %d %d from color %s\n", count, setval, CLRSTG(conflics[0]));
        } else {
            sprtf("CS: Multiple conflics %d, NO eliminations!\n", cfcnt);
        }
        // with CONFLICTS, do NOT do any other eliminations!!!!
        return count;
    }
#endif // 0 ---

    for (i = 0; i < mvrc; i++) {
        rc = vrc[i];
        row = rc.row;
        col = rc.col;
        if ( !rc.set.val[val0] ) continue; // no setval candidate
        if ( rc.set.flag[val0] & cf_CC ) continue; // mark include
        //if ( !(rc.set.flag[val] & cf_CC) ) continue; // mark include
        if( Row_Col_in_RC_Vector(members,row,col) )
            continue; // above should do this
        box = GETBOX(row,col);
        // Check for a ROW or COL blocked by members of opposite color
        pvrows = &rcrcb.vrows[row];
        pvcols = &rcrcb.vcols[col];
        sprintf(tb,"Check R%dC%dB%d ", row + 1, col + 1, box + 1);
        if (pvrows->size() && pvcols->size()) {
            done = false;
            for (j = 0; j < pvrows->size(); j++) {
                for (k = 0; k < pvcols->size(); k++) {
                    rcrow = pvrows->at(j);
                    rccol = pvcols->at(k);
                    if (GETBOX(rcrow.row,rcrow.col) == GETBOX(rc.row,rc.col)) continue; // skip my BOX
                    if (GETBOX(rccol.row,rccol.col) == GETBOX(rc.row,rc.col)) continue; // skip my BOX
                    // another restriction - MUST be member of the SAME chain
                    if (rccol.cnum != rc.cnum) continue;
                    if (rcrow.cnum != rc.cnum) continue;
                    if (rcrow.set.uval != rccol.set.uval) {
                        pb->line[rc.row].set[rc.col].flag[val0] |= cf_CCE;
                        sprintf(tb,"Elim %d R%dC%dB%d due ", setval, rc.row + 1,
                            rc.col + 1, GETBOX(rc.row,rc.col) + 1);
                        sprintf(EndBuf(tb),"R%dC%dB%d %s br %d ",
                            rcrow.row + 1, rcrow.col + 1, GETBOX(rcrow.row,rcrow.col) + 1, CLRSTG(rcrow.set.uval), rc.row + 1);
                        sprintf(EndBuf(tb),"R%dC%dB%d %s bc %d ",
                            rccol.row + 1, rccol.col + 1, GETBOX(rccol.row,rccol.col) + 1, CLRSTG(rccol.set.uval), rc.col + 1);
                        OUTITCS(tb);
                        count++;
                        done = true;
                        break;
                    }
                }
                if (done)
                    break;
            }
            if (!done)
                strcat(tb,"no elim found");
        } else {
            strcat(tb,"NO row or col cnt");
        }
        if (*tb) OUTITCS(tb);

        if (skip_box_elims)
            continue;

        // Check if have 2 members of opposite color in this BOX
        pvrc = &rcrcb.vboxs[GETBOX(rc.row,rc.col)]; // get BOX members
        cnt = 0;
        for (j = 0; j < pvrc->size(); j++) {
            rc2 = pvrc->at(j);
            if (GETBOX(rc2.row,rc2.col) != GETBOX(rc.row,rc.col)) continue; // not my BOX
            if ((rc2.row == rc.row)&&(rc2.col == rc.col)) continue; // skip me
            rcpairs[cnt++] = rc2; // keep members of SAME BOX
        }
        if (cnt >= 2) {
            // can elminate setval from rc, since color rule says if 2 color pairs
            // in same BOX (of opposite colors), then it MUST be one of them
            // so this MUST not be possible
            for (a = 0; a < cnt; a++) {
                b = a + 1;
                if (b == cnt)
                    b = 0;
                if (rcpairs[a].set.uval != rcpairs[b].set.uval) {
                    pb->line[rc.row].set[rc.col].flag[val0] |= cf_CCE;
                    sprintf(tb,"Elim %d %s due ", setval, Get_RC_RCB_Stg(&rc) );
                    sprintf(EndBuf(tb),"%s %s ",
                        Get_RC_RCB_Stg(&rcpairs[a]), CLRSTG(rcpairs[a].set.uval) );
                    sprintf(EndBuf(tb),"%s %s ",
                        Get_RC_RCB_Stg(&rcpairs[b]), CLRSTG(rcpairs[b].set.uval) );
                    OUTITCS(tb);
                    count++;
                    break;
                }
            }
        }
    }
    // Check SAME chain, if opposite colors isolate a single value
    if (b_bDoSameChain && (count == 0)) {
        int cn, val;
        vRC cmems, vdone;
        PSET ps;
        PROWCOL prc, prc2;
        ROWCOL rc3;
        for (cn = 0; cn <= chain_number; cn++) {
            cmems.clear();
            for (i = 0; i < max; i++) {
                rc = members[i];
                if (rc.cnum == cn)
                    cmems.push_back(rc);
            }
            if (cmems.size() > 2) {
                for (i = 0; i < cmems.size(); i++) {
                    rc = cmems[i];
                    vdone.push_back(rc);
                    for (j = 0; j < cmems.size(); j++) {
                        if (j == i) continue;
                        rc2 = cmems[j]; // got TWO members of THIS chain
                        if (rc.set.uval == rc2.set.uval) continue; // have SAME color - no interest
                        if (!Row_Col_in_RC_Vector(vdone, rc2.row, rc2.col)) {
                            // two members of SAME chain, DIFFERENT color
                            prc = &rc;
                            prc2 = &rc2;
                            if (SAME_BOX(prc,prc2)) {
                                // Hmmm, and ON and OFF in the SAME BOX
                                // Shud check and eliminate any other empties with this CANDIDATE
                                int r = (rc.row / 3) * 3;
                                int c = (rc.col / 3) * 3;
                                int rw, cl;
                                for (rw = 0; rw < 3; rw++) {
                                    for (cl = 0; cl < 3; cl++) {
                                        rc3.row = r + rw;
                                        rc3.col = c + cl;
                                        if ((rc3.row == rc.row) && (rc3.col == rc.col)) continue; // skip 1
                                        if ((rc3.row == rc2.row) && (rc3.col == rc2.col)) continue; // skip 2
                                        val = pb->line[rc3.row].val[rc3.col];    // row of 1, col of 2
                                        if (!val) {
                                            ps = &pb->line[rc3.row].set[rc3.col];
                                            if ( ps->val[val0] ) { // HAS the setval candidate
                                                if ( !(ps->flag[val0] & cf_CCE) ) {
                                                    // ps->flag[val0] |= cf_CCE;
                                                    sprtf("WARNING: missed elim of %d in %s due ", setval,
                                                        Get_RC_RCB_Stg(&rc3) );
                                                    sprintf(EndBuf(tb),"Box %s ", Get_RC_RCBC_Stg(&rc) );
                                                    sprintf(EndBuf(tb),"Box %s ", Get_RC_RCBC_Stg(&rc2) );
                                                    OUTITCS(tb);
                                                }
                                            }
                                        }
                                    }
                                }
                            } else {
                                // check the intersection of row and column contain a candidate
                                rc3.row = rc.row;
                                rc3.col = rc2.col;
                                if (!Row_Col_in_RC_Vector(cmems, rc3.row, rc3.col)) {
                                    val = pb->line[rc3.row].val[rc3.col];    // row of 1, col of 2
                                    if (!val) {
                                        ps = &pb->line[rc3.row].set[rc3.col];
                                        if ( ps->val[val0] ) { // HAS the setval candidate
                                            if ( !(ps->flag[val0] & cf_CCE) ) {
                                                ps->flag[val0] |= cf_CCE;
                                                sprintf(tb,"Elim %d %s due ", setval,
                                                    Get_RC_RCB_Stg(&rc3) );
                                                sprintf(EndBuf(tb),"Row %s ", Get_RC_RCBC_Stg(&rc) );
                                                sprintf(EndBuf(tb),"Col %s ", Get_RC_RCBC_Stg(&rc2) );
                                                OUTITCS(tb);
                                                count++;
                                            }
                                        }
                                    }
                                }
                                rc3.row = rc2.row;
                                rc3.col = rc.col;
                                if (!Row_Col_in_RC_Vector(cmems, rc3.row, rc3.col)) {
                                    val = pb->line[rc2.row].val[rc.col];    // row of 2, col of 1
                                    if (!val) {
                                        ps = &pb->line[rc2.row].set[rc.col];
                                        if ( ps->val[val0] ) { // HAS the setval candidate
                                            if ( !(ps->flag[val0] & cf_CCE) ) {
                                                ps->flag[val0] |= cf_CCE;
                                                sprintf(tb,"Elim %d R%dC%dB%d due ", setval, 
                                                    rc2.row + 1, rc.col + 1, GETBOX(rc2.row,rc.col));
                                                sprintf(EndBuf(tb),"Row %s ", Get_RC_RCBC_Stg(&rc2) );
                                                sprintf(EndBuf(tb),"Col %s ", Get_RC_RCBC_Stg(&rc) );
                                                OUTITCS(tb);
                                                count++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (count)
                break;
        }
    }
    if (on_color_test && (count == 0)) {
        count++;    // give a count anyway, to paint cells
        set_repaint(false);
        sprtf("on_color_test: bumped count to force display\n");
    }
    return count;
}

bool RC_Already_Marked( PROWCOL prc, vRC &vrc, int setval, int flag )
{
    size_t max = vrc.size();
    size_t i;
    ROWCOL rc;
    int row = prc->row;
    int col = prc->col;
    int val = setval - 1;
    for (i = 0; i < max; i++) {
        rc = vrc[i];
        if ((rc.row == row)&&(rc.col == col)) {
            if ( rc.set.flag[val] & flag ) // if marked
                return true;
            break;
        }
    }
    return false;
}

int Follow_Chain( PABOX2 pb, vRC &members, PROWCOL prc_in, int rccnt, PRCRCB prcrcb,
    int setval, int bgn_color, vRC &vrc2, int dep )
{
    int count = 0;
    int i, k;
    ROWCOL rc, rc2;
    vRC *pvrc;
    int val = setval - 1;
    char *tb = GetNxtBuf();
    size_t max = vrc2.size();
    size_t j;
    PROWCOL prc;
    int pcolor = (bgn_color ? 0 : 1);
    for (i = 0; i < rccnt; i++) {
        rc = prc_in[i];    // seek a PAIR for this - forming a chain
        prc = &rc;
        sprintf(tb,"FC%d:%d: %d %s %s ", (int)i + 1, dep, setval, 
            Get_RC_RCB_Stg(&rc),
            CLRSTG( (bgn_color ? 0 : 1) ) );
        ROWCOL rcpairs[3];
        int cnt = 0;
        //DBGRC(prc,7,9);
        pvrc = &prcrcb->vrows[rc.row];
        if (pvrc->size() == 2) {
            // got two in this ROW
            if (rc.col == pvrc->at(0).col) {
                rc2 = pvrc->at(1);
                prc = &pvrc->at(1);
            } else {
                rc2 = pvrc->at(0);
                prc = &pvrc->at(0);
            }
            if ( !(rc2.set.flag[val] & cf_CC) ) { // if NOT mark include
                // got a ROW pair, maybe
                if (RC_Already_Marked( prc, vrc2, setval, cf_CC )) {
                    prc->set.flag[val] |= cf_CC; // should MARK this
                } else {
                    sprintf(EndBuf(tb),"Rpair %s %s ", Get_RC_RCB_Stg(&rc2),
                        CLRSTG(pcolor) );
                    rcpairs[cnt++] = rc2;
                }
            }
        }
        pvrc = &prcrcb->vcols[rc.col];
        if (pvrc->size() == 2) {
            // got two in this COL
            if (rc.col == pvrc->at(0).col) {
                rc2 = pvrc->at(1);
                prc = &pvrc->at(1);
            } else {
                rc2 = pvrc->at(0);
                prc = &pvrc->at(0);
            }
            if ( !(rc2.set.flag[val] & cf_CC) ) { // if NOT mark include
                // got a ROW pair, maybe
                if (RC_Already_Marked( prc, vrc2, setval, cf_CC )) {
                    prc->set.flag[val] |= cf_CC; // should MARK this
                } else {
                    sprintf(EndBuf(tb),"Cpair %s %s ", Get_RC_RCB_Stg(&rc2),
                        CLRSTG(pcolor) );
                    rcpairs[cnt++] = rc2;
                }
            }
        }
        pvrc = &prcrcb->vboxs[GETBOX(rc.row,rc.col)];
        if (pvrc->size() == 2) {
            // got two in this BOX
            if (rc.col == pvrc->at(0).col) {
                rc2 = pvrc->at(1);
                prc = &pvrc->at(1);
            } else {
                rc2 = pvrc->at(0);
                prc = &pvrc->at(0);
            }
            if ( !(rc2.set.flag[val] & cf_CC) ) { // if NOT mark include
                // got a ROW pair, maybe
                if (RC_Already_Marked( prc, vrc2, setval, cf_CC )) {
                    prc->set.flag[val] |= cf_CC; // should MARK this
                } else {
                    sprintf(EndBuf(tb),"Bpair %s %s ", Get_RC_RCB_Stg(&rc2),
                        CLRSTG(pcolor) );
                    rcpairs[cnt++] = rc2;
                }
            }
        }
        //count += cnt;
        if (cnt) {
            // FOUND 1 or more PAIR
            OUTITCS(tb);
            sprintf(tb,"follow%d %d %s %s ", dep, setval, 
                Get_RC_RCB_Stg(&rc),
                CLRSTG( (bgn_color ? 0 : 1) ) );
            pcolor = bgn_color;
            for (k = 0; k < cnt; k++) {
                rc2 = rcpairs[k];
                if (Row_Col_in_RC_Vector(members, rc2.row, rc2.col)) continue;
                if (rc2.set.flag[val] & cf_CC) continue; // mark include
                rc2.set.flag[val] |= cf_CC; // mark include
                rc2.set.uval = pcolor;
                rc2.cnum = chain_number; // set chain number
                members.push_back(rc2);
                color_chain.push_back(rc2);
                count++;
                for (j = 0; j < max; j++) {
                    rc = vrc2[j];
                    if ((rc.row == rc2.row)&&(rc.col == rc2.col)) {
                        vrc2[j].set.flag[val] |= cf_CC; // mark included
                        break;
                    }
                }
                sprintf(EndBuf(tb),"pair %s %s ", Get_RC_RCB_Stg(&rc2),
                    CLRSTG(rc2.set.uval) );
            }
            OUTITCS(tb);
            // Need to FOLLOW chain on each of these... MAYBE??????
            count += Follow_Chain( pb, members, rcpairs, cnt, prcrcb, setval,
                 (bgn_color ? 0 : 1), vrc2, dep + 1 );  // mark with NEXT color
        } else {
            strcat(tb,"NONE");
        }
        if (*tb)
            OUTITCS(tb);
    }
    sprintf(tb,"Exit FC depth %d found %d", dep, count);
    OUTITCS(tb);
    return count;
}


int Do_Color_Vector( PABOX2 pb, vRC &vrc )
{
    int count = 0;
    int setval, val, cnt, k, counter, pcolor;
    size_t max = vrc.size();
    size_t i, j, max2;
    ROWCOL rc, rc2;
    vRC vrc2, members;
    vRC final_members;
    RCRCB rcrcb;
    vRC *pvrc;
    int icount = 0;
    PROWCOL prc;
    char *tb = GetNxtBuf();
    int bgn_color;

    for (setval = do_color_test; setval <= 9; setval++) {

        color_chain.clear();
        chain_number = 0;       // initialize chain number
        chain_valid = false;
        chain_value = setval;

        if (add_debug_cs) sprtf("CS: Processing candidate %d\n", setval);
        vrc2.clear();
        val = setval - 1;
        for (k = 0; k < 9; k++) {
            rcrcb.vrows[k].clear();
            rcrcb.vcols[k].clear();
            rcrcb.vboxs[k].clear();
        }
        for (i = 0; i < max; i++) {
            rc = vrc[i];
            if ( !rc.set.val[val] ) continue; // no setval candidate
            if (rc.set.flag[val] & cf_CC) continue; // already include
            rcrcb.vrows[rc.row].push_back(rc);
            rcrcb.vcols[rc.col].push_back(rc);
            rcrcb.vboxs[GETBOX(rc.row,rc.col)].push_back(rc);
            vrc2.push_back(rc);
        }
        max2 = vrc2.size(); // get sub-set
        if (add_debug_cs) {
            sprtf("CS: Process sub-set %d of %d\n", (int)max2, (int)max);
            sprintf(tb,"Rows with TWO %d ",setval);
            for (i = 0; i < 9; i++) {
                pvrc = &rcrcb.vrows[i];
                if ( pvrc->size() == 2 ) {
                    rc = pvrc->at(0);
                    rc2 = pvrc->at(1);
                    sprintf(EndBuf(tb),"(%s + %s) ", Get_RC_RCB_Stg(&rc), Get_RC_RCB_Stg(&rc2));
                }
            }
            OUTITCS(tb);
            sprintf(tb,"Cols with TWO %d ",setval);
            for (i = 0; i < 9; i++) {
                pvrc = &rcrcb.vcols[i];
                if ( pvrc->size() == 2 ) {
                    rc = pvrc->at(0);
                    rc2 = pvrc->at(1);
                    sprintf(EndBuf(tb),"(%s + %s) ", Get_RC_RCB_Stg(&rc), Get_RC_RCB_Stg(&rc2));
                }
            }
            OUTITCS(tb);
            sprintf(tb,"Boxs with TWO %d ",setval);
            for (i = 0; i < 9; i++) {
                pvrc = &rcrcb.vboxs[i];
                if ( pvrc->size() == 2 ) {
                    rc = pvrc->at(0);
                    rc2 = pvrc->at(1);
                    sprintf(EndBuf(tb),"(%s + %s) ", Get_RC_RCB_Stg(&rc), Get_RC_RCB_Stg(&rc2));
                }
            }
            OUTITCS(tb);
        }
        bgn_color = 0;
        members.clear();
        for (i = 0; i < max2; i++) {
            rc = vrc2[i];
            if (rc.set.flag[val] & cf_CC) continue; // already include
            if (Row_Col_in_RC_Vector(members, rc.row, rc.col)) continue;

            sprintf(tb,"%2d: %d %s ", (int)i + 1, setval, Get_RC_RCB_Stg(&rc));
            cnt = 0;
            ROWCOL rcpairs[3];
            pvrc = &rcrcb.vrows[rc.row];
            if (pvrc->size() == 2) {
                // got two in this ROW
                if (rc.col == pvrc->at(0).col) {
                    rc2 = pvrc->at(1);
                    prc = &pvrc->at(1);
                } else {
                    rc2 = pvrc->at(0);
                    prc = &pvrc->at(0);
                }
                // got a ROW pair
                sprintf(EndBuf(tb),"Rpair %s ", Get_RC_RCB_Stg(&rc2));
                rcpairs[cnt] = rc2;
                cnt++;
                prc->set.flag[val] |= cf_CC; // mark include
            }
            pvrc = &rcrcb.vcols[rc.col];
            if (pvrc->size() == 2) {
                // got two in this COL
                if (rc.row == pvrc->at(0).row) {
                    rc2 = pvrc->at(1);
                    prc = &pvrc->at(1);
                } else {
                    rc2 = pvrc->at(0);
                    prc = &pvrc->at(0);
                }
                sprintf(EndBuf(tb),"Cpair %s ", Get_RC_RCB_Stg(&rc2));
                rcpairs[cnt] = rc2;
                cnt++;
                prc->set.flag[val] |= cf_CC; // mark include
            }
            pvrc = &rcrcb.vboxs[GETBOX(rc.row,rc.col)];
            if (pvrc->size() == 2) {
                // got two in this BOX
                if ((rc.col == pvrc->at(0).col)&&(rc.row == pvrc->at(0).row)) {
                    rc2 = pvrc->at(1);
                    prc = &pvrc->at(1);
                } else {
                    rc2 = pvrc->at(0);
                    prc = &pvrc->at(0);
                }
                sprintf(EndBuf(tb),"Bpair %s ", Get_RC_RCB_Stg(&rc2));
                rcpairs[cnt] = rc2;
                cnt++;
                prc->set.flag[val] |= cf_CC; // mark include
            }
            if (cnt) {
                OUTITCS(tb);
                rc.set.flag[val] |= cf_CC; // mark include
                //color = (color ? 0 : 1);
                rc.set.uval = bgn_color;
                rc.cnum = chain_number;  // set chain number
                members.push_back(rc);  // store FIRST member
                color_chain.push_back(rc); // ADD to color chain
                //vrows[rc.row];
                vrc2[i].set.flag[val] |= cf_CC; // mark included
                icount++;
                sprintf(tb,"Follow %s %s ", Get_RC_RCB_Stg(&rc),
                    CLRSTG(rc.set.uval));
                pcolor = (bgn_color ? 0 : 1);    // flip color for pairs
                for (k = 0; k < cnt; k++) {
                    rc2 = rcpairs[k];
                    if (Row_Col_in_RC_Vector(members, rc2.row, rc2.col)) continue;
                    rc2.set.flag[val] |= cf_CC; // mark include
                    rc2.set.uval = pcolor;
                    rc2.cnum = chain_number; // set chain number
                    members.push_back(rc2);
                    color_chain.push_back(rc2);
                    for (j = 0; j < max; j++) {
                        if (j == i) continue;
                        rc = vrc2[j];
                        if ((rc.row == rc2.row)&&(rc.col == rc2.col)) {
                            vrc2[j].set.flag[val] |= cf_CC; // mark included
                            break;
                        }
                    }
                    icount++;
                    sprintf(EndBuf(tb),"pair %d %s %s ", k + 1,
                        Get_RC_RCB_Stg(&rc2),
                        CLRSTG(rc2.set.uval) );
                }
                OUTITCS(tb);
                // Need to FOLLOW chain on each of these...
                counter = Follow_Chain( pb, members, rcpairs, cnt, &rcrcb, setval,
                        (pcolor ? 0 : 1), vrc2, 1 );  // mark with NEXT color
                chain_number++; // bump chain number
            } else {
                strcat(tb,"NONE! ");
                sprintf(EndBuf(tb), "rc %d cc %d bc %d", (int)rcrcb.vrows[rc.row].size(),
                    (int)rcrcb.vcols[rc.col].size(), (int)rcrcb.vboxs[GETBOX(rc.row,rc.col)].size() );
                OUTITCS(tb);
            }
        }
        icount = Check_Color_Members( pb, members, vrc, setval );
        if (on_color_test && (icount == 0)) {
            sprtf("Bumping 'count' due to color test %d\n", setval);
            icount++;
        }
        if (icount) {
            count += Transfer_Color_Members( pb, members, final_members );
        }
        if (only_one_cs && count)
            break;
        if (on_color_test)
            break;
    }

    if (count ) {
        //Mark_Color_Members( pb, members );
        if (g_bMrkNM && (setval <= 9)) {
            Clear_Cell_Flag(pb, cl_CSC);
            Mark_Color_Non_Members( pb, final_members, setval, vrc );
        }
        Mark_Color_Members( pb, final_members );
    }
    return count;
}

// Simple Color Scan
int Do_Color_Scan( PABOX2 pb )
{
    int count = 0;
    int row, col, val;
    ROWCOL rc;
    vRC vrc;
    vrc.clear(); // clear vector
    // do whole 9x9
    int cnt = 0;
    if (add_debug_cs) sprtf("Doing Color scan...\n");
    // collect all EMPTY cells into a VECTOR
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue; // only empty cells
            PSET ps = &pb->line[row].set[col];
            rc.row = row;
            rc.col = col;
            //rc.cnt = GETBOX(row,col); // use for BOX number 0-9
            COPY_SET(&rc.set,ps);
            // pb->line[row].set[col].uval
            rc.set.uval = -1;   // mark INVALID color
            rc.set.cellflg = 0; // marked if a member
            vrc.push_back(rc);
            cnt++;
        }
    }
    if (vrc.size()) {
        if (add_debug_cs) sprtf("Passed %d to Do_Color_Vector\n", vrc.size());
        count = Do_Color_Vector( pb, vrc );
    }

    if (count) {
        sprtf("S2f: Color scan eliminations. To %d\n", sg_Fill_Color);
        pb->iStage = sg_Fill_Color;
    } else {
        Set_Chain_Invalid();
        //count = Do_Color_Scan2(pb);
        if (count) {
            sprtf("S2f: Color scan eliminations. To %d\n", sg_Fill_Color);
            pb->iStage = sg_Fill_Color;
        } else {
            sprtf("S2f: Color scan. No elims. To begin.\n");
            pb->iStage = sg_Begin;
        }
    }
    return count;
}


int Do_Fill_Color(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_CCE, cf_CC, "CC", "Color", cl_MCC );
    Set_Chain_Invalid();
    return count;
}

// ==========================================================
// END COLOR STRATEGY
// ====================
#endif // #ifndef USE_COLOR2_SCAN

// eof - Sudo_Coloring.cxx


#endif // 0 old code to be removed at some point

