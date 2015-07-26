// sprtf.hxx
#ifndef _SPRTF_HXX_
#define _SPRTF_HXX_


extern int add_std_out( int val );
extern int add_sys_time( int val );
extern int add_screen_out( int val );
extern int add_list_out( int val );

extern int open_log_file( void );
extern void close_log_file( void );
extern void   set_log_file( char * nf );
extern char * get_log_file( void );

extern int _cdecl sprtf( char * pf, ... );

extern char *GetNxtBuf();


#define EndBuf(a)   ( a + strlen(a) )

extern char *get_date_stg();
extern char *get_time_stg();
extern char *get_date_time_stg();

extern double g_Secs_in_SPRTF;  // formatting output strings, and IO (file,screen,...)

#define SPRTF   sprtf

#endif // #ifndef _SPRTF_HXX_
// oef - sprtf.hxx
