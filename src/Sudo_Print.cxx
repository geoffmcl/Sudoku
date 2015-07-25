// Sudo_Print.cxx

#include "Sudoku.hxx"

#ifdef WIN32 // Windows print the window image
//////////////////////////////////////////////////////////////////////////////////////////
// forward references
HDC GetPrinterDC(void);
HBITMAP Create24BPPDIBSection(HDC hDC, int iWidth, int iHeight); // Create a 24-bit-per-pixel surface.

// Create a copy of the current system palette.
HPALETTE GetSystemPalette()
{
    HDC hDC;
    HPALETTE hPal;
    //HANDLE hLogPal;
    LPLOGPALETTE lpLogPal;


    // Get a DC for the desktop.
    hDC = GetDC(NULL);

    // Check to see if you are a running in a palette-based video mode.
    if (!(GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE)) {
        ReleaseDC(NULL, hDC);
        return NULL;
    }

    // Allocate memory for the palette.
    lpLogPal = (LPLOGPALETTE)GlobalAlloc(GPTR, sizeof(LOGPALETTE) + 256 *
                            sizeof(PALETTEENTRY));
    if (!lpLogPal)
        return NULL;

    // Initialize.
    lpLogPal->palVersion = 0x300;
    lpLogPal->palNumEntries = 256;

    // Copy the current system palette into the logical palette.
    GetSystemPaletteEntries(hDC, 0, 256,
        (LPPALETTEENTRY)(lpLogPal->palPalEntry));

    // Create the palette.
    hPal = CreatePalette(lpLogPal);

    // Clean up.
    GlobalFree(lpLogPal);
    ReleaseDC(NULL, hDC);

    return hPal;
}

// Print the entire contents (including the non-client area) of
// the specified window to the default printer.
BOOL PrintWindowToDC(HWND hWnd)
{
    HBITMAP hbm;
    HDC     hdcPrinter;
    HDC     hdcMemory;
    HDC     hdcWindow;
    int     iWidth;
    int     iHeight;
    DOCINFO di;
    RECT    rc;
    DIBSECTION ds;
    HPALETTE   hPal;


    // Do you have a valid window?
    if (!IsWindow(hWnd))
        return FALSE;

    // Get a HDC for the default printer.
    hdcPrinter = GetPrinterDC();
    if (!hdcPrinter)
        return FALSE;

    // Get the HDC for the entire window.
    hdcWindow  = GetWindowDC(hWnd);

    // Get the rectangle bounding the window.
    GetWindowRect(hWnd, &rc);

    // Adjust coordinates to client area.
    OffsetRect(&rc, -rc.left, -rc.top);

    // Get the resolution of the printer device.
    iWidth  = GetDeviceCaps(hdcPrinter, HORZRES);
    iHeight = GetDeviceCaps(hdcPrinter, VERTRES);

    // Create the intermediate drawing surface at window resolution.
    hbm = Create24BPPDIBSection(hdcWindow, rc.right, rc.bottom);
    if (!hbm) {
        DeleteDC(hdcPrinter);
        ReleaseDC(hWnd, hdcWindow);
        return FALSE;
    }

    // Prepare the surface for drawing.
    hdcMemory = CreateCompatibleDC(hdcWindow);
    SelectObject(hdcMemory, hbm);

    // Get the current system palette.
    hPal = GetSystemPalette();

    // If a palette was returned.
    if (hPal) {
        // Apply the palette to the source DC.
        SelectPalette(hdcWindow, hPal, FALSE);
        RealizePalette(hdcWindow);

        // Apply the palette to the destination DC.
        SelectPalette(hdcMemory, hPal, FALSE);
        RealizePalette(hdcMemory);
    }

    // Copy the window contents to the memory surface.
    BitBlt(hdcMemory, 0, 0, rc.right, rc.bottom,
        hdcWindow, 0, 0, SRCCOPY);

    // Prepare the DOCINFO.
    ZeroMemory(&di, sizeof(di));
    di.cbSize = sizeof(di);
    di.lpszDocName = "Window Contents";

    // Initialize the print job.
    if (StartDoc(hdcPrinter, &di) > 0) {

        // Prepare to send a page.
        if (StartPage(hdcPrinter) > 0) {

            // Retrieve the information describing the surface.
            GetObject(hbm, sizeof(DIBSECTION), &ds);

            // Print the contents of the surface.
            StretchDIBits(hdcPrinter,
                0, 0, iWidth, iHeight,
                0, 0, rc.right, rc.bottom,
                ds.dsBm.bmBits,
                (LPBITMAPINFO)&ds.dsBmih,
                DIB_RGB_COLORS,
                SRCCOPY);

            // Let the driver know the page is done.
            EndPage(hdcPrinter);
        }

        // Let the driver know the document is done.
        EndDoc(hdcPrinter);
    }

    // Clean up the objects you created.
    DeleteDC(hdcPrinter);
    DeleteDC(hdcMemory);
    ReleaseDC(hWnd, hdcWindow);
    DeleteObject(hbm);
    if (hPal)
        DeleteObject(hPal);
    return TRUE;
}


HRESULT PrintIt( HWND hWnd, PRINTDLGEX *ppd )
{
    HRESULT hResult = 0;
    HDC     hdcPrinter = ppd->hDC;
    int     iWidth;
    int     iHeight;
    HDC     hdcMemory;
    HDC     hdcWindow;
    RECT    rc = g_rcSudokuLeg;
    SIZE    sz;
    HBITMAP hbm;
    DOCINFO di;
    HPALETTE   hPal;
    DIBSECTION ds;

    if (!hdcPrinter)
        return E_OUTOFMEMORY;

    sz.cx = rc.right - rc.left;
    sz.cy = rc.bottom - rc.top;

    // Get the resolution of the printer device.
    iWidth  = GetDeviceCaps(hdcPrinter, HORZRES);
    iHeight = GetDeviceCaps(hdcPrinter, VERTRES);

    hdcWindow = GetDC(hWnd);
    if (!hdcWindow)
        return E_OUTOFMEMORY;

    // Create the intermediate drawing surface at window resolution.
    hbm = Create24BPPDIBSection(hdcWindow, sz.cx, sz.cy);
    if (!hbm) {
        ReleaseDC(hWnd, hdcWindow);
        return FALSE;
    }
    // Prepare the surface for drawing.
    hdcMemory = CreateCompatibleDC(hdcWindow);
    if (!hdcMemory) {
        ReleaseDC(hWnd, hdcWindow);
        DeleteObject(hbm);
        return FALSE;
    }
    SelectObject(hdcMemory, hbm);

    // Get the current system palette - if any.
    hPal = GetSystemPalette();
    // If a palette was returned.
    if (hPal) {
        // Apply the palette to the source DC.
        SelectPalette(hdcWindow, hPal, FALSE);
        RealizePalette(hdcWindow);

        // Apply the palette to the destination DC.
        SelectPalette(hdcMemory, hPal, FALSE);
        RealizePalette(hdcMemory);
    }

    // Copy the window contents to the memory surface.
    BitBlt(hdcMemory, 0, 0, sz.cx, sz.cy,
        hdcWindow, rc.left, rc.top, SRCCOPY);

    // Prepare the DOCINFO.
    ZeroMemory(&di, sizeof(di));
    di.cbSize = sizeof(di);
    di.lpszDocName = Get_Act_File(); // "Window Contents";

    // Initialize the print job.
    if (StartDoc(hdcPrinter, &di) > 0) {

        // Prepare to send a page.
        if (StartPage(hdcPrinter) > 0) {

            // Retrieve the information describing the surface.
            GetObject(hbm, sizeof(DIBSECTION), &ds);

            // Print the contents of the surface.
            StretchDIBits(hdcPrinter,
                0, 0, iWidth, iHeight,
                0, 0, sz.cx, sz.cy,
                ds.dsBm.bmBits,
                (LPBITMAPINFO)&ds.dsBmih,
                DIB_RGB_COLORS,
                SRCCOPY);

            // Let the driver know the page is done.
            EndPage(hdcPrinter);
            hResult = TRUE;
        }

        // Let the driver know the document is done.
        EndDoc(hdcPrinter);
    }

    // Clean up the objects you created.
    DeleteDC(hdcMemory);
    ReleaseDC(hWnd, hdcWindow);
    DeleteObject(hbm);
    if (hPal)
        DeleteObject(hPal);
    return hResult;
}


// hWnd is the window that owns the property sheet.
HRESULT DisplayPrintPropertySheet(HWND hWnd)
{
    HRESULT hResult = 0;
    PRINTDLGEX pdx = {0};
    LPPRINTPAGERANGE pPageRanges = NULL;

    // Allocate an array of PRINTPAGERANGE structures.
    pPageRanges = (LPPRINTPAGERANGE) GlobalAlloc(GPTR, 10 * sizeof(PRINTPAGERANGE));
    if (!pPageRanges)
        return 0;

    //  Initialize the PRINTDLGEX structure.
    pdx.lStructSize = sizeof(PRINTDLGEX);
    pdx.hwndOwner = hWnd;
    pdx.hDevMode = NULL;
    pdx.hDevNames = NULL;
    pdx.hDC = NULL;
    pdx.Flags = PD_RETURNDC | PD_COLLATE;
    pdx.Flags2 = 0;
    pdx.ExclusionFlags = 0;
    pdx.nPageRanges = 0;
    pdx.nMaxPageRanges = 10;
    pdx.lpPageRanges = pPageRanges;
    pdx.nMinPage = 1;
    pdx.nMaxPage = 1000;
    pdx.nCopies = 1;
    pdx.hInstance = 0;
    pdx.lpPrintTemplateName = NULL;
    pdx.lpCallback = NULL;
    pdx.nPropertyPages = 0;
    pdx.lphPropertyPages = NULL;
    pdx.nStartPage = START_PAGE_GENERAL;
    pdx.dwResultAction = 0;
    
    //  Invoke the Print property sheet.
    
    hResult = PrintDlgEx(&pdx);

    if ((hResult == S_OK) && (pdx.dwResultAction == PD_RESULT_PRINT) && pdx.hDC) 
    {
        // User clicked the Print button, so use the DC and other information returned in the 
        // PRINTDLGEX structure to print the document.
        hResult = PrintIt( hWnd, &pdx );
    } else {
        hResult = 0;
    }

    if (pdx.hDevMode != NULL) 
        GlobalFree(pdx.hDevMode); 
    if (pdx.hDevNames != NULL) 
        GlobalFree(pdx.hDevNames); 
    if (pdx.lpPageRanges != NULL)
        GlobalFree(pPageRanges);

    if (pdx.hDC != NULL) 
        DeleteDC(pdx.hDC);

    return hResult;
}



VOID Do_ID_FILE_PRINT( HWND hWnd )
{

    HRESULT res = DisplayPrintPropertySheet(hWnd);

}

// Return a HDC for the default printer.
HDC GetPrinterDC(void)
{
    PRINTDLG pdlg;
    memset( &pdlg, 0, sizeof( PRINTDLG ) );
    pdlg.lStructSize = sizeof( PRINTDLG );
    pdlg.Flags = PD_RETURNDEFAULT | PD_RETURNDC;
    PrintDlg( &pdlg );
    return pdlg.hDC;
}


// Create a 24-bit-per-pixel surface.
HBITMAP Create24BPPDIBSection(HDC hDC, int iWidth, int iHeight)
{
    BITMAPINFO bmi;
    HBITMAP hbm;
    LPBYTE pBits;

    // Initialize to 0s.
    ZeroMemory(&bmi, sizeof(bmi));

    // Initialize the header.
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iWidth;
    bmi.bmiHeader.biHeight = iHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Create the surface.
    hbm = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void **)&pBits, NULL, 0);

    return(hbm);
}
	
BOOL CALLBACK PaintHook(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
    LPRECT lprc; 
    COLORREF crMargRect; 
    HDC hdc, hdcOld; 
 
    switch (uMsg) 
    { 
        // Draw the margin rectangle. 
        case WM_PSD_MARGINRECT: 
            hdc = (HDC) wParam; 
            lprc = (LPRECT) lParam; 
 
            // Get the system highlight color. 
            crMargRect = GetSysColor(COLOR_HIGHLIGHT); 
 
            // Create a dash-dot pen of the system highlight color and 
            // select it into the DC of the sample page. 
            hdcOld = (HDC) SelectObject(hdc, CreatePen(PS_DASHDOT, 1, crMargRect)); 
 
            // Draw the margin rectangle. 
            Rectangle(hdc, lprc->left, lprc->top, lprc->right, lprc->bottom); 
 
            // Restore the previous pen to the DC. 
            SelectObject(hdc, hdcOld); 
            return TRUE; 
 
        default: 
            return FALSE; 
    } 
    return TRUE; 
}

VOID Page_Setup( HWND hwnd )
{
    PAGESETUPDLG psd;    // common dialog box structure

    // Initialize PAGESETUPDLG
    ZeroMemory(&psd, sizeof(psd));
    psd.lStructSize = sizeof(psd);
    psd.hwndOwner   = hwnd;
    psd.hDevMode    = NULL; // Don't forget to free or store hDevMode.
    psd.hDevNames   = NULL; // Don't forget to free or store hDevNames.
    psd.Flags       = PSD_INTHOUSANDTHSOFINCHES | PSD_MARGINS | 
                  PSD_ENABLEPAGEPAINTHOOK; 
    psd.rtMargin.top = 1000;
    psd.rtMargin.left = 1250;
    psd.rtMargin.right = 1250;
    psd.rtMargin.bottom = 1000;
    psd.lpfnPagePaintHook = (LPPAGEPAINTHOOK)PaintHook;

    if (PageSetupDlg(&psd)==TRUE)
    {
        // check paper size and margin values here.
    }
    if (psd.hDevMode)
        GlobalFree(psd.hDevMode);
    if (psd.hDevNames)
        GlobalFree(psd.hDevNames);

}

VOID Direct_Print( HWND hwnd )
{
    PRINTDLG pd;

    // Initialize PRINTDLG
    ZeroMemory(&pd, sizeof(pd));
    pd.lStructSize = sizeof(pd);
    pd.hwndOwner   = hwnd;
    pd.hDevMode    = NULL;     // Don't forget to free or store hDevMode.
    pd.hDevNames   = NULL;     // Don't forget to free or store hDevNames.
    pd.Flags       = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC; 
    pd.nCopies     = 1;
    pd.nFromPage   = 0xFFFF; 
    pd.nToPage     = 0xFFFF; 
    pd.nMinPage    = 1; 
    pd.nMaxPage    = 0xFFFF; 

    if (PrintDlg(&pd)==TRUE) 
    {
        // GDI calls to render output. 

        // Delete DC when done.
        DeleteDC(pd.hDC);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
#else // !#ifdef WIN32 - TODO: Need alterative print window service
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef WIN32 y/n - print window

// eof - Sudo_Print.cxx
