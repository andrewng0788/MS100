#include "stdafx.h"
#include <Math.h>
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BinTable.h"
#include "BT_PhyBlk.h"
#include "FileUtil.h"

#pragma once

/****************************** Class CPhyBlk Start ******************************/

/***********************************/
/*     Constructor/Destructor      */
/***********************************/
CPhyBlk::CPhyBlk()
{
	m_lUpperLeftX = 0;
    m_lUpperLeftY = 0;
	m_lLowerRightX = 0;
	m_lLowerRightY = 0;
	m_ulWidth = 0;
	m_ulHeight = 0;
	m_bIsSetup = FALSE;
	m_ulNoOfBinBlkPerRow = 0;
	m_ulNoOfBinBlkPerCol = 0;
} //end constructor


CPhyBlk::~CPhyBlk()
{
} //end destructor



/***********************************/
/*        Get/Set functions        */
/***********************************/
LONG CPhyBlk::GetUpperLeftX()
{
	return m_lUpperLeftX;
} //end GetUpperLeftX


BOOL CPhyBlk::SetUpperLeftX(LONG lUpperLeftX)
{
	m_lUpperLeftX = lUpperLeftX;
	return TRUE;
} //end SetUpperLeftX
	

LONG CPhyBlk::GetUpperLeftY()
{
	return m_lUpperLeftY;
} //end GetUpperLeftY
		

BOOL CPhyBlk::SetUpperLeftY(LONG lUpperLeftY)
{
	m_lUpperLeftY = lUpperLeftY;
	return TRUE;
} //end SetUpperLeftY


LONG CPhyBlk::GetLowerRightX()
{
	return m_lLowerRightX;
} //end GetLowerRightX


BOOL CPhyBlk::SetLowerRightX(LONG lLowerRightX)
{
	m_lLowerRightX = lLowerRightX;
	return TRUE;
} //end SetLowerRightX


LONG CPhyBlk::GetLowerRightY()
{
	return m_lLowerRightY;
} //end GetLowerRightY


BOOL CPhyBlk::SetLowerRightY(LONG lLowerRightY)
{
	m_lLowerRightY = lLowerRightY;
	return TRUE;
} //end SetLowerRightY
	

ULONG CPhyBlk::GetWidth()
{
	return m_ulWidth;
} //end GetWidth
	

BOOL CPhyBlk::SetWidth()
{
	m_ulWidth = m_lLowerRightX - m_lUpperLeftX;
	return TRUE;
} //end SetWidth


ULONG CPhyBlk::GetHeight()
{
	return m_ulHeight;
} //end GetHeight
	

BOOL CPhyBlk::SetHeight()
{
	m_ulHeight = m_lLowerRightY - m_lUpperLeftY;
	return TRUE;
} //end SetHeight


BOOL CPhyBlk::GetIsSetup()
{
	return m_bIsSetup;
} //end GetIsSetup


BOOL CPhyBlk::SetIsSetup(BOOL bIsSetup)
{
	m_bIsSetup = bIsSetup;
	return TRUE;
} //end SetIsSetup


ULONG CPhyBlk::GetNoOfBinBlkPerRow()
{
	return m_ulNoOfBinBlkPerRow;
} //end GetNoOfBinBlkPerRow


BOOL CPhyBlk::SetNoOfBinBlkPerRow(ULONG ulNoOfBinBlkPerRow)
{
	m_ulNoOfBinBlkPerRow = ulNoOfBinBlkPerRow;

	return TRUE;
} //end SetNoOfBinBlkPerRow


ULONG CPhyBlk::GetNoOfBinBlkPerCol()
{
	return m_ulNoOfBinBlkPerCol;
} //end GetNoOfBinBlkPerCol


BOOL CPhyBlk::SetNoOfBinBlkPerCol(ULONG ulNoOfBinBlkPerCol)
{
	m_ulNoOfBinBlkPerCol = ulNoOfBinBlkPerCol;

	return TRUE;
} //end SetNoOfBinBlkPerCol


/***********************************/
/*          Setup functions        */
/***********************************/

ULONG CPhyBlk::CalculateBinBlkPerRow(LONG lBinBlkWidth, LONG lBinBlkPitchX, LONG lXOffset)
{	
	LONG lPhyBlkWidth;
	ULONG ulBinBlkPerRow = 0;	//Klocwork

	lPhyBlkWidth = m_lLowerRightX - m_lUpperLeftX; 

	if( (lBinBlkWidth + lBinBlkPitchX)!=0 )	// divide by zero
		ulBinBlkPerRow = (lPhyBlkWidth - lXOffset + lBinBlkPitchX) / (lBinBlkWidth + lBinBlkPitchX);

	SetNoOfBinBlkPerRow(ulBinBlkPerRow);

	return ulBinBlkPerRow;
} //end CalculateBinBlkPerRow


ULONG CPhyBlk::CalculateBinBlkPerCol(LONG lBinBlkHeight, LONG lBinBlkPitchY, LONG lYOffset)
{
	LONG lPhyBlkHeight;
	ULONG ulBinBlkPerCol = 0;	//Klocwork

	lPhyBlkHeight = m_lLowerRightY - m_lUpperLeftY;

	if( (lBinBlkHeight + lBinBlkPitchY)!=0 )	// divide by zero
		ulBinBlkPerCol = (lPhyBlkHeight - lYOffset + lBinBlkPitchY) / (lBinBlkHeight + lBinBlkPitchY);

	SetNoOfBinBlkPerCol(ulBinBlkPerCol);

	return ulBinBlkPerCol;
} //end CalculateBinBlkPerCol



/******************************* Class CPhyBlk End *******************************/