#ifndef WM_MarkDieRegion_H
#define WM_MarkDieRegion_H

#include "DieRegion.h"

//================================================================
// Class definition
//================================================================
class WM_CMarkDieRegion : public WM_CRegion
{
#define MARK_DIE_REGION_SZIE 50
public:
	WM_CMarkDieRegion(); //Constructor
	~WM_CMarkDieRegion(); //Deconstructor

	VOID SetMarkDieRegion(const ULONG ulUpperLeftRow, const ULONG ulUpperLeftCol, const ULONG ulLowerRightRow, const ULONG ulLowerRightCol);
	VOID SetMarkDieCoord(const ULONG ulMakeDieRow, const ULONG ulMakeDieCol);
	VOID AutoUpdateMarkDieCoord();

	ULONG GetMarkDieRow();
	ULONG GetMarkDieCol();

	VOID SetMarkDieOffset(LONG lMakeDieOffsetX, LONG lMakeDieOffsetY);
	LONG GetMarkDieOffsetX();
	LONG GetMarkDieOffsetY();

private:
	ULONG m_ulMakeDieRow;
	ULONG m_ulMakeDieCol;

	LONG m_lMakeDieOffsetX;
	LONG m_lMakeDieOffsetY;
};

#endif  // MarkDieRegion_H

//================================================================
// End of file MarkDieRegion.h
//================================================================