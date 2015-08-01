# Sudoku Project

A WIN32/WIN64 a GUI application to display and **solve** a Sudoku puzzle. Certainly **not** perfect, and **NOT** complete... else as a simple console app...

#### History

A little history. Up until circa 2012, my **only** involvement in Sudoku was to occasionally do the puzzle while sitting on the toilet ;=))

So decide why not try to write an eduational type program in C/C++, to solve Sudoku step by step. This was before I had found lots of sites, or done any other reading on internet...

It started as a windows app, and can solve a large number of puzzles. Of course by that I mean using ONLY the same logic used by humans... Yes, it can solve every puzzle by brute force trial and error, but that is **NOT** what it is all about...

So I started looking around for puzzles it can NOT solve, and then try to work out a strategy to get through it...

Then there was a 2/3 year delay before I decided to revive the project by adding a CMakeLists.txt, and a `build` folder to do the out-of-source build, and putting it in this repo...

#### Building

If you have your build tools setup, then the simple build steps are :-

 - cd build
 - cmake .. [OPTIONS]
 - cmake --build . --config Release
 
Of course, alteratively the cmake GUI can be used, and then the MSVC IDE can also be used to build the project. There is also a build-me.bat which may be useful...

#### Input files

The `examples` folder contains a considerable list of `sample` files which can be loaded. Or you can manually setup the puzzle from say a magazine and try to solve it... using the menu `->` Options `->` Solve!, or One Step, or simply selecting a square and entering a number manually...

#### Unix

In unix can only build a console app... have not yet chosen what GUI library to use... This console app, Sudo-con, can also be built in WIN32 using the option -DBUILD_WIN32_EXE:BOOL=OFF.

Ii uses the **SAME** OpenSudoku library as the WIN32 GUI app. It will read a text file in several formats (txt,csv,sdk,xml) and proceed to auto-solve it, if it can!

The main thing is **have FUN!**

Geoff.  
20150801 - 20150723

; eof
