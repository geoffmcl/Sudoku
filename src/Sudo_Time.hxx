//////////////////////////////////////////////////////////////////////////////
// Timer.hxx
// ==========
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system 
//
//  AUTHOR: Geoff R. McLane (reports-at-geoffair-dot-info)
//  CREATED: 2011-11-25
// Adapted from - 
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_HXX_DEF
#define TIMER_HXX_DEF
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef WIN32   // Windows system specific - include windows.h
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif


class Timer
{
public:
    Timer();                                    // default constructor
    ~Timer();                                   // default destructor

    void   start();                             // start timer
    void   stop();                              // stop the timer
    double getElapsedTime();                    // get elapsed time in second
    double getElapsedTimeInSec();               // get elapsed time in second (same as getElapsedTime)
    double getElapsedTimeInMilliSec();          // get elapsed time in milli-second
    double getElapsedTimeInMicroSec();          // get elapsed time in micro-second
    char  *getTimeStg();
    char  *setTimeStg(char *cp,double secs);    // set time string in users buffer

protected:


private:
    double startTimeInMicroSec;                 // starting time in micro-second
    double endTimeInMicroSec;                   // ending time in micro-second
    int    stopped;                             // stop flag 
#ifdef WIN32    // Use LARGE_INTEGER
    LARGE_INTEGER frequency;                    // ticks per second
    LARGE_INTEGER startCount;                   //
    LARGE_INTEGER endCount;                     //
#else   // !WIN32 - Use 'timeval
    timeval startCount;                         //
    timeval endCount;                           //
#endif  // WIN32 y/n
    char _tm_buf[128];
};

#endif // TIMER_HXX_DEF
// eof - Timer.hxx
