// Sudo_Strat.cxx

#include "Sudoku.hxx"

//   STRATEGIES
// Naked Pairs/Triples
// from : http://www.sudokuoftheday.com/pages/techniques-6.php
// This is one of the cleverer techniques - easier to use than to explain! 
// It works by spotting sets of pairs (or triples, or even quads) within an 
// area. The area could be a row, column, or a box, and the technique works 
// just the same.
 
// Basic - Eliminate in SAME Row, Col, Box
int Eliminate_Same_Col( PABOX2 pb, int col, PSET ps )
{
    int row2, val, cnt;
    cnt = 0;
    for (row2 = 0; row2 < 9; row2++) {
        val = pb->line[row2].val[col];    // get the value, if any
        if (val) {
            if ( ps->val[val -1] ) {
                ps->val[val - 1] = 0;   // remove value found in vert COLUMN search
                cnt++;
            }
        }
    }
    return cnt;
}

int Eliminate_Same_Row( PABOX2 pb, int row, PSET ps )
{
    int col2, val, cnt;
    cnt = 0;
    for (col2 = 0; col2 < 9; col2++) {
        val = pb->line[row].val[col2];    // get the value, if any
        if (val) {
            if ( ps->val[val -1] ) {
                ps->val[val - 1] = 0;   // remove value found in vert COLUMN search
                cnt++;
            }
        }
    }
    return cnt;
}

int Eliminate_Same_Box( int crow, int ccol, PABOX2 pb, PSET ps )
{
    int r = crow / 3;
    int c = ccol / 3;
    int rw, cl, val, cnt;
    r *= 3;
    c *= 3;
    cnt = 0;
    for (rw = 0; rw < 3; rw++) {
        for (cl = 0; cl < 3; cl++) {
            val = pb->line[r + rw].val[c + cl];    // get the value, if any
            if (val) {
                if (ps->val[val - 1] ) {
                    ps->val[val - 1] = 0;    // eliminate the value
                    cnt++;
                }
            }
        }
    }
    return cnt;
}

// ===========================================================





/*
  ==========
// Empty Box
//   1 2 3 4 5 6 7 8 9
// A  | | | | | | | |  1
// B  | | | | | | | |  2
// C  | | | | | | | |  3
// D  | | | | | | | |  4
// E  | | | | | | | |  5
// F  | | | | | | | |  6
// G  | | | | | | | |  7
// H  | | | | | | | |  8
// I  | | | | | | | |  9
// ===============================

// Filled with possibilities
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   ||123|123|123||123|123|123||123|123|123||  
// A ||456|456|456||456|456|456||456|456|456|| 1
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// B ||456|456|456||456|456|456||456|456|456|| 2
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// C ||456|456|456||456|456|456||456|456|456|| 3
//   ||789|789|789||789|789|789||789|789|789||  
//    =======================================
//   ||123|123|123||123|123|123||123|123|123||  
// D ||456|456|456||456|456|456||456|456|456|| 4
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// E ||456|456|456||456|456|456||456|456|456|| 5
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// F ||456|456|456||456|456|456||456|456|456|| 6
//   ||789|789|789||789|789|789||789|789|789||  
//    =======================================
//   ||123|123|123||123|123|123||123|123|123||  
// G ||456|456|456||456|456|456||456|456|456|| 7
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// H ||456|456|456||456|456|456||456|456|456|| 8
//   ||789|789|789||789|789|789||789|789|789||  
//    ---------------------------------------
//   ||123|123|123||123|123|123||123|123|123||  
// I ||456|456|456||456|456|456||456|456|456|| 9
//   ||789|789|789||789|789|789||789|789|789||  
//    =======================================
//      1   2   3    4   5   6    7   8   9

// Hidden Pairs Example
// File: hidden-pair.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   ||12 |123| 23||12 |123|   ||   | 23|  3||
// A || 5 |   |   ||   | 5 | 5 ||45 |456|456|| 1
//   || 8 | 8 |   ||  9|  9|  9|| 89|7 9|7 9||
//    ---------------------------------------
//   || * |123| * || * |123| * ||   | 23|  3||
// B ||*9*|   |*4*||*6*| 5 |*7*|| 5 | 5 | 5 || 2
//   || * | 8 | * || * |   | * || 8 |   |   ||
//    ---------------------------------------
//   || 2 | * | * || * | 23| * || * | 23|  3||
// C || 5 |*7*|*6*||*8*| 5 |*4*||*1*| 5 | 5 || 3
//   ||   | * | * || * |  9| * || * |  9|  9||
//    =======================================
//   || * | 2 | * || * | 2 | * ||   | * |   ||
// D ||*3*|4 6|*9*||*7*|456|*1*||45 |*8*|456|| 4
//   || * |   | * || * |   | * ||   | * |   ||
//    ---------------------------------------
//   || * | 2 | * || 2 | 2 |   || * |   | * ||
// E ||*7*|4 6|*8*||   |456| 56||*3*|456|*1*|| 5
//   || * |   | * ||  9|  9|  9|| * |  9| * ||
//    ---------------------------------------
//   ||   | * | * || * |   | * || * |   | * ||
// F ||4 6|*5*|*1*||*3*|4 6|*8*||*7*|4 6|*2*|| 6
//   ||   | * | * || * |  9| * || * |  9| * ||
//    =======================================
//   ||   |  3| * || * |   | * || * | * |  3||
// G ||4  |4  |*7*||*5*|   |*2*||*6*|*1*|4  || 7
//   || 8 | 89| * || * | 89| * || * | * |  9||
//    ---------------------------------------
//   ||1  |1  | * || * |1  | * || * |   | * ||
// H ||  6|  6|*5*||*4*|  6|*3*||*2*|   |*8*|| 8
//   ||   |  9| * || * |7 9| * || * |7 9| * ||
//    ---------------------------------------
//   ||12 |123| 23||1  |1  |   ||   |  3|  3||
// I ||4 6|4 6|   ||   |  6|  6||45 |45 |45 || 9
//   || 8 | 89|   ||  9|789|  9||  9|7 9|7 9||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// Note A8 and A9 contain possibilities of 6 & 7 with others...
// But with 6 in B4 and 7 in B6 rules out the B row
// and with 6 in C3 and 7 in C2 rules out the C row
// and with 6 in G7 and 7 in F7 rules out the 7 column
// So 67 are the ONLY possibilities for A8 and A9, thus can eliminate other values
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   ||12 |123| 23||12 |123|   ||   |   |   ||
// A || 5 |   |   ||   | 5 | 5 ||45 |  6|  6|| 1
//   || 8 | 8 |   ||  9|  9|  9|| 89|7  |7  ||
// Does not solve anything, but reduces the possibilities for 2 cells

  *********************************************************************** */
// Hidden Triples
// examples\hidden-triple.txt
// This can already be SOLVED using other techniques, but I 
// want to be able to mark hidden triples if they exist.

static char *pFile = __FILE__;
char *get_strat_name() { return pFile; }

// from : http://www.dadler.net/sudoku/Sudoku%20Manual.pdf
// Locked Candidates
// Locked Candidates occur when a digit is restricted to a particular 
// row or column within a box. They come in two flavors. The first is 
// when a row or column intersection with a box is found to be the only 
// place in the box where a specific digit is found. When that happens, 
// the digit can be eliminated from consideration wherever that row or 
// column extends outside of that box. Let’s return to Evil #6,328,269,907, 
// right after the hidden 8, 9 is “exposed” in box 1 to see an example of 
// locked 6’s in that same box (note that without the effect of the hidden 
// 8, 9 Pattern, there would not be any locked 6’s in box 1):
// see : examples\locked-04.txt

// read also : http://www.gamesudoku.blogspot.fr/2007/02/advanced-death-blossom.html
// but seems lightly different

// from : http://www.gamesudoku.blogspot.fr/2007/02/advanced-death-blossom.html
// Aligned Pair Exclusion is often referred to as APE.

// The final solution for any pair of cells cannot be the two numbers contained 
// in a bi-value cell that is a buddy cell to both cells. It would not leave a valid 
// candidate for the bi-value cell.
 
// If a pair of cells are in the same unit they cannot contain the same number.
 
// The Aligned Pair Exclusion strategy uses both these rules to exclude as many 
// of the possible candidate combinations from two cells aligned in a unit as it can.
 
// All possible candidate combinations are listed for an aligned pair of cells. 
// All possible candidate combinations that exist in bi-value cells that are 
// buddy cells to both cells in the aligned pair are removed from the list. 
// All possible candidate combinations that are the same number in both cells 
// are removed from the list. The solution for the pair of aligned cells has 
// to come from the possible candidate combinations that are left over.
 
// When the possible candidate combinations left over do not contain one of 
// the original candidates in the aligned pair that candidate can be removed.
 
// Aligned Pair Exclusion Type 2 is an extension of this strategy that also 
// uses the candidate pairs from an almost locked set to exclude possible 
// candidate combinations from the aligned pair of cells.


// from : http://www.sudokuessentials.com/sudoku-strategy.html
// File: locked-01.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || * |  3|1 3||1 3|12 |123|| * | * | * ||
// A ||*9*| 5 | 5 ||   |   |   ||*4*|*8*|*6*|| 1
//   || * |7  |7  ||7  |7  |7  || * | * | * ||
//    ---------------------------------------
//   ||  3| * |  3|| * | * |  3|| * | * | * ||
// B ||4  |*6*|4  ||*8*|*9*|4  ||*1*|*5*|*2*|| 2
//   ||7  | * |7  || * | * |7  || * | * | * ||
//    ---------------------------------------
//   || * |  3|1  || * |1  | * ||  3|   |  3||
// C ||*2*|   |4  ||*5*|4  |*6*||   |   |   || 3
//   || * |78 |78 || * |7  | * ||7 9|7 9|7 9||
//    =======================================

// In the above example, ONLY 1 ROW remains in Box (Region) 3
// And the candidates for the 3 cells in that ROW are
// 379 79 and 379. Thus a 3 MUST be used here
// Thus 3 can safely be removed from that ROW outside Box 3
// Just the 3, like
//    ---------------------------------------
//   || * |   |1  || * |1  | * ||  3|   |  3||
// C ||*2*|   |4  ||*5*|4  |*6*||   |   |   || 3
//   || * |78 |78 || * |7  | * ||7 9|7 9|7 9||
//    =======================================
// The beginning of the full puzzle is in 
// hard-02.txt

// Another example
// File: hard-07-1.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || * | * | * ||   |   | * ||   | * | * ||
// A ||*2*|*8*|*7*||4 6|4  |*5*||4  |*1*|*3*|| 1
//   || * | * | * ||  9|  9| * ||   | * | * ||
//    ---------------------------------------
//   || * | * |  3|| * | * |  3||   | * |   ||
// B ||*6*|*4*| 5 ||*1*|*2*|   || 5 |*9*| 5 || 2
//   || * | * |   || * | * |7  ||78 | * |78 ||
//    ---------------------------------------
//   || * |  3|  3|| * |  3|  3|| * | 2 |   ||
// C ||*1*| 5 | 5 ||*8*|4  |4  ||*6*|45 |45 || 3
//   || * |  9|  9|| * |7 9|7  || * |7  |7  ||
//    =======================================
//   ||  3| * | * ||  3| * | * ||   |   | * ||
// D || 5 |*1*|*8*||4  |*6*|*9*||45 |45 |*2*|| 4
//   ||7  | * | * ||   | * | * ||7  |7  | * ||
//    ---------------------------------------
//   ||   | * |   || * |   | * ||   | * |   ||
// E || 5 |*6*| 5 ||*2*|4  |*1*||45 |*3*|45 || 5
//   ||7 9| * |  9|| * |7  | * ||789| * |789||
//    ---------------------------------------
//   || * |  3| * || * | * |  3|| * |   |   ||
// F ||*4*|   |*2*||*5*|*8*|   ||*1*|  6|  6|| 6
//   || * |7 9| * || * | * |7  || * |7  |7 9||
//    =======================================
//   ||  3|  3| * ||  3|  3| * || 2 | 2 | * ||
// G || 5 | 5 |*6*||4  |45 |*8*||45 |45 |*1*|| 7
//   ||7 9|7 9| * ||  9|  9| * ||7 9|7  | * ||
//    ---------------------------------------
//   ||  3| * | * ||  3| * |  3||   | * |   ||
// H || 5 |*2*|*4*||  6|*1*|  6|| 5 |*8*| 56|| 8
//   ||7 9| * | * ||  9| * |   ||7 9| * |7 9||
//    ---------------------------------------
//   || * |   | * || * |   | * || * |   |   ||
// I ||*8*| 5 |*1*||*7*|45 |*2*||*3*|456|456|| 9
//   || * |  9| * || * |  9| * || * |   |  9||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// Viewing COLUMNS 1 2 3, particularly 3
// The choices for B3 = 35, and C3 = 359,
// and these are the ONLY places for a 3 
// left in this column.
// So within Box  (Region) 1, the 3 can be
// eliminated from C2 - 359 -> 59, like
//   || * |   |  3|| * |  3|  3|| * | 2 |   ||
// C ||*1*| 5 | 5 ||*8*|4  |4  ||*6*|45 |45 || 3
//   || * |  9|  9|| * |7 9|7  || * |7  |7  ||

// =================================================================

// from : http://su-doku.net/tech.php
// List of Stategies
// 1.1 - Naked Single (Sole Candidate)
// After checking same row, same column, and same box, only ONE candidate remains
// Example
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   ||   |   |   ||   |   |   ||   |   |   ||
// A ||   |   |   ||   |   |   ||   |   |   || 1
//   ||   |   |   ||   |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   |   |   ||   |   |   ||
// B ||   |   |   ||   |   |   ||   |   |   || 2
//   ||   |   |   ||   |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   | * |   ||   |   |   ||
// C ||   |   |   ||   |*4*|   ||   |   |   || 3
//   ||   |   |   ||   | * |   ||   |   |   ||
//    =======================================
//   ||   |   |   || * |   |   ||   |   |   ||
// D ||   |   |   ||*6*|   |   ||   |   |   || 4
//   ||   |   |   || * |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   |   | * ||   |   |   ||
// E ||   |   |   ||   |   |*9*||   |   |   || 5
//   ||   |   |   ||   |   | * ||   |   |   ||
//    ---------------------------------------
//   || * |   | * ||   |   |   ||   |   | * ||
// F ||*2*|   |*8*||   |   |   ||   |   |*3*|| 6
//   || * |   | * ||   |7  |   ||   |   | * ||
//    =======================================
//   ||   |   |   ||   | * |   ||   |   |   ||
// G ||   |   |   ||   |*1*|   ||   |   |   || 7
//   ||   |   |   ||   | * |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   | * |   ||   |   |   ||
// H ||   |   |   ||   |*5*|   ||   |   |   || 8
//   ||   |   |   ||   | * |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   |   |   ||   |   |   ||
// I ||   |   |   ||   |   |   ||   |   |   || 9
//   ||   |   |   ||   |   |   ||   |   |   ||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// It can be seen the SOLE candidate for cell F5 is 7

// 1.2 - Hidden Single (Unique Candidate)
// Due to row column elimination, ONLY a single UNIQUE value
// can be added.
// Example
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || * |   |   ||   |   |   ||   |   |   ||
// A ||*4*| ==============================> || 1
//   || * |   |   ||   |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   || * |   | * ||   |   |   ||
// B ||   |   |   ||*3*|4  |*2*||   |   |   || 2
//   ||   |   |   || * |   | * ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   |   |   ||   |   | * ||
// C || <============================== |*4*|| 3
//   ||   |   |   ||   |   |   ||   |   | * ||
//    =======================================
// In Box (Region) 2 the UNIQUE candidate for B5 is 4

// 1.3 - Block and Column/Row Interactions
// Does not yield an answer but reduces the candidates
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || * | 23| * ||   |   |   ||   |   |   ||
// A ||*1*|   |*5*||   |   |   ||   |   |   || 1
//   || * |789| * ||   |   |   ||   |   |   ||
//    ---------------------------------------
//   || 23| 23| 23||   |   | * ||   |   |   ||
// B ||   |   |   ||   |   |*9*||   |   |   || 2
//   ||78 |78 |78 ||   |   | * ||   |   |   ||
//    ---------------------------------------
//   || * | 23| * ||   |   |   ||   |   |   ||
// C ||*6*|   |*4*||   |   |   ||   |   |   || 3
//   || * |789| * ||   |   |   ||   |   |   ||
//    =======================================
// From this we KNOW the 9 MUST go in A2 or C2
// but does not yiled the asswer yet.

// 1.4 - Block/Block Interactions
// Again does not give an answer, but reduces the candidates
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   ||   | * |   ||   |   |   ||   |   |   ||
// A ||   |*7*|   ||   |   |   ||   |   |   || 1
//   ||   | * |   ||   |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   || * |   | * ||   |   |   ||
// B ||   |   |   ||*9*| A |*3*||   |   |   || 2
//   ||   |   |   || * |   | * ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   || * |   |   ||   |   |   ||
// C ||   |   |   ||*1*| A | A ||   |   |   || 3
//   ||   |   |   || * |   |   ||   |   |   ||
//    =======================================
//   ||   |   |   || * |   |   ||   |   |   ||
// D ||   |   |   ||*8*| B | B ||   |   |   || 4
//   ||   |   |   || * |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   || * |   |   ||   |   |   ||
// E ||   |   |   ||*2*| B | B ||   |   |   || 5
//   ||   |   |   || * |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   |   |   ||   | * |   ||
// F ||   |   |   ||   |   |   ||   |*7*|   || 6
//   ||   |   |   ||   |   |   ||   | * |   ||
//    =======================================
//   ||   |   |   ||   |   |   ||   |   |   ||
// G ||   |   |   ||   | E7| E7||   |   |   || 7
//   ||   |   |   ||   |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   |   |   ||   |   |   ||
// H ||   |   |   ||   | E7| E7||   |   |   || 8
//   ||   |   |   ||   |   |   ||   |   |   ||
//    ---------------------------------------
//   ||   |   |   ||   |   |   ||   |   |   ||
// I ||   |   |   ||   | E7| E7||   |   |   || 9
//   ||   |   |   ||   |   |   ||   |   |   ||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// A2 7 block row 1, and F8 7 blocks row 6 
// Thus 7 MUST be in A and B locations,
// so can be ELIMINATED from G5-6 H5-6 & I5-6

// 1.5 - Naked Subset (Naked Pair, Disjoint Subset) 
// Depending on the number of candidates used this technique 
// can also be called ‘Naked Triplet’ or ‘Naked Quad’. 
// This is another technique used for removing candidates 
// from a row, column or block, if two cells on a row 
// have the same candidates then it is logical that any 
// other cell on that row cannot possibly house those 
// values, and therefore can remove the values from 
// their candidate list.

// File: naked-pair.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   ||123|123|123||   | * |   ||  3|  3| * ||
// A ||  6|  6|  6||  6|*4*|  6||  6|  6|*5*|| 1
//   ||78 |78 |78 ||789| * |789||789|789| * ||
//    ---------------------------------------
//   || * | * | * ||   |   | * || * |  3|  3||
// B ||*5*|*4*|*9*||  6|  6|*1*||*2*|  6|  6|| 2
//   || * | * | * ||78 |78 | * || * |78 |78 ||
//    ---------------------------------------
//   ||   |   |   || * | * |   || * |   | * ||
// C ||  6|  6|  6||*3*|*2*| 5 ||*4*|  6|*1*|| 3
//   ||78 |78 |78 || * | * |   || * |789| * ||
//    =======================================

// BUT there is a PROBLEM with the example given. It suggests 
// since C2 and C6 contains 6 and 7 candidates, then C8 must 
// be a 9. Further analysis show C6 must in fact be UNIQUELY 
// a 5. The 5 in A9, and B1 precludes a 5 in box 2 in those
// rows, and since only C6 remains, then it must be a 5.

// 1.6 - Hidden Subset (Hidden pair, Unique Subset)
// Once again depending on the amount of candidates used this 
// technique can also be called ‘Hidden Triplet’ or ‘Hidden Quad’. 
// Similar to ‘Naked Subset’ this technique whittles down candidates 
// from the empty cells we are interested in, not from the rest 
// of the row, column or block. If three separate cells on a row 
// house many candidates, but there are three common numbers that 
// occupy at least one of the cells each then we can remove any 
// other candidates from these three cells, leaving the three 
// values as the only candidates.

// BUT do not understand the conclusion from example
//    =======================================
//   || * | * |12 ||   |1 3|123||123|12 | 2 ||
// A ||*8*|*5*|4  ||4 6|4 6|4 6||   |   |   || 1
//   || * | * |7  ||  9|7 9|7 9||7 9|7 9|  9||
//    ---------------------------------------
//   || * |123|12 ||   |1 3|123|| * |12 | * ||
// B ||*6*|   |   ||   |   |   ||*4*|   |*5*|| 2
//   || * |7  |7  || 89|789|789|| * |7 9| * ||
//    ---------------------------------------
//   || 2 | * |12 ||   |1 3| * || * |12 | 2 ||
// C ||4  |*9*|4  ||4  |4  |*5*||*6*|   |   || 3
//   ||   | * |7  || 8 |78 | * || * |7  | 8 ||
//    =======================================
// We have three cells here that house the numbers 1, 3 or 7, 
// we can simply remove all the other candidates from those cells 
// and we are left with our shadowed cell being a 1. ????????

// from : http://www.sudokuwiki.org/sudoku.htm
// 
// Another Naked Pair example
// Solving 
// 050000803100096500004085060900403006020060050000801007060030405000570631503600020
// File: examples\naked-pair2.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || 2 | * | 2 ||12 |1  |E2 || * |1  | * ||
// A ||  6|*5*|  6||   |4  |4  ||*8*|   |*3*|| 1
//   ||7  | * |7 9||7  |   |7  || * |7 9| * ||
//    ---------------------------------------
//   || * |  3| 2 || 23| * | * || * |   | 2 ||
// B ||*1*|   |   ||   |*9*|*6*||*5*|4  |4  || 2
//   || * |78 |78 ||7  | * | * || * |7  |   ||
//    ---------------------------------------
//   || 23|  3| * ||123| * | * ||1  | * | 2 ||
// C ||   |   |*4*||   |*8*|*5*||   |*6*|   || 3
//   ||7  |7 9| * ||7  | * | * ||7 9| * |  9||
//    =======================================
//   || * |1  |1  || * |N2 | * ||12 |1  | * ||
// D ||*9*|   | 5 ||*4*|P5 |*3*||   |   |*6*|| 4
//   || * |78 |78 || * |   | * ||   | 8 | * ||
//    ---------------------------------------
//   ||  3| * |1  ||   | * |   ||1 3| * |   ||
// E ||4  |*2*|E  ||N P|*6*|N P||   |*5*|4 E|| 5
//   || 8 | * |78 ||7 9| * |7 9|| E9| * | 89||
//    ---------------------------------------
//   ||  3|  3|   || * |N2 | * || 23|   | * ||
// F ||4 6|4  | 56||*8*|P5 |*1*||   |4  |*7*|| 6
//   ||   |   |   || * |   | * ||  9|  9| * ||
//    =======================================
//   || 2 | * |12 ||12 | * | 2 || * |   | * ||
// G ||   |*6*|   ||   |*3*|   ||*4*|   |*5*|| 7
//   ||78 | * |789||  9| * | 89|| * |789| * ||
//    ---------------------------------------
//   || 2 |   | 2 || * | * | 2 || * | * | * ||
// H ||4  |4  |   ||*5*|*7*|4  ||*6*|*3*|*1*|| 8
//   || 8 | 89| 89|| * | * | 89|| * | * | * ||
//    ---------------------------------------
//   || * |1  | * || * |1  |   ||   | * |   ||
// I ||*5*|4  |*3*||*6*|4  |4  ||   |*2*|   || 9
//   || * |789| * || * |   | 89||7 9| * | 89||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// eof - generated 2012/08/28 12:55:42
// E4 and E6 share a pair 79, which are NOT in the other ROW of this Box 5
// Ergo they can ONLY occupy THIS Row E
// Other 79s in Row E, in Box 4 or 6 can be eliminated
// Eliminate E1 & E3 2x7, E7 & E9 2x9 = 4 elimination
//
// D5 & F5 share a pair 25, whihc are NOT in the 2 other COLS of this Box 5
// Ergo they can ONLY occupy THIS COL 5
// Other 25s in Col 5, in Boxes 2 & 8 can be eliminated
// Eliminates A5 2 = 1 elimination



// 2.1 - X-Wing
// This solving method considers four interlinked cells. The 
// technique takes two rows and finds four empty cells perpendicular 
// to form a rectangle. The cells must be related to each other 
// using a common value that is held as a candidate for each of 
// the empty cells, there must only ever be two empty cells on 
// each of the two rows that house that certain common value. 
// Either top-left and bottom-right or top-right and bottom-left of 
// this pattern will hold the true candidates, any other empty cells 
// in the columns connecting the X-Wing can have the common value 
// withdrawn from their candidate list.

// 2.2 - Swordfish
// Very similar to the X-Wing method, the swordfish uses three rows 
// that have up to nine empty cells that house the same candidate number, 
// two empty cells from each row are selected and they must connect with 
// another selected empty cell from a different row through its column. 
// This time we get two rectangle patterns in our puzzle. We can then 
// eliminate the candidate number from any other empty cells along the 
// selected rows or columns excluding our ‘swordfish’ cells. The final 
// true positions for the candidate number will be in a diagonal formation 
// using our ‘swordfish’ cells.

// 2.3 - XY-Wing
// An XY-Wing makes good use of combining empty cells that follow a certain 
// pattern. When you have three empty cells along a row and a column you 
// get an L shape .The two outside cells hold a candidate number that is 
// the same as one of the centre cell candidate numbers. The two cells 
// also hold one other matching candidate number each. When you follow 
// the two outside cells along either their column or row you will find 
// another cell that will force the pattern into a square. If this empty 
// cell holds the matching candidate number then it can be removed from 
// that cells candidate list. The XY-Wing technique also has other 
// patterns that work in the same kind of way.

// 2.4 - Colouring
// For the colouring technique we are only interested in two cells that 
// share a row, column or block and hold an identical candidate number. 
// This means that one of the cells must hold the true candidate number, 
// for this to work we must colour each of the two cells different colours, 
// one of the coloured cells could also have the same relationship with another 
// cell along another row, column or block, we can then colour the new cell 
// with the opposite colour of the previous cell, so we get a kind of chain 
// forming of opposite colours. When we have two opposite coloured cells in 
// relation with a common cell (forming an L shape) we know that one of those 
// cells must be the true candidate number, This then allows us to remove the 
// candidate number from the new cell.

// File: Coloring2.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || * |  3| * ||12 |1  | 2 || * |  3|   ||
// A ||*5*|   |*9*||   |4 6|4 6||*7*|   |4 6|| 1
//   || * | 8 | * ||   |   |   || * | 8 |   ||
//    ---------------------------------------
//   ||  3| * | * ||   |   |   || * |  3|   ||
// B ||   |*2*|*7*|| 5 |456|456||*1*|   |4 6|| 2
//   || 8 | * | * ||  9|  9|   || * | 8 |   ||
//    ---------------------------------------
//   || * | * | * || * | * | * || * | * | * ||
// C ||*6*|*4*|*1*||*3*|*8*|*7*||*9*|*2*|*5*|| 3
//   || * | * | * || * | * | * || * | * | * ||
//    =======================================
//   ||1 3| * | * ||12 |1 3| 23|| * | * |1 3||
// D ||4  |*7*|*8*|| 5 |45 |45 ||*6*|*9*|   || 4
//   ||   | * | * ||   |   |   || * | * |   ||
//    ---------------------------------------
//   ||1 3|1 3| * || * |1 3| * || * |1  | * ||
// E ||4  |E1 |*5*||*6*|4  |*9*||*2*|(A)|*8*|| 5
//   ||   |   | * || * |7  | * || * |7  | * ||
//    ---------------------------------------
//   || * | * | * ||1  |1 3|  3|| * | * |1 3||
// F ||*9*|*6*|*2*||   |   |   ||*5*|*4*|   || 6
//   || * | * | * ||78 |7  | 8 || * | * |7  ||
//    =======================================
//   ||   |   | * || * | * | * || * |   | * ||
// G ||   | 5 |*6*||*4*|*2*|*1*||*3*| 5 |*9*|| 7
//   ||78 | 8 | * || * | * | * || * |7  | * ||
//    ---------------------------------------
//   || * |1  | * ||   |   |   || * | * |1  ||
// H ||*2*|(D)|*3*|| 5 | 5 | 5 ||*4*|*6*|(C)|| 8
//   || * |  9| * ||789|7 9| 8 || * | * |7  ||
//    ---------------------------------------
//   ||1  |1  | * ||   |  3|  3|| * |1  | * ||
// I ||   | 5 |*4*|| 5 | 56| 56||*8*|(B)|*2*|| 9
//   ||7  |  9| * ||7 9|7 9|   || * |7  | * ||
//    =======================================
//      1   2   3    4   5   6    7   8   9
// Cell E8 (A) has 1 as a candidate - Color 1
// Cell I8 (B) has 1 as a candidate - Color 2
// Cell H9 (C) has 1 as a candidate - Color 1
// Cell H2 (D) has 1 as a condidate - Color 2
// Now either (A) or (D) MUST be the real 1
// forcing the row/col intersection of (A) & (D)
// E2 to eliminate 1 from its candidates.

// More Coloring from : http://www.sudoku9981.com/sudoku-solving/simple-colors.php
// When a row, column or box contains only 2 candidates 
// for a digit, one of them must be true and the other 
// must be false
// examples\coloring5.txt shows a conflict

// More coloring from : http://sudoku.com.au/sudokutips.aspx?Go=D17-2-1994
// This technique is called by a few names. Amongst them are simple coloring, 
// multi-coloring, colors. I prefer just coloring. I also prefer to extend 
// this idea to include any technique that is performed on just one type of 
// candidate - or just one number, if you prefer.
//
// Xwings, Swordfish, Jellyfish, Finned Xwings, Finned Swordfish, Locked 
// Candidates, and many other techniques involving just one type of candidate 
// can be placed under the tag, coloring.



// 2.5 - Forcing Chains
// To start a chain you must have a candidate link between 
// empty cells, a link in a chain can be part of a row, column 
// or block. One cell must only have two candidates and one 
// candidate must always be the same as the next link in the 
// chain. We take the first link in our chain and systematically 
// go through pretending that one of our candidate numbers is the 
// true one, this will move through each link in the chain whittling 
// down the candidates to one possible true answer (like trial and 
// error). When we reach a dead end, we start over and we do exactly 
// the same thing again with the second candidate number following 
// the path until it reaches another dead end (should be on the same 
// cell as the last guess). Now if the two values that were found in 
// our final cell were matching, we know that our cell has to be that 
// value.

// 2.6 - Nishio (Limited Trial and Error)
// This is a technique that some Sudoku solvers find conterversial. 
// For each candidate you ask the question: Will placing this number 
// here stop me from completing the other placements of this number? 
// If yes then the candidate can be eliminated.

// X-Cycles
// from : http://zitowolf.net/sudoku/strategy.html#xcycles
// examples\xcycles1.txt an example with the odd length link, 5
// 6@A6=6@B4-6@B2=6@D2-6@D6=6@A6
// where "-" and "=" stand for weak and strong links respectively
// In this case the distinguished choice point is forced instead of 
// eliminated. Let's see how this works. If 6@A6 were eliminated, that 
// would force 6@B4, which forces 6@D2, which forces 6@A6 - a contradiction! 
// If 6@A6 is selected, the two adjacent choices are eliminated but since 
// the subsequent links are weak, further inferences stop and contradiction 
// is avoided. The X-Cycle forces A6 to be 6 (and in this case, solves the puzzle).

// BUT this SAME result also comes from 'Simple Coloring'
// The alternating CHAIN 6@B2-A -> 6@D6-B -> 6@F1-A -> 6@F4-A
// where -A and -B mean opposite colors - arbitrarily ON or OFF
// Now 6@B4 is exposed to 6@B2-A - same row, and 6@F4-B - same column,
// so whether A or B is chosen, 6@B4 can be eliminated.
// Thus like X-Cycles forces A6 to be 6, thus eliminates 6@B4,

// BOTH strategies FORCE A6 to be 6, as the ONLY remaining candidate
// in BOX 2. (See bmp\cycles1.bmp

// Unique Rectangles
// examples\uniqrect1.txt
// Another way of eliminating candidates is based on the presumed 
// uniqueness of the solution. In the following puzzle, consider 
// cells D1, D5, E1, and E5. Suppose we assigned 3 to D1, which would 
// force 6@D5, 3@E5, and 6@E1, and we were able to find a solution from 
// there. In that solution we could swap 3 and 6's among those 4 cells 
// and all the containing groups - Columns 1 and 5, Rows D and E, and 
// Boxes 4 and 5 - would still contain exactly one 3 and one 6. That 
// means the puzzle would have 2 solutions (at least). Assuming the 
// puzzle is well-formed with a unique solution, 3@D1 cannot be part 
// of the solution. We can eliminate 6@D1 likewise, leaving only 8 
// as a possibility for D1. This strategy typically involves a rectangle 
// of 4 cells and so is called the Unique Rectangle.


// examples\uniqrect4.txt
// Type 4 Unique Rectangles
// A type 4 unique rectangle is a situation where 2 cells of a rectangle 
// contain only a pair of possibilities a and b, the other cells C and D 
// contain a and b plus any number of additional values, and there is a 
// strong link between a@C and a@D. In that case, selecting b@C would 
// force a@D, creating a non-unique rectangle; ditto b@D. So b can be 
// eliminated from C and D. The next example is a type-4 unique rectangle 
// at D5, H5, D6 and H6: Value 2 must go in one of D6 and H6. So we must 
// eliminate 5 from D6 and H6.

// from : http://www.brainbashers.com/sudokuuniquerectangles.asp
// Unique Rectangles
// Unique Rectangles is an advanced technique. Its name comes from a collection 
// of two locked pairs that form a rectangle. Every Sudoku has to have a unique 
// solution, we are not allowed to create a situation where this rule is violated. 
// We need to look for four squares that share exactly two rows, two columns and 
// two blocks, if we had a Sudoku with a non-unique solution, then we could swap 
// the numbers and still have a valid Sudoku with no duplicates. The examples below 
// are a subset of the logic which can be deduced from the requirement of uniqueness, 
// if you come across a situation which looks similar, then carefully think about the 
// implications of the Sudoku being left in an invalid state.
// NOTE: the squares have to share exactly two rows, two columns and two blocks.




// XY Chain
// Used in examples\diabolical4.txt
// XY-Chains - from : http://www.sudokuwiki.org/XY_Chains
// The Y-Wing Chains are infact part of a more encompassing strategy called XY-Chains. 
// The commonality is the same pincer-like attack on candidates that both ends can see 
// and that the chain is made of bi-value cells. With Y-Chains the hinge was expanded 
// to a chain of identical bi-value cells but in an XY-Chain these can be different - 
// as long as there is one candidate to make all the links. The "X" and the "Y" in the 
// name represent these two values in each chain link.
// See examples xychain1.txt and xychain2.txt

// File: xychain1.txt
//      1   2   3    4   5   6    7   8   9
//    =======================================
//   || 2 | * | 2 || * | 2 | * ||   | * |   ||
// A ||  6|*8*|45 ||*1*|   |*3*|| 5 |*7*|456|| 1
//   ||   | * |   || * |  9| * ||  9| * |   ||
//    ---------------------------------------
//   ||  3| * | 2 || * | 2 | * ||1  |1  |  3||
// B ||   |*9*|4  ||*5*|   |*6*||   |4  |4  || 2
//   ||7  | * |   || * |7  | * || 8 |   | 8 ||
//    ---------------------------------------
//   ||  3|   | * || * |   | * ||  3| * |  3||
// C ||   | 56|*1*||*4*|   |*8*|| 5 |*2*| 56|| 3
//   ||7  |   | * || * |7 9| * ||  9| * |   ||
//    =======================================
//   || * | * | * || * | * | * || * | * | * ||
// D ||*5*|*7*|*8*||*2*|*4*|*1*||*6*|*3*|*9*|| 4
//   || * | * | * || * | * | * || * | * | * ||
//    ---------------------------------------
//   || * | * | * || * | * | * || * | * | * ||
// E ||*1*|*4*|*3*||*6*|*5*|*9*||*7*|*8*|*2*|| 5
//   || * | * | * || * | * | * || * | * | * ||
//    ---------------------------------------
//   || * | * | * || * | * | * || * | * | * ||
// F ||*9*|*2*|*6*||*8*|*3*|*7*||*4*|*5*|*1*|| 6
//   || * | * | * || * | * | * || * | * | * ||
//    =======================================
//   ||   | * | * || * |1  | * || * |1  |   ||
// G ||  6|*3*|*7*||*9*|  6|*5*||*2*|4  |4  || 7
//   || 8 | * | * || * |   | * || * |   | 8 ||
//    ---------------------------------------
//   || 2 |   | 2 || * |1  | * ||1  | * | * ||
// H ||  6| 56| 5 ||*3*|  6|*4*||   |*9*|*7*|| 8
//   || 8 |   |   || * |   | * || 8 | * | * ||
//    ---------------------------------------
//   || * | * | * || * | * | * ||  3| * |  3||
// I ||*4*|*1*|*9*||*7*|*8*|*2*|| 5 |*6*| 5 || 9
//   || * | * | * || * | * | * ||   | * |   ||
//    =======================================
//      1   2   3    4   5   6    7   8   9

// eof - generated 2012/09/21 11:55:03
// from : http://www.sudokuwiki.org/XY_Chains
// The example here is a very simple XY-Chain of length 4 
// which removed all 5's highlighted in yellow.
// ie Elim 5@A3, 5@c7 and 5@C9
// The chain ends are 5 A7 and C2 - so all cells that can 
// see both of these are under fire.
// ie Chain 5@A7-9@A7--9@A5-2@A5--2@A1-6@A1--6@C2-5@C2

// It's possible to start at either end but lets follow 
// the example from A7. We can reason as follows
// If A7 is 5 then A3/C7/C9 cannot be.
// if A7 is NOT 5 then it's 9, so A5 must be 2, which forces 
// A1 to be 6. If A1 is 6 then C2 is 5.
// Which ever choice in A7 the 5's in A3/C7/C9 cannot be 5. 
// The same logic can be traced from C2 to A7 so the strategy 
// is bi-directional, in the jargon.

// XY CHAIN length=4, chain ends: A7 and C2
// ie Chain 5@A7-9@A7--9@A5-2@A5--2@A1-6@A1--6@C2-5@C2
// This proves 5 is the solution at one end of the chain or the other
// -5[A7]+9[A7]-9[A5]+2[A5]-2[A1]+6[A1]-6[C2]+5[C2]
// - 5 taken off A3
// - 5 taken off C7
// - 5 taken off C9


// Online Solvers
// from : http://www.sudoku-solutions.com/ - can load from file
// from : http://www.sudoku.name/sudoku-solver/en - must enter manually - no help - uses brute force solving
// from : http://sudokusolver.com/sudoku/solver/ - must join
// from : http://www.sudoku-solver.com/ - must buy
// from : http://www.sudoku-solver.net/ - how to upload a puzzle? - kept 1 examples\hard1.txt - but solved
// from : http://www.fiendishsudoku.com/sudoku.html - see fiendish1.txt, but solved
// from : http://en.wikipedia.org/wiki/Suduko - good info - 
// Minimum NUmber of 'givens' -
// the fewest givens that render a solution unique—was recently proven to be 17.
// A number of valid puzzles with 17 givens have been found for the standard variation 
// without a symmetry constraint, by Japanese puzzle enthusiasts, and 18 with the givens 
// in rotationally symmetric cells. Over 48,000 examples of Sudoku puzzles with 17 givens 
// resulting in a unique solution are known - lot more there to read
// from : https://www.sudokudragon.com/ - 

// Alternating Inference Chains
// Chaining strategies now take a new leap with Alternating Inference 
// Chains. These extend X-Cycle into a new dimension - where X-Cycles 
// stuck to a single number, AICs use any candidate number.
// AICs encapsulate all the discussion of chaining strategies so far. 
// It’s very useful to split out chain-like strategies into X-Wings, 
// XY-Chains, Forcing Chains, XYZ-Wings, X-Cycles, Nice Loops and so 
// on, since they have special characteristics which make them spottable. 
// But it turns out they are all part of a larger extended family.
// As we saw in the previous chapter, alternation is just what X-Cycles 
// are about. However, you’ll remember that X-Cycles are applied only 
// to a single candidate number. AICs, on the other hand, take everything 
// from an X-Cycle and extend the logic to as many different candidate 
// numbers as necessary.
// AICs ask the question “How many ways are there to make a strong or a 
// weak link?” If there is more than one way, we can join them up in an 
// alternating manner and make deductions leading to eliminations. Let’s 
// look back on the previous chain-like strategies and note the following:
// We can link two candidates of the same value in a unit – this is 
// called “bi-location” (X-Cycles).
// We can link two different candidates in the same cell – this is called 
// “bi-value”.


// Nice Loops Rule 1
// Nice Loops that alternate all the way round are said to be 'continuous', 
// and they must have an even number of nodes. With a continuous AIC, 
// candidates are not removed from the loop since the loop does not have 
// any flaws. Instead we are looking to eliminate on the units that can 
// be seen by two or more cells that belong to the loop. 
// Specifically, if a unit has an ON number X and an OFF number X then 
// one or other will be the solution. All other candidates X in that unit 
// can be removed. These are called off-chain eliminations. Take this example:
// AIC (Alternating Inference Chain) Rule 1:
// +4[B7]-4[B2]+7[B2]-7[B5]+6[B5]-6[H5]+4[H5]-4[H7]+4[B7]
// - Off-chain candidate 7 taken off B3 - weak link: B2 to B5
// - Off-chain candidate 6 taken off J5 - weak link: B5 to H5
// Two off-chain eliminations occur on the weak links B2 to B5 (candidate 7) 
// and B5 to H5 (candidate 6).	

// This is a classic and pleasingly short Continuous Alternating Inference Chain. 
// Starting a B7 we turn 4 ON. 
// This removes the 4 in B2 turning on 7 in that cell.
// That turns OFF 7 in B5 turning on 6.
// 6 in B5 turns off 6 in H5.
// That turns on 4 in H5 removing 4 in H7
// Which confirms 4 must be ON in B7

// Thus...there is no contradiction in the loop. The nice thing about Nice Loops 
// is they can be reversed. Try starting with 4 ON in B7 and turning 4 OFF in H7 - 
// it will come back round with the same conclusion. In fact, the loop is 
// especially "Nice" because you can start with any candidate in the loop and 
// work your way round, provided it is the same On/Off state as described in the 
// example.

// from : http://www.sudocue.net/guide.php
// Rating system examples
// Mild - Modest - Mean - Maniac - Nightmare (Ruud) 
// Gentle - Moderate - Tough - Diabolical (Mike Mepham) 
// Easy - Moderate - Hard - Very Hard - Fiendish - Extreme (Gaby VanHegan) 
// Easy - Mild - Difficult - Fiendish (Times) 
// Gentle - Moderate - Tricky - Tough - Deadly (Times Killers) 
// Easy - Cool - Thinker - Brain - IQ - Insane (DJ Ape) 
// Super Easy - Very Easy - Easy - Medium - Hard - Harder - Very Hard - Super Hard (Menneske) 
// Very Easy - Easy - Medium - Hard - Harder - Very Hard - Super Hard (Brain Bashers) 
// Easy - Medium - Hard - Evil (Websudoku) 
// Easy - Moderate - Hard - Extreme - Outrageous - Mind Bending (Killer Sudoku Online) 

/// from : http://sudoku.ironmonger.com/home/home.tpl?board=040238600060140003203560041002475300007891400400326000824753006900604030006902054
// Strategies
// Solving Strategies
// Select Strategies to use.
// Disable All
// Cross Hatching
// Counting
// Naked Set
// Hidden Set
// Claiming
// Remote Pairs
// X Wing
// XY Wing
// Simple Coloring
// Fishy Things
// Unique Rectangle
// X Chain
// XYZ Wing
// WXYZ Wing
// XY Chain
// X Chain with Groups
// Aligned Pair Exclusion
// Sue-de-Coq
// Forcing Chains
// Multi Coloring
// 3D Medusa Coloring
// Finned Fishy Things
// Almost Locked Sets
// Nice Loop
// Nice Loop with Groups
// Pattern Overlay Method
// Enable All
// Only enabled strategies will be tested when looking for the next step.
// and candidate display
// Candidate Display
// Select Candidates to show.
//  Hide All Candidates
// Show Ones
// Show Twos
// Show Threes
// Show Fours
// Show Fives
// Show Sixs
// Show Sevens
// Show Eights
// Show Nines
//  Show All Candidates

// eof - Sudo_Strat.cxx
