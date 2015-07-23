// Sudo_XYZWings.cxx

#include "Sudoku.hxx"

#define xyz_elim cf_XYE
#define xyz_mark cf_XY
#define xyz_color   cl_MCC
#define xyz_clr1    cl_CLR1
#define xyz_clr2    cl_CLR2
#define xyz_clrs (cl_MCC|cl_CLR1|cl_CLR2)
//#define xyz_on  1
//#define xyz_off 0
#define OUTITXYZ(tb) if( *tb && add_debug_xyz ) OUTIT(tb)

int Mark_PB_per_SET( PABOX2 pb, PROWCOL prc, PSET ps, uint64_t mark, bool all = false );
int Mark_PB_per_SET( PABOX2 pb, PROWCOL prc, PSET ps, uint64_t mark, bool all )
{
    int count = 0;
    int row = prc->row;
    int col = prc->col;
    if (!VALIDRC(row,col)) return 0;
    int i, val;
    for (i = 0; i < 9; i++) {
        val = ps->val[i];
        if (!val) continue;
        if (all || !(pb->line[row].set[col].flag[ val - 1 ] & mark)) {
            pb->line[row].set[col].flag[ val - 1 ] |= mark;
            count++;
        }
    }
    return count;
}


// XYZ-Wings
// from : http://www.sadmansoftware.com/sudoku/xyzwing.htm
// This is a variation of an XY-wing

// from : http://www.brainbashers.com/sudokuxyzwing.asp
// An XYZ-Wing is a very advanced technique which can eliminate 
// certain candidates from squares and is a very similar technique to XY-Wing.
// We are looking for three squares. This time the first square contains XYZ 
// and we require a buddy of this square to contain XZ. We then require another 
// buddy of the XYZ square to contain YZ.
// Any squares that are buddies to all three squares, XYZ, XZ & YZ cannot contain Z. 
// Why? Because no matter which way around the numbers go, the Z will be in one of 
// the three squares. 
// see examples\xyzwing2.txt

// from : http://www.sudokuwiki.org/XYZ_Wing
// This is an extension of Y-Wing or (XY-Wing). John MacLeod defines one as 
// "three cells that contain only 3 different numbers between them, but which fall 
// outside the confines of one row/column/box, with one of the cells (the 'apex' or 'hinge') 
// being able to see the other two; those other two having only one number in common; and 
// the apex having all three numbers as candidates."
// It follows therefore that one or other of the three cells must contain the common 
// number; and hence any extraneous cell (there can only be two of them!) that "sees" all 
// three cells of the Extended Trio cannot have that number as its true value. 

// from : http://www.sudoku9x9.com/xyz_wing.html
// XZ@A1(Y) XYZ@B2(R) YZ@B5(G) eliminate Z from B1 and B3 (Blue)
// The yellow(Y) cell XZ shares a common 3x3 box with the red(R) cell XYZ 
// and the green(G) cell YZ shares a common row with the red cell XYZ. 
// These three cells form a XYZ-Wing pattern. If the red cell is X, then the 
// yellow cell must be Z and therefore the blue cells cannot be Z. If the red 
// cell is Y, then the green cell must be Z and the blue cells cannot be Z. 
// If the red cell itself is Z, then again the blue cells cannot be Z. So in any 
// of these three cases, the blue cells cannot be Z.
// Note that this technique also works if the green cell YZ shares the common 
// column with the red cell XYZ.

// from : http://www.sudoku9981.com/sudoku-solving/xyz-wing.php
// The XYZ-Wing is an extension of the XY-Wing. The pivot cell also carries the Z candidate. 
// Upto 2 candidates can be eliminated by an XYZ-Wing, because they need to 
// share an intersection with the pivot. The following diagram shows how it works: 
// ie elims in A1 and A2 due XYZ@A3 XZ@C1 YZ@A7
// The pivot has candidates XYZ. The implications of each option are: X 
// the XZ pincer will contain digit Z. This digit is eliminated from the starred cells. 
// Y the YZ pincer will contain digit Z. This digit is eliminated from the starred cells. 
// Z the pivot eliminates Z in the starred cells. 
// Under all circumstances, the starred cells will lose their candidates for digit Z. 
// ALS Alternative
// The XYZ-Wing can be replicated by an ALS-XZ move. 
// This is the Eureka notation for the ALS alternative: 
// (Z)r1c12-(Z=YX)r1c37-(X=Z)r3c1-(Z)r1c12 => r1c12<>Z
// XYZ-Wings can also be replicated by Aligned Pair Exclusion, by pairing one of 
// the target cells with the XYZ cell. 

// from : http://www.sudokuonline.us/xyz-wing.php = similar

// from : http://www.sudoku.org.uk/SolvingTechniques/XYZ-Wing.asp
// XYZ-Wing Strategy 
// This extends Y-Wings which have three bi-value cells to bi-tri-bi, 
// in other words the hinge contains three candidates, not two.  
 
// from : http://www.intosudoku.com/Doc/XYZWing.html - similar

// from : http://sudoku.oubk.com/Techniques/XYZ-Wing-Technique.html
// As a variation of an XY-Wing, XYZ-Wing is a more advanced technique. 
// Any cells that share a unit with all three cells XYZ, XY and YZ, can 
// have Z eliminated from their candidates. 

// etc...

// So find and empty cell with 3 candidate remaining, and check it 
// it has 'buddies' XZ and YZ sharing the BOX, AND a ROW or COL
// 1 - get all cells with 3 candidates remaining
// 2 - get all cells with 2 candidates remaining
// 3 - Scan thought the XYZ set, and seek XZ and YZ buddies
int Process_XYZ_Wings( PABOX2 pb, vRC *pelims, vRC *pelimby )
{
    int count = 0;
    vRC emp3;   // get empty cells with 3 candidates remaining
    int cnt3 = Get_Empty_Cells_per_cand_count( pb, &emp3, 3 );
    if (!cnt3) return 0;
    vRC emp2;
    RCRCB rcrcb2;   // get empty cells with 2 candidates remaining
    int cnt2 = Get_Empty_Cells_per_cand_count( pb, &emp2, 2, &rcrcb2 );
    if (cnt2 < 2) return 0;
    vRC empty;
    RCRCB rcrcb;    // get ALL empty cells
    int cnt = Get_Empty_Cells( pb, empty, &rcrcb );
    vRC *pemp3 = &emp3;
    vRC *pemp2 = &emp2;
    vRC *pempty = &empty;

    size_t ii, max2, i2, max3, i3, max4, i4;
    size_t max = pemp3->size();
    PROWCOL prc, prc2, prc3, prce;
    vRC *prow, *pcol, *pbox;
    vRC *pboxa;
    PSET ps, ps2, ps3, pse;
    SET comm, comm2, comm3;
    PRCRCB prcb2 = &rcrcb2;
    PRCRCB prcball = &rcrcb;
    int eval;
    char *tb = GetNxtBuf();
    // scan EACH empty with 3 candidates
    for (ii = 0; ii < max; ii++) {
        prc = &pemp3->at(ii);
        ps  = &prc->set;    // got XYZ
        pbox = &prcb2->vboxs[prc->box]; // get SAME BOX buddies with two
        prow = &prcb2->vrows[prc->row];
        pcol = &prcb2->vcols[prc->col];
        max2 = pbox->size();
        sprintf(tb,"XZY: %s ", Get_RC_setval_RC_Stg(prc));
        // find in same BOX a buddy SHARING 2 candidates - XZ or YZ
        for (i2 = 0; i2 < max2; i2++) {
            prc2 = &pbox->at(i2);
            ps2 = &prc2->set;
            ZERO_SET(&comm);
            cnt2 = Get_Shared_SET( ps, ps2, &comm);
            if (cnt2 != 2) continue;
            sprintf(EndBuf(tb),"BoxBud %s ",Get_RC_setval_RC_Stg(prc2));
            // got XYZ, and now XZ or YZ
            // search same ROW or COL, outside BOX for second buddy
            max3 = prow->size();
            for (i3 = 0; i3 < max3; i3++) {
                prc3 = &prow->at(i3);
                if (prc3->box == prc->box) continue;
                ps3 = &prc3->set;
                ZERO_SET(&comm2);
                cnt3 = Get_Shared_SET( ps, ps3, &comm2);
                if (cnt3 != 2) continue;
                // a potential buddy - but does it share 1 with the other buddy
                ZERO_SET(&comm3);
                cnt3 = Get_Shared_SET( &comm, &comm2, &comm3 );
                if (cnt3 != 1) continue;
                cnt3 = Get_Set_Cnt(&comm3,&eval); // get ELIMINATION value 
                if (cnt3 != 1) continue; // this would be an ERROR
                sprintf(EndBuf(tb),"RowBud %s ",Get_RC_setval_RC_Stg(prc3));
                // GOT and XYZ-Wing - comm3 is the common Z candidate
                // BUT does it ELIMINATE anything???
                // max of TWO cells to CHECK - in this BOX, sharing the same ROW
                // could continue scan or ROW keeping only those in this BOX
                // or could scan BOX, keeping only this in this ROW
                pboxa = &prcball->vboxs[prc->box]; // all empties in BOX
                max4 = pboxa->size();
                for (i4 = 0; i4 < max4; i4++) {
                    prce = &pboxa->at(i4);
                    if (SAMERCCELL(prce,prc)) continue; // skip XYZ 'hinge'
                    if (SAMERCCELL(prce,prc2)) continue; // skip in BOX buddy
                    if (prce->row != prc->row) continue; // skip if not 'hinge' ROW
                    pse = &prce->set;
                    if ( !(pse->val[eval - 1]) ) continue; // does NOT have elim cand
                    // GOT an XYZ-Wing ELIMINATION
                    pelims->push_back(*prce);
                    pelimby->push_back(*prc);   // XYZ 'hinge'
                    pelimby->push_back(*prc2);  // in BOX buddy XZ
                    pelimby->push_back(*prc3);  // same ROW/COL buddy YZ
                    pb->line[prce->row].set[prce->col].flag[eval - 1] |= xyz_elim;
                    pb->line[prc->row].set[prc->col].flag[eval - 1] |= xyz_mark;
                    pb->line[prc2->row].set[prc2->col].flag[eval - 1] |= xyz_mark;
                    pb->line[prc3->row].set[prc3->col].flag[eval - 1] |= xyz_mark;
                    Mark_PB_per_SET( pb, prc, ps, xyz_mark );
                    Mark_PB_per_SET( pb, prc2, ps2, xyz_mark );
                    Mark_PB_per_SET( pb, prc3, ps3, xyz_mark );
                    pb->line[prc->row].set[prc->col].cellflg   |= (xyz_color | xyz_clr1);
                    pb->line[prc2->row].set[prc2->col].cellflg |= (xyz_color | xyz_clr2);
                    pb->line[prc3->row].set[prc3->col].cellflg |= (xyz_color | xyz_clr2);
                    //pb->line[prc->row].set[prc->col].uval = xyz_on;
                    //pb->line[prc2->row].set[prc2->col].uval = xyz_off;
                    //pb->line[prc3->row].set[prc3->col].uval = xyz_off;
                    count++;
                    sprintf(EndBuf(tb),"Elim %s ",Get_RC_setval_RC_Stg(prce,eval));
                    OUTITXYZ(tb);
                }
            }
            max3 = pcol->size();
            for (i3 = 0; i3 < max3; i3++) {
                prc3 = &pcol->at(i3);
                if (prc3->box == prc->box) continue;
                ps3 = &prc3->set;
                ZERO_SET(&comm2);
                cnt3 = Get_Shared_SET( ps, ps3, &comm2);
                if (cnt3 != 2) continue;
                // a potential buddy - but does it share 1 with the other buddy
                ZERO_SET(&comm3);
                cnt3 = Get_Shared_SET( &comm, &comm2, &comm3 );
                if (cnt3 != 1) continue;
                sprintf(EndBuf(tb),"ColBud %s ",Get_RC_setval_RC_Stg(prc3));
                // GOT and XYZ-Wing - comm3 is the common Z candidate
                // BUT does it ELIMINATE anything???
                // max of TWO cells to CHECK - in this BOX, sharing the same ROW
                // could continue scan or ROW keeping only those in this BOX
                // or could scan BOX, keeping only this in this ROW
                pboxa = &prcball->vboxs[prc->box]; // all empties in BOX
                max4 = pboxa->size();
                for (i4 = 0; i4 < max4; i4++) {
                    prce = &pboxa->at(i4);
                    if (SAMERCCELL(prce,prc)) continue; // skip XYZ 'hinge'
                    if (SAMERCCELL(prce,prc2)) continue; // skip in BOX buddy
                    if (prce->col != prc->col) continue; // skip if not 'hinge' COL
                    pse = &prce->set;
                    if ( !(pse->val[eval - 1]) ) continue; // does NOT have elim cand
                    // GOT an XYZ-Wing ELIMINATION
                    pelims->push_back(*prce);   // ELIM
                    pelimby->push_back(*prc);   // XYZ 'hinge'
                    pelimby->push_back(*prc2);  // in BOX buddy XZ
                    pelimby->push_back(*prc3);  // same ROW/COL buddy YZ
                    pb->line[prce->row].set[prce->col].flag[eval - 1] |= xyz_elim;

                    pb->line[prc->row].set[prc->col].flag[eval - 1] |= xyz_mark;
                    pb->line[prc2->row].set[prc2->col].flag[eval - 1] |= xyz_mark;
                    pb->line[prc3->row].set[prc3->col].flag[eval - 1] |= xyz_mark;
                    Mark_PB_per_SET( pb, prc, ps, xyz_mark );
                    Mark_PB_per_SET( pb, prc2, ps2, xyz_mark );
                    Mark_PB_per_SET( pb, prc3, ps3, xyz_mark );
                    // mark HINGE XYZ
                    pb->line[prc->row].set[prc->col].cellflg   |= (xyz_color | xyz_clr1);
                    // mark Buddies
                    pb->line[prc2->row].set[prc2->col].cellflg |= (xyz_color | xyz_clr2);
                    pb->line[prc3->row].set[prc3->col].cellflg |= (xyz_color | xyz_clr2);
                    count++;
                    sprintf(EndBuf(tb),"Elim %s ",Get_RC_setval_RC_Stg(prce,eval));
                    OUTITXYZ(tb);
                }
            }
        }
    }
    // clean up
    Clear_PRCRCB(prcb2);
    Clear_PRCRCB(prcball);
    emp3.clear();
    emp2.clear();
    empty.clear();
    return count;
}

int Do_Clear_XYZWing( PABOX2 pb )
{
    int count =  Do_Fill_By_Flags( pb, xyz_elim, xyz_mark, "XYZ", "XYZ-scan", xyz_clrs, false );
    return count;
}


int Mark_XYZ_Wings( PABOX2 pb )
{
    int count;
    vRC elims, elimby;
    count = Process_XYZ_Wings( pb, &elims, &elimby );
    char *tb = GetNxtBuf();
    if (count) {
        size_t max, ii;
        PROWCOL prc;
        max = elims.size();
        sprintf(tb,"XYZ.Elim %d ", (int)max);
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc, prc->cnum));
        }
        max = elimby.size();
        for (ii = 0; ii < max; ii++) {
            prc = &elimby[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc));
        }
        OUTIT(tb);
        sprintf(tb,"Sxyz: Marked XYZ-Wing %d", count);
        Stack_Fill(Do_Clear_XYZWing);
    } else {
        sprintf(tb,"Sxyz: No XYZ-Wing marked");
    }
    elims.clear();
    elimby.clear();
    return count;
}

int Do_Fill_XYZWing( PABOX2 pb )
{
    int count =  Do_Fill_By_Flags( pb, xyz_elim, xyz_mark, "XYZ", "XYZ-scan", xyz_clrs, true );
    return count;
}


int Do_XYZ_Wings( PABOX2 pb )
{
    int count;
    vRC elims, elimby;
    count = Process_XYZ_Wings( pb, &elims, &elimby );
    char *tb = GetNxtBuf();
    if (count) {
        size_t max, ii;
        PROWCOL prc;
        max = elims.size();
        sprintf(tb,"XYZ.Elim %d ", (int)max);
        for (ii = 0; ii < max; ii++) {
            prc = &elims[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc, prc->cnum));
        }
        max = elimby.size();
        for (ii = 0; ii < max; ii++) {
            prc = &elimby[ii];
            sprintf(EndBuf(tb),"%s ", Get_RC_setval_RC_Stg(prc));
        }
        OUTIT(tb);
        sprintf(tb,"Sxyz: Marked XYZ-Wing %d - To fill", count);
    } else {
        sprintf(tb,"Sxyz: No XYZ-Wing marked - To bgn" );
    }
    elims.clear();
    elimby.clear();
    return count;
}


// eof - Sudo_XYZWings.cxx
