// Sudo_Change.hxx
#ifndef _SUDO_CHANGE_HXX_
#define _SUDO_CHANGE_HXX_


// extern void Add_2_Change(int Row, int Col, int nval, int flag = 0); // NOTE: A NEGATIVE nval denote DELETED
extern BOOL Change_Box(PABOX2 pb, int row, int col, int val, int flag = 0);

extern void Do_ID_EDIT_REDO(HWND hWnd);
extern void Do_ID_EDIT_UNDO(HWND hWnd);

extern void Kill_Change();
extern bool Got_Change_Stack();
extern bool Got_Redo_Stack();

extern PABOX2 Stack_Box( PABOX2 pb );

// ERRORS returned by the service
#define SERR_NONE   0
#define SERR_BADSV  1
#define SERR_BADRC  2
#define SERR_NOBP   3
#define SERR_NOFLG  4
#define SERR_NOCND  5
#define SERR_MKSLN  6
#define SERR_ISMKD  7
extern int OR_Row_Col_SetVal_with_Flag( PABOX2 pb, int row, int col, int setval,
                                        uint64_t flag, bool force = false );
extern int set_error;
extern char *get_last_or_setval_error(); // { return last_or_setval_error; }

extern void set_curr_box(PABOX2 pb);

// extra painting of CHAINS etc
// ============================
extern void Set_Paint_Chains_Invalid();


#endif // #ifndef _SUDO_CHANGE_HXX_
// eof - Sudo_Change.hxx
