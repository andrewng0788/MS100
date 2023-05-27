#include "stdafx.h" 
#include "BinFrame.h"
#include "stdio.h"
#include "StringMapFile.h"
#include "MS896A_Constant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CBinFrame::CBinFrame()
{
	m_lBinFrmaeStatus = 0;
	m_lBinFrameBlkNo = 0;
	m_lCurrentSlotNo = 0;
	m_lCurrentMagazineNo = 0;
	m_szBarcode = "";
	
}

CBinFrame::~CBinFrame()
{

}

CBinFrame::CBinFrame(CONST CBinFrame& oFrameCopy) 
{
	m_lBinFrmaeStatus = oFrameCopy.m_lBinFrmaeStatus;
	m_lBinFrameBlkNo = oFrameCopy.m_lBinFrameBlkNo;
	m_lCurrentSlotNo = oFrameCopy.m_lCurrentSlotNo;
	m_lCurrentMagazineNo = oFrameCopy.m_lCurrentMagazineNo;
	m_szBarcode = oFrameCopy.m_szBarcode;
   
}

VOID CBinFrame::SetFrameName(CString szFrameName)
{
	m_szFrameName = szFrameName;
}

		
VOID CBinFrame::SetFrameStatus(LONG lStatus)
{
	m_lBinFrmaeStatus = lStatus;
	LogBinFrameData("Set Status");
}

LONG CBinFrame::GetFrameStatus()
{
	return m_lBinFrmaeStatus;
}
		

VOID CBinFrame::SetFrameBlockNo(LONG lBlkNo)
{
	m_lBinFrameBlkNo = lBlkNo;
	LogBinFrameData("Set Block No");
}

LONG CBinFrame::GetFrameBlockNo()
{
	return m_lBinFrameBlkNo;
}

VOID CBinFrame::SetFrameCurSlotNo(LONG lCurSlotNo)
{
	m_lCurrentSlotNo = lCurSlotNo;
	LogBinFrameData("Set Slot No");
}

LONG CBinFrame::GetFrameCurSlotNo()
{
	return m_lCurrentSlotNo;
}

VOID CBinFrame::SetFrameCurMagzNo(LONG lCurMagNo)
{
	m_lCurrentMagazineNo = lCurMagNo;
	LogBinFrameData("Set Mag No");
}

LONG CBinFrame::GetFrameCurMagNo()
{
	return m_lCurrentMagazineNo;
}

VOID CBinFrame::SetFrameBarcode(CString szFrmaeBarcode)
{
	m_szBarcode = szFrmaeBarcode;
	LogBinFrameData("Set Barcode");
}

CString CBinFrame::GetFrameBarcode()
{
	return m_szBarcode;
}

VOID CBinFrame::UpdateFrameStatus(LONG lStatus, LONG lBlkNo, LONG lCurMagNo, 
								  LONG lCurSlotNo, CString szFrmaeBarcode)
{
	m_lBinFrmaeStatus = lStatus;
	m_lBinFrameBlkNo = lBlkNo;
	m_lCurrentMagazineNo = lCurMagNo;
	m_lCurrentSlotNo = lCurSlotNo;
	m_szBarcode = szFrmaeBarcode;
	LogBinFrameData("Update");
}

CBinFrame CBinFrame::operator=(CONST CBinFrame& oFristFrame)
{
    CBinFrame oSecondFrame;

	oSecondFrame.m_lBinFrmaeStatus = oFristFrame.m_lBinFrmaeStatus;
	oSecondFrame.m_lBinFrameBlkNo = oFristFrame.m_lBinFrameBlkNo;
	oSecondFrame.m_lCurrentSlotNo = oFristFrame.m_lCurrentSlotNo;
	oSecondFrame.m_lCurrentMagazineNo = oFristFrame.m_lCurrentMagazineNo;
	oSecondFrame.m_szBarcode = oFristFrame.m_szBarcode;

    return oSecondFrame;
}


VOID CBinFrame::SwapFrames(CBinFrame& oFirstFrame, CBinFrame& oSecondFrame)
{
	CBinFrame oBinFrameTemp;

	oBinFrameTemp = oFirstFrame;
	oFirstFrame = oSecondFrame;
	oSecondFrame = oFirstFrame;

	LogBinFrameData("SFirst");
}

VOID CBinFrame::ClearFrame()
{
	m_lBinFrmaeStatus = 0;
	m_lBinFrameBlkNo = 0;
	m_lCurrentSlotNo = 0;
	m_lCurrentMagazineNo = 0;
	m_szBarcode = "";

	LogBinFrameData("ClearFrame");
}

BOOL CBinFrame::SaveFrameData(CString szConfigFilename)
{

	return TRUE;
}

BOOL CBinFrame::LoadFrameData(CString szConfigFilename)
{
	return TRUE;

}


BOOL CBinFrame::LogBinFrameData(CString szFunc)
{
	CStdioFile cfLogFile;
	
	if (cfLogFile.Open("c:\\mapsorter\\UserData\\History\\BinFrame.log", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite) == FALSE)
		return FALSE;

	cfLogFile.SeekToEnd();

	CString szMsg;

	szMsg.Format("Frame Status:%d Block No:%d Cur Mag:%d Cur Slot:%d Barcode :%s", m_lBinFrmaeStatus,
		m_lBinFrameBlkNo,m_lCurrentMagazineNo ,m_lCurrentSlotNo, m_szBarcode);

	cfLogFile.WriteString(szFunc + "-" + szMsg + "\n");

	cfLogFile.Close();
	
	return TRUE;
}
