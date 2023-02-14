// Spectra4View.h : interface of the CSpectra4View class
//
/////////////////////////////////////////////////////////////////////////////
#include "Spectra4Doc.h"
#if !defined(AFX_SPECTRA4VIEW_H__4CDFB00F_BFCE_11D5_A8B3_0002B3090409__INCLUDED_)
#define AFX_SPECTRA4VIEW_H__4CDFB00F_BFCE_11D5_A8B3_0002B3090409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SCAN_MIN 0
#define SCAN_MAX 1
#define SCAN_AVE 2
#define SCAN_SLP 3
#define SCAN_ARE 4

#define WM_AUTOFIT	WM_USER+2

typedef struct
{
int		ID;
char	Name[50];
}CModeName;

class CGridCtrl;
class CSpectra4View : public CView
{
protected: // create from serialization only
	CSpectra4View();
	DECLARE_DYNCREATE(CSpectra4View)

// Attributes
public:
	TSpectrum*	Spectrum;
	CGridCtrl*	pSpecGrid;
	CGridCtrl*	pFitGrid;
	int			ParamFlag;
	int			ExtremSmoothPoint;
	int			ScanMode;
	int			TagComp;
	int			NComp;
	TFitParam	crFitState;
	TParam		TraceParam;
	int			PeakFitFlag;
	
	
	// Operations
public:
	CSpectra4Doc* GetDocument();
	void	SetGridParam();
	void	SetGridParam(int);
	void	GetGridParam(int GetKf=1);
	void	ApplyGridParam(int GetKf=1);
	void	GetFitParam(TParam*, int exMode=-1);
	void	SetFitParam(TParam* pParam, int exMode=-1);
	void	UpdateFitIndicator();
	void	SetTraceParam();
	int		SelectFitParam();
	void	UpdateSelection(short int* SelectList, int ReDraw = 1);
	void	UpdateScale(int GetScale = 1);
	void	UpdateScan(int ScanFlag = 1);
	void	UpdateFitResult(int TagComp, int FitComp=0);
	int		FitTrace(float* TraceX, float* TraceY, int NTracePar=3);
	int		VarAn(float* TraceX, float* TraceY);
	int		PeakTrace(TSpectrum* TargetSpec,int BegNum,int EndNum);
	void	Rand(float AmpLow=1, float AmpHigh=1);
	
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectra4View)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	protected:
	//}}AFX_VIRTUAL
	virtual void OnCompNew();
	virtual void OnCompDel();
	virtual void OnCompAlign();
	virtual void OnCompClip();
	virtual void OnApply();
	virtual void OnCopy();
	virtual void OnCut();
	virtual void OnFileSave();
	virtual void OnPaste();
	virtual void OnAverage();
	virtual void OnFitDuplicate(void);
	virtual void OnFitStep();
	virtual void OnFitSumm();
	virtual void OnFitAuto();
	virtual void OnFitSet();
	virtual void OnFitFind();
	virtual void OnFitMontec();
	virtual LRESULT OnAutoFit(WPARAM wParam, LPARAM lParam);
	virtual void OnToolsScale();
	virtual void OnToolsSmooth();
	virtual void OnToolsScan();
	virtual void OnToolsSeek();
	virtual void OnToolsArea();
	virtual void OnToolsLLHTest();
	virtual void OnIgnoreSelToggle();
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView);
// Implementation
public:
	virtual ~CSpectra4View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int		SelFitComp;
	float MouseX1;
	float MouseX2;
	float MouseY1;
	float MouseY2;
	void GetMouseCoord(CPoint cP);
// Generated message map functions
protected:
	//{{AFX_MSG(CSpectra4View)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Spectra4View.cpp
inline CSpectra4Doc* CSpectra4View::GetDocument()
   { return (CSpectra4Doc*)m_pDocument; }
#endif




/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRA4VIEW_H__4CDFB00F_BFCE_11D5_A8B3_0002B3090409__INCLUDED_)
