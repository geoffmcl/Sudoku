// Sudo_XCycle3.cxx

#include "Sudoku.hxx"

// *************************************************************
// ANOTHER TRY AT X-Cyles SCAN
// FIXED RULES
// For EACH candidate (setval) get the CHAINS where there is a STRONG link
// followed by a WEAK link - Start each CHAIN with a STRONG link with COLOR A
// then a WEAK link marked B, then a STRONG link marked A, and so on...
#define OUTITXC3(tb) if (add_debug_xc3) OUTIT(tb)

int xcycle_test_setval = 1;
int only_one_xc3 = 1;
bool bIncSLB = false;
#define MMX_LINKS 100
typedef struct tagRCSET {
    ROWCOL rowcol[MMX_LINKS + 6];
}RCSET, *PRCSET;

static RCSET rcset;
static RCPAIR _s_rcpair;
int xc3_chains = 0;
int xc3_setval = 0;
int Get_XC3_Chains() { return xc3_chains; }
int Get_XC3_setval() { return xc3_setval; }
PRCPAIR Get_XC3_RCSet(int num)
{
    if ((num+1) >= xc3_chains)
        return 0;
    int off = num / 3;
    int cnt = num % 3;
    PROWCOL prc1 = &rcset.rowcol[num];
    PROWCOL prc2 = &rcset.rowcol[num+1];
    PRCPAIR prcp = &_s_rcpair;
    return prcp;
}

static vRCP vrcpsl, vrcpwl, joined;
static int paint_links_setval = 0;
int Get_Pnt_Lnks_setval() { return paint_links_setval; }
vRCP *Get_Pnt_SL() { return &vrcpsl; }
vRCP *Get_Pnt_WL() { return &joined; }

bool Same_RCPAIR(PRCPAIR p1, PRCPAIR p2)
{
    PROWCOL r1 = &p1->rowcol[0];
    PROWCOL r2 = &p1->rowcol[1];
    PROWCOL r3 = &p2->rowcol[0];
    PROWCOL r4 = &p2->rowcol[1];
    if (SAMERCCELL(r1,r3) && SAMERCCELL(r2,r4))
        return true;
    if (SAMERCCELL(r2,r3) && SAMERCCELL(r1,r4))
         return true;
    return false;
}

bool Pair_Joined_to_Strong_Link(PABOX2 pb, PRCPAIR prcp1, PRCPAIR prcp2, vRCP *pvrcpsl, PRCPAIR *pprcp)
{
    size_t max1 = pvrcpsl->size();
    size_t ii;
    PRCPAIR prcp;
    PROWCOL prc1, prc2, prc3, prc4;
    prc1 = &prcp2->rowcol[0]; // 1
    prc2 = &prcp2->rowcol[1]; // 2
    for (ii = 0; ii < max1; ii++) {
        prcp = &pvrcpsl->at(ii);
        if (Same_RCPAIR(prcp,prcp1)) continue;
        prc3 = &prcp->rowcol[0];
        prc4 = &prcp->rowcol[1];
        if (SAMERCCELL(prc3,prc1)) {
            *pprcp = prcp;
            return true;
        }
        if (SAMERCCELL(prc3,prc2)) {
            *pprcp = prcp;
            return true;
        }
        if (SAMERCCELL(prc4,prc1)) {
            *pprcp = prcp;
            return true;
        }
        if (SAMERCCELL(prc4,prc2)) {
            *pprcp = prcp;
            return true;
        }
    }
    return false;
}

bool Joined_to_Strong_Link(PABOX2 pb, PRCPAIR prcp1, PROWCOL prc, vRCP *pvrcpsl,
    PRCPAIR *pprcp, PROWCOL *pprc)
{
    size_t max1 = pvrcpsl->size();
    size_t ii;
    PRCPAIR prcp;
    PROWCOL prc3, prc4;
    for (ii = 0; ii < max1; ii++) {
        prcp = &pvrcpsl->at(ii);
        if (Same_RCPAIR(prcp,prcp1)) continue;
        prc3 = &prcp->rowcol[0];
        prc4 = &prcp->rowcol[1];
        if (SAMERCCELL(prc3,prc)) {
            *pprcp = prcp;
            *pprc = prc3;
            return true;
        }
        if (SAMERCCELL(prc4,prc)) {
            *pprcp = prcp;
            *pprc = prc4;
            return true;
        }
    }
    return false;
}

// Have ALL SL and WL for this setval, in resp. vectors - once only for each link
// Put them into CHAINS SL-WL-SL adding COLOR
int Sort_SW_Linked_Pairs3( PABOX2 pb, int setval, vRCP *pvrcpsl, vRCP *pvrcpwl )
{
    int count = 0;
    size_t max1 = pvrcpsl->size();
    size_t max2 = pvrcpwl->size();
    size_t i1, i2;
    PRCPAIR prcp1, prcp2;
    PROWCOL prc1,prc2,prc3,prc4;
    int share;
    RCPAIR rcpair1, rcpair2;
    PRCPAIR psl;
    PROWCOL psll;
    vRCP chain;
    int i, chains;
    time_t ch_num, chn;
    char *tb = GetNxtBuf();
    PSET ps1, ps2, ps3;
    int lval = setval - 1;
    uint64_t flg1, flg2;
    PROWCOL prc, prct1, prct2, prct3;;

    sprintf(tb,"Have %d SL and %d WL to sort.", (int)max1, (int)max2);
    OUTITXC3(tb);

    memset(&rcset,0,sizeof(RCSET));
    chains = 0;
    ch_num = 0;
    for (i1 = 0; i1 < max1; i1++) {
        prcp1 = &pvrcpsl->at(i1);
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        // got a STRONG linked PAIR, seek a WEAK linked paired that JOINS these
        sprintf(tb,"SL %s joins ", Get_RCPAIR_Stg(prcp1,setval));
        for (i2 = 0; i2 < max2; i2++) {
            prcp2 = &pvrcpwl->at(i2);
            prc3 = &prcp2->rowcol[0];
            prc4 = &prcp2->rowcol[1];
            // NOW do they share an END
            sprintf(tb,"SL %s joins ", Get_RCPAIR_Stg(prcp1,setval));
            share = 0;
            if (SAMERCCELL(prc1,prc3)) {
                rcpair1.rowcol[0] = *prc2;
                rcpair1.rowcol[1] = *prc1;
                rcpair2.rowcol[0] = *prc3;
                rcpair2.rowcol[1] = *prc4;
                share = 1;
            } else if (SAMERCCELL(prc1,prc4)) {
                rcpair1.rowcol[0] = *prc2;
                rcpair1.rowcol[1] = *prc1;
                rcpair2.rowcol[0] = *prc4;
                rcpair2.rowcol[1] = *prc3;
                share = 2;
            } else if (SAMERCCELL(prc2,prc3)) {
                rcpair1.rowcol[0] = *prc1;
                rcpair1.rowcol[1] = *prc2;
                rcpair2.rowcol[0] = *prc3;
                rcpair2.rowcol[1] = *prc4;
                share = 3;
            } else if (SAMERCCELL(prc2,prc4)) {
                rcpair1.rowcol[0] = *prc1;
                rcpair1.rowcol[1] = *prc2;
                rcpair2.rowcol[0] = *prc4;
                rcpair2.rowcol[1] = *prc3;
                share = 4;
            }
            if (!share) continue; // no shared end

            prct1 = &rcpair1.rowcol[0]; // bgn == strong to
            prct2 = &rcpair1.rowcol[1]; // mid -- weak to 
            prct3 = &rcpair2.rowcol[1]; // end
            // BUT only accept this WEAK link now IFF it ALSO joins with some OTHER SL
            if (!Joined_to_Strong_Link(pb, prcp1, prct3, pvrcpsl, &psl, &psll)) continue;

            sprintf(EndBuf(tb),"%s & %s", Get_RCPAIR_Stg(prcp2,setval),
                Get_RCPAIR_Stg(psl,setval));
            OUTITXC3(tb);

            // have a STONG/WEAK/STRONG chain forming...
            // store these two PAIR, now in correct chaining order
            chain.push_back(rcpair1);
            chain.push_back(rcpair2);
            // But maybe build the chains in the RCSET - keep in order
            ps1 = &prct1->set;
            ps2 = &prct2->set;
            ps3 = &prct3->set;
            if (chains) {
                // have to find the location for this addition
                // if this pair of pairs ae linked with any already in the RCSET,
                // then they take the same ch_num as existing, and get COLOR from that joining
                for (i = 0; i < chains; i++) {
                    prc = &rcset.rowcol[i];
                    if (SAMERCCELL(prc,prct1)) {
                        flg1 = prc->set.flag[lval] & cf_XAB;
                        flg2 = (flg1 & cf_XCA) ? cf_XCB : cf_XCA;
                        chn = prc->set.tm;
                    } else if (SAMERCCELL(prc,prct2)) {
                        flg1 = prc->set.flag[lval] & cf_XAB;
                        flg2 = (flg1 & cf_XCA) ? cf_XCB : cf_XCA;
                        chn = prc->set.tm;
                    } else if (SAMERCCELL(prc,prct3)) {
                        flg1 = prc->set.flag[lval] & cf_XAB;
                        flg2 = (flg1 & cf_XCA) ? cf_XCB : cf_XCA;
                        chn = prc->set.tm;
                    } else {
                        // is a NEW CHAIN
                        ch_num++;
                        chn = ch_num;
                        flg1 = cf_XCA;
                        flg2 = cf_XCB;
                    }
                }
            } else {
                // this is the FIRST
                ch_num++;
                chn = ch_num;
                flg1 = cf_XCA;
                flg2 = cf_XCB;
            }
            ps1->tm = ch_num;
            ps2->tm = ch_num;
            ps3->tm = ch_num;
            ps1->flag[lval] |= cf_XCA; // ON
            ps2->flag[lval] |= cf_XCB; // OFF
            ps3->flag[lval] |= cf_XCA; // ON

            rcset.rowcol[chains++] = *prct1; // begin
            rcset.rowcol[chains++] = *prct2; // middle
            rcset.rowcol[chains++] = *prct3; // end
            if (chains >= MMX_LINKS)
                break;
        }
        //OUTITXC3(tb);
        if (chains >= MMX_LINKS)
           break;
    }
    sprintf(tb,"Now got %d S-W chains...", (int)(chain.size() / 2));
    OUTITXC3(tb);
    xc3_setval = setval;
    xc3_chains = chains;
    if (add_debug_xc3) {
        for (i = 0; i < chains / 3; i++) {
            prct1 = &rcset.rowcol[(i+3)+0];
            prct2 = &rcset.rowcol[(i+3)+1];
            prct3 = &rcset.rowcol[(i+3)+2];
            sprtf("Chn %d: %s %s %s\n", (int)prct1->set.tm,
                Get_RC_setval_color_Stg(prct1,setval),
                Get_RC_setval_color_Stg(prct2,setval),
                Get_RC_setval_color_Stg(prct3,setval));
        }
    }
    if (chains)
        count = 1;
    return count;
}

#define GOTCOLOR(p) (p->set.flag[lval] & cf_XAB)

void Mark_Colors_on_SW_Links3( PABOX2 pb, int setval, vRCP * pvrcpsl, vRCP *pvrcpwl )
{
    int lval = setval - 1;
    size_t max1 = pvrcpsl->size();
    size_t max2 = pvrcpwl->size();
    PRCPAIR prcp1, prcp2;
    PROWCOL prc, prc1, prc2, prc3, prc4;
    PROWCOL prclnk1, prclnk2, prcopn1, prcopn2;
    uint64_t flg1 = cf_XCA;
    uint64_t flg2 = cf_XCB;
    size_t i1, i2;
    size_t max, ii;
    vRC vdone;
    int lnked;
    vRCP vrcp;
    uint64_t flg;
    bool dn1, dn2, dn3, dn4;
    char *tb = GetNxtBuf();

    for (i1 = 0; i1 < max1; i1++) {
        prcp1 = &pvrcpsl->at(i1);
        //if (prcp_in_vrcp(prcp1,vrcp)) continue;
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        prc1->set.flag[lval] &= ~(cf_XAB);
        prc2->set.flag[lval] &= ~(cf_XAB);
    }
    for (i1 = 0; i1 < max1; i1++) {
        prcp1 = &pvrcpsl->at(i1);
        //if (prcp_in_vrcp(prcp1,vrcp)) continue;
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        lnked = 0;
        vrcp.push_back(*prcp1); // Done this one
        for (i2 = 0; i2 < max1; i2++) {
            prcp2 = &pvrcpsl->at(i2);
            if (i2 == i1) continue;
            //if (prcp_in_vrcp(prcp2,vrcp)) continue;
            prc3 = &prcp1->rowcol[0];
            prc4 = &prcp1->rowcol[1];
            max = vdone.size();
            dn1 = dn2 = dn3 = dn4 = false;
            for (ii = 0; ii < max; ii++) {
                prc = &vdone[ii];
                if (!dn1 && SAMERCCELL(prc,prc1)) {
                    dn1 = true;
                    dn2 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    if ( !(GOTCOLOR(prc1)) )
                        prc1->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    if ( !(GOTCOLOR(prc2)) )
                        prc2->set.flag[lval] |= flg;
                }
                if (!dn2 && SAMERCCELL(prc,prc2)) {
                    dn1 = true;
                    dn2 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    if ( !(GOTCOLOR(prc2)) )
                        prc2->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    if ( !(GOTCOLOR(prc1)) )
                        prc1->set.flag[lval] |= flg;
                }
                if (!dn3 && SAMERCCELL(prc,prc3)) {
                    dn3 = true;
                    dn4 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    if ( !(GOTCOLOR(prc3)) )
                        prc3->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    if ( !(GOTCOLOR(prc4)) )
                        prc4->set.flag[lval] |= flg;
                }
                if (SAMERCCELL(prc,prc4)) {
                    dn3 = true;
                    dn4 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    if ( !(GOTCOLOR(prc4)) )
                        prc4->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    if ( !(GOTCOLOR(prc3)) )
                        prc3->set.flag[lval] |= flg;
                }
                if (dn1 && dn2 && dn3 && dn4) break;
            }
            if (dn1 && dn2 && dn3 && dn4) {
                vrcp.push_back(*prcp2); // Done this one
                continue;
            }
            prclnk1 = prclnk2 = prcopn1 = prcopn2 = 0;
            if (SAMERCCELL(prc1,prc3)) {
                prclnk1 = prc1;
                prclnk2 = prc3;
                prcopn1 = prc2;
                prcopn2 = prc4;
                lnked = 13;
            } else if (SAMERCCELL(prc1,prc4)) {
                prclnk1 = prc1;
                prclnk2 = prc4;
                prcopn1 = prc2;
                prcopn2 = prc3;
                lnked = 14;
            } else if (SAMERCCELL(prc2,prc3)) {
                prclnk1 = prc2;
                prclnk2 = prc3;
                prcopn1 = prc1;
                prcopn2 = prc4;
                lnked = 23;
            } else if (SAMERCCELL(prc2,prc4)) {
                prclnk1 = prc2;
                prclnk2 = prc4;
                prcopn1 = prc1;
                prcopn2 = prc3;
                lnked = 24;
            }
            if (lnked)
                break;
        }
        if (lnked) {
            // mark link A
            if (dn1 || dn2 || dn3 || dn4) {
                if (dn1 || dn2) {
                    switch (lnked) {
                    case 13: // 1 & 3 linked, so 3 takes color of 1, and 4 color 2
                    case 24: // 2 & 4 linked, so 4 takes 2, 3 take 1
                        flg = prc1->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc3)) )
                            prc3->set.flag[lval] |= flg;
                        flg = prc2->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc4)) )
                            prc4->set.flag[lval] |= flg;
                        break;
                    case 14: // 1 & 4 linked, so 4 takes 1 and 3 takes 2
                    case 23: // 2 & 3 linked, so 3 takes 2, 4 takes 1
                        flg = prc1->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc4)) )
                            prc4->set.flag[lval] |= flg;
                        flg = prc2->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc3)) )
                            prc3->set.flag[lval] |= flg;
                        break;
                    }
                } else { // dn3 || dn4
                    switch (lnked) {
                    case 13: // Hv 3 & 4 - 1 & 3 linked, 1 takes 3, 2 takes 4
                    case 24: // Hv 3 & 4 - 2 & 4 linked, 2 takes 4, 1 takes 3
                        flg = prc3->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc1)) )
                            prc1->set.flag[lval] |= flg;
                        flg = prc4->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc2)) )
                            prc2->set.flag[lval] |= flg;
                        break;
                    case 14: // Hv 3 & 4 - 1 & 4 linked, 1 takes 4, 2 takes 3
                    case 23: // Hv 3 & 4 - 2 & 3 linked, 2 takes 3, 1 takes 4
                        flg = prc4->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc1)) )
                            prc1->set.flag[lval] |= flg;
                        flg = prc3->set.flag[lval] & cf_XAB;
                        if ( !(GOTCOLOR(prc2)) )
                            prc2->set.flag[lval] |= flg;
                        break;
                    }
                }
            } else {
                if ( !(GOTCOLOR(prclnk1)) )
                    prclnk1->set.flag[lval] |= flg1;
                if ( !(GOTCOLOR(prclnk2)) )
                    prclnk2->set.flag[lval] |= flg1;
                // mark open ends B
                if ( !(GOTCOLOR(prcopn1)) )
                    prcopn1->set.flag[lval] |= flg2;
                if ( !(GOTCOLOR(prcopn2)) )
                    prcopn2->set.flag[lval] |= flg2;
                vdone.push_back(*prclnk1);
                vdone.push_back(*prclnk2);
                vdone.push_back(*prcopn1);
                vdone.push_back(*prcopn2);
            }
        } else {
            // is a single strong link
            if ( !(GOTCOLOR(prc1)) )
                prc1->set.flag[lval] |= flg1;
            if ( !(GOTCOLOR(prc2)) )
                prc2->set.flag[lval] |= flg2;
        }
    }

    // Now the WEAK links
    max2 = pvrcpwl->size();
    // CLEAR COLOR
    for (i2 = 0; i2 < max2; i2++) {
        prcp1 = &pvrcpwl->at(i2);
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        prc1->set.flag[lval] &= ~(cf_XAB);
        prc2->set.flag[lval] &= ~(cf_XAB);
    }

    PRCPAIR psl;
    PROWCOL psll;
    // for each WEAK, find the STRONG link it is LINKED with, and COLOR pre the shared cell
    for (i2 = 0; i2 < max2; i2++) {
        prcp1 = &pvrcpwl->at(i2);
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        lnked = 0;
        sprintf(tb,"WL %s ", Get_RCPAIR_Stg(prcp1,setval));
        if (Joined_to_Strong_Link(pb, prcp1, prc1, pvrcpsl, &psl, &psll)) {
            lnked = 1;
        } else if (Joined_to_Strong_Link(pb, prcp1, prc2, pvrcpsl, &psl, &psll)) {
            lnked = 1;
        }
        if (lnked) {
            flg = psll->set.flag[lval] & cf_XAB;
            if ( !(GOTCOLOR(prc1)) )
                prc1->set.flag[lval] |= flg;
            flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
            if ( !(GOTCOLOR(prc2)) )
                prc2->set.flag[lval] |= flg;
            sprintf(tb,"WL: %s ", Get_RCPAIR_Stg(prcp1,setval));
            sprintf(EndBuf(tb)," SL %s ",  Get_RCPAIR_Stg(psl,setval));
        } else {
            strcat(tb,"NOT LINKED!!!");
        }
        OUTITXC3(tb);
    }

#if 0 // ===== FAILED??? =====

    // for each WEAK, find the STRONG link it is LINKED with, and COLOR pre the shared cell
    for (i2 = 0; i2 < max2; i2++) {
        prcp1 = &pvrcpwl->at(i2);
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        for (i1 = 0; i1 < max1; i1++) {
            prcp2 = &pvrcpsl->at(i1);
            prc3 = &prcp2->rowcol[0];
            prc4 = &prcp2->rowcol[1];
            lnked = 0;
            if (SAMERCCELL(prc1,prc3)) {
                flg = prc3->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc1)) )
                    prc1->set.flag[lval] |= flg;
                flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                //flg = prc4->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc2)) )
                    prc2->set.flag[lval] |= flg;
                lnked = 13;
            } else if (SAMERCCELL(prc1,prc4)) {
                flg = prc4->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc1)) )
                    prc1->set.flag[lval] |= flg;
                flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                //flg = prc3->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc2)) )
                    prc2->set.flag[lval] |= flg;
                lnked = 14;
            } else if (SAMERCCELL(prc2,prc3)) {
                flg = prc3->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc2)) )
                    prc2->set.flag[lval] |= flg;
                flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                //flg = prc4->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc1)) )
                    prc1->set.flag[lval] |= flg;
                lnked = 23;
            } else if (SAMERCCELL(prc2,prc4)) {
                flg = prc4->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc2)) )
                    prc2->set.flag[lval] |= flg;
                flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                //flg = prc3->set.flag[lval] & cf_XAB;
                if ( !(GOTCOLOR(prc1)) )
                    prc1->set.flag[lval] |= flg;
                lnked = 24;
            }
            if (lnked)
                break;
        }
    }
#endif // #if 0 // ===== FAILED??? =====

}

void Mark_Colors_on_SW_Links3_FAILED( PABOX2 pb, int setval, vRCP * pvrcpsl, vRCP *pvrcpwl )
{
    int lval = setval - 1;
    size_t max1 = pvrcpsl->size();
    size_t max2 = pvrcpwl->size();
    PRCPAIR prcp1, prcp2;
    PROWCOL prc, prc1, prc2, prc3, prc4;
    PROWCOL prclnk1, prclnk2, prcopn1, prcopn2;
    uint64_t flg1 = cf_XCA;
    uint64_t flg2 = cf_XCB;
    size_t i1, i2;
    size_t max, ii;
    vRC vdone;
    int lnked;
    vRCP vrcp;
    uint64_t flg;
    bool dn1, dn2, dn3, dn4;
    for (i1 = 0; i1 < max1; i1++) {
        prcp1 = &pvrcpsl->at(i1);
        if (prcp_in_vrcp(prcp1,vrcp)) continue;
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        lnked = 0;
        vrcp.push_back(*prcp1); // Done this one
        for (i2 = 0; i2 < max1; i2++) {
            prcp2 = &pvrcpsl->at(i2);
            if (prcp_in_vrcp(prcp2,vrcp)) continue;
            prc3 = &prcp1->rowcol[0];
            prc4 = &prcp1->rowcol[1];
            max = vdone.size();
            dn1 = dn2 = dn3 = dn4 = false;
            for (ii = 0; ii < max; ii++) {
                prc = &vdone[ii];
                if (!dn1 && SAMERCCELL(prc,prc1)) {
                    dn1 = true;
                    dn2 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    prc1->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    prc2->set.flag[lval] |= flg;
                }
                if (!dn2 && SAMERCCELL(prc,prc2)) {
                    dn1 = true;
                    dn2 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    prc2->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    prc1->set.flag[lval] |= flg;
                }
                if (!dn3 && SAMERCCELL(prc,prc3)) {
                    dn3 = true;
                    dn4 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    prc3->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    prc4->set.flag[lval] |= flg;
                }
                if (SAMERCCELL(prc,prc4)) {
                    dn3 = true;
                    dn4 = true;
                    flg = prc->set.flag[lval] & cf_XAB;
                    prc4->set.flag[lval] |= flg;
                    flg = (flg & cf_XCA) ? cf_XCB : cf_XCA;
                    prc3->set.flag[lval] |= flg;
                }
                if (dn1 && dn2 && dn3 && dn4) break;
            }
            if (dn1 && dn2 && dn3 && dn4) {
                vrcp.push_back(*prcp2); // Done this one
                continue;
            }
            prclnk1 = prclnk2 = prcopn1 = prcopn2 = 0;
            if (SAMERCCELL(prc1,prc3)) {
                prclnk1 = prc1;
                prclnk2 = prc3;
                prcopn1 = prc2;
                prcopn2 = prc4;
                lnked = 13;
            } else if (SAMERCCELL(prc1,prc4)) {
                prclnk1 = prc1;
                prclnk2 = prc4;
                prcopn1 = prc2;
                prcopn2 = prc3;
                lnked = 14;
            } else if (SAMERCCELL(prc2,prc3)) {
                prclnk1 = prc2;
                prclnk2 = prc3;
                prcopn1 = prc1;
                prcopn2 = prc4;
                lnked = 23;
            } else if (SAMERCCELL(prc2,prc4)) {
                prclnk1 = prc2;
                prclnk2 = prc4;
                prcopn1 = prc1;
                prcopn2 = prc3;
                lnked = 24;
            }
            if (lnked)
                break;
        }
        if (lnked) {
            // mark link A
            if (dn1 || dn2 || dn3 || dn4) {
                if (dn1 || dn2) {
                    switch (lnked) {
                    case 13: // 1 & 3 linked, so 3 takes color of 1, and 4 color 2
                    case 24: // 2 & 4 linked, so 4 takes 2, 3 take 1
                        flg = prc1->set.flag[lval] & cf_XAB;
                        prc3->set.flag[lval] |= flg;
                        flg = prc2->set.flag[lval] & cf_XAB;
                        prc4->set.flag[lval] |= flg;
                        break;
                    case 14: // 1 & 4 linked, so 4 takes 1 and 3 takes 2
                    case 23: // 2 & 3 linked, so 3 takes 2, 4 takes 1
                        flg = prc1->set.flag[lval] & cf_XAB;
                        prc4->set.flag[lval] |= flg;
                        flg = prc2->set.flag[lval] & cf_XAB;
                        prc3->set.flag[lval] |= flg;
                        break;
                    }
                } else { // dn3 || dn4
                    switch (lnked) {
                    case 13: // Hv 3 & 4 - 1 & 3 linked, 1 takes 3, 2 takes 4
                    case 24: // Hv 3 & 4 - 2 & 4 linked, 2 takes 4, 1 takes 3
                        flg = prc3->set.flag[lval] & cf_XAB;
                        prc1->set.flag[lval] |= flg;
                        flg = prc4->set.flag[lval] & cf_XAB;
                        prc2->set.flag[lval] |= flg;
                        break;
                    case 14: // Hv 3 & 4 - 1 & 4 linked, 1 takes 4, 2 takes 3
                    case 23: // Hv 3 & 4 - 2 & 3 linked, 2 takes 3, 1 takes 4
                        flg = prc4->set.flag[lval] & cf_XAB;
                        prc1->set.flag[lval] |= flg;
                        flg = prc3->set.flag[lval] & cf_XAB;
                        prc2->set.flag[lval] |= flg;
                        break;
                    }
                }
            } else {
                prclnk1->set.flag[lval] |= flg1;
                prclnk2->set.flag[lval] |= flg1;
                // mark open ends B
                prcopn1->set.flag[lval] |= flg2;
                prcopn2->set.flag[lval] |= flg2;
                vdone.push_back(*prclnk1);
                vdone.push_back(*prclnk2);
                vdone.push_back(*prcopn1);
                vdone.push_back(*prcopn2);
            }
        } else {
            // is a single strong link
            prc1->set.flag[lval] |= flg1;
            prc2->set.flag[lval] |= flg2;
        }
    }
}


int Sort_SW_Linked_Pairs3_CLOSE_BUT( PABOX2 pb, int setval, vRCP *pvrcpsl, vRCP *pvrcpwl )
{
    int count = 0;
    size_t max1 = pvrcpsl->size();
    size_t max2 = pvrcpwl->size();
    size_t i1, i2;
    PRCPAIR prcp1, prcp2;
    PROWCOL prc1,prc2,prc3,prc4;
    int share;
    RCPAIR rcpair1, rcpair2;
    PRCPAIR psl;
    PROWCOL psll;
    vRCP chain;
    int i, chains;
    time_t ch_num, chn;
    char *tb = GetNxtBuf();
    PSET ps1, ps2, ps3;
    int lval = setval - 1;
    uint64_t flg1, flg2;
    PROWCOL prc, prct1, prct2, prct3;;

    sprintf(tb,"Have %d SL and %d WL to sort.", (int)max1, (int)max2);
    OUTITXC3(tb);

    memset(&rcset,0,sizeof(RCSET));
    chains = 0;
    ch_num = 0;
    for (i1 = 0; i1 < max1; i1++) {
        prcp1 = &pvrcpsl->at(i1);
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        // got a STRONG linked PAIR, seek a WEAK linked paired that JOINS these
        sprintf(tb,"SL %s joins ", Get_RCPAIR_Stg(prcp1,setval));
        for (i2 = 0; i2 < max2; i2++) {
            prcp2 = &pvrcpwl->at(i2);
            prc3 = &prcp2->rowcol[0];
            prc4 = &prcp2->rowcol[1];
            // NOW do they share an END
            sprintf(tb,"SL %s joins ", Get_RCPAIR_Stg(prcp1,setval));
            share = 0;
            if (SAMERCCELL(prc1,prc3)) {
                rcpair1.rowcol[0] = *prc2;
                rcpair1.rowcol[1] = *prc1;
                rcpair2.rowcol[0] = *prc3;
                rcpair2.rowcol[1] = *prc4;
                share = 1;
            } else if (SAMERCCELL(prc1,prc4)) {
                rcpair1.rowcol[0] = *prc2;
                rcpair1.rowcol[1] = *prc1;
                rcpair2.rowcol[0] = *prc4;
                rcpair2.rowcol[1] = *prc3;
                share = 2;
            } else if (SAMERCCELL(prc2,prc3)) {
                rcpair1.rowcol[0] = *prc1;
                rcpair1.rowcol[1] = *prc2;
                rcpair2.rowcol[0] = *prc3;
                rcpair2.rowcol[1] = *prc4;
                share = 3;
            } else if (SAMERCCELL(prc2,prc4)) {
                rcpair1.rowcol[0] = *prc1;
                rcpair1.rowcol[1] = *prc2;
                rcpair2.rowcol[0] = *prc4;
                rcpair2.rowcol[1] = *prc3;
                share = 4;
            }
            if (!share) continue; // no shared end

            prct1 = &rcpair1.rowcol[0]; // bgn == strong to
            prct2 = &rcpair1.rowcol[1]; // mid -- weak to 
            prct3 = &rcpair2.rowcol[1]; // end
            // BUT only accept this WEAK link now IFF it ALSO joins with some OTHER SL
            if (!Joined_to_Strong_Link(pb, prcp1, prct3, pvrcpsl, &psl, &psll)) continue;

            sprintf(EndBuf(tb),"%s & %s", Get_RCPAIR_Stg(prcp2,setval),
                Get_RCPAIR_Stg(psl,setval));
            OUTITXC3(tb);

            // have a STONG/WEAK chain forming...
            // store these two PAIR, now in correct chaining order
            chain.push_back(rcpair1);
            chain.push_back(rcpair2);
            // But maybe build the chains in the RCSET - keep in order
            ps1 = &prct1->set;
            ps2 = &prct2->set;
            ps3 = &prct3->set;
            if (chains) {
                // have to find the location for this addition
                // if this pair of pairs ae linked with any already in the RCSET,
                // then they take the same ch_num as existing, and get COLOR from that joining
                for (i = 0; i < chains; i++) {
                    prc = &rcset.rowcol[i];
                    if (SAMERCCELL(prc,prct1)) {
                        flg1 = prc->set.flag[lval] & cf_XAB;
                        flg2 = (flg1 & cf_XCA) ? cf_XCB : cf_XCA;
                        chn = prc->set.tm;
                    } else if (SAMERCCELL(prc,prct2)) {
                        flg1 = prc->set.flag[lval] & cf_XAB;
                        flg2 = (flg1 & cf_XCA) ? cf_XCB : cf_XCA;
                        chn = prc->set.tm;
                    } else if (SAMERCCELL(prc,prct3)) {
                        flg1 = prc->set.flag[lval] & cf_XAB;
                        flg2 = (flg1 & cf_XCA) ? cf_XCB : cf_XCA;
                        chn = prc->set.tm;
                    } else {
                        // is a NEW CHAIN
                        ch_num++;
                        chn = ch_num;
                        flg1 = cf_XCA;
                        flg2 = cf_XCB;
                    }
                }
            } else {
                // this is the FIRST
                ch_num++;
                chn = ch_num;
                flg1 = cf_XCA;
                flg2 = cf_XCB;
            }
            ps1->tm = ch_num;
            ps2->tm = ch_num;
            ps3->tm = ch_num;
            ps1->flag[lval] |= cf_XCA; // ON
            ps2->flag[lval] |= cf_XCB; // OFF
            ps3->flag[lval] |= cf_XCA; // ON

            rcset.rowcol[chains++] = *prct1; // begin
            rcset.rowcol[chains++] = *prct2; // middle
            rcset.rowcol[chains++] = *prct3; // end
            if (chains >= MMX_LINKS)
                break;
        }
        //OUTITXC3(tb);
        if (chains >= MMX_LINKS)
           break;
    }
    sprintf(tb,"Now got %d S-W chains...", (int)(chain.size() / 2));
    OUTITXC3(tb);
    xc3_setval = setval;
    xc3_chains = chains;
    if (add_debug_xc3) {
        for (i = 0; i < chains / 3; i++) {
            prct1 = &rcset.rowcol[(i+3)+0];
            prct2 = &rcset.rowcol[(i+3)+1];
            prct3 = &rcset.rowcol[(i+3)+2];
            sprtf("Chn %d: %s %s %s\n", (int)prct1->set.tm,
                Get_RC_setval_color_Stg(prct1,setval),
                Get_RC_setval_color_Stg(prct2,setval),
                Get_RC_setval_color_Stg(prct3,setval));
        }
    }
    return count;
}

int Get_WL_with_SL_Connection( PABOX2 pb, int setval, vRCP *pvrcpsl, vRCP *pvrcpwl,
                                vRCP *pjoined)
{
    int count = 0;
    size_t max1 = pvrcpsl->size();
    size_t max2 = pvrcpwl->size();
    size_t i1, i2;
    PROWCOL prc1, prc2;
    PROWCOL prc3, prc4;
    PRCPAIR prcp1, prcp2;
    bool ok;
    for (i2 = 0; i2 < max2; i2++) {
        prcp1 = &pvrcpwl->at(i2);
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        // got a weak linked PAIR, seek a strong linked paired that JOINS these
        for (i1 = 0; i1 < max1; i1++) {
            prcp2 = &pvrcpsl->at(i1);
            prc3 = &prcp2->rowcol[0];
            prc4 = &prcp2->rowcol[1];
            ok = false;
            if (SAMERCCELL(prc1,prc3))
                ok = true;
            else if (SAMERCCELL(prc1,prc4))
                ok = true;
            else if (SAMERCCELL(prc2,prc3))
                ok = true;
            else if (SAMERCCELL(prc2,prc4))
                ok = true;
            if (!ok) continue;
            pjoined->push_back(*prcp1);
            count++;
            break;
        }
    }
    return count;
}

int XCycle_Scan3_for_setval( PABOX2 pb, int setval )
{
    int count = 0;
    int row, col, box, val;
    int row2, col2, box2;
    int lval = setval - 1;
    PSET ps1, ps2;
    ROWCOL rc1, rc2;
    uint64_t flag, lflg;
    RCPAIR rcpair;
    char *tb = GetNxtBuf();
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps1 = &pb->line[row].set[col];
            if ( !( ps1->val[lval] ) ) continue;
            // got FIRST
            box = GETBOX(row,col);
            for (row2 = 0; row2 < 9; row2++) {
                for (col2 = 0; col2 < 9; col2++) {
                    if ((row2 == row) && (col2 == col)) continue; // skip SELF
                    val = pb->line[row2].val[col2];
                    if (val) continue;
                    ps2 = &pb->line[row2].set[col2];
                    if ( !( ps2->val[lval] ) ) continue;
                    // got a SECOND with this candidate - does it share a ROW COL or BOX
                    box2 = GETBOX(row2,col2);
                    lflg = 0;
                    if (row2 == row)
                        lflg = cl_LBR; // linked by ROW
                    else if (col2 == col)
                        lflg = cl_LBC;
                    if (box2 == box)
                        lflg |= cl_LBB;
                    if (!lflg) continue; // NO SHARED ROW COL or BOX
                    rc1.row = row;
                    rc1.col = col;
                    rc2.row = row2;
                    rc2.col = col2;
                    //flag = Get_RC_Strong_Link_Flag(pb, &rc1, &rc2, setval);
                    flag = Get_RC_Strong_Link_Flag(pb, &rc1, &rc2, setval, bIncSLB);

                    // got a STRONG or WEAK linked PAIR
                    COPY_SET(&rc1.set,ps1);
                    COPY_SET(&rc2.set,ps2);

                    // clear any exisitng COLOR FLAG
                    rc1.set.flag[lval] &= ~(cf_XAB);
                    rc2.set.flag[lval] &= ~(cf_XAB);

                    if (flag) { // add in the LINK type
                        rc1.set.flag[lval] |= flag;
                        rc2.set.flag[lval] |= flag;
                    }

                    // set the setval
                    rc1.set.uval = setval;
                    rc2.set.uval = setval;

                    // clear chain number
                    rc1.set.tm = 0;
                    rc2.set.tm = 0;

                    // store as a PAIR
                    rcpair.rowcol[0] = rc1;
                    rcpair.rowcol[1] = rc2;
                    if (flag) { // is a STRONG link
                        if (!prcp_in_vrcp(&rcpair,vrcpsl))
                            vrcpsl.push_back(rcpair); // store a STRONG linked pair
                    } else {    // is a WEAK link
                        if (!prcp_in_vrcp(&rcpair,vrcpwl))
                            vrcpwl.push_back(rcpair); // store a WEAK linked pair
                    }
                }
            }
        }
    }
    if (vrcpsl.size() && vrcpwl.size()) { // BOTH have to have SIZE of this setval
        Get_WL_with_SL_Connection( pb, setval, &vrcpsl, &vrcpwl, &joined );
        if (joined.size() < vrcpwl.size()) {
            sprintf(tb,"Weak links reduced from %d to %d", (int)vrcpwl.size(),
                (int)joined.size() );
            OUTITXC3(tb);
        }
    }
    if (vrcpsl.size() && joined.size()) { // BOTH have to have SIZE of this setval
        count = Sort_SW_Linked_Pairs3( pb, setval, &vrcpsl, &joined );
        Mark_Colors_on_SW_Links3( pb, setval, &vrcpsl, &joined );
    }
    return count;
}

int Do_XCycles_Scan3( PABOX2 pb )
{
    int count = 0;
    int i;
    xc3_setval = 0;
    xc3_chains = 0;
    for (i = 0; i < 9; i++) {
        if (xcycle_test_setval && ((i + 1) != xcycle_test_setval)) continue;
        count += XCycle_Scan3_for_setval( pb, (i + 1) );
        if (count && only_one_xc3)
            break;
    }
    return count;
}

int test_xc_scan3( PABOX2 pb )
{
    if (paint_links_setval) {
        paint_links_setval = 0;
        return 0;
    }
    xcycle_test_setval = 1;
    sprtf("Doing X-Cycles test scan 3 for setval %d\n", xcycle_test_setval);
    int count = Do_XCycles_Scan3(pb);
    if (count)
        paint_links_setval = xcycle_test_setval;
    return count;
}  

// sudo_XCycle3.cxx
