// Sudo_NPairs.cxx

#include "Sudoku.hxx"

#define OUTITNP(a) if (add_debug_np) OUTIT(a)

////////////////////////////////////////////////////////////////////////
// Maked Pairs, Triple, Quads

int Mark_inside_Box4( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4, PSET ps_in, uint64_t flag,
            vRC *pvrc = 0);

int Mark_inside_Box4( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4, PSET ps_in, uint64_t flag,
            vRC *pvrc )
{
    int count = 0;
    int box = GETBOX(prc1->row,prc1->col);
    if ( !((box == GETBOX(prc2->row,prc2->col)&&
        (box == GETBOX(prc3->row,prc3->col))))&&
        (box == GETBOX(prc4->row,prc4->col))) {
        sprtf("CRIKEY: Mark_inside_Box: NOT SAME BOX! %s %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3),
            Get_RC_RCB_Stg(prc4));
        return 0;
    }
    int r = (prc1->row / 3) * 3;
    int c = (prc1->col / 3) * 3;
    int row, col, rw, cl, val, setval, i;
    PSET ps;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((row == prc1->row)&&(col == prc1->col)) continue;
            if ((row == prc2->row)&&(col == prc2->col)) continue;
            if ((row == prc3->row)&&(col == prc3->col)) continue;
            if ((row == prc4->row)&&(col == prc4->col)) continue;
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            for (i = 0; i < 9; i++) {
                setval = ps_in->val[i];
                if (!setval) continue;
                if ( !(ps->val[setval - 1]) ) continue;
                if ( ps->flag[setval - 1] & flag ) continue;
                ps->flag[setval - 1] |= flag;
                count++;
                if (pvrc) {
                    ROWCOL rc;
                    rc.row = row;
                    rc.col = col;
                    COPY_SET(&rc.set,ps);
                    pvrc->push_back(rc);
                }
            }
        }
    }
    return count;
}

int Mark_RC_with_Flag( PABOX2 pb, PROWCOL prc, uint64_t flag )
{
    int count = 0;
    int val = pb->line[prc->row].val[prc->col];
    if (val) {
        sprtf("UPHF: Mark_RC_with_Flag: %s HAS value %d\n", Get_RC_RCB_Stg(prc), val);
        return 0;
    }
    PSET ps = &pb->line[prc->row].set[prc->col];
    int i;
    for (i = 0; i < 9; i++) {
        val = ps->val[i];
        if (!val) continue; // No cand here
        if (ps->flag[i] & flag) continue; // Already MARKED
        ps->flag[i] |= flag;
        count++;
    }
    return count;
}

int Mark_Row_Only4( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4, PSET ps_in, uint64_t flag,
    vRC *pvrc = 0 );
int Mark_Row_Only4( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4, PSET ps_in, uint64_t flag,
    vRC *pvrc)
{
    int count = 0;
    int row = prc1->row;
    if ( !((row == prc2->row) && (row == prc3->row)  && (row == prc4->row)) ) {
        sprtf("CRIKEY: Mark_Row_Only: NOT SAME ROW! %s %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3),
            Get_RC_RCB_Stg(prc4) );
        return 0;
    }
    int col, val, setval, i;
    PSET ps;
    for (col = 0; col < 9; col++) {
        if (col == prc1->col) continue;
        if (col == prc2->col) continue;
        if (col == prc3->col) continue;
        if (col == prc4->col) continue;
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        for (i = 0; i < 9; i++) {
            setval = ps_in->val[i];
            if (!VALUEVALID(setval)) continue;
            if ( !(ps->val[setval - 1]) ) continue;
            if ( ps->flag[setval - 1] & flag ) continue;
            ps->flag[setval - 1] |= flag;
            count++;
            if (pvrc) {
                ROWCOL rc;
                rc.row = row;
                rc.col = col;
                rc.box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.cnum = setval;
                pvrc->push_back(rc);
            }
        }
    }
    return count;
}

int Mark_Col_Only4( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4, PSET ps_in, uint64_t flag,
    vRC *pvrc = 0 );

int Mark_Col_Only4( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PROWCOL prc4, PSET ps_in, uint64_t flag,
    vRC *pvrc)
{
    int count = 0;
    int col = prc1->col;
    if ( !((col == prc2->col) && (col == prc3->col) && (col == prc4->col)) ) {
        sprtf("CRIKEY: Mark_Row_Only: NOT SAME COL! %s %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3),
            Get_RC_RCB_Stg(prc4) );
        return 0;
    }
    int row, val, setval, i;
    PSET ps;
    for (row = 0; row < 9; row++) {
        if (row == prc1->row) continue;
        if (row == prc2->row) continue;
        if (row == prc3->row) continue;
        if (row == prc4->row) continue;
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        for (i = 0; i < 9; i++) {
            setval = ps_in->val[i];
            if (!VALUEVALID(setval)) continue;
            if ( !(ps->val[setval - 1]) ) continue;
            if ( ps->flag[setval - 1] & flag ) continue;
            ps->flag[setval - 1] |= flag;
            count++;
            if (pvrc) {
                ROWCOL rc;
                rc.row = row;
                rc.col = col;
                rc.box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.cnum = setval;
                pvrc->push_back(rc);
            }
        }
    }
    return count;
}

int Mark_Row_outside_Box( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc = 0 );

int Mark_Row_outside_Box( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc )
{
    int count = 0;
    int row = prc1->row;
    if ( !((row == prc2->row) && (row == prc3->row)) ) {
        sprtf("CRIKEY: Mark_Row_outside_Box: NOT SAME ROW! %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3) );
        return 0;
    }
    int box = GETBOX(prc1->row,prc1->col);
    if ( !((box == GETBOX(prc2->row,prc2->col)&&
        (box == GETBOX(prc3->row,prc3->col)))) ) {
        sprtf("CRIKEY: Mark_Row_outside_Box: NOT SAME BOX! %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3) );
        return 0;
    }
    int col, box2, val, i, setval;
    PSET ps;
    for (col = 0; col < 9; col++) {
        if (col == prc1->col) continue;
        if (col == prc2->col) continue;
        if (col == prc3->col) continue;
        box2 = GETBOX(row,col);
        if (box2 == box) continue;
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        for (i = 0; i < 9; i++) {
            setval = ps_in->val[i];
            if (!VALUEVALID(setval)) continue;
            if ( !(ps->val[setval - 1]) ) continue;
            if ( ps->flag[setval - 1] & flag ) continue;
            ps->flag[setval - 1] |= flag;
            count++;
            if (pvrc) {
                ROWCOL rc;
                rc.row = row;
                rc.col = col;
                rc.box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.cnum = setval;
                pvrc->push_back(rc);
            }
        }
    }
    return count;
}

int Mark_Col_outside_Box( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc = 0 );


int Mark_Col_outside_Box( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc )
{
    int count = 0;
    int col = prc1->col;
    if ( !((col == prc2->col) && (col == prc3->col)) ) {
        sprtf("CRIKEY: Mark_Col_outside_Box: NOT SAME COL! %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3) );
        return 0;
    }
    int box = GETBOX(prc1->row,prc1->col);
    if ( !((box == GETBOX(prc2->row,prc2->col)&&
        (box == GETBOX(prc3->row,prc3->col)))) ) {
        sprtf("CRIKEY: Mark_Col_outside_Box: NOT SAME BOX! %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3) );
        return 0;
    }
    int row, box2, val, i, setval;
    PSET ps;
    for (row = 0; row < 9; row++) {
        if (row == prc1->row) continue;
        if (row == prc2->row) continue;
        if (row == prc3->row) continue;
        box2 = GETBOX(row,col);
        if (box2 == box) continue;
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        for (i = 0; i < 9; i++) {
            setval = ps_in->val[i];
            if (!VALUEVALID(setval)) continue;
            if ( !(ps->val[setval - 1]) ) continue;
            if ( ps->flag[setval - 1] & flag ) continue;
            ps->flag[setval - 1] |= flag;
            count++;
            if (pvrc) {
                ROWCOL rc;
                rc.row = row;
                rc.col = col;
                rc.box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.cnum = setval;
                pvrc->push_back(rc);
            }
        }
    }
    return count;
}

int Mark_inside_Box( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc = 0 );


int Mark_inside_Box( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc )
{
    int count = 0;
    int box = GETBOX(prc1->row,prc1->col);
    if ( !((box == GETBOX(prc2->row,prc2->col)&&
        (box == GETBOX(prc3->row,prc3->col)))) ) {
        sprtf("CRIKEY: Mark_inside_Box: NOT SAME BOX! %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3) );
        return 0;
    }
    int r = (prc1->row / 3) * 3;
    int c = (prc1->col / 3) * 3;
    int row, col, rw, cl, val, setval, i;
    PSET ps;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((row == prc1->row)&&(col == prc1->col)) continue;
            if ((row == prc2->row)&&(col == prc2->col)) continue;
            if ((row == prc3->row)&&(col == prc3->col)) continue;
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            for (i = 0; i < 9; i++) {
                setval = ps_in->val[i];
                if (!VALUEVALID(setval)) continue;
                if ( !(ps->val[setval - 1]) ) continue;
                if ( ps->flag[setval - 1] & flag ) continue;
                ps->flag[setval - 1] |= flag;
                count++;
                if (pvrc) {
                    ROWCOL rc;
                    rc.row = row;
                    rc.col = col;
                    rc.box = GETBOX(row,col);
                    COPY_SET(&rc.set,ps);
                    rc.cnum = setval;
                    pvrc->push_back(rc);
                }
            }
        }
    }
    return count;
}

int Mark_Row_Only( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc = 0 );

int Mark_Row_Only( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc )
{
    int count = 0;
    int row = prc1->row;
    if ( !((row == prc2->row) && (row == prc3->row)) ) {
        sprtf("CRIKEY: Mark_Row_Only: NOT SAME ROW! %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3) );
        return 0;
    }
    int col, val, setval, i;
    PSET ps;
    for (col = 0; col < 9; col++) {
        if (col == prc1->col) continue;
        if (col == prc2->col) continue;
        if (col == prc3->col) continue;
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        for (i = 0; i < 9; i++) {
            setval = ps_in->val[i];
            if (!VALUEVALID(setval)) continue;
            if ( !(ps->val[setval - 1]) ) continue;
            if ( ps->flag[setval - 1] & flag ) continue;
            ps->flag[setval - 1] |= flag;
            count++;
            if (pvrc) {
                ROWCOL rc;
                rc.row = row;
                rc.col = col;
                rc.box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.cnum = setval;
                pvrc->push_back(rc);
            }
        }
    }
    return count;
}

int Mark_Col_Only( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc = 0 );

int Mark_Col_Only( PABOX2 pb, PROWCOL prc1, PROWCOL prc2, PROWCOL prc3, PSET ps_in, uint64_t flag,
    vRC *pvrc )
{
    int count = 0;
    int col = prc1->col;
    if ( !((col == prc2->col) && (col == prc3->col)) ) {
        sprtf("CRIKEY: Mark_Row_Only: NOT SAME COL! %s %s %s\n",
            Get_RC_RCB_Stg(prc1),
            Get_RC_RCB_Stg(prc2),
            Get_RC_RCB_Stg(prc3) );
        return 0;
    }
    int row, val, setval, i;
    PSET ps;
    for (row = 0; row < 9; row++) {
        if (row == prc1->row) continue;
        if (row == prc2->row) continue;
        if (row == prc3->row) continue;
        val = pb->line[row].val[col];
        if (val) continue;
        ps = &pb->line[row].set[col];
        for (i = 0; i < 9; i++) {
            setval = ps_in->val[i];
            if (!VALUEVALID(setval)) continue;
            if ( !(ps->val[setval - 1]) ) continue;
            if ( ps->flag[setval - 1] & flag ) continue;
            ps->flag[setval - 1] |= flag;
            count++;
            if (pvrc) {
                ROWCOL rc;
                rc.row = row;
                rc.col = col;
                rc.box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.cnum = setval;
                pvrc->push_back(rc);
            }
        }
    }
    return count;
}



// Find within a ROW COL or BOX 4 candidates shared
// ------------------------------------------------
// Just like Naked Triplets, any group of FOUR cells in the same unit that contain 
// IN TOTAL FOUR candidates is a Naked Quad. 
// Each cell can have two three or four numbers, as long as in 
// combination all four cells have only four numbers. 
// When this happens, the four candidates can be removed 
// from all other cells in the same unit - ROW, COL or BOX.
int only_one_quad = 1;
int Do_Naked_Quad_Scan( PABOX2 pb )
{
    int count = 0;
    RCRCB rcrcb;
    vRC *pvrc;
    int cnt;
    vRC empty, vdone;
    PRCRCB prcrcb = &rcrcb;
    int i, s;
    size_t max, j, k, l, m, me;
    ROWCOL rc, rc2, rc3, rc4;
    PROWCOL prc, prc2, prc3, prc4;
    int row, col, box;
    int row2, col2, box2;
    int row3, col3, box3;
    int row4, col4, box4;
    int scnt, scnt2, scnt3, scnt4;
    int setvals[9];
    int setvals2[9];
    int setvals3[9];
    int setvals4[9];
    PSET ps, ps2, ps3, ps4;
    SET set;
    int val;
    vRC vrc, elims, elimby;
    char *tb = GetNxtBuf();
    cnt = Get_Empty_Cells( pb, empty, prcrcb ); // get ALL empty cells
    for (i = 0; i < 9; i++) { // test ALL 9 boxes for 'Naked Quads'
        pvrc = &prcrcb->vboxs[i];   // get rc set for each box
        max = pvrc->size();
        if (max < 2) continue;  // must have 2 or more members
        for (j = 0; j < max; j++) {
            prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
            rc = *prc;
            row = prc->row;
            col = prc->col;
            box = GETBOX(row,col);
            if ( Row_Col_in_RC_Vector( vdone, row, col ) ) continue;
            vdone.push_back(rc);    // well it is NOW done
            ps = &prc->set; // pointer to list of candidates
            scnt = Get_Set_Cnt2(ps,setvals);
            if ( (scnt < 2) || (scnt > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
            for (k = j + 1; k < max; k++) {
                if (k == j) continue;
                prc2 = &pvrc->at(k);
                rc2 = *prc2;
                row2 = rc2.row;
                col2 = rc2.col;
                box2 = GETBOX(row2,col2);
                if ( Row_Col_in_RC_Vector( vdone, row2, col2 ) ) continue;
                ps2 = &rc2.set;
                scnt2 = Get_Set_Cnt2(ps2,setvals2);
                if ( (scnt2 < 2) || (scnt2 > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                for (l = k + 1; l < max; l++) {
                    if (l == j) continue;
                    if (l == k) continue;
                    prc3 = &pvrc->at(l);
                    rc3 = *prc3;
                    row3 = rc3.row;
                    col3 = rc3.col;
                    box3 = GETBOX(row3,col3);
                    if ( Row_Col_in_RC_Vector( vdone, row3, col3 ) ) continue;
                    ps3 = &rc3.set;
                    scnt3 = Get_Set_Cnt2(ps3,setvals3);
                    if ( (scnt3 < 2) || (scnt3 > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                    for (m = l + 1; m < max; m++) {
                        if (m == l) continue;
                        if (m == j) continue;
                        if (m == k) continue;
                        prc4 = &pvrc->at(m);
                        rc4 = *prc4;
                        row4 = rc4.row;
                        col4 = rc4.col;
                        box4 = GETBOX(row4,col4);
                        if ( Row_Col_in_RC_Vector( vdone, row4, col4 ) ) continue;
                        ps4 = &rc4.set;
                        scnt4 = Get_Set_Cnt2(ps4,setvals4);
                        if ( (scnt4 < 2) || (scnt4 > 3) ) continue; // seeking ONLY 2 3 or 4 candidates

                        // Now do they share just 4 values
                        Zero_SET(&set);
                        for (s = 0; s < scnt; s++) {
                            val = setvals[s];
                            set.val[val - 1] = val;
                        }
                        for (s = 0; s < scnt2; s++) {
                            val = setvals2[s];
                            set.val[val - 1] = val;
                        }
                        for (s = 0; s < scnt3; s++) {
                            val = setvals3[s];
                            set.val[val - 1] = val;
                        }
                        for (s = 0; s < scnt4; s++) {
                            val = setvals4[s];
                            set.val[val - 1] = val;
                        }
                        sprintf(tb,"Nak.Quad: %s %s %s %s cnt {%d,%d,%d,%d} shares ",
                            Get_RC_RCBS_Stg(&rc), 
                            Get_RC_RCBS_Stg(&rc2),
                            Get_RC_RCBS_Stg(&rc3),
                            Get_RC_RCBS_Stg(&rc4),
                            scnt, scnt2, scnt3, scnt4);
                            AddSet2Buf(tb,&set);
                        if (Get_Set_Cnt(&set) != 4) continue;
                        int ecnt = 0;
                        //if (row == row2) {
                        //    // TODO - elim cands from this COL, outside Box
                        //    ecnt += Mark_Row_outside_Box4( pb, &rc, &rc2, &rc3, rc4, &set, cf_NTE );
                        //} else if (col == col2) {
                        //    // TODO - elim cands from this COL, outside Box
                        //    ecnt += Mark_Col_outside_Box4( pb, &rc, &rc2, &rc3, &rc4, &set, cf_NTE );
                        //}
                        vrc.clear();
                        ecnt += Mark_inside_Box4( pb, &rc, &rc2, &rc3, &rc4, &set, cf_NTE, &vrc );
                        if (ecnt) {
                            sprintf(tb,"Nak.Quad %s %s %s %s shares ",
                                Get_RC_setval_RC_Stg(&rc), 
                                Get_RC_setval_RC_Stg(&rc2),
                                Get_RC_setval_RC_Stg(&rc3),
                                Get_RC_setval_RC_Stg(&rc4) );
                                AddSet2Buf(tb,&set);
                            sprintf(EndBuf(tb),", used to elim %d in ", ecnt);
                            for (me = 0; me < vrc.size(); me++) {
                                sprintf(EndBuf(tb),"%s ", Get_RC_RC_Stg(&vrc[me]));
                                elims.push_back(vrc[me]);
                            }
                            OUTITNP(tb);
                            Mark_RC_with_Flag( pb, &rc, cf_NT );
                            Mark_RC_with_Flag( pb, &rc2, cf_NT );
                            Mark_RC_with_Flag( pb, &rc3, cf_NT );
                            Mark_RC_with_Flag( pb, &rc4, cf_NT );
                            elimby.push_back(rc);
                            elimby.push_back(rc2);
                            elimby.push_back(rc3);
                            elimby.push_back(rc4);
                            count += ecnt;
                        }
                        vdone.push_back(rc2);    // well it is NOW done
                        vdone.push_back(rc3);    // well it is NOW done
                        vdone.push_back(rc4);
                        if (count && only_one_quad)
                            break;
                    }   // for (m = l + 1; m < max; m++)
                    if (count && only_one_quad)
                       break;
                }   // for (l = k + 1; l < max; l++)
                if (count && only_one_quad)
                   break;
            }   // for (k = j + 1; k < max; k++)
            if (count && only_one_quad)
               break;
        }   // for (j = 0; j < max; j++)
        if (count && only_one_quad)
            break;
    }   // for (i = 0; i < 9; i++) 

    if (!count) {
        // time to scan the ROWS
        vdone.clear();
        for (i = 0; i < 9; i++) { // test ALL 9 rows for 'Naked Quads'
            pvrc = &prcrcb->vrows[i];   // get rc set for each box
            max = pvrc->size();
            if (max < 2) continue;  // must have 2 or more members
            for (j = 0; j < max; j++) {
                prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
                rc = *prc;
                row = prc->row;
                col = prc->col;
                box = GETBOX(row,col);
                if ( Row_Col_in_RC_Vector( vdone, row, col ) ) continue;
                vdone.push_back(rc);    // well it is NOW done
                ps = &prc->set; // pointer to list of candidates
                scnt = Get_Set_Cnt2(ps,setvals);
                if ( (scnt < 2) || (scnt > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                for (k = 0; k < max; k++) {
                    if (k == j) continue;
                    prc2 = &pvrc->at(k);
                    rc2 = *prc2;
                    row2 = rc2.row;
                    col2 = rc2.col;
                    box2 = GETBOX(row2,col2);
                    if ( Row_Col_in_RC_Vector( vdone, row2, col2 ) ) continue;
                    ps2 = &rc2.set;
                    scnt2 = Get_Set_Cnt2(ps2,setvals2);
                    if ( (scnt2 < 2) || (scnt2 > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                    for (l = 0; l < max; l++) {
                        if (l == j) continue;
                        if (l == k) continue;
                        prc3 = &pvrc->at(l);
                        rc3 = *prc3;
                        row3 = rc3.row;
                        col3 = rc3.col;
                        box3 = GETBOX(row3,col3);
                        if ( Row_Col_in_RC_Vector( vdone, row3, col3 ) ) continue;
                        ps3 = &rc3.set;
                        scnt3 = Get_Set_Cnt2(ps3,setvals3);
                        if ( !((scnt3 == 2) || (scnt3 == 3)) ) continue; // seeking ONLY 2 3 or 4 candidates
                        for (m = 0; m < max; m++) {
                            if (m == l) continue;
                            if (m == j) continue;
                            if (m == k) continue;
                            prc4 = &pvrc->at(m);
                            rc4 = *prc4;
                            row4 = rc4.row;
                            col4 = rc4.col;
                            box4 = GETBOX(row4,col4);
                            if ( Row_Col_in_RC_Vector( vdone, row4, col4 ) ) continue;
                            ps4 = &rc4.set;
                            scnt4 = Get_Set_Cnt2(ps4,setvals4);
                            if ( (scnt4 < 2) || (scnt4 > 4) ) continue; // seeking ONLY 2 3 or 4 candidates

                            // Now do they share just 4 values
                            Zero_SET(&set);
                            for (s = 0; s < scnt; s++) {
                                val = setvals[s];
                                set.val[val - 1] = val;
                            }
                            for (s = 0; s < scnt2; s++) {
                                val = setvals2[s];
                                set.val[val - 1] = val;
                            }
                            for (s = 0; s < scnt3; s++) {
                                val = setvals3[s];
                                set.val[val - 1] = val;
                            }
                            for (s = 0; s < scnt4; s++) {
                                val = setvals4[s];
                                set.val[val - 1] = val;
                            }
                            sprintf(tb,"Cells %s %s %s %s {%d,%d,%d,%d} shares ",
                                Get_RC_RCBS_Stg(&rc), 
                                Get_RC_RCBS_Stg(&rc2),
                                Get_RC_RCBS_Stg(&rc3),
                                Get_RC_RCBS_Stg(&rc4),
                                scnt, scnt2, scnt3, scnt4);
                                AddSet2Buf(tb,&set);
                            if (Get_Set_Cnt(&set) != 4) continue;
                            vrc.clear();
                            int ecnt = Mark_Row_Only4( pb, &rc, &rc2, &rc3, &rc4, &set, cf_NTE, &vrc );
                            if (ecnt) {
                                Mark_RC_with_Flag( pb, &rc, cf_NT );
                                Mark_RC_with_Flag( pb, &rc2, cf_NT );
                                Mark_RC_with_Flag( pb, &rc3, cf_NT );
                                Mark_RC_with_Flag( pb, &rc4, cf_NT );
                                count += ecnt;
                                for (me = 0; me < vrc.size(); me++) {
                                    elims.push_back(vrc[me]);
                                }
                                elimby.push_back(rc);
                                elimby.push_back(rc2);
                                elimby.push_back(rc3);
                                elimby.push_back(rc4);
                            }
                            vdone.push_back(rc2);    // well it is NOW done
                            vdone.push_back(rc3);    // well it is NOW done
                            vdone.push_back(rc4);    // well it is NOW done
                            if (count && only_one_quad)
                                break;
                        }   // for (m = l + 1; m < max; m++)
                        if (count && only_one_quad)
                           break;
                    }   // for (l = k + 1; l < max; l++)
                    if (count && only_one_quad)
                       break;
                }   // for (k = j + 1; k < max; k++)
                if (count && only_one_quad)
                   break;
            }   // for (j = 0; j < max; j++)
            if (count && only_one_quad)
                break;
        }   // for (i = 0; i < 9; i++) 
    }
    if (!count) {
        // time to scan the COLS
        vdone.clear();
        for (i = 0; i < 9; i++) { // test ALL 9 cols for 'Naked Triples'
            pvrc = &prcrcb->vcols[i];   // get rc set for each box
            max = pvrc->size();
            if (max < 2) continue;  // must have 2 or more members
            for (j = 0; j < max; j++) {
                prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
                rc = *prc;
                row = prc->row;
                col = prc->col;
                box = GETBOX(row,col);
                if ( Row_Col_in_RC_Vector( vdone, row, col ) ) continue;
                vdone.push_back(rc);    // well it is NOW done
                ps = &prc->set; // pointer to list of candidates
                scnt = Get_Set_Cnt2(ps,setvals);
                if ( (scnt < 2) || (scnt > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                for (k = 0; k < max; k++) {
                    if (k == j) continue;
                    prc2 = &pvrc->at(k);
                    rc2 = *prc2;
                    row2 = rc2.row;
                    col2 = rc2.col;
                    box2 = GETBOX(row2,col2);
                    if ( Row_Col_in_RC_Vector( vdone, row2, col2 ) ) continue;
                    ps2 = &rc2.set;
                    scnt2 = Get_Set_Cnt2(ps2,setvals2);
                    if ( (scnt2 < 2) || (scnt2 > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                    for (l = 0; l < max; l++) {
                        if (l == j) continue;
                        if (l == k) continue;
                        prc3 = &pvrc->at(l);
                        rc3 = *prc3;
                        row3 = rc3.row;
                        col3 = rc3.col;
                        box3 = GETBOX(row3,col3);
                        if ( Row_Col_in_RC_Vector( vdone, row3, col3 ) ) continue;
                        ps3 = &rc3.set;
                        scnt3 = Get_Set_Cnt2(ps3,setvals3);
                        if ( (scnt3 < 2) || (scnt3 > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                        //if ( !(((row == row2) && (row == row3))||((col == col2)&&(col == col3))) ) continue;
                        for (m = 0; m < max; m++) {
                            if (m == l) continue;
                            if (m == j) continue;
                            if (m == k) continue;
                            prc4 = &pvrc->at(m);
                            rc4 = *prc4;
                            row4 = rc4.row;
                            col4 = rc4.col;
                            box4 = GETBOX(row4,col4);
                            if ( Row_Col_in_RC_Vector( vdone, row4, col4 ) ) continue;
                            ps4 = &rc4.set;
                            scnt4 = Get_Set_Cnt2(ps4,setvals4);
                            if ( (scnt4 < 2) || (scnt4 > 4) ) continue; // seeking ONLY 2 3 or 4 candidates
                            // Now do they share just 4 values
                            Zero_SET(&set);
                            for (s = 0; s < scnt; s++) {
                                val = setvals[s];
                                set.val[val - 1] = val;
                            }
                            for (s = 0; s < scnt2; s++) {
                                val = setvals2[s];
                                set.val[val - 1] = val;
                            }
                            for (s = 0; s < scnt3; s++) {
                                val = setvals3[s];
                                set.val[val - 1] = val;
                            }
                            for (s = 0; s < scnt4; s++) {
                                val = setvals4[s];
                                set.val[val - 1] = val;
                            }
                            sprintf(tb,"Cells %s %s %s %s {%d,%d,%d,%d} shares ",
                                Get_RC_RCBS_Stg(&rc), 
                                Get_RC_RCBS_Stg(&rc2),
                                Get_RC_RCBS_Stg(&rc3),
                                Get_RC_RCBS_Stg(&rc4),
                                scnt, scnt2, scnt3, scnt4);
                            AddSet2Buf(tb,&set);
                            if (Get_Set_Cnt(&set) != 4) continue;
                            vrc.clear();
                            int ecnt = Mark_Col_Only4( pb, &rc, &rc2, &rc3, &rc4, &set, cf_NTE, &vrc );
                            if (ecnt) {
                                Mark_RC_with_Flag( pb, &rc, cf_NT );
                                Mark_RC_with_Flag( pb, &rc2, cf_NT );
                                Mark_RC_with_Flag( pb, &rc3, cf_NT );
                                Mark_RC_with_Flag( pb, &rc4, cf_NT );
                                count += ecnt;
                                for (me = 0; me < vrc.size(); me++) {
                                    elims.push_back(vrc[me]);
                                }
                                elimby.push_back(rc);
                                elimby.push_back(rc2);
                                elimby.push_back(rc3);
                                elimby.push_back(rc4);
                            }
                            vdone.push_back(rc2);    // well it is NOW done
                            vdone.push_back(rc3);    // well it is NOW done
                            vdone.push_back(rc4);    // well it is NOW done
                            if (count && only_one_quad)
                                break;
                        }   // for (m = l + 1; m < max; m++)
                        if (count && only_one_quad)
                           break;
                    }   // for (l = k + 1; l < max; l++)
                    if (count && only_one_quad)
                       break;
                }   // for (k = j + 1; k < max; k++)
                if (count && only_one_quad)
                   break;
            }   // for (j = 0; j < max; j++)
            if (count && only_one_quad)
                break;
        }   // for (i = 0; i < 9; i++) 
    }
    if (count) {
        size_t max, ii, max2, m2;
        PROWCOL prc;
        max = elims.size();
        max2 = elimby.size();
        sprintf(tb,"NQ.Elims %d ", (int)max );
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            Append_RC_setval_RC_Stg(tb,prc,prc->cnum);
            ADDSP(tb);
            m2 = ii * 4;
            if ( !(m2 < max2) ) continue;
            strcat(tb,"by ");
            prc = &elimby[m2];
            Append_RC_setval_RC_Stg(tb,prc);
            m2++;
            if ( !(m2 < max2) ) continue;
            ADDSP(tb);
            prc = &elimby[m2];
            Append_RC_setval_RC_Stg(tb,prc);
            m2++;
            if ( !(m2 < max2) ) continue;
            ADDSP(tb);
            prc = &elimby[m2];
            Append_RC_setval_RC_Stg(tb,prc);
            m2++;
            if ( !(m2 < max2) ) continue;
            ADDSP(tb);
            prc = &elimby[m2];
            Append_RC_setval_RC_Stg(tb,prc);
            m2++;
            ADDSP(tb);
        }
        OUTITNP(tb);
        sprtf("S2c: Marked Naked Quad [%d] - to %d\n", count, sg_Fill_NakedQuad );
        //Stack_Fill( Do_Fill_NakedTriple );
        pb->iStage = sg_Fill_NakedQuad;
    } else {
        // sprtf("S2g: No Marked Naked Quad - to bgn\n");
        // pb->iStage = sg_Begin;
    }

    return count;
}

int Do_Fill_NakedQuad( PABOX2 pb )
{
    int count = Do_Fill_By_Flags( pb, cf_NTE, cf_NT, "NQ", "Naked.Quad" );
    if (add_debug_np)
        sprtf("Done Fill Naked Quad - count %d\n", count);
    if (count)
        pb->iStage = sg_Begin;
    return count;
}


// ID_VIEW_NAKEDTRIPLE
// from : http://www.sudokuwiki.org/Naked_Candidates
// Any group of three cells in the same unit that contain 
// IN TOTAL three candidates is a Naked Triple. 
// Each cell can have two or three numbers, as long as in 
// combination all three cells have only three numbers. 
// When this happens, the three candidates can be removed 
// from all other cells in the same unit.
int Do_Fill_NakedTriple( PABOX2 pb )
{
    int count = Do_Fill_By_Flags( pb, cf_NTE, cf_NT, "NT", "Naked.Trip" );
    sprtf("Done Fill Naked Triple - count %d\n", count);
    if (count)
        pb->iStage = sg_Begin;
    return count;
}

int only_one_triple = 1;
int Do_Naked_Triple_Scan( PABOX2 pb )
{
    int count = 0;
    RCRCB rcrcb;
    vRC *pvrc;
    int cnt;
    vRC empty, vdone;
    PRCRCB prcrcb = &rcrcb;
    int i, s;
    size_t max, j, k, l;
    ROWCOL rc, rc2, rc3;
    PROWCOL prc, prc2, prc3;
    int row, col, box;
    int row2, col2, box2;
    int row3, col3, box3;
    int scnt, scnt2, scnt3;
    int setvals[9];
    int setvals2[9];
    int setvals3[9];
    PSET ps, ps2, ps3;
    SET set;
    int val;
    vRC vrc, elims, elimby;
    size_t me;
    char *tb = GetNxtBuf();
    cnt = Get_Empty_Cells( pb, empty, prcrcb );
    for (i = 0; i < 9; i++) { // test ALL 9 boxes for 'Naked Triples'
        pvrc = &prcrcb->vboxs[i];   // get rc set for each box
        max = pvrc->size();
        if (max < 2) continue;  // must have 2 or more members
        for (j = 0; j < max; j++) {
            prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
            rc = *prc;
            row = prc->row;
            col = prc->col;
            box = GETBOX(row,col);
            if ( Row_Col_in_RC_Vector( vdone, row, col ) ) continue;
            vdone.push_back(rc);    // well it is NOW done
            ps = &prc->set; // pointer to list of candidates
            scnt = Get_Set_Cnt2(ps,setvals);
            if ( !((scnt == 2) || (scnt == 3)) ) continue; // seeking ONLY 2 or 3 candidates
            for (k = 0; k < max; k++) {
                if (k == j) continue;
                prc2 = &pvrc->at(k);
                rc2 = *prc2;
                row2 = rc2.row;
                col2 = rc2.col;
                box2 = GETBOX(row2,col2);
                if ( Row_Col_in_RC_Vector( vdone, row2, col2 ) ) continue;
                ps2 = &rc2.set;
                scnt2 = Get_Set_Cnt2(ps2,setvals2);
                if ( !((scnt2 == 2) || (scnt2 == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                for (l = 0; l < max; l++) {
                    if (l == j) continue;
                    if (l == k) continue;
                    prc3 = &pvrc->at(l);
                    rc3 = *prc3;
                    row3 = rc3.row;
                    col3 = rc3.col;
                    box3 = GETBOX(row3,col3);
                    if ( Row_Col_in_RC_Vector( vdone, row3, col3 ) ) continue;
                    ps3 = &rc3.set;
                    scnt3 = Get_Set_Cnt2(ps3,setvals3);
                    if ( !((scnt3 == 2) || (scnt3 == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                    if ( !(((row == row2) && (row == row3))||((col == col2)&&(col == col3))) ) continue;
                    // Now do they share just 3 values
                    Zero_SET(&set);
                    for (s = 0; s < scnt; s++) {
                        val = setvals[s];
                        set.val[val - 1] = val;
                    }
                    for (s = 0; s < scnt2; s++) {
                        val = setvals2[s];
                        set.val[val - 1] = val;
                    }
                    for (s = 0; s < scnt3; s++) {
                        val = setvals3[s];
                        set.val[val - 1] = val;
                    }
                    sprintf(tb,"Cells %s %s %s {%d,%d,%d} shares ",
                        Get_RC_RCBS_Stg(&rc), 
                        Get_RC_RCBS_Stg(&rc2),
                        Get_RC_RCBS_Stg(&rc3),
                        scnt, scnt2, scnt3);
                    AddSet2Buf(tb,&set);
                    if (Get_Set_Cnt(&set) != 3) continue;
                    int ecnt = 0;
                    vrc.clear();
                    if (row == row2) {
                        ecnt += Mark_Row_outside_Box( pb, &rc, &rc2, &rc3, &set, cf_NTE, &vrc );
                        // TODO - elim cands from this ROW, outside Box
                    } else if (col == col2) {
                        // TODO - elim cands from this COL, outside Box
                        ecnt += Mark_Col_outside_Box( pb, &rc, &rc2, &rc3, &set, cf_NTE, &vrc );
                    }
                    ecnt += Mark_inside_Box( pb, &rc, &rc2, &rc3, &set, cf_NTE, &vrc );
                    if (ecnt) {
                        Mark_RC_with_Flag( pb, &rc, cf_NT );
                        Mark_RC_with_Flag( pb, &rc2, cf_NT );
                        Mark_RC_with_Flag( pb, &rc3, cf_NT );
                        count += ecnt;
                        for (me = 0; me < vrc.size(); me++)
                            elims.push_back(vrc[me]);
                        elimby.push_back(rc);
                        elimby.push_back(rc2);
                        elimby.push_back(rc3);
                    }
                    vdone.push_back(rc2);    // well it is NOW done
                    vdone.push_back(rc3);    // well it is NOW done
                    if (count && only_one_triple)
                        break;
                }   // for (l = 0; l < max; l++)
                if (count && only_one_triple)
                    break;
            }   // for (k = 0; k < max; k++)
            if (count && only_one_triple)
                break;
        }   // for (j = 0; j < max; j++)
        if (count && only_one_triple)
            break;
    }   // for (i = 0; i < 9; i++) 
    if (!count) {
        // time to scan the ROWS
        vdone.clear();
        for (i = 0; i < 9; i++) { // test ALL 9 rows for 'Naked Triples'
            pvrc = &prcrcb->vrows[i];   // get rc set for each box
            max = pvrc->size();
            if (max < 2) continue;  // must have 2 or more members
            for (j = 0; j < max; j++) {
                prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
                rc = *prc;
                row = prc->row;
                col = prc->col;
                box = GETBOX(row,col);
                if ( Row_Col_in_RC_Vector( vdone, row, col ) ) continue;
                vdone.push_back(rc);    // well it is NOW done
                ps = &prc->set; // pointer to list of candidates
                scnt = Get_Set_Cnt2(ps,setvals);
                if ( !((scnt == 2) || (scnt == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                for (k = 0; k < max; k++) {
                    if (k == j) continue;
                    prc2 = &pvrc->at(k);
                    rc2 = *prc2;
                    row2 = rc2.row;
                    col2 = rc2.col;
                    box2 = GETBOX(row2,col2);
                    if ( Row_Col_in_RC_Vector( vdone, row2, col2 ) ) continue;
                    ps2 = &rc2.set;
                    scnt2 = Get_Set_Cnt2(ps2,setvals2);
                    if ( !((scnt2 == 2) || (scnt2 == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                    for (l = 0; l < max; l++) {
                        if (l == j) continue;
                        if (l == k) continue;
                        prc3 = &pvrc->at(l);
                        rc3 = *prc3;
                        row3 = rc3.row;
                        col3 = rc3.col;
                        box3 = GETBOX(row3,col3);
                        if ( Row_Col_in_RC_Vector( vdone, row3, col3 ) ) continue;
                        ps3 = &rc3.set;
                        scnt3 = Get_Set_Cnt2(ps3,setvals3);
                        if ( !((scnt3 == 2) || (scnt3 == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                        if ( !(((row == row2) && (row == row3))||((col == col2)&&(col == col3))) ) continue;
                        // Now do they share just 3 values
                        Zero_SET(&set);
                        for (s = 0; s < scnt; s++) {
                            val = setvals[s];
                            set.val[val - 1] = val;
                        }
                        for (s = 0; s < scnt2; s++) {
                            val = setvals2[s];
                            set.val[val - 1] = val;
                        }
                        for (s = 0; s < scnt3; s++) {
                            val = setvals3[s];
                            set.val[val - 1] = val;
                        }
                        sprintf(tb,"Cells %s %s %s {%d,%d,%d} shares ",
                            Get_RC_RCBS_Stg(&rc), 
                            Get_RC_RCBS_Stg(&rc2),
                            Get_RC_RCBS_Stg(&rc3),
                            scnt, scnt2, scnt3);
                        AddSet2Buf(tb,&set);
                        if (Get_Set_Cnt(&set) != 3) continue;
                        vrc.clear();
                        int ecnt = Mark_Row_Only( pb, &rc, &rc2, &rc3, &set, cf_NTE, &vrc );
                        if (ecnt) {
                            Mark_RC_with_Flag( pb, &rc, cf_NT );
                            Mark_RC_with_Flag( pb, &rc2, cf_NT );
                            Mark_RC_with_Flag( pb, &rc3, cf_NT );
                            count += ecnt;
                            for (me = 0; me < vrc.size(); me++)
                                elims.push_back(vrc[me]);
                            elimby.push_back(rc);
                            elimby.push_back(rc2);
                            elimby.push_back(rc3);
                        }
                        vdone.push_back(rc2);    // well it is NOW done
                        vdone.push_back(rc3);    // well it is NOW done
                        if (count && only_one_triple)
                            break;
                    }   // for (l = 0; l < max; l++)
                    if (count && only_one_triple)
                        break;
                }   // for (k = 0; k < max; k++)
                if (count && only_one_triple)
                    break;
            }   // for (j = 0; j < max; j++)
            if (count && only_one_triple)
                break;
        }   // for (i = 0; i < 9; i++) 
    }
    if (!count) {
        // time to scan the COLS
        vdone.clear();
        for (i = 0; i < 9; i++) { // test ALL 9 cols for 'Naked Triples'
            pvrc = &prcrcb->vcols[i];   // get rc set for each box
            max = pvrc->size();
            if (max < 2) continue;  // must have 2 or more members
            for (j = 0; j < max; j++) {
                prc = &pvrc->at(j); // do EACH for each candidate, for each of the others
                rc = *prc;
                row = prc->row;
                col = prc->col;
                box = GETBOX(row,col);
                if ( Row_Col_in_RC_Vector( vdone, row, col ) ) continue;
                vdone.push_back(rc);    // well it is NOW done
                ps = &prc->set; // pointer to list of candidates
                scnt = Get_Set_Cnt2(ps,setvals);
                if ( !((scnt == 2) || (scnt == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                for (k = 0; k < max; k++) {
                    if (k == j) continue;
                    prc2 = &pvrc->at(k);
                    rc2 = *prc2;
                    row2 = rc2.row;
                    col2 = rc2.col;
                    box2 = GETBOX(row2,col2);
                    if ( Row_Col_in_RC_Vector( vdone, row2, col2 ) ) continue;
                    ps2 = &rc2.set;
                    scnt2 = Get_Set_Cnt2(ps2,setvals2);
                    if ( !((scnt2 == 2) || (scnt2 == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                    for (l = 0; l < max; l++) {
                        if (l == j) continue;
                        if (l == k) continue;
                        prc3 = &pvrc->at(l);
                        rc3 = *prc3;
                        row3 = rc3.row;
                        col3 = rc3.col;
                        box3 = GETBOX(row3,col3);
                        if ( Row_Col_in_RC_Vector( vdone, row3, col3 ) ) continue;
                        ps3 = &rc3.set;
                        scnt3 = Get_Set_Cnt2(ps3,setvals3);
                        if ( !((scnt2 == 2) || (scnt2 == 3)) ) continue; // seeking ONLY 2 or 3 candidates
                        if ( !(((row == row2) && (row == row3))||((col == col2)&&(col == col3))) ) continue;
                        // Now do they share just 3 values
                        Zero_SET(&set);
                        for (s = 0; s < scnt; s++) {
                            val = setvals[s];
                            set.val[val - 1] = val;
                        }
                        for (s = 0; s < scnt2; s++) {
                            val = setvals2[s];
                            set.val[val - 1] = val;
                        }
                        for (s = 0; s < scnt3; s++) {
                            val = setvals3[s];
                            set.val[val - 1] = val;
                        }
                        sprintf(tb,"Cells %s %s %s {%d,%d,%d} shares ",
                            Get_RC_RCBS_Stg(&rc), 
                            Get_RC_RCBS_Stg(&rc2),
                            Get_RC_RCBS_Stg(&rc3),
                            scnt, scnt2, scnt3);
                        AddSet2Buf(tb,&set);
                        if (Get_Set_Cnt(&set) != 3) continue;
                        vrc.clear();
                        int ecnt = Mark_Col_Only( pb, &rc, &rc2, &rc3, &set, cf_NTE, &vrc );
                        if (ecnt) {
                            Mark_RC_with_Flag( pb, &rc, cf_NT );
                            Mark_RC_with_Flag( pb, &rc2, cf_NT );
                            Mark_RC_with_Flag( pb, &rc3, cf_NT );
                            count += ecnt;
                            for (me = 0; me < vrc.size(); me++)
                                elims.push_back(vrc[me]);
                            elimby.push_back(rc);
                            elimby.push_back(rc2);
                            elimby.push_back(rc3);
                        }
                        vdone.push_back(rc2);    // well it is NOW done
                        vdone.push_back(rc3);    // well it is NOW done
                        if (count && only_one_triple)
                            break;
                    }   // for (l = 0; l < max; l++)
                    if (count && only_one_triple)
                        break;
                }   // for (k = 0; k < max; k++)
                if (count && only_one_triple)
                    break;
            }   // for (j = 0; j < max; j++)
            if (count && only_one_triple)
                break;
        }   // for (i = 0; i < 9; i++) 
    }
    if (count) {
        size_t max, ii, max2, m2;
        PROWCOL prc;
        max = elims.size();
        max2 = elimby.size();
        sprintf(tb,"NT.Elims %d ", (int)max );
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            Append_RC_setval_RC_Stg(tb,prc,prc->cnum);
            ADDSP(tb);
            m2 = ii * 4;
            if ( !(m2 < max2) ) continue;
            strcat(tb,"by ");
            prc = &elimby[m2];
            Append_RC_setval_RC_Stg(tb,prc);
            m2++;
            if ( !(m2 < max2) ) continue;
            ADDSP(tb);
            prc = &elimby[m2];
            Append_RC_setval_RC_Stg(tb,prc);
            m2++;
            if ( !(m2 < max2) ) continue;
            ADDSP(tb);
            prc = &elimby[m2];
            Append_RC_setval_RC_Stg(tb,prc);
            m2++;
            ADDSP(tb);
        }
        OUTIT(tb);

        sprtf("S2c: Marked Naked Triple [%d] - to fill\n", count, sg_Fill_NakedTrip );
        //Stack_Fill( Do_Fill_NakedTriple );
        pb->iStage = sg_Fill_NakedTrip;
    } else {
        // sprtf("S2g: No Marked Naked Triple - to begin\n");
        // pb->iStage = sg_Begin;
    }

    return count;
}





////////////////////////////////////////////////////////////////////////
// Naked Pair Marking and Elimination
// ==================================
// =======================================================================
static bool bElim_All = false;

int Set_SET_flag( PSET ps2, uint64_t flag )
{
    int count = 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps2->val[i];
        if (!val) continue;
        if ( !(ps2->flag[i] & flag) ) {
            ps2->flag[i] |= flag;
            count++;
        }
    }
    return count;
}

int Mark_SET_in_Row( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, uint64_t eflg,
    vRC *pelim, vRC *elimby )
{
    int col, val, cnt, i;
    PSET ps;
    int count = 0;
    bool all = bElim_All;
    ROWCOL rc;
    char *tb = GetNxtBuf();
    for (col = 0; col < 9; col++) {
        if ((ccol1 == col)||(ccol2 == col)) continue; // skip PAIR
        val = pb->line[crow1].val[col];
        if (val) continue;
        ps = &pb->line[crow1].set[col];
        sprintf(tb," COLS row %d col %d vals ", crow1+1, col+1);
        Add_Set2Buf(tb, ps);
        cnt = 0;
        //cnt = Mark_SET_in_SET( ps, ps2, eflg ); // Mark_SET_in_Row()
        for (i = 0; i < 9; i++) {
            val = ps->val[i]; // get a SOURCE
            if (!val) continue;
            if (!ps2->val[i]) continue;
            // do NOT count repeated marks, unless ASKED
            if ( all || !(ps->flag[i] & eflg) ) {
                ps->flag[i] |= eflg;
                cnt++;
                rc.row = crow1;
                rc.col = col;
                rc.box = GETBOX(crow1,col);
                rc.cnum = i + 1;
                COPY_SET(&rc.set,ps);
                pelim->push_back(rc);
            }
        }
        if (cnt) {
            sprintf(EndBuf(tb)," ae %d ",cnt);
            Add_Set2Buf(tb, ps);
        } else {
            strcat(tb, " none");
        }
        OUTITNP(tb);
        count += cnt;
    }
    return count;
}

int Mark_SET_in_Col( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, uint64_t eflg,
        vRC *pelim, vRC *pelimby )
{
    int row, val, cnt, i;
    PSET ps;
    int count = 0;
    bool all = bElim_All;
    ROWCOL rc;
    // elim same COLUMN
    for (row = 0; row < 9; row++) {
        if ((crow1 == row)||(crow2 == row)) continue; // skip PAIR
        val = pb->line[row].val[ccol1];
        if (val) continue;
        ps = &pb->line[row].set[ccol1];
        char *tb = GetNxtBuf();
        sprintf(tb," ROWS row %d col %d vals ", row+1, ccol1+1);
        Add_Set2Buf(tb, ps);
        //cnt = Mark_SET_in_SET( ps, ps2, eflg ); // Mark_SET_in_Col()
        cnt = 0;
        for (i = 0; i < 9; i++) {
            val = ps->val[i]; // get a SOURCE
            if (!val) continue;
            if (!ps2->val[i]) continue;
            // do NOT count repeated marks, unless ASKED
            if ( all || !(ps->flag[i] & eflg) ) {
                ps->flag[i] |= eflg;
                cnt++;
                rc.row = row;
                rc.col = ccol1;
                rc.box = GETBOX(row,ccol1);
                rc.cnum = i + 1;
                COPY_SET(&rc.set,ps);
                pelim->push_back(rc);
            }
        }

        if (cnt) {
            sprintf(EndBuf(tb)," ae %d ",cnt);
            Add_Set2Buf(tb, ps);
        } else {
            strcat(tb, " none");
        }
        OUTITNP(tb);
        count += cnt;
    }
    return count;
}

int Mark_SET_in_Box( PABOX2 pb, int crow1, int ccol1, int crow2, int ccol2, PSET ps2, uint64_t eflg,
        vRC *pelim, vRC *pelimby )
{
    int row, col, val, cnt, i;
    PSET ps;
    int count = 0;
    bool all = bElim_All;
    int r, c, rw, cl;
    r = (crow1 / 3) * 3;
    c = (ccol1 / 3) * 3;
    ROWCOL rc;
    char *tb = GetNxtBuf();
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            row = r + rw;
            col = c + cl;
            if ((crow1 == row) && (ccol1 == col)) continue; // skip this PAIR
            if ((crow2 == row) && (ccol2 == col)) continue; // skip this PAIR
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            sprintf(tb," BOX1 row %d col %d vals ", row+1, col+1);
            Add_Set2Buf(tb, ps);
            //cnt = Mark_SET_in_SET( ps, ps2, eflg ); // Mark_SET_in_Box()
            cnt = 0;
            for (i = 0; i < 9; i++) {
                val = ps->val[i]; // get a SOURCE
                if (!val) continue;
                if (!ps2->val[i]) continue;
                // do NOT count repeated marks, unless ASKED
                if ( all || !(ps->flag[i] & eflg) ) {
                    ps->flag[i] |= eflg;
                    cnt++;
                    rc.row = row;
                    rc.col = col;
                    rc.box = GETBOX(row,col);
                    rc.cnum = i + 1;
                    COPY_SET(&rc.set,ps);
                    pelim->push_back(rc);
                }
            }
            count += cnt;
            if (cnt) {
                sprintf(EndBuf(tb)," ae %d ",cnt);
                Add_Set2Buf(tb, ps);
            } else {
                strcat(tb, " none");
            }
            OUTITNP(tb);
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
                if (val) continue;
                ps = &pb->line[row].set[col];
                sprintf(tb," BOX2 row %d col %d vals ", row+1, col+1);
                Add_Set2Buf(tb, ps);
                //cnt = Mark_SET_in_SET( ps, ps2, eflg ); // Mark_SET_in_Box()
                cnt = 0;
                for (i = 0; i < 9; i++) {
                    val = ps->val[i]; // get a SOURCE
                    if (!val) continue;
                    if (!ps2->val[i]) continue;
                    // do NOT count repeated marks, unless ASKED
                    if ( all || !(ps->flag[i] & eflg) ) {
                        ps->flag[i] |= eflg;
                        cnt++;
                        rc.row = row;
                        rc.col = col;
                        rc.box = GETBOX(row,col);
                        rc.cnum = i + 1;
                        COPY_SET(&rc.set,ps);
                        pelim->push_back(rc);
                    }
                }
                if (cnt) {
                    sprintf(EndBuf(tb)," ae %d ",cnt);
                    Add_Set2Buf(tb, ps);
                } else {
                    strcat(tb, " none");
                }
                OUTITNP(tb);
                count += cnt;
            }
        }
    }
    return count;
}


int Mark_Naked_Pair_In_Row( PABOX2 pb, int crow, int ccol, PSET ps2, uint64_t mflg, uint64_t eflg,
    vRC *pelim, vRC *pelimby )
{
    int col, val, count, fnd, cnt;
    PSET ps;
    ROWCOL rc;
    char *tb = GetNxtBuf();
    sprintf(tb,"NPRow: row %d col %d vals ", crow + 1, ccol + 1);
    Add_Set2Buf(tb, ps2);
    OUTITNP(tb);
    count = 0;
    fnd = 0;
    for (col = 0; col < 9; col++) {
        if (col == ccol) continue; // skip SELF
        val = pb->line[crow].val[col];
        if (val) continue; // skip with value
        ps = &pb->line[crow].set[col];
        if ( !Equal_SET( ps, ps2 ) ) continue; // skip NOT equal set
        sprintf(tb,"NPRow: row %d col %d vals ", crow + 1, col + 1);
        Add_Set2Buf(tb, ps);
        strcat(tb," Found");
        OUTITNP(tb);
        cnt = Mark_SET_in_Row( pb, crow, ccol, crow, col, ps, eflg, pelim, pelimby );
        count += cnt;
        if (cnt) {
            Set_SET_flag( ps2, mflg );
            Set_SET_flag( ps,  mflg );   // FIX20120927 - Mark second PAIR also
            rc.row = crow;
            rc.col = ccol;
            rc.box = GETBOX(crow,ccol);
            COPY_SET(&rc.set,ps2);
            rc.cnum = -1;   // all in set
            pelimby->push_back(rc);
            rc.row = crow;
            rc.col = col;
            rc.box = GETBOX(crow,col);
            COPY_SET(&rc.set,ps);
            rc.cnum = -1;   // all in set
            pelimby->push_back(rc);
        }
        fnd++;
    }
    if (!fnd) {
        sprintf(tb,"NPROw: row %d col %d vals ", crow + 1, ccol + 1);
        Add_Set2Buf(tb, ps2);
        strcat(tb," No Finds.");
        OUTITNP(tb);
    }
    return count;
}

int Mark_Naked_Pair_In_Col( PABOX2 pb, int crow, int ccol, PSET ps2, uint64_t mflg, uint64_t eflg,
        vRC *pelim, vRC *pelimby )
{
    int row, val, count, fnd, cnt;
    PSET ps;
    ROWCOL rc;
    char *tb = GetNxtBuf();
    sprintf(tb,"NPCol: row %d col %d vals ", crow + 1, ccol + 1);
    Add_Set2Buf(tb, ps2);
    OUTITNP(tb);
    count = 0;
    fnd = 0;
    for (row = 0; row < 9; row++) {
        if (row == crow) continue; // skip SELF
        val = pb->line[row].val[ccol];
        if (val) continue;
        ps = &pb->line[row].set[ccol];
        if ( !Equal_SET( ps, ps2 ) ) continue;
        sprintf(tb,"NPCol: row %d col %d vals ", row + 1, ccol + 1);
        Add_Set2Buf(tb, ps);
        strcat(tb," Found");
        OUTITNP(tb);
        cnt = Mark_SET_in_Col( pb, crow, ccol, row, ccol, ps, eflg, pelim, pelimby );
        count += cnt;
        if (cnt) {
            Set_SET_flag( ps2, mflg );
            Set_SET_flag( ps,  mflg );   // FIX20120927 - Mark second PAIR also
            rc.row = crow;
            rc.col = ccol;
            rc.box = GETBOX(crow,ccol);
            COPY_SET(&rc.set,ps2);
            rc.cnum = -1;   // all in set
            pelimby->push_back(rc);
            rc.row = row;
            rc.col = ccol;
            rc.box = GETBOX(row,ccol);
            COPY_SET(&rc.set,ps);
            rc.cnum = -1;   // all in set
            pelimby->push_back(rc);
        }
        fnd++;
    }
    if (!fnd) {
        tb = GetNxtBuf();
        sprintf(tb,"SPICol: row %d col %d vals ", crow + 1, ccol + 1);
        Add_Set2Buf(tb, ps2);
        strcat(tb," No finds");
        OUTITNP(tb);

    }
    return count;
}

int Mark_Naked_Pair_In_Box( PABOX2 pb, int crow, int ccol, PSET ps2, uint64_t mflg, uint64_t eflg,
        vRC *pelim, vRC *pelimby )
{
    int row, col, val, count, fnd, cnt;
    PSET ps;
    ROWCOL rc;
    char *tb = GetNxtBuf();
    sprintf(tb,"NPBox: row %d col %d vals ", crow + 1, ccol + 1);
    Add_Set2Buf(tb, ps2);
    OUTITNP(tb);
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
            if (val) continue;
            ps = &pb->line[row].set[col];
            if ( !Equal_SET( ps, ps2 ) ) continue;
            sprintf(tb,"NPBox: row %d col %d vals ", row + 1, col + 1);
            Add_Set2Buf(tb, ps);
            strcat(tb," Found");
            OUTITNP(tb);
            cnt = Mark_SET_in_Box( pb, crow, ccol, row, col, ps, eflg, pelim, pelimby );
            count += cnt;
            if (cnt) {
                Set_SET_flag( ps2, mflg );
                Set_SET_flag( ps,  mflg );   // FIX20120927 - Mark second PAIR also
                rc.row = crow;
                rc.col = ccol;
                rc.box = GETBOX(crow,ccol);
                COPY_SET(&rc.set,ps2);
                rc.cnum = -1;   // all in set
                pelimby->push_back(rc);
                rc.row = row;
                rc.col = col;
                rc.box = GETBOX(row,col);
                COPY_SET(&rc.set,ps);
                rc.cnum = -1;   // all in set
                pelimby->push_back(rc);
            }
            fnd++;
        }
    }
    if (!fnd) {
        sprintf(tb,"NPBox: row %d col %d vals ", crow + 1, ccol + 1);
        Add_Set2Buf(tb, ps2);
        strcat(tb, " No finds.");
        OUTITNP(tb);
    }
    return count;
}


// So try for the so called Naked Pair
// Find 2 cells in the same row, col, or box, having the same PAIR of number
// Since this is the only solution for these two cells, one will have one, and the other the other,
// and we hope to find which way later.
// But for now can ELIMINATE all others in the same Row, Col, or Box
int Do_Naked_Pairs(PABOX2 pb)
{
    int row, col, val, cnt, count;
    PSET ps;
    count = 0;
    if (add_debug_np) sprtf("S2c: Try to find Naked Pairs\n");
    vRC elim, elimby;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) continue;
            ps = &pb->line[row].set[col];
            cnt = Get_Set_Cnt(ps);
            if ( !(cnt == 2) ) continue;
            // ok, got a PAIR
            cnt = Mark_Naked_Pair_In_Row( pb, row, col, ps, cf_NP, cf_NPE, &elim, &elimby );
            if (cnt) {
                count += cnt;
            }
            cnt = Mark_Naked_Pair_In_Col( pb, row, col, ps, cf_NP, cf_NPE, &elim, &elimby );
            if (cnt) {
                count += cnt;
            }
            cnt = Mark_Naked_Pair_In_Box( pb, row, col, ps, cf_NP, cf_NPE, &elim, &elimby );
            if (cnt) {
                count += cnt;
            }
        }
    }
    if (count) {
        char *tb = GetNxtBuf();
        size_t max, ii, max2, m2;
        PROWCOL prc;
        max = elim.size();
        max2 = elimby.size();
        sprintf(tb,"S%d: NP-Elim %d ", pb->iStage, count);
        for (ii = 0; ii < max; ii++) {
            prc = &elim[ii];
            Append_RC_setval_RC_Stg(tb,prc,prc->cnum);
            m2 = ii * 2;
            if (m2 < max2) {
                strcat(tb," by ");
                prc = &elimby[m2];
                Append_RC_setval_RC_Stg(tb,prc);
                m2++;
                if (m2 < max2) {
                    strcat(tb," & ");
                    prc = &elimby[m2];
                    Append_RC_setval_RC_Stg(tb,prc);
                    ADDSP(tb);
                }
            }
        }
        OUTITNP(tb);
        sprtf("S2c: Found Naked Pairs. Elim %d To Fill\n", count, sg_Fill_Naked);
        pb->iStage = sg_Fill_Naked;
    } else {
        count = Do_Naked_Triple_Scan(pb);
        if (!count) {
            count = Do_Naked_Quad_Scan(pb);
            if (!count) {
                sprtf("S2c: NO Naked Pairs/Trip/Quad FOUND.\n");
                pb->iStage = sg_Begin;
            }
        }
    }
    return count;
}


// eof - Sudo_NPairs.cxx
