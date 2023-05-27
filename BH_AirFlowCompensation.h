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

#ifndef _BH_AIR_FLOW_COMPENSATION_H_
#define _BH_AIR_FLOW_COMPENSATION_H_

#include "StringMapFile.h"

class AD_CBHAirFlowCompensation : public CObject
{
public:
#define _MAX_SAMPLE_COUNT 300

	AD_CBHAirFlowCompensation(); //Constructor
	~AD_CBHAirFlowCompensation(); //Deconstructor
	VOID Init();
public:
	VOID WriteData(CMapElement *pElement);
	VOID ReadData(CMapElement *pElement);
	LONG _round(double val);

	VOID SetAirFlowPara(const DOUBLE dBHZMissingDieThresholdPercent, const DOUBLE dBHZColletJamThresholdPercent);
	VOID SetEmptyAirFlowValue(const ULONG ulValue);
	VOID SetDieAirFlowValue(const ULONG ulValue);

	ULONG GetBHZMissingDieThreshold();
	ULONG GetBHZColletJamThreshold();
	BOOL CalcAirFlowThreshold();

	VOID ResetMissingDieCount();
	VOID IncreaseMissingDieCount();
	BOOL ReachMissingDieCountLimit();

	VOID ResetDieAirFlowValueList();
private:
	ULONG GetBHZMissingDieThresholdValue(const ULONG ulBHZAirFlowUnBlockValue, const ULONG ulBHZAirFlowBlockValue);
	ULONG GetBHZColletJamThresholdValue(const ULONG ulBHZAirFlowUnBlockValue, const ULONG ulBHZAirFlowBlockValue);

private:
	CArray< long, long > m_EmptyAirFlowValueList;		// Get the value without die
	CArray< long, long > m_DieAirFlowValueList;		// Get the value with die

	DOUBLE m_dBHZMissingDieThresholdPercent;
	DOUBLE m_dBHZColletJamThresholdPercent;

	ULONG m_ulMissingDieThresholdValue;
	ULONG m_ulColletJamThresholdValue;

	LONG m_lContinuousMissingDieCounter;
};
#endif