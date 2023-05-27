// MS896ADoc.cpp : implementation of the CMS896ADoc class
//

#include "stdafx.h"
#include "MS896A.h"

#include "MS896ADoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMS896ADoc

IMPLEMENT_DYNCREATE(CMS896ADoc, CDocument)

BEGIN_MESSAGE_MAP(CMS896ADoc, CDocument)
END_MESSAGE_MAP()


// CMS896ADoc construction/destruction

CMS896ADoc::CMS896ADoc()
{
	// TODO: add one-time construction code here

}

CMS896ADoc::~CMS896ADoc()
{
}

BOOL CMS896ADoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CMS896ADoc serialization

void CMS896ADoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CMS896ADoc diagnostics

#ifdef _DEBUG
void CMS896ADoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMS896ADoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMS896ADoc commands
