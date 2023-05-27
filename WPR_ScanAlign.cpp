#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "GallerySearchController.h"
#include "WAF_CPhysicalInformation.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"
#include "LastScanPosition.h"
#include "TakeTime.h"
#include "WT_SubRegion.h"
#include "MS896A.h"
#include "WaferTable.h"
#include "WL_Constant.h"

BOOL CWaferPr::IsScnExist(LONG ulRow, LONG ulCol)
{
	LONG lIndex = 0, lPhyX = 0, lPhyY = 0;
	BOOL bOverLap = FALSE;

	BOOL bReturn = m_cPIMap.GetInfo(ulRow, ulCol, lPhyX, lPhyY, lIndex, bOverLap);
	if( !IsScanAlignWafer() && m_ucScanCheckMapOnWafer>0 && (m_bScanAlignAutoFromMap || m_bScanAlignFromFile) )
	{
		if( lIndex < 0 )	//	WPR_ADD_DIE_INDEX )
		{
			bReturn = FALSE;
		}
	}

	return bReturn;
}

BOOL CWaferPr::IsMapExist(LONG ulRow, LONG ulCol)
{
	UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
	UCHAR ucGrade	= m_WaferMapWrapper.GetGrade(ulRow, ulCol);
	UCHAR ucOffset	= m_WaferMapWrapper.GetGradeOffset();

	BOOL bValidGrade = ucGrade!=ucNullBin;
	if( IsSpecialReferGrade(ulRow, ulCol) || m_WaferMapWrapper.IsReferenceDie(ulRow, ulCol) )
		bValidGrade = FALSE;
	if( IsMS90HalfSortMode() )
	{
		if( IsScanMapNgGrade(ucGrade-ucOffset) )
			bValidGrade = FALSE;
	}

	return bValidGrade;
}

BOOL CWaferPr::SortScanAlignHoleOrder()
{
	if( m_bScanAlignFromFile != TRUE )
	{
		return FALSE;
	}

	m_ulFoundHoleCounter = 0;
	ULONG ulTotalMapPoints = 0;
	for(INT i=0; i<SA_HOLES_MAX; i++)
	{
		m_astHoleList[i].m_awHoleRow.RemoveAll();
		m_astHoleList[i].m_awHoleCol.RemoveAll();
		m_astHoleList[i].m_ucHoleNul.RemoveAll();
	}

	ULONG lTotalPoints = m_ulScanAlignTotalPoints;
	if( lTotalPoints<=0 )
	{
		HmiMessage_Red_Back("No contents in the selected file", "Scan Align Wafer");
		return FALSE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	FILE *fpMap = NULL;
	CString szHolePmpFile = szLogPath + "_Hole.txt";	//	sort order
	errno_t nErr = fopen_s(&fpMap, szHolePmpFile, "a");
	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "1. sort scan align hole patters befor to match scanned wafer\n\n");
	}

	WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
	ULONG ulTgtRegion = pSRInfo->GetTargetRegion();

	LONG lUserRow = 0, lUserCol = 0;
	ULONG ulMapRow = 0, ulMapCol = 0, ulMinRow = 9999, ulMinCol = 9999, ulMaxRow = 0, ulMaxCol = 0;
	CUIntArray aunRowList, aunColList, aunStateList;
	CByteArray aucStaList;
	aucStaList.RemoveAll();
	aunRowList.RemoveAll();
	aunColList.RemoveAll();
	aunStateList.RemoveAll();
	for(ULONG i=0; i<lTotalPoints; i++)
	{
		if (i >= (ULONG)m_awPointsListRow.GetSize())
		{
			break;
		}
		lUserRow = m_awPointsListRow.GetAt(i);
		lUserCol = m_awPointsListCol.GetAt(i);
		BYTE ucSta = m_ucPointsListSta.GetAt(i);
		if( ConvertOrgUserToAsm(lUserRow, lUserCol, ulMapRow, ulMapCol) )
		{
			if( IsMS90HalfSortMode() && IsOutMS90SortingPart(ulMapRow, ulMapCol) )	//	MS90 half sort and out of range
			{
				continue;
			}
			if( pUtl->GetPrescanRegionMode() && pSRInfo->IsWithinThisRegion(ulTgtRegion, ulMapRow, ulMapCol)==FALSE )
			{
				continue;
			}

			aunRowList.Add(ulMapRow);
			aunColList.Add(ulMapCol);
			aucStaList.Add(ucSta);
			aunStateList.Add(1);
			ulTotalMapPoints++;
			if( ulMinRow>ulMapRow )
				ulMinRow = ulMapRow;
			if( ulMinCol>ulMapCol )
				ulMinCol = ulMapCol;
			if( ulMaxRow<ulMapRow )
				ulMaxRow = ulMapRow;
			if( ulMaxCol<ulMapCol )
				ulMaxCol = ulMapCol;

			if ((nErr == 0) && (fpMap != NULL))
			{
				fprintf(fpMap, "point %3lu(%3lu,%3lu)[%3ld,%3ld][%d]\n", i+1, ulMapRow, ulMapCol, lUserRow, lUserCol, ucSta);
			}
		}
	}

	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "\n");
		fprintf(fpMap, "Scan align map range (%lu,%lu)==(%lu,%lu) total points %lu in map %lu\n\n",
			ulMinRow, ulMinCol, ulMaxRow, ulMaxCol, lTotalPoints, ulTotalMapPoints);
	}

	lTotalPoints = (ULONG)aunStateList.GetSize();
	for (ULONG ulRow = ulMinRow; ulRow <= ulMaxRow; ulRow++)
	{
		if (lTotalPoints == 0)
		{
			break;
		}

		for (ULONG ulCol = ulMinCol; ulCol <= ulMaxCol; ulCol++)
		{
			for (ULONG ulLoop = 0; ulLoop < lTotalPoints; ulLoop++)
			{
				if (aunStateList.GetAt(ulLoop) != 1)
				{
					continue;
				}
				if ((ulRow != aunRowList.GetAt(ulLoop)) || (ulCol != aunColList.GetAt(ulLoop)))
				{
					continue;
				}

				BYTE ucSta = aucStaList.GetAt(ulLoop);
				BOOL bFindHole = FALSE;
				for (ULONG i=0; i<m_ulFoundHoleCounter; i++)
				{
					INT nLimit = min((INT)m_astHoleList[i].m_awHoleRow.GetSize(),
									 (INT)m_astHoleList[i].m_awHoleCol.GetSize());
					for (INT j = 0; j < nLimit; j++)
					{
						ULONG ulHoleRow = m_astHoleList[i].m_awHoleRow.GetAt(j);
						ULONG ulHoleCol = m_astHoleList[i].m_awHoleCol.GetAt(j);
						if (labs(ulRow - ulHoleRow) <= 1 &&
							labs(ulCol - ulHoleCol) <= 1)
						{
							bFindHole = TRUE;
							m_astHoleList[i].m_awHoleRow.Add(ulRow);
							m_astHoleList[i].m_awHoleCol.Add(ulCol);
							m_astHoleList[i].m_ucHoleNul.Add(ucSta);
							break;
						}
					}
					if( bFindHole )
					{
						break;
					}
				}
				if( bFindHole==FALSE )
				{
					m_astHoleList[m_ulFoundHoleCounter].m_awHoleRow.Add(ulRow);
					m_astHoleList[m_ulFoundHoleCounter].m_awHoleCol.Add(ulCol);
					m_astHoleList[m_ulFoundHoleCounter].m_ucHoleNul.Add(ucSta);
					m_ulFoundHoleCounter++;
				}
				aunStateList.SetAt(ulLoop, 0);
				break;
			}
			if( m_ulFoundHoleCounter>=SA_HOLES_MAX )
			{
				break;
			}
		}
		if( m_ulFoundHoleCounter>=50 )
		{
			break;
		}
	}
	aunStateList.RemoveAll();
	aunRowList.RemoveAll();
	aunColList.RemoveAll();
	aucStaList.RemoveAll();

	ulTotalMapPoints = 0;
	for(ULONG i=0; i<m_ulFoundHoleCounter; i++)
	{
		ULONG nLimit = min((ULONG)m_astHoleList[i].m_awHoleRow.GetSize(),
						   (ULONG)m_astHoleList[i].m_awHoleCol.GetSize());

		ulTotalMapPoints += nLimit;

		if ((nErr == 0) && (fpMap != NULL))
		{
			fprintf(fpMap, "Hole %02lu has points %02lu;", i+1, nLimit);
		}
		for(ULONG j=0; j<nLimit; j++)
		{
			ulMapRow = m_astHoleList[i].m_awHoleRow.GetAt(j);
			ulMapCol = m_astHoleList[i].m_awHoleCol.GetAt(j);
			BYTE ucSta = m_astHoleList[i].m_ucHoleNul.GetAt(j);
			ConvertAsmToOrgUser(ulMapRow, ulMapCol, lUserRow, lUserCol);
			if( fpMap!=NULL )
			{
				fprintf(fpMap, " (%3lu,%3lu)[%3ld,%3ld][%d]", ulMapRow, ulMapCol, lUserRow, lUserCol, ucSta);
			}
		}

		if ((nErr == 0) && (fpMap != NULL))
		{
			fprintf(fpMap, "\n");
		}
	}

	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "\n");
		fprintf(fpMap, "valid in map total %lu\n\n", ulTotalMapPoints);
		fclose(fpMap);
	}

	if( (IsMS90HalfSortMode()==FALSE) && (pUtl->GetPrescanRegionMode()==FALSE) )
	{
		if( ulTotalMapPoints<m_ulScanAlignTotalPoints )
		{
			return	FALSE;
		}
	}	//	

	if( !IsScanAlignWafer() && m_ucScanCheckMapOnWafer>0 && m_bScanAlignFromFile )
	{
		return TRUE;
	}

	if( m_ulFoundHoleCounter<=0 )
	{
		return FALSE;
	}

	return TRUE;
}	// to load setting align file

LONG CWaferPr::ScanAlignPhysicalWaferWithHoles(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, LONG	&lMapScnOffsetRow, LONG &lMapScnOffsetCol)
{
	BOOL bInfo = FALSE, bFindMatchHole = FALSE;
	LONG lTopRow = m_astHoleList[0].m_awHoleRow.GetAt(0);
	LONG lTopCol = m_astHoleList[0].m_awHoleCol.GetAt(0);
	LONG lFindMatchCounter = 0;

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	FILE *fpMap = NULL;
	CString szHolePmpFile = szLogPath + "_Hole.txt";	//	scan align
	errno_t nErr = fopen_s(&fpMap, szHolePmpFile, "a");
	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "2. scan align map with scanned wafer range (%lu,%lu)==(%lu,%lu)\n\n",
			ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol);
	}

	ULONG ulHoleTry = 0;
	for(ULONG ulRow=ulScnMinRow; ulRow<=ulScnMaxRow; ulRow++)
	{
		BOOL bFindDieBefore = FALSE;
		for(ULONG ulCol=ulScnMinCol; ulCol<=ulScnMaxCol; ulCol++)
		{
			bInfo = IsScnExist(ulRow, ulCol);
			if( bInfo==TRUE )
			{
				bFindDieBefore = TRUE;
				continue;
			}
			// must a none empty die before in this row.
			if( bFindDieBefore==FALSE )
			{
				continue;
			}

			// check any die after the empty, if no any die after it, then no need to check.
			BOOL bFindDieAfter = FALSE;
			for(ULONG j=ulCol+1; j<=ulScnMaxCol; j++)
			{
				bInfo = IsScnExist(ulRow, j);
				if( bInfo )
				{
					bFindDieAfter = TRUE;
					break;
				}
			}
			if( bFindDieAfter==FALSE )
			{
				break;
			}

			bFindMatchHole = TRUE;
			CString szHoleLog = "";
			ulHoleTry++;
			for(ULONG ulHole=0; ulHole<m_ulFoundHoleCounter; ulHole++)
			{
				CString szLineLog = "";
				CString szError = "OK match";
				INT nLimit = min((INT)m_astHoleList[ulHole].m_awHoleRow.GetSize(),
								 (INT)m_astHoleList[ulHole].m_awHoleCol.GetSize());
				szLineLog.Format(" Hole %02d:", ulHole+1);
				ULONG ulHasInHoles = 0;
				CString szPiece = "";
				for(INT iPoint=0; iPoint<nLimit; iPoint++)
				{
					LONG lPointMapRow = m_astHoleList[ulHole].m_awHoleRow.GetAt(iPoint);
					LONG lPointMapCol = m_astHoleList[ulHole].m_awHoleCol.GetAt(iPoint);
					BYTE bPointMapSta = m_astHoleList[ulHole].m_ucHoleNul.GetAt(iPoint);

					LONG lPointScnRow = (LONG)ulRow + lPointMapRow - lTopRow;
					LONG lPointScnCol = (LONG)ulCol + lPointMapCol - lTopCol;

					szPiece.Format(" %02d Scn(%4ld,%4ld)==>Map(%4lu,%4lu)(%d);",
						iPoint+1, lPointScnRow, lPointScnCol, lPointMapRow, lPointMapCol, bPointMapSta);
					szLineLog = szLineLog + szPiece;
					if( lPointMapRow==lTopRow && lPointMapCol==lTopCol )
					{
						continue;
					}
					// Outof range, fail
					if( lPointScnRow<(LONG)ulScnMinRow || lPointScnRow>(LONG)ulScnMaxRow ||
						lPointScnCol<(LONG)ulScnMinCol || lPointScnCol>(LONG)ulScnMaxCol )
					{
						bFindMatchHole = FALSE;
						szError = "NO range";
						break;
					}

					// has die on it, fail
					bInfo = IsScnExist(lPointScnRow, lPointScnCol);
					if( bInfo==TRUE )
					{
						ulHasInHoles++;
						if( ulHasInHoles>m_ulHoleSkipPointLimit )
						{
							bFindMatchHole = FALSE;
						szError = "NO empty";
							break;
						}
					}

					BOOL bHasDieBefore = FALSE, bHasDieAfter = FALSE;
					// must a none empty die before in this row.
					for(LONG jCol=ulScnMinCol; jCol<lPointScnCol; jCol++)
					{
						bInfo = IsScnExist(lPointScnRow, jCol);
						if( bInfo==TRUE )
						{
							bHasDieBefore = TRUE;
							break;
						}
					}
					if( bHasDieBefore==FALSE )
					{
						bFindMatchHole = FALSE;
						szError = "NO ahead";
						break;
					}
					// check any die after the empty, if no any die after it, then no need to check.
					for(ULONG jCol=lPointScnCol+1; jCol<=ulScnMaxCol; jCol++)
					{
						bInfo = IsScnExist(lPointScnRow, jCol);
						if( bInfo )
						{
							bHasDieAfter = TRUE;
							break;
						}
					}
					if( bHasDieAfter==FALSE )
					{
						bFindMatchHole = FALSE;
						szError = "NO after";
						break;
					}
				}	// end hole points checking
				if( ulHole==0 )
					szPiece.Format("%04lu ", ulHoleTry);
				else
					szPiece = "     ";
				szHoleLog = szHoleLog + szPiece + szError + szLineLog + "\n";
				if( bFindMatchHole==FALSE )
				{
					break;
				}
			}	// end hole list checking

			if( fpMap!=NULL )
			{
				fprintf(fpMap, "%s", (LPCTSTR) szHoleLog);	//v4.40 Klocwork
			}
			if( bFindMatchHole )
			{
				lMapScnOffsetRow = (lTopRow - (LONG)ulRow);
				lMapScnOffsetCol = (lTopCol - (LONG)ulCol);
				lFindMatchCounter++;
				if( fpMap!=NULL )
				{
					fprintf(fpMap, "\n");
					szHoleLog.Format("Ok and pass at SCN %lu,%lu map %ld,%ld, offset %ld,%ld, match order %ld\n",
						ulRow, ulCol, lTopRow, lTopCol, lMapScnOffsetRow, lMapScnOffsetCol, lFindMatchCounter);
					fprintf(fpMap, "%s", (LPCTSTR) szHoleLog);
					fprintf(fpMap, "\n");
				}
			//	break;
			}
		}
		if( bFindMatchHole )
		{
		//	break;
		}
	}

	if( fpMap!=NULL )
	{
		fprintf(fpMap, "\n");
		fclose(fpMap);
	}

	return	lFindMatchCounter;
}

BOOL CWaferPr::FindSanAnCrossPatternInMap(LONG lOrgHomeRow, LONG lOrgHomeCol, ULONG &ulHomeRow, ULONG &ulHomeCol)
{
	// must sysmetic pattern such as cross, square cycle ...
	CString szMsg;
	ULONG ulRow[3][3], ulCol[3][3];
	ULONG ulAsmULRow = 0, ulAsmULCol = 0;
	for(int iRow=0; iRow<3; iRow++)
	{
		for(int iCol=0; iCol<3; iCol++)
		{
			ConvertOrgUserToAsm(iRow-3+lOrgHomeRow, iCol-1+lOrgHomeCol, ulRow[iRow][iCol], ulCol[iRow][iCol]);
			szMsg.Format("%d,%d ==> %d,%d", iRow-3+lOrgHomeRow, iCol-1+lOrgHomeCol, ulRow[iRow][iCol], ulCol[iRow][iCol]);
			SetErrorMessage(szMsg);
			if( iRow==0 && iCol==0 )
			{
				ulAsmULRow = ulRow[0][0];
				ulAsmULCol = ulCol[0][0];
			}
			else
			{
				if( ulAsmULRow>ulRow[iRow][iCol] )
					ulAsmULRow = ulRow[iRow][iCol];
				if( ulAsmULCol>ulCol[iRow][iCol] )
					ulAsmULCol = ulCol[iRow][iCol];
			}
		}
	}

	LONG lOrgRow = 0, lOrgCol = 0;
	ConvertAsmToOrgUser(ulAsmULRow, ulAsmULCol, lOrgRow, lOrgCol);
	szMsg.Format("%d,%d ==> %d,%d", lOrgRow, lOrgCol, ulAsmULRow, ulAsmULCol);
	SetErrorMessage(szMsg);
	for(int iRow=0; iRow<3; iRow++)
	{
		for(int iCol=0; iCol<3; iCol++)
		{
			ulRow[iRow][iCol] = ulAsmULRow + iRow;
			ulCol[iRow][iCol] = ulAsmULCol + iCol;
			ConvertAsmToOrgUser(ulRow[iRow][iCol], ulCol[iRow][iCol], lOrgRow, lOrgCol);
			szMsg.Format("%d,%d ==> %d,%d", lOrgRow, lOrgCol, ulRow[iRow][iCol], ulCol[iRow][iCol]);
			SetErrorMessage(szMsg);
		}
	}

//    oxo
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow[0][0], ulCol[0][0]))
		return FALSE;
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow[0][1], ulCol[0][1]))
		return FALSE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow[0][2], ulCol[0][2]))
		return FALSE;
//    xxx
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow[1][0], ulCol[1][0]))
		return FALSE;
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow[1][1], ulCol[1][1]))
		return FALSE;
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow[1][2], ulCol[1][2]))
		return FALSE;
//    oxo
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow[2][0], ulCol[2][0]))
		return FALSE;
	if (WM_CWaferMap::Instance()->IsMapHaveBin(ulRow[2][1], ulCol[2][1]))
		return FALSE;
	if (WM_CWaferMap::Instance()->IsMapNullBin(ulRow[2][2], ulCol[2][2]))
		return FALSE;

	ulHomeRow = ulAsmULRow + 3;
	ulHomeCol = ulAsmULCol + 1;

	ConvertAsmToOrgUser(ulHomeRow, ulHomeCol, lOrgRow, lOrgCol);
	szMsg.Format("found map cross bottom %d,%d(%d,%d)", lOrgRow, lOrgCol, ulHomeRow, ulHomeCol);
	SetErrorMessage(szMsg);

	return TRUE;
}

BOOL CWaferPr::MatchScanAnCrossPatternInWafer(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, LONG &lScanRow, LONG &lScanCol)
{
	LONG	lFindCrossCount = 0;
//	BOOL bFindCross = FALSE;
	BOOL bInfo = FALSE;

	for(LONG lScnRow=(LONG)ulScnMinRow; lScnRow<=(LONG)ulScnMaxRow; lScnRow++)
	{
		BOOL bFindDieBefore = FALSE;
		for(LONG lScnCol=(LONG)ulScnMinCol; lScnCol<=(LONG)ulScnMaxCol; lScnCol++)
		{
			if( lScnRow<1 )
				continue;
			if( lScnCol<2 )
				continue;
			bInfo = IsScnExist(lScnRow, lScnCol);
			if( bInfo==TRUE )
			{
				bFindDieBefore = TRUE;
				continue;
			}
			// must a none empty die before in this row.
			if( bFindDieBefore==FALSE )
			{
				continue;
			}

			// check any die after the empty, if no any die after it, then no need to check.
			BOOL bFindDieAfter = FALSE;
			for(ULONG j=lScnCol+1; j<=ulScnMaxCol; j++)
			{
				bInfo = IsScnExist(lScnRow, j);
				if( bInfo )
				{
					bFindDieAfter = TRUE;
					break;
				}
			}
			if( bFindDieAfter==FALSE )
			{
				break;
			}

			// oxo
			bInfo = IsScnExist(lScnRow, lScnCol-1);
			if( bInfo!=TRUE )
			{
				continue;
			}
			bInfo = IsScnExist(lScnRow, lScnCol+1);
			if( bInfo!=TRUE )
			{
				continue;
			}

			// xxx
			bInfo = IsScnExist(lScnRow+1, lScnCol-1);
			if( bInfo==TRUE )
			{
				continue;
			}
			bInfo = IsScnExist(lScnRow+1, lScnCol);
			if( bInfo==TRUE )
			{
				continue;
			}
			bInfo = IsScnExist(lScnRow+1, lScnCol+1);
			if( bInfo==TRUE )
			{
				continue;
			}

			// oxo
			bInfo = IsScnExist(lScnRow+2, lScnCol-1);
			if( bInfo!=TRUE )
			{
				continue;
			}
			bInfo = IsScnExist(lScnRow+2, lScnCol);
			if( bInfo==TRUE )
			{
				continue;
			}
			bInfo = IsScnExist(lScnRow+2, lScnCol+1);
			if( bInfo!=TRUE )
			{
				continue;
			}

			// check above has die
			BOOL bCheckNear = FALSE;
			for(LONG lRow=ulScnMinRow; lRow<lScnRow; lRow++)
			{
				bInfo = IsScnExist(lRow, lScnCol);
				if( bInfo==TRUE )
				{
					bCheckNear = TRUE;
					break;
				}
			}
			if( bCheckNear==FALSE )
			{
				continue;
			}

			// check below has die
			bCheckNear = FALSE;
			for(LONG lRow=lScnRow+3; lRow<(LONG)ulScnMaxRow; lRow++)
			{
				bInfo = IsScnExist(lRow, lScnCol);
				if( bInfo==TRUE )
				{
					bCheckNear = TRUE;
					break;
				}
			}
			if( bCheckNear==FALSE )
			{
				continue;
			}

			// check left has die
			bCheckNear = FALSE;
			for(LONG lCol=ulScnMinCol; lCol<lScnCol-1; lCol++)
			{
				bInfo = IsScnExist(lScnRow+1, lCol);
				if( bInfo==TRUE )
				{
					bCheckNear = TRUE;
					break;
				}
			}
			if( bCheckNear==FALSE )
			{
				continue;
			}

			// check right has die
			bCheckNear = FALSE;
			for(LONG lCol=lScnCol+2; lCol<(LONG)ulScnMaxCol; lCol++)
			{
				bInfo = IsScnExist(lScnRow+1, lCol);
				if( bInfo==TRUE )
				{
					bCheckNear = TRUE;
					break;
				}
			}
			if( bCheckNear==FALSE )
			{
				continue;
			}

			lScanRow = lScnRow + 3;
			lScanCol = lScnCol;
		//	bFindCross = TRUE;
			lFindCrossCount++;
		//	break;
		}
	//	if( bFindCross )
	//	{
	//		break;
	//	}
	}

	if( lFindCrossCount==1 )
		return TRUE;

	return FALSE;
//	return bFindCross;
}

BOOL CWaferPr::ScanAlignWaferMapWithRefer(LONG ulScnMinRow, LONG ulScnMinCol, LONG ulScnMaxRow, LONG ulScnMaxCol, LONG &lMapScnOffsetRow, LONG &lMapScnOffsetCol)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	FILE *fpRefer = NULL;
	LONG lMapReferTotal = min((LONG)m_awMapReferListRow.GetSize(), (LONG)m_awMapReferListCol.GetSize());
	LONG lWaferRefTotal = min((LONG)m_awWaferRefListRow.GetSize(), (LONG)m_awWaferRefListCol.GetSize());
	CString szMsg;
	if( pUtl->GetPrescanDebug() )
	{
		CString szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		CString szReferPmpFile = szLogPath + "_Refer.pmp";
		errno_t nErr = fopen_s(&fpRefer, szReferPmpFile, "w");
		if ((nErr == 0) && (fpRefer != NULL))
		{
			szMsg.Format("Total %d refer die list in scanned wafer", lWaferRefTotal);
			fprintf(fpRefer, "%s\n", (LPCTSTR) szMsg);	//Klocwork	//v4.46
			for(LONG i=0; i<lWaferRefTotal; i++)
			{
				szMsg.Format( "%5d,%5d", m_awWaferRefListRow.GetAt(i), m_awWaferRefListCol.GetAt(i));
				fprintf(fpRefer, "%s\n", (LPCTSTR) szMsg);
			}
			szMsg.Format("\nTotal %d refer die list in map file", lMapReferTotal);
			fprintf(fpRefer, "%s\n", (LPCTSTR) szMsg);
			for(LONG i=0; i<lMapReferTotal; i++)
			{
				szMsg.Format("%5d,%5d", m_awMapReferListRow.GetAt(i), m_awMapReferListCol.GetAt(i));
				fprintf(fpRefer, "%s\n", (LPCTSTR) szMsg);
			}
			szMsg.Format("To log the align mapping result");
			fprintf(fpRefer, "\n%s\n", (LPCTSTR) szMsg);
		}
	}

	LONG lFindMatchReferCounter = 0, lBestOffsetRow = 0, lBestOffsetCol = 0;
	BOOL bFoundBest = FALSE;
	for(LONG lScanIndex=0; lScanIndex<lWaferRefTotal; lScanIndex++)
	{
		LONG lScanRefRow = m_awWaferRefListRow.GetAt(lScanIndex);
		LONG lScanRefCol = m_awWaferRefListCol.GetAt(lScanIndex);
		for(LONG lMapIndex=0; lMapIndex<lMapReferTotal; lMapIndex++)
		{
			LONG lMapRow = m_awMapReferListRow.GetAt(lMapIndex);
			LONG lMapCol = m_awMapReferListCol.GetAt(lMapIndex);
			LONG lRefDiffRow = lScanRefRow - lMapRow;
			LONG lRefDiffCol = lScanRefCol - lMapCol;
			BOOL bReferPointOK = TRUE, bReferBestOK = TRUE;
			for(LONG lCheckMapIndex=0; lCheckMapIndex<lMapReferTotal; lCheckMapIndex++)
			{
				BOOL bMapPointOnWaferOK = FALSE;
				LONG lCheckRow = m_awMapReferListRow.GetAt(lCheckMapIndex) + lRefDiffRow;
				LONG lCheckCol = m_awMapReferListCol.GetAt(lCheckMapIndex) + lRefDiffCol;
				if( lCheckRow<ulScnMinRow || lCheckCol<ulScnMinCol ||
					lCheckRow>ulScnMaxRow || lCheckCol>ulScnMaxCol )
				{
					bReferPointOK = FALSE;
					bReferBestOK = FALSE;
					break;
				}

				for(LONG lLoopIndex=0; lLoopIndex<lWaferRefTotal; lLoopIndex++)
				{
					if( (lCheckRow==m_awWaferRefListRow.GetAt(lLoopIndex)) &&
						(lCheckCol==m_awWaferRefListCol.GetAt(lLoopIndex)) )
					{
						bMapPointOnWaferOK = TRUE;
						break;
					}
				}

				if( bMapPointOnWaferOK==FALSE )
				{
					bReferBestOK = FALSE;
					if( IsScnExist(lCheckRow, lCheckCol)==FALSE )
					{
						#define	NEAR_CHECK_BOUNDARY		5
						LONG lStartCheckRow = lCheckRow - NEAR_CHECK_BOUNDARY;
						LONG lStartCheckCol = lCheckCol - NEAR_CHECK_BOUNDARY;
						LONG lStopCheckRow  = lCheckRow + NEAR_CHECK_BOUNDARY;
						LONG lStopCheckCol  = lCheckCol + NEAR_CHECK_BOUNDARY;
						LONG lFindNormalDie = 0;

						if( lStartCheckRow<ulScnMinRow )
							lStartCheckRow = ulScnMinRow;
						if( lStartCheckCol<ulScnMinCol )
							lStartCheckCol = ulScnMinCol;
						if( lStopCheckRow>ulScnMaxRow )
							lStopCheckRow = ulScnMaxRow;
						if( lStopCheckCol>ulScnMaxCol )
							lStopCheckCol = ulScnMaxCol;

						for(LONG lNearRow=lStartCheckRow; lNearRow<=lStopCheckRow; lNearRow++)
						{
							for(LONG lNearCol=lStartCheckCol; lNearCol<=lStopCheckCol; lNearCol++)
							{
								if( IsScnExist(lNearRow, lNearCol) )
								{
									lFindNormalDie++;
								}
							}
						}
						if( lFindNormalDie>0 )
						{
							bMapPointOnWaferOK = TRUE;
						}
					}
				}

				if( bMapPointOnWaferOK==FALSE )
				{
					bReferPointOK = FALSE;
					break;
				}
			}

			if( bReferPointOK )
			{
				lMapScnOffsetRow = (lMapRow - lScanRefRow);
				lMapScnOffsetCol = (lMapCol - lScanRefCol);
				lFindMatchReferCounter++;
				if( bReferBestOK && bFoundBest==FALSE )
				{
					lBestOffsetRow = lMapScnOffsetRow;
					lBestOffsetCol = lMapScnOffsetCol;
					bFoundBest = TRUE;
				}
				if( fpRefer!=NULL )
				{
					szMsg.Format("Map %5d,%5d Wafer %5d,%5d",
						lMapRow, lMapCol, lScanRefRow, lScanRefCol);
					fprintf(fpRefer, "%s\n", (LPCTSTR) szMsg);
				}
				break;
			}
		}
	}

	if( bFoundBest )
	{
		lMapScnOffsetRow = lBestOffsetRow;
		lMapScnOffsetCol = lBestOffsetCol;
	}

	if( fpRefer!=NULL )
	{
		szMsg.Format("Total matched %d", lFindMatchReferCounter);
		fprintf(fpRefer, "\n%s\n", (LPCTSTR) szMsg);
		fclose(fpRefer);
	}

	return	lFindMatchReferCounter>0;
}


VOID CWaferPr::FindMinMaxRowColListInMatrix(const BOOL bScnMap,
											const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow,
											const ULONG ulMapValidMinCol, const ULONG ulMapValidMaxCol,
											CDWordArray &dwaRowMinList, CDWordArray &dwaRowMaxList,
											CDWordArray &dwaColMinList, CDWordArray &dwaColMaxList)
{
	dwaRowMinList.RemoveAll();
	dwaRowMaxList.RemoveAll();
	dwaColMinList.RemoveAll();
	dwaColMaxList.RemoveAll();

	for (ULONG ulCol = ulMapValidMinCol; ulCol <= ulMapValidMaxCol; ulCol++)
	{
		ULONG ulMinRow = ulMapValidMinRow;
		ULONG ulMaxRow = ulMapValidMaxRow;
		for (ULONG ulRow = ulMapValidMinRow; ulRow <= ulMapValidMaxRow; ulRow++)
		{
			if (!bScnMap && IsMapExist(ulRow, ulCol) || bScnMap && IsScnExist(ulRow, ulCol))
			{
				ulMinRow = ulRow;
				break;
			}
		}

		for (LONG ulRow = (LONG)ulMapValidMaxRow; ulRow >= (LONG)ulMapValidMinRow; ulRow--)
		{
			if (!bScnMap && IsMapExist(ulRow, ulCol) || bScnMap && IsScnExist(ulRow, ulCol))
			{
				ulMaxRow = ulRow;
				break;
			}
		}

		dwaRowMinList.Add(ulMinRow);
		dwaRowMaxList.Add(ulMaxRow);
	}

	for (ULONG ulRow = ulMapValidMinRow; ulRow <= ulMapValidMaxRow; ulRow++)
	{
		ULONG ulMinCol = ulMapValidMinCol;
		ULONG ulMaxCol = ulMapValidMaxCol;
		for (ULONG ulCol = ulMapValidMinCol; ulCol <= ulMapValidMaxCol; ulCol++)
		{
			if (!bScnMap && IsMapExist(ulRow, ulCol) || bScnMap && IsScnExist(ulRow, ulCol))
			{
				ulMinCol = ulCol;
				break;
			}
		}

		for (LONG ulCol = (LONG)ulMapValidMaxCol; ulCol >= (LONG)ulMapValidMinCol; ulCol--)
		{
			if (!bScnMap && IsMapExist(ulRow, ulCol) || bScnMap && IsScnExist(ulRow, ulCol))
			{
				ulMaxCol = ulCol;
				break;
			}
		}

		dwaColMinList.Add(ulMinCol);
		dwaColMaxList.Add(ulMaxCol);
	}
}


LONG CWaferPr::FindHoleList(const BOOL bScnMap,
							const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow,
							CDWordArray &dwaColMinList, CDWordArray &dwaColMaxList,
							CDWordArray &awHoleListRow, CDWordArray &awHoleListCol,
							ULONG &ulHoleMinRow, ULONG &ulHoleMaxRow,
							ULONG &ulHoleMinCol, ULONG &ulHoleMaxCol)

{
	LONG lTotalHolePoints = 0;

	awHoleListRow.RemoveAll();
	awHoleListCol.RemoveAll();

	ulHoleMinRow = 9999;
	ulHoleMinCol = 9999;
	ulHoleMaxRow = 0;
	ulHoleMaxCol = 0;

	for (ULONG ulRow = ulMapValidMinRow + 1; ulRow < ulMapValidMaxRow; ulRow++)
	{
		ULONG ulFirstCol = dwaColMinList.GetAt(ulRow - ulMapValidMinRow);
		ULONG ulRightCol = dwaColMaxList.GetAt(ulRow - ulMapValidMinRow);

		for (ULONG ulCol = ulFirstCol; ulCol <= ulRightCol; ulCol++)
		{
			if (!bScnMap && IsMapExist(ulRow, ulCol) || bScnMap && IsScnExist(ulRow, ulCol))
			{
				continue;
			}

			awHoleListRow.Add(ulRow);
			awHoleListCol.Add(ulCol);
			if (ulHoleMinRow > ulRow)
				ulHoleMinRow = ulRow;
			if (ulHoleMinCol > ulCol)
				ulHoleMinCol = ulCol;
			if (ulHoleMaxRow < ulRow)
				ulHoleMaxRow = ulRow;
			if (ulHoleMaxCol < ulCol)
				ulHoleMaxCol = ulCol;
			lTotalHolePoints++;
		}
	}

	return lTotalHolePoints;
}


VOID CWaferPr::MarkAllHoleDieFlag(const LONG lTotalHolePoints,
							   const ULONG ulHoleMinRow, const ULONG ulHoleMinCol,
							   CDWordArray &awHoleListRow, CDWordArray &awHoleListCol)
{
	for (INT i = 0; i < MAX_MAP_DIM; i++)
	{
		for (INT j = 0; j < MAX_MAP_DIM; j++)
		{
			m_ucaMatrix[i][j] = 0;
		}
	}

	//translate to origin(0,0)
	for (LONG ulLoop = 0; ulLoop < lTotalHolePoints; ulLoop++)
	{
		ULONG ulRow = awHoleListRow.GetAt(ulLoop) - ulHoleMinRow + 1;
		ULONG ulCol = awHoleListCol.GetAt(ulLoop) - ulHoleMinCol + 1;
		ulRow = min(ulRow, MAX_MAP_DIM - 1);
		ulCol = min(ulCol, MAX_MAP_DIM - 1);
		m_ucaMatrix[ulRow][ulCol] = 1;
	}
}


LONG CWaferPr::CreateHolePatterns(const ULONG ulHoleMinRow, const ULONG ulHoleMaxRow,
								  const ULONG ulHoleMinCol, const ULONG ulHoleMaxCol,
								  CDWordArray &awHoleListRow, CDWordArray &awHoleListCol,
								  SCAN_ALIGN_HOLE_LIST	astPatternList[MAX_PTNS])
{
	ULONG ulFoundPatterns = 0;
	//	for map and scanned wafer.
	for(INT i = 0; i < MAX_PTNS; i++)
	{
		astPatternList[i].m_awHoleRow.RemoveAll();
		astPatternList[i].m_awHoleCol.RemoveAll();
	}

	ULONG ulRowLoop = 1;
	if (ulHoleMaxRow >= ulHoleMinRow)
	{
		ulRowLoop = ulHoleMaxRow - ulHoleMinRow + 1;
	}

	ULONG ulColLoop = 1;
	if (ulHoleMaxCol >= ulHoleMinCol)
	{
		ulColLoop = ulHoleMaxCol - ulHoleMinCol + 1;
	}
	ulRowLoop = min(ulRowLoop, MAX_MAP_DIM - 2);
	ulColLoop = min(ulColLoop, MAX_MAP_DIM - 2);

	for (ULONG ulRow = 1; ulRow <= ulRowLoop; ulRow++)
	{
		for(ULONG ulCol = 1; ulCol <= ulColLoop; ulCol++)
		{
			if (m_ucaMatrix[ulRow][ulCol] == 0)
				continue;

			if (ulFoundPatterns == 0)
			{
				astPatternList[ulFoundPatterns].m_awHoleRow.Add(ulRow + ulHoleMinRow - 1);
				astPatternList[ulFoundPatterns].m_awHoleCol.Add(ulCol + ulHoleMinCol - 1);
				ulFoundPatterns++;
				m_ucaMatrix[ulRow][ulCol] = 0;
				continue;
			}

			while( 1 )
			{
				BOOL bNewUpdated = FALSE;
				for (ULONG i = 0; i < ulFoundPatterns; i++)
				{
					INT nLimit = min((LONG)astPatternList[i].m_awHoleRow.GetSize(), (LONG)astPatternList[i].m_awHoleCol.GetSize());
					for(INT j = 0; j < nLimit; j++)
					{
						ULONG ulPointRow = astPatternList[i].m_awHoleRow.GetAt(j);
						ULONG ulPointCol = astPatternList[i].m_awHoleCol.GetAt(j);
						ULONG ulCheckRow = ulPointRow - ulHoleMinRow + 1;
						ULONG ulCheckCol = ulPointCol - ulHoleMinCol + 1;

						ulCheckRow = min(ulCheckRow, MAX_MAP_DIM - 2);
						ulCheckCol = min(ulCheckCol, MAX_MAP_DIM - 2);
						ulCheckRow = max(ulCheckRow, 1);
						ulCheckCol = max(ulCheckCol, 1);
						if (m_ucaMatrix[ulCheckRow - 1][ulCheckCol] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow - 1);
							astPatternList[i].m_awHoleCol.Add(ulPointCol);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow - 1][ulCheckCol] = 0;
						}
						if (m_ucaMatrix[ulCheckRow + 1][ulCheckCol] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow + 1);
							astPatternList[i].m_awHoleCol.Add(ulPointCol);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow + 1][ulCheckCol] = 0;
						}
						if (m_ucaMatrix[ulCheckRow][ulCheckCol - 1] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow);
							astPatternList[i].m_awHoleCol.Add(ulPointCol - 1);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow][ulCheckCol - 1] = 0;
						}
						if (m_ucaMatrix[ulCheckRow][ulCheckCol + 1] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow);
							astPatternList[i].m_awHoleCol.Add(ulPointCol + 1);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow][ulCheckCol + 1] = 0;
						}
					}
				}
				if( bNewUpdated==FALSE )
				{
					break;
				}
			}

			if (m_ucaMatrix[ulRow][ulCol] == 1)
			{
				astPatternList[ulFoundPatterns].m_awHoleRow.Add(ulRow + ulHoleMinRow - 1);
				astPatternList[ulFoundPatterns].m_awHoleCol.Add(ulCol + ulHoleMinCol - 1);
				ulFoundPatterns++;
				m_ucaMatrix[ulRow][ulCol] = 0;
			}

			if( ulFoundPatterns>=MAX_PTNS )
			{
				break;
			}
		}
		if( ulFoundPatterns>=MAX_PTNS )
		{
			break;
		}
	}

	return ulFoundPatterns;
}


VOID CWaferPr::FilterHolePatterns(const BOOL bScnMap, const ULONG ulMapValidMinCol, CDWordArray &dwaRowMinList, CDWordArray &dwaRowMaxList,
								  const ULONG ulFoundPatterns, SCAN_ALIGN_HOLE_LIST	astPatternList[MAX_PTNS],
								  ULONG &ulMapAlnPatterns, ULONG &ulMapAlnHolesNum,
								  SCAN_ALIGN_HOLE_LIST	aMapAlnPatnList[MAX_PTNS])
{
	ulMapAlnPatterns = 0;
	ulMapAlnHolesNum = 0;

	for (ULONG i = 0; i < ulFoundPatterns; i++)
	{
		LONG nLimit = min((LONG)astPatternList[i].m_awHoleRow.GetSize(), (LONG)astPatternList[i].m_awHoleCol.GetSize());
		if (nLimit < m_lScanAlignPatternHolesMin)
		{
			continue;
		}
		if (m_lScanAlignPatternHolesMax > m_lScanAlignPatternHolesMin && nLimit > m_lScanAlignPatternHolesMax)
		{
			continue;
		}

		BOOL bThisPatternOK = TRUE;
		for (LONG j = 0; j < nLimit; j++)
		{
			ULONG ulRow = astPatternList[i].m_awHoleRow.GetAt(j);
			ULONG ulCol = astPatternList[i].m_awHoleCol.GetAt(j);

			ULONG ulTopMinRow = dwaRowMinList.GetAt(ulCol - ulMapValidMinCol);
			ULONG ulBtmMaxRow = dwaRowMaxList.GetAt(ulCol - ulMapValidMinCol);

			if (ulRow <= ulTopMinRow || ulRow >= ulBtmMaxRow )
			{
				bThisPatternOK = FALSE;
				break;
			}
		}

		if (bThisPatternOK == FALSE)
		{
			continue;
		}

		ulMapAlnHolesNum += nLimit;
		for (LONG j = 0; j < nLimit; j++)
		{
			ULONG ulRow = astPatternList[i].m_awHoleRow.GetAt(j);
			ULONG ulCol = astPatternList[i].m_awHoleCol.GetAt(j);
			aMapAlnPatnList[ulMapAlnPatterns].m_awHoleRow.Add(ulRow);
			aMapAlnPatnList[ulMapAlnPatterns].m_awHoleCol.Add(ulCol);

			if (!bScnMap)
			{
				SetPrescanTmpPosn(ulRow, ulCol, 0, 0);
			}
		}
		ulMapAlnPatterns++;
		if (ulMapAlnPatterns >= MAX_PTNS)
		{
			break;
		}
	}
}



VOID CWaferPr::LogHoleMapFile(CString szLogPath, const ULONG &ulMapAlnPatterns, SCAN_ALIGN_HOLE_LIST aMapAlnPatnList[MAX_PTNS])
{
	UCHAR ucMapGrade = 1;
	FILE *fpHoleMap = NULL;
	CString szHoleMapFile = szLogPath + "_HoleMap.pmp";
	errno_t nfHoleMapErr = fopen_s(&fpHoleMap, szHoleMapFile, "w");

	if ((nfHoleMapErr == 0) && (fpHoleMap != NULL))
	{
		for (ULONG i = 0; i < ulMapAlnPatterns; i++)
		{
			LONG nLimit = min((LONG)aMapAlnPatnList[i].m_awHoleRow.GetSize(), (LONG)aMapAlnPatnList[i].m_awHoleCol.GetSize());
			for (LONG j = 0; j < nLimit; j++)
			{
				ULONG ulRow = aMapAlnPatnList[i].m_awHoleRow.GetAt(j);
				ULONG ulCol = aMapAlnPatnList[i].m_awHoleCol.GetAt(j);

				LONG lOrgUserRow = 0, lOrgUserCol = 0;
				ConvertAsmToOrgUser(ulRow, ulCol, lOrgUserRow, lOrgUserCol);
				fprintf(fpHoleMap, "%ld,%ld,%d\n", lOrgUserCol, lOrgUserRow, ucMapGrade);
			}
		}

		fclose(fpHoleMap);
	}
}


VOID CWaferPr::LogAlignHoleFile(const errno_t nfHoleErr, FILE *fpHole, const BOOL bScnMap,
								const ULONG ulMapValidMinRow, const ULONG ulMapValidMaxRow, const ULONG ulMapValidMinCol, const ULONG ulMapValidMaxCol, 
								const ULONG ulHoleMinRow, const ULONG ulHoleMaxRow, const ULONG ulHoleMinCol, const ULONG ulHoleMaxCol,
								CDWordArray &dwaRowMinList, CDWordArray &dwaRowMaxList, CDWordArray &dwaColMinList, CDWordArray &dwaColMaxList,
								const ULONG ulMapAlnPatterns, SCAN_ALIGN_HOLE_LIST	aMapAlnPatnList[MAX_PTNS])
{
	if ((nfHoleErr == 0) && (fpHole != NULL))
	{
		if (bScnMap)
		{
			fprintf(fpHole, "\n\n\n");
		}
		fprintf(fpHole, "%3lu,%3lu,%3lu,%3lu\n", ulMapValidMinRow, ulMapValidMinCol, ulMapValidMaxRow, ulMapValidMaxCol);
		fprintf(fpHole, "%3lu,%3lu,%3lu,%3lu\n\n", ulHoleMinRow, ulHoleMinCol, ulHoleMaxRow, ulHoleMaxCol);
		if (bScnMap)
		{
			fprintf(fpHole, "Scn Valid Patterns %lu List\n", ulMapAlnPatterns);
		}
		else
		{
			fprintf(fpHole, "Map Valid Patterns %lu List\n", ulMapAlnPatterns);
		}
		for (ULONG i = 0; i < ulMapAlnPatterns; i++)
		{
			LONG nLimit = min((LONG)aMapAlnPatnList[i].m_awHoleRow.GetSize(), (LONG)aMapAlnPatnList[i].m_awHoleCol.GetSize());
			fprintf(fpHole, "%3lu %3ld ", i+1, nLimit);
			for (LONG j = 0; j < nLimit; j++)
			{
				ULONG ulRow = aMapAlnPatnList[i].m_awHoleRow.GetAt(j);
				ULONG ulCol = aMapAlnPatnList[i].m_awHoleCol.GetAt(j);
				if (j == 0)
				{
					fprintf(fpHole, "(%3lu,%3lu)", ulRow, ulCol);
				}
				else
				{
					fprintf(fpHole, ",(%3lu,%3lu)", ulRow, ulCol);
				}
			}
			fprintf(fpHole, "\n");
		}

		for(ULONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
		{
			ULONG ulFirstCol = dwaColMinList.GetAt(ulRow - ulMapValidMinRow);
			ULONG ulRightCol = dwaColMaxList.GetAt(ulRow - ulMapValidMinRow);
			for(ULONG ulCol = ulMapValidMinCol; ulCol <= ulMapValidMaxCol; ulCol++)
			{
				CString szState = " ";
				ULONG ulTopMinRow = dwaRowMinList.GetAt(ulCol - ulMapValidMinCol);
				ULONG ulBtmMaxRow = dwaRowMaxList.GetAt(ulCol - ulMapValidMinCol);
				if (ulCol < ulFirstCol || ulCol > ulRightCol )
				{
					szState = " ";
				}
				else if (!bScnMap && IsMapExist(ulRow, ulCol) || bScnMap && IsScnExist(ulRow, ulCol))
				{
					szState = "_";
				}
				else
				{
					bool bFind = false;
					for (ULONG i = 0; i < ulMapAlnPatterns; i++)
					{
						LONG nLimit = min((LONG)aMapAlnPatnList[i].m_awHoleRow.GetSize(), (LONG)aMapAlnPatnList[i].m_awHoleCol.GetSize());
						for (LONG j = 0; j < nLimit; j++)
						{
							if (ulRow == aMapAlnPatnList[i].m_awHoleRow.GetAt(j) && ulCol == aMapAlnPatnList[i].m_awHoleCol.GetAt(j))
							{
								bFind = true;
								break;
							}
						}
						if( bFind )
						{
							break;
						}
					}
					if( bFind )
						szState = "1";
					else
						szState = " ";
				}

				fprintf(fpHole, "%s", (LPCTSTR) szState);	//v4.51A20	//Klocwork
			}

			fprintf(fpHole, "\n");
		}
	}
}


LONG CWaferPr::AutoScanAlignWaferWithMap(ULONG ulScnValidMinRow, ULONG ulScnValidMinCol, ULONG ulScnValidMaxRow, ULONG ulScnValidMaxCol, LONG &lMapScnOffsetRow, LONG &lMapScnOffsetCol, CString &szOutMsg)
{
	ULONG ulMapValidMinRow = 0, ulMapValidMaxRow = 0;
	ULONG ulMapValidMinCol = 0, ulMapValidMaxCol = 0;
	GetMapValidSize(ulMapValidMaxRow, ulMapValidMinRow,	ulMapValidMaxCol, ulMapValidMinCol);

	//recalculate the minimum row or column of scan wafer accoring to roigin wafer map
	if (IsMS90HalfSortMode())
	{
		if (IsRowModeSeparateHalfMap())
		{
			ulScnValidMinRow	= ulScnValidMaxRow - (ulMapValidMaxRow - ulMapValidMinRow);
		}
		else
		{
			ulScnValidMinCol	= ulScnValidMaxCol - (ulMapValidMaxCol - ulMapValidMinCol);
		}
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( m_WaferMapWrapper.IsMapValid()==FALSE )
	{
		szOutMsg = "Wafer Map invalid";
		return -1;
	}

	if( pUtl->GetPrescanRegionMode() )	//	auto scan align pattern from map
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		pSRInfo->GetRegion(ulTgtRegion, ulMapValidMinRow, ulMapValidMinCol, ulMapValidMaxRow, ulMapValidMaxCol);
	}

	SCAN_ALIGN_HOLE_LIST	astPatternList[MAX_PTNS];	//	for map and scanned wafer.
	for(INT i=0; i<MAX_PTNS; i++)
	{
		astPatternList[i].m_awHoleRow.RemoveAll();
		astPatternList[i].m_awHoleCol.RemoveAll();
	}
	CDWordArray dwaColMinList, dwaColMaxList, dwaRowMinList, dwaRowMaxList;
	dwaColMinList.RemoveAll();
	dwaColMaxList.RemoveAll();
	dwaRowMinList.RemoveAll();
	dwaRowMaxList.RemoveAll();

	ULONG ulMapAlnPatterns = 0, ulMapAlnHolesNum = 0;
	SCAN_ALIGN_HOLE_LIST	aMapAlnPatnList[MAX_PTNS];
	for(INT i=0; i<MAX_PTNS; i++)
	{
		aMapAlnPatnList[i].m_awHoleRow.RemoveAll();
		aMapAlnPatnList[i].m_awHoleCol.RemoveAll();
	}
	DelPrescanTmpPosn();

	ULONG ulScnAlnPatterns = 0, ulScnAlnHolesNum = 0;
	SCAN_ALIGN_HOLE_LIST	aScnAlnPatnList[MAX_PTNS];
	for(INT i=0; i<MAX_PTNS; i++)
	{
		aScnAlnPatnList[i].m_awHoleRow.RemoveAll();
		aScnAlnPatnList[i].m_awHoleCol.RemoveAll();
	}

	//===============
	// 1  1  1  1  o
	// 1  1  x  x  1  
	// x  x  x  x  x
	// 1  x  x  x  1
	// 1  x  1  x  1
	// 1  1  1  x  1
	// 1  1  0  0  0
	//===============

	ULONG ulHoleMinRow = 9999, ulHoleMinCol = 9999, ulHoleMaxRow = 0, ulHoleMaxCol = 0;
	CDWordArray awHoleListRow, awHoleListCol;
	awHoleListRow.RemoveAll();
	awHoleListCol.RemoveAll();

//	obtain map valid pattern list end
	// first part, initialize data list and get hole from map
	FindMinMaxRowColListInMatrix(FALSE, ulMapValidMinRow, ulMapValidMaxRow, ulMapValidMinCol, ulMapValidMaxCol,
								 dwaRowMinList, dwaRowMaxList, dwaColMinList, dwaColMaxList);


	LONG lTotalHolePoints = FindHoleList(FALSE, ulMapValidMinRow, ulMapValidMaxRow, 
										 dwaColMinList, dwaColMaxList, awHoleListRow, awHoleListCol,
										 ulHoleMinRow, ulHoleMaxRow, ulHoleMinCol, ulHoleMaxCol);
	// second part, need to sort the holes into group
	MarkAllHoleDieFlag(lTotalHolePoints, ulHoleMinRow, ulHoleMinCol, awHoleListRow, awHoleListCol);

	//create hole pattern with actual wafer map coordinate
	ULONG ulFoundPatterns = CreateHolePatterns(ulHoleMinRow, ulHoleMaxRow, ulHoleMinCol, ulHoleMaxCol,
											   awHoleListRow, awHoleListCol, astPatternList);

	// third part, delete patterns that under specification for pattern mode.
	FilterHolePatterns(FALSE, ulMapValidMinCol, dwaRowMinList, dwaRowMaxList, ulFoundPatterns, astPatternList,
					   ulMapAlnPatterns, ulMapAlnHolesNum, aMapAlnPatnList);

	SaveScanTimeEvent("WPR: Obtain map valid pattern done, log to file");
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	CString szHolePmpFile = szLogPath + "_AlignHole.pmp";
	FILE *fpHole = NULL;
	errno_t nfHoleErr = fopen_s(&fpHole, szHolePmpFile, "w");

	LogHoleMapFile(szLogPath, ulMapAlnPatterns, aMapAlnPatnList);

	LogAlignHoleFile(nfHoleErr, fpHole, FALSE,
				    ulMapValidMinRow, ulMapValidMaxRow, ulMapValidMinCol, ulMapValidMaxCol, 
					ulHoleMinRow, ulHoleMaxRow, ulHoleMinCol, ulHoleMaxCol,
					dwaRowMinList, dwaRowMaxList, dwaColMinList, dwaColMaxList,
					ulMapAlnPatterns, aMapAlnPatnList);

	SaveScanTimeEvent("WPR: Obtain map valid pattern all done");
//	obtain map valid pattern list end

	if (ulScnValidMinRow == 0 && ulScnValidMaxRow == 0 && ulScnValidMaxCol == 0 && ulScnValidMinCol == 0)
	{
		szOutMsg = "scan map invalid";
		return -1;
	}

//	obtain scan wafer hole list. begin
	// first part, initialize data list and get hole from map
	FindMinMaxRowColListInMatrix(TRUE, ulScnValidMinRow, ulScnValidMaxRow, ulScnValidMinCol, ulScnValidMaxCol,
								 dwaRowMinList, dwaRowMaxList, dwaColMinList, dwaColMaxList);

	lTotalHolePoints = FindHoleList(TRUE, ulScnValidMinRow, ulScnValidMaxRow, 
									dwaColMinList, dwaColMaxList, awHoleListRow, awHoleListCol,
									ulHoleMinRow, ulHoleMaxRow, ulHoleMinCol, ulHoleMaxCol);

	// second part, need to sort the holes into group
	MarkAllHoleDieFlag(lTotalHolePoints, ulHoleMinRow, ulHoleMinCol, awHoleListRow, awHoleListCol);

	ulFoundPatterns = CreateHolePatterns(ulHoleMinRow, ulHoleMaxRow, ulHoleMinCol, ulHoleMaxCol,
										 awHoleListRow, awHoleListCol, astPatternList);

	lTotalHolePoints = 0;
	awHoleListCol.RemoveAll();
	awHoleListRow.RemoveAll();

	// third part, get valid patterns baded on criteria
	FilterHolePatterns(TRUE, ulScnValidMinCol, dwaRowMinList, dwaRowMaxList, ulFoundPatterns, astPatternList,
					   ulScnAlnPatterns, ulScnAlnHolesNum, aScnAlnPatnList);

	LogAlignHoleFile(nfHoleErr, fpHole, TRUE,
				    ulScnValidMinRow, ulScnValidMaxRow, ulScnValidMinCol, ulScnValidMaxCol, 
					ulHoleMinRow, ulHoleMaxRow, ulHoleMinCol, ulHoleMaxCol,
					dwaRowMinList, dwaRowMaxList, dwaColMinList, dwaColMaxList,
					ulScnAlnPatterns, aScnAlnPatnList);

	SaveScanTimeEvent("WPR: Obtain scan valid pattern list done");
//	obtain scan wafer hole list. end

	ulFoundPatterns = 0;
	for(INT i=0; i<MAX_PTNS; i++)
	{
		astPatternList[i].m_awHoleRow.RemoveAll();
		astPatternList[i].m_awHoleCol.RemoveAll();
	}
	dwaColMinList.RemoveAll();
	dwaColMaxList.RemoveAll();
	dwaRowMinList.RemoveAll();
	dwaRowMaxList.RemoveAll();

//	compare and match the patterns/points between map file and SCN result.
	szOutMsg.Format("Map Patterns %lu, points %lu, Scn Patterns %lu, points %lu.\nPattern points min %ld max %ld, total points min %ld.",
		ulMapAlnPatterns, ulMapAlnHolesNum, ulScnAlnPatterns, ulScnAlnHolesNum,
		m_lScanAlignPatternHolesMin, m_lScanAlignPatternHolesMax, m_lScanAlignTotalHolesMin);

	CString szResultMsg = "3 Total point Low limit";

	if ((nfHoleErr == 0) && (fpHole != NULL))
	{
		fprintf(fpHole, "\n\n\n");
		fprintf(fpHole, "Summary List\n");
		fprintf(fpHole, "%s\n", (LPCTSTR) szOutMsg);
	}

	BOOL bReturn = TRUE;
	if (ulMapAlnHolesNum < (ULONG)m_lScanAlignPatternHolesMin || ulScnAlnHolesNum < (ULONG)m_lScanAlignPatternHolesMin)
	{
		bReturn = FALSE;
	}
	if (ulMapAlnHolesNum < (ULONG)m_lScanAlignTotalHolesMin || ulScnAlnHolesNum < (ULONG)m_lScanAlignTotalHolesMin)
	{
		bReturn = FALSE;
	}
	if (ulMapAlnPatterns < 1 || ulScnAlnPatterns < 1)
	{
		bReturn = FALSE;
	}
	if( bReturn==FALSE )
	{
		for(INT i=0; i<MAX_PTNS; i++)
		{
			aMapAlnPatnList[i].m_awHoleRow.RemoveAll();
			aMapAlnPatnList[i].m_awHoleCol.RemoveAll();
		}
		for(INT i=0; i<MAX_PTNS; i++)
		{
			aScnAlnPatnList[i].m_awHoleRow.RemoveAll();
			aScnAlnPatnList[i].m_awHoleCol.RemoveAll();
		}
		if( fpHole!=NULL )
		{
			fprintf(fpHole, "\n\n\n");
			fprintf(fpHole, "%s\n", (LPCTSTR) szResultMsg);
			fclose(fpHole);
		}
		szOutMsg = "get points and pattern less than limit.";
		return -1;
	}

//	get top 2 matched list
	SCAN_ALIGN_TOP_LIST	aAlignTop3[3];
	for(int i=0; i<3; i++)
	{
		aAlignTop3[i].m_lMatchPoints = 0;
		aAlignTop3[i].m_lMapAlnRow = 0;
		aAlignTop3[i].m_lMapAlnCol = 0;
		aAlignTop3[i].m_lScnAlnRow = 0;
		aAlignTop3[i].m_lScnAlnCol = 0;
	}

	for(ULONG ulMapPtnIdx=0; ulMapPtnIdx<ulMapAlnPatterns; ulMapPtnIdx++)
	{
		ULONG ulMapPtnHoles = (ULONG)min((LONG)aMapAlnPatnList[ulMapPtnIdx].m_awHoleCol.GetSize(), (LONG)aMapAlnPatnList[ulMapPtnIdx].m_awHoleRow.GetSize());
		for(ULONG ulMapPtnHoleIdx = 0; ulMapPtnHoleIdx<ulMapPtnHoles; ulMapPtnHoleIdx++)
		{
			LONG lMapCheckRow = aMapAlnPatnList[ulMapPtnIdx].m_awHoleRow.GetAt(ulMapPtnHoleIdx);
			LONG lMapCheckCol = aMapAlnPatnList[ulMapPtnIdx].m_awHoleCol.GetAt(ulMapPtnHoleIdx);

			for(ULONG ulScnPtnIdx = 0; ulScnPtnIdx < ulScnAlnPatterns; ulScnPtnIdx++)
			{
				ULONG ulScnPtnHoles = min((ULONG)aScnAlnPatnList[ulScnPtnIdx].m_awHoleCol.GetSize(), (ULONG)aScnAlnPatnList[ulScnPtnIdx].m_awHoleRow.GetSize());
				for(ULONG ulScnPtnHoleIdx=0; ulScnPtnHoleIdx<ulScnPtnHoles; ulScnPtnHoleIdx++)
				{
					LONG lScnCheckRow = aScnAlnPatnList[ulScnPtnIdx].m_awHoleRow.GetAt(ulScnPtnHoleIdx);
					LONG lScnCheckCol = aScnAlnPatnList[ulScnPtnIdx].m_awHoleCol.GetAt(ulScnPtnHoleIdx);

					LONG lMatchPoints = 0;
					for(ULONG ulScnPtnLoop=ulScnPtnIdx; ulScnPtnLoop<ulScnAlnPatterns; ulScnPtnLoop++)
					{
						ULONG ulLoopPtnHoles = ulLoopPtnHoles = (ULONG)min((LONG)aScnAlnPatnList[ulScnPtnLoop].m_awHoleCol.GetSize(), (LONG)aScnAlnPatnList[ulScnPtnLoop].m_awHoleRow.GetSize());
						ULONG ulLoopPtnStartHole = 0;
						if( ulScnPtnLoop==ulScnPtnIdx )
							ulLoopPtnStartHole = ulScnPtnHoleIdx;
						for(ULONG ulScnPtnHoleLoop=ulLoopPtnStartHole; ulScnPtnHoleLoop<ulLoopPtnHoles; ulScnPtnHoleLoop++)
						{
							LONG lScnPointRow = aScnAlnPatnList[ulScnPtnLoop].m_awHoleRow.GetAt(ulScnPtnHoleLoop);
							LONG lScnPointCol = aScnAlnPatnList[ulScnPtnLoop].m_awHoleCol.GetAt(ulScnPtnHoleLoop);

							LONG lMapPointRow = lScnPointRow + lMapCheckRow - lScnCheckRow;
							LONG lMapPointCol = lScnPointCol + lMapCheckCol - lScnCheckCol;

							if ((lMapPointRow >= 0) && (lMapPointCol >= 0))
							{
								LONG encX = 0, encY = 0;
								if (GetPrescanTmpPosn(lMapPointRow, lMapPointCol, encX, encY))
								{
									lMatchPoints++;
								}
							}
							else
							{
								lMapPointRow = lMapPointRow;
							}
						}
					}

					// > max
					if( lMatchPoints>aAlignTop3[0].m_lMatchPoints )
					{
						// min
						aAlignTop3[1].m_lMatchPoints= aAlignTop3[0].m_lMatchPoints;
						aAlignTop3[1].m_lMapAlnRow	= aAlignTop3[0].m_lMapAlnRow;
						aAlignTop3[1].m_lMapAlnCol	= aAlignTop3[0].m_lMapAlnCol;
						aAlignTop3[1].m_lScnAlnRow	= aAlignTop3[0].m_lScnAlnRow;
						aAlignTop3[1].m_lScnAlnCol	= aAlignTop3[0].m_lScnAlnCol;
						// max
						aAlignTop3[0].m_lMatchPoints= lMatchPoints;
						aAlignTop3[0].m_lMapAlnRow	= lMapCheckRow;
						aAlignTop3[0].m_lMapAlnCol	= lMapCheckCol;
						aAlignTop3[0].m_lScnAlnRow	= lScnCheckRow;
						aAlignTop3[0].m_lScnAlnCol	= lScnCheckCol;
					}	// == max
					else if( lMatchPoints==aAlignTop3[0].m_lMatchPoints )
					{
						// max different, update to min.
						if( aAlignTop3[0].m_lMapAlnRow-aAlignTop3[0].m_lScnAlnRow!=lMapCheckRow-lScnCheckRow ||
							aAlignTop3[0].m_lMapAlnCol-aAlignTop3[0].m_lScnAlnCol!=lMapCheckCol-lScnCheckCol )
						{
							aAlignTop3[1].m_lMatchPoints= lMatchPoints;
							aAlignTop3[1].m_lMapAlnRow	= lMapCheckRow;
							aAlignTop3[1].m_lMapAlnCol	= lMapCheckCol;
							aAlignTop3[1].m_lScnAlnRow	= lScnCheckRow;
							aAlignTop3[1].m_lScnAlnCol	= lScnCheckCol;
						}
					}
					else
					{
						// > min
						if( lMatchPoints>aAlignTop3[1].m_lMatchPoints )
						{
							aAlignTop3[1].m_lMatchPoints= lMatchPoints;
							aAlignTop3[1].m_lMapAlnRow	= lMapCheckRow;
							aAlignTop3[1].m_lMapAlnCol	= lMapCheckCol;
							aAlignTop3[1].m_lScnAlnRow	= lScnCheckRow;
							aAlignTop3[1].m_lScnAlnCol	= lScnCheckCol;
						}
					}
				}
			}
		}
	}

	for(INT i=0; i<MAX_PTNS; i++)
	{
		aMapAlnPatnList[i].m_awHoleRow.RemoveAll();
		aMapAlnPatnList[i].m_awHoleCol.RemoveAll();
		aScnAlnPatnList[i].m_awHoleRow.RemoveAll();
		aScnAlnPatnList[i].m_awHoleCol.RemoveAll();
	}
	DelPrescanTmpPosn();
	SaveScanTimeEvent("WPR: Get Top 2 Match List Done");
//	ULONG ulMaxAlnPatterns = min(ulMapAlnPatterns, ulScnAlnPatterns);
//	DOUBLE dAlgnPatternsMin		= ((DOUBLE)ulMaxAlnPatterns*m_dSAMatchPatternsPercent/100.0);

	LONG ulMaxMatchPoints = aAlignTop3[0].m_lMatchPoints;
	ULONG ulMaxAlnHolesNum = min(ulMapAlnHolesNum, ulScnAlnHolesNum);
	DOUBLE dAlgnHoleMinPoints	= ((DOUBLE)ulMaxAlnHolesNum * m_dScanAlignMatchLowPercent / 100.0);

	LONG lPatternMatchCount = 0;
	if (ulMaxMatchPoints > 0 && ulMaxAlnHolesNum > 0 && ulMaxMatchPoints >= dAlgnHoleMinPoints)
	{
		if( ulMaxMatchPoints>aAlignTop3[1].m_lMatchPoints )
		{
			lPatternMatchCount = 1;
			lMapScnOffsetRow = aAlignTop3[0].m_lMapAlnRow - aAlignTop3[0].m_lScnAlnRow;
			lMapScnOffsetCol = aAlignTop3[0].m_lMapAlnCol - aAlignTop3[0].m_lScnAlnCol;
		}
		else if( ulMaxMatchPoints==aAlignTop3[1].m_lMatchPoints )
		{
			if( aAlignTop3[0].m_lMapAlnRow - aAlignTop3[0].m_lScnAlnRow==aAlignTop3[1].m_lMapAlnRow - aAlignTop3[1].m_lScnAlnRow &&
				aAlignTop3[0].m_lMapAlnCol - aAlignTop3[0].m_lScnAlnCol==aAlignTop3[1].m_lMapAlnCol - aAlignTop3[1].m_lScnAlnCol )
			{
				lPatternMatchCount = 1;
				lMapScnOffsetRow = aAlignTop3[0].m_lMapAlnRow - aAlignTop3[0].m_lScnAlnRow;
				lMapScnOffsetCol = aAlignTop3[0].m_lMapAlnCol - aAlignTop3[0].m_lScnAlnCol;
			}
			else
			{
				lPatternMatchCount = 2;
			}
		}
	}

	szResultMsg = "1 Align ok";
	szOutMsg = "";
	if( lPatternMatchCount==0 )
	{
		DOUBLE dYield = 0;
		if( ulMaxAlnHolesNum>0 )
			dYield = (DOUBLE)ulMaxMatchPoints/(DOUBLE)ulMaxAlnHolesNum*100.0;
		szOutMsg.Format("Scan align wafer via map pattern wrong.\nMax matched points %d, map points %d.\nScore %.2f < Low limit %.2f",
			ulMaxMatchPoints, ulMaxAlnHolesNum, dYield, m_dScanAlignMatchLowPercent);
		szResultMsg = "2 Yield Low limit";
	}
	else if( lPatternMatchCount>1 )
	{
		szOutMsg.Format("Scan align wafer via map pattern wrong.\nFound points %d(cases %d), map points %d",
			ulMaxMatchPoints, lPatternMatchCount, ulMaxAlnHolesNum);
		szResultMsg = "4 cases2";
	}	// check and message.

	if ((nfHoleErr == 0) && (fpHole != NULL))
	{
		fprintf(fpHole, "\n\n\n");
		fprintf(fpHole, "Scan Align Match List\n");
		for(int i=0; i<2; i++)
		{
			if( aAlignTop3[i].m_lMatchPoints<=0 )
			{
				continue;
			}
			LONG lOffsetRow = aAlignTop3[i].m_lMapAlnRow - aAlignTop3[i].m_lScnAlnRow;
			LONG lOffsetCol = aAlignTop3[i].m_lMapAlnCol - aAlignTop3[i].m_lScnAlnCol;
			fprintf(fpHole, "matched points %ld, map %ld,%ld, scn %ld,%ld, offset %ld,%ld\n", 
				aAlignTop3[i].m_lMatchPoints, 
				aAlignTop3[i].m_lMapAlnRow, aAlignTop3[i].m_lMapAlnCol,
				aAlignTop3[i].m_lScnAlnRow, aAlignTop3[i].m_lScnAlnCol,
				lOffsetRow,					lOffsetCol	);

			for(ULONG ulMapPtnIdx=0; ulMapPtnIdx<ulMapAlnPatterns; ulMapPtnIdx++)
			{
				ULONG ulMapPtnHoles = min((ULONG)aMapAlnPatnList[ulMapPtnIdx].m_awHoleCol.GetSize(), (ULONG)aMapAlnPatnList[ulMapPtnIdx].m_awHoleRow.GetSize());
				for(ULONG ulMapPtnHoleIdx=0; ulMapPtnHoleIdx<ulMapPtnHoles; ulMapPtnHoleIdx++)
				{
					LONG lMapCheckRow = aMapAlnPatnList[ulMapPtnIdx].m_awHoleRow.GetAt(ulMapPtnHoleIdx);
					LONG lMapCheckCol = aMapAlnPatnList[ulMapPtnIdx].m_awHoleCol.GetAt(ulMapPtnHoleIdx);

					for(ULONG ulScnPtnIdx=0; ulScnPtnIdx<ulScnAlnPatterns; ulScnPtnIdx++)
					{
						ULONG ulScnPtnHoles = min((ULONG)aScnAlnPatnList[ulScnPtnIdx].m_awHoleCol.GetSize(), (ULONG)aScnAlnPatnList[ulScnPtnIdx].m_awHoleRow.GetSize());
						for(ULONG ulScnPtnHoleIdx=0; ulScnPtnHoleIdx<ulScnPtnHoles; ulScnPtnHoleIdx++)
						{
							LONG lScnCheckRow = aScnAlnPatnList[ulScnPtnIdx].m_awHoleRow.GetAt(ulScnPtnHoleIdx);
							LONG lScnCheckCol = aScnAlnPatnList[ulScnPtnIdx].m_awHoleCol.GetAt(ulScnPtnHoleIdx);

							if( (lMapCheckRow-lScnCheckRow)==lOffsetRow && (lMapCheckCol-lScnCheckCol)==lOffsetCol )
							{
								fprintf(fpHole, "map ptn,%3lu,%3lu,%3lu,		scn ptn,%3lu,%3lu,%3lu\n",
									ulMapPtnIdx+1, lMapCheckRow, lMapCheckCol, ulScnPtnIdx+1, lScnCheckRow, lScnCheckCol);
							}
						}
					}
				}
			}
		}

		if( lPatternMatchCount!=1 )
		{
			fprintf(fpHole, "\n\n\n");
			fprintf(fpHole, "%s\n", (LPCTSTR) szOutMsg);
		}
		fprintf(fpHole, "\n\n\n");
		fprintf(fpHole, "%s\n", (LPCTSTR) szResultMsg);
		fprintf(fpHole, "at last %ld,%ld\n", lMapScnOffsetRow, lMapScnOffsetCol);
		fclose(fpHole);
	}

	return	lPatternMatchCount;
}


LONG CWaferPr::ScanAlignPatternFromMapTest(IPC_CServiceMessage &svMsg)
{
	if( IsScanAlignWafer() && m_bScanAlignAutoFromMap )
	{
		CString szMsg;
		LONG lMapScnOffsetRow = 0, lMapScnOffsetCol = 0;
//		szMsg.Format("WPR: test auto sort scan align hole die from map: Pattern points min %ld max %ld, total points min %ld. holes per %f",
//			m_lScanAlignPatternHolesMin, m_lScanAlignPatternHolesMax, m_lScanAlignTotalHolesMin, m_dScanAlignMatchLowPercent);
//		SaveScanTimeEvent(szMsg);
//		szMsg = "";
		AutoScanAlignWaferWithMap(0, 0, 0, 0, lMapScnOffsetRow, lMapScnOffsetCol, szMsg);
		SaveScanTimeEvent("WPR: Auto Scan Align Wafer WithMap done " + szMsg);
		HmiMessage("Auto scan align pattern from map tested");
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::AddScanAlignPattern4MS90(IPC_CServiceMessage &svMsg)
{
	if( IsScanAlignWafer() && m_bScanAlignFromFile && m_ulScanAlignTotalPoints>0 )
	{
		CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
		CString szLogPath;
		pUtl->GetPrescanLogPath(szLogPath);
		FILE *fpMap = NULL;
		CString szHolePmpFile = szLogPath + "_Hole.txt";	//	Add hole pattern for dummy MS90
		errno_t nErr = fopen_s(&fpMap, szHolePmpFile, "a");
		UCHAR ucNullBin = m_WaferMapWrapper.GetNullBin();
		LONG  lUserRow = 0, lUserCol = 0;
		ULONG ulAsmRow = 0, ulAsmCol = 0;
		if ((nErr == 0) && (fpMap != NULL))
		{
			fprintf(fpMap, "0. Add dummy map hole points for scan align\n");
		}

		for(ULONG i=0; i<m_ulScanAlignTotalPoints; i++)
		{
			if (i >= (ULONG)m_awPointsListRow.GetSize())
			{
				break;
			}
			lUserRow = m_awPointsListRow.GetAt(i);
			lUserCol = m_awPointsListCol.GetAt(i);
			//	m_ucPointsListSta.GetAt(i);
			if( ConvertOrgUserToAsm(lUserRow, lUserCol, ulAsmRow, ulAsmCol) )
			{
				m_WaferMapWrapper.ChangeGrade(ulAsmRow, ulAsmCol, ucNullBin);
				if ((nErr == 0) && (fpMap != NULL))
				{
					fprintf(fpMap, "empty %3lu(%3lu,%3lu)[%3ld,%3ld]\n", i+1, ulAsmRow, ulAsmCol, lUserRow, lUserCol);
				}
			}
		}

		if ((nErr == 0) && (fpMap != NULL))
		{
			fprintf(fpMap, "\n");
			fclose(fpMap);
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}	//	function to add empty hole when map loaded.

BOOL CWaferPr::CheckHolesInWaferWithMap(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, CString &szOutMsg)
{
	szOutMsg = "";
	if( m_WaferMapWrapper.IsMapValid()==FALSE )
	{
		szOutMsg = "Wafer Map invalid";
		return FALSE;
	}

	if(ulScnMinRow==0 && ulScnMaxRow==0 && ulScnMaxCol==0 && ulScnMinCol==0 )
	{
		szOutMsg = "scan map invalid";
		return FALSE;
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
//	#define	MAX_PTNS		500

	ULONG ulMapValidMinRow = 0, ulMapValidMaxRow = 0;
	ULONG ulMapValidMinCol = 0, ulMapValidMaxCol = 0;
	GetMapValidSize(ulMapValidMaxRow, ulMapValidMinRow,	ulMapValidMaxCol, ulMapValidMinCol);

	if( pUtl->GetPrescanRegionMode() )	//	auto scan align pattern from map
	{
		WT_CSubRegionsInfo *pSRInfo = WT_CSubRegionsInfo::Instance();
		ULONG ulTgtRegion = pSRInfo->GetTargetRegion();
		pSRInfo->GetRegion(ulTgtRegion, ulMapValidMinRow, ulMapValidMinCol, ulMapValidMaxRow, ulMapValidMaxCol);
	}

	ULONG ulFoundPatterns = 0;
	SCAN_ALIGN_HOLE_LIST	astPatternList[MAX_PTNS];	//	for map and scanned wafer.
	for(INT i=0; i<MAX_PTNS; i++)
	{
		astPatternList[i].m_awHoleRow.RemoveAll();
		astPatternList[i].m_awHoleCol.RemoveAll();
	}

	ULONG	m_ulMapAlnPatterns = 0;
	SCAN_ALIGN_HOLE_LIST	m_aMapAlnPatnList[MAX_PTNS];
	ULONG ulMapAlnHolesNum = 0;
	for(INT i=0; i<MAX_PTNS; i++)
	{
		m_aMapAlnPatnList[i].m_awHoleRow.RemoveAll();
		m_aMapAlnPatnList[i].m_awHoleCol.RemoveAll();
	}
	DelPrescanTmpPosn();

	//===============
	// 1  1  1  1  o
	// 1  1  x  x  1  
	// x  x  x  x  x
	// 1  x  x  x  1
	// 1  x  1  x  1
	// 1  1  1  x  1
	// 1  1  0  0  0
	//===============

	ULONG ulHoleMinRow = 9999, ulHoleMinCol = 9999, ulHoleMaxRow = 0, ulHoleMaxCol = 0;
	LONG lTotalHolePoints = 0;
	CDWordArray awHoleListRow, awHoleListCol;
	awHoleListRow.RemoveAll();
	awHoleListCol.RemoveAll();

//	obtain map valid pattern list end
	CDWordArray dwaColMinList, dwaColMaxList, dwaRowMinList, dwaRowMaxList;
	dwaColMinList.RemoveAll();
	dwaColMaxList.RemoveAll();
	dwaRowMinList.RemoveAll();
	dwaRowMaxList.RemoveAll();
	// first part, initialize data list and get hole from map
	for(ULONG ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
	{
		ULONG ulMinRow = ulMapValidMinRow;
		ULONG ulMaxRow = ulMapValidMaxRow;
		for(ULONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
		{
			if( IsMapExist(ulRow, ulCol) )
			{
				ulMinRow = ulRow;
				break;
			}
		}

		for(LONG ulRow=(LONG)ulMapValidMaxRow; ulRow>=(LONG)ulMapValidMinRow; ulRow--)
		{
			if( IsMapExist(ulRow, ulCol) )
			{
				ulMaxRow = ulRow;
				break;
			}
		}
		dwaRowMinList.Add(ulMinRow);
		dwaRowMaxList.Add(ulMaxRow);
	}

	for(ULONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
	{
		ULONG ulMinCol = ulMapValidMinCol;
		ULONG ulMaxCol = ulMapValidMaxCol;
		for(ULONG ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
		{
			if( IsMapExist(ulRow, ulCol) )
			{
				ulMinCol = ulCol;
				break;
			}
		}
		for(LONG ulCol=(LONG)ulMapValidMaxCol; ulCol>=(LONG)ulMapValidMinCol; ulCol--)
		{
			if( IsMapExist(ulRow, ulCol) )
			{
				ulMaxCol = ulCol;
				break;
			}
		}
		dwaColMinList.Add(ulMinCol);
		dwaColMaxList.Add(ulMaxCol);
	}

	for(ULONG ulRow=ulMapValidMinRow+1; ulRow<ulMapValidMaxRow; ulRow++)
	{
		ULONG ulFirstCol = dwaColMinList.GetAt(ulRow-ulMapValidMinRow);
		ULONG ulRightCol = dwaColMaxList.GetAt(ulRow-ulMapValidMinRow);

		for(ULONG ulCol=ulFirstCol; ulCol<=ulRightCol; ulCol++)
		{
			if( IsMapExist(ulRow, ulCol) )
			{
				continue;
			}

			awHoleListRow.Add(ulRow);
			awHoleListCol.Add(ulCol);
			if( ulHoleMinRow>ulRow )
				ulHoleMinRow = ulRow;
			if( ulHoleMinCol>ulCol )
				ulHoleMinCol = ulCol;
			if( ulHoleMaxRow<ulRow )
				ulHoleMaxRow = ulRow;
			if( ulHoleMaxCol<ulCol )
				ulHoleMaxCol = ulCol;
			lTotalHolePoints++;
		}
	}

	// second part, need to sort the holes into group
	for(INT i=0; i<MAX_MAP_DIM; i++)
	{
		for(INT j=0; j<MAX_MAP_DIM; j++)
		{
			m_ucaMatrix[i][j] = 0;
		}
	}
	for(LONG ulLoop=0; ulLoop<lTotalHolePoints; ulLoop++)
	{
		ULONG ulRow = awHoleListRow.GetAt(ulLoop) - ulHoleMinRow + 1;
		ULONG ulCol = awHoleListCol.GetAt(ulLoop) - ulHoleMinCol + 1;
		ulRow = min(ulRow, MAX_MAP_DIM-1);
		ulCol = min(ulCol, MAX_MAP_DIM-1);
		m_ucaMatrix[ulRow][ulCol] = 1;
	}

	ULONG ulRowLoop = 1;
	if( ulHoleMaxRow>=ulHoleMinRow )
		ulRowLoop = ulHoleMaxRow - ulHoleMinRow + 1;
	ULONG ulColLoop = 1;
	if( ulHoleMaxCol>=ulHoleMinCol )
		ulColLoop = ulHoleMaxCol - ulHoleMinCol + 1;
	ulRowLoop = min(ulRowLoop, MAX_MAP_DIM-2);
	ulColLoop = min(ulColLoop, MAX_MAP_DIM-2);

	for(ULONG ulRow=1; ulRow<=ulRowLoop; ulRow++)
	{
		for(ULONG ulCol=1; ulCol<=ulColLoop; ulCol++)
		{
			if (m_ucaMatrix[ulRow][ulCol] == 0)
				continue;

			if( ulFoundPatterns==0 )
			{
				astPatternList[ulFoundPatterns].m_awHoleRow.Add(ulRow + ulHoleMinRow - 1);
				astPatternList[ulFoundPatterns].m_awHoleCol.Add(ulCol + ulHoleMinCol - 1);
				ulFoundPatterns++;
				m_ucaMatrix[ulRow][ulCol] = 0;
				continue;
			}

			while( 1 )
			{
				BOOL bNewUpdated = FALSE;
				for(ULONG i=0; i<ulFoundPatterns; i++)
				{
					LONG nLimit = min((LONG)astPatternList[i].m_awHoleRow.GetSize(), (LONG)astPatternList[i].m_awHoleCol.GetSize());
					for(LONG j=0; j<nLimit; j++)
					{
						ULONG ulPointRow = astPatternList[i].m_awHoleRow.GetAt(j);
						ULONG ulPointCol = astPatternList[i].m_awHoleCol.GetAt(j);
						ULONG ulCheckRow = ulPointRow - ulHoleMinRow + 1;
						ULONG ulCheckCol = ulPointCol - ulHoleMinCol + 1;

						ulCheckRow = min(ulCheckRow, MAX_MAP_DIM-2);
						ulCheckCol = min(ulCheckCol, MAX_MAP_DIM-2);
						ulCheckRow = max(ulCheckRow, 1);
						ulCheckCol = max(ulCheckCol, 1);
						if (m_ucaMatrix[ulCheckRow-1][ulCheckCol] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow-1);
							astPatternList[i].m_awHoleCol.Add(ulPointCol);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow-1][ulCheckCol] = 0;
						}
						if (m_ucaMatrix[ulCheckRow+1][ulCheckCol] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow+1);
							astPatternList[i].m_awHoleCol.Add(ulPointCol);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow+1][ulCheckCol] = 0;
						}
						if (m_ucaMatrix[ulCheckRow][ulCheckCol-1] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow);
							astPatternList[i].m_awHoleCol.Add(ulPointCol-1);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow][ulCheckCol-1] = 0;
						}
						if (m_ucaMatrix[ulCheckRow][ulCheckCol+1] == 1)
						{
							astPatternList[i].m_awHoleRow.Add(ulPointRow);
							astPatternList[i].m_awHoleCol.Add(ulPointCol+1);
							bNewUpdated = TRUE;
							m_ucaMatrix[ulCheckRow][ulCheckCol+1] = 0;
						}
					}
				}
				if( bNewUpdated==FALSE )
				{
					break;
				}
			}

			if (m_ucaMatrix[ulRow][ulCol] == 1)
			{
				astPatternList[ulFoundPatterns].m_awHoleRow.Add(ulRow + ulHoleMinRow - 1);
				astPatternList[ulFoundPatterns].m_awHoleCol.Add(ulCol + ulHoleMinCol - 1);
				ulFoundPatterns++;
				m_ucaMatrix[ulRow][ulCol] = 0;
			}

			if( ulFoundPatterns>=MAX_PTNS )
			{
				break;
			}
		}
		if( ulFoundPatterns>=MAX_PTNS )
		{
			break;
		}
	}

	// third part, delete patterns that under specification for pattern mode.
	for(ULONG i=0; i<ulFoundPatterns; i++)
	{
		LONG nLimit = min((LONG)astPatternList[i].m_awHoleRow.GetSize(), (LONG)astPatternList[i].m_awHoleCol.GetSize());
		if (nLimit < m_lScanAlignPatternHolesMin)
		{
			continue;
		}
		if (m_lScanAlignPatternHolesMax > m_lScanAlignPatternHolesMin && nLimit > m_lScanAlignPatternHolesMax)
		{
			continue;
		}
		BOOL bThisPatternOK = TRUE;
		for(LONG j=0; j<nLimit; j++)
		{
			ULONG ulRow = astPatternList[i].m_awHoleRow.GetAt(j);
			ULONG ulCol = astPatternList[i].m_awHoleCol.GetAt(j);

			ULONG ulTopMinRow = dwaRowMinList.GetAt(ulCol-ulMapValidMinCol);
			ULONG ulBtmMaxRow = dwaRowMaxList.GetAt(ulCol-ulMapValidMinCol);

			if( ulRow<=ulTopMinRow || ulRow>=ulBtmMaxRow )
			{
				bThisPatternOK = FALSE;
				break;
			}
		}
		if( bThisPatternOK == FALSE )
		{
			continue;
		}

		ulMapAlnHolesNum += nLimit;
		for(LONG j=0; j<nLimit; j++)
		{
			ULONG ulRow = astPatternList[i].m_awHoleRow.GetAt(j);
			ULONG ulCol = astPatternList[i].m_awHoleCol.GetAt(j);
			m_aMapAlnPatnList[m_ulMapAlnPatterns].m_awHoleRow.Add(ulRow);
			m_aMapAlnPatnList[m_ulMapAlnPatterns].m_awHoleCol.Add(ulCol);
			SetPrescanTmpPosn(ulRow, ulCol, 0, 0);
		}
		m_ulMapAlnPatterns++;
		if( m_ulMapAlnPatterns>=MAX_PTNS )
		{
			break;
		}
	}

	SaveScanTimeEvent("WPR: Obtain map valid pattern done, log to file");
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	CString szHolePmpFile = szLogPath + "_AlignHole.pmp";
	FILE *fpHole = NULL;
	errno_t nErr = fopen_s(&fpHole, szHolePmpFile, "w");
	UCHAR ucMapGrade = 1;

	if ((nErr == 0) && (fpHole != NULL))
	{
		fprintf(fpHole, "%3lu,%3lu,%3lu,%3lu\n", ulMapValidMinRow, ulMapValidMinCol, ulMapValidMaxRow, ulMapValidMaxCol);
		fprintf(fpHole, "%3lu,%3lu,%3lu,%3lu\n\n", ulHoleMinRow, ulHoleMinCol, ulHoleMaxRow, ulHoleMaxCol);
		fprintf(fpHole, "Map Valid Patterns %lu List\n", m_ulMapAlnPatterns);	//v4.49 Klocwork
		for(ULONG i=0; i<m_ulMapAlnPatterns; i++)
		{
			LONG nLimit = min((LONG)m_aMapAlnPatnList[i].m_awHoleRow.GetSize(), (LONG)m_aMapAlnPatnList[i].m_awHoleCol.GetSize());
			fprintf(fpHole, "%3lu %3ld ", i+1, nLimit);
			for(LONG j=0; j<nLimit; j++)
			{
				ULONG ulRow = m_aMapAlnPatnList[i].m_awHoleRow.GetAt(j);
				ULONG ulCol = m_aMapAlnPatnList[i].m_awHoleCol.GetAt(j);
				if( j==0 )
					fprintf(fpHole, "(%3lu,%3lu)", ulRow, ulCol);
				else
					fprintf(fpHole, ",(%3lu,%3lu)", ulRow, ulCol);
			}
			fprintf(fpHole, "\n");
		}

		for(ULONG ulRow=ulMapValidMinRow; ulRow<=ulMapValidMaxRow; ulRow++)
		{
			ULONG ulFirstCol = dwaColMinList.GetAt(ulRow-ulMapValidMinRow);
			ULONG ulRightCol = dwaColMaxList.GetAt(ulRow-ulMapValidMinRow);
			for(ULONG ulCol=ulMapValidMinCol; ulCol<=ulMapValidMaxCol; ulCol++)
			{
				CString szState = " ";
				ULONG ulTopMinRow = dwaRowMinList.GetAt(ulCol-ulMapValidMinCol);
				ULONG ulBtmMaxRow = dwaRowMaxList.GetAt(ulCol-ulMapValidMinCol);
				if( ulCol<ulFirstCol || ulCol>ulRightCol )
				{
					szState = " ";
				}
				else if( IsMapExist(ulRow, ulCol) )
				{
					szState = "_";
				}
				else
				{
					bool bFind = false;
					for(ULONG i=0; i<m_ulMapAlnPatterns; i++)
					{
						LONG nLimit = min((LONG)m_aMapAlnPatnList[i].m_awHoleRow.GetSize(), (LONG)m_aMapAlnPatnList[i].m_awHoleCol.GetSize());
						for(LONG j=0; j<nLimit; j++)
						{
							if( ulRow == m_aMapAlnPatnList[i].m_awHoleRow.GetAt(j) && ulCol == m_aMapAlnPatnList[i].m_awHoleCol.GetAt(j) )
							{
								bFind = true;
								break;
							}
						}
						if( bFind )
						{
							break;
						}
					}
					if( bFind )
						szState = "1";
					else
						szState = " ";
				}
				fprintf(fpHole, "%s", (LPCTSTR) szState);	//v4.51A20	//Klocwork
			}
			fprintf(fpHole, "\n");
		}
	}

	SaveScanTimeEvent("WPR: Obtain map valid pattern all done");
	for(INT i=0; i<MAX_PTNS; i++)
	{
		astPatternList[i].m_awHoleRow.RemoveAll();
		astPatternList[i].m_awHoleCol.RemoveAll();
	}
	dwaColMinList.RemoveAll();
	dwaColMaxList.RemoveAll();
	dwaRowMinList.RemoveAll();
	dwaRowMaxList.RemoveAll();
	//	obtain map valid pattern list end

	//	compare and match the patterns/points between map file and SCN result.
	CString szMsg;
	szMsg.Format("Map Patterns %lu, points %lu\nPattern points min %ld max %ld, total points min %ld.",
		m_ulMapAlnPatterns, ulMapAlnHolesNum, m_lScanAlignPatternHolesMin, m_lScanAlignPatternHolesMax, m_lScanAlignTotalHolesMin);

	if( fpHole!=NULL )
	{
		fprintf(fpHole, "\n\n\n");
		fprintf(fpHole, "Summary List\n");
		fprintf(fpHole, "%s\n", (LPCTSTR) szMsg);
	}

	if( (ulMapAlnHolesNum<(ULONG)m_lScanAlignPatternHolesMin) ||
		(ulMapAlnHolesNum<(ULONG)m_lScanAlignTotalHolesMin) )
	{
		for(INT i=0; i<MAX_PTNS; i++)
		{
			m_aMapAlnPatnList[i].m_awHoleRow.RemoveAll();
			m_aMapAlnPatnList[i].m_awHoleCol.RemoveAll();
		}
		if( fpHole!=NULL )
		{
			fprintf(fpHole, "\n\n\n");
			fclose(fpHole);
		}
		szOutMsg = "Hole or Pattern < limit.";
		return FALSE;
	}

	ULONG ulHasDieInHoles = 0;
	for(ULONG ulMapPtnIdx=0; ulMapPtnIdx<m_ulMapAlnPatterns; ulMapPtnIdx++)
	{
		ULONG ulMapPtnHoles = min((ULONG)m_aMapAlnPatnList[ulMapPtnIdx].m_awHoleCol.GetSize(), (ULONG)m_aMapAlnPatnList[ulMapPtnIdx].m_awHoleRow.GetSize());
		for(ULONG ulMapPtnHoleIdx=0; ulMapPtnHoleIdx<ulMapPtnHoles; ulMapPtnHoleIdx++)
		{
			LONG lPointMapRow = m_aMapAlnPatnList[ulMapPtnIdx].m_awHoleRow.GetAt(ulMapPtnHoleIdx);
			LONG lPointMapCol = m_aMapAlnPatnList[ulMapPtnIdx].m_awHoleCol.GetAt(ulMapPtnHoleIdx);

			LONG lPointScnRow = lPointMapRow - m_lMapScanOffsetRow;
			LONG lPointScnCol = lPointMapCol - m_lMapScanOffsetCol;

			// Outof range, fail
			if( lPointScnRow<(LONG)ulScnMinRow || lPointScnRow>(LONG)ulScnMaxRow ||
				lPointScnCol<(LONG)ulScnMinCol || lPointScnCol>(LONG)ulScnMaxCol )
			{
				continue;
			}

			// has die on it, fail
			if( IsScnExist(lPointScnRow, lPointScnCol) )
			{
				ulHasDieInHoles++;
				CString szPiece = "";
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(lPointMapRow, lPointMapCol, lUserRow, lUserCol);
				szPiece.Format("Map row %ld(%ld), column %ld(%ld) has scanned die on wafer\n",
					lUserRow, lPointMapRow, lUserCol, lPointMapCol);
				if( fpHole!=NULL )
				{
					fprintf(fpHole, "%s", szPiece);
				}
				szPiece.Format("Map %4ld,%4ld has scanned die on wafer\n", lUserRow, lUserCol);
				szOutMsg += szPiece;
			}
		}
	}
	if( fpHole!=NULL )
	{
		fclose(fpHole);
	}


	for(INT i=0; i<MAX_PTNS; i++)
	{
		m_aMapAlnPatnList[i].m_awHoleRow.RemoveAll();
		m_aMapAlnPatnList[i].m_awHoleCol.RemoveAll();
	}
	DelPrescanTmpPosn();

	DOUBLE dAlgnHoleMinPoints	= ((DOUBLE)ulMapAlnHolesNum*m_dScanAlignMatchLowPercent/100.0);

	return	ulHasDieInHoles==0;
}

BOOL CWaferPr::CheckHolesInWaferWithFile(ULONG ulScnMinRow, ULONG ulScnMinCol, ULONG ulScnMaxRow, ULONG ulScnMaxCol, CString &szOutMsg)
{
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	CString szLogPath;
	pUtl->GetPrescanLogPath(szLogPath);
	FILE *fpMap = NULL;
	CString szHolePmpFile = szLogPath + "_Hole.txt";	//	scan align

	errno_t nErr = fopen_s(&fpMap, szHolePmpFile, "a");
	if ((nErr == 0) && (fpMap != NULL))
	{
		fprintf(fpMap, "2. check map holes with scanned wafer range (%lu,%lu)==(%lu,%lu)\n\n",
			ulScnMinRow, ulScnMinCol, ulScnMaxRow, ulScnMaxCol);
	}

	AutoBondScreen(FALSE);
	LONG lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY;
	pUtl->GetAlignPosition(lHomeOriRow, lHomeOriCol, lHomeWfX, lHomeWfY);
	ULONG ulHasDieInHoles = 0;
	szOutMsg = "";
	for(ULONG ulHole=0; ulHole<m_ulFoundHoleCounter; ulHole++)
	{
		LONG nLimit = min((LONG)m_astHoleList[ulHole].m_awHoleRow.GetSize(),
						 (LONG)m_astHoleList[ulHole].m_awHoleCol.GetSize());
		for(LONG iPoint = 0; iPoint < nLimit; iPoint++)
		{
			LONG lPointMapRow = m_astHoleList[ulHole].m_awHoleRow.GetAt(iPoint);
			LONG lPointMapCol = m_astHoleList[ulHole].m_awHoleCol.GetAt(iPoint);
			BYTE bPointMapSta = m_astHoleList[ulHole].m_ucHoleNul.GetAt(iPoint);

			//	if check point is home die, just skip
			if( lPointMapRow==lHomeOriRow && lPointMapCol==lHomeOriCol )
			{
				continue;
			}

			LONG lPointScnRow = lPointMapRow - m_lMapScanOffsetRow;
			LONG lPointScnCol = lPointMapCol - m_lMapScanOffsetCol;

			// Outof range, fail
			if( lPointScnRow<(LONG)ulScnMinRow || lPointScnRow>(LONG)ulScnMaxRow ||
				lPointScnCol<(LONG)ulScnMinCol || lPointScnCol>(LONG)ulScnMaxCol )
			{
				continue;
			}

			// has die on it, fail
			BOOL bWaferDieSta = IsScnExist(lPointScnRow, lPointScnCol);
			if( bWaferDieSta!=bPointMapSta )
			{
				CString szPiece = "";
				LONG lUserRow = 0, lUserCol = 0;
				ConvertAsmToOrgUser(lPointMapRow, lPointMapCol, lUserRow, lUserCol);
				szPiece.Format("Map %4ld,%4ld die %d but wafer %d\n", lUserRow, lUserCol, bPointMapSta, bWaferDieSta);
				if( m_ucScanCheckMapOnWafer==2 )
				{
					m_WaferMapWrapper.SetCurrentPosition(lPointMapRow, lPointMapCol);
					Sleep(300);
					m_WaferMapWrapper.SetCurrentPosition(lPointMapRow, lPointMapCol);
					Sleep(600);

					LONG lPhyX = 0, lPhyY = 0;
					//	LONG encX = 0, encY = 0;
					//	BOOL ret = GetDieValidPrescanPosn(ulRow, ulCol, 3, encX, encY);
					if ( WprGetDieValidPrescanPosn(lPointMapRow, lPointMapCol, 3, lPhyX, lPhyY) )
					{
						if( MoveWaferTable(lPhyX, lPhyY) )
						{
							Sleep(20);
							LONG lEncX, lEncY;
							WprSearchDie(TRUE, 1, lEncX, lEncY);
							Sleep(100);
						//	(*m_psmfSRam)["MS896A"]["WaferMapRow"] = ulIntRow;
						//	(*m_psmfSRam)["MS896A"]["WaferMapCol"] = ulIntCol;
						//	(*m_psmfSRam)["MS896A"]["WaferTableX"] = lPhyX;
						//	(*m_psmfSRam)["MS896A"]["WaferTableY"] = lPhyY;
						//	if( m_bScnCheckIsRefDie==FALSE )
						//	{
						//		GrabAndSaveImage(0, 1, WPR_GRAB_SAVE_IMG_5PC);	// FIVE POINT CHECK
						//	}
						//	else
						//	{
						//		GrabAndSaveImage(1, m_lScnCheckRefDieNo, WPR_GRAB_SAVE_IMG_5PC);	// FIVE POINT CHECK
						//	}
							HmiMessage_Red_Back(szPiece, "Scan Check");
						}
					}
				}

				ulHasDieInHoles++;
				szOutMsg += szPiece;

				szPiece.Format("Map row %ld(%ld), column %ld(%ld) die %d but wafer %d\n",
					lUserRow, lPointMapRow, lUserCol, lPointMapCol,
					bPointMapSta, bWaferDieSta);
				if( fpMap!=NULL )
				{
					fprintf(fpMap, "%s", szPiece);
				}
			}
		}	// end hole points checking
	}	// end hole list checking

	if( fpMap!=NULL )
	{
		fprintf(fpMap, "\n");
		fclose(fpMap);
	}

	AutoBondScreen(TRUE);

	return	ulHasDieInHoles==0;
}


LONG CWaferPr::SaveScanCheckOption(IPC_CServiceMessage &svMsg)
{
	BOOL bReturn = FALSE;
	UCHAR ucSelection = 0;
	svMsg.GetMsg(sizeof(UCHAR), &ucSelection);
	m_ucScanCheckMapOnWafer = ucSelection;
	if( IsScanAlignWafer() )
	{
		m_ucScanCheckMapOnWafer = 0;
	}
	else
	{
		m_bScanAlignAutoFromMap = FALSE;
		m_bScanAlignFromFile	= FALSE;
		switch( m_ucScanCheckMapOnWafer )
		{
		case 0:
			break;
		case 1:
			m_bScanAlignFromFile	= TRUE;
			break;
		case 2:
			m_bScanAlignFromFile	= TRUE;
			bReturn = TRUE;
			break;
		case 3:
		default:
			m_bScanAlignAutoFromMap = TRUE;
			break;
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

BOOL CWaferPr::GenerateCheckPointsTableFile()
{
	CString szFileName = "c:\\Mapsorter\\UserData\\Mapfile\\ScanCheckPoints.csv";

	FILE *fp = NULL;
	errno_t nErr = fopen_s(&fp, szFileName, "wt");
	if ((nErr == 0) && (fp != NULL))
	{
		for(ULONG i=0; i<m_ulScanAlignTotalPoints; i++)
		{
			if (i >= (ULONG)m_awPointsListRow.GetSize())
			{
				break;
			}
			LONG lUserRow = (LONG) m_awPointsListRow.GetAt(i);
			LONG lUserCol = (LONG) m_awPointsListCol.GetAt(i);
			BYTE ucDieSta = (BYTE) m_ucPointsListSta.GetAt(i);
			fprintf(fp, "%4d,%4d,%d,\n", lUserRow, lUserCol, ucDieSta);
		}
		fclose(fp);
	}

	return TRUE;
}

LONG CWaferPr::SelectCheckCenterDie(IPC_CServiceMessage &svMsg)
{
	ULONG ulAsmRow = 0, ulAsmCol = 0, ulMaxRow = 0, ulMaxCol = 0;
	m_WaferMapWrapper.GetSelectedPosition(ulAsmRow, ulAsmCol);
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMaxRow, ulMaxCol);
	INT i = 0;
	memset(m_bScanCheckPoints, FALSE, sizeof(m_bScanCheckPoints));
	for(LONG lRow=-1; lRow<=1; lRow++)
	{
		for(LONG lCol=-1; lCol<=1; lCol++)
		{
			LONG lTgtRow = (LONG) ulAsmRow + lRow;
			LONG lTgtCol = (LONG) ulAsmCol + lCol;
			if (lTgtRow >= 0 && lTgtCol >= 0 && lTgtRow < (LONG)ulMaxRow && lTgtCol < (LONG)ulMaxCol)
			{
				if( m_WaferMapWrapper.IsReferenceDie(lTgtRow, lTgtCol)==FALSE &&
					m_WaferMapWrapper.GetGrade(lTgtRow, lTgtCol)!=m_WaferMapWrapper.GetNullBin() )
				{
					m_bScanCheckPoints[i] = TRUE;
				}
			}
			i++;
		}
	}

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ConfirmMapScanPoints(IPC_CServiceMessage &svMsg)
{
	LONG lUserRow = 0, lUserCol = 0;
	ULONG ulAsmRow = 0, ulAsmCol = 0, ulMaxRow = 0, ulMaxCol = 0;
	m_WaferMapWrapper.GetSelectedPosition(ulAsmRow, ulAsmCol);
	WM_CWaferMap::Instance()->GetWaferMapDimension(ulMaxRow, ulMaxCol);

	INT i = 0;
	for(LONG lRow=-1; lRow<=1; lRow++)
	{
		if( m_ulScanAlignTotalPoints>500 )
		{
			break;
		}
		for(LONG lCol=-1; lCol<=1; lCol++)
		{
			BYTE ucDieSta = m_bScanCheckPoints[i];
			LONG lTgtRow = (LONG) ulAsmRow + lRow;
			LONG lTgtCol = (LONG) ulAsmCol + lCol;
			if (lTgtRow >= 0 && lTgtCol >= 0 && lTgtRow < (LONG)ulMaxRow && lTgtCol < (LONG)ulMaxCol)
			{
				ConvertAsmToOrgUser(lTgtRow, lTgtCol, lUserRow, lUserCol);
				m_awPointsListRow.Add(lUserRow);
				m_awPointsListCol.Add(lUserCol);
				m_ucPointsListSta.Add(ucDieSta);
				m_ulScanAlignTotalPoints++;
			}
			i++;
		}
	}

	GenerateCheckPointsTableFile();
	SavePrData();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::ClearAllMapScanPoints(IPC_CServiceMessage &svMsg)
{
	m_ulScanAlignTotalPoints = 0;
	m_awPointsListRow.RemoveAll();
	m_awPointsListCol.RemoveAll();
	m_ucPointsListSta.RemoveAll();

	GenerateCheckPointsTableFile();
	SavePrData();

	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}

LONG CWaferPr::IntoMapScanCheckPage(IPC_CServiceMessage &svMsg)
{
	GenerateCheckPointsTableFile();
	BOOL bReturn = TRUE;
	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}