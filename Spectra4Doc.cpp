// Spectra4Doc.cpp : implementation of the CSpectra4Doc class
//

#include "stdafx.h"
#include "Spectra4.h"
#include "Spectra4Doc.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// common variables
extern int crDocNum;
extern int crFitTraceNum; 
extern int crSlopeNum; 
extern int crScanNum;
extern int crHistNum;
extern int crPeakNum;
extern int crSeriesNum;
extern int crChanNum;
extern int crDocMode;
extern int ReadCfn;
extern CString ActiveDocName;
extern CString ExtDocName;
extern char szNum[64];	
extern char szBuff[256];
extern char szName[256];
extern char szExtName[8];
extern char szFileName[256];
extern char szStr[256];
extern int nBefore;
extern int nAfter;
extern int BaseBeg;
extern int BaseEnd;
extern int NoTrace;
extern int StimPoint;
extern TSpectrum* ClbSpectrum;
extern int ClbDataOK;
extern TSpectrum* ReadSpectrum;
extern int ReadDataOK;
extern int ReadDataContinue;

/////////////////////////////////////////////////////////////////////////////
// CSpectra4Doc

IMPLEMENT_DYNCREATE(CSpectra4Doc, CDocument)

BEGIN_MESSAGE_MAP(CSpectra4Doc, CDocument)
	//{{AFX_MSG_MAP(CSpectra4Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpectra4Doc construction/destruction

CSpectra4Doc::CSpectra4Doc()
{
	// TODO: add one-time construction code here

}

CSpectra4Doc::~CSpectra4Doc()
{
}
//
BOOL CSpectra4Doc::OnNewDocument()
{
	
	float*X;	float* Y; 
	TComponent* TestComp;
	int j;
	if (!CDocument::OnNewDocument())
		return FALSE;
	//AddView(CSpecTableView);
	if (crDocMode>=0)
		csDocName.LoadString(IDS_SPEC_TITLE+crDocMode);
	else csDocName=ExtDocName;
	//sprintf(szBuff,"%i", crDocMode);
	//AfxMessageBox(szBuff);
	switch (crDocMode)
	{
		case 0:	sprintf(szNum,"%i ", crDocNum);	crDocNum++;	break;
		case 1:	sprintf(szNum,"%i: ", crFitTraceNum);	crFitTraceNum++;	break;
		//case 2:	sprintf(szNum,"%i: ", crSlopeNum);	crSlopeNum++; 	break;
		case 2:	sprintf(szNum,"%i: ", crPeakNum);	crPeakNum++; 	break;
		case 3:	sprintf(szNum,"%i: ", crScanNum);	crScanNum++; break;
		case 4:	sprintf(szNum,"%i: ", crHistNum);	crHistNum++; break;
		//case 5:	sprintf(szNum,"%i: ", crPeakNum);	crPeakNum++; break;
	}	
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	if (crDocMode>=0)
	{
		csDocName+=szNum;
		csDocName+=ActiveDocName;
	}
	else
		csDocName+=ActiveDocName;
	if (ReadDataOK)
	{
		if (ReadSpectrum!=NULL)
		{
			Spectrum = ReadSpectrum;
			Spectrum->SetSelNum(1, 8, 0);
			if (crChanNum>=0)
			{
				sprintf(szNum," Ch%i", crChanNum);
				csDocName+=szNum;
			}
			if (crSeriesNum)
			{
				sprintf(szNum," s%i", crSeriesNum);
				csDocName+=szNum;
			}
			SetTitle(csDocName);
		
		}
		if (ReadDataContinue) 
		{
			ReadSpectrum = new TSpectrum;
			ReadDataContinue=0;
		}
	}
	else
	{
		Spectrum = new TSpectrum;
		
		if (ClbDataOK)
		{	
			Spectrum->PasteSelComp(ClbSpectrum,1,1);
			if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
			
			ClbDataOK=0;
			if (!ReadCfn) Spectrum->Average();
			Spectrum->SetSelNum(1, 8, 0);
			if (crChanNum>=0)
			{
				sprintf(szNum," Ch%i", crChanNum);
				csDocName+=szNum;
			}
			if (crSeriesNum)
			{
				sprintf(szNum," s%i", crSeriesNum);
				csDocName+=szNum;
			}
			SetTitle(csDocName);						
		}
	}
	SetTitle(csDocName);
	return TRUE;
}
//
void CSpectra4Doc::OnCloseDocument()
{
	delete Spectrum;
	CDocument::OnCloseDocument();
}


/////////////////////////////////////////////////////////////////////////////
// CSpectra4Doc serialization

void CSpectra4Doc::Serialize(CArchive& ar)
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

/////////////////////////////////////////////////////////////////////////////
// CSpectra4Doc diagnostics

#ifdef _DEBUG
void CSpectra4Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSpectra4Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpectra4Doc commands


