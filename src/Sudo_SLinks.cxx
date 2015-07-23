// Sudo_SLinks.cxx

#include "Sudoku.hxx"

// #define FLIP_COLOR(a) ( a & cf_XCA ) ? cf_XCB : cf_XCA

#define sl_elim cf_SLE
#define sl_mark cf_SL

#define OUTITSL(tb) if (*tb && add_debug_sl) OUTIT(tb)

// Clear ALL color flags and ADD Strong Link Flag
// Also CLEAR linked-samecell flag - cl_LSS
// ==============================================
static int add_sl_flag = 1;
void Flag_SL_Pairs_and_Clear_Color( PABOX2 pb, int setval, vRCP *pp )
{
    size_t max = pp->size();
    size_t ii;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    int lval = setval - 1;
    uint64_t lflg;
    int box1, box2;
    char *tb = GetNxtBuf();

    sprintf(tb,"Strong Links %d: ", (int)max);
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get PTR to a pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        prc1->set.flag[lval] &= ~(cf_XAB | cl_LSS);
        prc2->set.flag[lval] &= ~(cf_XAB | cl_LSS);
        prc1->set.tm = 0; // clear chain number
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
                        Get_RC_setval_color_Stg(prc2,setval) );
                    EXPLODE(tb);
                }
            }
            // MARK with the appropriate STRONG link - should have been DONE before here!!!
            prc1->set.flag[lval] |= lflg;
            prc2->set.flag[lval] |= lflg;
        }
        Append_RCPAIR_Settings(tb,prcp1,setval);
    }
    //OUTITSL2(tb);
}


int Color_SL_Pairs( PABOX2 pb, int setval, vRCP *pp )
{
    int count = 0;
    size_t max = pp->size();
    size_t ii;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    size_t jj;
    PRCPAIR prcp2;
    PROWCOL prc3, prc4;
    int lval = setval - 1;
    uint64_t flg1, flg2, flg3, flg4;
    int lnked, lnkcnt;
    time_t ch_num = 1;
    char *tb = GetNxtBuf();

    Flag_SL_Pairs_and_Clear_Color( pb, setval, pp );
    sprintf(tb,"For setval %d, coloring %d pairs... ", setval, (int)max);
    OUTITSL(tb);
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get PTR to a pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        flg1 = prc1->set.flag[lval] & cf_XAB;
        flg2 = prc2->set.flag[lval] & cf_XAB;
        lnkcnt = 0;
        for (jj = ii + 1; jj < max; jj++) {
            prcp2 = &pp->at(jj); // get PTR to a pair
            prc3 = &prcp2->rowcol[0];
            if ( !(prc3->set.uval == setval) ) continue;
            prc4 = &prcp2->rowcol[1];
            flg3 = prc3->set.flag[lval] & cf_XAB;
            flg4 = prc4->set.flag[lval] & cf_XAB;
            lnked = 0;
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
                sprintf(tb,"Link %d %s==%s chn %d ",lnked, Get_RCPAIR_Stg(prcp1,setval), Get_RCPAIR_Stg(prcp2,setval),
                    (int) ch_num);
                OUTITSL(tb);
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
                sprintf(tb,"Link %d %s==%s chn %d ",lnked, Get_RCPAIR_Stg(prcp1,setval), Get_RCPAIR_Stg(prcp2,setval),
                    (int) ch_num);
                OUTITSL(tb);
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
                sprintf(tb,"Link %d %s==%s chn %d ",lnked, Get_RCPAIR_Stg(prcp1,setval), Get_RCPAIR_Stg(prcp2,setval),
                    (int) ch_num);
                OUTITSL(tb);
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
                sprintf(tb,"Link %d %s==%s chn %d ",lnked, Get_RCPAIR_Stg(prcp1,setval), Get_RCPAIR_Stg(prcp2,setval),
                    (int) ch_num);
                OUTITSL(tb);
            }
            if (lnked) {
                if (prc1->set.tm) {
                    if (!prc2->set.tm)
                        prc2->set.tm = prc1->set.tm;
                    if (!prc3->set.tm)
                        prc3->set.tm = prc1->set.tm;
                    if (!prc4->set.tm)
                        prc4->set.tm = prc1->set.tm;
                } else if (prc2->set.tm) {
                    if (!prc1->set.tm)
                        prc1->set.tm = prc2->set.tm;
                    if (!prc3->set.tm)
                        prc3->set.tm = prc2->set.tm;
                    if (!prc4->set.tm)
                        prc4->set.tm = prc2->set.tm;
                } else {
                    prc1->set.tm = ch_num;
                    prc2->set.tm = ch_num;
                    prc3->set.tm = ch_num;
                    prc4->set.tm = ch_num;
                }
                lnkcnt++;
            }
        }   // for (jj = ii + 1; jj < max; jj++)
        // tried pair with others
        if (!lnkcnt) {
            // unmarked single pair
            if (flg1 && flg2) {
                // already marked
            } else if (flg1) {
                flg2 = FLIP_COLOR(flg1);
                prc2->set.flag[lval] |= flg2;
            } else if (flg2) {
                flg1 = FLIP_COLOR(flg2);
                prc1->set.flag[lval] |= flg1;
            } else {
                // no color yet
                flg1 = cf_XCA;
                flg2 = cf_XCB;
                prc1->set.flag[lval] |= flg1;
                prc2->set.flag[lval] |= flg2;
                //prc1->set.tm = ch_num;
                //prc2->set.tm = ch_num;
                //ch_num++;
            }
            prc1->set.tm = ch_num;
            prc2->set.tm = ch_num;
            ch_num++;
            sprintf(tb,"No Link %s, bump ch_num %d ", Get_RCPAIR_Stg(prcp1,setval), (int)ch_num);
            OUTITSL(tb);
        }
        count += lnkcnt;
    }   // for (ii = 0; ii < max; ii++) 

    return count;
}

size_t Get_Empty_Cells_of_setval_minus_pairs( PABOX2 pb, int setval, vRCP *pp, vRC *pempty, PRCRCB prcrcb = 0, bool clear = true );
size_t Get_Empty_Cells_of_setval_minus_pairs( PABOX2 pb, int setval,  vRCP *pp, vRC *pempty, PRCRCB prcrcb, bool clear )
{
    size_t count = 0;
    size_t maxpp = pp->size();
    int row, col, val, box, lval;
    ROWCOL rc;
    PSET ps;
    if (prcrcb && clear) {
        for (row = 0; row < 9; row++) {
            prcrcb->vrows[row].clear();
            prcrcb->vcols[row].clear();
            prcrcb->vboxs[row].clear();
        }
    }
    lval = setval - 1; // FIX20120924 - Changed to using lval
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;  // NOT empty
            ps = &pb->line[row].set[col];
            if ( !( ps->val[lval] ) ) continue; // NOT with this value
            rc.row = row;
            rc.col = col;
            rc.box = GETBOX(row,col);
            if (prc_in_pvrcp(&rc,pp)) continue;
            COPY_SET(&rc.set,ps);
            pempty->push_back(rc);  // add this one
            if (prcrcb) {
                box = GETBOX(row,col);
                prcrcb->vrows[row].push_back(rc);
                prcrcb->vcols[col].push_back(rc);
                prcrcb->vboxs[box].push_back(rc);
            }
            count++;
        }
    }
    return count;
}

bool timet_in_pvtt( time_t ch_num, vTIMET *pvtt )
{
    size_t max = pvtt->size();
    size_t ii;
    time_t tch_num;
    for (ii = 0; ii < max; ii++) {
        tch_num = pvtt->at(ii);
        if (tch_num == ch_num)
            return true;
    }
    return false;
}


size_t Get_Pair_ch_nums( vRCP *pp, vTIMET *pvtt )
{
    time_t ch_num;
    size_t ii, max;
    PRCPAIR prcp;
    PROWCOL prc;
    max = pp->size();
    for (ii = 0; ii < max; ii++) {
        prcp = &pp->at(ii);
        prc = &prcp->rowcol[0];
        ch_num = prc->set.tm;
        if (timet_in_pvtt( ch_num, pvtt )) continue;
        pvtt->push_back(ch_num);
    }
    return pvtt->size();
}

int Get_Pairs_of_setval_ch_num( PABOX2 pb, int setval, time_t ch_num, vRC *pvrc, PRCRCB prcrcb, vRCP *pvrcp,
    bool clear = true, bool debug = false);

int Get_Pairs_of_setval_ch_num( PABOX2 pb, int setval, time_t ch_num, vRC *pvrc, PRCRCB prcrcb, vRCP *pvrcp,
    bool clear, bool debug )
{
    int count = 0;
    size_t ii;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    size_t max = pvrcp->size();
    if (clear) {
        for (ii = 0; ii < 9; ii++) {
            prcrcb->vrows[ii].clear();
            prcrcb->vcols[ii].clear();
            prcrcb->vboxs[ii].clear();
        }
        pvrc->clear();
    }
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pvrcp->at(ii); // get OUT a pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        if ( !(prc1->set.tm == ch_num) ) continue;
        prc2 = &prcp1->rowcol[1];
        if (!prc_in_chain(prc1,pvrc)) {
            pvrc->push_back(*prc1);
            prcrcb->vrows[prc1->row].push_back(*prc1);
            prcrcb->vcols[prc1->col].push_back(*prc1);
            prcrcb->vboxs[GETBOX(prc1->row,prc1->col)].push_back(*prc1);
            count++;
        }
        if (!prc_in_chain(prc2,pvrc)) {
            pvrc->push_back(*prc2);
            prcrcb->vrows[prc2->row].push_back(*prc2);
            prcrcb->vcols[prc2->col].push_back(*prc2);
            prcrcb->vboxs[GETBOX(prc2->row,prc2->col)].push_back(*prc2);
            count++;
        }
    }
    if (debug) {
        char *tb = GetNxtBuf();
        sprintf(tb,"%d pairs: ", (int)max);
        int i, cnt;
        for (i = 0; i < 9; i++) {
            cnt = prcrcb->vrows[i].size();
            if (!cnt) continue;
            sprintf(EndBuf(tb),"R%d=%d ", i + 1, cnt);
        }
        for (i = 0; i < 9; i++) {
            cnt = prcrcb->vcols[i].size();
            if (!cnt) continue;
            sprintf(EndBuf(tb),"C%d=%d ", i + 1, cnt);
        }
        for (i = 0; i < 9; i++) {
            cnt = prcrcb->vboxs[i].size();
            if (!cnt) continue;
            sprintf(EndBuf(tb),"B%d=%d ", i + 1, cnt);
        }
        OUTIT(tb);
    }
    return count;
}


static int only_one_sl = 1;
int Check_SL_Pairs_for_elims( PABOX2 pb, int setval, vRCP *pp )
{
    int count = 0;
    vRC empty, pairs;
    RCRCB rcrcb, rcrcbpairs;
    size_t ii, max, maxt, tt, max2, i2, i22, max3;
    PROWCOL prc, prc1, prc2;
    vTIMET vtt;
    time_t ch_num;
    int cnt, box, ecnt;
    int lval = setval - 1;
    vRC *prow, *pcol, *pbox;
    uint64_t flg1, flg2;
    vRC elims, elimby;
    char *tb = GetNxtBuf();

    // cnt = Get_Pairs_of_setval( pb, setval, &pairs, &rcrcbpairs, pp );
    max = Get_Empty_Cells_of_setval_minus_pairs( pb, setval, pp, &empty, &rcrcb );
    maxt = Get_Pair_ch_nums( pp, &vtt );
    max2 = pp->size();

    sprintf(tb,"For setval %d got %d pairs, in %d chains, and %d other empty cells. ",
        setval, (int)max2, (int)maxt, (int)max);
    if (!max) {
        strcat(tb,"NONE to elim!");
        OUTITSL(tb);
        return 0; // NO empty of this setval
    }
    OUTITSL(tb);

    ecnt = 0;
    for (tt = 0; tt < maxt; tt++) {
        ch_num = vtt[tt];   // get CHAIN number
        cnt = Get_Pairs_of_setval_ch_num( pb, setval, ch_num, &pairs, &rcrcbpairs, pp );
        sprintf(tb,"For setval %d got %d pairs of chain number %d ", setval, (int)cnt, (int)ch_num);
        OUTITSL(tb);
        if (cnt <= 2) continue; // only two cells == a single SL pair - no interest
        // process each EMPTY cell, NOT in the STRONG LINKED pairs
        for (ii = 0; ii < max; ii++) {
            prc = &empty[ii];
            prow = &rcrcbpairs.vrows[prc->row];
            pcol = &rcrcbpairs.vcols[prc->col];
            box = GETBOX(prc->row,prc->col);
            pbox = &rcrcbpairs.vboxs[box];
            // if 2 ends == no cl_LSS, in same ROW, COL, BOX, of OPPOSITE COLOR
            max2 = prow->size();
            if (max2 >= 2) {
                for (i2 = 0; i2 < max2; i2++) {
                    prc1 = &prow->at(i2);
                    if ( prc1->set.flag[lval] & cl_LSS ) continue;
                    for (i22 = i2 + 1; i22 < max2; i22++) {
                        prc2 = &prow->at(i22);
                        if ( prc2->set.flag[lval] & cl_LSS ) continue;
                        // got 2 cells - part of STRONG LINKED pairs
                        flg1 = prc1->set.flag[lval] & cf_XAB;
                        flg2 = prc2->set.flag[lval] & cf_XAB;
                        if (flg1 && flg2 && (flg1 != cf_XAB) && (flg2 != cf_XAB) && (flg1 != flg2)) {
                            // looks like the CHICKEN has to DIE
                            if (!prc_in_chain(prc,&elims)) {
                                elims.push_back(*prc);
                                elimby.push_back(*prc1);
                                elimby.push_back(*prc2);
                                ecnt++;
                                sprintf(tb,"SL-Elim %s by %s & %s ", Get_RC_setval_RC_Stg(prc,setval),
                                    Get_RC_Stg(prc1), Get_RC_Stg(prc2));
                                OUTITSL(tb);
                            }
                        }
                    }
                }
            }
            max2 = pcol->size();
            if (max2 >= 2) {
                for (i2 = 0; i2 < max2; i2++) {
                    prc1 = &pcol->at(i2);
                    if ( prc1->set.flag[lval] & cl_LSS ) continue;
                    for (i22 = i2 + 1; i22 < max2; i22++) {
                        prc2 = &pcol->at(i22);
                        if ( prc2->set.flag[lval] & cl_LSS ) continue;
                        // got 2 cells - part of STRONG LINKED pairs
                        // END cells == not LINKED
                        flg1 = prc1->set.flag[lval] & cf_XAB;
                        flg2 = prc2->set.flag[lval] & cf_XAB;
                        if (flg1 && flg2 && (flg1 != cf_XAB) && (flg2 != cf_XAB) && (flg1 != flg2)) {
                            // looks like the CHICKEN has to DIE
                            if (!prc_in_chain(prc,&elims)) {
                                elims.push_back(*prc);
                                elimby.push_back(*prc1);
                                elimby.push_back(*prc2);
                                ecnt++;
                                sprintf(tb,"SL-Elim %s by %s & %s ", Get_RC_setval_RC_Stg(prc,setval),
                                    Get_RC_Stg(prc1), Get_RC_Stg(prc2));
                                OUTITSL(tb);
                            }
                        }
                    }
                }
            }
            max2 = pbox->size();
            if (max2 >= 2) {
                for (i2 = 0; i2 < max2; i2++) {
                    prc1 = &pbox->at(i2);
                    if ( prc1->set.flag[lval] & cl_LSS ) continue;
                    for (i22 = i2 + 1; i22 < max2; i22++) {
                        prc2 = &pbox->at(i22);
                        if ( prc2->set.flag[lval] & cl_LSS ) continue;
                        // got 2 cells - part of STRONG LINKED pairs
                        // END cells == not LINKED
                        flg1 = prc1->set.flag[lval] & cf_XAB;
                        flg2 = prc2->set.flag[lval] & cf_XAB;
                        if (flg1 && flg2 && (flg1 != cf_XAB) && (flg2 != cf_XAB) && (flg1 != flg2)) {
                            // looks like the CHICKEN has to DIE
                            if (!prc_in_chain(prc,&elims)) {
                                elims.push_back(*prc);
                                elimby.push_back(*prc1);
                                elimby.push_back(*prc2);
                                ecnt++;
                                sprintf(tb,"SL-Elim %s by %s & %s ", Get_RC_setval_RC_Stg(prc,setval),
                                    Get_RC_Stg(prc1), Get_RC_Stg(prc2));
                                OUTITSL(tb);
                            }
                        }
                    }
                }
            }
            // TODO - Only remaining test is is this chicken pincered by ROW/COL by two opposing COLOR ends
            max2 = prow->size();
            max3 = pcol->size();
            for (i2 = 0; i2 < max2; i2++) {
                prc1 = &prow->at(i2);
                if ( prc1->set.flag[lval] & cl_LSS ) continue;
                for (i22 = 0; i22 < max3; i22++) {
                    prc2 = &pcol->at(i22);
                    if ( prc2->set.flag[lval] & cl_LSS ) continue;
                    flg1 = prc1->set.flag[lval] & cf_XAB;
                    flg2 = prc2->set.flag[lval] & cf_XAB;
                    if (flg1 && flg2 && (flg1 != cf_XAB) && (flg2 != cf_XAB) && (flg1 != flg2)) {
                        // looks like the CHICKEN has to DIE
                        if (!prc_in_chain(prc,&elims)) {
                            elims.push_back(*prc);
                            elimby.push_back(*prc1);
                            elimby.push_back(*prc2);
                            ecnt++;
                            sprintf(tb,"SL-Elim %s by %s & %s ", Get_RC_setval_RC_Stg(prc,setval),
                                Get_RC_Stg(prc1), Get_RC_Stg(prc2));
                            OUTITSL(tb);
                        }
                    }
                }
            }

            // NO - Do ALL empty cells for this SL chain
            //if (ecnt && only_one_sl)
            //    break;

        }
        // but only ONE ch_num if got ELIMS
        if (ecnt && only_one_sl)
            break;
    }

    max = elims.size();
    if (max) {
        //we have eliminations
        sprintf(tb,"SL-Elims: %d ", (int) max);
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            pb->line[prc->row].set[prc->col].flag[lval] |= sl_elim;
            sprintf(EndBuf(tb),"%s ",Get_RC_setval_RC_Stg(prc,setval));
        }
        sprintf(EndBuf(tb), "by ");
        max2 = elimby.size();
        for (ii = 0; ii < max2; ii++) {
            prc = &elimby[ii];
            pb->line[prc->row].set[prc->col].flag[lval] |= sl_mark;
            sprintf(EndBuf(tb),"%s ",Get_RC_setval_color_Stg(prc,setval));
        }
        OUTIT(tb);
        count = (int) max;
    }
    return count;
}

int Do_Clear_Links(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, sl_elim, sl_mark, "SL", "S-Links", 0, false );
    return count;
}

int Do_Fill_Links(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, sl_elim, sl_mark, "SL", "S-Links", 0, true );
    if (count)
        pb->iStage = sg_Begin;
    return count;
}


// Just a DISPLAY per ONE setval - Show the LINKS and ELIMS if any
int Mark_Strong_Links_Pairs( PABOX2 pb, int setval, vRCP *pp )
{
    int count = Color_SL_Pairs( pb, setval, pp );
    if (count) {
        int elims = Check_SL_Pairs_for_elims( pb, setval, pp );
        if (elims) {
           Stack_Fill(Do_Clear_Links);
           count = elims;
        }
    }
    return count;
}

int Do_Strong_Links_Pairs( PABOX2 pb, int setval, vRCP *pp )
{
    int count = Color_SL_Pairs( pb, setval, pp );
    if (count) {
        int elims = Check_SL_Pairs_for_elims( pb, setval, pp );
        count = 0;
        if (elims) {
           count = elims;
        }
    }
    return count;
}

int Get_Strong_Links_for_setval( PABOX2 pb, int setval, vRCP *psl_pairs )
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
                        if (setcnt != 2) continue;
                        flag |= cl_SLR;
                    } else if (col2 == col) {   // SAME COLUMN
                        setcnt = Count_setval_in_Col(pb, row, col, setval );
                        if (setcnt != 2) continue;
                        flag |= cl_SLC;
                    } else { // NOT same ROW or COL
                        if (box2 == box) {
                            setcnt = Count_setval_in_Box(pb, row, col, setval );
                            if (setcnt != 2) continue;
                            flag |= cl_SLB;
                        } else
                            continue; // and NOT SAME BOX
                    }
                    // got a STRONG LINKED pair
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
                }   // for (col2 = fcol; col2 < 9; col2++)
                fcol = 0;
            }   // for (row2 = row; row2 < 9; row2++) 
        }   // for (col = 0; col < 9; col++)
    }   // for (row = 0; row < 9; row++)
    count = (int)psl_pairs->size();
    return count;
}

static char *stgstg = (char *)"sls";

int Do_Strong_Links_Scan( PABOX2 pb )
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
        vrcp.clear(); // clear vector, and add STRONG LINK pairs
        cnt = Get_Strong_Links_for_setval( pb, i + 1, &vrcp );
        if (cnt) {  // we got some for this setval
            cnt = Do_Strong_Links_Pairs( pb, i + 1, &vrcp );
            max = vrcp.size();
            sprintf(tb,"Got Strong Links for setval %d - count %d", i + 1, cnt);
            //OUTITSL3(tb);
            if (cnt && only_one_sl) {
                // Must transfer pairs to 'display' vector
                psl_pairs->clear();
                max = vrcp.size();
                // =================================
                for (ii = 0; ii < max; ii++)
                    psl_pairs->push_back(vrcp[ii]);
                // =================================
                if (max) {
                    Set_SLP_setval( i + 1 );
                    Set_SLP_Valid(true);
                    count = (int)max;
                }
                break;
            }
        }
    }
    if (!count) {
        Set_SLP_setval(0);
        Set_SLP_Valid(false);
    }
    if (count) {
        sprtf("%s: SL-Pairs %d - to fill\n", stgstg, count);
        pb->iStage = sg_Fill_SLPairs; // = int Do_Fill_Links(PABOX2 pb);
    } else {
        sprtf("%s: SL-Pairs - to bgn\n", stgstg, count);
        pb->iStage = sg_Begin;
    }

    return count;
}


// Sudo_SLinks.cxx
