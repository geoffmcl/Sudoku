// Sudo_SLink3.cxx

// ANOTHER TRY - Tried this so many times!!!

#include "Sudoku.hxx"

#define OUTITSL3(tb) if (add_debug_sl3) OUTIT(tb)

int only_one_sl3 = 1;

#define sl3_elim cf_CCE
#define sl3_mark cf_CC

vRCP added_pairs;    // piars added during LINK scan and elim scan

int Show_RCP_Pairs3( vRCP *psrc, char *type )
{
    int count = 0;
    RCPAIR rcpair;
    size_t max, ii;
    int setval;
    char *tb = GetNxtBuf();
    max = psrc->size();
    //sprintf(tb, "ExChain %d ", (int)max);
    sprintf(tb, "%s %d ", type, (int)max);
    for (ii = 0; ii < max; ii++) {
        rcpair = psrc->at(ii);
        setval = rcpair.rowcol[0].set.uval;
        Append_RCPAIR_Settings( tb, &rcpair, setval );
        count++;
    }
    OUTITSL3(tb);
    return count;
}


int Show_SL_Colored_Pair_Chains(PABOX2 pb, int setval, vRCP *pvrcp)
{
    int count = 0;
    size_t ii, max;
    int i, i2, cnt;
    char *tb = GetNxtBuf();
    vINT vint;
    time_t ch_num;
    PRCPAIR prcpair;
    max = pvrcp->size();
    sprintf(tb,"Chains %d Nums [", (int)max);
    for (ii = 0; ii < max; ii++) {
        prcpair = &pvrcp->at(ii);
        ch_num = prcpair->rowcol[0].set.tm;
        i = (int)ch_num;
        if (Int_in_iVector( i, &vint )) continue;
        vint.push_back(i);
        sprintf(EndBuf(tb),"#%d ", i);
    }
    // process CHAIN by CHAIN
    size_t jj;
    size_t max2 = vint.size();
    sprintf(EndBuf(tb),"] Total %d chains", (int)max2);
    OUTITSL3(tb);
    vRCP vrcp;
    for (jj = 0; jj < max2; jj++) {
        i = vint[jj];
        vrcp.clear();
        cnt = 0;
        for (ii = 0; ii < max; ii++) {
            prcpair = &pvrcp->at(ii);
            ch_num = prcpair->rowcol[0].set.tm;
            i2 = (int)ch_num;
            if (i2 == i) {
                vrcp.push_back(*prcpair);
                cnt++;
            }
        }
        if (cnt) {
            sprintf(tb,"SL Ch#%d: ", i);
            Show_RCP_Pairs3( &vrcp, tb );
        } else {
            sprintf(tb,"Seeking chain number %d, in list %d, but found NONE!", i, (int)max);
            EXPLODE(tb); // Show_SL_Colored_Pair_Chains() none in list
        }
    }
    return count;
}



// CALLED FROM int Process_SL_Pairs3(PABOX2 pb, int setval)
// if ((setvalcnt == 1) && count && g_bIBL && !g_bIWL && !g_bASW) {
// Is ONLY a list of STRONG LINKS for ONE setval
// BUT was NOT able to eliminate anything using a CHAIN
// Then added COLOR alternatively to the LINKS
// BUT THERE CAN BE SEVERAL CHAIN - Must ONLY process ch_num BY ch_num
BOOL g_bElimIfSRCB = FALSE; // NEW RULE: Only elim IF seen by opposite STRONG pairs
// COLORS, but where they are NOT same ROW, COL or BOX
BOOL g_bRCElimIfNot1 = FALSE;
int Process_SL_Colored_Pairs_for_Elims_per_ch_num3(PABOX2 pb, int setval, vRCP *pvrcp)
{
    int count = 0;
    size_t ii, max;
    vRC empty, pairs;
    RCRCB rcrcb, rcrcbpairs;
    PROWCOL prc, prcr, prcc, prcb;
    vRC *pvrow, *pvcol, *pvbox;
    int box;
    uint64_t flg1, flg2;
    size_t maxr, maxc, maxb, rr, cc, bb;
    int lval = setval - 1;
    vRC elims, elimby;
    bool bad;
    RCPAIR rcpair;
    char *tb = GetNxtBuf();
    // get ALL the emty cells with this setval
    int cnt = Get_Empty_Cells_of_setval( pb, empty, setval, &rcrcb );

    cnt = Get_Pairs_of_setval( pb, setval, &pairs, &rcrcbpairs, pvrcp);
    // ok, have all the SL pairs in vRC pairs, and int vRC row, col and boxed
    // Check each candidate, NOT a member of the pairs, ie UNCOLORED
    // with what colors it can 'SEE'
    max = empty.size();
    sprintf(tb,"Elims by %d SL pairs, with %d cand of %d. ", (int)pvrcp->size(),
        (int)(max - pairs.size()), setval);
    OUTITSL3(tb);
    for (ii = 0; ii < max; ii++) {
        prc = &empty[ii];
        if (prc_in_chain(prc,&pairs)) continue; // is a member of COLOR set
        if (prc_in_chain(prc,&elims)) continue; // is alread ELIMINATED
        // do an opposing COLOR pair 'see' this chicken?
        pvrow = &rcrcbpairs.vrows[prc->row];
        pvcol = &rcrcbpairs.vcols[prc->col];
        box = GETBOX(prc->row,prc->col);
        pvbox = &rcrcbpairs.vboxs[box];
        maxr = pvrow->size();
        maxc = pvcol->size();
        maxb = pvbox->size();
        bad = false;
        flg1 = 0;
        flg2 = 0;
        if (g_bElimIfSRCB) {
            // Rule 1 - If we have TWO chain members in this 'chickens' ROW of OPPOSITE COLOR
            for (rr = 0; rr < maxr; rr++) {
                prcr = &pvrow->at(rr); // get first in ROW
                flg1 = prcr->set.flag[lval] & cf_XAB; // extract FLAG
                for (cc = rr + 1; cc < maxr; rr++) {
                    prcc = &pvrow->at(cc); // get next in ROW
                    flg2 = prcc->set.flag[lval] & cf_XAB; // extract FLAG
                    if (flg1 && flg2 && (flg1 != flg2) && (flg1 != cf_XAB) && (flg2 != cf_XAB)) {
                        // good chance CICKEN must DIE ;=))
                        elims.push_back(*prc);
                        elimby.push_back(*prcr);
                        elimby.push_back(*prcc);
                        sprintf(tb,"RROW Chk %s facing R=%d C=%d B=%d ",  Get_RC_setval_RC_Stg(prc,setval),
                            (int)maxr, (int)maxc, (int)maxb);
                        sprintf(EndBuf(tb),"elim by %s & %s",Get_RC_setval_color_Stg(prcr,setval),
                            Get_RC_setval_color_Stg(prcc,setval));
                        OUTITSL3(tb);
                    }
                }
            }
            if (prc_in_chain(prc,&elims)) continue; // is now ELIMINATED
            for (cc = 0; cc < maxc; cc++) {
                prcc = &pvcol->at(cc); // get first in COL
                flg1 = prcc->set.flag[lval] & cf_XAB; // extract FLAG
                for (rr = cc + 1; rr < maxc; rr++) {
                    prcr = &pvcol->at(rr); // get next in COL
                    flg2 = prcr->set.flag[lval] & cf_XAB; // extract FLAG
                    if (flg1 && flg2 && (flg1 != flg2) && (flg1 != cf_XAB) && (flg2 != cf_XAB)) {
                        // good chance CICKEN must DIE ;=))
                        elims.push_back(*prc);
                        elimby.push_back(*prcr);
                        elimby.push_back(*prcc);
                        sprintf(tb,"CCOL Chk %s facing R=%d C=%d B=%d ",  Get_RC_setval_RC_Stg(prc,setval),
                            (int)maxr, (int)maxc, (int)maxb);
                        sprintf(EndBuf(tb),"elim by %s & %s",Get_RC_setval_color_Stg(prcr,setval),
                            Get_RC_setval_color_Stg(prcc,setval));
                        OUTITSL3(tb);
                    }
                }
            }
        }
        if (prc_in_chain(prc,&elims)) continue; // is now ELIMINATED
        if (g_bRCElimIfNot1 || ((maxr == 1) && (maxc == 1))) {
            for (rr = 0; rr < maxr; rr++) {
                prcr = &pvrow->at(rr); // get first in ROW
                flg1 = prcr->set.flag[lval] & cf_XAB; // extract FLAG
                // in a ROW the chicken can be seen by this COLOR
                for (cc = 0; cc < maxc; cc++) {
                    prcc = &pvcol->at(cc); // get first in COL
                    flg2 = prcc->set.flag[lval] & cf_XAB; // extract FLAG
                    // in a COL the chicken can be seen by this COLOR
                    if (flg1 && flg2 && (flg1 != flg2) && (flg1 != cf_XAB) && (flg2 != cf_XAB)) {
                        // good chance CHICKEN must DIE ;=))
                        elims.push_back(*prc);
                        elimby.push_back(*prcr);
                        elimby.push_back(*prcc);
                        rcpair.rowcol[0] = *prc;
                        rcpair.rowcol[1] = *prcr;
                        added_pairs.push_back(rcpair);
                        rcpair.rowcol[0] = *prc;
                        rcpair.rowcol[1] = *prcc;
                        added_pairs.push_back(rcpair);
                        sprintf(tb,"RC Chk %s facing R=%d C=%d B=%d ",  Get_RC_setval_RC_Stg(prc,setval),
                            (int)maxr, (int)maxc, (int)maxb);
                        sprintf(EndBuf(tb),"elim by %s & %s",Get_RC_setval_color_Stg(prcr,setval),
                            Get_RC_setval_color_Stg(prcc,setval));
                        OUTITSL3(tb);
                    }
                }
            }
        }
        if (prc_in_chain(prc,&elims)) continue; // is now ELIMINATED

        if ((maxb >= 2) && g_bElimIfSRCB) {
            flg1 = 0;
            flg2 = 0;
            bad = false;
            for (bb = 0; bb < maxb; bb++) {
                prcb = &pvbox->at(bb); // get first in BOX
                if (flg1 && flg2) {
                    bad = true;
                } else if (flg1) {
                    prcr = prcb; // set next in BOX
                    flg2 = prcb->set.flag[lval] & cf_XAB; // extract FLAG
                } else {
                    prcc = prcb; // set first
                    flg1 = prcb->set.flag[lval] & cf_XAB; // extract FLAG
                }
                if (bad)
                    break;
            }
            if (!bad && flg1 && flg2 && (flg1 != flg2) && (flg1 != cf_XAB) && (flg2 != cf_XAB)) {
                // the CHICKEN is BOXED IN
                elims.push_back(*prc);
                elimby.push_back(*prcr);
                elimby.push_back(*prcc);
                sprintf(tb,"BOX %s facing R=%d C=%d B=%d ",  Get_RC_setval_RC_Stg(prc,setval),
                    (int)maxr, (int)maxc, (int)maxb);
                sprintf(EndBuf(tb),"elim by %s & %s",Get_RC_setval_color_Stg(prcr,setval),
                    Get_RC_setval_color_Stg(prcc,setval));
                OUTITSL3(tb);
            }
        }
        if (prc_in_chain(prc,&elims)) continue; // is now ELIMINATED
        sprintf(tb,"Chk %s facing R=%d C=%d B=%d NOT ELIMINATED",  Get_RC_setval_RC_Stg(prc,setval),
            (int)maxr, (int)maxc, (int)maxb);
        OUTITSL3(tb);
    }
    max = elims.size();
    if (max) {
        sprintf(tb,"SL-ELIMS: ");
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            sprintf(EndBuf(tb),"%s ",Get_RC_setval_RC_Stg(prc,setval));
            pb->line[prc->row].set[prc->col].flag[setval - 1] |= sl3_elim; // cf_CCE;
        }
        max = elimby.size();
        for (ii = 0; ii < max; ii++) {
            prc = &elimby[ii];
            pb->line[prc->row].set[prc->col].flag[setval - 1] |= sl3_mark; // cf_CC;
            flg1 = (prc->set.flag[lval] & cf_XAB);
            const char *clrtype = ((flg1 == cf_XAB) ? "?" : (flg1 & cf_XCA) ? "A" : (flg1 & cf_XCB) ? "B" : "U");
            sprintf(EndBuf(tb),"seen %s%s ", Get_RC_setval_RC_Stg( prc, setval ), clrtype);
        }
        OUTITSL3(tb);
        count = (int)max;

    }

    return count;
}

int Process_SL_Colored_Pairs_for_Elims3(PABOX2 pb, int setval, vRCP *pvrcp)
{
    int count = 0;
    size_t ii, max;
    int i, i2;
    char *tb = GetNxtBuf();
    vINT vint;
    time_t ch_num;
    PRCPAIR prcpair;
    sprintf(tb,"Chain Nums ");
    max = pvrcp->size();
    for (ii = 0; ii < max; ii++) {
        prcpair = &pvrcp->at(ii);
        ch_num = prcpair->rowcol[0].set.tm;
        i = (int)ch_num;
        if (Int_in_iVector( i, &vint )) continue;
        vint.push_back(i);
        sprintf(EndBuf(tb),"#%d ", i);
    }
    // must process ch_num by ch_num
    size_t jj;
    size_t max2 = vint.size();
    sprintf(EndBuf(tb),"Total %d ch_nums", (int)max2);
    OUTITSL3(tb);
    vRCP vrcp;
    for (jj = 0; jj < max2; jj++) {
        i = vint[jj]; 
        vrcp.clear();
        sprintf(tb,"Ch_num %d: ", i);
        for (ii = 0; ii < max; ii++) {
            prcpair = &pvrcp->at(ii);
            ch_num = prcpair->rowcol[0].set.tm;
            i2 = (int)ch_num;
            if (i2 == i) {
                vrcp.push_back(*prcpair);
                sprintf(EndBuf(tb),"%s ", Get_RCPAIR_Stg(prcpair,setval));
            }
        }
        size_t max3 = vrcp.size();
        sprintf(EndBuf(tb),"Len %d", (int)max3);
        OUTITSL3(tb);
        if (max3 > 1) {
            count += Process_SL_Colored_Pairs_for_Elims_per_ch_num3( pb, setval, &vrcp );
            if (count && only_one_sl3)
                break;
        }
    }
    return count;
}

BOOL g_bChkWLAlso = TRUE;
int Mark_All_Pairs_linked_with_this_pair3( PABOX2 pb, int setval, vRCP *pp,
    PRCPAIR prcp1, PRCPAIR prcp2, time_t ch_num )
{
    int count = 0;
    PRCPAIR prcp;
    // 4 cells passed
    PROWCOL prc1, prc2;
    PROWCOL prc3, prc4;
    // 2 cell of each NEW pair
    PROWCOL prcA, prcB;
    size_t max, ii;
    int row1, col1, box1;
    int row2, col2, box2;
    int row3, col3, box3;
    int row4, col4, box4;
    int rowA, colA, boxA;
    int rowB, colB, boxB;
    uint64_t flg1, flg2, flg3, flg4, flgA, flgB;
    bool gotlnk;
    RCPAIR rcpair;
    int lval = setval - 1;
    max = pp->size();
    char *tb = GetNxtBuf();
    prc1 = &prcp1->rowcol[0];
    prc2 = &prcp1->rowcol[1];
    prc3 = &prcp2->rowcol[0];
    prc4 = &prcp2->rowcol[1];
    flg1 = prc1->set.flag[lval] & cf_XAB;
    flg2 = prc2->set.flag[lval] & cf_XAB;
    flg3 = prc3->set.flag[lval] & cf_XAB;
    flg4 = prc4->set.flag[lval] & cf_XAB;
    row1 = prc1->row;
    col1 = prc1->col;
    box1 = GETBOX(row1,col1);
    row2 = prc2->row;
    col2 = prc2->col;
    box2 = GETBOX(row2,col2);
    row3 = prc3->row;
    col3 = prc3->col;
    box3 = GETBOX(row3,col3);
    row4 = prc4->row;
    col4 = prc4->col;
    box4 = GETBOX(row4,col4);

    if ( !(flg1 && flg2 && flg3 && flg4) ) {
        sprintf(tb,"Have just COLOR marked ALL cells for setval %d, yet 1 or more is BLANK!",setval);
        strcat(tb,MEOL);
        sprintf(EndBuf(tb),"Cells: %s %s %s %s ", Get_RC_setval_color_Stg(prc1,setval),
            Get_RC_setval_color_Stg(prc2,setval), Get_RC_setval_color_Stg(prc3,setval),
            Get_RC_setval_color_Stg(prc4,setval));
        strcat(tb,MEOL "The COLOR should be 'A' or 'B'. If none will show otherwise");
        EXPLODE(tb); // Mark_All_Pairs_linked_with_this_pair3() - missing COLOR
    }
    sprintf(tb,"S%d: marked ch#%d ", pb->iStage, (int)ch_num);
    for (ii = 0; ii < max; ii++) {
        prcp = &pp->at(ii);
        if (Same_RC_Pair( prcp, prcp1 )) continue;
        if (Same_RC_Pair( prcp, prcp2 )) continue;
        prcA = &prcp->rowcol[0];
        prcB = &prcp->rowcol[1];
        if ( !(prcA->set.uval == setval) ) continue;
        flgA = prcA->set.flag[lval] & cf_XAB;
        flgB = prcB->set.flag[lval] & cf_XAB;
        rowA = prcA->row;
        colA = prcA->col;
        boxA = GETBOX(rowA,colA);
        rowB = prcB->row;
        colB = prcB->col;
        boxB = GETBOX(rowB,colB);
        gotlnk = false;
        // with first PAIR - prc1, prc2
        if (SAMERCCELL(prcA,prc1)) {
            gotlnk = true;
            if ( !flgA ) {
                flgA = flg1;
                prcA->set.flag[lval] |= flg1;
                prcA->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"A1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                if ( !flgB ) {
                    flgB = flg2;
                    prcB->set.flag[lval] |= flg2;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"a1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
            }
        } else if (SAMERCCELL(prcA,prc2)) {
            gotlnk = true;
            if ( !flgA ) {
                flgA = flg2;
                prcA->set.flag[lval] |= flg2;
                prcA->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"A2 %s ", Get_RC_setval_color_Stg(prcA,setval));
                if ( !flgB ) {
                    flgB = flg1;
                    prcB->set.flag[lval] |= flg1;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"a2 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
            }
        }
        if (SAMERCCELL(prcB,prc1)) {
            gotlnk = true;
            if ( !flgB ) {
                flgB = flg1;
                prcB->set.flag[lval] |= flg1;
                prcB->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"B1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                if ( !flgA ) {
                    flgA = flg2;
                    prcA->set.flag[lval] |= flg2;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"b1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
            }
        } else if (SAMERCCELL(prcB,prc2)) {
            gotlnk = true;
            if ( !flgB ) {
                flgB = flg2;
                prcB->set.flag[lval] |= flg2;
                prcB->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"B2 %s ", Get_RC_setval_color_Stg(prcB,setval));
                if ( !flgA ) {
                    prcA->set.flag[lval] |= flg1;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"b2 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
            }
        }
        // with second PAIR
        if (SAMERCCELL(prcA,prc3)) {
            gotlnk = true;
            if ( !flgA ) {
                flgA = flg3;
                prcA->set.flag[lval] |= flg3;
                prcA->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"A3 %s ", Get_RC_setval_color_Stg(prcA,setval));
                if ( !flgB ) {
                    flgB = flg4;
                    prcB->set.flag[lval] |= flg4;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"a3 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
            }
        } else if (SAMERCCELL(prcA,prc4)) {
            gotlnk = true;
            if ( !flgA ) {
                flgA = flg4;
                prcA->set.flag[lval] |= flg4;
                prcA->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"A4 %s ", Get_RC_setval_color_Stg(prcA,setval));
                if ( !flgB ) {
                    flgB = flg3;
                    prcB->set.flag[lval] |= flg3;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"a4 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
            }
        }
        if (SAMERCCELL(prcB,prc3)) {
            gotlnk = true;
            if ( !flgB ) {
                flgB = flg3;
                prcB->set.flag[lval] |= flg3;
                prcB->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"B3 %s ", Get_RC_setval_color_Stg(prcB,setval));
                if ( !flgA ) {
                    flgA = flg4;
                    prcA->set.flag[lval] |= flg4;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"b3 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
            }
        } else if (SAMERCCELL(prcB,prc4)) {
            gotlnk = true;
            if ( !flgB ) {
                flgB = flg4;
                prcB->set.flag[lval] |= flg4;
                prcB->set.tm = ch_num;
                count++;
                sprintf(EndBuf(tb),"B4 %s ", Get_RC_setval_color_Stg(prcB,setval));
                if ( !flgA ) {
                    flgA = flg3;
                    prcA->set.flag[lval] |= flg3;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"b4 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
            }
        }
        if (!gotlnk && g_bChkWLAlso) {
            // This SL pair has NO direct LINK with the given 2 pairs,
            // BUT may share a ROW, COL or BOX with one of the 4 cells
            // If YES, then it can also be COLORED accordingly
            int shrrcb = 0;
            if ((rowA == row1)||(colA == col1)||(boxA == box1)) {
                shrrcb = 'A'+1;
                if ( !flgA ) {
                    flgA = flg2;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg1;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcA;
                rcpair.rowcol[1] = *prc1;
            } else if ((rowA == row2)||(colA == col2)||(boxA == box2)) {
                shrrcb = 'A'+2;
                if ( !flgA ) {
                    flgA = flg1;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg2;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcA;
                rcpair.rowcol[1] = *prc2;
            } else if ((rowA == row3)||(colA == col3)||(boxA == box3)) {
                shrrcb = 'A'+3;
                if ( !flgA ) {
                    flgA = flg4;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg3;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcA;
                rcpair.rowcol[1] = *prc3;
            } else if ((rowA == row4)||(colA == col4)||(boxA == box4)) {
                shrrcb = 'A'+4;
                if ( !flgA ) {
                    flgA = flg3;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg4;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcA;
                rcpair.rowcol[1] = *prc4;
            } else if ((rowB == row1)||(colB == col1)||(boxB == box1)) {
                shrrcb = 'B'+1;
                if ( !flgA ) {
                    flgA = flg1;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg2;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcB;
                rcpair.rowcol[1] = *prc1;
            } else if ((rowB == row2)||(colB == col2)||(boxB == box2)) {
                shrrcb = 'B'+2;
                if ( !flgA ) {
                    flgA = flg2;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg1;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcB;
                rcpair.rowcol[1] = *prc2;
            } else if ((rowB == row3)||(colB == col3)||(boxB == box3)) {
                shrrcb = 'B'+3;
                if ( !flgA ) {
                    flgA = flg3;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg4;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcB;
                rcpair.rowcol[1] = *prc3;
            } else if ((rowB == row4)||(colB == col4)||(boxB == box4)) {
                shrrcb = 'B'+4;
                if ( !flgA ) {
                    flgA = flg4;
                    prcA->set.flag[lval] |= flgA;
                    prcA->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBA1 %s ", Get_RC_setval_color_Stg(prcA,setval));
                }
                if ( !flgB ) {
                    flgB = flg3;
                    prcB->set.flag[lval] |= flgB;
                    prcB->set.tm = ch_num;
                    count++;
                    sprintf(EndBuf(tb),"RCBa1 %s ", Get_RC_setval_color_Stg(prcB,setval));
                }
                rcpair.rowcol[0] = *prcB;
                rcpair.rowcol[1] = *prc4;
            }
            if (shrrcb) {
                // remove STRONG LINK flags
                rcpair.rowcol[0].set.flag[lval] &= ~(cl_SLA);
                rcpair.rowcol[1].set.flag[lval] &= ~(cl_SLA);
                // and KEEP this WEAK link
                if (!prcp_in_pvrcp(&rcpair,pp) && !prcp_in_pvrcp(&rcpair,&added_pairs))
                    added_pairs.push_back(rcpair);
            }
        }
    }

    if (count) {
        OUTITSL3(tb);
        //sprintf(tb,"Flag 1=%s 2=%s 3=%s 4=%s",
        //    ((flg1 == (cf_XCA|cf_XCB)) ? "?" : (flg1 & cf_XCA) ? "A" : (flg1 & cf_XCB) ? "B" : "U"),
        //    ((flg2 == (cf_XCA|cf_XCB)) ? "?" : (flg2 & cf_XCA) ? "A" : (flg2 & cf_XCB) ? "B" : "U"),
        //    ((flg3 == (cf_XCA|cf_XCB)) ? "?" : (flg3 & cf_XCA) ? "A" : (flg3 & cf_XCB) ? "B" : "U"),
        //    ((flg4 == (cf_XCA|cf_XCB)) ? "?" : (flg4 & cf_XCA) ? "A" : (flg4 & cf_XCB) ? "B" : "U"));
        //OUTITSL3(tb);
    }
    return count;
}



#define DO_IMM_MARK

// if ((setvalcnt == 1) && count && g_bIBL && !g_bIWL && !g_bASW) {
// Is ONLY a list of STRONG LINKS for ONE setval
// BUT was NOT able to eliminate anything using a CHAIN
// At least try adding COLOR alternatively to the LINKS
int add_sl_flag = 1; // should already HAVE SL flag ADDED!!!
// but somehow, along the way, it has been CLEARED! UGH - so add it NOW!
int Mark_SL_Pairs3(PABOX2 pb, int setval, vRCP *pp)
{
    int count = 0;
    size_t max, ii, i2;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    PRCPAIR prcp2;
    PROWCOL prc3, prc4;
    PRCPAIR prcplnk;
    int box1, box2;
    uint64_t flg1 = cf_XCA;
    uint64_t flg2 = cf_XCB;
    uint64_t flg, flg3, flg4;
    time_t ch_num = 0;
    int lval = setval - 1;
    int lnkcnt, marked, dnmrk, lnkw2;
    uint64_t lflg;
    char *tb = GetNxtBuf();
    max = pp->size();
    //vRCP mkd;
    if (!VALUEVALID(setval)) {
        sprintf(tb,"Invalid setval %d passed!",setval);   
        EXPLODE(tb); // Process_SL_Pairs3() - invalid setval
    }
    if (add_debug_sl2)
        Show_SL_Colored_Pair_Chains( pb, setval, pp );

    added_pairs.clear();  // clear ADDITIONAL links
    // Clear ALL color flags
    // =====================
    sprintf(tb,"Strong Links %d: ", (int)max);
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get PTR to a pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        prc1->set.flag[lval] &= ~(cf_XAB);
        prc2->set.flag[lval] &= ~(cf_XAB);
        prc1->set.tm = 0;
        prc2->set.tm = 0;
        if (add_sl_flag) {
            if (prc1->row == prc2->row)
                lflg = cl_SLR;
            else if (prc1->col == prc2->col)
                lflg = cl_SLC;
            else {
                box1 = GETBOX(prc1->row,prc1->col);
                box2 = GETBOX(prc2->row,prc2->col);
                if (box1 == box2) {
                    lflg = cl_SLB;
                } else {
                    sprintf(tb,"Cells %s & %s, for setval %d do NOT APPEAR LINKED!",
                        Get_RC_setval_color_Stg(prc1,setval),
                        Get_RC_setval_color_Stg(prc2,setval), setval );
                    EXPLODE(tb);
                }
            }
            // MARK with the appropriate STRONG link - should have been DONE before here!!!
            prc1->set.flag[lval] |= lflg;
            prc2->set.flag[lval] |= lflg;
        }
        Append_RCPAIR_Settings(tb,prcp1,setval);
    }
    OUTITSL3(tb);
    // Add COLOR to STRONG links
    flg1 = cf_XCA;
    flg2 = cf_XCB;
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get PTR to a pair
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        if ( !(prc1->set.uval == setval) ) continue;
        if ( !(prc1->set.uval == setval) ) continue;
        ch_num++; // BUMP chain number
        marked = 0;
        // MARK prc1 and prc2
        // ==================
        if ( !( prc1->set.flag[lval] & cf_XAB ) ) {
            prc1->set.flag[lval] |= flg1;
            prc1->set.tm = ch_num;
            marked++;
        }
        if ( !( prc2->set.flag[lval] & cf_XAB ) ) {
            prc2->set.flag[lval] |= flg2;
            prc2->set.tm = ch_num;
            marked++;
        }
        // if (marked) {
            sprintf(tb,"Marking ch#%d bgn %s ", (int)ch_num,
                Get_RCPair_Stg(prcp1,setval));
            OUTITSL3(tb);
        // }
        lnkcnt = 0;
        marked = 0;
        //mkd.clear();
        for (i2 = 0; i2 < max; i2++) {
            if (i2 == ii) continue;
            prcp2 = &pp->at(i2);
            prc3 = &prcp2->rowcol[0];
            prc4 = &prcp2->rowcol[1];
            // if ( ( prc3->set.flag[lval] & cf_XAB ) && ( prc4->set.flag[lval] & cf_XAB ) )
            //    continue;   // already fully colormarked
            // extract FLAGS
            flg3 = prc3->set.flag[lval] & cf_XAB;
            flg4 = prc4->set.flag[lval] & cf_XAB;

            // Mark 3 and 4 if they have a LINK to 1 or 2
            // Check 1 with 3 and 4
            dnmrk = 0;
            lnkw2 = 0;
            sprintf(tb,"Mkd ");
            if (SAMERCCELL(prc1,prc3)) {
                lnkcnt++;
                lnkw2 = 13;
                prcplnk = prcp2;
                if (flg3) {
                    if ( !flg4 ) {
                        flg = (flg3 & cf_XCA) ? cf_XCB : cf_XCA;
                        prc4->set.flag[lval] |= flg;
                        prc4->set.tm = ch_num;
                        sprintf(EndBuf(tb),"13 %s ", Get_RC_setval_color_Stg(prc4,setval));
                        //sprintf(EndBuf(tb),"prc4(%s) ", Get_RC_setval_RC_Stg(prc4,setval));
                        marked++;
                        dnmrk++;
                        //mkd.push_back(*prcp2);
                    }                    
                } else { // if ( !( prc3->set.flag[lval] & cf_XAB ) ) {
                    prc3->set.flag[lval] |= flg1;
                    prc3->set.tm = ch_num;
                    //sprintf(EndBuf(tb),"prc3(%s) ", Get_RC_setval_RC_Stg(prc3,setval));
                    sprintf(EndBuf(tb),"1!3 %s ", Get_RC_setval_color_Stg(prc3,setval));
                    marked++;
                    dnmrk++;
                    //mkd.push_back(*prcp2);
                    if ( !flg4 ) { // ( !( prc4->set.flag[lval] & cf_XAB ) ) {
                        prc4->set.flag[lval] |= flg2;
                        prc4->set.tm = ch_num;
                        sprintf(EndBuf(tb),"1!4 %s ", Get_RC_setval_color_Stg(prc4,setval));
                        //sprintf(EndBuf(tb),"prc4(%s) ", Get_RC_setval_RC_Stg(prc4,setval));
                        marked++;
                        dnmrk++;
                    }
                }
#ifdef DO_IMM_MARK
                if (dnmrk) OUTITSL3(tb);
                Mark_All_Pairs_linked_with_this_pair3( pb, setval, pp, prcp1, prcp2, ch_num );
                sprintf(tb,"Mkd ");
#endif // #ifdef DO_IMM_MARK
            } else if (SAMERCCELL(prc1,prc4)) {
                lnkcnt++;
                lnkw2 = 14;
                prcplnk = prcp2;
                dnmrk = 0;
                if (flg4) {
                    if (!flg3) {
                        flg = (flg4 & cf_XCA) ? cf_XCB : cf_XCA;
                        prc3->set.flag[lval] |= flg;
                        prc3->set.tm = ch_num;
                        sprintf(EndBuf(tb),"14 %s ", Get_RC_setval_color_Stg(prc3,setval));
                        marked++;
                        dnmrk++;
                        //mkd.push_back(*prcp2);
                    }
                } else { // if ( !( prc4->set.flag[lval] & cf_XAB ) ) {
                    prc4->set.flag[lval] |= flg1;
                    prc4->set.tm = ch_num;
                    sprintf(EndBuf(tb),"1!4 %s ", Get_RC_setval_color_Stg(prc4,setval));
                    //sprintf(EndBuf(tb),"prc4(%s) ", Get_RC_setval_RC_Stg(prc4,setval));
                    marked++;
                    dnmrk++;
                    //mkd.push_back(*prcp2);
                    if ( !flg3 ) { // ( !( prc3->set.flag[lval] & cf_XAB ) ) {
                        prc3->set.flag[lval] |= flg2;
                        prc3->set.tm = ch_num;
                        //sprintf(EndBuf(tb),"prc3(%s) ", Get_RC_setval_RC_Stg(prc3,setval));
                        sprintf(EndBuf(tb),"1!3 %s ", Get_RC_setval_color_Stg(prc3,setval));
                        marked++;
                        dnmrk++;
                    }
                }
#ifdef DO_IMM_MARK
                if (dnmrk) OUTITSL3(tb);
                Mark_All_Pairs_linked_with_this_pair3( pb, setval, pp, prcp1, prcp2, ch_num );
                sprintf(tb,"Mkd ");
#endif // #ifdef DO_IMM_MARK
            }

            // Check 2 with 3 nd 4
            if (SAMERCCELL(prc2,prc3)) {
                lnkcnt++;
                lnkw2 = 23;
                prcplnk = prcp2;
                dnmrk = 0;
                if (flg3) {
                    if (!flg4) {
                        flg = (flg3 & cf_XCA) ? cf_XCB : cf_XCA;
                        prc4->set.flag[lval] |= flg;
                        prc4->set.tm = ch_num;
                        sprintf(EndBuf(tb),"23 %s ", Get_RC_setval_color_Stg(prc3,setval));
                        marked++;
                        dnmrk++;
                        //mkd.push_back(*prcp2);
                    }
                } else { // if ( !( prc3->set.flag[lval] & cf_XAB ) ) {
                    prc3->set.flag[lval] |= flg2;
                    prc3->set.tm = ch_num;
                    //sprintf(EndBuf(tb),"prc3(%s) ", Get_RC_setval_RC_Stg(prc3,setval));
                    sprintf(EndBuf(tb),"2!3 %s ", Get_RC_setval_color_Stg(prc3,setval));
                    marked++;
                    //mkd.push_back(*prcp2);
                    dnmrk++;
                    if ( !( prc4->set.flag[lval] & cf_XAB ) ) {
                        prc4->set.flag[lval] |= flg1;
                        prc4->set.tm = ch_num;
                        //sprintf(EndBuf(tb),"prc4(%s) ", Get_RC_setval_RC_Stg(prc4,setval));
                        sprintf(EndBuf(tb),"2!4 %s ", Get_RC_setval_color_Stg(prc4,setval));
                        marked++;
                        dnmrk++;
                    }
                }
#ifdef DO_IMM_MARK
                if (dnmrk) OUTITSL3(tb);
                Mark_All_Pairs_linked_with_this_pair3( pb, setval, pp, prcp1, prcp2, ch_num );
                sprintf(tb,"Mkd ");
#endif // #ifdef DO_IMM_MARK
            } else if (SAMERCCELL(prc2,prc4)) {
                lnkcnt++;
                lnkw2 = 24;
                prcplnk = prcp2;
                dnmrk = 0;
                if (flg4 ) {
                    if ( !flg3 ) {
                        flg = (flg4 & cf_XCA) ? cf_XCB : cf_XCA;
                        prc3->set.flag[lval] |= flg;
                        prc3->set.tm = ch_num;
                        sprintf(EndBuf(tb),"24 %s ", Get_RC_setval_color_Stg(prc3,setval));
                        marked++;
                        dnmrk++;
                        //mkd.push_back(*prcp2);
                    }
                } else { //  if ( !( prc4->set.flag[lval] & cf_XAB ) ) {
                    prc4->set.flag[lval] |= flg2;
                    prc4->set.tm = ch_num;
                    //sprintf(EndBuf(tb),"prc4(%s) ", Get_RC_setval_RC_Stg(prc4,setval));
                    sprintf(EndBuf(tb),"2!4 %s ", Get_RC_setval_color_Stg(prc4,setval));
                    marked++;
                    dnmrk++;
                    //mkd.push_back(*prcp2);
                    if ( !( prc3->set.flag[lval] & cf_XAB ) ) {
                        prc3->set.flag[lval] |= flg1;
                        prc3->set.tm = ch_num;
                        //sprintf(EndBuf(tb),"prc3(%s) ", Get_RC_setval_RC_Stg(prc3,setval));
                        sprintf(EndBuf(tb),"2!3 %s ", Get_RC_setval_color_Stg(prc3,setval));
                        marked++;
                        dnmrk++;
                        // mkd.push_back(*prcp2);
                    }
                }
#ifdef DO_IMM_MARK
                if (dnmrk) OUTITSL3(tb);
                Mark_All_Pairs_linked_with_this_pair3( pb, setval, pp, prcp1, prcp2, ch_num );
#endif // #ifdef DO_IMM_MARK
            }
#ifdef DO_IMM_MARK  // 20120925 - try this... cand of
            if (!dnmrk && lnkw2) // maybe the LINKS are ALL marked, but there ARE LINKS
                Mark_All_Pairs_linked_with_this_pair3( pb, setval, pp, prcp1, prcp2, ch_num );
#endif // #ifdef DO_IMM_MARK
        }
        //if (marked) {
        //    sprintf(tb,"Markers ");
        //    Append_RCPairs_per_pvrcp(tb,setval,&mkd);
        //    OUTITSL3(tb);
        //}
        if (lnkcnt) {
#ifndef DO_IMM_MARK
            sprintf(tb,"Processing %s==%s", Get_RCPair_Stg(prcp1,setval),Get_RCPair_Stg(prcplnk,setval));
            OUTITSL3(tb);
            Mark_All_Pairs_linked_with_this_pair3( pb, setval, pp, prcp1, prcplnk, ch_num );
#endif // #ifdef DO_IMM_MARK
        } else {
            sprintf(tb,"Processed %s single pair", Get_RCPair_Stg(prcp1,setval));
            OUTITSL3(tb);
        }
    }
    if (add_debug_sl2)
        Show_SL_Colored_Pair_Chains( pb, setval, pp );
    count = Process_SL_Colored_Pairs_for_Elims3(pb, setval, pp);
    max = added_pairs.size();
    if (count && max) {
        for (ii = 0; ii < max; ii++)
            pp->push_back(added_pairs[ii]);
    }

    return count;
}

int Do_Clear_Links3(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, sl3_elim, sl3_mark, "SL3", "S-Links3", 0, false );
    return count;
}

int Do_Fill_Links3(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, sl3_elim, sl3_mark, "SL3", "S-Links3", 0, true );
    if (count)
        pb->iStage = sg_Begin;
    return count;
}

int Process_SL_Pairs3(PABOX2 pb, int setval, vRCP *pp)
{
    int count = Mark_SL_Pairs3( pb, setval, pp );
    if (count)
        Stack_Fill(Do_Clear_Links3);
    return count;
}
static char *stgstg = "sl3";
int add_sl_boxes3 = 1;
int add_weak_links3 = 0;
int Get_Strong_Links_for_setval3( PABOX2 pb, int setval, vRCP *psl_pairs )
{
    int count = 0;
    int row, col, val, box;
    int row2, col2, fcol, box2;
    int lval = setval - 1;
    PSET ps, ps2;
    int setcnt;
    uint64_t flag;
    RCPAIR rcpair;
    ROWCOL rc1, rc2;
    char *tb = GetNxtBuf();
    sprintf(tb,"Get Links: Inc.Block %s, Inc.Weak %s, Alt.WS %s",
        g_bIBL ? "On" : "Off",
        g_bIWL ? "On" : "Off",
        g_bASW ? "On" : "Off" );
    OUTITSL3(tb);
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            if ( !(ps->val[lval]) ) continue; // skip - does NOT have setval
            // found cell with value - seek second
            fcol = col + 1;
            for (row2 = row; row2 < 9; row2++) {
                for (col2 = fcol; col2 < 9; col2++) {
                    val = pb->line[row2].val[col2];
                    if (val) continue;
                    ps2 = &pb->line[row2].set[col2];
                    if ( !(ps2->val[lval]) ) continue; // skip - does NOT have setval
                    if ((row2 == row)&&(col2 == col)) continue; // skip SELF
                    flag = 0; // KILL the SL flag
                    box = GETBOX(row,col);
                    box2 = GETBOX(row2,col2);
                    if (row2 == row) {  // SAME ROW
                        setcnt = Count_setval_in_Row(pb, row, col, setval );
                        if (setcnt == 2) {
                            flag |= cl_SLR;
                        }
                    } else if (col2 == col) {   // SAME COLUMN
                        setcnt = Count_setval_in_Col(pb, row, col, setval );
                        if (setcnt == 2) {
                            flag |= cl_SLC;
                        }
                    } else { // NOT same ROW or COL
                        if (add_sl_boxes3) {
                            if (box2 == box) {
                                setcnt = Count_setval_in_Box(pb, row, col, setval );
                                if (setcnt == 2) flag |= cl_SLB;
                            } else continue; // or SAME BOX
                        } else continue; // seeking ONLY same COL or ROW
                    }

                    // found 2nd with value
                    if (setcnt != 2) {
                        if (!add_weak_links3) {
                            continue; // seek ONLY strong LINKS
                        }
                        flag = 0;   // clear the STRONG LINK flag
                    }

                    // got a STRONG (or WEAK) LINKED pair
                    rc1.row = row;
                    rc1.col = col;
                    COPY_SET(&rc1.set,ps);
                    rc2.row = row2;
                    rc2.col = col2;
                    COPY_SET(&rc2.set,ps2);
                    rc1.set.flag[lval] |= flag;
                    rc2.set.flag[lval] |= flag;
                    rc1.set.uval = setval;
                    rc2.set.uval = setval;
                    // STORE the PAIR, WITH SL flag
                    rcpair.rowcol[0] = rc1;
                    rcpair.rowcol[1] = rc2;
                    // STORE in vRCP vector
                    psl_pairs->push_back(rcpair);

                    sprintf(tb,"Link %s %s %s ",
                        Get_RC_setval_RC_Stg(&rc1,setval),
                        ((flag & cl_SLA) ? stglnk : weklnk),
                        Get_RC_setval_RC_Stg(&rc2,setval) );
                     OUTITSL3(tb);
                }   // for (col2 = fcol; col2 < 9; col2++)
                fcol = 0;
            }   // for (row2 = row; row2 < 9; row2++) 
        }   // for (col = 0; col < 9; col++)
    }   // for (row = 0; row < 9; row++)
    count = (int)psl_pairs->size();
    //if (count) {
    //    sl_pairs_valid = true;
    //}
    return count;
}


int Do_Strong_Link3_Scan( PABOX2 pb )
{
    int count = 0;
    int i, cnt;
    vRCP vrcp;
    vRCP *psl_pairs = Get_SL_Pairs();
    size_t max, ii;
    char *tb = GetNxtBuf();
    Set_SLP_setval(0);
    Set_SLP_Valid(false);
    for (i = 0; i < 9; i++) {
        // Do_Clear_Links3(pb);
        vrcp.clear();
        cnt = Get_Strong_Links_for_setval3( pb, i + 1, &vrcp );
        max = 0;
        sprintf(tb,"Got Strong Links for setval %d - count %d", i + 1, cnt);
        OUTITSL3(tb);
        if (cnt) {
            count += Mark_SL_Pairs3( pb, i + 1, &vrcp );
            max = vrcp.size();
        }
        if (count && only_one_sl3) {
            // Must transfer pairs to 'display' vector
            psl_pairs->clear();
            for (ii = 0; ii < max; ii++)
                psl_pairs->push_back(vrcp[ii]);
            if (max) {
                Set_SLP_setval( i + 1 );
                Set_SLP_Valid(true);
            }
            break;
        }
    }
    if (!count) {
        Set_SLP_setval(0);
        Set_SLP_Valid(false);
    }
    if (count) {
        sprtf("%s: SL-Pairs3 %d - to fill\n", stgstg, count);
        pb->iStage = sg_Fill_SLPairs3;
    } else {
        sprtf("%s: SL-Pairs3 - to bgn\n", stgstg, count);
        pb->iStage = sg_Begin;
    }

    return count;
}


// eof - Sudo_SLink3.cxx
