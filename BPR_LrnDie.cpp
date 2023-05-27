/////////////////////////////////////////////////////////////////
// BPR_LrnDie.cpp : Learn Die Function of the CBondPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wednesday, December 1, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondPr.h"
#include "BinTable.h"
#include "BondHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CBondPr::IsBinFrameRotated180()
{
	CBinTable *pBinTable = dynamic_cast<CBinTable*>(GetStation(BIN_TABLE_STN));
	if (pBinTable->IsBinFrameRotated180())
	{
		return TRUE;
	}

	return FALSE;
}

PR_UWORD CBondPr::AutoManualLearnDie(BOOL bDieType, LONG lInputDieNo)
{
	PR_MANUAL_LRN_DIE_CMD		stLrnDieCmd;
	PR_MANUAL_LRN_DIE_RPY1		stLrnDieRpy1;
	PR_MANUAL_LRN_DIE_RPY2		stLrnDieRpy2;
	PR_DEFECT_WIN				stInspDefectWin;
//	PR_DEFECT_WIN				stAlignDefectWin;
	PR_UWORD					usAlignRes;
	PR_UWORD					usInspAlg;
	PR_UWORD 					lLearnDieStauts;				
	LONG						lDieNo = 0;

	//Update Others die corner
	if (GetDieShape() == BPR_RECTANGLE_DIE)
	{
		m_stLearnDieCornerPos[PR_UPPER_RIGHT].x	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].x; 
		m_stLearnDieCornerPos[PR_UPPER_RIGHT].y	= m_stLearnDieCornerPos[PR_UPPER_LEFT].y; 
		m_stLearnDieCornerPos[PR_LOWER_LEFT].x	= m_stLearnDieCornerPos[PR_UPPER_LEFT].x; 
		m_stLearnDieCornerPos[PR_LOWER_LEFT].y	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].y; 
		CString szTemp;
		szTemp.Format(" (%ld, %ld, %ld, %ld)",
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x,		m_stLearnDieCornerPos[PR_UPPER_LEFT].y,
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x,	m_stLearnDieCornerPos[PR_LOWER_RIGHT].y);
		SetErrorMessage("BPR die size auto learn die, confirm die corner." + szTemp);	//xxxxxx
	}

	//Update Inspection detect / ignore PR structure
	stInspDefectWin.uwNDetectWin = 0;
	stInspDefectWin.uwNIgnoreWin = 0;
	if (m_lInspDetectWinNo > 0)
	{
		stInspDefectWin.uwNDetectWin = (PR_UWORD)m_lInspDetectWinNo;
		for (short i = 0; i < m_lInspDetectWinNo; i++)
		{
			stInspDefectWin.astDetectWin[i] = m_stInspDetectWin[i];
		}
	}
	m_lInspDetectWinNo	= 0;   //Ignore inspection windows

	if (m_lInspIgnoreWinNo > 0)
	{
		stInspDefectWin.uwNIgnoreWin = (PR_UWORD)m_lInspIgnoreWinNo;
		for (short i = 0; i < m_lInspIgnoreWinNo; i++)
		{
			stInspDefectWin.astIgnoreWin[i] = m_stInspIgnoreWin[i];
		}
	}
	m_lInspIgnoreWinNo	= 0;
/*
	//Update Alignment detect / ignore PR structure
	if (m_lAlnDetectWinNo == 0)
	{
		stAlignDefectWin.uwNDetectWin = 0;
	}
	else
	{
		stAlignDefectWin.uwNDetectWin = (PR_UWORD)m_lAlnDetectWinNo;
		for (i=0; i<m_lAlnDetectWinNo; i++)
		{
			stAlignDefectWin.astDetectWin[i] = m_stAlnDetectWin[i];
		}
	}

	if (m_lAlnIgnoreWinNo == 0)
	{
		stAlignDefectWin.uwNIgnoreWin = 0;
	}
	else
	{
		stAlignDefectWin.uwNIgnoreWin = (PR_UWORD)m_lAlnIgnoreWinNo;
		for (i=0; i<m_lAlnIgnoreWinNo; i++)
		{
			stAlignDefectWin.astIgnoreWin[i] = m_stAlnIgnoreWin[i];
		}
	}
*/

	if (lInputDieNo <= 0)
	{
		lInputDieNo = 1;
	}

	if (bDieType == BPR_NORMAL_DIE)
	{
		//Normal Die
		if (lInputDieNo == 1)
		{
			//Save the zoom factor of normal die
			BPR_SetNmlZoomFactor(BPR_GetRunZoom());
		}
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	}
	else
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	}

/*
	//Set Zoom Factor
	if (bDieType == BPR_NORMAL_DIE))
	{
		if (lDieNo == 0)
		{
			SetNormalZoomFactor(GetRunZoom());
		}
		 
		if (lDieNo == BPR_FF_MODE_DIE)
		{
			SetPrescanZoomFactor(GetRunZoom());
			GetScanFovWindow();	//	when learn prescan die PR record.
		}
	}
*/
	switch (m_lGenLrnInspMethod[lDieNo])
	{
		case 1:  
			usInspAlg = PR_DIE_INSP_ALG_BIN;
			break;

		default: 
			usInspAlg = PR_GOLDEN_DIE_TMPL;
			break;
	}

	//v4.08
	switch (m_lGenLrnFineSrch[lDieNo])
	{
	case 0:
		usAlignRes = PR_RESOLUTION_LOW;
		break;
	case 2:
		usAlignRes = PR_RESOLUTION_HIGH;
		break;
	case 1:
	default:
		usAlignRes = PR_RESOLUTION_MEDIUM;
		break;
	}

	LONG lObjType = PR_OBJ_TYPE_LED_DIE;
	if ((bDieType == BPR_NORMAL_DIE) && (lInputDieNo == BPR_NORMAL_PR_FF_MODE_INDEX3))
	{
		lObjType = PR_OBJ_TYPE_FAST_PRESCAN_LED_DIE;
	}

	//Free current PR record 
	PR_FREE_RECORD_ID_CMD		stFreeCmd;
	PR_FREE_RECORD_ID_RPY		stFreeCmdRpy;
	PR_InitFreeRecordIDCmd(&stFreeCmd);
	stFreeCmd.uwRecordID = (PR_UWORD)(m_ssGenPRSrchID[lDieNo]);
	PR_FreeRecordIDCmd(&stFreeCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stFreeCmdRpy);
	m_ssGenPRSrchID[lDieNo] = 0;


	PR_InitManualLrnDieCmd(&stLrnDieCmd);
	stLrnDieCmd.emCameraNo 						= MS899_BOND_CAM_ID;
	stLrnDieCmd.emObjectType					= (PR_OBJ_TYPE)lObjType;
	stLrnDieCmd.aemPurpose[0]					= MS899_BOND_PB_PURPOSE;


	//v4.57A13
	BOOL bBTPreBondAlign = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];		//v4.57A13
	if ( bBTPreBondAlign && 
		 (bDieType == BPR_REFERENCE_DIE) && 
		 (lInputDieNo == 1) )	//REF Die pattern #1
	{
		stLrnDieCmd.emObjectType	= PR_OBJ_TYPE_TEXTURAL_OBJ;		//PAD pattern
		CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Learn Ref Die #1 uses PAD OBJ type");		
		//HmiMessage("BPR REF Die pattern using TEXTURAL OBJ type ...");
	}

	if ( GetDieShape() == BPR_RECTANGLE_DIE )
	{
		stLrnDieCmd.emIsRectObj						= PR_TRUE;
		stLrnDieCmd.uwNumOfDieCorners				= 4;
		stLrnDieCmd.acoDieCorners[PR_UPPER_RIGHT]	= m_stLearnDieCornerPos[PR_UPPER_RIGHT];
		stLrnDieCmd.acoDieCorners[PR_UPPER_LEFT]	= m_stLearnDieCornerPos[PR_UPPER_LEFT];
		stLrnDieCmd.acoDieCorners[PR_LOWER_LEFT]	= m_stLearnDieCornerPos[PR_LOWER_LEFT];
		stLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT]	= m_stLearnDieCornerPos[PR_LOWER_RIGHT];
	}
	else if ( GetDieShape() == BPR_TRIANGULAR_DIE )		//v4.06
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= BPR_TRIANGLE_CORNERS;
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
	}
	else if ( GetDieShape() == BPR_RHOMBUS_DIE )
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= BPR_RHOMBUS_CORNERS;
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_3]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_3];
	}
	else
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= BPR_HEXAGON_CORNERS;
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_3]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_3];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_4]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_4];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_5]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_5];
	}


	stLrnDieCmd.emBackgroundInfo				= GetLrnBackGround(lDieNo);
	stLrnDieCmd.stDefectWin						= stInspDefectWin;		//Inspection ignore/detect region
	//stLrnDieCmd.stAlign.stStreet.stDetectIgnoreWin	= stAlignDefectWin;		//Alignment ignore/detect region -- new
	stLrnDieCmd.emPreRot						= PR_TRUE;
	stLrnDieCmd.emIsGrabFromPurpose				= PR_TRUE;
	stLrnDieCmd.emFineSrchResolution			= (PR_RESOLUTION)usAlignRes;
	stLrnDieCmd.emAlignAlg						= GetLrnAlignAlgo(lDieNo);
	stLrnDieCmd.emInspAlg						= (PR_DIE_INSP_ALG)(usInspAlg);

	//v3.25T3
	if (m_bEnableLineDefect)
		stLrnDieCmd.emInspResolution			= PR_RESOLUTION_HIGH;	//(PR_RESOLUTION)(m_lLrnInspRes+1);
		//stLrnDieCmd.emInspResolution					= (PR_RESOLUTION)(m_lGenLrnInspRes[lDieNo]);		//v3.17T1

	if (IsSensorZoomFFMode())
	{
		stLrnDieCmd.emCoarseSrchResolution = PR_RESOLUTION_MEDIUM;	//	PR_RESOLUTION_HIGH;
		CString szMsg;
		szMsg.Format("WPR auto learn die, set emCoarseSrchResolution medium(2)high(3) to %d",
			stLrnDieCmd.emCoarseSrchResolution);
		SetAlarmLog(szMsg);
	}

	if (IsBinFrameRotated180())
	{
		stLrnDieCmd.rExpectedAngle = 180;
	}
	else
	{
		stLrnDieCmd.rExpectedAngle = 0;
	}

	//Start AutoLearn Die & Get Reply 1
	PR_ManualLrnDieCmd(&stLrnDieCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stLrnDieRpy1);

	if ((stLrnDieRpy1.uwCommunStatus == PR_COMM_NOERR) && (stLrnDieRpy1.uwPRStatus == PR_ERR_NOERR))
	{
		//Get Replay 2	
		PR_ManualLrnDieRpy(MS899_BOND_CAM_SEND_ID, &stLrnDieRpy2);

		if (stLrnDieRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR)
		{
			if ((stLrnDieRpy2.stStatus.uwPRStatus == PR_ERR_NOERR) || PR_WARN_STATUS(stLrnDieRpy2.stStatus.uwPRStatus))
			{
				//Get PR ID & Die size
				//m_stDieSize.x	= (stLrnDieRpy2.acoDieCorners[PR_LOWER_RIGHT].x - stLrnDieRpy2.acoDieCorners[PR_UPPER_LEFT].x);
				//m_stDieSize.y	= (stLrnDieRpy2.acoDieCorners[PR_LOWER_RIGHT].y - stLrnDieRpy2.acoDieCorners[PR_UPPER_LEFT].y);
				short siMinX = (short)m_lPrCenterX, siMaxX = (short)m_lPrCenterX;
				short siMinY = (short)m_lPrCenterY, siMaxY = (short)m_lPrCenterY;
				if (IsSensorZoomFFMode())
				{
					siMinX = 8192;
					siMaxX = 0;

					siMinY = 8192;
					siMaxY = 0;
				}

				for (short i=0; i<(short)stLrnDieRpy2.uwNumOfDieCorners; i++ )
				{
					siMinX = min(siMinX, stLrnDieRpy2.acoDieCorners[i].x);
					siMaxX = max(siMaxX, stLrnDieRpy2.acoDieCorners[i].x);

					siMinY = min(siMinY, stLrnDieRpy2.acoDieCorners[i].y);
					siMaxY = max(siMaxY, stLrnDieRpy2.acoDieCorners[i].y);
				}

				m_stGenDieSize[lDieNo].x = siMaxX - siMinX;
				m_stGenDieSize[lDieNo].y = siMaxY - siMinY;
				m_ssGenPRSrchID[lDieNo]	= stLrnDieRpy2.uwRecordID;
				m_bGenDieLearnt[lDieNo]	= TRUE;

				//CString szTemp;
				//szTemp.Format("ID: % d %d",lDieNo, m_ssGenPRSrchID[lDieNo]	);
				//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

				switch(stLrnDieRpy2.stStatus.uwPRStatus)
				{
					case PR_WARN_TOO_DARK:
					case PR_WARN_TOO_BRIGHT:
					case PR_WARN_ROTATION_INVARIANT:
						lLearnDieStauts = stLrnDieRpy2.stStatus.uwPRStatus;
						break;
					default:
						lLearnDieStauts = PR_ERR_NOERR;
						break;
				}
			}
			else
			{
				m_bGenDieLearnt[lDieNo]	= FALSE;
				lLearnDieStauts = stLrnDieRpy2.stStatus.uwPRStatus;
			}
		}
		else
		{
			m_bGenDieLearnt[lDieNo]	= FALSE;
			lLearnDieStauts = PR_COMM_TIMEOUT;
		}
	}
	else
	{
		m_bGenDieLearnt[lDieNo]	= FALSE;
		lLearnDieStauts = PR_COMM_ERR;
	}

	if (IsSensorZoomFFMode())
	{
		for (LONG i = 0; i < BPR_MAX_DIE_CORNER; i++)
		{
			m_stLearnFFModeDieCornerPos[i] = m_stLearnDieCornerPos[i];
		}
	}
	else
	{
		for (LONG i = 0; i < BPR_MAX_DIE_CORNER; i++)
		{
			m_stLearnNormalDieCornerPos[i] = m_stLearnDieCornerPos[i];
		}
	}

	return lLearnDieStauts;
}



PR_UWORD CBondPr::AutoLearnDie(BOOL bDieType, LONG lInputDieNo)
{
	LONG lDieNo;
	PR_FREE_RECORD_ID_CMD		stFreeCmd;
	PR_FREE_RECORD_ID_RPY		stFreeCmdRpy;
	PR_AUTO_LRN_DIE_CMD			stLrnDieCmd;
	PR_AUTO_LRN_DIE_RPY1		stLrnDieRpy1;
	PR_AUTO_LRN_DIE_RPY2		stLrnDieRpy2;
	PR_DEFECT_WIN				stDefectWin;
	PR_UWORD					usAlignRes;
	PR_UWORD					usInspAlg;
	PR_UWORD 					lLearnDieStauts;				

	//Update Others die corner
	if ( GetDieShape() == BPR_RECTANGLE_DIE )
	{
		m_stLearnDieCornerPos[PR_UPPER_RIGHT].x	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].x; 
		m_stLearnDieCornerPos[PR_UPPER_RIGHT].y	= m_stLearnDieCornerPos[PR_UPPER_LEFT].y; 
		m_stLearnDieCornerPos[PR_LOWER_LEFT].x	= m_stLearnDieCornerPos[PR_UPPER_LEFT].x; 
		m_stLearnDieCornerPos[PR_LOWER_LEFT].y	= m_stLearnDieCornerPos[PR_LOWER_RIGHT].y; 
		CString szTemp;
		szTemp.Format(" (%ld, %ld, %ld, %ld)",
			m_stLearnDieCornerPos[PR_UPPER_LEFT].x,		m_stLearnDieCornerPos[PR_UPPER_LEFT].y,
			m_stLearnDieCornerPos[PR_LOWER_RIGHT].x,	m_stLearnDieCornerPos[PR_LOWER_RIGHT].y);
		SetErrorMessage("BPR die size auto learn die, confirm die corner." + szTemp);	//xxxxxx
	}

	//Update PR structure
	stDefectWin.uwNDetectWin = 0;
	stDefectWin.uwNIgnoreWin = 0;
	short						i;
	if (m_lInspDetectWinNo > 0)
	{
		stDefectWin.uwNDetectWin = (PR_UWORD)m_lInspDetectWinNo;
		for (i=0; i<m_lInspDetectWinNo; i++)
		{
			stDefectWin.astDetectWin[i] = m_stInspDetectWin[i];
		}
	}
	m_lInspDetectWinNo	= 0;
	if (m_lInspIgnoreWinNo > 0)
	{
		stDefectWin.uwNIgnoreWin = (PR_UWORD)m_lInspIgnoreWinNo;
		for (i=0; i<m_lInspIgnoreWinNo; i++)
		{
			stDefectWin.astIgnoreWin[i] = m_stInspIgnoreWin[i];
		}
	}
	m_lInspIgnoreWinNo	= 0;

	if (lInputDieNo <= 0)
	{
		lInputDieNo = 1;
	}

	if (bDieType == BPR_NORMAL_DIE)
	{
		//Normal Die
		if (lInputDieNo == 1)
		{
			//Save the zoom factor of normal die
			BPR_SetNmlZoomFactor(BPR_GetRunZoom());
		}
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	}
	else
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	}

	switch (m_lGenLrnInspMethod[lDieNo])
	{
		case 1:  
			usInspAlg = PR_DIE_INSP_ALG_BIN;
			break;

		default: 
			usInspAlg = PR_GOLDEN_DIE_TMPL;
			break;
	}

	//v4.08
	switch (m_lGenLrnFineSrch[lDieNo])
	{
	case 0:
		usAlignRes = PR_RESOLUTION_LOW;
		break;
	case 2:
		usAlignRes = PR_RESOLUTION_HIGH;
		break;
	case 1:
	default:
		usAlignRes = PR_RESOLUTION_MEDIUM;
		break;
	}

	LONG lObjType = PR_OBJ_TYPE_LED_DIE;
	if ((bDieType == BPR_NORMAL_DIE) && (lInputDieNo == 2))
	{
		lObjType = PR_OBJ_TYPE_FAST_PRESCAN_LED_DIE;
	}
	//Free current PR record 
	PR_InitFreeRecordIDCmd(&stFreeCmd);
	stFreeCmd.uwRecordID = (PR_UWORD)(m_ssGenPRSrchID[lDieNo]);
	PR_FreeRecordIDCmd(&stFreeCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stFreeCmdRpy);
	m_ssGenPRSrchID[lDieNo] = 0;


	PR_InitAutoLrnDieCmd(&stLrnDieCmd);
	stLrnDieCmd.emCameraNo 						= MS899_BOND_CAM_ID;
	stLrnDieCmd.emObjectType					= (PR_OBJ_TYPE)lObjType;
	stLrnDieCmd.aemPurpose[0]					= MS899_BOND_PB_PURPOSE;
	
	//v4.57A13
	BOOL bBTPreBondAlign = (BOOL)(LONG)(*m_psmfSRam)["BinTable"]["PreBondAlignment"];		//v4.57A13
	if ( bBTPreBondAlign && 
		 (bDieType == BPR_REFERENCE_DIE) && 
		 (lInputDieNo == 1) )	//REF Die pattern #1
	{
		stLrnDieCmd.emObjectType	= PR_OBJ_TYPE_TEXTURAL_OBJ;		//PAD pattern
		CMSLogFileUtility::Instance()->MS_LogOperation("BPR: Learn Ref Die #1 uses PAD OBJ type");		
		//HmiMessage("BPR REF Die pattern using TEXTURAL OBJ type ...");
	}

	if ( GetDieShape() == BPR_RECTANGLE_DIE )
	{
		stLrnDieCmd.emIsRectObj						= PR_TRUE;
		stLrnDieCmd.uwNumOfDieCorners				= 4;
		stLrnDieCmd.acoDieCorners[PR_UPPER_RIGHT]	= m_stLearnDieCornerPos[PR_UPPER_RIGHT];
		stLrnDieCmd.acoDieCorners[PR_UPPER_LEFT]	= m_stLearnDieCornerPos[PR_UPPER_LEFT];
		stLrnDieCmd.acoDieCorners[PR_LOWER_LEFT]	= m_stLearnDieCornerPos[PR_LOWER_LEFT];
		stLrnDieCmd.acoDieCorners[PR_LOWER_RIGHT]	= m_stLearnDieCornerPos[PR_LOWER_RIGHT];
	}
	else if ( GetDieShape() == BPR_TRIANGULAR_DIE )		//v4.06
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= BPR_TRIANGLE_CORNERS;
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
	}
	else if ( GetDieShape() == BPR_RHOMBUS_DIE )
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= BPR_RHOMBUS_CORNERS;
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_3]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_3];
	}
	else
	{
		stLrnDieCmd.emIsRectObj						= PR_FALSE;
		stLrnDieCmd.uwNumOfDieCorners				= BPR_HEXAGON_CORNERS;
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_0]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_0];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_1]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_1];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_2]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_2];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_3]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_3];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_4]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_4];
		stLrnDieCmd.acoDieCorners[BPR_DIE_CORNER_5]	= m_stLearnDieCornerPos[BPR_DIE_CORNER_5];
	}

	stLrnDieCmd.emBackgroundInfo				= GetLrnBackGround(lDieNo);
	stLrnDieCmd.stDefectWin						= stDefectWin;
	stLrnDieCmd.emPreRot						= PR_TRUE;
	stLrnDieCmd.emIsGrabFromPurpose				= PR_TRUE;
	stLrnDieCmd.emFineSrchResolution			= (PR_RESOLUTION)usAlignRes;
	stLrnDieCmd.emAlignAlg						= GetLrnAlignAlgo(lDieNo);
	stLrnDieCmd.emInspAlg						= (PR_DIE_INSP_ALG)(usInspAlg);

	//v3.25T3
	if (m_bEnableLineDefect)
		stLrnDieCmd.emInspResolution			= PR_RESOLUTION_HIGH;	//(PR_RESOLUTION)(m_lLrnInspRes+1);


	//Start AutoLearn Die & Get Reply 1
	PR_AutoLrnDieCmd(&stLrnDieCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &stLrnDieRpy1);

	if ((stLrnDieRpy1.uwCommunStatus == PR_COMM_NOERR) && (stLrnDieRpy1.uwPRStatus == PR_ERR_NOERR))
	{
		//Get Replay 2	
		PR_AutoLrnDieRpy(MS899_BOND_CAM_SEND_ID, &stLrnDieRpy2);

		if (stLrnDieRpy2.stStatus.uwCommunStatus == PR_COMM_NOERR)
		{
			if ((stLrnDieRpy2.stStatus.uwPRStatus == PR_ERR_NOERR) || PR_WARN_STATUS(stLrnDieRpy2.stStatus.uwPRStatus))
			{
				//Get PR ID & Die size
				//m_stDieSize.x	= (stLrnDieRpy2.acoDieCorners[PR_LOWER_RIGHT].x - stLrnDieRpy2.acoDieCorners[PR_UPPER_LEFT].x);
				//m_stDieSize.y	= (stLrnDieRpy2.acoDieCorners[PR_LOWER_RIGHT].y - stLrnDieRpy2.acoDieCorners[PR_UPPER_LEFT].y);
				short siMinX = (short)m_lPrCenterX, siMaxX = (short)m_lPrCenterX;
				short siMinY = (short)m_lPrCenterY, siMaxY = (short)m_lPrCenterY;

				for (short i=0; i<(short)stLrnDieRpy2.uwNumOfDieCorners; i++ )
				{
					siMinX = min(siMinX, stLrnDieRpy2.acoDieCorners[i].x);
					siMaxX = max(siMaxX, stLrnDieRpy2.acoDieCorners[i].x);

					siMinY = min(siMinY, stLrnDieRpy2.acoDieCorners[i].y);
					siMaxY = max(siMaxY, stLrnDieRpy2.acoDieCorners[i].y);
				}

				m_stGenDieSize[lDieNo].x = siMaxX - siMinX;
				m_stGenDieSize[lDieNo].y = siMaxY - siMinY;
				m_ssGenPRSrchID[lDieNo]	= stLrnDieRpy2.uwRecordID;
				m_bGenDieLearnt[lDieNo]	= TRUE;

				//CString szTemp;
				//szTemp.Format("ID: % d %d",lDieNo, m_ssGenPRSrchID[lDieNo]	);
				//AfxMessageBox(szTemp, MB_SYSTEMMODAL);

				switch(stLrnDieRpy2.stStatus.uwPRStatus)
				{
					case PR_WARN_TOO_DARK:
					case PR_WARN_TOO_BRIGHT:
					case PR_WARN_ROTATION_INVARIANT:
						lLearnDieStauts = stLrnDieRpy2.stStatus.uwPRStatus;
						break;
					default:
						lLearnDieStauts = PR_ERR_NOERR;
						break;
				}
			}
			else
			{
				m_bGenDieLearnt[lDieNo]	= FALSE;
				lLearnDieStauts = stLrnDieRpy2.stStatus.uwPRStatus;
			}
		}
		else
		{
			m_bGenDieLearnt[lDieNo]	= FALSE;
			lLearnDieStauts = PR_COMM_TIMEOUT;
		}
	}
	else
	{
		m_bGenDieLearnt[lDieNo]	= FALSE;
		lLearnDieStauts = PR_COMM_ERR;
	}

	return lLearnDieStauts;
}


VOID CBondPr::DisplayLearnDieResult(PR_UWORD lLearnStatus)
{
	CString		csMsgA;
	CString		csMsgB;
	ULONG		ulMsgCode;

	switch(lLearnStatus)
	{
		case PR_WARN_TOO_DARK:
			//csMsgA.Format("Die Too Dark");
			//csMsgB.Format("Learn Die Warning");
			ulMsgCode = IDS_BPR_DIETOODARK;
			break;

		case PR_WARN_TOO_BRIGHT:
			//csMsgA.Format("Die Too Bright");
			//csMsgB.Format("Learn Die Warning");
			ulMsgCode = IDS_BPR_DIETOOBRIGHT;
			break;

		case PR_WARN_ROTATION_INVARIANT:
			//csMsgA.Format("Die Rotation Invariant");
			//csMsgB.Format("Learn Die Warning");
			ulMsgCode = IDS_BPR_DIEROTATIONINVARIANT;
			break;

		case PR_ERR_NOERR:
			csMsgA.LoadString(HMB_BPR_LRN_DIE_OK);
			csMsgB.LoadString(HMB_BPR_LRN_DIE);
			HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			return;
//			ulMsgCode = IDS_BPR_LEARNDIEOK;
//			break;

		case PR_COMM_TIMEOUT:
			//csMsgA.Format("Get Die Reply 2 Error");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_BPR_GETDIEREPLY2ERROR;
			break;

		case PR_COMM_ERR:
			//csMsgA.Format("Get Die Reply 1 Error");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_BPR_GETDIEREPLY1ERROR;
			break;

		case PR_ERR_ROTATION:
			//csMsgA.Format("Die Angle too large");
			//csMsgB.Format("Learn Die Error");
			ulMsgCode = IDS_BPR_DIEANGLETOOLARGE;
			break;

		default:
			//csMsgA.Format("Unknown Die Error = %x",lLearnStatus);
			//csMsgB.Format("Learn Die Error");
			//HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_CENTER, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
			csMsgA.Format("Status = %x",lLearnStatus);
			SetAlert_Msg(IDS_BPR_LRN_DIE_FAILED, csMsgA); 
			return;
	}

	//HmiMessage(csMsgA,csMsgB,2,1);
	//HmiMessage(csMsgA,csMsgB, glHMI_MBX_OK, glHMI_ALIGN_LEFT, 36000000, glHMI_MSG_MODAL, NULL, 350, 200, NULL, NULL, NULL, NULL);
	SetAlert(ulMsgCode);

}

PR_DIE_ALIGN_ALG CBondPr::GetLrnAlignAlgo(LONG lDieNo)
{
	PR_DIE_ALIGN_ALG usAlignAlg;

	//Klocwork	//v4.02T5
	lDieNo = min(lDieNo, BPR_MAX_DIE-1);
	lDieNo = max(lDieNo, 0);

	switch (m_lGenLrnAlignAlgo[lDieNo])
	{
	case 1:		//Pattern matching	
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_PATTERN;
		break;
	case 2:		//Edge matching	 
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES_WO_PATTERN_GUIDED;
		break;
	case 3:		//BOTH matching	 
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_BOTH;		//v3.76
		break;
	case 4:		// Edge Synthetic Matching
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_SYNTHETIC_DIE_EDGE;
		break;
	default:	//Pattern Guided
		usAlignAlg = PR_EDGE_POINTS_MATCHING_ON_DIE_EDGES;
		break;
	}

	return usAlignAlg;
}

PR_BACKGROUND_INFO CBondPr::GetLrnBackGround(LONG lDieNo)
{
	PR_BACKGROUND_INFO emBGInfo;

	//Klocwork	//v4.02T5
	lDieNo = min(lDieNo, BPR_MAX_DIE-1);
	lDieNo = max(lDieNo, 0);

	switch( m_lGenLrnBackgroud[lDieNo] )
	{
	case 1:
		emBGInfo = PR_BACKGROUND_INFO_BRIGHT;
		break;
	case 2:
		emBGInfo = PR_BACKGROUND_INFO_AUTO;
		break;
	default:
		emBGInfo = PR_BACKGROUND_INFO_DARK;
		break;
	}

	return emBGInfo;
}

PR_UWORD CBondPr::AutoLearnCircle(PR_COORD stULCorner, PR_COORD stLRCorner)
{
	PR_LRN_SHAPE_CMD pstCmd;
	PR_LRN_SHAPE_RPY1 pstRpy1;
	PR_LRN_SHAPE_RPY2 pstRpy2;

	PR_InitLrnShapeCmd(&pstCmd);
	pstCmd.emSameView = PR_FALSE;							// Use previous grab image
	pstCmd.emPurpose = MS899_BOND_PB_PURPOSE;				// PR Logical Optics
//	pstCmd.coRefPoint;										// Reference Point
//	pstCmd.rRefAngle;										// Reference Angle
	pstCmd.emIsAligned = PR_FALSE;							// Is the object aligned?
	pstCmd.emShapeType = PR_SHAPE_TYPE_CIRCLE;				// Shape type (e.g: Circle, Rectangle ...etc)
	pstCmd.stEncRect.coCorner1 = stULCorner;				// Enclosing/Inscribing Rectangle (Used for Circle Extraction only)
	pstCmd.stEncRect.coCorner2 = stLRCorner;				// Enclosing/Inscribing Rectangle (Used for Circle Extraction only)
	pstCmd.emRectMode = PR_RECT_MODE_ENCLOSE;				// Is the Rectangle Enclosing or Inscribing?
	pstCmd.emDetectAlg = PR_SHAPE_DETECT_ALG_EDGE_PTS;		// Detection Algorithm
//	pstCmd.emAccuracy;			// Accuracy
	pstCmd.emObjAttribute = PR_OBJ_ATTRIBUTE_BRIGHT;		// Object Attribute (Bright\Dark\Both)
//	pstCmd.ubThreshold;			// Threshold
//	pstCmd.wEdgeMag;			// Edge Magnitude
//	pstCmd.uwRoughness;			// Roughness of the shape
	pstCmd.emIsSaveRecord = PR_TRUE;		// Save Record (Yes: Lrn & Srch/ No: Extract once only)
	pstCmd.uwMaskID = 0;					// Which Mask will be used? (0: Not use any Mask)
	pstCmd.rPassScore = 70;					// Pass Score for extraction
	pstCmd.uwNumOfLine;			// No of Line of the shape (Used for Rect, Square, Right Angle Corner & Line)
//	pstCmd.astLineCoord[PR_MAX_NO_OF_SHAPE_LINE];	// Line coord.
	pstCmd.emCentreDefinition = PR_CENTRE_DEFINITION_BY_CORNER;	// Definition of Centre of the shape (By Corner or By edge point)
//	pstCmd.uwNConfirmWin;
//	pstCmd.astConfirmWin[PR_MAX_NO_OF_SHA_WIN];	// SHA Windows (Confirm/Pos Emp/Neg Emp/Blob)
	pstCmd.emExtMethod = PR_EXT_CORNER;			// Extraction Method (Line/Corner Extraction)
	pstCmd.uwNIgnoreWin = 0;		// no. of ignore windows 
//	pstCmd.astIgnoreWin[PR_MAX_DETECT_IGNORE_WIN];
	pstCmd.emObjectType = PR_OBJ_TYPE_LED_PACKAGE;

	/////////
	PR_LrnShapeCmd(&pstCmd, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, &pstRpy1);
	if ( (pstRpy1.uwCommunStatus != PR_COMM_NOERR) || (pstRpy1.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("BPR PR_LrnShapeCmd Error1: COMM = 0x%x, PR = 0x%x", pstRpy1.uwCommunStatus, pstRpy1.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}
    PR_LrnShapeRpy(MS899_BOND_CAM_SEND_ID, &pstRpy2);
	
	if ( (pstRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR) || (pstRpy2.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		CString szLog;
		szLog.Format("BPR PR_LrnShapeCmd Error2: COMM = 0x%x, PR = 0x%x", pstRpy2.stStatus.uwCommunStatus, pstRpy2.stStatus.uwPRStatus);
		HmiMessageEx(szLog);
		return PR_FALSE;
	}

	if (pstRpy2.emShapeFound != PR_SHAPE_FOUND_CIRCLE)
	{
		HmiMessageEx("BPR ERROR: Shape found is NOT circle!!!");
		return PR_FALSE;
	}

	m_ssGenPRSrchID[BPR_GEN_RDIE_OFFSET+3]          = pstRpy2.uwRecordID;
	m_bGenDieLearnt[BPR_GEN_RDIE_OFFSET+3]          = TRUE;

	m_pPrGeneral->DrawSmallCursor(MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, pstRpy2.stCircleRes.coCentre.x, pstRpy2.stCircleRes.coCentre.y);

	HmiMessageEx("Learn collet hole success !!!!!!!!");
	//Get PR ID & Die size
	short siMinX = (short)m_lPrCenterX, siMaxX = (short)m_lPrCenterX;
	short siMinY = (short)m_lPrCenterY, siMaxY = (short)m_lPrCenterY;
	siMinX = (short)_round(siMinX - pstRpy2.stCircleRes.rRadius);
	siMaxX = (short)_round(siMaxX + pstRpy2.stCircleRes.rRadius);
	siMinY = (short)_round(siMinY - pstRpy2.stCircleRes.rRadius);
	siMaxY = (short)_round(siMinY + pstRpy2.stCircleRes.rRadius);
	m_stGenDieSize[BPR_GEN_RDIE_OFFSET+3].x         = siMaxX - siMinX;
	m_stGenDieSize[BPR_GEN_RDIE_OFFSET+3].y         = siMaxY - siMinY;

	return PR_TRUE;
}