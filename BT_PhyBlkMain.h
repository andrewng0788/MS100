#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_PhyBlk.h"

#pragma once

#define BT_MAX_PHYBLK_SIZE	176		//151			//v3.83


/**************************** Class CPhyBlkMain Start ****************************/

class CPhyBlkMain: public CObject
{
	public:

		/***********************************/
		/*     Constructor/Destructor      */
		/***********************************/
		CPhyBlkMain();
		virtual ~CPhyBlkMain();

		/***********************************/
		/*        Get/Set functions        */
		/***********************************/
		ULONG GetNoOfBlk();
		BOOL SetNoOfBlk(ULONG ulNoOfBlk);
		
		ULONG GetBlkPitchX();
		BOOL SetBlkPitchX(LONG lBlkPitchX);
		
		ULONG GetBlkPitchY();
		BOOL SetBlkPitchY(LONG lBlkPitchY);

		ULONG GetMsgCode();
		BOOL SetMsgCode(ULONG ulMsgCode);

		/***********************************/
		/*     Init from file functions    */
		/***********************************/
		BOOL InitPhyBlkMainData();
		BOOL InitPhyBlkData(ULONG ulBlkId, CStringMapFile *pBTMSDfile=NULL);
		BOOL InitAllPhyBlkData();			//v4.46T15

		/***********************************/
		/*     Save to file functions      */
		/***********************************/		
		BOOL SaveAllPhyBlkSetupData(ULONG ulNoOfBlk, LONG lBlkPitchX, 
									LONG lBlkPitchY);

		BOOL SaveSinglePhyBlkSetupData(ULONG ulNoOfBlk, ULONG ulBlkToSetup, 
									   LONG lUpperLeftX, LONG lUpperLeftY,
									   LONG lLowerRightX, LONG lLowerRightY);

		/***********************************/
		/*      BinTable Map Display       */
		/***********************************/
		ULONG ConvertUpperLeftX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
								ULONG ulBlkId, LONG lDisplayResol);
		ULONG ConvertUpperLeftY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
								ULONG ulBlkId, LONG lDisplayResol);
		ULONG ConvertLowerRightX(LONG lBTXNegLimit, ULONG ulBTXOffset, 
								ULONG ulBlkId, LONG lDisplayResol);
		ULONG ConvertLowerRightY(LONG lBTYNegLimit, ULONG ulBTYOffset, 
								ULONG ulBlkId, LONG lDisplayResol);

		/*************************************/
		/*   Clear Physical Block Settings   */
		/*************************************/
		VOID ClrAllBlksSettings();
		VOID ClrSingleBlkSettings(ULONG ulBlkId);

		/***********************************/
		/*      Physical Block Setup       */
		/***********************************/
		BOOL ValidateUpperLeft(ULONG ulBlkToTeach, LONG lBlkToTeachULX, LONG lBlkToTeachULY);
		BOOL ValidateLowerRight(LONG lBlkToTeachULX, LONG lBlkToTeachULY, 
								LONG lBlkToTeachLRX, LONG lBlkToTeachLRY);
								
		/***********************************/
		/*    All Physical Blocks Setup    */
		/***********************************/
		ULONG CalculateBlkPerRow(LONG lXNegLimit, ULONG ulXPosLimit, LONG lUpperLeftX, ULONG ulBlkWidth,
									LONG lBlkPitchX);

		ULONG CalculateBlkPerCol(LONG lYNegLimit, ULONG ulYPosLimit, LONG lUpperLeftY, ULONG ulBlkHeight,
									LONG lBlkPitchY);

		BOOL SetAllULsAndLRs(ULONG ulNoOfBlk, LONG lBlkPitchX, LONG lBlkPitchY,
							LONG lUpperLeftX, LONG lUpperLeftY, LONG lLowerRightX,
							LONG lLowerRightY, ULONG ulBlkPerRow, ULONG ulBlkPerCol);

		BOOL SetupAllBlks(ULONG ulNoOfBlk, LONG lBlkPitchX, LONG lBlkPitchY,
							LONG lUpperLeftX, LONG lUpperLeftY, LONG lLowerRightX,
							LONG lLowerRightY, LONG lXNegLimit, ULONG ulXPosLimit, 
							LONG lYNegLimit, ULONG ulYPosLimit);

		BOOL SetupAllBlks_BL(ULONG ulNoOfBlk, LONG lUpperLeftX, LONG lUpperLeftY, 
							LONG lLowerRightX, LONG lLowerRightY);
		
		/*************************************/
		/*    Single Physical Block Setup    */
		/*************************************/
		LONG GrabBlkUpperLeftX(ULONG ulBlkId);
		LONG GrabBlkUpperLeftY(ULONG ulBlkId);
		LONG GrabBlkLowerRightX(ULONG ulBlkId);
		LONG GrabBlkLowerRightY(ULONG ulBlkId);

		BOOL CheckOverlappingProblem(ULONG ulBlkToTeach, LONG lBlkToTeachULX, LONG lBlkToTeachULY,
									LONG lBlkToTeachLRX, LONG lBlkToTeachLRY);

		BOOL SetULandLR(ULONG ulBlkToSetup, LONG lUpperLeftX, LONG lUpperLeftY,
							LONG lLowerRightX, LONG lLowerRightY);

		BOOL SetupSingleBlk(ULONG ulNoOfBlk, ULONG ulBlkToSetup, LONG lUpperLeftX,
							LONG lUpperLeftY, LONG lLowerRightX, LONG lLowerRightY);

		BOOL SetupSingleBlk_BL(ULONG ulNoOfBlk, ULONG ulBlkToSetup, LONG lUpperLeftX,
							   LONG lUpperLeftY, LONG lLowerRightX, LONG lLowerRightY);

		ULONG CalculateTotalNoOfBinBlksCanHold(LONG lBinBlk1ULX, LONG lBinBlk1ULY,
							LONG lBinBlkLRX, LONG lBinBlkLRY,LONG lBinBlkPitchX, 
							LONG lBinBlkPitchY);

		/*******************************************/
		/*    Supplementary for Bin Block Setup    */
		/*******************************************/
		ULONG GrabNoOfBinBlkPerRow(ULONG ulBlkId);
		ULONG GrabNoOfBinBlkPerCol(ULONG ulBlkId);
		BOOL GrabIsSetup(ULONG ulBlkId);

	private:
		ULONG m_ulNoOfBlk;     
		LONG m_lBlkPitchX;
		LONG m_lBlkPitchY;
		ULONG m_ulMsgCode;

	protected:
		CPhyBlk m_oPhyBlk[BT_MAX_PHYBLK_SIZE];

}; //end Class CPhyBlkMain 

/***************************** Class CPhyBlkMain End *****************************/