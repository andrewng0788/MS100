/////////////////////////////////////////////////////////////////
//	HpMotorSensor.h : interface of the CHpMotorSensor class
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

#pragma once

#include "Sensor.h"
#include "HipecMotorSensorPort.h"
#include "HipecChannel.h"

static const LONG glHIPEC_HOME_SNR_MASK		= 0x0001;
static const LONG glHIPEC_POS_LIMIT_MASK	= 0x0002;
static const LONG glHIPEC_NEG_LIMIT_MASK	= 0x0004;
static const LONG glHIPEC_INDEX_SNR_MASK	= 0x0080;

class CHpMotorSensor : public SFM_CSensor,
					   public CObject
{
public:
	CHpMotorSensor(void);
	CHpMotorSensor(SFM_CHipecPort* pPort, LONG lMask, BOOL bActiveLogic = SFM_ACTIVEHIGH);
	virtual ~CHpMotorSensor(void);

protected:
	SFM_CHipecPort*		m_pPort;
	LONG				m_lMask;

public:
	virtual VOID Create(SFM_CDigitalIn* pInput, BOOL bActiveLogic = SFM_ACTIVEHIGH);
	virtual SFM_CDigitalIn* GetInputBit();

	virtual VOID Create(SFM_CHipecPort* pPort, LONG lMask, BOOL bActiveLogic = SFM_ACTIVEHIGH);
	virtual VOID Create(SFM_CHipecChannel* pChannel, LONG lMask, BOOL bActiveLogic = SFM_ACTIVEHIGH);
	virtual BOOL GetActiveLogic();

	virtual SFM_CHipecPort* Port();
	virtual LONG Mask();

	virtual BOOL IsActive();
	virtual BOOL IsNotActive();
};
