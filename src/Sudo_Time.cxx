//////////////////////////////////////////////////////////////////////////////
// Timer.cxx
// =========
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system 
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////

#include "Sudo_Time.hxx"
#include <stdlib.h>
#include <stdio.h>  // for sprintf()

///////////////////////////////////////////////////////////////////////////////
// constructor
///////////////////////////////////////////////////////////////////////////////
Timer::Timer()
{
#ifdef WIN32
    QueryPerformanceFrequency(&frequency);
    startCount.QuadPart = 0;
    endCount.QuadPart = 0;
#else
    startCount.tv_sec = startCount.tv_usec = 0;
    endCount.tv_sec = endCount.tv_usec = 0;
#endif

    stopped = 0;
    startTimeInMicroSec = 0;
    endTimeInMicroSec = 0;
}



///////////////////////////////////////////////////////////////////////////////
// distructor
///////////////////////////////////////////////////////////////////////////////
Timer::~Timer()
{
}



///////////////////////////////////////////////////////////////////////////////
// start timer.
// startCount will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Timer::start()
{
    stopped = 0; // reset stop flag
#ifdef WIN32
    QueryPerformanceCounter(&startCount);
#else
    gettimeofday(&startCount, NULL);
#endif
}



///////////////////////////////////////////////////////////////////////////////
// stop the timer.
// endCount will be set at this point.
///////////////////////////////////////////////////////////////////////////////
void Timer::stop()
{
    stopped = 1; // set timer stopped flag

#ifdef WIN32
    QueryPerformanceCounter(&endCount);
#else
    gettimeofday(&endCount, NULL);
#endif
}



///////////////////////////////////////////////////////////////////////////////
// compute elapsed time in micro-second resolution.
// other getElapsedTime will call this first, then convert to correspond resolution.
///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTimeInMicroSec()
{
#ifdef WIN32
    if(!stopped)
        QueryPerformanceCounter(&endCount);

    startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
    endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    if(!stopped)
        gettimeofday(&endCount, NULL);

    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

    return endTimeInMicroSec - startTimeInMicroSec;
}



///////////////////////////////////////////////////////////////////////////////
// divide elapsedTimeInMicroSec by 1000
///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTimeInMilliSec()
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}



///////////////////////////////////////////////////////////////////////////////
// divide elapsedTimeInMicroSec by 1000000
///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTimeInSec()
{
    return this->getElapsedTimeInMicroSec() * 0.000001;
}



///////////////////////////////////////////////////////////////////////////////
// same as getElapsedTimeInSec()
///////////////////////////////////////////////////////////////////////////////
double Timer::getElapsedTime()
{
    return this->getElapsedTimeInSec();
}

char *Timer::getTimeStg()
{
    char *cp = _tm_buf;
    double elap = this->getElapsedTimeInSec();
    bool neg = false;
    char *units;
    if (elap < 0.0) {
        neg = true;
        elap *= -1.0;
    }
    if ( !(elap > 0.0) )
        return "0.0 s";
    if (elap < 1e-21) {
        // yoto = 10^-24
        elap *= 1e+21;
        units = "ys";
    } else if (elap < 1e-18) {
        // zepto - 10^-21
        elap *= 1e+18;
        units = "zs";
    } else if (elap < 1e-15) {
        // atto - 10^-18
        elap *= 1e+15;
        units = "as";
    } else if (elap < 1e-12) {
        // femto - 10^-15
        elap *= 1e+12;
        units = "fs";
    } else if (elap < 1e-9) {
        // pico - 10^-12
        elap *= 1e+9;
        units = "ps";
    } else if (elap < 1e-6) {
        // nanosecond - one thousand millionth (10?9) of a second
        elap *= 1e+6;
        units = "ns";
    } else if (elap < 1e-3) {
        // microsecond - one millionth (10?6) of a second
        elap *= 1e+3;
        units = "us";
    } else if (elap < 1.0) {
        // millisecond
        elap *= 1000.0;
        units = "ms";
    } else if (elap < 60.0) {
        units = "s";
    } else {
        int secs = (int)(elap + 0.5);
        int mins = (int)(secs / 60);
        secs = (secs % 60);
        if (mins >= 60) {
            int hrs = (int)(mins / 60);
            mins = mins % 60;
            if (hrs >= 24) {
                int days = (int)(hrs / 24);
                hrs = hrs % 24;
                sprintf(cp,"%d days %2d:%02d:%02d hh:mm:ss", days, hrs, mins, secs);
            } else {
                sprintf(cp,"%2d:%02d:%02d hh:mm:ss", hrs, mins, secs);
            }
        } else {
            sprintf(cp,"%2d:%02d mm:ss", mins, secs);
        }
        return cp;
    }
    char *res = "";
    if (neg) {
        res = "-";
    }
    sprintf(cp,"%s%0.1f %s",res,elap,units);
    return cp;
}

// eof - Timer.cxx
