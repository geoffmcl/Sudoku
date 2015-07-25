// Sudo_XBrowse.cxx
//
// This file is based on XBrowseForFolder.cpp  Version 1.2
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

#ifdef WIN32    // windows XBrowseForFolder
////////////////////////////////////////////////////////////////////////////////
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <crtdbg.h>
#include <tchar.h>
#include <Shlobj.h>
#include <io.h>
#include "Sudo_XBrowse.hxx"

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop

//#define CRect RECT
#define rectWidth(pr) (pr->right - pr->left)
#define rectHeight(pr) (pr->bottom - pr->top)

//=============================================================================
// struct to pass to callback function
//=============================================================================
struct FOLDER_PROPS
{
	LPCTSTR lpszTitle;
	LPCTSTR lpszInitialFolder;
	UINT ulFlags;
};

///////////////////////////////////////////////////////////////////////////////
// ScreenToClientX - helper function in case non-MFC
static void ScreenToClientX(HWND hWnd, LPRECT lpRect)
{
	_ASSERTE(::IsWindow(hWnd));
	::ScreenToClient(hWnd, (LPPOINT)lpRect);
	::ScreenToClient(hWnd, ((LPPOINT)lpRect)+1);
}

///////////////////////////////////////////////////////////////////////////////
// MoveWindowX - helper function in case non-MFC
//static void MoveWindowX(HWND hWnd, CRect& rect, BOOL bRepaint)
static void MoveWindowX(HWND hWnd, PRECT rect, BOOL bRepaint)
{
	_ASSERTE(::IsWindow(hWnd));
	::MoveWindow(hWnd, rect->left, rect->top,
		rectWidth(rect), rectHeight(rect), bRepaint);
}


///////////////////////////////////////////////////////////////////////////////
// SizeBrowseDialog - resize dialog, move controls
static void SizeBrowseDialog(HWND hWnd, FOLDER_PROPS *fp)
{
	TRACE(_T("in void SizeBrowseDialog\n"));

	// find the folder tree and make dialog larger
	HWND hwndTree = FindWindowEx(hWnd, NULL, _T("SysTreeView32"), NULL);

	if (!hwndTree)
	{
		// ... this usually means that BIF_NEWDIALOGSTYLE is enabled.
		// Then the class name is as used in the code below.
		hwndTree = FindWindowEx(hWnd, NULL, _T("SHBrowseForFolder ShellNameSpace Control"), NULL);
		TRACE(_T("SHBrowseForFolder ShellNameSpace Control: hwndTree=%X\n"), hwndTree);
	}

	RECT rectDlg;
    PRECT pr;
	_ASSERTE(IsWindow(hwndTree));

	if (hwndTree)
	{
		// check if edit box
		int nEditHeight = 0;
		HWND hwndEdit = FindWindowEx(hWnd, NULL, _T("Edit"), NULL);
		TRACE(_T("hwndEdit=%x\n"), hwndEdit);
		RECT rectEdit;
        pr = &rectEdit;
		if (hwndEdit && (fp->ulFlags & BIF_EDITBOX))
		{
			::GetWindowRect(hwndEdit, &rectEdit);
			ScreenToClientX(hWnd, &rectEdit);
			nEditHeight = rectHeight(pr);
		}
		else if (hwndEdit)
		{
			::MoveWindow(hwndEdit, 20000, 20000, 10, 10, FALSE);
			::ShowWindow(hwndEdit, SW_HIDE);
			hwndEdit = 0;
		}

		// make the dialog larger
		::GetWindowRect(hWnd, &rectDlg);
		rectDlg.right += 40;
		rectDlg.bottom += 30;
		if (hwndEdit)
			rectDlg.bottom += nEditHeight + 5;
		MoveWindowX(hWnd, &rectDlg, TRUE);
		::GetClientRect(hWnd, &rectDlg);

		int hMargin = 10;
		int vMargin = 10;

		// check if new dialog style - this means that there will be a resizing
		// grabber in lower right corner
		if (fp->ulFlags & BIF_NEWDIALOGSTYLE)
			hMargin = ::GetSystemMetrics(SM_CXVSCROLL);

		// move the Cancel button
		RECT rectCancel;
		HWND hwndCancel = ::GetDlgItem(hWnd, IDCANCEL);
		if (hwndCancel)
			::GetWindowRect(hwndCancel, &rectCancel);
		ScreenToClientX(hWnd, &rectCancel);
        pr = &rectCancel;
		int h = rectHeight(pr);
		int w = rectWidth(pr);
		rectCancel.bottom = rectDlg.bottom - vMargin;//nMargin;
		rectCancel.top = rectCancel.bottom - h;
		rectCancel.right = rectDlg.right - hMargin; //(scrollWidth + 2*borderWidth);
		rectCancel.left = rectCancel.right - w;
		if (hwndCancel)
		{
			//TRACERECT(rectCancel);
			MoveWindowX(hwndCancel, &rectCancel, FALSE);
		}

		// move the OK button
        RECT rectOK = {0, 0, 0, 0};
		HWND hwndOK = ::GetDlgItem(hWnd, IDOK);
		if (hwndOK)
			::GetWindowRect(hwndOK, &rectOK);
		ScreenToClientX(hWnd, &rectOK);
		rectOK.bottom = rectCancel.bottom;
		rectOK.top = rectCancel.top;
		rectOK.right = rectCancel.left - 10;
		rectOK.left = rectOK.right - w;
		if (hwndOK)
		{
			MoveWindowX(hwndOK, &rectOK, FALSE);
		}

		// expand the folder tree to fill the dialog
		RECT rectTree;
		::GetWindowRect(hwndTree, &rectTree);
		ScreenToClientX(hWnd, &rectTree);
		if (hwndEdit)
		{
			rectEdit.left = hMargin;
			rectEdit.right = rectDlg.right - hMargin;
			rectEdit.top = vMargin;
			rectEdit.bottom = rectEdit.top + nEditHeight;
			MoveWindowX(hwndEdit, &rectEdit, FALSE);
			rectTree.top = rectEdit.bottom + 5;
		}
		else
		{
			rectTree.top = vMargin;
		}
		rectTree.left = hMargin;
		rectTree.bottom = rectOK.top - 10;//nMargin;
		rectTree.right = rectDlg.right - hMargin;
		//TRACERECT(rectTree);
		MoveWindowX(hwndTree, &rectTree, FALSE);
	}
	else
	{
		TRACE(_T("ERROR - tree control not found.\n"));
		//_ASSERTE(hwndTree);
	}
}


///////////////////////////////////////////////////////////////////////////////
// BrowseCallbackProc - SHBrowseForFolder callback function
static int CALLBACK BrowseCallbackProc(HWND hWnd,		// Window handle to the browse dialog box
									   UINT uMsg,		// Value identifying the event
									   LPARAM lParam,	// Value dependent upon the message 
									   LPARAM lpData)	// Application-defined value that was 
														// specified in the lParam member of the 
														// BROWSEINFO structure
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:		// sent when the browse dialog box has finished initializing. 
		{
			TRACE(_T("hWnd=%X\n"), hWnd);

			// remove context help button from dialog caption
			LONG lStyle = ::GetWindowLong(hWnd, GWL_STYLE);
			lStyle &= ~DS_CONTEXTHELP;
			::SetWindowLong(hWnd, GWL_STYLE, lStyle);
			lStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
			lStyle &= ~WS_EX_CONTEXTHELP;
			::SetWindowLong(hWnd, GWL_EXSTYLE, lStyle);

			FOLDER_PROPS *fp = (FOLDER_PROPS *) lpData;
			if (fp)
			{
				if (fp->lpszInitialFolder && (fp->lpszInitialFolder[0] != _T('\0')))
				{
					// set initial directory
					::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)fp->lpszInitialFolder);
				}

				if (fp->lpszTitle && (fp->lpszTitle[0] != _T('\0')))
				{
					// set window caption
					::SetWindowText(hWnd, fp->lpszTitle);
				}
			}

			SizeBrowseDialog(hWnd, fp);
		}
		break;

		case BFFM_SELCHANGED:		// sent when the selection has changed
		{
			TCHAR szDir[MAX_PATH*2] = { 0 };

			// fail if non-filesystem
			BOOL bRet = SHGetPathFromIDList((LPITEMIDLIST) lParam, szDir);
			if (bRet)
			{
				// fail if folder not accessible
				if (_taccess(szDir, 00) != 0)
				{
					bRet = FALSE;
				}
				else
				{
					SHFILEINFO sfi;
					::SHGetFileInfo((LPCTSTR)lParam, 0, &sfi, sizeof(sfi), 
							SHGFI_PIDL | SHGFI_ATTRIBUTES);
					TRACE(_T("dwAttributes=0x%08X\n"), sfi.dwAttributes);

					// fail if pidl is a link
					if (sfi.dwAttributes & SFGAO_LINK)
					{
						TRACE(_T("SFGAO_LINK\n"));
						bRet = FALSE;
					}
				}
			}

			// if invalid selection, disable the OK button
			if (!bRet)
			{
				::EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);
			}

			TRACE(_T("szDir=%s\n"), szDir);
		}
		break;
	}
         
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// XBrowseForFolder()
//
// Purpose:     Invoke the SHBrowseForFolder API.  If lpszInitialFolder is
//              supplied, it will be the folder initially selected in the tree 
//              folder list.  Otherwise, the initial folder will be set to the 
//              current directory.  The selected folder will be returned in 
//              lpszBuf.
//
// Parameters:  hWnd              - handle to the owner window for the dialog
//              lpszInitialFolder - initial folder in tree;  if NULL, the initial
//                                  folder will be the current directory; if
//                                  if this is a CSIDL, must be a real folder.
//              nRootFolder       - optional CSIDL of root folder for tree;
//                                  -1 = use default.
//              lpszCaption       - optional caption for folder dialog
//              lpszBuf           - buffer for the returned folder path
//              dwBufSize         - size of lpszBuf in TCHARs
//              bEditBox          - TRUE = include edit box in dialog
//
// Returns:     BOOL - TRUE = success;  FALSE = user hit Cancel
//
BOOL XBrowseForFolder(HWND hWnd,
					  LPCTSTR lpszInitialFolder,
					  LPCTSTR lpszCaption,
					  LPTSTR lpszBuf,
					  DWORD dwBufSize,
					  int nRootFolder /*= -1*/,
					  BOOL bEditBox /*= FALSE*/)
{
	_ASSERTE(lpszBuf);
	_ASSERTE(dwBufSize >= MAX_PATH);

	if (lpszBuf == NULL || dwBufSize < MAX_PATH)
		return FALSE;

	ZeroMemory(lpszBuf, dwBufSize*sizeof(TCHAR));

	BROWSEINFO bi = { 0 };

	// check if there is a special root folder
	LPITEMIDLIST pidlRoot = NULL;
	if (nRootFolder != -1)
	{
		if (SUCCEEDED(SHGetSpecialFolderLocation(hWnd, nRootFolder, &pidlRoot)))
			bi.pidlRoot = pidlRoot;
	}

	TCHAR szInitialPath[MAX_PATH*2] = { _T('\0') };
	if (lpszInitialFolder)
	{
		// is this a folder path string or a csidl?
		if (HIWORD(lpszInitialFolder) == 0)
		{
			// csidl
			int nFolder = LOWORD((UINT)(UINT_PTR)lpszInitialFolder);
			TRACE(_T("csidl:  nFolder=0x%X\n"), nFolder);
			SHGetSpecialFolderPath(hWnd, szInitialPath, nFolder, FALSE);
		}
		else
		{
			// string
			_tcsncpy(szInitialPath, lpszInitialFolder, 
						sizeof(szInitialPath)/sizeof(TCHAR)-2);
		}
		TRACE(_T("szInitialPath=<%s>\n"), szInitialPath);
	}

	if ((szInitialPath[0] == _T('\0')) && (bi.pidlRoot == NULL))
	{
		// no initial folder and no root, set to current directory
		::GetCurrentDirectory(sizeof(szInitialPath)/sizeof(TCHAR)-2, 
				szInitialPath);
	}

	FOLDER_PROPS fp;

	bi.hwndOwner = hWnd;
	bi.ulFlags   = BIF_RETURNONLYFSDIRS;	// do NOT use BIF_NEWDIALOGSTYLE, 
											// or BIF_STATUSTEXT
	if (bEditBox)
		bi.ulFlags |= BIF_EDITBOX;
	bi.ulFlags  |= BIF_NONEWFOLDERBUTTON;
	bi.lpfn      = BrowseCallbackProc;
	bi.lParam    = (LPARAM) &fp;

	fp.lpszInitialFolder = szInitialPath;
	fp.lpszTitle = lpszCaption;
	fp.ulFlags = bi.ulFlags;

	BOOL bRet = FALSE;

	LPITEMIDLIST pidlFolder = SHBrowseForFolder(&bi);

	if (pidlFolder)
	{
		TCHAR szBuffer[MAX_PATH*2] = { _T('\0') };

		if (SHGetPathFromIDList(pidlFolder, szBuffer))
		{
			_tcsncpy(lpszBuf, szBuffer, dwBufSize-1);
			bRet = TRUE;
		}
		else
		{
			TRACE(_T("SHGetPathFromIDList failed\n"));
		}
	}

	// free up pidls
	IMalloc *pMalloc = NULL; 
	if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc) 
	{
		if (pidlFolder)
			pMalloc->Free(pidlFolder);  
		if (pidlRoot)
			pMalloc->Free(pidlRoot);  
		pMalloc->Release(); 
	}

	return bRet;
}
////////////////////////////////////////////////////////////////////////////////
#else // !#ifdef WIN32 - TODO: Need alternative code for XBrowseForFolder
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n - XBrowseForFolder

// eof - Sudo_Xbrowse.cxx
