/////////////////////////////////////////////////////////////////
// WPR_SrchDie_HWTri.cpp : Search Die Function of the CWaferPr class
//
//	Description:
//		NVC
//
//	Date:		Tuesday, April 7, 2020
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Assembly Automation Ltd., 2020.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "TakeTime.h"
#include "BondResult.h"
#include "MS896A.h"
#include "PrescanUtility.h"
#include "PrescanInfo.h"
#include "GallerySearchController.h"
#include "WaferPr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
LONG CWaferPr::UserSearchDie_HWTri(IPC_CServiceMessage &svMsg)
{
	PR_UWORD		usDieType = PR_ERR_NO_OBJECT_FOUND;		//v4.00;
	PR_COORD		stDieOffset; 
	PR_REAL			fDieRotate; 
	PR_REAL			fDieScore;
	BOOL			bReturn = TRUE;
	BOOL			bNoDieFound = TRUE;

	if (PR_NotInit())
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (IsNormalDieLearnt() == FALSE)		//v4.48A8
	{
		SetAlert(IDS_WPR_DIENOTLEARNT);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

	if (m_bDieCalibrated == FALSE)			//v4.48A8
	{
		SetAlert(IDS_WPR_DIENOTCALIBRATED);
		svMsg.InitMessage(sizeof(BOOL), &bReturn);
		return 1;
	}

// prescan relative code	B
	if (IsPrescanning())
	{
		m_pPrescanPrCtrl->ResumePR();
		GetImageNumInGallery();
	}	// Exit if there are still image in gallery
// prescan relative code	E

	//Clear PR screen & Off joystick
	ChangeCamera(WPR_CAM_WAFER);
	SetWaferTableJoystick(FALSE);

	LONG lDieNo = WPR_NORMAL_DIE;

	//Change the zoom mode before searching die
	LONG lOldZoom = GetRunZoom();
	ChangePrRecordZoomMode(lDieNo);

	UpdateSearchDieArea(lDieNo, GetSrchDieAreaX(), GetSrchDieAreaY(), TRUE, FALSE);


	DrawSearchBox( PR_COLOR_GREEN);
	PR_COORD stCorner1, stCorner2;
	stCorner1.x = (PR_WORD) GetPrCenterX()-GetNmlSizePixelX()/2;
	stCorner1.y = (PR_WORD) GetPrCenterY()-GetNmlSizePixelY()/2;
	stCorner2.x = (PR_WORD) GetPrCenterX()+GetNmlSizePixelX()/2;
	stCorner2.y = (PR_WORD) GetPrCenterY()+GetNmlSizePixelY()/2;
	DrawRectangleBox(stCorner1, stCorner2, PR_COLOR_GREEN);
	PR_WORD rtnPR;
	rtnPR = ManualSearchDie_HWTri(WPR_NORMAL_DIE, lDieNo, PR_TRUE, PR_TRUE, PR_TRUE, &usDieType, &fDieRotate, &stDieOffset, &fDieScore, GetSrchArea().coCorner1, GetSrchArea().coCorner2,
								TRUE);	
	if (rtnPR != -1)
	{
		if (DieIsAlignable(usDieType) == TRUE)
		{
			bNoDieFound = FALSE;
			bReturn = TRUE;
			
			//Allow die to rotate
			ManualDieCompenate(stDieOffset, fDieRotate);
			DrawSearchBox( PR_COLOR_TRANSPARENT);
			// If video test enabled, not show the die size rectangle
			
			if (m_bDebugVideoTest == FALSE)
			{
				DrawRectangleDieSize(PR_COLOR_GREEN);
				if (m_bBadCutDetection)
				{
					DrawRectangleBox(m_stBadCutPoint[0], m_stBadCutPoint[1], PR_COLOR_BLUE);
				}
			}
		}
		else
		{
			bReturn = TRUE;		//v3.98T3	//Need to set to TRUE again for BURNIN SetStartPt fcn; use another fcn for auto-Learn Ej fcn
			//bReturn = FALSE;	//v3.98T1
		}

		//Display Message on HMI
		DisplaySearchDieResult(usDieType, WPR_NORMAL_DIE, WPR_NORMAL_DIE, fDieRotate, stDieOffset, fDieScore);
	}
	else
	{
		SetAlert_Red_Back(IDS_WPR_PRSEARCHERROR);
		bReturn = FALSE;
	}

	//On Joystick & Clear PR screen & Change to the original zoom mode
	SetWaferTableJoystick(TRUE);
	ChangeCamera(WPR_CAM_WAFER);
	LiveViewZoom(lOldZoom, FALSE, FALSE);

	svMsg.InitMessage(sizeof(BOOL), &bReturn);
	return 1;
}
*/

PR_WORD CWaferPr::ManualSearchDie_HWTri(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, 
										PR_COORD stCorner1, PR_COORD stCorner2, BOOL bLog)
{
	CString szMsg;
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	
	OpPrescanInit(GetPrescanPrID());

	//m_pPrGeneral->DrawRectangle(GetSrchArea().coCorner1.x, GetSrchArea().coCorner1.y,
	//									GetSrchArea().coCorner2.x, GetSrchArea().coCorner2.y, TRUE);

	szMsg.Format("ManualSearchDie_HWTri Start .... Rec ID = %d", m_stMultiSrchCmd.auwRecordID[0]);
HmiMessage(szMsg);


	PR_UBYTE ubSID = GetRunSenID();
	PR_UBYTE ubRID = GetRunRecID();
	
	//** STEP 1: Grab Share Image to gallery
	PR_UWORD usSearchResult = GrabShareImage(WPR_NORMAL_DIE, WPR_NORMAL_DIE, ubSID, ubRID);
	if (usSearchResult != PR_ERR_NOERR)
	{
		szMsg.Format("ManualSearchDie_HWTri - Fail to GrabShareImage ERR = 0x%x", usSearchResult);
		HmiMessage_Red_Back(szMsg);
		return (PR_WORD) usSearchResult;
	}

	Sleep(30);


	//** STEP 2: init HW trigger wiring signal
	GrabHwTrigger();
	Sleep(10);


	//** STEP 3: init HW trigger wiring signal
	PR_ULWORD ulImageID		= 0;
	PR_ULWORD ulStationID	= 0;
	usSearchResult = GrabShareImageRpy2(ubSID, &ulImageID, &ulStationID);
	if (usSearchResult != PR_ERR_NOERR)
	{
		szMsg.Format("ManualSearchDie_HWTri - Fail to GrabHWTrigger ERR = 0x%x", usSearchResult);
		HmiMessage_Red_Back(szMsg);
		return (PR_WORD) usSearchResult;
	}


	//** STEP 4: SrchDieCmd
	PR_SRCH_DIE_CMD		stSrchCmd;
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;
	IMG_UWORD uwCommStatus = 0;
	PR_BOOLEAN doInspection = PR_FALSE;
	BOOL bGetRpy3 = FALSE;
	PR_WIN stLFArea = GetSrchArea();

	stSrchCmd					= m_stMultiSrchCmd;
	stSrchCmd.ulLatchImageID	= ulImageID;
	stSrchCmd.uwLatchStationID	= (PR_UWORD) ulStationID;
	stSrchCmd.emLatch			= PR_TRUE;

	//Vincent Mok Suggestion 2020/04/23
	stSrchCmd.stDieAlign.stStreet.emSelectMode		= PR_SRCH_DIE_SELECT_MODE_HIGHEST_SCORE;

	stSrchCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
	stSrchCmd.uwHostMallocMaxNoOfDieInRpy2			= 100;
	if (stSrchCmd.emDefectInsp == PR_TRUE)
	{
		stSrchCmd.uwHostMallocMaxNoOfDieInRpy3		= 100;
		doInspection = PR_TRUE;
	}
	if (stSrchCmd.emPostBondInsp == PR_TRUE)
	{
		stSrchCmd.uwHostMallocMaxNoOfDieInRpy4		= 100;
	}

	stSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1	= stLFArea.coCorner1;
	stSrchCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2	= stLFArea.coCorner2;
	stSrchCmd.stDieAlign.stStreet.coProbableDieCentre.x		= (stLFArea.coCorner1.x + stLFArea.coCorner2.x) / 2;
	stSrchCmd.stDieAlign.stStreet.coProbableDieCentre.y		= (stLFArea.coCorner1.y + stLFArea.coCorner2.y) / 2;

	PR_SrchDieCmd(&stSrchCmd, ubSID, ubRID, &uwCommStatus);


	//Get reply 1
	usSearchResult = SearchDieRpy1(ubSID, &stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR && usSearchResult != 2)
	{
		szMsg.Format("ManualSearchDie_HWTri - PR_SrchDieCmd RPY1 cmd fails - ErrCode = 0x%x", usSearchResult);
		HmiMessage_Red_Yellow(szMsg);
		return usSearchResult;
	}


	//Get reply 2
	stSrchRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*) malloc(stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 * sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
	if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign == NULL)
	{
		szMsg = "ManualSearchDie_HWTri: Init stSrchRpy2 null pointer";
		HmiMessage_Red_Back(szMsg);
		return -1; 
	}
	PR_SrchDieRpy2(ubSID, &stSrchRpy2);
	if (stSrchRpy2.stStatus.uwCommunStatus == PR_COMM_ERR)
	{
		szMsg.Format("ManualSearchDie_HWTri - PR_SrchDieCmd RPY2 COMM fails - ErrCode = 0x%x", stSrchRpy2.stStatus.uwCommunStatus);
		HmiMessage_Red_Yellow(szMsg);
		return -1; 
	}

	*usDieType = stSrchRpy2.stStatus.uwPRStatus;
	if (*usDieType == PR_ERR_NOERR)
	{
        *usDieType = PR_ERR_GOOD_DIE;
    }


	//Get reply 3
	if (DieIsAlignable(*usDieType) && doInspection)
	{
		stSrchRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*) malloc(stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 * sizeof(PR_DIE_INSP_MINIMAL_RPY));
		if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin == NULL)
		{
			//Free the reply 2
			if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
			{
				free(stSrchRpy2.pstDieAlign);
			}
			szMsg = "ManualSearchDie_HWTri: Init stSrchRpy3 null pointer";
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		PR_SrchDieRpy3(ubSID, &stSrchRpy3);
		if (stSrchRpy3.stStatus.uwCommunStatus == PR_COMM_ERR)
		{
			szMsg.Format("ManualSearchDie_HWTri - PR_SrchDieRpy3 RPY2 COMM fails - ErrCode = 0x%x", stSrchRpy3.stStatus.uwCommunStatus);
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		bGetRpy3 = TRUE;
	}


	//** STEP 5: Retrieve SrchDieCmd Results
	PR_COORD stDieSize; 
	ExtractGrabShareImgDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore, &stDieSize);


	//** STEP 6: Free SrchDieCmd memory
	if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
	{
		free(stSrchRpy2.pstDieAlign);
	}
	
	if (DieIsAlignable(*usDieType) && doInspection)
	{
		if (stSrchCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin != NULL)
		{
			free(stSrchRpy3.pstDieInspExtMin);
		}
	}

	usSearchResult = m_pPrGeneral->RemoveShareImage(ubSID, ubRID, ulImageID, ulStationID);
	if (usSearchResult != PR_COMM_NOERR)
	{
		szMsg.Format("ManualSearchDie_HWTri - Fail to RemoveShareImage = 0x%x", usSearchResult);
		HmiMessage_Red_Back(szMsg);
		return -1;
	}

	return PR_ERR_NOERR;	// = 0

	/*

	PR_UBYTE ubSenderID		= GetRunSenID();	//GetScnSenID();
	PR_UBYTE ubReceiverID	= GetRunRecID();	//GetScnRecID();

	PR_GRAB_SHARE_IMAGE_CMD	stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);

	stGrbCmd.emPurpose			= GetRunPurposeI();		//GetRunPurposeI();
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stMultiSrchCmd.auwRecordID[0];
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_HW_TRIGGER;	//PR_GRAB_MODE_NORMAL;

	PR_GRAB_SHARE_IMAGE_RPY	stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		szMsg.Format("ManualSearchDie_HWTri: grab share image rpy1 fail com %d", stGrbRpy.uwCommunStatus);
		HmiMessage_Red_Back(szMsg);
		return stGrbRpy.uwCommunStatus;
	}

	Sleep(10);
	BOOL bStatus = CMS896AStn::MotionSetOutputBit("EJ_TABLE_WAFER_OUT_BIT_WAFER_CAMERA_TRIGGER", TRUE);
	if (!bStatus)
	{
		HmiMessage_Red_Back("Output BIT: EJ_TABLE_WAFER_OUT_BIT_WAFER_CAMERA_TRIGGER Error!");
		return -1;
	}
	Sleep(10);

	PR_GRAB_SHARE_IMAGE_RPY2 stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("ManualSearchDie_HWTri: grab share image rpy2 fail com %d, status %d",
							stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		
		if (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus)
		{
			szMsg.Format("ManualSearchDie_HWTri: PR_GrabShareImgRpy fail PRERR =0x%x", stGrbRpy2.stStatus.uwPRStatus);
			HmiMessage_Red_Back(szMsg);
			return stGrbRpy2.stStatus.uwPRStatus;
		}
		else
		{
			szMsg.Format("ManualSearchDie_HWTri: PR_GrabShareImgRpy fail COMERR =0x%x", stGrbRpy2.stStatus.uwCommunStatus);
			HmiMessage_Red_Back(szMsg);
			return stGrbRpy2.stStatus.uwCommunStatus;
		}
	}


	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;


	CMS896AStn::MotionSetOutputBit("EJ_TABLE_WAFER_OUT_BIT_WAFER_CAMERA_TRIGGER", FALSE);


	//for (LONG lLoop = 1; lLoop < 2; lLoop++)
	//{
		IMG_UWORD uwCommStatus;
		PR_SRCH_DIE_CMD stInspCmd	= m_stMultiSrchCmd;

		stInspCmd.ulLatchImageID	= ulImageID;
		stInspCmd.uwLatchStationID	= (PR_UWORD) ulStationID;

		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1	= GetSrchArea().coCorner1;
		stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2	= GetSrchArea().coCorner2;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		= (GetSrchArea().coCorner1.x + GetSrchArea().coCorner2.x) / 2;
		stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		= (GetSrchArea().coCorner1.y + GetSrchArea().coCorner2.y) / 2;


		stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
		stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= 100;
		if (m_stMultiSrchCmd.emDefectInsp == PR_TRUE)
		{
			stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= 100;
		}
		if (m_stMultiSrchCmd.emPostBondInsp == PR_TRUE)
		{
			stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= 100;
		}


		PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);
		if ( uwCommStatus != PR_COMM_NOERR )
		{
			szMsg.Format("ManualSearchDie_HWTri: PR_SrchDieCmd finished with com = 0x%x!", uwCommStatus);
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		PR_SRCH_DIE_RPY1 stInspRpy1;
		PR_SrchDieRpy1(ubSenderID, &stInspRpy1);
		PR_UWORD uwRpy1ComStt = stInspRpy1.uwCommunStatus;
		PR_UWORD uwRpy1Status = stInspRpy1.uwPRStatus;

		if (uwRpy1ComStt != PR_COMM_NOERR || PR_ERROR_STATUS(uwRpy1Status))
		{
			szMsg.Format("ManualSearchDie_HWTri: PR_SrchDieRpy1 finished with status 0x%x, 0x%x!\n",	uwRpy1ComStt, uwRpy1Status);
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		PR_SRCH_DIE_RPY2 stInspRpy2;
		stInspRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*) malloc (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 * sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign == NULL)
		{
			szMsg = "ManualSearchDie_HWTri: Init stDieAlign null pointer";
			HmiMessage_Red_Back(szMsg);
			return -1; 
		}

		PR_SrchDieRpy2(ubSenderID, &stInspRpy2);
		PR_UWORD uwRpy2ComStt = stInspRpy2.stStatus.uwCommunStatus;
		if (uwRpy2ComStt != PR_COMM_NOERR)
		{
			//Free the reply 2
			if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
			{
				free(stInspRpy2.pstDieAlign);
			}
			szMsg.Format("ManualSearchDie_HWTri: PR_SrchDieRpy2 finished with status 0x%x!\n", uwRpy2ComStt);
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		*usDieType = stInspRpy2.stStatus.uwPRStatus;
		BOOL bGetRpy3 = FALSE;
		PR_SRCH_DIE_RPY3 stInspRpy3;
		PR_SRCH_DIE_RPY4 stInspRpy4;

		CString szMsg1;
		szMsg1.Format("Die RPY2 result: NoOfDices found = %lu, DieType = %d, DieCenter (%f, %f), Rot = %.3f",
			stInspRpy2.uwNResults, *usDieType, stInspRpy2.pstDieAlign[0].rcoDieCentre.x, stInspRpy2.pstDieAlign[0].rcoDieCentre.y, 
			stInspRpy2.pstDieAlign[0].rDieRot);

		if ( ( (PR_TRUE == m_stMultiSrchCmd.emDefectInsp) || (PR_TRUE == m_stMultiSrchCmd.emPostBondInsp)) &&
			 (PR_COMM_NOERR == uwRpy2ComStt) && !PR_ERROR_STATUS(*usDieType) )
		{

			if ( m_stMultiSrchCmd.emDefectInsp == PR_TRUE )
			{
				stInspRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*) malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy3 * sizeof(PR_DIE_INSP_MINIMAL_RPY));
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stInspRpy3.pstDieInspExtMin == NULL)
				{
					//Free the reply 2
					if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2>0 && stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}

					szMsg = "ManualSearchDie_HWTri: Init stDieAlign null pointer";
					HmiMessage_Red_Back(szMsg);
					return -1; 
				}
			}

			if (m_stMultiSrchCmd.emPostBondInsp == PR_TRUE)
			{
				stInspRpy4.pstPostBondInspMin = (PR_POST_INSP_MINIMAL_RPY*) malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy4 * sizeof(PR_POST_INSP_MINIMAL_RPY));
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy4 > 0 && stInspRpy4.pstPostBondInspMin == NULL )
				{
					//Free the reply 2
					if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}
					//Free the reply 3
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stInspRpy3.pstDieInspExtMin != NULL )
					{
						free(stInspRpy3.pstDieInspExtMin);
					}

					szMsg = "ManualSearchDie_HWTri: Init stDieAlign null pointer";
					HmiMessage_Red_Back(szMsg);
					return -1; 
				}
			}

			bGetRpy3 = TRUE;

			PR_SrchDieRpy3(ubSenderID, &stInspRpy3);
			PR_UWORD uwRpy3ComStt = stInspRpy3.stStatus.uwCommunStatus;
			if (uwRpy3ComStt != PR_COMM_NOERR)
			{
				//Free the reply 2
				if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stInspRpy2.pstDieAlign != NULL)
				{
					free(stInspRpy2.pstDieAlign);
				}
				//Free the reply 3
				if ( stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stInspRpy3.pstDieInspExtMin != NULL )
				{
					free(stInspRpy3.pstDieInspExtMin);
				}
				//Free the reply 4
				if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4 > 0 && stInspRpy4.pstPostBondInspMin != NULL )
				{
					free(stInspRpy4.pstPostBondInspMin);
				}	// free point for one search
				
				szMsg.Format("ManualSearchDie_HWTri: PR_SrchDieRpy3 finished with status %u!\n",	uwRpy3ComStt);
				HmiMessage_Red_Back(szMsg);
				return -1;
			}

			PR_UWORD uwRpy3Status = stInspRpy3.stStatus.uwPRStatus;
			// inspection result
			if ( (PR_TRUE == m_stMultiSrchCmd.emPostBondInsp) && PR_COMM_NOERR == uwRpy3ComStt && !PR_ERROR_STATUS(uwRpy3Status) )
			{
				PR_SrchDieRpy4(ubSenderID, &stInspRpy4);

				PR_UWORD uwRpy4ComStt = stInspRpy4.stStatus.uwCommunStatus;
				if (uwRpy4ComStt != PR_COMM_NOERR)
				{
					//Free the reply 2
					if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stInspRpy2.pstDieAlign != NULL)
					{
						free(stInspRpy2.pstDieAlign);
					}
					//Free the reply 3
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stInspRpy3.pstDieInspExtMin!=NULL )
					{
						free(stInspRpy3.pstDieInspExtMin);
					}
					//Free the reply 4
					if( stInspCmd.uwHostMallocMaxNoOfDieInRpy4 > 0 && stInspRpy4.pstPostBondInspMin!=NULL )
					{
						free(stInspRpy4.pstPostBondInspMin);
					}	// free point for one search
					
					szMsg.Format("ManualSearchDie_HWTri: PR_SrchDieRpy4 finished with status %u!\n", uwRpy4ComStt);
					HmiMessage_Red_Back(szMsg);
					return -1;
				}
			}
		}

		USHORT usDieSum = stInspRpy2.uwNResults;
		PR_COORD stPRDieSize;

		//Identify die type
		//ExtractDieResult(stInspRpy2, stInspRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore, &stPRDieSize);
		ExtractGrabShareImgDieResult(stInspRpy2, stInspRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore, &stPRDieSize);

//HmiMessage(szMsg1);
		szMsg.Format("Die result: DieType = %d, DieOffset (%d, %d), Rot = %f, DieScore = %f",
			*usDieType, stDieOffset->x, stDieOffset->y, fDieRotate, fDieScore);
HmiMessage(szMsg);

		//Free the reply 2
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stInspRpy2.pstDieAlign != NULL)
		{
			free(stInspRpy2.pstDieAlign);
		}
		//Free the reply 3
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stInspRpy3.pstDieInspExtMin != NULL )
		{
			free(stInspRpy3.pstDieInspExtMin);
		}
		//Free the reply 4
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy4 > 0 && stInspRpy4.pstPostBondInspMin != NULL )
		{
			free(stInspRpy4.pstPostBondInspMin);
		}
	
	//}	//END for (LONG lLoop = 1; lLoop < 2; lLoop++)

	PR_REMOVE_SHARE_IMAGE_CMD stRmvCmd;
	PR_InitRemoveShareImgCmd(&stRmvCmd);

	stRmvCmd.ulImageID = ulImageID;
	stRmvCmd.emFreeAll = PR_TRUE;

	PR_REMOVE_SHARE_IMAGE_RPY stRmvRpy;
	PR_RemoveShareImgCmd(&stRmvCmd, ubSenderID, ubReceiverID, &stRmvRpy);

	if (PR_COMM_NOERR != stRmvRpy.uwCommunStatus)
	{
		szMsg.Format("ManualSearchDie_HWTri: PR_InitRemoveShareImgCmd fail com %d to delete the image", stRmvRpy.uwCommunStatus);
		HmiMessage_Red_Back(szMsg);
		pUtl->PrescanMoveLog(szMsg);
	}

HmiMessage("ManualSearchDie_HWTri DONE.");
	return PR_ERR_NOERR;
	*/
}

PR_UWORD CWaferPr::GrabShareImage(BOOL bDieType, LONG lInputDieNo, PR_UBYTE ubSID, PR_UBYTE ubRID)
{
	LONG		lDieNo = 0;
	PR_UWORD	lPrSrchID;
	CString		szMsg;

	if (lInputDieNo <= 0)
		lInputDieNo = 1;
	if (bDieType == WPR_NORMAL_DIE)
		lDieNo = WPR_GEN_NDIE_OFFSET + lInputDieNo;
	if (bDieType == WPR_REFERENCE_DIE)
		lDieNo = WPR_GEN_RDIE_OFFSET + lInputDieNo;
	lPrSrchID	= m_ssGenPRSrchID[lDieNo];

	if (lPrSrchID == 0)
	{
		return IDS_WPR_DIENOTLEARNT;
	}

	PR_GRAB_SHARE_IMAGE_CMD		stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);

	stGrbCmd.emPurpose			= GetRunPurposeI();
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= lPrSrchID;
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_HW_TRIGGER;
	stGrbCmd.emDisplayAutoBond	= PR_FALSE; 

	PR_GRAB_SHARE_IMAGE_RPY		stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSID, ubRID, &stGrbRpy);

	if (stGrbRpy.uwCommunStatus != PR_COMM_NOERR)
	{
		szMsg.Format("ManualSearchDie_HWTri: grab share image rpy1 fail com %d", stGrbRpy.uwCommunStatus);
		HmiMessage_Red_Back(szMsg);
		return stGrbRpy.uwCommunStatus;
	}

	return PR_ERR_NOERR;
}


BOOL CWaferPr::GrabHwTrigger()
{
	MotionSetOutputBit("EJ_TABLE_WAFER_OUT_BIT_WAFER_CAMERA_TRIGGER", TRUE);
	DOUBLE dSetBitStartTime = GetTime();

	//Exposure time
	DOUBLE dDelay	= 3 - _round(GetTime() - dSetBitStartTime);
	LONG lDelay		= _round(dDelay);
	if (lDelay > 0)
	{
		Sleep(lDelay);
	}

	MotionSetOutputBit("EJ_TABLE_WAFER_OUT_BIT_WAFER_CAMERA_TRIGGER", FALSE);
	return TRUE;
}


PR_UWORD CWaferPr::GrabShareImageRpy2(PR_UBYTE ubSID, PR_ULWORD *ulImageID, PR_ULWORD *ulStationID)
{
	//Grab Done
	PR_GRAB_SHARE_IMAGE_RPY2 stGrbRpy2;
	PR_GrabShareImgRpy(ubSID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		CString szMsg;
		szMsg.Format("GrabHWTrigger: grab share image rpy2 fail com %d, status %d",
							stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		if (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus)
		{
			szMsg.Format("GrabHwTrigger: PR_GrabShareImgRpy fail PRERR =0x%d", stGrbRpy2.stStatus.uwPRStatus);
			HmiMessage_Red_Back(szMsg);
			return stGrbRpy2.stStatus.uwPRStatus;
		}
		else
		{
			szMsg.Format("GrabHwTrigger: PR_GrabShareImgRpy fail COMERR =0x%d", stGrbRpy2.stStatus.uwCommunStatus);
			HmiMessage_Red_Back(szMsg);
			return stGrbRpy2.stStatus.uwCommunStatus;
		}
	}

	// now add the image to queue
	*ulStationID = stGrbRpy2.ulStationID;
	*ulImageID	 = stGrbRpy2.ulImageID;
	return PR_ERR_NOERR;
}


PR_UWORD CWaferPr::ExtractGrabShareImgDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, PR_COORD *stDieSize)
{
	CString szTemp, szLog;
	INT dX0, dX1, dY0, dY1;
	DOUBLE dReturnScore = 0.0;
	
	PR_UBYTE ubSID = GetRunSenID();	
	PR_UBYTE ubRID = GetRunRecID();

	DOUBLE dDieRot = stSrchRpy2.pstDieAlign[0].rDieRot;
	DOUBLE dDieCenterX = stSrchRpy2.pstDieAlign[0].rcoDieCentre.x;
	DOUBLE dDieCenterY = stSrchRpy2.pstDieAlign[0].rcoDieCentre.y;

	INT nDieCenterX = (INT) dDieCenterX;
	INT nDieCenterY = (INT) dDieCenterY;

	//szMsg1.Format("Die RPY2 result: NoOfDices found = %lu, DieType = %d, DieCenter (%f, %f), Rot = %.3f",
	//	stInspRpy2.uwNResults, *usDieType, stInspRpy2.pstDieAlign[0].rcoDieCentre.x, stInspRpy2.pstDieAlign[0].rcoDieCentre.y, 
	//	stInspRpy2.pstDieAlign[0].rDieRot);

	//Single SrchDieCmd should only contains ONE die within normal Die 1 Srch Window
	if (stSrchRpy2.uwNResults != 1)
	{
		stDieOffset->x	= 0;	
		stDieOffset->y	= 0;
		*fDieScore		= 0;
		return 0;
	}

	*fDieScore = 0;	//stSrchRpy2.stDieAlign[0].stGen.rMatchScore;

	switch (stSrchRpy2.stStatus.uwPRStatus)
	{
	case PR_ERR_NOERR:
	case PR_WARN_SMALL_SRCH_WIN:
	case PR_WARN_TOO_MANY_DIE:
	case PR_WARN_BACKUP_ALIGNED:
		if (bUseReply3 == TRUE)
		{
			*usDieType = stSrchRpy3.stStatus.uwPRStatus;

			//Check BadCut	
			if ((m_bGetBadCutReply == TRUE) && (*usDieType == PR_ERR_NOERR))
			{
				//memset(&stSrchRpy4,0,sizeof(stSrchRpy4));
				PR_SRCH_DIE_RPY4 stSrchRpy4;

				if (SearchDieRpy4(ubSID, &stSrchRpy4) == PR_ERR_INSUFF_EPOXY_COVERAGE)
				{
					*usDieType = PR_ERR_INSUFF_EPOXY_COVERAGE;
				}
			}
		}
		else
		{
			*usDieType = stSrchRpy2.stStatus.uwPRStatus;
		}

		*fDieRotate	= dDieRot;

		if (IsMS90HalfSortMode())
		{
			if (*fDieRotate > 90)
				*fDieRotate -= 180;
			else if ( *fDieRotate <-90)
				*fDieRotate += 180;
		}

		stDieOffset->x	= nDieCenterX;
		stDieOffset->y	= nDieCenterY;
		*fDieScore		= 0;	//stSrchRpy2.stDieAlign[0].stGen.rMatchScore;

		//dX0 = (INT) (stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].x);
		//dX1 = (INT) (stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[1].x);
		stDieSize->x	= 0;	//abs(dX0 - dX1);

		//dY0 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].y);
		//dY1 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[3].y);
		stDieSize->y	= 0;	//abs(dY0 - dY1);

		if (*usDieType == PR_ERR_NOERR)
		{
			*usDieType	= PR_ERR_GOOD_DIE;
		}

		dReturnScore = stSrchRpy2.stDieAlign[0].stGen.rFuzzifiedScore;
		break;

	case PR_ERR_LOCATED_DEFECTIVE_DIE:
	case PR_ERR_ROTATION:
		*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
		*fDieRotate		= dDieRot;
		
		if (IsMS90HalfSortMode())
		{
			if( *fDieRotate>90 )
				*fDieRotate -= 180;
			else if( *fDieRotate<-90 )
				*fDieRotate += 180;
		}

		//*stDieOffset	= stSrchRpy2.stDieAlign[0].stGen.coDieCentre;
		stDieOffset->x	= nDieCenterX;
		stDieOffset->y	= nDieCenterY;
		*fDieScore		= 0;	//stSrchRpy2.stDieAlign[0].stGen.rMatchScore;

		////dX0 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].x);
		dX1 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[1].x);
		stDieSize->x	= 0;	//abs(dX0 - dX1);
		
		//dY0 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[0].y);
		//dY1 = (INT)(stSrchRpy2.stDieAlign[0].stGen.arcoDieCorners[3].y);
		stDieSize->y	= 0;	//abs(dY0 - dY1);

		dReturnScore = 0;	//stSrchRpy2.stDieAlign[0].stGen.rFuzzifiedScore;
		break;

	case PR_ERR_DIE_SIZE_RJT:
	case PR_ERR_DEFECTIVE_DIE:
	case PR_ERR_CORNER_ANGLE_FAIL:
	case PR_ERR_NO_DIE:
	case PR_ERR_ROTATION_EXCEED_SPEC:
	case PR_ERR_NON_LOCATED_DEFECTIVE_DIE:
	case PR_ERR_NO_SYS_MEM:
	case PR_ERR_FAIL_CONTRAST:
	case PR_ERR_PID_NOT_LD:
	default:
		*usDieType		= stSrchRpy2.stStatus.uwPRStatus;
		if (*usDieType == PR_ERR_DEFECTIVE_DIE)
		{
			*usDieType = PR_ERR_NON_LOCATED_DEFECTIVE_DIE;
		}

		*fDieRotate		= 0.0;
		stDieOffset->x	= (PR_WORD)GetPrCenterX();
		stDieOffset->y	= (PR_WORD)GetPrCenterY();
		stDieSize->x	= 0;
		stDieSize->y	= 0;
		break;
	}

	//(*m_psmfSRam)["WaferTable"]["Die Orientation Score"] = (DOUBLE) dReturnScore;
	return 0;
}

	
BOOL CWaferPr::ReceiveHWTriRpyinHouseKeeping()	//andrewng 2020-0616
{
	PR_UBYTE ubSID = GetRunSenID();	
	PR_UBYTE ubRID = GetRunRecID();
	CString szMsg;
	PR_ULWORD ulImageID = 0;
	PR_ULWORD ulStationID = 0;


	DisplaySequence("WPR - GrabHwTrigger Start at House keeping");
	GrabHwTrigger();

	PR_UWORD usSearchResult = GrabShareImageRpy2(ubSID, &ulImageID, &ulStationID);
	if (usSearchResult != PR_ERR_NOERR)
	{
		szMsg.Format("WPR - Fail to GrabHWTrigger = %x", usSearchResult);
		DisplaySequence(szMsg);
		return FALSE;
	}

	DisplaySequence("WPR - RemoveShareImage at House keeping");
	usSearchResult = m_pPrGeneral->RemoveShareImage(ubSID, ubRID, ulImageID, ulStationID);
	if (usSearchResult != PR_COMM_NOERR)
	{
		szMsg.Format("WPR - Fail to RemoveShareImage = %x", usSearchResult);
		DisplaySequence(szMsg);
		return FALSE;
	}

	return TRUE;
}

