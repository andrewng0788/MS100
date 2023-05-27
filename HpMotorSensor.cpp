/////////////////////////////////////////////////////////////////
//	HpMotorSensor.cpp : implementation of the CHpMotorSensor class
//
//	Description:
//		AD8930 Discrete Die Bonder
//
//	Date:		Wednesday, 28 April, 2004
//	Revision:	1.00
//
//	By:			
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "HpMotorSensor.h"

CHpMotorSensor::CHpMotorSensor(void)
{
	m_bActiveLogic		= TRUE;
	m_pPort				= NULL;
	m_lMask				= 0;
}

CHpMotorSensor::CHpMotorSensor(SFM_CHipecPort* pPort, LONG lMask, BOOL bActiveLogic)
{
	Create(pPort, lMask, bActiveLogic);
}

CHpMotorSensor::~CHpMotorSensor(void)
{
}

VOID CHpMotorSensor::Create(SFM_CDigitalIn* pInput, BOOL bActiveLogic)
{
	// Dummy function
}

SFM_CDigitalIn* CHpMotorSensor::GetInputBit()
{
	// Dummy function
	return NULL;
}

VOID CHpMotorSensor::Create(SFM_CHipecPort* pPort, LONG lMask, BOOL bActiveLogic)
{
	m_pPort				= pPort;
	m_bActiveLogic		= bActiveLogic;
	m_lMask				= lMask;
}

VOID CHpMotorSensor::Create(SFM_CHipecChannel* pChannel, LONG lMask, BOOL bActiveLogic)
{
	if (pChannel)
	{
		SFM_CHipecMotorSensorPort* pPort = pChannel->GetMotorSensorPort();
		Create(pPort, lMask, bActiveLogic);
	}
}

BOOL CHpMotorSensor::GetActiveLogic()
{
	return m_bActiveLogic;
}

SFM_CHipecPort* CHpMotorSensor::Port()
{
	return m_pPort;
}

LONG CHpMotorSensor::Mask()
{
	return m_lMask;
}

BOOL CHpMotorSensor::IsActive()
{
	BOOL bState			= TRUE;

	if (m_pPort)
	{
		LONG lValue		= m_pPort->GetPattern() & m_lMask;

		if (lValue != 0)
		{
			bState		= m_bActiveLogic;
		}
		else
		{
			bState		= !m_bActiveLogic;
		}
	}

	return bState;
}

BOOL CHpMotorSensor::IsNotActive()
{
	BOOL bState			= TRUE;

	if (m_pPort)
	{
		LONG lValue		= m_pPort->GetPattern() & m_lMask;

		if (lValue != 0)
		{
			bState		= !m_bActiveLogic;
		}
		else
		{
			bState		= m_bActiveLogic;
		}
	}

	return bState;
}
