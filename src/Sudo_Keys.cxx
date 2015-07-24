// Sudo_keys.cxx

#include "Sudoku.hxx"

bool Check_Exit() 
{
    if (g_bChanged && total_empty_count) {
        int res = MB2("The Sudoku box has been changed\nDo you want to SAVE the results.\nClick Yes to abort this exit.\nClick [ NO ] to exit without saving.",
            "Save on Exit?");
        if (res == IDYES) {
            Post_Command(ID_FILE_SAVE);
            return false;
        }

    }
    return true;
}


VOID Do_WM_KEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
#ifdef WIN32
///////////////////////////////////////////////////////////////////
    switch (wParam)
    {
    case VK_UP:
        Move_Selection_Up(hWnd);
        break;

    case VK_DOWN:
        Move_Selection_Down(hWnd);
        break;

    case VK_LEFT:
        Move_Selection_Left(hWnd);
        break;

    case VK_RIGHT:
        Move_Selection_Right(hWnd);
        break;
    case VK_TAB:
        if (g_bAShiftDown)
            Move_Selection_Left(hWnd);
        else
            Move_Selection_Right(hWnd);
        break;

    case VK_PRIOR:   //  PAGE UP key 
        // PostMessage (hWnd, WM_VSCROLL, SB_PAGEUP,   0L);
      //i = get_page_key_steps();  // 10;
      //while(i--)
      //   increase_lat_offset();
      //Do_InvalidateRect(hWnd);
      break;

    case VK_NEXT:    // PAGE DOWN key
        // PostMessage (hWnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
      //i = get_page_key_steps();  // 10;
      //while(i--)
      //   decrease_lat_offset();
      //Do_InvalidateRect(hWnd);
      break;

    case VK_HOME:
        // PostMessage (hWnd, WM_HSCROLL, SB_PAGEUP,   0L);
        // Select top/left
        Move_Selection(hWnd, 0, 0);
        break;

    case VK_END:
        // PostMessage (hWnd, WM_HSCROLL, SB_PAGEDOWN, 0L);
        // Select bottom/right
        Move_Selection(hWnd, 8, 8);
        break;

    case VK_SHIFT:
        g_bAShiftDown = TRUE;
        break;
    case VK_CONTROL:
        g_bCtrlDown = TRUE; // CTRL key
        break;
    case VK_MENU:
        g_bAltDown = TRUE;  // ALT key  
        break;
    case VK_F3: // F3 function key
        //Do_F3_Key( hWnd );
        break;
    case VK_F5:
        Post_Command(ID_OPTIONS_ONESTEP);
        break;
    //default:
    //    if (g_bCtrlDown) {
    //        // hmmm, Ctrl+R say arrives here
    //        int row, col;
    //        get_curr_selRC( &row, &col );
    //        bool valid = (Is_Selected_Valid() && VALIDRC(row,col)) ? true : false;
    //   }
    }
    if ((wParam >= 0x31) && (wParam <= 0x39)) {
        // a numeric key
        int nval = wParam - '0';
        PABOX2 pb = get_curr_box();
        if (g_bCtrlDown) {
            int row, col;
            SET set;
            get_curr_selRC( &row, &col );
            bool valid = (Is_Selected_Valid() && VALIDRC(row,col)) ? true : false;
            if (!valid)
                return; // NO SELECTION
            int val = pb->line[row].val[col];
            if (val) {
                sprtf("Cell has value %d, so no elim of %d\n", val, nval);
                return;
            }
            COPY_SET(&set,&pb->line[row].set[col]);
            if (set.val[nval - 1]) {
                // cell has this value
                Stack_Box(pb);
                sprtf("Usr.Elim of %d@%c%d\n", nval, (char)row + 'A', col + 1);
                pb->line[row].set[col].val[nval - 1] = 0;
                set_repaint();
            }
        }
    }
///////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32
}

VOID Do_WM_KEYUP( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
#ifdef WIN32
///////////////////////////////////////////////////////////////////
   switch (wParam)
   {
   case VK_UP:
   case VK_DOWN:
   case VK_PRIOR:
   case VK_NEXT:
      //PostMessage (hWnd, WM_VSCROLL, SB_ENDSCROLL, 0L);
      break;

   case VK_HOME:
   case VK_END:
   case VK_LEFT:
   case VK_RIGHT:
      //PostMessage (hWnd, WM_HSCROLL, SB_ENDSCROLL, 0L);
      break;
   case VK_SHIFT:
       g_bAShiftDown = FALSE;
       break;
   case VK_CONTROL:
      g_bCtrlDown = FALSE; // CTRL key
      break;
   case VK_MENU:
      g_bAltDown = FALSE;  // ALT key  
      break;
   case VK_DELETE:  // DEL key
       Post_Command(ID_OPTIONS_DELETECELL); // If selection and has a value, REMOVE it...
       break;
   case VK_INSERT:
       Check_For_Insertion();   // Do we have a SELECTION, and is there ONLY
       // ONE valid value fo rhte cell, then put in in
       break;
   }
///////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32
}

// WM_CHAR
void Do_WM_CHAR( HWND hWnd, WPARAM wParam )
{
    PABOX2 pb = get_curr_box();
    int nval = wParam - '0';
    int row, col;
    SET set;
    get_curr_selRC( &row, &col );
    bool valid = (Is_Selected_Valid() && VALIDRC(row,col)) ? true : false;
    if (wParam == 0x1b) {   // ESC key - EXIT APP
        //if (Check_Exit()) {
        //    DestroyWindow(hWnd);
        //}
        if (g_bAShiftDown)
            Post_Command(ID_FILE_EXITWITHOUTSAVE);
        else
            Post_Command(IDM_EXIT);
        return;
    } else if (ISDIGIT(wParam)) {
        if (nval) {
            // Got a NUMBER 1 t0 9 - can this be applied to the SELECTED cell
            if (valid) {
                int val = pb->line[row].val[col];
                valid = false;
                if (!val) {
                    int i;
                    Fill_SET(&set); // fill ALL values
                    Eliminate_Same_Col( pb, col, &set );
                    Eliminate_Same_Row(pb, row, &set);
                    Eliminate_Same_Box( row, col, pb, &set );
                    // this now seems suspect - the user can enter ANY easily verified valid number
                    // should NOT use any LOGIC to bar the entry other than the above simple rules of play
                    // That is let the user make mistakes at this point
                    // Could mark the value in some way if it can be eliminated in some way, but maybe not ;=()
                    //if (use_elim_on_square) Eliminate_on_Square( row, col, pb, &set ); // Eliminate_Square_Values
                    //if ( !(Get_Set_Cnt(&set) < 2) ) {
                    //    Eliminate_Unique_In_Row( row, col, pb, &set );
                    //}
                    for (i = 0; i < 9; i++) {
                        val = set.val[i]; // get any value
                        if (val == nval) {
                            // of User entered value is one of the options
                            // SET IT NOW
                            Change_Box(pb,row,col,val);
                            valid = true;
                            break;
                        }
                    }
                    if (!valid && g_bWarnOBV) { // Entered a NON-VALID value
                        // TODO - MBW("What is this")
                    }
                }
            }
        } else {
            // User entered ZERO - this could mean several things
            // for now no noise - just bump to next cell if 'valid' selection
        }
        if (valid && g_bAutoTab) {
            // Bump to NEXT location
            // ---------------------
            col++;
            if (col >= 9) {
                col = 0;
                row++;
                if (row >= 9)
                    row = 0;
            }
            Move_Selection(hWnd, row, col);
            // -----------------------
        }
        return;
    }
    // =======================
    // NOT ESC, or DIGIT entry
    // =======================
    switch (wParam) 
    {
    case 'a':
        // g_bAutoComplete = TRUE; - this is NOT used pesently
        //g_bAutoSolve = TRUE; // but this will take steps...
        break;
    case 'b':
    case 'B':
        Do_Write_BMP(hWnd);
        break;

    // case 'h': // in accellerator
    case 'g':
        Post_Command(ID_EDIT_GENERATE);
        break;
    case 'o':
        Post_Command(ID_OPTIONS_ONESTEP);
        //Do_ID_OPTIONS_ONESTEP(hWnd);
        break;
    case 'r':   // Reset candidate to ALL
        Reset_Candidates();
        set_repaint();
        break;
    case 't':   // also in accelerator
        //run_test(get_curr_box());
        run_test3(get_curr_box());
        break;
    case 'W':
        Post_Command(ID_FILE_SAVETEMP);
        break;
    case 'w':
        Write_Temp_File();
        set_repaint(false);
        break;
    }
}

// eof - Sudo_Keys.cxx
