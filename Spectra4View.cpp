// Spectra4View.cpp : implementation of the CSpectra4View class
//

#include "stdafx.h"
#include "Spectra4.h"

#include "Spectra4Doc.h"
#include "Spectra4View.h"
#include "Spectra4Dlg.h"
#include "GridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TSpectrum* ClbSpectrum;
extern int ClbDataOK;
extern CSpectra4Doc* ActiveDoc;
extern CStatic* SpecStatic;
extern CSpectra4Dlg* pSpectradDlg;
extern CSpectraGridDlg* pSpectraGridDlg;
extern CGridCtrl* pGridCtrl;
extern CGridCtrl* pFitGridCtrl;
extern CSpectraDlgBar* pDialogBar;
extern CStatusBar* pStatusBar;
extern CSpectra4View* ActiveView;
extern CSpectra4App* SpecApp;
extern TPhourier Filter;
extern float FilterDepth;
//extern CMainFrame* pMainFrame;

extern char szNum[64];	
extern char szBuff[256];
extern char szName[256];
extern char szExtName[8];
extern char szFileName[256];
extern int	BaseBeg;
extern int	BaseEnd;
extern int	StimPoint;
extern void SetFileNameExt();

CRect rect, rc;
CRect StaticRect;
extern CString Indicator1;
extern CString Indicator2;
extern CString Indicator3;

extern int CreatePoint;
float	StepWid;
float 	StepMax;
float 	StepSplit;
float	StepMix;
int	NCicleMax = 25;	int	NStepMax=77;	float	AccurH=0.264;	float	AccurHrej=0.289;	float	AccurL=1.0e-6;
float SeekAmpL=100;	float SeekAmpH=1000;
int IgnoreSelComp=0;
/////////////////////////////////////////////////////////////////////////////
// CSpectra4View

IMPLEMENT_DYNCREATE(CSpectra4View, CView)

BEGIN_MESSAGE_MAP(CSpectra4View, CView)
	//{{AFX_MSG_MAP(CSpectra4View)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_COMP_NEW,		OnCompNew)
	ON_COMMAND(ID_COMP_DEL,		OnCompDel)
	ON_COMMAND(ID_COMP_ALIGN,	OnCompAlign)
	ON_COMMAND(ID_COMP_CLIP,	OnCompClip)
	ON_COMMAND(ID_SPEC_APPLY,	OnApply)
	ON_COMMAND(ID_SPEC_AVER,	OnAverage)
	ON_COMMAND(ID_EDIT_COPY,	OnCopy)
	ON_COMMAND(ID_EDIT_PASTE,	OnPaste)
	ON_COMMAND(ID_EDIT_CUT,		OnCut)
	ON_COMMAND(ID_FILE_SAVE,	OnFileSave)
	ON_COMMAND(ID_TOOLS_SCALE,	OnToolsScale)
	ON_COMMAND(ID_TOOLS_SCAN,	OnToolsScan)
	ON_COMMAND(ID_TOOLS_SMOOTH,	OnToolsSmooth)
	ON_COMMAND(ID_TOOLS_LLHTEST,OnToolsLLHTest)
	ON_COMMAND(ID_TOOLS_SEEK,	OnToolsSeek)
	ON_COMMAND(ID_TOOLS_AREA,	OnToolsArea)
	ON_COMMAND(ID_FIT_STEP,		OnFitStep)
	ON_COMMAND(ID_FIT_AUTO,		OnFitAuto)
	ON_COMMAND(ID_FIT_SUMM,		OnFitSumm)
	ON_COMMAND(ID_FIT_SET,		OnFitSet)
	ON_COMMAND(ID_FIT_FIND,		OnFitFind)
	ON_COMMAND(ID_FIT_MONTEC,	OnFitMontec)
	ON_COMMAND(ID_FIT_REM_DUPL,	OnFitDuplicate)
	ON_COMMAND(ID_SEL_TOGGLE,	OnIgnoreSelToggle)
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_AUTOFIT, OnAutoFit)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSpectra4View construction/destruction

CSpectra4View::CSpectra4View()
{
	// TODO: add construction code here
	 //pSpecGridDlg = new CSpectra4Dlg(this);
	pSpecGrid =NULL;
	Spectrum=NULL;
	//Parameters initializing
	MouseX1=0; MouseX2=0; MouseY1=0; MouseY2=0;
	ExtremSmoothPoint=3; ScanMode = SCAN_MIN;
	TagComp=1;	SelFitComp=0;
	
	
	crFitState.StepMax=0.18;	crFitState.StepWid=0.14;
	crFitState.StepSplit=0.4;	crFitState.StepMix=0.2;
	crFitState.NewAccur=0;		crFitState.OldAccur=0;
	crFitState.NewCicleAccur=0;	crFitState.OldCicleAccur=2;
	crFitState.StepRel=0;	
	crFitState.NStepMax=100;	crFitState.NCicleMax=25;	crFitState.StepPerCicle=7;
	crFitState.crStep=0;		crFitState.crCicle=0;		crFitState.totStep=0;
	crFitState.BegComp=2;		
	crFitState.AccurL=0.001;		crFitState.AccurH=0.25;		crFitState.StepProgress=1;
	crFitState.AccurHrej=0.31;
	TraceParam.Mode=RIDEC_I;	TraceParam.Mult=1;		TraceParam.Time=10;
	TraceParam.Max=20;			TraceParam.MaxL=18;		TraceParam.MaxH=22;
	TraceParam.Wid=3;			TraceParam.WidL=2;		TraceParam.WidH=9;
	TraceParam.Split=30;		TraceParam.SplitL=20;	TraceParam.SplitH=99;
	TraceParam.Mix=1;			TraceParam.MixL=0;		TraceParam.MixH=2;	
	TraceParam.Select=NONE;
}

CSpectra4View::~CSpectra4View()
{
	//delete	pSpecGridDlg;
}

BOOL CSpectra4View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}
// Activate and Update event handlers
void CSpectra4View::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	PeakFitFlag=0;
	GetClientRect(rect);
	pSpecGrid = pGridCtrl;
	pFitGrid = pFitGridCtrl;
	ParamFlag = 1;
	SetGridParam();
	ActiveView = this;
	UpdateScale();
	SetFitParam(&TraceParam);
	
}
//
void CSpectra4View::OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (bActivate ==TRUE)
	{
		if (pGridCtrl!=NULL)
		{
			pSpecGrid = pGridCtrl;
			pFitGrid = pFitGridCtrl;
			//GetGridParam();
			if (ActiveView != this)
			{
				SetGridParam();
				SetFitParam(&TraceParam);
			}
			ActiveView = this;
			UpdateScale();
		}
	}
}
//
void CSpectra4View::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pGridCtrl!=NULL)
		{
			pSpecGrid = pGridCtrl;
			pFitGrid = pFitGridCtrl;
			SetGridParam();
			ActiveView = this;
			UpdateScale();
		}
}
//
void CSpectra4View::UpdateScale(int GetScale)
{
	Spectrum = GetDocument()->Spectrum;
	if (GetScale)
	{
	pDialogBar->xBeg = Spectrum->X_Axis->Beg;	pDialogBar->xEnd = Spectrum->X_Axis->End;
	pDialogBar->yBeg = Spectrum->Y_Axis->Beg;	pDialogBar->yEnd = Spectrum->Y_Axis->End;
	pDialogBar->GetAxis();
	if (GetScale>1) OnDraw(GetDC());
	}
	else
	{
	Spectrum->X_Axis->Beg = pDialogBar->xBeg;	Spectrum->X_Axis->End = pDialogBar->xEnd;
	Spectrum->Y_Axis->Beg = pDialogBar->yBeg;	Spectrum->Y_Axis->End = pDialogBar->yEnd;
	if (!pDialogBar->m_xSwitch)	Spectrum->X_Axis->ReScaleFlag=NONE-1;
	if (!pDialogBar->m_ySwitch)	Spectrum->Y_Axis->ReScaleFlag=NONE-1;
	Spectrum->ResetAxis();
	OnDraw(GetDC());
	}


}
//
void CSpectra4View::SetTraceParam()
{
	int N=Spectrum->NSComp;
	float Delta;	
	if ((TraceParam.Select==NONE)&&(N))
	{
		Delta= *(Spectrum->SpecBase+Spectrum->NSPoint-1)-(*(Spectrum->SpecBase));
		TraceParam.MaxL=((*Spectrum->SpecBase)-Delta);
		TraceParam.MaxH=((*Spectrum->SpecBase)+Delta);
		TraceParam.SplitH=4*Delta;
		TraceParam.SplitL=0.00005*Delta;
		TraceParam.FixSplit=0;
		TraceParam.WidH=5*Delta;
		TraceParam.WidL=0.00001*Delta;
		TraceParam.TimeH=10*Delta;
		TraceParam.TimeL=0.0001*Delta;
		TraceParam.Select=NONE+1;
	}
}
//
int CSpectra4View::SelectFitParam()
{
	int j, jFocus, N;
	Spectrum = GetDocument()->Spectrum;
	N = Spectrum->NSComp;
	jFocus=pSpecGrid->GetFocusCell().row-1;
	if ((jFocus>=0)&&(jFocus<Spectrum->BegFitComp))
	{
		TagComp=jFocus;
		//SetFitParam(Spectrum->SpecParam+jFocus);
	}
	else
		for (j=0; j<Spectrum->BegFitComp;j++)
			if ((Spectrum->SpecParam+j)->Select!=NONE) TagComp=j;
	
	if (Spectrum->BegFitComp<N)
		{
			if(jFocus<Spectrum->BegFitComp)
			{
				if (SelFitComp>=Spectrum->BegFitComp) // at least one of FitComps is present and has been selected
					jFocus=SelFitComp;
				else
					jFocus=Spectrum->BegFitComp;
				for (j=Spectrum->BegFitComp; j<N;j++)
					if ((Spectrum->SpecParam+j)->Select!=NONE) jFocus=j;
			}
			SelFitComp=jFocus;
			pSpectraGridDlg->UpdateFitNum(SelFitComp-Spectrum->BegFitComp+1);
			//return jFocus;
			
		
		}
	else 
		if (Spectrum->NSComp<=2)	
		{
			SelFitComp=jFocus;
			//return jFocus;
		}
		else
			SelFitComp=0;
			//return 0;
	if (SelFitComp<0) SelFitComp=0;
	return SelFitComp;
}
//
void CSpectra4View::SetFitParam(TParam* pParam, int extMode)
{
	int j;
	if ((pSpectraGridDlg!=NULL)&&(pFitGrid!=NULL))
	{
		
		if (extMode>0)
			pSpectraGridDlg->UpdateMode(extMode);
		else
			pSpectraGridDlg->UpdateMode(pParam->Mode);
		if (pParam->Mode!=FILE)
		{
			
			if ((pParam->Mode>=BINOM)&&(pParam->Mode<=BINOM_QU))
			{
				crFitState.StepMax=0.005;
				crFitState.StepMix=0.005;
				crFitState.StepWid=0.005;
				crFitState.StepSplit=0.005;

			}
			j = MAX;
			SprintFloat(pParam->Max,szBuff);
			pFitGrid->SetItemText(j+1,1,szBuff);
			SprintFloat(crFitState.StepMax,szBuff);
			pFitGrid->SetItemText(j+1,2,szBuff);
			SprintFloat(pParam->MaxL,szBuff);
			pFitGrid->SetItemText(j+1,3,szBuff);
			SprintFloat(pParam->MaxH,szBuff);
			pFitGrid->SetItemText(j+1,4,szBuff);
			j = WID;
			SprintFloat(pParam->Wid,szBuff);
			pFitGrid->SetItemText(j+1,1,szBuff);
			SprintFloat(crFitState.StepWid,szBuff);
			pFitGrid->SetItemText(j+1,2,szBuff);
			SprintFloat(pParam->WidL,szBuff);
			pFitGrid->SetItemText(j+1,3,szBuff);
			SprintFloat(pParam->WidH,szBuff);
			pFitGrid->SetItemText(j+1,4,szBuff);
			j = SPLIT;
			SprintFloat(pParam->Split,szBuff);
			pFitGrid->SetItemText(j+1,1,szBuff);
			SprintFloat(crFitState.StepSplit,szBuff);
			pFitGrid->SetItemText(j+1,2,szBuff);
			SprintFloat(pParam->SplitL,szBuff);
			pFitGrid->SetItemText(j+1,3,szBuff);
			SprintFloat(pParam->SplitH,szBuff);
			pFitGrid->SetItemText(j+1,4,szBuff);
			j = MIX;
			SprintFloat(pParam->Mix,szBuff);
			pFitGrid->SetItemText(j+1,1,szBuff);
			SprintFloat(crFitState.StepMix,szBuff);
			pFitGrid->SetItemText(j+1,2,szBuff);
			SprintFloat(pParam->MixL,szBuff);
			pFitGrid->SetItemText(j+1,3,szBuff);
			SprintFloat(pParam->MixH,szBuff);
			pFitGrid->SetItemText(j+1,4,szBuff);
			j = TIME;
			SprintFloat(pParam->Time,szBuff);
			pFitGrid->SetItemText(j+1,1,szBuff);
			SprintFloat(pParam->TimeL,szBuff);
			pFitGrid->SetItemText(j+1,3,szBuff);
			SprintFloat(pParam->TimeH,szBuff);
			pFitGrid->SetItemText(j+1,4,szBuff);
			j = MULT;
			SprintFloat(pParam->Mult,szBuff);
			pFitGrid->SetItemText(j+1,1,szBuff);
		}
		for (j=MAX;j<=MULT;j++)
			pFitGrid->RedrawRow(j+1);
	}

}
//
void CSpectra4View::GetFitParam(TParam* pParam, int exMode)
{
	int j,gMode;
	if ((pSpectraGridDlg!=NULL)&&(pFitGrid!=NULL))
	{
		gMode=pSpectraGridDlg->GetMode();
		if ((exMode>0)&&(gMode==FILE))
			pParam->Mode = exMode;
		else
			pParam->Mode = gMode;
		if (pParam->Mode!=FILE)
		{
			j = MAX;
			pParam->Max=atof(pFitGrid->GetItemText(j+1,1));
			crFitState.StepMax=atof(pFitGrid->GetItemText(j+1,2));
			pParam->MaxL=atof(pFitGrid->GetItemText(j+1,3));
			pParam->MaxH=atof(pFitGrid->GetItemText(j+1,4));
			j = WID;
			pParam->Wid=atof(pFitGrid->GetItemText(j+1,1));
			crFitState.StepWid=atof(pFitGrid->GetItemText(j+1,2));
			pParam->WidL=atof(pFitGrid->GetItemText(j+1,3));
			pParam->WidH=atof(pFitGrid->GetItemText(j+1,4));
			j = SPLIT;
			pParam->Split=atof(pFitGrid->GetItemText(j+1,1));
			crFitState.StepSplit=atof(pFitGrid->GetItemText(j+1,2));
			pParam->SplitL=atof(pFitGrid->GetItemText(j+1,3));
			pParam->SplitH=atof(pFitGrid->GetItemText(j+1,4));
			j = MIX;
			pParam->Mix=atof(pFitGrid->GetItemText(j+1,1));
			crFitState.StepMix=atof(pFitGrid->GetItemText(j+1,2));
			pParam->MixL=atof(pFitGrid->GetItemText(j+1,3));
			pParam->MixH=atof(pFitGrid->GetItemText(j+1,4));
			j = TIME;
			pParam->Time=atof(pFitGrid->GetItemText(j+1,1));
			pParam->TimeL=atof(pFitGrid->GetItemText(j+1,3));
			pParam->TimeH=atof(pFitGrid->GetItemText(j+1,4));
			j = MULT;
			pParam->Mult=atof(pFitGrid->GetItemText(j+1,1));
		}		
		
	}

}
//
void CSpectra4View::UpdateSelection(short int* SelectList, int ReDraw)	
{
	int j, N, jSel; 
	Spectrum = GetDocument()->Spectrum;
	N = Spectrum->NSComp;
	for (j=0;j<N;j++)
		if (*(SelectList+j+1))
		{
			(Spectrum->SpecParam+j)->Select = NONE+1;
			jSel=j;
		}
		else (Spectrum->SpecParam+j)->Select = NONE;
	UpdateScan();
	SelFitComp=SelectFitParam();
	if (SelFitComp>1)	SetFitParam(Spectrum->SpecParam+SelFitComp,(Spectrum->SpecParam+jSel)->Mode);
	else
		if (!( ((Spectrum->SpecParam+SelFitComp)->Mode==FILE)||((Spectrum->SpecParam+SelFitComp)->Mode==SUMM) ))
			SetFitParam(Spectrum->SpecParam+SelFitComp);
		else
		{
			SetTraceParam();
			SetFitParam(&TraceParam,(Spectrum->SpecParam+SelFitComp)->Mode);
		}
	UpdateFitIndicator();
	if (ReDraw)OnDraw(GetDC());	
		
}
//
void CSpectra4View::UpdateFitIndicator()
{
	sprintf(szBuff, "Curve %i / %i ,    Cicle%i,  Step%i,      MSE: %3.4f%s",TagComp,NComp,crFitState.crCicle+1, crFitState.totStep,100*(Spectrum->Accurasy), "%");
	Indicator3=szBuff;
	pStatusBar->SetPaneText(2,Indicator3);
}
//
void CSpectra4View::UpdateFitResult(int jTagComp, int FitComp)
{
	int j,N,i,col, ExtraCol;
	Spectrum = GetDocument()->Spectrum;
	N=Spectrum->NSComp;
	if ((pSpecGrid !=NULL)&&N&&(Spectrum->BegFitComp<N))
	{
		ExtraCol = 3*(N-Spectrum->BegFitComp);
		col=pSpecGrid->GetColumnCount();
		if (col<10+ExtraCol)
		// adding new columns
		{
			pSpecGrid->SetColumnCount(10+ExtraCol);
			for (j = col; j <10+ExtraCol; j++)
				pSpecGrid->SetColumnWidth(j,42);
			j=col; 
			i=1;
			while (j <10+ExtraCol)
			{
				sprintf (szBuff, "Amp%i",(int)(1+(j-10)/3));
				pSpecGrid->SetItemText(0,j,szBuff);		j++;
				pSpecGrid->SetItemText(0,j,"t rise");	j++;
				pSpecGrid->SetItemText(0,j,"t dec");	j++;
			}
		}
		N=Spectrum->NSComp-Spectrum->BegFitComp;
		for (j=0;j<N;j++)
		{
			SprintFloat((Spectrum->SpecParam+j+Spectrum->BegFitComp)->Amp,szBuff);
			pSpecGrid->SetItemText(jTagComp+1,10+j*3,szBuff);
			SprintFloat((Spectrum->SpecParam+j+Spectrum->BegFitComp)->Wid,szBuff);
			pSpecGrid->SetItemText(jTagComp+1,10+j*3+1,szBuff);
			SprintFloat((Spectrum->SpecParam+j+Spectrum->BegFitComp)->Split,szBuff);
			pSpecGrid->SetItemText(jTagComp+1,10+j*3+2,szBuff);
			
			if (Spectrum->NFitResult)
			{
				(Spectrum->CompFitResult+jTagComp*N+j)->Amp = (Spectrum->SpecParam+j+Spectrum->BegFitComp)->Amp;
				(Spectrum->CompFitResult+jTagComp*N+j)->tBeg = (Spectrum->SpecParam+j+Spectrum->BegFitComp)->Max;
				(Spectrum->CompFitResult+jTagComp*N+j)->tRise = (Spectrum->SpecParam+j+Spectrum->BegFitComp)->Wid;
				(Spectrum->CompFitResult+jTagComp*N+j)->tDec = (Spectrum->SpecParam+j+Spectrum->BegFitComp)->Split;
				(Spectrum->CompFitResult+jTagComp*N+j)->TotAmp = (Spectrum->SpecParam)->Amp;
			}
		}
	pSpecGrid->RedrawRow(jTagComp+1);
	}
}
//
void CSpectra4View::UpdateScan(int ScanFlag)
{
	int j,N;
	float Mean, SD, xBeg, xEnd, xMid;
	Spectrum = GetDocument()->Spectrum;
	N=Spectrum->NSComp;
	if (pSpecGrid !=NULL)
		if (ScanFlag)
		{
			
			xBeg =Spectrum->BegSelect;	xEnd =Spectrum->EndSelect;
			
			/*
			xMid =0.5*(xEnd+xBeg);
			if ( fabs(xMid)>=1)
			{
				sprintf(szNum, "<>%f3.0", xMid);
				sprintf(szBuff, "SD%f3.0", xMid);
			}
			else
			{
				sprintf(szNum, "<>%f3.3", xMid);
				sprintf(szBuff, "SD%f3.3", xMid);
			}
			pSpecGrid->SetItemText(0,6,szNum);
			pSpecGrid->SetItemText(0,7,szBuff);
			pSpecGrid->RedrawRow(0);
			*/
			for (j=0;j<N;j++)
			{
				if ( (Spectrum->SpecParam+j)->Select==NONE) 
				{
				pSpecGrid->SetItemText(j+1,6,"");
				pSpecGrid->SetItemText(j+1,7,"");
				//pSpecGrid->UnSelectRow(j+1);
				}
				else
				{
				
					(Spectrum->SpecComp+j)->StatRep(&Mean, &SD,xBeg,xEnd);
					if (ScanMode==SCAN_SLP)
						Mean=(Spectrum->SpecComp+j)->Slope(xBeg,xEnd);
					if (ScanMode==SCAN_ARE)
						Mean=(Spectrum->SpecComp+j)->Area(xBeg,xEnd);
					Mean*=(Spectrum->SpecParam+j)->Amp;
					Mean+=(Spectrum->SpecParam+j)->Zero;
					SD*=(Spectrum->SpecParam+j)->Amp;
					SprintFloat(Mean,szBuff);
					pSpecGrid->SetItemText(j+1,6,szBuff);
					SprintFloat(SD,szBuff);
					pSpecGrid->SetItemText(j+1,7,szBuff);
					//pSpecGrid->SelectRow(j+1);
				
				}
			pSpecGrid->RedrawRow(j+1);
			}
		}

	
}
//
void CSpectra4View::GetGridParam(int GetKf)
{
	int j, N;
	float NewKf;
	Spectrum = GetDocument()->Spectrum;
	N = Spectrum->NSComp;
	if (pSpecGrid ==NULL)
	{
		//AfxMessageBox("NoGrid");
	}
	else
	{
	if (N&&GetKf)
	{
		for (j=0;j<N;j++)
		{
			(Spectrum->SpecParam+j)->Zero = atof(pSpecGrid->GetItemText(j+1,1));
			(Spectrum->SpecParam+j)->Amp = atof(pSpecGrid->GetItemText(j+1,2));
			NewKf=ValidFloat(atof(pSpecGrid->GetItemText(j+1,8)), 0.000001);
			if (NewKf < 0 )
			{
				*(Spectrum->SpecComp+j)*=-1;
				if ((Spectrum->SpecParam+j)->Mode!=SINUS)
					*(Spectrum->SpecComp+j)+=1;
				NewKf=fabs(NewKf);
				sprintf(szBuff,"%4.4f", NewKf);
				//SprintFloat(NewKf, szBuff);
				pSpecGrid->SetItemText(j+1,8,szBuff);
			}
			(Spectrum->SpecParam+j)->Amp/=(Spectrum->SpecParam+j)->Kf;
			(Spectrum->SpecParam+j)->Kf=NewKf;
			(Spectrum->SpecParam+j)->Amp*=(Spectrum->SpecParam+j)->Kf;
		}
	
	Spectrum->ResetAxis();
	}
	//OnDraw(GetDC());
	}
}
//
void CSpectra4View::SetGridParam(int j)
{
	int i,N;
	Spectrum = GetDocument()->Spectrum;
	if ((j>=0)&&(j<Spectrum->NSComp))
	{
		if ((Spectrum->BegFitComp<Spectrum->NSComp)&&(j>=Spectrum->BegFitComp))
			sprintf(szBuff, "Fit%i",j-Spectrum->BegFitComp+1);
		else
			sprintf(szBuff, "%i",j);
		pSpecGrid->SetItemText(j+1,0,szBuff);
		SprintFloat((Spectrum->SpecParam+j)->Zero,szBuff);
		pSpecGrid->SetItemText(j+1,1,szBuff);
		SprintFloat((Spectrum->SpecParam+j)->Amp,szBuff);
		pSpecGrid->SetItemText(j+1,2,szBuff);
		SprintFloat((Spectrum->SpecParam+j)->Time,szBuff,2);
		pSpecGrid->SetItemText(j+1,3,szBuff);
		SprintFloat((Spectrum->SpecParam+j)->ExtMin,szBuff);
		pSpecGrid->SetItemText(j+1,4,szBuff);
		SprintFloat((Spectrum->SpecParam+j)->ExtMax,szBuff);
		pSpecGrid->SetItemText(j+1,5,szBuff);
		//SprintFloat((Spectrum->SpecParam+j)->Kf,szBuff);
		sprintf(szBuff,"%4.4f",(Spectrum->SpecParam+j)->Kf);
		pSpecGrid->SetItemText(j+1,8,szBuff);
		//pSpecGrid->RedrawRow(j);
		if ( (Spectrum->SpecParam+j)->Select==NONE) 
			pSpecGrid->UnSelectRow(j+1);
		else
			pSpecGrid->SelectRow(j+1);
		if ((j>0)&&(j<Spectrum->NFitResult))
		{	
			N=Spectrum->NSComp-Spectrum->BegFitComp;
			for (i=0;i<N;i++)
			{
				SprintFloat((Spectrum->CompFitResult+j*N+i)->Amp,szBuff);
				pSpecGrid->SetItemText(j+1,10+i*3,szBuff);
				SprintFloat((Spectrum->CompFitResult+j*N+i)->tRise,szBuff);
				pSpecGrid->SetItemText(j+1,10+i*3+1,szBuff);
				SprintFloat((Spectrum->CompFitResult+j*N+i)->tDec,szBuff);
				pSpecGrid->SetItemText(j+1,10+i*3+2,szBuff);
				
			}
		SprintFloat((Spectrum->CompFitResult+j*N)->TotAmp,szBuff);
		pSpecGrid->SetItemText(j+1,9,szBuff);
		}
		pSpecGrid->RedrawRow(j+1);	
	}
}
//
void CSpectra4View::SetGridParam()
{
	int j, N; 
	CCellID cell;
	Spectrum = GetDocument()->Spectrum;
	N = Spectrum->NSComp;
	if (pSpecGrid ==NULL)
	{
		//AfxMessageBox("NoGrid");
		
	}
	else
	{
		pSpecGrid->m_pView = this;
		if (!N)		
		{
			pSpecGrid->SetRowCount(12);
			//pSpecGrid->SetColumnCount(1);
			
		}
		else
		if (ParamFlag)
		{
			pSpecGrid->SetRowCount(N+3);
			if (pSpecGrid->GetColumnCount()<10)
				pSpecGrid->SetColumnCount(10);
			//if (pSpecGrid->GetRowCount()<12)
				//pSpecGrid->SetRowCount(12);
			pSpecGrid->SetColumnWidth(0,46);
			for (int col = 1; col < 16; col++)
				pSpecGrid->SetColumnWidth(col,56);
			// setting colimn titles
			pSpecGrid->SetItemText(0,0,"N");
			pSpecGrid->SetItemText(0,1,"Bottom");
			pSpecGrid->SetItemText(0,2,"Scale");
			pSpecGrid->SetItemText(0,3,"Time");
			pSpecGrid->SetItemText(0,4,"ExMin");
			pSpecGrid->SetItemText(0,5,"ExMax");
			pSpecGrid->SetItemText(0,6,"<scan>");
			pSpecGrid->SetItemText(0,7,"SDsc");
			pSpecGrid->SetItemText(0,8,"Coefficient");
			pSpecGrid->SetItemText(0,9,"FitAmp");
			/*
			pSpecGrid->SetItemText(0,9,"Par");
			pSpecGrid->SetItemText(0,10,"Par2");
			pSpecGrid->SetItemText(0,11,"Par3");
			pSpecGrid->SetItemText(0,12,"Par4");
			*/
			pSpecGrid->SetItemText(pSpecGrid->GetRowCount()-2,0,"<>");
			pSpecGrid->SetItemText(pSpecGrid->GetRowCount()-1,0,"SD");
			for (j=1;j<pSpecGrid->GetColumnCount();j++)
			{
				pSpecGrid->SetItemText(N+2,j,"");
				pSpecGrid->SetItemText(N+1,j,"");
			}
			//pSpecGrid->SetSingleRowSelection();
			for (j=0;j<N;j++)
				SetGridParam(j);
			pSpecGrid->SetItemText(1,0,"Summ");
			SelFitComp=SelectFitParam();
			if (SelFitComp)	SetFitParam(Spectrum->SpecParam+SelFitComp);
			else
				if (!( ((Spectrum->SpecParam+SelFitComp)->Mode==FILE)||((Spectrum->SpecParam+SelFitComp)->Mode==SUMM) ))
					SetFitParam(Spectrum->SpecParam+SelFitComp);
				else
				{
					SetTraceParam();
					SetFitParam(&TraceParam,(Spectrum->SpecParam+SelFitComp)->Mode);
				}
			UpdateFitIndicator();
			
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CSpectra4View drawing

void CSpectra4View::OnDraw(CDC* pDC)
{
	char szText[100];
	LPRECT SpecRect;
	CSpectra4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	GetClientRect(rect);
	//rect.left = 200; rect.top = 200; rect.right = 400; rect.bottom = 400;
	SpecRect = LPRECT(rect);
	//pDC->SetMapMode(MM_ANISOTROPIC);
	//pDC->SetWindowExt(1000,800);
	//pDC->SetViewportExt(rect.right,rect.bottom);
	if (pDoc->Spectrum->NSComp>1)
		pDoc->Spectrum->ReDraw(SpecRect,pDC->m_hDC);
	//if (ClbDataOK) ClbSpectrum->ReDraw(SpecRect,pDC->m_hDC);

}

/////////////////////////////////////////////////////////////////////////////
// CSpectra4View diagnostics

#ifdef _DEBUG
void CSpectra4View::AssertValid() const
{
	CView::AssertValid();
}

void CSpectra4View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSpectra4Doc* CSpectra4View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSpectra4Doc)));
	return (CSpectra4Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpectra4View message handlers
// 
void CSpectra4View::OnCompNew()
{
	
	int j;
	float* DataX;
	float* DataY;
	if ((pSpectraGridDlg!=NULL)&&(pFitGrid!=NULL))
		if (pSpectraGridDlg->GetMode())
		{
			Spectrum = GetDocument()->Spectrum;
			if (Spectrum->NSComp)
			{
				
				Spectrum->CreateComp();
				
			}
			else
			{
				DataX=new float[CreatePoint];
				DataY=new float[CreatePoint];
				GetFitParam(Spectrum->SpecParam+1);
				for (j=0;j<CreatePoint;j++)
				{
					*(DataY+j)=0;
					*(DataX+j)=(Spectrum->SpecParam+1)->MaxL+j*((Spectrum->SpecParam+1)->MaxH-(Spectrum->SpecParam+1)->MaxL)/(CreatePoint-1);
				}
				Spectrum->NewComp(DataX,DataY,CreatePoint);
				
				//(Spectrum->SpecParam+1)->Amp=1;
				//(Spectrum->SpecParam+1)->Zero=0;
				(Spectrum->SpecParam+1)->Amp=(Spectrum->SpecParam+1)->WidH;
				(Spectrum->SpecParam+1)->Zero=(Spectrum->SpecParam+1)->WidL;
			}
			j=Spectrum->NSComp-1;
			GetFitParam(Spectrum->SpecParam+j);
			(Spectrum->SpecComp+j)->SetFilter(NULL);
			(Spectrum->SpecComp+j)->SetStimPoint(StimPoint);
			(Spectrum->SpecComp+j)->Reset(Spectrum->SpecParam+j);
			SetGridParam();
			OnDraw(GetDC());
		}
}
// 
void CSpectra4View::OnCompDel()
{
	
	int j,jSel;
	CCellID cell;
	Spectrum = GetDocument()->Spectrum;
	for (j=Spectrum->NSComp-1;j>=0;j--)
   		if ((Spectrum->SpecParam+j)->Select!=NONE)
      		Spectrum->RemoveComp(j);
	if (pSpecGrid!=NULL)
	{	
		SetGridParam();
		for (j=1;j<pSpecGrid->GetColumnCount();j++)
		{
			jSel=pSpecGrid->GetRowCount();
			pSpecGrid->SetItemText(jSel-2,j,"");
			pSpecGrid->SetItemText(jSel-1,j,"");
		}
		j=(pSpecGrid->GetFocusCell()).row-1;
		pSpecGrid->SetFocusCell(-1,-1);
		jSel=Spectrum->SelComp(j);
		
		
		//pSpecGrid->SetSelectedRange(jSel, 0, jSel+2, pSpecGrid->GetColumnCount());
		//cell.col = 0;
		//cell.row = jSel+1;	
		//pSpecGrid->SelectRows(cell);
		//pSpecGrid->RedrawRow(jSel+1);
	}
	OnDraw(GetDC());
	
}
// 
void CSpectra4View::OnAverage()
{
	
	Spectrum = GetDocument()->Spectrum;
	Spectrum->Average(0);
	Spectrum->SelComp(0);
	OnDraw(GetDC());
	SetGridParam();
}
// 
void CSpectra4View::OnCompAlign()
{
	
	int j,N,jBeg,jEnd;
	float Shift;
	Spectrum = GetDocument()->Spectrum;
	N=Spectrum->NSComp;
	if (Spectrum->IsSelected) { jBeg=Spectrum->BegSelPoint; jEnd=Spectrum->EndSelPoint; }
		else {jBeg = BaseBeg; jEnd=BaseEnd;}
	if (pSpecGrid!=NULL)
		for (j=0;j<N;j++)
			if (((Spectrum->SpecParam+j)->Select != NONE)||IgnoreSelComp)
				{
				Shift=(Spectrum->SpecParam+j)->Zero+(Spectrum->SpecParam+j)->Amp*((Spectrum->SpecComp+j)->GetBaseLevel(jBeg,jEnd));
				//Shift-=(Spectrum->SpecParam+1)->Zero+(Spectrum->SpecParam+1)->Amp*((Spectrum->SpecComp+1)->GetBaseLevel(jBeg,jEnd));
				(Spectrum->SpecParam+j)->Zero-=Shift;
				SprintFloat((Spectrum->SpecParam+j)->Zero,szBuff);
				pSpecGrid->SetItemText(j+1,3,szBuff);
				SprintFloat((Spectrum->SpecParam+j)->Amp,szBuff);
				pSpecGrid->SetItemText(j+1,4,szBuff);
				pSpecGrid->RedrawRow(j+1);
				}
		Spectrum->Y_Axis->ReScaleFlag=NONE+1;
		Spectrum->ResetAxis();
		UpdateScale(1);
		//SetGridParam();
		OnDraw(GetDC());
		IgnoreSelComp=0;
		
}
//
void CSpectra4View::OnCompClip()
{
	int j,N;
	int jBeg=0; int jEnd=-1;
	Spectrum = GetDocument()->Spectrum;
	N=Spectrum->NSComp;
	if (Spectrum->IsSelected) { jBeg=Spectrum->BegSelPoint; jEnd=Spectrum->EndSelPoint; }
	for (j=0;j<N;j++)
	{
		if (((Spectrum->SpecParam+j)->Select != NONE)||IgnoreSelComp)
		{
			*(Spectrum->SpecComp+j) *=(Spectrum->SpecParam+j)->Amp;
			*(Spectrum->SpecComp+j) +=(Spectrum->SpecParam+j)->Zero;
			(Spectrum->SpecComp+j)->Clip(jBeg,jEnd);
			(Spectrum->SpecParam+j)->Zero=(Spectrum->SpecComp+j)->MinVal();
			(Spectrum->SpecParam+j)->Amp=(Spectrum->SpecComp+j)->MaxVal()-(Spectrum->SpecParam+j)->Zero;
			*(Spectrum->SpecComp+j) -=(Spectrum->SpecParam+j)->Zero;
			*(Spectrum->SpecComp+j) /=(Spectrum->SpecParam+j)->Amp;
		}

	}
	Spectrum->ResetAxis();
	SetGridParam();
	OnDraw(GetDC());
	IgnoreSelComp=0;

}

//
void CSpectra4View::OnCopy()
{
	Spectrum = GetDocument()->Spectrum;
	if (pSpecGrid!=NULL)
	{
	pSpecGrid->OnEditCopy();
	
	//GetGridParam();
	}
	if (ClbSpectrum == NULL) ClbSpectrum = new TSpectrum;
	if (ClbSpectrum!=NULL)
	{
		delete ClbSpectrum;
		ClbSpectrum = new TSpectrum;
		//if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		Spectrum->CopySelComp(ClbSpectrum,1,IgnoreSelComp);
		ClbDataOK=1;
	}
}
//
void CSpectra4View::OnCut()
{
	int NData,j;
	float* DataX;
	float* DataY;
	Spectrum = GetDocument()->Spectrum;
	int N = Spectrum->NSComp;
	DataX = new float[Spectrum->NSPoint];
	DataY = new float[Spectrum->NSPoint];
	if (ClbSpectrum!=NULL)
		delete ClbSpectrum;
	ClbSpectrum = new TSpectrum;
	ClbDataOK=0;
	for (j=0; j<N; j++) if (((Spectrum->SpecParam+j)->Select != NONE)||(IgnoreSelComp&&j))
		if (NData=Spectrum->CopySelPoint(DataX,DataY,j))
			{
				ClbDataOK=1;
				ClbSpectrum->NewComp(DataX, DataY, NData,1);
				(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->Time = (Spectrum->SpecParam+j)->Time;
				(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->ExtMin = (Spectrum->SpecParam+j)->ExtMin;
				(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->ExtMax = (Spectrum->SpecParam+j)->ExtMax;
				//(ClbSpectrum->SpecParam+ClbSpectrum->NSComp-1)->Paste(Spectrum->SpecParam+j);

			}
	IgnoreSelComp=0;
	delete[] DataX;
	delete[] DataY;
}
//
void CSpectra4View::OnPaste()
{
	
	if (pSpecGrid!=NULL)
	{
	pSpecGrid->OnEditPaste();
	//GetGridParam();
	}
	if (ClbDataOK)
	{
	
		Spectrum->PasteSelComp(ClbSpectrum);
		//if (ClbSpectrum->NSComp) ClbSpectrum->ClearAllComp(0);
		delete ClbSpectrum;
		ClbSpectrum=NULL;
		ClbDataOK=0;
		SetGridParam();
		OnDraw(GetDC());
	}
}
//
void CSpectra4View::OnFileSave()
{
	char DataName[512];
	char* CrChar;
	int j, j_ext;
	char szTemp[] = "Multiple column ASCII  (*.*) \0*.dt*\0Multiple column ASCII  (*.dt*) \0*.dt\0Spectra binary files  (*.cfn) \0*.cfn\0Ledar binary files (*.rdt)\0*.rdt\0\0";
	int nName, nData;
	Spectrum = GetDocument()->Spectrum;
	CFileDialog fd(false,NULL,NULL,OFN_OVERWRITEPROMPT |OFN_EXTENSIONDIFFERENT |OFN_CREATEPROMPT,szTemp);
	fd.m_ofn.lpstrFilter = (LPSTR)szTemp;	// See previous note concerning string
	fd.m_ofn.lpstrFile = (LPSTR)szName;	// Stores the result in this variable
	fd.m_ofn.nMaxFile = sizeof( szName );
	fd.m_ofn.lpstrFileTitle = NULL;
	fd.m_ofn.nMaxFileTitle = 0;
	fd.m_ofn.lpstrTitle = "Save data";	// Title for dialog
	fd.m_ofn.lpstrDefExt = "\0";
	if ((fd.DoModal()==IDOK)&&(strlen(szName)>1))
	{
		SetFileNameExt();	
		j_ext=0;	j=0;
		CrChar=szName;
		while ((*CrChar!='.')&&(*CrChar!='\0')&&(j_ext<510) )
      	{
			*(DataName+j_ext)=*CrChar;
			j_ext++; CrChar++;
		}
		*(DataName+j_ext)='\0';
		nName=fd.m_ofn.nFilterIndex;
		switch (nName)
		{
			case 1:Spectrum->SaveComp(szName);break; 
			case 2:strcat(DataName,".dt");Spectrum->SaveComp(DataName);break;
			case 3:strcat(DataName,".cfn");Spectrum->SaveConfig(DataName);break;
			case 4: AfxMessageBox("Ledar");strcat(DataName,".rdt");break;
			
		}
	//AfxMessageBox(DataName);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
//   Mouse reactions
////////////////////////////////////////////////////////////////////////////////////
void CSpectra4View::GetMouseCoord(CPoint cP)
{
	Spectrum = GetDocument()->Spectrum;
	if(Spectrum->NSComp)
	{
		GetClientRect(rect);
		rc=rect;
		rc.left+=(int)(MarginL*(rc.right-rc.left));
		rc.right-=(int)(MarginR*(rc.right-rc.left));
		rc.bottom-=(int)(MarginB*(rc.bottom-rc.top));
		rc.top+=(int)(MarginT*(rc.bottom-rc.top));
		MouseX2=Spectrum->X_Axis->Beg+(cP.x-rc.left)*(Spectrum->X_Axis->End-Spectrum->X_Axis->Beg)/(rc.right-rc.left);
		MouseY2=Spectrum->Y_Axis->End-(cP.y-rc.top)*(Spectrum->Y_Axis->End-Spectrum->Y_Axis->Beg)/(rc.bottom-rc.top);
	}
}
//
void CSpectra4View::OnMouseMove(UINT, CPoint cP)
{
	char szX[20];
	char szY[20];
	GetMouseCoord(cP);
	SprintFloat(MouseX2,szX);
	SprintFloat(MouseY2,szY);
	sprintf(szBuff, "(X,Y): %s, %s",szX, szY);
	Indicator1=szBuff;
	pStatusBar->SetPaneText(0,Indicator1);
	
}
//
void CSpectra4View::OnRButtonDown(UINT, CPoint cP)
{
	char szX[20];
	char szY[20];
	GetMouseCoord(cP);
	SprintFloat(MouseX2,szX);
	SprintFloat(MouseY2,szY);
	sprintf(szBuff, "%s, %s",szX, szY);
	Indicator2=szBuff;
	pStatusBar->SetPaneText(1,Indicator2);
	SetGridParam();
	
}
//
void CSpectra4View::OnLButtonDown(UINT, CPoint cP)
{
	int j,N;
	float Mean, SD, xBeg, xEnd, xMid;
	GetMouseCoord(cP);
	Spectrum = GetDocument()->Spectrum;
	if(Spectrum->NSComp)
	{
		Spectrum->SelChange(MouseX1,MouseX2,1);
		MouseX1 = MouseX2; MouseY1 = MouseY2;
		UpdateScan();
	}
	//SetGridParam();
	
	
	
	OnDraw(GetDC());
}
//
void CSpectra4View::OnToolsLLHTest()
{
	int StatModelFlag;
	float LL;
	Spectrum = GetDocument()->Spectrum;
	switch((Spectrum->SpecParam+1)->Mode)
	{
	case BINOM:case UNIMODAL:case BIMODAL:case BINOM_NZ:case GAUSLOR:StatModelFlag=1; break; 
		case BINOM_EF:case BINOM_QU:case BINOM_SUB:case COMPOUND:StatModelFlag=1; break; 
		default: StatModelFlag=0;
	}
	if ((Spectrum->NSComp==2)&&(StatModelFlag))
	{
	   //*(Spectrum->SpecComp)*=(Spectrum->SpecParam)->Amp;
	   //*(Spectrum->SpecComp)+=(Spectrum->SpecParam)->Zero;
		*(Spectrum->SpecComp+1)*=(Spectrum->SpecParam+1)->Amp;
		*(Spectrum->SpecComp+1)+=(Spectrum->SpecParam+1)->Zero;
		LL=(Spectrum->SpecComp+1)->LikeHoodTest(Spectrum->SpecParam+1);
		sprintf(szBuff,"LLH %f",LL);
		AfxMessageBox(szBuff,MB_OK);
		*(Spectrum->SpecComp)-=(Spectrum->SpecParam)->Zero;
		*(Spectrum->SpecComp)/=(Spectrum->SpecParam)->Amp;
		*(Spectrum->SpecComp+1)-=(Spectrum->SpecParam+1)->Zero;
		*(Spectrum->SpecComp+1)/=(Spectrum->SpecParam+1)->Amp;
	}
	else
		AfxMessageBox("Wrong parameters or sweep specified");
}
//
void CSpectra4View::OnToolsSeek()
{
	
	TFitResult* MinResult = new TFitResult;
	TFitResult* MaxResult = new TFitResult;
	Spectrum = GetDocument()->Spectrum;
	if (Spectrum->GetFitResultMargin(MinResult,MaxResult))
	{
		CFitSeekDlg* fDlg = new CFitSeekDlg();
		fDlg->m_Amp1=MinResult->Amp;
		fDlg->m_Amp2=MaxResult->Amp;
		fDlg->m_tRise1=MinResult->tRise;
		fDlg->m_tRise2=MaxResult->tRise;
		fDlg->m_tDec1=MinResult->tDec;
		fDlg->m_tDec2=MaxResult->tDec;
		if(fDlg->DoModal()==1)
		Spectrum->SeekFitResult(fDlg->m_Amp1,fDlg->m_Amp2,fDlg->m_tRise1,fDlg->m_tRise2,fDlg->m_tDec1,fDlg->m_tDec2,1);
	}
	else
	{
		CRandDlg* Dlg = new CRandDlg();
		Dlg->m_AmpLow	=	SeekAmpL;
		Dlg->m_AmpHigh	=	SeekAmpH;
		if(Dlg->DoModal())
		{
			SeekAmpL	=	Dlg->m_AmpLow;
			SeekAmpH	=	Dlg->m_AmpHigh;
			Spectrum->SeekComp(SeekAmpL,SeekAmpH,-1);
		}
	}
	SetGridParam();
	OnDraw(GetDC());
	delete MaxResult;
	delete MinResult;
	
	
}
//
void CSpectra4View::OnToolsArea()
{
	Spectrum = GetDocument()->Spectrum;
	sprintf(szBuff, "Area: %f",Spectrum->Area()); 
	AfxMessageBox(szBuff);

}
//
void CSpectra4View::OnToolsScale()
{
	CSpecScaleDlg* Dlg = new CSpecScaleDlg();
	Dlg->pView = this;
	if (Dlg->DoModal())
	{
		if (Dlg->Norm>0)
		{
			if (Spectrum->Normalized)
				Spectrum->DeNormalize();	
			else	
				Spectrum->Normalize(10);
		}
		if (!Dlg->Norm)
			Spectrum->ReScale(Dlg->m_BegComp,Dlg->m_EndComp,Dlg->Factor);
		SetGridParam();
		OnDraw(GetDC());
	}
	delete Dlg;
}
//
void CSpectra4View::OnToolsScan()
{
	float Mean, SD;
	int jBeg, jEnd, N,j;
	CSpecScanDlg* Dlg = new CSpecScanDlg();
	Dlg->pView = this;
	if(Dlg->DoModal()&&(pSpecGrid!=NULL))
	{
		Spectrum = GetDocument()->Spectrum;
		N=Spectrum->NSComp;
		jBeg = Spectrum->BegSelPoint;
		jEnd = Spectrum->EndSelPoint;
		if (ScanMode ==SCAN_SLP)
		pSpecGrid->SetItemText(0,8,"Slope");
		if (ScanMode ==SCAN_ARE)
			pSpecGrid->SetItemText(0,8,"Area");
		if (ScanMode ==SCAN_AVE)
		{
			pSpecGrid->SetItemText(0,8,"Mean");
			pSpecGrid->SetItemText(0,9,"StDev");
		}
		pSpecGrid->RedrawRow(0);
		for (j=0;j<N;j++)
			if (((Spectrum->SpecParam+j)->Select !=NONE)||(ScanMode==SCAN_SLP))
			{
				switch (ScanMode)
				{
				case SCAN_MIN:
					Mean = (Spectrum->SpecComp+j)->MinVal(jBeg,jEnd,ExtremSmoothPoint);
					Mean*=(Spectrum->SpecParam+j)->Amp;
					Mean+=(Spectrum->SpecParam+j)->Zero;
					SprintFloat(Mean,szBuff);
					pSpecGrid->SetItemText(j+1,4,szBuff);
					break;
				case SCAN_MAX:
					Mean = (Spectrum->SpecComp+j)->MaxVal(jBeg,jEnd,ExtremSmoothPoint);
					Mean*=(Spectrum->SpecParam+j)->Amp;
					Mean+=(Spectrum->SpecParam+j)->Zero;
					SprintFloat(Mean,szBuff);
					pSpecGrid->SetItemText(j+1,5,szBuff);
					break;
				case SCAN_SLP:
					Mean = (Spectrum->SpecComp+j)->Slope(jBeg,jEnd);
					Mean*=(Spectrum->SpecParam+j)->Amp;
					SprintFloat(Mean,szBuff);
					pSpecGrid->SetItemText(j+1,8,szBuff);
				break;
				case SCAN_AVE:
					(Spectrum->SpecComp+j)->StatRep(&Mean, &SD,Spectrum->BegSelect,Spectrum->EndSelect);
					Mean*=(Spectrum->SpecParam+j)->Amp;
					Mean+=(Spectrum->SpecParam+j)->Zero;
					SD*=(Spectrum->SpecParam+j)->Amp;
					SprintFloat(Mean,szBuff);
					pSpecGrid->SetItemText(j+1,8,szBuff);
					SprintFloat(SD,szBuff);
					pSpecGrid->SetItemText(j+1,9,szBuff);
				break;
				case SCAN_ARE:
					*(Spectrum->SpecComp+j)*=(Spectrum->SpecParam+j)->Amp;
					*(Spectrum->SpecComp+j)+=(Spectrum->SpecParam+j)->Zero;
					Mean = (Spectrum->SpecComp+j)->Area(jBeg, jEnd);
					SprintFloat(Mean,szBuff);
					pSpecGrid->SetItemText(j+1,8,szBuff);
					*(Spectrum->SpecComp+j)-=(Spectrum->SpecParam+j)->Zero;
					*(Spectrum->SpecComp+j)/=(Spectrum->SpecParam+j)->Amp;
				break;
				}
			pSpecGrid->RedrawRow(j+1);
			}
	}
	delete Dlg;	
}
//
void CSpectra4View::ApplyGridParam(int GetKf)
{
	ParamFlag = 0;
	if ((pSpectraGridDlg!=NULL)&&(pFitGrid!=NULL))
	{
		Spectrum=GetDocument()->Spectrum;
		if (Spectrum->NSComp)
		{
			SelFitComp = SelectFitParam();
			
			if (SelFitComp>1)
			{
				
				GetFitParam(Spectrum->SpecParam+SelFitComp,(Spectrum->SpecParam+SelFitComp)->Mode);
				(Spectrum->SpecComp+SelFitComp)->Reset(Spectrum->SpecParam+SelFitComp);
				SetFitParam(Spectrum->SpecParam+SelFitComp);
				
			}
			else
			{
				//sprintf(szBuff, "%i",SelFitComp); 
				//AfxMessageBox(szBuff);
				GetFitParam(Spectrum->SpecParam+SelFitComp);
				//(Spectrum->SpecComp)->Reset(Spectrum->SpecParam);
				//*(Spectrum->SpecComp+1)=(Spectrum->SpecComp);
				if (((Spectrum->SpecParam+SelFitComp)->Mode!=FILE)&&((Spectrum->SpecParam+SelFitComp)->Mode!=SUMM))
					Spectrum->BegFitComp=1;
				SetFitParam(Spectrum->SpecParam+SelFitComp);
			}
			ParamFlag = 1;
		}
		
		GetGridParam(GetKf);
		SetGridParam();	
	}
}
//
void CSpectra4View::OnApply()
{
	ApplyGridParam();
	OnDraw(GetDC());
}

//
void CSpectra4View::OnFitFind()
{
	int j;
	Spectrum = GetDocument()->Spectrum;
	Spectrum->Phourier(&Filter);
	SetGridParam();
	OnDraw(GetDC());
}
//
void CSpectra4View::OnToolsSmooth()
{
	int j;
	Spectrum = GetDocument()->Spectrum;
	j=Spectrum->FindSelComp();
	Spectrum->Smooth(j,1-FilterDepth);
	SetGridParam();
	OnDraw(GetDC());
}
//
void CSpectra4View::OnFitStep()
{
	int j;
	Spectrum = GetDocument()->Spectrum;
	if (Spectrum->NSComp)
	{
		ApplyGridParam(0);
		Spectrum->TagComp=TagComp;
		if (Spectrum->NSComp>2)
		{
      		crFitState.BegComp=Spectrum->BegFitComp;
			Spectrum->TagComp=TagComp;
		}
		else
      	// when LLH method is applicable
		{
      		crFitState.BegComp=1;
			Spectrum->TagComp=1;
			
		}
		Spectrum->Accurasy=Spectrum->LeastSqr();
		SprintFloat(Spectrum->SpecParam->Amp,szBuff);
		pSpecGrid->SetItemText(TagComp+1,9,szBuff);
		UpdateFitResult(TagComp);
		for (j=Spectrum->BegFitComp;j<Spectrum->NSComp;j++)
		{
			(Spectrum->SpecParam+j)->Select=NONE+1;
			SetGridParam(j);
		}
		SelFitComp=SelectFitParam();
		SetFitParam(Spectrum->SpecParam+SelFitComp);
		(Spectrum->SpecParam+TagComp)->Select=NONE+1;
		(Spectrum->SpecParam)->Select=NONE+1;
		UpdateFitIndicator();
		OnDraw(GetDC());
	}
	
}
//
void CSpectra4View::OnFitMontec()
{
	//Spectrum = GetDocument()->Spectrum;
	ApplyGridParam(1);
	if ((Spectrum->NSComp==2)&&((Spectrum->SpecParam+1)->Mode!=FILE)&&((Spectrum->SpecParam+1)->Mode!=SUMM))
      	{
         (Spectrum->SpecComp+1)->Randomize(Spectrum->SpecParam+1);
		 //*(Spectrum->SpecComp+1)=(Spectrum->SpecComp);
         //(Spectrum->SpecParam+1)->Zero=(Spectrum->SpecParam)->Zero;
         //(Spectrum->SpecParam+1)->Amp=(Spectrum->SpecParam)->Amp;
         *(Spectrum->SpecComp)=(Spectrum->SpecComp+1);
         (Spectrum->SpecParam)->Zero=(Spectrum->SpecParam+1)->Zero;
         (Spectrum->SpecParam)->Amp=(Spectrum->SpecParam+1)->Amp;
		 Spectrum->ResetAxis(0);
         }
	
	SetGridParam(0);
	SetGridParam(1);
	OnDraw(GetDC());
}
//
void CSpectra4View::OnFitSumm()
{
	//Spectrum = GetDocument()->Spectrum;
	ApplyGridParam(1);
	Spectrum->AllSumm(0);
	SetGridParam(0);
	OnDraw(GetDC());
}
//
void CSpectra4View::OnIgnoreSelToggle()
{
	IgnoreSelComp=1;	
}
//
void CSpectra4View::OnFitSet()
{
	Spectrum = GetDocument()->Spectrum;
	CFitSetDlg* Dlg = new CFitSetDlg();
	Dlg->pView=this;
	Dlg->m_CicleMax	=	NCicleMax;
	Dlg->m_StepMax	=	NStepMax;
	Dlg->m_AccurH	=	100*AccurHrej;
	Dlg->m_AccurL	=	100*AccurH;
	Dlg->m_HighX	=	Filter.FreqH;
	Dlg->m_LowX		=	Filter.CutFreq;
	Dlg->m_TresX	=	Filter.CutAmp;		
	if(Dlg->DoModal())
	{
		NCicleMax=	Dlg->m_CicleMax;
		NStepMax	=	Dlg->m_StepMax;
		AccurH	=	0.01*Dlg->m_AccurL;
		AccurHrej	=	0.01*Dlg->m_AccurH;
		
		crFitState.NCicleMax=	Dlg->m_CicleMax;
		crFitState.NStepMax	=	Dlg->m_StepMax;
		crFitState.AccurHrej	=	0.01*Dlg->m_AccurH;
		crFitState.AccurH	=	0.01*Dlg->m_AccurL;
		Filter.CutAmp		=	Dlg->m_TresX;
		Filter.CutFreq		=	Dlg->m_LowX;
		Filter.FreqH		=	Dlg->m_HighX;
	}
	delete Dlg;
}
// routine for removing of duplicate spontaneous peaks
void CSpectra4View::OnFitDuplicate()
{
	int* CompNum = new int[NCompMax];
	int j, j1,j2,N;
	N=Spectrum->NSComp;
	for (j=0;j<N;j++) *(CompNum+j)=NONE;
	j=1;j2=1;j1=0;
	while((j1<N-1)&&(j1<=j2))
	{
		// finding duplicates by time and amplitude
		while ((j<N-1)&&((Spectrum->SpecParam+j)->Select==NONE)) j++;
		j1=j;
		while ((j<N-1)&&((Spectrum->SpecParam+j)->Select!=NONE)) j++;
		j2=j-1;
		if (j2>j1)
			for(j=j1+1;j<=j2;j++)
			{
				if((Spectrum->SpecParam+j)->Time-(Spectrum->SpecParam+j-1)->Time<5.0*Filter.FreqH)
					*(CompNum+j)=NONE+1;
				if(fabs((Spectrum->SpecParam+j)->Amp-(Spectrum->SpecParam+j-1)->Amp)<0.03*(Spectrum->SpecParam+j)->Amp)
					*(CompNum+j)=NONE+1;
			}
		j=j2+1;
	}
	for (j=1;j<N;j++)
		(Spectrum->SpecParam+j)->Select= *(CompNum+j);
	OnCompDel();
	delete[] CompNum;

}
//
void CSpectra4View::OnFitAuto()
{
	int j;
	Spectrum = GetDocument()->Spectrum;
	if (Spectrum->NSComp)
	{
		ApplyGridParam(0);
		int FitStop=1;
		if (Spectrum!=NULL)
		{
			crFitState.NewCicleAccur=crFitState.OldCicleAccur+2*crFitState.AccurL;
			if (Spectrum->NSComp>2)
			{
      			crFitState.BegComp=Spectrum->BegFitComp;
				Spectrum->TagComp=TagComp;
			}
			else
      		// when LLH method is applicable
			{
      			crFitState.BegComp=1;
				Spectrum->TagComp=1;
			}
			crFitState.crStep=0;
			crFitState.crCicle=0;
			crFitState.totStep=0;
			crFitState.StepProgress=1;
			// remember steps
			StepWid=crFitState.StepWid;
			StepMax=crFitState.StepMax;
			StepSplit=crFitState.StepSplit;
			StepMix=crFitState.StepMix;
			TagComp=Spectrum->TagComp;
			NComp=Spectrum->NSComp;
			while (FitStop)
      		{
				FitStop=Spectrum->FitStep(&crFitState);
				UpdateFitIndicator();
			}
			// restore steps
			crFitState.StepWid=StepWid;
			crFitState.StepMax=StepMax;
			crFitState.StepSplit=StepSplit;
			crFitState.StepMix=StepMix;
		    SprintFloat(Spectrum->SpecParam->Amp,szBuff);
			pSpecGrid->SetItemText(TagComp+1,9,szBuff);
			UpdateFitResult(TagComp);
			for (j=Spectrum->BegFitComp;j<Spectrum->NSComp;j++)
			{
				(Spectrum->SpecParam+j)->Select=NONE+1;
				SetGridParam(j);
			}
			SelFitComp=SelectFitParam();
			SetFitParam(Spectrum->SpecParam+SelFitComp);
			(Spectrum->SpecParam+TagComp)->Select=NONE+1;
			(Spectrum->SpecParam)->Select=NONE+1;
			OnDraw(GetDC());
		}
	}
}
//
int CSpectra4View::PeakTrace(TSpectrum* TargetSpec, int BegNum, int EndNum)
{
	int j,i,k,PeakMode,jBeg,jEnd,TotPeak,NPeak,PeakPoint;
	float xBeg,xEnd,dW,Zero,Amp,BcNoise,xShift;
	float LocX[220];
	float* CrX;
	float* CrY;
	float* DataY;
	TotPeak=0;
	dW=1.4*Filter.FreqH;
	Spectrum = GetDocument()->Spectrum;
	if (TargetSpec->CreateMode==RIDEC_O) PeakMode=1;
		else	
		{
			if (TargetSpec->CreateMode==RIDEC_I)PeakMode=-1;
			else PeakMode=0;
		}
	CrX=TargetSpec->SpecBase;
	xShift=*CrX;
	for(j=0;j<TargetSpec->NSPoint;j++) *(CrX+j)-=xShift;
	CrY=TargetSpec->SpecComp->GetData();
	PeakPoint=TargetSpec->SpecComp->FindPoint(4.2*dW)-TargetSpec->SpecComp->FindPoint(0)-1;
	Spectrum->NewComp(CrX, CrY, PeakPoint,1);
	(Spectrum->SpecParam+1)->Paste(TargetSpec->SpecParam);
	(Spectrum->SpecParam)->Paste(TargetSpec->SpecParam);
	(Spectrum->SpecParam+Spectrum->NSComp-1)->Paste(TargetSpec->SpecParam);
	Spectrum->CreateComp();
	(Spectrum->SpecParam+Spectrum->NSComp-1)->Mode=TargetSpec->CreateMode;
	TotPeak=0;
	DataY=new float[NDATA_MAX];
	for (j=BegNum;j<=EndNum;j++)
	{
		// seeking for local peaks
		
		NPeak=(TargetSpec->SpecComp+j)->ExtremSearch(&Filter,LocX,200,PeakMode,&BcNoise,TargetSpec->BegSelPoint+3,TargetSpec->EndSelPoint-3);
		CrY=(TargetSpec->SpecComp+j)->GetData();
		Zero=(TargetSpec->SpecParam+j)->Zero;
		Amp=(TargetSpec->SpecParam+j)->Amp;
		BcNoise*=Amp;
		for (i=0;i<NPeak;i++)
		{
			jBeg=(TargetSpec->SpecComp+j)->FindPoint(LocX[i]-dW);
			if (jBeg<0) jBeg=0;
			if (jBeg+PeakPoint>TargetSpec->NSPoint) jBeg=TargetSpec->NSPoint-PeakPoint-1;
			xBeg = *(CrX+jBeg); // [msec]
			xEnd = *(CrX+jBeg+PeakPoint-1);
			for(k=0;k<PeakPoint;k++)
				*(DataY+k)= Zero+(*(CrY+jBeg+k))*Amp;
			Spectrum->NewComp(CrX, DataY, PeakPoint,1); // copying data beginig from jBeg
			Spectrum->TagComp=Spectrum->BegFitComp-1;
			(Spectrum->SpecParam+Spectrum->TagComp)->Paste(TargetSpec->SpecParam+j,0);
			//(Spectrum->SpecParam+Spectrum->TagComp)->Time = (TargetSpec->SpecParam+j)->Time+0.001*xBeg; //[sec]
			(Spectrum->SpecParam+Spectrum->TagComp)->Time = (TargetSpec->SpecParam+j)->Time+0.001*LocX[i]; //[sec]
			//(Spectrum->SpecParam+Spectrum->BegFitComp)->Max=LocX[i]-xBeg-1.2*Filter.CutFreq;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->Max=LocX[i]-xBeg;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->MaxL=0;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->MaxH=xEnd;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->Wid=0.3*Filter.CutFreq;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->WidL=0.3*Filter.CutFreq;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->WidH=1.3*Filter.CutFreq;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->Split=0.3*Filter.FreqH;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->SplitL=1.0*Filter.CutFreq;
			(Spectrum->SpecParam+Spectrum->BegFitComp)->SplitH=1.5*Filter.FreqH;
			(Spectrum->SpecParam+Spectrum->TagComp)->Max=(Spectrum->SpecParam+Spectrum->BegFitComp)->Max;
			(Spectrum->SpecParam+Spectrum->TagComp)->Wid=(Spectrum->SpecParam+Spectrum->BegFitComp)->Wid;
			(Spectrum->SpecParam+Spectrum->TagComp)->Split=(Spectrum->SpecParam+Spectrum->BegFitComp)->Split;
			(Spectrum->SpecComp+Spectrum->BegFitComp)->Reset(Spectrum->SpecParam+Spectrum->BegFitComp);
			// preliminary test of peak amplitude
			Spectrum->LeastSqr();	
			Spectrum->SelComp(0);
			Spectrum->SelComp(Spectrum->TagComp,0);
			if (Spectrum->SpecParam->Amp<0.4*Filter.CutAmp*BcNoise)
				Spectrum->RemoveComp(Spectrum->TagComp);
			else
			{
				OnDraw(GetDC());	
				/*if ((Spectrum->SpecParam+Spectrum->BegFitComp)->Amp<0.7*Filter.CutAmp*BcNoise)
				{
					sprintf(szBuff,"signal/noise= %f , Accept?",(Spectrum->SpecParam->Amp)/BcNoise); 
					if (AfxMessageBox(szBuff,MB_YESNO)==IDYES) 
						TotPeak++;
					else
						Spectrum->RemoveComp(Spectrum->TagComp);
				}
				else
			*/
			TotPeak++;
			}
		
		}
	}
	CrX=TargetSpec->SpecBase;
	for(j=0;j<TargetSpec->NSPoint;j++) *(CrX+j)+=xShift;
	Spectrum->RemoveComp(1);
	sprintf(szBuff, "Preliminary %i peaks",TotPeak);
	AfxMessageBox(szBuff);
	PeakFitFlag=1;
	SetGridParam();
	OnDraw(GetDC());
	delete[] DataY;
	return TotPeak;
}
//
int CSpectra4View::FitTrace(float* TraceX, float* TraceY, int NTracePar)
{
	int j, nFitComp, crComp, crTrace, N, NData,i, AcceptFit,FitAtBorder;
	float Mean,SDN;
	float AccurLim=crFitState.AccurHrej;
	//float AccurLim=crFitState.AccurH*1.5;
	//if(AccurLim<0.3)AccurLim=0.3;	if(AccurLim>0.38)AccurLim=0.38;
	Spectrum = GetDocument()->Spectrum;
	N=Spectrum->NSComp;
	NComp=N;
	crComp=0; crTrace=0;
	CFitResultDlg* Dlg = new CFitResultDlg();
	if ((N>2)&&(Spectrum->BegFitComp<N))
	{
		nFitComp=N-Spectrum->BegFitComp;
		ApplyGridParam(0);
		/*
		if ((Spectrum->TagComp>1)&&(Spectrum->TagComp<N))
		{
			sprintf(szBuff, "Do you want to continue fit from curve# %i?", Spectrum->TagComp+1);
			if (AfxMessageBox(szBuff,MB_YESNO)==IDYES) 
				Spectrum->TagComp++;
			else 
				Spectrum->TagComp=1;
		}
		else
		*/
		Spectrum->TagComp=1;
		if (Spectrum->NFitResult) delete[] Spectrum->CompFitResult;
		Spectrum->CompFitResult=new TFitResult[nFitComp*Spectrum->NSComp];
		Spectrum->NFitResult=Spectrum->BegFitComp;
		while ((crTrace>-1)&&(Spectrum->TagComp<Spectrum->BegFitComp))
		// fitting all the data curves
		{
			
			if (PeakFitFlag)
			{
				(Spectrum->SpecParam+Spectrum->BegFitComp)->Max=(Spectrum->SpecParam+Spectrum->TagComp)->Max;
				(Spectrum->SpecParam+Spectrum->BegFitComp)->Wid=(Spectrum->SpecParam+Spectrum->TagComp)->Wid;
				(Spectrum->SpecParam+Spectrum->BegFitComp)->Split=(Spectrum->SpecParam+Spectrum->TagComp)->Split;
			}
			int FitStop=1;
			crTrace++;
			Dlg->m_CommandFlag=0;
			(Spectrum->SpecComp+Spectrum->TagComp)->StatRep(&Mean, &SDN, 0, Spectrum->NSPoint);
			SDN*=(Spectrum->SpecParam+Spectrum->TagComp)->Amp;
			crFitState.NewCicleAccur=crFitState.OldCicleAccur+2*crFitState.AccurL;
			crFitState.BegComp=Spectrum->BegFitComp;
			TagComp=Spectrum->TagComp;
			crFitState.crStep=0;
			crFitState.crCicle=0;
			crFitState.totStep=0;
			crFitState.StepProgress=1;
			// remember steps
			StepWid=crFitState.StepWid;
			StepMax=crFitState.StepMax;
			StepSplit=crFitState.StepSplit;
			StepMix=crFitState.StepMix;
			FitAtBorder=0;
			while (!Dlg->m_CommandFlag)
			{
				while (FitStop)
      			{
					FitStop=Spectrum->FitStep(&crFitState);
					UpdateFitIndicator();
				}
				// restore steps
				crFitState.StepWid=StepWid;
				crFitState.StepMax=StepMax;
				crFitState.StepSplit=StepSplit;
				crFitState.StepMix=StepMix;
				SprintFloat(Spectrum->SpecParam->Amp,szBuff);
				pSpecGrid->SetItemText(Spectrum->TagComp+1,9,szBuff);
				//UpdateFitResult(Spectrum->TagComp);
				Spectrum->SelComp(0);
				Spectrum->SelComp(Spectrum->TagComp,0);
				for (j=Spectrum->BegFitComp;j<Spectrum->NSComp;j++)
				{
					(Spectrum->SpecParam+j)->Select=NONE+1;
					SetGridParam(j);
				}
				SelFitComp=SelectFitParam();
				SetFitParam(Spectrum->SpecParam+SelFitComp,(Spectrum->SpecParam+SelFitComp)->Mode);
				Spectrum->ResetAxis(0,0);
				OnDraw(GetDC());
				//(Spectrum->SpecParam+TagComp)->Select=NONE+1;
				//(Spectrum->SpecParam)->Select=NONE+1;
				FitAtBorder=((Spectrum->SpecParam+Spectrum->BegFitComp)->Wid >= (Spectrum->SpecParam+Spectrum->BegFitComp)->WidH);
				FitAtBorder=FitAtBorder||((Spectrum->SpecParam+Spectrum->BegFitComp)->Wid <= (Spectrum->SpecParam+Spectrum->BegFitComp)->WidL);
				FitAtBorder=FitAtBorder||((Spectrum->SpecParam+Spectrum->BegFitComp)->Split >= (Spectrum->SpecParam+Spectrum->BegFitComp)->SplitH);
				FitAtBorder=FitAtBorder||((Spectrum->SpecParam+Spectrum->BegFitComp)->Split <= (Spectrum->SpecParam+Spectrum->BegFitComp)->SplitL);
				if ( ((Spectrum->SpecParam)->Amp<0.3*SDN*Filter.CutAmp)||(crFitState.NewAccur>crFitState.AccurH)||FitAtBorder)
				{
					if ((crFitState.NewAccur>AccurLim)&&(PeakFitFlag))
					{
						//if (PeakFitFlag)
						//{
							Spectrum->RemoveComp(Spectrum->TagComp);
							Spectrum->TagComp--;

						//}
						//else
							//(Spectrum->CompFitResult+Spectrum->TagComp)->AcceptFlag=0;
						AcceptFit=0;
						Dlg->m_CommandFlag=2;
					}
					else
					{
						/*sprintf(szBuff,"Poor fit for curve %i. Accept?", Spectrum->TagComp);
						//if (AfxMessageBox(szBuff,MB_YESNO)==IDYES) 
							AcceptFit=1;
						else 
						{
							AcceptFit=0;
							(Spectrum->CompFitResult+Spectrum->TagComp)->AcceptFlag=0;
						}
						*/
						//i=0;
						//for (i=0;i<nFitComp;i++)
						i=0;
						Dlg->m_Max=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Max;
						Dlg->m_Amp=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Amp;
						Dlg->m_Trise=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Wid;
						Dlg->m_Tdec=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Split;
						if (nFitComp>1)
						{
							i=1;
							Dlg->m_Max2=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Max;
							Dlg->m_Amp2=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Amp;
							Dlg->m_Trise2=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Wid;
							Dlg->m_Tdec2=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Split;
						}
						sprintf(Dlg->m_SwNumStr,"%i Fit%i",crTrace,i+1); 
						sprintf(Dlg->m_PeakNumStr,"%i",Spectrum->TagComp);
						SprintFloat((Spectrum->SpecParam->Amp)/SDN,Dlg->m_SgnStr);
						SprintFloat(100*crFitState.NewAccur,Dlg->m_AccurStr);
						Dlg->DoModal();
					
						if(Dlg->m_CommandFlag==-1)
						{
							crTrace=-1;
							SetGridParam();
							return 0;
						}
						else
						{
							if (Dlg->m_CommandFlag)
							{
								if(Dlg->m_CommandFlag==1)
								{
									AcceptFit=1;
									i=0;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Max=Dlg->m_Max;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Amp=Dlg->m_Amp;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Wid=Dlg->m_Trise;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Split=Dlg->m_Tdec;
									if (nFitComp>1)
									{
										i=1;
										(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Max=Dlg->m_Max2;
										(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Amp=Dlg->m_Amp2;
										(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Wid=Dlg->m_Trise2;
										(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Split=Dlg->m_Tdec2;
									}

								}
								else 
								{
									AcceptFit=0;
									if (!i)
									{
										Spectrum->RemoveComp(Spectrum->TagComp);
										Spectrum->TagComp--;
									}
									/*
									if (PeakFitFlag)
									{
										Spectrum->RemoveComp(Spectrum->TagComp);
										Spectrum->TagComp--;

									}
									else
									{
										PeakFitFlag=1;
										(Spectrum->CompFitResult+Spectrum->TagComp)->AcceptFlag=0;
									}
									*/
								}
							}
							else
							{
								i=0;
								(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Max=Dlg->m_Max;
								(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Amp=Dlg->m_Amp;
								(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Wid=Dlg->m_Trise;
								(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Split=Dlg->m_Tdec;
								if (nFitComp>1)
								{
									i=1;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Max=Dlg->m_Max2;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Amp=Dlg->m_Amp2;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Wid=Dlg->m_Trise2;
									(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Split=Dlg->m_Tdec2;
								}
								crFitState.NewCicleAccur=crFitState.OldCicleAccur+2*crFitState.AccurL;
								crFitState.crStep=0;
								crFitState.crCicle=0;
								crFitState.totStep=0;
								crFitState.StepProgress=1;
								FitStop=1;
								AcceptFit=0;
								
							}
						}
						

					}
				}
				else
				{ AcceptFit=1; Dlg->m_CommandFlag=1;}
				if (AcceptFit)
				{
					(Spectrum->CompFitResult+Spectrum->TagComp)->AcceptFlag=1;	
					*(TraceX+crComp)=(Spectrum->SpecParam+Spectrum->TagComp)->Time;
					//*(TraceX+crComp)=Spectrum->TagComp;
					//SprintFloat(Spectrum->SpecParam->Amp,szBuff);
					//pSpecGrid->SetItemText(Spectrum->TagComp+1,9,szBuff);
					//AfxMessageBox(szBuff);
					UpdateFitResult(Spectrum->TagComp);
					for (i=0;i<nFitComp;i++)
						*(TraceY+i*NCompMax+crComp)=(Spectrum->SpecParam+Spectrum->BegFitComp+i)->Amp;
					crComp++;

				}
			}
			Spectrum->TagComp++;
			//OnDraw(GetDC());
			
			
		}
		// Creating report
		
	
	}
	delete Dlg;
	return crComp;
}
//
int	CSpectra4View::VarAn(float* TraceX, float* TraceY)
{
	int j,i,L,jBeg,jEnd, nFitFlag, crComp, N, NP,NData,FitStop;
	float Mean,SDN,DX,DY,Amp,Zero;
	float A01,Z01,A11,Z11,Peak,Peak0,Base0,Peak1,Base1;
	float* DataMean; float* DataFit; float* DataVar;
	float nsX1, nsX2, dtX1, dtX2,FitMix,tDec;
	float AccurLim=crFitState.AccurHrej;
	Spectrum = GetDocument()->Spectrum;
	N=Spectrum->NSComp;	NP=Spectrum->NSPoint;
	TComponent* AveComp= new TComponent(NP);
	if(Spectrum->BegFitComp<Spectrum->NSComp)
		nFitFlag=Spectrum->BegFitComp;
	else
		nFitFlag=0;
	// averaging only data comp, excluding fit comp
	Spectrum->SelComp(1,1);
	for(j=2;j<Spectrum->BegFitComp;j++)	Spectrum->SelComp(j,0);
	Spectrum->Average();
	*AveComp=Spectrum->SpecComp;
	Amp=Spectrum->SpecParam->Amp;
	Zero=Spectrum->SpecParam->Zero;
	// setting noise and data analysis regions
	nsX1= *(Spectrum->SpecBase);
	nsX2= *(Spectrum->SpecBase+50);
	FitMix=0.2;
	dtX2=*(Spectrum->SpecBase+NP-10);
	if (nFitFlag)
	{
		tDec=(Spectrum->SpecParam+nFitFlag)->Split;
		dtX1=(Spectrum->SpecParam+nFitFlag)->Max+3*(Spectrum->SpecParam+nFitFlag)->Wid;
	}
		else
		{
			tDec=20.0;
			dtX1=0.4*(*(Spectrum->SpecBase))+0.6*(*(Spectrum->SpecBase+NP-1));
		}
	if (Spectrum->IsSelected)
	{
		dtX1=Spectrum->BegSelect;
		dtX2=Spectrum->EndSelect;
	}
	CVarAnSetDlg* Dlg = new CVarAnSetDlg();
	Dlg->m_NsBeg	=	nsX1;	Dlg->m_NsEnd	=	nsX2;
	Dlg->m_AnBeg	=	dtX1;	Dlg->m_AnEnd	=	dtX2;
	Dlg->m_FitMix=FitMix;	Dlg->m_tDec=tDec;
	if(Dlg->DoModal()==1)
	{
		
		nsX1  =	Dlg->m_NsBeg;	nsX2	=	Dlg->m_NsEnd;
		dtX1  =	Dlg->m_AnBeg;	dtX2	=	Dlg->m_AnEnd;
		tDec = Dlg->m_tDec;		FitMix = Dlg->m_FitMix;
		jBeg=Spectrum->SpecComp->FindPoint(dtX1);
		jEnd=Spectrum->SpecComp->FindPoint(dtX2);
		// formatting the region for analysis
		L=(jEnd-jBeg)/100;	// length of single window
		NData=100;			// number of windows = data points for output
		if (L<4) 
		{
			L=4; NData = (jEnd-jBeg)/L;
			if (NData<25) NData = (NP-1-jBeg)/L;
		}
		//sprintf(szBuff, "N: %i/%i from %f  to %f",NData,L,*(Spectrum->SpecBase+jBeg),*(Spectrum->SpecBase+jBeg+L*NData-1));
		//AfxMessageBox(szBuff);
		
		// starting analysis cycle
		for (j=0;j<NData;j++) { *(TraceX+j)=0; *(TraceY+j)=0;}
		AveComp->StatRep(&Peak0,&SDN,jBeg-4,jBeg-1);
		AveComp->StatRep(&Base0,&SDN,nsX1,nsX2);
		if (nFitFlag) N=nFitFlag-1; // total number of data curves, count starts from 1
			else {	N=Spectrum->NSComp-1; FitMix=0;}
		DataMean=AveComp->GetData();
		if (nFitFlag) DataFit=(Spectrum->SpecComp)->GetData();
			else DataFit=AveComp->GetData();
		for (crComp=1;crComp<=N;crComp++)
		{
			// analysis of single data curve
			DataVar=(Spectrum->SpecComp+crComp)->GetData();
			(Spectrum->SpecComp+crComp)->StatRep(&Peak,&SDN,jBeg-4,jBeg-1);
			(Spectrum->SpecComp+crComp)->StatRep(&Mean,&SDN,nsX1,nsX2);
			SDN*=0.95*(Spectrum->SpecParam+crComp)->Amp;
			// normalize to peaks
			A01=((Spectrum->SpecParam+crComp)->Amp)*(Peak-Mean)/(Peak0-Base0);			// amplitude for ave comp
			Z01=((Spectrum->SpecParam+crComp)->Amp)*Mean+(Spectrum->SpecParam+crComp)->Zero-Base0*A01;						// zero level for ave comp
			A11=A01; Z11=Z01;															// for compatibility
			// fit data curve
			if (nFitFlag)
			{
				FitStop=1;
				crFitState.NewCicleAccur=crFitState.OldCicleAccur+2*crFitState.AccurL;
				crFitState.BegComp=Spectrum->BegFitComp;
				Spectrum->TagComp=crComp;
				TagComp=Spectrum->TagComp;
				crFitState.crStep=0;
				crFitState.crCicle=0;
				crFitState.totStep=0;
				crFitState.StepProgress=1;
				// remember steps
				StepWid=crFitState.StepWid;
				StepMax=crFitState.StepMax;
				StepSplit=crFitState.StepSplit;
				StepMix=crFitState.StepMix;
				while (FitStop)
      			{
					FitStop=Spectrum->FitStep(&crFitState);
					UpdateFitIndicator();
				}
				// restore steps
				crFitState.StepWid=StepWid;
				crFitState.StepMax=StepMax;
				crFitState.StepSplit=StepSplit;
				crFitState.StepMix=StepMix;
				//Normalize
				Base1= *(DataFit+1); Peak1 = *(DataFit+jBeg-1); 
				A11=((Spectrum->SpecParam+crComp)->Amp)*(Peak-Mean)/(Peak1-Base1);			
				Z11=((Spectrum->SpecParam+crComp)->Amp)*Mean+(Spectrum->SpecParam+crComp)->Zero-Base1*A11;	
			}
			
			// variance
			for (j=0;j<NData;j++)
			{
				for (i=0;i<L;i++)
				{
					DX= (1-FitMix)*((*(DataMean+j*L+i))*A01+Z01);
					//DX+= FitMix*((*(DataFit+j*L+i))*((Spectrum->SpecParam+nFitFlag)->Amp)+(Spectrum->SpecParam+nFitFlag)->Zero);
					DX+= FitMix*((*(DataFit+j*L+i))*A11+Z11);
					DY= (*(DataVar+j*L+i))*((Spectrum->SpecParam+crComp)->Amp)+(Spectrum->SpecParam+crComp)->Zero;
					DY-=DX;
					*(TraceY+j)+=(DY*DY-SDN*SDN)/(L*N);
					//*(TraceX+j)+=fabs( (DX-Mean)*(Spectrum->SpecParam+crComp)->Amp )/(L*N);
				}
				
			}
			if (nFitFlag)	{Spectrum->SpecParam->Zero=Z11; Spectrum->SpecParam->Amp=A11;} 
			else	{Spectrum->SpecParam->Zero=Z01; Spectrum->SpecParam->Amp=A01;}
			Spectrum->SelComp(0);
			Spectrum->SelComp(crComp,0);
			Spectrum->ResetAxis(0,0);
			OnDraw(GetDC());
			
		}
		for (j=0;j<NData;j++)
			for (i=0;i<L;i++)
				{
					DX= *(DataMean+j*L+i)-Base0;
					*(TraceX+j)+=fabs(DX*Amp)/L;
				}
		//sorting results - just in case
		do
		{
			FitStop=0;
			for(j=1;j<NData;j++)
				if ( *(TraceX+j)< *(TraceX+j-1) )
				{
					DX= *(TraceX+j); DY= *(TraceY+j);
					*(TraceX+j)= *(TraceX+j-1);	*(TraceX+j-1)=DX;
					*(TraceY+j)= *(TraceY+j-1);	*(TraceY+j-1)=DY;
					FitStop=1;
				}

		}
		while (FitStop==1);
	

	}
	delete AveComp;
	return NData;
}
//
void CSpectra4View::Rand(float AmpLow, float AmpHigh)
{
	float aL, aH;
	Spectrum = GetDocument()->Spectrum;
	aL=AmpLow/(Spectrum->SpecParam->Amp);	aH=AmpHigh/(Spectrum->SpecParam->Amp);
	//aL=AmpLow;	aH=AmpHigh;
	Spectrum->SpecComp->Randomize(aH, aL); 
	//Spectrum->SelComp(0);
	OnDraw(GetDC());
}
//
LRESULT CSpectra4View::OnAutoFit(WPARAM wParam, LPARAM lParam)
{
	//LRESULT mLResult =CWnd::WindowProc(WM_AUTOFIT,0, 0);
	int j;
	j=1;
	AfxMessageBox("Message Received");
	return FALSE;
	
}
