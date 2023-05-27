#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "stdafx.h"
#include "StringMapFile.h"


class CBinFrame: public CObject
{
	
	public:
		enum 
		{	SORTING_FRAME = 1,
			EMPTY_FRAME,
			FULL_FRAME
		};

		CBinFrame();
		~CBinFrame();

		CBinFrame(CONST CBinFrame& oFrameCopy);

		VOID SetFrameName(CString szFrameName);
		
		VOID SetFrameStatus(LONG lStatus);
		LONG GetFrameStatus();
		
		VOID SetFrameBlockNo(LONG lBlkNo);
		LONG GetFrameBlockNo();

		VOID SetFrameCurSlotNo(LONG lCurSlotNo);
		LONG GetFrameCurSlotNo();

		VOID SetFrameCurMagzNo(LONG lCurMagNo);
		LONG GetFrameCurMagNo();

		VOID SetFrameBarcode(CString szFrmaeBarcode);
		CString GetFrameBarcode();

		VOID UpdateFrameStatus(LONG lStatus, LONG lBlkNo, LONG lCurMagNo, LONG lCurSlotNo, CString szFrmaeBarcode);
		VOID SwapFrames(CBinFrame& oFirstFrame, CBinFrame& oSecondFrame);
		VOID ClearFrame();

		BOOL LogBinFrameData(CString szFunc);

		BOOL LoadFrameData(CString szConfigFilename);
		BOOL SaveFrameData(CString szConfigFilename);

		CBinFrame operator=(CONST CBinFrame& oFristFrame);
		
	private:

		CString m_szFrameName;
		LONG m_lBinFrmaeStatus;
		LONG m_lBinFrameBlkNo;
		LONG m_lCurrentSlotNo;
		LONG m_lCurrentMagazineNo;
		CString m_szBarcode;
}; 
