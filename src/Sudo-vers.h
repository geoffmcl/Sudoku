/* ==================================================
   Sudoku project
   |S|U| |  Created: (C) Geoff R. McLane - Aug 2012
   | |D|O|  <reports .at. geoffair .dot. info>
   |K|U| |  License: GNU GPL v.2 (or later)
   This program is free software. You can redistribute it
   and/or modify it under the terms of the GNU General 
   Public Licence as published by the Free Software Foundation.
   This program is distributed in the hope it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty 
   of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
   the GNU General Public License for more details.
   ================================================== */
#ifndef _SUDO_VERS_H_
#define _SUDO_VERS_H_
// #pragma once // prefer the above as croos platform compatible

#ifdef WIN32    // include 'SDKDDKVer.h'
// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>
#endif  // #ifdef WIN32

// This should be the SAME as the VERSION resource
#define APP_VERSION "1.0.0.RC1"

// 20121003 - accepted USE_BOX_STACK // stack the BOX on every change, what ever the change

//#define ADD_LISTVIEW // LISTVIEW moved to a Message Dialog, which actually now uses an EDIT CONTROL
#define ADD_MESSAGE_DIALOG
// 20120917 - looking GOOD - // 20120914 - experimental
#define USE_COLOR2_SCAN

#endif // #ifndef _SUDO_VERS_H_
// eof targetver.h
