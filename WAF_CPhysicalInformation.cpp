#include "stdafx.h"
#include "WAF_CPhysicalInformation.h"

// Physical Information
WAF_CPhysicalInformation::WAF_CPhysicalInformation()
{
	m_lX = 0;
	m_lY = 0;
	m_lDataListIndex = 0;
	m_bOverLapDie	= FALSE;

	m_lRow = 0;
	m_lCol = 0;

	m_bProcessed = FALSE;
}

WAF_CPhysicalInformation::WAF_CPhysicalInformation(const long lX, const long lY, 
												   const LONG lDataListIndex,
												   const BOOL bOverLapDie)
{
	m_lX = lX;
	m_lY = lY;

	m_bProcessed = FALSE;
	m_lDataListIndex = lDataListIndex;
	m_bOverLapDie = bOverLapDie;

	m_lRow = 0; 
	m_lCol = 0;
}

WAF_CPhysicalInformation::~WAF_CPhysicalInformation()
{
}

WAF_CPhysicalInformation::WAF_CPhysicalInformation(const WAF_CPhysicalInformation& Src)
{
	m_lX = Src.m_lX;
	m_lY = Src.m_lY;

	m_bProcessed = Src.m_bProcessed;
	m_lDataListIndex = Src.m_lDataListIndex;
	m_bOverLapDie = Src.m_bOverLapDie;

	m_lRow = Src.m_lRow;
	m_lCol = Src.m_lCol;
}

void WAF_CPhysicalInformation::SetX(const long lX)
{
	m_lX = lX;
}

void WAF_CPhysicalInformation::SetY(const long lY)
{
	m_lY = lY;
}

void WAF_CPhysicalInformation::SetDataListIndex(const LONG lDataListIndex)
{
	m_lDataListIndex = lDataListIndex;
}

void WAF_CPhysicalInformation::SetOverLapDie(const BOOL bOverLapDie)
{
	m_bOverLapDie = bOverLapDie;
}


void WAF_CPhysicalInformation::SetRow(const long lRow)
{
	m_lRow = lRow;
}

void WAF_CPhysicalInformation::SetCol(const long lCol)
{
	m_lCol = lCol;
}

long WAF_CPhysicalInformation::GetX() const
{
	return m_lX;
}

long WAF_CPhysicalInformation::GetY() const
{
	return m_lY;
}

LONG WAF_CPhysicalInformation::GetListIndex() const
{
	return m_lDataListIndex;
}

BOOL WAF_CPhysicalInformation::GetOverLapDie() const
{
	return m_bOverLapDie;
}

long WAF_CPhysicalInformation::GetRow() const
{
	return m_lRow;
}

long WAF_CPhysicalInformation::GetCol() const
{
	return m_lCol;
}

void WAF_CPhysicalInformation::SetProcessed(const BOOL bProcessed)
{
	m_bProcessed = bProcessed;
}

BOOL WAF_CPhysicalInformation::GetProcessed() const
{
	return m_bProcessed;
}

const WAF_CPhysicalInformation& WAF_CPhysicalInformation::operator=(const WAF_CPhysicalInformation& Src)
{
	m_lX = Src.m_lX;
	m_lY = Src.m_lY;

	m_bProcessed = Src.m_bProcessed;
	m_lDataListIndex = Src.m_lDataListIndex;
	m_bOverLapDie = Src.m_bOverLapDie;

	m_lRow = Src.m_lRow;
	m_lCol = Src.m_lCol;

	return *this;
}

// Physical information map
WAF_CPhysicalInformationMap::WAF_CPhysicalInformationMap()
{
	m_aalMapIndex = NULL;
	m_ulRows = 0;
	m_ulCols = 0;
	m_bKeepIsolatedDice = FALSE;
	m_lXOffset = 0;

	m_bDebugOn = FALSE;
	m_bScan2PrRecord	= FALSE;
}

WAF_CPhysicalInformationMap::~WAF_CPhysicalInformationMap()
{
	RemoveAll();
}

void WAF_CPhysicalInformationMap::SetDebugOn(const BOOL bEnable)
{
	m_bDebugOn = bEnable;
}

void WAF_CPhysicalInformationMap::KeepIsolatedDice(const BOOL bKeepIsolated)
{
	m_bKeepIsolatedDice = bKeepIsolated;
}

// Remove all position
void WAF_CPhysicalInformationMap::RemoveAll()
{
	int i;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		delete m_PhysicalInformationArray.GetAt(i);
	}
	m_PhysicalInformationArray.RemoveAll();

	if (m_aalMapIndex != NULL) 
	{
		for (i=0; i<(int)m_ulRows; i++) 
		{
			delete[] m_aalMapIndex[i];
		}
		delete[] m_aalMapIndex;
	}
	m_aalMapIndex = NULL;
}

// Add position
void WAF_CPhysicalInformationMap::SortAdd_Tail(const long lX, const long lY, const LONG lIndex, const BOOL bOverLap)
{
	// add the position according to the Y location
	if (m_PhysicalInformationArray.GetSize() == 0)
	{
		m_PhysicalInformationArray.Add(new WAF_CPhysicalInformation(lX, lY, lIndex, bOverLap));
		return;
	}

	int i = (int)m_PhysicalInformationArray.GetSize()-1;
	while (i >= 0) 
	{
		if (m_PhysicalInformationArray.GetAt(i)->GetY() < lY)
			i--;
		else 
			break;
	}

	if (i == m_PhysicalInformationArray.GetSize()-1) 
		m_PhysicalInformationArray.Add(new WAF_CPhysicalInformation(lX, lY, lIndex, bOverLap));
	else 
		m_PhysicalInformationArray.InsertAt(i+1, new WAF_CPhysicalInformation(lX, lY, lIndex, bOverLap));
}

void WAF_CPhysicalInformationMap::SortAdd_Head(const long lX, const long lY, const LONG lIndex, const BOOL bOverLap)
{
	// add the position according to the Y location
	if (m_PhysicalInformationArray.GetSize() == 0)
	{
		m_PhysicalInformationArray.Add(new WAF_CPhysicalInformation(lX, lY, lIndex, bOverLap));
		return;
	}

	int i = 0;
	while (i < m_PhysicalInformationArray.GetSize()) 
	{
		if (m_PhysicalInformationArray.GetAt(i)->GetY() > lY)
			i++;
		else 
			break;
	}

	if (i == m_PhysicalInformationArray.GetSize()) 
		m_PhysicalInformationArray.Add(new WAF_CPhysicalInformation(lX, lY, lIndex, bOverLap));
	else 
		m_PhysicalInformationArray.InsertAt(i, new WAF_CPhysicalInformation(lX, lY, lIndex, bOverLap));
}

void WAF_CPhysicalInformationMap::PreSortFindDie(WAF_CPhysicalInformationArray& TempArray, 
												 WAF_CPhysicalInformationArray& CompleteArray, 
												 WAF_CPhysicalInformationArray& Stack, 
												 BOOL** aabDone, 
												 const long lX, const long lY, 
												 const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
												 const long lRow, const long lCol)
{
	int i;
	int nStart = 0; 
	int nEnd = (int)TempArray.GetSize()-1;
	int nTarget = 0;

	// Use the binary search on Y location
	while (nStart <= nEnd) 
	{
		int nMid = (nEnd + nStart) / 2;
		WAF_CPhysicalInformation* p = TempArray.GetAt(nMid);
		int nSize = abs(lY - p->GetY());
		if (nSize < (int)ulDieSizeYTol)
		{
			nTarget = nMid;
			break;
		}
		else if (p->GetY() > lY) 
		{
			nStart = nMid + 1;
		}
		else 
		{
			nEnd = nMid - 1;
		}
	}

	// Linear search to find x location
	CUIntArray astrIndex;
	int nIndex = nTarget;
	while ((nIndex >= 0) && (nIndex < TempArray.GetSize()))
	{
		WAF_CPhysicalInformation* p = TempArray.GetAt(nIndex);
		if (abs(lY - p->GetY()) < (int)ulDieSizeYTol) 
		{
			if (abs(lX - p->GetX()) < (int)ulDieSizeXTol)
			{
				astrIndex.Add(nIndex);
			}
		}
		else 
			break;
		nIndex--;
	}

	nIndex = nTarget+1;
	while (nIndex < TempArray.GetSize())
	{
		WAF_CPhysicalInformation* p = TempArray.GetAt(nIndex);
		if (abs(lY - p->GetY()) < (int)ulDieSizeYTol)
		{
			if (abs(lX - p->GetX()) < (int)ulDieSizeXTol)
			{
				astrIndex.Add(nIndex);
			}
		}
		else 
			break;

		nIndex++;
	}

	// Add to the cluster
	if (astrIndex.GetSize() > 0) 
	{
#if 1
		int n = astrIndex[0];
		WAF_CPhysicalInformation* pTargetInfo = TempArray.GetAt(astrIndex[0]);
#else
		int nIndex = 0;
		WAF_CPhysicalInformation* pTargetInfo;
		for (i=0; i<astrIndex.GetSize(); i++) 
		{
			pTargetInfo = TempArray.GetAt(astrIndex[i]);
			if (!pTargetInfo->GetOverLapDie()) 
			{
              nIndex = i;
			  break;
			}
		}
		pTargetInfo = TempArray.GetAt(astrIndex[nIndex]);
#endif

		pTargetInfo->SetRow(lRow);
		pTargetInfo->SetCol(lCol);
		Stack.Add(pTargetInfo);

		if ((lRow >= 0) && (lCol >= 0)) 
			aabDone[lRow][lCol] = TRUE;

		pTargetInfo->SetProcessed(TRUE);
		CompleteArray.Add(pTargetInfo);

		int nSize = (int)astrIndex.GetSize();
		for (i=0; i<nSize; i++) 
		{
			int j;
			int nMaxIndex = 0;
			for (j=1; j<astrIndex.GetSize(); j++) 
			{
				if (astrIndex[j] > astrIndex[nMaxIndex]) 
					nMaxIndex = j;
			}

			TempArray.RemoveAt(astrIndex[nMaxIndex]);
			astrIndex.RemoveAt(nMaxIndex);
		}
	}
}

void WAF_CPhysicalInformationMap::PreSortFindCluster(WAF_CPhysicalInformationArray& TempArray, 
													 WAF_CPhysicalInformationArray& CompleteArray, 
													 BOOL** aabDone, 
													 const long lRows, const long lCols, 
													 const long lX, const long lY, 
													 const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
													 const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
													 const long lStartRow, const long lStartCol)
{
	WAF_CPhysicalInformationArray Stack;

	PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lX, lY, ulDieSizeXTol, ulDieSizeYTol, lStartRow, lStartCol);

	while (Stack.GetSize() > 0)  
	{
		WAF_CPhysicalInformation* p = Stack.GetAt(0);
		Stack.RemoveAt(0);

		long lX = p->GetX();
		long lY = p->GetY();

		long lRow = p->GetRow();
		long lCol = p->GetCol();

		// Left
		if ((lRow >= 0) && (lCol > 0) && (!aabDone[lRow][lCol-1]))
		{
			long lLeftPitchX = p->GetX() + ulDiePitchX;
			long lLeftPitchY = p->GetY();

			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lLeftPitchX, lLeftPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow, lCol-1);
		}

		// Right
		if ((lRow >= 0) && (lCol >= 0) && (lCol < lCols) && (!aabDone[lRow][lCol+1]))
		{
			long lRightPitchX = p->GetX() - ulDiePitchX;
			long lRightPitchY = p->GetY();

			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lRightPitchX, lRightPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow, lCol+1);
		}

		// Up die
		if ((lRow > 0) && (lCol >= 0) && (!aabDone[lRow-1][lCol]))
		{
			long lUpPitchX = p->GetX() + m_lXOffset;
			long lUpPitchY = p->GetY() + ulDiePitchY;

			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lUpPitchX, lUpPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow-1, lCol);
		}

		// Down
		if ((lRow >= 0) && (lCol >= 0) && (lRow < lRows-1) && (!aabDone[lRow+1][lCol]))
		{
			long lDownPitchX = p->GetX() - m_lXOffset;
			long lDownPitchY = p->GetY() - ulDiePitchY;

			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lDownPitchX, lDownPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow+1, lCol);
		}

		// NE
		if ((lRow > 0) && (lCol >= 0) && (lCol < lCols) && (!aabDone[lRow-1][lCol+1]))
		{
			long lNEPitchX = p->GetX() - ulDiePitchX + m_lXOffset;
			long lNEPitchY = p->GetY() + ulDiePitchY;
			
			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lNEPitchX, lNEPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow-1, lCol+1);
		}

		// NW
		if ((lRow > 0) && (lCol > 0) && (!aabDone[lRow-1][lCol-1]))
		{
			long lNWPitchX = p->GetX() + ulDiePitchX + m_lXOffset;
			long lNWPitchY = p->GetY() + ulDiePitchY;

			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lNWPitchX, lNWPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow-1, lCol-1);
		}

		// SW
		if ((lRow >= 0) && (lCol > 0) && (lRow < lRows-1) && (!aabDone[lRow+1][lCol-1]))
		{
			long lSWPitchX = p->GetX() + ulDiePitchX - m_lXOffset;
			long lSWPitchY = p->GetY() - ulDiePitchY;

			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lSWPitchX, lSWPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow+1, lCol-1);
		}

		// SE
		if ((lRow >= 0) && (lCol >= 0) && (lRow < lRows-1) && (lCol < lCols) && (!aabDone[lRow+1][lCol+1]))
		{
			long lSEPitchX = p->GetX() - ulDiePitchX - m_lXOffset;
			long lSEPitchY = p->GetY() - ulDiePitchY;

			PreSortFindDie(TempArray, CompleteArray, Stack, aabDone, lSEPitchX, lSEPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow+1, lCol+1);
		}
	}

}			

// Construct the map by pitch
void WAF_CPhysicalInformationMap::ConstructMapByPitch(const long lRefPhyX, const long lRefPhyY, const long lRow, const long lCol, 
													  const unsigned long ulDiePitchX, const unsigned long ulDiePitchY)
{
	// Form the 2D array indices for fast access
	int nRows = 0;
	int nCols = 0;

	int i;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		
		long lCurCol = (lRefPhyX-p->GetX())/(long)ulDiePitchX + lCol;
		long lCurRow = (lRefPhyY-p->GetY())/(long)ulDiePitchY + lRow;

		p->SetRow(lCurRow);
		p->SetCol(lCurCol);
		p->SetProcessed(TRUE);
		
		if (i == 0) 
		{
			nRows = p->GetRow();
			nCols = p->GetCol();
		}
		else 
		{
			if (nRows < p->GetRow()) 
				nRows = p->GetRow();
			if (nCols < p->GetCol())
				nCols = p->GetCol();
		}
	}

	m_ulRows = nRows + 1; 
	m_ulCols = nCols + 1;

	m_aalMapIndex = new long*[m_ulRows];
	for (i=0; i<(int)m_ulRows; i++) 
	{
		m_aalMapIndex[i] = new long[m_ulCols];
		memset(m_aalMapIndex[i], 0xFF, sizeof(long)*m_ulCols);
	}

	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		
		if (p->GetProcessed()) 
		{
			if ((p->GetRow() < (int)m_ulRows) || (p->GetCol() < (int)m_ulCols))
			{
				m_aalMapIndex[p->GetRow()][p->GetCol()] = i;
			}
		}
	}
}

// Construct the map by pitch
void WAF_CPhysicalInformationMap::ConstructMapByPitchEx(const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
														const unsigned long ulDupXSize, const unsigned long ulDupYSize, 
														const BOOL bAppendDup)
{
	// Sort by X
	int i, j;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		int nMaxIndex = i;
		for (j=i+1; j<m_PhysicalInformationArray.GetSize(); j++) 
		{
			if (m_PhysicalInformationArray.GetAt(nMaxIndex)->GetX() < 
				m_PhysicalInformationArray.GetAt(j)->GetX())
			{
				nMaxIndex = j;
			}
		}

		WAF_CPhysicalInformation* pTemp = m_PhysicalInformationArray.GetAt(nMaxIndex);
		m_PhysicalInformationArray.SetAt(nMaxIndex, m_PhysicalInformationArray.GetAt(i));
		m_PhysicalInformationArray.SetAt(i, pTemp);
	}

	int nCurX;
	int nCurCol;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		if (i == 0) 
		{
			nCurCol = 0;
			nCurX = p->GetX();
			p->SetCol(0);
			p->SetProcessed(TRUE);
		}
		else 
		{
			int nNewCol = (int)((double)(nCurX-p->GetX())/(double)ulDiePitchX+0.5) + nCurCol;
			p->SetCol(nNewCol);
			p->SetProcessed(TRUE);
		}
	}

	// Sort by Y
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		int nMaxIndex = i;
		for (j=i+1; j<m_PhysicalInformationArray.GetSize(); j++) 
		{
			if (m_PhysicalInformationArray.GetAt(nMaxIndex)->GetY() < 
				m_PhysicalInformationArray.GetAt(j)->GetY())
			{
				nMaxIndex = j;
			}
		}

		WAF_CPhysicalInformation* pTemp = m_PhysicalInformationArray.GetAt(nMaxIndex);
		m_PhysicalInformationArray.SetAt(nMaxIndex, m_PhysicalInformationArray.GetAt(i));
		m_PhysicalInformationArray.SetAt(i, pTemp);
	}

	int nCurY;
	int nCurRow;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		if (i == 0) 
		{
			nCurRow = 0;
			nCurY = p->GetY();
			p->SetRow(0);
			p->SetProcessed(TRUE);
		}
		else 
		{
			int nNewRow = (int)((double)(nCurY-p->GetY())/(double)ulDiePitchY+0.5) + nCurRow;
			p->SetRow(nNewRow);
			p->SetProcessed(TRUE);
		}
	}

	// Put into 2D array
	int nRows = 0; 
	int nCols = 0;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		
		if (i == 0) 
		{
			nRows = p->GetRow();
			nCols = p->GetCol();
		}
		else 
		{
			if (nRows < p->GetRow()) 
				nRows = p->GetRow();
			if (nCols < p->GetCol())
				nCols = p->GetCol();
		}
	}

	m_ulRows = nRows + 50; 
	m_ulCols = nCols + 50;

	m_aalMapIndex = new long*[m_ulRows];
	for (i=0; i<(int)m_ulRows; i++) 
	{
		m_aalMapIndex[i] = new long[m_ulCols];
		memset(m_aalMapIndex[i], 0xFF, sizeof(long)*m_ulCols);
	}

	FILE *fp = NULL;
	FILE *fp1 = NULL; 
	errno_t nErr = 0;
	errno_t nErr1 = 0;
	if (m_bDebugOn) 
	{
		nErr = fopen_s(&fp, "Duplicate.txt", "w");
		nErr1 = fopen_s(&fp1, "Recover.txt", "w");
	}

	// Note: the physical information is sorted by Y
	CUIntArray aulArray;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		
		if (p->GetProcessed()) 
		{
			if ((p->GetRow() < (int)m_ulRows) || (p->GetCol() < (int)m_ulCols))
			{
				if (m_aalMapIndex[p->GetRow()][p->GetCol()] != 0xFFFFFFFF) 
				{
					WAF_CPhysicalInformation* q = m_PhysicalInformationArray.GetAt(m_aalMapIndex[p->GetRow()][p->GetCol()]);

					if ((abs(p->GetX()-q->GetX())<(int)ulDupXSize) && 
						(abs(p->GetY()-q->GetY())<(int)ulDupYSize))
					{
						// Duplicate die
						if (m_bDebugOn && (nErr == 0) && (fp != NULL))		//v4.51A20	//Klocwork
						{
							fprintf(fp, "%ld %ld %ld %ld %ld %ld\n", p->GetRow(), p->GetCol(), p->GetX(), 
								p->GetY(), q->GetX(), q->GetY());
						}

						aulArray.Add(i);
					}
					else 
					{
						int nCol = p->GetCol()+1;
						while (nCol < (int)m_ulCols)
						{
							if (m_aalMapIndex[p->GetRow()][nCol] >= 0)
							{
								q = m_PhysicalInformationArray.GetAt(m_aalMapIndex[p->GetRow()][nCol]);
								if ((abs(p->GetX()-q->GetX())<(int)ulDupXSize) && 
									(abs(p->GetY()-q->GetY())<(int)ulDupYSize))
								{
									// Duplicate die
									if (m_bDebugOn && (nErr == 0) && (fp != NULL))
									{
										fprintf(fp, "%ld %ld %ld %ld %ld %ld\n", p->GetRow(), p->GetCol(), p->GetX(), 
											p->GetY(), q->GetX(), q->GetY());
									}

									aulArray.Add(i);
									break;
								}
								nCol++;
							}
							else 
							{
								p->SetCol(nCol);
								m_aalMapIndex[p->GetRow()][nCol] = i;

								if (m_bDebugOn && (nErr1 == 0) && (fp1 != NULL))
								{
									fprintf(fp1, "%ld %ld %ld %ld %ld %ld\n", p->GetRow(), p->GetCol(), p->GetX(),	//v4.51A20	//Klocwork
										p->GetY(), q->GetX(), q->GetY());
								}

								break;
							}
						}

						if (nCol >= (int)m_ulCols)
						{
							aulArray.Add(i);
						}
					}
				}
				else
				{
					m_aalMapIndex[p->GetRow()][p->GetCol()] = i;
				}
			}
		}
	}

	if (bAppendDup) 
	{
		int nTempRow = nRows+1;
		int nTempCol = 0;
		for (i=0; i<aulArray.GetSize(); i++) 
		{
			UINT nIndex = aulArray[i];
			WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(nIndex);

			p->SetRow(nTempRow);
			p->SetCol(nTempCol);
			m_aalMapIndex[nTempRow][nTempCol] = nIndex;
			nTempCol++;
			if (nTempCol >= (int)m_ulCols)
			{
				nTempCol = 0;
				nTempRow++;
			}

			if (nTempRow >= (int)m_ulRows)
				break;
		}
	}

	if (m_bDebugOn)
	{
		if (fp != NULL)
		{
			fclose(fp);
		}

		if (fp1 != NULL)
		{
			fclose(fp1);
		}
	}
}


// Construct the map using physical location
void WAF_CPhysicalInformationMap::ConstructMap(const long lRefPhyX, const long lRefPhyY, const long lRow, const long lCol, 
											   const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
											   const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
											   const BOOL bPreSorted, const int nMaxIterateCount, const int nMaxAllowLeft, 
											   const int nMaxAllowPitch)
{
	// Clear the indices
	if (m_aalMapIndex != NULL) 
	{
		int i;
		for (i=0; i<(int)m_ulRows; i++) 
		{ 
			delete[] m_aalMapIndex[i];
		}
		delete[] m_aalMapIndex;
	}
	m_aalMapIndex = NULL;

	// Temporary indices
	int i;
	WAF_CPhysicalInformationArray CompleteArray;
	WAF_CPhysicalInformationArray TempArray;

	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		TempArray.Add(m_PhysicalInformationArray.GetAt(i));
	}

	// Find the physical size of the wafer
	long lMinPhyX=0, lMinPhyY=0, lMaxPhyX=0, lMaxPhyY=0;	//Klocwork
	for (i=0; i<TempArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = TempArray.GetAt(i);
		if (i == 0) 
		{
			lMinPhyX = p->GetX();
			lMinPhyY = p->GetY();
			lMaxPhyX = p->GetX();
			lMaxPhyY = p->GetY();
		}
		else 
		{
			if (lMinPhyX > p->GetX()) 
				lMinPhyX = p->GetX();
			if (lMaxPhyX < p->GetX())
				lMaxPhyX = p->GetX();
			if (lMinPhyY > p->GetY()) 
				lMinPhyY = p->GetY();
			if (lMaxPhyY < p->GetY())
				lMaxPhyY = p->GetY();
		}
	}

	if ((lMaxPhyX == lMinPhyX) || (lMaxPhyY == lMinPhyY))
	{
		return;
	}

	int nEdge = 50;
	int nLftSide = (lMaxPhyX - lRefPhyX) / ((int)ulDiePitchX);
	int nRhtSide = (lRefPhyX - lMinPhyX) / ((int)ulDiePitchX);
	int nTopSide = (lRefPhyY - lMinPhyY) / ((int)ulDiePitchY);
	int nBtmSide = (lMaxPhyY - lRefPhyY) / ((int)ulDiePitchY);
	if( lCol>nLftSide )
		nLftSide = lCol;
	if( lRow>nTopSide )
		nTopSide = lRow;

	int nCols =  nEdge + nLftSide + nRhtSide + nEdge;
	int nRows =  nEdge + nTopSide + nBtmSide + nEdge;

	nCols = nCols + (abs(m_lXOffset) * nRows) / (int)ulDiePitchX;

//	int nCols = (lMaxPhyX - lMinPhyX) / ulDiePitchX + 100;
//	int nRows = (lMaxPhyY - lMinPhyY) / ulDiePitchY + 100;

	// Done array
	BOOL** aabDone = new BOOL*[nRows];
	for (i=0; i<nRows; i++) 
	{
		aabDone[i] = new BOOL[nCols];
		memset(aabDone[i], 0, nCols*sizeof(BOOL));
	}

	long lStartRow = lRow; 
	long lStartCol = lCol;
	long lStartPhyX = lRefPhyX;
	long lStartPhyY = lRefPhyY;

	// Trim the TempArray, remove all isolated dice
	int nTolPitchY = 2*ulDiePitchY;
	int nTolPitchX = 2*ulDiePitchX;

	if (!m_bKeepIsolatedDice)	//	m_bScan2PrRecord
	{
		CUIntArray aunDeleteArray;
		for (i=0; i<TempArray.GetSize(); i++) 
		{
			WAF_CPhysicalInformation* p = TempArray.GetAt(i);
			long lPhyX = p->GetX();
			long lPhyY = p->GetY();

			BOOL bFound = FALSE;
			int j = i-1;
			while (j >= 0) 
			{
				WAF_CPhysicalInformation* q = TempArray.GetAt(j);
				if (abs(lPhyY-q->GetY()) >= nTolPitchY)
					break;

				if (abs(lPhyX-q->GetX()) < nTolPitchX)
				{
					bFound = TRUE;
					break;
				}
				j--;
			}

			j = i+1;
			while ((!bFound) && (j < TempArray.GetSize()))
			{
				WAF_CPhysicalInformation* q = TempArray.GetAt(j);
				if (abs(lPhyY-q->GetY()) >= nTolPitchY)
					break;

				if (abs(lPhyX-q->GetX()) < nTolPitchX)
				{
					bFound = TRUE;
					break;
				}
				j++;
			}

			if (!bFound)
				aunDeleteArray.Add(i);
		}
		for (i=(int)aunDeleteArray.GetSize()-1; i>=0; i--) 
		{
			TempArray.RemoveAt(aunDeleteArray[i]);
		}
	}

	WAF_CPhysicalInformationArray EdgeArray;
	int nLastSize = 0;
	BOOL bStartReady = TRUE;

	if (m_bDebugOn)
	{
		FILE* fp = NULL;
		errno_t nErr = fopen_s(&fp, "List.txt", "w");
		if ((nErr == 0) && (fp != NULL))
		{
			for (i=0; i<TempArray.GetSize(); i++) 
			{
				if (fp != NULL)		//v4.51A20	//Klocwork
				{
					fprintf(fp, "%d\t%ld\t%ld\n", i, TempArray.GetAt(i)->GetX(), TempArray.GetAt(i)->GetY());
				}
			}
			fclose(fp);
		}
	}

	int nIterateCount = 0;
	// Start constructing
	do 
	{
		// Find the cluster
		if (bStartReady) 
		{
			if (bPreSorted) 
			{
				PreSortFindCluster(TempArray, CompleteArray, aabDone, nRows, nCols, lStartPhyX, lStartPhyY, ulDieSizeXTol, ulDieSizeYTol, 
					ulDiePitchX, ulDiePitchY, lStartRow, lStartCol);
			}
			else 
			{
				FindCluster(TempArray, CompleteArray, aabDone, nRows, nCols, lStartPhyX, lStartPhyY, ulDieSizeXTol, ulDieSizeYTol, 
					ulDiePitchX, ulDiePitchY, lStartRow, lStartCol);
			}
		}

		// Find minimum distance between clusters
		// Find the maximum physical position in TempArray
		BOOL bTooFar = FALSE;
		for (i=0; i<TempArray.GetSize(); i++) 
		{
			WAF_CPhysicalInformation* p = TempArray.GetAt(i);
			if (i == 0) 
			{
				lMaxPhyX = p->GetX();
				lMaxPhyY = p->GetY();
			}
			else 
			{
				if (lMaxPhyX < p->GetX())
					lMaxPhyX = p->GetX();
				if ((bPreSorted) && ((lMaxPhyY-p->GetY())>(int)ulDieSizeYTol))
					break;
				else if (lMaxPhyY < p->GetY())
					lMaxPhyY = p->GetY();
			}
		}

		// Find the minimum physical position in TempArray
		for (i=(int)TempArray.GetSize()-1; i>=0; i--) 
		{
			WAF_CPhysicalInformation* p = TempArray.GetAt(i);
			if (i == TempArray.GetSize()-1) 
			{
				lMinPhyX = p->GetX();
				lMinPhyY = p->GetY();
			}
			else 
			{
				if (lMinPhyX > p->GetX())
					lMinPhyX = p->GetX();
				if ((bPreSorted) && ((p->GetY()-lMinPhyY)>(int)ulDieSizeYTol))
					break;
				else if (lMinPhyY > p->GetY())
					lMinPhyY = p->GetY();
			}
		}

		// Get the edge die first
		for (i=nLastSize; i<CompleteArray.GetSize(); i++) 
		{
			WAF_CPhysicalInformation* p = CompleteArray.GetAt(i);
			int nCurrRow = p->GetRow();
			int nCurrCol = p->GetCol();
 
			// 1. The die is far away from the min side, and nearer die is not processed. 
			//            -> if die is far away from min y side and its upper neighbor is not processed
			//            -> if die is far away from min x side and its left neighbor is not processed
			// 2. The die is far away from the max side, and nearer die is not processed.
			//            -> if die is far away from max y side and its lower neighbor is not processed
			//            -> if die is far away from max x side and its right neighbor is not processed
			// 3. The die is near the min side, but its surrounding dice are not processed. Strange, include it
			// 4. The die is near the max side, but its surrounding dice are not processed. Strange, include it
			// Illustration:   C   Min  Max - Current die is at the left side. 
			//                              - We use the formula Max - C and check if right of C is not processed
			//                 Min  Max  C  - Current die is at the right side.
			//                              - We use the formula C - Min and check if left of C is not processed
			BOOL bFound = FALSE;
			if (((p->GetY()-lMinPhyY) > (int)ulDieSizeYTol) && (nCurrRow > 0) && (!aabDone[nCurrRow-1][nCurrCol]))
				bFound = TRUE;
			else if (((p->GetX()-lMinPhyX) > (int)ulDieSizeXTol) && (nCurrCol > 0) && (!aabDone[nCurrRow][nCurrCol-1]))
				bFound = TRUE;
			else if (((lMaxPhyY-p->GetY()) > (int)ulDieSizeYTol) && (nCurrRow < nRows-1) && (!aabDone[nCurrRow+1][nCurrCol]))
				bFound = TRUE;
			else if (((lMaxPhyX-p->GetX()) > (int)ulDieSizeXTol) && (nCurrCol < nCols-1) && (!aabDone[nCurrRow][nCurrCol+1]))
				bFound = TRUE;
			if( bFound==FALSE && m_bScan2PrRecord==FALSE )
			{
				if (((p->GetY()-lMinPhyY) < (int)ulDieSizeYTol) && (nCurrRow > 0) && (!aabDone[nCurrRow-1][nCurrCol]) && 
					((p->GetX()-lMinPhyX) < (int)ulDieSizeXTol) && (nCurrCol > 0) && (!aabDone[nCurrRow][nCurrCol-1]))
					bFound = TRUE;
				else if (((p->GetY()-lMinPhyY) < (int)ulDieSizeYTol) && (nCurrRow > 0) && (!aabDone[nCurrRow-1][nCurrCol]) && 
					((lMaxPhyX-p->GetX()) < (int)ulDieSizeXTol) && (nCurrCol < nCols-1) && (!aabDone[nCurrRow][nCurrCol+1]))
					bFound = TRUE;
				else if (((lMaxPhyY-p->GetY()) < (int)ulDieSizeYTol) && (nCurrRow < nRows-1) && (!aabDone[nCurrRow+1][nCurrCol]) &&
					((lMaxPhyX-p->GetX()) < (int)ulDieSizeXTol) && (nCurrCol < nCols-1) && (!aabDone[nCurrRow][nCurrCol+1]))
					bFound = TRUE;
				else if (((lMaxPhyY-p->GetY()) < (int)ulDieSizeYTol) && (nCurrRow < nRows-1) && (!aabDone[nCurrRow+1][nCurrCol]) &&
					((p->GetX()-lMinPhyX) > (int)ulDieSizeXTol) && (nCurrCol > 0) && (!aabDone[nCurrRow][nCurrCol-1]))
					bFound = TRUE;
			}

			if (bFound) 
				EdgeArray.Add(p);
		}
		nLastSize = (int)CompleteArray.GetSize();

		// Compare the distance between the outliners with the edges
		int nMinIndexT	= 0;
		int nMinIndexC	= 0;
		int nMinDist	= 0;	//Klocwork 
		for (i=0; i<TempArray.GetSize(); i++) 
		{ 
			int j;
			for (j=0; j<EdgeArray.GetSize(); j++) 
			{
				int nDist = abs(TempArray.GetAt(i)->GetRow()-EdgeArray.GetAt(j)->GetRow())+
					abs(TempArray.GetAt(i)->GetCol()-EdgeArray.GetAt(j)->GetCol());

				if ((i==0) && (j==0))
					nMinDist = nDist;
				else 
				{
					if (nMinDist > nDist)
					{
						nMinIndexT = i;
						nMinIndexC = j;
					}
				}
			}
		}

		if (m_bDebugOn)
		{
			FILE* fp = NULL;
			errno_t nErr = fopen_s(&fp, "Sorted.txt", "w");
			if ((nErr == 0) && (fp != NULL))
			{
				int m, n;
				for (m=0; m<nRows; m++) 
				{
					for (n=0; n<nCols; n++) 
					{
						if (fp != NULL)		//v4.51A20	//Klocwork
						{
							if (aabDone[m][n]) 
								fprintf(fp, "X");
							else 
								fprintf(fp, " ");
						}
					}

					if (fp != NULL)			//v4.51A20	//Klocwork
						fprintf(fp, "\n");
				}
				fclose(fp);
			}
		}

		// Another area found. Construct the cluster on this area
		int nXJumpPitch; 
		int nYJumpPitch;
		if ((TempArray.GetSize() > 0) && (EdgeArray.GetSize() > 0))
		{
			WAF_CPhysicalInformation* pT = TempArray.GetAt(nMinIndexT);
			WAF_CPhysicalInformation* pC = EdgeArray.GetAt(nMinIndexC);

			nXJumpPitch = abs(pC->GetX()-pT->GetX());
			nYJumpPitch = abs(pC->GetY()-pT->GetY());

			if ((nMaxAllowPitch >= 0) && ((nXJumpPitch > nMaxAllowPitch*(long)ulDiePitchX) 
				|| (nYJumpPitch > nMaxAllowPitch*(long)ulDiePitchY)))
			{
				bTooFar = TRUE;
			}

			lStartRow = (pC->GetY()-pT->GetY())/(long)ulDiePitchY + pC->GetRow();
			lStartCol = (pC->GetX()-pT->GetX())/(long)ulDiePitchX + pC->GetCol() + 
				(lStartRow - pC->GetRow()) * m_lXOffset / (long)ulDiePitchX; 

			lStartPhyX = pT->GetX();
			lStartPhyY = pT->GetY();

			if ((lStartRow < 0) || (lStartCol < 0)) 
			{
				TempArray.RemoveAt(nMinIndexT);
				bStartReady = FALSE;
			}
			else 
			{
				bStartReady = TRUE;
			}
		}
		else 
		{
			bTooFar = TRUE;
		}

		nIterateCount++;
		if ((nMaxIterateCount >= 0) && (nIterateCount >= nMaxIterateCount))
			break;
		if ((nMaxAllowLeft >= 0) && (TempArray.GetSize() <= nMaxAllowLeft))
			break;
		if (bTooFar) 
			break;
	} while ((TempArray.GetSize() > 0) && (CompleteArray.GetSize() > 0));

	for (i=0; i<nRows; i++) 
	{
		delete[] aabDone[i];
	}
	delete[] aabDone;

	// Form the 2D array indices for fast access
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		if (i == 0) 
		{
			nRows = p->GetRow();
			nCols = p->GetCol();
		}
		else 
		{
			if (nRows < p->GetRow()) 
				nRows = p->GetRow();
			if (nCols < p->GetCol())
				nCols = p->GetCol();
		}
	}

	m_ulRows = nRows + 1; 
	m_ulCols = nCols + 1;

	m_aalMapIndex = new long*[m_ulRows];
	for (i=0; i<(int)m_ulRows; i++) 
	{
		m_aalMapIndex[i] = new long[m_ulCols];
		memset(m_aalMapIndex[i], 0xFF, sizeof(long)*m_ulCols);
	}

	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		if (p->GetProcessed()) 
		{
			if ((p->GetRow() < (int)m_ulRows) || (p->GetCol() < (int)m_ulCols))
				m_aalMapIndex[p->GetRow()][p->GetCol()] = i;
		}
	}
}

// Construct the map using physical location
void WAF_CPhysicalInformationMap::ConstructMap(const CDWordArray& alRefPhyX, const CDWordArray& alRefPhyY, 
											   const CDWordArray& alRow, const CDWordArray& alCol, 
											   const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
											   const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
											   const BOOL bPreSorted, const int nMaxIterateCount, const int nMaxAllowLeft, 
											   const int nMaxAllowPitch)
{
	// Clear the indices
	if (m_aalMapIndex != NULL) 
	{
		int i;
		for (i=0; i<(int)m_ulRows; i++) 
		{
			delete[] m_aalMapIndex[i];
		}
		delete[] m_aalMapIndex;
	}
	m_aalMapIndex = NULL;

	if( ulDiePitchX==0 || ulDiePitchY==0 )
	{
		return ;
	}

	// Temporary indices
	int i;
	WAF_CPhysicalInformationArray CompleteArray;
	WAF_CPhysicalInformationArray TempArray;

	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		TempArray.Add(m_PhysicalInformationArray.GetAt(i));
	}

	// Find the physical size of the wafer
	long lMinPhyX=0, lMinPhyY=0, lMaxPhyX=0, lMaxPhyY=0;	//Klocwork
	for (i=0; i<TempArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = TempArray.GetAt(i);
		if (i == 0) 
		{
			lMinPhyX = p->GetX();
			lMinPhyY = p->GetY();
			lMaxPhyX = p->GetX();
			lMaxPhyY = p->GetY();
		}
		else 
		{
			if (lMinPhyX > p->GetX()) 
				lMinPhyX = p->GetX();
			if (lMaxPhyX < p->GetX())
				lMaxPhyX = p->GetX();
			if (lMinPhyY > p->GetY()) 
				lMinPhyY = p->GetY();
			if (lMaxPhyY < p->GetY())
				lMaxPhyY = p->GetY();
		}
	}

	if ((lMaxPhyX == lMinPhyX) || (lMaxPhyY == lMinPhyY))
	{
		return;
	}

//	int nWidth = (lMaxPhyX - lMinPhyX) / ulDiePitchX + 100;
//	int nHeight= (lMaxPhyY - lMinPhyY) / ulDiePitchY + 100;

	int nEdge = 50;
	long lRow = alRow[0]; 
	long lCol = alCol[0];
	long lRefPhyX = alRefPhyX[0];
	long lRefPhyY = alRefPhyY[0];
	int nLftSide = (lMaxPhyX - lRefPhyX) / ((int)ulDiePitchX);
	int nRhtSide = (lRefPhyX - lMinPhyX) / ((int)ulDiePitchX);
	int nTopSide = (lMaxPhyY - lRefPhyY) / ((int)ulDiePitchY);
	int nBtmSide = (lRefPhyY - lMinPhyY) / ((int)ulDiePitchY);
	if( lCol>nLftSide )
		nLftSide = lCol;
	if( lRow>nTopSide )
		nTopSide = lRow;
	int nCols =  nEdge + nLftSide + nRhtSide + nEdge;
	int nRows =  nEdge + nTopSide + nBtmSide + nEdge;

	nCols = nCols + (abs(m_lXOffset) * nRows) / (int)ulDiePitchX;

	// Done array
	BOOL** aabDone = new BOOL*[nRows];
	for (i=0; i<nRows; i++) 
	{
		aabDone[i] = new BOOL[nCols];
		memset(aabDone[i], 0, nCols*sizeof(BOOL));
	}

	// Trim the TempArray, remove all isolated dice
	int nTolPitchY = 2*ulDiePitchY;
	int nTolPitchX = 2*ulDiePitchX;

	//delete isolated dice
	if (!m_bKeepIsolatedDice)	//	m_bScan2PrRecord
	{
		CUIntArray aunDeleteArray;
		for (i=0; i<TempArray.GetSize(); i++) 
		{
			WAF_CPhysicalInformation* p = TempArray.GetAt(i);
			long lPhyX = p->GetX();
			long lPhyY = p->GetY();

			BOOL bFound = FALSE;
			int j = i-1;
			while (j >= 0) 
			{
				WAF_CPhysicalInformation* q = TempArray.GetAt(j);
				if (abs(lPhyY-q->GetY()) >= nTolPitchY)
					break;

				if (abs(lPhyX-q->GetX()) < nTolPitchX)
				{
					bFound = TRUE;
					break;
				}
				j--;
			}

			j = i+1;
			while ((!bFound) && (j < TempArray.GetSize()))
			{
				WAF_CPhysicalInformation* q = TempArray.GetAt(j);
				if (abs(lPhyY-q->GetY()) >= nTolPitchY)
					break;

				if (abs(lPhyX-q->GetX()) < nTolPitchX)
				{
					bFound = TRUE;
					break;
				}
				j++;
			}

			if (!bFound)
				aunDeleteArray.Add(i);
		}
		for (i=(int)aunDeleteArray.GetSize()-1; i>=0; i--) 
		{
			TempArray.RemoveAt(aunDeleteArray[i]);
		}
	}

	WAF_CPhysicalInformationArray EdgeArray;
	int nLastSize = 0;
	BOOL bStartReady = TRUE;

	int nIterateCount = 0;

	if ((alRow.GetSize() == 0) || (alCol.GetSize() == 0) ||
		(alRefPhyX.GetSize() == 0) || (alRefPhyY.GetSize() == 0))
	{
		//Klocwork
		for (i=0; i<nRows; i++) 
		{
			delete[] aabDone[i];
		}
		delete[] aabDone;
		return;
	}

	int nRefCount = 0;
	long lStartRow = alRow[nRefCount]; 
	long lStartCol = alCol[nRefCount];
	long lStartPhyX = alRefPhyX[nRefCount];
	long lStartPhyY = alRefPhyY[nRefCount];
	nRefCount++;

	// Start constructing
	do 
	{
		// Find the cluster
		if (bStartReady) 
		{
			if (bPreSorted) 
			{
				PreSortFindCluster(TempArray, CompleteArray, aabDone, nRows, nCols, lStartPhyX, lStartPhyY, ulDieSizeXTol, ulDieSizeYTol, 
					ulDiePitchX, ulDiePitchY, lStartRow, lStartCol);
			}
			else 
			{
				FindCluster(TempArray, CompleteArray, aabDone, nRows, nCols, lStartPhyX, lStartPhyY, ulDieSizeXTol, ulDieSizeYTol, 
					ulDiePitchX, ulDiePitchY, lStartRow, lStartCol);
			}
		}

		BOOL bTooFar = FALSE;
		if ((nRefCount < alRow.GetSize()) && (nRefCount < alCol.GetSize()) && 
			(nRefCount < alRefPhyX.GetSize()) && (nRefCount < alRefPhyY.GetSize()))
		{
			lStartRow = alRow[nRefCount]; 
			lStartCol = alCol[nRefCount];
			lStartPhyX = alRefPhyX[nRefCount];
			lStartPhyY = alRefPhyY[nRefCount];
			nRefCount++;
		}
		else 
		{
			// Find minimum distance between clusters
			// Find the maximum physical position in TempArray
			for (i=0; i<TempArray.GetSize(); i++) 
			{
				WAF_CPhysicalInformation* p = TempArray.GetAt(i);
				if (i == 0) 
				{
					lMaxPhyX = p->GetX();
					lMaxPhyY = p->GetY();
				}
				else 
				{
					if (lMaxPhyX < p->GetX())
						lMaxPhyX = p->GetX();
					if ((bPreSorted) && ((lMaxPhyY-p->GetY())>(int)ulDieSizeYTol))
						break;
					else if (lMaxPhyY < p->GetY())
						lMaxPhyY = p->GetY();
				}
			}

			// Find the minimum physical position in TempArray
			for (i=(int)TempArray.GetSize()-1; i>=0; i--) 
			{
				WAF_CPhysicalInformation* p = TempArray.GetAt(i);
				if (i == TempArray.GetSize()-1) 
				{
					lMinPhyX = p->GetX();
					lMinPhyY = p->GetY();
				}
				else 
				{
					if (lMinPhyX > p->GetX())
						lMinPhyX = p->GetX();
					if ((bPreSorted) && ((p->GetY()-lMinPhyY)>(int)ulDieSizeYTol))
						break;
					else if (lMinPhyY > p->GetY())
						lMinPhyY = p->GetY();
				}
			}

			// Get the edge die first
			for (i=nLastSize; i<CompleteArray.GetSize(); i++) 
			{
				WAF_CPhysicalInformation* p = CompleteArray.GetAt(i);
				int nCurrRow = p->GetRow();
				int nCurrCol = p->GetCol();
	 
				// 1. The die is far away from the min side, and nearer die is not processed. 
				//            -> if die is far away from min y side and its upper neighbor is not processed
				//            -> if die is far away from min x side and its left neighbor is not processed
				// 2. The die is far away from the max side, and nearer die is not processed.
				//            -> if die is far away from max y side and its lower neighbor is not processed
				//            -> if die is far away from max x side and its right neighbor is not processed
				// 3. The die is near the min side, but its surrounding dice are not processed. Strange, include it
				// 4. The die is near the max side, but its surrounding dice are not processed. Strange, include it
				// Illustration:   C   Min  Max - Current die is at the left side. 
				//                              - We use the formula Max - C and check if right of C is not processed
				//                 Min  Max  C  - Current die is at the right side.
				//                              - We use the formula C - Min and check if left of C is not processed
				BOOL bFound = FALSE;
				if (((p->GetY()-lMinPhyY) > (int)ulDieSizeYTol) && (nCurrRow > 0) && (!aabDone[nCurrRow-1][nCurrCol]))
					bFound = TRUE;
				else if (((p->GetX()-lMinPhyX) > (int)ulDieSizeXTol) && (nCurrCol > 0) && (!aabDone[nCurrRow][nCurrCol-1]))
					bFound = TRUE;
				else if (((lMaxPhyY-p->GetY()) > (int)ulDieSizeYTol) && (nCurrRow < nRows-1) && (!aabDone[nCurrRow+1][nCurrCol]))
					bFound = TRUE;
				else if (((lMaxPhyX-p->GetX()) > (int)ulDieSizeXTol) && (nCurrCol < nCols-1) && (!aabDone[nCurrRow][nCurrCol+1]))
					bFound = TRUE;
				if( bFound==FALSE && m_bScan2PrRecord==FALSE )
				{
					if (((p->GetY()-lMinPhyY) < (int)ulDieSizeYTol) && (nCurrRow > 0) && (!aabDone[nCurrRow-1][nCurrCol]) && 
						((p->GetX()-lMinPhyX) < (int)ulDieSizeXTol) && (nCurrCol > 0) && (!aabDone[nCurrRow][nCurrCol-1]))
						bFound = TRUE;
					else if (((p->GetY()-lMinPhyY) < (int)ulDieSizeYTol) && (nCurrRow > 0) && (!aabDone[nCurrRow-1][nCurrCol]) && 
						((lMaxPhyX-p->GetX()) < (int)ulDieSizeXTol) && (nCurrCol < nCols-1) && (!aabDone[nCurrRow][nCurrCol+1]))
						bFound = TRUE;
					else if (((lMaxPhyY-p->GetY()) < (int)ulDieSizeYTol) && (nCurrRow < nRows-1) && (!aabDone[nCurrRow+1][nCurrCol]) &&
						((lMaxPhyX-p->GetX()) < (int)ulDieSizeXTol) && (nCurrCol < nCols-1) && (!aabDone[nCurrRow][nCurrCol+1]))
						bFound = TRUE;
					else if (((lMaxPhyY-p->GetY()) < (int)ulDieSizeYTol) && (nCurrRow < nRows-1) && (!aabDone[nCurrRow+1][nCurrCol]) &&
						((p->GetX()-lMinPhyX) > (int)ulDieSizeXTol) && (nCurrCol > 0) && (!aabDone[nCurrRow][nCurrCol-1]))
						bFound = TRUE;
				}

				if (bFound) 
					EdgeArray.Add(p);
			}
			nLastSize = (int)CompleteArray.GetSize();

			// Compare the distance between the outliners with the edges
			int nMinIndexT	= 0;
			int nMinIndexC	= 0;
			int nMinDist	= 0;	//Klocwork 
			for (i=0; i<TempArray.GetSize(); i++) 
			{ 
				int j;
				for (j=0; j<EdgeArray.GetSize(); j++) 
				{
					int nDist = abs(TempArray.GetAt(i)->GetRow()-EdgeArray.GetAt(j)->GetRow())+
						abs(TempArray.GetAt(i)->GetCol()-EdgeArray.GetAt(j)->GetCol());

					if ((i==0) && (j==0))
						nMinDist = nDist;
					else 
					{
						if (nMinDist > nDist)
						{
							nMinIndexT = i;
							nMinIndexC = j;
						}
					}
				}
			}

			if (m_bDebugOn)
			{
				FILE* fp = NULL;
				errno_t nErr = fopen_s(&fp, "Sorted.txt", "w");
				if ((nErr == 0) && (fp != NULL))
				{
					int m, n;
					for (m=0; m<nRows; m++) 
					{
						for (n=0; n<nCols; n++) 
						{
							if (aabDone[m][n]) 
								fprintf(fp, "X");
							else 
								fprintf(fp, " ");
						}
						fprintf(fp, "\n");
					}
					fclose(fp);
				}
			}

			// Another area found. Construct the cluster on this area
			int nXJumpPitch; 
			int nYJumpPitch;
			if ((TempArray.GetSize() > 0) && (CompleteArray.GetSize() > 0))
			{
				WAF_CPhysicalInformation* pT = TempArray.GetAt(nMinIndexT);
				WAF_CPhysicalInformation* pC = EdgeArray.GetAt(nMinIndexC);

				nXJumpPitch = abs(pC->GetX()-pT->GetX());
				nYJumpPitch = abs(pC->GetY()-pT->GetY());

				if ((nMaxAllowPitch >= 0) && ((nXJumpPitch > nMaxAllowPitch*(long)ulDiePitchX) 
					|| (nYJumpPitch > nMaxAllowPitch*(long)ulDiePitchY)))
				{
					bTooFar = TRUE;
				}

				nXJumpPitch = pC->GetX()-pT->GetX();
				nYJumpPitch = pC->GetY()-pT->GetY();
				if( nMaxAllowPitch!=2 )
				{
					lStartRow = (nYJumpPitch)/(long)ulDiePitchY + pC->GetRow();
					lStartCol = (nXJumpPitch)/(long)ulDiePitchX + pC->GetCol();
				}
				else
				{
					if( nYJumpPitch>=0 )
						lStartRow = (nYJumpPitch + ulDieSizeYTol)/(long)ulDiePitchY + pC->GetRow();
					else
						lStartRow = (nYJumpPitch - ulDieSizeYTol)/(long)ulDiePitchY + pC->GetRow();
					if( nXJumpPitch>=0 )
						lStartCol = (nXJumpPitch + ulDieSizeXTol)/(long)ulDiePitchX + pC->GetCol();
					else
						lStartCol = (nXJumpPitch - ulDieSizeXTol)/(long)ulDiePitchX + pC->GetCol();
				}

				if( m_lXOffset!=0 )
				{
					lStartCol += ((lStartRow - pC->GetRow()) * m_lXOffset / (long)ulDiePitchX);
				}

				lStartPhyX = pT->GetX();
				lStartPhyY = pT->GetY();

				if ((lStartRow < 0) || (lStartCol < 0)) 
				{
					TempArray.RemoveAt(nMinIndexT);
					bStartReady = FALSE;
				}
				else 
				{
					bStartReady = TRUE;
				}
			}
		}
		nIterateCount++;
		if ((nMaxIterateCount >= 0) && (nIterateCount >= nMaxIterateCount))
			break;
		if ((nMaxAllowLeft >= 0) && (TempArray.GetSize() <= nMaxAllowLeft))
			break;
		if (bTooFar) 
			break;
	} while ((TempArray.GetSize() > 0) && (CompleteArray.GetSize() > 0));

	for (i=0; i<nRows; i++) 
	{
		delete[] aabDone[i];
	}
	delete[] aabDone;

	// Form the 2D array indices for fast access
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		if (i == 0) 
		{
			nRows = p->GetRow();
			nCols = p->GetCol();
		}
		else 
		{
			if (nRows < p->GetRow()) 
				nRows = p->GetRow();
			if (nCols < p->GetCol())
				nCols = p->GetCol();
		}
	}

	m_ulRows = nRows + 1; 
	m_ulCols = nCols + 1;

	m_aalMapIndex = new long*[m_ulRows];
	for (i=0; i<(int)m_ulRows; i++) 
	{
		m_aalMapIndex[i] = new long[m_ulCols];
		memset(m_aalMapIndex[i], 0xFF, sizeof(long)*m_ulCols);
	}

	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(i);
		if (p->GetProcessed()) 
		{
			if ((p->GetRow() < (int)m_ulRows) || (p->GetCol() < (int)m_ulCols))
				m_aalMapIndex[p->GetRow()][p->GetCol()] = i;
		}
	}

}

// Getting physical location
BOOL WAF_CPhysicalInformationMap::GetInfo(const unsigned long ulRow, const unsigned long ulCol, long& lX, long& lY, 
										  LONG &lDataListIndex, BOOL &bOverLapDie)
{
	lX = 0;
	lY = 0;

	if ((m_aalMapIndex != NULL) && (ulRow < m_ulRows) && (ulCol < m_ulCols))
	{
		int nIndex = m_aalMapIndex[ulRow][ulCol];
		if (nIndex >= 0) 
		{
			WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(nIndex);
			lX = p->GetX();
			lY = p->GetY();
			lDataListIndex = p->GetListIndex();
			bOverLapDie = p->GetOverLapDie();

			return TRUE;
		}
	}
	return FALSE;
}

VOID WAF_CPhysicalInformationMap::GetDimension(ULONG &ulRowMax, ULONG &ulColMax)
{
	ulRowMax = m_ulRows;
	ulColMax = m_ulCols;
}

/////////////////////////////////////////////////////////////////////////
// These functions are non-optimized
// Add die 
void WAF_CPhysicalInformationMap::Add(const long lX, const long lY, const LONG lIndex, const BOOL bOverLap)
{
	m_PhysicalInformationArray.Add(new WAF_CPhysicalInformation(lX, lY, lIndex, bOverLap));
}

// Find the die
void WAF_CPhysicalInformationMap::FindDie(WAF_CPhysicalInformationArray& TempArray, 
										  WAF_CPhysicalInformationArray& CompleteArray, 
										  WAF_CPhysicalInformationArray& Stack, 
										  BOOL** aabDone, 
										  const long lX, const long lY, 
										  const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
										  const long lRow, const long lCol)
{
	// Using linear search to find the die
	int i;
	for (i=0; i<TempArray.GetSize(); i++) 
	{
		WAF_CPhysicalInformation* p = TempArray.GetAt(i);
		if ((abs(lX - p->GetX()) < (int)ulDieSizeXTol) && (abs(lY-p->GetY()) < (int)ulDieSizeYTol))
		{
			Stack.Add(p);

			p->SetRow(lRow);
			p->SetCol(lCol);

			if ((lRow >= 0) && (lCol >= 0)) 
				aabDone[lRow][lCol] = TRUE;

			TempArray.RemoveAt(i);
			CompleteArray.Add(p);

			break;
		}
	}
}

// Construct the cluster
void WAF_CPhysicalInformationMap::FindCluster(WAF_CPhysicalInformationArray& TempArray, 
											  WAF_CPhysicalInformationArray& CompleteArray, 
											  BOOL** aabDone, 
											  const long lRows, const long lCols, 
											  const long lX, const long lY, 
										      const unsigned long ulDieSizeXTol, const unsigned long ulDieSizeYTol, 
											  const unsigned long ulDiePitchX, const unsigned long ulDiePitchY, 
											  const long lStartRow, const long lStartCol)
{
	WAF_CPhysicalInformationArray Stack;

	// Find the center die's row and column
	FindDie(TempArray, CompleteArray, Stack, aabDone, lX, lY, ulDieSizeXTol, ulDieSizeYTol, lStartRow, lStartCol);
	
	while (Stack.GetSize() > 0)  
	{
		WAF_CPhysicalInformation* p = Stack.GetAt(0);
		Stack.RemoveAt(0);

		long lRow = p->GetRow();
		long lCol = p->GetCol();

		// Left die
		if ((lRow >= 0) && (lCol > 0) && (!aabDone[lRow][lCol-1]))
		{
			long lLeftPitchX = p->GetX() - ulDiePitchX;
			long lLeftPitchY = p->GetY();

			FindDie(TempArray, CompleteArray, Stack, aabDone, lLeftPitchX, lLeftPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow, lCol-1);
		}

		// Right die
		if ((lRow >= 0) && (lCol >= 0) && (lRow < lRows-1) && (!aabDone[lRow+1][lCol]))
		{
			long lDownPitchX = p->GetX();
			long lDownPitchY = p->GetY() - ulDiePitchY;

			FindDie(TempArray, CompleteArray, Stack, aabDone, lDownPitchX, lDownPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow+1, lCol);
		}

		// Up die
		if ((lRow > 0) && (lCol >= 0) && (!aabDone[lRow-1][lCol]))
		{
			long lUpPitchX = p->GetX();
			long lUpPitchY = p->GetY() + ulDiePitchY;

			FindDie(TempArray, CompleteArray, Stack, aabDone, lUpPitchX, lUpPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow-1, lCol);
		}

		// Bottom die
		if ((lRow >= 0) && (lCol >= 0) && (lCol < lCols-1) && (!aabDone[lRow][lCol+1]))
		{
			long lRightPitchX = p->GetX() + ulDiePitchX;
			long lRightPitchY = p->GetY();

			FindDie(TempArray, CompleteArray, Stack, aabDone, lRightPitchX, lRightPitchY, ulDieSizeXTol, ulDieSizeYTol, lRow, lCol+1);
		}
	}
}			

// Sort the physical position using Y location
void WAF_CPhysicalInformationMap::Sort()
{
	// Presort the physical position
	int i;
	for (i=0; i<m_PhysicalInformationArray.GetSize(); i++) 
	{
		int nMaxIndex = i;
		int j;
		for (j=i+1; j<m_PhysicalInformationArray.GetSize(); j++) 
		{
			WAF_CPhysicalInformation* p = m_PhysicalInformationArray.GetAt(nMaxIndex);
			WAF_CPhysicalInformation* q = m_PhysicalInformationArray.GetAt(j);
			if (p->GetY() < q->GetY())
				nMaxIndex = j;
		}

		WAF_CPhysicalInformation* pTemp = m_PhysicalInformationArray.GetAt(nMaxIndex);
		m_PhysicalInformationArray.SetAt(nMaxIndex, m_PhysicalInformationArray.GetAt(i));
		m_PhysicalInformationArray.SetAt(i, pTemp);
	}
}

void WAF_CPhysicalInformationMap::SetXOffset(const long lXOffset)
{
	m_lXOffset = lXOffset;
}

void WAF_CPhysicalInformationMap::SetScan2Pr(const BOOL bEnable)
{
	m_bScan2PrRecord = bEnable;
}
