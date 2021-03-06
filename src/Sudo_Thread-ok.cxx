// Sudo_Thread.cxx

#include "Sudoku.hxx"
#include <process.h>

static HANDLE hThread = 0;
static unsigned threadID = 0;
static PABOX2 pb_copy = 0;
static int got_solution = 0;
static int in_thread = 0;
//static DWORD dwMilliseconds = INFINITE;
static DWORD dwMilliseconds = 10000;    // ten secs
static int use_endthreadex = 0;     // this causes the thread to EXIT immediately

int get_solution_value( int row, int col )
{
    if (!in_thread && got_solution)
        return pb_copy->line[row].val[col];
    return 0;
}

void wait_thread_exit()
{
    if (hThread) {
        WaitForSingleObject( hThread, dwMilliseconds );
    }
}

unsigned __stdcall ThreadFunc( void* pArguments )
{
    in_thread++;
    sprtf( "In second thread...\n" );
    got_solution = Get_Solution(pb_copy);
    sprtf( "Thread exit... %d\n", got_solution );
    HANDLE hand = hThread;
    hThread = 0;
    if (hand)
        CloseHandle(hand);
    in_thread--;
    if (use_endthreadex)
        _endthreadex( 0 );
    return 0;
} 

bool StartThread()
{
    bool bret = false;
    if (in_thread)
        return bret;
    if (!is_curr_box_valid_for_paint())
        return bret;
    int cnt = Get_Spot_Count();
    if (!cnt || (cnt >= 81))
        return bret;
    PABOX2 pb = get_curr_box();
    sprtf( "Creating second thread... %d spots\n", cnt );
    if (pb_copy) {
        int row, col;
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                pb_copy->line[row].val[col] = pb->line[row].val[col];
            }
        }
    } else 
        pb_copy = Copy_Box2(pb);

    // Create the second thread.
    hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, NULL, 0, &threadID );
    if (hThread) {
        sprtf( "Got second thread...\n" );
        bret = true;
    } else {
        sprtf( "Second thread failed...%d - %s\n", errno, strerror(errno) );
    }
    return bret;
}

// eof - Sudo_Thread.cxx
