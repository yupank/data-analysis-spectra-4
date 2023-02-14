// Spectra4Doc.h : interface of the CSpectra4Doc class
//
/////////////////////////////////////////////////////////////////////////////
#include "Speccomp.h"
#if !defined(AFX_SPECTRA4DOC_H__4CDFB00D_BFCE_11D5_A8B3_0002B3090409__INCLUDED_)
#define AFX_SPECTRA4DOC_H__4CDFB00D_BFCE_11D5_A8B3_0002B3090409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSpectra4Doc : public CDocument
{
public: // create from serialization only
	CSpectra4Doc();
	DECLARE_DYNCREATE(CSpectra4Doc)

// Attributes
public:
	CString csDocName;
	TSpectrum* Spectrum;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectra4Doc)
	public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpectra4Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSpectra4Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRA4DOC_H__4CDFB00D_BFCE_11D5_A8B3_0002B3090409__INCLUDED_)
