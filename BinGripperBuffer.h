/////////////////////////////////////////////////////////////////
// BinLoader.h : interface of the CBinLoader class
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

#pragma once

#ifndef _BIN_GRIPPER_BUFFER_H_
#define _BIN_GRIPPER_BUFFER_H_

#include "StringMapFile.h"

#define BL_BUFFER_UPPER					TRUE
#define BL_BUFFER_LOWER					FALSE

class CBinGripperBuffer : public CObject
{
public:
	CBinGripperBuffer(); //Constructor
	~CBinGripperBuffer(); //Deconstructor

	VOID WriteData(CMapElement *pElement);
	VOID ReadData(CMapElement *pElement);

	VOID SetName(const CString szName);
	CString GetName();

	VOID SetType(const BOOL bType);
	BOOL GetType();
	BOOL IsUpperBuffer();

	VOID InitBufferInfo();

	VOID SetBufferBlock(const LONG lBufferBlock);
	BOOL IsBufferBlockExist();
	LONG GetBufferBlock();

	VOID SetBufferBarcode(const CString szBufferBarcode);
	CString GetBufferBarcode();

	VOID SetBufferInfo(const LONG lBufferBlock, const CString szBufferBarcode);

	VOID SetFrameInClampSensorName(const CString szFrameInClampSensorName);
	CString GetFrameInClampSensorName();

	VOID SetFrameJamSensorName(const CString szFrameJamSensorName);
	CString GetFrameJamSensorName();

	VOID SetFrameExistSensorName(const CString szFrameExistSensorName);
	CString GetFrameExistSensorName();

	VOID SetGripperClampName(const CString szClampName);
	CString GetGripperClampName();

	VOID SetGripperClampStatus(const BOOL bClampStatus);
	BOOL IsGripperClampStatusOn();

	VOID SetTriggerThetaHome(const BOOL bTriggerThetaHome);
	BOOL IsTriggerThetaHome();

	VOID SetContTriggerThetaHomeCounter(const LONG lContTriggerThetaHomeCounter);
	VOID AddContTriggerThetaHomeCounter();
	BOOL IsReachTriggerThetaHomeLimit();

	VOID SetMgznSlotNo(const LONG lCurrMgzn, const LONG lCurrSlot);
	LONG GetMgznNo();
	LONG GetSlotNo();

	VOID SetFromEmptyFrame(const BOOL bFromEmptyFrame);
	BOOL IsFromEmptyFrame();

	VOID SetBinFull(const BOOL bBinFull);
	BOOL IsBinFull();

	VOID SetBinFullOutputFileCreated(const BOOL bBinFullOutputFileCreated);
	BOOL IsBinFullOutputFileCreated();

	VOID SetUnloadDieGradeCount(const ULONG ulUnloadDieGradeCount);
	ULONG GetUnloadDieGradeCount();

	VOID SetAddrReadyPos_X(LONG *plReadyPos_X);
	LONG GetReadyPos_X();
	VOID SetAddrPreUnloadPos_X(LONG *lpPreUnloadPos_X);
	LONG GetPreUnloadPos_X();
	VOID SetAddrLoadMagPos_X(LONG *plLoadMagPos_X);
	LONG GetLoadMagPos_X();
	VOID SetAddrUnloadMagPos_X(LONG *plUnloadMagPos_X);
	LONG GetUnloadMagPos_X();
	VOID SetAddrBarcodePos_X(LONG *plBarcodePos_X);
	LONG GetBarcodePos_X();
	VOID SetAddrUnloadPos_X(LONG *plUnloadPos_X);
	LONG GetUnloadPos_X();

	VOID SetAddrSIS_Top1UnloadMagClampOffsetX(LONG *plSIS_Top1UnloadMagClampOffsetX);
	VOID SetAddrTop2UnloadMagClampOffsetX(LONG *plTop2UnloadMagClampOffsetX);

	LONG GetUnloadMagClampOffsetX();
public:
	long m_lBufferBlock;
	CString m_szBufferBarcode;

protected:
	long m_lCurrMgzn;
	long m_lCurrSlot;

	BOOL m_bFromEmptyFrame;
	BOOL m_bBinFull;
	BOOL m_bBinFullOutputFileCreated;
	ULONG m_ulUnloadDieGradeCount;   //it only record the counter of the current grade for WIP usage

	LONG *m_lpPreUnloadPos_X;
	LONG *m_plReadyPos_X;
	LONG *m_plLoadMagPos_X;
	LONG *m_plUnloadMagPos_X;
	LONG *m_plBarcodePos_X;
	LONG *m_plUnloadPos_X;

	LONG *m_plSIS_Top1UnloadMagClampOffsetX;
	LONG *m_plTop2UnloadMagClampOffsetX;
private:
	BOOL	m_bType; //0--BL_BUFFER_LOWER, 1--BL_BUFFER_UPPER
	CString m_szName;
	CString m_szFrameInClampSensorName;
	CString m_szFrameJamSensorName;
	CString m_szFrameExistSensorName;
	CString m_szClampName;
	BOOL m_bClampStatus;

	BOOL m_bTriggerThetaHome;
	LONG m_lContTriggerThetaHomeCounter;
};
#endif