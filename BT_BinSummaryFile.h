#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "StringMapFile.h"

#define BT_BIN_GRADE		176		//151
#define	HEADER				"Header"

#define LOTID				"LOT_ID"
#define BIN_BIN_CODE		"BIN,Bin_CODE"
#define BIN_BIN_CODE2		"Bin,Bin_CODE"		//Genesis 3F PkgSort
#define BIN_CODE			"Bin_CODE"
#define BIN_INFO			"Bin Info"
#define TOTAL_COUNT			"TotalCount"
#define TOTAL_COUNT2		"Total"				//v4.49A12
#define PKG_NAME			"Pkg_Name"			//4.53D26  

#define FRAME_INFO	"Frame Info"
#define	CURRENT_NO	"Current No"
#define TAPEID		"TapeID"
#define FRAMEID		"FrameID"
#define BINCODE		"Bin_CODE"
#define QTY			"QTY"
#define BIN			"Bin"

#define HEADER		"Header"
#define WO			"WO"
#define LOT_ID		"LOT_ID"
#define OPERATOR	"OPERATOR"

#define EOF_TOTAL_COUNT "Total count"
#define EOF_TOTAL_COUNT2 "TotalCount"
//Dynamic Grade Assign Flag
#define GRADE_ASSIGN	"(Grade Assign)"
#define GRADE_BASE_ON_BINBLK "(Grade Based On Bin Block)"
#define TRUE_STR		"1"

#define LOT_HEADER_A_WAFER_NO		"NUM_OF_WAFERS"
#define LOT_WAFER_NO				"WAFER_"
#define LOT_WAFER_LOADED			"Loaded"

typedef struct 
{
	ULONG ulDieCount;
	UCHAR ucGrade;
	unsigned short usOriginalGrade;

}BIN_FILE_INFO;

class CBinSummaryFile: public CObject
{
	public:
	
		CBinSummaryFile();

		VOID ClearData();

		BOOL GetEnableGradeAssign();

		BOOL GetSelectGradeBaseOnBinBlockGrade();

		BOOL LoadData();
		BOOL ReadBinSummaryFile(CString szFilename, BOOL bUseOptBinCountDynAssignGrade);

		BOOL GetGadeInfo(CArray<BIN_FILE_INFO, BIN_FILE_INFO>& aGradeInfo);

		VOID SetMinLotCount(ULONG ulMinLotCount);
		ULONG GetMinLotCount();

		VOID SetEnableBinSumWaferLot(BOOL bEnable);
		CString GetLastError();
		
		CString GetPkgName();
		
	private:
	
		BOOL IsEndOfFile(CString szData);
		BOOL IsContent(CString szData);
		BOOL IsGradeAssignField(CString szData);
		BOOL IsSelectGradeBaseOnBinBlockGrade(CString szData);
		
		BOOL ReadHeader(CStdioFile& cfSetupFile,CStringMapFile& BinSummaryFile);
		BOOL DecodeHeaderInfo(CStdioFile& cfSetupFile);
		BOOL SaveHeaderData(CStringMapFile& BinSummaryFile);
		
		BOOL DecodeContentHeader(CStdioFile& cfSetupFile, CStringArray& szaContentHeader);

		BOOL CheckNeedEnableGradeAssign(BOOL bUseOptBinCountDynAssignGrade);
		BOOL CheckSelectGradeBaseOnBinBlockGrade();
		
		BOOL SaveLotWaferIds(CStringArray& szaContentHeader);

		BOOL ReadConent(CStdioFile& cfSetupFile, CStringMapFile& BinSummaryFile,  CStringArray& szaContentHeader);
		BOOL UpdateAndSaveContent(CStringMapFile& BinSummaryFile, CStringArray& szaContentHeader, 
											 CStringArray& szaContent);

		BOOL CompareString(CString szStr1, CString szStr2);

		BOOL GetDataFromHeaderString(CString szData, CString& szContent);
		BOOL GetDataFromContentString(CString szData, CStringArray& szaContent);
		
		BOOL CreateWaferLotCheckDatabase();

		CStringArray m_szWaferIdsInLot;
		CArray<BIN_FILE_INFO, BIN_FILE_INFO> m_aGradeInfo;

		CMapStringToString m_szaHeaderInfo;

		ULONG	m_ulMinLotCount;
		BOOL	m_bEnableBinSumWaferLot;
		BOOL	m_bEnableGradeAssign;
		BOOL	m_bSelectGradeBaseOnBinBlockGrade;
		CString m_szErrorCode;
		CString m_szSummaryFilePkgName;

};