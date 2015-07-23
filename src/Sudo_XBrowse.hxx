// Sudo_XBrowse.hxx
//
// This file is based on XBrowseForFolder.h  Version 1.2
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// This software is released into the public domain.  You are free to use
// it in any way you like, except that you may not sell this source code.
//
// This software is provided "as is" with no expressed or implied warranty.
// I accept no liability for any damage or loss of business that this
// software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SUDO_XBROWSE_HXX_
#define _SUDO_XBROWSE_HXX_

// Returns:     BOOL - TRUE = success;  FALSE = user hit Cancel
extern BOOL XBrowseForFolder(HWND hWnd,
					  LPCTSTR lpszInitialFolder,
					  LPCTSTR lpszCaption,
					  LPTSTR lpszBuf,
					  DWORD dwBufSize,
					  int nRootFolder = -1,
					  BOOL bEditBox = FALSE);

#endif // #ifndef _SUDO_XBROWSE_HXX_
// eof - Sud_XBrowse.hxx

