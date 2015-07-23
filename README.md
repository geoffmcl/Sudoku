# Sudoku Project

A WIN32 application to **solve** a Sudoku puzzle. NOt perfect, and **NOT** complete...

A little history. Up until circa 2012, my only involvement in Sudoku was to occasionally do the puzzle while sitting on the toilet ;=))

So decide why not try to write an eduational type program in C/C++, to solve Sudoku step by step. This was before I had found lots of sites, or done any other reading on internet...

It is a windows app, and now it can solve a large number of puzzles. Of course by that I mean using ONLY the same logic used by humans... Yes, it can solve every puzzle by brute force trial and error, but that is NOT what it is all about...

So I started looking around for puzzles it can NOT solve, and then try to work out a strategy to get through it...

Then there was a 2/3 year delay before I decided to revive the project by adding a CMakeLists.txt, and a `build` folder to do the out-of-source build.

If you have your build tools setup, then the simple build steps are :-

 - cd build
 - cmake ..
 - cmake --build . --config Release
 
Of course, alteratively the cmake GUI can be used, and then the MSVC IDE can also be used to build the project.

The `examples` folder contains a considerable list of `sample` files which can be loaded. Or you can manually setup the puzzle from say a magazine and try to solve it...

The main thing is **have FUN!**

Geoff.  
20150723

; eof
