#pragma once

#include <deque>
#include <list>
using std::deque;
using std::list;
#define TOTAL_LAST_SCAN_CHANGE_MODE_THRESHOLD	700


void ClearLastScanPosition();
int GetTotalLastScanPosition();
bool GetLastScanPosition(unsigned int count, int *row, int *col);
void AddLastScanPosition(int row, int col);
bool FindNearestLastScanPosition(int currentRow, int currentCol, int &nearestRow, int &nearestCol, LONG &maxJump, int nearestThreshold, list<CPoint>&nearest10);
