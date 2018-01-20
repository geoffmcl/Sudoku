
#include "Sudoku.hxx"

/* SU DOKU */

#include <iostream>
#include <sstream>
#include <time.h>   // for time(0)
#include "Sudo_Time.hxx"    // hi-res timer

using namespace std;

#ifndef EOL_CHR
#define EOL_CHR "\r\n"
#endif

enum Ret { 
    Unique, 
    NotUnique, 
    NoSolution 
};

Ret last_uniq_test = NoSolution;
bool is_unque_solution() {
    if (last_uniq_test == Unique)
        return true;
    return false;
}
static int k[9][9],K[9][9];

int get_matrix = 1;
bool shown_solution = false;
int max_solutions = 100;
static double solution_time = 0.0;

static int max_depths, max_deptht;
static int test_iters;
static int solve_iter;

#ifndef _SUDOKU_HXX_
typedef struct tagGRID {
    int v[9][9];
}GRID, *PGRID;
typedef vector<GRID> vGRID;
typedef vGRID::iterator vGRIDi;
#endif // #ifndef _SUDOKU_HXX_

vGRID vGrid;

ostringstream msg;

vGRID *get_vector_grid() { return &vGrid; }

bool is_valid_grid( GRID & grid )
{
    int i, j, v;
    for(i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            v = grid.v[i][j];
            if ( !((v >= 0) && (v <= 9)) ) {
                return false;
            }
        }
    }
    return true;
}

int get_new_matrix(int n)
{
    int i, j, error, temp;

    if (n)
        goto matrixupdation;

	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
		K[i][j]=0;

	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
	{
		cin>>K[i][j];
		k[i][j]=K[i][j];
	}
	msg <<"O.K.? (Enter 0 if OK, 1 to update): ";
	cin>>error;
	if(error==0)
		goto matrixvalidation;

matrixupdation:
	while(1)
	{
		msg <<"Enter Row, Col, Revised number:(0 to exit) ";
		cin>>i;
		if(i==0)break;
		cin>>j>>temp;
		if(i>0&&j>0&&temp>=0&&i<10&&j<10&&temp<10)
		{
			K[i-1][j-1]=temp;
			k[i-1][j-1]=temp;
		}
		else
			msg <<"Enter row/column 1 to 9 & number 0 to 9 only.";
	}

matrixvalidation:
    return 0;
}

void sudo_main()
{
	int i,j,i1,j1,i2,j2;
	int error;
	int count=0;

    if (get_matrix)
        get_new_matrix(0);

matrixupdation:
	msg << "Input matrix: " << endl;
	for(i=0;i<9;i++)
	{
		for(j=0;j<9;j++)
			msg << k[i][j];
	}
    msg << endl;

	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
		if(k[i][j]<0||k[i][j]>9)
		{
			msg <<""<<i+1<<" "<<j+1<<" "<<k[i][j];
			msg <<"Input matrix error.";
			msg <<"Numbers should be 1 to 9 only.";
            get_new_matrix(1);
			goto matrixupdation;
		}

	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
	{
		if(k[i][j]==0)continue;
		error=0;
		for(i1=0;i1<9;i1++)
			if(i!=i1&&k[i][j]==k[i1][j])
			{
				error=1;
				i2=i1;
				j2=j;
			}
		for(j1=0;j1<9;j1++)
			if(j!=j1&&k[i][j]==k[i][j1])
			{
				error=1;
				i2=i;
				j2=j1;
			}
		for(i1=0;i1<9;i1++)
		for(j1=0;j1<9;j1++)
			if((i!=i1||j!=j1)&&i/3==i1/3&&j/3==j1/3&&k[i][j]==k[i1][j1])
			{
				error=1;
				i2=i1;
				j2=j1;
			}
		if(error)
		{
			msg <<""<<i+1<<" "<<j+1<<" "<<k[i][j];
			msg <<""<<i2+1<<" "<<j2+1<<" "<<k[i2][j2];
			msg << " Input matrix error." << endl;
			msg << "A number has been repeated in the same row, col or block." << endl;
            if (get_matrix)
                goto matrixupdation;
            return;
		}
	}

/* Logic starts: 
    Hmmm, NOT REALLY 'logic' ;=))
    just BRUTE FORCE - TRIAL AND ERROR ONLY
    No LOGIC in that, but shows there IS 
    a solution or more available */

	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
	{
		if(K[i][j]>0) goto chksol;

		for( k[i][j]++; k[i][j]<=9; k[i][j]++ )
		{
			error=0;
			for(i1=0;i1<9;i1++)
				if(i!=i1&&k[i][j]==k[i1][j])
                    error=1;

			for(j1=0;j1<9;j1++)
				if(j!=j1&&k[i][j]==k[i][j1])
                    error=1;

			for(i1=0;i1<9;i1++)
			for(j1=0;j1<9;j1++)
				if((i!=i1||j!=j1)&&i/3==i1/3&&j/3==j1/3&&k[i][j]==k[i1][j1])
					error=1;

			if(error==0)break;
		}

		if(k[i][j]>9)
		{
			k[i][j]=0;
			do
			{
				if(i==0&&j==0)
                    goto nomoresol;
				if(j>0)
                    j--;
                else {
                    j=8;i--;
                }
			}while(K[i][j]>0);
			j--;
		}
chksol:
        if(i==8 && j==8)
		{
			msg << " Sol: "<< ++count;
            if (!shown_solution) {
                shown_solution = true;
                msg << endl;
			    for(i1=0;i1<9;i1++)
			    {
				    for(j1=0;j1<9;j1++)
					    msg << k[i1][j1];
			    }
			    msg << endl;
            }
            GRID grid;
		    for(i1 = 0; i1 < 9; i1++) {
                for(j1 = 0; j1 < 9; j1++) {
                    grid.v[i1][j1] = k[i1][j1];
                }
            }
            if (is_valid_grid(grid))
               vGrid.push_back(grid);
			// if(count == max_solutions)
			if( (int)vGrid.size() >= max_solutions )
			{
                msg << endl;
				msg << "Too many solutions. Greater than " << max_solutions << endl;
                msg << "Not checking for more solutions." << endl;
				return;
			}

			while(K[i][j]>0)
			{
				if(i==0&&j==0)goto nomoresol;
				if(j>0)j--;else{j=8;i--;}
			}
			k[i][j]=0;
			do
			{
				if(i==0&&j==0)goto nomoresol;
				if(j>0)j--;else{j=8;i--;}
			}while(K[i][j]>0);
			j--;
		}
	}

nomoresol:
	if(count>0)
		msg << "No more solutions." << endl;
	else
		msg <<"No solution." << endl;
}


// ===============================================
// csharp
byte m_sudoku[9][9];

// Maps sub square to index
const byte m_subSquare[9][9] = {
    {0,0,0,1,1,1,2,2,2},
	{0,0,0,1,1,1,2,2,2},
	{0,0,0,1,1,1,2,2,2},
	{3,3,3,4,4,4,5,5,5},
	{3,3,3,4,4,4,5,5,5},
	{3,3,3,4,4,4,5,5,5},
	{6,6,6,7,7,7,8,8,8},
	{6,6,6,7,7,7,8,8,8},
	{6,6,6,7,7,7,8,8,8}
};

// Maps sub square index to m_sudoku
const byte m_subIndex[9][9][2] = {
    { {0,0},{0,1},{0,2},{1,0},{1,1},{1,2},{2,0},{2,1},{2,2}},
    { {0,3},{0,4},{0,5},{1,3},{1,4},{1,5},{2,3},{2,4},{2,5}},
    { {0,6},{0,7},{0,8},{1,6},{1,7},{1,8},{2,6},{2,7},{2,8}},
    { {3,0},{3,1},{3,2},{4,0},{4,1},{4,2},{5,0},{5,1},{5,2}},
    { {3,3},{3,4},{3,5},{4,3},{4,4},{4,5},{5,3},{5,4},{5,5}},
    { {3,6},{3,7},{3,8},{4,6},{4,7},{4,8},{5,6},{5,7},{5,8}},
    { {6,0},{6,1},{6,2},{7,0},{7,1},{7,2},{8,0},{8,1},{8,2}},
    { {6,3},{6,4},{6,5},{7,3},{7,4},{7,5},{8,3},{8,4},{8,5}},
    { {6,6},{6,7},{6,8},{7,6},{7,7},{7,8},{8,6},{8,7},{8,8}}
};

bool Feasible(byte *M)
{
    for (int d = 1; d < 10; d++)
        if (M[d] > 1)
            return false;
    return true;
}

// Is there one and only one solution?
Ret TestUniqueness(int lev)
{
	// Find untouched location with most information
	int xp = 0;
	int yp = 0;
    byte Mp[10]	= {0};
    int cMp = 10;

    if (lev > max_deptht)
        max_deptht = lev;
    test_iters++;

	for(int y = 0; y < 9; y++)
    {
        for(int x = 0; x < 9; x++)
		{
			// Is this spot unused?
			if(m_sudoku[y][x] == 0)
			{
				// Set M of possible solutions
				byte M[10] = {0,1,2,3,4,5,6,7,8,9};

				// Remove used numbers in the vertical direction
				for(int a = 0; a < 9; a++)
					M[m_sudoku[a][x]] = 0;

				// Remove used numbers in the horizontal direction
				for(int b = 0; b < 9; b++)
					M[m_sudoku[y][b]] = 0;

				// Remove used numbers in the sub square.
				int	squareIndex = m_subSquare[y][x];
                for(int c = 0; c < 9; c++)
				{
					//EntryPoint p = m_subIndex[squareIndex,c];
                    int yy = m_subIndex[squareIndex][c][0];
                    int xx = m_subIndex[squareIndex][c][1];
					M[m_sudoku[yy][xx]] = 0;
                }

				int cM = 0;
				// Calculate cardinality of M
				for(int d = 1; d < 10; d++)
					cM += (M[d] == 0) ? 0 : 1;

				// Is there more information in this spot than in the best yet?
				if(cM < cMp)
				{
					cMp = cM;
					memcpy(&Mp,&M,sizeof(M));
					xp = x;
					yp = y;
				}
            }
        }
    }
	// Finished?
	if(cMp == 10)
		return Unique;

	// Couldn't find a solution?
	if(cMp == 0)
		return NoSolution;

	// Try elements
	int success = 0;
	for(int i = 1; i < 10; i++)
	{
		if(Mp[i] != 0)
		{
			m_sudoku[yp][xp] = Mp[i];
			switch(TestUniqueness(lev + 1))
			{
				case Unique:
					success++;
					break;

				case NotUnique:
					return NotUnique;

				case NoSolution:
					break;
			}

			// More than one solution found?
			if(success > 1)
				return NotUnique;
		}
	}

	// Restore to original state.
	m_sudoku[yp][xp] = 0;

	switch(success)
	{
		case 0:
			return NoSolution;
		case 1:
			return Unique;
		default:
			// Won't happen.
			return NotUnique;
	}
    return NoSolution;
}

bool Solve(int lev)
{
	// Find untouched location with most information
	int xp = 0;
	int yp = 0;
    byte Mp[10] = {0};
	int cMp = 10;

    if (lev > max_depths)
        max_depths = lev;
    solve_iter++;

	for(int y = 0; y < 9; y++)
	{
		for(int x = 0; x < 9; x++)
		{
            if(m_sudoku[y][x] == 0)
            {
                // Set M of possible solutions
                byte M[10] = {0,1,2,3,4,5,6,7,8,9};
                // Remove used numbers in the vertical direction
				for(int a = 0; a < 9; a++)
                    M[m_sudoku[a][x]] = 0;

                // Remove used numbers in the horizontal direction
				for(int b = 0; b < 9; b++)
					M[m_sudoku[y][b]] = 0;

                // Remove used numbers in the sub square.
				int	squareIndex = m_subSquare[y][x];
				for(int c = 0; c < 9; c++)
				{
                    byte yy = m_subIndex[squareIndex][c][0];
                    byte xx = m_subIndex[squareIndex][c][1];
                    // EntryPoint p = m_subIndex[squareIndex,c];
					//		M[m_sudoku[p.x,p.y]] = 0;
                    M[m_sudoku[yy][xx]] = 0;
                }

                int cM = 0;
				// Calculate cardinality of M
				for(int d = 1; d < 10; d++)
					cM += (M[d] == 0) ? 0 : 1;

				// Is there more information in this spot than in the best yet?
				if(cM < cMp)
				{
					cMp = cM;
					memcpy( &Mp, &M, sizeof(M) );
					xp = x;
					yp = y;
				}

            }
        }
    }

	// Finished?
	if(cMp == 10)
		return true;

	// Couldn't find a solution?
	if(cMp == 0)
		return false;

	// Try elements
	for(int i = 1; i < 10; i++)
	{
		if(Mp[i] != 0)
		{
			m_sudoku[yp][xp] = Mp[i];
			if(Solve(lev + 1))
				return true;
		}
	}

	// Restore to original state.
	m_sudoku[yp][xp] = 0;
	return false;
}

bool done_seed = false;
int Randomizer_GetInt( double range_max, double range_min = 0.0);
int Randomizer_GetInt( double range_max, double range_min)
{
    if (!done_seed) {
        srand( (unsigned) time(0) );
        done_seed = true;
    }
    //double r = (double)rand() / (double)(RAND_MAX + 1);
    double r = ((double)rand() / (double)RAND_MAX);
    int u = (int)((r * (range_max - range_min)) + range_min);
    return u;
}

// Generate spots
bool Gen(int spots)
{
    for (int i = 0; i < spots; i++)
	{
		int xRand,yRand;
        int cnt = 0;
		do
		{
			xRand = Randomizer_GetInt(9);
			yRand = Randomizer_GetInt(9);
		} while(m_sudoku[yRand][xRand] != 0);

		/////////////////////////////////////
		// Get feasible values for spot.
		/////////////////////////////////////

		// Set M of possible solutions
		byte M[10] = {0,1,2,3,4,5,6,7,8,9};

		// Remove used numbers in the vertical direction
		for(int a = 0; a < 9; a++)
			M[m_sudoku[a][xRand]] = 0;

		// Remove used numbers in the horizontal direction
		for(int b = 0; b < 9; b++)
			M[m_sudoku[yRand][b]] = 0;

		// Remove used numbers in the sub square.
		int	squareIndex = m_subSquare[yRand][xRand];
		for(int c = 0; c < 9; c++)
		{
			int yy = m_subIndex[squareIndex][c][0];
            int xx = m_subIndex[squareIndex][c][1];
			M[m_sudoku[yy][xx]] = 0;
		}

		int cM = 0;
		// Calculate cardinality of M
		for(int d = 1; d < 10; d++)
			cM += (M[d] == 0) ? 0 : 1;

		// Is there a number to use?
		if(cM > 0)
		{
			int e = 0;

			do
			{
				// Randomize number from the feasible set M
				e =  Randomizer_GetInt(10,1);
			} while(M[e] == 0);

			// Set number in Sudoku
			m_sudoku[yRand][xRand] = (byte)e;
		}
		else
		{
			// Error
			return false;
		}
	}

	// Successfully generated a feasible set.
	return true;
}
bool Generate(int spots, int numberOfTries = 1000000);

bool IsSudokuFeasible()
{
	for(int y = 0; y < 9; y++)
	{
		for(int x = 0; x < 9; x++)
		{
            int m;
			// Set M of possible solutions
			byte *M = new byte[10];
			for(m = 0; m < 10; m++)
                M[m] = 0;

			// Count used numbers in the vertical direction
			for(int a = 0; a < 9; a++)
				M[m_sudoku[a][x]]++;

			// Sudoku feasible?
			if(!Feasible(M)) {
                delete M;
				return false;
            }

			//M = new byte[10];
			for(m = 0; m < 10; m++)
                M[m] = 0;

			// Count used numbers in the horizontal direction
			for(int b = 0; b < 9; b++)
				M[m_sudoku[y][b]]++;

			if(!Feasible(M)) {
                delete M;
				return false;
            }

			//M = new byte[10];
			for(m = 0; m < 10; m++)
                M[m] = 0;

			// Count used numbers in the sub square.
			int	squareIndex = m_subSquare[y][x];
			for(int c = 0; c < 9; c++)
			{
				int yy = m_subIndex[squareIndex][c][0];
                int xx = m_subIndex[squareIndex][c][1];
				if(yy != y && xx != x)
					M[m_sudoku[yy][xx]]++;
			}
			if(!Feasible(M)) {
                delete M;
				return false;
            }
		}
	}

	return true;
}

bool IsSudokuUnique()
{
    byte m[9][9]; // = Data;
    memcpy(&m,&m_sudoku, sizeof(m_sudoku));
    bool b = TestUniqueness(0) == Unique;
    // Data = m;
    memcpy(&m_sudoku, &m, sizeof(m_sudoku));
    return b;
}

int GetNumberSpots()
{
    int num = 0;
    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++)
            num += m_sudoku[y][x] == 0 ? 0 : 1;
    return num;
}

bool Generate(int spots, int numberOfTries)
{
	// Number of set spots.
	int num = GetNumberSpots();

	if(!IsSudokuFeasible() || num > spots)
	{
		// The supplied data is not feasible.
		// - or -
		// The supplied data has too many spots set.
        return false;
	}

	/////////////////////////////////////
	// Randomize spots
	/////////////////////////////////////

    byte originalData[9][9];
    memcpy(&originalData, &m_sudoku, sizeof(m_sudoku));
    int test = 0;
    long tries = 0;
    for (; tries < numberOfTries; tries++)
    {
        // Try to generate spots
        if (Gen(spots - num))
        {
            // Test if unique solution.
            if (IsSudokuUnique())
            {
                return true;
            }
        }
        if (tries && ((tries % 1000) == 0)) {
            test++;
        }
        // Start over.
        memcpy(&m_sudoku, &originalData, sizeof(m_sudoku));
        //Data = originalData;
    }

    return false;
}

void Clear_Sudoku()
{
    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++)
            m_sudoku[y][x] = 0;
}


// ==================================================================
// my sudoku code
// ==============

void Write_To_File(char * file, char *mode)
{
    FILE *fp = fopen(file,mode);
    if (!fp) return;
    msg.str("");
    vGRIDi ii;
    GRID grid;
    int i, j, v, ok;
    for (ii = vGrid.begin(); ii != vGrid.end(); ii++) {
        grid = *ii;
        ok = 0;
        for(i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                v = grid.v[i][j];
                if ( !((v >= 0) && (v <= 9)) ) {
                    ok = 1;
                    break;
                }
            }
            if (ok)
                break;
        }
        if (ok)
            continue;

        for(i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                msg << "" << grid.v[i][j];
            }
        }
        msg << endl;
    }
    fwrite( msg.str().c_str(), 1, msg.str().size(), fp);
    fclose(fp);
}


void Write_Solution_File()
{
    char *file = "temptest.txt";
    char *mode = "w";
    Write_To_File(file,mode);
}

void run_test(PABOX2 pb)
{
    Test_Solve(pb);
}

void run_test2(PABOX2 pb)
{
    int i,j;
	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
		K[i][j]=0;

	for(i=0;i<9;i++) {
    	for(j=0;j<9;j++) {
    		k[i][j] = pb->line[i].val[j];
        }
    }
    get_matrix = 0;
    shown_solution = false;
    msg.str("");
    vGrid.clear();
    sudo_main();
#ifdef WIN32 // windows MessageBox
    MessageBox( g_hWnd, msg.str().c_str(), "Test Solution", MB_OK | MB_ICONINFORMATION );
#endif // WIN32 MessageBox - TODO: alternative    
    if (vGrid.size()) {
        Write_Solution_File();
    }
}

vGRID *Get_Hints(PABOX2 pb)
{
    int y,x;
    for (y = 0; y < 9; y++) {
        for (x = 0; x < 9; x++) {
            m_sudoku[y][x] = pb->line[y].val[x];
        }
    }
    vGRID *vgp = get_vector_grid();
    vgp->clear();
    x = 0;
    max_depths = 0;
    solve_iter = 0;
    if (Solve(0)) {
        GRID grid;
        for (y = 0; y < 9; y++) {
            for (x = 0; x < 9; x++) {
                grid.v[y][x] = m_sudoku[y][x];
            }
        }
        vgp->push_back(grid);
        x = 1;
    }
    if (vgp->size()) {
        Write_Solution_File();
    }
    return vgp;
}

vGRID *Get_Hints2(PABOX2 pb)
{
    int i, j;
	for(i=0;i<9;i++)
	for(j=0;j<9;j++)
		K[i][j]=0;

	for(i=0;i<9;i++) {
    	for(j=0;j<9;j++) {
    		k[i][j] = pb->line[i].val[j];
        }
    }
    get_matrix = 0;
    shown_solution = false;
    msg.str("");
    vGrid.clear();
    sudo_main();
    return get_vector_grid();
}

int Test_Solve( PABOX2 pb )
{
    int y,x, val, cnt;
    Ret r = NoSolution;
    cnt = 0;
    for (y = 0; y < 9; y++) {
        for (x = 0; x < 9; x++) {
            val = pb->line[y].val[x];
            m_sudoku[y][x] = val;
            if (val)
                cnt++;
        }
    }
    if (cnt < 5) {
        sprtf("Spot count %d < 5. No solution attempted\n",cnt);
        return 0;
    }
    x = 0;
    max_depths = 0;
    solve_iter = 0;
    if (Solve(0))
        x = 1;

    char *tb = GetNxtBuf();
    sprintf(tb,"With %d cycles of Solve(), max depth %d, it %s",
        solve_iter, max_depths,
        (x ? "SUCCEEDED" : "FAILED"));
    sprtf("%s\n",tb);

    if (x) {
        for (y = 0; y < 9; y++) {
            for (x = 0; x < 9; x++) {
                m_sudoku[y][x] = pb->line[y].val[x];
            }
        }
        x = 0;
        max_deptht = 0;
        test_iters = 0;
        r = TestUniqueness(0);
        last_uniq_test = r;
        sprintf(tb,"With %d cycles of TestUniqueness(), max depth %d, %s",
            test_iters, max_deptht,
            (r == Unique) ? "solution is UNIQUE" : (r == NotUnique) ? "NOT UNIQUE" : "FAILED");
        sprtf("%s\n",tb);
        x = 1;
    }

    return x;
}

int Get_Solution( PABOX2 pb, bool test_unique )
{
    Timer tm;
    char *tb;
    int y,x, val, cnt;
    Ret r = NoSolution;
    last_uniq_test = NoSolution;
    cnt = 0;
    tm.start();
    for (y = 0; y < 9; y++) {
        for (x = 0; x < 9; x++) {
            val = pb->line[y].val[x];
            m_sudoku[y][x] = val;
            if (val)
                cnt++;
        }
    }
    if (cnt < 5) {
        sprtf("Spot count %d < 5. No solution attempted\n",cnt);
        return 0;
    }
    x = 0;
    max_depths = 0;
    solve_iter = 0;
    pb->bflag &= ~(bf_DnTest | bf_Unique);
    if (Solve(0)) {
        for (y = 0; y < 9; y++) {
            for (x = 0; x < 9; x++) {
                pb->line[y].val[x] = m_sudoku[y][x];
            }
        }
        x = 1;
    }
    const char *uniq_test = 0;
    if (x && test_unique) {
        for (y = 0; y < 9; y++) {
            for (x = 0; x < 9; x++) {
                m_sudoku[y][x] = pb->line[y].val[x];
            }
        }
        x = 0;
        max_deptht = 0;
        test_iters = 0;
        r = TestUniqueness(0);
        uniq_test = (r == Unique) ? "UNIQUE" : (r == NotUnique) ? "NOT UNIQUE" : "FAILED";
        //tb = GetNxtBuf();
        //sprintf(tb,"In %d cyc, dep %d, Solution is %s",
        //    test_iters, max_deptht, uniq_test);
        //sprtf("%s\n",tb);
        x = 2;
    }
    tb = GetNxtBuf();
    sprintf(tb,"solved cycs %d, dep %d %s ",
        solve_iter, max_depths,
        (x ? "ok" : "FAILED"));
        //(x ? "SUCCEEDED" : "FAILED"));
    if ((x == 2) && uniq_test)
        strcat(tb,uniq_test);
    tm.stop();
    solution_time = tm.getElapsedTime();
    sprintf(EndBuf(tb)," in %s.",tm.getTimeStg());
    sprtf("%s\n",tb);   // SHOW some results after running a brute force solution
    if (x) {
        pb->bflag |= bf_DnTest;
        if (r == Unique)
            pb->bflag |= bf_Unique;
    }
    set_repaint(false);
    return x;
}

bool Check_Changed() 
{
    int res;
    if (g_bChanged && total_empty_count) {
        char *tb = GetNxtBuf();
        sprintf(tb,"Have generated a NEW box with %d spots.", g_iMinGiven);
        strcat(tb,EOL_CHR"But current Sudoku box has been changed." EOL_CHR "Do you want to SAVE these results?" EOL_CHR "Click [Yes] to save this change." EOL_CHR "Click [ NO ] to over write with the new.");
        //res = MB2(tb,"Save on Generation?");
        //if (res == IDYES) {
        //    Do_ID_FILE_SAVE(g_hWnd);
        //    return false;
        //}
        res = Do_MsgBox_YN(tb,"Save on Generation?");
        if (!res) {
            Do_ID_FILE_SAVE(g_hWnd);
            return false;
        }
    }
    return true;
}

void Write_Generation_File()
{
    char *file = "tempgen.txt";
    char *mode = "a";   // append
    Write_To_File(file,mode);
}


void Save_New_Generation(PABOX2 pb)
{
    vGRID *vgp = get_vector_grid();
    vgp->clear();
    GRID grid;
    int y, x;
    for (y = 0; y < 9; y++) {
       for (x = 0; x < 9; x++) {
          grid.v[y][x] = m_sudoku[y][x];
       }
    }
    vgp->push_back(grid);
    Write_Generation_File();
}

bool Generate_New( PABOX2 pb, int spots )
{
    int y,x;
    for (y = 0; y < 9; y++) {
        for (x = 0; x < 9; x++) {
            m_sudoku[y][x] = pb->line[y].val[x];
        }
    }
    bool res = Generate(spots);
    if (res) {
        Check_Changed();
        Reset_Active_File(g_pSpecial);
        File_Reset();
        for (y = 0; y < 9; y++) {
            for (x = 0; x < 9; x++) {
                pb->line[y].val[x] = m_sudoku[y][x];
            }
        }
        Save_New_Generation(pb);
        set_repaint();
    }
    return res;
}

// eof - Sudo_Scaps.cxx

