#include "stdafx.h"
#include "MS896A_Constant.h"
#include "WaferPr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CWaferPr* CAdaptWaferWrapper::m_pWaferPr = NULL;

VOID CAdaptWaferWrapper::SetWaferPr(CWaferPr* pPr)
{
	m_pWaferPr = pPr;
}

////#ifdef PR_ADAPT_WAFER

PR_VVOID CAdaptWaferWrapper::MoveMotorFunc( PR_RFUNC_MOVE_WAF_TABLE_IN	const	*pstInPar,
								  PR_RFUNC_MOVE_WAF_TABLE_OUT			*pstOutPar)
{
	DOUBLE dX = (DOUBLE) pstInPar->rTargetX;
	DOUBLE dY = (DOUBLE) pstInPar->rTargetY;
	DOUBLE dT = (DOUBLE) pstInPar->rTargetTheta;
	DOUBLE dDist = (DOUBLE)pstInPar->rTargetDistance;

	LONG lX = (LONG) pstInPar->rTargetX;
	LONG lY = (LONG) pstInPar->rTargetY;
	dT		= (DOUBLE) pstInPar->rTargetTheta;
	
	LONG lEncX=0, lEncY=0;
	DOUBLE dEncT=0;

	INT nStatus = 0;
	
	if (m_pWaferPr != NULL)
	{
		nStatus = m_pWaferPr->AdaptMoveWaferTable(lX, lY, dT, lEncX, lEncY, dEncT);
	}

	pstOutPar->wStatus		= (IMG_WORD) 0;		//0 = OK
	pstOutPar->rActualX		= (IMG_REAL) lEncX;
	pstOutPar->rActualY		= (IMG_REAL) lEncY;
	pstOutPar->rActualTheta = (IMG_REAL) dEncT;
	pstOutPar->rActualDistance = (IMG_REAL) 0;

}

PR_VVOID CAdaptWaferWrapper::GetMotorFunc( PR_RFUNC_GETPOS_WAF_TABLE_IN	const	*pstInPar,
								 PR_RFUNC_GETPOS_WAF_TABLE_OUT		*pstOutPar)
{

	LONG lEncX=0, lEncY=0, lEncT=0;
	DOUBLE dTheta=0.00;

	//Get encoder values from WT
	INT nStatus = 0;
	switch (pstInPar->wMode)
	{
	case 1:
		//nStatus = m_pWaferPr->IM_SetJoystickOn(TRUE);
		//break;
	case 2:
		//nStatus = m_pWaferPr->IM_SetJoystickOn(FALSE);
		//break;
	default:
		nStatus = m_pWaferPr->GetWTStatus(lEncX, lEncY, lEncT, dTheta);
		break;
	}

	pstOutPar->wStatus			= (IMG_WORD) 0;
	pstOutPar->rPositionX		= (IMG_REAL) lEncX;
	pstOutPar->rPositionY		= (IMG_REAL) lEncY;
	pstOutPar->rPositionTheta	= (IMG_REAL) dTheta;
}

//#endif

INT CWaferPr::AdaptMoveWaferTable(CONST LONG lx, CONST LONG ly, CONST DOUBLE dT, LONG& lEncX, LONG& lEncY, DOUBLE& dEncT)
{
	typedef struct {
		LONG lX;
		LONG lY;
		DOUBLE dT;
	} WT_XYT_STRUCT2;

	IPC_CServiceMessage stMsg;
	int nConvID = 0;
	WT_XYT_STRUCT2 stXYT;
	stXYT.lX = lx;
	stXYT.lY = ly;
	stXYT.dT = dT;
	stMsg.InitMessage(sizeof(WT_XYT_STRUCT2), &stXYT);

	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "XYT_MoveAdaptWaferCallBack", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(WT_XYT_STRUCT2), &stXYT);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	//Return result back to Vision IM
	lEncX = stXYT.lX;
	lEncY = stXYT.lY;
	dEncT = stXYT.dT;
	return 0;
}


INT CWaferPr::GetWTStatus(LONG& lEncX, LONG& lEncY, LONG& lEncT, DOUBLE& dTheta)
{
	typedef struct {
		LONG lX;
		LONG lY;
		LONG lT;
		DOUBLE dT;
	} WT_XYT_STRUCT2;

	WT_XYT_STRUCT2 stXYT;
	IPC_CServiceMessage stMsg;
	int nConvID = 0;

	stXYT.lX = 0;
	stXYT.lY = 0;
	stXYT.lT = 0;
	stXYT.dT = 0;
	
	nConvID = m_comClient.SendRequest(WAFER_TABLE_STN, "AdaptWaferGetEncoderValue", stMsg);
	while(1)
	{
		if ( m_comClient.ScanReplyForConvID(nConvID, 5000) == TRUE )
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(WT_XYT_STRUCT2), &stXYT);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	lEncX	= stXYT.lX;
	lEncY	= stXYT.lY;
	lEncT	= stXYT.lT;
	dTheta	= stXYT.dT;
	return 0;
}


