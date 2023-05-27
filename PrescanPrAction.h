#pragma once
#include <WaferMapWrapper.h>

class CPrescanPRAction
{
public:
	virtual ~CPrescanPRAction() = 0 {};

	virtual int GoodDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col) {return 1;}
	virtual int DefectiveDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col) {return 1;}
	virtual int EmptyDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col) {return 1;}
	virtual int BadCutDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col) {return 1;}
	virtual bool IsGoodAction()	{ return false; }
};

class CDefectScanPRAction : public CPrescanPRAction
{
public:
	CDefectScanPRAction();
	virtual ~CDefectScanPRAction();

	virtual int GoodDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col);
	virtual int DefectiveDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col);
	virtual int EmptyDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col);
	virtual int BadCutDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col);
	virtual bool IsGoodAction();

	void SetGoodGrade(bool state, unsigned char grade);
	void SetDefectiveGrade(bool state, unsigned char grade);
	void SetEmptyGrade(bool state, unsigned char grade);
	void SetBadCutGrade(bool state, unsigned char grade);

protected:
	bool	m_goodChangeGrade;
	bool	m_defectiveChangeGrade;
	bool	m_emptyChangeGrade;
	bool	m_badCutChangeGrade;

	unsigned char m_goodGrade;
	unsigned char m_defectiveGrade;
	unsigned char m_emptyGrade;
	unsigned char m_badCutGrade;
};