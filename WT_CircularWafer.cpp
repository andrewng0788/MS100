#include <Math.h>
#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WT_CircularWafer.h"
#include "WT_Constant.h"
#include "FileUtil.h"

#pragma once


/**************************** Class CCircularWafer Start ****************************/

/***********************************/
/*     Constructor/Destructor      */
/***********************************/
CCircularWafer::CCircularWafer()
{
	//InitWaferData();

	//++++++++ hardcode for now for testing +++++++++

	//CMSFileUtility  *pUtl = CMSFileUtility::Instance();
 //   CStringMapFile  *psmf;

 //   // open config file
 //   pUtl->LoadWTConfig("WaferTable.smf"); 

 //   // get file pointer
 //   psmf = pUtl->GetWTConfigFile();

	//// update data
	//m_lWaferCenterX							= (*psmf)[WT_PROCESS_DATA][WT_CENTER_X];
	//m_lWaferCenterY							= (*psmf)[WT_PROCESS_DATA][WT_CENTER_Y];
	//m_lWaferDiameter						= (*psmf)[WT_PROCESS_DATA][WT_WAFER_SIZE];
	//m_lWalkPath								= 1;			
	//m_lEmptyHoleLimit						= (*psmf)[WT_PROCESS_DATA][WT_EMPTY_LIMIT];
	//m_lEndStreetLimit						= (*psmf)[WT_PROCESS_DATA][WT_STREET_LIMIT];
	//m_lHoriDiePitchX						= 0;	//to be set later	
	//m_lHoriDiePitchY						= 0;	//to be set later		
	//m_lVertDiePitchX						= 0;	//to be set later		
	//m_lVertDiePitchY						= 0;	//to be set later	
	//m_lStartX								= 0;	//to be set later
	//m_lStartY								= 0;	//to be set later

	//m_bIsFirstDieInCycle					= TRUE;
	//m_bWaferFinished						= FALSE;
	//m_lEmptyHoleCount						= 0;	
	//m_lEndStreetCount						= 0;	
	//m_bRowChange							= FALSE;
	//m_bCurrRowEmpty							= TRUE;
	//m_bNeedTransverseBack					= FALSE;
	//m_bNeedCalibrateFirstDieOnRow			= FALSE;
	//m_bNeedFindNearestWithinWaferLimitPosn	= FALSE;
	//m_lCurrX								= 0;
	//m_lCurrY								= 0;
	//m_lFirstDieOnRowX						= 0;
	//m_lFirstDieOnRowY						= 0;
	//m_lWalkDirection						= 1;	
	//m_bIsLastDieEmpty						= FALSE;
	//++++++++ hardcode for now for testing +++++++++



} //end constructor


CCircularWafer::~CCircularWafer()
{
} //end destructor

/***********************************/
/*        Get/Set functions        */
/***********************************/
BOOL CCircularWafer::GetNeedCalibrateFirstDieOnRow()
{
	return m_bNeedCalibrateFirstDieOnRow;
} //end GetNeedCalibrateFirstDieOnRow


BOOL CCircularWafer::SetNeedCalibrateFirstDieOnRow(BOOL bNeedCalibrateFirstDieOnRow)
{
	m_bNeedCalibrateFirstDieOnRow = bNeedCalibrateFirstDieOnRow;
	return TRUE;
} //end SetNeedCalibrateFirstDieOnRow


BOOL CCircularWafer::SetCurrX(LONG lCurrX)
{
	m_lCurrX = lCurrX;
	return TRUE;
} //end SetCurrX

BOOL CCircularWafer::SetCurrY(LONG lCurrY)
{
	m_lCurrY = lCurrY;
	return TRUE;
} //end SetCurrY

BOOL CCircularWafer::SetWaferCenterX(LONG lWaferCenterX)
{
	m_lWaferCenterX = lWaferCenterX;
	return TRUE;
} //end SetWaferCenterX


BOOL CCircularWafer::SetWaferCenterY(LONG lWaferCenterY)
{
	m_lWaferCenterY = lWaferCenterY;
	return TRUE;
} //end SetWaferCenterY


BOOL CCircularWafer::SetWaferDiameter(LONG lWaferDiameter)
{
	m_lWaferDiameter = lWaferDiameter;
	return TRUE;
} //end SetWaferDiameter


BOOL CCircularWafer::SetWalkPath(LONG lWalkPath)
{
	m_lWalkPath = lWalkPath;
	return TRUE;
} //end SetWalkPath


BOOL CCircularWafer::SetEmptyHoleLimit(LONG lEmptyHoleLimit)
{
	m_lEmptyHoleLimit = lEmptyHoleLimit;
	return TRUE;
} //end SetEmptyHoleLimit


BOOL CCircularWafer::SetEndStreetLimit(LONG lEndStreetLimit)
{
	m_lEndStreetLimit = lEndStreetLimit;
	return TRUE;
} //end SetEndStreetLimit


BOOL CCircularWafer::SetHoriDiePitchX(LONG lHoriDiePitchX)
{	
	m_lHoriDiePitchX = lHoriDiePitchX;
	return TRUE;
} //end SetHoriDiePitchX


BOOL CCircularWafer::SetHoriDiePitchY(LONG lHoriDiePitchY)
{	
	m_lHoriDiePitchY = lHoriDiePitchY;
	return TRUE;
} //end SetHoriDiePitchY


BOOL CCircularWafer::SetVertDiePitchX(LONG lVertDiePitchX)
{
	m_lVertDiePitchX = lVertDiePitchX;
	return TRUE;
} //end SetVertDiePitchX


BOOL CCircularWafer::SetVertDiePitchY(LONG lVertDiePitchY)
{
	m_lVertDiePitchY = lVertDiePitchY;
	return TRUE;
} //end SetVertDiePitchY


BOOL CCircularWafer::SetStartX(LONG lStartX)
{
	m_lStartX = lStartX;
	return TRUE;
} //end SetStartX


BOOL CCircularWafer::SetStartY(LONG lStartY)
{
	m_lStartY = lStartY;
	return TRUE;
} //end SetStartY


BOOL CCircularWafer::SetFirstDieOnRowX(LONG lFirstDieOnRowX)
{
	m_lFirstDieOnRowX = lFirstDieOnRowX;
	return TRUE;
} //end SetFirstDieOnRowX


BOOL CCircularWafer::SetFirstDieOnRowY(LONG lFirstDieOnRowY)
{
	m_lFirstDieOnRowY = lFirstDieOnRowY;
	return TRUE;
} //end SetFirstDieOnRowY


BOOL CCircularWafer::GetIsFirstDieInCycle()
{
	return m_bIsFirstDieInCycle;
} //end GetIsFirstDieInCycle


BOOL CCircularWafer::SetIsFirstDieInCycle(BOOL bIsFirstDieInCycle)
{
	m_bIsFirstDieInCycle = bIsFirstDieInCycle;
	return TRUE;
} //end SetIsFirstDieInCycle


BOOL CCircularWafer::SetIsLastDieEmpty(BOOL bIsLastDieEmpty)
{
	m_bIsLastDieEmpty = !bIsLastDieEmpty;
	return TRUE;
} //end SetIsLastDieEmpty


/***********************************/
/*         Member functions        */
/***********************************/
BOOL CCircularWafer::InitWaferData()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

    // open config file
    pUtl->LoadWTConfig("WaferTable.msd"); 

    // get file pointer
    psmf = pUtl->GetWTConfigFile();

	// update data
	m_lWaferCenterX							= (*psmf)[WT_PROCESS_DATA][WT_CENTER_X];
	m_lWaferCenterY							= (*psmf)[WT_PROCESS_DATA][WT_CENTER_Y];
	m_lWaferDiameter						= (*psmf)[WT_PROCESS_DATA][WT_WAFER_SIZE];
	m_lWalkPath								= (*psmf)[WT_PROCESS_DATA][WT_INDEX_PATH];			
	m_lEmptyHoleLimit						= (*psmf)[WT_PROCESS_DATA][WT_EMPTY_LIMIT];
	m_lEndStreetLimit						= (*psmf)[WT_PROCESS_DATA][WT_STREET_LIMIT];
	m_lStartX								= 0;	
	m_lStartY								= 0;	

	m_bIsFirstDieInCycle					= TRUE;
	m_bWaferFinished						= FALSE;
	m_lEmptyHoleCount						= 0;	
	m_lEndStreetCount						= 0;	
	m_bRowChange							= FALSE;
	m_bCurrRowEmpty							= TRUE;
	m_bNeedTransverseBack					= FALSE;
	m_bNeedCalibrateFirstDieOnRow			= FALSE;
	m_bNeedFindNearestWithinWaferLimitPosn	= FALSE;
	m_lCurrX								= 0;
	m_lCurrY								= 0;
	m_lFirstDieOnRowX						= 0;
	m_lFirstDieOnRowY						= 0;
	
	if ((m_lWalkPath == 0) || (m_lWalkPath == 1))
		m_lWalkDirection = 1;	
	else 
		m_lWalkDirection = 3;

	m_bIsLastDieEmpty						= FALSE;

	return TRUE;

} //end InitWaferData


BOOL CCircularWafer::IsFirstDieInCycle()
{
	return m_bIsFirstDieInCycle;
} //end IsFirstDieInCycle


BOOL CCircularWafer::IsWaferFinished()
{
	return m_bWaferFinished;
} //end IsWaferFinished


BOOL CCircularWafer::IsWithinWaferBoundary(LONG lX, LONG lY)
{
	LONG lWaferRadius, lXLowerBoundary, lXUpperBoundary, lYLowerBoundary, lYUpperBoundary; 

	lWaferRadius = m_lWaferDiameter / 2;	
	
	lXLowerBoundary = m_lWaferCenterX - lWaferRadius - abs(m_lHoriDiePitchX);
	lXUpperBoundary = m_lWaferCenterX + lWaferRadius + abs(m_lHoriDiePitchX);
	lYLowerBoundary = m_lWaferCenterY - lWaferRadius - abs(m_lVertDiePitchY);
	lYUpperBoundary = m_lWaferCenterY + lWaferRadius + abs(m_lVertDiePitchY);

	if ((lXLowerBoundary <= lX) && (lX <= lXUpperBoundary)
		&&
		(lYLowerBoundary <= lY) && (lY <= lYUpperBoundary))
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
} //end IsWithinWaferBoundary


BOOL CCircularWafer::IsLastPosnEmpty()
{
	return m_bIsLastDieEmpty;
} //end IsLastPosnEmpty


BOOL CCircularWafer::DoesHitEmptyHoleLimit()
{
	if (m_lEmptyHoleCount >= m_lEmptyHoleLimit)
		return TRUE;
	else
		return FALSE;
} //end DoesHitEmptyHoleLimit


BOOL CCircularWafer::IsCurrRowEmpty()
{	
	return m_bCurrRowEmpty;
} //end IsCurrRowEmpty


BOOL CCircularWafer::DoesHitEndStreetLimit()
{
	if (m_lEndStreetCount >= m_lEndStreetLimit)
		return TRUE;
	else
		return FALSE;
} //end DoesHitEndStreetLimit


BOOL CCircularWafer::IsRowChange()
{
	return m_bRowChange;
} //end IsRowChange


BOOL CCircularWafer::IsWithinWaferLimit(LONG lX, LONG lY)
{
	LONG lWaferRadius = m_lWaferDiameter / 2;
	DOUBLE	dDistance = 0.0;

	dDistance = sqrt(((double)abs(lX - m_lWaferCenterX) * (double)abs(lX - m_lWaferCenterX))
				+  ((double)abs(lY - m_lWaferCenterY) * (double)abs(lY -m_lWaferCenterY)));

	if (dDistance <= (DOUBLE)(lWaferRadius))
		return TRUE;
	else
		return FALSE;
} //end IsWithinWaferLimit


BOOL CCircularWafer::DoesNeedTransverseBack()
{
	return m_bNeedTransverseBack;
} //end DoesNeedTransverseBack


BOOL CCircularWafer::DoesNeedFindNearestWithinWaferLimitPosn()
{
	return m_bNeedFindNearestWithinWaferLimitPosn;
} //end DoesNeedFindNearestWithinWaferLimitPosn


BOOL CCircularWafer::ReverseWalkDirection()
{
	if (m_lWalkDirection == 1)
		m_lWalkDirection = 2;
	else if (m_lWalkDirection == 2)
		m_lWalkDirection = 1;
	else if (m_lWalkDirection == 3)
		m_lWalkDirection = 4;
	else if (m_lWalkDirection == 4)
		m_lWalkDirection = 3;

	return TRUE;
}


DOUBLE CCircularWafer::DistanceToCenter(LONG lX, LONG lY)
{
	LONG lWaferRadius = m_lWaferDiameter / 2;
	DOUBLE	dDistance = 0.0;

	dDistance = sqrt(((double)abs(lX - m_lWaferCenterX) * (double)abs(lX - m_lWaferCenterX))
				+  ((double)abs(lY - m_lWaferCenterY) * (double)abs(lY -m_lWaferCenterY)));

	return dDistance;
} //end DistanceToCenter


BOOL CCircularWafer::FindNextDieOnTop(LONG lPrevX, LONG lPrevY, LONG& lNextOnTopX, LONG& lNextOnTopY)
{
	LONG lX, lY;

	lX = lPrevX + m_lVertDiePitchX;	//for adjustment only
	lY = lPrevY + m_lVertDiePitchY;

	lNextOnTopX = lX;
	lNextOnTopY = lY;

	return TRUE;
} //end FindNextDieOnTop


BOOL CCircularWafer::FindNextDieAtBottom(LONG lPrevX, LONG lPrevY, LONG& lNextAtBottomX, LONG& lNextAtBottomY)
{
	LONG lX, lY;

	lX = lPrevX - m_lVertDiePitchX;	//for adjustment only
	lY = lPrevY - m_lVertDiePitchY;

	lNextAtBottomX = lX;
	lNextAtBottomY = lY;

	return TRUE;
} //end FindNextDieAtBottom


BOOL CCircularWafer::FindNextDieOnLeft(LONG lPrevX, LONG lPrevY, LONG& lNextOnLeftX, LONG& lNextOnLeftY)
{
	LONG lX, lY;

	lX = lPrevX + m_lHoriDiePitchX;
	lY = lPrevY + m_lHoriDiePitchY; //for adjustment only

	lNextOnLeftX = lX;
	lNextOnLeftY = lY;

	return TRUE;
} //end FindNextDieOnLeft


BOOL CCircularWafer::FindNextDieOnRight(LONG lPrevX, LONG lPrevY, LONG& lNextOnRightX, LONG& lNextOnRightY)
{
	LONG lX, lY;

	lX = lPrevX - m_lHoriDiePitchX;
	lY = lPrevY - m_lHoriDiePitchY; //for adjustment only

	lNextOnRightX = lX;
	lNextOnRightY = lY;

	return TRUE;
} //end FindNextDieOnRight


BOOL CCircularWafer::FindNextDiePosn(LONG lPrevX, LONG lPrevY, LONG& lNextX, LONG& lNextY)
{
	LONG lX, lY;

	//Initialization
	lX = 0;
	lY = 0;

	if (IsFirstDieInCycle())
	{
		lX = m_lStartX;
		lY = m_lStartY;
		m_bIsFirstDieInCycle = FALSE; //reset 
	}
	else
	{
		//if ((m_lWalkPath == 1) || (m_lWalkPath == 2)) //Walk Path: 1 = TLH; 2 = BLH
		if ((m_lWalkPath == 0) || (m_lWalkPath == 1)) //Walk Path: 0 = TLH; 1 = BLH
		{
			//Walk Direction: 1 = Right; 2 = Left
			if ((m_lWalkDirection == 1) && (! IsRowChange()))
				FindNextDieOnRight(lPrevX, lPrevY, lX, lY);
			else if ((m_lWalkDirection == 2) && (! IsRowChange()))
				FindNextDieOnLeft(lPrevX, lPrevY, lX, lY);

			if (IsRowChange())
			{
				//if (m_lWalkPath == 1)
				if (m_lWalkPath == 0)
					FindNextDieAtBottom(lPrevX, lPrevY, lX, lY);
				//else if (m_lWalkPath == 2)
				else if (m_lWalkPath == 1)
					FindNextDieOnTop(lPrevX, lPrevY, lX, lY);
			}
		}
		//else if ((m_lWalkPath == 3) || (m_lWalkPath == 4)) //Walk Path: 3 = TLV; 4 = TRV
		else if ((m_lWalkPath == 2) || (m_lWalkPath == 3)) //Walk Path: 2 = TLV; 3 = TRV
		{
			//Walk Direction: 3 = Down; 4 = Up
			if ((m_lWalkDirection == 3) &&  (! IsRowChange()))
					FindNextDieAtBottom(lPrevX, lPrevY, lX, lY);
			else if ((m_lWalkDirection == 4) && (! IsRowChange()))
					FindNextDieOnTop(lPrevX, lPrevY, lX, lY);

			if (IsRowChange())
			{
				//if (m_lWalkPath == 3)
				if (m_lWalkPath == 2)
					FindNextDieOnRight(lPrevX, lPrevY, lX, lY);
				//else if (m_lWalkPath == 4)
				else if (m_lWalkPath == 3)
					FindNextDieOnLeft(lPrevX, lPrevY, lX, lY);
			}
		}
	}

	lNextX = lX;
	lNextY = lY;

	return TRUE;
} //end FindNextDiePosn


BOOL CCircularWafer::FindPickXYPosn(LONG& lPickXPosn, LONG& lPickYPosn)
{
	BOOL bAvailablePosnFound = FALSE;
	LONG lNextX, lNextY, lPrevX, lPrevY;
	CString szDebug;
	FILE *pfFile;

	pfFile = fopen("Debug.txt", "a");

	if (! IsFirstDieInCycle())
	{
		m_bRowChange = FALSE; //reset
		lPrevX = m_lCurrX;
		lPrevY = m_lCurrY;

		szDebug.Format("%d %d", lPrevX, lPrevY);
		fprintf(pfFile, "Prev X, Y: " + szDebug + "\n");

		if (IsWaferFinished())
		{
			fprintf(pfFile, "Wafer Finished! \n");
			lPickXPosn = NULL;
			lPickYPosn = NULL;

			fclose(pfFile);
			return FALSE;
		}
		else	//NOT wafer finished 
		{
			if (IsLastPosnEmpty())
			{
				m_lEmptyHoleCount++;

				szDebug.Format("%d", m_lEmptyHoleCount);
				fprintf(pfFile, "Last Posn Empty -> Empty Hole Count: " + szDebug + "\n");

				if (DoesHitEmptyHoleLimit())
				{
					fprintf(pfFile, "Hit Empty Hole Limit! \n");

					if (DoesNeedTransverseBack())
					{
						fprintf(pfFile, "Need transverse back! \n");

						m_lEmptyHoleCount = 0;
						ReverseWalkDirection();

						fprintf(pfFile, "Reverse Walk Direction! \n");

						lPrevX = m_lFirstDieOnRowX;
						lPrevY = m_lFirstDieOnRowY;
						m_bNeedTransverseBack = FALSE; //reset for next time
					}
					else //NO NEED to transverse back => change row
					{
						fprintf(pfFile, "No need transverse back -> Change Row! \n");

						m_bRowChange = TRUE;

						if (IsCurrRowEmpty())
						{
							m_lEndStreetCount++;
							
							szDebug.Format("%d", m_lEndStreetCount);
							fprintf(pfFile, "Current Row Empty -> End Street Count: " + szDebug + "\n");

							if (DoesHitEndStreetLimit())
							{
								fprintf(pfFile, "Hit End Street Limit! \n");

								m_bWaferFinished = TRUE;
								lPickXPosn = NULL;
								lPickYPosn = NULL;

								fclose(pfFile);
								return FALSE;
							}
						}
					}
				}
			}
			else	// last position is NOT empty
			{
				fprintf(pfFile, "Last position is not empty! \n");

				m_bCurrRowEmpty = FALSE;
				m_lEmptyHoleCount = 0;
				m_lEndStreetCount = 0;
			}		
		}
	}

	do 
	{
		fprintf(pfFile, "==================================================\n");
		FindNextDiePosn(lPrevX, lPrevY, lNextX, lNextY);
		
		szDebug.Format("%d %d", lNextX, lNextY);
		fprintf(pfFile, "Next X, Y: " + szDebug + "\n");

		szDebug.Format("%d", m_lWalkDirection);
		fprintf(pfFile, "Current Walk Direction: " + szDebug + "\n");
		
		if (! IsWithinWaferBoundary(lNextX, lNextY))
		{
			AfxMessageBox( "NOT within Wafer Boundary!");
			fprintf(pfFile, "NOT within Wafer Boundary! \n");

			m_bWaferFinished = TRUE;
			bAvailablePosnFound = FALSE;
			lPickXPosn = NULL;
			lPickYPosn = NULL;

			fclose(pfFile);
			return FALSE;
		}

		if (IsRowChange())
		{
			fprintf(pfFile, "There was row change! \n");

			m_bCurrRowEmpty = TRUE;
			m_lEmptyHoleCount = 0;
			m_bIsLastDieEmpty = FALSE; //reset
			
			if (IsWithinWaferLimit(lNextX, lNextY))
			{
				fprintf(pfFile, "Within wafer limit after row change -> Need transverse back later! \n");

				m_bNeedTransverseBack = TRUE;
				m_bNeedCalibrateFirstDieOnRow = TRUE;
				bAvailablePosnFound = TRUE;
				lPickXPosn = lNextX;
				lPickYPosn = lNextY;

				fclose(pfFile);
				return TRUE;
			}
			else	//NOT within wafer limit
			{
				fprintf(pfFile, "Not within wafer limit after row change -> Reverse Walk Direction! \n");

				if ((m_lWalkPath == 0) || (m_lWalkPath == 1))
				{
					LONG lDieOnLeftX, lDieOnLeftY, lDieOnRightX, lDieOnRightY;
					DOUBLE dDistFromLeftDieToCenter, dDistFromRightDieToCenter;

					FindNextDieOnLeft(lNextX, lNextY, lDieOnLeftX, lDieOnLeftY);
					dDistFromLeftDieToCenter = DistanceToCenter(lDieOnLeftX, lDieOnLeftY);

					FindNextDieOnRight(lNextX, lNextY, lDieOnRightX, lDieOnRightY);
					dDistFromRightDieToCenter = DistanceToCenter(lDieOnRightX, lDieOnRightY);

					if (dDistFromRightDieToCenter < dDistFromLeftDieToCenter)
						m_lWalkDirection = 1;	//right
					else
						m_lWalkDirection = 2;	//left
				}
				else if ((m_lWalkPath == 2) || (m_lWalkPath == 3))
				{
					LONG lDieOnTopX, lDieOnTopY, lDieAtBottomX, lDieAtBottomY;
					DOUBLE dDistFromTopDieToCenter, dDistFromBottomDieToCenter;

					FindNextDieOnTop(lNextX, lNextY, lDieOnTopX, lDieOnTopY);
					dDistFromTopDieToCenter = DistanceToCenter(lDieOnTopX, lDieOnTopY);

					FindNextDieAtBottom(lNextX, lNextY, lDieAtBottomX, lDieAtBottomY);
					dDistFromBottomDieToCenter = DistanceToCenter(lDieAtBottomX, lDieAtBottomY);

					if (dDistFromBottomDieToCenter < dDistFromTopDieToCenter)
						m_lWalkDirection = 3;	//down
					else
						m_lWalkDirection = 4;	//up
				}

				szDebug.Format("%d", m_lWalkDirection);
				fprintf(pfFile, "New Walk Direction :" + szDebug + "\n");

				m_bRowChange = FALSE;
				m_bNeedFindNearestWithinWaferLimitPosn = TRUE;
				
				lPrevX = lNextX;
				lPrevY = lNextY;
				bAvailablePosnFound = FALSE;
				
			}
		}
		else	//NOT row change
		{
			fprintf(pfFile, "There was no row change! \n");

			if (DoesNeedFindNearestWithinWaferLimitPosn())
			{
				fprintf(pfFile, "Need to find nearest within limit posn! \n");

				if (IsWithinWaferLimit(lNextX, lNextY))
				{
					fprintf(pfFile, "Within limit posn found! \n");

					bAvailablePosnFound = TRUE;
					lPickXPosn = lNextX;
					lPickYPosn = lNextY;
					m_bNeedFindNearestWithinWaferLimitPosn = FALSE;
					fclose(pfFile);
					return TRUE;
				}
				else	//NOT within wafer limit
				{
					fprintf(pfFile, "Not Within limit! \n");

					lPrevX = lNextX;
					lPrevY = lNextY;
					bAvailablePosnFound = FALSE;
				}
			}
			else	//NO NEED to find nearest within wafer limit posn
			{
				fprintf(pfFile, "No need to find nearest within limit posn! \n");

				if (IsWithinWaferLimit(lNextX, lNextY))
				{
					fprintf(pfFile, "Within wafer limit! \n");

					bAvailablePosnFound = TRUE;
					lPickXPosn = lNextX;
					lPickYPosn = lNextY;
					fclose(pfFile);
					return TRUE;
				}
				else	//NOT within wafer limit
				{
					fprintf(pfFile, "Not within wafer limit! \n");

					if (DoesNeedTransverseBack())
					{
						fprintf(pfFile, "Transverse back! \n");

						ReverseWalkDirection();
						m_bRowChange = FALSE;
						lPrevX = m_lFirstDieOnRowX;
						lPrevY = m_lFirstDieOnRowY;
						m_bNeedTransverseBack = FALSE;	//reset
						bAvailablePosnFound = FALSE;
					}
					else	//NO need to transverse back
					{
						fprintf(pfFile, "No need transverse back -> Row Change!\n");

						m_bRowChange = TRUE;
						lPrevX = lNextX;
						lPrevY = lNextY;
						bAvailablePosnFound = FALSE;
					}
				}
			}
		}
	} while (! bAvailablePosnFound);

	fprintf(pfFile, "**************************************************\n");

	fclose(pfFile);

	return TRUE;

} //end FindPickXYPosn


BOOL CCircularWafer::ResetParameters()
{
	m_bIsFirstDieInCycle					= TRUE;
	m_bWaferFinished						= FALSE;
	m_lEmptyHoleCount						= 0;	
	m_lEndStreetCount						= 0;	
	m_bRowChange							= FALSE;
	m_bCurrRowEmpty							= TRUE;
	m_bNeedTransverseBack					= FALSE;
	m_bNeedCalibrateFirstDieOnRow			= FALSE;
	m_bNeedFindNearestWithinWaferLimitPosn	= FALSE;
	m_lCurrX								= 0;
	m_lCurrY								= 0;
	m_lFirstDieOnRowX						= 0;
	m_lFirstDieOnRowY						= 0;
	m_lWalkDirection						= 1;	
	m_bIsLastDieEmpty						= FALSE;
	
	return TRUE;
} //end ResetParameters


/***************************** Class CCircularWafer End *****************************/
