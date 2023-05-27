#include "stdafx.h"
#include <Math.h>
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_PhyBlkMain.h"
#include "BT_PhyBlk.h"
#include "FileUtil.h"
#include "GenerateDatabase.h"

#pragma once

/**************************** Class CPhyBlkMain Start ****************************/

/***********************************/
/*     Constructor/Destructor      */
/***********************************/
CPhyBlkMain::CPhyBlkMain()
{
	//m_ulNoOfBlk = 1;
	m_ulNoOfBlk = 0;
	m_lBlkPitchX = 0;
	m_lBlkPitchY = 0;
	m_ulMsgCode = 0;
} //end constructor


CPhyBlkMain::~CPhyBlkMain()
{
} //end destructor

/***********************************/
/*        Get/Set functions        */
/***********************************/
ULONG CPhyBlkMain::GetNoOfBlk()
{
	return m_ulNoOfBlk;
} //end GetNoOfBlk


BOOL CPhyBlkMain::SetNoOfBlk(ULONG ulNoOfBlk)
{
	m_ulNoOfBlk = ulNoOfBlk;
	return TRUE;
} //end SetNoOfBlk


ULONG CPhyBlkMain::GetBlkPitchX()
{
	return m_lBlkPitchX;
} //end GetBlkPitchX

BOOL CPhyBlkMain::SetBlkPitchX(LONG lBlkPitchX)
{
	m_lBlkPitchX = lBlkPitchX;
	return TRUE;
} //end SetBlkPitchX


ULONG CPhyBlkMain::GetBlkPitchY()
{
	return m_lBlkPitchY;
} //end GetBlkPitchY

BOOL CPhyBlkMain::SetBlkPitchY(LONG lBlkPitchY)
{
	m_lBlkPitchY = lBlkPitchY;
	return TRUE;
} //end SetBlkPitchY

ULONG CPhyBlkMain::GetMsgCode()
{
	return m_ulMsgCode;
} //end GetMsgCode

BOOL CPhyBlkMain::SetMsgCode(ULONG ulMsgCode)
{
	m_ulMsgCode = ulMsgCode;
	return TRUE;
} //end SetMsgCode


/***********************************/
/*    Init from file functions     */
/***********************************/

BOOL CPhyBlkMain::InitPhyBlkMainData()
{
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if ( pBTfile == NULL )
		return FALSE;

	//load data values from smf to data structure
	if ((!((*pBTfile)["PhysicalBlock"]["NoOfBlk"])) == FALSE)
	{
		m_ulNoOfBlk = (*pBTfile)["PhysicalBlock"]["NoOfBlk"];
	}
	else
	{
		m_ulNoOfBlk = 0;
	}

	if ((!((*pBTfile)["PhysicalBlock"]["BlkPitchX"])) == FALSE)
	{
		m_lBlkPitchX = (*pBTfile)["PhysicalBlock"]["BlkPitchX"];
	}
	else
	{
		m_lBlkPitchX = 0;
	}

	if ((!((*pBTfile)["PhysicalBlock"]["BlkPitchY"])) == FALSE)
	{
		m_lBlkPitchY = (*pBTfile)["PhysicalBlock"]["BlkPitchY"];
	}
	else
	{
		m_lBlkPitchY = 0;
	}

	CMSFileUtility::Instance()->CloseBTConfig();

	return TRUE;
} //end InitPhyBlkMainData


BOOL CPhyBlkMain::InitPhyBlkData(ULONG ulBlkId, CStringMapFile *pBTMSDfile)
{
	CStringMapFile* pBTfile = pBTMSDfile;
	if (pBTMSDfile == NULL)
	{
		pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	}

	LONG lUpperLeftX, lUpperLeftY, lLowerRightX, lLowerRightY, lIsSetup;
	BOOL bIsSetup;

	//Check Load/Save Data
	if (pBTfile == NULL)
		return FALSE;

	//save data values from smf to data structure
	if (((!((*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftX"])) == FALSE)
		&&
		((!((*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightX"])) == FALSE))
	{
		lUpperLeftX = (*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftX"];
		lLowerRightX = (*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightX"];
	}
	else
	{
		lUpperLeftX = 0;
		lLowerRightX = 0;
	}

	if (((!((*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftY"])) == FALSE)
		&&
		((!((*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightY"])) == FALSE))
	{
		lUpperLeftY = (*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftY"];
		lLowerRightY = (*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightY"];
	}
	else
	{
		lUpperLeftY = 0;
		lLowerRightY = 0;
	}

	m_oPhyBlk[ulBlkId].SetUpperLeftX(lUpperLeftX);
	m_oPhyBlk[ulBlkId].SetLowerRightX(lLowerRightX);
	m_oPhyBlk[ulBlkId].SetWidth();
	m_oPhyBlk[ulBlkId].SetUpperLeftY(lUpperLeftY);
	m_oPhyBlk[ulBlkId].SetLowerRightY(lLowerRightY);
	m_oPhyBlk[ulBlkId].SetHeight();

	if ((!((*pBTfile)["PhysicalBlock"][ulBlkId]["IsSetup"])) == FALSE)
	{
		lIsSetup = (*pBTfile)["PhysicalBlock"][ulBlkId]["IsSetup"];
		bIsSetup = BOOL(lIsSetup);
	}
	else
	{
		bIsSetup = FALSE;
	}

	m_oPhyBlk[ulBlkId].SetIsSetup(bIsSetup);

	if (pBTMSDfile == NULL)		//v4.52A8
	{
		CMSFileUtility::Instance()->CloseBTConfig();
	}

	return TRUE;
} //end InitPhyBlkData

BOOL CPhyBlkMain::InitAllPhyBlkData()
{
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	LONG lUpperLeftX, lUpperLeftY, lLowerRightX, lLowerRightY, lIsSetup;
	BOOL bIsSetup;

	//Check Load/Save Data
	if (pBTfile == NULL)
		return FALSE;

	ULONG ulBlkId = 1;

	for (ulBlkId=1; ulBlkId<=GetNoOfBlk(); ulBlkId++)
	{
		//save data values from smf to data structure
		if (((!((*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftX"])) == FALSE)
			&&
			((!((*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightX"])) == FALSE))
		{
			lUpperLeftX = (*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftX"];
			lLowerRightX = (*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightX"];
		}
		else
		{
			lUpperLeftX = 0;
			lLowerRightX = 0;
		}

		if (((!((*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftY"])) == FALSE)
			&&
			((!((*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightY"])) == FALSE))
		{
			lUpperLeftY = (*pBTfile)["PhysicalBlock"][ulBlkId]["UpperLeftY"];
			lLowerRightY = (*pBTfile)["PhysicalBlock"][ulBlkId]["LowerRightY"];
		}
		else
		{
			lUpperLeftY = 0;
			lLowerRightY = 0;
		}

		m_oPhyBlk[ulBlkId].SetUpperLeftX(lUpperLeftX);
		m_oPhyBlk[ulBlkId].SetLowerRightX(lLowerRightX);
		m_oPhyBlk[ulBlkId].SetWidth();
		m_oPhyBlk[ulBlkId].SetUpperLeftY(lUpperLeftY);
		m_oPhyBlk[ulBlkId].SetLowerRightY(lLowerRightY);
		m_oPhyBlk[ulBlkId].SetHeight();

		if ((!((*pBTfile)["PhysicalBlock"][ulBlkId]["IsSetup"])) == FALSE)
		{
			lIsSetup = (*pBTfile)["PhysicalBlock"][ulBlkId]["IsSetup"];
			bIsSetup = BOOL(lIsSetup);
		}
		else
		{
			bIsSetup = FALSE;
		}

		m_oPhyBlk[ulBlkId].SetIsSetup(bIsSetup);
	}

	CMSFileUtility::Instance()->CloseBTConfig();
	return TRUE;

} //end InitPhyBlkData


/***********************************/
/*     Save to file functions      */
/***********************************/
BOOL CPhyBlkMain::SaveAllPhyBlkSetupData(ULONG ulNoOfBlk, LONG lBlkPitchX, LONG lBlkPitchY)
{
	ULONG i;
	ULONG ulOldNoOfBlk;
	CString szTemp;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	
	//Check Load/Save Data
	if (pBTfile == NULL)
		return FALSE;

	ulOldNoOfBlk = (*pBTfile)["PhysicalBlock"]["NoOfBlk"];
	//Remove old values
	for (i = 1; i <= ulOldNoOfBlk; i++)
	{
		szTemp.Format("%d", i);
		(*pBTfile)["PhysicalBlock"].Remove(szTemp);
	}

	//Write new values
	(*pBTfile)["PhysicalBlock"]["NoOfBlk"] = ulNoOfBlk;
	(*pBTfile)["PhysicalBlock"]["BlkPitchX"] = lBlkPitchX;
	(*pBTfile)["PhysicalBlock"]["BlkPitchY"] = lBlkPitchY;

	for (i = 1; i <= ulNoOfBlk; i++)
	{
		szTemp.Format("%d", i);
		(*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftX"] = 
													m_oPhyBlk[i].GetUpperLeftX();
		(*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftY"] = 
													m_oPhyBlk[i].GetUpperLeftY();
		(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightX"] = 
													m_oPhyBlk[i].GetLowerRightX();
		(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightY"] = 
													m_oPhyBlk[i].GetLowerRightY();
		(*pBTfile)["PhysicalBlock"][szTemp]["IsSetup"] =
													m_oPhyBlk[i].GetIsSetup();
	}

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;

} //end SaveAllPhyBlkSetupData 


BOOL CPhyBlkMain::SaveSinglePhyBlkSetupData(ULONG ulNoOfBlk, ULONG ulBlkToSetup, 
									   LONG lUpperLeftX, LONG lUpperLeftY,
									   LONG lLowerRightX, LONG lLowerRightY)
{
	ULONG i;
	CString szTemp;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	if (pBTfile == NULL)
		return FALSE;

	//Remove old values
	for (i = ulNoOfBlk + 1; i <= (*pBTfile)["PhysicalBlock"]["NoOfBlk"]; i++)
	{
		szTemp.Format("%d", i);
		(*pBTfile)["PhysicalBlock"].Remove(szTemp);
	}

	//Write new values
	(*pBTfile)["PhysicalBlock"]["NoOfBlk"] = ulNoOfBlk;

	szTemp.Format("%d", ulBlkToSetup);
	(*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftX"] = 
													m_oPhyBlk[ulBlkToSetup].GetUpperLeftX();
	(*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftY"] = 
													m_oPhyBlk[ulBlkToSetup].GetUpperLeftY();
	(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightX"] = 
													m_oPhyBlk[ulBlkToSetup].GetLowerRightX();
	(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightY"] = 
													m_oPhyBlk[ulBlkToSetup].GetLowerRightY();
	(*pBTfile)["PhysicalBlock"][szTemp]["IsSetup"] = 
						m_oPhyBlk[ulBlkToSetup].GetIsSetup();

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;

} //end SaveSinglePhyBlkSetupData


/***********************************/
/*      BinTable Map Display       */
/***********************************/
ULONG CPhyBlkMain::ConvertUpperLeftX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
									 ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedULX;
	LONG lUpperLeftX;

	lUpperLeftX = m_oPhyBlk[ulBlkId].GetUpperLeftX();

	ulConvertedULX = (ULONG)(((lUpperLeftX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) + 15 
					+ ulBTXOffset;	//15 is the offset added for better looking

	ulConvertedULX -= 40; //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedULX = (ULONG)(((lUpperLeftX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) - 100 
//					+ ulBTXOffset;	//BT Home pos not at center, -100 to adjust the drawing
#endif

	return ulConvertedULX;
} //end ConvertUpperLeftX


ULONG CPhyBlkMain::ConvertUpperLeftY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
									 ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedULY;
	LONG lUpperLeftY;

	lUpperLeftY = m_oPhyBlk[ulBlkId].GetUpperLeftY();

	ulConvertedULY = (ULONG)(((lUpperLeftY + abs(lBTYNegLimit)) /lDisplayResol) + 0.5) + 15 
					+ ulBTYOffset;	//15 is the offset added for better looking

	ulConvertedULY -= 170;  //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedULY = (ULONG)(((lUpperLeftY + abs(lBTYNegLimit)) /lDisplayResol) + 0.5) + 100 
//				+ ulBTYOffset;	//BT Home pos not at center, +100 to adjust the drawing
#endif

	return ulConvertedULY;
} //end ConvertUpperLeftY


ULONG CPhyBlkMain::ConvertLowerRightX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
									  ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedLRX;
	LONG lLowerRightX;

	lLowerRightX = m_oPhyBlk[ulBlkId].GetLowerRightX();
		
	ulConvertedLRX = (ULONG)(((lLowerRightX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) + 15 
					+ ulBTXOffset;	//15 is the offset added for better looking

	ulConvertedLRX -= 40; //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedLRX = (ULONG)(((lLowerRightX + abs(lBTXNegLimit)) / lDisplayResol) + 0.5) - 100
//					+ ulBTXOffset;	//BT Home pos not at center, -100 to adjust the drawing
#endif

	return ulConvertedLRX;
} //end ConvertLowerRightX


ULONG CPhyBlkMain::ConvertLowerRightY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
									  ULONG ulBlkId, LONG lDisplayResol)
{
	ULONG ulConvertedLRY;
	LONG lLowerRightY;

	lLowerRightY = m_oPhyBlk[ulBlkId].GetLowerRightY();

	ulConvertedLRY = (ULONG)(((lLowerRightY + abs(lBTYNegLimit)) / lDisplayResol) + 0.5) + 15 
					+ ulBTYOffset;	//15 is the offset added for better looking

	ulConvertedLRY -= 170;  //BT Home pos not at center, -100 to adjust the drawing
#ifdef NU_MOTION
//	ulConvertedLRY = (ULONG)(((lLowerRightY + abs(lBTYNegLimit)) / lDisplayResol) + 0.5) + 100 
//					+ ulBTYOffset;	//BT Home pos not at center, +100 to adjust the drawing
#endif

	return ulConvertedLRY;
} //end ConvertLowerRightY

/***************************************/
/*    Clear Physical Block Settings    */
/***************************************/
VOID CPhyBlkMain::ClrAllBlksSettings()
{
	ULONG i;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	//Clear data structures
	m_ulNoOfBlk = 0;
	m_lBlkPitchX = 0;
	m_lBlkPitchY = 0;
	
	for (i = 1; i <= BT_MAX_PHYBLK_SIZE-1; i++)
	{
		m_oPhyBlk[i].SetUpperLeftX(0);
		m_oPhyBlk[i].SetUpperLeftY(0);
		m_oPhyBlk[i].SetLowerRightX(0);
		m_oPhyBlk[i].SetLowerRightY(0);
		m_oPhyBlk[i].SetHeight();
		m_oPhyBlk[i].SetWidth();
		m_oPhyBlk[i].SetIsSetup(FALSE);
	}

	//Check Load/Save Data
	if (pBTfile != NULL)		//Remove values from string map file
		(*pBTfile).Remove("PhysicalBlock");

	CMSFileUtility::Instance()->SaveBTConfig();
} //end ClrAllBlksSettings


VOID CPhyBlkMain::ClrSingleBlkSettings(ULONG ulBlkId)
{
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	//Clear data structure
	m_oPhyBlk[ulBlkId].SetUpperLeftX(0);
	m_oPhyBlk[ulBlkId].SetUpperLeftY(0);
	m_oPhyBlk[ulBlkId].SetLowerRightX(0);
	m_oPhyBlk[ulBlkId].SetLowerRightY(0);
	m_oPhyBlk[ulBlkId].SetHeight();
	m_oPhyBlk[ulBlkId].SetWidth();
	m_oPhyBlk[ulBlkId].SetIsSetup(FALSE);

	//Check Load/Save Data
	if (pBTfile != NULL)		//Remove values from string map file
		(*pBTfile)["PhysicalBlock"].Remove(ulBlkId);

	CMSFileUtility::Instance()->SaveBTConfig();
} //end ClrSingleBlkSettings


/***********************************/
/*      Physical Block Setup       */
/***********************************/

BOOL CPhyBlkMain::ValidateUpperLeft(ULONG ulBlkToTeach, LONG lBlkToTeachULX, LONG lBlkToTeachULY)
{
	ULONG i;

	//Check if inputted Upper Left is within existing physical block
	for (i = 1; i <= m_ulNoOfBlk; i++)
	{
		if ((i != ulBlkToTeach) && (m_oPhyBlk[i].GetIsSetup()))
		{
			if (((m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachULX) &&
				(lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()))
				&&
				((m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachULY) &&
				(lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY())))
						//inputted Upper Left is within existing physical block
			{
				//SetMsgCode("Error - Inputted Upper Left is within existing physical block!\nPlease re-teach.");
				SetMsgCode(IDS_BT_ULCHECK1);
				return FALSE;
			}
		}
	}

	return TRUE;
} //end ValidateUpperLeft


BOOL CPhyBlkMain::ValidateLowerRight(LONG lBlkToTeachULX, LONG lBlkToTeachULY, 
									LONG lBlkToTeachLRX, LONG lBlkToTeachLRY)
{
	//Check if inputted Lower Right is at Lower Right corner
	if ((lBlkToTeachLRX <= lBlkToTeachULX) || (lBlkToTeachLRY <= lBlkToTeachULY))
	{
		//SetMsgCode("Error: Inputted Lower Right is not at lower right corner!\nPlease re-teach.");
		SetMsgCode(IDS_BT_LRCHECK2);
		return FALSE;
	}

	return TRUE;
} //end ValidateLowerRight


/**************************************/
/*     All Physical Blocks Setup      */
/**************************************/
ULONG CPhyBlkMain::CalculateBlkPerRow(LONG lXNegLimit, ULONG ulXPosLimit, LONG lUpperLeftX, ULONG ulBlkWidth,
									LONG lBlkPitchX)
{
	ULONG ulBlkPerRow;

	if (ulBlkWidth + lBlkPitchX > 0)
		ulBlkPerRow = (ulXPosLimit - lUpperLeftX + lBlkPitchX) / (ulBlkWidth + lBlkPitchX);
	else
		ulBlkPerRow = 0;
	
	return ulBlkPerRow;
} //end CalculateBlkPerRow


ULONG CPhyBlkMain::CalculateBlkPerCol(LONG lYNegLimit, ULONG ulYPosLimit, LONG lUpperLeftY, 
									  ULONG ulBlkHeight, LONG lBlkPitchY)
{
	LONG ulBlkPerCol;

	if (ulBlkHeight + lBlkPitchY > 0)
		ulBlkPerCol = (ulYPosLimit - lUpperLeftY + lBlkPitchY) / (ulBlkHeight + lBlkPitchY);
	else
		ulBlkPerCol = 0;

	return ulBlkPerCol;
} //end CalculateBlkPerCol


BOOL CPhyBlkMain::SetAllULsAndLRs(ULONG ulNoOfBlk, LONG lBlkPitchX, LONG lBlkPitchY,
							LONG lUpperLeftX, LONG lUpperLeftY, LONG lLowerRightX,
							LONG lLowerRightY, ULONG ulBlkPerRow, ULONG ulBlkPerCol)
{
	BOOL bFuncRtnValue = TRUE;
	ULONG i;
	CString szTemp;

	if ( ulNoOfBlk > (ulBlkPerRow * ulBlkPerCol) )
	{
		bFuncRtnValue = FALSE;

		if ((ulBlkPerRow == 0) && (ulBlkPerCol == 0))
		{
			//SetMsgCode("Setup Error: Please re-teach Upper Left and Lower Right");
			SetMsgCode(IDS_BT_ULLRCHECK1);
		}
		else
		{
			//SetMsgCode("Setup Error: Bin table cannot hold the number of physical blocks with the specified dimensions!");
			SetMsgCode(IDS_BT_NOTENOUGHROOM);
		}
	}
	else
	{
		ClrAllBlksSettings();

		//setup 1st block
		m_oPhyBlk[1].SetUpperLeftX(lUpperLeftX);
		m_oPhyBlk[1].SetUpperLeftY(lUpperLeftY);
		m_oPhyBlk[1].SetLowerRightX(lLowerRightX);
		m_oPhyBlk[1].SetLowerRightY(lLowerRightY);

		m_oPhyBlk[1].SetWidth();
		m_oPhyBlk[1].SetHeight();

		m_oPhyBlk[1].SetIsSetup(TRUE);

		//setup from 2nd blk on
		for (i = 2; i <= ulNoOfBlk; i++)
		{
			if ( ulBlkPerCol == 1)
			{
				m_oPhyBlk[i].SetUpperLeftX(m_oPhyBlk[i-1].GetUpperLeftX()+
					m_oPhyBlk[i-1].GetWidth()+lBlkPitchX);
				m_oPhyBlk[i].SetUpperLeftY(m_oPhyBlk[i-1].GetUpperLeftY());

				m_oPhyBlk[i].SetLowerRightX(m_oPhyBlk[i-1].GetLowerRightX()+
						m_oPhyBlk[i-1].GetWidth()+lBlkPitchX);
				m_oPhyBlk[i].SetLowerRightY(m_oPhyBlk[i-1].GetLowerRightY());
			}
			else
			{
				if ( i % ulBlkPerCol != 1) //not 1st row
				{
					m_oPhyBlk[i].SetUpperLeftX(m_oPhyBlk[i-1].GetUpperLeftX());
					m_oPhyBlk[i].SetUpperLeftY(m_oPhyBlk[i-1].GetUpperLeftY()+
						m_oPhyBlk[1].GetHeight()+lBlkPitchY);

					m_oPhyBlk[i].SetLowerRightX(m_oPhyBlk[i-1].GetLowerRightX());
					m_oPhyBlk[i].SetLowerRightY(m_oPhyBlk[i-1].GetLowerRightY()+
						m_oPhyBlk[1].GetHeight()+lBlkPitchY);
				}
				else //on 1st row
				{
					m_oPhyBlk[i].SetUpperLeftX(m_oPhyBlk[i-ulBlkPerCol].GetUpperLeftX()+
						m_oPhyBlk[i-ulBlkPerCol].GetWidth()+lBlkPitchX);
					m_oPhyBlk[i].SetUpperLeftY(m_oPhyBlk[i-ulBlkPerCol].GetUpperLeftY());

					m_oPhyBlk[i].SetLowerRightX(m_oPhyBlk[i-ulBlkPerCol].GetLowerRightX()+
						m_oPhyBlk[i-ulBlkPerCol].GetWidth()+lBlkPitchX);
					m_oPhyBlk[i].SetLowerRightY(m_oPhyBlk[i-ulBlkPerCol].GetLowerRightY());
				}
			}
			m_oPhyBlk[i].SetWidth();
			m_oPhyBlk[i].SetHeight();
			m_oPhyBlk[i].SetIsSetup(TRUE);
		}	
	}

	return bFuncRtnValue;

} //end SetAllULsAndLRs


BOOL CPhyBlkMain::SetupAllBlks(ULONG ulNoOfBlk, LONG lBlkPitchX, LONG lBlkPitchY,
							LONG lUpperLeftX, LONG lUpperLeftY, LONG lLowerRightX,
							LONG lLowerRightY, LONG lXNegLimit, ULONG ulXPosLimit, 
							LONG lYNegLimit, ULONG ulYPosLimit)
{
	BOOL bFuncRtnValue;
	BOOL bEnoughRoom;
	ULONG ulBlkWidth;
	ULONG ulBlkHeight;
	ULONG ulBlkPerRow;    
	ULONG ulBlkPerCol;

	ulBlkWidth = abs(lLowerRightX - lUpperLeftX);
	ulBlkHeight = abs(lLowerRightY - lUpperLeftY);   

	ulBlkPerRow = CalculateBlkPerRow(lXNegLimit, ulXPosLimit, lUpperLeftX, ulBlkWidth, lBlkPitchX);
	
	ulBlkPerCol = CalculateBlkPerCol(lYNegLimit, ulYPosLimit, lUpperLeftY, ulBlkHeight, lBlkPitchY);

	bEnoughRoom = SetAllULsAndLRs(ulNoOfBlk, lBlkPitchX, lBlkPitchY, lUpperLeftX, lUpperLeftY, 
					lLowerRightX, lLowerRightY, ulBlkPerRow, ulBlkPerCol);

	if (bEnoughRoom) //bin table can hold the number of physical blks entered
	{
		SetNoOfBlk(ulNoOfBlk);
		SetBlkPitchX(lBlkPitchX);
		SetBlkPitchY(lBlkPitchY);

		SaveAllPhyBlkSetupData(ulNoOfBlk, lBlkPitchX, lBlkPitchY);
		bFuncRtnValue = TRUE;
	}
	else //bin table cannot hold the number of physical blks entered
	{
		bFuncRtnValue = FALSE;
	}

	return bFuncRtnValue;

} //end SetupAllBlks


BOOL CPhyBlkMain::SetupAllBlks_BL(ULONG ulNoOfBlk, LONG lUpperLeftX, LONG lUpperLeftY, 
								  LONG lLowerRightX, LONG lLowerRightY)
{
	ULONG i, ulBlkWidth, ulBlkHeight;

	m_ulNoOfBlk = ulNoOfBlk;
	m_lBlkPitchX = 0;
	m_lBlkPitchY = 0;

	ulBlkWidth = abs(lLowerRightX - lUpperLeftX);
	ulBlkHeight = abs(lLowerRightY - lUpperLeftY);  

	for (i = 1; i <= ulNoOfBlk; i++)
	{
		m_oPhyBlk[i].SetUpperLeftX(lUpperLeftX);
		m_oPhyBlk[i].SetUpperLeftY(lUpperLeftY);
		m_oPhyBlk[i].SetLowerRightX(lLowerRightX);
		m_oPhyBlk[i].SetLowerRightY(lLowerRightY);

		m_oPhyBlk[i].SetWidth();
		m_oPhyBlk[i].SetHeight();

		m_oPhyBlk[i].SetIsSetup(TRUE);
	}

	SaveAllPhyBlkSetupData(ulNoOfBlk, 0, 0);

	return TRUE;
} //end SetupAllBlks_BL


/************************************/
/*   Single Physical Block Setup    */
/************************************/
LONG CPhyBlkMain::GrabBlkUpperLeftX(ULONG ulBlkId)
{
	return m_oPhyBlk[ulBlkId].GetUpperLeftX();
} //end GrabBlkUpperLeftX


LONG CPhyBlkMain::GrabBlkUpperLeftY(ULONG ulBlkId)
{
	return m_oPhyBlk[ulBlkId].GetUpperLeftY();
} //end GrabBlkUpperLeftY


LONG CPhyBlkMain::GrabBlkLowerRightX(ULONG ulBlkId)
{
	return m_oPhyBlk[ulBlkId].GetLowerRightX();
} //end GrabBlkLowerRightX


LONG CPhyBlkMain::GrabBlkLowerRightY(ULONG ulBlkId)
{
	return m_oPhyBlk[ulBlkId].GetLowerRightY();
} //end GrabBlkLowerRightY


BOOL CPhyBlkMain::CheckOverlappingProblem(ULONG ulBlkToTeach, LONG lBlkToTeachULX, 
										  LONG lBlkToTeachULY, LONG lBlkToTeachLRX,
										  LONG lBlkToTeachLRY)
{
	if (CMS896AApp::m_bEnableSubBin == TRUE)
	{
		return TRUE;
	}
	BOOL bIsValidUpperLeft, bIsValidLowerRight;
	ULONG i;

	//Check if UL of BlkToTeach is within existing physical block
	bIsValidUpperLeft = ValidateUpperLeft(ulBlkToTeach, lBlkToTeachULX, lBlkToTeachULY);
	if (! bIsValidUpperLeft)
	{
		return FALSE;
	}

	//Check if LR is at lower-right corner
	bIsValidLowerRight = ValidateLowerRight(lBlkToTeachULX, lBlkToTeachULY,
							lBlkToTeachLRX, lBlkToTeachLRY);
	if (! bIsValidLowerRight)
	{
		return FALSE;
	}

	for (i = 1; i <= m_ulNoOfBlk; i++)
	{
		if ((i != ulBlkToTeach) && (m_oPhyBlk[i].GetIsSetup()))
		{
			//Check if UR of BlkToTeach is within existing physical block
			if (((m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachLRX) &&
				(lBlkToTeachLRX <= m_oPhyBlk[i].GetLowerRightX()))
				&&
				((m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachULY) &&
				(lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY())))
			{
				//SetMsgCode("Error: Inputted Upper Right is within existing physical block!\nPlease re-teach.");
				SetMsgCode(IDS_BT_URCHECK1);
				return FALSE;				
			}

			//Check if LL of BlkToTeach is within existing physical block
			if (((m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachULX) &&
				(lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()))
				&&
				((m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachLRY) &&
				(lBlkToTeachLRY <= m_oPhyBlk[i].GetLowerRightY())))
			{
				//SetMsgCode("Error: Inputted Lower Left is within existing physical block!\nPlease re-teach.");
				SetMsgCode(IDS_BT_LLCHECK1);

				return FALSE;				
			}	

			//Check if UL and LL of existing physical block is within BlkToTeach
			if (((lBlkToTeachULX <= m_oPhyBlk[i].GetUpperLeftX()) &&
				(m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetUpperLeftY()) &&
				(m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachLRY))
				&&
				((lBlkToTeachULX <= m_oPhyBlk[i].GetUpperLeftX()) &&
				(m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY()) &&
				(m_oPhyBlk[i].GetLowerRightY() <= lBlkToTeachLRY)))

			{
				//SetMsgCode("Error: UL and LL of existing physical block are within Block To Teach!\nPlease re-teach.");
				SetMsgCode(IDS_BT_ULLLCHECK1);
				return FALSE;
			}

			//Check if LR and UR of existing physical block is within BlkToTeach
			if (((lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()) &&
				(m_oPhyBlk[i].GetLowerRightX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY()) &&
				(m_oPhyBlk[i].GetLowerRightY() <= lBlkToTeachLRY))
				&&
				((lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()) &&
				(m_oPhyBlk[i].GetLowerRightX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetUpperLeftY()) &&
				(m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachLRY)))
			{
				//SetMsgCode("Error: LR and UR of existing physical block are within Block To Teach!\nPlease re-teach.");
				SetMsgCode(IDS_BT_LRURCHECK1);
				return FALSE;
			}

			//Check if UR and UL of existing physical block is within BlkToTeach
			if (((lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()) &&
				(m_oPhyBlk[i].GetLowerRightX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetUpperLeftY()) &&
				(m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachLRY))
				&&
				((lBlkToTeachULX <= m_oPhyBlk[i].GetUpperLeftX()) &&
				(m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetUpperLeftY()) &&
				(m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachLRY)))
			{
				//SetMsgCode("Error: UR and UL of existing physical block are within Block To Teach!\nPlease re-teach.");
				SetMsgCode(IDS_BT_URULCHECK1);
				return FALSE;
			}

			//Check if LL and LR of existing physical block is within BlkToTeach
			if (((lBlkToTeachULX <= m_oPhyBlk[i].GetUpperLeftX()) &&
				(m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY()) &&
				(m_oPhyBlk[i].GetLowerRightY() <= lBlkToTeachLRY))
				&&
				((lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()) &&
				(m_oPhyBlk[i].GetLowerRightX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY()) &&
				(m_oPhyBlk[i].GetLowerRightY() <= lBlkToTeachLRY)))
			{
				//SetMsgCode("Error: LL and LR of existing physical block are within Block To Teach!\nPlease re-teach.");
				SetMsgCode(IDS_BT_LLLRCHECK1);
				return FALSE;
			}

			//Check if BlkToTeach overlaps with existing physical block (Case 1)
			if (((lBlkToTeachULX <= m_oPhyBlk[i].GetUpperLeftX()) &&
				(m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachLRX))
				&&
				((lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()) &&
				(m_oPhyBlk[i].GetLowerRightX() <= lBlkToTeachLRX))
				&&
				((m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachULY) &&
				(lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY()))
				&&
				((m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachLRY) &&
				(lBlkToTeachLRY <= m_oPhyBlk[i].GetLowerRightY())))
			{
				//SetMsgCode("Error: Block to teach overlaps with existing physical block!\nPlease re-teach.");
				SetMsgCode(IDS_BT_OVERLAPPINGCHK1);
				return FALSE;
			}

			//Check if BlkToTeach overlaps with existing physical block (Case 2)
			if (((lBlkToTeachULY <= m_oPhyBlk[i].GetUpperLeftY()) &&
				(m_oPhyBlk[i].GetUpperLeftY() <= lBlkToTeachLRY))
				&&
				((lBlkToTeachULY <= m_oPhyBlk[i].GetLowerRightY()) &&
				(m_oPhyBlk[i].GetLowerRightY() <= lBlkToTeachLRY))
				&&
				((m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachULX) &&
				(lBlkToTeachULX <= m_oPhyBlk[i].GetLowerRightX()))
				&&
				((m_oPhyBlk[i].GetUpperLeftX() <= lBlkToTeachLRX) &&
				(lBlkToTeachLRX <= m_oPhyBlk[i].GetLowerRightX())))
			{
				//SetMsgCode("Error: Block to teach overlaps with existing physical block!\nPlease re-teach.");
				SetMsgCode(IDS_BT_OVERLAPPINGCHK1);
				return FALSE;
			}
		}
	}

	return TRUE;
} //end CheckOverlappingProblem


BOOL CPhyBlkMain::SetULandLR(ULONG ulBlkToSetup, LONG lUpperLeftX, LONG lUpperLeftY,
							LONG lLowerRightX, LONG lLowerRightY)
{
	m_oPhyBlk[ulBlkToSetup].SetUpperLeftX(lUpperLeftX);
	m_oPhyBlk[ulBlkToSetup].SetUpperLeftY(lUpperLeftY);
	m_oPhyBlk[ulBlkToSetup].SetLowerRightX(lLowerRightX);
	m_oPhyBlk[ulBlkToSetup].SetLowerRightY(lLowerRightY);
	m_oPhyBlk[ulBlkToSetup].SetIsSetup(TRUE);

	return TRUE;
} //end SetULandLR


BOOL CPhyBlkMain::SetupSingleBlk(ULONG ulNoOfBlk, ULONG ulBlkToSetup, LONG lUpperLeftX,
							LONG lUpperLeftY, LONG lLowerRightX, LONG lLowerRightY)
{
	BOOL bIsValidSetup, bIsValidBlkDimension;

	bIsValidSetup = FALSE;

	bIsValidBlkDimension = CheckOverlappingProblem(ulBlkToSetup, lUpperLeftX,
										lUpperLeftY, lLowerRightX, lLowerRightY);

	if (bIsValidBlkDimension)
	{
		bIsValidSetup = TRUE;
		SetNoOfBlk(ulNoOfBlk);
		SetULandLR(ulBlkToSetup, lUpperLeftX, lUpperLeftY, lLowerRightX, lLowerRightY);
		SaveSinglePhyBlkSetupData(ulNoOfBlk, ulBlkToSetup, lUpperLeftX,lUpperLeftY, 
									lLowerRightX, lLowerRightY);
	}

	return bIsValidSetup;
} //end SetupSingleBlk


BOOL CPhyBlkMain::SetupSingleBlk_BL(ULONG ulNoOfBlk, ULONG ulBlkToSetup, LONG lUpperLeftX,
									LONG lUpperLeftY, LONG lLowerRightX, LONG lLowerRightY)
{
	ULONG ulBlkWidth, ulBlkHeight;

	m_ulNoOfBlk = ulNoOfBlk;
	m_lBlkPitchX = 0;
	m_lBlkPitchY = 0;

	ulBlkWidth = abs(lLowerRightX - lUpperLeftX);
	ulBlkHeight = abs(lLowerRightY - lUpperLeftY);  

	m_oPhyBlk[ulBlkToSetup].SetUpperLeftX(lUpperLeftX);
	m_oPhyBlk[ulBlkToSetup].SetUpperLeftY(lUpperLeftY);
	m_oPhyBlk[ulBlkToSetup].SetLowerRightX(lLowerRightX);
	m_oPhyBlk[ulBlkToSetup].SetLowerRightY(lLowerRightY);

	m_oPhyBlk[ulBlkToSetup].SetWidth();
	m_oPhyBlk[ulBlkToSetup].SetHeight();

	m_oPhyBlk[ulBlkToSetup].SetIsSetup(TRUE);

	SaveSinglePhyBlkSetupData(ulNoOfBlk, ulBlkToSetup, lUpperLeftX, lUpperLeftY,
								lLowerRightX, lLowerRightY);

	return TRUE;
} //end SetupSingleBlk_BL


ULONG CPhyBlkMain::CalculateTotalNoOfBinBlksCanHold(LONG lBinBlk1ULX, LONG lBinBlk1ULY,
							LONG lBinBlkLRX, LONG lBinBlkLRY, LONG lBinBlkPitchX, 
							LONG lBinBlkPitchY)
{
	ULONG i;
	LONG lXOffset, lYOffset;
	//LONG lPhyBlkWidth, lPhyBlkHeight;
	LONG lBinBlkWidth, lBinBlkHeight;
	ULONG ulBlkPerRow, ulBlkPerCol;
	ULONG ulTotalNoOfBinBlks = 0;

	lXOffset = lBinBlk1ULX - m_oPhyBlk[1].GetUpperLeftX();
	lYOffset = lBinBlk1ULY - m_oPhyBlk[1].GetUpperLeftY();

	lBinBlkWidth = lBinBlkLRX - lBinBlk1ULX;
	lBinBlkHeight = lBinBlkLRY - lBinBlk1ULY;

	for (i = 1; i <= m_ulNoOfBlk; i++)
	{
		if (m_oPhyBlk[i].GetIsSetup())
		{
			//debug only
			//CString szTemp;
			//szTemp.Format("%d %d %d %d %d", i, m_oPhyBlk[i].GetUpperLeftX(),
			//				m_oPhyBlk[i].GetUpperLeftY(), m_oPhyBlk[i].GetLowerRightX(),
			//				m_oPhyBlk[i].GetLowerRightY());
			//AfxMessageBox(szTemp);

			//lPhyBlkWidth = m_oPhyBlk[i].GetLowerRightX() - m_oPhyBlk[i].GetUpperLeftX();
			//lPhyBlkHeight = m_oPhyBlk[i].GetLowerRightY() - m_oPhyBlk[i].GetUpperLeftY();

			ulBlkPerRow = 
					m_oPhyBlk[i].CalculateBinBlkPerRow(lBinBlkWidth, lBinBlkPitchX, lXOffset);
			ulBlkPerCol = 
					m_oPhyBlk[i].CalculateBinBlkPerCol(lBinBlkHeight, lBinBlkPitchY, lYOffset);
			ulTotalNoOfBinBlks = ulTotalNoOfBinBlks + ulBlkPerRow * ulBlkPerCol;
		}
		//for debug use only
		//if (i == 1)
		//{
		//	CString szTemp;
		//	szTemp.Format("%d %d", ulBlkPerRow, ulBlkPerCol);
		//	AfxMessageBox(szTemp);
		//}
	}

	return ulTotalNoOfBinBlks;

} //end CalculateTotalNoOfBinBlksCanHold

/***********************************************/
/*     Supplementary for Bin Block Setup       */
/***********************************************/
ULONG CPhyBlkMain::GrabNoOfBinBlkPerRow(ULONG ulBlkId)
{
	return m_oPhyBlk[ulBlkId].GetNoOfBinBlkPerRow();
} //end GrabNoOfBinBlkPerRow


ULONG CPhyBlkMain::GrabNoOfBinBlkPerCol(ULONG ulBlkId)
{
	return m_oPhyBlk[ulBlkId].GetNoOfBinBlkPerCol();
} //end GrabNoOfBinBlkPerCol


BOOL CPhyBlkMain::GrabIsSetup(ULONG ulBlkId)
{
	return m_oPhyBlk[ulBlkId].GetIsSetup();
} //end GrabIsSetup

/***************************** Class CPhyBlkMain End *****************************/