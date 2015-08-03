@setlocal
@set TMPEXE=Release\Sudo-con.exe
@if NOT EXIST %TMPEXE% goto NOEXE
@set TMP_DEF_FILE=C:\GTools\tools\Sudoku\examples\diabolical.txt
@REM set TMP_DEF_FILE    "/home/geoff/projects/Sudoku/examples/y-wing.txt"
@if NOT EXIST %TMP_DEF_FILE% goto NOFIL

@set TMPFIL=
@set GOTFIL=0
@set TMPCMD=

:RPT
@if "%~1x" == "x" goto GOTCMD
@set TMPCMD=%TMPCMD% %1
@set TMPFIL=%1
@shift
@if NOT EXIST %TMPFIL% goto RPT
@set GOTFIL=1
@goto RPT
:GOTCMD

@if "%GOTFIL%x" == "0x" (
@set TMPCMD=%TMPCMD% %TMP_DEF_FILE%
)

%TMPEXE% %TMPCMD%

@goto END

:NOEXE
@echo Can NOT locate %TMPEXE%! Has it been built? *** FIX ME ***
:END

