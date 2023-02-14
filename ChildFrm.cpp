// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Spectra4.h"
#include "Spectra4View.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

/*
BOOL CChildFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,	const RECT& rect ,
						 				CMDIFrameWnd* pParentWnd,CCreateContext* pContext) 
{
	CRect ParRect;
	RECT newRect;
	char szBuf[128];
	//pParentWnd->GetClientRect(ParRect);
	//newRect.bottom = rect.top+1.1*(rect.bottom-rect.top);
	//newRect.right = rect.left+1.1*(rect.right-rect.left);
	sprintf(szBuf, "t: %i, b: %i",rect.top, rect.bottom);
	AfxMessageBox(szBuf);
	newRect.bottom = rect.bottom+10;
	newRect.top = rect.top+10;
	return CMDIChildWnd::Create(lpszClassName,lpszWindowName, dwStyle,newRect,pParentWnd, pContext);
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	
	//if( !CMDIChildWnd::Create(lpszClassName,lpszWindowName, dwStyle,rect,pParentWnd, pContext) )
	//	return FALSE;
	//ASSERT(m_wndSplitter.CreateStatic(this,1,2));
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		AfxMessageBox("Failed to create static splitters\n");
		return FALSE;
	}
	//ASSERT(m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CSpecTableView),
	//	CSize(100,50), pContext));
	
	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CSpecTableView), CSize(500, 80), pContext))
	{
		AfxMessageBox("Failed to create first pane\n");
		//return FALSE;
	}
	
	ASSERT(m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CSpectra4View),
		CSize(0,0), pContext));
	SetActiveView((CView*)m_wndSplitter.GetPane(0,0));
	CreateView(pContext, AFX_IDW_PANE_FIRST);
	CreateView(pContext, AFX_IDW_PANE_FIRST+1);
	//CFrameWnd::OnCreateClient(lpcs, pContext);
	//if( !CMDIChildWnd::Create(lpszClassName,lpszWindowName, dwStyle,rect,pParentWnd, pContext) )
	//	return FALSE;
	return TRUE;

}
*/

  /////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
