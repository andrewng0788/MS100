#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"

#pragma once

/****************************** Class CPhyBlk Start ******************************/

class CPhyBlk: public CObject
{
	public:

		/***********************************/
		/*     Constructor/Destructor      */
		/***********************************/
		CPhyBlk();
		virtual ~CPhyBlk();

		/***********************************/
		/*        Get/Set functions        */
		/***********************************/
		LONG GetUpperLeftX();
		BOOL SetUpperLeftX(LONG lUpperLeftX);

		LONG GetUpperLeftY();
		BOOL SetUpperLeftY(LONG lUpperLeftY);

		LONG GetLowerRightX();
		BOOL SetLowerRightX(LONG lLowerRightX);

		LONG GetLowerRightY();
		BOOL SetLowerRightY(LONG lLowerRightY);

		ULONG GetWidth();
		BOOL SetWidth();

		ULONG GetHeight();
		BOOL SetHeight();

		BOOL GetIsSetup();
		BOOL SetIsSetup(BOOL bIsSetup);

		ULONG GetNoOfBinBlkPerRow();
		BOOL SetNoOfBinBlkPerRow(ULONG ulNoOfBinBlkPerRow);

		ULONG GetNoOfBinBlkPerCol();
		BOOL SetNoOfBinBlkPerCol(ULONG ulNoOfBinBlkPerCol);


		/***********************************/
		/*         Setup functions         */
		/***********************************/
		ULONG CalculateBinBlkPerRow(LONG lBinBlkWidth, LONG lBinBlkPitchX, LONG lXOffset);
		ULONG CalculateBinBlkPerCol(LONG lBinBlkHeight, LONG lBinBlkPitchY, LONG lYOffset);

		

	private:
		LONG m_lUpperLeftX;
		LONG m_lUpperLeftY;
		LONG m_lLowerRightX;
		LONG m_lLowerRightY;
		ULONG m_ulWidth;
		ULONG m_ulHeight;
		BOOL m_bIsSetup;
		ULONG m_ulNoOfBinBlkPerRow;
		ULONG m_ulNoOfBinBlkPerCol;

}; //end Class CPhyBlk

/******************************* Class CPhyBlk End *******************************/
