#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"
#include "CycleState.h"
#include "WPR_Log.h"
#include "GallerySearchController.h"
#include "LastScanPosition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// PreScan used for table continueous moving and grabbing
#if 0
//==========================================================================//
//	Vision diebond On-the-fly (OTF)
//==========================================================================//

// OTF Function Index 
typedef enum {  
	PR_OTF_FUNC_NULL, 
	PR_OTF_FUNC_SYN_ENCODER,				//	[1] PR_OTFSynEncoder
	PR_OTF_FUNC_SET_IMG_LENGTH,				//	[2] PR_SetOTFCmd
	PR_OTF_FUNC_SET_TRIG_POS,				//	[3] PR_SetOTFTrigPos
	PR_OTF_FUNC_ENABLE,						//	[4] PR_EnableOTF
	PR_OTF_FUNC_DISABLE,					//	[5] PR_DisableOTF
	PR_OTF_FUNC_GET_NEXT_TRIG_POS_INDEX,	//	[6] PR_GetOTFNextTrigPosIndex
	PR_OTF_FUNC_END
} PR_OTF_FUNC;

// SynEncoder 
typedef struct {
	PR_CAMERA			emCamera;
	PR_OTF_SYN_MODE		emOTFSynMode;
} PR_OTF_SYN_ENCODER_CMD;

// SetOtfImageLength
typedef struct {
	PR_ULWORD		ulImageListLength;
} PR_SET_OTF_CMD;


#define PR_OTF_FIELD_NUM_OF_PR_OTF_TRIG_POS  (4)

// SetTrigPos
typedef struct {
	PR_UWORD			uwRecordID;
	PR_OTF_TRIG_MODE	emOTFTrigMode;
	PR_ULWORD			ulPosX;
	PR_ULWORD			ulPosY;
} PR_OTF_TRIG_POS;

typedef struct {
	PR_ULWORD			ulNumOfTrigPos;

	PR_OTF_TRIG_POS		astOTFTrigPos[PR_MAX_OTF_TRIG_POS];
} PR_SET_OTF_TRIG_POS_CMD;

// OTF Specific Reply 
typedef struct {
	PR_COMMON_RPY		stStatus;
	PR_ULWORD			ulNextTrigPosIndex;
} PR_GET_OTF_TRIG_INDEX_RPY;

// General Command 
typedef struct {
	PR_OTF_FUNC				emOtfFuncIndex;
	PR_OTF_SYN_ENCODER_CMD	stOtfSynEncoder;
	PR_SET_OTF_TRIG_POS_CMD	stOtfTrigPos;
	PR_SET_OTF_CMD			stOtfSet;
}PR_OTF_GENERAL_CMD;

// General Reply
typedef PR_GET_OTF_TRIG_INDEX_RPY	PR_OTF_GEN_RPY;


/* Init OTF Command	*/
PR_VVOID	PR_InitOTFSynEncoderCmd(	PR_OTF_SYN_ENCODER_CMD	*pstCmd);
PR_VVOID	PR_InitOtfTrigPosCmd (		PR_SET_OTF_TRIG_POS_CMD	*pstCmd );
PR_VVOID	PR_InitSetOTFCmd(			PR_SET_OTF_CMD			*pstCmd);


/* OTF Sub-Functions */
PR_VVOID	PR_OTFSynEncoder(	PR_OTF_SYN_ENCODER_CMD	*pstCmd,
								PR_UBYTE				ubSenderID,
								PR_UBYTE				ubReceiverID,
								PR_COMMON_RPY			*pstRpy);

PR_VVOID	PR_OTFSetCmd (		PR_SET_OTF_CMD			*pstCmd,
								PR_UBYTE				ubSenderID,
								PR_UBYTE				ubReceiverID,
								PR_COMMON_RPY			*pstRpy);

//	MUST sure live video is OFF before PR_EnableOTF reply
PR_VVOID	PR_OTFRunStart (	PR_UBYTE				ubSenderID,
								PR_UBYTE				ubReceiverID,
								PR_COMMON_RPY			*pstRpy);

PR_VVOID	PR_OTFRunEnd(		PR_UBYTE				ubSenderID,
								PR_UBYTE				ubReceiverID,
								PR_COMMON_RPY			*pstRpy);

PR_VVOID	PR_OTFSetTrigPos (	PR_SET_OTF_TRIG_POS_CMD	*pstCmd,
								PR_UBYTE				ubSenderID,
								PR_UBYTE				ubReceiverID,
								PR_COMMON_RPY			*pstRpy);

PR_VVOID	PR_OTFGetNextTrigPosIndex (	PR_UBYTE		ubSenderID,
										PR_UBYTE					ubReceiverID,
										PR_GET_OTF_TRIG_INDEX_RPY	*pstRpy);

PR_VVOID	PR_OTFGeneralCmd (	PR_OTF_GENERAL_CMD		*pstCmd,
								PR_UBYTE				ubSenderID,
								PR_UBYTE				ubReceiverID,
								PR_OTF_GEN_RPY			*pstRpy );

Please check for the sample codes for the On The Fly, 
the host interface is a debug version, as we still need to have some minor fixation on OTHER functions.
For the ．get image・ command, I will update you tomorrow


[Step 1] 

Name - Synchronize Encoder (Home Encoder)

Function:

	Vision will set the motor count reference as 0x800000 (HEX) 
	(relative to the current absolute motor count (by HiPEC) )

Example:         

// OTF Sync Encoder
LONG CBondOptics::OnTheFlySyncEncoder()
{
	PR_UBYTE	ubSenderID	 =  BOND_PR_SEND_ID;
	PR_UBYTE	ubReceiverID =  BOND_PR_RECEIVE_ID;

	PR_OTF_SYN_ENCODER_CMD  stCmd;
	PR_COMMON_RPY           stRpy;

	PR_InitOTFSynEncoderCmd (&stCmd);

	stCmd.emCamera		= (PR_CAMERA)BPR_CAM_NUM;
	stCmd.emOTFSynMode	= PR_OTF_SYN_MODE_ALL;

	// Reset OTF Encoder
	PR_OTFSynEncoder ( &stCmd , ubSenderID, ubReceiverID, &stRpy);

	if ( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0)
	{
		CString str;
		str.Format("Sync Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwCommunStatus);
		DisplayMessage(str);
		return PR_ERR_OTF_SYNC_ERROR;
	}

	// Note Down Current Cmd Count
	GANTRY_GetVirXGlobalPosition(&xEncPosOTFInitial);
	GANTRY_GetVirYGlobalPosition(&yEncPosOTFInitial);

	CString str;
	str.Format("Cur Vis %d %d", xEncPosOTFInitial, yEncPosOTFInitial);
	DisplayMessage(str);

	PR_GET_OTF_CURRENT_MOTOR_COUNT_RPY  pstRpy;
	PR_OTFGetCurrentMotorCountCmd (ubSenderID,ubReceiverID,&pstRpy);

	str.Format("Cur Vis %d %d", pstRpy.stCurrentMotorCount.ulCounterX, pstRpy.stCurrentMotorCount.ulCounterY);
	DisplayMessage(str);

	return 0;
}
	
[Step 2] 

Name Set Trigger Position

Function:

	User input the trigger position relative to 0x800000 (HEX) for grabbing 

// OTF Set Trigger Position
LONG CBondOptics::OnTheFlySetTriggerPosition(LONG lNumOfTrigPos, PR_UWORD recordID, LONG* xPosList, LONG* yPosList, LONG xInitialPosn, LONG yInitialPosn)
{
	/*
	//	439272
	#define		MOTOR_COUNT_MAX			(8388608)			// 0x800000 (HEX)
	#define		MOTOR_COUNT_MIN			(7949336)			// 0x794C18 (HEX)
	#define		MOTOR_COUNT_PER_STEP	(4096)				// 0x1000 (HEX)

	PR_REAL           rTimeForEachMotorCount = 0.00000667;

	About Vision . 

	[Camera]
	60 frame          32 micro * (480 + overhead) = 16 ms (image transfer time)
	90 frame          21 micro * (480 + overhead) = 11 ms (image transfer time)

	[Delay Exposure ]

	60 frame => 32 ( 6 ) = 192 micro sec
	90 frame => 21 ( 6 ) = 126 micro sec

	[Total Time]

	Time for expected Motor Count Interval > Delay Exposure + Expo + Image Transfer Time 

	rTimeForEachMotorCount * MinMotorCount  > 0.000192 + 0.000300 + 0.016 

	MinMotorCount > 2549
	PR_ULWORD    ulNumOfStep = 100;
	// Expected Number of Steps 
	PR_UWORD      uwRecordID = 0;
	PR_WORD        wNumOfTrigPos = 0;
	PR_WORD        i = 0;
	PR_ULWORD    ulHardwareStart = MOTOR_COUNT_MAX - 1000;
	// Offset 1000 for margin, prevent over-shoot 
	PR_ULWORD    ulHardwareEnd  = MOTOR_COUNT_MIN + 1000;
	// Offset 1000 for margin, prevent over-shoot
	PR_REAL           rMinMotorCountInterval = (0.000192 + 0.000300 + 0.016) / rTimeForEachMotorCount ;
	*/

	PR_UBYTE	ubSenderID	 =  BOND_PR_SEND_ID;
	PR_UBYTE	ubReceiverID =  BOND_PR_RECEIVE_ID;
	PR_ULWORD	uwStartEnc	 =  (PR_ULWORD) 0x800000;

	PR_SET_OTF_TRIG_POS_CMD	stCmd;
	PR_COMMON_RPY	stRpy;

	PR_InitOtfTrigPosCmd ( &stCmd );

	for ( int i = 0; i < lNumOfTrigPos; i ++ )
	{
		stCmd.astOTFTrigPos[i].uwRecordID   = (PR_UWORD) recordID;
		stCmd.astOTFTrigPos[i].emOTFTrigMode = PR_OTF_TRIG_MODE_X;

		stCmd.astOTFTrigPos[i].ulPosX = (PR_ULWORD) ( uwStartEnc + xPosList[i] - xInitialPosn );
		stCmd.astOTFTrigPos[i].ulPosY = (PR_ULWORD) ( uwStartEnc + yPosList[i] - yInitialPosn );

		//CString str; str.Format("%d %d %d",xPosList[i],yPosList[i],i);
		//DisplayMessage(str);

		stCmd.ulNumOfTrigPos ++ ;
	}
	/*
	uwRecordID = 2;                      // for Lighting 
	wNumOfTrigPos = 100;
	PR_ULWORD    ulInterval = floor ( ( ulHardwareStart - ulHardwareEnd )/ (wNumOfStep) );
	ulInterval = max ( ulInterval, rMinMotorCountInterval);
	for ( i = 0; i < wNumOfTrigPos; i ++ )
	{
		// Since Motor Count of Start is larger than End
		if ( (ulHardwareStart - (i)*ulInterval ) <= ulHardwareEnd )
		{
			assert ( 0 );                                                                                           // Error Handling !!! 
			break;
		}

		stCmd.astOTFTrigPos[i].uwRecordID   = uwRecordID;
		// for lighting 
		stCmd.astOTFTrigPos[i].emOTFTrigMode = PR_OTF_TRIG_MODE_X;
		// X-Direction Trigger 
		stCmd.astOTFTrigPos[i].ulPosX = ulHardwareStart - (i)*ulInterval;
	}
	*/

	PR_OTFSetTrigPos ( &stCmd , ubSenderID, ubReceiverID, &stRpy);

	if ( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0)
	{
		CString str;
		str.Format("Trigger Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwCommunStatus);
		DisplayMessage(str);
		return PR_ERR_OTF_TRIGGER_ERROR;
	}

	return 0;
}


	

[Step 3] 

Name  Start On The Fly Grabbing 

Function:

	Vision will Enable the motor count triggering, 
	when the motor count reach the expected position, image would be grabbed.

Remarks:

	There would be time-out(around few seconds), 
	please move the motor immediately right after the function is returned!!!

Example:         

// OTF Start
LONG CBondOptics::OnTheFlyStart()
{
	PR_UBYTE	ubSenderID	 =  BOND_PR_SEND_ID;
	PR_UBYTE	ubReceiverID =  BOND_PR_RECEIVE_ID;

	PR_COMMON_RPY                   stRpy;

	PR_OTFRunStart ( ubSenderID, ubReceiverID, &stRpy );

	if ( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0)
	{
		CString str;
		str.Format("OTF Start Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwPRStatus);
		DisplayMessage(str);
		return PR_ERR_OTF_START_ERROR;
	}

	/*
	 PR_OTFStartGetMcLog(ubSenderID,ubReceiverID,&stRpy);

	 if( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0) {
		CString str;
		str.Format("OTF Start Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwCommunStatus);
		DisplayMessage(str);
		return PR_ERR_OTF_START_ERROR;
	}
	*/

	return 0;
}



[Step 4] 

Name Stop On The Fly Grabbing 
	

Function:

	Vision will DISABLE and RESET the motor count triggering, 
	even when a new trigger position is reached.
	No action would be done.
	It can also be used as ．RESET・ function.


Example:

// OTF End
LONG CBondOptics::OnTheFlyEnd()
{
	PR_UBYTE	ubSenderID	 =  BOND_PR_SEND_ID;
	PR_UBYTE	ubReceiverID =  BOND_PR_RECEIVE_ID;

	PR_COMMON_RPY                   stRpy;

	/*
		PR_OTFEndGetMcLog(ubSenderID,ubReceiverID,&stRpy);

		if( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0)
		{
			CString str;
			str.Format("OTF End Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwCommunStatus);
			DisplayMessage(str);
			return PR_ERR_OTF_END_ERROR;
		}
	*/
	PR_OTFRunEnd ( ubSenderID, ubReceiverID, &stRpy );

	if ( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0)
	{
		CString str;
		str.Format("OTF End Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwCommunStatus);
		DisplayMessage(str);
		return PR_ERR_OTF_END_ERROR;
	}

	/*
			PR_GET_OTF_CURRENT_MOTOR_COUNT_RPY  pstRpy;
	PR_OTFGetCurrentMotorCountCmd (ubSenderID,ubReceiverID,&pstRpy);

	CString str;
	str.Format("Cur Vis %d %d", pstRpy.ulPosX, pstRpy.ulPosY);
	DisplayMessage(str);

	*/

	// disable fast mode
	theApp.m_pPrFunc->wpr_fast_mode_disable();

	return 0;
}

								
LONG CBondOptics::OnTheFlyTest(IPC_CServiceMessage& svMsg)
{
	LONG nErr = 0;
	CStringList List;
	LONG lSelIndex = 0;

	List.AddTail("On the fly testing");
	List.AddTail("Log data to file");
	List.AddTail("Vision Encoder XY");
	List.AddTail("Linear Move X");
	List.AddTail("On the Fly End");
	lSelIndex = ed_hmi_selection("Selection", "OTF Selection", List);

	if (lSelIndex == 0)
	{
		if (nErr = OnTheFlyTest())
			ed_err_displ(ED_PR, ED_TOKEN_ERROR,nErr,TRUE);
	}
	else if (lSelIndex == 1)
	{
		FILE *fp;
		if ( (fp=fopen("OnTheFly.txt", "wt") ) != NULL )
		{
			LONG lNumOfTrigPos = lf.NumOfUnit * lf.NumOfCol;
			LONG lNumOfOTF = lf.NumOfRow;

			if (lNumOfTrigPos <= 0)
				return 0;
			else if (lNumOfTrigPos > 1000)
				lNumOfTrigPos = 1000;

			for (int j=0; j<lNumOfOTF; j++)
			{
				BOOL rev = (j % 2 == 1);

				for (int i=0; i<lNumOfTrigPos; i++)
				{

					LONG curUnitNum = 0;
					LONG curColNum = 0;

					pos_2D prPoint = OnTheFlyGenPoint(i, j, rev, &curUnitNum, &curColNum);

					if (bOption.AlignMode != ALIGN_BY_DISABLE_PR)
						fprintf(fp,"Unit %d Pad %d X %d Y %d\n", curUnitNum,curColNum,PRData[curUnitNum][curColNum].xoffset + BHPosnX[curUnitNum] + BondPosnX[curColNum] ,PRData[curUnitNum][curColNum].yoffset + BHPosnY[curUnitNum] + BondPosnY[curColNum]);
				}
			}
			fclose(fp);
		}
	}
	else if (lSelIndex == 2)
	{

		LONG result = OneTheFlyGetEncoderCompare();

		if (result)
			DisplayMessage("Compare Return 1");
		else
			DisplayMessage("Compare Return 0");
	}
	else if (lSelIndex == 3)
	{
		LONG relDist = (LONG)(60 * cGantryMM);
		LONG absDist;
		if (bDly.CurXPosn - relDist < 0)
		{
			absDist = bDly.CurXPosn + relDist;
		}
		else
		{
			absDist = bDly.CurXPosn - relDist;
		}

		LONG speed = 2500;

		if (nErr = theApp.BndHeadXYLinearMoveToLite(absDist,0,speed,SFM_NOWAIT))
		{
			ed_err_displ(ED_BOND_HEAD,ED_TOKEN_ERROR,nErr,TRUE);
			return nErr;
		}
	}
	else if (lSelIndex == 4)
	{
		OnTheFlyEnd();
	}

	return 1;
}

pos_2D CBondOptics::OnTheFlyGenPoint(LONG lIndex, LONG lRowNum, BOOL bRev, LONG *curUnitNum, LONG *curColNum)
{
	pos_2D posPR;

	if (bRev)
		lIndex = lf.NumOfUnit * lf.NumOfCol - lIndex - 1;

	LONG lUnitNum = 0;
	LONG lColNum =  0;
	if (lf.NumOfCol > 0)
	{
		lUnitNum = lIndex/lf.NumOfCol;
		lColNum = lIndex % lf.NumOfCol;
	}
	LONG lPadNum = lRowNum + lf.NumOfRow * lColNum;

	if (lColNum % 2)
		lPadNum = ( lf.NumOfRow - lRowNum - 1 ) + lf.NumOfRow * lColNum;

	if (bOption.AlignMode != ALIGN_BY_DISABLE_PR)
	{
		posPR.x_pos = BHPosnX[lUnitNum] + BondPosnX[lPadNum];
		posPR.y_pos = BHPosnY[lUnitNum] + BondPosnY[lPadNum];
	}
	else
	{
		posPR.x_pos = LFDelta.firstPad.x_pos;
		posPR.y_pos = LFDelta.firstPad.y_pos;
	}

	*curUnitNum = lUnitNum;
	*curColNum  = lPadNum;

	return posPR;
}

// OTF Get Encoder
LONG CBondOptics::OneTheFlyGetEncoderCompare()
{
	PR_GET_OTF_CURRENT_MOTOR_COUNT_RPY  pstRpy;

	PR_UBYTE	ubSenderID	 =  BOND_PR_SEND_ID;
	PR_UBYTE	ubReceiverID =  BOND_PR_RECEIVE_ID;
	PR_ULWORD	uwStartEnc	 =  (PR_ULWORD) 0x800000;

	PR_OTFGetCurrentMotorCountCmd (ubSenderID, ubReceiverID,&pstRpy);

	LONG diff = 0;
	diff = uwStartEnc - pstRpy.stCurrentMotorCount.ulCounterY;
	pstRpy.stCurrentMotorCount.ulCounterY += 2 * diff;

	pstRpy.stCurrentMotorCount.ulCounterX -= uwStartEnc;
	pstRpy.stCurrentMotorCount.ulCounterY -= uwStartEnc;

	int xGloPos = 0;
	int yGloPos = 0;

	GANTRY_GetVirXGlobalPosition(&xGloPos);
	GANTRY_GetVirYGlobalPosition(&yGloPos);

	pstRpy.stCurrentMotorCount.ulCounterX -= xGloPos;
	pstRpy.stCurrentMotorCount.ulCounterY -= yGloPos;

	LONG xCnt = pstRpy.stCurrentMotorCount.ulCounterX;
	LONG yCnt = pstRpy.stCurrentMotorCount.ulCounterY;

	CString str;
	str.Format("Vision Enc %d %d", xCnt, yCnt);
	DisplayMessage(str);

	if ( xCnt < -5 || xCnt > 5 )
		return 1;

	if ( yCnt < -5 || yCnt > 5 )
		return 1;

	return 0;
}

// Latch From Current Gallery
LONG CBondOptics::OnTheFlyAlign(LONG lCurUnit, LONG lCurCol, PR_UWORD recordID, LONG xInitialPosn, LONG yInitialPosn, LONG offsetY, LONG *targetX, LONG *targetY)
{
	PR_UBYTE	ubSenderID	 =  BOND_PR_SEND_ID;
	PR_UBYTE	ubReceiverID =  BOND_PR_RECEIVE_ID;
	PR_WORD      wNumOfTrigPos = 0;
	PR_ULWORD	uwStartEnc	 =  (PR_ULWORD) 0x800000;
	LONG nErr = 0;

	PR_GRAB_SHARE_IMAGE_CMD      stCmd;
	PR_GRAB_SHARE_IMAGE_RPY      stRpy;
	PR_GRAB_SHARE_IMAGE_RPY2     stRpy2;

	stCmd.emOnTheFlyGrab = PR_TRUE;
	stCmd.uwRecordID = recordID;
	stCmd.emGrabFromRecord = PR_TRUE;
	stCmd.emPurpose = PR_PRE_LF_ALIGN;

	PR_GrabShareImgCmd ( &stCmd, ubSenderID, ubReceiverID, &stRpy );

	if ( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0)
	{
		CString str;
		str.Format("OTF Grab1 Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwCommunStatus);
		DisplayMessage(str);
		return PR_ERR_OTF_GRAB_ERROR;
	}

	PR_GrabShareImgRpy ( ubSenderID,  &stRpy2 );

	if ( stRpy.uwCommunStatus != 0 || stRpy.uwPRStatus != 0)
	{
		CString str;
		str.Format("OTF Grab2 Commun/PR Status %d %d \n" ,stRpy.uwCommunStatus, stRpy.uwCommunStatus);
		DisplayMessage(str);
		return PR_ERR_OTF_GRAB_ERROR;
	}

	// calculate X Start End in terms of Gantry Enc
	LONG xStart = stRpy2.stOtfGrabResult.ulStartX - uwStartEnc + xInitialPosn;
	LONG xEnd = stRpy2.stOtfGrabResult.ulEndX - uwStartEnc + xInitialPosn;

	// Force Start Exposure = End
	//xStart = xEnd;

	// Adjust Y1 Motor Direction (Opposite to OTF)
	LONG diff = 0;
	diff = uwStartEnc - stRpy2.stOtfGrabResult.ulStartY;
	stRpy2.stOtfGrabResult.ulStartY += 2 * diff;
	diff = uwStartEnc - stRpy2.stOtfGrabResult.ulEndY;
	stRpy2.stOtfGrabResult.ulEndY += 2 * diff;

	// calculate Y Start End in terms of Gantry Enc
	LONG yStart = stRpy2.stOtfGrabResult.ulStartY - uwStartEnc + yInitialPosn;
	LONG yEnd = stRpy2.stOtfGrabResult.ulEndY	- uwStartEnc + yInitialPosn;

	LONG yShift = (LONG)((offsetY+0.0)/cGantryMM/(BPR_CFACTOR+0.0)+0.5);


	// Search Template
	if (nErr = LFAlignCompensation(0,yShift,1,1,stRpy2.ulImageID,stRpy2.ulStationID))
	{
		LF_Rec.Offset.x_pos = 0;
		LF_Rec.Offset.y_pos = 0;
		PRData[lCurUnit][lCurCol].skip = true;
		return 0;
	}

	PRData[lCurUnit][lCurCol].skip = false;

	// std Gantry Bond Posn Array

	LONG xStd = 0;
	LONG yStd = 0;

	if ( bOption.AlignMode == ALIGN_BY_DISABLE_PR )
	{
		xStd = LFDelta.firstPad.x_pos;
		yStd = LFDelta.firstPad.y_pos;
	}
	else
	{
		xStd = BHPosnX[lCurUnit] + BondPosnX[lCurCol];
		yStd = BHPosnY[lCurUnit] + BondPosnY[lCurCol];
	}

//	Useless
//	LONG overShootX = (LONG)((xStart + xEnd) / 2 + 0.5) - xStd;
//	LONG overShootY = (LONG)(slope * overShootX + 0.5);

	// final = mid exposure point
	LONG xFinal = RoundValue((xStart + xEnd + 0.0)/2) + LF_Rec.Offset.x_pos;
	LONG yFinal = RoundValue((yStart + yEnd + 0.0)/2) + LF_Rec.Offset.y_pos;

	// store PRData for bonding

	PRData[lCurUnit][lCurCol].xoffset = xFinal - xStd;
	PRData[lCurUnit][lCurCol].yoffset = yFinal - yStd;
	PRData[lCurUnit][lCurCol].skip = false;

	if (abs(PRData[lCurUnit][lCurCol].xoffset) < cGantryMM*BPR_FOV && abs(PRData[lCurUnit][lCurCol].yoffset) < cGantryMM*BPR_FOV) {
		// Note down final Enc for testing
		*targetX = xFinal;
		*targetY = yFinal;
	}
	else {
		PRData[lCurUnit][lCurCol].xoffset = 0;
		PRData[lCurUnit][lCurCol].yoffset = 0;
		PRData[lCurUnit][lCurCol].skip = true;
		*targetX = xStd;
		*targetY = yStd;
		return 0;
	}
	//CString str;
	//str.Format("Grab Result [Index = %d], [X: %d - %d] %d, [Y: %d - %d] %d\n",
	//												stRpy2.stOtfGrabResult.lwIndex,
	//												xStart, xEnd, xEnd-xStart,
	//												yStart, yEnd, yEnd-yStart);
	//DisplayMessage(str);

	// Print Offset On Screen
	if (SysTask.OpMode == IDLE ) {
		char str2[50];
		sprintf(str2,"%s",AlignAlgString(LF_Rec.Circle, LF_Rec.lAlignAlg));
		theApp.m_pPrFunc->util_pr_outtext(str2,0,14,1);
		sprintf(str2,"OTF X:%3d Y:%3d T:%.3f",PRData[lCurUnit][lCurCol].xoffset,PRData[lCurUnit][lCurCol].yoffset,Wfr2.bFinalPickErrT);
		theApp.m_pPrFunc->util_pr_outtext(str2,0,15,1);
	}

	return 0;
}

LONG CBondOptics::OnTheFlyTest()
{
	LONG nErr = 0;

	try
	{
		OnTheFlySyncEncoder();

		// Turn to Bond Camera
		theApp.m_pPrFunc->ShowBondCamera();

		// Num of Trigger Points
		LONG lNumOfTrigPos = lf.NumOfUnit * lf.NumOfCol;
		// Num of OTF
		LONG lNumOfOTF = lf.NumOfRow;

		// Bound Check for Num of Trigger Point
		if (lNumOfTrigPos <= 0)
			return 0;
		else if (lNumOfTrigPos > 1000)
			lNumOfTrigPos = 1000;

		// storage arrays
		LONG xlist[1000];
		LONG ylist[1000];
		LONG unit[1000];
		LONG pad[1000];
		LONG xOTFlist[1000];
		LONG yOTFlist[1000];
		LONG w=0;

		// loop through OTF
		for (int j=0; j<lNumOfOTF; j++)
		{
			BOOL rev = (j % 2 == 1);

			// Generate Trigger Points
			for (int i=0; i<lNumOfTrigPos; i++) 
			{
				LONG curUnitNum = 0;
				LONG curColNum = 0;

				pos_2D prPoint = OnTheFlyGenPoint(i, j, rev, &curUnitNum, &curColNum);
				xlist[i] = prPoint.x_pos;
				ylist[i] = prPoint.y_pos;
				unit[i] = curUnitNum;
				pad[i] = curColNum;
			}

			// calculate end points
			LONG firstPadX = xlist[0] + cGantry10MM;
			LONG lastUnitPadX = xlist[lNumOfTrigPos-1] - cGantry10MM;
			LONG lastUnitPadY = ylist[lNumOfTrigPos-1];
			LONG commonY = (ylist[0] + ylist[lNumOfTrigPos-1])/2;

			// reverse scanning
			if (rev)
			{
				firstPadX = xlist[0] - cGantry10MM;
				lastUnitPadX = xlist[lNumOfTrigPos-1] + cGantry10MM;
			}

			//double slope = (ylist[0] - ylist[lNumOfTrigPos-1] + 0.0)/(xlist[0] - xlist[lNumOfTrigPos-1]);

			// move to start pos
			if (nErr = theApp.BndHeadXYMoveToCal(&firstPadX,&commonY,SFM_WAIT))
				return nErr;

			// Update Bond ID to LF_Rec.Num
			theApp.UpdateBondID(BPR_FIRST_LEVEL,0,1);

			// assign trigger points
			if (nErr =  OnTheFlySetTriggerPosition(lNumOfTrigPos, (PR_UWORD)LF_Rec.Num, xlist, ylist, xEncPosOTFInitial, yEncPosOTFInitial))
				return nErr;

			// enable fast mode for OTF
			if (j == 0)
			{
				theApp.m_pPrFunc->wpr_fast_mode_enable();
			}

			// start on the fly
			if (nErr =  OnTheFlyStart())
			{
				OnTheFlyEnd();
				return nErr;
			}

			LONG speed = 2500;

			// Move Motor with Linear Move
			if (nErr = theApp.BndHeadXYLinearMoveToLite(lastUnitPadX,commonY,speed,SFM_NOWAIT))
			{
				OnTheFlyEnd();
				return nErr;
			}

			for (int i=0; i<lNumOfTrigPos; i++)
			{
				// Align each OTF Image
				if (nErr = OnTheFlyAlign(unit[i], pad[i], (PR_UWORD)LF_Rec.Num, xEncPosOTFInitial, yEncPosOTFInitial, commonY-ylist[i],&xOTFlist[w],&yOTFlist[w])) {
					OnTheFlyEnd();
					return nErr;
				}
				w++; // record total number of points
			}
		}

		// end on the fly
		if (nErr =  OnTheFlyEnd())
		{
			return nErr;
		}

		// loop through each point for alignment verify
		/*
		for (int z=0; z<w; z++) {
			if (nErr = theApp.BndHeadXYMoveToCal(&xOTFlist[z],&yOTFlist[z],SFM_WAIT)) {
				return nErr;
			}
			Sleep(100);
			if (nErr = LFAlignCompensation(0,0,1)) {
				// ignore errors
			}
		}
		*/
	}
	catch (...)
	{
		return PR_ERR_OTF_COM_ERROR;
	}

	return 0;
}

#endif