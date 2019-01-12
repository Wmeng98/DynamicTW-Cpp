#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <exception>
#include <cstdlib>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

struct point {
  int x;
  int y;
};

void print_matrix(vector< vector<int> > &matrix) {
  cout << endl;
  for(auto &row : matrix) {
    for(auto &el : row) {
      cout << el;
      if (el < 10 && el >= 0) { // temporary format fix for numbers with 1 & 2 digits only...
        cout << "   ";
      }
      else {
        cout << "  ";
      }
    }
    cout << endl;
  }
  cout << endl;
}

// generate a NxN matrix, populated with empty(-1) cells
void gen_matrix(vector< vector<int> > &matrix, int n) {
  int randNum = -1;
  for(int i = 0; i < n; i++) {
    vector<int> rowN;
    for(int j = 0; j < n; j++) {
      rowN.push_back(randNum);
    }
    matrix.push_back(rowN);
  }
}

// Fill matrix with time series A/B
// Assume: timeSeries size fit into dimensions of the matrix
void populate_matrix(vector<vector<int> > &matrix, vector<int> &timeSeries, int AB) {
  if (AB == 0) {
    // vertical
    int j = 0;
    for (int i = timeSeries.size() - 1; i >= 0; i--) {
      matrix[i][0] = timeSeries[j];
      j++;
    }
  }
  else {
    // horizontal
    for (int i = timeSeries.size() - 1; i >= 0; i--) {
      matrix[matrix.size() - 1][i + 1] = timeSeries[i];
    }
  }

}

int getTimeSeriesSize() {
  int pointSize;
  cin >> pointSize;
  return pointSize;
}

int getPolySize() {
  int size;
  cin >> size;
  return size;
}


// Read time series values from a file
void readTimeSeries(vector<int> &timeS, int size, vector<point> &timeSS) {
  // read in from stdin
  int c = 0;
  int val;
  while (c < size) {
    point p;
    cin >> val; // x val
    p.x = val;
    cin >> val;
    p.y = val;

    timeS.push_back(val);
    timeSS.push_back(p);

    c++;
  } // skip the next "size n" values because they represent the correspnding x values to the
}

// Pass in matrix coordinates
int DL(int colSize, int x, int y, vector<vector<int> > &matrix) {
  if (x > colSize) {
    return 0;
  }
  return matrix[x][y];
}
// Pass in matrix coordinates
int DB(int x, int y, vector<vector<int> > &matrix) {
  if (y < 1) {
    return 0;
  }
  return matrix[x][y];
}

void dtw_leftCol(vector<vector<int> > &matrix) {
  // nxn matrix
  // col 1, matrix[x][1]; size of col is (n - 1)
  int colSize = matrix[0].size() - 2; // assume valid
  int val;
  for (int i = colSize; i >= 0; i--) {
    // | Ai - Bi | + D [ i-1, 0 ]
    val = abs(matrix[i][0] - matrix[matrix[0].size() -1][1]) + DL(colSize, i + 1, 1, matrix);

    matrix[i][1] = val;
  }
}
void dtw_bottomRow(vector<vector<int> > &matrix) {
  int rowIndex = matrix[0].size() - 2;
  int bottomIndex = matrix[0].size() - 1; // assume valid
  int val;
  for (int i = 1; i <= bottomIndex; i++) {
    // | Ai - Bi | + D [ i-1, 0 ]

    val = abs(matrix[rowIndex][0] - matrix[bottomIndex][i]) + DB(rowIndex, i - 1, matrix);

    matrix[rowIndex][i] = val;
  }
}

// Assumption is passed in valid starting coordinate [][]
// x: row index
// y: col index
void dtw_subMatrix(vector<vector<int> > &matrix, int x, int y) {
  // base case(s)
  if (x < 0 || y > matrix[0].size() - 1) {
    return;
  }
  int val;
  val = abs(matrix[x][0] - matrix[matrix[0].size() - 1][y]) + min(min(matrix[x + 1][y - 1], matrix[x + 1][y]), matrix[x][y - 1]); // assume no segment fault
  matrix[x][y] = val;
  dtw_subMatrix(matrix, x - 1, y);
  dtw_subMatrix(matrix, x, y + 1);
  // dtw_subMatrix(matrix, x - 1, y + 1);
}
void augmentSmallestPath(vector<vector<int> > &matrix, int x, int y) {
  if (matrix[x][y] > 0) {
    matrix[x][y] *= -1; // indicate current val in matrix is apart of the shortest warped path
  }
  
  if (x + 1 > matrix[0].size() - 1 || y - 1 < 0) {
    return;
  }

  // Find the next smallest path from topRight to bottomLeft
  // smallest/shortest paths are given priority
  int neighbourLeft, neighbourBottomLeft, neighboutBottom;
  neighbourLeft = matrix[x][y - 1];
  neighbourBottomLeft = matrix[x + 1][y - 1];
  neighboutBottom = matrix[x + 1][y];
  int minVal = min(min(neighbourLeft, neighbourBottomLeft), neighboutBottom);
  if (minVal == neighboutBottom) {
    augmentSmallestPath(matrix, x + 1, y);
  }
  else if (minVal == neighbourBottomLeft) {
    augmentSmallestPath(matrix, x + 1, y - 1);
  }
  else {
    augmentSmallestPath(matrix, x, y - 1);
  }
}



int main(int argc, char const *argv[]) {
  
  int polySize;
  vector<int> timeA;
  vector<int> timeB;
  vector<vector<int> > matrix;

  vector<point> timeAA;
  vector<point> timeBB;

  polySize = getPolySize();

  gen_matrix(matrix, polySize + 1);

  // init time series
  readTimeSeries(timeA, polySize, timeAA);
  readTimeSeries(timeB, polySize, timeBB);

  populate_matrix(matrix, timeA, 0);
  populate_matrix(matrix, timeB, 1);

  dtw_leftCol(matrix);
  dtw_bottomRow(matrix);
  dtw_subMatrix(matrix, matrix[0].size() - 3, 2);
  augmentSmallestPath(matrix, 0, matrix[0].size() - 1);

  print_matrix(matrix);
}
