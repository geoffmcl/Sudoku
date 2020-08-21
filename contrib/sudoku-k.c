/* 
   Module: sudokuk.c

   1. Check a file contains a valid sudoku puzzle 
      as the first 81 characters of the file, and
   2. Solve the puzzle, by brute force recursion
   3. Show the results.

   Written by Karl Dahlke, 17 Aug, 2020.
   Minor output enhancements by Geoff R. McLane.

*/

#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h> // for strcpy, ...

typedef unsigned char uchar;

static int iter_cnt = 0;
static int max_lev = 0;

static const struct constraint {
uchar loc[9];
const char *name;
} constraints[27] = {
{{0,1,2,3,4,5,6,7,8}, "row A"},
{{9,10,11,12,13,14,15,16,17}, "row B"},
{{18,19,20,21,22,23,24,25,26}, "row C"},
{{27,28,29,30,31,32,33,34,35}, "row D"},
{{36,37,38,39,40,41,42,43,44}, "row E"},
{{45,46,47,48,49,50,51,52,53}, "row F"},
{{54,55,56,57,58,59,60,61,62}, "row G"},
{{63,64,65,66,67,68,69,70,71}, "row H"},
{{72,73,74,75,76,77,78,79,80}, "row I"},
{{0,9,18,27,36,45,54,63,72}, "column 1"},
{{1,10,19,28,37,46,55,64,73}, "column 2"},
{{2,11,20,29,38,47,56,65,74}, "column 3"},
{{3,12,21,30,39,48,57,66,75}, "column 4"},
{{4,13,22,31,40,49,58,67,76}, "column 5"},
{{5,14,23,32,41,50,59,68,77}, "column 6"},
{{6,15,24,33,42,51,60,69,78}, "column 7"},
{{7,16,25,34,43,52,61,70,79}, "column 8"},
{{8,17,26,35,44,53,62,71,80}, "column 9"},
{{0,1,2,9,10,11,18,19,20}, "upper left box"},
{{3,4,5,12,13,14,21,22,23}, "upper middle box"},
{{6,7,8,15,16,17,24,25,26}, "upper right box"},
{{27,28,29,36,37,38,45,46,47}, "middle left box"},
{{30,31,32,39,40,41,48,49,50}, "middle middle box"},
{{33,34,35,42,43,44,51,52,53}, "middle right box"},
{{54,55,56,63,64,65,72,73,74}, "lower left box"},
{{57,58,59,66,67,68,75,76,77}, "lower middle box"},
{{60,61,62,69,70,71,78,79,80}, "lower right box"},
};

// each square belongs to 3 constraints, row, column, box.
// It will run faster if we compile this information.
static uchar assigned[81][4];
static int assignConstraints(void)
{
    uchar mapped[81];
    int k, i;
    memset(mapped, 0, sizeof(mapped));
    for (k = 0; k < 27; ++k) {
        const struct constraint* s = constraints + k;
        for (i = 0; i < 9; ++i) {
            uchar p = s->loc[i];
            assigned[p][mapped[p]++] = (uchar)k;
        }
    }
    for (i = 0; i < 81; ++i) {
        if (mapped[i] != 3) {
            fprintf(stderr, "assignconstraints location %d has %d constraints\n", i, mapped[i]);
            return 2;
        }
    }
    return 0;
}

static char board[100]; // input, from file, and final results buffer
static char ascboard[100]; // copy of input

static int readBoard(char *file)
{
    uchar c;
    int i, k;
    FILE* fp = fopen(file, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open '%s'\n", file);
        return 1;
    }
    //if(!fgets(board, sizeof(board), stdin)) exit(0);
    i = (int)fread(board, 1, 81, fp);
    fclose(fp);
    if (i != 81) {
        fprintf(stderr, "Error: Failed to read 81 bytes from file '%s'\n", file);
        return 1;
    }
    board[81] = 0;
    strcpy(ascboard, board);    // keep copy of original
    // if(strlen(board) < 81) { fprintf(stderr, "please enter 81 digits\n"); exit(1); }
    for(i=0; i<81; ++i) {
        c = board[i];
        if(!isdigit(c)) { 
            fprintf(stderr, "file '%s', character '%c' unexpected; must only be digits\n", file, c); 
            return 1;
        }
        board[i] = c - '0';
    }

    // loop through constraints
    for(k=0; k<27; ++k) {
        const struct constraint *s = constraints + k;
        uchar used[10];
        memset(used, 0, 10);
        for(i=0; i<9; ++i) {
            c = board[s->loc[i]];
            if(!c) continue;
            if(used[c] != (uchar)0) { 
                fprintf(stderr, "file '%s', constraint violated, value %c, col %d, %s\n", file, (c + '0'), (i+1), s->name );
                return 1;
            }
            used[c] = (uchar)1;
        }
    }
    return 0;
}

// return the numbers that are still unused at a given location
static int available(unsigned int loc, short* bitmap)
{
    short bits = 0x3fe;
    int count = 9;
    const struct constraint* s;
    int k, i;
    uchar used[10];
    memset(used, 0, 10);
    for (k = 0; k < 3; ++k) {
        s = assigned[loc][k] + constraints;
        for (i = 0; i < 9; ++i) {
            uchar c = board[s->loc[i]];
            if (!c) continue;
            if (used[c]) continue; // excluded by another constraint
            used[c] = 1;
            bits &= ~(1 << c);
            --count;
        }
    }
    *bitmap = bits;
    return count;
}

static int place(int lev)
{
    int bestloc, bestcount = 10, count;
    short map, bestmap;
    int i;
    if (lev > max_lev)
        max_lev = lev;
    iter_cnt++;
    bestloc = 0;
    // find best location
    for (i = 0; i < 81; ++i) {
        if (board[i]) continue; // already placed
        count = available(i, &map);
        if (count < bestcount) bestcount = count, bestmap = map, bestloc = i;
    }
    if (bestcount == 10) {
        // this can only happen if the entire board is filled
        //for (i = 0; i < 81; ++i) printf("%c", board[i] + '0');
        //printf("\n");
        return 0;
    }
    // bestloc is the answer
    for (i = 1; i <= 9; ++i) {
        if (!((1 << i) & bestmap)) continue;
        board[bestloc] = i;
        if (!place(lev + 1)) {
            return 0;
        }
        board[bestloc] = 0; // undo the move
    }
    return 1;
}


int main(int argc, char **argv)
{
    int i, slots = 0;
    char* file;
    if (argc < 2) {
        puts("Enter a file name, which starts with the 81 digits of a puzzle");
        return 1;
    }

    file = argv[1];
    assignConstraints();

    if (readBoard(file)) {
        return 1;
    }

    if (place(0)) {
        // this should /NEVER/ happen!!!
        printf("NO SOLUTION: file '%s', iter cnt %d, max lev %d \n", file, iter_cnt, max_lev);
        return 2;
    }

    // show the SOLUTION
    // massage the output
    for (i = 0; i < 81; i++) {
        if (ascboard[i] != '0') {
            board[i] = '=';
            slots++;
        }
        else {
            board[i] += '0';
            ascboard[i] = '-';
        }
    }
    printf("SOLVED: In file '%s', %d slots, iter cnt %d, max lev %d \n", file, slots, iter_cnt, max_lev);
    printf("Inputs: %s\n", ascboard);
    printf("Solved: %s\n", board);

    return 0;
}

// eof - sudokuk.c
