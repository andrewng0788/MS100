#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"

#pragma once

class CCircularWafer: public CObject
{
	public:

		/***********************************/
		/*     Constructor/Destructor      */
		/***********************************/
		CCircularWafer();
		virtual ~CCircularWafer();

		/***********************************/
		/*        Get/Set functions        */
		/***********************************/
		BOOL GetNeedCalibrateFirstDieOnRow();
		BOOL SetNeedCalibrateFirstDieOnRow(BOOL bNeedCalibrateFirstDieOnRow);

		BOOL SetCurrX(LONG lCurrX);
		BOOL SetCurrY(LONG lCurrY);
		BOOL SetWaferCenterX(LONG lWaferCenterX);
		BOOL SetWaferCenterY(LONG lWaferCenterY);
		BOOL SetWaferDiameter(LONG lWaferDiameter);
		BOOL SetWalkPath(LONG lWalkPath);
		BOOL SetEmptyHoleLimit(LONG lEmptyHoleLimit);
		BOOL SetEndStreetLimit(LONG lEndStreetLimit);
		BOOL SetHoriDiePitchX(LONG lHoriDiePitchX);
		BOOL SetHoriDiePitchY(LONG lHoriDiePitchY);
		BOOL SetVertDiePitchX(LONG lVertDiePitchX);
		BOOL SetVertDiePitchY(LONG lVertDiePitchY);
		BOOL SetStartX(LONG lStartX);
		BOOL SetStartY(LONG lStartY);
		BOOL SetFirstDieOnRowX(LONG lFirstDieOnRowX);
		BOOL SetFirstDieOnRowY(LONG lFirstDieOnRowY);

		BOOL GetIsFirstDieInCycle();
		BOOL SetIsFirstDieInCycle(BOOL bIsFirstDieInCycle);

		BOOL SetIsLastDieEmpty(BOOL bIsLastDieEmpty);
		

		/***********************************/
		/*         Member functions        */
		/***********************************/
		BOOL InitWaferData();
		BOOL IsFirstDieInCycle();
		BOOL IsWaferFinished();
		BOOL IsWithinWaferBoundary(LONG lX, LONG lY);
		BOOL IsLastPosnEmpty();
		BOOL DoesHitEmptyHoleLimit();
		BOOL IsCurrRowEmpty();
		BOOL DoesHitEndStreetLimit();
		BOOL IsRowChange();
		BOOL IsWithinWaferLimit(LONG lX, LONG lY);
		BOOL DoesNeedTransverseBack();
		BOOL DoesNeedFindNearestWithinWaferLimitPosn();
		BOOL ReverseWalkDirection();
		DOUBLE DistanceToCenter(LONG lX, LONG lY);
		BOOL FindNextDieOnTop(LONG lPrevX, LONG lPrevY, LONG& lNextOnTopX, LONG& lNextOnTopY);
		BOOL FindNextDieAtBottom(LONG lPrevX, LONG lPrevY, LONG& lNextAtBottomX, LONG& lNextAtBottomY);
		BOOL FindNextDieOnLeft(LONG lPrevX, LONG lPrevY, LONG& lNextOnLeftX, LONG& lNextOnLeftY);
		BOOL FindNextDieOnRight(LONG lPrevX, LONG lPrevY, LONG& lNextOnRightX, LONG& lNextOnRightY);
		BOOL FindNextDiePosn(LONG lPrevX, LONG lPrevY, LONG& lNextX, LONG& lNextY);
		BOOL FindPickXYPosn(LONG& lPickXPosn, LONG& lPickYPosn);
		BOOL ResetParameters();
		

	private:
		LONG	m_lWaferCenterX;		//= m_lWaferCenterX in CWaferTable
		LONG	m_lWaferCenterY;		//= m_lWaferCenterY in CWaferTable
		LONG	m_lWaferDiameter;		//= m_lWaferSize in CWaferTable
		LONG	m_lWalkPath;			//= m_lWaferIndexPath in CWaferTable
		LONG	m_lEmptyHoleLimit;		//= m_lWaferEmptyLimit in CWaferTable
		LONG	m_lEndStreetLimit;		//= m_lWaferStreetLimit in CWaferTable
		LONG	m_lHoriDiePitchX;		//= m_siDiePitchXX in CWaferTable
		LONG	m_lHoriDiePitchY;		//= m_siDiePitchYX in CWaferPr
		LONG	m_lVertDiePitchX;		//= m_siDiePitchXY in CWaferPr
		LONG	m_lVertDiePitchY;		//= m_siDiePitchYY in CWaferPr
		LONG	m_lStartX;				//= m_lStart_X in CWaferPr
		LONG	m_lStartY;				//= m_lStart_Y in CWaferPr


		BOOL	m_bIsFirstDieInCycle;
		BOOL	m_bWaferFinished;
		LONG	m_lEmptyHoleCount;
		LONG	m_lEndStreetCount;
		BOOL	m_bRowChange;
		BOOL	m_bCurrRowEmpty;
		BOOL	m_bNeedTransverseBack;
		BOOL	m_bNeedCalibrateFirstDieOnRow;
		BOOL	m_bNeedFindNearestWithinWaferLimitPosn;
		LONG	m_lCurrX;
		LONG	m_lCurrY;
		LONG	m_lFirstDieOnRowX;
		LONG	m_lFirstDieOnRowY;
		LONG	m_lWalkDirection;		//(1=Right; 2=Left; 3=Down; 4=Up)
		BOOL	m_bIsLastDieEmpty;

}; //end class CCircularWafer