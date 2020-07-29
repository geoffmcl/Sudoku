// Sudo_XCycles.cxx

#include "Sudoku.hxx"

#define OUTITXC(tb) if (add_debug_xc) OUTIT(tb)
#define DBGSTOP(r,c) if (((r - 1) == row)&&((c - 1) == col)) debug_stop2() // based on row col
#define DBGSTOPRC(rw,r,cl,c) \
    if (((rw == (r - 1)) && (cl == (c - 1)))) debug_stop2()

#define OUTITSL2(tb) if (add_debug_sl2) OUTIT(tb)

static int only_one_sl = 1;

int only_one_xc = 1;
int min_chain_members = 6; // probably should be LARGER - and what about EVEN
int min_sl_count = 3;       // hmm, not sure
int in_debug_set = 0;
BOOL g_bDoElims = FALSE;

// from : http://www.sudoku9981.com/sudoku-solving/x-cycle.php
// Fishy Cycle (X-Cycle)

// A Fishy Cycle is a continuous loop of candidates for a single digit. It has 
// alternating strong links and weak links. 

// The Fishy Cycle was first introduced as a generalization of fish patterns 
// like X-Wing, Swordfish and Jellyfish, before these patterns were seen as 
// constraint sets. At that time, players believed that each defining line 
// should contain only 2 candidates. There are still some websites that contain 
// this old definition of Swordfish and Jellyfish. 
// An example may help you understand the concept: see xcycle-01.png
// Rows 1, 3, 4 & 7 all have 2 candidates. A cycle can be formed by connecting 
// these candidates using the columns. The resulting cycle is: 
// r1c2 = r1c4 - r3c4 = r3c7 - r7c7 = r7c5 - r4c5 = r4c2 - r1c2
// This pattern is a Jellyfish. We can eliminate all remaining candidates 
// for this digit from columns 2, 4, 5 & 7. 
// A Fishy Cycle does not need to be restricted to rows and columns. Boxes can 
// also provide strong or weak links. Here is another example: see xcycle-02.png
// This cycle uses 1 box and reads: 
// r2c2 = r2c5 - r5c5 = r6c6 - r8c6 = r8c2 - r2c2
// Remaining candidates can now be eliminated from columns 2, 5 & 6. 

// from : http://www.sudokuwiki.org/X_Cycles
// X-Cycles are the start of a large family of 'chaining strategies' which 
// are fundamental to solving the harder Sudoku puzzles. X-Cycles are strongly 
// related to Simple Coloring.
// X-Wing is a Continuous X-Cycle with the length of four.
// Sword-Fish of the 2-2-2 formation is a Continuous X-Cycle with the length of six.
// Start with any candidate of a slot, give it the color A
// Find the next cell also with that candidate, give it the color B

int check_strong_link = 0; // No, here want weak links as well
int Do_XCycles_Scan2( PABOX2 pb, PROWCOL prc, int setval, vRC *pvrc, time_t ch_num );

#define DO_IMMED_SCAN

// This time process each NOW found immediately
// And ONLY onwards from last find
int Do_XCycles_Scan2( PABOX2 pb, PROWCOL prc, int setval, vRC *pvrc, time_t ch_num,
                      int dep )
{
    int count = 0;
    int row, col, val, box2, cnt, fcol, scnt;
    ROWCOL rc;
    PSET ps;
    int box = GETBOX(prc->row,prc->col);
    bool isAclr = (prc->set.flag[setval - 1] & cf_XCA) ? true : false;
    uint64_t cflg = isAclr ? cf_XCB : cf_XCA;
    char *tb = GetNxtBuf();
    uint64_t slflg;
#ifndef DO_IMMED_SCAN
    vRC todo;
#endif
    // scan onwards for a PAIR to this slot
    fcol = prc->col + 1; // start at LAST
    // except if a strong link in this ROW
    // uint64_t Get_RC_Strong_Link_Flag( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, int setval )
    cnt = Count_setval_in_Row( pb, prc->row, prc->col, setval );
    if (cnt == 2)
        fcol = 0;

    for (row = prc->row; row < 9; row++) {
        for (col = fcol; col < 9; col++) {
    //for (row = 0; row < 9; row++) {
    //    for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            if ( !(ps->val[setval - 1]) ) continue;
            if (RC_in_pvrc(pvrc, row, col)) continue;
            box2 = GETBOX(row,col);
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
            // DBGSTOP(4,4);
            rc.row = row;
            rc.col = col;
            COPY_SET((void *)&rc.set,(const void *)ps);
            // COLOR set per candidate as a flag!!! rc.set.uval = prc->set.uval ? 0 : 1;    // set COLOR
            slflg = Get_RC_Strong_Link_Flag(pb,prc,&rc,setval);
            cflg = isAclr ? cf_XCB : cf_XCA;
            rc.set.flag[setval -1] |= (cflg | slflg);
            if (slflg)
                prc->set.flag[setval - 1] |= slflg;
            rc.set.tm = ch_num;
            sprintf(tb,"join %d:%d:%d: %s %s to %s %s %s", setval, (int)ch_num, dep,
                Get_RC_RCB_Stg(prc), (isAclr ? "A" : "B"),
                Get_RC_RCB_Stg(&rc), (isAclr ? "B" : "A"),
                (slflg ? "SL" : "WL") );
            OUTITXC(tb);
            pvrc->push_back(rc);
            count++;
#ifdef DO_IMMED_SCAN
            cflg = rc.set.flag[setval-1]; 
            scnt = Do_XCycles_Scan2( pb, &rc, setval, pvrc, ch_num, dep + 1 );
            count += scnt;
            sprintf(tb,"scan from %s %s got %d ", Get_RC_RCB_Stg(&rc),
                ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"),
                scnt );
            OUTITXC(tb);
#else
            todo.push_back(rc); // add it for continuation of this chain
#endif // #ifdef DO_IMMED_SCAN
        }
        fcol = 0;   // now from COL 0
    }
#ifndef DO_IMMED_SCAN
    // now process the 2nd of each, going to next in chain
    size_t max = todo.size();
    size_t i;
    for (i = 0; i < max; i++) {
        rc = todo[i];
        cflg = rc.set.flag[setval-1]; 
        scnt = Do_XCycles_Scan2( pb, &rc, setval, pvrc, ch_num, dep + 1 );
        count += scnt;
        sprintf(tb,"scan from %s %s got %d ", Get_RC_RCB_Stg(&rc),
            ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"),
            scnt );
        OUTITXC(tb);
    }
#endif // #ifndef DO_IMMED_SCAN
    return count;
}


vRC vrc_XCycles[9];
vRC *get_XCycles_pvrc( int val ) { return &vrc_XCycles[val]; }
PROWCOL get_XCycles_RC( int row, int col, int setval )
{
    static ROWCOL rc;
    vRC *pvrc = &vrc_XCycles[setval - 1];
    size_t max = pvrc->size();
    size_t i;
    for (i = 0; i < max; i++) {
        rc = pvrc->at(i);
        if ((rc.row == row)&&(rc.col == col))
            return &rc;
    }
    return 0;
}

typedef struct tagCHNSTR {
    int count;
    PABOX2 pb;
    PROWCOL prc, prcfirst;
    int setval;
    time_t ch_num;
    int dep;
    bool last_was_row;
    int complete;
    int sl_count;
}CHNSTR, *PCHNSTR;

// can the LAST member, pch->prc join to first, pch->prcfirst
int End_of_Chain( PCHNSTR pch )
{
    PROWCOL prc  = pch->prc;
    PROWCOL prc2 = pch->prcfirst;
    if (GETBOX(prc->row,prc->col) == GETBOX(prc2->row,prc2->col)) {
        pch->complete |= 1;
    }
    if (prc->row == prc2->row) {
        pch->complete |= 2;
    } else if (prc->col == prc2->col ) {
        pch->complete |= 4;
    }
    return pch->complete;
}

// Have two spots with a STRONG link 
// Follow CHAIN until the END
int add_color_too = 0;
int Do_XCycles_Chain( PCHNSTR pch, vRC &members )
{
    int count = 0;
    PABOX2 pb = pch->pb;
    PROWCOL prc = pch->prc;
    int setval = pch->setval;
    time_t ch_num = pch->ch_num;
    int dep = pch->dep;
    int row, col, val, cnt;
    bool last_was_row = pch->last_was_row;
    int sl_count = pch->sl_count;
    PSET ps;
    ROWCOL rc;
    uint64_t slflg;
    int mbox = GETBOX(prc->row, prc->col);
    uint64_t cflg = (prc->set.flag[setval - 1] & cf_XCA) ? cf_XCB : cf_XCA;
    char *tb = GetNxtBuf();
    if (last_was_row) {
        col = prc->col;
        //cnt = Count_setval_in_Col(pb, prc->row, prc->col, setval);
        //sprintf(tb,"Scanning COL %d, with %d cands - color %s", col + 1, cnt, (cflg & cf_XCA ? "A" : "B") );
        //OUTITXC(tb);
        for (row = 0; row < 9; row++) {
            if (row == prc->row) continue;
            if (Row_in_RC_Vector(members, row, col)) continue; // already in chain
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if (!ps->val[setval - 1]) continue; // does NOT have the candidate
            if (Row_Col_in_RC_Vector(members, row, col)) continue; // already in chain
            // got another member
            rc.row = row;
            rc.col = col;
            slflg = Get_RC_Strong_Link_Flag(pb, &rc, prc, setval);
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
            pch->prc = &rc;
            sprintf(tb,"%d:%d: COL %d ", setval, dep, col + 1);
            if (add_color_too) {
                sprintf(EndBuf(tb),"from %s %s ",
                    Get_RC_RCB_Stg(prc), ((prc->set.flag[setval - 1] & cf_XCA) ? "A" : "B") );
                sprintf(EndBuf(tb),"to %s %s ",
                    Get_RC_RCB_Stg(pch->prc),
                    ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"));
            } else {
                sprintf(EndBuf(tb),"from %s ", Get_RC_RCB_Stg(prc));
                sprintf(EndBuf(tb),"to %s ", Get_RC_RCB_Stg(pch->prc));
            }
            // **********************************************************************
            pch->complete = 0;
            if (End_of_Chain(pch)) {
                sprintf(EndBuf(tb),"First %s ", Get_RC_RCB_Stg(pch->prcfirst));
                if (pch->sl_count < min_sl_count) {
                    pch->count = count;     // restart counter
                    pch->dep = dep;         // back up depth
                    pch->sl_count = sl_count; // back up SL count
                    members.pop_back();     // remove last member
                    sprintf(EndBuf(tb),"SKIPPED count %d, SL %d", cnt, pch->sl_count );
                    OUTITXC(tb);
                    continue;
                } else {
                    sprintf(EndBuf(tb),"END CHAIN count %d, SL %d", cnt, pch->sl_count );
                    OUTITXC(tb);
                    return 1;
                }
            }
            // **********************************************************************
            val = Do_XCycles_Chain(pch, members);
            cnt = pch->count;
            sprintf(EndBuf(tb),"ret %d count %d", val, cnt );
            OUTITXC(tb);
            if (val == 1)
                return 1;
            if (val < 0) {
                pch->count = count;     // restart counter
                pch->dep = dep;         // back up depth
                pch->sl_count = sl_count; // back up SL count
                members.pop_back();     // remove last member
                continue;
            }
        }
    } else {
        row = prc->row;
        //cnt = Count_setval_in_Row(pb, prc->row, prc->col, setval);
        //sprintf(tb,"Scanning ROW %d, with %d cands - color %s", row + 1, cnt, (cflg & cf_XCA ? "A" : "B") );
        //OUTITXC(tb);
        for (col = 0; col < 9; col++) {
            if (col == prc->col) continue;
            if (Col_in_RC_Vector(members, row, col)) continue; // already in chain
            val = pb->line[row].val[col];
            if (val) continue; // has a value
            ps = &pb->line[row].set[col];
            if (!ps->val[setval - 1]) continue; // does NOT have the candidate
            if (Row_Col_in_RC_Vector(members, row, col)) continue; // already in chain
            // got another member
            rc.row = row;
            rc.col = col;
            slflg = Get_RC_Strong_Link_Flag(pb, &rc, prc, setval);
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
            pch->prc = &rc;
            sprintf(tb,"%d:%d: ROW %d ", setval, dep, row + 1);
            if (add_color_too) {
                sprintf(EndBuf(tb),"from %s %s ",
                    Get_RC_RCB_Stg(prc), ((prc->set.flag[setval - 1] & cf_XCA) ? "A" : "B") );
                sprintf(EndBuf(tb),"to %s %s ",
                    Get_RC_RCB_Stg(pch->prc),
                    ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"));
            } else {
                sprintf(EndBuf(tb),"from %s ", Get_RC_RCB_Stg(prc));
                sprintf(EndBuf(tb),"to %s ", Get_RC_RCB_Stg(pch->prc));
            }
            // **********************************************************************
            pch->complete = 0;
            if (End_of_Chain(pch)) {
                sprintf(EndBuf(tb),"First %s ", Get_RC_RCB_Stg(pch->prcfirst));
                if (pch->sl_count < min_sl_count) {
                    pch->count = count;     // restart counter
                    pch->dep = dep;         // back up depth
                    pch->sl_count = sl_count; // back up SL count
                    members.pop_back();     // remove last member
                    sprintf(EndBuf(tb),"SKIPPED count %d, SL %d", cnt, pch->sl_count );
                    OUTITXC(tb);
                    continue;
                } else {
                    sprintf(EndBuf(tb),"END CHAIN count %d, SL %d", cnt, pch->sl_count );
                    OUTITXC(tb);
                    return 1;
                }
            }
            // **********************************************************************
            OUTITXC(tb);
            val = Do_XCycles_Chain(pch, members);
            cnt = pch->count;
            //sprintf(EndBuf(tb),"ret %d count %d", val, cnt );
            //OUTITXC(tb);
            if (val == 1)
                return val;
            if (val < 0) {
                pch->count = count;     // restart counter
                pch->dep = dep;         // back up depth
                pch->sl_count = sl_count; // back up SL count
                members.pop_back();     // remove last member
                continue;
            }
        }
    }
    return -1;
}

// A "Cycle", as the name implies, is a loop or joined-up chain of single 
// digits with alternating strong and weak links.
// So scanning per digit (candidate),
// First find a STRONG linked PAIR
int Do_XCycles_Scan( PABOX2 pb )
{
    int count = 0;
    int row, col, box, val, i, setval, cnt, fcol;
    int row2, col2, box2;
    int scnt;
    ROWCOL rc, rc2;
    PSET ps, ps2;
    vRC *pvrc;
    bool good, done;
    uint64_t slflg;
    char *tb = GetNxtBuf();
    // for EACH candidate
    time_t ch_num;
    int ccnt;
    vRC members;    // accumuate the chain members into here
    CHNSTR chnstr;
    for (i = 0; i < 9; i++) {
        pvrc = get_XCycles_pvrc(i);
        pvrc->clear();
        setval = i + 1;
        // find a cell (spot)
        ch_num = 1; // start chain number for this candidate
        sprintf(tb,"Processing %d ",setval);
        ccnt = 0;
        done = false;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                val = pb->line[row].val[col];
                if (val) continue;
                ps = &pb->line[row].set[col];
                if (!ps->val[i]) continue; // does NOT have candidate
                rc.row = row;
                rc.col = col;
                //if (i == 0) DBGSTOP(4,4);
                //if ((i == 0)&&(row == 3)&&(col == 3)) {
                //    debug_stop2();
                //    in_debug_set = 1;
                //}
                box = GETBOX(row,col);
                // now find a SECOND
                good = false; // may NEVER find one
                slflg = 0;
                // continue onwards finding 2nds...
                fcol = col + 1;
                cnt = 0;
                for (row2 = row; row2 < 9; row2++) {
                    for (col2 = fcol; col2 < 9; col2++) {
                        // continue scan for a SL on this candidate
                        if (in_debug_set && (row2 == 7) && (col2 == 3))
                            debug_stop2();
                        val = pb->line[row2].val[col2];
                        if (val) continue;
                        ps2 = &pb->line[row2].set[col2];
                        if (!ps2->val[i]) continue; // does NOT have this candidate
                        // FOUND a second with this candidate
                        // DBGSTOPRC(row2,8,col2,4);
                        box2 = GETBOX(row2,col2);
                        // if ( !((box2 == box)||(row2 == row)||(col2 == col)) ) continue;
                        if ( !((row2 == row)||(col2 == col)) ) continue;
                        // they share a ROW or COL
                        rc2.row = row2;
                        rc2.col = col2;
                        // MUST be a STRONG LINK in just a ROW or COL
                        slflg = Get_RC_Strong_Link_Flag(pb, &rc, &rc2, setval, false);
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
                        members.push_back(rc);
                        members.push_back(rc2);
                        ccnt++;
                        // got FIRST in chain - get chain from this
                        sprintf(tb,"Join %d:%d:0: %s A to %s B SL", setval, (int)ch_num,
                            Get_RC_RCB_Stg(&rc),
                            Get_RC_RCB_Stg(&rc2) );
                        OUTITXC(tb);
                        uint64_t cflg = rc.set.flag[setval - 1]; 
                        count = 2;
                        chnstr.count = 2;
                        chnstr.ch_num = ch_num;
                        chnstr.dep = 1;
                        chnstr.pb = pb;
                        chnstr.prc = &rc2;
                        chnstr.prcfirst = &rc;
                        chnstr.setval = setval;
                        chnstr.last_was_row = (rc.row == rc2.row) ? true : false;
                        chnstr.sl_count = 1;
                        chnstr.complete = 0;
                        // DBGSTOP2(row,4,col,4,row2,8,col2,4);
                        scnt = Do_XCycles_Chain(&chnstr, members);
                        count = chnstr.count;
                        //if (count < min_chain_members) {
                        if ((count < min_chain_members)||(chnstr.sl_count < min_sl_count)) {
                            members.clear(); // remove ALL members, and continue search
                            continue;
                        }
                        sprintf(tb,"Scan %d from %s%s ret %d count %d slc %d", i+1,
                            Get_RC_RCB_Stg(&rc),
                                ((cflg & cf_XCA) ? "A" : (cflg & cf_XCB) ? "B" : "???"),
                                scnt, count, chnstr.sl_count );
                        OUTITXC(tb);
                        size_t v;
                        for (v = 0; v < members.size(); v++) {
                            rc = members[v];
                            pvrc->push_back(rc);
                        }
                        members.clear();
                        cnt++;
                        if (only_one_xc) {
                            done = true;
                            break;
                        }
                    }   // for (col2 = col + 1; col2 < 9; col2++
                    fcol = 0;
                    if (done)
                        break;
                }   // for (row2 = row; row2 < 9; row2++) // each row onwards
                if (cnt)
                    ch_num++; // bump chain number
                if (done)
                    break;
            }   // for (col = 0; col < 9; col++) // for each COL
            if (done)
                break;
        }   // for (row = 0; row < 9; row++) // for each ROW
    }   // for (i = 0; i < 9; i++) // for each candidate
    return count;
}

// ANOTHER STRONG TRY AT X-Cycles CHAINS
// =====================================
// At this stage still ONLY for SHOW
// but it does show an elimination in 
static int act_setval2[9];   // list of setval marked
static vRCP sl_pairs2;       // RCPAIR list of LINKS
static bool sl_pairs_valid2 = false; // above LIST is VALID
static int sl_eliminations2 = 0;
int *Get_Act_Setval() { return act_setval2; }
vRCP *Get_SL_Pairs() { return &sl_pairs2;  }
bool Is_SLP_Valid() { return sl_pairs_valid2; }
void Set_SLP_setval( int val )
{
    memset(act_setval2,0,sizeof(act_setval2));
    if (VALUEVALID(val))
        act_setval2[val - 1] = val;
}
bool Set_SLP_Valid(bool b)
{
    bool a = sl_pairs_valid2;
    sl_pairs_valid2 = b;
    return a;
}
void Invalidate_SLP()
{ 
    sl_pairs_valid2 = false; 
    Do_Fill_Stack(get_curr_box());
}

#define add_sl_boxes    g_bIBL
#define add_weak_links  g_bIWL


#define OUTITSL(tb) if (add_debug_sl) OUTIT(tb)


bool Cell_Has_Strong_Link( vRCP &sla, vRCP &tmp, int setval, PROWCOL prch4 )
{
    bool bret = false;
    int lval = setval - 1;
    size_t max;
    size_t jj;
    PRCPAIR prcp2;
    PROWCOL prc3, prc4;
    bool fnd;
    vRCP *pp = Get_SL_Pairs();
    max = pp->size();
    for (jj = 0; jj < max; jj++) {
        prcp2 = &pp->at(jj); // get OUT a pair
        if (prcp_in_vrcp(prcp2,sla)) continue;
        prc3 = &prcp2->rowcol[0];
        if ( !(prc3->set.uval == setval) ) continue;
        prc4 = &prcp2->rowcol[1];
        if ( !((prc3->set.flag[lval] & cl_SLA)&&(prc4->set.flag[lval] & cl_SLA)) ) continue;
        fnd = false;
        if ((prch4->row == prc3->row)&&(prch4->col == prc3->col)) {
            fnd = true;
        } else if ((prch4->row == prc4->row)&&(prch4->col == prc4->col)) {
            fnd = true;
        }
        if (fnd) {
            tmp.push_back(*prcp2);
            bret = true;
        }
    }
    return bret;
}

int Find_Next_Link_OK( PABOX2 pb, vRCP &sla, vRCP &wla, int setval, PROWCOL prc1, PROWCOL prc2, bool findweak,
                        PBOOL pChg, time_t ch_num )
{
    int count = 0;
    int lval = setval - 1;
    size_t max;
    size_t jj;
    PRCPAIR prcp2;
    PROWCOL prc3, prc4;
    PROWCOL prch1, prch2, prch3, prch4;
    bool fnd;
    uint64_t flg1, flg2;
    char *tb = GetNxtBuf();
    vRCP *pp = Get_SL_Pairs();
    flg1 = prc1->set.flag[lval];
    max = pp->size();
    for (jj = 0; jj < max; jj++) {
        prcp2 = &pp->at(jj); // get OUT a pair
        if (prcp_in_vrcp(prcp2,sla)) continue;
        prc3 = &prcp2->rowcol[0];
        if ( !(prc3->set.uval == setval) ) continue;
        prc4 = &prcp2->rowcol[1];
        if (findweak) {
            if ((prc3->set.flag[lval] & cl_SLA)||(prc4->set.flag[lval] & cl_SLA)) continue; // STRONG - skip
        } else {
            if ( !((prc3->set.flag[lval] & cl_SLA)&&(prc4->set.flag[lval] & cl_SLA)) ) continue; // WEAK - skip
        }
        fnd = false;

        flg2 = prc3->set.flag[lval];
        char *sm = "";
        prch1 = prc1;
        prch2 = prc2;
        prch3 = prc3;
        prch4 = prc4;

        if ((prc1->row == prc3->row)&&(prc1->col == prc3->col)) {
            fnd = true;
            sm = "13";
            prch1 = prc2;
            prch2 = prc1;
            prch3 = prc3;
            prch4 = prc4;
        } else if ((prc1->row == prc4->row)&&(prc1->col == prc4->col)) {
            fnd = true;
            sm = "14";
            prch1 = prc2;
            prch2 = prc1;
            prch3 = prc4;
            prch4 = prc3;
        } else if ((prc2->row == prc3->row)&&(prc2->col == prc3->col)) {
            fnd = true;
            sm = "23";
            prch1 = prc1;
            prch2 = prc2;
            prch3 = prc3;
            prch4 = prc4;
        } else if ((prc2->row == prc4->row)&&(prc2->col == prc4->col)) {
            fnd = true;
            sm = "24";
            prch1 = prc1;
            prch2 = prc2;
            prch3 = prc4;
            prch4 = prc3;
        }
        if (fnd) {
            vRCP tmp;
            sla.push_back(*prcp2);
            if ( Cell_Has_Strong_Link( sla, tmp, setval, prch4 ) ) {
                // hmmm, this did NOT do it because it is ALREADY stacked on the vector, so
                if (prch1 == prc2) {
                //    ROWCOL rc;
                //    rc = *prc1;
                //    *prc1 = *prc2;
                //    *prc2 = rc;
                    *pChg = TRUE;
                }
                if (prch3 == prc4) {
                    // need to SWITCH ends
                    sla.pop_back();
                    RCPAIR rcpair;
                    rcpair.rowcol[0] = *prch3;
                    rcpair.rowcol[1] = *prch4;
                    sla.push_back(rcpair);
                }
                sprintf(tb,"Link %s %s %s ",
                    Get_RC_setval_RC_Stg(prch1,setval),
                    ((flg1 & cl_SLA) ? stglnk : weklnk),
                    Get_RC_setval_RC_Stg(prch2,setval) );
                sprintf(EndBuf(tb),"%s %s %s %s ", sm,
                    Get_RC_setval_RC_Stg(prch3,setval),
                    ((flg2 & cl_SLA) ? stglnk : weklnk),
                    Get_RC_setval_RC_Stg(prch4,setval) );
                size_t max2 = tmp.size();
                size_t kk;
                for (kk = 0; kk < max2; kk++) {
                    prcp2 = &tmp[kk]; // get OUT a pair
                    prc3 = &prcp2->rowcol[0];
                    prc4 = &prcp2->rowcol[1];
                    flg2 = prc3->set.flag[lval];
                    sprintf(EndBuf(tb),"= %s %s %s ",
                        Get_RC_setval_RC_Stg(prc3,setval),
                        ((flg2 & cl_SLA) ? stglnk : weklnk),
                        Get_RC_setval_RC_Stg(prc4,setval) );
                    sla.push_back(*prcp2);
                    wla.push_back(*prcp2);
                    count++;
                }
                OUTITSL(tb);
            } else {
                sla.pop_back();
            }
        }
    }
    return count;
}

// One type of ELIMINATION is if it is EVEN and CLOSED
int Check_XCyles_Elim( PABOX2 pb, vRCP *pchain, int setval, bool closed )
{
    int count = 0;
    int box, cnt, i;
    vRCP tmp;
    vRC vrc;
    vRC elims;
    size_t maxch = pchain->size();
    size_t ii, max2, max3;
    PRCPAIR pp;
    PROWCOL prc1, prc2;
    ROWCOL rcs[9];
    PRCPAIR prcpairs[9];
    vRC empty;
    RCRCB rcrcb;
    vRC *pvbox;
    char *tb = GetNxtBuf();
    // get ALL the emty cells with this setval
    cnt = Get_Empty_Cells_of_setval( pb, empty, setval, &rcrcb );
    if (maxch && !(maxch & 1) && closed) {
        // Maybe check BOX by BOX
        for (box = 0; box < 9; box++) {
            pvbox = &rcrcb.vboxs[box];
            max3 = pvbox->size();
            sprintf(tb,"Box %d with %d ", box + 1, (int)max3);
            if (max3 < 3) continue; // no interest unless this setval exists 3 or more time
            tmp.clear();
            vrc.clear();
            for (ii = 0; ii < maxch; ii++) {
                pp = &pchain->at(ii);
                prc1 = &pp->rowcol[0];
                prc2 = &pp->rowcol[1];
                // Since joined chain will happen twice, but ONLY want first time
                if (GETBOX(prc1->row,prc1->col) == box) {
                    if (!Row_Col_in_RC_Vector(vrc,prc1->row,prc1->col)) {
                        tmp.push_back(*pp);
                        vrc.push_back(*prc1);
                    }
                } else if (GETBOX(prc2->row,prc2->col) == box) {
                    if (!Row_Col_in_RC_Vector(vrc,prc2->row,prc2->col)) {
                        tmp.push_back(*pp);
                        vrc.push_back(*prc2);
                    }
                }
            }
            // got the LIST per BOX
            max2 = tmp.size();
            sprintf(EndBuf(tb),"with %d ends ", (int)max2);
            if (max2 < 2) continue;
            // got a BOX containing TWO (or more?) RCPAIR cells
            // and the vector vrc contains the ROWCOL cell in the BOX
            cnt = 0;
            for (ii = 0; ii < max2; ii++) {
                prcpairs[cnt] = &tmp[ii];
                Append_RCPAIR_Settings( tb, prcpairs[cnt], setval );
                rcs[cnt] = vrc[ii];
                cnt++;
            }
            // got an extracted set to party with
            // pvbox contains ALL empty cells in THIS box, with setval
            // get candidates in this box
            elims.clear();
            for (ii = 0; ii < max3; ii++) {
                prc1 = &pvbox->at(ii);
                for (i = 0; i < cnt; i++) {
                    prc2 = &rcs[i];
                    if (SAMERCCELL(prc1,prc2)) {
                        break;
                    }
                }
                if (i < cnt) continue;
                elims.push_back(*prc1);
            }
            size_t max4 = elims.size();
            size_t kk;
            sprintf(EndBuf(tb),"Test elim of %d ", (int)max4);
            for (kk = 0; kk < max4; kk++) {
                prc1 = &elims[kk];
                sprintf(EndBuf(tb), "%s ",
                    Get_RC_setval_RC_Stg( prc1, setval )); // return 7@A5, 345@B8, etc...
            }
            OUTITSL(tb);

            sprintf(tb,"For elim of ");
            int scol = 0;
            int srow = 0;
            uint64_t flag = 0;
            vRC elim2;
            elim2.clear();
            for (kk = 0; kk < max4; kk++) {
                prc1 = &elims[kk];
                sprintf(EndBuf(tb), "%s ",
                    Get_RC_setval_RC_Stg( prc1, setval )); // return 7@A5, 345@B8, etc...
                for (i = 0; i < cnt; i++) {
                    prc2 = &rcs[i];
                    if (prc2->row == prc1->row) {
                        sprintf(EndBuf(tb), "same ROW %d as %s of pair ", prc1->row + 1, Get_RC_setval_RC_Stg( prc2, setval )); // return 7@A5, 345@B8, etc...
                        Append_RCPAIR_Settings( tb, prcpairs[i], setval );
                        ADDSP(tb);
                        flag |= prc2->set.flag[setval - 1];
                        srow++;
                    } else if (prc2->col == prc1->col) {
                        sprintf(EndBuf(tb), "same COL %d as %s of pair ", prc1->col + 1, Get_RC_setval_RC_Stg( prc2, setval )); // return 7@A5, 345@B8, etc...
                        Append_RCPAIR_Settings( tb, prcpairs[i], setval );
                        ADDSP(tb);
                        flag |= prc2->set.flag[setval - 1];
                        scol++;
                    }
                }
                flag = (flag & (cf_XCA|cf_XCB));
                if (((srow == 2)||(scol == 2))&&(flag == (cf_XCA|cf_XCB))) {
                    strcat(tb,"ELIM SR|SC & A&B color ");
                    elim2.push_back(*prc1);
                } else {
                    strcat(tb,"NO ");
                }
            }
            OUTITSL(tb);
            max4 = elim2.size();
            for (kk = 0; kk < max4; kk++) {
                prc1 = &elim2[kk];
                sprtf("Elim of %s ",
                    Get_RC_setval_RC_Stg( prc1, setval )); // return 7@A5, 345@B8, etc...
                pb->line[prc1->row].set[prc1->col].flag[setval - 1] |= cf_XCE;
                sl_eliminations2++;
                count++;
            }
            if (max4) {
                for (ii = 0; ii < maxch; ii++) {
                    pp = &pchain->at(ii);
                    prc1 = &pp->rowcol[0];
                    prc2 = &pp->rowcol[1];
                    pb->line[prc1->row].set[prc1->col].flag[setval - 1] |= cf_XC;
                    pb->line[prc2->row].set[prc2->col].flag[setval - 1] |= cf_XC;
                }
                break;
            }
        }
    }
    return count;
}


int Extend_Chain_Pairs( PABOX2 pb, vRCP *pdst, vRCP &src )
{
    int count = 0;
    bool closed = false;
    PRCPAIR pp, pp2, pplast, ppfirst;
    PROWCOL prc1, prc2, prc3, prc4;
    PROWCOL prch1, prch2, prch3, prch4;
    int lval, setval, cnt;
    size_t max2, jj, max, ii;
    time_t ch_num;
    bool first, fnd;
    RCPAIR rcpair;
    uint64_t flg1, flg2;
    int last_setval = -1;
    time_t last_ch_num = -1;
    char *tb = GetNxtBuf();
    max = pdst->size();
    max2 = src.size();
    //sprintf(tb,"Extend: ");
    for (jj = 0; jj < max2; jj++) {
        pp = &src[jj];
        prc1 = &pp->rowcol[0];
        setval = prc1->set.uval;
        ch_num = prc1->set.tm;
        prc2 = &pp->rowcol[1];
        lval = setval - 1;
        if ( !((prc1->set.flag[lval] & cl_SLA)&&(prc2->set.flag[lval] & cl_SLA)) ) continue;
        if (prcp_in_pvrcp(pp,pdst)) continue; // aleady in chain
        // Does this STRONG LINK, NOT in CHAIN, join with either the first or last in the current chains
        first = false;
        ppfirst = 0;
        pplast = 0;
        // Get FIRST and LAST of THIS chain
        for (ii = 0; ii < max; ii++) {
            pp2 = &pdst->at(ii);
            prc3 = &pp2->rowcol[0];
            prc4 = &pp2->rowcol[1];
            if ( !(setval == prc3->set.uval) ) continue;
            if ( !(ch_num == prc3->set.tm) ) continue;
            if (first) {
                pplast = pp2;
            } else {
                ppfirst = pp2;
                first = true;
            }
        }
        if ( !ppfirst || !pplast ) continue;
        last_setval = setval;
        last_ch_num = ch_num;
        // GOT a STRONG link that maybe can EXTEND the current CHAIN, either 
        // from first inserted, or last links appended
        pp2 = ppfirst;
        prc3 = &pp2->rowcol[0];
        prc4 = &pp2->rowcol[1]; // for first ONLY extension is to FIRST
        sprintf(tb,"ExFirst %s%s%s ",
            Get_RC_setval_RC_Stg(prc3,setval),
            stglnk,
            Get_RC_setval_RC_Stg(prc4,setval) );
        fnd = false;
        prch1 = prc1;
        prch2 = prc2;
        prch3 = prc3;
        prch4 = prc4;
        if ((prc1->row == prc3->row)&&(prc1->col == prc3->col)) {
            fnd = true;
            prch1 = prc2;
            prch2 = prc1;
            prch3 = prc3;
            prch4 = prc4;
        //} else if ((prc1->row == prc4->row)&&(prc1->col == prc4->col)) {
        //    fnd = true;
        //    prch1 = prc2;
        //    prch2 = prc1;
        //    prch3 = prc4;
        //    prch4 = prc3;
        } else if ((prc2->row == prc3->row)&&(prc2->col == prc3->col)) {
            fnd = true;
            prch1 = prc1;
            prch2 = prc2;
            prch3 = prc3;
            prch4 = prc4;
        //} else if ((prc2->row == prc4->row)&&(prc2->col == prc4->col)) {
        //    fnd = true;
        //    prch1 = prc1;
        //    prch2 = prc2;
        //    prch3 = prc4;
        //    prch4 = prc3;
        }
        if (fnd) {
            // YOW, can EXTEND chain by this STRONG LINK
            count++;
            strcat(tb, "ok ");
            rcpair.rowcol[0] = *prch1;
            rcpair.rowcol[1] = *prch2;
            // 2 is being inserted before 3
            flg1 = prch3->set.flag[setval - 1];
            flg1 = (flg1 & cf_XCA) ? cf_XCA : (flg1 & cf_XCB) ? cf_XCB : 0;
            flg2 = (flg1 & cf_XCA) ? cf_XCB : (flg1 & cf_XCB) ? cf_XCA : 0;
            rcpair.rowcol[1].set.flag[setval - 1] |= flg1;
            rcpair.rowcol[0].set.flag[setval - 1] |= flg2;
            pdst->insert(pdst->begin(),rcpair);
            sprintf(EndBuf(tb),"Insert %s%s%s ",
                Get_RC_setval_RC_Stg(prch1,setval),
                stglnk,
                Get_RC_setval_RC_Stg(prch2,setval) );
            // dst.push_back(rcpair);
            max = pdst->size();
        } else {
            strcat(tb, "NO ");
            pp2 = pplast;
            prc3 = &pp2->rowcol[0];
            prc4 = &pp2->rowcol[1]; // for first ONLY extension is to FIRST
            sprintf(EndBuf(tb),"ELast %s%s%s ",
                Get_RC_setval_RC_Stg(prc3,setval),
                stglnk,
                Get_RC_setval_RC_Stg(prc4,setval) );
            fnd = false;
            prch1 = prc1;
            prch2 = prc2;
            prch3 = prc3;
            prch4 = prc4;
            //if ((prc1->row == prc3->row)&&(prc1->col == prc3->col)) {
            //    fnd = true;
            //    prch1 = prc2;
            //    prch2 = prc1;
            //    prch3 = prc3;
            //    prch4 = prc4;
            //} else
            if ((prc1->row == prc4->row)&&(prc1->col == prc4->col)) {
                fnd = true;
                prch1 = prc2;
                prch2 = prc1;
                prch3 = prc4;
                prch4 = prc3;
            //} else if ((prc2->row == prc3->row)&&(prc2->col == prc3->col)) {
            //    fnd = true;
            //    prch1 = prc1;
            //    prch2 = prc2;
            //    prch3 = prc3;
            //    prch4 = prc4;
            } else if ((prc2->row == prc4->row)&&(prc2->col == prc4->col)) {
                fnd = true;
                prch1 = prc1;
                prch2 = prc2;
                prch3 = prc4;
                prch4 = prc3;
            }
            if (fnd) {
                // YOW, can EXTEND chain by this STRONG LINK
                count++;
                strcat(tb, "ok ");
                rcpair.rowcol[0] = *prch1;
                rcpair.rowcol[1] = *prch2;
                // 1 is being inserted after 4
                flg1 = prch4->set.flag[setval - 1];
                flg1 = (flg1 & cf_XCA) ? cf_XCA : (flg1 & cf_XCB) ? cf_XCB : 0;
                flg2 = (flg1 & cf_XCA) ? cf_XCB : (flg1 & cf_XCB) ? cf_XCA : 0;
                // so 1 takes same color as 4
                rcpair.rowcol[0].set.flag[setval - 1] |= flg1;
                // and 2 takes opposite
                rcpair.rowcol[1].set.flag[setval - 1] |= flg2;
                sprintf(EndBuf(tb),"Append %s%s%s ",
                    Get_RC_setval_RC_Stg(prch1,setval),
                    stglnk,
                    Get_RC_setval_RC_Stg(prch2,setval) );
                // dst.insert(dst.begin(),rcpair);
                pdst->push_back(rcpair);
                max = pdst->size();
            } else {
                strcat(tb,"NO ");
            }
        }
        OUTITSL(tb);
    }

    // NEXT STEP
    first = false;
    ppfirst = 0;
    pplast = 0;
    setval = last_setval;
    ch_num = last_ch_num;
    if (count && (last_setval |= -1) && (last_ch_num != -1)) {
        // we added EXTENSIONS - try to JOIN ENDS OF chain
        // ==============================================
        max = pdst->size();
        // Get FIRST and LAST of THIS LAST chain
        for (ii = 0; ii < max; ii++) {
            pp2 = &pdst->at(ii);
            prc3 = &pp2->rowcol[0];
            prc4 = &pp2->rowcol[1];
            if ( !(setval == prc3->set.uval) ) continue;
            if ( !(ch_num == prc3->set.tm) ) continue;
            if (first) {
                pplast = pp2;
            } else {
                ppfirst = pp2;
                first = true;
            }
        }
    }

    vRCP tmp;
    const char *lnktyp;
    if ( ppfirst && pplast ) { // got FIRST and LAST in CHAIN
        PROWCOL prcl1, prcl2, prcf1, prcf2;
        uint64_t flg;
        const char *clrtype1;
        const char *clrtype2;
        prcl1 = &pplast->rowcol[0];
        prcl2 = &pplast->rowcol[1];  // LAST on CHAIN
        prcf1 = &ppfirst->rowcol[0]; // FIRST on CHAIN
        prcf2 = &ppfirst->rowcol[1];
        // ==========================================
        // Show FIRST prcf1 and 2
        sprintf(tb,"ExFirst ");
        Append_RCPAIR_Settings(tb, ppfirst, setval );
        // Show LAST prcl1 and 2
        strcat(tb,"ExLast ");
        Append_RCPAIR_Settings(tb, pplast, setval );
        // =====================================================
        OUTITSL(tb);
        // collect ALL links, that join with FIRST of first, and LAST of last...
        // ExFirst 1@G6 ?==? 1@H4 ExLast 1@I2 ?==? 1@I9 
        max2 = src.size();
        prc3 = &ppfirst->rowcol[0];
        prc4 = &pplast->rowcol[1];
        sprintf(tb,"Conns: %s and %s ",
            Get_RC_setval_RC_Stg(prc3,setval),
            Get_RC_setval_RC_Stg(prc4,setval));
        for (jj = 0; jj < max2; jj++) {
            pp = &src[jj];
            if (prcp_in_pvrcp(pp,pdst)) continue; // already in chain
            prc1 = &pp->rowcol[0];
            if ( !(setval == prc1->set.uval) ) continue;
            if ( !(ch_num == prc1->set.tm) ) continue;
            prc2 = &pp->rowcol[1];
            // A possible LINK
            fnd = false;
            prch1 = prc1;
            prch2 = prc2;
            prch3 = prc3;
            prch4 = prc4;
            if ((prc1->row == prc3->row)&&(prc1->col == prc3->col)) {
                fnd = true;
                prch1 = prc1; // 1 con to 1
                prch2 = prc2; // 2 con to 2 
                prch3 = prc3; // 3 con to 1
                prch4 = prc4; // 4 con to 2
            } else if ((prc2->row == prc3->row)&&(prc2->col == prc3->col)) {
                fnd = true;
                prch1 = prc2; // 2 con to 1
                prch2 = prc1; // 1 con to 2 
                prch3 = prc4; // 4 con to 1
                prch4 = prc3; // 3 con to 2
            } else if ((prc1->row == prc4->row)&&(prc1->col == prc4->col)) {
                fnd = true;
                prch1 = prc1; // 1 con to 1
                prch2 = prc2; // 2 con to 2 
                prch3 = prc4; // 4 con to 1
                prch4 = prc3; // 3 con to 2
            } else if ((prc2->row == prc4->row)&&(prc2->col == prc4->col)) {
                fnd = true;
                prch1 = prc2; // 2 con to 1
                prch2 = prc1; // 1 con to 2 
                prch3 = prc3; // 3 con to 1
                prch4 = prc4; // 4 con to 2
            }
            if (fnd) {
                //tmp.push_back(*pp);
                rcpair = *pp;
                rcpair.rowcol[0] = *prch1;
                rcpair.rowcol[1] = *prch2;
                tmp.push_back(rcpair);
                flg1 = (rcpair.rowcol[0].set.flag[setval - 1] & cl_SLA);
                lnktyp = (flg1 & cl_SLA) ? stglnk : weklnk;
                sprintf(EndBuf(tb),"%s%s%s ",
                    Get_RC_setval_RC_Stg(prch1,setval),
                    lnktyp,
                    Get_RC_setval_RC_Stg(prch2,setval));
            } else {
                // strcat(tb,"no ");
            }
        }   // for (jj = 0; jj < max2; jj++) 
        OUTITSL(tb);
        max2 = tmp.size();
        fnd = false;
        for (ii = 0; ii < max2; ii++) {
            pp = &tmp[ii];
            prc1 = &pp->rowcol[0];
            prc2 = &pp->rowcol[1];
            for (jj = ii + 1; jj < max2; jj++) {
                pp2 = &tmp[jj];
                prc3 = &pp2->rowcol[0];
                prc4 = &pp2->rowcol[1];
                if (SAMERCCELL(prc2,prc4)) {
                    sprintf(tb,"Append ");
                    fnd = false;
                    if (SAMERCCELL(prc1,prcl2)) {
                        fnd = true;
                    } else if (SAMERCCELL(prc3,prcl2)) {
                        // BUT must SWITCH order of APPENDING
                        fnd = true;
                        PRCPAIR ptmp = pp;
                        pp = pp2;
                        pp2 = ptmp;
                        prc1 = &pp->rowcol[0];
                        prc2 = &pp->rowcol[1];
                        prc4 = &pp2->rowcol[0];
                        prc3 = &pp2->rowcol[1];
                    } else {
                        continue; // SHOULD NEVER BE HERE!!!
                    }
                    flg = prcl1->set.flag[setval - 1];
                    flg1 = (flg & cl_SLA); // isolate LINK type of PAIR
                    lnktyp = (flg1 & cl_SLA) ? stglnk : weklnk;
                    flg = (flg & (cf_XCA|cf_XCB)); // isolate COLOR flags
                    clrtype1 = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
                    flg = prcl2->set.flag[setval - 1];
                    flg = (flg & (cf_XCA|cf_XCB)); // isolate COLOR flags
                    clrtype2 = ((flg == (cf_XCA|cf_XCB)) ? "?" : (flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");
                    sprintf(EndBuf(tb),"to last %s%s%s%s%s ",
                        Get_RC_setval_RC_Stg(prcl1,setval), clrtype1,
                        lnktyp,
                        Get_RC_setval_RC_Stg(prcl2,setval), clrtype2 );

                    rcpair.rowcol[0] = *prc1;
                    rcpair.rowcol[1] = *prc2;
                    // is being inserted after last
                    flg = prcl2->set.flag[setval - 1]; // get LAST flag
                    flg1 = (flg & (cf_XCA|cf_XCB));    // isolate COLOR flags
                    flg2 = (flg1 & cf_XCA) ? cf_XCB : (flg1 & cf_XCB) ? cf_XCA : 0; // flip COLOR
                    // so 1 takes same color as 2 of last
                    rcpair.rowcol[0].set.flag[setval - 1] |= flg1;
                    // and 2 takes opposite
                    rcpair.rowcol[1].set.flag[setval - 1] |= flg2;
                    pdst->push_back(rcpair); // APPEND RCPAIR
                    // Check the SETTINGS of an RCPAIR per a setval
                    Append_RCPAIR_Settings( tb, &rcpair, setval );

                    // is being inserted after last pushed which is in rcpair
                    flg = rcpair.rowcol[1].set.flag[setval - 1];
                    flg1 = (flg & (cf_XCA|cf_XCB)); // isolate COLOR flags
                    flg2 = (flg1 & cf_XCA) ? cf_XCB : (flg1 & cf_XCB) ? cf_XCA : 0; // flip COLOR
                    rcpair.rowcol[0] = *prc3;
                    rcpair.rowcol[1] = *prc4;
                    rcpair.rowcol[0].set.flag[setval - 1] |= flg1;
                    // and 2 takes opposite
                    rcpair.rowcol[1].set.flag[setval - 1] |= flg2;
                    pdst->push_back(rcpair);  // append RCPAIR
                    // Check the SETTINGS of an RCPAIR per a setval
                    Append_RCPAIR_Settings( tb, &rcpair, setval );

                    OUTITSL(tb);
                    fnd = true;
                    count += 2; // bump chain count by 2 appended
                    closed = true;
                    break;
                }
            }
            if (fnd)
                break;
        }
    }
    // FINALLY seek any ELIMINATIONS
    // =============================
    max = pdst->size();
    if (g_bDoElims && count) { // && max && !(max & 1) && closed) {
        // got COUNT, and is EVEN and is CLOSED loop
        cnt = Check_XCyles_Elim( pb, pdst, setval, closed );
    }
    return count;
}

int Show_RCP_Pairs( vRCP *psrc, char *type )
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
    if (add_debug_sl) {
        OUTITSL(tb);
    } else if (add_debug_sl2) {
        OUTITSL2(tb);
    }

    return count;
}

// Used to process EXTENDED list
// Done ONLY IF
// if ( count && g_bIBL && g_bIWL && g_bASW ) {
int Transfer_Chain_Pairs( PABOX2 pb, vRCP *pdst, vRCP &src )
{
    int count = 0;
    RCPAIR rcpair, rcpair2, rcpairprev;
    uint64_t flg;
    size_t max, ii, i2;
    int setval;
    ROWCOL rc;
    bool change = false;
    const char *lnktype;
    const char *clrtype1;
    const char *clrtype2;
    uint64_t flag;
    time_t ch_num = -1;
    vRCP dst_copy;
    char *tb = GetNxtBuf();

    // COPY destination before CLEARLING
    // could work with pointer, and just change pointer - later
    max = pdst->size();
    for (ii = 0; ii < max; ii++)
        dst_copy.push_back(pdst->at(ii));
    pdst->clear();


    max = src.size();
    sprintf(tb,"Chain %d ", (int)max );
    flag = cf_XCA;
    for (ii = 0; ii < max; ii++) {
        // here should sort out which end joined to which, if NOT done before
        i2 = ii + 1;
        rcpair = src[ii];
        if (i2 < max) {
            rcpair2 = src[i2];
            change = false;
            // if the FIRST cell equals either of the 2 of the NEXT pair
            if ((rcpair.rowcol[0].row == rcpair2.rowcol[0].row)&&
                (rcpair.rowcol[0].col == rcpair2.rowcol[0].col)) {
                change = true; // need to SWITCH
            } else if ((rcpair.rowcol[0].row == rcpair2.rowcol[1].row)&&
                (rcpair.rowcol[0].col == rcpair2.rowcol[1].col)) {
                change = true; // need to SWITCH
            }
            if (change) {
                rc = rcpair.rowcol[0];
                rcpair.rowcol[0] = rcpair.rowcol[1];
                rcpair.rowcol[1] = rc;
            }
        } else if (max > 1) {
            rcpair2 = rcpairprev;
            // if the SECOND cell of this LAST equals either of the 2 of the PREV pair
            if ((rcpair.rowcol[1].row == rcpair2.rowcol[0].row)&&
                (rcpair.rowcol[1].col == rcpair2.rowcol[0].col)) {
                change = true; // need to SWITCH
            } else if ((rcpair.rowcol[1].row == rcpair2.rowcol[1].row)&&
                (rcpair.rowcol[1].col == rcpair2.rowcol[1].col)) {
                change = true; // need to SWITCH
            }
            if (change) {
                rc = rcpair.rowcol[0];
                rcpair.rowcol[0] = rcpair.rowcol[1];
                rcpair.rowcol[1] = rc;
            }
        }

        if (ch_num != rcpair.rowcol[0].set.tm) {
            ch_num = rcpair.rowcol[0].set.tm;
            flag = cf_XCA; // start with A color for each chain
        }

        setval = rcpair.rowcol[0].set.uval;
        rcpair.rowcol[0].set.flag[setval - 1] |= flag;  // set color first cell
        flag = (flag & cf_XCA) ? cf_XCB : cf_XCA;       // flip color
        rcpair.rowcol[1].set.flag[setval - 1] |= flag;  // set opposite color for next cell

        pdst->push_back(rcpair);
        count++;

        flg = rcpair.rowcol[0].set.flag[setval - 1];
        lnktype = ((flg & cl_SLA) ? stglnk : weklnk);
        clrtype1 = ((flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");

        flg = rcpair.rowcol[1].set.flag[setval - 1];
        clrtype2 = ((flg & cf_XCA) ? "A" : (flg & cf_XCB) ? "B" : "U");

        sprintf(EndBuf(tb),"%s%s%s%s%s ",
            Get_RC_setval_RC_Stg(&rcpair.rowcol[0],setval), clrtype1,
            lnktype,
            Get_RC_setval_RC_Stg(&rcpair.rowcol[1],setval), clrtype2 );

        rcpairprev = rcpair;
    }
    OUTITSL(tb);


    if (count && g_bExtChn) {
        int cnt = Extend_Chain_Pairs( pb, pdst, dst_copy );
        if (cnt) Show_RCP_Pairs( pdst, "ExChain" );
        count += cnt;
    }
    return count;
}

void Show_RC_SL_Chain(vRC * vprc, int setval)
{
    size_t ii;
    size_t max = vprc->size();
    PROWCOL prc;
    bool onoff;
    char *tb = GetNxtBuf();
    sprintf(tb,"Chain: ");
    onoff = true;
    for (ii = 0; ii < max; ii++) {
        prc = &vprc->at(ii);
        sprintf(EndBuf(tb),"%s", Get_RC_setval_RC_Stg(prc,setval));
        strcat(tb, (onoff ? "A" : "B"));
        onoff = !onoff;
        if ((ii + 1) < max)
            strcat(tb,"==");
    }
    if (!max)
        strcat(tb,"No chain formed!");
    OUTIT(tb);
}


// Have CHAINS based on ONE setval
// See if it finds any simple coloring ELIMINATIONS
// Like say
// 6@C2A==6@B3B==6@D3A==6@D6B eliminate 6@C6
// which is in SAME ROW as 6@C2A
// AND   is in SAME COL as 6@D6B
static vRC SL_chain;
static int SL_setval;
int Get_SL_Chain_setval() { return SL_setval; }
void Set_SL_invlaid() { SL_chain.clear(); SL_setval = 0; }
vRC *Get_SL_Chain()
{
    if (!VALUEVALID(SL_setval))
        return 0;
    if (SL_chain.size() < 4)
        return 0;
    return &SL_chain;
}

// got two PAIRS already MARKED with color and ch_num
// Find further LINKS to the link end or the two cont ends
int Mark_All_Pairs( PABOX2 pb, vRCP &vrcp, int setval,
            PROWCOL prclnk, PROWCOL prcont1, PROWCOL prcont2,
            time_t ch_num, uint64_t flag )
{
    int count = 0;
    size_t max;
    size_t ii;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    PROWCOL prcont;
    uint64_t flg;
    vRCP *pp = Get_SL_Pairs();
    max = pp->size();
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get OUT a pair
        if (prcp_in_vrcp(prcp1, vrcp)) continue; // already done this pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        prcont = 0;
        if (SAMERCCELL(prclnk,prc1)) {
            prcont = prc2;
            flg = prclnk->set.flag[setval - 1];
            prc1->set.flag[setval - 1] |= flg;
            flg = (flg == (cf_XCA|cf_XCB)) ? flg : (flg & cf_XCA) ? cf_XCB : cf_XCA;
            prc2->set.flag[setval - 1] |= flg;
            prc1->set.tm = ch_num;
            prc2->set.tm = ch_num;
            vrcp.push_back(*prcp1);
        } else if (SAMERCCELL(prclnk,prc2)) {
            prcont = prc1;
            flg = prclnk->set.flag[setval - 1];
            prc2->set.flag[setval - 1] |= flg;
            flg = (flg == (cf_XCA|cf_XCB)) ? flg : (flg & cf_XCA) ? cf_XCB : cf_XCA;
            prc1->set.flag[setval - 1] |= flg;
            prc1->set.tm = ch_num;
            prc2->set.tm = ch_num;
        }
    }
    return count;
}

// Simple idea - search the collected SL pairs for any end of the pair
// that matches this single CELL - reutnr COUNT of cells marked, 
// NOT already MARKED with a COLOR
// ==========================================================================================
int Mark_Links_to_This_Cell( PABOX2 pb, int setval, vRCP *plnks,
                             PRCPAIR prcp, PROWCOL prc, time_t ch_num, uint64_t flag )
{
    int count = 0;
    size_t max;
    size_t ii;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    vRCP *pp = Get_SL_Pairs();
    max = pp->size();
    char *tb = GetNxtBuf();
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get OUT a pair
        if (Same_RC_Pair(prcp1, prcp)) continue; // skip SELF
        //if (prcp_in_vrcp(prcp1, vrcp)) continue; // already done this pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        if (SAMERCCELL(prc1,prc)) {
            if ( !( prc1->set.flag[setval - 1] & cf_XAB ) ) {
                prc1->set.flag[setval - 1] |= flag;
                prc1->set.tm = ch_num;
                plnks->push_back(*prcp1); // pass back LINKS
                count++;
                sprintf(tb,"SL1 %s==%s", Get_RCPair_Stg(prcp,setval), Get_RCPair_Stg(prcp1,setval));
                OUTITSL2(tb);
            }
        } else if (SAMERCCELL(prc2,prc)) {
            if ( !( prc2->set.flag[setval - 1] & cf_XAB ) ) {
                prc2->set.flag[setval - 1] |= flag;
                prc2->set.tm = ch_num;
                plnks->push_back(*prcp1); // pass back LINKS
                count++;
                sprintf(tb,"SL2 %s==%s", Get_RCPair_Stg(prcp,setval), Get_RCPair_Stg(prcp1,setval));
                OUTITSL2(tb);
            }
        }
    }
    return count;
}


vRCP *Get_SL_Pairs_NO_DUPES()
{
    vRCP *pp = Get_SL_Pairs();
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    PRCPAIR prcp2;
    PROWCOL prc3, prc4;
    size_t max, ii, i2;
    max = pp->size();
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get PTR to a pair
        prc1 = &prcp1->rowcol[0];
        prc2 = &prcp1->rowcol[1];
        for (i2 = 0; i2 < max; i2++) {
            if (i2 == ii) continue;
            prcp2 = &pp->at(i2);
            prc3 = &prcp2->rowcol[0];
            prc4 = &prcp2->rowcol[1];
            if (( SAMERCCELL(prc1,prc3) && SAMERCCELL(prc2,prc4))||
                ( SAMERCCELL(prc2,prc3) && SAMERCCELL(prc1,prc4))) {
                // eliminate this DUPE
                pp->erase(pp->begin()+ii);
                max = pp->size();
                break;
            }
        }
    }
    return pp;
}

void Append_RCPairs_per_pvrcp(char *tb, int setval, vRCP *pvrcp)
{
    size_t max = pvrcp->size();
    size_t ii;
    PRCPAIR prcp;
    for (ii = 0; ii < max; ii++) {
        prcp = &pvrcp->at(ii);
        Append_RCPAIR_Settings(tb,prcp,setval);
        ADDSP(tb);
    }
}


int Mark_Other_Pairs_Linked_to_this_pair(PABOX2 pb, int setval, PROWCOL prcA, PROWCOL prcB,
                                         vRCP &vrcp, time_t ch_num, int lev)
{
    int count = 0;
    size_t max;
    size_t ii;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    uint64_t flag = 0;
    bool lnked;
    vRCP *pp = Get_SL_Pairs();
    max = pp->size();
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get OUT a pair
        if (prcp_in_vrcp(prcp1, vrcp)) continue; // already done this pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        lnked = false;
        if ( SAMERCCELL(prcA,prc1) && SAMERCCELL(prcB,prc2)) continue;
        if ( SAMERCCELL(prcB,prc1) && SAMERCCELL(prcA,prc2)) continue;

        if (SAMERCCELL(prcA,prc1)) {
            flag = prcA->set.flag[setval - 1];
            lnked = true;
        } else if (SAMERCCELL(prcB,prc1)) {
            flag = prcB->set.flag[setval - 1];
            lnked = true;
        } else if (SAMERCCELL(prcA,prc2)) {
            flag = prcA->set.flag[setval - 1];
            lnked = true;
        } else if (SAMERCCELL(prcB,prc2)) {
            flag = prcB->set.flag[setval - 1];
            lnked = true;
        }
        if (lnked) {
            flag = (flag & (cf_XCA|cf_XCB)); // isolate COLOR only
            // mark as A
            prc1->set.flag[setval - 1] |= flag;
            // Mark other as B
            flag = (flag == (cf_XCA|cf_XCB)) ? flag : (flag & cf_XCA) ? cf_XCB : cf_XCA;
            prc2->set.flag[setval - 1] |= flag;
            // set ALL with this chain number
            prc1->set.tm = ch_num;
            prc2->set.tm = ch_num;
            Mark_Other_Pairs_Linked_to_this_pair( pb, setval, prc1, prc2, vrcp, ch_num, lev+1 );
            if ((lev == 0) && !prcp_in_vrcp(prcp1, vrcp)) // already done this pair
                vrcp.push_back(*prcp1); // Have done this ONE
        }
    }
    return count;
}


#if 0 // 0000000000000000000000000000000000000000000000000000000000000000000000000
// if ((setvalcnt == 1) && count && g_bIBL && !g_bIWL && !g_bASW) {
// Is ONLY a list of STRONG LINKS for ONE setval
int Process_SL_Chains_NOT_USED(PABOX2 pb, int setval) // strong links for ONE setval
{
    int count = 0;
    size_t max;
    size_t ii, i2;
    PRCPAIR prcp1, prcp2, prcplnk;
    PROWCOL prc1, prc2;
    PROWCOL prc3, prc4;
    PROWCOL prclnk1,prclnk2,prcend,prcfirst;
    vRCP vrcp;
    int lnkcnt, cnt;
    int iflg;
    vRCP *pp = Get_SL_Pairs();
    time_t ch_num = 0;
    vRC *pvrcChain = &SL_chain;
    pvrcChain->clear();
    SL_setval = 0;
    max = pp->size();
    if (add_debug_xc) Show_RCP_Pairs( pp, "Strong Links");
    // First get this set of STRONG LINKS into CHAINS, if POSSIBLE
    for (ii = 0; ii < max; ii++) {
        prcp1 = &pp->at(ii); // get OUT a pair
        if (prcp_in_vrcp(prcp1, vrcp)) continue; // already done this pair
        prc1 = &prcp1->rowcol[0];
        if ( !(prc1->set.uval == setval) ) continue;
        prc2 = &prcp1->rowcol[1];
        lnkcnt = 0;
        iflg = 0;
        for (i2 = 0; i2 < max; i2++) {
            if (ii == i2) continue;
            prcp2 = &pp->at(i2); // get OUT a pair
            if (prcp_in_vrcp(prcp2, vrcp)) continue; // already done this pair
            prc3 = &prcp2->rowcol[0];
            if ( !(prc3->set.uval == setval) ) continue;
            prc4 = &prcp2->rowcol[1];
            if (SAMERCCELL(prc1,prc3)) {
                lnkcnt++;
                prclnk1 = prc1;
                prclnk2 = prc3;
                iflg |= 1;
                prcplnk = prcp2;
            } else if (SAMERCCELL(prc1,prc4)) {
                lnkcnt++;
                prclnk1 = prc1;
                prclnk2 = prc4;
                iflg |= 2;
                prcplnk = prcp2;
            } else if (SAMERCCELL(prc2,prc3)) {
                lnkcnt++;
                prclnk1 = prc2;
                prclnk2 = prc3;
                iflg |= 4;
                prcplnk = prcp2;
            } else if (SAMERCCELL(prc2,prc4)) {
                lnkcnt++;
                prclnk1 = prc2;
                prclnk2 = prc4;
                iflg |= 8;
                prcplnk = prcp2;
            }
        }
        if (lnkcnt == 1) {
            // found a STRONG link with ONLY one end joining another - ie the START of a CHAIN
            size_t curr = pvrcChain->size();
            ch_num++;
            prc1->set.tm = ch_num;
            prc2->set.tm = ch_num;
            if (prclnk1 == prc1) { // Get FIRST
                pvrcChain->push_back(*prc2);
                prcfirst = prc2;
            } else {
                pvrcChain->push_back(*prc1);
                prcfirst = prc1;
            }
            pvrcChain->push_back(*prclnk1); // got SECOND
            if (prclnk2 == prc3) { // get THIRD
                pvrcChain->push_back(*prc4);
                prcend = prc4;
            } else {
                pvrcChain->push_back(*prc3);
                prcend = prc3;
            }
            // started CHAIN with 3 members, pushed in link ORDER
            vrcp.push_back(*prcp1);
            vrcp.push_back(*prcplnk);
            // start again and get next LINK in the CHAIN
            cnt = 0;
            for (i2 = 0; i2 < max; i2++) {
                if (ii == i2) continue;
                prcp2 = &pp->at(i2); // get OUT a pair
                prc3 = &prcp2->rowcol[0];
                if ( !(prc3->set.uval == setval) ) continue;
                prc4 = &prcp2->rowcol[1];
                if (prcp_in_vrcp(prcp2, vrcp)) continue; // already done this pair
                if (SAMERCCELL(prcend,prc3)) {
                    prc4->set.tm = ch_num;
                    pvrcChain->push_back(*prc4);
                    prcend = prc4;
                    vrcp.push_back(*prcp2);
                    i2 = -1; // restart search for next in chain
                    cnt++;
                } else if (SAMERCCELL(prcend,prc4)) {
                    prc3->set.tm = ch_num;
                    pvrcChain->push_back(*prc3);
                    prcend = prc3;
                    vrcp.push_back(*prcp2);
                    i2 = -1; // restart search for next in chain
                    cnt++;
                }
            }
            if (!cnt) {
                // ONLY one PAIR not a CHAIN
                if (curr) 
                    pvrcChain->erase(pvrcChain->begin() + (curr-1),pvrcChain->end());
                else
                    pvrcChain->clear();

            }

        }
    }

    max = pvrcChain->size();
    if (max)
        Show_RC_SL_Chain( pvrcChain, setval);
    ROWCOL rc, rc2;
    int val;
    PSET ps;
    int elims = 0;
    if ((max >= 4) && !(max & 1)) {
        prcfirst = &pvrcChain->at(0);
        prcend   = &pvrcChain->at(max - 1);
        rc.row = prcfirst->row;
        rc.col = prcend->col;
        rc2.row = prcend->row;
        rc2.col = prcfirst->col;
        //if (!Row_Col_in_RC_Vector( vrc, rc.row, rc.col )) {
        if (!prc_in_chain( &rc, pvrcChain)) {
            val = pb->line[rc.row].val[rc.col];
            if (!val) {
                ps = &pb->line[rc.row].set[rc.col];
                if ( ps->val[setval - 1] ) {
                    ps->flag[setval - 1] |= cf_CCE;
                    elims++;
                }
            }
        }
        if (!prc_in_chain( &rc2, pvrcChain)) {
            val = pb->line[rc2.row].val[rc2.col];
            if (!val) {
                ps = &pb->line[rc2.row].set[rc2.col];
                if ( ps->val[setval - 1] ) {
                    ps->flag[setval - 1] |= cf_CCE;
                    elims++;
                }
            }
        }
        if (elims) {
            rc = *prcfirst;
            pb->line[rc.row].set[rc.col].cellflg |= cf_XCA;
            rc = *prcend;
            pb->line[rc.row].set[rc.col].cellflg |= cf_XCB;
            for (ii = 0; ii < max; ii++) {
                prc1 = &pvrcChain->at(ii);
                rc = *prc1;
                pb->line[rc.row].set[rc.col].flag[setval - 1] |= cf_CC;
            }
        }
    }
    //if (!elims) {
    //    // NO ELIMS from CHAINING, process just SL pairs
    //    elims = Process_SL_Pairs3(pb, setval, pp);
    //}
    return count;
}

#endif // if 0 // 0000000000000000000000000000000000000000000000000000000000000000000000000

// Used to process EXTENDED list
// if ( count && g_bIBL && g_bIWL && g_bASW ) {
int Process_SW_Chains_OK(PABOX2 pb)
{
    int count = 0;
    size_t max;
    size_t ii, i2;
    vRCP sla;   // Alternating S->W->S->... CHAIN
    vRCP wla;
    PRCPAIR prcp1;
    PROWCOL prc1, prc2;
    int setval, lval;
    bool findweak;
    BOOL change;
    time_t ch_num = 1;
    ROWCOL rc;
    RCPAIR rcpair;
    vRCP *pp = Get_SL_Pairs();
    max = pp->size();
    count = (int)max;
    for (lval = 0; lval < 9; lval++) {
        setval = lval + 1;
        ch_num = 1;
        for (ii = 0; ii < max; ii++) {
            prcp1 = &pp->at(ii); // get OUT a pair
            prc1 = &prcp1->rowcol[0];
            if ( !(prc1->set.uval == setval) ) continue;
            prc2 = &prcp1->rowcol[1];
            if ( !((prc1->set.flag[lval] & cl_SLA)&&(prc2->set.flag[lval] & cl_SLA)) ) continue;
            if (prcp_in_vrcp(prcp1,sla))
                continue; // aleady in chain

            // GOT A NEW STRONG LINK - Join chain, if any
            rcpair = *prcp1;
            rcpair.rowcol[0].set.tm = ch_num; // CHAIN number - only works if ONLY searching chain
            rcpair.rowcol[1].set.tm = ch_num; // for SINGLE setval
            rcpair.rowcol[0].set.flag[lval] &= ~cf_XCB; // remove B
            rcpair.rowcol[0].set.flag[lval] |= cf_XCA;  // add A color
            rcpair.rowcol[1].set.flag[lval] &= ~cf_XCA; // remove A
            rcpair.rowcol[1].set.flag[lval] |= cf_XCB;  // add B color
            i2 = sla.size();    // get OFFSET BEFORE push
            sla.push_back(rcpair); // push onto vector

            findweak = true;
            wla.clear();
            change = FALSE;
            Find_Next_Link_OK( pb, sla, wla, setval, prc1, prc2, findweak, &change, ch_num );
            if (wla.size() == 0) {
                sla.pop_back();
                continue;
            }
            if (change) {
                rcpair = *prcp1;
                rc = rcpair.rowcol[0];
                rcpair.rowcol[0] = rcpair.rowcol[1];
                rcpair.rowcol[1] = rc;
                sla[i2] = rcpair; // CHANGE this offset around
            }
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
                    change = false;
                    Find_Next_Link_OK( pb, sla, wla2, setval, prc1, prc2, findweak, &change, ch_num );
                }
                wla.clear();
                max2 = wla2.size();
                for (jj = 0; jj < max2; jj++) {
                    wla.push_back(wla2[jj]);
                }
            }
            ch_num++;
        }
    }
    max = sla.size();
    if (max) {
        // if we FOUND alternating then pass back that
        count = Transfer_Chain_Pairs( pb, pp, sla );
    }
    return count;
}

// Accumulate the LINKED pairs
// Depending on BOOL option whether BOX links included - g_bIBL
// and depends on BOOL whether WEAK links also included - g_bIWL
int Get_Links_for_setval( PABOX2 pb, int setval, vRCP *psl_pairs )
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
    act_setval2[lval] = setval;  // MARK THIS VALID
    char *tb = GetNxtBuf();
    sprintf(tb,"Get Links: Inc.Box %s, Inc.Weak %s, Alt.WS %s",
        g_bIBL ? "On" : "Off",
        g_bIWL ? "On" : "Off",
        g_bASW ? "On" : "Off" );
    OUTITSL(tb);
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
                        if (add_sl_boxes) { // = g_bIBL
                            if (box2 == box) {
                                setcnt = Count_setval_in_Box(pb, row, col, setval );
                                if (setcnt == 2) flag |= cl_SLB;
                            } else continue; // or SAME BOX
                        } else continue; // seeking ONLY same COL or ROW
                    }

                    // found 2nd with value
                    if (setcnt != 2) {
                        if (!add_weak_links) {  // g_bIWL
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

                    sprintf(tb,"Link %s%s%s ",
                        Get_RC_setval_RC_Stg(&rc1,setval),
                        ((flag & cl_SLA) ? stglnk : weklnk),
                        Get_RC_setval_RC_Stg(&rc2,setval) );
                    //sprintf(EndBuf(tb),"(%s) ", Get_RCPAIR_Stg(&rcpair,setval));
                    if (add_debug_sl2) {
                        OUTITSL2(tb);
                    } else {
                        OUTITSL(tb);
                    }
                }   // for (col2 = fcol; col2 < 9; col2++)
                fcol = 0;
            }   // for (row2 = row; row2 < 9; row2++) 
        }   // for (col = 0; col < 9; col++)
    }   // for (row = 0; row < 9; row++)
    count = (int)psl_pairs->size();
    if (count) {
        sl_pairs_valid2 = true;
    }
    return count;
}

int Do_Clear_XCycle(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_XCE, cf_XC, "XC", "X-Cycles", 0, false );
    return count;
}

int Do_Fill_XCycles(PABOX2 pb)
{
    int count = Do_Fill_By_Flags( pb, cf_XCE, cf_XC, "XC", "X-Cycles", 0, true );
    return count;
}


int Mark_Links( PABOX2 pb ) // Diagnostic at this stage - only called by case  case ID_VIEW_LINKS:
{
    int count = 0;
    int setvalcnt = 0;
    int i, val, len, c;
    int setval = 0;
    char *ps = g_szCandList;
    char *tb = GetNxtBuf();
    vRCP *psl_pairs = Get_SL_Pairs();
    for (i = 0; i < 9; i++)
        act_setval2[i] = 0;
    len = (int)strlen(ps);
    for (i = 0; i < len; i++) {
        c = ps[i];
        if ( !ISDIGIT(c) ) continue;
        val = c - '0';
        if ( !VALUEVALID(val) ) continue;
        act_setval2[val - 1] = val;
        setvalcnt++;
        setval = val;
    }
    if (!setvalcnt) {
        sprtf("NO setval in list!\n");
        return 0;
    }
    if ( (setvalcnt == 1) && g_bIBL && g_bIWL && g_bASW ) {
        g_bDoElims = TRUE;
    }

    count = 0;
    sl_pairs_valid2 = false;
    psl_pairs->clear();
    sl_eliminations2 = 0;
    for (i = 0; i < 9; i++) {
        val = act_setval2[i];
        if (!val) continue;
        count += Get_Links_for_setval( pb, val, psl_pairs );
    }

    if (count && sl_pairs_valid2) {
        len = 0;
        *tb = 0;
        for (i = 0; i < 9; i++) {
            val = act_setval2[i];
            if (!val) continue;
            sprintf(EndBuf(tb),"%d",val);
            len++;
        }
        sprtf("Viewing %d LINKS for setval%s %s.\n", 
            count, ((len == 1) ? "" : "s"), tb);
    } else {
        sl_pairs_valid2 = false;
        sprtf("NO STRONG LINKS for setval(s) %s.\n", tb);
    }

    if ( count && g_bIBL && g_bIWL && g_bASW ) {
        int cnt = Process_SW_Chains_OK(pb);
        if (cnt != count) {
            len = 0;
            *tb = 0;
            for (i = 0; i < 9; i++) {
                val = act_setval2[i];
                if (!VALUEVALID(val)) continue;
                sprintf(EndBuf(tb),"%d",val);
                len++;
            }
            sprtf("Viewing %d Ext.LINKS for setval%s %s.\n",
                cnt, ((len == 1) ? "" : "s"), tb );
            if (sl_eliminations2) {
                Stack_Fill(Do_Clear_XCycle);
            }
        }
        count = cnt;
    }
    if ((setvalcnt == 1) && count && g_bIBL && !g_bIWL && !g_bASW) {
        // We ONLY have STRONG links, including in Boxes
        Mark_Strong_Links_Pairs( pb, setval, psl_pairs );

    }

    return count;
}



// eof - Sudo_XCycles.cxx
