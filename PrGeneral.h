#pragma once

#include "Dpr_Ram.h"
#include "math.h"
#include <prheader.h>
#include <prmsg.h>
#include <prstatus.h>
#include <prtype.h>
#include <afxmt.h>

#define PR_WARNING_START	0x8000
#define PRS_PRNoErr(res)	((res == PR_ERR_NOERR) || ((PR_UWORD)res > PR_WARNING_START))
#define UP_LF_X PR_MAX_WIN_ULC_X						//(256)
#define UP_LF_Y PR_MAX_WIN_ULC_Y						//(256)
#define LO_RT_X PR_MAX_WIN_LRC_X						//(7936)
#define LO_RT_Y PR_MAX_WIN_LRC_Y						//(7936)

class CPrGeneral
{
#define F_OK						0
#define	MS899_GEN_SEND_ID			0x43
#define	MS899_GEN_RECV_ID			0xa3

#define DUMMY_PR_SEND_ID		MS899_GEN_SEND_ID
#define DUMMY_PR_RECEIVE_ID		MS899_GEN_RECV_ID

private:
	BOOL m_bUseMouse;
	BOOL m_bEnableMouseClickTriggered;
public:
//	pos_2D_REAL MouseClickPosn;
//	BOOL MouseClickTriggered;
//	BOOL MouseDragTriggered;

	BOOL m_bDisbaleMouseMove;

private:
	LONG PRDrawXCross(int x0, int y0, int color, int len);
public:
	CPrGeneral();
	virtual ~CPrGeneral();


	VOID DisableMouseClickTriggered() 
	{
		m_bEnableMouseClickTriggered = FALSE;
	};
	VOID EnableMouseClickTriggered() 
	{
		m_bEnableMouseClickTriggered = TRUE;
	};
	BOOL IsEnableMouseClickTriggered() 
	{
		return m_bEnableMouseClickTriggered;
	};
	LONG _round(double val);
	LONG PRClearScreenNoCursor(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID);
	LONG PRClearScreen(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const LONG lPRCenterX = 4096, const LONG lPRCenterY = 4096);
	LONG DrawHomeCursor(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const LONG lPRCenterX, const LONG lPRCenterY, CString &szErrMsg);
	LONG PRDrawCross(int x0, int y0, int color);
	LONG PRDrawSCross(int x0, int y0, int color, int len = 400);
	LONG PRDrawSXCross(int x0, int y0, int color, int len = 2048);
	void PRS_DrawCross(short x, short y, short color, short size);
	void PRS_EraseCross(short x, short y, short color, short size);
	LONG PRDrawLine1(LONG x0, LONG y0, LONG x1, LONG y1, int color, int lCamera);
	LONG PRDrawLine2(LONG x0, LONG y0, LONG x1, LONG y1, int color, int lCamera);

	PR_COORD *SetPointXY(PR_COORD *pco, short x, short y);
	PR_COORD *ShiftPoint(PR_COORD *pco, const PR_COORD *pcoShift);
	PR_COORD *NegShiftPoint(PR_COORD *pco, const PR_COORD *pcoShift);
	PR_WIN *ShiftWinXY(PR_WIN *pstWin, int x, int y);
	PR_WIN *ShiftWin(PR_WIN *pstWin, const PR_COORD *pcoShift);
	PR_WIN *NegShiftWin(PR_WIN *pstWin, const PR_COORD *pcoShift);
	BOOL IsPointInsideScreen(const PR_COORD *pco);
	BOOL IsWinInsideScreen(const PR_WIN *pstWin);
	BOOL PRS_IsPointInsidePrLimit(const PR_COORD *pco);
	PR_COORD *CheckPointInsidePrLimit(PR_COORD *pco);
	PR_COORD *CheckPointInScreen(PR_COORD *pco);
	PR_WIN *PRS_CheckWinInsidePrLimit(PR_WIN *pstWin);
	VOID CheckPRCoordLimit(PR_COORD &stCoord);
	VOID CheckPRCoordLimit(PR_COORD stCorner[], int nMaxCorner);
	VOID CheckPRCoordLimit(PR_WORD &lX, PR_WORD &lY);

	LONG wpr_draw_cross(PR_COORD pt, PR_COLOR color);
	LONG wpr_draw_Smallcross(PR_COORD pt, PR_COLOR color);

	void PRS_DrawTmpl(PR_COORD center, PR_SIZE size, short color);
//	void PRS_outtext(char *buf, short x, short y, short color);
	VOID PRS_clearline(short row);
	VOID PRS_DrawSearchRange(PR_WIN *pWin);

	LONG MouseDefineRegion(const PR_SHAPE_TYPE emShape = PR_SHAPE_TYPE_RECTANGLE, const PR_COLOR emColor = PR_COLOR_RED);
	LONG MouseSet2PointRegion(const PR_COORD coCorner1, const PR_COORD coCorner2, const PR_SHAPE_TYPE emShape = PR_SHAPE_TYPE_RECTANGLE, const PR_COLOR emColor = PR_COLOR_RED);
	LONG MouseGetRegion(PR_COORD &coCorner1, PR_COORD &coCorner2);
	LONG MouseGetRegion(PR_WORD *Corner1x, PR_WORD *Corner1y, PR_WORD *Corner2x, PR_WORD *Corner2y);
	LONG MouseGetAllCorners(PR_COORD acoObjCorner[PR_MAX_NO_OF_SHAPE_CORNER]);
	VOID EraseMouseCtrlObj();
	VOID DrawAndEraseCursor(PR_COORD stStartPos, unsigned char ucCorner, BOOL bDraw);
	VOID AdjCorner(long temp, PR_WORD step, PR_WORD &x, PR_WORD &y);
//	LONG AdjRectangleConer(BOOL bLeftTopRightLowerCorner, long temp, PR_COORD &ulc, PR_COORD &lrc);
//	LONG MoveRectange(long temp, PR_COORD &ulc, PR_COORD &lrc);
//	LONG AdjBoptCalibration(LONG temp);

	LONG UsePRMouse(PR_COORD stULCorner, PR_COORD stLRCorner);
	LONG PRMouseDrawComplete(PR_COORD &stULCorner, PR_COORD &stLRCorner);
	LONG DrawSearchRegion(int x0, int y0, int x1, int y1);
	LONG DrawSearchRegion(PR_WIN stWin);
	LONG EraseSearchRegion(LONG x0, LONG y0, LONG x1, LONG y1);
//	LONG PRDrawBox(PR_COORD stCorner[4], int color);
//	LONG PRDrawBox(PR_COORD coCorner[4], int nCornerNo, int color);
	LONG EraseSearchRegion(PR_WIN stWin);

	VOID SetPRCoord(PR_COORD &stCoord, const LONG lX, const LONG lY);
	VOID PRConvertDieCornersToWin(PR_WIN *pstWin, const PR_COORD acoDieCorners[]);
//	LONG PRDrawUpLeftConer(LONG x, LONG y, LONG length);
//	LONG PRDrawLowerRightConer(LONG x, LONG y, LONG length);
	LONG PRDrawLine(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const LONG x0, const LONG y0, const LONG x1, const LONG y1, const PR_COLOR emColor);
	LONG PRDrawLine(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const PR_COORD coPoint1, const PR_COORD coPoint2, const PR_COLOR emColor);
	LONG wpr_draw_box(PR_COORD ulc, PR_COORD lrc, int color);
	LONG PRDrawCircle(PR_COORD pt, PR_LENGTH radius);
	LONG wpr_draw_rect(PR_COORD ulc, PR_COORD lrc, PR_COLOR color, PR_UBYTE SID, PR_UBYTE RID);


	double fabs(double temp);
	LONG abs(long temp);

	LONG util_pr_outtext(const char *Str, int x, int y, int on);

	CString GetPrVersionString();
	CString GetPrITFVersionString();

	VOID ResetMouseClickData();
	LONG CaptureLiveVideoImage(char *szFileFullPathName, BOOL bCaptureGraphic);

	void PRS_clearscreen();
	void PRS_DrawLargestHomeCursor(short x, short y);
	VOID VerifyPRRegion(PR_WIN *stInputArea);

	LONG ResetLighting(short prid, PR_PURPOSE emPurpose);

	LONG DrawManualAlignSpeed(int speed);
	VOID DrawRectangle(long TmplUpperLeftX, long TmplUpperLeftY, long TmplLowerRightX, long TmplLowerRightY, BOOL bDraw);

	BOOL VerifyPRRegion(PR_WIN *stInputArea, const PR_WORD sPRWinULX, const PR_WORD sPRWinULY, const PR_WORD sPRWinLRX, const PR_WORD sPRWinLRY);
	BOOL VerifyPRPoint(PR_COORD *stPoint, const PR_WORD sPRWinULX, const PR_WORD sPRWinULY, const PR_WORD sPRWinLRX, const PR_WORD sPRWinLRY);

	LONG PRS_SensorZoom(const PR_CAMERA eCurCamera, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID,
						const PR_SET_VIDEO emType, const PR_ZOOM_SENSOR_MODE emZoomMode, 
						const PR_ZOOM_SENSOR_METHOD emZoomMethod, const PR_CONT_ZOOM_SENSOR_PARA stContZoomPar, 
						const PR_REAL_ROI stROI, PR_REAL_ROI *RpyROI, CString &szErrMsg);

	VOID DigitalZoomScreen(const short hZoomFactor, const PR_WORD sPRCenterX, const PR_WORD sPRCenterY, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID);
	VOID DrawSmallCursor(PR_UBYTE ubSID, PR_UBYTE ubRID, LONG CrossX, LONG CrossY);
	BOOL InitDisplayVideo(BOOL bOn, PR_UBYTE ubSID, PR_UBYTE ubRID, PR_PURPOSE emPurpose);
	BOOL PreEpoxyShow(const PR_COORD stULCorner, const PR_COORD stLRCorner, const PR_COORD prcenter,
					  const PR_UBYTE ubSID, const PR_UBYTE ubRID, const PR_PURPOSE emPurpose, const LONG lThreshold,
					  PR_PRE_EPOXY_SHOW_CMD &stEpoxyShowCmd, CString &szErrMsg);
	BOOL LearnEpoxyExt(const PR_COORD prcenter, const PR_PRE_EPOXY_SHOW_CMD stEpoxyShowCmd, const PR_UBYTE ubSID, const PR_UBYTE ubRID,
					   PR_UWORD &uwRecordID, PR_AREA &aeEpoxyArea, CString &szErrMsg);
	PR_WORD SearchEpoxyExt(const LONG lPRID, const DOUBLE dMinEpoxySize, const DOUBLE dMaxEpoxySize, const PR_COORD prcenter,
						   const BOOL bWholeAreaChk, const BOOL bWholeShapeChk, const BOOL bPositionChk,
						   const BOOL bDisplayGraphicInfo, const BOOL bLatch,
						   const PR_UBYTE ubSID, const PR_UBYTE ubRID, 
						   UINT &nX, UINT &nY, ULONG &ulEpoxyAreaSize, CEvent *pevGrabImageReady, CString &szErrMsg);

	VOID SetLighting(const PR_PURPOSE emPurpose, 
					 const PR_SOURCE emCoaxID, const PR_SOURCE emRingID, const PR_SOURCE emSideID,
					 const PR_UWORD uwCoaxLevel, const PR_UWORD uwRingLevel,const PR_UWORD uwSideLevel,
					 const PR_UBYTE ubSID, const PR_UBYTE ubRID);
	BOOL SetExposureTime(const PR_PURPOSE emPurpose, const PR_EXPOSURE_TIME stTime, const PR_UBYTE ubSID, const PR_UBYTE ubRID, CString &szErrMsg);
	BOOL GetExposureTime(const PR_PURPOSE emPurpose, const PR_UBYTE ubSID, const PR_UBYTE ubRID, PR_EXPOSURE_TIME &stTime, CString &szErrMsg);

	VOID FreePRRecord(LONG &lPRID, const PR_UBYTE ubSID, const PR_UBYTE ubRID);
	VOID FreePRRecord(PR_UWORD &uwRecordID, const PR_UBYTE ubSID, const PR_UBYTE ubRID);
	VOID FreeAllPRRecord(const PR_UBYTE ubSID, const PR_UBYTE ubRID);
	PR_UWORD UploadPRRecord(const PR_ULWORD ulRecordID, CHAR acFilename[PR_MAX_FILE_PATH_LENGTH], const PR_UBYTE ubSID, const PR_UBYTE ubRID, CString &szErrMsg);
	PR_ULWORD DownloadPRRecord(const PR_ULWORD ulRecordID, CHAR acTemp[PR_MAX_FILE_NAME_LENGTH], const PR_UBYTE ubSID, const PR_UBYTE ubRID, CString &szErrMsg);
	PR_UWORD DisplayLearnInfo(const LONG lPrID, const LONG lPrCenterX, const LONG lPrCenterY, const PR_UBYTE ubSID, const PR_UBYTE ubRID);
	LONG ExtractCircleObject(PR_UWORD lPRID, PR_UBYTE sid, PR_UBYTE rid, PR_PURPOSE emPurpose, PR_REAL rPassScorePer, 
		 PR_RCOORD rcoDieCentre, PR_LENGTH len_x, PR_LENGTH len_y, PR_RCOORD &rcoResultDieCentre);
	LONG ExtractBlobObject(PR_UWORD lPRID, PR_UBYTE sid, PR_UBYTE rid, PR_PURPOSE emPurpose, PR_REAL rPassScorePer, 
		 PR_RCOORD rcoDieCentre, PR_LENGTH len_x, PR_LENGTH len_y, PR_RCOORD &rcoResultDieCentre);
	PR_UWORD RemoveShareImage(PR_UBYTE ubSID, PR_UBYTE ubRID, PR_ULWORD ulImageID, PR_ULWORD ulStationID);
};