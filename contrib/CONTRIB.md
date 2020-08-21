# Contributions

This is all about the [Sudoku](https://en.wikipedia.org/wiki/Sudoku) classic 9x9 number placement puzzle...

This folder contains sources contributed by others, each in their own folder

## Folders:

### Karl

This brute force sudoku solver, `sudoku-k`, was contributed by Karl Dahlke, after just a very brief introduction to the Sudoku classic 9x9 puzzle. It uses simple recursion, finding the first empty slot, selection a possible value, and progress to solve the puzzle, backing up if that fails. It should `solve` any given puzzle... 

But makes no check that the solution is unique, this will even `solve` given a completely blank board... but does carefully check that the input puzzle in valid.

Karl has his own `trec` solver [here](https://github.com/eklhad/trec) ...

Thanks Karl, for such a quick, simple **sudoku** `solver`, given only the briefest introduction...

; eof
