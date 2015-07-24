
// Sudo_PntColors.cxx

#include "Sudoku.hxx"
#ifdef WIN32
////////////////////////////////////////////////////////////////////////////////////
#define OUTITPFR(tb) if (add_debug_pfr) OUTIT(tb)

BOOL g_bPntCC = TRUE;

#define SAMERCB(p1,p2) ((p1->row == p2->row)||(p1->col == p2->col)||(GETBOX(p1->row,p1->col) == GETBOX(p2->row,p2->col)))

int Get_Line_Point( PRECT pr1, PPOINT ppts, int max, int setval )
{
    int iret = 2;
    int val = setval - 1;
    RECT r1 = *pr1;
    r1.left   += ht_margin;
    r1.right  -= ht_margin;
    r1.top    += vt_margin;
    r1.bottom -= vt_margin;
    int height = r1.bottom - r1.top;
    int width  = r1.right  - r1.left;
    int h3 = height / 3;
    int w3 = width / 3;
    r1.right = r1.left + w3;
    r1.bottom = r1.top + h3;
    int cnt = 0;
    RECT rc1 = r1;
    for (int j = 0; j < 9; j++) {
        if ( j && ((j % 3) == 0) ) {
            // go down a line
            cnt++;
            rc1 = r1;
            rc1.top += (h3 * cnt);
            rc1.bottom = rc1.top + h3;
            rc1.right = rc1.left + w3;
        }
        if (j == val) break;
        rc1.left  += w3;
        rc1.right += w3;
    }
    r1 = rc1;
    iret = 0;
    PPOINT ppt;
    if ((h3 > 3)&&(w3 > 3)) {
        r1 = rc1;
        OffsetRect(&r1,1,1);
        ppt = &ppts[iret++];
        ppt->x = (r1.left + r1.right) / 2;
        ppt->y = (r1.top + r1.bottom) / 2;
        r1 = rc1;
        OffsetRect(&r1,-1,-1);
        ppt = &ppts[iret++];
        ppt->x = (r1.left + r1.right) / 2;
        ppt->y = (r1.top + r1.bottom) / 2;
    } else {
        ppt = &ppts[iret++];
        ppt->x = (r1.left + r1.right) / 2;
        ppt->y = (r1.top + r1.bottom) / 2;
    }
    return iret;
}


int Get_Line_Points( PRECT pr1, PRECT pr2, PPOINT ppts, int max, int setval )
{
    int iret = 2;
    int val = setval - 1;
    RECT r1 = *pr1;
    RECT r2 = *pr2;
    r1.left   += ht_margin;
    r1.right  -= ht_margin;
    r1.top    += vt_margin;
    r1.bottom -= vt_margin;
    r2.left   += ht_margin;
    r2.right  -= ht_margin;
    r2.top    += vt_margin;
    r2.bottom -= vt_margin;

    int height = r1.bottom - r1.top;
    int width  = r1.right  - r1.left;
    int h3 = height / 3;
    int w3 = width / 3;
    r1.right = r1.left + w3;
    r1.bottom = r1.top + h3;
    r2.right = r2.left + w3;
    r2.bottom = r2.top + h3;
    int cnt = 0;
    RECT rc1 = r1;
    RECT rc2 = r2;
    for (int j = 0; j < 9; j++) {
        if ( j && ((j % 3) == 0) ) {
            // go down a line
            cnt++;
            rc1 = r1;
            rc1.top += (h3 * cnt);
            rc1.bottom = rc1.top + h3;
            rc1.right = rc1.left + w3;
            rc2 = r2;
            rc2.top += (h3 * cnt);
            rc2.bottom = rc2.top + h3;
            rc2.right = rc2.left + w3;
        }
        if (j == val) break;
        rc1.left  += w3;
        rc1.right += w3;
        rc2.left  += w3;
        rc2.right += w3;
    }
    r1 = rc1;
    r2 = rc2;
    iret = 0;
    PPOINT ppt;
    if ((h3 > 3)&&(w3 > 3)) {
        r1 = rc1;
        r2 = rc2;
        OffsetRect(&r1,1,1);
        OffsetRect(&r2,1,1);
        ppt = &ppts[iret++];
        ppt->x = (r1.left + r1.right) / 2;
        ppt->y = (r1.top + r1.bottom) / 2;
        ppt = &ppts[iret++];
        ppt->x = (r2.left + r2.right) / 2;
        ppt->y = (r2.top + r2.bottom) / 2;
        r1 = rc1;
        r2 = rc2;
        OffsetRect(&r1,-1,-1);
        OffsetRect(&r2,-1,-1);
        ppt = &ppts[iret++];
        ppt->x = (r1.left + r1.right) / 2;
        ppt->y = (r1.top + r1.bottom) / 2;
        ppt = &ppts[iret++];
        ppt->x = (r2.left + r2.right) / 2;
        ppt->y = (r2.top + r2.bottom) / 2;
    } else {
        ppt = &ppts[iret++];
        ppt->x = (r1.left + r1.right) / 2;
        ppt->y = (r1.top + r1.bottom) / 2;
        ppt = &ppts[iret++];
        ppt->x = (r2.left + r2.right) / 2;
        ppt->y = (r2.top + r2.bottom) / 2;
    }
    return iret;
}


void draw_link(HDC hdc, PROWCOL prc1, PROWCOL prc2, int setval, bool one)
{
    RECT r1, r2;
    POINT pts[4];
    int j;
    r1 = *get_rcSquares(prc1->row,prc1->col);
    r2 = *get_rcSquares(prc2->row,prc2->col);
    int cnt = Get_Line_Points( &r1, &r2, pts, 4, setval );
    for (j = 0; j < (cnt / 2); j++) {
        draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
        if (one) break;
    }
}


void paint_color_pairs( HDC hdc, int setval, vRCP *pvrc )
{
    ROWCOL rc, rc2;
    int count = 0;
    int row, col, box;
    int row2, col2, box2;
    PROWCOL prc1,prc2;
    PABOX2 pb = get_curr_box();
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    int bcnt, rcnt, ccnt;
    RCPAIR rcpair;

    size_t max = pvrc->size();
    size_t j;

    for (j = 0; j < max; j++) {
        rcpair = pvrc->at(j);
        rc = rcpair.rowcol[0];
        rc2 = rcpair.rowcol[1];
        prc1 = &rc;
        prc2 = &rc2;
        // NOW we have exactly PAIRS to PAINT
        //if (rc2.set.tm != rc.set.tm) continue;
        //if (!SAMERCB(prc1,prc2)) continue;

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
        draw_link(hdc, &rc, &rc2, setval);
        count++;
    }
}

void paint_color_test2( HDC hdc, int setval ) // if (do_color_test2)
{
    int i = setval - 1;
    size_t max, j;
    vRCP *pvrc = Get_Color2_Pairs(setval);
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
    RCPAIR rcpair;
    for (j = 0; j < max; j++) {
        rcpair = pvrc->at(j);
        rc = rcpair.rowcol[0];
        rc2 = rcpair.rowcol[1];
        prc1 = &rc;
        prc2 = &rc2;
        // NOW we have exactly PAIRS to PAINT
        //if (rc2.set.tm != rc.set.tm) continue;
        //if (!SAMERCB(prc1,prc2)) continue;

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


void Paint_Color_Test2( HDC hdc )
{
    if (!do_color_test2)
        return;
    sprtf("Paint Color Test for %d\n", do_color_test2);
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpold = (HPEN)SelectObject(hdc,hpslnk);
    paint_color_test2(hdc, do_color_test2);
    SelectObject(hdc,hpold);
}


#ifdef USE_COLOR2_SCAN
void Paint_Chain2( HDC hdc )
{
    vRCP *pvrcp = Get_Color_Chain2();
    if (!pvrcp) return;
    size_t max = pvrcp->size();
    if (!max) return;
    int setval = Get_Chain_Value2();
    if (!VALUEVALID(setval)) return;

    HPEN hp = Get_hpPink_2();
    HPEN hpold = (HPEN)SelectObject(hdc,hp);
    paint_color_pairs( hdc, setval, pvrcp );
    SelectObject(hdc,hpold);
}

#else // #ifdef USE_COLOR2_SCAN

void Paint_Chain( HDC hdc )
{
    vRC *pvrc = Get_Color_Chain();
    if (!pvrc) return;
    size_t max = pvrc->size();
    if (!max) return;
    int setval = Get_Chain_Value();
    if (!VALUEVALID(setval)) return;

    size_t i, i2;
    ROWCOL src1, src2;
    RECT r1, r2;
    POINT p1, p2;
    int val = setval - 1;
    HPEN hp = Get_hpPink_2();
    HPEN hpold = (HPEN)SelectObject(hdc,hp);
    SIZE sz,sz24;
    int j;
    int height, width, h3, w3, cnt;
    RECT rc1, rc2;
    HFONT hf24 = Get_CN24b_Font(hdc);

    get_CN8_Text_Size(hdc, &sz);

    if (g_bLineSB) {
        for (i = 0; i < max; i++) {
            for( i2 = 0; i2 < max; i2++) {
                src1 = pvrc->at(i);
                src2 = pvrc->at(i2);
                if (RC_Same_Box(&src1,&src2)) {
                    r1 = rcSquares[src1.row][src1.col];
                    r2 = rcSquares[src2.row][src2.col];
                    // simple line from r1 to r1
                    if (!g_bPntCtr) {
                        // or from candidate to candidate

                        // Adjust for margins
                        r1.left   += ht_margin;
                        r1.right  -= ht_margin;
                        r1.top    += vt_margin;
                        r1.bottom -= vt_margin;
                        r2.left   += ht_margin;
                        r2.right  -= ht_margin;
                        r2.top    += vt_margin;
                        r2.bottom -= vt_margin;

                        height = r1.bottom - r1.top;
                        width  = r1.right  - r1.left;
                        h3 = height / 3;
                        w3 = width / 3;
                        r1.right = r1.left + w3;
                        r1.bottom = r1.top + h3;
                        r2.right = r2.left + w3;
                        r2.bottom = r2.top + h3;
                        cnt = 0;
                        rc1 = r1;
                        rc2 = r2;
                        for (j = 0; j < 9; j++) {
                            if ( j && ((j % 3) == 0) ) {
                                // go down a line
                                cnt++;
                                rc1 = r1;
                                rc1.top += (h3 * cnt);
                                rc1.bottom = rc1.top + h3;
                                rc1.right = rc1.left + w3;
                                rc2 = r2;
                                rc2.top += (h3 * cnt);
                                rc2.bottom = rc2.top + h3;
                                rc2.right = rc2.left + w3;
                            }
                            if (j == val) break;
                            rc1.left  += w3;
                            rc1.right += w3;
                            rc2.left  += w3;
                            rc2.right += w3;
                        }
                        r1 = rc1;
                        r2 = rc2;
                    }
                    p1.x = (r1.left + r1.right) / 2;
                    p1.y = (r1.top + r1.bottom) / 2;
                    p2.x = (r2.left + r2.right) / 2;
                    p2.y = (r2.top + r2.bottom) / 2;
                    draw_line(hdc, &p1, &p2 );
                }
            }
        }
    }

    if (g_bPntSeq) {
        HFONT hfold = (HFONT)SelectObject(hdc,hf24);
        COLORREF crold = SetTextColor(hdc,crLGray);
        COLORREF crbk = SETBkColor(hdc,crWhite);
        COLORREF cr;
        char *tb = GetNxtBuf();
        UINT format = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
        int len = sprintf(tb,"%d",9);
        get_text_size_len_hdc( &sz24, tb, len, hdc );
        for (i = 0; i < max; i++) {
            src1 = pvrc->at(i);
            r1 = rcSquares[src1.row][src1.col];
            if (g_bPntChN) {
                len = sprintf(tb,"%d",src1.cnum + 1);
            } else {
                len = sprintf(tb,"%d",i + 1);
            }
            r1.top += ht_margin;
            r1.left += vt_margin;
            r1.right = r1.left + sz24.cx;
            r1.bottom = r1.top + sz24.cy;
            p1.x = (r1.left + r1.right) / 2;
            p1.y = (r1.top + r1.bottom) / 2;
            cr = GetPixel(hdc, p1.x, p1.y);
            SETBkColor(hdc,cr);
            if (Get_Chain_Value() == 1) {
                r1.left += sz.cx * 2;
                r1.right = r1.left + sz24.cx;
            }
            DrawText( hdc, // handle to DC
                tb,         // text to draw
                len,        // text length
                &r1,        // formatting dimensions
                format );   // text-drawing options
        }
        SETBkColor(hdc,crbk);
        SetTextColor(hdc,crold);
        SelectObject(hdc,hfold);
    }

    if (g_bPntChN) {
        // this will paint lines twice, but what the hell
        for (i = 0; i < max; i++) {
            for (i2 = 0; i2 < max; i2++) {
                if (i2 == i) continue;
                src1 = pvrc->at(i);
                src2 = pvrc->at(i2);
                if ((src1.cnum == src2.cnum) && RC_Same_RCB(&src1,&src2)) {
                    r1 = rcSquares[src1.row][src1.col];
                    r2 = rcSquares[src2.row][src2.col];
                    // simple line from r1 to r1
                    if (!g_bPntCtr) {
                        // or from candidate to candidate
                        // Adjust for margins
                        r1.left   += ht_margin;
                        r1.right  -= ht_margin;
                        r1.top    += vt_margin;
                        r1.bottom -= vt_margin;
                        r2.left   += ht_margin;
                        r2.right  -= ht_margin;
                        r2.top    += vt_margin;
                        r2.bottom -= vt_margin;

                        height = r1.bottom - r1.top;
                        width  = r1.right  - r1.left;
                        h3 = height / 3;
                        w3 = width / 3;
                        r1.right = r1.left + w3;
                        r1.bottom = r1.top + h3;
                        r2.right = r2.left + w3;
                        r2.bottom = r2.top + h3;
                        cnt = 0;
                        rc1 = r1;
                        rc2 = r2;
                        for (j = 0; j < 9; j++) {
                            if ( j && ((j % 3) == 0) ) {
                                // go down a line
                                cnt++;
                                rc1 = r1;
                                rc1.top += (h3 * cnt);
                                rc1.bottom = rc1.top + h3;
                                rc1.right = rc1.left + w3;
                                rc2 = r2;
                                rc2.top += (h3 * cnt);
                                rc2.bottom = rc2.top + h3;
                                rc2.right = rc2.left + w3;
                            }
                            if (j == val) break;
                            rc1.left  += w3;
                            rc1.right += w3;
                            rc2.left  += w3;
                            rc2.right += w3;
                        }
                        r1 = rc1;
                        r2 = rc2;
                    }
                    p1.x = (r1.left + r1.right) / 2;
                    p1.y = (r1.top + r1.bottom) / 2;
                    p2.x = (r2.left + r2.right) / 2;
                    p2.y = (r2.top + r2.bottom) / 2;
                    draw_line(hdc, &p1, &p2 );
                }
            }
        }

    } else {
        for (i = 0; i < max; i++) {
            i2 = i + 1;
            if (i2 >= max)
                break;
            src1 = pvrc->at(i);
            src2 = pvrc->at(i2);
            if ((src1.cnum == src2.cnum)&&(RC_Same_RCB(&src1,&src2))) {
                r1 = rcSquares[src1.row][src1.col];
                r2 = rcSquares[src2.row][src2.col];
                // simple line from r1 to r1
                if (!g_bPntCtr) {
                    // or from candidate to candidate
                    // Adjust for margins
                    r1.left   += ht_margin;
                    r1.right  -= ht_margin;
                    r1.top    += vt_margin;
                    r1.bottom -= vt_margin;
                    r2.left   += ht_margin;
                    r2.right  -= ht_margin;
                    r2.top    += vt_margin;
                    r2.bottom -= vt_margin;

                    height = r1.bottom - r1.top;
                    width  = r1.right  - r1.left;
                    h3 = height / 3;
                    w3 = width / 3;
                    r1.right = r1.left + w3;
                    r1.bottom = r1.top + h3;
                    r2.right = r2.left + w3;
                    r2.bottom = r2.top + h3;
                    cnt = 0;
                    rc1 = r1;
                    rc2 = r2;
                    for (j = 0; j < 9; j++) {
                        if ( j && ((j % 3) == 0) ) {
                            // go down a line
                            cnt++;
                            rc1 = r1;
                            rc1.top += (h3 * cnt);
                            rc1.bottom = rc1.top + h3;
                            rc1.right = rc1.left + w3;
                            rc2 = r2;
                            rc2.top += (h3 * cnt);
                            rc2.bottom = rc2.top + h3;
                            rc2.right = rc2.left + w3;
                        }
                        if (j == val) break;
                        rc1.left  += w3;
                        rc1.right += w3;
                        rc2.left  += w3;
                        rc2.right += w3;
                    }
                    r1 = rc1;
                    r2 = rc2;
                }
                p1.x = (r1.left + r1.right) / 2;
                p1.y = (r1.top + r1.bottom) / 2;
                p2.x = (r2.left + r2.right) / 2;
                p2.y = (r2.top + r2.bottom) / 2;
                draw_line(hdc, &p1, &p2 );
            }
        }
    }
    SelectObject(hdc,hpold);
}
#endif // #ifdef USE_COLOR2_SCAN

// ======================================
// X-Cycles

void paint_xcycles( HDC hdc, int setval )
{
    vRC *pvrc;
    int i;
    size_t max, j, k;
    ROWCOL rc, rc2;
    int count = 0;
    i = setval - 1;
    pvrc = get_XCycles_pvrc(i);
    max = pvrc->size();
    int row, col, box;
    int row2, col2, box2;
    PABOX2 pb = get_curr_box();
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    int bcnt, rcnt, ccnt;
    if (max < 4)
        return;
    for (j = 0; j < max - 1; j++) {
        k = j + 1;
        rc = pvrc->at(j);
        rc2 = pvrc->at(k);
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
    if (max > 2) {
        rc = pvrc->at(0);
        rc2 = pvrc->at(max - 1);
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
        draw_link(hdc, &rc, &rc2, i + 1, true);
        //count++;
    }
    sprtf("Painted %d XCycles links\n", count);
    set_repaint2(false);
}


void Paint_XCycles( HDC hdc )
{
    sprtf("Paint XCycles %d\n",on_xcycles_test);
    HPEN hpslnk = Get_SLnk_Pen();
    //HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = (HPEN)SelectObject(hdc,hpslnk);
    paint_xcycles(hdc, on_xcycles_test);
    SelectObject(hdc,hpold);
}

// Strong Links
void Paint_Strong_Links(HDC hdc)
{
    int ssl_cnt = Get_Rect_Link_Cnt();
    if (!g_bShwStLnks && !ssl_cnt) return;
    vRC *pvsl = (ssl_cnt ? Get_Rect_Links() : Get_Strong_Links() ); // { return &vStrong_Links; }
    size_t max = pvsl->size();
    if (!max) return;
    max /= 2;
    if (!max) return;
    size_t i, i2;
    ROWCOL rc1, rc2;
    int row, col, row2, col2, cnt;
    RECT rt1, rt2;
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    int uval, j;
    POINT pts[4];
    hpold = (HPEN)SelectObject(hdc,hpslnk);

    for (i = 0; i < max; i++) {
        i2 = i * 2; // stored as PAIRS
        rc1 = pvsl->at(i2 + 0);
        rc2 = pvsl->at(i2 + 1);
        Get_Set_Cnt( &rc1.set, &uval); // NOTE: Strong link pairs are stored with ONLY the link candidate value = 1 value
        // Also the rc.set.flag[setval - 1] set with cl_SLR[ow] and/or cl_SLC[ol]... if needed
        row = rc1.row;
        col = rc1.col;
        rt1 = *get_rcSquares(row,col);
        row2 = rc2.row;
        col2 = rc2.col;
        rt2 = *get_rcSquares(row2,col2);
        cnt = Get_Line_Points( &rt1, &rt2, pts, 4, uval );
        for (j = 0; j < (cnt / 2); j++) {
            draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
        }
    }

    SelectObject(hdc,hpold);
}

void Paint_Strong_Links2(HDC hdc)
{
    if (!Is_SLP_Valid()) return;
    int *psetvals = Get_Act_Setval();
    int setval, lval;
    vRCP *pvrcp = Get_SL_Pairs(); // TO Paint_Strong_Links2()
    size_t max = pvrcp->size();
    if (!max) return;
    size_t ii;
    PRCPAIR prcpair;
    RECT rt1, rt2;
    ROWCOL rc1, rc2;
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    int ij, cnt, k;
    POINT pts[4];
    hpold = (HPEN)SelectObject(hdc,hpslnk);
    bool one;
    int lncnt = 0;
    int setcnt = 0;
    for (ij = 0; ij < 9; ij++) {
        setval = psetvals[ij];
        if (!VALUEVALID(setval)) continue;
        setcnt++;
        lval = setval - 1;
        for (ii = 0; ii < max; ii++) {
            prcpair = &pvrcp->at(ii);
            rc1 = prcpair->rowcol[0];
            rc2 = prcpair->rowcol[1];
            if ( !(rc1.set.val[lval] && rc2.set.val[lval]) ) continue;

            rt1 = *get_rcSquares(rc1.row,rc1.col);
            rt2 = *get_rcSquares(rc2.row,rc2.col);
            cnt = Get_Line_Points( &rt1, &rt2, pts, 4, setval );
            if ((rc1.set.flag[lval] & cl_SLA) && (rc2.set.flag[lval] & cl_SLA)) {
                SelectObject(hdc,hpslnk);
                one = false;
            } else {
                SelectObject(hdc,hpwlnk);
                one = true;
            }
            for (k = 0; k < (cnt / 2); k++) {
                lncnt++;
                draw_line( hdc, &pts[k*2], &pts[(k*2)+1] );
                if (one) break;
            }
        }
    }
    SelectObject(hdc,hpold);
    sprtf("Paint_Strong_Links2: Drawn %d lines for %d setvals\n", lncnt, setcnt);
}


void Paint_Strong_Links3(HDC hdc)
{
    int setval = Get_SL_setval();
    if (!VALUEVALID(setval)) return;
    vRC *pvrc = Get_SL_Chain_for_setval(setval);
    if (!pvrc) return;
    size_t max = pvrc->size();
    if (max < 2) return;
    size_t ii, jj;
    PROWCOL prc1, prc2;
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    POINT pts[4];
    int box1, box2;
    RECT rt1, rt2;
    vRC vdone;
    int cnt, j;
    char *tb = GetNxtBuf();
    vRCP * pvrcp = Get_SL_FL_Pairs_for_setval(setval);
    if (pvrcp) {
        HBRUSH hboff = Get_Off_Brush();  // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
        HBRUSH hbon  = Get_On_Brush();  // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
        RECT rc;
        max = pvrcp->size();
        for (ii = 0; ii < max; ii++) {
            PRCPAIR prcpair = &pvrcp->at(ii);
            prc1 = &prcpair->rowcol[0];
            prc2 = &prcpair->rowcol[1];
            rc = *get_rcSquares(prc1->row,prc1->col);
            rc.left += 1;
            rc.right -= 1;
            rc.top += 1;
            rc.bottom -= 1;
            FillRect(hdc,&rc,hbon); // TO Paint_Strong_Links3() into rcSquares[row][col] OUTITPFR
            sprintf(tb,"Paint_Strong_Links3: ");
            sprintf(EndBuf(tb),"ON %s ",Get_RC_RC_Stg(prc1));
            sprintf(EndBuf(tb),"%s ",Get_Rect_Stg(&rc));
            rc = *get_rcSquares(prc2->row,prc2->col);
            rc.left += 1;
            rc.right -= 1;
            rc.top += 1;
            rc.bottom -= 1;
            FillRect(hdc,&rc,hboff); // TO Paint_Strong_Links3() into rcSquares[row][col] OUTITPFR
            sprintf(EndBuf(tb),"OFF %s ",Get_RC_RC_Stg(prc2));
            sprintf(EndBuf(tb),"%s ",Get_Rect_Stg(&rc));
            OUTITPFR(tb);
        }
    }
    hpold = (HPEN)SelectObject(hdc,hpslnk);
    max = pvrc->size();
    for (ii = 0; ii < max; ii++ ) {
        prc1 = &pvrc->at(ii);
        box1 = GETBOX(prc1->row,prc1->col);
        for (jj = 0; jj < max; jj++) {
            if (jj == ii) continue;
            prc2 = &pvrc->at(jj);
            box2 = GETBOX(prc2->row,prc2->col);
            if ( !((box1 == box2)||(prc1->row == prc2->row)||(prc1->col == prc2->col)) ) continue;
            rt1 = *get_rcSquares(prc1->row,prc1->col);
            rt2 = *get_rcSquares(prc2->row,prc2->col);
            cnt = Get_Line_Points( &rt1, &rt2, pts, 4, setval );
            for (j = 0; j < (cnt / 2); j++) {
                draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
            }
        }
    }

    SelectObject(hdc,hpold);

}
// =======================================================
// XY-Chains ========================================

void Paint_XY_Chains(HDC hdc)
{
    vRC *pvrcp = Get_XY_Chain(); // TO Paint_XY_Chains()
    if (!pvrcp) return;
    size_t max = pvrcp->size();
    if (max < 4) return;
    int bgnval = Get_XY_Bgn_Val();
    if (!VALUEVALID(bgnval)) return;
    size_t ii;
    PROWCOL prc1, prc2;
    PSET ps;
    int setvals[9];
    int cnt1, nxtval, cnt2, cnt, j;
    RECT rc, rc2;
    POINT pt1[4];
    POINT pt2[4];
    POINT pts[4];
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    HBRUSH hboff = Get_Off_Brush();  // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    HBRUSH hbon  = Get_On_Brush();  // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
    char *tb = GetNxtBuf();
    hpold = (HPEN)SelectObject(hdc,hpslnk);
    max = pvrcp->size();
    sprintf(tb,"Paint_XY_Chains: ");
    prc1 = &pvrcp->at(0); // get FIRST
    rc = *get_rcSquares(prc1->row,prc1->col);
    rc.left += 1;
    rc.right -= 1;
    rc.top += 1;
    rc.bottom -= 1;
    FillRect(hdc,&rc,hbon); // TO Paint_XY_Chains() into rcSquares[row][col]  OUTITPFR
    sprintf(EndBuf(tb),"ON %s ",Get_RC_RC_Stg(prc1));
    sprintf(EndBuf(tb),"%s ",Get_Rect_Stg(&rc));
    prc1 = &pvrcp->at(max-1); // get LAST
    rc = *get_rcSquares(prc1->row,prc1->col);
    rc.left += 1;
    rc.right -= 1;
    rc.top += 1;
    rc.bottom -= 1;
    FillRect(hdc,&rc,hboff); // TO Paint_XY_Chains() into rcSquares[row][col] OUTITPFR
    sprintf(EndBuf(tb),"and OFF %s ",Get_RC_RC_Stg(prc1));
    sprintf(EndBuf(tb),"%s ",Get_Rect_Stg(&rc));
    OUTITPFR(tb);
    prc2 = 0;
    for (ii = 0; ii < max; ii++) {
        prc1 = &pvrcp->at(ii);
        ps = &prc1->set;
        if (Get_Set_Cnt2(ps, setvals) != 2) continue;
        if (bgnval == setvals[0])
            nxtval = setvals[1];
        else if (bgnval == setvals[1])
            nxtval = setvals[0];
        else continue;
        
        // paint short line from bgn to nxt
        rc = *get_rcSquares(prc1->row,prc1->col);
        cnt1 = Get_Line_Point( &rc, pt1, 2, bgnval );
        rc = *get_rcSquares(prc1->row,prc1->col);
        cnt2 = Get_Line_Point( &rc, pt2, 2, nxtval );
        cnt = (cnt1 < cnt2) ? cnt1 : cnt2;
        for (j = 0; j < cnt; j++) {
            draw_line( hdc, &pt1[j], &pt2[j] );
        }
        if (prc2) {
            rc = *get_rcSquares(prc1->row,prc1->col);
            rc2 = *get_rcSquares(prc2->row,prc2->col);
            cnt = Get_Line_Points( &rc, &rc2, pts, 4, bgnval );
            for (j = 0; j < (cnt / 2); j++) {
                draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
            }
        }

        bgnval = nxtval;
        prc2 = prc1;
    }
    SelectObject(hdc,hpold);
}

HBRUSH Get_Fill_Color( uint64_t flag )
{
    uint64_t flg = flag & cf_XAB;   // isolate COLOR bits
    HBRUSH hbfill = 0;
    if ( flg == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
        hbfill = Get_Sel_Brush();
    } else if (flg & cf_XCA) {
        hbfill = Get_On_Brush();
    } else if (flg & cf_XCB) {
        hbfill = Get_Off_Brush();
    } else {
        hbfill = Get_UnRect_Brush();
    }
    return hbfill;
}

#define OUTITPXY2(tb) if (add_debug_pxy2) OUTIT(tb)

void Paint_XY_Chain2(HDC hdc)
{
    if (!Get_XY_Elim_Count()) {
        if (add_debug_pxy2) sprtf("Paint_XY_Chain2: NO elim count!\n");
        return;
    }
    vRC *pvrcp = Get_XY_Act_Chain(); // TO Paint_XY_Chains2()
    if (!pvrcp) {
        if (add_debug_pxy2) sprtf("Paint_XY_Chain2: NO act chain pointer!\n");
        return;
    }
    size_t max = pvrcp->size();
    if (max < 4) {
        if (add_debug_pxy2) sprtf("Paint_XY_Chain2: Count LESS THAN 4 = %d!\n", (int)max );
        return;
    }
    int setval = Get_XY_setval();
    if (!VALUEVALID(setval)) {
        if (add_debug_pxy2) sprtf("Paint_XY_Chain2: Invalid setval %d!\n", setval);
        return;
    }
    // PAINT the CHAIN
    size_t ii, pcnt;
    PROWCOL prc1, prc2;
    PSET ps;
    int setvals[9];
    int cnt1, nxtval, cnt2, cnt, j;
    RECT rc, rc2;
    POINT pt1[4];
    POINT pt2[4];
    POINT pts[4];
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    HBRUSH hboff = Get_Off_Brush();  // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    HBRUSH hbon  = Get_On_Brush();  // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
    char *tb = GetNxtBuf();
    hpold = (HPEN)SelectObject(hdc,hpslnk);
    max = pvrcp->size();
    sprintf(tb,"Paint_XY_Chains2: cnt %d setval %d ", (int)max, setval);
    prc1 = &pvrcp->at(0); // get FIRST
    rc = *get_rcSquares(prc1->row,prc1->col);
    rc.left += 1;
    rc.right -= 1;
    rc.top += 1;
    rc.bottom -= 1;
    FillRect(hdc,&rc,hbon); // TO Paint_XY_Chains() into rcSquares[row][col]  OUTITPFR
    sprintf(EndBuf(tb),"ON %s ",Get_RC_setval_RC_Stg(prc1));
    sprintf(EndBuf(tb),"%s ",Get_Rect_Stg(&rc));
    prc1 = &pvrcp->at(max-1); // get LAST
    rc = *get_rcSquares(prc1->row,prc1->col);
    rc.left += 1;
    rc.right -= 1;
    rc.top += 1;
    rc.bottom -= 1;
    FillRect(hdc,&rc,hboff); // TO Paint_XY_Chains() into rcSquares[row][col] OUTITPFR
    sprintf(EndBuf(tb),"and OFF %s ",Get_RC_setval_RC_Stg(prc1));
    sprintf(EndBuf(tb),"%s ",Get_Rect_Stg(&rc));
    OUTITPXY2(tb);

    prc2 = 0;
    sprintf(tb,"Paint: ");
    pcnt = 0;
    for (ii = 0; ii < max; ii++) {
        prc1 = &pvrcp->at(ii);
        sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc1));
        ps = &prc1->set;
        cnt = Get_Set_Cnt2(ps, setvals);
        if (cnt != 2) {
            if (add_debug_pxy2) sprtf("Paint_XY_Chain2: %s - Cnt %d NOT 2!\n",Get_RC_setval_RC_Stg(prc1), cnt);
            continue;
        }
        if (setval == setvals[0])
            nxtval = setvals[1];
        else if (setval == setvals[1])
            nxtval = setvals[0];
        else {
            if (add_debug_pxy2) sprtf("Paint_XY_Chain2: %s - setval %d NOT in list!\n",Get_RC_setval_RC_Stg(prc1), setval);
            continue;
        }

        // paint short line from bgn to nxt
        rc = *get_rcSquares(prc1->row,prc1->col);
        cnt1 = Get_Line_Point( &rc, pt1, 2, setval );
        rc = *get_rcSquares(prc1->row,prc1->col);
        cnt2 = Get_Line_Point( &rc, pt2, 2, nxtval );
        cnt = (cnt1 < cnt2) ? cnt1 : cnt2;
        for (j = 0; j < cnt; j++) {
            draw_line( hdc, &pt1[j], &pt2[j] );
        }
        if (prc2) {
            rc = *get_rcSquares(prc1->row,prc1->col);
            rc2 = *get_rcSquares(prc2->row,prc2->col);
            cnt = Get_Line_Points( &rc, &rc2, pts, 4, setval );
            for (j = 0; j < (cnt / 2); j++) {
                draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
            }
        }

        setval = nxtval;
        prc2 = prc1;
        pcnt++;
    }

    SelectObject(hdc,hpold);
    if (pcnt < max) {
        OUTITPXY2(tb);
        sprintf(tb,"WARNING: Painted only %d of %d! WHY!!!", (int)pcnt, (int)max);
    }
    OUTITPXY2(tb);

}
// ========================================================
// Strong/Weak Links
BOOL g_bFillWL = FALSE; // this clutters the image
void Paint_SW_Links( HDC hdc )
{
    int setval = Get_Pnt_Lnks_setval();
    if (!VALUEVALID(setval))
        return;
    vRCP *pvsl = Get_Pnt_SL();
    vRCP *pvwl = Get_Pnt_WL();
    if (!pvsl || !pvwl || !pvsl->size() || !pvwl->size())
        return;
    PROWCOL prc1, prc2;
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    POINT pts[4];
    RECT rc, rt1, rt2;
    PRCPAIR pp;
    int cnt, j;

    HBRUSH hbsel = Get_Sel_Brush(); // Get_Solid_Brush(crPBlue);
    HBRUSH hboff = Get_Off_Brush();  // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    HBRUSH hbon  = Get_On_Brush();  // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
    HBRUSH hbunrect = Get_UnRect_Brush(); // Get_Solid_Brush(crBYellow); // = RG(248,252,121);
    HBRUSH hbfill = NULL;
    uint64_t flg;

    hpold = (HPEN)SelectObject(hdc,hpslnk);
    size_t ii, max;

    if (g_bFillWL) {
        max = pvwl->size();
        for (ii = 0; ii < max; ii++ ) {
            pp = &pvwl->at(ii);
            // one end of pair
            prc1 = &pp->rowcol[0];
            flg = prc1->set.flag[setval - 1] & cf_XAB;
            hbfill = 0;
            if (flg) {
                if ( flg == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
                    hbfill = hbsel;
                } else if (flg & cf_XCA) {
                    hbfill = hbon;
                } else if (flg & cf_XCB) {
                    hbfill = hboff;
                } else {
                    //strcat(tb,"Clr UNK ");
                }
            } else {
                hbfill = hbunrect;
                //strcat(tb,"NF ");
            }
            if (hbfill) {
                rc = *get_rcSquares(prc1->row,prc1->col);
                rc.left += 1;
                rc.right -= 1;
                rc.top += 1;
                rc.bottom -= 1;
                FillRect(hdc,&rc,hbfill); // TO Paint_SW_Links() into rcSquares[row][col]
            }
            // other end of pair
            prc2 = &pp->rowcol[1];
            flg = prc2->set.flag[setval - 1] & cf_XAB;
            hbfill = 0;
            if (flg) {
                if ( flg == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
                    hbfill = hbsel;
                } else if (flg & cf_XCA) {
                    hbfill = hbon;
                } else if (flg & cf_XCB) {
                    hbfill = hboff;
                } else {
                    //strcat(tb,"Clr UNK ");
                }
            } else {
                hbfill = hbunrect;
                //strcat(tb,"NF ");
            }
            if (hbfill) {
                rc = *get_rcSquares(prc2->row,prc2->col);
                rc.left += 1;
                rc.right -= 1;
                rc.top += 1;
                rc.bottom -= 1;
                FillRect(hdc,&rc,hbfill); // TO Paint_SW_Links() into rcSquares[row][col]
            }
        }
    }

    max = pvsl->size();
    for (ii = 0; ii < max; ii++ ) {
        pp = &pvsl->at(ii);
        // one end of pair
        prc1 = &pp->rowcol[0];
        flg = prc1->set.flag[setval - 1] & cf_XAB;
        hbfill = 0;
        if (flg) {
            if ( flg == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
                hbfill = hbsel;
            } else if (flg & cf_XCA) {
                hbfill = hbon;
            } else if (flg & cf_XCB) {
                hbfill = hboff;
            } else {
                //strcat(tb,"Clr UNK ");
            }
        } else {
            hbfill = hbunrect;
            //strcat(tb,"NF ");
        }
        if (hbfill) {
            rc = *get_rcSquares(prc1->row,prc1->col);
            rc.left += 1;
            rc.right -= 1;
            rc.top += 1;
            rc.bottom -= 1;
            FillRect(hdc,&rc,hbfill); // TO Paint_SW_Links() into rcSquares[row][col]
        }
        // other end of pair
        prc2 = &pp->rowcol[1];
        flg = prc2->set.flag[setval - 1] & cf_XAB;
        hbfill = 0;
        if (flg) {
            if ( flg == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
                hbfill = hbsel;
            } else if (flg & cf_XCA) {
                hbfill = hbon;
            } else if (flg & cf_XCB) {
                hbfill = hboff;
            } else {
                //strcat(tb,"Clr UNK ");
            }
        } else {
            hbfill = hbunrect;
            //strcat(tb,"NF ");
        }
        if (hbfill) {
            rc = *get_rcSquares(prc2->row,prc2->col);
            rc.left += 1;
            rc.right -= 1;
            rc.top += 1;
            rc.bottom -= 1;
            FillRect(hdc,&rc,hbfill); // TO Paint_SW_Links() into rcSquares[row][col]
        }
    }

    for (ii = 0; ii < max; ii++ ) {
        pp = &pvsl->at(ii);
        prc1 = &pp->rowcol[0];
        prc2 = &pp->rowcol[1];
        rt1 = *get_rcSquares(prc1->row,prc1->col);
        rt2 = *get_rcSquares(prc2->row,prc2->col);
        cnt = Get_Line_Points( &rt1, &rt2, pts, 4, setval );
        for (j = 0; j < (cnt / 2); j++) {
            draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
        }
    }

    SelectObject(hdc,hpwlnk);
    max = pvwl->size();
    for (ii = 0; ii < max; ii++ ) {
        pp = &pvwl->at(ii);
        prc1 = &pp->rowcol[0];
        prc2 = &pp->rowcol[1];
        rt1 = *get_rcSquares(prc1->row,prc1->col);
        rt2 = *get_rcSquares(prc2->row,prc2->col);
        cnt = Get_Line_Points( &rt1, &rt2, pts, 4, setval );
        for (j = 0; j < (cnt / 2); j++) {
            draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
        }
    }

    SelectObject(hdc,hpold);

}

// ========================================================
// Paint Unique Rects
void Paint_UniqRects(HDC hdc)
{
    int cnt = Get_UR_Cnt(); // { return ur_rect_cnt; }
    if (!cnt)
        return;
    vRCR *pvrcr = Get_UR_Rects2_Ptr(); // { return &ur_rects2; }
    if (!pvrcr)
        return;
    size_t max = pvrcr->size();
    if (!max)
        return;

    size_t ii;
    PRCRECT prcr;
    PRCPAIR prcp;
    PROWCOL prc1, prc2;
    HBRUSH hb = Get_Off_Brush();
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    POINT pts[4];
    RECT rc1, rc2;
    int i, j, k, scnt, setval, lval;
    int setvals[9];
    vRC vrc;
    bool one;
    uint64_t flg;

    hpold = (HPEN)SelectObject(hdc,hpwlnk);

    for (ii = 0; ii < max; ii++) {
        prcr = &pvrcr->at(ii);
        for (i = 0; i < 4; i++) {
            prcp = &prcr->rcpair[i];
            scnt = Get_Set_Cnt2(&prcr->set,setvals);
            lval = 0;
            if (scnt) {
                setval = setvals[0];
                lval = setval - 1;
            }
            prc1 = &prcp->rowcol[0];
            prc2 = &prcp->rowcol[1];
            rc1 = *get_rcSquares(prc1->row,prc1->col);
            rc2 = *get_rcSquares(prc2->row,prc2->col);
            if (!prc_in_chain(prc1,&vrc)) {
                flg = prc1->set.flag[lval];
                hb = Get_Fill_Color(flg);
                FillRect( hdc, &rc1, hb );
                vrc.push_back(*prc1);
            }
            if (!prc_in_chain(prc2,&vrc)) {
                flg = prc2->set.flag[lval];
                hb = Get_Fill_Color(flg);
                FillRect( hdc, &rc2, hb );
                vrc.push_back(*prc2);
            }
            for (k = 0; k < scnt; k++) {
                setval = setvals[k];
                lval = setval - 1;
                cnt = Get_Line_Points( &rc1, &rc2, pts, 4, setval );
                one = false;
                if ( (prc1->set.flag[lval] & cl_SLA) && (prc2->set.flag[lval] & cl_SLA) ) {
                    SelectObject(hdc,hpslnk);
                } else {
                    one = true;
                    SelectObject(hdc,hpwlnk);
                }
                for (j = 0; j < (cnt / 2); j++) {
                    draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
                    if (one)
                        break;
                }
            }
        }
    }

    SelectObject(hdc,hpold);
}

// ========================================================
// Paint AIC - Alternating Inference Chains
// Extension of X-Cycles where any candidate can be used
int max_append_aic = 7; //max before NEW line for display - optimized for 1024

#define OUTITPAIC(tb) if (add_debug_paic) OUTIT(tb)

void Fill_RC_setval( HDC hdc, PROWCOL prc1, int setval, HBRUSH hbsel, HBRUSH hbon, HBRUSH hboff, HBRUSH hbunrect )
{
    uint64_t flg;
    HBRUSH hbfill;
    RECT rc;
    if (VALUEVALID(setval)) {
        flg = prc1->set.flag[setval - 1] & cf_XAB;
        hbfill = 0;
        if (flg) {
            if ( flg == (cf_XCA|cf_XCB) ) { // BOTH COLORS ON - eek
                hbfill = hbsel;
            } else if (flg & cf_XCA) {
                hbfill = hbon;
            } else if (flg & cf_XCB) {
                hbfill = hboff;
            } else {
                //strcat(tb,"Clr UNK ");
            }
        } else {
            hbfill = hbunrect;
            //strcat(tb,"NF ");
        }
        if (hbfill) {
            rc = *get_rcSquares(prc1->row,prc1->col);
            rc.left += 1;
            rc.right -= 1;
            rc.top += 1;
            rc.bottom -= 1;
            FillRect(hdc,&rc,hbfill); // TO Paint_SW_Links() into rcSquares[row][col]
        }
    }
}

void Fill_RC( HDC hdc, PROWCOL prc1, HBRUSH hbsel, HBRUSH hbon, HBRUSH hboff, HBRUSH hbunrect)
{
    int setval = prc1->cnum;
    if (VALUEVALID(setval)) {
        Fill_RC_setval( hdc, prc1, setval, hbsel, hbon, hboff, hbunrect );
    }
}

void Fill_RCPAIR( HDC hdc, PROWCOL prc1, PROWCOL prc2,
    HBRUSH hbsel, HBRUSH hbon, HBRUSH hboff, HBRUSH hbunrect)
{
    Fill_RC( hdc, prc1, hbsel, hbon, hboff, hbunrect );
    Fill_RC( hdc, prc2, hbsel, hbon, hboff, hbunrect );
}


void Paint_One_AIC_Chain(HDC hdc, vRCP *pvrcp, int scnt, int count)
{
    size_t max = pvrcp->size();
    if (!max) return;
    size_t ii;
    PRCPAIR prcp;
    PROWCOL prc1, prc2;
    int setval1, setval2;
    uint64_t flg1, flg2;
    RECT rc1, rc2;
    int cnt, cnt1, cnt2, j;
    bool one;
    POINT pt1[4];
    POINT pt2[4];
    POINT pts[4];
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    HBRUSH hbsel = Get_Sel_Brush(); // Get_Solid_Brush(crPBlue);
    HBRUSH hboff = Get_Off_Brush();  // Get_Solid_Brush(crLOrange2); // = RGB(255,238,221);
    HBRUSH hbon  = Get_On_Brush();  // Get_Solid_Brush(crLPink);    // = RGB(255,192,255);
    HBRUSH hbunrect = Get_UnRect_Brush(); // Get_Solid_Brush(crBYellow); // = RG(248,252,121);
    HBRUSH hbfill = NULL;
    vRC filled;
    char *tb = GetNxtBuf();

    hpold = (HPEN)SelectObject(hdc,hpwlnk);
    // got a pair to paint
    int pcnt = 0;
    sprintf(tb,"AIC P%d:%d: %d ", scnt, count, (int)max);
    for (ii = 0; ii < max; ii++) {
        prcp = &pvrcp->at(ii);
        prc1 = &prcp->rowcol[0];
        prc2 = &prcp->rowcol[1];
        setval1 = prc1->cnum;
        setval2 = prc2->cnum;
        flg1 = prc1->set.flag[setval1 - 1];
        flg2 = prc1->set.flag[setval2 - 1];
        rc1 = *get_rcSquares(prc1->row,prc1->col);
        rc2 = *get_rcSquares(prc2->row,prc2->col);
        if (setval1 == setval2) {
            if (!SAMERCCELL(prc1,prc2)) {
                if (!prc_in_chain( prc1, &filled )) {
                    Fill_RC_setval( hdc, prc1, setval1, hbsel, hbon, hboff, hbunrect );
                    filled.push_back(*prc1);
                }
                if (!prc_in_chain( prc2, &filled )) {
                    Fill_RC_setval( hdc, prc2, setval2, hbsel, hbon, hboff, hbunrect );
                    filled.push_back(*prc2);
                }
            }
            if (add_debug_paic) {
                if (pcnt == 0) ADDSP(tb);
                Append_RCPAIR_Stg(tb,prcp,setval1);
                ADDSP(tb);
                pcnt++;
                if (pcnt >= max_append_aic) {
                    OUTIT(tb);
                    pcnt = 0;
                }
            }
            cnt = Get_Line_Points( &rc1, &rc2, pts, 4, setval1 );
            if (flg1 & cf_XCA) {
                SelectObject(hdc,hpslnk);
                one = false;
            } else {
                SelectObject(hdc,hpwlnk);
                one = true;
            }
            for (j = 0; j < (cnt / 2); j++) {
                draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
                if (one)
                    break;
            }
        } else {
            // short line between setvals only
            if (add_debug_paic) {
                if (pcnt == 0) ADDSP(tb);
                Append_RC_Stg(tb,prc1,setval1);
                strcat(tb,"-");
                Append_RC_Stg(tb,prc2,setval2);
                ADDSP(tb);
                pcnt++;
                if (pcnt >= max_append_aic) {
                    OUTIT(tb);
                    pcnt = 0;
                }
            }

            SelectObject(hdc,hpwlnk);
            one = true;
            cnt1 = Get_Line_Point( &rc1, pt1, 2, setval1 );
            cnt2 = Get_Line_Point( &rc2, pt2, 2, setval2 );
            cnt = (cnt1 < cnt2) ? cnt1 : cnt2;
            for (j = 0; j < cnt; j++) {
                draw_line( hdc, &pt1[j], &pt2[j] );
                if (one)
                    break;
            }
        }   // if (setval1 == setval2) ... else ...
    }   // for (ii = 0; ii < max; ii++) - for this chain
    if (pcnt) OUTIT(tb);
    SelectObject(hdc,hpold);

}


void Paint_AIC_Chains(HDC hdc)
{
    if (!are_AIC_chains_valid() ) return;
    int count = Get_AIC_chain_count();
    if (!count) return;
    vRCP **ppvrcp = Get_AIC_chains();
    if (!ppvrcp) return;
    vRCP *pvrcp;
    size_t max;
    int scnt, i;
    char *tb = GetNxtBuf();

    sprintf(tb,"AIC Paint %d chains", count);
    OUTITPAIC(tb);
    scnt = 0;
    for (i = 0; i < count; i++) {
        scnt++;
        pvrcp = ppvrcp[i];
        if (!pvrcp) continue;
        max = pvrcp->size();
        if (!max) continue;
        // got a chain of pairs to paint
        Paint_One_AIC_Chain(hdc, pvrcp, scnt, count);
    }   // for (i = 0; i < count; i++) - separate chains
}

void Paint_AIC_Chains_OK(HDC hdc)
{
    if (!are_AIC_chains_valid() ) return;
    int count = Get_AIC_chain_count();
    if (!count) return;
    vRCP **ppvrcp = Get_AIC_chains();
    if (!ppvrcp) return;
    vRCP *pvrcp;
    int i, setval1, setval2, cnt, j;
    size_t max, ii;
    PROWCOL prc1, prc2;
    PRCPAIR prcp;
    HPEN hpslnk = Get_SLnk_Pen();
    HPEN hpwlnk = Get_WLnk_Pen();
    HPEN hpold = 0;
    POINT pts[4];
    RECT rc1, rc2;
    uint64_t flg1, flg2;
    bool one;
    int cnt1, cnt2, pcnt, scnt;
    POINT pt1[4];
    POINT pt2[4];
    char *tb = GetNxtBuf();

    hpold = (HPEN)SelectObject(hdc,hpwlnk);
    one = true;
    sprintf(tb,"AIC Paint %d chains", count);
    OUTITPAIC(tb);
    scnt = 0;
    pcnt = 0;
    for (i = 0; i < count; i++) {
        scnt++;
        pvrcp = ppvrcp[i];
        if (!pvrcp) continue;
        max = pvrcp->size();
        if (!max) continue;
        // got a pair to paint
        pcnt = 0;
        sprintf(tb,"AIC P%d:%d: %d ", scnt, count, (int)max);
        for (ii = 0; ii < max; ii++) {
            prcp = &pvrcp->at(ii);
            prc1 = &prcp->rowcol[0];
            prc2 = &prcp->rowcol[1];
            setval1 = prc1->cnum;
            setval2 = prc2->cnum;
            flg1 = prc1->set.flag[setval1 - 1];
            flg2 = prc1->set.flag[setval2 - 1];
            rc1 = *get_rcSquares(prc1->row,prc1->col);
            rc2 = *get_rcSquares(prc2->row,prc2->col);
            if (setval1 == setval2) {
                if (add_debug_paic) {
                    if (pcnt == 0) ADDSP(tb);
                    Append_RCPAIR_Stg(tb,prcp,setval1);
                    ADDSP(tb);
                    pcnt++;
                    if (pcnt >= max_append_aic) {
                        OUTIT(tb);
                        pcnt = 0;
                    }
                }
                cnt = Get_Line_Points( &rc1, &rc2, pts, 4, setval1 );
                if (flg1 & cf_XCA) {
                    SelectObject(hdc,hpslnk);
                    one = false;
                } else {
                    SelectObject(hdc,hpwlnk);
                    one = true;
                }
                for (j = 0; j < (cnt / 2); j++) {
                    draw_line( hdc, &pts[j*2], &pts[(j*2)+1] );
                    if (one)
                        break;
                }
            } else {
                // short line between setvals only
                if (add_debug_paic) {
                    if (pcnt == 0) ADDSP(tb);
                    Append_RC_Stg(tb,prc1,setval1);
                    strcat(tb,"-");
                    Append_RC_Stg(tb,prc2,setval2);
                    ADDSP(tb);
                    pcnt++;
                    if (pcnt >= max_append_aic) {
                        OUTIT(tb);
                        pcnt = 0;
                    }
                }

                SelectObject(hdc,hpwlnk);
                one = true;
                cnt1 = Get_Line_Point( &rc1, pt1, 2, setval1 );
                cnt2 = Get_Line_Point( &rc2, pt2, 2, setval2 );
                cnt = (cnt1 < cnt2) ? cnt1 : cnt2;
                for (j = 0; j < cnt; j++) {
                    draw_line( hdc, &pt1[j], &pt2[j] );
                    if (one)
                        break;
                }
            }   // if (setval1 == setval2) ... else ...
        }   // for (ii = 0; ii < max; ii++) - for this chain
        if (pcnt) OUTIT(tb);
        pcnt = 0;
    }   // for (i = 0; i < count; i++) - separate chains
    SelectObject(hdc,hpold);
    if (pcnt) OUTIT(tb);
}

// ========================================================

// ========================================================
void Paint_Colors( HDC hdc )
{
#ifdef USE_COLOR2_SCAN
    if (g_bPntCC && Is_Chain_Valid2()) 
        Paint_Chain2(hdc);
#else // #ifdef USE_COLOR2_SCAN
    if (g_bPntCC && Is_Chain_Valid()) 
        Paint_Chain(hdc);
#endif // #ifdef USE_COLOR2_SCAN

    if (on_xcycles_test)
        Paint_XCycles(hdc);

    if (do_color_test2)
        Paint_Color_Test2(hdc);

    Paint_Strong_Links(hdc);
    Paint_Strong_Links2(hdc);
    Paint_Strong_Links3(hdc);
    Paint_XY_Chains(hdc);
    Paint_XY_Chain2(hdc);
    Paint_SW_Links(hdc);
    Paint_UniqRects(hdc);
    Paint_AIC_Chains(hdc);

}
// ========================================================


////////////////////////////////////////////////////////////////////////////////////
#else // #ifdef WIN32
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32

// eof - Sudo_PntColors.cxx

