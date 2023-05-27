// MS896ADoc.h : interface of the CMS896ADoc class
//


#pragma once

class CMS896ADoc : public CDocument
{
protected: // create from serialization only
	CMS896ADoc();
	DECLARE_DYNCREATE(CMS896ADoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CMS896ADoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


