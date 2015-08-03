// Sudo_Thread.cxx

#include "Sudoku.hxx"
#ifdef WIN32    // include 'process.h'
#include <process.h>
#else // !WIN32 // include <pthread.h>
#include <pthread.h>
#endif  // WIN32 process.h/pthread.h
#include "Sudo_Time.hxx"

static PABOX2 pb_copy = 0;
static int got_solution = 0;
static int in_thread = 0;

static HANDLE hThread = 0;
#ifdef WIN32 // windows threadID
static unsigned threadID = 0;
#else // !WIN32 pthread_t
static pthread_t threadID = 0;
#endif // WIN32 y/n - threadID

//static DWORD dwMilliseconds = INFINITE;
static DWORD dwMilliseconds = 10000;    // ten secs
static int use_endthreadex = 0;     // this causes the thread to EXIT immediately
int get_solution_valid() { return got_solution; }
int set_solution_valid(int val) { int curr = got_solution; got_solution = val; return curr; }
static double thread_time = 0.0;

int get_solution_value( int row, int col )
{
    if (!in_thread && got_solution)
        return pb_copy->line[row].val[col];
    return 0;
}

static void solve_on_thread()
{
    Timer tm;
    in_thread++;
    tm.start();
    set_solution_valid(0);
    //sprtf( "In second thread...\n" );
    got_solution = Get_Solution(pb_copy, true);
    //sprtf( "Thread exit... %d\n", got_solution );
    tm.stop();
    thread_time = tm.getElapsedTime();
    if (got_solution) {
        PABOX2 pb = get_curr_box();
        pb->bflag |= pb_copy->bflag;    // transfer some flags
    }
    in_thread--;
}

#ifdef WIN32    // window THREAD handling
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void wait_thread_exit()
{
    if (hThread) {
        WaitForSingleObject( hThread, dwMilliseconds );
    }
}

unsigned __stdcall ThreadFunc( void* pArguments )
{
    solve_on_thread();
    HANDLE hand = hThread;
    hThread = 0;
    if (hand)
        CloseHandle(hand);
    if (use_endthreadex)
        _endthreadex( 0 );
    return 0;
} 

bool StartThread()  // windows - uses _beginthreadex()
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
    // sprtf( "Creating second thread... %d spots\n", cnt );
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
        //sprtf( "Got second thread...\n" );
        bret = true;
    } else {
        sprtf( "Second thread failed...%d - %s\n", errno, strerror(errno) );
    }
    return bret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
#else // !#ifdef WIN32 - TODO: Need alterative THREAD handling
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* this function is run by the second thread */
void *ThreadFunc(void *vp)
{
    solve_on_thread();
    return 0;
}

bool StartThread()  // unix - uses pthread_create()
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
    // sprtf( "Creating second thread... %d spots\n", cnt );
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
    // hThread = (HANDLE)_beginthreadex( NULL, 0, &ThreadFunc, NULL, 0, &threadID );
    /* create a second thread which executes inc_x(&x) */
    int x = 0;
    if(pthread_create(&threadID, NULL, ThreadFunc, &x)) 
    {
        fprintf(stderr, "Error creating thread\n");
        return false;

    }
    //if (hThread) {
        //sprtf( "Got second thread...\n" );
        bret = true;
    //} else {
    //    sprtf( "Second thread failed...%d - %s\n", errno, strerror(errno) );
    //}
    return bret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n - THREAD handling

// eof - Sudo_Thread.cxx
