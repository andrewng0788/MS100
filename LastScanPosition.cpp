#include "stdafx.h"
#include <afxmt.h>
#include "LastScanPosition.h"

#include <algorithm>

#define KNOWN_POSITION_LIMIT	1200

// CPoint.y is row
// CPoint.x is col
static deque<CPoint> lastKnownPosition;
static CCriticalSection lspCS;

void ClearLastScanPosition()
{
	lspCS.Lock();
	lastKnownPosition.clear();
	lspCS.Unlock();
}

int GetTotalLastScanPosition()
{
	lspCS.Lock();
	int num = (int)lastKnownPosition.size();
	lspCS.Unlock();
	return num;
}

bool GetLastScanPosition(unsigned int count, int *row, int *col)
{
	lspCS.Lock();

	// use <= to ignore the 1st
	if (lastKnownPosition.size() <= count || count == 0)
	{
		lspCS.Unlock();
		return false;
	}
	unsigned int num = 0;
	deque<CPoint>::const_iterator iter = lastKnownPosition.begin();
	while (num < count && iter != lastKnownPosition.end())
	{
		row[num] = (*iter).y;
		col[num] = (*iter).x;
		++num;
		++iter;
	}

	lspCS.Unlock();
	return true;
}

void AddLastScanPosition(int row, int col)
{
	lspCS.Lock();
	lastKnownPosition.push_front(CPoint(col, row));
	if (lastKnownPosition.size() > KNOWN_POSITION_LIMIT)
		lastKnownPosition.pop_back();
	lspCS.Unlock();
}

typedef struct
{
	int row;
	int col;
	int jump;
} NEAREST_WM_COORD;

struct NearestWmComparison
{
	bool operator() (const NEAREST_WM_COORD &lhs, const NEAREST_WM_COORD &rhs)
	{
		return lhs.jump < rhs.jump;
	}
};

bool FindNearestLastScanPosition(int currentRow, int currentCol, int &nearestRow, int &nearestCol, LONG &maxJump,
								 int nearestThreshold, list<CPoint>&nearest10)
{
	nearest10.clear();
	int size = (int)lastKnownPosition.size();
	if (0 == size)
		return false;

	list<NEAREST_WM_COORD> allNearest;

	CPoint currentPt(currentCol, currentRow);
	lspCS.Lock();
	deque<CPoint>::iterator iter = std::find(lastKnownPosition.begin(), lastKnownPosition.end(), currentPt);

	bool found = false;
	if (iter != lastKnownPosition.end())
	{
		nearestRow = currentRow;
		nearestCol = currentCol;
		found = true;
	}
	else
	{
		iter = lastKnownPosition.begin();
		CPoint tempTarget;
		int tempJump;
		int confirmJump = maxJump+1;

		NEAREST_WM_COORD wmCoord;
		while (iter != lastKnownPosition.end())
		{
			tempTarget = *iter;
			tempJump = abs(tempTarget.y - currentRow) + abs(tempTarget.x - currentCol);
			if (tempJump < confirmJump ||
				(tempJump == confirmJump && currentRow == tempTarget.y))
			{
				nearestRow = tempTarget.y;
				nearestCol = tempTarget.x;
				confirmJump = tempJump;
				maxJump = confirmJump;
				found = true;
			}
			if (tempJump <= nearestThreshold)
			{
				wmCoord.row = tempTarget.y;
				wmCoord.col = tempTarget.x;
				wmCoord.jump = tempJump;
				allNearest.push_back(wmCoord);
			}
			++iter;	
		}
		if (allNearest.size() > 10)
		{
			allNearest.sort(NearestWmComparison());

			list<NEAREST_WM_COORD>::iterator iter = allNearest.begin();
			int count = 0;
			CPoint point;

			while (iter != allNearest.end() && count < 10)
			{
				point.y = (*iter).row;
				point.x = (*iter).col;
				nearest10.push_back(point);
				++iter;
				++count;
			}
		}
	}
	lspCS.Unlock();
	return found;
}