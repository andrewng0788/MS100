/////////////////////////////////////////////////////////////////
// BPR_SrchDie_HWTri.cpp : HMI Registered Command of the CBondPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		MOn, June 1, 2020
//	Revision:	1.00
//
//	By:			Andrew Ng
//
//	Copyright @ ASM Assembly Automation Ltd., 2020.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MS896A_Constant.h"
#include "BondPr.h"
#include "BPR_Constant.h"
#include "PRTYPE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


PR_WORD CBondPr::ManualSearchDie_HWTri(BOOL bDieType, LONG lDieNo, PR_BOOLEAN bLatch, PR_BOOLEAN bAlign, PR_BOOLEAN bInspect, 
										PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore,
										PR_COORD stCorner1, PR_COORD stCorner2, DOUBLE dRefAngle)
{
	PR_UWORD usSearchResult;
	BOOL bGetRpy3 = FALSE;
	PR_BOOLEAN bDoInspection = PR_FALSE;
	CString szMsg;

	LONG lInputDieNo = lDieNo;
	if (lInputDieNo <= 0)
	{
		lInputDieNo = 1;
	}

	if (bDieType == BPR_NORMAL_DIE)
	{
		lDieNo = BPR_GEN_NDIE_OFFSET + lInputDieNo;
	}
	else
	{
		lDieNo = BPR_GEN_RDIE_OFFSET + lInputDieNo;
	}

	LONG lPrSrchID	= m_ssGenPRSrchID[lDieNo];
	if (lPrSrchID == 0)
	{
		szMsg.Format("BPR ManualSearchDie_HWTri -- Die record not valid - DieType=%d, DieNo=%ld. PRID=%ld", 
						bDieType, lDieNo, lPrSrchID);
		HmiMessage_Red_Yellow(szMsg);
		return -1;
	}

	m_ulStationID = 0;
	m_ulImageID = 0;

	PR_UBYTE ubSenderID	 = MS899_BOND_CAM_SEND_ID;
	PR_UBYTE ubReceiverID = MS899_BOND_CAM_RECV_ID;
	
	if (!PreAutoSearchDie_HWTri())
	{
		HmiMessage_Red_Yellow("PreAutoSearchDie_HWTri FAIL");
		return -1;
	}
	Sleep(30);
	
	/*
	OpPrescanInit(1);

	PR_GRAB_SHARE_IMAGE_CMD	stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);

	stGrbCmd.emPurpose			= MS899_BOND_PB_PURPOSE;
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stMultiSrchCmd.auwRecordID[0];
	//stGrbCmd.emGrabMode		= PR_GRAB_MODE_NORMAL;
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_HW_TRIGGER;
	//stGrbCmd.emDisplayAutoBond	= PR_FALSE; 


	PR_GRAB_SHARE_IMAGE_RPY	 stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		szMsg.Format("BPR ManualSearchDie_HWTri -- PR_GrabShareImgCmd fail - COMM ERR = 0x%x", stGrbRpy.uwCommunStatus);
		HmiMessage_Red_Yellow(szMsg);
		return -1;
	}

	Sleep(30);
	*/

	//** STEP 2: init HW trigger wiring signal
	if (!AutoSearchDie_HWTri())
	{
		HmiMessage_Red_Yellow("AutoSearchDie_HWTri FAIL");
		return -1;
	}
	AutoGrabDieDone_HWTri();
	
	/*
	GrabHwTrigger();
	Sleep(10);


	PR_GRAB_SHARE_IMAGE_RPY2 stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("BPR ManualSearchDie_HWTri -- PR_GrabShareImgRpy fail - COMM ERR = 0x%x, PR ERR = 0x%x", 
				stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		HmiMessage_Red_Yellow(szMsg);
		return -1;
	}


	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;
	m_ulStationID	= stGrbRpy2.ulStationID;
	m_ulImageID		= stGrbRpy2.ulImageID;

	MotionSetOutputBit("EJ_TABLE_BIN_OUT_BIT_BIN_CAMERA_TRIGGER", FALSE);
	*/

	if (!AutoDieResult_HWTri(usDieType, fDieRotate, stDieOffset, fDieScore, FALSE))
	{
		HmiMessage_Red_Yellow("AutoDieResult_HWTri FAIL");
		return -1;
	}

	/*
	PR_ULWORD ulStationID	= m_ulStationID;
	PR_ULWORD ulImageID		= m_ulImageID;

	
	IMG_UWORD uwCommStatus = 0;
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_WIN stSearchArea;
	stSearchArea.coCorner1 = stCorner1;
	stSearchArea.coCorner2 = stCorner2;
	VerifyPRRegion(&stSearchArea);

	PR_SRCH_DIE_CMD stInspCmd	= m_stMultiSrchCmd;  //used the m_stMultiSrchCmd
	stInspCmd.ulLatchImageID	= ulImageID;
	stInspCmd.uwLatchStationID	= (PR_UWORD) ulStationID;

	PR_WORD lULX = stSearchArea.coCorner1.x;
	PR_WORD lULY = stSearchArea.coCorner1.y; 
	PR_WORD lLRX = stSearchArea.coCorner2.x;
	PR_WORD lLRY = stSearchArea.coCorner2.y;
	PR_WORD lCTX = (lULX + lLRX)/2;
	PR_WORD lCTY = (lULY + lLRY)/2;

	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
	stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
	stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;

	stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
	stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= 10;	//BOND_ONE_SEARCH_MAX_DIE;
	if (m_stMultiSrchCmd.emDefectInsp == PR_TRUE)
	{
		stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= 10;	//BOND_ONE_SEARCH_MAX_DIE;
		bDoInspection = PR_TRUE;
	}
	if (m_stMultiSrchCmd.emPostBondInsp == PR_TRUE)
	{
		stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= 10;	//BOND_ONE_SEARCH_MAX_DIE;
	}
	
	
	PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);
	if ( uwCommStatus != PR_COMM_NOERR )
	{
		szMsg.Format("BPR ManualSearchDie_HWTri - PR_SrchDieCmd Fail - COMM ERR = 0x%x", uwCommStatus);
		HmiMessage_Red_Yellow(szMsg);
		return -1;
	}


	//Get reply 1
	usSearchResult = SearchDieRpy1(&stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR && usSearchResult != 2)
	{
		szMsg.Format("BPR ManualSearchDie_HWTri - PR_SrchDieCmd RPY1 cmd fails - ErrCode = 0x%x", usSearchResult);
		HmiMessage_Red_Yellow(szMsg);
		return usSearchResult;
	}


	//Get reply 2
	stSrchRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*) malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy2 * sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
	if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign == NULL)
	{
		szMsg = "ManualSearchDie_HWTri: Init stSrchRpy2 NULL pointer";
		HmiMessage_Red_Back(szMsg);
		return -1; 
	}
	PR_SrchDieRpy2(ubSenderID, &stSrchRpy2);
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
	//if (DieIsAlignable(*usDieType) && bInspect)
	if ( DieIsAlignable(*usDieType) && (bDoInspection == PR_TRUE) )
	{
		stSrchRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*) malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy3 * sizeof(PR_DIE_INSP_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin == NULL)
		{
			//Free the reply 2
			if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
			{
				free(stSrchRpy2.pstDieAlign);
			}
			szMsg = "BPR ManualSearchDie_HWTri: Init stSrchRpy3 null pointer";
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		PR_SrchDieRpy3(ubSenderID, &stSrchRpy3);
		if (stSrchRpy3.stStatus.uwCommunStatus == PR_COMM_ERR)
		{
			szMsg.Format("BPR ManualSearchDie_HWTri - PR_SrchDieRpy3 RPY2 COMM fails - ErrCode = 0x%x", stSrchRpy3.stStatus.uwCommunStatus);
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		bGetRpy3 = TRUE;
	}


	//** STEP 5: Retrieve SrchDieCmd Results
	PR_COORD stDieSize; 
	ExtractGrabShareImgDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, usDieType, fDieRotate, stDieOffset, fDieScore, &stDieSize);


	//** STEP 6: Free SrchDieCmd memory
	if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
	{
		free(stSrchRpy2.pstDieAlign);
	}
	
	if (DieIsAlignable(*usDieType) && (bDoInspection == PR_TRUE))
	{
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin != NULL)
		{
			free(stSrchRpy3.pstDieInspExtMin);
		}
	}


	//** STEP 7: Free Gallery image buffer
	usSearchResult = m_pPrGeneral->RemoveShareImage(ubSenderID, ubReceiverID, ulImageID, ulStationID);
	if (usSearchResult != PR_COMM_NOERR)
	{
		szMsg.Format("BPR ManualSearchDie_HWTri - Fail to RemoveShareImage = 0x%x", usSearchResult);
		HmiMessage_Red_Back(szMsg);
		return -1;
	}
	*/
	return PR_ERR_NOERR;	// = 0
}


PR_UWORD CBondPr::ExtractGrabShareImgDieResult(PR_SRCH_DIE_RPY2 stSrchRpy2, PR_SRCH_DIE_RPY3 stSrchRpy3, BOOL bUseReply3, PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, PR_COORD *stDieSize)
{
	CString szTemp, szLog;
	INT dX0, dX1, dY0, dY1;
	DOUBLE dReturnScore = 0.0;
	
	PR_UBYTE ubSID = GetPostBondPRSenderID();	
	PR_UBYTE ubRID = GetPostBondPRReceiverID();

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


BOOL CBondPr::GrabHwTrigger()
{
	MotionSetOutputBit("EJ_TABLE_BIN_OUT_BIT_BIN_CAMERA_TRIGGER", TRUE);
	//DOUBLE dSetBitStartTime = GetTime();
	//Exposure time
	Sleep(3);

	//MotionSetOutputBit("EJ_TABLE_BIN_OUT_BIT_BIN_CAMERA_TRIGGER", FALSE);
	return TRUE;
}

BOOL CBondPr::PreAutoSearchDie_HWTri(BOOL bAutoBond)
{
	PR_UBYTE ubSenderID	 = MS899_BOND_CAM_SEND_ID;
	PR_UBYTE ubReceiverID = MS899_BOND_CAM_RECV_ID;

	OpPrescanInit(1);
	
	m_ulStationID = 0;
	m_ulImageID = 0;

	PR_GRAB_SHARE_IMAGE_CMD	stGrbCmd;
	PR_InitGrabShareImgCmd(&stGrbCmd);
	
	stGrbCmd.emPurpose			= MS899_BOND_PB_PURPOSE;
	stGrbCmd.emGrabFromRecord	= PR_TRUE;
	stGrbCmd.emOnTheFlyGrab		= PR_FALSE;
	stGrbCmd.uwRecordID			= m_stMultiSrchCmd.auwRecordID[0];
	//stGrbCmd.emGrabMode		= PR_GRAB_MODE_NORMAL;
	stGrbCmd.emGrabMode			= PR_GRAB_MODE_HW_TRIGGER;
	//stGrbCmd.emDisplayAutoBond	= PR_FALSE; 

	if (bAutoBond)
	{
		DisplaySequence("BPR - PreAutoSearchDie_HWTri (HW)");
	}

	PR_GRAB_SHARE_IMAGE_RPY	stGrbRpy;
	PR_GrabShareImgCmd(&stGrbCmd, ubSenderID, ubReceiverID, &stGrbRpy);
	if (PR_COMM_NOERR != stGrbRpy.uwCommunStatus)
	{
		CString szMsg;
		szMsg.Format("BPR PreAutoSearchDie_HWTri -- PR_GrabShareImgCmd fail - COMM ERR = 0x%x", stGrbRpy.uwCommunStatus);
		HmiMessage_Red_Yellow(szMsg);
		return FALSE;
	}

	if (bAutoBond)
	{
		m_unHWGrabState = 1;
	}
	return TRUE;
}

BOOL CBondPr::AutoSearchDie_HWTri(BOOL bAutoBond)
{
	GrabHwTrigger();
	//Sleep(10);
	if (bAutoBond)
	{
		TakeTime(BPR2);		
		SetBPRLatched(TRUE);	//Allow BT to Move
	}
	
	Sleep(10);

	PR_UBYTE ubSenderID	 = MS899_BOND_CAM_SEND_ID;
	PR_UBYTE ubReceiverID = MS899_BOND_CAM_RECV_ID;
	PR_GRAB_SHARE_IMAGE_RPY2 stGrbRpy2;
	CString szMsg;

	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("BPR ManualSearchDie_HWTri -- PR_GrabShareImgRpy fail - COMM ERR = 0x%x, PR ERR = 0x%x", 
				stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		HmiMessage_Red_Yellow(szMsg);
		return FALSE;
	}

	MotionSetOutputBit("EJ_TABLE_BIN_OUT_BIT_BIN_CAMERA_TRIGGER", FALSE);

	m_ulStationID	= stGrbRpy2.ulStationID;
	m_ulImageID		= stGrbRpy2.ulImageID;
	return TRUE;
}

BOOL CBondPr::AutoGrabDieDone_HWTri(VOID)
{
	return TRUE;
}

LONG CBondPr::AutoDieResult_HWTri(PR_UWORD *usDieType, PR_REAL *fDieRotate, PR_COORD *stDieOffset, PR_REAL *fDieScore, BOOL bPostBond)
{
	CString szMsg;

	PR_UBYTE ubSenderID	 = MS899_BOND_CAM_SEND_ID;
	PR_UBYTE ubReceiverID = MS899_BOND_CAM_RECV_ID;

	/*
	PR_GRAB_SHARE_IMAGE_RPY2 stGrbRpy2;
	PR_GrabShareImgRpy(ubSenderID, &stGrbRpy2);
	if (PR_COMM_NOERR != stGrbRpy2.stStatus.uwCommunStatus || (PR_ERR_NOERR != stGrbRpy2.stStatus.uwPRStatus) )
	{
		szMsg.Format("BPR AutoDieResult_HWTri -- PR_GrabShareImgRpy fail - COMM ERR = 0x%x, PR ERR = 0x%x", 
				stGrbRpy2.stStatus.uwCommunStatus, stGrbRpy2.stStatus.uwPRStatus);
		HmiMessage_Red_Yellow(szMsg);
		return FALSE;
	}


	// now add the image to queue
	PR_ULWORD ulStationID	= stGrbRpy2.ulStationID;
	PR_ULWORD ulImageID		= stGrbRpy2.ulImageID;
	*/

	if ( (m_ulImageID == 0) && (m_ulStationID == 0) )
	{
		HmiMessage_Red_Yellow("AutoDieResult_HWTri: Invalid Image ID and Station ID !");
		return FALSE;
	}

	PR_BOOLEAN bDoInspection = PR_FALSE;

	IMG_UWORD uwCommStatus = 0;
	PR_SRCH_DIE_RPY1	stSrchRpy1;
	PR_SRCH_DIE_RPY2	stSrchRpy2;
	PR_SRCH_DIE_RPY3	stSrchRpy3;

	PR_WIN stSearchArea;
	stSearchArea.coCorner1 = m_stSearchArea.coCorner1;
	stSearchArea.coCorner2 = m_stSearchArea.coCorner2;
	VerifyPRRegion(&stSearchArea);

	PR_SRCH_DIE_CMD stInspCmd	= m_stMultiSrchCmd;  //used the m_stMultiSrchCmd
	stInspCmd.ulLatchImageID	= m_ulImageID;
	stInspCmd.uwLatchStationID	= (PR_UWORD) m_ulStationID;

	PR_WORD lULX = stSearchArea.coCorner1.x;
	PR_WORD lULY = stSearchArea.coCorner1.y; 
	PR_WORD lLRX = stSearchArea.coCorner2.x;
	PR_WORD lLRY = stSearchArea.coCorner2.y;
	PR_WORD lCTX = (lULX + lLRX)/2;
	PR_WORD lCTY = (lULY + lLRY)/2;

	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.x = lULX;
	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner1.y = lULY;
	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.x = lLRX;
	stInspCmd.stDieAlign.stStreet.stLargeSrchWin.coCorner2.y = lLRY;
	stInspCmd.stDieAlign.stStreet.coProbableDieCentre.x		 = lCTX;
	stInspCmd.stDieAlign.stStreet.coProbableDieCentre.y		 = lCTY;

	stInspCmd.stDieAlign.stStreet.emSingleDieSrch	= PR_FALSE;
	stInspCmd.uwHostMallocMaxNoOfDieInRpy2			= 10;	//BOND_ONE_SEARCH_MAX_DIE;
	if (m_stMultiSrchCmd.emDefectInsp == PR_TRUE)
	{
		stInspCmd.uwHostMallocMaxNoOfDieInRpy3		= 10;	//BOND_ONE_SEARCH_MAX_DIE;
		bDoInspection = PR_TRUE;
	}
	if (m_stMultiSrchCmd.emPostBondInsp == PR_TRUE)
	{
		stInspCmd.uwHostMallocMaxNoOfDieInRpy4		= 10;	//BOND_ONE_SEARCH_MAX_DIE;
	}	
	

	MotionSetOutputBit(BPR_SO_SEARCH_DIE_CMD, TRUE);
	PR_SrchDieCmd(&stInspCmd, ubSenderID, ubReceiverID, &uwCommStatus);
	MotionSetOutputBit(BPR_SO_SEARCH_DIE_CMD, FALSE);

	if ( uwCommStatus != PR_COMM_NOERR )
	{
		szMsg.Format("BPR AutoDieResult_HWTri - PR_SrchDieCmd Fail - COMM ERR = 0x%x", uwCommStatus);
		HmiMessage_Red_Yellow(szMsg);
		return FALSE;
	}


	//Get reply 1
	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_1, TRUE);
	PR_UWORD usSearchResult = SearchDieRpy1(&stSrchRpy1);
	if (usSearchResult != PR_ERR_NOERR && usSearchResult != 2)
	{
		szMsg.Format("BPR ManualSearchDie_HWTri - PR_SrchDieCmd RPY1 cmd fails - ErrCode = 0x%x", usSearchResult);
		HmiMessage_Red_Yellow(szMsg);
		return usSearchResult;
	}
	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_1, FALSE);


	//Get reply 2
	stSrchRpy2.pstDieAlign = (PR_DIE_ALIGN_MINIMAL_RPY*) malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy2 * sizeof(PR_DIE_ALIGN_MINIMAL_RPY));
	if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign == NULL)
	{
		szMsg = "ManualSearchDie_HWTri: Init stSrchRpy2 NULL pointer";
		HmiMessage_Red_Back(szMsg);
		return -1; 
	}

	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_2, TRUE);
	PR_SrchDieRpy2(ubSenderID, &stSrchRpy2);
	if (stSrchRpy2.stStatus.uwCommunStatus == PR_COMM_ERR)
	{
		szMsg.Format("ManualSearchDie_HWTri - PR_SrchDieCmd RPY2 COMM fails - ErrCode = 0x%x", stSrchRpy2.stStatus.uwCommunStatus);
		HmiMessage_Red_Yellow(szMsg);
		return -1; 
	}
	MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_2, FALSE);


	PR_UWORD usDType = stSrchRpy2.stStatus.uwPRStatus;
	if (usDType == PR_ERR_NOERR)
	{
        usDType = PR_ERR_GOOD_DIE;
    }
	*usDieType = usDType;

	PR_COORD stDOffset;
	//PR_COORD stDSize;
	PR_REAL fDRotate = 0;
	PR_REAL fDScore = 0;
	BOOL bGetRpy3 = FALSE;

	//Get reply 3
	//if (DieIsAlignable(*usDieType) && bInspect)
	if ( DieIsAlignable(usDType) && (bDoInspection == PR_TRUE) )
	{
		stSrchRpy3.pstDieInspExtMin = (PR_DIE_INSP_MINIMAL_RPY*) malloc(stInspCmd.uwHostMallocMaxNoOfDieInRpy3 * sizeof(PR_DIE_INSP_MINIMAL_RPY));
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin == NULL)
		{
			//Free the reply 2
			if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
			{
				free(stSrchRpy2.pstDieAlign);
			}
			szMsg = "BPR ManualSearchDie_HWTri: Init stSrchRpy3 null pointer";
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_3, TRUE);
		PR_SrchDieRpy3(ubSenderID, &stSrchRpy3);
		if (stSrchRpy3.stStatus.uwCommunStatus == PR_COMM_ERR)
		{
			szMsg.Format("BPR ManualSearchDie_HWTri - PR_SrchDieRpy3 RPY2 COMM fails - ErrCode = 0x%x", stSrchRpy3.stStatus.uwCommunStatus);
			HmiMessage_Red_Back(szMsg);
			return -1;
		}

		MotionSetOutputBit(BPR_SO_SEARCH_DIE_REPLY_3, FALSE);
		bGetRpy3 = TRUE;
	}


	//** STEP 5: Retrieve SrchDieCmd Results
	PR_COORD stDSize; 
	ExtractGrabShareImgDieResult(stSrchRpy2, stSrchRpy3, bGetRpy3, &usDType, &fDRotate, &stDOffset, &fDScore, &stDSize);

	*usDieType		= usDType;
	*fDieRotate		= fDRotate;
	*stDieOffset	= stDOffset;
	*fDieScore		= fDScore;


	//** STEP 6: Free SrchDieCmd memory
	if (stInspCmd.uwHostMallocMaxNoOfDieInRpy2 > 0 && stSrchRpy2.pstDieAlign != NULL)
	{
		free(stSrchRpy2.pstDieAlign);
	}
	
	if (DieIsAlignable(usDType) && (bDoInspection == PR_TRUE))
	{
		if (stInspCmd.uwHostMallocMaxNoOfDieInRpy3 > 0 && stSrchRpy3.pstDieInspExtMin != NULL)
		{
			free(stSrchRpy3.pstDieInspExtMin);
		}
	}


	//** STEP 7: Free Gallery image buffer
	usSearchResult = m_pPrGeneral->RemoveShareImage(ubSenderID, ubReceiverID, m_ulImageID, m_ulStationID);
	if (usSearchResult != PR_COMM_NOERR)
	{
		szMsg.Format("BPR ManualSearchDie_HWTri - Fail to RemoveShareImage = 0x%x", usSearchResult);
		HmiMessage_Red_Back(szMsg);
		return -1;
	}

	m_ulStationID = 0;
	m_ulImageID = 0;

	if (DieIsAlignable(usDType) == TRUE)
	{
		m_stAutoDieOffset.x = stDOffset.x; 
		m_stAutoDieOffset.y = stDOffset.y; 
	}
	else
	{
		m_stAutoDieOffset.x = (PR_WORD)m_lPrCenterX; 
		m_stAutoDieOffset.y = (PR_WORD)m_lPrCenterY; 
	}

	//Log result into Postbond SPC
	if (!bPostBond)
	{
		return TRUE;
	}

	return (LogPostBondData(BPR_POSTBOND_RUNTIME, usDType, fDRotate, stDOffset));
}



