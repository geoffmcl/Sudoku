// Sudo_Timer.cxx

#include "Sudoku.hxx"
#include "Sudo_Time.hxx"

bool Done_Auto = false;

bool g_bAutoComplete = false;
bool g_bAutoSolve = false;
bool done_end_dialog = false;
int iSolveStage = -1;
PABOX2 bgn_box;

char *done_msg = "Sudoku has been SOLVED!\nTime for a NEW one ;=))\nClick [Cancel] (or ESC) to Exit application\n";

int Compare_with_Solution(PABOX2 pb, char *tb_in)
{
    int count = 0;
    int row, col, val, val2;
    ROWCOL rc;
    char *tb = EndBuf(tb_in);
    *tb = 0;
    strcpy(tb,"Diffs: ");
    for (row = 0; row < 9; row++) {
        for (col = 0; col < 9; col++) {
            val = pb->line[row].val[col];
            val2 = get_solution_value(row,col);
            if (val == val2) continue;
            count++;
            rc.row = row;
            rc.col = col;
            sprintf(EndBuf(tb),"%s vs %s ", Get_RC_setval_RC_Stg(&rc,val), Get_RC_setval_RC_Stg(&rc,val2));
            if (count > 5) {
                strcat(tb,"...etc...");
                break;
            }
        }
        if (count > 5)
            break;
    }

    if (count)
        sprintf(EndBuf(tb),"%d diffs"MEOL, count);
    else {
        strcpy(tb,"Is SAME as brute force solution!"MEOL);
    }
    sprtf(tb);
    return count;
}

VOID Do_Solved_Message()
{
    char *tb =GetNxtBuf();
    PABOX2 pb = get_curr_box();
    *tb = 0;
    Compare_with_Solution(pb, tb);
    sprtf("Sudoku has been SOLVED!\n");
    strcat(tb,done_msg);
    set_repaint();
    int res = MB3(tb,"SUDOKU SOLVED");
    if (res == IDYES) {
        Post_Command(ID_FILE_OPEN);
    } else if (res == IDCANCEL) {
        Post_Command(IDM_EXIT);
    }
}

static Timer *pAutoTime = 0;
static double last_seconds = -1.0;
double g_AutoDelay = 1.0;

VOID Start_Auto_Solve()
{
    if (!pAutoTime)
        pAutoTime = new Timer;

    if (pAutoTime) {
        if (g_bAutoSolve) {
            g_bAutoSolve = false;    // repeat steps
            pAutoTime->stop();
            char *tb = pAutoTime->getTimeStg();
            sprtf("User stopped Autosolve after %s\n", tb);
            set_repaint();
        } else {
            pAutoTime->start();
            last_seconds = -1.0;
            g_bAutoSolve = true;    // repeat steps
            sprtf("Starting AutoSolve with delay of %s secs between steps.\n", double_to_stg(g_AutoDelay));
            set_repaint2();
        }
    }
}


void Do_WM_TIMER( HWND hWnd )
{
    if (!Done_Auto) {
        if (g_bAutoLoad)
            Auto_Load_file(hWnd);
    }
    Done_Auto = true;

    if (total_empty_count == 0) {
        g_bAutoSolve = FALSE;
        g_bAutoComplete = false;
        if (!done_end_dialog) {
            done_end_dialog = true;
            Do_Solved_Message();
        }
    }
    if (g_bAutoSolve && pAutoTime) {
        bool go = false;
        if (last_seconds < 0.0) {
            // first time
            last_seconds = pAutoTime->getElapsedTime();
            go = true;
        } else {
            double new_secs = pAutoTime->getElapsedTime();
            double new_time = last_seconds + g_AutoDelay;
            if (new_secs >= new_time) {
                last_seconds = new_secs;
                go = true;
            }
        }
        if (go) {
            PABOX2 pb = get_curr_box();
            if (pb->iStage != iSolveStage) {
                iSolveStage = pb->iStage;
                Do_ID_OPTIONS_ONESTEP( hWnd );
                // if no change in 'stage' then failed or finished
                if (pb->iStage == iSolveStage) {
                    g_bAutoSolve = false;
                    pAutoTime->stop();
                    char *tb = pAutoTime->getTimeStg();
                    sprtf("Ended Autosolve after %s\n", tb);
                }
            }
        }
    }
}


// eof - Sudo_Timer.cxx
