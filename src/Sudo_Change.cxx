// Sudo_Change.cxx

#include "Sudoku.hxx"

BOOL g_bWarnOnChange = TRUE;
// NOTE: A NEGATIVE nval denote DELETED
BOOL Change_Box(PABOX2 pb, int row, int col, int val, int flag)
{
    Set_Paint_Chains_Invalid(); // STOP PAINTING PAIRS, etc 
    if (val > 0) {
        int val2 = get_solution_value(row,col);
        if (val2 && (val2 != val)) {
            g_bAutoSolve = FALSE;   // stop any AUTO SOLVE!!!
            sprtf("Cheat: Change R%dC%d: Solution value is %d, NOT %d!\n", row + 1, col + 1, val2, val);
            if (g_bWarnOnChange) {
                char *tb = GetNxtBuf();
                sprintf(tb,"Cell R%dC%d (%c%d) is about to be SET with value [%d]" MEOL,
                    row + 1, col + 1, (char)(row + 'A'), col + 1, val );
                sprintf(EndBuf(tb),"However a generated test solution indicates the value" MEOL
                    "of this cell should be [%d]" MEOL, val2 );
                sprintf(EndBuf(tb),"Do you want to CONTINUE with the setting of value [%d]?", val);
                int res = Do_MsgBox_YN(tb,"CHECK BOX CHANGE");
                //int res = MB2(tb,"CHECK BOX CHANGE");
                //if (res != IDYES) {
                if (res) {
                    return FALSE;
                }
            }
        }
    }

    Stack_Box(pb);  // KEEP BACKUP POSITION
    ///Add_Box2(pb);   // stack the BOX, ready for BACKUP
    if (val < 0) {
        pb->line[row].val[col] = 0; // DELETE A VALUE
        PSET ps = &pb->line[row].set[col];
        Fill_SET(ps); // EMPTY CELL - restore ALL candidates
    } else {
        pb->line[row].val[col] = val;
        pb->line[row].set[col].cellflg |= cl_New;
    }

    g_bChanged = TRUE;
    Set_Window_Title();
    pb->iStage = sg_Begin;

    set_repaint();
    return TRUE;
}

// Hmmm, this change does NOT keep the WHOLE state
// 20120808 - Change to Stack_Box(pb) on EACH change
// Then Undo just does Pop_Box() backing up one, putting on a back up stack
// Thus Redo checks the back up stack, and returns them to the main stack
// Thus Undo and Redo represent EVERY change, including ELIMINATION
// THus one undo would undo the last elimination, and redo would process again
// Sounds good - let's see how it works out
// After making SURE what is PUSHED is a NEW memory address started to work fine

int debug_push_pop = 0;
PABOX2 pCurr_Box = 0;

PABOX2 Copy_Box2( PABOX2 pb_in )
{
    PABOX2 pb = (PABOX2)malloc( sizeof(ABOX2) );
    if (!pb) {
        MB("ERROR: Memory allocation FAILED!\n");
        exit(1);
    }
    memcpy(pb,pb_in,sizeof(ABOX2));
    //Show_Box2(pb);
    return pb;
}

PABOX2 get_curr_box() 
{
    if (pCurr_Box == 0) {
        pCurr_Box = (PABOX2)malloc(sizeof(ABOX2));
        if (!pCurr_Box) {
            MB("Memory allocation failed!\nWill abort on OK\n");
            exit(1);
        }
        invalidate_box(pCurr_Box);
    }
    return pCurr_Box;
}

bool is_curr_box_valid()
{
    PABOX2 pb = get_curr_box();
    return validate_box(pb) ? false : true;
}

bool is_curr_box_valid_for_paint()
{
    PABOX2 pb = get_curr_box();
    return (validate_box(pb) == 1) ? false : true;
}

// extra painting of CHAINS etc
// ============================
void Set_Paint_Chains_Invalid()
{
    Set_Chain_Invalid2();    // stop painting chains
    Invalidate_SLP();       // AND pairs
    Set_XYChain_Invalid();
    Set_Rect_Link_Cnt(0);   // stop showing unique rectange links
    Set_UR_invalid();   // Unique Rectangles
    Kill_AIC_chains(); // remove any previous chains
}
// =============================

//extern PABOX2 Stack_Curr_Box();
//extern PABOX2 Pop_Curr_Box();
//extern bool Got_Box_Stack();

typedef vector<PABOX2> vpAB2;
typedef vpAB2::iterator vpABi2;

vpAB2 vBoxStack;
vpAB2 vPopStack;

bool Got_Redo_Stack()
{
    return (vPopStack.size()) ? true : false;
}

bool Got_Change_Stack()
{
    return vBoxStack.size() ? true : false;
}


void Free_Boxes2()
{
    vpABi2 ii;
    size_t i, max;
    for (ii = vBoxStack.begin(); ii != vBoxStack.end(); ii++) {
        PABOX2 pb = *ii;
        if (debug_push_pop) sprtf("debug_push_pop: Delete Box %p\n",pb);
        free(pb);
    }
    max = vBoxStack.size();
    for (ii = vPopStack.begin(); ii != vPopStack.end(); ii++) {
        PABOX2 pb = *ii;
        for (i = 0; i < max; i++) {
            PABOX2 pb2 = vBoxStack[i];
            if (pb == pb2) break;
        }
        if (i == max) {
            if (debug_push_pop) sprtf("debug_push_pop: delete Box %p\n",pb);
            free(pb);
        }
    }

    // clear both vectors
    vBoxStack.clear();
    vPopStack.clear();
    Kill_UR2(); // clear and delete Unique.Rect2
}

void set_curr_box(PABOX2 pb)
{
    if (pCurr_Box) Stack_Box(pCurr_Box);
    pCurr_Box = pb; 
}

// Add this BOX to the STACK
// FIX20120809 - BUT never push a Box that is ALREADY on the stack
// Instead make a COPY, and push the COPY
static PABOX2 last_box = 0;
PABOX2 Stack_Box( PABOX2 pb )
{
    size_t max = vBoxStack.size();
    size_t i;
    if (max) {
        if (last_box && (memcmp((const void *)pb, (const void *)last_box, sizeof(ABOX2)) == 0)) {
            if (debug_push_pop) sprtf("debug_push_pop: NoPush Box %p - %d BOX UNCHANGED!\n",pb, (int)vBoxStack.size());
            return pb;
        }
        for (i = 0; i < max; i++) {
            PABOX2 pb2 = vBoxStack[i]; // get the Box
            if (pb == pb2) {    // EEK! Do NOT push SAME box twice!!!
                pb2 = Copy_Box2(pb);    // Make a COPY
                vBoxStack.push_back(pb2);   // push the COPY
                if (debug_push_pop) sprtf("debug_push_pop: Pushed Box %p - %d COPY %p\n",pb2, (int)vBoxStack.size(), pb);
                break;
            }
        }
        if (i == max) {
            vBoxStack.push_back(pb);
            if (debug_push_pop) sprtf("debug_push_pop: Pushed Box %p - %d\n",pb, (int)vBoxStack.size());
        }
    } else {
        vBoxStack.push_back(pb);
        if (debug_push_pop) sprtf("debug_push_pop: Pushed Box %p - %d FIRST\n",pb, (int)vBoxStack.size());
    }
    last_box = pb;
    return pb;
}

PABOX2 Redo_Box()
{
    size_t cs = vPopStack.size();
    if (cs) {   // IFF there is an Undo stack
        PABOX2 pb = vPopStack.back();   // get LAST
        vPopStack.pop_back();           // and remove it
        size_t ns = vPopStack.size();   // get NEW size
        if (debug_push_pop) sprtf("debug_push_pop: Redo   Box %p - %d-%d\n",pb, (int)cs, (int)ns);
        return Stack_Box(pb);           // and STACK this BOX for Undo to find
    }
    return 0;   // nothing on the Redo stack
}

PABOX2 Undo_Box()
{
    size_t cs = vBoxStack.size();
    if (cs)
    {
        PABOX2 pb = vBoxStack.back(); // get last OFF the stack
        vBoxStack.pop_back();         // and REMOVE
        size_t ns = vBoxStack.size(); // get NEW size
        // push this onto the Pop stack, for Redo to find
        size_t max = vPopStack.size();
        size_t i;
        if (max) {
            for (i = 0; i < max; i++) {
                PABOX2 pb2 = vPopStack[i];
                if (pb == pb2) {    // This address already ON  STACK
                    pb2 = Copy_Box2(pb);
                    vPopStack.push_back(pb2);
                    if (debug_push_pop) sprtf("debug_push_pop: Undo   Box %p - %d-%d COPY of %p\n",pb2, (int)cs, (int)ns, pb);
                    break;
                }
            }
            if ( i == max ) {
                vPopStack.push_back(pb);
                if (debug_push_pop) sprtf("debug_push_pop: Undo   Box %p - %d-%d\n",pb, (int)cs, (int)ns);
            }
        } else {
            vPopStack.push_back(pb);
            if (debug_push_pop) sprtf("debug_push_pop: Undo   Box %p - %d-%d FIRST\n",pb, (int)cs, (int)ns);
        }
        return pb;  // return the LAST BOX - ie BACKUP ONE STEP
    }
    return 0;   // nothing on stack
}

void Do_ID_EDIT_UNDO(HWND hWnd)
{
    PABOX2 pb = Undo_Box(); // back up ONE BOX
    if (pb) {
        //set_curr_box(pb); // NO, this would RE-STACK it///
        sprtf("Undo: Box %p backup to %p\n", pCurr_Box, pb);
        pCurr_Box = pb;     // just SET
        Set_Paint_Chains_Invalid();
        set_repaint();
    } else {
        sprtf("Undo: Box %p but none to backup to!\n", pCurr_Box, pb);
    }
}

void Do_ID_EDIT_REDO(HWND hWnd)
{
    PABOX2 pb = Redo_Box(); // got FORWARD ONE BOX
    if (pb) {
        sprtf("Redo: Box %p reset to %p\n", pCurr_Box, pb);
        pCurr_Box = pb; 
        //set_curr_box(pb);
    } else {
        sprtf("Redo: Box %p but none to reset to!\n", pCurr_Box);
    }
    set_repaint();
}

int Clear_Bad_Flag( PABOX2 pb )
{
    int count = 0;
    int row, col, val;
    uint64_t cellflg;
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (val) {
                cellflg = pb->line[row].set[col].cellflg;
                if ( cellflg & cl_Bad ) {
                    count++;    // note, only ever counted once
                    cellflg &= ~cl_Bad;
                    pb->line[row].set[col].cellflg = cellflg;
                }
            }
        }
    }
    return count;
}

int Mark_Bad_Values( PABOX2 pb )
{
    int count = 0;
    int row, col, val;
    int row2, col2, val2;
    uint64_t cellflg;
    bool bad;
    Clear_Bad_Flag(pb); // CLEAR all bad flags
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            if (!val) continue; // no interest
            bad = false;
            // is it repeated in this row
            for (col2 = 0; col2 < 9; col2++) {
                if (col2 == col) continue;  // skip self
                val2 = pb->line[row].val[col2];
                if (val2 == val) {
                    bad = true;
                    break;
                }
            }
            if (!bad) {
                // check this column
                for (row2 = 0; row2 < 9; row2++) {
                    if (row2 == row) continue; // skip self
                    val2 = pb->line[row2].val[col];
                    if (val2 == val) {
                        bad = true;
                        break;
                    }
                }
            }
            if (!bad) {
                // check this box
                int r, c, rw, cl;
                r = (row / 3) * 3;
                c = (col / 3) * 3;
                for (rw = 0; rw < 3; rw++) {
                    for (cl = 0; cl < 3; cl++) {
                        row2 = r + rw;
                        col2 = c + cl;
                        if ((row2 == row) && (col2 == col)) continue; // skip self
                        val2 = pb->line[row2].val[col2];
                        if (val2 == val) {
                            bad = true;
                            break;
                        }
                    }
                    if (bad)
                        break;
                }
            }
            if (bad) {
                cellflg = pb->line[row].set[col].cellflg;
                if ( !(cellflg & cl_Bad) ) {
                    count++;    // note, only ever counted once
                    cellflg |= cl_Bad;
                    pb->line[row].set[col].cellflg = cellflg;
                }
            }
        }   // for each COL
    }   // for each ROW
    return count;
}

static char *last_or_setval_error = "none";
int set_error = 0;
char *get_last_or_setval_error() { return last_or_setval_error; }

int OR_Row_Col_SetVal_with_Flag( PABOX2 pb, int row, int col, int setval,
                                 uint64_t flag, bool force )
{
    set_error = SERR_NONE;
    last_or_setval_error = "none";
    if (!VALUEVALID(setval) ) {
        last_or_setval_error = "setval value NOT valid!";
        set_error = SERR_BADSV;
        return SERR_BADSV;
    }
    if (!VALIDRC(row,col)) {
        last_or_setval_error = "row or col NOT valid!";
        set_error = SERR_BADRC;
        return SERR_BADRC;
    }
    if (!pb) {
        last_or_setval_error = "No Box pointer given!";
        set_error = SERR_NOBP;
        return SERR_NOBP;
    }
    if (!flag) {
        last_or_setval_error = "NO flag to set!";
        set_error = SERR_NOFLG;
        return SERR_NOFLG;
    }
    // qiht override 'force'
    if ( !(pb->line[row].set[col].val[setval - 1]) ) {
        last_or_setval_error = "No such candidate at this row/col!";
        set_error = SERR_NOCND;
        if (!force)
            return SERR_NOCND;
    }
    int val2 = get_solution_value(row,col);
    if (val2 && (val2 == setval)) {
        if (set_error == SERR_NONE) {
            last_or_setval_error = "Solution suggests setval is correct candidate!";
            set_error = SERR_MKSLN;
        }
        if (!force)
            return SERR_MKSLN;
    }

    if (pb->line[row].set[col].flag[setval - 1] & flag) {
        if (set_error == SERR_NONE) {
            last_or_setval_error = "This candidate already marked with flag!";
            set_error = SERR_ISMKD;
        }
        return SERR_ISMKD;
    }
    pb->line[row].set[col].flag[setval - 1] |= flag;
    sprtf("S%d: Marked %d@%c%d with %s\n", pb->iStage, setval, (char)(row + 'A'), col + 1, Flag2Name(flag));
    // set_error = SERR_NONE;
    // last_or_setval_error = "none";
    return SERR_NONE;
}


// Sudo_Change.cxx
