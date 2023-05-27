#include "stdafx.h" 
#include "Plane.h"
#include "stdio.h"
#include "StringMapFile.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"
#include "LogFileUtil.h"
#include <Math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPlane::CPlane()
{
}

CPlane::~CPlane()
{
}

BOOL CPlane::CalPlaneEqn(LONG lPosX[], LONG lPosY[], LONG lPosZ[])
{
	typedef struct
	{
		DOUBLE dX;
		DOUBLE dY;
		DOUBLE dZ;
	} VECTOR;

	VECTOR v1, v2, vCrossProduct[4];
	DOUBLE dProduct[4];
	DOUBLE dZLevel[4], dZLevelDiff[4], dAngle[4];
	INT		nRefPos = 0, i;


	//v4.39T10	//Klocwork
	for (INT m=0; m<4; m++)
	{
		dProduct[m]		= 0;
		dZLevel[m]		= 0;
		dZLevelDiff[m]	= 0;
		dAngle[m]		= 0;
	}

	for(i=0; i<MAX_NO_OF_EQUATION; i++)
	{
		switch(i)
		{
			case 0:
				// 0.0005 is the resolution
				v1.dX = ( lPosX[0] - lPosX[1] ) * 0.0005;
				v1.dY = ( lPosY[0] - lPosY[1] ) * 0.0005;
				v1.dZ = ( lPosZ[0] - lPosZ[1] ) * 0.0005;

				v2.dX = ( lPosX[0] - lPosX[2] ) * 0.0005;
				v2.dY = ( lPosY[0] - lPosY[2] ) * 0.0005;
				v2.dZ = ( lPosZ[0] - lPosZ[2] ) * 0.0005;
				
				nRefPos = 3;
				break;
			case 1:
				v1.dX = ( lPosX[1] - lPosX[0] ) * 0.0005;
				v1.dY = ( lPosY[1] - lPosY[0] ) * 0.0005;
				v1.dZ = ( lPosZ[1] - lPosZ[0] ) * 0.0005;

				v2.dX = ( lPosX[1] - lPosX[3] ) * 0.0005;
				v2.dY = ( lPosY[1] - lPosY[3] ) * 0.0005;
				v2.dZ = ( lPosZ[1] - lPosZ[3] ) * 0.0005;

				nRefPos = 2;
				break;
			case 2:
				v1.dX = ( lPosX[2] - lPosX[0] ) * 0.0005;
				v1.dY = ( lPosY[2] - lPosY[0] ) * 0.0005;
				v1.dZ = ( lPosZ[2] - lPosZ[0] ) * 0.0005;

				v2.dX = ( lPosX[2] - lPosX[3] ) * 0.0005;
				v2.dY = ( lPosY[2] - lPosY[3] ) * 0.0005;
				v2.dZ = ( lPosZ[2] - lPosZ[3] ) * 0.0005;
				nRefPos = 1;
				break;

				//Klocwork	//v4.39T10
/*
			case 3:
				v1.dX = ( lPosX[3] - lPosX[1] ) * 0.0005;
				v1.dY = ( lPosY[3] - lPosY[1] ) * 0.0005;
				v1.dZ = ( lPosZ[3] - lPosZ[1] ) * 0.0005;

				v2.dX = ( lPosX[3] - lPosX[2] ) * 0.0005;
				v2.dY = ( lPosY[3] - lPosY[2] ) * 0.0005;
				v2.dZ = ( lPosZ[3] - lPosZ[2] ) * 0.0005;
				nRefPos = 0;
				break;
*/
		}
		//Calculation Cross Product
		vCrossProduct[i].dX = v1.dY * v2.dZ - v1.dZ * v2.dY;
		vCrossProduct[i].dY = -(v1.dX * v2.dZ - v1.dZ * v2.dX);
		vCrossProduct[i].dZ = v1.dX * v2.dY - v1.dY * v2.dX;

		dProduct[i]	= ( vCrossProduct[i].dX * lPosX[i] + vCrossProduct[i].dY * lPosY[i] + vCrossProduct[i].dZ * lPosZ[i] ) * 0.0005;

		if (vCrossProduct[i].dZ != 0.0)
		{
			dZLevel[i] = ( (dProduct[i] / 0.0005) - vCrossProduct[i].dX * lPosX[nRefPos] - vCrossProduct[i].dY * lPosY[nRefPos] )/vCrossProduct[i].dZ;
		}

		dZLevelDiff[i] = fabs(dZLevel[i] - lPosZ[nRefPos]);

		if ((sqrt(vCrossProduct[i].dX * vCrossProduct[i].dX + vCrossProduct[i].dY * vCrossProduct[i].dY + vCrossProduct[i].dZ * vCrossProduct[i].dZ)) != 0.0)
		{
			dAngle[i] = vCrossProduct[i].dZ/ (sqrt(vCrossProduct[i].dX * vCrossProduct[i].dX + vCrossProduct[i].dY * vCrossProduct[i].dY + vCrossProduct[i].dZ * vCrossProduct[i].dZ));
		}

	}

	for (i=0; i<MAX_NO_OF_EQUATION; i++)
	{
		m_dFactorX[i] = vCrossProduct[i].dX;
		m_dFactorY[i] = vCrossProduct[i].dY;
		m_dFactorZ[i] = vCrossProduct[i].dZ;
		m_dFactorSum[i] = dProduct[i];
	}

	//m_bPlanLearnt = TRUE;
	//Define Max, Min and Avg Z level
	LONG lSumLevel = lPosZ[0];
	m_lMaxLevel = lPosZ[0];		//max
	m_lMinLevel = lPosZ[0];		//min
	for (i=1; i<MAX_NO_OF_EQUATION; i++)
	{
		lSumLevel += lPosZ[i];
		if (lPosZ[i] > m_lMaxLevel)
		{
			m_lMaxLevel = lPosZ[i];
		}

		if (lPosZ[i] < m_lMinLevel)
		{
			m_lMinLevel = lPosZ[i];
		}
	}
	m_lAvgLevel = (LONG)(lSumLevel/4);	//avg

	return TRUE;
}


BOOL CPlane::CalZLevel(LONG lEqnNo, LONG lX, LONG lY, LONG& lZ)
{
	if (m_dFactorZ[lEqnNo] != 0.0)
	{
		lZ = (LONG) ( ( (m_dFactorSum[lEqnNo] / 0.0005) - m_dFactorX[lEqnNo] * lX - m_dFactorY[lEqnNo] * lY )/m_dFactorZ[lEqnNo]);
	}

	return lZ;
}