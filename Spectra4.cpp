// Spectra4.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Fstream.h"
#include "Spectra4.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "Spectra4Doc.h"
#include "Spectra4View.h"
#include "Spectra4Dlg.h"
#include "Pulse.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// auxillary structures for reading of PULSE data
struct TPulseSweep
{
int Offset; // absolute  offset from the beginig of file
double	ScaleY;  // pA/ADS units
//short int	StimN;
};

struct TPulseSeries
{
int NSweep;
int NPoint; // points in the each sweep
double	ScaleX;	// ms/point
double	Interval; // ms between sweeps;
};
//Common variables for data access
CStatusBar* pStatusBar;
CSpectraDlgBar* pDialogBar;
CToolBar	CompToolBar;
CString Indicator1;
CString Indicator2;
CString Indicator3;
CSpectra4Doc* ActiveDoc;
TSpectrum*	ActiveSpec;
TSpectrum* ClbSpectrum = NULL;
TSpectrum* ReadSpectrum = NULL;
CSpectra4View* ActiveView;

CStatic* SpecStatic;

CSpectra4Dlg* pSpectra4Dlg;
CSpectraGridDlg* pSpectraGridDlg = NULL;
CGridCtrl* pGridCtrl = NULL;
CGridCtrl* pFitGridCtrl = NULL;
CSpectra4App* SpecApp;
CMainFrame* pMainFrame;


//common variables
// data reading and proceeding parameters

int nBefore=1; int nAfter=0; float FilterDepth=0.99; 
int BaseBeg=0;
int BaseEnd=85;
int NoTrace=0;
int StimPoint = 80;
int CreatePoint = 501;
int AutoReline=0;	int	AutoCalib=1;	int AutoBound=1;	int AutoTrace=0; int SwapSeries =0;
float TimeCourseInt=60;

#define AnSize 70
int		ScrSize=250;
int* Lotable; 
float* LotPr=NULL;
float* tbLotPr=NULL;
float* MeanPr=NULL;
float* DrawDist=NULL;
float* tbDrawDist=NULL;
float* SevenDist=NULL;
int LotSize=0;	int OldLotSize=0;	int TirEnd=0;	int TirBeg=0;
float Rating[10050];
float GenRating[1000];
float RateWt[8][5];
float tbRating[140];
float AveNum[4][AnSize];
float LotPair[2500];
float MinPr[50];
float MaxPr[50];
int LotFlag,LotAnalyzeFlag,bNum,eNum,tNum,mNum;
TSpectrum* PlaceHstSpec=NULL; TSpectrum* RelPlaceHstSpec=NULL; TSpectrum* PlSum34Spec=NULL; TSpectrum* PlaceSumSpec=NULL; int RateHstFlag=0;

int ClbDataOK =0;	int ReadDataOK =0;
// Data fitting and analisys
TFitParam	crFitState={0.07,0.07,0.2,0.1,0,0,0,2,0,255,2,25,0,0,0,2,1.0e-6,0.25,1};
TParam TraceParam={1,0,1,2,1,0,2,-2,2,0,1,0,0,1,20,0,0,RIDEC_I,1,1,100,0,0,0,NONE+1,1,0,2,1,"Trace"};
TPhourier Filter = {0.001,30,1.6,2.0,3,1,0};
float SeekAmp1=0;		float SeekAmp2=10000;	float SeekTRise1=-1;
float SeekTRise2=-1;	float SeekTDec1=-1;		float SeekTDec2=-1;	int	SeekIgnoreFlag=0;
// Document creation styles and titles
int crDocNum = 1;
CString ExtDocName=" ";
int crFitTraceNum = 0 ; 
int crSlopeNum = 1; 
int crScanNum = 1;
int crHistNum = 1;
int crAutoCorrNum = 1;
int crPeakNum = 1;
int crDocMode=0;
int crSeriesNum = 0;
int ReadCfn = 0;
CString ActiveDocName = " ";
// auxillary srings
char szNum[64];	
char szBuff[256];
char szName[256];
char szExtName[8];
char szFileName[256];
char szStr[256];

float RAmpL=0;
float RAmpH=0;
int ROk=0;
/*****************************************/
void SwapInt(int* SourceVal)
{

   char* Source;
   char*	Target;
   void* Buf;
   long int Result;
   Buf = SourceVal; Source = (char*)Buf;
   Buf = &Result;	Target = (char*)Buf;
   *(Target+0) = *(Source+3);
   *(Target+1) = *(Source+2);
   *(Target+2) = *(Source+1);
   *(Target+3) = *(Source+0);
   *(SourceVal) = Result;

}
void SwapShortInt(short int *SourceVal)
{

   char* Source;
   char*	Target;
   void* Buf;
   short int Result;
   Buf = SourceVal; Source = (char*)Buf;
   Buf = &Result;	Target = (char*)Buf;
   *(Target+0) = *(Source+1);
   *(Target+1) = *(Source+0);
   *(SourceVal) = Result;

}
void SwapDouble(double* SourceVal)
{

	int j;
	char* Source;
	char*	Target;
	void* Buf;
	double Result;
	Buf = SourceVal; Source = (char*)Buf;
	Buf = &Result;	Target = (char*)Buf;
	for (j=0;j<8;j++)
		*(Target+j) = *(Source+7-j);
    *(SourceVal) = Result;

}
//****************************************/
void SetFileNameExt(void)
{
	int j, j_ext;
	char* CrChar;
	if (strlen(szName)>1)
   	{
		*szExtName='\0';
		j_ext=0;
		j=0;
		CrChar=szName;
		while ((*CrChar!='.')&&(*CrChar!='\0')&&(j_ext<255) )
      	{
			j_ext++; CrChar++;
		}
		j_ext++;
		if ((j_ext)&&(j_ext<255))
		{
			CrChar=szName+j_ext;
         	while ( (j<3)&&(*CrChar!='\0') )
			{
				*(szExtName+j)=*CrChar;
				j++;
				CrChar++;
			}
            *(szExtName+j)='\0';
		}
	sprintf(szFileName, "%s", strrchr(szName,'\\')+1);
	}


}
float DownScore(int* DestTab, int* CheckTab, int nTab)
{
	int i, j;
	int Count=0; 
	float S;
	for(i=0;i<nTab;i++)
		for(j=0;j<nTab;j++)
			if(*(DestTab+i)== *(CheckTab+j)) Count++;
	S=1.0;
	for(i=0;i<Count;i++)
		S*=nTab;
	
	return S;
}
float ScorePairs(int* DestTab, int nTab)
{
	int i, j, Num, Num2;
	float S,S2;
	float Norm=0.0172;
	S=0;
	for(j=0;j<nTab;j++)
	{
		Num= *(DestTab+j);
		for(i=j+1;i<nTab;i++)
		{
			Num2= *(DestTab+i);
			S2=(*(LotPair+Num*50+Num2-1)-Norm)/Norm;
			if (S2>0)
				S+=S2*S2;
			else
				S-=S2*S2;
		}
	}
	return S;
}

float MatchScore(float* SumPlace)
{
	float S=-1;
	float crSum3,crSum4;
	int j,i, nData,NSum,Num1,Num2;
	if(RateHstFlag)
	{
		nData=PlaceSumSpec->NSPoint;
		NSum=PlaceSumSpec->NSComp-1;
		S=0;
		for(j=0;j<nData;j++)
		{
			for(i=1;i<=NSum;i++)
			{
				crSum3= *((PlaceSumSpec->SpecComp+i)->GetData()+j);
				crSum3*=(PlaceSumSpec->SpecParam+i)->Amp;
				crSum3+=(PlaceSumSpec->SpecParam+i)->Zero;
				Num1=(int)(*(SumPlace+i-1)); Num2=(int)(crSum3);
				if(Num1==Num2) S+=1.0;
			}
			crSum3= *((PlaceSumSpec->SpecComp+3)->GetData()+j);
			crSum3*=(PlaceSumSpec->SpecParam+3)->Amp;
			crSum3+=(PlaceSumSpec->SpecParam+3)->Zero;
			crSum4= *((PlaceSumSpec->SpecComp+4)->GetData()+j);
			crSum4*=(PlaceSumSpec->SpecParam+4)->Amp;
			crSum4+=(PlaceSumSpec->SpecParam+4)->Zero;
			Num1=(int)(*(SumPlace+3)+ *(SumPlace+4)); Num2=(int)(crSum3+crSum4);
			if(Num1==Num2) S+=1.0;
		}
		
	}
	return S;
}

void SortRating(float* rateX, float* rateY, int NData)
{
	float rateOut[1000];
	float bufX,bufY;
	int N,j,SortFlag;
	if (NData<1000)N=NData;
		else N=1000;
	for (j=0;j<N;j++) *(rateOut+j)=*(rateX+j);
	do
	{
		SortFlag=0;
		for (j=1;j<N;j++)
		{
			if(*(rateY+j)>*(rateY+j-1))
			{
				SortFlag=1;
				//changing places
				bufX=*(rateOut+j-1);		bufY=*(rateY+j-1);
				*(rateY+j-1)=*(rateY+j);	*(rateOut+j-1)=*(rateOut+j);
				*(rateY+j)=bufY;			*(rateOut+j)=bufX;
			}
		}
	}
	while(SortFlag);
	//creating report
	for(j=0;j<N;j++)
		*(rateY+j)=*(rateOut+j);
	
}
int MatchNum(int LotSz, int crDraw, int DL)
{
	int i,j,k,m;
	int N=0;
	if((LotSz<LotSize)&&((crDraw-DL)>-2))
	{
		for(j=0;j<DL;j++)
			for(i=0;i<eNum;i++)
				for(k=0;k<DL;k++)
					for(m=0;m<eNum;m++)
						if( *(Lotable+(LotSz-j)*8+i)==(*(Lotable+(crDraw-k)*8+m)) )
							N++;
	}
	return N;
}
/////////////////////////////////////////////////////////////////////////////
// CSpectra4App

BEGIN_MESSAGE_MAP(CSpectra4App, CWinApp)
	//{{AFX_MSG_MAP(CSpectra4App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnCloseAll)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEWSPEC, OnFileNew)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FIT_TRACE, OnFitTrace)
	ON_COMMAND(ID_COMP_MANAG,OnStartGrid)
	ON_COMMAND(IDC_AX_APPLY,OnAxisApply)
	ON_COMMAND(ID_FIT_RAND,	OnFitRand)
	ON_COMMAND(ID_SPEC_RANDOM,	OnSpecRand)
	ON_COMMAND(ID_TOOLS_OPTS,OnToolsOptions)
	ON_COMMAND(ID_TOOLS_HIST,OnToolsHist)
	ON_COMMAND(ID_TOOLS_TMCOUR,OnToolsTimeCourse)
	ON_COMMAND(ID_TOOLS_PWSPEC,OnToolsPowerSpec)
	ON_COMMAND(ID_TOOLS_AC,	OnToolsAutoCorr)
	ON_COMMAND(ID_TOOLS_MINIAN,OnToolsMinal)
	ON_COMMAND(ID_TOOLS_EXTRAP,OnToolsExtrapolate)
	ON_COMMAND(ID_LOT_LOAD,OnLotLoad)
	ON_COMMAND(ID_LOT_ANAL,OnLotAnalyze)
	ON_COMMAND(ID_LOT_PRED,OnLotPredict)
	ON_COMMAND(ID_LOT_RATE_HST,OnLotRateHst)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpectra4App construction

CSpectra4App::CSpectra4App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSpectra4App object

CSpectra4App theApp;

/////////////////////////////////////////////////////////////////////////////
// CSpectra4App initialization
//
BOOL CSpectra4App::InitInstance()
{
	AfxEnableControlContainer();
	// common variables initialization
	crDocMode=0;
	*(szName)='\0';
	*(szFileName)='\0';
	ClbSpectrum = NULL;	ClbDataOK=0;
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	
	pDocTemplate = new CMultiDocTemplate(
		IDR_SPECTRTYPE,
		RUNTIME_CLASS(CSpectra4Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSpectra4View));
	AddDocTemplate(pDocTemplate);
	pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	pMainFrame->LoadAccelTable(MAKEINTRESOURCE(IDR_SPECTRTYPE));
	m_pMainWnd = pMainFrame;
	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	
	//pSpectra4Dlg = new CSpectra4Dlg(m_pMainWnd);
	//pSpectra4Dlg->Create();
	SpecStatic = new CStatic();
	return TRUE;
	SpecApp=this;
}
//
void CSpectra4App::OnCloseAll()
{
	CWinApp::CloseAllDocuments(FALSE);
}
//
void CSpectra4App::OnFileNew()
{
	
	if (pSpectraGridDlg ==NULL)
	{
		pSpectraGridDlg = new CSpectraGridDlg(m_pMainWnd);
		pSpectraGridDlg->Create();
		pGridCtrl = pSpectraGridDlg->GetGrid();
		pFitGridCtrl = pSpectraGridDlg->GetGrid(1);
	
	}
	CWinApp::OnFileNew();
	
}
//
void CSpectra4App::OnAxisApply()
{
	pDialogBar->Apply();
}
//
void CSpectra4App::OnStartGrid()
{
	
	if (pSpectraGridDlg ==NULL)
	{
		pSpectraGridDlg = new CSpectraGridDlg(m_pMainWnd);
		pSpectraGridDlg->Create();
		pGridCtrl = pSpectraGridDlg->GetGrid();
		pFitGridCtrl = pSpectraGridDlg->GetGrid(1);
	}
	
	
}
//
void CSpectra4App::OnToolsOptions()
{
	CSpecOptionsDlg* Dlg = new CSpecOptionsDlg();
	Dlg->m_nStim=StimPoint;
	Dlg->m_nCreate=CreatePoint;
	Dlg->m_nBefore=nBefore;
	Dlg->m_nAfter=nAfter;
	Dlg->m_SwapSer=SwapSeries;
	Dlg->m_Depth=FilterDepth;
	if(Dlg->DoModal())
	{
		StimPoint=Dlg->m_nStim;
		CreatePoint=Dlg->m_nCreate;
		nBefore=Dlg->m_nBefore;
		nAfter=Dlg->m_nAfter;
		SwapSeries=Dlg->m_SwapSer;
		FilterDepth=Dlg->m_Depth;
	}
	delete Dlg;

}
//
void CSpectra4App::OnToolsMinal()
{
	int NPeak;
	TSpectrum*	TargetSpec;
	TSpectrum*	ShowSpec;
	CSpectra4View* ShowView;
	TargetSpec=ActiveView->Spectrum;
	CSweepNumDlg* Dlg=new CSweepNumDlg;
	Dlg->m_BegNum=1;
	Dlg->m_EndNum=TargetSpec->NSComp-1;
	if (Dlg->DoModal()==1)
	{
		crDocMode=2;
		ActiveDocName=ActiveView->GetDocument()->GetTitle();
		if (Dlg->m_BegNum>TargetSpec->NSComp-1)Dlg->m_BegNum=TargetSpec->NSComp-1;
		if (Dlg->m_EndNum>TargetSpec->NSComp-1)Dlg->m_EndNum=TargetSpec->NSComp-1;
		CWinApp::OnFileNew();
		ShowView=ActiveView;
		NPeak = ShowView->PeakTrace(TargetSpec,Dlg->m_BegNum,Dlg->m_EndNum);
		ActiveDocName="";
		crDocMode=0;
	}
	delete Dlg;	
}
//
void CSpectra4App::OnToolsPowerSpec()
{
	int j;
	float*	TmBase;
	float*	TmData;
	int		TmPoint=2000;
	TSpectrum* Spectrum = ActiveView->Spectrum;
	int NData=3*Spectrum->NSPoint;
	//if (NData>TmPoint) 
	NData=TmPoint;
	TmBase=new float[NData+1];	TmData=new float[3*NData+3];
	TmPoint=Spectrum->CreatePowerSpec(TmBase,TmData,NData);
	ActiveDocName=ActiveView->GetDocument()->GetTitle();
	ExtDocName="PowerSpec: ";
	if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
	//for (j=0;j<3;j++)
		ClbSpectrum->NewComp(TmBase,TmData,TmPoint);
	ClbDataOK = 1;
	crSeriesNum=0;
	crDocMode=-1;
	CWinApp::OnFileNew();
	delete ClbSpectrum;
	ClbSpectrum = NULL;
	ClbDataOK = 0;
	crDocMode=0;
	ActiveDocName="";
	ExtDocName="";
}
//
void CSpectra4App::OnToolsTimeCourse()
{
	float*	TmCrBase;
	float*	TmCrData;
	int NData, nTmPoint;
	CTimeCourseDlg* Dlg = new CTimeCourseDlg();
	TSpectrum* Spectrum = ActiveView->Spectrum;
	NData=Spectrum->NSPoint;
	TmCrBase=new float[NData];	TmCrData=new float[NData*3];
	Dlg->m_begX= *(Spectrum->SpecBase);
	Dlg->m_endX= *(Spectrum->SpecBase+NData-1);
	Dlg->m_stepX=TimeCourseInt;
	if(Dlg->DoModal()==1)
	{
		TimeCourseInt=Dlg->m_stepX;
		nTmPoint=Spectrum->CreateTimeCourse(Dlg->m_begX,Dlg->m_endX,TimeCourseInt,TmCrBase,TmCrData,NData);
		if(NData)
		{
			ActiveDocName=ActiveView->GetDocument()->GetTitle();
			ExtDocName="TimeCourse: ";
			if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
			if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
			for (int j=0;j<3;j++)
				ClbSpectrum->NewComp(TmCrBase,TmCrData+NData*j,nTmPoint);
			ClbDataOK = 1;
			crSeriesNum=0;
			crDocMode=-1;
			CWinApp::OnFileNew();
			delete ClbSpectrum;
			ClbSpectrum = NULL;
			ClbDataOK = 0;
			crDocMode=0;
			ActiveDocName="";
			ExtDocName="";
		}
	}
	delete[] TmCrBase; delete[] TmCrData;
	
}
//
void CSpectra4App::OnToolsHist()
{
	float*	HstBase;
	float*	HstData;
	float BinSize;
	HstBase=new float[NCompMax];
	HstData=new float[NCompMax];
	CHistDlg* Dlg = new CHistDlg();
	TSpectrum* Spectrum = ActiveView->Spectrum;
	Spectrum->StatReport(0);
	Dlg->m_BegPoint=0;
	Dlg->m_EndPoint=Spectrum->NSPoint-1;
	Dlg->m_BinSize=0.14*Spectrum->StDev;
	Dlg->m_NBin=Spectrum->NSPoint/10;
	if (Dlg->m_NBin<100) Dlg->m_NBin=100;
	if(Dlg->DoModal()==1)
	{
		BinSize=Dlg->m_BinSize;
		Spectrum->CreateHist(HstBase,HstData,Dlg->m_NBin,1,Dlg->m_BegPoint, Dlg->m_EndPoint, 100,&BinSize);
		ActiveDocName=ActiveView->GetDocument()->GetTitle();
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(HstBase,HstData,Dlg->m_NBin);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=4;
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
	}
	delete HstBase;
	delete HstData;
	delete Dlg;
}
//
void CSpectra4App::OnToolsAutoCorr()
{
	float*	ACBase;
	float*	ACData;
	float ACscore, Period, sdPer;
	TSpectrum* Spectrum = ActiveView->Spectrum;
	int nACData=Spectrum->NSPoint;
	ACBase=new float[nACData+1];
	ACData=new float[2*nACData+1];
	ACscore=Spectrum->AutoCorr(ACBase,ACData,&Period,&sdPer);
	ActiveDocName=ActiveView->GetDocument()->GetTitle();
	ExtDocName="ACorr";
	if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
	ClbSpectrum->NewComp(ACBase,ACData,nACData);
	ClbDataOK = 1;
	crSeriesNum=0;
	crDocMode=-1;
	CWinApp::OnFileNew();
	delete ClbSpectrum;
	ClbSpectrum = NULL;
	ClbDataOK = 0;
	crDocMode=0;
	ActiveDocName="";
	ExtDocName="";
	delete ACBase;
	delete ACData;
	sprintf(szStr,"ACscore: %5.3f  Period: %5.2f  SD: %3.1f%",ACscore,Period,100*sdPer);
	AfxMessageBox(szStr);
}
//
void CSpectra4App::OnToolsExtrapolate()
{
	float*	xBase;
	float*	yData;
	int j,i,N;
	float ACscore, Period, sdPer;
	TSpectrum* Spectrum = ActiveView->Spectrum;
	TFitParam FitState=ActiveView->crFitState;
	int nACData=2*Spectrum->NSPoint;
	xBase=new float[nACData+1];
	yData=new float[2*nACData+1];
	int nData=Spectrum->Extrapolate(xBase,yData,&Filter,&FitState,0,1);
	if (nData>Spectrum->NSPoint)
	{
		ActiveDocName=ActiveView->GetDocument()->GetTitle();
		ExtDocName="Extra ";
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(xBase,yData,nData);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		ExtDocName="";
	}

	delete xBase;
	delete yData;
}
//
void CSpectra4App::OnFitRand()
{
	
	float L,H;
	CRandDlg* Dlg = new CRandDlg();
	if (ROk)
	{
		Dlg->m_AmpLow	=	RAmpL;
		Dlg->m_AmpHigh	=	RAmpH;
		if(Dlg->DoModal())
		{
			RAmpL	=	Dlg->m_AmpLow;
			RAmpH	=	Dlg->m_AmpHigh;
		ActiveView->Rand(RAmpL, RAmpH);
		}
	}
	else
	{
	if(Dlg->DoModal())
		if ((Dlg->m_AmpLow==1)&&(Dlg->m_AmpHigh==1719))
		{
			AfxMessageBox("Right Values");
			ROk=1;
		}
		else
		{
		L = Rand();
		H = Rand();
		sprintf(szBuff, "Random values:  %f,  %f", L,H);
		AfxMessageBox(szBuff);
		}
	}
	delete Dlg;
}
//
void CSpectra4App::OnSpecRand()
{
	int N, NComp,j;
	float A,Z;
	TParam* pParamA;	TParam* pParamB;	TParam* pParamC;
	TComponent* pCompA;	TComponent* pCompB;	TComponent* pCompC;
	float* DataX;	float* DataY; float* Amp;	float* crY;
	N=CreatePoint;
	NComp=ActiveView->Spectrum->NSPoint;
	if (ROk&&(NComp)&&(ActiveView->Spectrum->NSComp==5)&&(ActiveView->Spectrum->BegFitComp=2))
	{
		DataX = new float [N];
		DataY = new float [3*N];
		Amp = new float [NComp];
		crY=(ActiveView->Spectrum->SpecComp+1)->GetData();
		Z=(ActiveView->Spectrum->SpecParam+1)->Zero;
		A=(ActiveView->Spectrum->SpecParam+1)->Amp;
		for (j=0;j<NComp;j++)
		{
			*(Amp+j)=(*crY)*A+Z;
			if (*(Amp+j)<0) *(Amp+j)=0;
			crY++;
		}
		pParamA = (ActiveView->Spectrum->SpecParam+2);
		pParamB = (ActiveView->Spectrum->SpecParam+3);
		pParamC = (ActiveView->Spectrum->SpecParam+4);
		for (j=0;j<N;j++)
			*(DataX+j)=pParamB->MaxL+(j*(pParamB->MaxH-pParamB->MaxL))/(N-1);
		pCompA = new TComponent(DataY,DataX,N);
		pCompB = new TComponent(DataY+N,DataX,N);
		pCompC = new TComponent(DataY+2*N,DataX,N);
		pCompB->Reset(pParamB);		pCompC->Reset(pParamC);
		*pCompB *= pParamB->Amp;	*pCompB += pParamB->Zero;  
		*pCompC *= pParamC->Amp;	*pCompC += pParamC->Zero;
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		else
		{
			delete ClbSpectrum;
			ClbSpectrum = new TSpectrum;
		}
		for (j=0;j<NComp;j++)
		{
			pParamA->Max=pParamA->MaxL+Rand()*(pParamA->MaxH-pParamA->MaxL);
			pParamA->Wid=pParamA->WidL+Rand()*(pParamA->WidH-pParamA->WidL);
			pParamA->Split=pParamA->SplitL+Rand()*(pParamA->SplitH-pParamA->SplitL);
			pCompA->Reset(pParamA);
			*pCompA *= *(Amp+j);	*pCompA -= *(Amp+j);  
			*pCompA += pCompB;	*pCompA += pCompC;
			pCompA->Randomize(RAmpL, RAmpH);
			ClbSpectrum->NewComp(DataX, DataY,N);
		}
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=1;
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		delete pCompA;	delete pCompB;	delete pCompC;
		delete[] DataX;	delete[] DataY;	delete[] Amp;
	}
}
//
void CSpectra4App::OnFitTrace()
{
	int nFitComp,j, NData;
	float* TraceX;
	float* TraceY;
	nFitComp=ActiveView->Spectrum->NSComp-ActiveView->Spectrum->BegFitComp;
	ActiveDocName=ActiveView->GetDocument()->GetTitle();
	if (nFitComp>0)
	{
		TraceX = new float[NCompMax];
		TraceY = new float[NCompMax*nFitComp];
		if ((NData=ActiveView->FitTrace(TraceX, TraceY))>2)
		{
			if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
			if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
			for (j=0;j<nFitComp;j++)
				ClbSpectrum->NewComp(TraceX,TraceY+j*NCompMax,NData);
			ClbDataOK = 1;
			crSeriesNum=0;
			crDocMode=1;
			CWinApp::OnFileNew();
			delete ClbSpectrum;
			ClbSpectrum = NULL;
			ClbDataOK = 0;
			crDocMode=0;
			ActiveDocName="";
			
		}
		
	delete[] TraceX;	delete[] TraceY;
	}
	
	
	//ClbSpectrum=NULL;
	//ClbDataOK = 0;
	//pMainFrame->CreateNewChild(RUNTIME_CLASS(CSpectra4View), IDR_SPECTRTYPE);
	//OnFileNew();
	//pDocTemplate->CreateNewDocument();
	
}
//
void CSpectra4App::OnFileOpen()
{
	
	
	char szTemp[] = "All Files (*.*)\0*.*\0Ledar Files \0*.rdt\0HEKA Files\0*.pul\0SCR Data Files \0*.dat\0Spectra Files (*.cfn)\0*.cfn\0";
	int nName, nData;
	//if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	CFileDialog fd(true,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,NULL,m_pMainWnd);
	fd.m_ofn.lpstrFilter = (LPSTR)szTemp;	// See previous note concerning string
	fd.m_ofn.lpstrFile = (LPSTR)szName;	// Stores the result in this variable
	fd.m_ofn.nMaxFile = sizeof(szName);
	fd.m_ofn.lpstrFileTitle = NULL;
	fd.m_ofn.nMaxFileTitle = 0;
	fd.m_ofn.lpstrTitle = " Open File";	// Title for dialog
	if (fd.DoModal()==IDOK)
	{
		if (pSpectraGridDlg ==NULL)
		{
			pSpectraGridDlg = new CSpectraGridDlg(m_pMainWnd);
			pSpectraGridDlg->Create();
			pGridCtrl = pSpectraGridDlg->GetGrid();
			pFitGridCtrl = pSpectraGridDlg->GetGrid(1);
		}
		SetFileNameExt();	
		nName=fd.m_ofn.nFilterIndex;
		//sprintf(szNum, "%i", nName);	AfxMessageBox(szNum);
		switch (nName)
		{
		case 1:
			ActiveDocName=szFileName;
			crDocMode = -1;
			if ((strcmpi(szExtName,"dat")==0)||(strcmpi(szExtName,"pgf")==0))
				ReadPULSEMac();
            else
            	if (strcmpi(szExtName,"cfn")==0)
               		ReadConfig();
				else
                  if (strcmpi(szExtName,"epr")==0)
				  {
					  //ActiveSpecWnd->FileReadEPR();
                  }
				  else
                  	if (strcmpi(szExtName,"pul")==0)
                  		ReadPULSEMac();
			                  else
                           		if (strcmpi(szExtName,"rdt")==0)
								{
									//ReadLedar();
								}
								else
									ReadASCII();
	
			break;	
		//case 2: ReadLedar(); break;
		case 3: ReadPULSEMac(); break;
		case 4:	ReadSCRData(); break;
		case 5:	ReadConfig(); break;
		//case 6: ReadPULSEMac(); break;
		}	

		crDocMode = 0;		
		
	}
	
	

}
//
void CSpectra4App::ReadASCII()
{
	if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	if (ClbSpectrum!=NULL)
	{
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		//if (int nData=ClbSpectrum->Read(szName,nBefore,nAfter,0)) 
		if (int nData=ClbSpectrum->ReadComp(szName,nBefore,nAfter,StimPoint))
		{
			ClbDataOK = 1;
			crSeriesNum=0;
			CWinApp::OnFileNew();
		}
		else AfxMessageBox("Unknown data format");
	delete ClbSpectrum;
	ClbSpectrum=NULL;
	ClbDataOK = 0;
	}
}
//
void CSpectra4App::ReadConfig()
{
	if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	if (ClbSpectrum!=NULL)
	{
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->ReadConfig(szName,1); 
		ReadCfn = 1;
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName=szFileName;
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum=NULL;
		ClbDataOK = 0;
		ReadCfn = 0;
		crDocMode=0;
		ActiveDocName="";
	}
}			
//
void CSpectra4App::ReadSCRData()
{
	const float VmaxIn=10.0;
	CFile  cFile;
	int ReadGood=0;
	short int far* Inp;
	short int far*	Param;
	char	Byte;
	char CommentStr[21];
	char TimeStr[8];
	float  Count,Mult,ControlSumm,Min,Max,Extr,LimDown,LimUp,AccumTime;
	float far* DataX;
	float far* DataY;
	float far* TraceX;
	float far* TraceY;
	float InitX,BaseY,RecTime;
	TComponent* AuxComp;
	long int ADCTime,PrevADCTime;
	short int U_Hold1,U_Hold2;
	short int Gain,TraceNum,j, AcTime;
	short int Points,MinPoint,ExtrPoint;
	int N_Data=501;
	DataX=new float[16000];
	DataY=new float[16000];
	TraceX=new float[16000];
	TraceY=new float[16000];
	Inp=new short int[10000];
	Param=new short int[20];
	
	//AfxMessageBox(szName,"Check4 ",MB_OK);
	if (cFile.Open(szName, CFile::modeRead|CFile::typeBinary))
	{
	ActiveDocName=szFileName;
	crDocMode = -1;
	//MessageBox(szName,"Check5 ",MB_OK);
	PrevADCTime=1;
	TraceNum=0;
	RecTime=0;
	U_Hold1=0;
	U_Hold2=0;
	*TimeStr='\0';
	*CommentStr='\0';
	crSeriesNum=0;
	if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
	do
	{
		
		//reading the block record
		ReadGood=cFile.Read(Inp,sizeof(short int)*N_Data);
		ReadGood=cFile.Read(&ADCTime,sizeof(long int));
		ReadGood=cFile.Read(Param,sizeof(short int)*18);
		ReadGood=cFile.Read(TimeStr,9);
		TimeStr[8]='\0';
		// reading the Commemnt String
		//ReadFile((HANDLE)hFile,CommentStr,21,(LPDWORD)&ReadGood,NULL);
		ReadGood=cFile.Read(CommentStr,21);
		CommentStr[20]='\0';
		Gain=(*(Param+10));
		Points=(*(Param+11));
		AcTime=*(Param+9); AccumTime = 0.001*AcTime;
		Mult=VmaxIn*100/(204.8*Gain);
		BaseY=0;
		BaseEnd=StimPoint-10;
		if (AutoCalib)
			for (j=BaseBeg;j<BaseEnd;j++)
				BaseY+=(*(Inp+j))/((float)(BaseEnd-BaseBeg));

		// data convertion
		ControlSumm=0;
		for (j=0;j<N_Data;j++)
		{
			Count=(*(Inp+j))-BaseY;
			ControlSumm+=fabs(Count);
			*(DataY+j)=Mult*Count;
			*(DataX+j)=(float)(j*ADCTime/1000.00);
		}
		//sprintf(szStr,"%i",ControlSumm);
		//AfxMessageBox(szStr);
		Min=200000;
		//finding minimum
		ExtrPoint=StimPoint+10;
		for (j=StimPoint+10;j<N_Data;j++)
			{
			if ( (*(DataY+j))<Min ) Min= *(DataY+j);
			ExtrPoint=j;
			}
		Extr=0;
		if ((ExtrPoint>5)&&(ExtrPoint<N_Data-6))
			for(j=0;j<10;j++)
			{
				Extr-= (*(DataY+j+ExtrPoint-5))/10;
			}
		else
			Extr=-Min;
		//finding maximum
		if (*(Param+17)==4097)
		{
			ExtrPoint=StimPoint+10;
			Max=-200000;
			for (j=StimPoint+10;j<N_Data;j++)
			{
				if ( (*(DataY+j))>Max ) Max= *(DataY+j);
				ExtrPoint=j;
			}
			if ((ExtrPoint>5)&&(ExtrPoint<N_Data-6))
				for(j=0;j<10;j++)
				{
					Extr+= (*(DataY+j+ExtrPoint-5))/10;
				}
			else
				Extr=Max-Min;
		}
		LimDown=Min;
		LimUp=fabs(Min)/5;
		//cutting the stimulus artifact
		for (j=StimPoint-11;j<StimPoint;j++)
			if ( (*(DataY+j))<LimDown ) (*(DataY+j))=LimDown;
				else
					if ( (*(DataY+j))>LimUp )   (*(DataY+j))=LimUp;

				//(*(DataY+j))*=(1000.00/MultX);
		if (ControlSumm>=1)
		{
			//Accepting Data		
			AuxComp=new TComponent((DataY),(DataX),N_Data);
			*(TraceX+TraceNum)=RecTime;
			*(TraceY+TraceNum)=Extr;
			TraceNum++;
			Max=AuxComp->MaxVal(StimPoint-10);
			Min=AuxComp->MinVal(StimPoint-10);
			if ((nBefore>4)&&(nAfter>=1))
				N_Data=AuxComp->Smooth(nBefore,nAfter);
			delete AuxComp;
			if ((((U_Hold1!=*(Param+3))||(U_Hold2!= *(Param+7))||(ADCTime!=PrevADCTime))&&(ClbSpectrum->NSComp>0))||(ClbSpectrum->NSComp>NCompMax))
			// aquisition parameters have changed
			{
			// creating new Spectrum and Document Window	
				ClbDataOK = 1;
				CWinApp::OnFileNew();	
				crSeriesNum++;
				if (ClbSpectrum != NULL) 
				{
					delete ClbSpectrum;
					ClbSpectrum = new TSpectrum;
					ClbDataOK = 0;
				}
			}
			ClbSpectrum->ADCTime=ADCTime;
			for (j=0;j<18;j++)
				*(ClbSpectrum->Param+j)= *(Param+j);
			strcpy(ClbSpectrum->TimeStr,TimeStr);
			strcpy(ClbSpectrum->CommentStr,CommentStr);
			ClbSpectrum->NewComp(DataX,DataY,N_Data);
			//AfxMessageBox("Chek1");
			//sprintf(StrBuf,"%i,%i,%i,%s",*(ActiveSpecWnd->Spectrum->Param+3),*(ActiveSpecWnd->Spectrum->Param+11),ActiveSpecWnd->Spectrum->ADCTime,ActiveSpecWnd->Spectrum->TimeStr);
			//MessageBox(StrBuf,"Check",MB_OK);
			(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->Time=RecTime;
			(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->ExtMin=Min;
			(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->ExtMax=Max;
			
			
		}
		PrevADCTime=ADCTime;
		U_Hold1=*(Param+3);
		U_Hold2=*(Param+7);
		RecTime+=AccumTime;
	}
	while (ReadGood);
	//???ActiveSpecWnd->Spectrum->RemoveComp(ActiveSpecWnd->Spectrum->NSComp-1);
	if (AutoBound)
   	{
   		TraceParam.MaxH= *(ClbSpectrum->SpecBase+(ClbSpectrum->NSPoint-1));
   		TraceParam.MaxL=TraceParam.MaxH*(StimPoint+1)/(ClbSpectrum->NSPoint);
		TraceParam.WidL=TraceParam.MaxH*0.0001;
   		TraceParam.WidH=TraceParam.MaxH*5;
		TraceParam.SplitL=TraceParam.MaxH*0.0002;
   		TraceParam.SplitH=TraceParam.MaxH*10;
		TraceParam.Max=TraceParam.MaxH*(StimPoint+8)/(ClbSpectrum->NSPoint);
   		SeekTRise1=TraceParam.WidL; SeekTRise2=TraceParam.WidH; SeekTDec1=TraceParam.SplitL; SeekTDec2=TraceParam.SplitH;
		AutoBound=0;
     }
	if (ClbSpectrum->NSComp)
	{
		ClbDataOK = 1;
		CWinApp::OnFileNew();	
	}
	crSeriesNum=0;
	if (ClbSpectrum != NULL) 
	{
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
	}

	//creating the trace window
	if ((AutoTrace)&&(TraceNum>2))
		{
		if (*(TraceX+TraceNum-1)-(*TraceX) < TraceNum*0.00001)
   			for (j=0;j<TraceNum;j++)
      			*(TraceX+j)=j*4.8;
		crDocMode=0;
		if (ClbSpectrum == NULL) 
			ClbSpectrum = new TSpectrum;
		ClbSpectrum->NewComp(TraceX,TraceY,TraceNum);
        ClbDataOK = 1;
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=-1;
		}
		   
	}
	cFile.Close();
	//CloseHandle((HANDLE)hFile);
	delete[] DataX;
	delete[] DataY;
	delete[] TraceX;
	delete[] TraceY;
	delete[] Inp;
	delete[] Param;
}
//
void CSpectra4App::ReadPULSEMac()
{
//#define SEEK_CUR CFile::current
	CFile cFile;
	char DataName[512];
	char StimName[512];
	char PulsName[512];
	char* DataExt = ".dat";
	char* StimExt = ".pgf";
	char* PulsExt = ".pul";
	char* CrChar;
	const int MagicN 		= 0x54726565;
	const int SwapMagicN 	= 0x65657254;
	int 	SwapFlag = 0;
	int hFile;
	int ReadGood =0;
	int i,j,j_ext,k;
	TRoot 	aRoot;
	TGroup	aGroup;
	int NRoot, NGroup, crGroup;
	TSweep	aSweep;
	TPulseSeries* Series;
	TPulseSeries* SwappedSeries;
	TPulseSweep*  Sweeps;
	TComponent* AuxComp;
	double SampleInt,Min,Max,BaseY,LimUp,LimDown,DoubleBuf;
	int NStim, NSeg, N_Data;
	int NSeries, crSeries, TotSeries, crSweep, NSweep, TotSweep, SweepSize;
	OFSTRUCT aFStruct;
	long int MagicNumber;
	long int NLevels;
	int IntBuf, DataMax;
	long int	LevelSize[16];
	short int	RawData[NDATA_MAX];
	short int ByteShift, ShortBuf;
	float*	DataX;
	float*	DataY;
	// finding data and stimulation file names
	for (i=0;i<16;i++) LevelSize[i]=0;
	if (strlen(szName)>1)
   	{
      //MessageBox(szName,"Check1 ",MB_OK);
	  //StimPoint = 67; 
	  BaseEnd = StimPoint-10;	
      j_ext=0;
      j=0;
      CrChar=szName;
      while ((*CrChar!='.')&&(*CrChar!='\0')&&(j_ext<510) )
      	{
         *(DataName+j_ext)=*CrChar;
         *(StimName+j_ext)=*CrChar;
		 *(PulsName+j_ext)=*CrChar;
         j_ext++; CrChar++;
         }
		}
	*(DataName+j_ext)='\0';
	*(StimName+j_ext)='\0';
	*(PulsName+j_ext)='\0';
	strcat(DataName,DataExt);
	strcat(StimName,StimExt);
	strcat(PulsName,PulsExt);
	Series = new TPulseSeries[500];
	Sweeps	= new TPulseSweep[10000];
	// reading the stimulatioin file
	if (cFile.Open(StimName, CFile::modeRead|CFile::typeBinary))
   	{
		ReadGood=cFile.Read(&MagicNumber,sizeof(long int));
		//sprintf(szStr,"MagicNumber%i",MagicNumber);
		//AfxMessageBox(szStr);
		if (MagicNumber == SwapMagicN)
   			SwapFlag = 1;
		//ReadGood=cFile.Read(&NLevels,sizeof(long int));
		if (SwapFlag) ByteShift=1;
			else ByteShift=0;
		ReadGood=cFile.Read(&IntBuf,4);
		if (SwapFlag) SwapInt(&IntBuf);
		NLevels = IntBuf;
		//sprintf(szStr,"NLevels %i",NLevels);
		//AfxMessageBox(szStr);
		for (i=0;i<NLevels;i++)
		{
			//ReadGood=cFile.Read(LevelSize,sizeof(long int)*NLevels);
			ReadGood=cFile.Read(&IntBuf,4);
			if (SwapFlag) SwapInt(&IntBuf);
			LevelSize[i] = IntBuf;
		
		}
		//sprintf(szStr,"L[0]-[3] sz: %i, %i, %i, %i",LevelSize[0],LevelSize[1],LevelSize[2],LevelSize[3]);
   		//AfxMessageBox(szStr);
		cFile.Seek(LevelSize[0],CFile::current);
		//ReadGood=cFile.Read(&NStim,sizeof(long int));
		ReadGood=cFile.Read(&IntBuf,4);
		if (SwapFlag) SwapInt(&IntBuf);
		NStim = IntBuf;
		for (i=0;i<NStim;i++)
   		{
			
			cFile.Seek(28,CFile::current);
			ReadGood=cFile.Read(&DoubleBuf,8);
			if (SwapFlag) SwapDouble(&DoubleBuf);
			SampleInt = DoubleBuf;
			(Series+i)->ScaleX = 1000*SampleInt;
			cFile.Seek(8,CFile::current);
			ReadGood=cFile.Read(&DoubleBuf,8);
			if (SwapFlag) SwapDouble(&DoubleBuf);
			(Series+i)->Interval = DoubleBuf;
			//sprintf(szStr,"interval %f",(Series+i)->Interval);
   			//AfxMessageBox(szStr);
			cFile.Seek(LevelSize[1]-52,CFile::current);
			ReadGood=cFile.Read(&IntBuf,4);
			if (SwapFlag) SwapInt(&IntBuf);
			NSeg = IntBuf;
			//sprintf(szStr,"NSeg %i, Series %i, ScaleX %f",NSeg,i,(Series+i)->ScaleX);
   			//AfxMessageBox(szStr);
			cFile.Seek((LevelSize[2]+4)*NSeg,CFile::current);

			       
		}
   	}

	cFile.Close();
	// reading the Pulse records ".pul" file
	if (cFile.Open(PulsName, CFile::modeRead|CFile::typeBinary))
	{
		ReadGood=cFile.Read(&MagicNumber,sizeof(long int));
		if (MagicNumber == SwapMagicN)
   			SwapFlag = 1;
   		else
      		SwapFlag = 0;
		if (SwapFlag) ByteShift=1;
			else ByteShift=0;
		ReadGood=cFile.Read(&IntBuf,4);		if (SwapFlag) SwapInt(&IntBuf);		NLevels = IntBuf;
		//sprintf(szStr,"NLevels %i",NLevels);
		//AfxMessageBox(szStr);
		for (i=0;i<NLevels;i++)
		{
			//ReadGood=cFile.Read(LevelSize,sizeof(long int)*NLevels);
			ReadGood=cFile.Read(&IntBuf,4);
			if (SwapFlag) SwapInt(&IntBuf);
			LevelSize[i] = IntBuf;
		
		}

		//sprintf(szStr,"L[0]-[3] sz: %i, %i, %i, %i",LevelSize[0],LevelSize[1],LevelSize[2],LevelSize[3]);
   		//AfxMessageBox(szStr);
		//cFile.Seek(sizeof(TRoot),CFile::current);
		cFile.Seek(LevelSize[0],CFile::current);
		ReadGood=cFile.Read(&NGroup,sizeof(long int));
		if (SwapFlag) SwapInt(&NGroup);
		NSeries = 0;
		TotSweep = 0; TotSeries = 0; crSeries = 0; crSweep =0; SweepSize=0;
		DataMax = 20000;
		//if (NGroup > 100)NGroup = 1;
		for (i=0;i<NGroup;i++)
   		{
			//cFile.Seek(sizeof(TGroup),CFile::current);
			cFile.Seek(LevelSize[1],CFile::current);
			ReadGood=cFile.Read(&NSeries,sizeof(long int));
			if (SwapFlag) SwapInt(&NSeries);
			for (j=0;j<NSeries;j++)
      		{
	   			//cFile.Seek(sizeof(TSeries),CFile::current);
				cFile.Seek(LevelSize[2],CFile::current);
				ReadGood=cFile.Read(&NSweep,sizeof(long int));
				if (SwapFlag) SwapInt(&NSweep);
				(Series+crSeries)->NSweep=NSweep;
				TotSweep+=NSweep;
				for (k=0;k<NSweep;k++)
         		{
   					//ReadGood=cFile.Read(&aSweep, sizeof(TSweep));
					ReadGood=cFile.Read(&aSweep, LevelSize[3]);
					//ReadGood=cFile.Read(&IntBuf,sizeof(long int));
         			ReadGood=cFile.Read(&IntBuf,4);		
					if (SwapFlag) SwapInt(&IntBuf);		
					SweepSize+=IntBuf;
					if (SwapFlag)
					{
						IntBuf=aSweep.DataPoints;	SwapInt(&IntBuf);	aSweep.DataPoints=IntBuf;
						IntBuf=aSweep.TotalPoints;	SwapInt(&IntBuf);	aSweep.TotalPoints=IntBuf;
						IntBuf=aSweep.Offset;		SwapInt(&IntBuf);	aSweep.Offset=IntBuf;
						IntBuf=aSweep.Data;			SwapInt(&IntBuf);	aSweep.Data=IntBuf;
						DoubleBuf=aSweep.DataFactor1;	SwapDouble(&DoubleBuf);	aSweep.DataFactor1=DoubleBuf;
						DoubleBuf=aSweep.DataFactor2;	SwapDouble(&DoubleBuf);	aSweep.DataFactor2=DoubleBuf;
						DoubleBuf=aSweep.RsValue;	SwapDouble(&DoubleBuf);	aSweep.RsValue=DoubleBuf;
		
					}
					IntBuf=aSweep.TotalPoints;
					(Series+crSeries)->NPoint = aSweep.TotalPoints;
					if (IntBuf > DataMax )DataMax = IntBuf;
					(Sweeps+crSweep)->Offset=aSweep.Data;
					(Sweeps+crSweep)->ScaleY=1.0e12*aSweep.DataFactor1;
					//(Sweeps+crSweep)->ScaleY=0.01;
					crSweep++;
					if (aSweep.SecondTrace)
					{
						if (aSweep.Leak)
							(Sweeps+crSweep)->Offset=aSweep.Data+2*sizeof(short int)*((Series+crSeries)->NPoint);
						else
							(Sweeps+crSweep)->Offset=aSweep.Data+sizeof(short int)*((Series+crSeries)->NPoint);
						(Sweeps+crSweep)->ScaleY=1.0e12*aSweep.DataFactor1;
						//(Sweeps+crSweep)->ScaleY=0.01;
						crSweep++;
						(Series+crSeries)->NSweep++;
						TotSweep++;
					}
					
				}
				//sprintf(szStr,"Check3 Series %i, %i Sweeps, %i points", crSeries,NSweep,(Series+crSeries)->NPoint);
   				//AfxMessageBox(szStr);
				crSeries++;	TotSeries++;
      		}
		}
		if (SwapSeries)
		{
			SwappedSeries=(TPulseSeries*)malloc(TotSeries*sizeof(TPulseSeries));
			crSeries=0;
			for (j=0;j<TotSeries;j++)
				(SwappedSeries+j)->NSweep=0;
			SwappedSeries->NSweep=Series->NSweep;
			SwappedSeries->NPoint=Series->NPoint;
			SwappedSeries->ScaleX=Series->ScaleX;
			for (j=1;j<TotSeries;j++)
			{
				if( ((Series+j)->ScaleX!=(Series+j-1)->ScaleX)||((Series+j)->NPoint!=(Series+j-1)->NPoint) )
				{
					crSeries++;
					(SwappedSeries+crSeries)->NSweep=(Series+j)->NSweep;
					(SwappedSeries+crSeries)->NPoint=(Series+j)->NPoint;
					(SwappedSeries+crSeries)->ScaleX=(Series+j)->ScaleX;

				}
				else
					(SwappedSeries+crSeries)->NSweep+=(Series+j)->NSweep;
			}
			TotSeries=crSeries+1;
			for (j=0;j<TotSeries;j++)
			{
				(Series+j)->NPoint=(SwappedSeries+j)->NPoint;
				(Series+j)->NSweep=(SwappedSeries+j)->NSweep;
				(Series+j)->ScaleX=(SwappedSeries+j)->ScaleX;
			}
			free(SwappedSeries);
		}
	}
	cFile.Close();
	// reading raw data
	if (cFile.Open(DataName, CFile::modeRead|CFile::typeBinary))
   	{
		
		//RawData=(short int*)malloc(DataMax*sizeof(short int));	
		DataX=new float[DataMax];	DataY=new float[DataMax];
		crSweep = 0;
		crDocMode = -1;
		crSeriesNum = 1;
		ActiveDocName=szFileName;
		//sprintf(szStr,"Series %i",TotSeries);
		//AfxMessageBox(szStr);
		
		for (j=0;j<TotSeries;j++)
      	{
			N_Data=(Series+j)->NPoint;
			if (N_Data > 2)	
				{
				//if (ClbSpectrum == NULL) 
				ReadSpectrum = new TSpectrum;
				for (k=0;k<N_Data;k++)
					*(DataX+k)=k*(Series+j)->ScaleX;
					//*(DataX+k)=0.4*k;
				sprintf(szNum, "Series %i", j+1);
				Indicator1=szNum;
				pStatusBar->SetPaneText(0,Indicator1);
				for(i=0;i<(Series+j)->NSweep;i++)
				{
					cFile.Seek((Sweeps+crSweep)->Offset,CFile::begin);
					ReadGood=cFile.Read(RawData,sizeof(short int)*N_Data);
					Min=  200000; Max= -200000;
					for (k=0;k<N_Data;k++)
					{
						ShortBuf = *(RawData+k);
						if (SwapFlag) SwapShortInt(&ShortBuf);
            			*(DataY+k)=(ShortBuf)*(Sweeps+crSweep)->ScaleY;
						if (k>StimPoint+5)
						{
							if (*(DataY+k)>Max) Max=*(DataY+k);
							if (*(DataY+k)<Min) Min=*(DataY+k);
						}

					}
					//*(DataY+k)=(float)(*(RawData+k));
					//baseline compensation
					
					if (AutoCalib)
            		{
						BaseY = 0;
						for (k=5;k<StimPoint-10;k++)
               			BaseY+=(*(DataY+k))/(StimPoint-15);
						for (k=0;k<N_Data;k++)
            			*(DataY+k)-=BaseY;
						Max-=BaseY;	Min-=BaseY;
					}
					
            //finding extremum and cutting the stim. artifact
					Min=  200000; Max= -200000;
					for (k=StimPoint+10;k<N_Data;k++)
					{
			   			if (*(DataY+k)>Max) Max=*(DataY+k);
						if (*(DataY+k)<Min) Min=*(DataY+k);
					}
					LimDown=Min-0.3*(Max-Min); LimUp=Max+0.3*(Max-Min);
					for (k=StimPoint-10;k<StimPoint+10;k++)
					{
            			if ((*(DataY+k))>0.3*LimUp) (*(DataY+k))=0.3*LimUp;
						if ((*(DataY+k))<0.3*LimDown) (*(DataY+k))=0.3*LimDown;
					}
					
					crSweep++;
					//AuxComp=new TComponent(DataY,DataX,N_Data);
					//if ((nBefore>4)&&(nAfter>=1))
					//	N_Data=AuxComp->Smooth(nBefore,nAfter);
					ReadSpectrum->NewComp(DataX,DataY,N_Data,1);
					(ReadSpectrum->SpecParam+ReadSpectrum->NSComp-1)->ExtMax=Max;
					(ReadSpectrum->SpecParam+ReadSpectrum->NSComp-1)->ExtMin=Min;
					(ReadSpectrum->SpecParam+ReadSpectrum->NSComp-1)->Time=(crSweep-1)*((Series+j)->Interval+0.001*N_Data*(Series+j)->ScaleX);//ReadSpectrum->NSComp-1;
					//sprintf(szBuff,"Min%f Max%f", Min,Max);
					//AfxMessageBox(szBuff);
					//delete AuxComp;
					sprintf(szNum, "Sweep %i", i+1);
					Indicator2=szNum;
					pStatusBar->SetPaneText(1,Indicator2);
				}
				ReadDataOK = 1;ClbDataOK = 0;
				CWinApp::OnFileNew();
				//if (ClbSpectrum != NULL) 
				//delete ClbSpectrum;
				//ClbSpectrum = NULL;
				//ClbDataOK = 0;
				crSeriesNum++;
			}
		}
		//free(RawData);	
		delete[] DataX;	delete[] DataY;
    }
   	cFile.Close();
    crSeriesNum=0;
	crDocMode=0;
	ActiveDocName="";
	delete[] Series; delete[] Sweeps;
	ReadDataOK=0;


}
//
void CSpectra4App::ReadPULSE()
{
//#define SEEK_CUR CFile::current
	CFile cFile;
	char DataName[512];
	char StimName[512];
	char* DataExt = ".dat";
	char* StimExt = ".pgf";
	char* CrChar;
	const int MagicN 		= 0x54726565;
	const int SwapMagicN 	= 0x65657254;
	int 	SwapFlag = 0;
	int hFile;
	int ReadGood =0;
	int i,j,j_ext,k;
	TRoot 	aRoot;
	TGroup	aGroup;
	int NRoot, NGroup, crGroup;
	TSweep	aSweep;
	TPulseSeries* Series;
	TPulseSweep*  Sweeps;
	TComponent* AuxComp;
	double SampleInt,Min,Max,BaseY,LimUp,LimDown;
	int NStim, NSeg, N_Data;
	int NSeries, crSeries, TotSeries, crSweep, NSweep, TotSweep, SweepSize;
	OFSTRUCT aFStruct;
	long int MagicNumber;
	long int NLevels;
	int IntBuf, DataMax;
	long int	LevelSize[16];
	short int far* RawData;
	float*	DataX;
	float*	DataY;
	// finding data and stimulation file names
	if (strlen(szName)>1)
   	{
      //MessageBox(szName,"Check1 ",MB_OK);
	  //StimPoint = 67; 
	  BaseEnd = StimPoint-10;	
      j_ext=0;
      j=0;
      CrChar=szName;
      while ((*CrChar!='.')&&(*CrChar!='\0')&&(j_ext<510) )
      	{
         *(DataName+j_ext)=*CrChar;
         *(StimName+j_ext)=*CrChar;
         j_ext++; CrChar++;
         }
		}
	*(DataName+j_ext)='\0';
	*(StimName+j_ext)='\0';
	strcat(DataName,DataExt);
	strcat(StimName,StimExt);
	Series = new TPulseSeries[500];
	Sweeps	= new TPulseSweep[10000];
	// reading the stimulatioin file
	if (cFile.Open(StimName, CFile::modeRead|CFile::typeBinary))
   	{
		ReadGood=cFile.Read(&MagicNumber,sizeof(long int));
		if (MagicNumber == SwapMagicN)
   			SwapFlag = 1;
		else
      		SwapFlag = 0;
		ReadGood=cFile.Read(&NLevels,sizeof(long int));
		ReadGood=cFile.Read(LevelSize,sizeof(long int)*NLevels);
		
		cFile.Seek(LevelSize[0],CFile::current);
		ReadGood=cFile.Read(&NStim,sizeof(long int));
		for (i=0;i<NStim;i++)
   		{
			cFile.Seek(28,CFile::current);
			ReadGood=cFile.Read(&SampleInt,8);
			(Series+i)->ScaleX = 1000*SampleInt;
			cFile.Seek(LevelSize[1]-36,CFile::current);
			ReadGood=cFile.Read(&IntBuf,4);
			if (SwapFlag) SwapInt(&IntBuf);
			NSeg = IntBuf;
			
			cFile.Seek((LevelSize[2]+4)*NSeg,CFile::current);
			       
		}
   	}

	cFile.Close();
	// reading the Pulse records ".pul" file
	if (cFile.Open(szName, CFile::modeRead|CFile::typeBinary))
	{
		ReadGood=cFile.Read(&MagicNumber,sizeof(long int));
		if (MagicNumber == SwapMagicN)
   			SwapFlag = 1;
   		else
      		SwapFlag = 0;
		
		ReadGood=cFile.Read(&NLevels,sizeof(long int));
		//ReadGood=cFile.Read(LevelSize,sizeof(long int)*4);
		ReadGood=cFile.Read(LevelSize,sizeof(long int)*NLevels);
		//sprintf(szStr,"NLevels %i",NLevels);
		//AfxMessageBox(szStr);
		//sprintf(szStr,"L[0]-[3] sz: %i, %i, %i, %i",LevelSize[0],LevelSize[1],LevelSize[2],LevelSize[3]);
   		//AfxMessageBox(szStr);
		//sprintf(szStr,"TRoot: %i TGroup %i TSeries %i TSweep %i",sizeof(TRoot),sizeof(TGroup),sizeof(TSeries),sizeof(TSweep));
   		//AfxMessageBox(szStr);
		//cFile.Seek(sizeof(TRoot),CFile::current);
		cFile.Seek(LevelSize[0],CFile::current);
		ReadGood=cFile.Read(&NGroup,sizeof(long int));
		if (SwapFlag) SwapInt(&NGroup);
		NSeries = 0;
		TotSweep = 0; TotSeries = 0; crSeries = 0; crSweep =0; SweepSize=0;
		DataMax = 0;
		//if (NGroup > 100)NGroup = 1;
		for (i=0;i<NGroup;i++)
   		{
			//cFile.Seek(sizeof(TGroup),CFile::current);
			cFile.Seek(LevelSize[1],CFile::current);
			ReadGood=cFile.Read(&NSeries,sizeof(long int));
			if (SwapFlag) SwapInt(&NSeries);
			TotSeries+=NSeries;
			for (j=0;j<NSeries;j++)
      		{
	   			//cFile.Seek(sizeof(TSeries),CFile::current);
				cFile.Seek(LevelSize[2],CFile::current);
				ReadGood=cFile.Read(&NSweep,sizeof(long int));
				if (SwapFlag) SwapInt(&NSweep);
				(Series+crSeries)->NSweep=NSweep;
				TotSweep+=NSweep;
				for (k=0;k<NSweep;k++)
         		{
   					//ReadGood=cFile.Read(&aSweep, sizeof(TSweep));
					ReadGood=cFile.Read(&aSweep, LevelSize[3]);
					ReadGood=cFile.Read(&IntBuf,sizeof(long int));
         			SweepSize+=IntBuf;
					IntBuf=aSweep.TotalPoints;
					if (SwapFlag) SwapInt(&IntBuf);
					(Series+crSeries)->NPoint = IntBuf-2;
					if (IntBuf > DataMax )DataMax = IntBuf;
					IntBuf=aSweep.Data;
					if (SwapFlag) SwapInt(&IntBuf);
					(Sweeps+crSweep)->Offset=IntBuf;
					(Sweeps+crSweep)->ScaleY=1.0e12*aSweep.DataFactor1;
					crSweep++;
					if (aSweep.SecondTrace)
					{
						if (aSweep.Leak)
							(Sweeps+crSweep)->Offset=IntBuf+2*sizeof(short int)*((Series+crSeries)->NPoint+2);
						else
							(Sweeps+crSweep)->Offset=IntBuf+sizeof(short int)*((Series+crSeries)->NPoint+2);
						(Sweeps+crSweep)->ScaleY=1.0e12*aSweep.DataFactor1;
						crSweep++;
						(Series+crSeries)->NSweep++;
						TotSweep++;
					}
				}
				//sprintf(szStr,"Check3 Series %i, %i Sweeps, %i points", crSeries,NSweep,(Series+crSeries)->NPoint);
   				//AfxMessageBox(szStr);
				crSeries++;
      		}
   		}
		
	}
	cFile.Close();
	// reading raw data
	if (cFile.Open(DataName, CFile::modeRead|CFile::typeBinary))
   	{
		
		RawData=new short int[DataMax];	
		DataX=new float[DataMax];	DataY=new float[DataMax];
		crSweep = 0;
		crDocMode = -1;
		crSeriesNum = 1;
		ActiveDocName=szFileName;
		for (j=0;j<TotSeries;j++)
      	{
			N_Data=(Series+j)->NPoint;
			if (N_Data > 2)
			{
				if (ClbSpectrum == NULL) 
					ClbSpectrum = new TSpectrum;
				for (k=0;k<N_Data;k++)
				*(DataX+k)=k*(Series+j)->ScaleX;
				sprintf(szNum, "Series %i", j+1);
				Indicator1=szNum;
				pStatusBar->SetPaneText(0,Indicator1);
				for(i=0;i<(Series+j)->NSweep;i++)
				{
					cFile.Seek((Sweeps+crSweep)->Offset,CFile::begin);
					ReadGood=cFile.Read(RawData,sizeof(short int)*N_Data);
					for (k=0;k<N_Data;k++)
            		*(DataY+k)=(*(RawData+k))*(Sweeps+crSweep)->ScaleY;
					//baseline compensation
					if (AutoCalib)
            		{
						BaseY = 0;
						for (k=5;k<StimPoint-10;k++)
               			BaseY+=(*(DataY+k))/(StimPoint-15);
						for (k=0;k<N_Data;k++)
            			*(DataY+k)-=BaseY;
					}
            //finding extremum and cutting the stim. artifact
					Min=  200000; Max= -200000;
					for (k=StimPoint+10;k<N_Data;k++)
					{
			   			if (*(DataY+k)>Max) Max=*(DataY+k);
						if (*(DataY+k)<Min) Min=*(DataY+k);
					}
					LimDown=Min-0.3*(Max-Min); LimUp=Max+0.3*(Max-Min);
					for (k=StimPoint-10;k<StimPoint+10;k++)
					{
            			if ((*(DataY+k))>0.3*LimUp) (*(DataY+k))=0.3*LimUp;
						if ((*(DataY+k))<0.3*LimDown) (*(DataY+k))=0.3*LimDown;
					}
					crSweep++;
					AuxComp=new TComponent(DataY,DataX,N_Data);
					Max=AuxComp->MaxVal(StimPoint+5);
					Min=AuxComp->MinVal(StimPoint+5);
					if ((nBefore>4)&&(nAfter>=1))
						N_Data=AuxComp->Smooth(nBefore,nAfter);
					ClbSpectrum->NewComp(DataX,DataY,N_Data);
					(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->ExtMax=Max;
					(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->ExtMin=Min;
					(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->Time=ClbSpectrum->NSComp-1;
					delete AuxComp;
					sprintf(szNum, "Sweep %i", i+1);
					Indicator2=szNum;
					pStatusBar->SetPaneText(1,Indicator2);
				}
				ClbDataOK = 1;
				CWinApp::OnFileNew();
				if (ClbSpectrum != NULL) 
				delete ClbSpectrum;
				ClbSpectrum = NULL;
				ClbDataOK = 0;
				crSeriesNum++;
			}
		}
      delete[] RawData;	delete[] DataX;	delete[] DataY;
    }
   	cFile.Close();
    crSeriesNum=0;
	delete[] Series; delete[] Sweeps;

}
//
void CSpectra4App::OnLotLoad()
{
	const int MaxL=1000;
	ifstream inpFile;
	char szTemp[] = "All Files (*.*)\0*.*\0";
	CFileDialog fd(true,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,NULL,m_pMainWnd);
	fd.m_ofn.lpstrFilter = (LPSTR)szTemp;	// See previous note concerning string
	fd.m_ofn.lpstrFile = (LPSTR)szName;	// Stores the result in this variable
	fd.m_ofn.nMaxFile = sizeof(szName);
	fd.m_ofn.lpstrFileTitle = NULL;
	fd.m_ofn.nMaxFileTitle = 0;
	fd.m_ofn.lpstrTitle = " Open File";	// Title for dialog


	char StrRead[MaxL];
	int nData, nTir,i,n,sortFlag;
	int* rawData;
	int* crData;
	//if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	fd.m_ofn.lpstrFilter = (LPSTR)szTemp;
	if (fd.DoModal()==IDOK)
	{
		nData=0;	mNum=0; 
		rawData=new int[60000];
		inpFile.open(szName,in);
		while (!inpFile.eof())
		{
			inpFile.getline(StrRead,MaxL,'\n');
			crData=rawData+8*nData;
			for(i=0;i<8;i++) *(crData+i)=0;
			sscanf(StrRead,"%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i",crData,(crData+1),(crData+2),(crData+3),(crData+4),(crData+5),(crData+6),(crData+7));
			nData++;
		}
		inpFile.close();
	
		if(*(crData+7))
		{
			bNum=1;	eNum=7;	tNum=0; mNum=49;
		}
		else
		{
			bNum=1;	eNum=5;	tNum=6; mNum=34;
		}
		if(nData)
		{
			if (LotSize)
			{
				delete[] Lotable; delete[] LotPr; delete[] DrawDist; delete[] SevenDist;	delete[] tbLotPr;	delete[] tbDrawDist;
				LotSize=0;
			}
			LotSize=nData;
			Lotable=new int[(LotSize+1)*16];
			LotPr=new float[LotSize*50];
			tbLotPr=new float[LotSize*15];
			MeanPr=new float[LotSize*50];
			DrawDist=new float[LotSize*225];
			tbDrawDist=new float[LotSize*75];
			SevenDist=new float[LotSize*2+2];
			for (nTir=0;nTir<nData;nTir++)
				for (i=0;i<8;i++)
					*(Lotable+nTir*8+i)= *(rawData+nTir*8+i);
			//sorting
			for(nTir=0;nTir<nData;nTir++)
			{
				do
				{
					sortFlag=0;
					for(i=bNum+1;i<=eNum;i++)
						if( *(Lotable+nTir*8+i)<*(Lotable+nTir*8+i-1))
						{
							sortFlag=1;
							n=*(Lotable+nTir*8+i);
							*(Lotable+nTir*8+i)=*(Lotable+nTir*8+i-1);
							*(Lotable+nTir*8+i-1)=n;

						}
				}
				while(sortFlag);
			}
			
			delete[] rawData;
			crData=Lotable+8*(LotSize-1);
			TirBeg=*(Lotable);
			TirEnd=*crData;
			LotAnalyzeFlag=-1;
			sprintf(szStr,"%i %i %i %i %i %i %i %i, Size:%i",TirEnd,*(crData+1),*(crData+2),*(crData+3),*(crData+4),*(crData+5),*(crData+6),*(crData+7),LotSize);
   			AfxMessageBox(szStr);
		}
	}
}
void CSpectra4App::OnLotAnalyze()
{
	int NData,DistOffset,FindFlag,DistCount,DrawInt;
	int* crNum;
	short int Num,Num2,i,j,k,m;
	float* TraceX;
	float* TraceY;
	float* Score;
	float* PairScore;
	float* crData;
	float Norm,Cent,S;
	CSweepNumDlg* Dlg=new CSweepNumDlg;
	Dlg->m_BegNum=1;
	Dlg->m_EndNum=TirEnd;
	ExtDocName="";
	if (Dlg->DoModal())
	{
		j=Dlg->m_EndNum;
		if(j<=TirEnd)
		{
			OldLotSize=LotSize;
			LotSize=j-TirBeg+1;
		}
	}
	delete Dlg;
	
	if (LotSize>0)
	{
		if (pSpectraGridDlg ==NULL)
		{
			pSpectraGridDlg = new CSpectraGridDlg(m_pMainWnd);
			pSpectraGridDlg->Create();
			pGridCtrl = pSpectraGridDlg->GetGrid();
			pFitGridCtrl = pSpectraGridDlg->GetGrid(1);
		}
		TraceX = new float[LotSize*(mNum+1)];
		TraceY = new float[LotSize*(mNum+1)];
		Score = new float[LotSize*2];
		PairScore = new float[LotSize*2];
		//tracy vse vypavshih nomerov
		NData=LotSize*eNum;
		for (j=0;j<LotSize;j++)
			for(i=0;i<eNum;i++)
			{
				*(TraceX+j*eNum+i)=i+(*(Lotable+j*8))*10.0;
				*(TraceY+j*eNum+i)=(float)(*(Lotable+j*8+i+1));

			}
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(TraceX,TraceY,NData);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="LotTraceAll";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		if(tNum)
		{
			for (j=0;j<LotSize;j++)
			{
				*(TraceX+j)=*(Lotable+j*8);
				*(TraceY+j)=(float)(*(Lotable+j*8+tNum));
			}
			if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
			if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
			ClbSpectrum->NewComp(TraceX,TraceY,LotSize);
			ClbDataOK = 1;
			crSeriesNum=0;
			crDocMode=-1;
			ActiveDocName="tbTraceAll";
			CWinApp::OnFileNew();
			delete ClbSpectrum;
			ClbSpectrum = NULL;
			ClbDataOK = 0;
			crDocMode=0;
			ActiveDocName="";
			
		}
		//izmenenie verojatnostei vo vremeni
		for (j=0;j<LotSize;j++)
		{
			//initializing
			*(TraceX+j)=(float)(*(Lotable+j*8));
			for(i=0;i<mNum;i++) 
			{
				*(LotPr+i*LotSize+j)=0;
				*(MeanPr+i*LotSize+j)=0;
			}
		}
		for(k=bNum;k<=eNum;k++)
		{
			Num=(*(Lotable+k));
			*(LotPr+(Num-1)*LotSize)+=1.0;
		}
		for (j=1;j<LotSize;j++)
		{
			//podschityvaem chislo vypadenii s pervogo po j tirazh
			//zapominaem predydushee chislo s 1 po j-1 tirazh
			//for(i=0;i<eNum;i++) *(SevenDist+i*LotSize+j)=0;
			for(i=0;i<mNum;i++) 
				*(LotPr+i*LotSize+j)= *(LotPr+i*LotSize+j-1);
			//uvelichvaem chlislo vypadenii dlja chicel vypavshih v j tirazh
			for(k=bNum;k<=eNum;k++)
			{
				Num=(*(Lotable+j*8+k));
				*(LotPr+(Num-1)*LotSize+j)=(*(LotPr+(Num-1)*LotSize+j-1))+1.0;
				i=(Num-1)/7;
				//*(SevenDist+i*LotSize+j)+=1.0;
			
			//sprintf(szStr,"Draw: %i, %i: %f,", j,Num,*(LotPr+(Num-1)*LotSize+j));
			//AfxMessageBox(szStr);
			}
		}
		/*
		for(j=0;j<LotSize;j++)
		{
			Cent=0;
			for(k=bNum;k<=eNum;k++)
			{
				Norm=0;
				S=0;
				Num=(*(Lotable+j*8+k));
				for(i=1;i<=mNum;i++)
				{
					S+=(i+0.0001)*exp(-(Num-i-0.001)*(Num-i-0.001));
					Norm+=exp(-(Num-i-0.001)*(Num-i-0.001));
				}
				Cent+=S/Norm;
			}
			*(SevenDist+LotSize*eNum+j)=Cent/7.0;
		}
		*/
		//normirovka
		for (j=0;j<LotSize;j++)
		{
			Norm=(float)(bNum*(j+1.0));
			for(i=0;i<mNum;i++) (*(LotPr+i*LotSize+j))/=Norm;

		}
		//sprintf(szStr,"3: %f, 9: %f, 15: %f, 32: %f",*(LotPr+3*LotSize-1),*(LotPr+9*LotSize-1),*(LotPr+15*LotSize-1),*(LotPr+32*LotSize-1));
		//AfxMessageBox(szStr);
		
		// thunBall cumul prob
		if(tNum)
		{
			for (j=0;j<LotSize;j++)
				for(i=0;i<14;i++) 
					*(tbLotPr+i*LotSize+j)=0;
			Num=(*(Lotable+tNum));
			*(tbLotPr+(Num-1)*LotSize)+=1.0;
			
			for (j=1;j<LotSize;j++)
			{
				//podschityvaem chislo vypadenii s pervogo po j tirazh
				//zapominaem predydushee chislo s 1 po j-1 tirazh
				for(i=0;i<14;i++) 
					*(tbLotPr+i*LotSize+j)= *(tbLotPr+i*LotSize+j-1);
				//uvelichvaem chlislo vypadenii dlja chicel vypavshih v j tirazh
				Num=(*(Lotable+j*8+tNum));
				*(tbLotPr+(Num-1)*LotSize+j)=(*(tbLotPr+(Num-1)*LotSize+j-1))+1.0;
			}
			//normirovka
			for (j=0;j<LotSize;j++)
			{
				Norm=(float)(j+1.0);
				for(i=0;i<14;i++) (*(tbLotPr+i*LotSize+j))/=Norm;

			}
			
		}
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		for (i=0;i<mNum;i++)
		{
			crData=LotPr+LotSize*i;
			//ClbSpectrum->NewComp(TraceX+LotSize-AnSize,crData+LotSize-AnSize,AnSize,1);
			ClbSpectrum->NewComp(TraceX+10,crData+10,LotSize-10,1);
		}
		if(tNum)
			for (i=0;i<14;i++)
			{
				crData=tbLotPr+LotSize*i;
				ClbSpectrum->NewComp(TraceX+10,crData+10,LotSize-10,1);
			}
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="CumulProbTrace";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		
		// raspredelenie po 7-kam
		for(j=0;j<LotSize;j++)
		{
			*(SevenDist+j)=0;
			for(i=bNum;i<=eNum;i++)
			{
				m=(int)((mNum-(*(Lotable+j*8+i)))/7.0);
				S=1;
				for(k=0;k<m;k++)S*=4;
				*(SevenDist+j)+=S;
			}
			*(SevenDist+LotSize+j)=0;
			for(i=bNum;i<=eNum;i++)
			{
				m=(mNum-(*(Lotable+j*8+i)))%7;
				S=1;
				for(k=0;k<m;k++)S*=4;
				*(SevenDist+LotSize+j)+=S;
			}
			
		}
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(TraceX,SevenDist,LotSize,1);
		ClbSpectrum->NewComp(TraceX,SevenDist+LotSize,LotSize,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="7thDistr";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		/*
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		crData=SevenDist+LotSize*eNum;
		ClbSpectrum->NewComp(TraceX,crData,LotSize,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="DrawCenter";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		*/
		
		//mean prob in window
		for(j=0;j<49;j++)
			for(i=0;i<LotSize;i++)
				*(MeanPr+j*LotSize+i)=0;
		DrawInt=25; //window size
		for(i=DrawInt;i<LotSize;i++)
		{
			for(j=1;j<=i;j++)
			{
				for(k=bNum;k<=eNum;k++)
				{
					Num=(*(Lotable+j*8+k));
					*(MeanPr+(Num-1)*LotSize+i)+=exp((j-i)/(DrawInt+0.01))/(eNum*DrawInt);
				}
				if(tNum)
				{
					Num=(*(Lotable+j*8+tNum));
					*(MeanPr+(mNum+Num-1)*LotSize+i)+=exp((j-i)/(DrawInt+0.01))/(DrawInt);
				}

			}
		}
		for(j=0;j<49;j++)
		{
			*(MinPr+j)=1; *(MaxPr+j)=0;
			for(i=DrawInt;i<LotSize;i++)
			{
				if(*(MinPr+j)> *(MeanPr+j*LotSize+i)) *(MinPr+j)= *(MeanPr+j*LotSize+i);
				if(*(MaxPr+j)< *(MeanPr+j*LotSize+i)) *(MaxPr+j)= *(MeanPr+j*LotSize+i);
			}
		}
			
		
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		
		for (i=0;(i<mNum)||(tNum&&(i<mNum+14));i++)
		{
			crData=MeanPr+LotSize*i;
			ClbSpectrum->NewComp(TraceX+LotSize-AnSize,crData+LotSize-AnSize,AnSize,1);
		}
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="MeanProbTrace";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
	
		//time correlation
		
		for(j=1;j<LotSize;j++)
		{
			*(TraceX+j-1)=(float)(j);
			for(k=1;k<=mNum;k++)
				*(DrawDist+(k-1)*LotSize+j-1)=0;
		}

		for(i=1;i<=mNum;i++)
		{
			DistCount=0;
			DistOffset=-1;
			for (j=0;j<LotSize;j++)
			{
				FindFlag=0;
				k=1;
				do
				{
					if(*(Lotable+j*8+k)==i)
					{
						FindFlag=1;
						
						
					}
					k++;
				}
				while((!FindFlag)&&(k<=eNum));
				if(FindFlag)
				{
					if(DistOffset>=0)
					{
						DistCount++;
						*(DrawDist+(i-1)*LotSize+j-DistOffset-1)+=1.0;
						DistOffset=j;
					}
					else
						DistOffset=j;
					
				}
			}
			for(j=0;j<LotSize-1;j++) *(DrawDist+(i-1)*LotSize+j)/=DistCount;
		}
		for(j=0;j<LotSize-1;j++)
		{
			*(DrawDist+mNum*LotSize+j)=0;
			for(i=1;i<=mNum;i++)
				*(DrawDist+mNum*LotSize+j)+=(*(DrawDist+(i-1)*LotSize+j))/(mNum+0.0000001);
		}
		/*
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		for (i=0;i<mNum;i++)
		{
			crData=DrawDist+LotSize*i;
			ClbSpectrum->NewComp(TraceX,crData,50,1);
		}
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="TimeDist";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
	*/
		if(tNum)
		{
			//tBall time correlation
			
			for(j=1;j<LotSize;j++)
			{
				*(TraceX+j-1)=(float)(j);
				for(k=1;k<=14;k++)
					*(tbDrawDist+(k-1)*LotSize+j-1)=0;
			}

			for(i=1;i<=14;i++)
			{
				DistCount=0;
				DistOffset=-1;
				for (j=0;j<LotSize;j++)
				{
					if(*(Lotable+j*8+tNum)==i)
						FindFlag=1;
					else
						FindFlag=0;
					if(FindFlag)
					{
						if(DistOffset>=0)
						{
							DistCount++;
							*(tbDrawDist+(i-1)*LotSize+j-DistOffset-1)+=1.0;
							DistOffset=j;
						}
						else
							DistOffset=j;
						
					}
				}
				for(j=0;j<LotSize-1;j++) *(tbDrawDist+(i-1)*LotSize+j)/=DistCount;
			}
			for(j=0;j<LotSize-1;j++)
			{
				*(tbDrawDist+14*LotSize+j)=0;
				for(i=1;i<=14;i++)
					*(tbDrawDist+14*LotSize+j)+=(*(tbDrawDist+(i-1)*LotSize+j))/(14.00000001);
			}
			/*
			if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
			if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
			
			for (i=0;i<14;i++)
			{
				crData=tbDrawDist+LotSize*i;
				ClbSpectrum->NewComp(TraceX,crData,50,1);
			}
			ClbDataOK = 1;
			crSeriesNum=0;
			crDocMode=-1;
			ActiveDocName="tBallTimeDist";
			CWinApp::OnFileNew();
			delete ClbSpectrum;
			ClbSpectrum = NULL;
			ClbDataOK = 0;
			crDocMode=0;
			ActiveDocName="";
			*/
			
		}
		
		//scoring
		for(j=10;j<LotSize;j++)
		{
			*(Score+j)=0.0;
			crData=LotPr+LotSize*(j-1);
			for(i=bNum;i<=eNum;i++)
			{
				Num=(*(Lotable+j*8+i));
				*(Score+j)+= 100*( (*(LotPr+Num*LotSize+j-1))/0.735294);//previous cumulative prob
			}
			*(Score+j)-=100;
		}
		
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(TraceX+40,Score,LotSize-40,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="CumPrbScore";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";

		//if(ScrSize<(LotSize/2))
		//{
		for(j=0;j<ScrSize;j++)
		{
			*(Score+j)=0.0;
			k=LotSize-ScrSize+j; 
			for(i=1;i<=ScrSize;i++)
				*(Score+j)-=(DownScore(Lotable+k*8+1,Lotable+(k-i)*8+1,eNum))/ScrSize;
		}
		
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(TraceX+LotSize-ScrSize,Score,ScrSize,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="Score";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		
		//pairs
		
		for(j=1;j<=mNum;j++)
			for(i=1;i<=mNum;i++) *(LotPair+j*50+i-1)=0;
		for(i=0;i<mNum;i++) *(LotPair+i)=i+1.0;
		for (j=0;j<LotSize;j++)
			for(i=bNum;i<=eNum;i++)
			{
				Num=(*(Lotable+j*8+i));
				for(k=bNum;k<=eNum;k++)
				{
					Num2=(*(Lotable+j*8+k));
					if(Num2!=Num)
					*(LotPair+Num*50+Num2-1)+=(float)(1.0/LotSize);
				}
			}
		for(j=0;j<ScrSize;j++)
		{
			*(PairScore+j)=0.0;
			k=LotSize-ScrSize+j; 
			for(i=1;i<=ScrSize;i++)
				*(PairScore+j)=ScorePairs(Lotable+k*8+1,eNum);
		}
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(TraceX+LotSize-ScrSize,PairScore,ScrSize,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="PairScore";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		// matches in the draws
		m=5;
		for(j=m;j<LotSize-m;j++)
		{
			*(TraceX+j)=(float)j;
			for(k=0;k<m;k++)
				*(TraceY+k*LotSize+j)=(float)MatchNum(LotSize-1-k,j,m);
		}
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		for(k=0;k<m;k++)
			ClbSpectrum->NewComp(TraceX+m,TraceY+k*LotSize,LotSize-2*m,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="Matches";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		//}
	/*
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		for (i=1;i<=mNum;i++)
		{
			crData=LotPair+50*i;
			ClbSpectrum->NewComp(LotPair,crData,mNum,1);
		}
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="PairCount";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		*/
		LotAnalyzeFlag=0;
		delete[] TraceX;	delete[] TraceY; delete[] Score;	delete[] PairScore;
		
	}
}
void CSpectra4App::OnLotRateHst()
{
	
	char szTemp[] = "All Files (*.*)\0*.*\0";
	CFileDialog fd(true,NULL,NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,NULL,m_pMainWnd);
	fd.m_ofn.lpstrFilter = (LPSTR)szTemp;	// See previous note concerning string
	fd.m_ofn.lpstrFile = (LPSTR)szName;	// Stores the result in this variable
	fd.m_ofn.nMaxFile = sizeof(szName);
	fd.m_ofn.lpstrFileTitle = NULL;
	fd.m_ofn.nMaxFileTitle = 0;
	fd.m_ofn.lpstrTitle = " Open SumPlace File";	// Title for dialog
	int nRate,nData,j,i;
	float PlSum,PlSum2;
	float* ScoreY;
	float* ScoreX;
	float* ScoreZ;
	float* ScoreM;
	float plSumTab[5];
	if (fd.DoModal()==IDOK)
	{
		if(PlaceSumSpec==NULL)
			PlaceSumSpec = new TSpectrum;
		else
			PlaceSumSpec->ClearAllComp();
		PlaceSumSpec->ReadComp(szName,nBefore,nAfter,StimPoint);
		nRate=PlaceSumSpec->NSComp-1;
		nData=PlaceSumSpec->NSPoint;
		//sprintf(szStr,"%i", nData);
		//AfxMessageBox(szStr,MB_YESNO);

	}
	if(nRate&&nData)
	{
		fd.m_ofn.lpstrTitle = " Open SumPlaceHst File";
		if (fd.DoModal()==IDOK)
		{
			if(PlaceHstSpec==NULL)
				PlaceHstSpec = new TSpectrum;
			else 
				PlaceHstSpec->ClearAllComp(0);
			PlaceHstSpec->ReadComp(szName,nBefore,nAfter,StimPoint);
		
		}
		fd.m_ofn.lpstrTitle = " Open RelPlHst File";
		if (fd.DoModal()==IDOK)
		{
			if(RelPlaceHstSpec==NULL)
				RelPlaceHstSpec = new TSpectrum;
			else 
				RelPlaceHstSpec->ClearAllComp(0);
			RelPlaceHstSpec->ReadComp(szName,nBefore,nAfter,StimPoint);
			RateHstFlag=1;
		}
		fd.m_ofn.lpstrTitle = " Open PlSum34 dist File";
		if (fd.DoModal()==IDOK)
		{
			if(PlSum34Spec==NULL)
				PlSum34Spec = new TSpectrum;
			else 
				PlSum34Spec->ClearAllComp(0);
			PlSum34Spec->ReadComp(szName,nBefore,nAfter,StimPoint);
			RateHstFlag=1;
		}
		ScoreX=new float[1000];
		ScoreY=new float[1000];
		ScoreZ=new float[1000];
		ScoreM=new float[1000];
		for(j=0;j<nData;j++)
		{
			*(ScoreX+j)=*(PlaceSumSpec->SpecBase+j);
			*(ScoreY+j)=0;
			*(ScoreZ+j)=0;
			
			PlSum2=*((PlaceSumSpec->SpecComp+2)->GetData()+j);
			PlSum2*=(PlaceSumSpec->SpecParam+2)->Amp;
			PlSum2+=(PlaceSumSpec->SpecParam+2)->Zero;
			for(i=1;i<=nRate;i++)
			{
				PlSum=*((PlaceSumSpec->SpecComp+i)->GetData()+j);
				PlSum*=(PlaceSumSpec->SpecParam+i)->Amp;
				PlSum+=(PlaceSumSpec->SpecParam+i)->Zero;
				plSumTab[i-1]=PlSum;
				*(ScoreY+j)+=(PlaceHstSpec->SpecComp+i)->OutSpline(PlSum);
				if((i!=2)&&(i<nRate))
					*(ScoreZ+j)+=(RelPlaceHstSpec->SpecComp+i)->OutSpline(PlSum/PlSum2);
			}
			*(ScoreM+j)=MatchScore(plSumTab)-5;
		}
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		ClbSpectrum->NewComp(ScoreX,ScoreY,nData,1);
		ClbSpectrum->NewComp(ScoreX,ScoreZ,nData,1);
		ClbSpectrum->NewComp(ScoreX,ScoreM,nData,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="PlSumScores";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		delete[] ScoreX; delete[] ScoreY; delete[] ScoreZ; delete[] ScoreM;
		

	}
}
void CSpectra4App::OnLotPredict()
{
	
	float*	xBase;
	float*	yData;
	float*  crRating;
	float*	crDrawDist;
	float*  ReportData;
	float plSumTab[5];
	int* crN;
	int j,i,N,k,k0,m,dN,dK,nData,FindFlag,nRate,jOffset,jSize,CombCount,CombSelTot,ShiftFlag;
	int sortFlag,sortBuf;
	int tDist[6]={20,8,20,4,20,50};
	int numFav[500];
	int totFav=75;
	int drawFav[10];
	int	totDrawFav;
	int rateLim[6][3];
	float ACscore, Period, sdPer,meanY,S,S2,S7,Score,CumulScr,PairScr,Scale, PlSum34, PlSum34Rate,MatchScr,SevDist,SevDistT,LimScore;
	float tmpBase[100];
	float Place[111];
	float GenPlace[500];
	int DrawInt=8;
	float begRel1=0.1;	float endRel1=1.252;	float begRel2=1.58; float endRel2=2.92;
	float begPlSum=0.86; float endPlSum=3.352;	float begPair=-0.08;	float endPair=3.95;
	ExtDocName="";
	TPhourier aFilter = {0.001,-1,0.025,0.025,3,1,0};
	TSpectrum* Spectrum = ActiveView->Spectrum;
	TFitParam FitState=ActiveView->crFitState;
	Spectrum->CreateMode=RIDEC_O;
	int nACData=2*Spectrum->NSPoint;
	xBase=new float[nACData+1];
	yData=new float[2*nACData+1];
	ReportData= new float[3000];
	N=Spectrum->NSComp-1;
	dN=4;	nRate=5;
	drawFav[0]=310;	drawFav[1]=274;	drawFav[2]=226;	drawFav[3]=173;	drawFav[4]=142;	drawFav[5]=97;
	drawFav[6]=59;	drawFav[7]=38;	drawFav[8]=39;	drawFav[9]=10;
	totDrawFav=10;
	numFav[0]=1;	numFav[1]=2;	numFav[2]=8;	numFav[3]=3;	numFav[4]=29;	numFav[5]=14;	numFav[6]=17;
	rateLim[0][0]=-1;	rateLim[0][1]=282;	rateLim[0][2]=404;	
	rateLim[1][0]=1;	rateLim[1][1]=278;	rateLim[1][2]=403;
	rateLim[2][0]=1;	rateLim[2][1]=162;	rateLim[2][2]=340;
	rateLim[3][0]=1;	rateLim[3][1]=190;	rateLim[3][2]=388;
	rateLim[4][0]=1;	rateLim[4][1]=197;	rateLim[4][2]=388;
	rateLim[5][0]=-1;	rateLim[5][1]=263;	rateLim[5][2]=388;
	totFav=0;
	for (j=0;j<34;j++)
			numFav[totFav+j]=j+1;
	totFav+=34;
	for(m=0;m<totDrawFav;m++)
		for(k=-2;k<3;k++)
			for(i=0;i<eNum;i++)
			{
				numFav[totFav]= *(Lotable+(drawFav[m]+k)*8+i+1);
				totFav++;
			}
	
	for (j=1;j<=N;j++)
		*(tmpBase+j-1)=(float)(j);
	if(LotAnalyzeFlag==0)
	{
		
		for (j=1;j<=N;j++)
		{
			nData=Spectrum->Extrapolate(xBase,yData,&aFilter,&FitState,dN,j);
			//nData=Spectrum->NSPoint;
			sprintf(szNum, "Sweep %i", j);
			Indicator2=szNum;
			pStatusBar->SetPaneText(1,Indicator2);
			if(j<=mNum)
			{
				crRating=Rating+mNum; jOffset=1;	jSize=mNum; k0=1;	crDrawDist=DrawDist;
			}
			else
			{
				crRating=tbRating;	jOffset=mNum+1;	jSize=14;	k0=tNum;	crDrawDist=tbDrawDist;
			}
			Scale=(Spectrum->SpecParam+j)->Zero+(Spectrum->SpecParam+j)->Amp;
			*(crRating+j-jOffset)=((*(yData+nData-dN+1))-(*(yData+nData-dN)));
			dK=2;
			meanY=*(yData+nData-dN);
			for(i=1;i<dK;i++)
				meanY+=*(yData+nData-dN+i);
			meanY/=dK;
			//*(Rating+2*N+j-1)=meanY-(*(yData+nData-dN-1));
			*(crRating+jSize+j-jOffset)=((*(yData+nData-dN-1))-meanY);
			
			//rating by mean prob
			meanY= *(MeanPr+(j-1)*LotSize+LotSize-1);
			*(crRating+2*jSize+j-jOffset)=(MaxPr[j-1]-meanY)/(MaxPr[j-1]-MinPr[j-1]);
			//rating by time inetrvals
			for(m=0;m<1;m++)
			{
				*(crRating+(m+3)*jSize+j-jOffset)=0; 
				for(i=0;i<tDist[m];i++)
				{
					FindFlag=0;
					k=k0;
					do
					{
						if(*(Lotable+(LotSize-i-1)*8+k)==j-jOffset+1)
							FindFlag=1;
						k++;
					}
					while((!FindFlag)&&(k<=eNum));
					if(FindFlag)
					{
						if(m)
							*(crRating+(m+3)*jSize+j-jOffset)+=1.5*(*(crDrawDist+(j-jOffset)*LotSize+i))+0.5*(*(crDrawDist+jSize*LotSize+i));
						else
							*(crRating+(m+3)*jSize+j-jOffset)+= *(crDrawDist+jSize*LotSize+i);
						if(!i)
							*(crRating+(m+3)*jSize+j-jOffset)+=(eNum+1)*(*(crDrawDist+jSize*LotSize+i));

					}

				}
			}
		
		}
		for(i=1;i<nRate;i++)
		{
			S=0;
			for(j=0;j<mNum;j++) S+=*(Rating+i*mNum+j);
			for(j=0;j<mNum;j++)
			{
				S2=*(Rating+i*mNum+j)/S;
				*(Rating+i*mNum+j)=S2*eNum;
			}
		}
		RateWt[0][0]=1;RateWt[0][1]=1;RateWt[0][2]=1;RateWt[0][3]=1;RateWt[0][4]=1;
		RateWt[1][0]=3;RateWt[1][1]=1;RateWt[1][2]=1;RateWt[1][3]=1;RateWt[1][4]=-2;
		RateWt[2][0]=2;RateWt[2][1]=2;RateWt[2][2]=1;RateWt[2][3]=1;RateWt[2][4]=-2;
		RateWt[3][0]=2;RateWt[3][1]=1;RateWt[3][2]=2;RateWt[3][3]=1;RateWt[3][4]=-2;
		RateWt[4][0]=2;RateWt[4][1]=1;RateWt[4][2]=1;RateWt[4][3]=2;RateWt[4][4]=-2;
		for(k=0;k<5;k++)
			for(j=0;j<mNum;j++)
			{
				*(GenRating+k*mNum+j)=0;
				for(i=0;i<nRate;i++)
					*(GenRating+k*mNum+j)+=RateWt[k][i]*(*(Rating+i*mNum+j));

			}
		for(i=0;i<nRate;i++)
			SortRating(tmpBase,GenRating+i*mNum,mNum);
		for (i=1;i<nRate;i++)
		{
			SortRating(tmpBase,Rating+i*mNum,mNum);
			if(tNum)
				SortRating(tmpBase,tbRating+(i-1)*14,14);
		}
		
	}

	CombCount=0;
	CombSelTot=0;
	do
	{
		if(*(Lotable+8*(LotSize-1))==TirEnd)
		{
		
		
			//dobavlyaem novyi tirazh
			CombCount++;
			for(i=0;i<eNum;i++)	*(Lotable+LotSize*8+i+1);
			if(CombSelTot<30)
			{
				i=0;
				do
				{
					S2=Rand();
					k=1;
					S=*(Rating+1000+k-1);
					while((k<mNum)&&(S2>S)) 
					{
						S+=*(Rating+1000+k-1);
						k++;
					}
					if(!i)
					{
						*(Lotable+LotSize*8+i+1)=k;
						i++;
					}
					else
					{
						N=0;
						for(m=0;m<=i;m++)
							if (*(Lotable+LotSize*8+m)==k) N++;
						//N=0;
						if(!N)
						{
							*(Lotable+LotSize*8+i+1)=k;
							i++;
						}
					}
					
				}
				while(i<eNum);
			}
			else
			{
				i=0;
				do
				{
					S2=Rand();
					k=(int)(S2*(totFav-0.9));
					N=0;
					for(m=0;m<=i;m++)
						if (*(Lotable+LotSize*8+m)==numFav[k]) N++;
					
					//N=0;
					if(!N)
					{
						*(Lotable+LotSize*8+i+1)=numFav[k];
						i++;
					}
					
				}
				while(i<eNum);
			}
			ShiftFlag=CombSelTot%2;
			if((ShiftFlag)&&(CombSelTot<30))
			//deflecting
				for(i=0;i<eNum;i++)
				{
					S2=Rand();
					k=(int)(S2*2.5-1.25);
					k+=(*(Lotable+LotSize*8+i+1));
					if(k<1)k=mNum; 
					if(k>mNum)k=1;
					*(Lotable+LotSize*8+i+1)=k;
				}
		
		}
		
		//scoring 
		crN=Lotable+LotSize*8+1;
		
		//Score=0;
		//for(m=1;m<=ScrSize;m++)
			//Score-=(DownScore(crN,Lotable+(LotSize-m)*8+1,eNum))/ScrSize;
		PairScr=ScorePairs(crN,eNum);
		//finding places
		SortRating(tmpBase,Rating,mNum);
		for(k=0;k<nRate;k++)
			for(m=0;m<eNum;m++)
				*(Place+k*11+m)=0.0;
		for(i=0;i<eNum;i++)
			for(j=1;j<=mNum;j++)
				for(k=0;k<nRate;k++)
				{
					if(*(Lotable+LotSize*8+i+1)==((int)(*(Rating+mNum*k+j-1))))
						*(Place+k*11+i)=*(tmpBase+j-1);
				}
		//places for all the numbers
		m=0;
		for(j=0;j<mNum;j++)
		{
			*(GenPlace+j)=(float)(j+1);
			for(k=0;k<nRate;k++)
			{
				i=0;
				while((i<mNum)&&((j+1)!=(int)(*(Rating+mNum*k+i)))) i++;
				*(GenPlace+50*(k+1)+j)=*(tmpBase+i);
			}
			for(i=0;i<eNum;i++)
			{
				if(*(Lotable+LotSize*8+i+1)==(j+1))
				{
					*(GenPlace+mNum+m)=(float)(100+j+1);
					for(k=0;k<nRate;k++) *(GenPlace+50*(k+1)+mNum+m)=*(GenPlace+50*(k+1)+j);
					m++;
				}
			}
		}
		S2=0;meanY=0;
		for(k=0;k<nRate;k++)
		{
			*(Place+k*11+7)=0;
			for(i=0;i<eNum;i++)
			{
				m=(int)((mNum-(*(Place+k*11+i)))/7.0);
				S=1;
				for(j=0;j<m;j++)S*=4;
				*(Place+k*11+7)+=S;
			}
			meanY+=(*(Place+k*11+7))/nRate;
			if(RateHstFlag)
			{
				S2+=(PlaceHstSpec->SpecComp+k+1)->OutSpline(*(Place+k*11+7));
			}
		}
		S=0; 
		if(RateHstFlag)
			for(k=0;k<nRate-1;k++)
			{
				if(k!=1)
					S+=(RelPlaceHstSpec->SpecComp+k+1)->OutSpline((*(Place+k*11+7))/(*(Place+11+7)));
			}
		PlSum34= *(Place+3*11+7)+ (*(Place+4*11+7));
		if(*(Lotable+8*(LotSize-1))==TirEnd)
		{
			crN=Lotable+LotSize*8+1;
			//checking 7th distr
			SevDist=0;
			for(i=0;i<eNum;i++)
			{
				m=(int)((mNum-(*(crN+i)))/7.0);
				S7=1;
				for(k=0;k<m;k++)S7*=4;
				SevDist+=S7;
			}
			SevDistT=0;
			for(i=0;i<eNum;i++)
			{
				m=((int)(mNum-(*(crN+i))))%7;
				S7=1;
				for(k=0;k<m;k++)S7*=4;
				SevDistT+=S7;
			}
			
			j=0;
			while((j<PlSum34Spec->NSPoint)&&(PlSum34 > *(PlSum34Spec->SpecBase+j)-0.1)) j++;
			PlSum34Rate= *(PlSum34Spec->SpecComp->GetData()+j-1); 
			PlSum34Rate*=PlSum34Spec->SpecParam->Amp;PlSum34Rate+=PlSum34Spec->SpecParam->Zero;
			for(i=0;i<nRate;i++)
			{
					
					plSumTab[i]=*(Place+i*11+7);
			}
			MatchScr=MatchScore(plSumTab);
			//cumul score
			CumulScr=0.0;
			for(i=bNum;i<=eNum;i++)
			{
				m= *(crN+i);
				CumulScr+= 100*((*(LotPr+m*LotSize+LotSize-1))/0.735294); 
			}
			CumulScr-=100;
			//if(( (MatchScr>4)&&(MatchScr<45.3)&&(PlSum34Rate>2.4)&&(PairScr>begPair)&&(S2>begPlSum)&&(S2<endPlSum)&&(((S>begRel1)&&(S<endRel1))||((S>begRel2)&&(S<endRel2)))))
			if((ShiftFlag)||( (MatchScr>4)&&(MatchScr<45.3)&&(PlSum34Rate>2.4)&&(PairScr>begPair)&&(S2>begPlSum)&&(S2<endPlSum)&&(((S>begRel1)&&(S<endRel1))||((S>begRel2)&&(S<endRel2)))))
			{
				//sprintf(szStr,"Check2: %i",ShiftFlag);
				//AfxMessageBox(szStr);
				/*
				if(tNum)
					sprintf(szStr,"Continue?: %i %i %i %i %i Sum34:%2.0f Scr: %3.1f Pairs: %2.4f",*crN,*(crN+1),*(crN+2),*(crN+3),*(crN+4), PlSum34, PlSum34Rate,PairScr);
				else
					sprintf(szStr,"Continue?: %i %i %i %i %i %i %i  Sum3&4 :%2.0f Scr: %3.1f Pairs: %2.4f",*crN,*(crN+1),*(crN+2),*(crN+3),*(crN+4),*(crN+5),*(crN+6), PlSum34, PlSum34Rate,PairScr);
				LotFlag=AfxMessageBox(szStr,MB_YESNO);
				*/
				//sprintf(szStr,"Rates# %2.0f %2.0f %2.0f %2.0f %2.0f Mn:%2.0f Sc: %4.3f Rel:%4.3f Count %i",*(Place+7),*(Place+11+7),*(Place+2*11+7),*(Place+3*11+7),*(Place+4*11+7),meanY,S2,S,CombCount);
				//AfxMessageBox(szStr);
				*(ReportData+CombSelTot)=(float)CombCount;
				//sorting
				do
				{
					sortFlag=0;
					for(i=0;i<eNum-1;i++)
					if(*(crN+i)> *(crN+i+1))
					{
						sortFlag=1;
						sortBuf=*(crN+i);	*(crN+i)= *(crN+i+1);	*(crN+i+1)=sortBuf;
					}
				}
				while(sortFlag);
				for(i=0;i<eNum;i++)
					*(ReportData+(i+1)*100+CombSelTot)=(float)(*(crN+i));
				for(i=0;i<nRate;i++)
				{
					*(ReportData+(i+eNum+1)*100+CombSelTot)=*(Place+i*11+7);
		
				}
				//checking colors
				LimScore=0;
				for(i=0;i<=nRate;i++)
				{
					k=(int)(*(ReportData+(i+eNum+1)*100+CombSelTot));
					if(k<rateLim[i][1]) 
						LimScore-=rateLim[i][0];
					else
					{
						if(k>rateLim[i][2])
							LimScore+=rateLim[i][0];
					}
		
				}
				//*(ReportData+(eNum+nRate+1)*100+CombSelTot)=PlSum34;
				//*(ReportData+(eNum+nRate+2)*100+CombSelTot)=PlSum34Rate;
				//*(ReportData+(eNum+nRate+3)*100+CombSelTot)=S;
				//*(ReportData+(eNum+nRate+4)*100+CombSelTot)=S2;
				*(ReportData+(eNum+nRate+1)*100+CombSelTot)=SevDist;
				*(ReportData+(eNum+nRate+2)*100+CombSelTot)=SevDistT;
				*(ReportData+(eNum+nRate+3)*100+CombSelTot)=MatchScr;
				*(ReportData+(eNum+nRate+4)*100+CombSelTot)=CumulScr;
				*(ReportData+(eNum+nRate+5)*100+CombSelTot)=LimScore;
				CombSelTot++;
				LotFlag=6;
			}
			else
				LotFlag=6;
		}
		//finding tBal place
		else
		{
			sprintf(szStr,"Rates# %2.0f %2.0f %2.0f %2.0f %2.0f Mn:%2.0f Sc:%4.3f Rel:%4.3f",*(Place+7),*(Place+11+7),*(Place+2*11+7),*(Place+3*11+7),*(Place+4*11+7),meanY,S2,S);
			AfxMessageBox(szStr);
			if(tNum)
			{
				for(j=1;j<=14;j++)
					for(k=0;k<nRate-1;k++)
					{
						if(*(Lotable+LotSize*8+tNum)==((int)(*(tbRating+14*k+j-1))))
							*(Place+k*11+10)=*(tmpBase+j-1);
					}
				sprintf(szStr,"tBall rate: %2.0f  %2.0f  %2.0f  %2.0f",*(Place+10),*(Place+11+10),*(Place+2*11+10),*(Place+3*11+10));
				AfxMessageBox(szStr);
			}
		}
	//LotSize=OldLotSize; OldLotSize=0;
	}while ((LotFlag==6)&&(CombSelTot<99)&&(CombCount<10000));
	if(CombSelTot>2)
	{
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		for (i=0;i<eNum+nRate+5;i++)
			ClbSpectrum->NewComp(ReportData,ReportData+(i+1)*100,CombSelTot,1);
		
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="Report";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
	}	
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		if(LotAnalyzeFlag==0)
		{
			for (i=0;i<nRate;i++)
				ClbSpectrum->NewComp(tmpBase,Rating+i*mNum,mNum,1);
			LotAnalyzeFlag++;
		}
		else
			ClbSpectrum->NewComp(tmpBase,Rating,mNum,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="Rating";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		
		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		for (i=0;i<nRate-1;i++)
				ClbSpectrum->NewComp(tmpBase,tbRating+i*14,14,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="tBall Rating";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";

		if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
		if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		for (i=0;i<nRate;i++)
			ClbSpectrum->NewComp(GenPlace,GenPlace+(i+1)*50,mNum+eNum,1);
		ClbDataOK = 1;
		crSeriesNum=0;
		crDocMode=-1;
		ActiveDocName="Places";
		CWinApp::OnFileNew();
		delete ClbSpectrum;
		ClbSpectrum = NULL;
		ClbDataOK = 0;
		crDocMode=0;
		ActiveDocName="";
		
		//sprintf(szStr,"Flag %i", LotAnalyzeFlag);
		//AfxMessageBox(szStr);

	if(RateHstFlag)
	{
		delete PlaceHstSpec; RateHstFlag=0; PlaceHstSpec=NULL;delete RelPlaceHstSpec; RelPlaceHstSpec=NULL;
		delete PlaceSumSpec; PlaceSumSpec=NULL;
	}
	delete[] xBase;
	delete[] yData;
	delete[] ReportData;

}		
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSpectra4App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSpectra4App message handlers

