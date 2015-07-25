// Sudo_BMP.cxx

#include "Sudoku.hxx"

// Sometimes is is just better to SEE the image ;=))
#ifdef WIN32    // use Windows BMP genration

// WIDTHBYTES takes # of bits in a scan line and rounds up to nearest
//  dword (32-bits). The # of bits in a scan line would typically be
//  the pixel width (bmWidth) times the BPP (bits-per-pixel = bmBitsPixel)
#define WIDTHBYTES(bits)      (((bits) + 31) / 32 * 4)
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')	/* Simple "BM" ... */
#define  MALLOC(a)      new BYTE[a]
#define  MFREE(a)       delete a

size_t stBmpCoreHdr = sizeof(BITMAPCOREHEADER);
size_t stBmpInfoHdr = sizeof(BITMAPINFOHEADER);
size_t stBmpV4Hdr   = sizeof(BITMAPV4HEADER);
size_t stBmpV5Hdr   = sizeof(BITMAPV5HEADER);

// #define	Win30_Bmp	(sizeof (BITMAPINFOHEADER))
//#define IS_WIN30_DIB(lpbi)  ( (*(LPDWORD) (lpbi)) == Win30_Bmp )
#define IS_CORE_DIB(lpbi)  ( (*(LPDWORD) (lpbi)) == stBmpCoreHdr )
#define IS_WIN30_DIB(lpbi)  ( (*(LPDWORD) (lpbi)) == stBmpInfoHdr )
#define IS_V4_DIB(lpbi)  ( (*(LPDWORD) (lpbi)) == stBmpV4Hdr )
#define IS_V5_DIB(lpbi)  ( (*(LPDWORD) (lpbi)) == stBmpV5Hdr )

size_t DIBNumColors ( void * pv )
{
   INT                 bits;
   LPBITMAPINFOHEADER  lpbi;
   LPBITMAPCOREHEADER  lpbc;

   lpbi = ((LPBITMAPINFOHEADER)pv);
   lpbc = ((LPBITMAPCOREHEADER)pv);

   /*  With the BITMAPINFO format headers, the size of the palette
    *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
    *  is dependent on the bits per pixel ( = 2 raised to the power of
    *  bits/pixel).
    */
   if (lpbi->biSize != sizeof(BITMAPCOREHEADER)){
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    } else
        bits = lpbc->bcBitCount;

    switch (bits)
    {
    case 1: return 2;
    case 4: return 16;
    case 8: return 256;
    /* A 24 bitcount DIB has no color table */
    }
    return 0;
}

size_t DIBPaletteSize (LPSTR lpbi)
{
   if (IS_WIN30_DIB (lpbi))
      return (DIBNumColors (lpbi) * sizeof (RGBQUAD));
   else
      return (DIBNumColors (lpbi) * sizeof (RGBTRIPLE));
}

size_t PaletteSize (LPSTR lpbi)
{
   return DIBPaletteSize( lpbi );
}

DWORD	OffsetToColor( LPSTR lpbi )
{
//	return( *(LPDWORD)lpbi );
	DWORD	   dwi;
	LPDWORD	lpdw;

	lpdw = (LPDWORD)lpbi;
	dwi = *lpdw;
	if(( dwi == sizeof(BITMAPINFOHEADER) ) ||
		( dwi == sizeof(BITMAPCOREHEADER) ) )
	{
		/* this is ok */
	}
	else
	{
		dwi = sizeof(BITMAPINFOHEADER);
	}
	return dwi;
}

DWORD	OffsetToBits( LPSTR lpbi )
{
	return( OffsetToColor(lpbi) + PaletteSize(lpbi) );
}

LPSTR FindDIBBits (LPSTR lpbi)
{
   return( lpbi + OffsetToBits( lpbi ));
}

// InitBitmapInfoHeader - InitBMPInfoHeader
void InitBitmapInfoHeader( LPBITMAPINFOHEADER lpBmInfoHdr,
						  DWORD dwWidth,
						  DWORD dwHeight,
						  int nBPP )
{
	int		iBPP;
	//dv_fmemset( lpBmInfoHdr, 0, sizeof (BITMAPINFOHEADER) );
	ZeroMemory( lpBmInfoHdr, sizeof (BITMAPINFOHEADER) );

	lpBmInfoHdr->biSize      = sizeof(BITMAPINFOHEADER);
	lpBmInfoHdr->biWidth     = dwWidth;
	lpBmInfoHdr->biHeight    = dwHeight;
	lpBmInfoHdr->biPlanes    = 1;

	// Fix Bits per Pixel
	if( nBPP <= 1 )
		iBPP = 1;
	else if( nBPP <= 4 )
		iBPP = 4;
	else if( nBPP <= 8 )
		iBPP = 8;
	else
		iBPP = 24;

	lpBmInfoHdr->biBitCount  = iBPP;
	lpBmInfoHdr->biSizeImage = WIDTHBYTES(dwWidth * iBPP) * dwHeight;
}

HANDLE BMPToDIB( HBITMAP hBitmap, HPALETTE hPal )
{
	BITMAP				Bitmap;
	BITMAPINFOHEADER	bmInfoHdr;
	LPBITMAPINFOHEADER	lpbmInfoHdr;
	LPSTR				lpBits;
	HDC					hMemDC;
	HANDLE				hDIB;
	HPALETTE			hOldPal;
   size_t         dwSize;

	// Do some setup -- make sure the Bitmap passed in is valid,
	//  get info on the bitmap (like its height, width, etc.),
	//  then setup a BITMAPINFOHEADER.
	hOldPal = NULL;
	hDIB    = NULL;

	if( !hBitmap )
		return hDIB;

	if( !GetObject( hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap ) )
		return hDIB;

	InitBitmapInfoHeader( &bmInfoHdr,
		Bitmap.bmWidth,
		Bitmap.bmHeight,
		Bitmap.bmPlanes * Bitmap.bmBitsPixel );

	// Now allocate memory for the DIB.
	// Then, set the BITMAPINFOHEADER
	//  into this memory, and
	// find out where the bitmap bits go.
	dwSize = ( sizeof(BITMAPINFOHEADER) +
               PaletteSize((LPSTR) &bmInfoHdr) + bmInfoHdr.biSizeImage );
	hDIB = MALLOC( dwSize );
	if( !hDIB )
		return hDIB;

	lpbmInfoHdr  = (LPBITMAPINFOHEADER)hDIB;   // CAST DIB HANDLE
	*lpbmInfoHdr = bmInfoHdr;
	lpBits       = FindDIBBits( (LPSTR)lpbmInfoHdr );

	// Now, we need a DC to hold our bitmap.
	// If the app passed us
	//  a palette, it should be selected into the DC.
	hMemDC = GetDC( NULL );
	if( !hMemDC ) {
		MFREE( hDIB );
		hDIB = NULL;
		return hDIB;
	}

	if( hPal ) {
		hOldPal = SelectPalette( hMemDC, hPal, FALSE );
		RealizePalette( hMemDC );
	}

	// We're finally ready to get the DIB.
	// Call the driver and let it party on our bitmap.
	// It will fill in the color table,
	//  and bitmap bits of our global memory block.
	if( ! GetDIBits( hMemDC,
            		hBitmap,
		            0,
		            Bitmap.bmHeight,
		            lpBits,
		            (LPBITMAPINFO)lpbmInfoHdr,
		            DIB_RGB_COLORS ) )
	{
		MFREE( hDIB );
		hDIB = NULL;
	}

	// Finally, clean up and return.
	if( hOldPal )
		SelectPalette( hMemDC, hOldPal, FALSE );

	ReleaseDC( NULL, hMemDC );

	return hDIB;   // RETURN memory pointer
}

// NOTE: This is NOT a GlobalAlloc (HGLOBAL), but actually
// just a memory pointer. It was allocated using 'new' ...
BOOL  WriteBMPFile( PTSTR pf, HANDLE hDIB )
{
    BOOL                 flg = FALSE;
    FILE *               fh;
	BITMAPFILEHEADER	   hdr;
	LPBITMAPINFOHEADER   lpbi;
    SIZE_T               dws, dwis;
    if( !hDIB || !pf )
        return flg;

   // ===================================================================
   lpbi = (LPBITMAPINFOHEADER)hDIB;  // CAST DIB HANDLE TO WHAT IT IS ...
   // ===================================================================

   dws = lpbi->biSizeImage;   // extract IMAGE SIZE, and COMPUTE IMAGE SIZE
   dwis = WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) * lpbi->biHeight;
   if( dws != dwis ) // computed versus calculated should be the SAME!!!
      return flg;

   // add to IMAGE SIZE
   dws += lpbi->biSize +    // size of 	LPBITMAPINFOHEADER
          PaletteSize((LPSTR)lpbi ); // plus PALETTE (if any)

   //if( dws < dwCopySize )
   //   dws = dwCopySize; // update to this SIZE
   // Fill in the fields of the file header
   ZeroMemory( &hdr, sizeof(BITMAPFILEHEADER) );
   hdr.bfType		 = DIB_HEADER_MARKER;
   hdr.bfReserved1 = 0;
   hdr.bfReserved2 = 0;
   hdr.bfOffBits   = (DWORD)sizeof(BITMAPFILEHEADER) +   // file header plus
                        lpbi->biSize +    // size of 	LPBITMAPINFOHEADER
                        PaletteSize((LPSTR)lpbi ); // plus PALETTE (if any)
   hdr.bfSize		 = (DWORD)(dws + sizeof (BITMAPFILEHEADER));
   // CREATE the OUTPUT FILE
   // ======================
   fh = fopen( pf, "wb" );
   if( fh ) {
      // write header to file
      if( fwrite( &hdr, 1, sizeof(BITMAPFILEHEADER), fh ) ==
         sizeof( BITMAPFILEHEADER ) ) {
         // write the DATA BLOCK, after the FILE HEADER
         if( fwrite( lpbi, 1, dws, fh ) == dws ) {
            flg = TRUE; // written DIB to Windows BMP file
         }
      }
      fclose(fh);
   }
   return flg;
}

void Do_Write_BMP( HWND hWnd )
{
    BOOL bRet = FALSE;
    char *pfile = "tempsudo.bmp";
    HDC memDC = 0;
    HBITMAP old;
    HBITMAP bitmap = 0;
    HDC hdc = GetDC(hWnd);
    if (!hdc) return;
    SIZE sz;
    RECT rc = g_rcSudokuLeg;
    sz.cx = rc.right - rc.left;
    sz.cy = rc.bottom - rc.top;

    memDC = CreateCompatibleDC(hdc);
    if( !memDC ) goto Cleanup;
    bitmap = CreateCompatibleBitmap( hdc, sz.cx, sz.cy );
    if( !bitmap ) goto Cleanup;
    old  = (HBITMAP)SelectObject(memDC,bitmap);
    BitBlt( memDC, 0, 0, sz.cx, sz.cy, hdc, rc.left, rc.top, SRCCOPY );
    HBITMAP _hbm;
    _hbm = (HBITMAP)GetCurrentObject(memDC, OBJ_BITMAP);
    if(_hbm) {
        HANDLE _hdib = BMPToDIB(_hbm, NULL );
        if( _hdib ) {
            bRet = WriteBMPFile(pfile,_hdib);
            MFREE(_hdib);
        }
    }
Cleanup:
    if (memDC) DeleteObject(memDC);
    if (bitmap) DeleteObject(bitmap);
    ReleaseDC(hWnd,hdc);
    if (bRet)
        sprtf("Written BMP file [%s]\n",pfile);
    else
        sprtf("Failed to write BMP file!\n");
    set_repaint(false);
}

#else // !#ifdef WIN32 - TODO: Need alternative to write an image, bmp or otherwise
void Do_Write_BMP( HWND hWnd )
{
    sprtf("TODO: Write BMP file NOT implemented in unix yet!\n");
    set_repaint2();
}

#endif // #ifdef WIN32 y/n - write image file of GUI

// eof - Sudo_BMP.cxx


