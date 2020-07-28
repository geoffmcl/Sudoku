// Sudo_File.hxx
#ifndef _SUDO_FILE_HXX_
#define _SUDO_FILE_HXX_
#include <vector>
using namespace std;

extern void Set_Window_Title();

extern VOID Do_ID_FILE_OPEN(HWND hWnd);
extern VOID Do_ID_FILE_SAVE(HWND hWnd);

extern bool is_curr_box_valid();
extern bool is_curr_box_valid_for_paint();

extern PABOX2 get_curr_box();
extern void Auto_Load_file(HWND hWnd);
extern bool Load_a_file( HWND hWnd, LPTSTR lpstrFile );
extern void Free_Boxes2();
extern PABOX2 Copy_Box2( PABOX2 pb );
extern void Add_Box2(PABOX2 pb);
extern PABOX2 Get_Box2( int offset );
extern int Get_Box2_Count();
extern PABOX2 get_file_box();   // initial file loaded box

#define flg_ADDSCREEN 0x01
#define flg_SHOWVALS  0x02
#define flg_NOSORT    0x03
#define flg_NOPUTPUT  0x04

#define CHKVAL(v) (( v >= 0) && (v < 9))
extern VOID Do_ID_FILE_RE(HWND hwnd);
extern void Write_Temp_File();

#define sff_NO_LENGTH_CHECK   0x01
#define sff_NO_ADD_ASCII      0x02
#define sff_NO_CHANGE_RESET   0x04
#define sff_NO_RESET_TITLE    0x08
#define sff_NO_ADD_CSV        0x10
#define sff_NO_CHANGE_ACTIVE  0x20
#define sff_ADD_ASCII_BLOCK   0x40
#define sff_ADD_CANDIDATES    0x80

// special st when writing just a TEMPORARY file
#define sff_TEMP_FILE (sff_NO_LENGTH_CHECK | sff_NO_CHANGE_RESET | sff_NO_ADD_CSV | \
    sff_NO_RESET_TITLE | sff_NO_CHANGE_ACTIVE | sff_ADD_ASCII_BLOCK | sff_ADD_CANDIDATES)

extern bool Save_a_File( HWND hWnd, char *pf, int flag = 0 );
extern void Reset_Active_File(char *file);
extern char *Get_Act_File();
extern char *g_pSpecial; // = "Untitled";  // when a box is say generated
extern void File_Reset();

extern VOID Do_ID_FILE_OPENDIRECTORY(HWND hWnd);

extern char *get_ASCII_81_Stg(PABOX2 pb); // get simple list of the 81 puzzle values


#endif // #ifndef _SUDO_FILE_HXX_
// eof - Sudo_File.hxx

