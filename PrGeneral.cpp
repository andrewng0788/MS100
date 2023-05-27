#include "stdafx.h"
#include "prGeneral.h"
#include <mbstring.h>
#include <io.h>
#include <math.h>
#include "TakeTime.h"
#include "MathFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPrGeneral::CPrGeneral()
{
	m_bUseMouse = FALSE;
	m_bEnableMouseClickTriggered = TRUE;

//	MouseClickTriggered = FALSE;
//	MouseClickPosn.x_pos = 0;
//	MouseClickPosn.y_pos = 0;
//	MouseDragTriggered = FALSE;

	m_bDisbaleMouseMove = FALSE;
}

CPrGeneral::~CPrGeneral()
{
}

LONG CPrGeneral::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}

LONG CPrGeneral::PRClearScreenNoCursor(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID)
{
	PR_CLEAR_SCREEN_RPY		 rpy;
	PR_ClearScreenCmd(ubSenderID, ubReceiverID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}

LONG CPrGeneral::PRClearScreen(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const LONG lPRCenterX, const LONG lPRCenterY)
{
	PR_CLEAR_SCREEN_RPY		 rpy;
	PR_ClearScreenCmd(ubSenderID, ubReceiverID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	CString szErrMsg;
	DrawHomeCursor(ubSenderID, ubReceiverID, lPRCenterX, lPRCenterY, szErrMsg);
	return 0;
}


LONG CPrGeneral::DrawHomeCursor(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const LONG lPRCenterX, const LONG lPRCenterY, CString &szErrMsg)
{
	PR_DRAW_HOME_CURSOR_CMD cmd;
	PR_DRAW_HOME_CURSOR_RPY rpy;

	cmd.emTmplSize = PR_TMPL_SIZE_0;

	cmd.coPosition.x = (PR_WORD)lPRCenterX;
	cmd.coPosition.y = (PR_WORD)lPRCenterY;
	PR_DrawHomeCursorCmd(&cmd, ubSenderID, ubReceiverID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	if (rpy.uwCommunStatus != PR_COMM_NOERR )
	{
		szErrMsg.Format("PR draw home cursor, cmd error %d", rpy.uwCommunStatus);
		return rpy.uwCommunStatus;
	}
	
	if (PR_ERROR_STATUS(rpy.uwPRStatus))
	{
		szErrMsg.Format("PR draw home cursor, status error %d", rpy.uwPRStatus);
		return rpy.uwPRStatus;
	}

	return 0;			
}

LONG CPrGeneral::PRDrawCross(int x0, int y0, int color)
{
	int len = 400;
	PRDrawLine1(x0 - len, y0, x0 + len, y0, color, 0);
	PRDrawLine1(x0, y0 - len, x0, y0 + len, color, 0);
	return 0;
}

LONG CPrGeneral::PRDrawSCross(int x0, int y0, int color, int len)
{
	PRDrawLine2(x0 - len, y0, x0 + len, y0, color, 0);
	PRDrawLine2(x0, y0 - len, x0, y0 + len, color, 0);
	return 0;
}

// 20120713 Yip: Add Function For Draw X-Shape Cross in PR Screen
LONG CPrGeneral::PRDrawXCross(int x0, int y0, int color, int len)
{
	LONG x[4], y[4];
	x[0] = x[2] = x0 - len;
	x[1] = x[3] = x0 + len;
	y[0] = y[1] = y0 - len;
	y[2] = y[3] = y0 + len;

	LONG offset = 0;
	if (x[0] < UP_LF_X) 
	{
		offset = UP_LF_X - x[0];
		x[0] += offset;	
		y[0] += offset;
		x[2] += offset;	
		y[2] -= offset;
	}
	if (x[1] > LO_RT_X) 
	{
		offset = LO_RT_X - x[1];
		x[1] += offset;	
		y[1] -= offset;
		x[3] += offset;	
		y[3] += offset;
	}
	if (y[0] < UP_LF_Y) 
	{
		offset = UP_LF_Y - y[0];
		x[0] += offset;	
		y[0] += offset;
	}
	if (y[1] < UP_LF_Y) 
	{
		offset = UP_LF_Y - y[1];
		x[1] -= offset;	
		y[1] += offset;
	}
	if (y[2] > LO_RT_Y) 
	{
		offset = LO_RT_Y - y[2];
		x[2] -= offset;	
		y[2] += offset;
	}
	if (y[3] > LO_RT_Y) 
	{
		offset = LO_RT_Y - y[3];
		x[3] += offset;	
		y[3] += offset;
	}
	PRDrawLine2(x[0], y[0], x[3], y[3], color, 0);
	PRDrawLine2(x[1], y[1], x[2], y[2], color, 0);
	return 0;
}

LONG CPrGeneral::PRDrawSXCross(int x0, int y0, int color, int len)
{
	PRDrawSCross(x0, y0, color, len);
	PRDrawXCross(x0, y0, color, len);	// 20120713 Yip: Draw X-Shape Cross
	return 0;
}

void CPrGeneral::PRS_DrawCross(short x, short y, short color, short size)
{
	PR_DRAW_CROSS_CMD stCmd;
	PR_DRAW_CROSS_RPY stRpy;

	stCmd.coCrossCentre.x = x;
	stCmd.coCrossCentre.y = y;
	stCmd.szCrossSize.x = size;
	stCmd.szCrossSize.y = size;
	stCmd.emColor = (PR_COLOR)color;
	PR_DrawCrossCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
//	theApp.LogVisionRpy(stRpy.uwCommunStatus, stRpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
}

void CPrGeneral::PRS_EraseCross(short x, short y, short color, short size)
{
	PR_DRAW_CROSS_CMD stCmd;
	PR_DRAW_CROSS_RPY stRpy;

	stCmd.coCrossCentre.x = x;
	stCmd.coCrossCentre.y = y;
	stCmd.szCrossSize.x = size;
	stCmd.szCrossSize.y = size;
	stCmd.emColor = (PR_COLOR)color;
	PR_EraseCrossCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
//	theApp.LogVisionRpy(stRpy.uwCommunStatus, stRpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
}

PR_COORD *CPrGeneral::SetPointXY(PR_COORD *pco, short x, short y)
{
	pco->x = x;
	pco->y = y;
	return pco;
}

PR_COORD *CPrGeneral::ShiftPoint(PR_COORD *pco, const PR_COORD *pcoShift)
{
	pco->x += pcoShift->x;
	pco->y += pcoShift->y;
	return pco;
}

PR_COORD *CPrGeneral::NegShiftPoint(PR_COORD *pco, const PR_COORD *pcoShift)
{
	pco->x -= pcoShift->x;
	pco->y -= pcoShift->y;
	return pco;
}

PR_WIN *CPrGeneral::ShiftWinXY(PR_WIN *pstWin, int x, int y)
{
	pstWin->coCorner1.x += x;
	pstWin->coCorner1.y += y;
	pstWin->coCorner2.x += x;
	pstWin->coCorner2.y += y;
	return pstWin;
}

PR_WIN *CPrGeneral::ShiftWin(PR_WIN *pstWin, const PR_COORD *pcoShift)
{
	return ShiftWinXY(pstWin, pcoShift->x, pcoShift->y);
}

PR_WIN *CPrGeneral::NegShiftWin(PR_WIN *pstWin, const PR_COORD *pcoShift)
{
	return ShiftWinXY(pstWin, -pcoShift->x, -pcoShift->y);
}

BOOL CPrGeneral::IsPointInsideScreen(const PR_COORD *pco)
{
	return ((pco->x < PR_MIN_COORD) || (pco->y < PR_MIN_COORD) ||
			(pco->x > PR_MAX_COORD) || (pco->y > PR_MAX_COORD)) ? FALSE : TRUE;
}

BOOL CPrGeneral::IsWinInsideScreen(const PR_WIN *pstWin)
{
	return ((pstWin->coCorner1.x < PR_MIN_COORD) || (pstWin->coCorner1.y < PR_MIN_COORD) ||
			(pstWin->coCorner2.x > PR_MAX_COORD) || (pstWin->coCorner2.y > PR_MAX_COORD)) ? FALSE : TRUE;
}

BOOL CPrGeneral::PRS_IsPointInsidePrLimit(const PR_COORD *pco)
{
	return ((pco->x < PR_MAX_WIN_ULC_X) || (pco->y < PR_MAX_WIN_ULC_Y) ||
			(pco->x > PR_MAX_WIN_LRC_X) || (pco->y > PR_MAX_WIN_LRC_Y)) ? FALSE : TRUE;
}

PR_COORD *CPrGeneral::CheckPointInsidePrLimit(PR_COORD *pco)
{
	if (pco)
	{
		if (pco->x < PR_MAX_WIN_ULC_X) 
		{
			pco->x = PR_MAX_WIN_ULC_X;
		}
		else if (pco->x > PR_MAX_WIN_LRC_X) 
		{
			pco->x = PR_MAX_WIN_LRC_X;
		}

		if (pco->y < PR_MAX_WIN_ULC_Y) 
		{
			pco->y = PR_MAX_WIN_ULC_Y;
		}
		else if (pco->y > PR_MAX_WIN_LRC_Y) 
		{
			pco->y = PR_MAX_WIN_LRC_Y;
		}
	}
	return pco;
}

PR_COORD *CPrGeneral::CheckPointInScreen(PR_COORD *pco)
{
	if (pco)
	{
		if (pco->x <= PR_MIN_COORD) 
		{
			pco->x = PR_MIN_COORD + 1;
		}
		else if (pco->x >= PR_MAX_COORD) 
		{
			pco->x = PR_MAX_COORD - 1;
		}

		if (pco->y <= PR_MIN_COORD) 
		{
			pco->y = PR_MIN_COORD + 1;
		}
		else if (pco->y >= PR_MAX_COORD) 
		{
			pco->y = PR_MAX_COORD - 1;
		}
	}
	return pco;
}

PR_WIN *CPrGeneral::PRS_CheckWinInsidePrLimit(PR_WIN *pstWin)
{
	if (pstWin)
	{
		CheckPointInsidePrLimit(&pstWin->coCorner1);
		CheckPointInsidePrLimit(&pstWin->coCorner2);
	}
	return pstWin;
}

VOID CPrGeneral::CheckPRCoordLimit(PR_COORD &stCoord)
{
	if (stCoord.x < UP_LF_X) 
	{
		stCoord.x = UP_LF_X;
	}
	if (stCoord.x > LO_RT_X) 
	{
		stCoord.x = LO_RT_X;
	}
	if (stCoord.y < UP_LF_Y) 
	{
		stCoord.y = UP_LF_Y;
	}
	if (stCoord.y > LO_RT_Y) 
	{
		stCoord.y = LO_RT_Y;
	}
}

VOID CPrGeneral::CheckPRCoordLimit(PR_COORD stCorner[], int nMaxCorner)
{
	if (stCorner && (nMaxCorner > 0))
	{
		for (int i = 0; i < nMaxCorner; i++)
		{
			CheckPRCoordLimit(stCorner[i]);
		}
	}
}

VOID CPrGeneral::CheckPRCoordLimit(PR_WORD &lX, PR_WORD &lY)
{
	if (lX < UP_LF_X) 
	{
		lX = UP_LF_X;
	}
	if (lX > LO_RT_X) 
	{
		lX = LO_RT_X;
	}
	if (lY < UP_LF_Y) 
	{
		lY = UP_LF_Y;
	}
	if (lY > LO_RT_Y) 
	{
		lY = LO_RT_Y;
	}
}

VOID CPrGeneral::SetPRCoord(PR_COORD &stCoord, const LONG lX, const LONG lY)
{
	stCoord.x = (PR_WORD)lX;
	stCoord.y = (PR_WORD)lY;
}

VOID CPrGeneral::PRConvertDieCornersToWin(PR_WIN *pstWin, const PR_COORD acoDieCorners[])
{
	PR_WIN stWin;
	stWin.coCorner1 = acoDieCorners[0];
	stWin.coCorner2 = acoDieCorners[0];
	for (int i = 1; i < 4; i++)
	{
		PR_COORD	pos = acoDieCorners[i];
		stWin.coCorner1.x = min(stWin.coCorner1.x, pos.x);
		stWin.coCorner1.y = min(stWin.coCorner1.y, pos.y);
		stWin.coCorner2.x = max(stWin.coCorner2.x, pos.x);
		stWin.coCorner2.y = max(stWin.coCorner2.y, pos.y);
	}
	if (pstWin)
	{
		*pstWin = stWin;
	}
}
/*
LONG CPrGeneral::PRDrawUpLeftConer(LONG x, LONG y, LONG length)
{
	CheckPRCoordLimit(x, y);
	PRDrawLine(x, y, x + length, y, 1); //Green Color
	PRDrawLine(x, y, x, y + length, 1); //Green Color
	return 0;
}

LONG CPrGeneral::PRDrawLowerRightConer(LONG x, LONG y, LONG length)
{
	CheckPRCoordLimit(x, y);
	PRDrawLine(x - length, y, x, y, 1); //Green Color
	PRDrawLine(x, y - length, x, y, 1); //Green Color
	return 0;
}
*/

LONG CPrGeneral::PRDrawLine(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const LONG x0, const LONG y0, const LONG x1, const LONG y1, const PR_COLOR emColor)
{
	PR_COORD coPoint1, coPoint2;
	coPoint1.x = (PR_WORD)x0;
	coPoint1.y = (PR_WORD)y0;
	CheckPRCoordLimit(coPoint1);

	coPoint2.x = (PR_WORD)x1;
	coPoint2.y = (PR_WORD)y1;
	CheckPRCoordLimit(coPoint2);

	return PRDrawLine(ubSenderID, ubReceiverID, coPoint1, coPoint2, emColor);
}

LONG CPrGeneral::PRDrawLine(const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID, const PR_COORD coPoint1, const PR_COORD coPoint2, const PR_COLOR emColor)
{
	PR_DRAW_LINE_CMD cmd;
	PR_DRAW_LINE_RPY rpy;
	
	PR_InitDrawLineCmd(&cmd);

	cmd.stLine.coPoint1 = coPoint1;
	cmd.stLine.coPoint2 = coPoint2;

	cmd.emColor = emColor;

	if (cmd.emColor == PR_COLOR_TRANSPARENT)
	{
		PR_EraseLineCmd(&cmd, ubSenderID, ubReceiverID, &rpy);
//		theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	}
	else
	{
		PR_DrawLineCmd(&cmd, ubSenderID, ubReceiverID, &rpy);
//		theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	}
	return 0;
}

LONG CPrGeneral::PRDrawLine1(LONG x0, LONG y0, LONG x1, LONG y1, int color, int lCamera)
{
	PR_DRAW_LINE_CMD cmd;
	PR_DRAW_LINE_RPY rpy;
	
	PR_InitDrawLineCmd(&cmd);
	cmd.stLine.coPoint1.x = (PR_WORD)x0;
	cmd.stLine.coPoint1.y = (PR_WORD)y0;
	cmd.stLine.coPoint2.x = (PR_WORD)x1;
	cmd.stLine.coPoint2.y = (PR_WORD)y1;

	CheckPRCoordLimit(cmd.stLine.coPoint1);
	CheckPRCoordLimit(cmd.stLine.coPoint2);
	cmd.emColor = (PR_COLOR)color;

	PR_DrawLineCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}

LONG CPrGeneral::PRDrawLine2(LONG x0, LONG y0, LONG x1, LONG y1, int color, int lCamera)
{
	PR_DRAW_LINE_CMD cmd;
	PR_DRAW_LINE_RPY rpy;
	
	PR_InitDrawLineCmd(&cmd);
	cmd.stLine.coPoint1.x = (PR_WORD)x0;
	cmd.stLine.coPoint1.y = (PR_WORD)y0;
	cmd.stLine.coPoint2.x = (PR_WORD)x1;
	cmd.stLine.coPoint2.y = (PR_WORD)y1;

	CheckPRCoordLimit(cmd.stLine.coPoint1);
	CheckPRCoordLimit(cmd.stLine.coPoint2);
	cmd.emColor = (PR_COLOR)(color);

	PR_DrawLineCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}

LONG CPrGeneral::wpr_draw_cross(PR_COORD pt, PR_COLOR color)
{
	PR_DRAW_LINE_CMD cmd;
	PR_DRAW_LINE_RPY rpy;

	PR_InitDrawLineCmd(&cmd);
	CheckPRCoordLimit(pt);
	cmd.emColor = color;
	cmd.stLine.coPoint1.x = UP_LF_X; 
	cmd.stLine.coPoint2.x = LO_RT_X;
	cmd.stLine.coPoint1.y = pt.y;
	cmd.stLine.coPoint2.y = pt.y;
	PR_DrawLineCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM

	cmd.emColor = color;
	cmd.stLine.coPoint1.x = pt.x; 
	cmd.stLine.coPoint2.x = pt.x;
	cmd.stLine.coPoint1.y = UP_LF_Y;
	cmd.stLine.coPoint2.y = LO_RT_Y;
	PR_DrawLineCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM

	return 0;
}

LONG CPrGeneral::wpr_draw_rect(PR_COORD ulc, PR_COORD lrc, PR_COLOR color, PR_UBYTE SID, PR_UBYTE RID)
{
	PR_DRAW_RECT_CMD cmd;
	PR_DRAW_RECT_RPY rpy;

	cmd.stWin.coCorner1 = ulc;
	cmd.stWin.coCorner2 = lrc;
	cmd.emColor = color;
	PR_DrawRectCmd(&cmd, SID, RID, &rpy);
	return 0;
}

LONG CPrGeneral::wpr_draw_box(PR_COORD ulc, PR_COORD lrc, int color)
{
	PR_DRAW_RECT_CMD cmd;
	PR_DRAW_RECT_RPY rpy;
	cmd.stWin.coCorner1.x = ulc.x;
	cmd.stWin.coCorner1.y = ulc.y;
	cmd.stWin.coCorner2.x = lrc.x;
	cmd.stWin.coCorner2.y = lrc.y;
	
	if (color == 2)
	{
		cmd.emColor = PR_COLOR_YELLOW;
	}
	else if ((color == 1) || (color == 15))
	{
		cmd.emColor = PR_COLOR_GREEN;
	}
	else if (color)
	{
		cmd.emColor = (PR_COLOR)color;
	}
	else
	{
		cmd.emColor = PR_COLOR_TRANSPARENT;
	}
	PR_DrawRectCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}


double CPrGeneral::fabs(double temp)
{
	if (temp >= 0) 
	{
		return temp;
	}
	return -temp;
}

LONG CPrGeneral::abs(long temp)
{
	if (temp >= 0) 
	{
		return temp;
	}
	return -temp;
}


LONG CPrGeneral::DrawSearchRegion(int x0, int y0, int x1, int y1)
{
	PR_DRAW_SRCH_AREA_CMD cmd;
	PR_DRAW_SRCH_AREA_RPY rpy;

	if (x0 < UP_LF_X) 
	{
		x0 = UP_LF_X;
	}
	if (y0 < UP_LF_Y) 
	{
		y0 = UP_LF_Y;
	}
	if (x1 > LO_RT_X) 
	{
		x1 = LO_RT_X;
	}
	if (y1 > LO_RT_Y) 
	{
		y1 = LO_RT_Y;
	}

	cmd.stWin.coCorner1.x = (unsigned short)x0;
	cmd.stWin.coCorner1.y = (unsigned short)y0;
	cmd.stWin.coCorner2.x = (unsigned short)x1;
	cmd.stWin.coCorner2.y = (unsigned short)y1;
	PR_DrawSrchAreaCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}


LONG CPrGeneral::DrawSearchRegion(PR_WIN stWin)
{
	PR_DRAW_SRCH_AREA_CMD cmd;
	PR_DRAW_SRCH_AREA_RPY rpy;

	cmd.stWin = stWin;
	PR_DrawSrchAreaCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}

LONG CPrGeneral::EraseSearchRegion(LONG x0, LONG y0, LONG x1, LONG y1)
{
	PR_DRAW_RECT_CMD cmd;
	PR_DRAW_RECT_RPY rpy;

	cmd.stWin.coCorner1.x = (unsigned short)x0;
	cmd.stWin.coCorner1.y = (unsigned short)y0;
	cmd.stWin.coCorner2.x = (unsigned short)x1;
	cmd.stWin.coCorner2.y = (unsigned short)y1;

	CheckPRCoordLimit(cmd.stWin.coCorner1);
	CheckPRCoordLimit(cmd.stWin.coCorner2);

	cmd.emColor = PR_COLOR_TRANSPARENT;
	PR_DrawRectCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}

LONG CPrGeneral::EraseSearchRegion(PR_WIN stWin)
{
	PR_DRAW_RECT_CMD cmd;
	PR_DRAW_RECT_RPY rpy;

	cmd.stWin = stWin;
	cmd.emColor = PR_COLOR_TRANSPARENT;
	PR_DrawRectCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
	return 0;
}
/*
LONG CPrGeneral::PRDrawBox(PR_COORD stCorner[4], int color)
{
	PRDrawLine(stCorner[0].x, stCorner[0].y, stCorner[1].x, stCorner[1].y, color); 
	PRDrawLine(stCorner[2].x, stCorner[2].y, stCorner[1].x, stCorner[1].y, color); 
	PRDrawLine(stCorner[0].x, stCorner[0].y, stCorner[3].x, stCorner[3].y, color); 
	PRDrawLine(stCorner[2].x, stCorner[2].y, stCorner[3].x, stCorner[3].y, color);
	return 0;
}

LONG CPrGeneral::PRDrawBox(PR_COORD coCorner[4], int nCornerNo, int color)
{
	if ((nCornerNo < 0) || (nCornerNo > 3))
	{
		return 0;
	}
	int maxCorner = (color == 0) ? 3 : nCornerNo;
	if (color == 0)
	{
		PRS_EraseCross(coCorner[nCornerNo].x, coCorner[nCornerNo].y, color, 1000);
	}
	for (int i = 1; i <= maxCorner; i++)
	{
		if (i == maxCorner)
		{
			PRDrawLine(coCorner[i - 1].x, coCorner[i - 1].y, coCorner[i].x, coCorner[i].y, color);
			if (i == 3)
			{
				PRDrawLine(coCorner[0].x, coCorner[0].y, coCorner[i].x, coCorner[i].y, color);
			}
		}
		else
		{
			PRDrawLine(coCorner[i - 1].x, coCorner[i - 1].y, coCorner[i].x, coCorner[i].y, color);
		}
	}
	if (color != 0)
	{
		PRS_DrawCross(coCorner[nCornerNo].x, coCorner[nCornerNo].y, PR_COLOR_RED, 1000);
	}
	return 0;
}
*/
LONG CPrGeneral::util_pr_outtext(const char *Str, int x, int y, int on)
{
	PR_DISPLAY_CHAR_CMD cmd;
	PR_DISPLAY_CHAR_RPY rpy;

	cmd.pubChar = (unsigned char*)Str;
	cmd.tcoPosition.x = x;
	cmd.tcoPosition.y = y;
	if (on == 1)
	{
		cmd.emColor = PR_COLOR_GREEN;
	}
	else if (on == 2)
	{
		cmd.emColor = PR_COLOR_RED;
	}
	else
	{
		cmd.emColor = PR_COLOR_TRANSPARENT;
	}
	cmd.uwNChar = (PR_UWORD)strlen((char*)cmd.pubChar);
	PR_DisplayCharCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}


CString CPrGeneral::GetPrVersionString()
{
	CString szVersion;

	PR_GET_VERSION_NO_RPY	stRpy;
	PR_GetVersionNoCmd(DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);

	if ((stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR) || 
		(stRpy.stStatus.uwPRStatus != PR_ERR_NOERR))
	{
		return "Error";
	}

	szVersion = stRpy.aubVersionNo;
	return szVersion;

}

CString CPrGeneral::GetPrITFVersionString()
{
	char szVersion[40];
	PR_UBYTE *pVersion = (PR_UBYTE*)szVersion;
	PR_GetHostITFVersionNo(pVersion);
	return CString(szVersion);
}


//=========================================================================================
//  Mouse Function 
//=========================================================================================
LONG CPrGeneral::UsePRMouse(PR_COORD stULCorner, PR_COORD stLRCorner)
{
	if (m_bUseMouse == FALSE)
	{
		// Erase the original region
		DrawAndEraseCursor(stULCorner, PR_UPPER_LEFT, 0);
		DrawAndEraseCursor(stLRCorner, PR_LOWER_RIGHT, 0);

		MouseDefineRegion();
		m_bUseMouse = TRUE;
	}
	return m_bUseMouse;
}

LONG CPrGeneral::PRMouseDrawComplete(PR_COORD &stULCorner, PR_COORD &stLRCorner)
{
	if (m_bUseMouse == TRUE)
	{
		// Get the mouse defined region
		LONG uwNumOfCorners = MouseGetRegion(stULCorner, stLRCorner);

		if (uwNumOfCorners == 2)
		{
			// Draw the defined region
			wpr_draw_rect(stULCorner, stLRCorner, PR_COLOR_CYAN, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID);
		}
		m_bUseMouse = FALSE;
		return TRUE;
	}
	return FALSE;
}

LONG CPrGeneral::MouseDefineRegion(const PR_SHAPE_TYPE emShape, const PR_COLOR emColor)
{
	PR_DEFINE_MOUSE_CTRL_OBJ_CMD	stDefineCmd;
	PR_DEFINE_MOUSE_CTRL_OBJ_RPY	stDefineRpy;	

	PR_InitDefineMouseCtrlObjCmd(&stDefineCmd);
	stDefineCmd.emShape = emShape;
	stDefineCmd.emColor = emColor;
	stDefineCmd.ulObjectId = 1;
	PR_DefineMouseCtrlObjCmd(&stDefineCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stDefineRpy);
//	theApp.LogVisionRpy(stDefineRpy.stStatus.uwCommunStatus, stDefineRpy.stStatus.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}


LONG CPrGeneral::MouseSet2PointRegion(const PR_COORD coCorner1, const PR_COORD coCorner2, const PR_SHAPE_TYPE emShape, const PR_COLOR emColor)
{
	PR_SET_MOUSE_CTRL_OBJ_CMD	stSetCmd;
	PR_SET_MOUSE_CTRL_OBJ_RPY	stSetRpy;

	// Set the mouse size first
	PR_InitSetMouseCtrlObjCmd(&stSetCmd);
	stSetCmd.ulObjectId = 1;
	stSetCmd.emShape = emShape;
	stSetCmd.emColor = emColor;
	stSetCmd.uwNumOfCorners = 2;

	stSetCmd.acoObjCorner[0] = coCorner1;		
	stSetCmd.acoObjCorner[1] = coCorner2;		

	PR_SetMouseCtrlObjCmd(&stSetCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stSetRpy);
	return 0;
}
	

LONG CPrGeneral::MouseGetRegion(PR_COORD &coCorner1, PR_COORD &coCorner2)
{
	return MouseGetRegion(&coCorner1.x, &coCorner1.y, &coCorner2.x, &coCorner2.y);
}

LONG CPrGeneral::MouseGetRegion(PR_WORD *Corner1x, PR_WORD *Corner1y, PR_WORD *Corner2x, PR_WORD *Corner2y)
{
	PR_COORD acoObjCorner[PR_MAX_NO_OF_SHAPE_CORNER];

	LONG uwNumOfCorners = MouseGetAllCorners(acoObjCorner);
	if ((uwNumOfCorners == 2) || (uwNumOfCorners == 4))
	{
		*Corner1x = acoObjCorner[0].x;
		*Corner1y = acoObjCorner[0].y;
		*Corner2x = acoObjCorner[1].x;
		*Corner2y = acoObjCorner[1].y;
	}

	return uwNumOfCorners;
}

LONG CPrGeneral::MouseGetAllCorners(PR_COORD acoObjCorner[PR_MAX_NO_OF_SHAPE_CORNER])
{
	PR_GET_MOUSE_CTRL_OBJ_CMD		stGetCmd; 
	PR_GET_MOUSE_CTRL_OBJ_RPY		stGetRpy;

	PR_InitGetMouseCtrlObjCmd(&stGetCmd);
	stGetCmd.ulObjectId = 1;
	PR_GetMouseCtrlObjCmd(&stGetCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stGetRpy);

	LONG uwNumOfCorners = stGetRpy.uwNumOfCorners;
	if (uwNumOfCorners >= PR_MAX_NO_OF_SHAPE_CORNER)
	{
		uwNumOfCorners = PR_MAX_NO_OF_SHAPE_CORNER;
	}
	for (long i = 0; i < uwNumOfCorners; i++)
	{
		acoObjCorner[i] = stGetRpy.acoObjCorner[i];
	}
//	theApp.LogVisionRpy(stGetRpy.stStatus.uwCommunStatus, stGetRpy.stStatus.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM

	// Erase the mouse object
	EraseMouseCtrlObj();
	return stGetRpy.uwNumOfCorners;
}

VOID CPrGeneral::EraseMouseCtrlObj()
{
	PR_ERASE_MOUSE_CTRL_OBJ_CMD		stEraseCmd;
	PR_ERASE_MOUSE_CTRL_OBJ_RPY		stEraseRpy;

	PR_InitEraseMouseCtrlObjCmd(&stEraseCmd);
	stEraseCmd.ulObjectId = 1;
	PR_EraseMouseCtrlObjCmd(&stEraseCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stEraseRpy);
//	theApp.LogVisionRpy(stEraseRpy.stStatus.uwCommunStatus, stEraseRpy.stStatus.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
}

LONG CPrGeneral::wpr_draw_Smallcross(PR_COORD pt, PR_COLOR color)
{
	PR_DRAW_LINE_CMD cmd;
	PR_DRAW_LINE_RPY rpy;

	PR_InitDrawLineCmd(&cmd);
	CheckPRCoordLimit(pt);

	cmd.emColor = color;
	cmd.stLine.coPoint1.x = pt.x - UP_LF_X; 
	cmd.stLine.coPoint2.x = pt.x + UP_LF_X; 
	cmd.stLine.coPoint1.y = pt.y;
	cmd.stLine.coPoint2.y = pt.y;
	CheckPRCoordLimit(cmd.stLine.coPoint1);
	CheckPRCoordLimit(cmd.stLine.coPoint2);
	PR_DrawLineCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);

	cmd.emColor = color;
	cmd.stLine.coPoint1.x = pt.x; 
	cmd.stLine.coPoint2.x = pt.x;
	cmd.stLine.coPoint1.y = pt.y - UP_LF_X;
	cmd.stLine.coPoint2.y = pt.y + UP_LF_X;
	CheckPRCoordLimit(cmd.stLine.coPoint1);
	CheckPRCoordLimit(cmd.stLine.coPoint2);

	PR_DrawLineCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
	return 0;
}

void CPrGeneral::PRS_DrawTmpl(PR_COORD center, PR_SIZE size, short color)
{
	PR_DRAW_TMPL_CMD stCmd;
	PR_DRAW_TMPL_RPY stRpy;

	stCmd.coTmplCentre = center;
	stCmd.szTmplSize = size;
	if (color == PR_COLOR_TRANSPARENT)
	{
		PR_EraseTmplCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
	}
	else
	{
		stCmd.emColor = (PR_COLOR)color;
		PR_DrawTmplCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
	}
}

/*
void CPrGeneral::PRS_outtext(char *buf, short x, short y, short color)
{
	PR_DISPLAY_CHAR_CMD cmd;
	PR_DISPLAY_CHAR_RPY rpy;
	short c;

	if (x > 27) 
	{
		x = 27;
	}
	if (y > 17) 
	{
		y = 17;
	}
	cmd.tcoPosition.x = x;
	cmd.tcoPosition.y = y;
	switch (color)
	{
	case MN_DARKGRAY :
	case MN_BLACK : 
		c = PR_COLOR_BLACK; 
		break;
	case MN_BRIGHTWHITE :
	case MN_WHITE : 
		c = PR_COLOR_GREY; 
		break;
	case MN_LIGHTRED :
	case MN_RED : 
		c = PR_COLOR_RED; 
		break;
	case MN_LIGHTBLUE :
	case MN_BLUE : 
		c = PR_COLOR_BLUE; 
		break;
	case MN_LIGHTGREEN :
	case MN_GREEN : 
		c = PR_COLOR_GREEN; 
		break;
	case MN_LIGHTCYAN :
	case MN_CYAN : 
		c = PR_COLOR_CYAN; 
		break;
	case MN_LIGHTMAGENTA :
	case MN_MAGENTA : 
		c = PR_COLOR_MAGENTA; 
		break;
	case MN_BROWN :
	case MN_YELLOW : 
		c = PR_COLOR_YELLOW; 
		break;
	default : 
		c = PR_COLOR_GREEN;
	}
	cmd.emColor = (PR_COLOR)c;
	cmd.uwNChar = strlen(buf);
	cmd.pubChar = (PR_UBYTE*)buf;
	PR_DisplayCharCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
}
*/

VOID CPrGeneral::PRS_clearline(short row)
{
	PR_CLEAR_TEXT_ROW_CMD cmd;
	PR_CLEAR_TEXT_ROW_RPY rpy;

	cmd.wTextRow = row;
	PR_ClearTextRowCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
}


VOID CPrGeneral::PRS_DrawSearchRange(PR_WIN *pWin)
{
	PR_DRAW_SRCH_AREA_CMD stCmd;
	PR_DRAW_SRCH_AREA_RPY stRpy;

	stCmd.stWin = *pWin;
	PR_DrawSrchAreaCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
}


//======================================================================================================
//  PR General Ultity Function
//======================================================================================================
LONG CPrGeneral::ResetLighting(short prid, PR_PURPOSE emPurpose)
{
	PR_OPTIC      stOptic;
	PR_COMMON_RPY stCommonRpy;
	int           nSourceNo;
	int           i;

	if (prid != 0)
	{
		PR_LoadOptic(prid, emPurpose, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stOptic, &stCommonRpy);
	}
	else
	{
		PR_GetOptic(emPurpose, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stOptic, &stCommonRpy);
	}
	if (stCommonRpy.uwPRStatus != PR_ERR_NOERR)
	{
		return stCommonRpy.uwPRStatus;
	}

	PR_SetExposureTime(stOptic.emExposureTime, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stOptic, &stCommonRpy);

	nSourceNo = stOptic.uwSourceNo;
	for (i = 0; i < nSourceNo; i++)
	{
		PR_SOURCE		 emSource;
		PR_UWORD         uwLevel;
		PR_GetLightSource(&stOptic, i, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &emSource, &stCommonRpy);
		PR_GetLighting(&stOptic, emSource, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &uwLevel, &stCommonRpy);
		if (stCommonRpy.uwPRStatus == PR_ERR_NOERR)
		{
			PR_SetLighting(emSource, uwLevel, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stOptic, &stCommonRpy);
		}
	}
	if (stCommonRpy.uwPRStatus != PR_ERR_NOERR)
	{
		return stCommonRpy.uwPRStatus;
	}
	return PR_ERR_NOERR;
}


void CPrGeneral::PRS_clearscreen()
{
	PR_CLEAR_SCREEN_RPY	rpy;
	PR_ClearScreenCmd(DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
}

void CPrGeneral::PRS_DrawLargestHomeCursor(short x, short y)
{
	PR_DRAW_HOME_CURSOR_CMD stCmd;
	PR_DRAW_HOME_CURSOR_RPY stRpy;

	stCmd.emTmplSize = PR_TMPL_SIZE_DISABLE;
	stCmd.coPosition.x = x;
	stCmd.coPosition.y = y;
	PR_DrawHomeCursorCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
//	theApp.LogVisionRpy(stRpy.uwCommunStatus, stRpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
}



LONG CPrGeneral::PRDrawCircle(PR_COORD pt, PR_LENGTH radius)
{
	PR_DRAW_CIRCLE_CMD cmd;
	PR_DRAW_CIRCLE_RPY rpy;
	
	CheckPRCoordLimit(pt);

	cmd.stCentre = pt;

	cmd.leRadius = radius;

	
	PR_DrawCircleCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);
//	theApp.LogVisionRpy(rpy.uwCommunStatus, rpy.uwPRStatus, __FUNCTION__, __LINE__); //20131113AM
	return 0;
}


VOID CPrGeneral::VerifyPRRegion(PR_WIN *stInputArea)
{
	if (stInputArea->coCorner1.x < PR_MAX_WIN_ULC_X)
	{
		stInputArea->coCorner1.x = PR_MAX_WIN_ULC_X;
	}

	if (stInputArea->coCorner1.y < PR_MAX_WIN_ULC_Y)
	{
		stInputArea->coCorner1.y = PR_MAX_WIN_ULC_Y;
	}
	
	if (stInputArea->coCorner2.x > PR_MAX_WIN_LRC_X)
	{
		stInputArea->coCorner2.x = PR_MAX_WIN_LRC_X;
	}

	if (stInputArea->coCorner2.y > PR_MAX_WIN_LRC_Y)
	{
		stInputArea->coCorner2.y = PR_MAX_WIN_LRC_Y;
	}
}


VOID CPrGeneral::DrawAndEraseCursor(PR_COORD stStartPos, unsigned char ucCorner, BOOL bDraw)
{
	PR_DRAW_LINE_CMD		stLineCmd;
	PR_DRAW_LINE_RPY		stLineRpy;
	PR_COORD				stLineLength;
	short					i;

	PR_InitDrawLineCmd(&stLineCmd);
	stLineLength.x = 50 * PR_SCALE_FACTOR;
	stLineLength.y = 50 * PR_SCALE_FACTOR; 

	stLineCmd.stLine.coPoint1 = stStartPos;
	stLineCmd.stLine.coPoint2 = stStartPos; 


	if (ucCorner == PR_LOWER_RIGHT) 	
	{
		stLineLength.x = -stLineLength.x;
		stLineLength.y = -stLineLength.y; 
	}

	if (bDraw == 1)
	{
		stLineCmd.emColor = PR_COLOR_GREEN;
	}
	else
	{
		stLineCmd.emColor = PR_COLOR_TRANSPARENT;
	}

	stLineCmd.stLine.coPoint1 = stStartPos;
	stLineCmd.stLine.coPoint2 = stStartPos; 

	for (i = 0; i < 2; i++)
	{
		stLineCmd.stLine.coPoint2 = stStartPos; 
		if (i == 0)	//Draw Horz Line
		{
			stLineCmd.stLine.coPoint2.x += stLineLength.x;
		}
		else		//Draw Vert Line
		{
			stLineCmd.stLine.coPoint2.y += stLineLength.y;
		}

		PR_DrawLineCmd(&stLineCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stLineRpy);
	}
}

VOID CPrGeneral::AdjCorner(long temp, PR_WORD step, PR_WORD &x, PR_WORD &y)
{
	switch (temp)
	{   
	case 1:  //up												
		y -= step;				
		break;

	case 2: // up right				
		y -= step;
		x += step;	
		break;

	case 3: // right
		x += step;
		break;

	case 4: // lower right
		x += step;
		y += step;
		break;

	case 5: // lower 				
		y += step;
		break;

	case 6:  // lower left				
		x -= step;
		y += step;
		break;

	case 7: // left				
		x -= step;			
		break;

	case 8: // upper left
		x -= step;
		y -= step;
		break;

	default:
		break;
	}
	if (temp) 
	{
		CheckPRCoordLimit(x, y);
	}
}

/*
LONG CPrGeneral::AdjRectangleConer(BOOL bLeftTopRightLowerCorner, long temp, PR_COORD &ulc, PR_COORD &lrc)
{	
	long step;
	PR_WORD ltemp0, ltemp1, ltemp2, ltemp3;

	if (!bool_wt_fast_joy) 
	{
		step = 5;
	}
	else if (bool_wt_fast_joy == 1) 
	{
		step = 10;
	}
	else 
	{
		step = 20;
	}
	
	if (temp)
	{
		ltemp1 = ulc.x;
		ltemp0 = ulc.y;
		ltemp3 = lrc.x;
		ltemp2 = lrc.y;
		wpr_draw_box(ulc, lrc, 0);
	}
	if (bLeftTopRightLowerCorner)
	{
		AdjCorner(temp, step, ltemp3, ltemp2);    //RightLower 
	} 
	else
	{
		AdjCorner(temp, step, ltemp1, ltemp0);    //Left Top
	} 
	if (temp)
	{
		ulc.x = (PR_WORD)ltemp1;
		ulc.y = (PR_WORD)ltemp0;
		lrc.x = (PR_WORD)ltemp3;
		lrc.y = (PR_WORD)ltemp2;				
		wpr_draw_box(ulc, lrc, 1);	
	} 
	return 0;
}


LONG CPrGeneral::MoveRectange(long temp, PR_COORD &ulc, PR_COORD &lrc)
{	
	long step;
	long ltemp0, ltemp1, ltemp2, ltemp3;

	if (!bool_wt_fast_joy) 
	{
		step = 5;
	}
	else if (bool_wt_fast_joy == 1) 
	{
		step = 10;
	}
	else 
	{
		step = 20;
	}
	
	if (temp)
	{
		ltemp1 = ulc.x;
		ltemp0 = ulc.y;
		ltemp3 = lrc.x;
		ltemp2 = lrc.y;
		wpr_draw_box(ulc, lrc, 0);
	}
	switch (temp)
	{   
	case 1:  // up
		if ((ltemp0 - step) > UP_LF_Y)
		{
			ltemp0 -= step;
			ltemp2 -= step;
		}
		break;

	case 2: // up right
		if ((ltemp0 - step) > UP_LF_Y)
		{
			ltemp0 -= step;
			ltemp2 -= step;
		}
		if ((ltemp3 + step) < LO_RT_X)
		{
			ltemp1 += step;
			ltemp3 += step;
		}
		break;

	case 3: // right
		if ((ltemp3 + step) < LO_RT_X)
		{
			ltemp1 += step;
			ltemp3 += step;
		}
		break;

	case 4:  //lower right
		if ((ltemp2 + step) < LO_RT_Y)
		{
			ltemp0 += step;
			ltemp2 += step;
		}
		if ((ltemp3 + step) < LO_RT_X)
		{
			ltemp1 += step;
			ltemp3 += step;
		}
		break;
			
	case 5:  // down
		if ((ltemp2 + step) < LO_RT_Y)
		{
			ltemp0 += step;
			ltemp2 += step;
		}
		break;

	case 6:  // lower left
		if ((ltemp2 + step) < LO_RT_Y)
		{
			ltemp0 += step;
			ltemp2 += step;
		}
		if ((ltemp1 - step) > UP_LF_X)
		{
			ltemp1 -= step;
			ltemp3 -= step;
		}
		break;

	case 7: // left
		if ((ltemp1 - step) > UP_LF_X)
		{
			ltemp1 -= step;
			ltemp3 -= step;
		}
		break;

	case 8:  // upper left
		if ((ltemp1 - step) > UP_LF_X)
		{
			ltemp1 -= step;
			ltemp3 -= step;
		}
		if ((ltemp0 - step) > UP_LF_Y)
		{
			ltemp0 -= step;
			ltemp2 -= step;
		}
		break;
			
	default:
		break;
	}

	if (temp)
	{	
		ulc.x = (PR_WORD)ltemp1;
		ulc.y = (PR_WORD)ltemp0;
		lrc.x = (PR_WORD)ltemp3;
		lrc.y = (PR_WORD)ltemp2;				
		wpr_draw_box(ulc, lrc, 1);	
	} 
	return 0;
}


LONG CPrGeneral::AdjBoptCalibration(LONG temp)
{	
	PR_COORD ulc, lrc;
	if (temp)
	{
		ulc.x = (PR_WORD)ltemp1;
		ulc.y = (PR_WORD)ltemp0;
		lrc.x = (PR_WORD)ltemp3;
		lrc.y = (PR_WORD)ltemp2;				
		MoveRectange(temp, ulc, lrc);
		ltemp1 = ulc.x;
		ltemp0 = ulc.y;
		ltemp3 = lrc.x;
		ltemp2 = lrc.y;
	}
	return 0;
}
*/
VOID CPrGeneral::ResetMouseClickData()
{
//	MouseClickPosn.x_pos = 0;
//	MouseClickPosn.y_pos = 0;
//	MouseClickTriggered = FALSE;
}


LONG CPrGeneral::CaptureLiveVideoImage(char *szFileFullPathName, BOOL bCaptureGraphic)
{
	char szDrive[_MAX_DRIVE];
	char szPath[_MAX_PATH];
	char szFileName[_MAX_PATH];
	char ext[_MAX_PATH];
	char DestPath[_MAX_PATH + 40];

	_splitpath(szFileFullPathName, szDrive, szPath, szFileName, ext);
	sprintf_s(DestPath, 300, "%s%s", szDrive, szPath);
	
	if (_access(DestPath, F_OK) != F_OK)
	{
		if (!CreateDirectory(DestPath, NULL))
		{
			return TRUE;
		}
	}

	if (!bCaptureGraphic)
	{
		PR_SAVE_DISP_IMG_CMD	stCmd;
		PR_SAVE_DISP_IMG_RPY	stRpy;

		PR_InitSaveDispImgCmd(&stCmd);
		_mbscpy_s(stCmd.aubSourceLogFileNameWithPath, 100, (PR_UBYTE*)szFileFullPathName);
		PR_SaveDispImgCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
		if (PR_ERROR_STATUS(stRpy.stStatus.uwCommunStatus) || PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
		{
			return TRUE;
		}
	}
	else
	{
		PR_SERVICE_LOG_CMD	stCmd;
		PR_SERVICE_LOG_RPY	stRpy;
		PR_InitServiceLogCmd(&stCmd);
		stCmd.emServiceMode = PR_SERVICE_MODE_GRAB_DISP_IMG_WITH_GRAPHICS;
		_mbscpy_s(stCmd.aubSourceLogFileNameWithPath, 100, (PR_UBYTE*)szFileFullPathName);
		PR_ServiceLogCmd(&stCmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &stRpy);
		if (PR_ERROR_STATUS(stRpy.stStatus.uwCommunStatus) || PR_ERROR_STATUS(stRpy.stStatus.uwPRStatus))
		{
			return TRUE;
		}
	}
	return F_OK;
}


LONG CPrGeneral::DrawManualAlignSpeed(int speed)
{
	char szDisplay[40];	
	
	strcpy_s(szDisplay, 40, "Manual Align:");
	if (speed == 0)
	{
		strcat_s(szDisplay, 20, " (Slow)");
	}
	else if (speed == 1)
	{
		strcat_s(szDisplay, 20, " (Medium)");
	}
	else
	{
		strcat_s(szDisplay, 20, " (Fast)");
	}
	PR_DISPLAY_CHAR_CMD cmd;
	PR_DISPLAY_CHAR_RPY rpy;
	cmd.pubChar = (unsigned char*)szDisplay;
	cmd.tcoPosition.x = 2;
	cmd.tcoPosition.y = 19;
	cmd.emColor = PR_COLOR_GREEN;
	cmd.uwNChar = (PR_UWORD)strlen((char*)cmd.pubChar);
	PR_DisplayCharCmd(&cmd, DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, &rpy);			
	return 0;
}

VOID CPrGeneral::DrawRectangle(long TmplUpperLeftX, long TmplUpperLeftY, long TmplLowerRightX, long TmplLowerRightY, BOOL bDraw)
{
	PR_COORD ulc, lrc;

	ulc.x = (PR_WORD)TmplUpperLeftX;
	ulc.y = (PR_WORD)TmplUpperLeftY;
	lrc.x = (PR_WORD)TmplLowerRightX;
	lrc.y = (PR_WORD)TmplLowerRightY;
	wpr_draw_box(ulc, lrc, bDraw);
	PRDrawLine(DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, UP_LF_X, (TmplUpperLeftY + TmplLowerRightY) / 2 , LO_RT_X, (TmplUpperLeftY + TmplLowerRightY) / 2, bDraw ? PR_COLOR_RED : PR_COLOR_TRANSPARENT);
	PRDrawLine(DUMMY_PR_SEND_ID, DUMMY_PR_RECEIVE_ID, (TmplUpperLeftX + TmplLowerRightX) / 2, UP_LF_Y, (TmplUpperLeftX + TmplLowerRightX) / 2, LO_RT_Y, bDraw ? PR_COLOR_RED : PR_COLOR_TRANSPARENT);
}


BOOL CPrGeneral::VerifyPRRegion(PR_WIN *stInputArea, const PR_WORD sPRWinULX, const PR_WORD sPRWinULY, const PR_WORD sPRWinLRX, const PR_WORD sPRWinLRY)
{
	BOOL bOK = TRUE;
	// Check negative because negative may be greater than positve in bit
	// eg -29870 > 8136
	if (stInputArea->coCorner1.x < sPRWinULX || stInputArea->coCorner1.x < 0)
	{
		stInputArea->coCorner1.x = sPRWinULX;
		bOK = FALSE;
	}
	
	if (stInputArea->coCorner1.y < sPRWinULY || stInputArea->coCorner1.y < 0)
	{
		stInputArea->coCorner1.y = sPRWinULY;
		bOK = FALSE;
	}

	if (stInputArea->coCorner2.x > sPRWinLRX || stInputArea->coCorner2.x < 0)
	{
		stInputArea->coCorner2.x = sPRWinLRX;
		bOK = FALSE;
	}

	if (stInputArea->coCorner2.y > sPRWinLRY || stInputArea->coCorner2.y < 0)
	{
		stInputArea->coCorner2.y = sPRWinLRY;
		bOK = FALSE;
	}

	return bOK;
}

BOOL CPrGeneral::VerifyPRPoint(PR_COORD *stPoint, const PR_WORD sPRWinULX, const PR_WORD sPRWinULY, const PR_WORD sPRWinLRX, const PR_WORD sPRWinLRY)
{
	BOOL bOK = TRUE;

	if (stPoint->x < sPRWinULX)
	{
		stPoint->x = sPRWinULX;
		bOK = FALSE;
	}
	
	if (stPoint->y < sPRWinULY)
	{
		stPoint->y = sPRWinULY;
		bOK = FALSE;
	}

	if (stPoint->x > sPRWinLRX)
	{
		stPoint->x = sPRWinLRX;
		bOK = FALSE;
	}

	if (stPoint->y > sPRWinLRY)
	{
		stPoint->y = sPRWinLRY;
		bOK = FALSE;
	}

	return TRUE;
}


//================================================================================================================
//  PR Zoom Sensor Command
//================================================================================================================
LONG CPrGeneral::PRS_SensorZoom(const PR_CAMERA emCurCamera, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID,
								const PR_SET_VIDEO emType, const PR_ZOOM_SENSOR_MODE emZoomMode, 
								const PR_ZOOM_SENSOR_METHOD emZoomMethod, const PR_CONT_ZOOM_SENSOR_PARA stContZoomPar, 
								const PR_REAL_ROI stROI,
								PR_REAL_ROI *RpyROI, CString &szErrMsg)
{
#ifdef OFFLINE
	return 0;
#endif
	// Example 1 (Typical usage, what you view is exactly what you grab)
	//		Live Video Display = Snapshot = #X_ZOOM_NORMAL , where # = 1, 2 or 4
	PR_ZOOM_SENSOR_CMD	stCmd;
	PR_ZOOM_SENSOR_RPY	stRpy;
	PR_InitZoomSensorCmd(&stCmd);

	stCmd.stROI.rcoCenter = stROI.rcoCenter;
	stCmd.stROI.rszSize = stROI.rszSize;
	//stCmd.emIsSA		= PR_FALSE;
	stCmd.ulStationID = ubReceiverID;

	stCmd.emCamera = emCurCamera;

	stCmd.emType = emType; // Zoom Sensor setting applys to both "Live Video" and "snapshot"
	stCmd.emZoomMode = emZoomMode;
	stCmd.emZoomMethod = emZoomMethod;
	if (stCmd.emZoomMethod == PR_CONTINUOUS_ZOOM_SENSOR)
	{
		stCmd.stContZoomPar = stContZoomPar;
	}
	
	PR_ZoomSensorCmd(&stCmd, ubSenderID, ubReceiverID, &stRpy);

	if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR)
	{
		//andrewng //2020-0616
		szErrMsg.Format("Zoom Sensor error COM-ERR = 0x%x (CAM=%d, SenID=0x%x, RecID=0x%x, ZOOM-MODE=%d)",
						stRpy.stStatus.uwCommunStatus, emCurCamera, ubSenderID, ubReceiverID, emZoomMode);
		return 1;
	}
	if (stRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
	{
		//andrewng //2020-0616
		szErrMsg.Format("Zoom Sensor error PR-ERR = 0x%x (CAM=%d, SenID=0x%x, RecID=0x%x, ZOOM-MODE=%d)",
						stRpy.stStatus.uwPRStatus, emCurCamera, ubSenderID, ubReceiverID, emZoomMode);
		return 2;
	}
	*RpyROI = stRpy.stAdjustedROI;
	return 0;
}

//================================================================================================================
//  PR digital Zoom command
//================================================================================================================
VOID CPrGeneral::DigitalZoomScreen(const short hZoomFactor, const PR_WORD sPRCenterX, const PR_WORD sPRCenterY, const PR_UBYTE ubSenderID, const PR_UBYTE ubReceiverID)
{
	PR_SET_DISPLAY_MAGNIFY_FACTOR_CMD	stCmd;
	PR_SET_DISPLAY_MAGNIFY_FACTOR_RPY	stRpy;

	PR_InitSetDisplayMagnifyFactorCmd(&stCmd);

	PR_ZOOM_MODE	emMode = PR_ZOOM_MODE_NONE;
	PR_ZOOM_FACTOR	emFactor = PR_ZOOM_FACTOR_1X;

	if (hZoomFactor > 0)
	{
		emMode = PR_ZOOM_MODE_IN;
	}
	else if (hZoomFactor < 0)
	{
		emMode = PR_ZOOM_MODE_OUT;
	}

	switch (hZoomFactor)
	{
	case 1:
		emFactor = PR_ZOOM_FACTOR_2X;
		break;
	case 2:
		emFactor = PR_ZOOM_FACTOR_4X;
		break;
	case 3:
		emFactor = PR_ZOOM_FACTOR_8X;
		break;
	case -1:
		emFactor = PR_ZOOM_FACTOR_2X;
		break;
	case -2:
		emFactor = PR_ZOOM_FACTOR_4X;
		break;
	case -3:
		emFactor = PR_ZOOM_FACTOR_8X;
		break;
	}

	stCmd.coZoomRegionCenter.x = sPRCenterX;
	stCmd.coZoomRegionCenter.y = sPRCenterY;
	stCmd.emZoomMode	= emMode;
	stCmd.emZoomFactor	= emFactor;
	PR_SetDisplayMagnifyFactorCmd(&stCmd, ubSenderID, ubReceiverID, &stRpy);
}


VOID CPrGeneral::DrawSmallCursor(PR_UBYTE ubSID, PR_UBYTE ubRID, LONG lCrossX, LONG lCrossY)
{
	PR_DRAW_SMALL_CURSOR_CMD centredraw ;
	centredraw.coPosition.x = (PR_WORD)lCrossX;
	centredraw.coPosition.y = (PR_WORD)lCrossY;
	PR_DRAW_SMALL_CURSOR_RPY drawpstRpy ;
	PR_DrawSmallCursorCmd( &centredraw, ubSID, ubRID, &drawpstRpy);
}

BOOL CPrGeneral::InitDisplayVideo(BOOL bOn, PR_UBYTE ubSID, PR_UBYTE ubRID, PR_PURPOSE emPurpose)
{
	PR_DISPLAY_VIDEO_CMD		stDisplayVideoCmd;
	PR_DISPLAY_VIDEO_RPY		stRpy;
	stDisplayVideoCmd.emEnableVideo = PR_FALSE;

	PR_InitDisplayVideoCmd(&stDisplayVideoCmd);

	// Display live video
	stDisplayVideoCmd.emDisplayCursor = (PR_BOOLEAN)bOn;
	stDisplayVideoCmd.emPurpose = emPurpose;
	PR_DisplayVideoCmd(&stDisplayVideoCmd, ubSID, ubRID, &stRpy);
	if (stRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stRpy.uwPRStatus))
	{
		 printf("PR_DisplayVideoCmd() is finished with status %u, %u!\n", stRpy.uwCommunStatus, stRpy.uwPRStatus);
		return FALSE;
	}

	return TRUE;
}


BOOL CPrGeneral::PreEpoxyShow(const PR_COORD stULCorner, const PR_COORD stLRCorner, const PR_COORD prcenter,
							   const PR_UBYTE ubSID, const PR_UBYTE ubRID, const PR_PURPOSE emPurpose, const LONG lThreshold,
							   PR_PRE_EPOXY_SHOW_CMD &stEpoxyShowCmd, CString &szErrMsg)
{
	PR_COMMON_RPY                 stCommonRpy;
//	PR_PRE_EPOXY_SHOW_CMD         stEpoxyShowCmd;
	PR_PRE_EPOXY_SHOW_RPY2        stEpoxyShowRpy2;

	PR_InitPreEpoxyShowCmd(&stEpoxyShowCmd);

	stEpoxyShowCmd.stWin.coCorner1		= stULCorner;//Input Window
	stEpoxyShowCmd.stWin.coCorner2		= stLRCorner;
	stEpoxyShowCmd.emIsGrabFromPurpose	= PR_TRUE;
	stEpoxyShowCmd.aemPurpose[0]		= emPurpose;
	stEpoxyShowCmd.emSubType			= (PR_OBJ_TYPE)PR_OBJ_TYPE_BGA_SUBSTRATE;	// (Bright Background Dark Object)PR_OBJ_TYPE_HORIZONTAL_LEADFRAME,
	stEpoxyShowCmd.emEpoxyInspAlg		= PR_SINGLE_DOT_WO_NOISE;
	// (Dark Background Bright Object)PR_OBJ_TYPE_BGA_SUBSTRATE;
	//stEpoxyShowCmd.emEpoxyInspAlg = (PR_EPOXY_INSP_ALG) PR_SINGLE_DOT_EPOXY_INSP;
	stEpoxyShowCmd.emEpoxySegMode		= (PR_PRE_EPOXY_SEG_MODE) PR_PRE_EPOXY_SEG_MODE_MANUAL_THRES;
	stEpoxyShowCmd.emDisplayInfo		= PR_DISPLAY_INFO_BINARY;
	stEpoxyShowCmd.coRefPoint			= prcenter;	

	//LONG lThreshold						=  126;
	stEpoxyShowCmd.ubThreshold			= (PR_UBYTE) lThreshold;		// Threshold (0--255)

	stEpoxyShowCmd.ubThreshold = (PR_UBYTE)lThreshold;				// Threshold (0--255);
	PR_PreEpoxyShowCmd(&stEpoxyShowCmd, ubSID, ubRID, &stCommonRpy);
	if (stCommonRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCommonRpy.uwPRStatus))
	{
		szErrMsg.Format("PR_PreEpoxyShowCmd Error1: COMM = %d, PR = %d", stCommonRpy.uwCommunStatus, stCommonRpy.uwPRStatus);
		return FALSE;
	}

	PR_PreEpoxyShowRpy(ubSID, &stEpoxyShowRpy2);
	if (stEpoxyShowRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stEpoxyShowRpy2.stStatus.uwPRStatus))
	{
		szErrMsg.Format("PR_PreEpoxyShowRpy Error1: COMM = %d, PR = %d", stEpoxyShowRpy2.stStatus.uwCommunStatus, stEpoxyShowRpy2.stStatus.uwPRStatus);
		return FALSE;
	}

	return TRUE;
}


BOOL CPrGeneral::LearnEpoxyExt(const PR_COORD prcenter, const PR_PRE_EPOXY_SHOW_CMD stEpoxyShowCmd, const PR_UBYTE ubSID, const PR_UBYTE ubRID,
								PR_UWORD &uwRecordID, PR_AREA &aeEpoxyArea, CString &szErrMsg)
{
	PR_COMMON_RPY           stCommonRpy;
	PR_LRN_EPOXY_EXT_CMD	stLrnEpoxyCmd;
    PR_LRN_EPOXY_EXT_RPY2	stLrnEpoxyRpy2;

	uwRecordID	= 0;
	aeEpoxyArea = 0;

	PR_InitLrnEpoxyExtCmd(&stLrnEpoxyCmd);
	stLrnEpoxyCmd.emLatchMode			= PR_LATCH_NO_LATCH;
	stLrnEpoxyCmd.emIsGrabFromPurpose	= PR_TRUE;
	stLrnEpoxyCmd.aemPurpose[0]			= stEpoxyShowCmd.aemPurpose[0];

	stLrnEpoxyCmd.emEpoxyAttribute		= (stEpoxyShowCmd.emSubType == PR_OBJ_TYPE_HORIZONTAL_LEADFRAME) ? PR_OBJ_ATTRIBUTE_DARK : PR_OBJ_ATTRIBUTE_BRIGHT;
	stLrnEpoxyCmd.emEpoxyInspAlg		= stEpoxyShowCmd.emEpoxyInspAlg;
	stLrnEpoxyCmd.emEpoxySegMode		= stEpoxyShowCmd.emEpoxySegMode;
	stLrnEpoxyCmd.ubThreshold			= stEpoxyShowCmd.ubThreshold;
	stLrnEpoxyCmd.leMax					= stEpoxyShowCmd.leUpperLimit;
	stLrnEpoxyCmd.leMin					= stEpoxyShowCmd.leLowerLimit;

	stLrnEpoxyCmd.stOpRegion.uwNumOfCorners					= PR_NO_OF_CORNERS;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_LEFT]		= stEpoxyShowCmd.stWin.coCorner1;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_RIGHT]		= stEpoxyShowCmd.stWin.coCorner2;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].x	= stEpoxyShowCmd.stWin.coCorner2.x;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_UPPER_RIGHT].y	= stEpoxyShowCmd.stWin.coCorner1.y;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].x	= stEpoxyShowCmd.stWin.coCorner1.x;
	stLrnEpoxyCmd.stOpRegion.acoCorners[PR_LOWER_LEFT].y	= stEpoxyShowCmd.stWin.coCorner2.y;
	stLrnEpoxyCmd.coRefPoint = prcenter;		//Image Center (4096,4096)


	PR_LrnEpoxyExtCmd(&stLrnEpoxyCmd, ubSID, ubRID, &stCommonRpy);
	stLrnEpoxyCmd.emEpoxyInspAlg	= PR_SINGLE_DOT_WO_NOISE;

    if (stCommonRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCommonRpy.uwPRStatus))
    {
		szErrMsg.Format("PR_LrnEpoxyExtCmd Error1: COMM = %d, PR = %d", stCommonRpy.uwCommunStatus, stCommonRpy.uwPRStatus);
		return FALSE;
    }
    
	PR_LrnEpoxyExtRpy(ubSID, &stLrnEpoxyRpy2);
	   
	if (stLrnEpoxyRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || 
		PR_ERROR_STATUS(stLrnEpoxyRpy2.stStatus.uwPRStatus))
    {
		szErrMsg.Format("PR_LrnEpoxyExtRpy Error1: COMM = %d, PR = %d", stLrnEpoxyRpy2.stStatus.uwCommunStatus, stLrnEpoxyRpy2.stStatus.uwPRStatus);
		return FALSE;
    }

	uwRecordID	= stLrnEpoxyRpy2.uwRecordID;
	aeEpoxyArea	= stLrnEpoxyRpy2.aeEpoxyArea;

	szErrMsg.Format("Epoxy is learnt: ID = %d, Size = %ld", stLrnEpoxyRpy2.uwRecordID, stLrnEpoxyRpy2.aeEpoxyArea);
	return TRUE;
}
    
   
PR_WORD CPrGeneral::SearchEpoxyExt(const LONG lPRID, const DOUBLE dMinEpoxySize, const DOUBLE dMaxEpoxySize, const PR_COORD prcenter, 
								   const BOOL bWholeAreaChk, const BOOL bWholeShapeChk, const BOOL bPositionChk,
								   const BOOL bDisplayGraphicInfo, const BOOL bLatch,
								   const PR_UBYTE ubSID, const PR_UBYTE ubRID, 
								   UINT &nX, UINT &nY, ULONG &ulEpoxyAreaSize, CEvent *pevGrabImageReady, CString &szErrMsg)
{
	PR_INSP_EPOXY_EXT_CMD               stInspEpoxyCmd;
	PR_INSP_EPOXY_EXT_RPY2              stInspEpoxyRpy2;
	PR_COMMON_RPY					    stCommonRpy;

	PR_InitInspEpoxyExtCmd(&stInspEpoxyCmd);
	stInspEpoxyCmd.emGraphicInfo = bDisplayGraphicInfo ? PR_DISPLAY_CENTRE : PR_NO_DISPLAY;
	stInspEpoxyCmd.uwRecordID = (PR_UWORD)lPRID;//(PR_UWORD)stLrnEpoxyRpy2.uwRecordID;
	stInspEpoxyCmd.coRefPoint = prcenter;	  //Image Center (4096,4096)
	stInspEpoxyCmd.emIsWholeAreaChk		= bWholeAreaChk ? PR_TRUE : PR_FALSE;
	stInspEpoxyCmd.emIsWholeShapeChk	= bWholeShapeChk ? PR_TRUE : PR_FALSE;
	stInspEpoxyCmd.emIsPositionChk		= bPositionChk ? PR_TRUE : PR_FALSE;
	stInspEpoxyCmd.emLatchMode = bLatch ? PR_LATCH_FROM_CAMERA : PR_LATCH_NO_LATCH;

	PR_InspEpoxyExtCmd(&stInspEpoxyCmd, ubSID, ubRID, &stCommonRpy);

	if (stCommonRpy.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stCommonRpy.uwPRStatus))
	{
		szErrMsg.Format("PR_InspEpoxyExtCmd Error1: COMM = %d, PR = %d", stCommonRpy.uwCommunStatus, stCommonRpy.uwPRStatus);
		return PR_FALSE;
	}

	if (pevGrabImageReady != NULL)
	{
		pevGrabImageReady->SetEvent();
	}
	PR_InspEpoxyExtRpy(ubSID, &stInspEpoxyRpy2);
	if (stInspEpoxyRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stInspEpoxyRpy2.stStatus.uwPRStatus))
	{
		szErrMsg.Format("PR_InspEpoxyExtRpy Error1: COMM = %d, PR = %d", stInspEpoxyRpy2.stStatus.uwCommunStatus, stInspEpoxyRpy2.stStatus.uwPRStatus);
		return PR_FALSE;
	}

	ulEpoxyAreaSize = stInspEpoxyRpy2.aeEpoxyArea;

	if ((DOUBLE)ulEpoxyAreaSize < dMinEpoxySize || (DOUBLE)ulEpoxyAreaSize > dMaxEpoxySize)
	{
		nX = 0;
		nY = 0;
		return 2;
	}
	else
	{	
		nX = (UINT) stInspEpoxyRpy2.coEpoxyCentre.x;
		nY = (UINT) stInspEpoxyRpy2.coEpoxyCentre.y;

		if (bDisplayGraphicInfo)
		{
			DrawSmallCursor(ubSID, ubRID, nX, nY);
		}
	}
	return PR_TRUE;
}
VOID CPrGeneral::SetLighting(const PR_PURPOSE emPurpose, 
							 const PR_SOURCE emCoaxID, const PR_SOURCE emRingID, const PR_SOURCE emSideID,
							 const PR_UWORD uwCoaxLevel, const PR_UWORD uwRingLevel,const PR_UWORD uwSideLevel,
							 const PR_UBYTE ubSID, const PR_UBYTE ubRID)
{
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;

	PR_GetOptic(emPurpose, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emCoaxID, uwCoaxLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emRingID, uwRingLevel, ubSID, ubRID, &stOptic, &stRpy);
	PR_SetLighting(emSideID, uwSideLevel, ubSID, ubRID, &stOptic, &stRpy);
}


BOOL CPrGeneral::SetExposureTime(const PR_PURPOSE emPurpose, const PR_EXPOSURE_TIME stTime, const PR_UBYTE ubSID, const PR_UBYTE ubRID, CString &szErrMsg)
{
	PR_COMMON_RPY		stRpy;
	PR_OPTIC			stOptic;

	PR_GetOptic(emPurpose, ubSID, ubRID, &stOptic, &stRpy);
	if ((stRpy.uwCommunStatus == PR_COMM_NOERR) && (stRpy.uwPRStatus == PR_ERR_NOERR))
	{
		PR_SetExposureTime(stTime, ubSID, ubRID, &stOptic, &stRpy);

		if ((stRpy.uwCommunStatus == PR_COMM_NOERR) && (stRpy.uwPRStatus == PR_ERR_NOERR))
		{
			return TRUE;
		}
		szErrMsg.Format("PR_SetExposureTime ERROR: COMM = %d, PR = %d", stRpy.uwCommunStatus, stRpy.uwPRStatus);
	}
	else
	{
		szErrMsg.Format("PR_SetExposureTime (PR_GetOptic) ERROR: COMM = %d, PR = %d", stRpy.uwCommunStatus, stRpy.uwPRStatus);
	}

	return FALSE;
}


BOOL CPrGeneral::GetExposureTime(const PR_PURPOSE emPurpose, const PR_UBYTE ubSID, const PR_UBYTE ubRID, PR_EXPOSURE_TIME &stTime, CString &szErrMsg)
{
	PR_COMMON_RPY	stRpy;
	PR_OPTIC		stOptic;

	PR_GetOptic(emPurpose, ubSID, ubRID, &stOptic, &stRpy);

	if ((stRpy.uwCommunStatus == PR_COMM_NOERR) && (stRpy.uwPRStatus == PR_ERR_NOERR))
	{
		PR_GetExposureTime(&stOptic, ubSID, ubRID, &stTime, &stRpy);

		if ((stRpy.uwCommunStatus == PR_COMM_NOERR) && (stRpy.uwPRStatus == PR_ERR_NOERR))
		{
			return TRUE;
		}
		szErrMsg.Format("PR_GetExposureTime ERROR: COMM = %d, PR = %d", stRpy.uwCommunStatus, stRpy.uwPRStatus);
	}
	else
	{
		szErrMsg.Format("PR_GetExposureTime (PR_GetOptic) ERROR: COMM = %d, PR = %d", stRpy.uwCommunStatus, stRpy.uwPRStatus);
	}

	return FALSE;
}


//===========================================================================================================================
//													PRID
//===========================================================================================================================
VOID CPrGeneral::FreePRRecord(LONG &lPRID, const PR_UBYTE ubSID, const PR_UBYTE ubRID)
{
	PR_UWORD uwRecordID = (PR_UWORD)lPRID;
	if (lPRID > 0)
	{
		FreePRRecord(uwRecordID, ubSID, ubRID);
	}
	lPRID = 0;
}


VOID CPrGeneral::FreePRRecord(PR_UWORD &uwRecordID, const PR_UBYTE ubSID, const PR_UBYTE ubRID)
{
	PR_FREE_RECORD_ID_CMD		stFreeCmd;
	PR_FREE_RECORD_ID_RPY		stFreeCmdRpy;
	PR_InitFreeRecordIDCmd(&stFreeCmd);
	stFreeCmd.uwRecordID = uwRecordID;
	PR_FreeRecordIDCmd(&stFreeCmd, ubSID, ubRID, &stFreeCmdRpy);

	uwRecordID = 0;
}


VOID CPrGeneral::FreeAllPRRecord(const PR_UBYTE ubSID, const PR_UBYTE ubRID)
{
	PR_FREE_ALL_RECORDS_RPY		stFreeAllCmdRpy;
	PR_FreeAllRecordsCmd(ubSID, ubRID, &stFreeAllCmdRpy);
}


PR_UWORD CPrGeneral::UploadPRRecord(const PR_ULWORD ulRecordID, CHAR acFilename[PR_MAX_FILE_PATH_LENGTH], const PR_UBYTE ubSID, const PR_UBYTE ubRID, CString &szErrMsg)
{
	PR_UPLOAD_RECORD_PROCESS_CMD	stCmd;
	PR_UPLOAD_RECORD_PROCESS_RPY	stRpy;
	BOOL bReturn=TRUE;
	CString szPath;
	CString szLog;

	PR_InitUploadRecordProcessCmd(&stCmd);

	stCmd.ulRecordID = ulRecordID;
	stCmd.emIsOverwrite = PR_TRUE;
//	stCmd.emZipOn = PR_FALSE;

	strcpy_s((char*)stCmd.acFilename, sizeof(stCmd.acFilename), acFilename);
	PR_UploadRecordProcessCmd(&stCmd, ubSID, ubRID, &stRpy);

	if ( stRpy.uwPRStatus == PR_ERR_NO_PIDREC )		// Given record ID not exist
	{
		return PR_ERR_NO_PIDREC;
	} 
	else if (stRpy.uwCommunStatus != PR_COMM_NOERR || stRpy.uwPRStatus != PR_ERR_NOERR)
	{
		szErrMsg.Format("Upload PR record %ld error! = %x, %x", stCmd.ulRecordID,
						stRpy.uwCommunStatus,stRpy.uwPRStatus);
		if (stRpy.uwCommunStatus != PR_COMM_NOERR)
		{
			return stRpy.uwCommunStatus;
		}
		return stRpy.uwPRStatus;
	}
	return PR_ERR_NOERR;
}


PR_ULWORD CPrGeneral::DownloadPRRecord(const PR_ULWORD ulRecordID, CHAR acFilename[PR_MAX_FILE_NAME_LENGTH], const PR_UBYTE ubSID, const PR_UBYTE ubRID, CString &szErrMsg)
{
	PR_DOWNLOAD_RECORD_PROCESS_CMD	stCmd;
	PR_DOWNLOAD_RECORD_PROCESS_RPY	stRpy;

	PR_InitDownloadRecordProcessCmd(&stCmd);

	stCmd.ulRecordID = ulRecordID;			// Let WinEagle to Auto-assign record ID
	//stCmd.ulRecordID = i + 41;	// Auto-assign record ID

	stCmd.emIsOverwrite = PR_TRUE;
//	stCmd.emIsOverwrite = PR_FALSE;
//	stCmd.emZipOn = PR_FALSE;
	strcpy_s((char*)stCmd.acFilename, sizeof(stCmd.acFilename), acFilename);
//	stCmd.emFileType = PR_DOWNLOAD_RECORD_FILE_TYPE_RECORD;
//	stCmd.wIsBinOnly = 0;
//	stCmd.emFileMode = PR_FILE_MODE_REMOTE;
//	stCmd.emIsCreateDevFile = PR_FALSE;
//	stCmd.emIsCreateCriteriaFile = PR_FALSE;

	PR_DownloadRecordProcessCmd(&stCmd, ubSID, ubRID, &stRpy);
	if (stRpy.stStatus.uwCommunStatus != PR_COMM_NOERR || stRpy.stStatus.uwPRStatus != PR_ERR_NOERR)
	{
		szErrMsg.Format("Download PR record file %s error! = %x, %x",
						acFilename,	stRpy.stStatus.uwCommunStatus,stRpy.stStatus.uwPRStatus);

		return 0;
	}
		
	return stRpy.ulRetRecordID;
}


PR_UWORD CPrGeneral::DisplayLearnInfo(const LONG lPrID, const LONG lPrCenterX, const LONG lPrCenterY, const PR_UBYTE ubSID, const PR_UBYTE ubRID)
{
	PR_DISP_LRN_INFO_CMD	stLrntCmd;
	PR_DISP_LRN_INFO_RPY	stLrntRpy;

	PR_InitDispLrnInfoCmd(&stLrntCmd);
	stLrntCmd.uwRecordID			= (PR_UWORD)(lPrID);
	stLrntCmd.emDispLrnInfoLevel	= PR_DISP_LRN_INFO_LEVEL_DETAIL;
	stLrntCmd.coRefPoint.x			= (PR_WORD)lPrCenterX;
	stLrntCmd.coRefPoint.y			= (PR_WORD)lPrCenterY;
	stLrntCmd.rRefAngle				= 0.0;
	stLrntCmd.emIsRefPointAligned	= PR_FALSE;
	stLrntCmd.emIsOverlayLrn		= PR_TRUE;

	PR_DispLrnInfoCmd(&stLrntCmd, ubSID, ubRID, &stLrntRpy);
	return stLrntRpy.uwPRStatus;
}


//===============================================================================================================================
//  Extract Circle Object
//==============================================================================================================================
LONG CPrGeneral::ExtractCircleObject(PR_UWORD lPRID, PR_UBYTE sid, PR_UBYTE rid, PR_PURPOSE emPurpose, PR_REAL rPassScorePer, 
							  PR_RCOORD rcoDieCentre, PR_LENGTH len_x, PR_LENGTH len_y, PR_RCOORD &rcoResultDieCentre)
{
	PR_EXTRACT_OBJ_CMD		stExtractCmd;
	PR_EXTRACT_OBJ_RPY1		stExtractRpy1;
	PR_EXTRACT_OBJ_RPY2		stExtractRpy2;

	PRClearScreenNoCursor(sid, rid);

	PR_InitExtractObjCmd(&stExtractCmd);

	stExtractCmd.stLatchPar.emLatch		=	PR_LATCH_FROM_CAMERA;
	stExtractCmd.stLatchPar.emGrab		=	PR_GRAB_FROM_RECORD_ID;
	stExtractCmd.stLatchPar.uwRecordID	=	lPRID;

	stExtractCmd.stLatchPar.emPurpose	=	emPurpose;
	stExtractCmd.rPassScore				=	(PR_REAL)rPassScorePer;

	stExtractCmd.emShapeType = PR_OBJSHAPE_TYPE_CIRCLE;
	stExtractCmd.emObjAttribute = PR_OBJ_ATTRIBUTE_DARK;
	stExtractCmd.emGraphicInfo = PR_SHOW_SEARCH_CONTOUR;

	stExtractCmd.stOpRegion.uwNumOfCorners = 4;
	stExtractCmd.stOpRegion.acoCorners[0].x = (PR_WORD)(rcoDieCentre.x + _round((double)len_x / 2));
	stExtractCmd.stOpRegion.acoCorners[0].y = (PR_WORD)(rcoDieCentre.y - _round((double)len_y / 2));

	stExtractCmd.stOpRegion.acoCorners[1].x = (PR_WORD)(rcoDieCentre.x - _round((double)len_x / 2));
	stExtractCmd.stOpRegion.acoCorners[1].y = (PR_WORD)(rcoDieCentre.y - _round((double)len_y / 2));

	stExtractCmd.stOpRegion.acoCorners[2].x = (PR_WORD)(rcoDieCentre.x - _round((double)len_x / 2));
	stExtractCmd.stOpRegion.acoCorners[2].y = (PR_WORD)(rcoDieCentre.y + _round((double)len_y / 2));

	stExtractCmd.stOpRegion.acoCorners[3].x = (PR_WORD)(rcoDieCentre.x + _round((double)len_x / 2));
	stExtractCmd.stOpRegion.acoCorners[3].y = (PR_WORD)(rcoDieCentre.y + _round((double)len_y / 2));


	stExtractCmd.stExtractObjPar.stCircleInput.rMaxScale = 100;
	stExtractCmd.stExtractObjPar.stCircleInput.rMinScale = 50;
	double dRadius = sqrt((double)(len_x * len_x + len_y * len_y)) / 2;
	stExtractCmd.stExtractObjPar.stCircleInput.rRadius = (PR_REAL)(len_x / 5);

	PR_UWORD	uwCommunStatus = 0;
	PR_ExtractObjCmd(&stExtractCmd, sid, rid, &uwCommunStatus);
	if (uwCommunStatus != PR_COMM_NOERR)
	{
		return 300;
	}

	PR_ExtractObjRpy1(sid, &stExtractRpy1);
	if (stExtractRpy1.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stExtractRpy1.uwPRStatus))
	{
		return 301;
	}

	PR_ExtractObjRpy2(sid, &stExtractRpy2);
	if (stExtractRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stExtractRpy2.stStatus.uwPRStatus))
	{
		return 302;
	}
				
	if (stExtractRpy2.emShapeFound != PR_OBJSHAPE_FOUND_CIRCLE) 
	{
		return 303;
	}

	rcoResultDieCentre = stExtractRpy2.stResult.astCircleResult[0].rcoCentre;
	return 0;
}

LONG CPrGeneral::ExtractBlobObject(PR_UWORD lPRID, PR_UBYTE sid, PR_UBYTE rid, PR_PURPOSE emPurpose, PR_REAL rPassScorePer, 
							  PR_RCOORD rcoDieCentre, PR_LENGTH len_x, PR_LENGTH len_y, PR_RCOORD &rcoResultDieCentre)
{
	//PR_EXTRACT_OBJ_CMD		stExtractCmd;
	//PR_EXTRACT_OBJ_RPY1		stExtractRpy1;
	//PR_EXTRACT_OBJ_RPY2		stExtractRpy2;

	//PRClearScreenNoCursor(sid, rid);

	//PR_InitExtractObjCmd(&stExtractCmd);

	//stExtractCmd.stLatchPar.emLatch		=	PR_LATCH_FROM_CAMERA;
	//stExtractCmd.stLatchPar.emGrab		=	PR_GRAB_FROM_RECORD_ID;
	//stExtractCmd.stLatchPar.uwRecordID	=	lPRID;

	//stExtractCmd.stLatchPar.emPurpose	=	emPurpose;
	//stExtractCmd.rPassScore				=	(PR_REAL)rPassScorePer;

	//stExtractCmd.emShapeType = PR_OBJSHAPE_TYPE_CIRCLE;
	//stExtractCmd.emObjAttribute = PR_OBJ_ATTRIBUTE_DARK;
	//stExtractCmd.emGraphicInfo = PR_SHOW_SEARCH_CONTOUR;

	//stExtractCmd.stOpRegion.uwNumOfCorners = 4;
	//stExtractCmd.stOpRegion.acoCorners[0].x = (PR_WORD)(rcoDieCentre.x + _round((double)len_x / 2));
	//stExtractCmd.stOpRegion.acoCorners[0].y = (PR_WORD)(rcoDieCentre.y - _round((double)len_y / 2));

	//stExtractCmd.stOpRegion.acoCorners[1].x = (PR_WORD)(rcoDieCentre.x - _round((double)len_x / 2));
	//stExtractCmd.stOpRegion.acoCorners[1].y = (PR_WORD)(rcoDieCentre.y - _round((double)len_y / 2));

	//stExtractCmd.stOpRegion.acoCorners[2].x = (PR_WORD)(rcoDieCentre.x - _round((double)len_x / 2));
	//stExtractCmd.stOpRegion.acoCorners[2].y = (PR_WORD)(rcoDieCentre.y + _round((double)len_y / 2));

	//stExtractCmd.stOpRegion.acoCorners[3].x = (PR_WORD)(rcoDieCentre.x + _round((double)len_x / 2));
	//stExtractCmd.stOpRegion.acoCorners[3].y = (PR_WORD)(rcoDieCentre.y + _round((double)len_y / 2));


	//stExtractCmd.stExtractObjPar.stCircleInput.rMaxScale = 100;
	//stExtractCmd.stExtractObjPar.stCircleInput.rMinScale = 50;
	//double dRadius = sqrt((double)(len_x * len_x + len_y * len_y)) / 2;
	//stExtractCmd.stExtractObjPar.stCircleInput.rRadius = (PR_REAL)(len_x / 5);

	//PR_UWORD	uwCommunStatus = 0;
	//PR_ExtractObjCmd(&stExtractCmd, sid, rid, &uwCommunStatus);
	//if (uwCommunStatus != PR_COMM_NOERR)
	//{
	//	return 300;
	//}

	//PR_ExtractObjRpy1(sid, &stExtractRpy1);
	//if (stExtractRpy1.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stExtractRpy1.uwPRStatus))
	//{
	//	return 301;
	//}

	//PR_ExtractObjRpy2(sid, &stExtractRpy2);
	//if (stExtractRpy2.stStatus.uwCommunStatus != PR_COMM_NOERR || PR_ERROR_STATUS(stExtractRpy2.stStatus.uwPRStatus))
	//{
	//	return 302;
	//}
	//			
	//if (stExtractRpy2.emShapeFound != PR_OBJSHAPE_FOUND_CIRCLE) 
	//{
	//	return 303;
	//}

	//rcoResultDieCentre = stExtractRpy2.stResult.astCircleResult[0].rcoCentre;
	//return 0;
	return 1;
}


PR_UWORD CPrGeneral::RemoveShareImage(PR_UBYTE ubSID, PR_UBYTE ubRID, PR_ULWORD ulImageID, PR_ULWORD ulStationID)
{
	PR_REMOVE_SHARE_IMAGE_CMD stRmvCmd;
	PR_InitRemoveShareImgCmd(&stRmvCmd);

	stRmvCmd.ulImageID = ulImageID;
	stRmvCmd.emFreeAll = PR_TRUE;

	PR_REMOVE_SHARE_IMAGE_RPY stRmvRpy;
	PR_RemoveShareImgCmd(&stRmvCmd, ubSID, ubRID, &stRmvRpy);

	return stRmvRpy.uwCommunStatus;
}