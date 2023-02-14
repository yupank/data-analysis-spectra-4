#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Fstream.h"
#include "math.h"
#include "speccomp.h"
#include "fftransf.h"


#define in 0x01
#define out 0x02
#define noreplace 0x40

char* SummStr="Summ";
char* GaLorStr=" Gauss-Lor.";
char* GaussDStr="Gauss'";
char* LorDStr="Lorentz'";
char* AbsStr="Doublet";
extern char szNum[64];
extern int NDataPointMax;

/*******************************************************************/
int  ReadData (const char* FileName, float far* DataX, float far* DataY, int NMax)                                 
	{
	const int MaxLen = 20000;
	char arStr[MaxLen];
	char Str1[64];
	char Str2[64];
	char crStr[256];
	char* cr_char;
	int N_Data,N_Str, NewOffset;
	int symb1,symb2,crL, ReadOk1, ReadOk2;
	ifstream inpFile;
	inpFile.open(FileName,in);
	//for (N_Data=0;N_Data<6;N_Data++)
	//		inpFile.getline(arStr,128,'\n');
	// reading data from file
	N_Data=0;
	N_Str=0;
	while( (!inpFile.eof())&&(N_Data<=NMax) )
		{
		inpFile.getline(arStr,MaxLen,'\n');
		// Cycle of string analysis
		symb1=0; ReadOk1=0;
		crL=0;
		cr_char=arStr;
		if (*cr_char!=';')
			{
			while( !( (((*cr_char==' ')||(*cr_char==',')||(*cr_char=='\t'))&&(symb1>=1))||(crL>MaxLen)) )
			//while( ( ((*cr_char==' ')&&(symb1>=1))||((*cr_char==',')&&(symb1>=1) )||(crL>128))==0 )
					{
					switch (*cr_char)
							{
							case '0':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '1':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '2':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '3':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '4':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '5':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '6':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '7':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '8':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '9':Str1[symb1]=*cr_char;symb1++;ReadOk1=1;break;
							case '+':Str1[symb1]=*cr_char;symb1++;break;
							case '-':Str1[symb1]=*cr_char;symb1++;break;
							case '.':Str1[symb1]=*cr_char;symb1++;break;
							case 'e':Str1[symb1]=*cr_char;symb1++;break;
							case 'E':Str1[symb1]=*cr_char;symb1++;break;
							default: if (!((*cr_char==' ')||(*cr_char==',')||(*cr_char=='\t')||(*cr_char=='\0')||(*cr_char=='\n'))) ReadOk1 = 0;
									
							}
					crL++;
					cr_char++;
					}
				//cr_char+=Offset;
				NewOffset=0;
				Str1[symb1]='\0';
				symb2=0;	ReadOk2=0;
				//while (crL<MaxLen)
				while( !((*cr_char=='\n')||(*cr_char=='\0')||(crL>MaxLen)))
					{
					switch (*cr_char)
						{
						case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
							Str2[symb2]=*cr_char;symb2++;ReadOk2=1;break;
						case '+':case '-':case '.':case 'e':case 'E':Str2[symb2]=*cr_char;symb2++;break;
						case '\0':Str2[symb2]=*cr_char;symb2++;break;
						case '\n': case ' ': if (symb2) ReadOk2 =1;
						default: ReadOk2 = 0;
						}
					crL++; NewOffset++;
					cr_char++;
					}
				Str2[symb2]='\0';
				if ( ReadOk1&&ReadOk2 )
					{
					if((!N_Str)&&(!N_Data))
						{
						*(DataX+N_Data)=0.0;
						*(DataY+N_Data)=0.0;	
						N_Data++;
						}
						
					*(DataX+N_Data)=(float)atof(Str1);
					*(DataY+N_Data)=(float)atof(Str2);
					N_Data++;
				}
			}
		N_Str++;
		}
		inpFile.close();
		// end of reading data
		return N_Data;
}

/*********************
void TMark::SetStr(char* DestStr)
	{
	sprintf(DestStr,"%f;%f;%f;%i;%s",X,Y,Len,Style,Text);
	}
/*********************
int TMark::GetStr(char* DestStr)
	{
	char* separator=";";
	char* ptrRead;
	char  strRead[5][16];
	int ReadFlag,ReadGood;
	int j,k,i;
	ptrRead=strtok(DestStr,separator);
	ReadFlag=0;
	do
				{
				i=0;
         	k=0;
				while ( (*(ptrRead+k)!='\0')&&(*(ptrRead+k)!='\n') )
					{
					switch (*(ptrRead+k))
            		{
						case '0':case '1':case '2':case '3':
						case '4':case '5':case '6':case '7':case '8':
						case '9':strRead[ReadFlag][i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '+':case '-':case '.':
						case 'E':strRead[ReadFlag][i]= (*(ptrRead+k));i++;break;
						default:ReadGood=0;
						}
					k++;
					}
				strRead[ReadFlag][i]='\0';
				if (ReadGood)
					ReadFlag++;
				ptrRead=strtok(NULL,separator);
				}
	while ((ReadFlag<4)&&(ptrRead!=NULL));
	if (ReadFlag==4)
   	{
		X=atof(strRead[0]);
		Y=atof(strRead[1]);
		Len=atof(strRead[2]);
		Style=atoi(strRead[3]);
		for(j=0;j<TextLen;j++)
			*(Text+j)='\0';
		j=0;
		while ( ((ptrRead+j)!=NULL)&&(j<TextLen)&&(*(ptrRead+j)!='\0')&&(*(ptrRead+j)!='\n'))
         {
			*(Text+j)= *(ptrRead+j);
			j++;
			}
		return 1;
		}
		else return 0;
	}
*/
/********************************************************************/
TFrame::TFrame(void)
{
	Beg=Lock=BegMark=0.0;
	End=1.0;
   Step=1.0;
	Style=NONE;
	NMark=5;
	Factor=1;
	ReScaleFlag=NONE+1;
}
/*********************/
void TFrame::Import(TFrame* DestFrame)
{
	Beg=DestFrame->Beg;
	End=DestFrame->End;
	Step=DestFrame->Step;
   BegMark=DestFrame->BegMark;
	Lock=DestFrame->Lock;
	Factor=DestFrame->Factor;
	NMark=DestFrame->NMark;
	Style=DestFrame->Style;
	ReScaleFlag=DestFrame->ReScaleFlag;
   LogScale=DestFrame->LogScale;
}
/*********************/
void TFrame::SetStep()
{
   if (ReScaleFlag!=NONE)
      {
		Step=(End-Beg)/NMark;
      BegMark=Beg;
      }
}
/*********************/
void TFrame::SetRoundScale(void)
{
	float Mantiss,EndMark;
	float Mult_10, Step_10;
	int j;
	if (ReScaleFlag!=NONE)
	{
		if (End>Beg)
		{
			if (NMark>7)NMark=7;
			Step=(End-Beg)/NMark;
			Mult_10=pow(10, floor(log10(fabs(Step))));
			Mantiss=floor(Step/Mult_10);
			Step=Mult_10*Mantiss;
			EndMark=Step*(floor(End/Step)+0.55);
			BegMark=Step*floor(Beg/Step);
			//BegMark=Step*floor(Beg/Step);
			//if(BegMark<Beg) BegMark+=Step;
			//NMark=0.5+(End-BegMark)/Step;
			if(ReScaleFlag!=NONE-1)
			{
				Beg=BegMark;	End=EndMark;
				NMark=(End-Beg)/Step;
			}
			else
			{
				NMark=(EndMark-BegMark)/Step;
			}
		}
		else
		{
      		End=Beg+1;
			Step=(End-Beg)/NMark;
		}
	}
}
/*********************
void TFrame::SetStr(char* DestStr)
	{
	sprintf(DestStr,"%f;%f;%f;%f;%i",Beg,End,Lock,NMark,Style);
	}
/*********************
int TFrame::GetStr(char* DestStr)
	{
	char* separator=";";
	char* ptrRead;
	char  strRead[5][16];
	int ReadFlag,ReadGood;
	int j,k,i;
	ptrRead=strtok(DestStr,separator);
   	ReadFlag=0;
	do
				{
				i=0;
         	k=0;
				while ( (*(ptrRead+k)!='\0')&&(*(ptrRead+k)!='\n') )
					{
					switch (*(ptrRead+k))
            		{
						case '0':case '1':case '2':case '3':
						case '4':case '5':case '6':case '7':case '8':
						case '9':strRead[ReadFlag][i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '+':case '-':case '.':
						case 'E':strRead[ReadFlag][i]= (*(ptrRead+k));i++;break;
						default:ReadGood=0;
						}
					k++;
					}
				strRead[ReadFlag][i]='\0';
				if (ReadGood)
					ReadFlag++;
				ptrRead=strtok(NULL,separator);
				}
	while ((ReadFlag<5)&&(ptrRead!=NULL));
	if (ReadFlag==5)
		{
		Beg=atof(strRead[0]);
		End=atof(strRead[1]);
		Lock=atof(strRead[2]);
		NMark=atof(strRead[3]);
		Style=atoi(strRead[4]);
		return 1;
		}
		else return 0;
	}
*/
/********************************************************************/
TSpectrum::TSpectrum(void)
{
	int j;
	NSMem=0;
	UniScaled=0;
	CreateMode=RIDEC_I; CreateNum=500;
	CreateScaleX=1;
	ShowSumm=1;
	NFitResult=0;
	BcgrComp = NULL;	BcgrParam = NULL;
	HCompMem=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)(sizeof(TComponent)*NCompMax));
	SpecComp=(PTComponent)GlobalLock(HCompMem);
	//SpecComp=new TComponent[NCompMax];
	HParamMem=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)(sizeof(TParam)*NCompMax));
	SpecParam=(PTParam)GlobalLock(HParamMem);
	//SpecParam=new TParam[NCompMax];
	HFitMem=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)(sizeof(float)*10*NDataPointMax));
	FitComp=(float far*)GlobalLock(HFitMem);
	//FitComp=new float[NDATA_MAX];
	
	
	HSMem[NSMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)(21*NDataPointMax*sizeof(float)));
	if ( HSMem[NSMem]!=NULL)
		DataBegin=(float far*)GlobalLock(HSMem[NSMem]);
	if (DataBegin!=NULL)
		{
		BytesRem=GlobalSize(HSMem[NSMem]);
		NSMem=1;
		}
	NSComp=0;
	NSPoint=0;
	Accurasy=1.0;
	IsSelected=0;	BegSelPoint=0;	EndSelPoint=0;
	Normalized=0;
	FitDomSize=0;
	StDev=0;   MeanSqrErr=0;	NoiseSqr=0;
	TagComp=1;	BegFitComp=2;
	ADCTime=-1; *(Param+3)=-100; *(Param+7)=-100; *(Param+10)=222; *(Param+11)=100;
	//Marker=(PTMark)malloc(2*sizeof(TMark));
	Title=new TMark[N_TITLE];
	Label=new TMark[N_LINE];//(PTMark)malloc(16*sizeof(TMark));
   /*
	*(Marker->Text)='\0';
	Marker->Style=NONE;
	Marker->X=0;
	Marker->Y=0;
	Marker->Len=0;
   */
	DotStyle=NONE;
	X_Axis=new TFrame;
	Y_Axis=new TFrame;
	X_Axis->NMark=5;
	Y_Axis->NMark=5;
	X_Axis->Style=NONE+1;
	Y_Axis->Style=NONE+1;
	X_Axis->LogScale=NONE;
	Y_Axis->LogScale=NONE;
	DeltaX=0;
	DeltaY=0;
	SpecBrush.lbStyle=BS_SOLID;
	SpecBrush.lbColor=RGB(255,255,255);
	for (j=0;j<N_LINE;j++)
      {
		*((Label+j)->Text)='\0';
		((Label+j))->Style=NONE;
		((Label+j))->X=0.5;
		((Label+j))->Y=0.1;
		((Label+j))->SizeX=0;
      ((Label+j))->SizeY=0;
      ((Label+j))->TextX=0;
      ((Label+j))->Visible=0;
		}
	for (j=0;j<N_TITLE;j++)
		{
      ((Title+j))->Visible=0;
		((Title+j))->Style=NONE;
		((Title+j))->X=0.5;
		((Title+j))->SizeX=0.05;
      ((Title+j))->SizeY=0.05;
      ((Title+j))->TextY=0.075;
      ((Title+j))->TextX=0.02;
		((Title+j))->Y=0.2+0.1*j;
		*((Title+j)->Text)='\0';
      SpecFont[j].lfHeight=0;
		}
	for (j=0;j<16;j++)
		{
		SpecPen[j].lopnStyle=PS_SOLID;
		SpecPen[j].lopnWidth.x=1;
		SpecPen[j].lopnWidth.y=0;
		}
	//SpecPen[1].lopnWidth.x=10;
	for (j=0;j<2;j++)
		{
		SpecPen[j*8].lopnColor=RGB(0,0,0);
		SpecPen[j*8+1].lopnColor=RGB(255,0,255);
		SpecPen[j*8+2].lopnColor=RGB(0,0,0);
		SpecPen[j*8+3].lopnColor=RGB(0,0,255);
		SpecPen[j*8+4].lopnColor=RGB(0,255,0);
		SpecPen[j*8+5].lopnColor=RGB(255,0,0);
		SpecPen[j*8+6].lopnColor=RGB(0,255,255);
		SpecPen[j*8+7].lopnColor=RGB(127,127,127);
		}
	SelNum=0;
	SelPageSize=10;
	for (j=0;j<SelPageSize;j++)
		(SpecParam+j)->Select=NONE+1;
	//AfxMessageBox("NewSpec");

}
/********************************************************************/
TSpectrum::~TSpectrum(void)
{
	int j;
	PTParam DelPar;
	PTComponent DelComp;
	if (NSMem!=0)
		for (j=0;j<NSMem;j++)
			{
			GlobalUnlock(HSMem[j]);
			GlobalFree(HSMem[j]);
			}
	
	
   	delete X_Axis;
	delete Y_Axis;
	//free(Marker);
	delete Title;
	delete Label;
	if (BcgrComp!=NULL) delete BcgrComp;
	if (BcgrParam!=NULL) delete BcgrParam;
	//delete SpecComp;	 delete SpecParam;	delete FitComp;
  	GlobalUnlock(HCompMem);
	GlobalFree(HCompMem);
	GlobalUnlock(HFitMem);
	GlobalFree(HFitMem);
	GlobalUnlock(HParamMem);
	GlobalFree(HParamMem);
	if (FitDomSize)
      		{
            delete FitDomLow;	delete FitDomHigh;	delete FitDomCenter;	delete FitDomAccur;
         	}
    if (NFitResult)    delete[] CompFitResult;
   

}
/********************************************************************/
int TSpectrum::Read(const char* FileName,int nBefore,int nAfter, int Format)
	{
	HGLOBAL HMemX;
	HGLOBAL HMemY;
	HGLOBAL HMemInp;
	int      IsFloat;
	char crStr[256];
	char  ar_char;
	int hFile;
	int far* Inp;
	int      Count;
	float far* DataX;
	float far* DataY;
	int N_Data;
	int SortEnd;
	float Buf1,Buf2;
	float InitX;
	float ScanX; 
	TComponent* AuxComp;
	ifstream inpFile;
	int j,DataBlockSize;
	DataBlockSize=5*NDataPointMax;
	
	HMemX=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,DataBlockSize);
	DataX=(float far*)GlobalLock(HMemX);
	HMemY=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,DataBlockSize);
	DataY=(float far*)GlobalLock(HMemY);
	N_Data=0;
	if ((DataX!=NULL)&&(DataY!=NULL))
	{
	//if  (Format==0)
		N_Data=ReadData(FileName, DataX, DataY,DataBlockSize);
		// compatibility with older versions & avoding memory fragmentation
		if (N_Data<NDataPointMax) DataBlockSize = NDataPointMax;
			else
			if (N_Data<5*NDataPointMax) DataBlockSize = N_Data;
				else DataBlockSize = 5*NDataPointMax;

	}
	if (N_Data >=3 )
		if ( NSComp==0 )
			{
			//DeltaX = *DataX;
			//DeltaY = *DataY;
			NSPoint=N_Data-2;
			// sorting the data
			do
				{
				SortEnd=1;
				for (j=1;j<NSPoint;j++)
					{
					if ( (*(DataX+j)) > (*(DataX+j+1)) )
						{
						SortEnd *=0;
						Buf1= (*(DataX+j));
						Buf2= (*(DataX+j+1));
						(*(DataX+j))=Buf2;
						(*(DataX+j+1))=Buf1;
						Buf1= (*(DataY+j));
						Buf2= (*(DataY+j+1));
						(*(DataY+j))=Buf2;
						(*(DataY+j+1))=Buf1;
						}
					else SortEnd *=1;
					}
				}
			while (!SortEnd);
			// end of do-wnile
			AuxComp=new TComponent((DataY+1),(DataX+1),NSPoint);
			//
			if ((nBefore>=4)&&(nAfter>=1))
			NSPoint=AuxComp->Smooth(nBefore,nAfter);
			delete AuxComp;
			// copieing the X-coordinates data
			SpecBase=DataBegin;
			DataBegin+=NSPoint+1;
			for (j=0;j<NSPoint;j++)
				*(SpecBase+j)= *(DataX+j+1);
			BegSelect= *(SpecBase+1);
			EndSelect= *(SpecBase+NSPoint-2);
			BytesRem-=(NSPoint+1)*sizeof(float);
			BcgrComp=new TComponent(DataBegin, SpecBase, NSPoint);
			BcgrComp->InitData(1);
			BytesRem-=(NSPoint+1)*sizeof(float);
			DataBegin+=NSPoint+1;
			BcgrParam=new TParam;
			BcgrParam->Kf=1;
			BcgrParam->Amp=1;
			BcgrParam->Zero=0;
			// copieing the Y-coordinates data
			if (NSPoint<N_Spline)
				N_Data=2*NSPoint+1;
			else
				N_Data=NSPoint+1;
			while ( NSComp<=1)
				{
				if ( BytesRem<=N_Data*sizeof(float) )
					{                            // allocation of new memory block
					HSMem[NSMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,DataBlockSize*4);
					if ( HSMem[NSMem]!=NULL)
						DataBegin=(float far*)GlobalLock(HSMem[NSMem]);
					if (DataBegin!=NULL)
						{
						BytesRem=GlobalSize(HSMem[NSMem]);
						NSMem++;
						}
					}
				for (j=0;j<NSPoint;j++)
					*(DataBegin+j)= *(DataY+j+1);
				BytesRem-=N_Data*sizeof(float);
				(SpecComp+NSComp)->Set(DataBegin, SpecBase, NSPoint);
				DataBegin+=N_Data;
				//((SpecParam+NSComp))=new TParam;
				NSComp++;
				
				}// of while
			strncpy(((SpecParam))->Name,SummStr,strlen(((SpecParam))->Name));
			strncpy(((SpecParam+1))->Name,FileName,strlen(((SpecParam+1))->Name));
			((SpecParam))->Mode=SUMM;
			((SpecParam+1))->Mode=FILE;
			((SpecParam+1))->Zero=((SpecComp+1))->MinVal();
			((SpecParam+1))->Amp=((SpecComp+1))->MaxVal()-((SpecParam+1))->Zero;
			((SpecParam))->Zero=((SpecParam+1))->Zero;
			((SpecParam))->Amp=((SpecParam+1))->Amp;
			((SpecParam))->Kf=1.0;
			((SpecParam))->NormCf=1.0;
			((SpecParam+1))->Kf=1.0;
			((SpecParam+1))->NormCf=1.0;
			/*
			X_Axis->Beg= *SpecBase;
			X_Axis->End= *(SpecBase+NSPoint-1);
			Y_Axis->Beg=((SpecParam+1))->Zero;
			Y_Axis->End=((SpecParam+1))->Zero+((SpecParam+1))->Amp;
			X_Axis->SetRoundScale();
			Y_Axis->SetRoundScale();
			Y_Axis->Lock=X_Axis->BegMark;
			X_Axis->Lock=Y_Axis->BegMark;
			X_Axis->ReScaleFlag=NONE;
			*/
			Y_Axis->ReScaleFlag=NONE+1;
			X_Axis->ReScaleFlag=NONE;
			ResetAxis();
			BegSelect= *(SpecBase);
			EndSelect= *(SpecBase+NSPoint-1);
			IsSelected=0;
			*(((SpecComp)))-=((SpecParam))->Zero;
			*(((SpecComp)))/=((SpecParam))->Amp;
			((SpecComp))->ReSpline();
			*(((SpecComp+1)))-=((SpecParam+1))->Zero;
			*(((SpecComp+1)))/=((SpecParam+1))->Amp;
			DeltaY/=((SpecParam+1))->Amp;
			DeltaX=0;
			((SpecComp+1))->ReSpline();
			//return N_Data;
			
			}
		else
			{
			//return 0;

			AuxComp=new TComponent((DataY+1),(DataX+1),N_Data-2);
			AuxComp->ReSpline();
			if (NSPoint<N_Spline)
				N_Data=2*NSPoint+1;
			else
				N_Data=NSPoint+1;

			if ( BytesRem<=N_Data*sizeof(float) )
				{
				// allocation of new memory block
				HSMem[NSMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,DataBlockSize*4);
				if ( HSMem[NSMem]!=NULL)

					DataBegin=(float far*)GlobalLock(HSMem[NSMem]);
				if (DataBegin!=NULL)
					{
					BytesRem=GlobalSize(HSMem[NSMem]);
					NSMem++;
					}
				}
			BytesRem-=N_Data*sizeof(float);
			// re-calculation and copieing the data

			if (Format)
         	for (j=0;j<NSPoint;j++)
					*(DataBegin+j)= *(DataY+j+1);
				else
					for (j=0;j<NSPoint;j++)
						*(DataBegin+j)=AuxComp->OutSpline(*(SpecBase+j));
			(SpecComp+NSComp)->Set(DataBegin, SpecBase, NSPoint);
			DataBegin+=N_Data;
			//((SpecParam+NSComp))=new TParam;
			((SpecParam+NSComp))->Mode=FILE;
			strncpy(((SpecParam+NSComp))->Name,FileName,strlen(((SpecParam+NSComp))->Name));
			((SpecParam+NSComp))->Kf=1.0;
         ((SpecParam+NSComp))->NormCf=1.0;
			((SpecParam+NSComp))->Max=2*(*(SpecBase+NSPoint-2));
			((SpecParam+NSComp))->Zero=((SpecComp+NSComp))->MinVal();
			((SpecParam+NSComp))->Amp=((SpecComp+NSComp))->MaxVal()-((SpecParam+NSComp))->Zero;
			*(((SpecComp+NSComp)))-=((SpecParam+NSComp))->Zero;
			*(((SpecComp+NSComp)))/=((SpecParam+NSComp))->Amp;
			if (UniScaled)
				{
				((SpecParam+NSComp))->Zero=(SpecParam+1)->Zero;
				((SpecParam+NSComp))->Amp=(SpecParam+1)->Amp;
				}
			/*
			if ( Y_Axis->Beg > ((SpecParam+NSComp))->Zero )
				Y_Axis->Beg=((SpecParam+NSComp))->Zero;
			if ( Y_Axis->End < (((SpecParam+NSComp))->Zero+((SpecParam+NSComp))->Amp) )
				Y_Axis->End=((SpecParam+NSComp))->Zero+((SpecParam+NSComp))->Amp;
			Y_Axis->ReScaleFlag=NONE+1;
			Y_Axis->SetRoundScale();
			Y_Axis->Lock=X_Axis->BegMark;
			X_Axis->Lock=Y_Axis->BegMark;
			*/
			Y_Axis->ReScaleFlag=NONE+1;
			X_Axis->ReScaleFlag=NONE;
			ResetAxis();
			((SpecComp+NSComp))->ReSpline();
			NSComp++;
			delete AuxComp;
			}

			// of else
	GlobalUnlock(HMemX);
	GlobalFree(HMemX);
	GlobalUnlock(HMemY);
	GlobalFree(HMemY);
	
	//free(DataY);
	//free(DataX);
	return N_Data;
	}

void TSpectrum::ReScale(int Beg, int End, float Factor)
{
	int jBeg, jEnd,j;
	if (Beg>=1 && Beg<NSComp-1) jBeg=Beg;
		else jBeg=1;
	if (End>=1 && End<NSComp-1) jEnd=End;
		else jEnd=0;
	if (fabs(Factor)>MinFloat)
		for(j=jBeg;j<=jEnd;j++)
		{
			(SpecParam+j)->Zero *= Factor;
			(SpecParam+j)->Amp *= Factor;
		}
}
/**************************************************************************/
void TSpectrum::ResetAxis(int ResetX, int IgnoreSel)
{
	// auto rescaling: NONE - no, NONE+1 - full, NONE-1 - just round scaling
	int j,Count;
	if (Y_Axis->ReScaleFlag==NONE+1)
	{
	Y_Axis->Beg=(SpecParam+1)->Zero;
	Y_Axis->End=(SpecParam+1)->Zero+(SpecParam+1)->Amp;
	Count=0;
	for (j=0;j<NSComp;j++)
		{
			if(((SpecParam+j)->Select!=NONE)||(IgnoreSel))
			{
				if ((!Count)||( (SpecParam+j)->Zero < Y_Axis->Beg )) Y_Axis->Beg=(SpecParam+j)->Zero;
				if ((!Count)||( (SpecParam+j)->Zero+(SpecParam+j)->Amp> Y_Axis->End )) Y_Axis->End=(SpecParam+j)->Zero+(SpecParam+j)->Amp;
				Count++;
			}
		}
	}
	Y_Axis->SetRoundScale();
	if (ResetX)
   	{
		if (X_Axis->ReScaleFlag!=NONE)
			X_Axis->Lock=Y_Axis->BegMark;
		if (X_Axis->ReScaleFlag!=NONE-1)
		{
			X_Axis->Beg= *SpecBase;
			X_Axis->End= *(SpecBase+NSPoint-1);
		}
		X_Axis->SetRoundScale();
      }
   if (Y_Axis->ReScaleFlag!=NONE-1)
   {
		Y_Axis->Lock=X_Axis->BegMark;
		X_Axis->Lock=Y_Axis->BegMark;
		
   }
   else
   {
		if(X_Axis->Beg<X_Axis->BegMark)
			Y_Axis->Lock=X_Axis->BegMark-0.5*X_Axis->Step;
		else
			Y_Axis->Lock=X_Axis->BegMark;
		if(Y_Axis->Beg>Y_Axis->BegMark)
			X_Axis->Lock=Y_Axis->BegMark+Y_Axis->Step;
		else
			X_Axis->Lock=Y_Axis->BegMark;
		
		
   }

}

/**************************************************************************/
void TSpectrum::ReadResults(const char* FileName)
{
	char* strBuf;
	float ReadX[10]; 
	char* ptrRead;
	char  strRead[80];
	char* separator="\r\n;:, ";
	int   ReadFlag=-100;
	int j,i,k,ReadGood;
	strBuf=(char*)malloc(1600);
	ifstream inpFile;
	inpFile.open(FileName,in);
	while (!inpFile.eof())
		{
		inpFile.getline(strBuf,128,'\n');
		*(strBuf+127)='\0';
		if ( (*(strBuf)=='1')||(*(strBuf)=='2')||(*(strBuf)=='3')||(*(strBuf)=='4')||(*(strBuf)=='5')||(*(strBuf)=='6')||(*(strBuf)=='7')||(*(strBuf)=='8')||(*(strBuf)=='9') )
		{
		ptrRead=strtok(strBuf,separator);
		i=0;
		ReadFlag=0;
		for(j=0;j<10;j++)
		ReadX[j]=0;
		do
				{
				i=0;
				k=0;
				while ( (*(ptrRead+k)!='\0')&&(*(ptrRead+k)!='\n'))
					{
					switch (*(ptrRead+k))
						{
						case '0':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '1':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '2':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '3':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '4':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '5':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '6':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '7':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '8':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '9':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '+':strRead[i]= (*(ptrRead+k));i++;break;
						case '-':strRead[i]= (*(ptrRead+k));i++;break;
						case '.':strRead[i]= (*(ptrRead+k));i++;break;
						case 'E':strRead[i]= (*(ptrRead+k));i++;break;
						default:ReadGood=0;
						}
					k++;
					}
				strRead[i]='\0';
				if (ReadGood)
					{
					ReadX[ReadFlag]=(float)atof(strRead);
					//MessageBox(HWindow,(LPSTR)strRead,"Check ",MB_OK);
					ReadFlag++;
					}
				ptrRead=strtok(NULL,separator);
				}
		while ( ptrRead!=NULL);
		if (ReadFlag>=5)
			{
			CreateComp();
			j=NSComp-1;
			((SpecParam+j))->Wid=1;
			((SpecParam+j))->Mix=0;
			((SpecParam+j))->Max=0;
			((SpecParam+j))->Split=0;
			if (ReadFlag==5)
				{
				((SpecParam+j))->Max=ReadX[1];
				((SpecParam+j))->Wid=ReadX[2];
				((SpecParam+j))->Mode=LORDIR;
				}
			if (ReadFlag==6)
				{
				((SpecParam+j))->Mix=ReadX[1];
				((SpecParam+j))->Max=ReadX[2];
				((SpecParam+j))->Wid=ReadX[3];
				((SpecParam+j))->Mode=GAUSLOR;
				}
			if (ReadFlag==7)
				{
				((SpecParam+j))->Mix=ReadX[1];
				((SpecParam+j))->Max=ReadX[2];
				((SpecParam+j))->Split=ReadX[3];
				((SpecParam+j))->Wid=ReadX[4];
				((SpecParam+j))->Mode=ABSORB;
				}
			//sprintf(crStr,"%i %f %f %f",ReadFlag,ReadX[0],ReadX[1],ReadX[4]);
			//MessageBox(HWindow,(LPSTR)crStr,"Check ",MB_OK);
			((SpecComp+j))->Reset(((SpecParam+j)));
			}
		}
		}
	free(strBuf);
	inpFile.close();
	
}
/*******************************************************************/
int TSpectrum::CreateComp(void)
{
	int N_Data;
	float Delta;
	if (NSPoint<N_Spline)
		N_Data=2*NSPoint+1;
	else
		N_Data=NSPoint+1;
	if ( BytesRem<=N_Data*sizeof(float) )
	{                            // allocation of new memory block
		HSMem[NSMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,sizeof(float)*(10*NDataPointMax+1));
		if ( HSMem[NSMem]!=NULL)
			DataBegin=(float far*)GlobalLock(HSMem[NSMem]);
		if (DataBegin!=NULL)
			{
			BytesRem=GlobalSize(HSMem[NSMem]);
			NSMem++;
			}
	}
	if ((DataBegin!=NULL)&&(NSComp<NCompMax))
		{
		if (BegFitComp>=NSComp) BegFitComp=NSComp;
		Delta=*(SpecBase+NSPoint-1)- *SpecBase;
		BytesRem-=N_Data*sizeof(float);
		(SpecComp+NSComp)->Set(DataBegin, SpecBase, NSPoint);
		DataBegin+=N_Data;
		//((SpecParam+NSComp))=new TParam;
		(SpecParam+NSComp)->Zero=((SpecParam+1))->Zero;
		(SpecParam+NSComp)->Amp=((SpecParam+1))->Amp;
		(SpecParam+NSComp)->Kf=1.0;
		(SpecParam+NSComp)->FixWid=0;
		(SpecParam+NSComp)->FixMax=0;
		(SpecParam+NSComp)->MaxL=(*SpecBase-Delta);
		(SpecParam+NSComp)->MaxH=(*SpecBase+Delta);
		(SpecParam+NSComp)->SplitH=4*Delta;
		(SpecParam+NSComp)->SplitL=0.00005*Delta;
		(SpecParam+NSComp)->FixSplit=0;
		(SpecParam+NSComp)->WidH=3*Delta;
		(SpecParam+NSComp)->WidL=0.00001*Delta;
		(SpecParam+NSComp)->TimeH=10*Delta;
		(SpecParam+NSComp)->TimeL=0.0001*Delta;
		(SpecParam+NSComp)->Select=1;
		(SpecParam+NSComp)->Mix=0;
		(SpecParam+NSComp)->Mult=1;
		NSComp++;
		}
		else N_Data=0;
	return (N_Data);
}
/*******************************************************************/
int TSpectrum::NewComp(float far* DataX, float far* DataY, int N_Data, int SimpleCopy)
{
	int j,jInsert,DataBlockSize;
	TComponent* AuxComp;
	
	if(NSComp<NCompMax)
	{
	if ( NSComp==0 )
		{
		// compatibility with older versions & avoding memory fragmentation
		NSPoint=N_Data;
		if (N_Data<NDataPointMax) 
			DataBlockSize = NDataPointMax*10+1;
		else
			if (N_Data<10*NDataPointMax) DataBlockSize = 2*N_Data+1;
			else 
				{
					DataBlockSize = 10*NDataPointMax;
					NSPoint=DataBlockSize-1;
				}

		// copieing the X-coordinates data
		SpecBase=DataBegin;
		DataBegin+=NSPoint+1;
		for (j=0;j<NSPoint;j++)
			*(SpecBase+j)= *(DataX+j);
		BytesRem-=(NSPoint+1)*sizeof(float);
		//BcgrComp=new TComponent(DataBegin, SpecBase, NSPoint);
		BcgrComp=new TComponent(SpecBase, NSPoint);
		BcgrComp->InitData(1);
		//BytesRem-=(NSPoint+1)*sizeof(float);
		//DataBegin+=NSPoint+1;
		//sprintf(szNum,"BR_1:%i",BytesRem);
		//AfxMessageBox(szNum);
		BcgrParam=new TParam;
		BcgrParam->Kf=1;
		BcgrParam->Amp=1;
		BcgrParam->Zero=0;
			// copieing the Y-coordinates data
		if (NSPoint<N_Spline)
			N_Data=2*NSPoint+1;
			else
				N_Data=NSPoint+1;
		while ( NSComp<=1)
			{
			if ( BytesRem<=N_Data*sizeof(float) )
				{                            
				// allocation of new memory block
				//AfxMessageBox("LowMem")
				HSMem[NSMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,DataBlockSize*sizeof(float));
				if ( HSMem[NSMem]!=NULL)
					DataBegin=(float far*)GlobalLock(HSMem[NSMem]);
				if (DataBegin!=NULL)
					{
					BytesRem=GlobalSize(HSMem[NSMem]);
					NSMem++;
					}
				}
			for (j=0;j<NSPoint;j++)
				*(DataBegin+j)= *(DataY+j);
			BytesRem-=N_Data*sizeof(float);
			(SpecComp+NSComp)->Set(DataBegin, SpecBase, NSPoint);
			DataBegin+=N_Data;
			//((SpecParam+NSComp))=new TParam;
			NSComp++;
			
			}
		strncpy(((SpecParam))->Name,SummStr,strlen(((SpecParam))->Name));
		(SpecParam)->Mode=SUMM;
		(SpecParam+1)->Mode=FILE;
		(SpecParam+1)->Zero=((SpecComp+1))->MinVal();
		(SpecParam+1)->Amp=((SpecComp+1))->MaxVal()-((SpecParam+1))->Zero;
		(SpecParam)->Zero=((SpecParam+1))->Zero;
		(SpecParam)->Amp=((SpecParam+1))->Amp;
		(SpecParam+1)->Kf=1.0;
		(SpecParam+1)->NormCf=1.0;
		(SpecParam)->Kf=1.0;
		BegSelect= *(SpecBase);
		EndSelect= *(SpecBase+NSPoint-1);
		X_Axis->Beg= *SpecBase;
		X_Axis->End= *(SpecBase+NSPoint-1);
		//X_Axis->SetStep();
		Y_Axis->Beg=((SpecParam+1))->Zero;
		Y_Axis->End=((SpecParam+1))->Zero+((SpecParam+1))->Amp;
		//Y_Axis->Lock=X_Axis->BegMark;
		//Y_Axis->SetStep();
		//X_Axis->Lock=Y_Axis->BegMark;
		ResetAxis();
		*(((SpecComp)))-=((SpecParam))->Zero;
		*(((SpecComp)))/=((SpecParam))->Amp;
		((SpecComp))->ReSpline();
		*(((SpecComp+1)))-=((SpecParam+1))->Zero;
		*(((SpecComp+1)))/=((SpecParam+1))->Amp;
		((SpecComp+1))->ReSpline();
		}
		else
		{
			AuxComp=new TComponent((DataY),(DataX),N_Data);
			//AuxComp->ReSpline();
			if (NSPoint<N_Spline)
				N_Data=2*NSPoint+1;
			else
				N_Data=NSPoint+1;
			if (N_Data<NDataPointMax) 
				DataBlockSize = DataBlockSize = NDataPointMax*10+1;
			else
				if (N_Data<10*NDataPointMax) DataBlockSize = 2*N_Data+1;
				else 
						DataBlockSize = 10*NDataPointMax;
			if ( BytesRem<=N_Data*sizeof(float) )
				{                            // allocation of new memory block
				HSMem[NSMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,DataBlockSize*sizeof(float));
				if ( HSMem[NSMem]!=NULL)
					DataBegin=(float far*)GlobalLock(HSMem[NSMem]);
				if (DataBegin!=NULL)
					{
					BytesRem=GlobalSize(HSMem[NSMem]);
					NSMem++;
					}
				}
			BytesRem-=N_Data*sizeof(float);
			// re-calculation and copieing the data
			if (SimpleCopy)
				for (j=0;j<NSPoint;j++)
					*(DataBegin+j)=(*(DataY+j));
			else
			{
				AuxComp->ReSpline();
				for (j=0;j<NSPoint;j++)
					*(DataBegin+j)=AuxComp->OutSpline(*(SpecBase+j));
			}
			if (BegFitComp<NSComp)
			// there are model components
			{
				//moving model componets' data and params to new location
				for (j=NSComp;j>BegFitComp;j--)
				{
					*(SpecParam+j)= *(SpecParam+j-1);
					(SpecComp+j)->Set((SpecComp+j-1)->GetData(), SpecBase, NSPoint);
				}
				// location to insert new  component
				jInsert=BegFitComp;
				BegFitComp++;
			}
			else
			// there are no model components
			{
				BegFitComp=NSComp+1;
				jInsert=NSComp;
			}
			(SpecComp+jInsert)->Set(DataBegin, SpecBase, NSPoint);
			DataBegin+=N_Data;
			//((SpecParam+NSComp))=new TParam;
			(SpecParam+jInsert)->Mode=FILE;
			(SpecParam+jInsert)->Kf=1.0;
			(SpecParam+jInsert)->NormCf=1.0;
			(SpecParam+jInsert)->FixWid=0;
			(SpecParam+jInsert)->FixMax=0;
			(SpecParam+jInsert)->Zero=((SpecComp+jInsert))->MinVal();
			(SpecParam+jInsert)->Amp=((SpecComp+jInsert))->MaxVal()-((SpecParam+jInsert))->Zero;
			(SpecParam+jInsert)->Select=NONE+1;
			*(((SpecComp+jInsert)))-=((SpecParam+jInsert))->Zero;
			*(((SpecComp+jInsert)))/=((SpecParam+jInsert))->Amp;
			if ( Y_Axis->Beg > ((SpecParam+jInsert))->Zero )
				Y_Axis->Beg=((SpecParam+jInsert))->Zero;
			if ( Y_Axis->End < (((SpecParam+jInsert))->Zero+((SpecParam+jInsert))->Amp) )
				Y_Axis->End=((SpecParam+jInsert))->Zero+((SpecParam+jInsert))->Amp;
			//Y_Axis->SetStep();
			//Y_Axis->Lock=X_Axis->BegMark;
			//X_Axis->Lock=Y_Axis->BegMark;
			ResetAxis();
			((SpecComp+jInsert))->ReSpline();
			NSComp++;
			delete AuxComp;
		}
	}
	return N_Data;
}

/*******************************************************************/
void TSpectrum::RemoveComp(int CurveN)
{
	int j,i,N;
	int N_Data;
	PTParam DelPar;
	PTComponent DelComp;
	if (NSPoint<N_Spline)
		N_Data=2*NSPoint+1;
	else
		N_Data=NSPoint+1;
	if (NSComp>0)
	{
		for (j=CurveN;j<NSComp-1;j++)
		{
			*(SpecComp+j) = *(SpecComp+j+1);
			//*(SpecParam+j) = *(SpecParam+j+1);
			(SpecParam+j+1)->Copy(SpecParam+j);
		}
		if(NFitResult)
		{
			N=NSComp-BegFitComp;
			for (j=CurveN;j<NSComp-1;j++)
			{
				for(i=0;i<N;i++)
				{
					(CompFitResult+j*N+i)->Amp=(CompFitResult+(j+1)*N+i)->Amp;
					(CompFitResult+j*N+i)->TotAmp=(CompFitResult+(j+1)*N+i)->TotAmp;
					(CompFitResult+j*N+i)->tRise=(CompFitResult+(j+1)*N+i)->tRise;
					(CompFitResult+j*N+i)->tDec=(CompFitResult+(j+1)*N+i)->tDec;
					(CompFitResult+j*N+i)->tBeg=(CompFitResult+(j+1)*N+i)->tBeg;
					(CompFitResult+j*N+i)->AcceptFlag=(CompFitResult+(j+1)*N+i)->AcceptFlag;
				}
			}
		}
		NSComp--;
		//DelPar=((SpecParam+NSComp));
		//delete DelPar;
		//DelComp=((SpecComp+NSComp));
		//delete DelComp;
		//DataBegin-=N_Data;
		//BytesRem+=N_Data*sizeof(float);
		//if ( BytesRem>64000) BytesRem=64000;
	}
	// finding the first model component
	if (NSComp>2)
	{
		j=2;
		while ((j<=NSComp-1)&&((SpecParam+j)->Mode==FILE))
			j++;
		if (j>=NSComp) BegFitComp=NSComp;
		else BegFitComp=j;	
	}
	else BegFitComp=2;
}
/**************************************************/
void TSpectrum::ClearAllComp(int LastNum)
{
	int j;
	while (NSComp>LastNum)
		RemoveComp(NSComp-1);
}

/*******************************************************************/
void TSpectrum:: SaveResults(const char* FileName)
{
	int j;
	char crStr[256];
	char* StrMix="mix:";
	char* StrMax="max:";
	char* StrWid="wid:";
	char* StrCoef="rate:";
   char* StrSplit="split:";
	ofstream outFile;
	outFile.open(FileName,out);
	for (j=2;j<NSComp;j++)
		{
		if (((SpecParam+j))->Mode==FILE)
			sprintf(crStr,"%i,%s,%s%f,%f",(j-1),((SpecParam+j))->Name,StrCoef,((SpecParam+j))->Kf,((SpecParam+j))->Amp);
		if (((SpecParam+j))->Mode==GAUSLOR)
			sprintf(crStr,"%i,%s,%s%1.3f,%s%f,%s%f,%s%1.3f,%f",(j-1),GaLorStr,StrMix,((SpecParam+j))->Mix,
			StrMax,((SpecParam+j))->Max,StrWid,((SpecParam+j))->Wid,StrCoef,((SpecParam+j))->Kf,((SpecParam+j))->Amp);
		if (((SpecParam+j))->Mode==ABSORB)
			sprintf(crStr,"%i,%s,%s%1.3f,%s%f,%s%f,%s%f,%s%1.3f,%f",(j-1),AbsStr,StrMix,((SpecParam+j))->Mix,
			StrMax,((SpecParam+j))->Max,StrSplit,((SpecParam+j))->Split,StrWid,((SpecParam+j))->Wid,StrCoef,((SpecParam+j))->Kf,((SpecParam+j))->Amp);
		if (((SpecParam+j))->Mode==LORDIR)
			sprintf(crStr,"%i,%s,%s%f,%s%f,%s%1.3f,%f",(j-1),LorDStr,
			StrMax,((SpecParam+j))->Max,StrWid,((SpecParam+j))->Wid,StrCoef,((SpecParam+j))->Kf,((SpecParam+j))->Amp);
		if (((SpecParam+j))->Mode==GAUSDIR)
			sprintf(crStr,"%i,%s,%s%f,%s%f,%s%1.3f,%f",(j-1),GaussDStr,
			StrMax,((SpecParam+j))->Max,StrWid,((SpecParam+j))->Wid,StrCoef,((SpecParam+j))->Kf,((SpecParam+j))->Amp);
		strcat(crStr,"\n");
		outFile.write(crStr,strlen(crStr));
	  }
	  sprintf(crStr,"%f",Accurasy);
	  strcat(crStr,"\n");
	  outFile.write(crStr,strlen(crStr));
	outFile.close();
}
/*******************************************************************/
void TSpectrum:: SaveSCRData(const char* FileName)
{
	HFILE hFile;
	OFSTRUCT aFStruct;
	int ReadGood=0;
	short int OutData[501];
   int j,i;
   float Mult,Z,A;
   float* crData;
   long int adcTime;
   int FreeGain = 0;
   hFile=OpenFile(FileName,&aFStruct,OF_CREATE);
   if (hFile!=HFILE_ERROR)
   	{
      if (*(Param+10)==222)
         FreeGain=1;
      for (j=1;j<NSComp;j++)
       if ( (SpecParam+j)->Select>=NONE+1)
         {
         if (FreeGain)
            {
            A=fabs((SpecParam+j)->Amp+(SpecParam+j)->Zero);
            Z=fabs((SpecParam+j)->Zero);
            if ((Z!=0)&&(A!=0))
               {
            	if (A>Z) *(Param+10)=(short int)(2500/A);
						else	*(Param+10)=(short int)(2500/Z);
               }
            if (*(Param+10)<1) *(Param+10)=1;
            }
         Mult=0.2048*(*(Param+10));
         *(((SpecComp+j)))*=(SpecParam+j)->Amp;
         *(((SpecComp+j)))+=(SpecParam+j)->Zero;
         crData=(SpecComp+j)->GetData();
         if (ADCTime<=0)
      		adcTime=(long int)(1000*(*(SpecBase+NSPoint-1)-(*SpecBase))/(NSPoint-1));
         else	adcTime=ADCTime;
         if (NSPoint>=501)
         	for (i=0;i<501;i++)
         		OutData[i]=(int)(Mult*(*(crData+i)));
         else
         	for (i=0;i<501;i++)
         		OutData[i]=(int)(Mult*(*(crData+1)));
      	WriteFile((HANDLE)hFile,OutData,1002,(LPDWORD)&ReadGood,NULL);
		WriteFile((HANDLE)hFile,&adcTime,4,(LPDWORD)&ReadGood,NULL);
        WriteFile((HANDLE)hFile,Param,36,(LPDWORD)&ReadGood,NULL);
        WriteFile((HANDLE)hFile,TimeStr,9,(LPDWORD)&ReadGood,NULL);
        WriteFile((HANDLE)hFile,CommentStr,21,(LPDWORD)&ReadGood,NULL);
        *(((SpecComp+j)))-=((SpecParam+j))->Zero;
		*(((SpecComp+j)))/=((SpecParam+j))->Amp;
         }
      }
   CloseHandle((HANDLE)hFile);
}
/**************************************************************************/
int TSpectrum::ReadComp(const char* FileName, int nBefore,int nAfter, int StimPoint)
{
	TComponent* AuxComp;
	float* DataY;
	float* DataX;
	const int MaxL = 20000;
	char* StrRead;
	char* ptrRead;
	char* separator="\t\r\n;:, ";
	char  strRead[50];
	int nPoint, nComp, crLen, Offset, Row,Col, ReadGood, i,k, nData, N_Data;
	float Min,Max;
	ifstream inpFile;
	StrRead = new char[MaxL];
	inpFile.open(FileName,in);
	inpFile.getline(StrRead,MaxL,'\n');
	nComp=0;
	/*
	Offset = 0;
	ptrRead = strtok(StrRead,separator);
	
	while (ptrRead!=NULL)
	{
		Offset++;
		ptrRead=strtok(NULL,separator);
	}
	nComp = Offset-1;
	*/
	nPoint = 0;
	while (!inpFile.eof())
	{
		inpFile.getline(StrRead,MaxL,'\n');
		if (*StrRead!=';') 
		{
			nPoint++;
			if(!nComp)
			{
				Offset = 0;
				ptrRead = strtok(StrRead,separator);
				while (ptrRead!=NULL)
				{
					Offset++;
					ptrRead=strtok(NULL,separator);
				}
				nComp = Offset-1;
			}
		}
	}
	inpFile.close();
	//inpFile.seekg(0);
	DataY = new float[nPoint*nComp*sizeof(float)];
	DataX = new float[nPoint*sizeof(float)];
	inpFile.open(FileName,in);
	Row = 0;
	nData=0;
	while (Row<nPoint)
	{
		inpFile.getline(StrRead,MaxL,'\n');
		if (*StrRead!=';')
		{
			ptrRead=strtok(StrRead,separator);
			Col=0;
			do
			{
				i=0; k=0;
				while ( (*(ptrRead+k)!='\0')&&(*(ptrRead+k)!='\n'))
				{
					switch (*(ptrRead+k))
					{
						case '5':case '6':case '7':case '8':case '9':case '0':case '1':case '2':case '3':case '4':strRead[i]= (*(ptrRead+k));ReadGood=1;i++;break;
						case '+':case '-':case '.':case 'E':strRead[i]= (*(ptrRead+k));i++;break;
						default:ReadGood=0;
					}
					k++;
				}
				strRead[i]='\0';
				if (ReadGood)
				{
					if (Col)
						*(DataY+nPoint*(Col-1)+Row)=(float)atof(strRead);
					else
					{
						*(DataX+Row)=(float)atof(strRead);
						nData++;
					}
				}
				Col++;
				ptrRead=strtok(NULL,separator);
			}
			while ( (ptrRead!=NULL)&&(Col<=nComp));
			Row++;
		}
	}
	inpFile.close();
	if (nData!=nPoint)AfxMessageBox("Probably incorrect format");
	for (Col=1; Col<=nComp;Col++)
	{
		AuxComp=new TComponent(DataY+nPoint*(Col-1),DataX,nData);
		Max=AuxComp->MaxVal(StimPoint);
		Min=AuxComp->MinVal(StimPoint);
		if ((nBefore>4)&&(nAfter>=1))
			N_Data=AuxComp->Smooth(nBefore,nAfter);
		else N_Data=nData;
		if (N_Data>10*NDataPointMax)  N_Data = 10*NDataPointMax;
		NewComp(DataX,DataY+nPoint*(Col-1),N_Data,1);
		(SpecParam+NSComp-1)->ExtMax=Max;
		(SpecParam+NSComp-1)->ExtMin=Min;
		(SpecParam+NSComp-1)->Time=Col;
		delete AuxComp;
	}
	delete[] StrRead;	delete[] DataX; delete[] DataY;
	return nData-1;
}
/*******************************************************************/
void TSpectrum:: SaveComp(const char* FileName)
{
	ofstream outFile;
	int ColumnNum,j,i,ReadGood;
	float* DataY[MaxSaveColumnNum];
	char SaveStr[MaxSaveColumnNum*20];
	ColumnNum = 0;
	for (j=0;j<NSComp;j++)
		if( ((SpecParam+j)->Select!=NONE) && (ColumnNum<MaxSaveColumnNum-1) )
		{
			*(SpecComp+j)*= (SpecParam+j)->Amp;
			*(SpecComp+j)+= (SpecParam+j)->Zero;
			DataY[ColumnNum] = (SpecComp+j)->GetData();
			ColumnNum++;
		}
	outFile.open(FileName,out);
	for (i=0;i<NSPoint;i++)
	{
		sprintf(SaveStr,"%f",*(SpecBase+i));
		for (j=0;j<ColumnNum;j++)
		{
			sprintf(szNum,"\t%f",*(DataY[j]+i));
			strcat(SaveStr,szNum); 
		}
		strcat(SaveStr,"\n");
		outFile.write(SaveStr,strlen(SaveStr));	
		
	}
	outFile.close();
	for (j=0;j<NSComp;j++)
		if( ((SpecParam+j)->Select!=NONE) && (ColumnNum<MaxSaveColumnNum-1) )
		{
			*(SpecComp+j)-= (SpecParam+j)->Zero;
			*(SpecComp+j)/= (SpecParam+j)->Amp;
		}
	
			
}
/*******************************************************************/
void TSpectrum:: SaveConfig(const char* FileName)
{
	HFILE hFile;
	TOldParam*	SaveParam;
	OFSTRUCT aFStruct;
	int ReadGood=0;
	int j;
	int NLabel=0;
	int NTitle=0;
	SaveParam=new TOldParam;
	for (j=0;j<N_TITLE;j++)
   	if ((Title+j)->Visible) NTitle++;
	hFile=OpenFile(FileName,&aFStruct,OF_CREATE);
	j=NSComp-1;
	if (hFile!=HFILE_ERROR)
   	{
		WriteFile((HANDLE)hFile,"SCFG",5,(LPDWORD)&ReadGood,NULL);
		WriteFile((HANDLE)hFile,&NSPoint,sizeof(int),(LPDWORD)&ReadGood,NULL);
		WriteFile((HANDLE)hFile,&j,sizeof(int),(LPDWORD)&ReadGood,NULL);
		WriteFile((HANDLE)hFile,SpecBase,sizeof(float)*NSPoint,(LPDWORD)&ReadGood,NULL);
		for (j=1;j<NSComp;j++)
        {
      		WriteFile((HANDLE)hFile,(SpecComp+j)->GetData(),sizeof(float)*NSPoint,(LPDWORD)&ReadGood,NULL);
			SaveParam->CopyNew2Old(SpecParam+j);
			WriteFile((HANDLE)hFile,SaveParam,sizeof(TOldParam),(LPDWORD)&ReadGood,NULL);
        }
		SaveParam->CopyNew2Old(SpecParam);
		WriteFile((HANDLE)hFile,SaveParam,sizeof(TOldParam),(LPDWORD)&ReadGood,NULL);
		for (j=0;((j<NSComp)&&(j<N_LINE));j++)
        {
      		WriteFile((HANDLE)hFile,(SpecPen+j+1),sizeof(LOGPEN),(LPDWORD)&ReadGood,NULL);
			WriteFile((HANDLE)hFile,(Label+j),sizeof(TMark),(LPDWORD)&ReadGood,NULL);
      	}
		WriteFile((HANDLE)hFile,X_Axis,sizeof(TFrame),(LPDWORD)&ReadGood,NULL);
		WriteFile((HANDLE)hFile,Y_Axis,sizeof(TFrame),(LPDWORD)&ReadGood,NULL);
		WriteFile((HANDLE)hFile,&NTitle,sizeof(int),(LPDWORD)&ReadGood,NULL);
		for (j=0;j<N_TITLE;j++)
      	if ((Title+j)->Visible)
        {
      		WriteFile((HANDLE)hFile,(Title+j),sizeof(TMark),(LPDWORD)&ReadGood,NULL);
			WriteFile((HANDLE)hFile,(SpecFont+j),sizeof(LOGFONT),(LPDWORD)&ReadGood,NULL);
         }
      }
	delete SaveParam;
	CloseHandle((HANDLE)hFile);
}
/*******************************************************************/
void TSpectrum:: ReadConfig(const char* FileName, int SimpleCopy)
{
	//HFILE hFile;
	CFile cFile;
	TOldParam*	ReadParam;
	OFSTRUCT aFStruct;
	char StrBuf[128];
	int ReadGood=0;
	int j,NTitle,NTitle2,NComp,NPoint;
	TOldParam aParam;
	TFrame aFrame;
	TMark	 aMarker;
	LOGPEN aPen;
	LOGFONT aFont;
	float* DataX;
	float* DataY;
	int OldNSComp=NSComp;
	DataX=new float[20000];
	DataY=new float[20000];
	NTitle=0;
	for (j=0;j<N_TITLE;j++)
   	if ((Title+j)->Visible) NTitle++;
	//hFile=OpenFile(FileName,&aFStruct,OF_READ);
	
	if (cFile.Open(FileName, CFile::modeRead|CFile::typeBinary))
	{
		//ReadFile((HANDLE)hFile,StrBuf,5,(LPDWORD)&ReadGood,NULL);
		cFile.Read(StrBuf,5);
		*(StrBuf+4)='\0';
		cFile.Read(&NPoint,sizeof(int));
		//ReadFile((HANDLE)hFile,&NPoint,sizeof(int),(LPDWORD)&ReadGood,NULL);
		//ReadFile((HANDLE)hFile,&NComp,sizeof(int),(LPDWORD)&ReadGood,NULL);
		cFile.Read(&NComp,sizeof(int));
		if ((NComp>0)&&(NComp<NCompMax)&&(NPoint>1)&&(NPoint<NDataPointMax))
		{
			//ReadFile((HANDLE)hFile,DataX,sizeof(float)*NPoint,(LPDWORD)&ReadGood,NULL);
			cFile.Read(DataX,sizeof(float)*NPoint);
			
			for (j=0;j<NComp;j++)
			{
      			//ReadFile((HANDLE)hFile,DataY,sizeof(float)*NPoint,(LPDWORD)&ReadGood,NULL);
				cFile.Read(DataY,sizeof(float)*NPoint);
				//ReadFile((HANDLE)hFile,&aParam,sizeof(TOldParam),(LPDWORD)&ReadGood,NULL);
				cFile.Read(&aParam,sizeof(TOldParam));
				NewComp(DataX,DataY,NPoint,SimpleCopy);
				aParam.CopyOld2New(SpecParam+NSComp-1);
				
				//(SpecParam+NSComp-1)->Time=NSComp-1;
			}// of for
			//ReadFile((HANDLE)hFile,&aParam,sizeof(TOldParam),(LPDWORD)&ReadGood,NULL);
			cFile.Read(&aParam,sizeof(TOldParam));
			ResetAxis();
			if (!OldNSComp)
			aParam.CopyOld2New(SpecParam);
			for (j=OldNSComp;(j<OldNSComp+NComp+1);j++)
			{
      			//ReadFile((HANDLE)hFile,&aPen,sizeof(LOGPEN),(LPDWORD)&ReadGood,NULL);
				//ReadFile((HANDLE)hFile,&aMarker,sizeof(TMark),(LPDWORD)&ReadGood,NULL);
				cFile.Read(&aPen,sizeof(LOGPEN));
				cFile.Read(&aMarker,sizeof(TMark));
				if (j<N_LINE)
         		{
         			*(SpecPen+j+1)=aPen;
					*(Label+j)=aMarker;
         		}
      		}// of for
			//ReadFile((HANDLE)hFile,&aFrame,sizeof(TFrame),(LPDWORD)&ReadGood,NULL);
			
			cFile.Read(&aFrame,sizeof(TFrame));
			X_Axis->Import(&aFrame);
			//ReadFile((HANDLE)hFile,&aFrame,sizeof(TFrame),(LPDWORD)&ReadGood,NULL);
			cFile.Read(&aFrame,sizeof(TFrame));
			Y_Axis->Import(&aFrame);
			//ResetAxis();
			//ReadFile((HANDLE)hFile,&NTitle2,sizeof(int),(LPDWORD)&ReadGood,NULL);
			cFile.Read(&NTitle2,sizeof(int));
			for (j=NTitle;j<NTitle+NTitle2;j++)
			{
				//ReadFile((HANDLE)hFile,&aMarker,sizeof(TMark),(LPDWORD)&ReadGood,NULL);
				//ReadFile((HANDLE)hFile,&aFont,sizeof(LOGFONT),(LPDWORD)&ReadGood,NULL);
      			cFile.Read(&aMarker,sizeof(TMark));
				cFile.Read(&aFont,sizeof(LOGFONT));
				if (j<N_TITLE)
         		{
					*(Title+j)=aMarker;
					*(SpecFont+j)=aFont;
         		}
				
				
			}//of for
   		}// of if 
	}// of if 
	//CloseHandle((HANDLE)hFile);
	
	cFile.Close();
	delete DataX;
	delete DataY;
}
/*******************************************************************/
void TSpectrum:: ResetComp(int DestN)
{
	if ( (SpecParam+DestN)->Mode != FILE )
		(SpecComp+DestN)->Reset(SpecParam+DestN);
}
/*******************************************************************/
float TSpectrum:: GetAccur(void)
{
	return 0;
}
/*******************************************************************/
float TSpectrum::LeastSqr(int TestMode)
{
	int j,k,N;
	int BinomFlag,ClampFlag, SinusFlag;
	float Sigma, Shift;
	float far* A;
	float far* B;
	float far* Y;
	//Y=(float*)malloc(NSComp*sizeof(float));
	//B=(float*)malloc(NSComp*sizeof(float));
	//A=(float*)malloc(NSComp*(NSComp+2)*sizeof(float));
	N=NSComp-BegFitComp;
	A=FitComp;
	B=A+(N+2)*(N+2);
	Y=B+(N+2);
	if (TestMode==TEST_LLH)
   	{
    j=TagComp;
		//j=0;
		*((SpecComp+j))*=((SpecParam+j))->Amp;
		*((SpecComp+j))+=((SpecParam+j))->Zero;
		Sigma=0.0001*((SpecComp+j)->LikeHoodTest(SpecParam+BegFitComp));
		*((SpecComp+j))-=((SpecParam+j))->Zero;
		*((SpecComp+j))/=((SpecParam+j))->Amp;
		return (Sigma);
    }
	else
	{
	if ( NSComp>BegFitComp)
	{
		*(((SpecComp)))=((SpecComp+TagComp));
		((SpecParam))->Amp=((SpecParam+TagComp))->Amp;
		((SpecParam))->Zero=((SpecParam+TagComp))->Zero;
		// creating and resetting the background component
      //NSComp++;
      //(SpecComp+NSComp-1)->Set(BcgrComp->GetData(),SpecBase,NSPoint);
      //(SpecParam+NSComp-1)->Amp=1;
		//(SpecParam+NSComp-1)->Zero=0;
      //(SpecParam+NSComp-1)->Kf=1;
		BinomFlag=0;
		ClampFlag=0;
		SinusFlag=0;
     	for (j=BegFitComp;j<NSComp;j++)
      		{
			if ( ((SpecParam+j)->Mode==UNIMODAL)||((SpecParam+j)->Mode==BIMODAL)||((SpecParam+j)->Mode>=BINOM)||((SpecParam+j)->Mode==BINOM_NZ)) BinomFlag=1;
			if((SpecParam+j)->Mode==SINUS)
			{
				BinomFlag=0;
				SinusFlag=j;
			}
		}
		BcgrComp->InitData(1);
		if (SinusFlag) (SpecComp+SinusFlag)->InitData(0.000001);
      /*
		(*((SpecComp+NSComp-1)))*=(0.0);
		(*((SpecComp+NSComp-1)))+=(1.0);
       */

		// re-norming of the component
		((SpecParam+TagComp))->Amp/=((SpecParam+TagComp))->Kf;
		(SpecParam)->Amp/=(SpecParam)->Kf;
		for (j=BegFitComp;j<NSComp;j++)
		{
			((SpecParam+j))->Amp/=((SpecParam+j))->Kf;
			if ((SpecParam+j)->Mode==CLAMP_ST)
         	{
            //ClampFlag=1;
				Shift=(SpecComp+TagComp)->GetBaseLevel(0,20)-(SpecComp+j)->GetBaseLevel(0,20);
				*((SpecComp+j))+=Shift;
				((SpecParam+j))->Zero+=Shift;
            }
		}
		for (j=BegFitComp;j<NSComp;j++)
		//if ( (SpecParam+j)->Mode==FILE )
		{
			(SpecParam+j)->Amp=(SpecParam+TagComp)->Amp;
			(SpecParam+j)->Zero=(SpecParam+TagComp)->Zero;
		}
		N=NSComp-BegFitComp;
		for (j=0;j<N;j++)
		{
			*(B+j)=Mult(((SpecComp+TagComp)),(SpecComp+j+BegFitComp));
         	for (k=0;k<N;k++)
			*(A+j*(N+1)+k)=Mult((SpecComp+k+BegFitComp),(SpecComp+j+BegFitComp));
			*(A+j*(N+1)+N)=Mult(BcgrComp,(SpecComp+j+BegFitComp));
		}
		*(B+N)=Mult(((SpecComp+TagComp)),BcgrComp);
		for (k=0;k<N;k++)
			*(A+N*(N+1)+k)=Mult((SpecComp+k+BegFitComp),BcgrComp);
		*(A+N*(N+1)+N)=Mult(BcgrComp,BcgrComp);
		if (BinomFlag||ClampFlag)
      	{
         EquSet(Y,A,B,N);
         *(Y+N)=0;
        }
		else
	      EquSet(Y,A,B,N+1);
		for (j=0;j<N;j++)
			{
			if ( (*(Y+j))<=0 )  (*(Y+j))=0.000001;
			((SpecParam+j+BegFitComp)->Amp)*= (*(Y+j));
			(SpecParam+j+BegFitComp)->Kf = (*(Y+j));
			}

		//(SpecParam+NSComp-1)->Amp *= (*(Y+j));
		//(SpecParam+NSComp-1)->Kf = (*(Y+j));

		for (j=BegFitComp;j<NSComp;j++)
			*(SpecComp+j)*=(SpecParam+j)->Kf;
		
		
		//else
		*(BcgrComp)*=(*(Y+N));
		for (j=BegFitComp;j<NSComp;j++)
			*(SpecComp)-=((SpecComp+j));
		*(SpecComp)-=BcgrComp;
		if (SinusFlag) 
		{
			j=SinusFlag;
			(SpecParam+j)->Amp=(SpecParam+TagComp)->Amp;
			(SpecParam+j)->Zero=(SpecParam+TagComp)->Zero;
			(SpecParam+j)->Kf=1;
			(SpecComp+j)->Reset(SpecParam+j);
			//*(SpecComp+j)*=(SpecParam+j)->Amp;
			*(SpecComp)-=((SpecComp+j));
			//*(SpecComp+j)/=(SpecParam+j)->Amp;
		}
		//Sigma=((SpecComp))->SSqu()/(((SpecComp+1))->SSqu());

		if (ShowSumm)
			{
			*((SpecComp))-=((SpecComp+TagComp));
   		*((SpecComp))*=(-1.0);
         if (ClampFlag)
         	{
         	Shift=(SpecComp+TagComp)->GetBaseLevel(0,20)-(SpecComp)->GetBaseLevel(0,20);
         	*((SpecComp))+=Shift;
         	}
			Sigma=Norm((SpecComp),(SpecComp+TagComp));
			*((SpecComp))*=(SpecParam)->Amp;
			*((SpecComp))+=(SpecParam)->Zero;
			(SpecParam)->Zero=SpecComp->MinVal();
			(SpecParam)->Amp=SpecComp->MaxVal()-(SpecParam)->Zero;
			*((SpecComp))-=(SpecParam)->Zero;
			*((SpecComp))/=(SpecParam)->Amp;
			}
		else
			{
			*((SpecComp))-=((SpecComp+TagComp));
			*((SpecComp))*=(-1.0);
			Sigma=Norm((SpecComp),(SpecComp+TagComp));
			*((SpecComp))*=(-1.0);
			*((SpecComp))+=((SpecComp+TagComp));
         
			}
      	for (j=BegFitComp;j<NSComp;j++)
		{
			(SpecParam+j)->Kf = ValidFloat((SpecParam+j)->Kf, 0.000001);
			*(SpecComp+j)/=(SpecParam+j)->Kf;
		}
		//for (j=2;j<NSComp;j++)

		((SpecComp))->ReSpline();
		//((SpecComp+1))->ReSpline();
      //NSComp--;
		return (Sigma);
		}
	//free(A);
	//free(B);
	//free(Y);

	else
		return -1;
	}

}
/****************************************/
float TSpectrum::Intensity()
{
float S=0;
int j;
for(j=2;j<NSComp;j++)
	switch ((SpecParam+j)->Mode)
		{
		case LORDIR:S+=3.141593*((SpecParam+j)->Amp)*((SpecParam+j)->Wid)*((SpecParam+j)->Wid)/1000;break;
		case ABSORB:S+=3.141593*((SpecParam+j)->Amp)*((SpecParam+j)->Wid);break;
		caseGAUSLOR:S+=3.141593*((SpecParam+j)->Amp)*((SpecParam+j)->Wid);break;
		};
return S;
}
/************************************************************************/
void TSpectrum::BackSubst(int N)
{
int j;
float OldBase,NewBase;
if ((N>=2)&&(N<NSComp))
	{
	*(((SpecComp+1)))*=((SpecParam+1))->Amp;
	*(((SpecComp+1)))+=((SpecParam+1))->Zero;
   OldBase=(SpecComp+1)->GetBaseLevel(0);
	*(((SpecComp+N)))*=((SpecParam+N))->Amp;
	*(((SpecComp+N)))+=((SpecParam+N))->Zero;
	*(((SpecComp+1)))-=((SpecComp+N));
   NewBase=(SpecComp+1)->GetBaseLevel(0);
   *(((SpecComp+1)))-=NewBase-OldBase;
	((SpecParam+1))->Zero=((SpecComp+1))->MinVal();
	((SpecParam+1))->Amp=((SpecComp+1))->MaxVal()-((SpecParam+1))->Zero;
	*(((SpecComp+1)))-=((SpecParam+1))->Zero;
	if (((SpecParam+1))->Amp!=0.0)
		{
		*(((SpecComp+1)))/=((SpecParam+1))->Amp;
		}
	*(((SpecComp+N)))-=((SpecParam+N))->Zero;
	*(((SpecComp+N)))/=((SpecParam+N))->Amp;
	((SpecParam+N))->Zero=((SpecParam+1))->Zero;
   for (j=2;j<NSComp;j++)
		if ( ((SpecParam+j))->Mode!=FILE )((SpecParam+j))->Zero=((SpecParam+1))->Zero;
	((SpecComp+1))->ReSpline();
	ResetAxis();
	}
}

/*******************************************************************/
float TSpectrum::AllSumm(int SelectedMode)
{
	int j;
	float Sigma,Delta;
	if ( NSComp>2)
		{
		//*(SpecComp)=(SpecComp+1);
		*(SpecComp)*=0;
		//SpecParam->Amp=(SpecParam+1)->Amp;
		//SpecParam->Zero=(SpecParam+1)->Zero;
		SpecParam->Select=NONE+1;
		// re-norming of the component
		for (j=1;j<NSComp;j++)
		if ( ((SpecParam+j)->Select!=NONE)||(SelectedMode) )
			{
			*(SpecComp+j)*=(SpecParam+j)->Amp;
			*(SpecComp+j)+=(SpecParam+j)->Zero;
			*(SpecComp)+=(SpecComp+j);
			}
		/*
		for (j=1;j<NSComp;j++)
			if ( ((SpecParam+j)->Select!=NONE)||(SelectedMode) )
			*(SpecComp)-=(SpecComp+j);
		//Sigma=(((SpecComp))->SSqu())/(((SpecComp+1))->SMod());
		
		*(SpecComp)-=(SpecComp+1);
		*(SpecComp)*=(-1.0);
		*/
      Delta=(SpecComp)->GetBaseLevel(0)-(SpecComp+1)->GetBaseLevel(0);
      *(SpecComp)-=Delta;
      Sigma=Norm((SpecComp),(SpecComp+1));
		(SpecParam)->Zero=(SpecComp)->MinVal();
		(SpecParam)->Amp=(SpecComp)->MaxVal()-(SpecParam)->Zero;

		for (j=0;j<NSComp;j++)
			if ( ((SpecParam+j)->Select!=NONE)||(SelectedMode) )
			{
				*(SpecComp+j)-=(SpecParam+j)->Zero;
				*(SpecComp+j)/=(SpecParam+j)->Amp;
			}
		(SpecComp)->ReSpline();
		return (Sigma);
		}
   return 0;

}
/*******************************************************************/
float TSpectrum::Average(int SelectedMode)
{
	int j,N;
	float Sigma;
	if ( NSComp>2)
	{
		*(SpecComp)*=0.0;
		// re-norming of the component
		for (j=1;j<NSComp;j++)
		{
			*((SpecComp+j))*=(SpecParam+j)->Amp;
			*((SpecComp+j))+=(SpecParam+j)->Zero;
		}
		N=0;
		for (j=1;j<NSComp;j++)
			if ( ((SpecParam+j)->Select>=NONE+1)||(SelectedMode) )
			{
				*(SpecComp)+=(SpecComp+j);
				N++;
			}
		if (N)
			*(SpecComp)/=N;
		Sigma=0;
		for (j=1;j<NSComp;j++)
			Sigma=Norm(SpecComp,SpecComp+j);
		Sigma/=(NSComp-1);
		StDev=Sigma;
		(SpecParam)->Zero=(SpecComp)->MinVal();
		(SpecParam)->Amp=(SpecComp)->MaxVal()-(SpecParam)->Zero;
		for (j=0;j<NSComp;j++)
			{
			*(SpecComp+j)-=(SpecParam+j)->Zero;
			*(SpecComp+j)/=(SpecParam+j)->Amp;
			}
		(SpecComp)->ReSpline();
		return (Sigma);
		}
   	else
      {
        *(SpecComp)*=0.0;
        *(SpecComp)+=(SpecComp+1);
		(SpecParam)->Zero=(SpecParam+1)->Zero;
		(SpecParam)->Amp=(SpecParam+1)->Amp;
		return 0;
       }
}
/*******************************************************************/
void  TSpectrum::PhrFilter(TPhourier* Filter)
{
   int BaseBeg=0;
	int BaseEnd=10;
   float Shift;
	(*(SpecComp))*=SpecParam->Amp;
   (*(SpecComp))+=SpecParam->Zero;
   SpecComp->PhrFilterHigh(Filter);
   ((SpecParam))->Zero=((SpecComp))->MinVal();
	((SpecParam))->Amp=((SpecComp))->MaxVal()-((SpecParam))->Zero;
   (*(SpecComp))-=SpecParam->Zero;
	(*(SpecComp))/=SpecParam->Amp;
   ((SpecComp))->ReSpline();
   Shift=(SpecParam)->Zero+(SpecParam)->Amp*((SpecComp)->GetBaseLevel(BaseBeg,BaseEnd));
   Shift-=(SpecParam+1)->Zero+(SpecParam+1)->Amp*((SpecComp+1)->GetBaseLevel(BaseBeg,BaseEnd));
   (SpecParam)->Zero-=Shift;
}
/*******************************************************************/
// xBase and yData should refer to data block not less than 2*NSPoint !
int  TSpectrum::Extrapolate(float* xBase, float* yData, TPhourier* Filter, TFitParam*	FitState, float xSize, int CurveN)
{
	int Count,j,i,k,PrevNSComp,extData;
	float Period, Kf, Wid, Split,NextMax,dX,EndX;
	float Lock[500];
	float* tmpData;
	float* OldData;
	Count=(SpecComp+CurveN)->ExtremSearch(Filter, Lock, 50);
	if (Count>1)
	{
		k=0;	Period=0;
		while(k<Count-1)
		{
			Period+=Lock[k+1]-Lock[k];
			k++;
		}
		Period/=k;
		PrevNSComp=NSComp;
		//approximation with a set of model curves 
		for (i=0;i<Count;i++)
			if(CreateComp()!=0)
			{
			  j=NSComp-1;
			  if ((CreateMode==RIDEC_O)||(CreateMode==RIDEC_I))
			  {
				  (SpecParam+j)->Max=Lock[i]-0.5;
				  (SpecParam+j)->Wid=0.1*Period;
				  (SpecParam+j)->Split=Period;
			  }
			  else
			  {
				  (SpecParam+j)->Max=0.5*(Lock[i+1]+Lock[i]);
				  (SpecParam+j)->Wid=1.41*fabs(Lock[i+1]-Lock[i]);
				  (SpecParam+j)->Mix=0.5;
			  }
			  (SpecParam+j)->Mode=CreateMode;
			  (SpecComp+j)->Reset(SpecParam+j);
			}
		TagComp=CurveN;
		for(i=0;i<3;i++) AutoFit(FitState);
		k=0;	Kf=0;	Period=0;	Wid=0;	Split=0;
		for(j=BegFitComp+1;j<NSComp;j++)
			if ( (SpecParam+j)->Kf>0.05 )
			{
				Period+=(SpecParam+j)->Max-(SpecParam+j-1)->Max;
				Kf+=(SpecParam+j)->Kf;
				Wid+=(SpecParam+j)->Wid;
				Split+=(SpecParam+j)->Split;
				k++;
			}
		Period/=k; Wid/=k; Kf/=k; Split/=k;
		NextMax=0;
		for(j=BegFitComp+1;j<NSComp;j++)
			if ( (SpecParam+j)->Kf>0.05 )
				NextMax+=(SpecParam+j)->Max+Period*(NSComp-j);
		NextMax/=k;
		//creating extra curve
		if(CreateComp()!=0)
		{
		  j=NSComp-1;
		  (SpecParam+j)->Max=NextMax;
		  (SpecParam+j)->Wid=Wid;
		  (SpecParam+j)->Split=Split;
		  (SpecParam+j)->Mix=0.5;
		  (SpecParam+j)->Mode=CreateMode;
		  (SpecParam+j)->Kf=0.66*Kf;
		}
		//recalculating the x base
		dX=(*(SpecBase+NSPoint-1)-(*SpecBase))/NSPoint;
		if (xSize>0) EndX=xSize;
			else	EndX=2*Period;
		extData=(int)((*(SpecBase+NSPoint-1)-(*SpecBase)+EndX)/dX);
		if (extData>2*NSPoint-1) extData=2*NSPoint-1;
		for (j=0;j<extData;j++) *(xBase+j)=*SpecBase+j*dX;
		// initializing the data
		tmpData=new float[2*extData+1];
		OldData=SpecComp->GetData();
		SpecComp->Set(yData,xBase,extData);
		SpecComp->InitData(*(BcgrComp->GetData()));
		//calculating the sum of model curves
		for(j=BegFitComp;j<NSComp;j++)
		{
			(SpecComp+j)->Set(tmpData,xBase,extData);
			(SpecComp+j)->Reset(SpecParam+j);
			*(SpecComp+j)*=(SpecParam+j)->Kf;
			*(SpecComp)+=(SpecComp+j);
		}
		*(SpecComp)*=(SpecParam+TagComp)->Amp;
		*(SpecComp)+=(SpecParam+TagComp)->Zero;
		SpecComp->Set(OldData,SpecBase,NSPoint);
		//sprintf(szNum,"Per: %f  tr: %f tdec: %f Next: %f",Period,Wid,Split,NextMax);
		//AfxMessageBox(szNum);
		ClearAllComp(PrevNSComp);
		return extData;
	}
	else 
		return 0;
}
/*******************************************************************/
void  TSpectrum::Phourier(TPhourier* Filter)
{
	int Count,j,i,jSel;
	float Lock[500];
	jSel=FindSelComp(BegFitComp);
	if(!jSel)jSel=1;
		if (BegFitComp<NSComp)
		ClearAllComp(BegFitComp);
	BegFitComp=NSComp;
	if ((CreateMode==RIDEC_O)||(CreateMode==RIDEC_I))
		{
		//(SpecComp)->SmoothCut(0,NSPoint-1,0.01);
		//(SpecComp)->Derive();
		if (CreateMode==RIDEC_I)
			Count=(SpecComp+jSel)->ExtremSearch(Filter, Lock, 50,-1);
		else
			Count=(SpecComp+jSel)->ExtremSearch(Filter, Lock, 50,1);
		if (Count>0)
			for (i=0;i<Count;i++)
				if(CreateComp()!=0)
				  {
				  j=NSComp-1;
				  (SpecParam+j)->Max=Lock[i]-1.05;
				  (SpecParam+j)->Wid=1;
				  (SpecParam+j)->Split=7.1;
				  (SpecParam+j)->Mode=CreateMode;
				  (SpecComp+j)->Reset(SpecParam+j);
				  }
		//*(((SpecComp)))=((SpecComp+1));
		}
	if (CreateMode==LORDIR)
		{
		Count=(SpecComp+jSel)->ExtremSearch(Filter, Lock, 50);
		if (Count>1)
			for (i=0;i<Count;i+=2)
				if(CreateComp()!=0)
				  {
				  j=NSComp-1;
				  ((SpecParam+j))->Max=0.5*(Lock[i+1]+Lock[i]);
				  ((SpecParam+j))->Wid=1.7*fabs(Lock[i+1]-Lock[i]);
				  ((SpecParam+j))->Mix=0.5;
				  ((SpecParam+j))->Mode=CreateMode;
				  (SpecComp+j)->Reset((SpecParam+j));
				  }
		}
	if (CreateMode==GAUSLOR)
		{
		*(SpecComp)=(SpecComp+jSel);
		(SpecParam)->Amp=(SpecParam+jSel)->Amp;
		(SpecParam)->Zero=(SpecParam+jSel)->Zero;
		(SpecComp)->Derive();
		Count=(SpecComp)->ExtremSearch(Filter, Lock, 50,0);
		if (Count>1)
			for (i=0;i<Count;i+=2)
				if(CreateComp()!=0)
				  {
				  j=NSComp-1;
				  ((SpecParam+j))->Max=0.5*(Lock[i+1]+Lock[i]);
				  ((SpecParam+j))->Wid=1.99*fabs(Lock[i+1]-Lock[i]);
				  ((SpecParam+j))->Mix=0.5;
				  ((SpecParam+j))->Mode=CreateMode;
				  (SpecComp+j)->Reset((SpecParam+j));
				  }
		*(SpecComp)=(SpecComp+jSel);
		}
	if (CreateMode==ABSORB)
		{
		*(SpecComp)=(SpecComp+jSel);
		(SpecParam)->Amp=(SpecParam+jSel)->Amp;
		(SpecParam)->Zero=(SpecParam+jSel)->Zero;
		*(SpecComp)-=1;
		*(SpecComp)*=1;
		(SpecComp)->Derive();
		Count=(SpecComp)->ExtremSearch(Filter, Lock, 50);
		if (Count>1)
			for (i=0;i<Count;i+=2)
				if(CreateComp()!=0)
				  {
				  j=NSComp-1;
				  ((SpecParam+j))->Max=0.5*(Lock[i+1]+Lock[i]);
				  ((SpecParam+j))->Wid=0.8661*fabs(Lock[i+1]-Lock[i]);
				  ((SpecParam+j))->Mix=0.5;
				  ((SpecParam+j))->Mode=CreateMode;
				  (SpecComp+j)->Reset((SpecParam+j));
				  }
		*(SpecComp)=(SpecComp+jSel);
		}
}
/*******************************************************************/
int	TSpectrum::SeekComp(float Amp1, float Amp2,int StimPoint)
{
	int j;	int Count=0;
	float PeakAmp;
	for (j=0;j<NSComp;j++)
		(SpecParam+j)->Select=NONE;
	for (j=1;j<NSComp;j++)
   	{
		if(StimPoint>0)
			PeakAmp=((SpecComp+j)->GetBaseLevel(0,(int)(0.6*StimPoint))-(SpecComp+j)->MinVal(StimPoint))*(SpecParam+j)->Amp;
		else
			PeakAmp=(SpecParam+j)->Amp;
		if ((PeakAmp>=Amp1)&&(PeakAmp<=Amp2))
      	{
			(SpecParam+j)->Select=NONE+1;
			Count++;
        }
    }
	if (!Count)
      (SpecParam)->Select=NONE+1;
	return Count;
}
/******************************************************************/
int	TSpectrum::SeekFitResult(float Amp1, float Amp2, float tR1, float tR2, float tD1, float tD2, int IgnoreFlag)
{
	int j;	int Count=0;
	for (j=0;j<NSComp;j++)
		(SpecParam+j)->Select=NONE;
  	for (j=1;j<NFitResult;j++)
      	if (((CompFitResult+j)->Amp>=Amp1)&&((CompFitResult+j)->Amp<=Amp2)&&((CompFitResult+j)->tDec>=tD1)&&((CompFitResult+j)->tDec<=tD2)&&((CompFitResult+j)->tRise>=tR1)&&((CompFitResult+j)->tRise<=tR2)&&((CompFitResult+j)->AcceptFlag||IgnoreFlag))
				{
            (SpecParam+j)->Select=NONE+1;
         	Count++;
            }
   if (!Count)
      (SpecParam)->Select=NONE+1;
   return Count;
}
//
int TSpectrum::GetFitResultMargin(TFitResult* MinResult, TFitResult* MaxResult)
{
	int j; int  Count=0;
	if (NFitResult)
	{
		MinResult->Amp=(CompFitResult+1)->Amp;
		MinResult->tRise=(CompFitResult+1)->tRise;
		MinResult->tDec=(CompFitResult+1)->tDec;
		MaxResult->Amp=(CompFitResult+1)->Amp;
		MaxResult->tRise=(CompFitResult+1)->tRise;
		MaxResult->tDec=(CompFitResult+1)->tDec;
		for (j=2;j<NFitResult;j++)
			if((CompFitResult+j)->AcceptFlag)
			{
				Count++;
				if((CompFitResult+j)->Amp < MinResult->Amp) MinResult->Amp=(CompFitResult+j)->Amp;
				if((CompFitResult+j)->Amp > MaxResult->Amp) MaxResult->Amp=(CompFitResult+j)->Amp;
				if((CompFitResult+j)->tRise < MinResult->tRise) MinResult->tRise=(CompFitResult+j)->tRise;
				if((CompFitResult+j)->tRise > MaxResult->tRise) MaxResult->tRise=(CompFitResult+j)->tRise;
				if((CompFitResult+j)->tDec < MinResult->tDec) MinResult->tDec=(CompFitResult+j)->tDec;
				if((CompFitResult+j)->tDec > MaxResult->tDec) MaxResult->tDec=(CompFitResult+j)->tDec;
			}
    }
	return Count;
}

/******************************************************************\
* ATTENTION!!!	size of HistBase shold be > = NBin+1 !              *
\******************************************************************
void  TSpectrum::CreatePDF(float* HistBase, float* HistData, float minVal, float maxVal, int NBin, int BegPoint, int EndPoint, float BinSize)
{
	int j,i,Count;
	float crVal;
	crData=new float [NDATA_MAX];
	for (j=0;j<NBin+1;j++)
        {
        *(HistBase+j)=minVal+j*(maxVal-minVal)/(NBin-0.5);
        for (i=0;i<Count;i++)
            {
            crVal=(*(HistBase+j)-(*(crData+i)))/BinSize;
            *(HistData+j)+=0.3989*exp(-crVal*crVal)/(Count*BinSize);
            }
         }
}
/*****************************************************************/
void  TSpectrum::CreateHist(float* HistBase, float* HistData,int NBin, int PDFMode, int BegPoint, int EndPoint, int StimPoint, float* PBinSize)
{
	int j,i,Count,NComp;
	float* crData;
	float* crY;
	float*	CumHstY;
	float*	CumHstX;
	TComponent* PDFComp;
	float BinSize,ExtreMin,ExtreMax,crVal,Norm;
	Count=0;
	crData=new float [NDATA_MAX];
	if (NSComp>2)
	{
		for(j=0;j<NSComp;j++)
			if((SpecParam+j)->Select!=NONE)
				NComp=j;
	/*
		for (j=BegPoint;j<=EndPoint;j++)
      	{
	      *(crData+Count)=((SpecComp+j)->GetBaseLevel(0,(int)(0.6*StimPoint))-(SpecComp+j)->MinVal(StimPoint))*(SpecParam+j)->Amp;
         Count++;
         }
		*/
	}
	else
   		NComp=1;
	
	
	
	ExtreMin=(SpecParam+1)->Zero;
	ExtreMax=(SpecParam+1)->Amp;
   	crY=(SpecComp+NComp)->GetData();
	Count=NSPoint;
	for (j=0;j<Count;j++)
     *(crData+j)=ExtreMin+(*(crY+j))*ExtreMax;
 
   ExtreMin= *(crData);
   ExtreMax=ExtreMin;
   for (j=1;j<Count;j++)
     {
		crVal= *(crData+j);
		if (crVal>ExtreMax)ExtreMax=crVal;
		if (crVal<ExtreMin)ExtreMin=crVal;
  	  }
   if (PBinSize!=NULL)
   {
   	BinSize= *PBinSize;
      //ExtreMin=0;
    }
   else
   	{
      BinSize=(ExtreMax-ExtreMin)/(NBin-0.5);
      }
   for (j=0;j<NBin+1;j++)
         {
      	*(HistBase+j)=ExtreMin+j*BinSize;
         *(HistData+j)=0;
         }
	if (Count)
   	if (PDFMode>0)
	{
        
		for (j=0;j<NBin+1;j++)
        {
            if (PDFMode>=1)
            	*(HistBase+j)=ExtreMin+j*(ExtreMax-ExtreMin)/(NBin-0.5);
            else
					*(HistBase+j)=ExtreMin+j*BinSize*1.5;
		
			for (i=0;i<Count;i++)
            {
               crVal=(*(HistBase+j)-(*(crData+i)))/BinSize;
               //*(HistData+j)+=0.3989*exp(-crVal*crVal)/(Count*BinSize);
			   *(HistData+j)+=0.5714*exp(-crVal*crVal)/(Count*BinSize);
            }
			
         }
		//Normalization
		Norm=(*HistData)*(*(HistBase+1)- *HistBase); 
		for(j=1;j<NBin+1;j++)
			Norm+=0.5*(*(HistData+j)+ *(HistData+j-1))*(*(HistBase+j)- *(HistBase+j-1));
		for (j=0;j<NBin+1;j++)
			*(HistData+j)/=Norm;
		if (PDFMode>1) // cumulative histogram
		{
			
			CumHstX= new float[NBin+1];
			CumHstY= new float[NBin+1];
			PDFComp= new TComponent(CumHstY, CumHstX, NBin+1);
			*(CumHstY)=0;
			for (j=1;j<NBin+1;j++)
			{
				//*(CumHstX+j)=*(HistBase+j);
				*(CumHstY+j)=0;
				for (i=1;i<=j;i++)
					*(CumHstY+j)+= fabs(0.5*(*(HistData+i)+ *(HistData+i-1))*(*(HistBase+i)- *(HistBase+i-1)));
			}
			for (j=0;j<NBin+1;j++)
			{
				*(HistData+j) = *(CumHstY+j)/(*(CumHstY+NBin));
				//*(HistBase+j)= *(CumHstX+j);
			}
		
			delete PDFComp;
			delete CumHstY;
			delete CumHstX;
		}

	 }
    else
   	for (j=0;j<Count;j++)
      	{
         crVal= *(crData+j);
         if ( crVal<=(*(HistBase)+0.5*BinSize) )
            	*(HistData)+=1.0/Count;
         else
         for (i=1;i<NBin;i++)
         	if ( (crVal>*(HistBase+i-1)+0.5*BinSize)&&(crVal<=(*(HistBase+i)+0.5*BinSize) ))
            	*(HistData+i)+=1.0/Count;
         }
}
/**********************************************************************************/
int TSpectrum::CreateTimeCourse(float tmBeg, float tmEnd, float tmInt, float* tmBase, float* tmData, int tmSize)
{
	int j,i,jSel,nTime,lastN,firstN,nTmCrData;
	float Mean, SD,begX,endX,Y,Amp,Zero,Count;
	float* crMean;
	float* crSD;
	float* crFreq;
	float* crData;
	if (tmBeg< *SpecBase) begX=*(SpecBase);
	else begX=tmBeg;
	if (tmEnd> *(SpecBase+NSPoint-1)) endX=*(SpecBase+NSPoint-1);
	else endX=tmEnd;
	nTime=(int)(1+(endX-begX)/tmInt);
	//if (begX+nTime*tmInt<endX) nTime++;
	jSel=FindSelComp();
	Amp=(SpecParam+jSel)->Amp;	Zero=(SpecParam+jSel)->Zero;
	lastN=0;
	nTmCrData=0;
	for(j=0;j<nTime;j++)
	{
		firstN=lastN;
		lastN=(SpecComp+1)->FindPoint(begX+(j+1)*tmInt);
		if(lastN>firstN)
		{
			*(tmBase+nTmCrData)=begX+(0.5+j)*tmInt;
			Mean=0;
			SD=0;
			crData=(SpecComp+jSel)->GetData()+firstN;
			Count=lastN-firstN+0.9999999;
			for(i=firstN;i<=lastN;i++)
			{
				Y=*crData;
				Mean+=Y/Count;
				SD+=Y*Y/Count;
				crData++;
			}
			*(tmData+nTmCrData)=Mean*Amp+Zero; // tmCourse of avereage amplitude
			*(tmData+tmSize+nTmCrData)=sqrt(fabs(SD-Mean*Mean))*Amp;// tmCourse of SD
			*(tmData+tmSize*2+nTmCrData)=Count/((*(SpecBase+lastN))-(*(SpecBase+firstN))); //tmCourse of frequency
			nTmCrData++;
		}
	}
	return nTmCrData;
}  
/*****************************************************************************/
int TSpectrum::CreatePowerSpec(float* tmBase, float* tmData, int tmSize)         
{
	/* Old Version
	int kMax,k,j,jSel;
	float T,dt,Wk,S,S2;
	float *crX;
	float *crY;
	//finding min interval
	T= *(SpecBase+NSPoint-1)-(*SpecBase);
	crX=SpecBase;
	dt=*(crX+1)-(*crX);
	for(j=2;j<NSPoint;j++)
		if( dt>(*(crX+j)-(*(crX+j-1))) ) dt=*(crX+j)-(*(crX+j-1));
	kMax=1+T/dt;
	//kMax=2*NSPoint;
	if(kMax>tmSize) kMax=tmSize;
	
	jSel=FindSelComp();
	*(SpecComp+jSel)*=(SpecParam+jSel)->Amp;
	*(SpecComp+jSel)+=(SpecParam+jSel)->Zero;
	crY=(SpecComp+jSel)->GetData();
	//frequencies
	for(k=0;k<kMax;k++) *(tmBase+k)=(k+0.001)/T;
	//Phourier coefficients
	*tmData=0;
	for(k=0;k<kMax;k++)
	{
		S=0;
		Wk=6.28318*(*(tmBase+k));
		for(j=0;j<NSPoint;j++)
		{
			S2=(*(crY+j))*cos(Wk*(*(crX+j)));
			S+=S2*S2;
		}
		//S/=(3.14159*NSPoint);
		*(tmData+k)=T*S;
	}
	*(SpecComp+jSel)-=(SpecParam+jSel)->Zero;
	*(SpecComp+jSel)/=(SpecParam+jSel)->Amp;
	return kMax;
	*/
	//new version
#define NFMax 131072
	int j, jSel, NData;
	float* fftData;//[NFMax];
	float* crY;
	float re, im, f;
	float dt= *(SpecBase+1)- *SpecBase;
	NData=2;
	if (NSPoint>tmSize)
		fftData=new float [2*NSPoint+1];
	else
		fftData=new float [2*tmSize+1];
	//while (NData<NSPoint && NData<=NFMax/4 && NData<=tmSize) NData*=2;
	while (NData<NSPoint && NData<=tmSize) NData*=2;
	sprintf(szNum,"N=%i  dt=%f",NData,dt);
	AfxMessageBox(szNum);
	// puting data in fftData array, actual size = 2XNDATA
	jSel=FindSelComp();
	*(SpecComp+jSel)*=(SpecParam+jSel)->Amp;
	*(SpecComp+jSel)+=(SpecParam+jSel)->Zero;
	crY=(SpecComp+jSel)->GetData();	
	for (j=0;j<NSPoint;j++)
	{
		fftData[2*j]= *(crY+j);
		fftData[2*j+1]=0;
	}
	for (j=NSPoint;j<NData;j++)
	{
		fftData[2*j]=0;
		fftData[2*j+1]=0;
	}
	//FourTransf((SpecComp+jSel)->GetData(), tmData, tmData+NSPoint, NSPoint);
	FFourT(fftData, 2*NData, 1);
	*(SpecComp+jSel)-=(SpecParam+jSel)->Zero;
	*(SpecComp+jSel)/=(SpecParam+jSel)->Amp;
	// testing for filtering
	for (j=0;j<NData;j++)
	{
		//re=fftData[2*j]*dt;
		//im=fftData[2*j+1]*dt;
		if (j>NData/8)
		{
			re=fftData[2*j]*0.01;//(1.0/(j-NData/8.01));
			im=fftData[2*j+1]*0.01;//(1.0/(j-NData/8.01));
			fftData[2*j]=re;
			fftData[2*j+1]=im;
		}
	}

	FFourT(fftData, 2*NData, -1);
	//NData/=2;
	//NData=NSPoint;
	for (j=0;j<NSPoint;j++)
	{
		re=fftData[2*j]*dt;
		im=fftData[2*j+1]*dt;
		*(tmBase+j)=*(SpecBase+j);
		*(tmData+j)=re/(dt*NData);
		*(tmData+NData+j)=re;
		*(tmData+2*NData+j)=im;
	}
	NData=NSPoint;



	/* right version
	// transfer results: positive frerquensies only
	NData/=2;
	for (j=0;j<NData;j++)
	{
		re=fftData[2*j]*dt;
		im=fftData[2*j+1]*dt;
		*(tmBase+j)=((float)j)/(2*dt*NData);
		*(tmData+j)=sqrt(re*re+im*im);
		*(tmData+NData+j)=re;
		*(tmData+2*NData+j)=im;
	}
	*/
	delete [] fftData;
	return NData;
	//


}
/*********************************************************/
float TSpectrum::AutoCorr(float* acBase, float* acData, float* Period, float* sdPer)
{
	int i,j,k,kMax,Count;
	float S1,S2,Delta,Per,Per2,acMax,acScore;
	float* crData;
	float AcExtY[9];
	float AcExtX[9];
	TComponent* ACcomp;
	//creating the autocorrelation fraction
	//subtracting the backgound
	j=FindSelComp();
	if(!j)j=1;
	*(SpecComp)=(SpecComp+j);
	(SpecComp)->SmoothCut(0,NSPoint,0.01);
	*(SpecComp)-=(SpecComp+j);
	*(SpecComp)*=-1.0;
	*(SpecComp) *=(SpecParam+j)->Amp;
	*(SpecComp) +=(SpecParam+j)->Zero;
	(SpecParam)->Zero=(SpecComp)->MinVal();
	(SpecParam)->Amp=(SpecComp)->MaxVal()-(SpecParam)->Zero;
	//calculation the autocorrelation
    crData=(SpecComp)->GetData();
    Delta= (*(SpecBase+NSPoint-1)-(*(SpecBase)))/NSPoint;
	for (j=0;j<NSPoint;j++)
	{
        S1=0; S2=0;
        for (i=0;i<NSPoint-j;i++)
		{
            k=i+j;
            if (k>NSPoint-1)k=NSPoint-1;
            S1+=(*(crData+i))*(*(crData+k));
            S2+=(*(crData+i))*(*(crData+i));
		}
        if (S2<MinFloat) S2=MinFloat;
        *(acBase+j)=j*Delta;
        //*(acData+j)=S1/S2;
        *(acData+j)=S1*NSPoint*NSPoint;

	}
	acMax= *(acData);
	//*(acBase+NSPoint-1)=NSPoint-1;
    //*(acData+j)=0;
    *(SpecComp)-=(SpecParam)->Zero;
    *(SpecComp)/=(SpecParam)->Amp;
	//calculatiog the AC-score
	if (Period!=NULL)
	{
		ACcomp=new TComponent(acData,acBase,NSPoint-1);
		for (k=0;k<9;k++)	AcExtY[k]=0;
		ACcomp->ReSpline();
		kMax=ACcomp->ExtremSearch(5*Delta,0.02*acMax,AcExtX,AcExtY,9);
		acScore=fabs(AcExtY[2]-AcExtY[1]);
		//AcScore+=fabs(AcExtY[2]-AcExtY[1]);
		//AcScore+=fabs(AcExtY[3]-AcExtY[2]);
		//AcScore/=2;
		Per=0; Count=0;   Per2=0;
		for (k=2;k<kMax;k+=2)
		{
			Per+=2*AcExtX[k]/k;
			Per2+=(2*AcExtX[k]/k)*(2*AcExtX[k]/k);
			Count++;
		}
		Per/=Count;
		Per2/=Count;
		Per2=sqrt(fabs(Per2-Per*Per));
		Per2/=Per;
		acScore=0;
		for (k=2;k<kMax;k+=2)
		{
			 acScore+=ACcomp->OutSpline(k*Per/2);
			 acScore-=ACcomp->OutSpline((k-1)*Per/2);
		}
		acScore*=(1-Per2);
		*Period=Per;
		*sdPer=Per2;
		
		delete ACcomp;
		return acScore;
		
		
	}
	else
	{
		return 0;
	}

}
/*******************************************************************/
float  TSpectrum::ScanReport(char* ScanStr, int StartPoint)
{
	int	j,jSel,Count,N1,N2;
	float	Mean, S1,S2;
	float* crData;
	float* Extrem;
	Extrem=new float[NDataPointMax];
   	if (IsSelected)
      	{
      		N1=BegSelPoint;
			N2=EndSelPoint;
        }
      else
      	{N1=0; N2=NSPoint-1;}
      Count=0;
      Mean=0;
      jSel=FindSelComp();
      (SpecParam+jSel)->ExtMin=(SpecComp+jSel)->MinVal(StartPoint+10);
      (SpecParam+jSel)->ExtMax=(SpecComp+jSel)->MaxVal(StartPoint+10);
      (SpecParam+jSel)->ExtMin*=(SpecParam+jSel)->Amp;
      (SpecParam+jSel)->ExtMin+=(SpecParam+jSel)->Zero;
      (SpecParam+jSel)->ExtMax*=(SpecParam+jSel)->Amp;
      (SpecParam+jSel)->ExtMax+=(SpecParam+jSel)->Zero;
      crData=(SpecComp+jSel)->GetData();
      for (j=N1;j<=N2;j++)
      	{
         (*(Extrem+Count))=(*(crData+j));
         (*(Extrem+Count))*=((SpecParam+jSel))->Amp;	(*(Extrem+Count))+=((SpecParam+jSel))->Zero;
         Mean+=(*(Extrem+Count));
         Count++;
         }

	Mean=Mean/Count;
  	MeanSqrErr=0;   StDev=0;
	for (j=0;j<Count;j++)
	{
   		S1=(*(Extrem+j)-Mean);
		MeanSqrErr+=S1*S1;
		S2=*(Extrem+j);
		StDev+=S2*S2;
	}
	S1=MeanSqrErr;
	S2=StDev;
	MeanSqrErr=sqrt(S1/Count);
	StDev=sqrt(fabs(S2/Count-Mean*Mean));
   //MeanSqrErr=Mean/100;
   //StDev=Mean/10;
	delete Extrem;
	sprintf (ScanStr,"Comp%i    Min: %5.2f, Max: %5.2f, Mean: %5.2f, SD: %5.2f",jSel,(SpecParam+jSel)->ExtMin,(SpecParam+jSel)->ExtMax, Mean, StDev);
return Mean;
}
/*******************************************************************/
float  TSpectrum::StatReport(int StartPoint)
{
	int	j,Count,N1,N2;
   float	Mean, S1,S2;
   float* crData;
   float* Extrem;
   Extrem=new float[NDataPointMax];
   if ((NSComp>2)&&StartPoint)
   	{
      Count=0;
      Mean=0;
      NoiseSqr=0;
      for (j=1;j<NSComp;j++)
      	{
      	*(Extrem+Count)=(SpecComp+j)->MinVal(StartPoint);
      	(*(Extrem+Count))*=((SpecParam+j))->Amp;	(*(Extrem+Count))+=((SpecParam+j))->Zero;
         Mean+=(*(Extrem+Count));
         Count++;
         NoiseSqr+=((SpecComp+j)->NoiseVal(StartPoint))*((SpecParam+j)->Amp);
         }
      NoiseSqr/=Count;
      }
	else
   	{
      if (IsSelected)
      	{
      		N1=BegSelPoint;
			N2=EndSelPoint;
         }
      else
      	{N1=0; N2=NSPoint-1;}
      Count=0;
      Mean=0;
      j=FindSelComp();
      crData=(SpecComp+j)->GetData();
      for (j=N1;j<=N2;j++)
      	{
         (*(Extrem+Count))=(*(crData+j));
         (*(Extrem+Count))*=((SpecParam+1))->Amp;	(*(Extrem+Count))+=((SpecParam+1))->Zero;
         Mean+=(*(Extrem+Count));
         Count++;
         }
      }
   Mean=Mean/Count;
  	MeanSqrErr=0;   StDev=0;
   for (j=0;j<Count;j++)
      {
   	S1=(*(Extrem+j)-Mean);
      MeanSqrErr+=S1*S1;
      S2=*(Extrem+j);
      StDev+=S2*S2;
      }
   S1=MeanSqrErr;
   S2=StDev;
   MeanSqrErr=sqrt(S1/Count);
   StDev=sqrt(fabs(S2/Count-Mean*Mean));
   //MeanSqrErr=Mean/100;
   //StDev=Mean/10;
   delete Extrem;
return Mean;
}

/*******************************************************************/
void  TSpectrum::Normalize(int StartPoint)
{
int j;
float K;
float* Extrems;
Extrems=new float[NSComp];
if (!Normalized)
   {
		for (j=1;j<NSComp;j++)
		{
			*(Extrems+j)=(SpecComp+j)->MinVal(StartPoint);
			(*(Extrems+j))*=((SpecParam+j))->Amp;
			(*(Extrems+j))+=((SpecParam+j))->Zero;
			//*(((SpecComp+j)))*=((SpecParam+j))->Amp;
			//*(((SpecComp+j)))+=((SpecParam+j))->Zero;
		}
		for (j=2;j<NSComp;j++)
		{
			K=(*(Extrems+j))/(*(Extrems+1));
			//*(((SpecComp+j)))/=K;
			(SpecParam+j)->NormCf=K;
			(SpecParam+j)->Zero/=K;//(SpecComp+j)->MinVal();
			(SpecParam+j)->Amp/=K;//(SpecComp+j)->MaxVal()-(SpecParam+j)->Zero;
		}
   /*
	for (j=1;j<NSComp;j++)
		{
		*(((SpecComp+j)))-=((SpecParam+j))->Zero;
		*(((SpecComp+j)))/=((SpecParam+j))->Amp;
	   }
    */
		Normalized=1;
	}
}
/*******************************************************************/
void  TSpectrum::DeNormalize()
{
	int j;
   /*
	for (j=2;j<NSComp;j++)
		{
		*(((SpecComp+j)))*=((SpecParam+j))->Amp;
		*(((SpecComp+j)))+=((SpecParam+j))->Zero;
		}
   */
	for (j=2;j<NSComp;j++)
		{
		//*(((SpecComp+j)))*=(SpecParam+j)->NormCf;
		(SpecParam+j)->Zero*=(SpecParam+j)->NormCf;//(SpecComp+j)->MinVal();
		(SpecParam+j)->Amp*=(SpecParam+j)->NormCf;//(SpecComp+j)->MaxVal()-(SpecParam+j)->Zero;
      (SpecParam+j)->NormCf=1.0;
		}
   /*
	for (j=2;j<NSComp;j++)
		{
		*(((SpecComp+j)))-=((SpecParam+j))->Zero;
		*(((SpecComp+j)))/=((SpecParam+j))->Amp;
	   }
    */  
   Normalized=0;
}

/*******************************************************************/
void  TSpectrum::Correct(int CurveN, int PointN)
{
}
/*******************************************************************/
void  TSpectrum::ReLine2(float far* OldMark, float far* NewMark, int MarkN)
{
	int j;
	float X;
	BcgrComp->Set(NewMark,OldMark,MarkN);
	BcgrComp->ReSpline();
	for (j=0;j<NSPoint;j++)
		{
		X= *(SpecBase+j);
		*(SpecBase+j)=BcgrComp->OutSpline(X);
      }
	for (j=0;j<NSComp;j++)
			((SpecComp+j))->ReSpline();
	ResetAxis();
}
/*******************************************************************/
void  TSpectrum::ReLine(float far* OldMark, float far* NewMark, int MarkN)
{
	int j,k,m;
	double XX;
	double A[20];
	double B[400];
	double C[20];
	double Mult;
	if (MarkN > 20) MarkN=20;
	for (j=0;j<MarkN;j++)
		*(C+j)=(double)(*(NewMark+j));
	for (j=0;j<MarkN;j++)
		{
		XX= (*(OldMark+j));
		for (k=0;k<MarkN;k++)
			{
			Mult=1;
			(*(B+MarkN*j+k))=0;
			for (m=0;m<k;m++)
				{
				Mult*=XX;
				}
			(*(B+MarkN*j+k))+=Mult;
			}
		}
	if ( EquSet2(A,B,C,MarkN) )
		{
		for (j=0;j<NSPoint;j++)
			{
			XX=(*(SpecBase+j));
			(*(SpecBase+j))=0;
			Mult=1;
			for (k=0;k<MarkN;k++)
				{
				(*(SpecBase+j))+=(float)(Mult*(*(A+k)));
				Mult*=XX;
				}
			}
		for (j=0;j<NSComp;j++)
			((SpecComp+j))->ReSpline();
		}
	
	X_Axis->Beg= *SpecBase;
	X_Axis->End= *(SpecBase+NSPoint-1);
	ResetAxis();
	//Y_Axis->Lock=X_Axis->BegMark;
	//free(A);	free(B);	free(C);
}
/***************************************************************/
void TSpectrum::InvertColor(void)
{
	BYTE r,g,b,j;
	r=GetRValue(SpecBrush.lbColor);
	g=GetGValue(SpecBrush.lbColor);
	b=GetBValue(SpecBrush.lbColor);
	SpecBrush.lbColor=RGB(255-r,255-g,255-b);
	for (j=0;j<16;j++)
		{
		r=GetRValue(SpecPen[j].lopnColor);
		g=GetGValue(SpecPen[j].lopnColor);
		b=GetBValue(SpecPen[j].lopnColor);
		SpecPen[j].lopnColor=RGB(255-r,255-g,255-b);
		}
}
/***************************************************************/
void TSpectrum::Smooth(int nCurve,float Fraction)
{
	
	*(SpecComp)=(SpecComp+nCurve);
	(SpecComp)->SmoothCut(BegSelect,EndSelect,Fraction);
	*(SpecComp) *=(SpecParam+nCurve)->Amp;
	*(SpecComp) +=(SpecParam+nCurve)->Zero;
	(SpecParam)->Zero=(SpecComp)->MinVal();
	(SpecParam)->Amp=(SpecComp)->MaxVal()-(SpecParam)->Zero;
	*(SpecComp) -=(SpecParam)->Zero;
	*(SpecComp) /=(SpecParam)->Amp;

}
/***************************************************************/
void TSpectrum::Monochrome(void)
{
	BYTE j;
	SpecBrush.lbColor=RGB(255,255,255);
	for (j=0;j<16;j++)
		SpecPen[j].lopnColor=RGB(0,0,0);
	SpecPen[1].lopnColor=RGB(127,127,127);
		
}
/*******************************************************************/
void  TSpectrum::SelChange(float Sel1, float Sel2, int IsSel)
{
	if (Sel1>Sel2)
	{
		EndSelect=Sel1;
		BegSelect=Sel2;
	}
		else
		{
			EndSelect=Sel2;
			BegSelect=Sel1;
		}
	//BegSelPoint=NSPoint*(BegSelect-(*SpecBase))/(*(SpecBase+NSPoint-1)-(*SpecBase));
	BegSelPoint=SpecComp->FindPoint(BegSelect)-1;
	if (BegSelPoint<0)BegSelPoint=0;
	if (BegSelPoint>NSPoint-1)BegSelPoint=NSPoint-1;
	//EndSelPoint=NSPoint*(EndSelect-(*SpecBase))/(*(SpecBase+NSPoint-1)-(*SpecBase));
	EndSelPoint=SpecComp->FindPoint(EndSelect)+1;
	if (EndSelPoint<0)EndSelPoint=0;
	if (EndSelPoint>NSPoint-1)EndSelPoint=NSPoint-1;
	IsSelected=IsSel;
}
/********************************************************************/
int TSpectrum::SelComp(int DestNum,int DeSelectMode)
{
	int j;
	if (DeSelectMode)
		for (j=0;j<NSComp;j++)
			(SpecParam+j)->Select=NONE;		
	if (DestNum<=0) j = 0;
	else
		if (DestNum >=NSComp) j = NSComp-1;
		else
			j = DestNum;
	(SpecParam+j)->Select=NONE+1;
	return j;
}
/********************************************************************/
int TSpectrum::FindSelComp(int LastNum)
{
	int j,Count,jSel,jStop;
	Count=0;
	jSel=1;
	if(LastNum)
	{
		if(LastNum<=NSComp)
			jStop=LastNum;
		else jStop=NSComp;
	}
	else
		jStop=NSComp;
	for (j=0;j<jStop;j++)
   	{
   		if ((SpecParam+j)->Select!=NONE)
		{
    //if (Count==0)
			jSel=j;
			Count++;
		}
	}
	if ((jSel==0)&&(Count>1))jSel=1;
	return (jSel);
}
/********************************************************************/
void TSpectrum::SelCompIndex(int* DestNum)
{
	int j;
	for (j=0;j<NSComp;j++)
		(SpecParam+j)->Select=NONE;
	j=0;
	while ( (*(DestNum+j)!=-1)&&(j<NSComp) )
	{
		(SpecParam+(*(DestNum+j)))->Select=NONE+1;
		j++;
	}
}
/******************************************************************/
int TSpectrum::CopySelPoint(float far* BufX, float far* BufY, int SelFlag)
{
 	int j,N1,N2,jSel;
	float Zero, Amp;
	float far* Data;
	if (IsSelected) {N1=BegSelPoint; N2=EndSelPoint;}
	else			{N1=0;N2=NSPoint-1;}
	if (SelFlag>=0)
		jSel = SelFlag;
	else
		jSel=FindSelComp();
	Data=(SpecComp+jSel)->GetData();
	Zero=(SpecParam+jSel)->Zero;
	Amp=(SpecParam+jSel)->Amp;
	if (N2>N1+1)
   	{
		for (j=0;j<N2-N1+1;j++)
      	{
			*(BufX+j)= *(SpecBase+j+N1);
			*(BufY+j)= Zero+(*(Data+j+N1))*Amp;
        }
		return N2-N1+1;
     }
	else
   	return 0;
}
/*******************************************************************/
void TSpectrum::CopySelComp(TSpectrum* DestSpec, int SimpleCopy, int IgnoreSelect)
{
	int j,CopyMode;
	if (!SimpleCopy)
		CopyMode=(DestSpec->NSPoint==NSPoint);
	else CopyMode=1;
	if (DestSpec!=NULL)
	{
		for (j=0;j<NSComp;j++)
			{
			if (( (SpecParam+j)->Select!=NONE )||(IgnoreSelect&&j))
				{
				*(SpecComp+j)*=(SpecParam+j)->Amp;
				*(SpecComp+j)+=(SpecParam+j)->Zero;
				DestSpec->NewComp(SpecBase,(SpecComp+j)->GetData(),NSPoint,CopyMode);
				*(SpecComp+j)-=(SpecParam+j)->Zero;
				*(SpecComp+j)/=(SpecParam+j)->Amp;
				(DestSpec->SpecParam+DestSpec->NSComp-1)->Paste(SpecParam+j);
				(DestSpec->SpecParam+DestSpec->NSComp-1)->Select=NONE;
				}
			}
		j=0;
		do
		{	j++; }
		while ( ((DestSpec->SpecParam+j)->Mode==FILE)&&(j<DestSpec->NSComp));
		if ((DestSpec->SpecParam+j)->Mode!=FILE) DestSpec->BegFitComp=j;
	}
}	
/*******************************************************************/
int TSpectrum::PasteSelComp(TSpectrum* DestSpec,int SimpleCopy, int IgnoreSelect)
{
	int j,CopyMode;
	int ContrSum=0;
	if (!SimpleCopy)
		CopyMode=(DestSpec->NSPoint==NSPoint)&&(*(DestSpec->SpecBase+DestSpec->NSPoint-1)==(*SpecBase+NSPoint-1));
	else
		CopyMode=1;
	if (DestSpec!=NULL)
	{
		ADCTime=DestSpec->ADCTime;
		strcpy(TimeStr,DestSpec->TimeStr);
		strcpy(CommentStr,DestSpec->CommentStr);
		for (j=0;j<18;j++)
				*(Param+j)= *(DestSpec->Param+j);
		for (j=1;j<DestSpec->NSComp;j++)
		{
			//if (( (DestSpec->SpecParam+j)->Select!=NONE )||IgnoreSelect)
			{
				*(DestSpec->SpecComp+j)*=(DestSpec->SpecParam+j)->Amp;
				*(DestSpec->SpecComp+j)+=(DestSpec->SpecParam+j)->Zero;
				ContrSum+=NewComp(DestSpec->SpecBase,(DestSpec->SpecComp+j)->GetData(),DestSpec->NSPoint,CopyMode);
				*(DestSpec->SpecComp+j)-=(DestSpec->SpecParam+j)->Zero;
				*(DestSpec->SpecComp+j)/=(DestSpec->SpecParam+j)->Amp;
			//(SpecParam+NSComp-1)->Time=(DestSpec->SpecParam+j)->Time;
			//(SpecParam+j)->ExtMin=(DestSpec->SpecParam+j)->ExtMin;
			//(SpecParam+j)->ExtMax=(DestSpec->SpecParam+j)->ExtMax;
			(SpecParam+NSComp-1)->Paste(DestSpec->SpecParam+j);
			if (j>8) (SpecParam+NSComp-1)->Select=NONE;
			
			}
		
		}
		j=0;
		do
		{	j++; }
		while ( ((SpecParam+j)->Mode==FILE)&&(j<NSComp));
		if ((SpecParam+j)->Mode!=FILE) BegFitComp=j;
	
	}
	return ContrSum;
}


/*******************************************************************/
void  TSpectrum::SetSelNum(int Dir, int PgSize, int InitNum)
{
	int j;
	if (InitNum < 0 )
	{
		//deselection
		for (j=0;j<SelPageSize;j++)
			(SpecParam+SelNum+j)->Select=NONE;
		if (PgSize) SelPageSize=PgSize;
		SelNum+=Dir*SelPageSize;
		if (SelNum<0)SelNum=0;
		if (SelNum > NSComp-SelPageSize)SelNum=NSComp-SelPageSize;
		for(j=SelNum;j<SelNum+SelPageSize;j++)
			(SpecParam+j)->Select=NONE+1;
	}
	else 
	{
	// select the page beginig from InitNum
	for (j=0;j<NSComp;j++)
			(SpecParam+j)->Select=NONE;
	if (PgSize) SelPageSize=PgSize;
	SelNum = InitNum;
	for(j=SelNum;j<SelNum+SelPageSize;j++)
			(SpecParam+j)->Select=NONE+1;
	}

}
/*******************************************************************/			
void TSpectrum::GetSelStr(char* DestStr)
{
	int	j,EndNum,BegNum;
	char NumStr[16];
	*DestStr='\0';
	j=0;
	while (j<NSComp)
		{
		if ((SpecParam+j)->Select!=NONE) 
			{
			BegNum=j;
			EndNum=j+1;
			while ((j<NSComp)&&((SpecParam+EndNum)->Select!=NONE)) 
				{
				if (j>NSComp-1) EndNum=NSComp-1;
					else 	EndNum=j;
				j++;	
				}
			if (EndNum-BegNum>1)	
				sprintf(DestStr,"%i-%i;",BegNum,EndNum);
			else	
				sprintf(DestStr,"%i;",BegNum);
			}
		j++;	
		}
}
	
/*******************************************************************/			
int TSpectrum::SetSelStr(char* SelStr)
{
	char 	NumStr[16];
	char  DestStr[64];
	//char*	Separat=";, :-=_";
	//char*	Separat2="-=_";
	int 	BegNum,EndNum,j,SelectNum,ReadFlag,SelMode;
	int jNum=0;
	int jDest=0;
	strcpy(DestStr,SelStr);
	DestStr[63]=';';	
	SelectNum=0;
	//deselect old
	
	for (j=0;j<NSComp;j++)
		(SpecParam+j)->Select=NONE;		
	ReadFlag=0; SelMode=0;
	do
		{
		while ( (!ReadFlag)&&(DestStr[jDest]!='\0') )
			{
			switch (DestStr[jDest])
				{
				case '0':case '1':case '2':case '3': 
				case '4':case '5':case '6':case '7':
				case '8':case '9': NumStr[jNum]= DestStr[jDest]; jNum++;break;
				case '-': ReadFlag=1;break;
				case ',':case':':case';':case '\0':ReadFlag=2;break;
				}
			jDest++;	
			}	
		NumStr[jNum]='\0';
		if (ReadFlag==1)
			{
			SelMode=1;
			BegNum=atoi(NumStr);
			}
		if (ReadFlag==2)
			{
			if (SelMode)
				{
				EndNum=atoi(NumStr);
				if (BegNum<NSComp) SelNum=BegNum;
				for(j=BegNum;(j<=EndNum)&&(j<NSComp);j++)
					{(SpecParam+j)->Select=NONE+1; SelectNum++;}
				SelMode=0;
				}
				else
					{
					BegNum=atoi(NumStr);
					if (BegNum<NSComp) 
						{(SpecParam+BegNum)->Select=NONE+1; SelectNum++;}
					}	
			 }
		jNum=0;
		ReadFlag=0;
		}
	while(DestStr[jDest]!='\0');		 
	/*?      
	NumStr=strtok(DestStr,Separat);
	while(NumStr!=NULL)
		{
		BegNum=atoi(NumStr);
		if (BegNum<NSComp)
			{ (SpecParam+BegNum)->Select=NONE+1;SelectNum++;}
		NumStr=strtok(NULL,Separat);
		}
	/*select new
	while(NumStr!=NULL)
		{
		BegNum=atoi(NumStr);
		if (BegNum<NSComp)
			{ (SpecParam+BegNum)->Select=NONE+1;}
		NumStr=strtok(NULL,Separat);
		if (NumStr!=NULL)
			{
			if ((*(NumStr-1)=='-'))
				{
				j=atoi(NumStr);
				if (j>=BegNum)
					EndNum=j;
				else
					{
					EndNum=BegNum;
					BegNum=j;
					}
				if (BegNum<NSComp)
					for(j=BegNum;(j<=EndNum)&&(j<NSComp);j++)
						{(SpecParam+j)->Select=NONE+1; }
				}
			NumStr=strtok(NULL,Separat);
			}
		}
		*/
	return SelectNum;
}
/*******************************************************************
void  TSpectrum::ReDraw(RECT* OutRect, const char* FileName, HDC refDC)
{
   const float sq=0.70711;
   const float A=0.75;
	char crStr[32];
	HDC hDC;
   int penN,sigN,j,dtStyle;
	int Xe,Ye;
   float Tic1Beg,Tic2Beg,EXe,EYe,Ex,Ey,L,D;
	POINT crPoint;
	float TicSizeY=(0.02*(OutRect->right-OutRect->left));
	float TicSizeX=(0.02*(OutRect->bottom-OutRect->top));
	float ScaleX,ScaleY,cr_x,StepX,StepY;
	float MultX=X_Axis->Factor;
	float MultY=Y_Axis->Factor;
	RECT CompRect,EnhRect;
	RECT rText;
	HPEN OldPen,crPen;
	HBRUSH OldBrush,crBrush;
   HFONT  OldFont,crFont;
   HENHMETAFILE hMF;

   EnhRect.top=0;	EnhRect.bottom=15000;	EnhRect.left=0;	EnhRect.right=30000;
   hDC=CreateEnhMetaFile(refDC,FileName,&EnhRect,NULL);
	SetMapMode(hDC, MM_ANISOTROPIC);
	SetWindowExtEx(hDC,1000,500,NULL);
	SetViewportExtEx(hDC,OutRect->right,OutRect->bottom,NULL);
	OutRect->left=0;	OutRect->right=1000;	OutRect->top=0;	OutRect->bottom=500;


   hMF=CloseEnhMetaFile(hDC);
   PlayEnhMetaFile(hDC,hMF,&EnhRect);
   PlayEnhMetaFile(refDC,hMF,&EnhRect);
   DeleteEnhMetaFile(hMF);
}
*/
/*******************************************************************/
void  TSpectrum::ReDraw(RECT* OutRect, HDC hDC)
{
	const float sq=0.70711;
	const float A=0.75;
	char crStr[256];
	//hDC=HDC(SpDC);
	int penN,sigN,j,dtStyle,jSel;
	int Xe,Ye;
	float Tic1Beg,Tic2Beg,EXe,EYe,Ex,Ey,L,D;
	POINT crPoint;
	POINT TriPoint[3];
	float TicSizeY=(0.02*(OutRect->right-OutRect->left));
	float TicSizeX=(0.02*(OutRect->bottom-OutRect->top));
	float ScaleX,ScaleY,cr_x,StepX,StepY;
	float MultX=X_Axis->Factor;
	float MultY=Y_Axis->Factor;
	RECT CompRect;
	RECT rText;
	HPEN OldPen,crPen;
	HBRUSH OldBrush,crBrush;
	HFONT  OldFont,crFont;
	crBrush=CreateBrushIndirect(&SpecBrush);
	OldBrush=(HBRUSH)SelectObject(hDC,crBrush);
	FillRect(hDC,OutRect,crBrush);
	//Rectangle(hDC,OutRect->left,OutRect->top,OutRect->right,OutRect->bottom);
	OutRect->left+=(int)(MarginL*(OutRect->right-OutRect->left));
	OutRect->right-=(int)(MarginR*(OutRect->right-OutRect->left));
	OutRect->bottom-=(int)(MarginB*(OutRect->bottom-OutRect->top));
	OutRect->top+=(int)(MarginT*(OutRect->bottom-OutRect->top));
	if (!NSComp)
	{
	rText.top=300; rText.bottom=400; rText.left = 200;	rText.right = 800;
	sprintf(crStr, "There are no data, bytes remaining = %i", BytesRem);
	DrawText(hDC,(LPSTR)crStr,strlen(crStr),&rText, DT_SINGLELINE | DT_BOTTOM | DT_CENTER);
	}
	else
	{
	//if (X_Axis->ReScaleFlag!=NONE)
	//	X_Axis->SetRoundScale();
	//if (Y_Axis->ReScaleFlag!=NONE)
	//	Y_Axis->SetRoundScale();

	ScaleY=(OutRect->bottom-OutRect->top)/(Y_Axis->End-Y_Axis->Beg);
	ScaleX=(OutRect->right-OutRect->left)/(X_Axis->End-X_Axis->Beg);
	StepX=X_Axis->Step;
	StepY=Y_Axis->Step;
	crPen=CreatePenIndirect(SpecPen);
	OldPen=(HPEN)SelectObject(hDC,crPen);
	if (SpecFont[0].lfHeight!=0)
   	{
      crFont=CreateFontIndirect(SpecFont);
      OldFont=(HFONT)SelectObject(hDC,crFont);
      }

	// drawing the X - axis
	if ( X_Axis->Style !=NONE)
		{
		//if (Y_Axis->ReScaleFlag!=NONE)
   			//X_Axis->Lock=Y_Axis->BegMark;
		//X_Axis->Lock=Y_Axis->Beg;
  		crPoint.y=OutRect->bottom-(X_Axis->Lock-Y_Axis->Beg)*ScaleY;
		//SpDC->MoveTo(OutRect->left,crPoint.y);
		//SpDC->LineTo(OutRect->right,crPoint.y);
		if(fabs(StepX/MultX) >= 2.000) sigN=0;
			else
				if (fabs(StepX/MultX) >=0.1) sigN=1;
					else
						if (fabs(StepX/MultX)>=0.001) sigN=2;
							else sigN=3;
		cr_x=X_Axis->BegMark;
		//cr_x=X_Axis->Beg;
		crPoint.x=(int)(OutRect->left+(cr_x-X_Axis->Beg)*ScaleX);
		MoveToEx(hDC,crPoint.x,crPoint.y,NULL);
		if (X_Axis->Style!=NONE+4)
      		LineTo(hDC,(int)(OutRect->left+(X_Axis->End-X_Axis->Beg)*ScaleX),crPoint.y);
		else
      		LineTo(hDC,crPoint.x+X_Axis->NMark*StepX*ScaleX,crPoint.y);
		cr_x=X_Axis->BegMark;
		switch (X_Axis->Style)
			{
			case NONE+1:             // upper tics
            Tic1Beg=0;
				Tic2Beg=0;

				break;
			case NONE+2:               // lower tics
            Tic1Beg=-1.5*TicSizeX;
				Tic2Beg=-0.9*TicSizeX;
				break;
			case NONE+3:              // centered tics
				Tic1Beg=(-0.75*TicSizeX);
				Tic2Beg=(-0.45*TicSizeX);
				break;
			}
      if (X_Axis->Style!=NONE+4)
      while (cr_x<=X_Axis->End-0.5*StepX)
			{
			crPoint.x=(int)(OutRect->left+(cr_x-X_Axis->Beg)*ScaleX);
         MoveToEx(hDC,crPoint.x,(int)(crPoint.y+Tic1Beg),NULL);
			LineTo(hDC,crPoint.x,(int)(crPoint.y+1.5*TicSizeX+Tic1Beg));
         MoveToEx(hDC,crPoint.x+(int)(0.5*StepX*ScaleX),(int)(crPoint.y+Tic2Beg),NULL);
			LineTo(hDC,crPoint.x+(int)(0.5*StepX*ScaleX),(int)(crPoint.y+0.9*TicSizeX+Tic2Beg));
			cr_x+=StepX;
         }
      cr_x=X_Axis->BegMark;
	  //cr_x=X_Axis->Beg;
      if (X_Axis->Style!=NONE+4)
      for (j=0;j<X_Axis->NMark-0.5;j++)
			{
			crPoint.x=(int)(OutRect->left+(cr_x-X_Axis->Beg)*ScaleX);
			//MoveToEx(hDC,crPoint.x,(int)(crPoint.y+Tic1Beg),NULL);
			//LineTo(hDC,crPoint.x,(int)(crPoint.y+1.5*TicSizeX+Tic1Beg));
			switch (sigN)
				{
				case 0:sprintf((LPSTR)crStr,"%8.0f",(cr_x/MultX));break;
				case 1:sprintf((LPSTR)crStr,"%3.1f",(cr_x/MultX));break;
				case 2:sprintf((LPSTR)crStr,"%5.3f",(cr_x/MultX));break;
				case 3:sprintf((LPSTR)crStr,"%8.6f",(cr_x/MultX));break;
				}
         rText.left=(int)(crPoint.x-0.5*StepX*ScaleX);
			rText.right=(int)(crPoint.x+0.5*StepX*ScaleX);
			rText.top=(int)(crPoint.y+1.5*TicSizeX);
			rText.bottom=crPoint.y+10*TicSizeX;
         if (X_Axis->Style!=NONE+2)
				DrawText(hDC,(LPSTR)crStr,strlen(crStr),&rText, DT_SINGLELINE | DT_TOP | DT_CENTER);
         	else
            	{
               rText.top=(int)(crPoint.y-10*TicSizeX);
					rText.bottom=crPoint.y-1.6*TicSizeX;
               DrawText(hDC,(LPSTR)crStr,strlen(crStr),&rText, DT_SINGLELINE | DT_BOTTOM | DT_CENTER);
               }
			//MoveToEx(hDC,crPoint.x+(int)(0.5*StepX*ScaleX),(int)(crPoint.y+Tic2Beg),NULL);
			//LineTo(hDC,crPoint.x+(int)(0.5*StepX*ScaleX),(int)(crPoint.y+0.9*TicSizeX+Tic2Beg));
			cr_x+=StepX;
			}
		}
	// drawing the Y - axis
	if ( Y_Axis->Style !=NONE)
		{
		crPoint.x=OutRect->left+(Y_Axis->Lock-X_Axis->Beg)*ScaleX;
		//SpDC->MoveTo(crPoint.x,OutRect->bottom);
		//SpDC->LineTo(crPoint.x,OutRect->top);
		if(fabs(StepY/MultY) >= 2.000) sigN=0;
			else
				if (fabs(StepY/MultY) >=0.1) sigN=1;
					else
						if (fabs(StepY/MultY)>=0.001) sigN=2;
							else sigN=3;
		cr_x=Y_Axis->BegMark;
      crPoint.y=(int)(OutRect->bottom-(cr_x-Y_Axis->Beg)*ScaleY);
      MoveToEx(hDC,crPoint.x,crPoint.y,NULL);

      if (Y_Axis->Style!=NONE+4)
	      LineTo(hDC,crPoint.x,(int)(OutRect->bottom-(Y_Axis->End-Y_Axis->Beg)*ScaleY));
		else
	      LineTo(hDC,crPoint.x,crPoint.y-Y_Axis->NMark*StepY*ScaleY);
		switch (Y_Axis->Style)
			{
			case NONE+1:             // upper tics
            Tic1Beg=0;
				Tic2Beg=0;
				break;
			case NONE+2:               // lower tics
            Tic1Beg=1.5*TicSizeY;
				Tic2Beg=0.9*TicSizeY;

				break;
			case NONE+3:              // centered tics
				Tic1Beg=0.75*TicSizeY;
				Tic2Beg=0.45*TicSizeY;
				break;
			}
      if (Y_Axis->Style!=NONE+4)
		while (cr_x<=Y_Axis->End-0.5*StepY)
			{
			crPoint.y=(int)(OutRect->bottom-(cr_x-Y_Axis->Beg)*ScaleY);
         MoveToEx(hDC,(int)(crPoint.x+Tic1Beg),crPoint.y,NULL);
			LineTo(hDC,(int)(crPoint.x-1.5*TicSizeY+Tic1Beg),crPoint.y);
         MoveToEx(hDC,(int)(crPoint.x+Tic2Beg),crPoint.y-(int)(0.5*StepY*ScaleY),NULL);
			LineTo(hDC,(int)(crPoint.x-0.9*TicSizeY+Tic2Beg),crPoint.y-(int)(0.5*StepY*ScaleY));
			cr_x+=StepY;
         }
      cr_x=Y_Axis->BegMark;
      if (Y_Axis->Style!=NONE+4)
		for (j=0;j<Y_Axis->NMark-0.5;j++)
			{
			crPoint.y=(int)(OutRect->bottom-(cr_x-Y_Axis->Beg)*ScaleY);
			//MoveToEx(hDC,(int)(crPoint.x+Tic1Beg),crPoint.y,NULL);
			//LineTo(hDC,(int)(crPoint.x-1.5*TicSizeY+Tic1Beg),crPoint.y);
			rText.top=(int)(crPoint.y-0.5*StepY*ScaleY);
			rText.bottom=(int)(crPoint.y+0.5*StepY*ScaleY);
			rText.right=(int)(crPoint.x-1.8*TicSizeY);
			rText.left=0;
			switch (sigN)
				{
				case 0:sprintf((LPSTR)crStr,"%8.0f",(cr_x/MultY));break;
				case 1:sprintf((LPSTR)crStr,"%3.1f",(cr_x/MultY));break;
				case 2:sprintf((LPSTR)crStr,"%5.3f",(cr_x/MultY));break;
				case 3:sprintf((LPSTR)crStr,"%8.6f",(cr_x/MultY));break;
				}
         if (Y_Axis->Style!=NONE+2)
				DrawText(hDC,(LPSTR)crStr,strlen(crStr),&rText, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
         else
         	{
            rText.right=(int)(crPoint.x+11.8*TicSizeY);
            rText.left=(int)(crPoint.x+1.8*TicSizeY);
            DrawText(hDC,(LPSTR)crStr,strlen(crStr),&rText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
            }
			//MoveToEx(hDC,(int)(crPoint.x+Tic2Beg),crPoint.y-(int)(0.5*StepY*ScaleY),NULL);
			//LineTo(hDC,(int)(crPoint.x-0.9*TicSizeY+Tic2Beg),crPoint.y-(int)(0.5*StepY*ScaleY));
			cr_x+=StepY;
			}
		}


	//drawinig marker , labels and titles
   for (j=0;j<N_TITLE;j++)
		if ((Title+j)->Visible)
			{
			crPoint.x=(int)(OutRect->left+((Title+j)->X)*(OutRect->right-OutRect->left));
			crPoint.y=(int)(OutRect->top+((Title+j)->Y)*(OutRect->bottom-OutRect->top));
         if ((Title+j)->Style!=NONE)
         switch ((Title+j)->Style)
         	{
            case BAR_O:
            	Rectangle(hDC,crPoint.x,crPoint.y,crPoint.x+((Title+j)->SizeX)*(OutRect->right-OutRect->left),crPoint.y+((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
            	break;
            case BAR_F:
               crBrush=CreateSolidBrush(SpecPen->lopnColor);
					OldBrush=(HBRUSH)SelectObject(hDC,crBrush);
					Rectangle(hDC,crPoint.x,crPoint.y,crPoint.x+((Title+j)->SizeX)*(OutRect->right-OutRect->left),crPoint.y+((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               SelectObject(hDC,OldBrush);
               DeleteObject(crBrush);
            	break;
            case CIRCLE_O:
            	Ellipse(hDC,crPoint.x,crPoint.y,crPoint.x+((Title+j)->SizeX)*(OutRect->right-OutRect->left),crPoint.y+((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
            	break;
            case CIRCLE_F:
               crBrush=CreateSolidBrush(SpecPen->lopnColor);
				OldBrush=(HBRUSH)SelectObject(hDC,crBrush);
				Ellipse(hDC,crPoint.x,crPoint.y,crPoint.x+((Title+j)->SizeX)*(OutRect->right-OutRect->left),crPoint.y+((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
				SelectObject(hDC,OldBrush);
				DeleteObject(crBrush);
            	break;
            case TRI_O:
            	TriPoint[0].x=crPoint.x-0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left);
               TriPoint[0].y=crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top);
               TriPoint[1].x=crPoint.x;
               TriPoint[1].y=crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top);
               TriPoint[2].x=crPoint.x+0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left);
               TriPoint[2].y=crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top);
               Polygon(hDC,TriPoint,3);
            	break;
            case TRI_F:
               crBrush=CreateSolidBrush(SpecPen->lopnColor);
					OldBrush=(HBRUSH)SelectObject(hDC,crBrush);
               TriPoint[0].x=crPoint.x-0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left);
               TriPoint[0].y=crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top);
               TriPoint[1].x=crPoint.x;
               TriPoint[1].y=crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top);
               TriPoint[2].x=crPoint.x+0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left);
               TriPoint[2].y=crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top);
               Polygon(hDC,TriPoint,3);
               SelectObject(hDC,OldBrush);
               DeleteObject(crBrush);
            	break;
            case CROSS_R:
               MoveToEx(hDC,crPoint.x-0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),crPoint.y,NULL);
               LineTo(hDC,crPoint.x+0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),crPoint.y);
               MoveToEx(hDC,crPoint.x,crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top),NULL);
               LineTo(hDC,crPoint.x,crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               break;
            case CROSS_D:
               MoveToEx(hDC,crPoint.x-0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top),NULL);
               LineTo(hDC,crPoint.x+0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               MoveToEx(hDC,crPoint.x-0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top),NULL);
               LineTo(hDC,crPoint.x+0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               break;

            case CROSS_T:
               MoveToEx(hDC,crPoint.x,crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top),NULL);
               LineTo(hDC,crPoint.x,crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               MoveToEx(hDC,crPoint.x-0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top),NULL);
               LineTo(hDC,crPoint.x+0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y-0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               MoveToEx(hDC,crPoint.x-0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top),NULL);
               LineTo(hDC,crPoint.x+0.5*((Title+j)->SizeX)*(OutRect->right-OutRect->left),
                             crPoint.y+0.5*((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               break;

               /*
					HS_BDIAGONAL	45-degree downward left-to-right hatch
					HS_CROSS	Horizontal and vertical crosshatch
					HS_DIAGCROSS	45-degree crosshatch
					HS_FDIAGONAL	45-degree upward left-to-right hatch
					HS_HORIZONTAL	Horizontal hatch
					HS_VERTICAL	Vertical hatch
               */
            case BAR_H:
               crBrush=CreateHatchBrush(HS_FDIAGONAL,SpecPen->lopnColor);
					OldBrush=(HBRUSH)SelectObject(hDC,crBrush);
					Rectangle(hDC,crPoint.x,crPoint.y,crPoint.x+((Title+j)->SizeX)*(OutRect->right-OutRect->left),crPoint.y+((Title+j)->SizeY)*(OutRect->bottom-OutRect->top));
               SelectObject(hDC,OldBrush);
               DeleteObject(crBrush);
            	break;
            default:
            	L=(Title+j)->SizeX;
               D=0.5*(Title+j)->SizeY;
               MultX=(OutRect->right-OutRect->left);
               MultY=2*(OutRect->bottom-OutRect->top);
               switch ((Title+j)->Style)
         			{
                  case ARR_UL:EXe=-sq;EYe=-sq;Ex=sq;Ey=-sq;break;
                  case ARR_UR:EXe=sq;EYe=-sq;Ex=sq;Ey=sq;break;
                  case ARR_DL:EXe=-sq;EYe=sq;Ex=-sq;Ey=-sq;break;
                  case ARR_DR:EXe=sq;EYe=sq;Ex=-sq;Ey=sq;break;
                  case ARR_UP:EXe=0;EYe=-1;Ex=1;Ey=0;break;
                  case ARR_DN:EXe=0;EYe=1;Ex=1;Ey=0;break;
                  case ARR_LT:EXe=-1;EYe=0;Ex=0;Ey=1;break;
                  case ARR_RT:EXe=1;EYe=0;Ex=0;Ey=1;break;
                  }
               Xe=crPoint.x+EXe*L*MultX;
               Ye=crPoint.y+EYe*L*MultY;
               MoveToEx(hDC,crPoint.x,crPoint.y,NULL);
               LineTo(hDC,Xe,Ye);
               MoveToEx(hDC,Xe,Ye,NULL);
               LineTo(hDC,crPoint.x+EXe*A*L*MultX+Ex*D*MultX,crPoint.y+EYe*A*L*MultY+Ey*D*MultY);
               MoveToEx(hDC,Xe,Ye,NULL);
               LineTo(hDC,crPoint.x+EXe*A*L*MultX-Ex*D*MultX,crPoint.y+EYe*A*L*MultY-Ey*D*MultY);
            	break;

            }
         crPoint.x+=(int)(((Title+j)->TextX)*(OutRect->right-OutRect->left));
			crPoint.y-=(int)(((Title+j)->TextY)*(OutRect->bottom-OutRect->top));
         if (SpecFont[j].lfHeight!=0)
         	{
            crFont=CreateFontIndirect(SpecFont+j);
            OldFont=(HFONT)SelectObject(hDC,crFont);
            }
			TextOut(hDC,crPoint.x,crPoint.y,(LPSTR)((Title+j))->Text,strlen(((Title+j))->Text));
         if (SpecFont[j].lfHeight!=0)
         	{
            SelectObject(hDC,OldFont);
            DeleteObject(crFont);
            }
			}

	SelectObject(hDC,OldPen);
	DeleteObject(crPen);
	//drawing the selection markers
	if ( (IsSelected)&&(EndSelect!=BegSelect))
	{
		jSel=FindSelComp();
		//jSel=1;
      //if ((!jSel)&&((SpecParam+1)->Select!=NONE)) jSel=1;
		crPen=CreatePen(PS_DASHDOT,1,RGB(0,127,0));
		OldPen=(HPEN)SelectObject(hDC,crPen);
		crPoint.x=(int)(OutRect->left+ScaleX*(BegSelect - X_Axis->Beg));
		crPoint.y=(int)(OutRect->bottom-ScaleY*((SpecParam+jSel)->Zero+((SpecParam+jSel)->Amp)*(SpecComp+jSel)->OutSpline(BegSelect) - Y_Axis->Beg));
		MoveToEx(hDC,crPoint.x,crPoint.y-(int)(ScaleY*StepY),NULL);
		LineTo(hDC,crPoint.x,crPoint.y+(int)(ScaleY*StepY));
		MoveToEx(hDC,crPoint.x-(int)(0.5*ScaleX*StepX),crPoint.y,NULL);
		LineTo(hDC,crPoint.x+(int)(0.5*ScaleX*StepX),crPoint.y);
		crPoint.x=(int)(OutRect->left+ScaleX*(EndSelect - X_Axis->Beg));
		crPoint.y=(int)(OutRect->bottom-ScaleY*((SpecParam+jSel)->Zero+((SpecParam+jSel)->Amp)*(SpecComp+jSel)->OutSpline(EndSelect) - Y_Axis->Beg));
		MoveToEx(hDC,crPoint.x,crPoint.y-(int)(ScaleY*StepY),NULL);
		LineTo(hDC,crPoint.x,crPoint.y+(int)(ScaleY*StepY));
		MoveToEx(hDC,crPoint.x-(int)(0.5*ScaleX*StepX),crPoint.y,NULL);
		LineTo(hDC,crPoint.x+(int)(0.5*ScaleX*StepX),crPoint.y);
		SelectObject(hDC,OldPen);
		DeleteObject(crPen);
	}
	//drawing  components
	penN=1;
	if (NSComp>0)
		{
		CompRect.left=(int)(OutRect->left+(*SpecBase-X_Axis->Beg)*ScaleX);
		CompRect.right=(int)(OutRect->left+(*(SpecBase+NSPoint-1)-X_Axis->Beg)*ScaleX);
		for (j=0;j<NSComp;j++)
			{
			if ( (SpecParam+j)->Select!=NONE )
			{
			crPen=CreatePenIndirect(SpecPen+penN);
			OldPen=(HPEN)SelectObject(hDC,crPen);
			if (((SpecParam+j))->Amp ==0) ((SpecParam+j))->Amp=((SpecParam))->Amp;
			CompRect.bottom=OutRect->bottom;
			CompRect.top=OutRect->top;
			CompRect.bottom=(int)(OutRect->bottom-(((SpecParam+j))->Zero-Y_Axis->Beg)*ScaleY);
			CompRect.top=(int)(OutRect->bottom-(((SpecParam+j))->Amp+((SpecParam+j))->Zero-Y_Axis->Beg)*ScaleY);

			if ((j<N_LINE)&&(((Label+j))->Visible))
				{
				//((Label+j))->Y/=((SpecParam+1))->Amp;
				((SpecComp+j))->ShowLabel(&CompRect,hDC,((Label+j)));
				//((Label+j))->Y*=((SpecParam+1))->Amp;
				}
			if ((SpecPen+penN)->lopnStyle!=PS_NULL)
				((SpecComp+j))->Draw(&CompRect,hDC);
         SelectObject(hDC,OldPen);
			DeleteObject(crPen);
         if ((j<N_LINE)&&((Label+j)->Style!=NONE))
         	{
            crPen=CreatePen(PS_SOLID,1,(SpecPen+penN)->lopnColor);
				OldPen=(HPEN)SelectObject(hDC,crPen);
				((SpecComp+j))->Dot(&CompRect,hDC,(Label+j));
            SelectObject(hDC,OldPen);
            DeleteObject(crPen);
				}
			penN++;
			if (penN > 15)
					penN=1;
			}
			}
		}
	//}
   if (SpecFont[0].lfHeight!=0)
         	{
            SelectObject(hDC,OldFont);
            DeleteObject(crFont);
            }

	SelectObject(hDC,OldBrush);
	DeleteObject(crBrush);
	}
}

/*******************************************************************/
int TSpectrum::FitFind(TFitParam* FitState)
{
	static int NDomCenter;
	float L,H;
	int j_beg,j,i,NDim,ParamDim,crMask,NComp;
	int DimIndex[200];
	int TestMode=TEST_MSR;
	//j_beg=FitState->BegComp;
	j_beg=BegFitComp;
	if (j_beg==1)
		TestMode=TEST_LLH;
	ParamDim=OFFSET_NUM;
	NComp=NSComp-j_beg;
	NDim=ParamDim*NComp;
	if (!FitState->crCicle)
   		{
		  NDomCenter=1;
		  for (j=0;j<NDim;j++) NDomCenter*=2;
		  if (FitDomSize!=NDim)
      		{
      		if (FitDomSize)
      			{
				delete FitDomLow;	delete FitDomHigh;	delete FitDomCenter;	delete FitDomAccur;
         		}
			 FitDomLow=new float[sizeof(float)*NDim];
			 FitDomHigh=new float[sizeof(float)*NDim];
			 FitDomCenter=new float[sizeof(float)*NDomCenter*NDim];
			 FitDomAccur=new float[sizeof(float)*NDomCenter];
			 FitDomSize=NDim;
			 }
		  for (j=0;j<NComp;j++)
      		{
			 *(FitDomLow+j*ParamDim+OFFSET_MAX)=(SpecParam+j_beg+j)->MaxL;
			 *(FitDomLow+j*ParamDim+OFFSET_WID)=(SpecParam+j_beg+j)->WidL;
			 *(FitDomLow+j*ParamDim+OFFSET_SPL)=(SpecParam+j_beg+j)->SplitL;
			 *(FitDomLow+j*ParamDim+OFFSET_MIX)=(SpecParam+j_beg+j)->MixL;
			 *(FitDomLow+j*ParamDim+OFFSET_TIM)=(SpecParam+j_beg+j)->TimeL;
			 *(FitDomHigh+j*ParamDim+OFFSET_MAX)=(SpecParam+j_beg+j)->MaxH;
			 *(FitDomHigh+j*ParamDim+OFFSET_WID)=(SpecParam+j_beg+j)->WidH;
			 *(FitDomHigh+j*ParamDim+OFFSET_SPL)=(SpecParam+j_beg+j)->SplitH;
			 *(FitDomHigh+j*ParamDim+OFFSET_MIX)=(SpecParam+j_beg+j)->MixH;
			 *(FitDomHigh+j*ParamDim+OFFSET_TIM)=(SpecParam+j_beg+j)->TimeH;
			 }
		}
	   if ((FitState->crCicle<FitState->NCicleMax)&&(fabs(FitState->NewAccur-FitState->OldAccur)>FitState->AccurL))
   		{
		  for (j=0;j<NDomCenter;j++)
      		{
			 crMask=1;
			 for (i=0;i<NDim;i++)
         		{
				if ((j&crMask)!=0) DimIndex[i]=1;
            		else DimIndex[i]=0;
				L=(*(FitDomLow+i));	H=(*(FitDomHigh+i));
				*(FitDomCenter+j*NDim+i)=L+(0.5+DimIndex[i])*0.5*(H-L);
				crMask*=2;
				}
			 for (i=0;i<NComp;i++)
         		{
				(SpecParam+j_beg+i)->Max=( *(FitDomCenter+j*NDim+i*ParamDim+OFFSET_MAX) );
				(SpecParam+j_beg+i)->Wid=( *(FitDomCenter+j*NDim+i*ParamDim+OFFSET_WID) );
				(SpecParam+j_beg+i)->Split=( *(FitDomCenter+j*NDim+i*ParamDim+OFFSET_SPL) );
				(SpecParam+j_beg+i)->Mix=( *(FitDomCenter+j*NDim+i*ParamDim+OFFSET_MIX) );
				(SpecParam+j_beg+i)->Time=( *(FitDomCenter+j*NDim+i*ParamDim+OFFSET_TIM) );
				if (TestMode!=TEST_LLH)
            		(SpecComp+j_beg+i)->Reset(SpecParam+j_beg+i);
				}

			 *(FitDomAccur+j)=LeastSqr(TestMode);
			 }
		  FitState->OldAccur=FitState->NewAccur;
		  FitState->NewAccur= *FitDomAccur;	i=0;
			for (j=1;j<NDomCenter;j++)
			 if ( *(FitDomAccur+j)<FitState->NewAccur )
	      		{
				Accurasy=FitState->NewAccur= *(FitDomAccur+j); i=j;
   			  }
		  for (j=0;j<NDim;j++)
      		{
			  L=(*(FitDomLow+j));	H=(*(FitDomHigh+j));
			 *(FitDomLow+j)= (*(FitDomCenter+i*NDim+j))-0.25*(H-L);
			 *(FitDomHigh+j)= (*(FitDomCenter+i*NDim+j))+0.25*(H-L);
			 }
		  FitState->crCicle++;
		  return (1);
		  }
		  else
      		{
			 if (FitState->OldAccur<FitState->NewAccur)
         		Accurasy=FitState->NewAccur=FitState->OldAccur;
			 else
					Accurasy=FitState->OldAccur=FitState->NewAccur;
			 return (0);
			 }
	}
/***********************************************************\
*    returns 0 when fitting is completed                    *
\***********************************************************/
int TSpectrum::FitStep(TFitParam* FitState)
{
static float GradMax[64];
static float GradWid[64];
static float GradSplit[64];
static float GradMix[32];
static float StepVal=100*MinFloat;
double GradModMax,GradModWid,GradModSplit,GradModMix;
int j_beg,j;
int TestMode=TEST_MSR;
//j_beg=FitState->BegComp;
j_beg=BegFitComp;
if ((j_beg==1)&&(TagComp<=1))
	TestMode=TEST_LLH;
if (!FitState->crStep)
	if  ((FitState->crCicle<FitState->NCicleMax)&&(fabs(FitState->NewCicleAccur-FitState->OldCicleAccur)>FitState->AccurL))
	   //if  (FitState->crCicle<FitState->NCicleMax)
   	//new cicle initialization
		{
		GradModWid=0.000001;
		GradModMax=0.000001;
		GradModMix=0.000001;
		GradModSplit=0.000001;
   	// calculation of accurasy gradient
	   for (j=j_beg;j<NSComp;j++)
			{
			if ((SpecParam+j)->Mode!=FILE)
			 {
			 	// gradient by value of line width
			 	if (!(SpecParam+j)->FixWid)
					{
					if (FitState->StepRel)
						StepVal=FitState->StepWid*((SpecParam+j)->Wid);
					else StepVal=FitState->StepWid;
          if (StepVal<MinFloat)StepVal=MinFloat;
					(SpecParam+j)->Wid-=StepVal;
               if (j_beg!=1)
               	(SpecComp+j)->Reset(SpecParam+j);
               FitState->OldAccur=LeastSqr(TestMode);
					(SpecParam+j)->Wid+=(2*StepVal);
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
					FitState->NewAccur=LeastSqr(TestMode);
					GradWid[j-j_beg]=0.5*(FitState->OldAccur-FitState->NewAccur)/StepVal;
					(SpecParam+j)->Wid-=StepVal;
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
			}
			else GradWid[j-j_beg]=0;
            // gradient by value of line maximum
         	if (!(SpecParam+j)->FixMax)
					{
					if (FitState->StepRel)
               	if ( ((SpecParam+j)->Mode==BINOM_EF)||((SpecParam+j)->Mode==BINOM_QU)||((SpecParam+j)->Mode==BINOM_NZ)||((SpecParam+j)->Mode==BINOM)||((SpecParam+j)->Mode==COMPOUND) )
							StepVal=FitState->StepMax*((SpecParam+j)->Max);
                  else
							StepVal=FitState->StepMax*((SpecParam+j)->Wid);
					else StepVal=FitState->StepMax;
               if (StepVal<MinFloat)	StepVal=MinFloat;
					(SpecParam+j)->Max-=StepVal;
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
					FitState->OldAccur=LeastSqr(TestMode);
					(SpecParam+j)->Max+=(2*StepVal);
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
					FitState->NewAccur=LeastSqr(TestMode);
					GradMax[j-j_beg]=0.5*(FitState->OldAccur-FitState->NewAccur)/StepVal;
					(SpecParam+j)->Max-=StepVal;
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
					}
				else GradMax[j-j_beg]=0;
            // gradient by value of Roo or Mix
        if ( ((SpecParam+j)->Mode==CLAMP_ST)||((SpecParam+j)->Mode==CLAMP_FST)||((SpecParam+j)->Mode==CLAMP_FST2)||((SpecParam+j)->Mode==STEPRESP)||((SpecParam+j)->Mode==ABSORB)||((SpecParam+j)->Mode==SINUS)||((SpecParam+j)->Mode==BIMODAL)||((SpecParam+j)->Mode==GAUSLOR))
			  {
				if (FitState->StepRel)
               		StepVal=FitState->StepMix*((SpecParam+j)->Mix);
				else StepVal=FitState->StepMix;
				if (StepVal<MinFloat)	StepVal=MinFloat;
					(SpecParam+j)->Mix-=StepVal;
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
					FitState->OldAccur=LeastSqr(TestMode);
					(SpecParam+j)->Mix+=(2*StepVal);
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
					FitState->NewAccur=LeastSqr(TestMode);
					GradMix[j-j_beg]=0.5*(FitState->OldAccur-FitState->NewAccur)/StepVal;
					(SpecParam+j)->Mix-=StepVal;
               if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
			}
			else GradMix[j-j_beg]=0;
            // gradient by value of line splitting
			if (!((((SpecParam+j)->Mode==GAUSLOR)&&(TestMode!=TEST_LLH))||((SpecParam+j)->Mode==LORDIR)||((SpecParam+j)->Mode==GAUSDIR)||((SpecParam+j)->Mode==BINOM_QU)||((SpecParam+j)->FixSplit)))
			{
				if (FitState->StepRel)
						StepVal=FitState->StepSplit*((SpecParam+j)->Wid);
					else StepVal=FitState->StepSplit;
				if (StepVal<MinFloat)StepVal=MinFloat;
					(SpecParam+j)->Split-=StepVal;
				if (j_beg!=1)
						(SpecComp+j)->Reset(SpecParam+j);
					FitState->OldAccur=LeastSqr(TestMode);
					(SpecParam+j)->Split+=(2*StepVal);
				if (j_beg!=1)
					(SpecComp+j)->Reset(SpecParam+j);
				FitState->NewAccur=LeastSqr(TestMode);
				GradSplit[j-j_beg]=0.5*(FitState->OldAccur-FitState->NewAccur)/StepVal;
				(SpecParam+j)->Split-=StepVal;
				if (j_beg!=1)
					(SpecComp+j)->Reset(SpecParam+j);
			}
            else GradSplit[j-j_beg]=0;
            }
         else
			{
				GradMax[j-j_beg]=0;
				GradWid[j-j_beg]=0;
				GradSplit[j-j_beg]=0;
				GradMix[j-j_beg]=0;
      }
      	GradModWid+=(GradWid[j-j_beg])*(GradWid[j-j_beg]);
		GradModMax+=(GradMax[j-j_beg])*(GradMax[j-j_beg]);
        GradModMix+=(GradMix[j-j_beg])*(GradMix[j-j_beg]);
		GradModSplit+=(GradSplit[j-j_beg])*(GradSplit[j-j_beg]);
		}
		// re-norming the gradient
      if (GradModWid<MinFloat)GradModWid=MinFloat;
      if (GradModMax<MinFloat)GradModMax=MinFloat;
      if (GradModMix<MinFloat)GradModMix=MinFloat;
      if (GradModSplit<MinFloat)GradModSplit=MinFloat;
   		GradModWid=0.0000001+sqrt(GradModWid);
		GradModMax=0.0000001+sqrt(GradModMax);
		GradModMix=0.0000001+sqrt(GradModMix);
		GradModSplit=0.0000001+sqrt(GradModSplit);
		if (GradModWid!=0.0)
			if (FitState->StepRel)
				for (j=j_beg;j<NSComp;j++)
					GradWid[j-j_beg]*=(FitState->StepWid*((SpecParam+j)->Wid)/GradModWid);
			else
      		for (j=j_beg;j<NSComp;j++)
					GradWid[j-j_beg]*=(FitState->StepWid/GradModWid);
   	if (GradModMax!=0.0)
			if (FitState->StepRel)
           	if ( ((SpecParam+j)->Mode==CLAMP_ST)||((SpecParam+j)->Mode==BINOM_EF)||((SpecParam+j)->Mode==BINOM_QU)||((SpecParam+j)->Mode==BINOM)||((SpecParam+j)->Mode==BINOM_NZ)||((SpecParam+j)->Mode==COMPOUND) )
           		for (j=j_beg;j<NSComp;j++)
						GradMax[j-j_beg]*=(FitState->StepMax*((SpecParam+j)->Max)/GradModMax);
         	else
					for (j=j_beg;j<NSComp;j++)
						GradMax[j-j_beg]*=(FitState->StepMax*((SpecParam+j)->Wid)/GradModMax);
         else
				for (j=j_beg;j<NSComp;j++)
					GradMax[j-j_beg]*=(FitState->StepMax/GradModMax);
      if (GradModMix!=0.0)
			if (FitState->StepRel)
				for (j=j_beg;j<NSComp;j++)
					GradMix[j-j_beg]*=(FitState->StepMix*((SpecParam+j)->Mix)/GradModMix);
			else
				for (j=j_beg;j<NSComp;j++)
					GradMix[j-j_beg]*=(FitState->StepMix/GradModMix);
   	if (GradModSplit!=0.0)
			if (FitState->StepRel)
				for (j=j_beg;j<NSComp;j++)
					GradSplit[j-j_beg]*=(FitState->StepSplit*((SpecParam+j)->Wid)/GradModSplit);
			else
				for (j=j_beg;j<NSComp;j++)
					GradSplit[j-j_beg]*=(FitState->StepSplit/GradModSplit);
	  FitState->OldAccur=LeastSqr(TestMode);
   	} //of if at 3765 - new cycle intializtion
   else
   	{
      FitState->NewAccur=LeastSqr(TestMode);
      return(0);
      }
//continueing steps
for (j=j_beg;j<NSComp;j++)
	{
	if((SpecParam+j)->Mode!=FILE)
		{
		if (!(SpecParam+j)->FixWid)
			(SpecParam+j)->Wid+=GradWid[j-j_beg];
		if (!(SpecParam+j)->FixMax)
			(SpecParam+j)->Max+=GradMax[j-j_beg];
      if ( ((SpecParam+j)->Mode==CLAMP_ST)||((SpecParam+j)->Mode==CLAMP_FST)||((SpecParam+j)->Mode==CLAMP_FST2)||((SpecParam+j)->Mode==STEPRESP)||((SpecParam+j)->Mode==GAUSLOR)||((SpecParam+j)->Mode==ABSORB)||((SpecParam+j)->Mode==SINUS)||((SpecParam+j)->Mode==BIMODAL))
			(SpecParam+j)->Mix+=GradMix[j-j_beg];
		if (!(((SpecParam+j)->Mode==GAUSLOR)||((SpecParam+j)->Mode==LORDIR)||((SpecParam+j)->Mode==GAUSDIR)||((SpecParam+j)->FixSplit)))
				(SpecParam+j)->Split+=GradSplit[j-j_beg];
         if (j_beg!=1)
				(SpecComp+j)->Reset(SpecParam+j);
			}
   }
FitState->NewAccur=LeastSqr(TestMode);
FitState->crStep++;
FitState->totStep++;
if ((FitState->crStep > FitState->NStepMax)||(FitState->NewAccur>=FitState->OldAccur))
   {
	FitState->crStep=0;
	FitState->crCicle++;
   //steps diminishing : in 2 times for each 5 cicles
   if (FitState->StepProgress){
     FitState->StepMax/=1.148;	FitState->StepWid/=1.148;	FitState->StepSplit/=1.148;
   	}
   FitState->OldCicleAccur=FitState->NewCicleAccur;
   FitState->NewCicleAccur=FitState->OldAccur;
	for (j=j_beg;j<NSComp;j++)
		{
		if((SpecParam+j)->Mode!=FILE)
			{
			if (!(SpecParam+j)->FixWid)
				(SpecParam+j)->Wid-=0.5*GradWid[j-j_beg];
			if (!(SpecParam+j)->FixMax)
				(SpecParam+j)->Max-=0.5*GradMax[j-j_beg];
         if ( ((SpecParam+j)->Mode==CLAMP_ST)||((SpecParam+j)->Mode==CLAMP_FST)||((SpecParam+j)->Mode==CLAMP_FST2)||((SpecParam+j)->Mode==STEPRESP)||((SpecParam+j)->Mode==GAUSLOR)||((SpecParam+j)->Mode==ABSORB))
				(SpecParam+j)->Mix-=0.5*GradMix[j-j_beg];
			if (((SpecParam+j)->Mode==ABSORB)&&(!(SpecParam+j)->FixSplit))
				(SpecParam+j)->Split-=0.5*GradSplit[j-j_beg];
         if (j_beg!=1)
				(SpecComp+j)->Reset((SpecParam+j));
			}
   	}
   }
Accurasy=FitState->OldAccur=FitState->NewAccur;
return 1;
}
void TSpectrum::AutoFit(TFitParam* FitState)
{
	int FitStop;
	float StepWid, StepMax, StepSplit,StepMix;
	//initialization
	FitState->NewCicleAccur=FitState->OldCicleAccur+2*FitState->AccurL;
	FitState->crStep=0;
	FitState->crCicle=0;
	FitState->totStep=0;
	FitState->StepProgress=1;
	// remember steps
	StepWid=FitState->StepWid;
	StepMax=FitState->StepMax;
	StepSplit=FitState->StepSplit;
	StepMix=FitState->StepMix;
	FitStop=1;
	while (FitStop)
    {
		FitStop=FitStep(FitState);
		
	}
	// restore steps
	FitState->StepWid=StepWid;
	FitState->StepMax=StepMax;
	FitState->StepSplit=StepSplit;
	FitState->StepMix=StepMix;
}
/********************************************/
float TSpectrum::Test(void)
{
	int j;
	float S;
   S=0;
   for (j=0;j<NSComp;j++)
   	{
      TagComp=j;
   	S+=Mult((SpecComp),(SpecComp+TagComp))/NSPoint;
      //S+=LeastSqr(NSComp-2);
      }
	return S;
}
//
float TSpectrum::Area()
{
	int j;
	float Area;
	j=FindSelComp();
	*(SpecComp+j)*=(SpecParam+j)->Amp;
	*(SpecComp+j)+=(SpecParam+j)->Zero;
	Area=(SpecComp+j)->Area(BegSelect,EndSelect);
	*(SpecComp+j)-=(SpecParam+j)->Zero;
	*(SpecComp+j)/=(SpecParam+j)->Amp;
	return Area;
}
//
float	TSpectrum::Slope(int Beg, int End, int Comp)
{
	int N;
	N=Comp;
	if (N<0) N=0;
	else
		if(N>NSComp-1)N=NSComp-1;

	return ((SpecComp+N)->Slope(Beg,End))*((SpecParam+N)->Amp);
}

float TSpectrum::OutSpline(int jComp, float X, int FastFlag)
{
	if ((jComp>=0)&&(jComp<NSComp))
		return (SpecParam+jComp)->Zero+((SpecParam+jComp)->Amp)*((SpecComp+jComp)->OutSpline(X,FastFlag));
	else return 0;
}
