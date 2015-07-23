// Sudo_Dialogs,hxx
#ifndef _SUDO_DIALOGS_HXX_
#define _SUDO_DIALOGS_HXX_

extern void Do_IDM_ABOUT(HWND hWnd);
extern void Do_ID_EDIT_GENERATE(HWND hWnd);
extern VOID Do_ID_FILE_SAVETEMP(HWND hWnd);

extern void CreateProgressDialog(HWND hWnd);

// put TEXT into separate dialog window
// ------------------------------------
extern BOOL CreateMessageDialog( HWND hWnd );
extern BOOL Get_Dlg_OnShow(); // { return bOnShow; }
extern BOOL Set_Dlg_OnShow( BOOL bShw ); // { BOOL ret = bOnShow; bOnShow = bShw; return ret; }
extern BOOL Toggle_Dlg_Show();
// ------------------------------------

extern BOOL Init_Common_Controls(HWND hWnd);
extern bool Do_ID_VIEW_STRONGLINKS( HWND hWnd );
extern bool Show_SL_Value( int setval );

extern VOID Do_ID_FILE_IMPORT(HWND hWnd);
extern VOID Do_Import_Help(HWND hDlg);

//char szASV[] = "All_Candidates";
extern BOOL g_bAllSL;
//BOOL gChgASV = FALSE;

extern void Toggle_CheckBox_Bool( UINT ret, PBOOL pOpt, PBOOL pChg );
extern BOOL CenterWindow(HWND hwndChild, HWND hwndParent);

extern VOID Do_ID_OPTIONS_DEBUGOPTIONS(HWND hWnd);
extern VOID Do_ID_OPTIONS_ENABLESTRATEGIES(HWND hWnd);
extern VOID Do_ID_OPTIONS_MISCOPTIONS(HWND hWnd);


#endif // #ifndef _SUDO_DIALOGS_HXX_
// eof - Sudo_Dialogs.hxx

