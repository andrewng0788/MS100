#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"

#pragma once

/************************* Class CBinGradeData Start *************************/

class CBinGradeData: public CObject
{
	public:
		CBinGradeData();
		virtual ~CBinGradeData();

		/*************************/
		/*   Get/Set Functions   */
		/*************************/
		ULONG GetGradeCapacity();
		BOOL SetGradeCapacity(ULONG ulGradeCapacity);

		ULONG GetInputCount();
		BOOL SetInputCount(ULONG ulInputCount);

		UCHAR GetAliasGrade();
		BOOL SetAliasGrade(UCHAR ucAliasGrade);

		BOOL GetIsAssigned();
		BOOL SetIsAssigned(BOOL bIsAssigned);

		ULONG GetStatRowIndex();
		BOOL SetStatRowIndex(ULONG ulStatRowIndex);

		BOOL SetPSTNVBinGradeData(void *pvNVRAM, LONG lNVRAM_BinTable_Start, 
			LONG lNVRAM_BinTable_Size, UCHAR ucGrade);

		BOOL SetPSTNVBinGradeData_HW(void *pvNVRAM_HW,	LONG lNVRAM_BinTable_Start,
		LONG lNVRAM_BinTable_Size, UCHAR ucGrade);

		ULONG GetNVNoOfSortedDie();
		BOOL SetNVNoOfSortedDie(ULONG ulNvNoOfSortedDie);

		ULONG GetNVBlkInUse();
		BOOL SetNVBlkInUse(ULONG ulNvBlkInUse);

		BOOL GetNVIsFull();
		BOOL SetNVIsFull(BOOL bNvIsFull);

		CString GetRankID();
		VOID SetRankID(CString szRankID);

		CString GetBlockSize();
		VOID SetBlockSize(CString szBlockSize);

		/************************/
		/*   Clear Grade Info   */
		/************************/
		VOID ClrGradeInfo();

		VOID SetLotRemainingCount(ULONG ulRemainCount);
		ULONG GetLotRemainingCount();
		
		VOID SetMinFrameDieCount(ULONG ulMinFrameDieCount);
		ULONG GetMinFrameDieCount();

		VOID SetMaxFrameDieCount(ULONG ulMaxFrameDieCount);
		ULONG GetMaxFrameDieCount();

	private:
		ULONG m_ulGradeCapacity;
		UCHAR m_ucAliasGrade;
		ULONG m_ulInputCount;
		BOOL m_bIsAssigned;
		ULONG m_ulStatRowIndex;
		CString m_szRankID;
		CString m_szBlockSize;

		ULONG m_ulLotRemainingCount;	
		ULONG m_ulMinFrameDieCount;
		ULONG m_ulMaxFrameDieCount;
		
		typedef struct
		{
			ULONG ulNoOfSortedDie;
			ULONG ulBlkInUse;
			BOOL bIsFull;
		} BT_NVBINGRADEDATA;

		BT_NVBINGRADEDATA *m_pstNvBinGradeData;	
		//BT_NVBINGRADEDATA *m_pstNvBinGradeData_HW;		//v4.65A3

}; //end class CBinGradeData

/************************** Class CBinGradeData End **************************/
