/////////////////////////////////////////////////////////////////
// BHAirFlowCompensation.cpp : interface of the AD_CBHAirFlowCompensation class
//
//	Description:
//		
//
//	Date:		3 July 20
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
#include "BH_AirFlowCompensation.h"
#include "MathFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


AD_CBHAirFlowCompensation::AD_CBHAirFlowCompensation()
{
	Init();
}

AD_CBHAirFlowCompensation::~AD_CBHAirFlowCompensation()
{
	Init();
}


VOID AD_CBHAirFlowCompensation::Init()
{
	m_EmptyAirFlowValueList.RemoveAll();
	m_DieAirFlowValueList.RemoveAll();
	m_dBHZMissingDieThresholdPercent = 0;
	m_dBHZColletJamThresholdPercent	 = 0;

	m_ulMissingDieThresholdValue	= 0;
	m_ulColletJamThresholdValue		= 0;

	m_lContinuousMissingDieCounter  = 0;
}


VOID AD_CBHAirFlowCompensation::WriteData(CMapElement *pElement)
{
}


VOID AD_CBHAirFlowCompensation::ReadData(CMapElement *pElement)
{
}

LONG AD_CBHAirFlowCompensation::_round(double val)
{
	return CMathFunc::Instance()->_round(val);
}

VOID AD_CBHAirFlowCompensation::SetAirFlowPara(const DOUBLE dBHZMissingDieThresholdPercent, const DOUBLE dBHZColletJamThresholdPercent)
{
	m_dBHZMissingDieThresholdPercent	= dBHZMissingDieThresholdPercent;
	m_dBHZColletJamThresholdPercent		= dBHZColletJamThresholdPercent;
}


//================================================================
// Function Name: 		SetEmptyAirFlowValue
// Input arguments:		ulValue
// Output arguments:	None
// Description:   		Set the value of Air flow without die on BHZ
// Return:				None
// Remarks:				None
//================================================================
VOID AD_CBHAirFlowCompensation::SetEmptyAirFlowValue(const ULONG ulValue)
{
	if (m_EmptyAirFlowValueList.GetSize() < _MAX_SAMPLE_COUNT)
	{
		m_EmptyAirFlowValueList.Add(ulValue);
	}
}


//================================================================
// Function Name: 		SetDieAirFlowValue
// Input arguments:		ulValue
// Output arguments:	None
// Description:   		Set the value of Air flow with die on BHZ
// Return:				None
// Remarks:				None
//================================================================
VOID AD_CBHAirFlowCompensation::SetDieAirFlowValue(const ULONG ulValue)
{
	if (m_DieAirFlowValueList.GetSize() < _MAX_SAMPLE_COUNT)
	{
		m_DieAirFlowValueList.Add(ulValue);
	}
}


//================================================================
// Function Name: 		GetBHZMissingDieThreshold
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Missing die on BHZ
// Return:				None
// Remarks:				None
//================================================================
ULONG AD_CBHAirFlowCompensation::GetBHZMissingDieThreshold()
{
	return m_ulMissingDieThresholdValue;
}


//================================================================
// Function Name: 		GetBHZColletJamThreshold
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Collet jam on BHZ
// Return:				None
// Remarks:				None
//================================================================
ULONG AD_CBHAirFlowCompensation::GetBHZColletJamThreshold()
{
	return m_ulColletJamThresholdValue;
}

//================================================================
// Function Name: 		CalcAirFlowThreshold
// Input arguments:		None
// Output arguments:	None
// Description:   		Get the threshold of Missing die and collet jam on BHZ
// Return:				None
// Remarks:				None
//================================================================
BOOL AD_CBHAirFlowCompensation::CalcAirFlowThreshold()
{
	if ((m_EmptyAirFlowValueList.GetSize() >= _MAX_SAMPLE_COUNT) && 
		(m_DieAirFlowValueList.GetSize() >= _MAX_SAMPLE_COUNT))
	{
		DOUBLE dBHZAirFlowUnBlockValue = 0;
		LONG lSize = (LONG)m_EmptyAirFlowValueList.GetSize();
		for (LONG i = 0; i < m_EmptyAirFlowValueList.GetSize(); i++)
		{
			dBHZAirFlowUnBlockValue += m_EmptyAirFlowValueList[i];
		}
		dBHZAirFlowUnBlockValue = dBHZAirFlowUnBlockValue / lSize;

		DOUBLE dBHZAirFlowBlockValue = 0;
		lSize = (LONG)m_DieAirFlowValueList.GetSize();
		for (LONG i = 0; i < m_DieAirFlowValueList.GetSize(); i++)
		{
			dBHZAirFlowBlockValue += m_DieAirFlowValueList[i];
		}
		dBHZAirFlowBlockValue = dBHZAirFlowBlockValue / lSize;

		m_ulMissingDieThresholdValue = GetBHZMissingDieThresholdValue(_round(dBHZAirFlowUnBlockValue), _round(dBHZAirFlowBlockValue));
		m_ulColletJamThresholdValue = GetBHZColletJamThresholdValue(_round(dBHZAirFlowUnBlockValue), _round(dBHZAirFlowBlockValue));
		ResetDieAirFlowValueList();
		return TRUE;
	}

	return FALSE;
}


//================================================================
// Function Name: 		ResetDieAirFlowValueList
// Input arguments:		None
// Output arguments:	None
// Description:   		Reset the threshold of Air Flow Value List
// Return:				None
// Remarks:				None
//================================================================
VOID AD_CBHAirFlowCompensation::ResetDieAirFlowValueList()
{
	m_EmptyAirFlowValueList.RemoveAll();
	m_DieAirFlowValueList.RemoveAll();
	m_lContinuousMissingDieCounter  = 0;
}


//================================================================
// Function Name: 		GetBHZMissingDieThresholdValue
// Input arguments:		ulBHZAirFlowUnBlockValue, ulBHZAirFlowBlockValue
// Output arguments:	None
// Description:   		Get the threshold of Missing die on BHZ
// Return:				None
// Remarks:				None
//================================================================
ULONG AD_CBHAirFlowCompensation::GetBHZMissingDieThresholdValue(const ULONG ulBHZAirFlowUnBlockValue, const ULONG ulBHZAirFlowBlockValue)
{
	ULONG ulThresholdValue = 0;

	if ((m_dBHZMissingDieThresholdPercent >= 10) && (m_dBHZMissingDieThresholdPercent <= 90))
	{
		ulThresholdValue = (ULONG)_round(m_dBHZMissingDieThresholdPercent / 100.0 * (ulBHZAirFlowUnBlockValue - ulBHZAirFlowBlockValue)) + ulBHZAirFlowBlockValue;
	}
	else
	{
		ulThresholdValue = (ULONG)_round((ulBHZAirFlowUnBlockValue - ulBHZAirFlowBlockValue) / 2.0) + ulBHZAirFlowBlockValue;
	}
	
	return ulThresholdValue;
}


//================================================================
// Function Name: 		GetBHZColletJamThresholdValue
// Input arguments:		ulBHZAirFlowUnBlockValue, ulBHZAirFlowBlockValue
// Output arguments:	None
// Description:   		Get the threshold of Collet jam on BHZ
// Return:				None
// Remarks:				None
//================================================================
ULONG AD_CBHAirFlowCompensation::GetBHZColletJamThresholdValue(const ULONG ulBHZAirFlowUnBlockValue, const ULONG ulBHZAirFlowBlockValue)
{
	ULONG ulThresholdValue = 0;

	if ((m_dBHZColletJamThresholdPercent >= 10) && (m_dBHZColletJamThresholdPercent <= 90))
	{
		ulThresholdValue = (ULONG)_round(m_dBHZColletJamThresholdPercent / 100.0 * (ulBHZAirFlowUnBlockValue - ulBHZAirFlowBlockValue)) + ulBHZAirFlowBlockValue;
	}
	else
	{
		ulThresholdValue = (ULONG)_round((ulBHZAirFlowUnBlockValue - ulBHZAirFlowBlockValue) / 2.0) + ulBHZAirFlowBlockValue;
	}

	return ulThresholdValue;
}

VOID AD_CBHAirFlowCompensation::ResetMissingDieCount()
{
	m_lContinuousMissingDieCounter = 0;
}

VOID AD_CBHAirFlowCompensation::IncreaseMissingDieCount()
{
	m_lContinuousMissingDieCounter++;
}

BOOL AD_CBHAirFlowCompensation::ReachMissingDieCountLimit()
{
	return (m_lContinuousMissingDieCounter >= 2);
}