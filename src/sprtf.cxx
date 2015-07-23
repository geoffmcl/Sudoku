// sprtf.cxx

#include <Windows.h>
#include <stdio.h> // fopen()...
#include "Sudoku.hxx"
#include "sprtf.hxx"
#include "Sudo_Paint.hxx"

// sprtf.cxx
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif // #ifndef _CRT_SECURE_NO_DEPRECATE
#pragma warning( disable:4996 )

#ifndef MX_ONE_BUF
#define MX_ONE_BUF 1024
#endif
#ifndef MX_BUFFERS
#define MX_BUFFERS 1024
#endif

static char _s_strbufs[MX_ONE_BUF * MX_BUFFERS];
static int iNextBuf = 0;

char *GetNxtBuf()
{
   iNextBuf++;
   if(iNextBuf >= MX_BUFFERS)
      iNextBuf = 0;
   return &_s_strbufs[MX_ONE_BUF * iNextBuf];
}


#define  MXIO     512
static char def_log[] = "tempsudo.txt";
char logfile[256] = "\0";
FILE * outfile = NULL;
static int addsystime = 0;
static int addstdout = 0;
static int addflush = 1;
static int add2screen = 1;
static int add2listview = 1;

#ifndef VFP
#define VFP(a) ( a && ( a != (FILE *)-1 ) )
#endif

int   add_list_out( int val )
{
   int i = add2listview;
   add2listview = val;
   return i;
}

int   add_std_out( int val )
{
   int i = addstdout;
   addstdout = val;
   return i;
}

int   add_screen_out( int val )
{
   int i = add2screen;
   add2screen = val;
   return i;
}


int   add_sys_time( int val )
{
   int   i = addsystime;
   addsystime = val;
   return i;
}

int   open_log_file( void )
{
   if (logfile[0] == 0)
      strcpy(logfile,def_log);
   outfile = fopen(logfile, "wb");
   if( outfile == 0 ) {
      outfile = (FILE *)-1;
      sprtf("WARNING: Failed to open log file [%s] ...\n", logfile);
      return 0;   /* failed */
   }
   return 1; /* success */
}

void close_log_file( void )
{
   if( VFP(outfile) ) {
      fclose(outfile);
   }
   outfile = NULL;
}
char * get_log_file( void )
{
   if (logfile[0] == 0)
      strcpy(logfile,def_log);
   return logfile;
}

void   set_log_file( char * nf )
{
   if (logfile[0] == 0)
      strcpy(logfile,def_log);
   if ( nf && *nf && strcmpi(nf,logfile) ) {
      close_log_file(); // remove any previous
      strcpy(logfile,nf); // set new name
      open_log_file();  // and open it ... anything previous written is 'lost'
   }
}

void add_date_stg( char *ps, SYSTEMTIME *pst )
{
    sprintf(ps, "%4d/%02d/%02d",
        pst->wYear & 0xffff,
        pst->wMonth & 0xffff,
        pst->wDay & 0xffff );
}


void add_time_stg( char *ps, SYSTEMTIME *pst )
{
    sprintf(ps, "%2d:%02d:%02d",
        pst->wHour & 0xffff,
        pst->wMinute & 0xffff,
        pst->wSecond & 0xffff );
}


char *get_date_stg()
{
    SYSTEMTIME st;
    char *ps = GetNxtBuf();
    GetLocalTime(&st);
    add_date_stg( ps, &st );
    return ps;
}

char *get_time_stg()
{
    SYSTEMTIME st;
    char *ps = GetNxtBuf();
    GetLocalTime( &st );
    add_time_stg( ps, &st );
    return ps;
}

char *get_date_time_stg()
{
    SYSTEMTIME st;
    char *ps = GetNxtBuf();
    GetLocalTime( &st );
    add_date_stg( ps, &st );
    strcat(ps," ");
    add_time_stg( EndBuf(ps), &st );
    return ps;
}

static void oi( char * ps )
{
   //char * ps = psin;
    if (!ps)
        return;

   int len = (int)strlen(ps);
   if (len) {

      if( outfile == 0 ) {
         open_log_file();
      }
      if( VFP(outfile) ) {
          if( addsystime ) {
              char *tb = GetNxtBuf();
              len = sprintf( tb, "%s - %s", get_time_stg(), ps );
              ps = tb;
          }
         int w;
         w = (int)fwrite( ps, 1, len, outfile );
         if( w != len ) {
            fclose(outfile);
            outfile = (FILE *)-1;
            sprtf("WARNING: Failed write to log file [%s] ...\n", logfile);
         } else {
            fflush( outfile );
         }
      }

      if( addstdout ) {
         fwrite( ps, 1, len, stdout );
      }
#ifdef ADD_LISTVIEW
       if (add2listview) {
           LVInsertItem(ps);
       } 
#endif // ADD_LISTVIEW
       if (add2screen) {
          Add_String(ps);    // add string to screen list
       }
   }
}

static void	prt( char * ps )
{
    //static char buf[MXIO + 4];
    static char _s_buf[1024];
	char * pb = _s_buf;
	size_t i, j, k;
	char   c, d;
    i = strlen(ps);
	k = 0;
	d = 0;
	if(i) {
		k = 0;
		d = 0;
		for( j = 0; j < i; j++ ) {
			c = ps[j];
			if( c == 0x0d ) {
				if( (j+1) < i ) {
					if( ps[j+1] != 0x0a ) {
						pb[k++] = c;
						c = 0x0a;
					}
            } else {
					pb[k++] = c;
					c = 0x0a;
				}
			} else if( c == 0x0a ) {
				if( d != 0x0d ) {
					pb[k++] = 0x0d;
				}
			}
			pb[k++] = c;
			d = c;
			if( k >= MXIO ) {
				pb[k] = 0;
				oi(pb);
				k = 0;
			}
		}	// for length of string
		if( k ) {
			//if( ( gbCheckCrLf ) &&
			//	( d != 0x0a ) ) {
				// add Cr/Lf pair
				//pb[k++] = 0x0d;
				//pb[k++] = 0x0a;
				//pb[k] = 0;
			//}
			pb[k] = 0;
			oi( pb );
		}
	}
}

// STDAPI StringCchVPrintf( OUT LPTSTR  pszDest,
//   IN  size_t  cchDest, IN  LPCTSTR pszFormat, IN  va_list argList );
int _cdecl sprtf( char * pf, ... )
{
   static char _s_sprtfbuf[1024];
   char * pb = _s_sprtfbuf;
   int   i;
   va_list arglist;
   va_start(arglist, pf);
   i = vsprintf( pb, pf, arglist );
   va_end(arglist);
   prt(pb);
   return i;
}

#ifdef UNICODE
// WIDE VARIETY
static void wprt( PTSTR ps )
{
   static char _s_woibuf[1024];
   char * cp = _s_woibuf;
   int len = (int)lstrlen(ps);
   if(len) {
      int ret = WideCharToMultiByte( CP_ACP, // UINT CodePage,            // code page
         0, // DWORD dwFlags,            // performance and mapping flags
         ps,   // LPCWSTR lpWideCharStr,    // wide-character string
         len,     // int cchWideChar,          // number of chars in string.
         cp,      // LPSTR lpMultiByteStr,     // buffer for new string
         1024,    // int cbMultiByte,          // size of buffer
         NULL,    // LPCSTR lpDefaultChar,     // default for unmappable chars
         NULL );  // LPBOOL lpUsedDefaultChar  // set when default char used
      //oi(cp);
      prt(cp);
   }
}

int _cdecl wsprtf( PTSTR pf, ... )
{
   static WCHAR _s_sprtfwbuf[1024];
   PWSTR pb = _s_sprtfwbuf;
   int   i = 1;
   va_list arglist;
   va_start(arglist, pf);
   *pb = 0;
   StringCchVPrintf(pb,1024,pf,arglist);
   //i = vswprintf( pb, pf, arglist );
   va_end(arglist);
   wprt(pb);
   return i;
}

#endif // #ifdef UNICODE

// eof - sprtf.cxx
