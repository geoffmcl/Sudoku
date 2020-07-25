// Sudo_Thread.hxx
#ifndef _SUDO_THREAD_HXX_
#define _SUDO_THREAD_HXX_

extern bool StartThread();
extern int get_solution_value( int row, int col );
extern void wait_thread_exit();
extern int get_solution_valid();        // { return got_solution; }
extern int set_solution_valid(int val); // { int curr = got_solution; got_solution = val; return curr; }
extern PABOX2 get_pb_con();     // get that static thread solution ABOX2


#endif // #ifndef _SUDO_THREAD_HXX_
// eof - Sudo_Thread.hxx
