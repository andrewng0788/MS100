#ifndef WM_MarkDieRegionManager_H
#define WM_MarkDieRegionManager_H

typedef enum 
{
	UPPER_RIGHT = 0,
	UPPER_LEFT,
	LOWER_LEFT,
	LOWER_RIGHT
}  MARK_DIE_COORDINATE;

//================================================================
// Class definition
//================================================================
class WM_CMarkDieRegionManager : public CObject
{
	friend class WM_CMarkDieRegion;
public:
	WM_CMarkDieRegionManager(); //Constructor
	~WM_CMarkDieRegionManager(); //Deconstructor

	static WM_CMarkDieRegionManager* Instance();

	VOID ToggleScanRegionMarkDieCaseLog();
	BOOL IsEnableScanRegionMarkDieCaseLog();

	VOID SetScanRegionMarkDie(BOOL bScanRegionMarkDie);
	BOOL IsScanRegionMarkDie();

	VOID RemoveMarkDieRegionList();
	WM_CMarkDieRegion *GetMarkDieRegion(LONG lPos);

	LONG _round(double val);
	VOID DefineMarkDieRegion(const ULONG ulMaxRow, const ULONG ulMaxCol);
	VOID AutoUpdateMarkDieCoord();
	ULONG GetMarkDieRegionRowNum();
	ULONG GetMarkDieRegionColNum();

	BOOL SubGetMarkDieRegionPRInfo(const LONG lRegionNo, ULONG &ulMarkDieRow, ULONG &ulMarkDieCol, LONG &lMarkDieOffsetX, LONG &lMarkDieOffsetY);
	BOOL GetMarkDieRegionPRInfo(const LONG lRegionNo, ULONG &ulMarkDieRow, ULONG &ulMarkDieCol, LONG &lMarkDieOffsetX, LONG &lMarkDieOffsetY);

	VOID GetUpperRightCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY);
	VOID GetUpperLeftCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY);
	VOID GetLowerLeftCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY);
	VOID GetLowerRightCornerPRInfoOfMarkDieRegion(ULONG ulRegionNum, double &dResultRow, double &dResultCol, double &dResultOffsetX, double &dResultOffsetY);
	VOID CalFourPointInterpolation(const double dDieRow[4], const double dDieCol[4], const double dDiePROffsetX[4], const double dDiePROffsetY[4],
								   const ULONG ulCenterRow, const ULONG ulCenterCol, double &dCenterResultOffsetX, double &dCenterResultOffsetY);

public:
	BOOL m_bScanRegionMarkDieCaselogEnable;
private:
	BOOL m_bScanRegionMarkDie;
	ULONG m_ulMarkDieRegionRowNum;
	ULONG m_ulMarkDieRegionColNum;
	CObArray	m_MarkDieRegionList;

	static WM_CMarkDieRegionManager* m_pInstance;
};

#endif  // MarkDieRegion_H

//================================================================
// End of file MarkDieRegion.h
//================================================================