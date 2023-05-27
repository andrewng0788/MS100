/////////////////////////////////////////////////////////////////
// BinLoader.cpp : interface of the CBinLoader class
//
//	Description:
//		
//
//	Date:		12 August 2004
//	Revision:	1.00
//
//	By:			
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BL_STRUCT.h"
#include "BinGripperBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CBinGripperBuffer::CBinGripperBuffer()
{
	m_lBufferBlock = 0;

	m_lCurrMgzn = 0;
	m_lCurrSlot = 0;

	m_bFromEmptyFrame = FALSE;
	m_bBinFull = FALSE;
	m_bBinFullOutputFileCreated = FALSE;
	m_ulUnloadDieGradeCount = 0;

	m_bClampStatus = FALSE;

	m_bType = 0;

	//Position
	m_lpPreUnloadPos_X	= NULL;
	m_plReadyPos_X		= NULL;
	m_plLoadMagPos_X	= NULL;
	m_plUnloadMagPos_X	= NULL;
	m_plBarcodePos_X	= NULL;
	m_plUnloadPos_X		= NULL;

	m_plSIS_Top1UnloadMagClampOffsetX	= NULL;
	m_plTop2UnloadMagClampOffsetX		= NULL;

	m_bTriggerThetaHome = FALSE;
	m_lContTriggerThetaHomeCounter = 0;
}

CBinGripperBuffer::~CBinGripperBuffer()
{
}


VOID CBinGripperBuffer::WriteData(CMapElement *pElement)
{
	if (pElement)
	{
		(*pElement)["BufferBlock"]				= m_lBufferBlock;
		(*pElement)["CurrMgzn"]					= m_lCurrMgzn;
		(*pElement)["CurrSlot"]					= m_lCurrSlot;
		(*pElement)["FromEmptyFrame"]			= m_bFromEmptyFrame;
		(*pElement)["BinFull"]					= m_bBinFull;
		(*pElement)["BinFullOutputFileCreated"]	= m_bBinFullOutputFileCreated;
		(*pElement)["UnloadDieGradeCount"]		= m_ulUnloadDieGradeCount;
	}
}


VOID CBinGripperBuffer::ReadData(CMapElement *pElement)
{
	if (pElement)
	{
		m_lBufferBlock					= (LONG)(*pElement)["BufferBlock"];
		m_lCurrMgzn						= (LONG)(*pElement)["CurrMgzn"];
		m_lCurrSlot						= (LONG)(*pElement)["CurrSlot"];
		m_bFromEmptyFrame				= (BOOL)(LONG)(*pElement)["FromEmptyFrame"];
		m_bBinFull						= (BOOL)(LONG)(*pElement)["BinFull"];
		m_bBinFullOutputFileCreated		= (BOOL)(LONG)(*pElement)["BinFullOutputFileCreated"];
		m_ulUnloadDieGradeCount			= (ULONG)(LONG)(*pElement)["UnloadDieGradeCount"];
	}
}
//==============================================================================
//				Set Buffer Name
//==============================================================================
VOID CBinGripperBuffer::SetName(const CString szName)
{
	m_szName = szName;
}

CString CBinGripperBuffer::GetName()
{
	return m_szName;
}


//==============================================================================
//				Set Buffer Type
//==============================================================================
VOID CBinGripperBuffer::SetType(const BOOL bType)
{
	m_bType = bType;
}

BOOL CBinGripperBuffer::GetType()
{
	return m_bType;
}

BOOL CBinGripperBuffer::IsUpperBuffer()
{
	return (m_bType == BL_BUFFER_UPPER);
}

//==============================================================================

//===================================================================================
//			Set Buffer block&Barcode
//===================================================================================
VOID CBinGripperBuffer::InitBufferInfo()
{
	m_lBufferBlock = 0;
	m_szBufferBarcode.Empty();
	SetBinFullOutputFileCreated(FALSE);
	m_ulUnloadDieGradeCount = 0;
}

VOID CBinGripperBuffer::SetBufferInfo(const LONG lBufferBlock, const CString szBufferBarcode)
{
	m_lBufferBlock		= lBufferBlock;
	m_szBufferBarcode	= szBufferBarcode;
}


//===================================================================================
//			Set Buffer Block Id
//===================================================================================
VOID CBinGripperBuffer::SetBufferBlock(const LONG lBufferBlock)
{
	m_lBufferBlock = lBufferBlock;
}

LONG CBinGripperBuffer::GetBufferBlock()
{
	return m_lBufferBlock;
}

BOOL CBinGripperBuffer::IsBufferBlockExist()
{
	return (m_lBufferBlock > 0);
}

//===================================================================================
//			Set Barcode Name
//===================================================================================
VOID CBinGripperBuffer::SetBufferBarcode(const CString szBufferBarcode)
{
	m_szBufferBarcode = szBufferBarcode;
}

CString CBinGripperBuffer::GetBufferBarcode()
{
	return m_szBufferBarcode;
}


//===================================================================================
//			Frame In Clamp Sensor(Frame Detect)
//===================================================================================
VOID CBinGripperBuffer::SetFrameInClampSensorName(const CString szFrameInClampSensorName)
{
	m_szFrameInClampSensorName = szFrameInClampSensorName;
}

CString CBinGripperBuffer::GetFrameInClampSensorName()
{
	return m_szFrameInClampSensorName;
}

//===================================================================================
//			Frame Jam Sensor
//===================================================================================
VOID CBinGripperBuffer::SetFrameJamSensorName(const CString szFrameJamSensorName)
{
	m_szFrameJamSensorName = szFrameJamSensorName;
}

CString CBinGripperBuffer::GetFrameJamSensorName()
{
	return m_szFrameJamSensorName;
}

//===================================================================================
//			Frame Exist Sensor
//===================================================================================
VOID CBinGripperBuffer::SetFrameExistSensorName(const CString szFrameExistSensorName)
{
	m_szFrameExistSensorName = szFrameExistSensorName;
}

CString CBinGripperBuffer::GetFrameExistSensorName()
{
	return m_szFrameExistSensorName;
}


//===================================================================================
//			Gripper clamp sol
//===================================================================================
VOID CBinGripperBuffer::SetGripperClampName(const CString szClampName)
{
	m_szClampName = szClampName;
}

CString CBinGripperBuffer::GetGripperClampName()
{
	return m_szClampName;
}


//===================================================================================
//			Gripper clamp sol's status
//===================================================================================
VOID CBinGripperBuffer::SetGripperClampStatus(const BOOL bClampStatus)
{
	m_bClampStatus = bClampStatus;
}

BOOL CBinGripperBuffer::IsGripperClampStatusOn()
{
	return m_bClampStatus;
}


//===================================================================================
//			Gripper Trigger Theta Home signal
//===================================================================================
VOID CBinGripperBuffer::SetTriggerThetaHome(const BOOL bTriggerThetaHome)
{
	m_bTriggerThetaHome = bTriggerThetaHome;
}

BOOL CBinGripperBuffer::IsTriggerThetaHome()
{
	return m_bTriggerThetaHome;
}


//===================================================================================
//			Gripper Trigger Theta Home signal Counter
//===================================================================================
VOID CBinGripperBuffer::SetContTriggerThetaHomeCounter(const LONG lContTriggerThetaHomeCounter)
{
	m_lContTriggerThetaHomeCounter = lContTriggerThetaHomeCounter;
}

VOID CBinGripperBuffer::AddContTriggerThetaHomeCounter()
{
	m_lContTriggerThetaHomeCounter++;
}

BOOL CBinGripperBuffer::IsReachTriggerThetaHomeLimit()
{
	return (m_lContTriggerThetaHomeCounter == 2);
}

//===================================================================================
//			Set the Slot Number
//===================================================================================
VOID CBinGripperBuffer::SetMgznSlotNo(const LONG lCurrMgzn, const LONG lCurrSlot)
{
	m_lCurrMgzn = lCurrMgzn;
	m_lCurrSlot = lCurrSlot;
}

LONG CBinGripperBuffer::GetMgznNo()
{
	return m_lCurrMgzn;
}

LONG CBinGripperBuffer::GetSlotNo()
{
	return m_lCurrSlot;
}


//===================================================================================
//			Set the Bin Frame Source
//===================================================================================
VOID CBinGripperBuffer::SetFromEmptyFrame(const BOOL bFromEmptyFrame)
{
	m_bFromEmptyFrame = bFromEmptyFrame;
}

BOOL CBinGripperBuffer::IsFromEmptyFrame()
{
	return m_bFromEmptyFrame;
}

//===================================================================================
//			Set the Bin Full
//===================================================================================
VOID CBinGripperBuffer::SetBinFull(const BOOL bBinFull)
{
	m_bBinFull = bBinFull;
	SetBinFullOutputFileCreated(FALSE);
}

BOOL CBinGripperBuffer::IsBinFull()
{
	return m_bBinFull;
}


//===================================================================================
//			Create Output File
//===================================================================================
VOID CBinGripperBuffer::SetBinFullOutputFileCreated(const BOOL bBinFullOutputFileCreated)
{
	m_bBinFullOutputFileCreated = bBinFullOutputFileCreated;
}

BOOL CBinGripperBuffer::IsBinFullOutputFileCreated()
{
	return m_bBinFullOutputFileCreated;
}

//===================================================================================
//			Set the Bin Full
//===================================================================================
VOID CBinGripperBuffer::SetUnloadDieGradeCount(const ULONG ulUnloadDieGradeCount)
{
	m_ulUnloadDieGradeCount = ulUnloadDieGradeCount;
}

ULONG CBinGripperBuffer::GetUnloadDieGradeCount()
{
	return m_ulUnloadDieGradeCount;
}


//==============================================================================
//  Gripper Ready Position
//==============================================================================
VOID CBinGripperBuffer::SetAddrReadyPos_X(LONG *plReadyPos_X)
{
	m_plReadyPos_X = plReadyPos_X;
}


LONG CBinGripperBuffer::GetReadyPos_X()
{
	return *m_plReadyPos_X;
}

//==============================================================================
//  Gripper Unload Position
//==============================================================================
VOID CBinGripperBuffer::SetAddrPreUnloadPos_X(LONG *lpPreUnloadPos_X)
{
	m_lpPreUnloadPos_X = lpPreUnloadPos_X;
}


LONG CBinGripperBuffer::GetPreUnloadPos_X()
{
	return *m_lpPreUnloadPos_X;
}

//==============================================================================
//  Gripper Load Magazine Position
//==============================================================================
VOID CBinGripperBuffer::SetAddrLoadMagPos_X(LONG *plLoadMagPos_X)
{
	m_plLoadMagPos_X = plLoadMagPos_X;
}


LONG CBinGripperBuffer::GetLoadMagPos_X()
{
	return *m_plLoadMagPos_X;
}


//==============================================================================
//  Gripper Unload Magazine Position
//==============================================================================
VOID CBinGripperBuffer::SetAddrUnloadMagPos_X(LONG *plUnloadMagPos_X)
{
	m_plUnloadMagPos_X = plUnloadMagPos_X;
}


LONG CBinGripperBuffer::GetUnloadMagPos_X()
{
	return *m_plUnloadMagPos_X;
}

//==============================================================================
//  Gripper Barcode Position
//==============================================================================
VOID CBinGripperBuffer::SetAddrBarcodePos_X(LONG *plBarcodePos_X)
{
	m_plBarcodePos_X = plBarcodePos_X;
}

LONG CBinGripperBuffer::GetBarcodePos_X()
{
	return *m_plBarcodePos_X;
}


//==============================================================================
//  Gripper Unload Position
//==============================================================================
VOID CBinGripperBuffer::SetAddrUnloadPos_X(LONG *plUnloadPos_X)
{
	m_plUnloadPos_X = plUnloadPos_X;
}

LONG CBinGripperBuffer::GetUnloadPos_X()
{
	return *m_plUnloadPos_X;
}


//==============================================================================
//  Gripper SIS load Top1 clamp offset
//==============================================================================
VOID CBinGripperBuffer::SetAddrSIS_Top1UnloadMagClampOffsetX(LONG *plSIS_Top1UnloadMagClampOffsetX)
{
	m_plSIS_Top1UnloadMagClampOffsetX = plSIS_Top1UnloadMagClampOffsetX;
}

//==============================================================================
//  Gripper load Top2 clamp offset
//==============================================================================
VOID CBinGripperBuffer::SetAddrTop2UnloadMagClampOffsetX(LONG *plTop2UnLoadMagClampOffsetX)
{
	m_plTop2UnloadMagClampOffsetX = plTop2UnLoadMagClampOffsetX;
}


LONG CBinGripperBuffer::GetUnloadMagClampOffsetX()
{
	LONG lUnloadMagClampOffsetX = 0;

	switch (m_lCurrMgzn)
	{
	case BL_MGZN_TOP1:
		if (m_plSIS_Top1UnloadMagClampOffsetX)
		{
			lUnloadMagClampOffsetX = *m_plSIS_Top1UnloadMagClampOffsetX;
		}
		break;
	case BL_MGZN_MID1:
	case BL_MGZN_BTM1:
	case BL_MGZN8_BTM1:
		break;
	case BL_MGZN_MID2:
	case BL_MGZN_BTM2:
	case BL_MGZN8_BTM2:
	case BL_MGZN_TOP2:
		if (m_plTop2UnloadMagClampOffsetX)
		{
			 lUnloadMagClampOffsetX = *m_plTop2UnloadMagClampOffsetX;
		}
		break;
	}

	return lUnloadMagClampOffsetX;
}
