#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <random>
#include <array>
#include <chrono>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <windows.h> // for sleep function 
using namespace std;



// BASIC LAYOUT OF A BOX IN OUR MATRIX / PUZZLE
// -------
// |     |
// |  7  |
// |     |
// -------

const int n=9; // n is the Puzzle size
const int EMPTY_VALUE=0;
array<int, 9> values{1, 2, 3, 4, 5, 6, 7, 8, 9}; // array of allowed values
int numberOfSolution = 1;


// this function will be used to prevent terminal flickering when animate puzzle solving process.
void setCursorPosition(int x, int y)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    cout.flush();
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(hOut, coord);
}

// this function will tell us which cell are empty
bool hasEmptyCell(int puzzle[n][n])
{
    for (int r = 0; r < n; r++) // r-> row traversal 
    {
        for (int c = 0; c < n; c++) // c -> column traversal
        {
            if (puzzle[r][c] == EMPTY_VALUE)
                return true;
        }
    }
    return false;
}

// this function help us to print the overall framework of the sudoku puzzle whre we cal place our elements  
void printPuzzle(int puzzle[n][n], bool clear = true)
{
    if (clear)
    {
        setCursorPosition(0, 0);
    }
    string text, separator, padding;
    
    for (int i = 0; i < n; i++)
    {
        text = "|";
        separator = " -";
        padding = "|";
        for (int j = 0; j < n; j++)
        {
            string value = puzzle[i][j] == EMPTY_VALUE ? " " : std::to_string(puzzle[i][j]);
            text += "  " + value + "  |";
            separator += "------";
            padding += "     |";
            if (j % 3 == 2 && j != n - 1)
            {
                text += "|";
                padding += "|";
            }
        }
        if (i != 0 && i % 3 == 0)
        {
            replace(separator.begin(), separator.end(), '-', '=');
        }
        cout << separator << endl;
        cout << padding << endl;
        cout << text << endl;
        cout << padding << endl;
    }
    cout << separator << endl;
}

//  this function checks whether the element put in its place is valid according to the rules of sudoku game
bool isValid(int puzzle[n][n], int row, int col, int value)
{
    for (int c = 0; c < n; c++)
    {
        if (puzzle[row][c] == value) // if we found the same value in the row
            return false;
    }
    for (int r = 0; r < n; r++)
    {
        if (puzzle[r][col] == value) // if we found the same element in the 
            return false;
    }
    

    // if we found the same element in the box (3*3)
    int startRow = floor(row / 3) * 3; // getting the starting row of the that box by obtaining the quotient after dividing the coordinate by 3 and ten multiplying it my 3 
    int startCol = floor(col / 3) * 3; // similarly for the column
    for (int r = startRow; r < startRow + 3; r++)
    {
        for (int c = startCol; c < startCol + 3; c++)
        {
            if (puzzle[r][c] == value) // finding the same element in the box 
                return false;
        }
    }
    return true;
}

// this function helps us fill the empty values in aour sudoku matrix
bool fillPuzzle(int (&puzzle)[n][n])
{
    int row, col;
    for (int i = 0; i < n * n; i++)
    {
        row = floor(i / n);
        col = i % n;
        if (puzzle[row][col] == EMPTY_VALUE)
        {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            // time_since_epoch. Returns the amount of time between this time_point and the clock's epoch. (A clock's epoch is the time from which the clock starts measuring time, such as midnight 1/1/1970). It's returned as a duration value.
            shuffle(values.begin(), values.end(), std::default_random_engine(seed)); // used to generate a rndom number from the values array
            for (int j = 0; j < n; j++)
            {
                if (isValid(puzzle, row, col, values[j]))
                {
                    puzzle[row][col] = values[j];
                    if (!hasEmptyCell(puzzle) || fillPuzzle(puzzle))
                    {
                        return true;
                    }
                }
            }
            break;
        }
    }
    puzzle[row][col] = EMPTY_VALUE;
    return false;
}

// solving sudoku using the backtracking algorithm
bool solveSudoku(int puzzle[n][n], bool visualize = false)
{
    int row, col;
    for (int i = 0; i < n * n; i++)
    {
        row = floor(i / n);
        col = i % n;
        if (puzzle[row][col] == EMPTY_VALUE)
        {
            for (int value = 1; value <= n; value++)
            {
                if (isValid(puzzle, row, col, value))
                {
                    puzzle[row][col] = value;
                    if (visualize)
                    {
                        Sleep(100);
                        printPuzzle(puzzle);
                    }
                    if (!hasEmptyCell(puzzle))
                    {
                        numberOfSolution++;
                        if (visualize)
                        {
                            Sleep(100);
                            printPuzzle(puzzle);
                            return true;
                        }
                        break;
                    }
                    else if (solveSudoku(puzzle, visualize))
                    {
                        return true;
                    }
                }
            }
            break;
        }
    }
    puzzle[row][col] = EMPTY_VALUE;
    if (visualize)
    {
        Sleep(100);
        printPuzzle(puzzle);
    }
    return false;
}

// this function helps us create the puzzle for our problem
void generatePuzzle(int (&puzzle)[n][n], int difficulty = 1)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            puzzle[i][j] = EMPTY_VALUE;
        }
    }
    fillPuzzle(puzzle);
    srand((unsigned)time(0)); // initializes the random number generator with a seed based on the current time.
    int attempt = difficulty;
    while (attempt > 0)
    {
        int row = floor(rand() % n);
        int col = floor(rand() % n);
        while (puzzle[row][col] == EMPTY_VALUE)
        {
            row = floor(rand() % n);
            col = floor(rand() % n);
        }
        int backupValue = puzzle[row][col];
        puzzle[row][col] = EMPTY_VALUE;
        numberOfSolution = 0;
        solveSudoku(puzzle);
        if (numberOfSolution != 1)
        {
            puzzle[row][col] = backupValue;
            attempt--;
        }
    }
}

int main (int, char **){
    int puzzle[n][n];

    generatePuzzle(puzzle);
    printPuzzle(puzzle, true);

    
    system("cls");
    printPuzzle(puzzle);
    string run;
    cout << "Do want to solve the puzzle (Y/n) ";
    cin >> run;
    if (run == "Y" || run == "y")
    {
        cout.flush(); // used to synchronize the associated stream buffer with its controlled output sequence.
        solveSudoku(puzzle, true);
    }
    
    
    // cout.flush();
    return 0;
}


