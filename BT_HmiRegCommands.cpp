#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "HmiDataManager.h"
#include "BinTable.h"
#include "FileUtil.h"
#include "stdio.h"

#pragma once



/***********************************/
/*     HMI reg service commands    */
/***********************************/
//DEBUG TEST function --> to be erased later
LONG CBinTable::Testing123(IPC_CServiceMessage& svMsg)
{
	LONG lX, lY;
	ULONG ulBondedBlk;
	LONG lWaferX, lWaferY;

	lWaferX = 12345;
	lWaferY = 67890;
	
	//CString szDebug;
	//szDebug.Format("%d", m_ucGrade);
	//AfxMessageBox(szDebug);
	

	ulBondedBlk = GrabBondXYPosn(m_ucGrade, lX, lY);

	if (ulBondedBlk == -1)
		HmiMessage(m_oBinBlkMain.GetMsgCode());
	else
	{
		//debug only
		//CString szDebug;
		//szDebug.Format("%d %d %d", m_ucGrade, lX, lY);
		//AfxMessageBox(szDebug);
		//debug only

		IfBondOK(ulBondedBlk, lWaferX, lWaferY); //hardcode for now
		SaveBondResult(m_ucGrade, lX, lY, lWaferX, lWaferY);
		
	}


	return 1;
} //end Testing 123


LONG CBinTable::FunctionReply(IPC_CServiceMessage& svMsg)
{
	svMsg.InitMessage(sizeof(BOOL), &m_bReply);

	return 1;
} //end FunctionReply


//////////////////////////////////////////////////////////
//					Physical Block Setup				//
//////////////////////////////////////////////////////////
LONG CBinTable::PhyBlkSetupPreRoutine(IPC_CServiceMessage& svMsg)
{
	//get the BT config file handle
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	m_ulNoOfPhyBlk = (*pBTfile)["PhysicalBlock"]["NoOfBlk"];

	m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["UpperLeftX"]);
	m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["UpperLeftY"]);
	m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["LowerRightX"]);
	m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["LowerRightY"]);

	m_lPhyBlkPitchX = ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["BlkPitchX"]);
	m_lPhyBlkPitchY = ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["BlkPitchY"]);

   
	RunMap(svMsg);
	return 1;
} //end PhyBlkSetupPreRoutine


LONG CBinTable::Display1stPhyBlkData(IPC_CServiceMessage& svMsg)
{
	//display 1st blk data
	//m_lPhyUpperLeftX = m_oPhyBlkMain.GrabBlkUpperLeftX(1);
	//m_lPhyUpperLeftY = m_oPhyBlkMain.GrabBlkUpperLeftY(1);
	//m_lPhyLowerRightX = m_oPhyBlkMain.GrabBlkLowerRightX(1);
	//m_lPhyLowerRightY = m_oPhyBlkMain.GrabBlkLowerRightY(1);

	m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(1));
	m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(1));
	m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(1));
	m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(1));

	return 1;
} //end Display1stPhyBlkData


LONG CBinTable::SetPhyBlkSetupUL(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(TRUE);
	m_ulJoystickFlag = 1;

	return 1;

} //end SetPhyBlkSetupUL


LONG CBinTable::ConfirmPhyBlkSetupUL(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(FALSE);
	m_ulJoystickFlag = 0;

	return 1;
} //end ConfirmPhyBlkSetupUL


LONG CBinTable::SetPhyBlkSetupLR(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(TRUE);
	m_ulJoystickFlag = 2;

	return 1;
} //end SetPhyBlkSetupLR


LONG CBinTable::ConfirmAllPhyBlksSetupLR(IPC_CServiceMessage& svMsg)
{
	BOOL bIsValidLowerRight;
	LONG lConvertedULX;
	LONG lConvertedULY;
	LONG lConvertedLRX;
	LONG lConvertedLRY;

	SetJoystickOn(FALSE);

	lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
	lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
	lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
	lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

	bIsValidLowerRight = m_oPhyBlkMain.ValidateLowerRight(lConvertedULX, lConvertedULY,
									 lConvertedLRX, lConvertedLRY);
	if (! bIsValidLowerRight)
		HmiMessage(m_oPhyBlkMain.GetMsgCode());

	m_ulJoystickFlag = 0;

	return 1;
} //end ConfirmAllPhyBlksSetupLR


LONG CBinTable::SubmitAllPhyBlksSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bFlag;
	LONG lHmiMsgReply;
	LONG lConvertedULX;
	LONG lConvertedULY;
	LONG lConvertedLRX;
	LONG lConvertedLRY;


	if  (m_bClrAllPhyBlkSettings) //Clear All Physical Blks Setting is checked
	{
		lHmiMsgReply = HmiMessage("Are you sure to Clear ALL Physical Block Settings?", 
			"Clear All Blocks Settings", 103, 3); //103 = Yes ,No
											      //3 = align Center

		if (lHmiMsgReply == 3) //3 = Yes
		{
			m_oPhyBlkMain.ClearAllBlkSettings();
			m_BinTableWrapper.DeleteAllBoundary();   
			m_ulNoOfPhyBlk = 0;	
			m_lPhyBlkPitchX = 0;
			m_lPhyBlkPitchY = 0;
			m_lPhyUpperLeftX = 0;
			m_lPhyUpperLeftY = 0;
			m_lPhyLowerRightX = 0;
			m_lPhyLowerRightY = 0;

			HmiMessage("All block settings have been cleared!");
		}
		m_bClrAllPhyBlkSettings = FALSE; //clear this flag
		m_bReply = FALSE; //commands below WON'T be executed
	}
	else //Clear All Physical Blks Setting is unchecked
	{
		//bFlag = m_oPhyBlkMain.SetupAllBlks(m_ulNoOfPhyBlk, m_lPhyBlkPitchX, m_lPhyBlkPitchY, 
		//					m_lPhyUpperLeftX, m_lPhyUpperLeftY, m_lPhyLowerRightX, m_lPhyLowerRightY,
		//					m_lXNegLimit, m_lXPosLimit, m_lYNegLimit, m_lYPosLimit);

		lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
		lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
		lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
		lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);


		bFlag = m_oPhyBlkMain.SetupAllBlks(m_ulNoOfPhyBlk, 
						ConvertDisplayUnitToFileUnit(m_lPhyBlkPitchX), 
						ConvertDisplayUnitToFileUnit(m_lPhyBlkPitchY), 
						lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY,
						m_lXNegLimit, m_lXPosLimit, m_lYNegLimit, m_lYPosLimit);

		if (!bFlag) //setup fail!
		{
			HmiMessage(m_oPhyBlkMain.GetMsgCode());
			SetStatusMessage(m_oPhyBlkMain.GetMsgCode());
		} 
		else //setup OK
		{
			HmiMessage("Setup Complete!");
			SetStatusMessage("Setup Complete");
			m_bCreated = TRUE;

			//update the data structures
			CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

			m_oPhyBlkMain.InitPhyBlkMainData((*pBTfile)["PhysicalBlock"]["NoOfBlk"], 
			(*pBTfile)["PhysicalBlock"]["BlkPitchX"], (*pBTfile)["PhysicalBlock"]["BlkPitchY"]);

			ULONG i;
			CString szTemp;
			ULONG ulTemp;

			m_oPhyBlkMain.InitPhyBlkData(0, 0, 0, 0, 0, FALSE); //index 0

			for (i = 1; i <= (*pBTfile)["PhysicalBlock"]["NoOfBlk"]; i++)
			{
				szTemp.Format("%d", i);
				m_oPhyBlkMain.InitPhyBlkData(i, (*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftX"],
					(*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftY"], 
					(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightX"],
					(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightY"], TRUE);
			}

			ulTemp = (*pBTfile)["PhysicalBlock"]["NoOfBlk"];
			for (i = ulTemp + 1; i < 49; i++)
			{
				m_oPhyBlkMain.InitPhyBlkData(i, 0, 0, 0, 0, FALSE); 
			}
			   
		}
		m_bReply = TRUE; //commands below WILL be executed
		
	} 
	//svMsg.InitMessage(sizeof(BOOL), &m_bReply);

	return 1;      
} //end SubmitAllPhyBlksSetup


LONG CBinTable::CancelAllPhyBlksSetup(IPC_CServiceMessage& svMsg)
{
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	//restore original data values from file
	m_ulNoOfPhyBlk = (*pBTfile)["PhysicalBlock"]["NoOfBlk"];
	m_lPhyBlkPitchX = ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["BlkPitchX"]);
	m_lPhyBlkPitchY = ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["BlkPitchY"]);
	m_lPhyUpperLeftX = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["UpperLeftX"]);
	m_lPhyUpperLeftY = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["UpperLeftY"]);
	m_lPhyLowerRightX = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["LowerRightX"]);
	m_lPhyLowerRightY = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["LowerRightY"]);

	return 1;
} //end CancelAllPhyBlksSetup


LONG CBinTable::DisplaySinglePhyBlkData(IPC_CServiceMessage& svMsg)
{
	ULONG ulData;
	try
	{
		svMsg.GetMsg(sizeof(ULONG), &ulData);
	}
	catch (CAsmException e)
	{
		return -1;
	}

	//display blk dimensions upon selecting "blk to setup"
	//m_lPhyUpperLeftX = m_oPhyBlkMain.GrabBlkUpperLeftX(ulData);	
	//m_lPhyUpperLeftY = m_oPhyBlkMain.GrabBlkUpperLeftY(ulData);
	//m_lPhyLowerRightX = m_oPhyBlkMain.GrabBlkLowerRightX(ulData);
	//m_lPhyLowerRightY = m_oPhyBlkMain.GrabBlkLowerRightY(ulData);

	m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(ulData));	
	m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(ulData));
	m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(ulData));
	m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(ulData));

	return 1;
} //end DisplaySinglePhyBlkData


LONG CBinTable::ConfirmSinglePhyBlkSetupLR(IPC_CServiceMessage& svMsg)
{
	BOOL bIsValidLowerRight;
	BOOL bIsValidDimension;
	LONG lConvertedULX;
	LONG lConvertedULY;
	LONG lConvertedLRX;
	LONG lConvertedLRY;

	SetJoystickOn(FALSE);

	lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
	lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
	lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
	lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);


	bIsValidLowerRight = m_oPhyBlkMain.ValidateLowerRight(lConvertedULX, lConvertedULY, 
						lConvertedLRX, lConvertedLRY);
	if (! bIsValidLowerRight)
	{
		HmiMessage(m_oPhyBlkMain.GetMsgCode());

		//restore old file values
		m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(m_ulPhyBlkToSetup));	
		m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulPhyBlkToSetup));
		m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(m_ulPhyBlkToSetup));
		m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(m_ulPhyBlkToSetup));
	}
	else
	{
		bIsValidDimension = m_oPhyBlkMain.ValidateBlkDimension(m_ulNoOfPhyBlk, 
			m_ulPhyBlkToSetup, lConvertedULX, lConvertedULY, lConvertedLRX, lConvertedLRY);

		if (! bIsValidDimension)
		{
			HmiMessage(m_oPhyBlkMain.GetMsgCode());

			//restore old file values
			m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(m_ulPhyBlkToSetup));	
			m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulPhyBlkToSetup));
			m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(m_ulPhyBlkToSetup));
			m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(m_ulPhyBlkToSetup));
		}
	} 

	return 1;
} //end ConfirmSinglePhyBlkSetupLR


LONG CBinTable::SubmitSinglePhyBlkSetup(IPC_CServiceMessage& svMsg)
{
	BOOL bIsValidSetup;
	LONG lConvertedULX;
	LONG lConvertedULY;
	LONG lConvertedLRX;
	LONG lConvertedLRY;
	

	if (m_bClrSinglePhyBlkSettings)
	{
		m_oPhyBlkMain.ClearSingleBlkSetting(m_ulPhyBlkToClrSettings);
		
		m_bReply = TRUE; //updated bintable map will show
	}
	else
	{
		lConvertedULX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftX);
		lConvertedULY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyUpperLeftY);
		lConvertedLRX = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightX);
		lConvertedLRY = 0 - ConvertDisplayUnitToFileUnit(m_lPhyLowerRightY);

		bIsValidSetup = m_oPhyBlkMain.SetupSingleBlk(m_ulNoOfPhyBlk, m_ulPhyBlkToSetup, 
							lConvertedULX, lConvertedULY, lConvertedLRX,
							lConvertedLRY);

		if (bIsValidSetup)
		{
			HmiMessage("Setup Complete!");
			SetStatusMessage("Setup Complete!");
			m_bReply = TRUE; //bintable will show

			//update the data structures
			CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

			m_oPhyBlkMain.InitPhyBlkMainData((*pBTfile)["PhysicalBlock"]["NoOfBlk"], 
			(*pBTfile)["PhysicalBlock"]["BlkPitchX"], (*pBTfile)["PhysicalBlock"]["BlkPitchY"]);

			ULONG i;
			CString szTemp;
			ULONG ulTemp;

			m_oPhyBlkMain.InitPhyBlkData(0, 0, 0, 0, 0, FALSE); //index 0

			for (i = 1; i <= (*pBTfile)["PhysicalBlock"]["NoOfBlk"]; i++)
			{
				szTemp.Format("%d", i);
				m_oPhyBlkMain.InitPhyBlkData(i, (*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftX"],
					(*pBTfile)["PhysicalBlock"][szTemp]["UpperLeftY"], 
					(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightX"],
					(*pBTfile)["PhysicalBlock"][szTemp]["LowerRightY"], TRUE);
			}

			ulTemp = (*pBTfile)["PhysicalBlock"]["NoOfBlk"];
			for (i = ulTemp + 1; i < 49; i++)
			{
				m_oPhyBlkMain.InitPhyBlkData(i, 0, 0, 0, 0, FALSE); 
			}
		}
		else //setup fail!
		{
			HmiMessage(m_oPhyBlkMain.GetMsgCode());
			SetStatusMessage(m_oPhyBlkMain.GetMsgCode());
			m_bReply = FALSE; //NO bintable map will show
			//restore old file values
			m_lPhyUpperLeftX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftX(m_ulPhyBlkToSetup));	
			m_lPhyUpperLeftY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkUpperLeftY(m_ulPhyBlkToSetup));
			m_lPhyLowerRightX = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightX(m_ulPhyBlkToSetup));
			m_lPhyLowerRightY = 0 - ConvertFileUnitToDisplayUnit(m_oPhyBlkMain.GrabBlkLowerRightY(m_ulPhyBlkToSetup));
		}
	}

	svMsg.InitMessage(sizeof(BOOL), &m_bReply);

	return 1;

} //end SubmitSinglePhyBlkSetup

   
LONG CBinTable::CancelSinglePhyBlkSetup(IPC_CServiceMessage& svMsg)
{
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	//restore original data values from file 
	m_ulNoOfPhyBlk = (*pBTfile)["PhysicalBlock"]["NoOfBlk"];
	m_ulPhyBlkToSetup = 1;
	m_lPhyUpperLeftX = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["UpperLeftX"]);
	m_lPhyUpperLeftY = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["UpperLeftY"]);
	m_lPhyLowerRightX = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["LowerRightX"]);
	m_lPhyLowerRightY = 
			0 - ConvertFileUnitToDisplayUnit((*pBTfile)["PhysicalBlock"]["1"]["LowerRightY"]);

	return 0;

} //end CancelSinglePhyBlkSetup


//////////////////////////////////////////////////////////
//			  	      Bin Block Setup		     		//
//////////////////////////////////////////////////////////

LONG CBinTable::BinBlkSetupPreRoutine(IPC_CServiceMessage& svMsg)
{
	//get the BT config file handle
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	m_ulNoOfBinBlk = (*pBTfile)["BinBlock"]["NoOfBlk"];

	m_lBinUpperLeftX = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["1"]["UpperLeftX"]);
	m_lBinUpperLeftY = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["1"]["UpperLeftY"]);
	m_lBinLowerRightX = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["1"]["LowerRightX"]);
	m_lBinLowerRightY = 0 - ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["1"]["LowerRightY"]);

	m_lBinBlkPitchX = ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["BlkPitchX"]);
	m_lBinBlkPitchY = ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["BlkPitchY"]);

	m_lDiePitchX = ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["1"]["DiePitchX"]);
	m_lDiePitchY = ConvertFileUnitToDisplayUnit((*pBTfile)["BinBlock"]["1"]["DiePitchY"]);

	//m_ulWalkPath = (*pBTfile)["BinBlock"]["1"]["WalkPath"];
	//ULONG ulWalkPath = (*pBTfile)["BinBlock"]["1"]["WalkPath"];   

	//if (ulWalkPath == 1)
	//	m_szWalkPath = "TL-Horizontal";
	//else if (ulWalkPath == 2)
	//	m_szWalkPath = "TL-Vertical";
	
	//debug only
	//AfxMessageBox(m_szWalkPath);
	
	m_ulDiePerRow = (*pBTfile)["BinBlock"]["1"]["DiePerRow"];
	m_ulDiePerBlk = (*pBTfile)["BinBlock"]["1"]["DiePerBlk"];
	

	RunBinBlkMap(svMsg);
	return 1;
} //end BinBlkSetupPreRoutine

LONG CBinTable::SetBinBlkSetupUL(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(TRUE);
	m_ulJoystickFlag = 3;

	return 1;
} //end SetBinBlkSetupUL


LONG CBinTable::SetBinBlkSetupLR(IPC_CServiceMessage& svMsg)
{
	SetJoystickOn(TRUE);
	m_ulJoystickFlag = 4;

	return 1;
} //end SetBinBlkSetupLR


LONG CBinTable::ConfirmAllBinBlksSetupUL(IPC_CServiceMessage& svMsg)
{
	BOOL bIsValidUL;
	LONG lPhyBlk1ULX;
	LONG lPhyBlk1ULY;
	LONG lPhyBlk1LRX;
	LONG lPhyBlk1LRY;
	LONG lBinBlkULX;
	LONG lBinBlkULY;

	SetJoystickOn(FALSE);

	lPhyBlk1ULX = m_oPhyBlkMain.GrabBlkUpperLeftX(1);
	lPhyBlk1ULY = m_oPhyBlkMain.GrabBlkUpperLeftY(1);
	lPhyBlk1LRX = m_oPhyBlkMain.GrabBlkLowerRightX(1);
	lPhyBlk1LRY = m_oPhyBlkMain.GrabBlkLowerRightY(1);
	lBinBlkULX = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
	lBinBlkULY = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);

	bIsValidUL = m_oBinBlkMain.ValidateAllSetupUL(lPhyBlk1ULX, lPhyBlk1ULY, lPhyBlk1LRX, lPhyBlk1LRY, 
										lBinBlkULX, lBinBlkULY);

	if (! bIsValidUL)
	{
		HmiMessage("Error - Upper Left is not on Physical Block 1!\nPlease re-enter");
	}

	return 1;
} //end ConfirmAllBinBlksSetupUL


LONG CBinTable::ConfirmAllBinBlksSetupLR(IPC_CServiceMessage& svMsg)
{
	LONG lIsValidLR;
	LONG lPhyBlk1ULX;
	LONG lPhyBlk1ULY;
	LONG lPhyBlk1LRX;
	LONG lPhyBlk1LRY;
	LONG lBinBlkULX;
	LONG lBinBlkULY;
	LONG lBinBlkLRX;
	LONG lBinBlkLRY;

	SetJoystickOn(FALSE);   

   	lPhyBlk1ULX = m_oPhyBlkMain.GrabBlkUpperLeftX(1);
	lPhyBlk1ULY = m_oPhyBlkMain.GrabBlkUpperLeftY(1);
	lPhyBlk1LRX = m_oPhyBlkMain.GrabBlkLowerRightX(1);
	lPhyBlk1LRY = m_oPhyBlkMain.GrabBlkLowerRightY(1);

	lBinBlkULX = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
	lBinBlkULY = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);
	lBinBlkLRX = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightX);
	lBinBlkLRY = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightY);


	lIsValidLR = m_oBinBlkMain.ValidateAllSetupLR(lPhyBlk1ULX, lPhyBlk1ULY, lPhyBlk1LRX, lPhyBlk1LRY, 
										lBinBlkULX, lBinBlkULY, lBinBlkLRX, lBinBlkLRY);

	if (lIsValidLR == 1)
	{
		HmiMessage("Error: Lower Right is not on Physical Block 1!\nPlease re-enter");
	}
	else if (lIsValidLR == 2)
	{
		HmiMessage("Error: Lower Right is not at lower right corner!\nPlease re-enter");
	}
	
	return 1;
} //end ConfirmAllBinBlksSetupLR


LONG CBinTable::SubmitAllBinBlksSetup(IPC_CServiceMessage& svMsg)
{
	CString szTemp;
	LONG lBinUpperLeftX, lBinUpperLeftY, lBinLowerRightX, lBinLowerRightY;
	LONG lPhyUpperLeftX, lPhyUpperLeftY, lPhyLowerRightX, lPhyLowerRightY;
	LONG lBinBlkPitchX, lBinBlkPitchY;
	LONG lDiePitchX, lDiePitchY;
	ULONG ulTotalNoOfBinBlksCanHold;
	BOOL bIsValidUL;
	LONG lIsValidLR;

	lBinUpperLeftX = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftX);
	lBinUpperLeftY = 0 - ConvertDisplayUnitToFileUnit(m_lBinUpperLeftY);
	lBinLowerRightX = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightX);
	lBinLowerRightY = 0 - ConvertDisplayUnitToFileUnit(m_lBinLowerRightY);   

	lPhyUpperLeftX = m_oPhyBlkMain.GrabBlkUpperLeftX(1);
	lPhyUpperLeftY = m_oPhyBlkMain.GrabBlkUpperLeftY(1);
	lPhyLowerRightX = m_oPhyBlkMain.GrabBlkLowerRightX(1);
	lPhyLowerRightY = m_oPhyBlkMain.GrabBlkLowerRightY(1);


	lBinBlkPitchX = ConvertDisplayUnitToFileUnit(m_lBinBlkPitchX);
	lBinBlkPitchY = ConvertDisplayUnitToFileUnit(m_lBinBlkPitchY);

	lDiePitchX = ConvertDisplayUnitToFileUnit(m_lDiePitchX);
	lDiePitchY = ConvertDisplayUnitToFileUnit(m_lDiePitchY);



	bIsValidUL =  m_oBinBlkMain.ValidateAllSetupUL(lPhyUpperLeftX, lPhyUpperLeftY, 
					lPhyLowerRightX, lPhyLowerRightY, lBinUpperLeftX, lBinUpperLeftY);

	if (!bIsValidUL)
	{
		HmiMessage("Error - Upper Left is not on Physical Block 1!\nPlease re-enter");
	}
	else //valid UL
	{
		lIsValidLR = m_oBinBlkMain.ValidateAllSetupLR(lPhyUpperLeftX, lPhyUpperLeftY, 
			lPhyLowerRightX, lPhyLowerRightY, lBinUpperLeftX, lBinUpperLeftY, 
			lBinLowerRightX, lBinLowerRightY);

		//check if LR is valid
		if (lIsValidLR == 1)
		{
			HmiMessage("Error: Lower Right is not on Physical Block 1!\nPlease re-enter");
		}
		else if (lIsValidLR == 2)
		{
			HmiMessage("Error: Lower Right is not at lower right corner!\nPlease re-enter");
		}
	}	

	if (bIsValidUL && lIsValidLR == 3) //both UL and LR are valid
	{
		ulTotalNoOfBinBlksCanHold = m_oPhyBlkMain.CalculateTotalNoOfBinBlksCanHold(lBinUpperLeftX,
			lBinUpperLeftY, lBinLowerRightX, lBinLowerRightY, lBinBlkPitchX, lBinBlkPitchY);

		if (m_ulNoOfBinBlk > ulTotalNoOfBinBlksCanHold)
		{
			HmiMessage("Setup failed - Not Enough Room!");
			SetStatusMessage("Setup failed - Not Enough Room!");

			//m_bReply = FALSE; //commands below WILL be executed
			m_bBinBlkCreated = FALSE;
			
		}
		else
		{
			m_oBinBlkMain.SetupAllBlks(&m_oPhyBlkMain, m_ulNoOfBinBlk, 
						lBinUpperLeftX, lBinUpperLeftY, lBinLowerRightX, lBinLowerRightY, 
						lBinBlkPitchX, lBinBlkPitchY, lDiePitchX, lDiePitchY, 
						m_ulWalkPath, m_bAutoAssignGrade);

			m_ulDiePerBlk = m_oBinBlkMain.GrabNoOfDiePerBlk(1); //update screen display
			m_ulDiePerRow = m_oBinBlkMain.GrabNoOfDiePerRow(1); //update screen display

			//ULONG i;

			//to be done in CBinBlkMain
			//for (i = 1; i <= m_ulNoOfBinBlk; i++)
			//{
			//	m_oBinGradeData[i].SetBlkInUse(i);
			//	m_oBinGradeData[i].SetInputCount(m_ulDiePerBlk);
			//}

			//m_bReply = TRUE; //commands below WILL be executed

			m_bBinBlkCreated = TRUE;
			

			HmiMessage("Setup Complete!");
			SetStatusMessage("Setup Complete!");
		}
	}
	//svMsg.InitMessage(sizeof(BOOL), &m_bReply);

	return 1;
} //end SubmitAllBinBlksSetup


LONG CBinTable::CancelAllBinBlksSetup(IPC_CServiceMessage& svMsg)
{
	return 1;
} //end CancelAllBinBlksSetup