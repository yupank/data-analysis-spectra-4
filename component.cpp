#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "Fstream.h"
#include "string.h"
#include "math.h"
#include "component.h"
// Auxillary functions and variables

// Diffusion model
const float Release=6.6e-6;	
float Sigma = 40;	float h=0.02; float Rmax=0.5; float Zmax=0.3;	
float Rcleft=0.1;
float DiffSourceOn=1;
float DiffSourceData[DiffSize];
float DiffSourceBase[DiffSize];
float MediatX[NMediat];
float MediatY[NMediat]; 
float Simps[50];
int Crit;
char StrBuf[128];
extern float Rating[10050];
extern int tNum;
extern int eNum;
extern int mNum; 
TComponent* MediatC;
TComponent* MediatComp;
TComponent* SourceComp;
// Auxillary functions for spillover stimulation
//   Advanced model of mediator diffusion
void	MediatCalc(float R, float D, float Z, float Btot, int NVes, float* Base, float* Data, int NPoint)
{
	// this version is improvement of last DIFFUS_2, with correction on dependence of Uptake on [Glu]
	//AfxMessageBox("Check 1");
	int i,j,k,m;
	int NSimps=30;
	float Simps[40];
	float al, dl, D2, NRelease, Uptake,Mediat;
	float glu,X,S,t,tau,dtau;
	float Kup, K12;
	al=0.1;	dl=1.34;	D2=D/(dl*dl);
    NRelease=(1.59e-06)*4000*NVes;
	
	//SourceComp=new TComponent(DiffSourceData,DiffSourceBase,DiffSize);
	
	for (i=1;i<=NSimps/2-1;i++)
	{
		Simps[i*2-1]=4;	Simps[i*2]=2;
	}
    Simps[0]=1;	Simps[NSimps-1]=1;
    for (j=0;j<=NPoint;j++)
		*(Data+j)=0;
    if (R>Rcleft)
	{
		Uptake=Btot*(3.14159*Rcleft*Rcleft*h)*1.0e-6;
		//SourceComp=new TComponent(DiffSourceData,DiffSourceBase,DiffSize);
		if (DiffSourceOn)
		{
			for (j=0;j<DiffSize;j++)
			*(DiffSourceBase+j)=j*10.5/DiffSize;
			DiffSourceCalc(Rcleft, D, Uptake, NVes, DiffSourceBase, DiffSourceData, DiffSize);
			*(SourceComp)/=al;
			*(SourceComp)*=NRelease;
		}
			
			
		//Uptake=Spl*(3.14159*Rmax*Rmax*h)*1.0e-3;
		Uptake=Btot*1.0e-3; // units: mM/msec
            //Uptake =0;
		for (j=1;j<NPoint;j++)
		{
			// Uptake depends on time! 1st ms - 10 times stronger
			t= *(Base+j);
			S=0;
			dtau=t/NSimps;
			for (i=0;i<NSimps;i++)
			{
				tau=(i+0.5)*dtau;
				m=j*(tau/t);
				if (m<1) m=1;
				if (m>j-1) m=j-1;
				glu= *(Data+m)+(tau-(*(Base+m)))*((*(Data+m))-(*(Data+m-1)))/((*(Base+m))-(*(Base+m)-1));
				Mediat=SourceComp->OutSpline(tau,1); //diffusion source
         	   	S+=(dtau/3)*Simps[i]*(Mediat-Uptake*Upt(tau,glu))*exp(-(Z*Z+(R-Rcleft)*(R-Rcleft))/(4*D2*(t-tau)))/exp(1.5*log(12.57*D2*(t-tau)));
			}
				         	
			if (S>0)
				//*(Data+j)=1000*S/(12.57*(h)*D);
			{
				*(Data+j)=S; k=j;
			}
			else
				*(Data+j)=0;
		   //*(Data+j)=1000*S/(12.57*h*D2);
			
			
		}
			
		while ((k>2)&&(*(Data+k-1)> *(Data+k))) k--;
		k+=12;
		if ( (*(Data+k-10)>0)&&(*(Data+k-4)>0)&&(*(Data+k-8)>0) )
		{
			Kup=log(*(Data+k-10)/(*(Data+k-8)))/(*(Base+k-8)-(*(Base+k-10)));
			K12=log(*(Data+k-10)/(*(Data+k-4)))/(*(Base+k-4)-(*(Base+k-10)));
			S = 0.5*(*(Data+k-10));
			//if ((Kup>0)&&(K12>0))
			if (Kup<0.01) Kup=0.01;	if (Kup>5) Kup = 5;
			if (K12<0.01) K12=0.01;	if (K12>2) K12=2;
			for (j=k-10;j<NPoint;j++)
			{
				X = *(Base+k-10)-(*(Base+j));
				*(Data+j)=S*(exp(Kup*X)+exp(K12*X));
			}

		}
		//delete SourceComp;
			
	}
	else // R<=Rcleft
	{

		//Uptake=Spl*(3.14159*Rcleft*Rcleft*h)*1.0e-3;
		Uptake=Btot*1.0e-4;
		for (j=1;j<NPoint;j++)
		{
			t= *(Base+j);
			S=0;
			if (t>(3/Sigma)*(NSimps+1)/NSimps)
			{
            	dtau=(3/Sigma)/NSimps;
	   			for (i=0;i<NSimps;i++)
   	   			{
      	   			tau=(i+1)*dtau;
         			S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*NRelease*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
            	}
				dtau=(0.99*t-(3/Sigma)*(NSimps+1)/NSimps)/(NSimps-1);
   				for (i=0;i<NSimps;i++)
      			{
         			tau=(3/Sigma)*(NSimps+1)/NSimps+i*dtau;
            		S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*NRelease*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
               	}
            }
            else
            {
				dtau=0.99*t/NSimps;
   				for (i=0;i<NSimps;i++)
      			{
         			tau=(i+1)*dtau;
            		S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*NRelease*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
               	}
            }
			if (S>0)
				*(Data+j)=S/(12.57*h*D);
			else
				*(Data+j)=0;
			
			
		}
		
	}
	//AfxMessageBox("Check 2");
	//delete SourceComp;

}
void DiffSourceCalc(float R, float D, float Uptake, int NVes, float* Base, float* SourceData, int NPoint)
{
	float tau, dtau, t, NRelease, S;
	int i,j;
	int NSimps=10;
	float Simps[20];
	float* Data; 
	NRelease = Release*NVes; 
	for (i=1;i<=NSimps/2-1;i++)
	{
		Simps[i*2-1]=4;	Simps[i*2]=2;
	}
	Simps[0]=1;	Simps[NSimps-1]=1;
	if (NPoint)
	{
		//Data = new float[NPoint];
		Data=(float*) malloc(NPoint*sizeof(float));
		for (j=1;j<NPoint;j++)
		{
			t= *(Base+j);
			S=0;
			if (t>(3/Sigma)*(NSimps+1)/NSimps)
			{
				dtau=(3/Sigma)/NSimps;
				for (i=0;i<NSimps;i++)
   				{

					tau=(i+1)*dtau;
					S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*Release*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
				}
				dtau=(0.99*t-(3/Sigma)*(NSimps+1)/NSimps)/(NSimps-1);
   				for (i=0;i<NSimps;i++)
      			{
					tau=(3/Sigma)*(NSimps+1)/NSimps+i*dtau;
					S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*Release*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
				}
			}
			else
			{
				dtau=0.99*t/NSimps;
   				for (i=0;i<NSimps;i++)
      			{
					tau=(i+1)*dtau;
					S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*Release*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
				}
			}
			*(Data+j)=1000*S/(12.57*h*D);
		}
		S=0;
		for (j=0;j<NPoint-1;j++)
		{
			t= *(Base+j+1)-(*(Base+j));
			S+= 0.5*(*(Data+j+1)+(*(Data+j)))*t;
		}
			
		
		for (j=0;j<NPoint;j++)
			*(SourceData+j)=*(Data+j)/S;
		
		//delete Data;
		free(Data);
	}
}
/***************************************************************************/
float Upt(float t)
{
	const float a = 0.2;
	return 2*exp(-t)+(1-a)*exp(-0.1*t)+a;
}
float Upt(float t, float g)
{
	const float K1 = 5; // mM-1 ms-1
	const float K_1= 0.1;//ms-1
	const float K2 = 0.02;//ms-1
	const float Kup = 0.022;//mM
	float B = Kup/(Kup+g)+(g/(g+Kup))*exp(-(K1*g+K_1+K2)*t);
	return ( g*B*K1/K2-K_1*(1-B)/K2);

}

float NMDArec(float t)
{
	float KDes, K_Des, alp, bet,KD, K1, K_1, K2, K_2;
	float tau, dtau, S, Mediat, Beta, Beta2;
	int NSimps,i;
	alp=0.0738;
	bet=0.0465;	KDes=0.0084;	K_Des=0.0018;	KD=0.0277;	K_1=10;   K1=10;
	NSimps=30;
	Crit=3;
	for (i=1;i<=NSimps/2-1;i++)
	{
		Simps[i*2-1]=4;	Simps[i*2]=2;
    }
	Simps[0]=1;	Simps[NSimps-1]=1;
	S=0;
	/*
	dtau=t/(NSimps+1);
	for (i=0;i<NSimps;i++)
    {
	    tau=(i+0.5)*dtau;
   		Mediat=MediatC->OutSpline(t-tau,1);
		Beta=Mediat/KD;	Beta*=Beta;
		//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(exp((-tau)*(KDes+K_Des))-exp((-tau)*(alp+bet)));
      	//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*(1+K_Des/(KDes+K_Des))*exp((-tau)*(alp+bet)));
		//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*((alp+bet)*K1*Mediat/(alp+bet+K1*Mediat))));
		Beta2=1-exp((tau-t)*(K_1+K1*Mediat));
		S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2*Beta2;
		//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)));
    }
	*/
   	//if (t>=(Crit/(alp+bet))*(NSimps+1)/NSimps)
    
	if (t>=(Crit/Sigma)*(NSimps+1)/NSimps)
    {
		//dtau=(Crit/(alp+bet))/NSimps;
		dtau=(Crit/(Sigma))/NSimps;
		for (i=0;i<NSimps;i++)
		{
      		tau=(i+1)*dtau;
			Mediat=MediatComp->OutSpline(t-tau,1);
			Beta=Mediat/KD;	Beta*=Beta;
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(exp((-tau)*(KDes+K_Des))-exp((-tau)*(alp+bet)));
      		//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*(1+K_Des/(KDes+K_Des))*exp((-tau)*(alp+bet)));
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*((alp+bet)*K1*Mediat/(alp+bet+K1*Mediat))));
			Beta2=1-exp((tau-t)*(K_1+K1*Mediat));
			S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2*Beta2;
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)));
		}
		//dtau=(0.99*t-(Crit/(alp+bet))*(NSimps+1)/NSimps)/(NSimps-1);
        dtau=(0.99*t-(Crit/(Sigma))*(NSimps+1)/NSimps)/(NSimps-1);
		for (i=0;i<NSimps;i++)
      	{
			//tau=((Crit/(alp+bet))*(NSimps+1)/NSimps)+i*dtau;
			tau=((Crit/(Sigma))*(NSimps+1)/NSimps)+i*dtau;
      		Mediat=MediatComp->OutSpline(t-tau,1);
			Beta=Mediat/KD;	Beta*=Beta;
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(exp((-tau)*(KDes+K_Des))-exp((-tau)*(alp+bet)));
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*(1+K_Des/(KDes+K_Des))*exp((-tau)*(alp+bet)));
      		//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*((alp+bet)*K1*Mediat/(alp+bet+K1*Mediat))));
			Beta2=1-exp((tau-t)*(K_1+K1*Mediat));
			S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2*Beta2;
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)));
		}
	}
	else
   	{
		dtau=0.99*t/NSimps;
		for (i=0;i<NSimps;i++)
      	{
	        tau=(i+1)*dtau;
   			Mediat=MediatComp->OutSpline(t-tau,1);
			Beta=Mediat/KD;	Beta*=Beta;
		    //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(exp((-tau)*(KDes+K_Des))-exp((-tau)*(alp+bet)));
      		//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*(1+K_Des/(KDes+K_Des))*exp((-tau)*(alp+bet)));
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*((alp+bet)*K1*Mediat/(alp+bet+K1*Mediat))));
			Beta2=1-exp((tau-t)*(K_1+K1*Mediat));
			S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2*Beta2;
			//S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-KDes*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)));
        }
    }
	
	if (S<0)S=0;
	else S*=(bet/(-KDes-K_Des+alp+bet));
		
	return S;
}
float AMPArec(float t)
{
	float KDes, K_Des, alp, bet,KD, K1, K_1, K2, K_2;
	float tau, dtau, S, Mediat, Beta, Beta2;
	int NSimps,i;
	alp=0.9;
    bet=4.24;	KDes=0.017;	K_Des=0.0007;	KD=1;	K_1=4.26;  K1=4.6; K_2=3.26; K2=28.4;
	NSimps=30;
	Crit=3;
	for (i=1;i<=NSimps/2-1;i++)
	{
		Simps[i*2-1]=4;	Simps[i*2]=2;
    }
	Simps[0]=1;	Simps[NSimps-1]=1;
	S=0;
	
	dtau=t/(NSimps+1);
	for (i=0;i<NSimps;i++)
    {
	    tau=(i+0.5)*dtau;
   		Mediat=MediatComp->OutSpline(t-tau,1);
		Beta=Mediat/KD;
		Beta2=1-((K2*Mediat+K_2)*exp((tau-t)*(K_1+K1*Mediat))-(K1*Mediat+K_1)*exp((tau-t)*(K_2+K2*Mediat)))/((K2-K1)*Mediat+K_2-K_1);
        S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2;

    }
	if (S<0)S=0;
	else S*=(bet/(-KDes-K_Des+alp+bet));
	return S;
}


/***********************************************************************************************\
*                         Solving the set of linear equation(using Gauss method)                *
\***********************************************************************************************/
int EquSet(float far* Y, float far* A, float far* B, int N)
{
int i,j,k;
float OldPar,Mult;
int Arr[2000];                           // arrangement: number of variable - number of column
//Arr=(int*)malloc((size_t)(500*sizeof(int)));

for (j=0;j<N;j++)
    *(Arr+j)=j;
for (k=0;k < (N-1);k++)
    {                               // system matrix tramsformation

    // testing the value of k-th coefficient of k-th  equation
    if ( *(A+N*k+k)==0 )
       {
       i=k;
       while ( (*(A+N*i+k)==0)&&(i < N) )
             i++;
       if ( *(A+N*i+k)!=0 )
       // rearranging the aquations ( k-th and i-th)
          {
          for (j=0;j < N;j++)
              {
              OldPar=(*(A+N*k+j));
              *(A+N*k+j)= *(A+N*i+j);
              *(A+N*i+j)=OldPar;
              }
          OldPar= *(B+k);
          *(B+k)= *(B+i);
          *(B+i)=OldPar;
          }
          else
              //rearranging the variables
              {
              i=k;
              while ( (*(A+N*k+i)==0)&&(i < N) )
                    i++;
              if ( *(A+N*k+i)!=0 )
                 for (j=0;j < N;j++)
                     {
                     OldPar= *(A+N*j+k);
                     *(A+N*j+k)= *(A+N*j+i);
                     *(A+N*j+i)= OldPar;
                     *(Arr+i)=k;
                     *(Arr+k)=i;
                     }
                 else
                     return(0); // system can't been solved
              } // of else
       }
    for (j=k+1;j < N;j++)
        // subtracting k-th equation  from j-th one
        {
        Mult=(*(A+N*j+k))/(*(A+N*k+k));
        for (i=k;i < N;i++)
        *(A+N*j+i)-=Mult*(*(A+k*N+i));
        *(B+j)-=Mult*(*(B+k));
        }
	 }
i=N-1;
*(Y+i)=(*(B+i))/(*(A+N*i+i));
for (i=N-2;i>=0;i--)
    {
    for (j=i+1;j < N;j++)
        *(B+i)-=(*(A+N*i+j))*(*(Y+j));
    *(Y+i)=(*(B+i))/(*(A+N*i+i));
    }
for (k=0;k < N;k++)
    {
    OldPar= *(Y+k);
    *(Y+k)= *(Y+(*(Arr+k)));
    *(Y+(*(Arr+k)))=OldPar;
	 }
//free(Arr);
return(1);
}

int EquSet2(double far* Y, double far* A, double far* B, int N)
{
int i,j,k;
double OldPar,Mult;
int Arr[500];                           // arrangement: number of variable - number of column 
//Arr=(int*)malloc((size_t)(500*sizeof(int)));

for (j=0;j<N;j++)
    *(Arr+j)=j;
for (k=0;k < (N-1);k++)
    {                               // system matrix tramsformation
    
    // testing the value of k-th coefficient of k-th  equation
    if ( *(A+N*k+k)==0 )
       {
       i=k;
       while ( (*(A+N*i+k)==0)&&(i < N) )
             i++;
       if ( *(A+N*i+k)!=0 )
       // rearranging the aquations ( k-th and i-th)
          {
          for (j=0;j < N;j++)
              {
              OldPar=(*(A+N*k+j));
              *(A+N*k+j)= *(A+N*i+j);
              *(A+N*i+j)=OldPar;
              }
          OldPar= *(B+k);
          *(B+k)= *(B+i);
          *(B+i)=OldPar;
          }
          else
              //rearranging the variables
              {
              i=k;
              while ( (*(A+N*k+i)==0)&&(i < N) )
                    i++;
              if ( *(A+N*k+i)!=0 )
                 for (j=0;j < N;j++)
                     {
                     OldPar= *(A+N*j+k);
                     *(A+N*j+k)= *(A+N*j+i);
                     *(A+N*j+i)= OldPar;
                     *(Arr+i)=k;
                     *(Arr+k)=i;
                     }
                 else
                     return(0); // system can't been solved
              } // of else
       }
    for (j=k+1;j < N;j++)
        // subtracting k-th equation  from j-th one
        {
        Mult=(*(A+N*j+k))/(*(A+N*k+k));
        for (i=k;i < N;i++)
        *(A+N*j+i)-=Mult*(*(A+k*N+i));
        *(B+j)-=Mult*(*(B+k));
        }
    }
i=N-1;
*(Y+i)=(*(B+i))/(*(A+N*i+i));
for (i=N-2;i>=0;i--)
    {
    for (j=i+1;j < N;j++)
        *(B+i)-=(*(A+N*i+j))*(*(Y+j));
    *(Y+i)=(*(B+i))/(*(A+N*i+i));
    }
for (k=0;k < N;k++)
    {
    OldPar= *(Y+k);
    *(Y+k)= *(Y+(*(Arr+k)));
    *(Y+(*(Arr+k)))=OldPar;
	 }
//free(Arr);
return(1);
}
/********************************************/
long int Factorial(int N)
{
	long int j,S;
   S=1;
   if (N)
     for (j=1;j<=N;j++)
     	S*=j;
   return S;
}
///////////////////////////////////////////////////////////////////////
// Printing float value in ecomomical format
void	SprintFloat(float Val, char* szNum, int ExtraDig)
{
	float Factor;
	int Dig;
	if (fabs(Val)!=0)
		Factor=log10(fabs(Val));
	else Factor=0;
	if (Factor>0)
	{
  		Dig = (int)(Factor);
		sprintf(szNum,"%*.*f",Dig+1,(unsigned int)(3+ExtraDig-Dig),Val);
		
	}
	else
		{
		Dig = (int)fabs(Factor-0.99);
		sprintf(szNum,"%*.*f",Dig+1,Dig+2+ExtraDig,Val);
		}
}
// Avoiding of float overflow
float ValidFloat(float Val, float Min)
{
	float MinV;
	if (Min>0) MinV=Min;
	else MinV=MinFloat;
	if (fabs(Val)<MinV)
	{
		if (Val<0) return -MinV;
		else return MinV;
	}
	else return Val;
}
/********************************************/
void TOldParam::CopyOld2New(TParam* ptParam)
{
	ptParam->Kf=Kf;
	ptParam->Zero=Zero;
	ptParam->Amp=Amp;
	ptParam->Wid=Wid;
	ptParam->WidH=WidH;
	ptParam->WidL=WidL;
	ptParam->MaxH=MaxH;
	ptParam->MaxL=MaxL;
	ptParam->SplitH=SplitH;
	ptParam->SplitL=SplitL;
	ptParam->Max=Max;
	ptParam->Mix=Mix;
	ptParam->Split=Split;
	ptParam->ExtMin=ExtMin;
	ptParam->ExtMax=ExtMax;
	ptParam->Mode=Mode;
	ptParam->Mult=Mult;
	ptParam->FixWid=FixWid;
	ptParam->FixMax=FixMax;
	ptParam->FixSplit=FixSplit;
	ptParam->Select=Select;
   ptParam->Time=Time;
   ptParam->NormCf=NormCf;
   strcpy(ptParam->Name,Name);
}
/********************************************/
void TOldParam::CopyNew2Old(TParam* ptParam)
{
	Kf=ptParam->Kf;
	Zero=ptParam->Zero;
	Amp=ptParam->Amp;
	Wid=ptParam->Wid;
	WidH=ptParam->WidH;
	WidL=ptParam->WidL;
	MaxH=ptParam->MaxH;
	MaxL=ptParam->MaxL;
	SplitH=ptParam->SplitH;
	SplitL=ptParam->SplitL;
	Max=ptParam->Max;
	Mix=ptParam->Mix;
	Split=ptParam->Split;
	ExtMin=ptParam->ExtMin;
	ExtMax=ptParam->ExtMax;
	Mode=ptParam->Mode;
	Mult=ptParam->Mult;
	FixWid=ptParam->FixWid;
	FixMax=ptParam->FixMax;
	FixSplit=ptParam->FixSplit;
	Select=ptParam->Select;
	Time=ptParam->Time;
	NormCf=ptParam->NormCf;
	strcpy(Name,ptParam->Name);
}
/********************************************/
void TParam::Paste(TParam* ptParam,int ScaleCopy)
{
	Kf=ptParam->Kf;
	if(ScaleCopy)
	{
		Zero=ptParam->Zero;
		Amp=ptParam->Amp;
	}
	Wid=ptParam->Wid;
	WidH=ptParam->WidH;
	WidL=ptParam->WidL;
	MaxH=ptParam->MaxH;
	MaxL=ptParam->MaxL;
	SplitH=ptParam->SplitH;
	SplitL=ptParam->SplitL;
	Max=ptParam->Max;
	Mix=ptParam->Mix;
	MixH=ptParam->MixH;
	MixL=ptParam->MixL;
	Split=ptParam->Split;
	ExtMin=ptParam->ExtMin;
	ExtMax=ptParam->ExtMax;
	Mode=ptParam->Mode;
	Mult=ptParam->Mult;
	MultL=ptParam->MultL;
	MultH=ptParam->MultH;
	FixWid=ptParam->FixWid;
	FixMax=ptParam->FixMax;
	FixSplit=ptParam->FixSplit;
	Select=ptParam->Select;
	Time=ptParam->Time;
	TimeL=ptParam->TimeL;
	TimeH=ptParam->TimeH;
	ptParam->NormCf=NormCf;
	strcpy(Name,ptParam->Name);
}
/********************************************/
void TParam::Copy(TParam* ptParam)
{
	ptParam->Kf=Kf;
	ptParam->Zero=Zero;
	ptParam->Amp=Amp;
	ptParam->Wid=Wid;
	ptParam->WidH=WidH;
	ptParam->WidL=WidL;
	ptParam->MaxH=MaxH;
	ptParam->MaxL=MaxL;
	ptParam->SplitH=SplitH;
	ptParam->SplitL=SplitL;
	ptParam->MultL=ptParam->MultL;
	ptParam->MultH=ptParam->MultH;
	ptParam->MixH=ptParam->MixH;
	ptParam->MixL=ptParam->MixL;
	ptParam->Max=Max;
	ptParam->Mix=Mix;
	ptParam->Split=Split;
	ptParam->ExtMin=ExtMin;
	ptParam->ExtMax=ExtMax;
	ptParam->Mode=Mode;
	ptParam->Mult=Mult;
	ptParam->FixWid=FixWid;
	ptParam->FixMax=FixMax;
	ptParam->FixSplit=FixSplit;
	ptParam->Select=Select;
	ptParam->Time=Time;
	ptParam->TimeL=TimeL;
	ptParam->TimeH=TimeH;
	ptParam->NormCf=NormCf;
	strcpy(ptParam->Name,Name);
}
//
float Rand(void)
{
	float R;
	R=(float)rand();
	return(R/RAND_MAX);
}
/*************************************/
int RandomBinom(int n, float p)
{
   int j,m;
   m=0;
   for (j=0;j<n;j++)
      if ( p>=Rand()) m++;
   return m;
}
/*************************************/
float RandomNorm(float Mean, float SD)
{
   const int N=64;
	int j;
   float X;
   SD*=6/sqrt(3.0*N);
   X=Mean-0.5*N*SD;
   for (j=1;j<=N;j++)
   	X+=SD*1.0001*Rand();
       //X+=SD*0.5*(0.01*random(101)+0.001*random(1001)+0.0001*random(10001)+0.00001*random(100001));
   return X;
}
/********************************************************************/
void TComponent::Derive()
	{
	HGLOBAL HMem;
	float far* D;
   	float Min,Max;
	int j;
	HMem=GlobalAlloc(GHND,64000);
	D=(float far*)GlobalLock(HMem);
	*D=(*(Data+1)-(*(Data)))/(*(Base+1)-(*(Base)));
	*(D+NPoint-1)=(*(Data+NPoint-1)-(*(Data+NPoint-2)))/(*(Base+NPoint-1)-(*(Base+NPoint-2)));
	for (j=1;j<NPoint-1;j++)
		 *(D+j)=(*(Data+j+1)-(*(Data+j-1)))/(*(Base+j+1)-(*(Base+j-1)));
	for (j=0;j<NPoint;j++)
		if ( *(D+j)< Min) Min= *(D+j);
			else
				if ( *(D+j)> Max) Max= *(D+j);
   Max-=Min;
	if (Max!=0)
		for (j=0;j<NPoint;j++)
			*(Data+j)= ( *(D+j)-Min)/Max;
	GlobalUnlock(HMem);
	GlobalFree(HMem);
}

/*********************************************************************/
int TComponent::ExtremSearch(TPhourier* Filter, float far* LockX, int NMax,int SearchMode, float *PNoise)
{

	float* CrX;
	float* CrY;
	float* BcgrY;
	int j,i, Count;
	float ExL_Y,ExL_X,ExR_X,ExR_Y,dW,Gr,PreLockX,PreLockY;
	float BcMean, BcNoise,BcNoise1,BcNoise2,SlopeL,SlopeR,sdN,dwL,dwR;
	Count=0;
	if ((SearchMode==0)||(Filter->FreqH<0)) //compatibility to old version
	{
		dW=(Filter->CutFreq)*(*(Base+NPoint-2)-(*Base));
		CrX=Base;
		CrY=Data;
		ExL_X= *CrX;
		ExL_Y= *CrY;
		j=0;
		if ( *(CrY+1)>ExL_Y ) Gr=1;
			else Gr=-1;
		do
		{
			// Finding the current extremum
			do
			{
				j++; CrX++; CrY++;
				ExR_X= *CrX;
				ExR_Y= *CrY;
			}
			while ( (Gr*(*(CrY+1)-ExR_Y) > 0)&&(j<NPoint-1) ) ;
			// Testing the current extremum
			if ( (ExR_X-ExL_X>=dW)&&(fabs(ExR_Y-ExL_Y)>=Filter->CutAmp) )
			{
				if ( ExL_Y > ExR_Y )
				{
					//Returning current pair
					*(LockX+Count)=ExL_X;
					*(LockX+Count+1)=ExR_X;
					Count+=2;
				}
				ExL_X= *CrX;
				ExL_Y= *CrY;
				Gr*=-1;
			}
			

		}
		while( (j<NPoint-1)&&(Count<NMax-1) );
		
		
	}// end of compatibility to previous version
	else
	{
		
		TComponent* BcgrComp=new TComponent(Base,NPoint);
		*BcgrComp=this;
		BcgrComp->StatRep(&BcMean,&BcNoise1,0,(int)(0.05*NPoint));
		BcgrComp->StatRep(&BcMean,&BcNoise2,(int)(0.95*NPoint),NPoint-1);
		BcgrComp->StatRep(&BcMean,&BcNoise,(int)(0.4*NPoint),(int)(0.6*NPoint));
		if(BcNoise1>BcNoise2) BcNoise1=BcNoise2;
		if(BcNoise>BcNoise1)BcNoise=BcNoise1;
		if (PNoise!=NULL) *PNoise = BcNoise;
		//sprintf(StrBuf,"BcMean %f BcNoise %f", BcMean, BcNoise);
		//AfxMessageBox(StrBuf);
		BcgrComp->SmoothCut(0,NPoint-1,0.01);
		BcgrY=BcgrComp->GetData();
		dwL=1.5*Filter->CutFreq;
		dwR=1.5*Filter->FreqH;
		//BcgrComp->StatRep(&BcMean,&BcNoise,0,NPoint-1);
		//sprintf(StrBuf,"%f %f", *(BcgrY+10), *(BcgrY+100));
		//AfxMessageBox(StrBuf);
		if (SearchMode>0)
			Gr=-1; // search for maximums
		else 
			Gr=1;// search for minimums
		CrY=Data+1;
		CrX=Base+1;
		ExR_Y=(*CrY+(*(CrY-1))+(*(CrY+1)))/3.0;
		ExR_X=*CrX;
		PreLockX=ExR_X; PreLockY=fabs(*(BcgrY+1)-ExR_Y);
		j=1;
		do
		//for (j=1;j<NPoint-1;j+=2)
		{
			// Finding current extremum
			do
			{
				ExL_X=ExR_X; ExL_Y=ExR_Y;
				j+=2; CrY+=2; CrX+=2;
				ExR_Y=(*CrY+(*(CrY-1))+(*(CrY+1)))/3.0;
				ExR_X=*CrX;
			}
			while ( (Gr*(ExL_Y-ExR_Y) > 0)&&(j<NPoint-3) ) ;
			// testing extremum
			//discrimination by amplitude and location from previous extremum
			if ( (Gr*(*(BcgrY+j)-ExL_Y)>0.44*BcNoise*Filter->CutAmp)&&(Count<NMax) )
			{
				// PreLockX and Y - location and amplitude of previous extremum
				if ((ExL_X-PreLockX>0.55*dwR)&&(fabs(*(BcgrY+j)-ExL_Y)-PreLockY*exp(1.44*(PreLockX-ExL_X)/dwR)>0.8*BcNoise*Filter->CutAmp))
				{
					StatRep(&SlopeL,&sdN,ExL_X-dwL,ExL_X+dwL);
					StatRep(&SlopeR,&sdN,ExL_X-2*dwL,ExL_X+dwR);
					BcgrComp->StatRep(&BcMean,&sdN,ExL_X-2*dwL,ExL_X+dwR);
					if((Gr*(BcMean-SlopeL)>BcNoise*Filter->CutAmp)&&(Gr*(BcMean-SlopeR)<0.95*BcNoise*Filter->CutAmp))
					{
						*(LockX+Count)= ExL_X;
						Count++;
						PreLockX=ExL_X; PreLockY=fabs(*(BcgrY+j)-ExL_Y);
					}
				}
			
			}
			//Gr*=-1;
			//ExL_X= *CrX;
			//ExL_Y= (*CrY+(*(CrY-1))+(*(CrY+1)))/3.0;
		}
		while( (j<NPoint-3)&&(Count<NMax) );
		//sprintf(StrBuf, "%i",Count);
		//AfxMessageBox(StrBuf);
		delete BcgrComp;
		
	}
	return Count;
}
/*********************************************************************/
int TComponent::ExtremSearch(float dW, float CutAmp, float far* LockX, float far* LockY, int NMax)
{
float far* CrX;
float far* CrY;
int j, Count, Gr;
float ExL_Y,ExL_X,ExR_X,ExR_Y;
CrX=Base;
CrY=Data;
ExL_X= *CrX;
ExL_Y= *CrY;
Count=0;
j=0;
if ( *(CrY+1)>ExL_Y ) Gr=1;
	else Gr=-1;
do
	{
	// Finding the current extremum
	do
		{
		j++; CrX++; CrY++;
		ExR_X= *CrX;
		ExR_Y= *CrY;
		}
	while ( (Gr*(*(CrY+1)-ExR_Y) > 0)&&(j<NPoint-1) ) ;
	// Testing the current extremum
	if ( (ExR_X-ExL_X>=dW)&&(fabs(ExR_Y-ExL_Y)>=CutAmp) )
		{
		if ( ExL_Y > ExR_Y )
			{
			//Returning current pair
			*(LockX+Count)=ExL_X;
			*(LockX+Count+1)=ExR_X;
         *(LockY+Count)=ExL_Y;
			*(LockY+Count+1)=ExR_Y;
			Count+=2;
			}
		ExL_X= *CrX;
		ExL_Y= *CrY;
		Gr*=-1;
		}
	 }
while( (j<NPoint-1)&&(Count<NMax-1) );
return Count;
}
/********************************************************************/
void TComponent::StatRep(float* pMean, float* pSD, float BegX, float EndX)
{
	int jBeg =0;
	int jEnd =1;
	jBeg=FindPoint(BegX)-1;
	jEnd=FindPoint(EndX);
	//jBeg=(int)(NPoint*(BegX-(*Base))/(*(Base+NPoint-1)-(*Base)));
	//jEnd=(int)(NPoint*(EndX-(*Base))/(*(Base+NPoint-1)-(*Base)));
	StatRep(pMean,pSD,jBeg,jEnd);
}
/********************************************************************/
void TComponent::StatRep(float* pMean, float* pSD, int BegPoint, int EndPoint)
{
	int j;
	int jBeg =0;
	int jEnd =1;
	float Mean, SD, Y;
	jBeg=BegPoint;	jEnd=EndPoint;
	if (jBeg<1) jBeg=1;
	if (jEnd>NPoint-1) jEnd=NPoint-1;
	if (jBeg!=jEnd)
	{
		Mean = 0; SD =0;
		for (j=jBeg-1;j<=jEnd;j++)
		{
			Y= *(Data+j);
			Mean+=Y/(jEnd-jBeg+2);
			SD+=Y*Y/(jEnd-jBeg+2);
		}
		*pMean=Mean;
		*pSD=sqrt(fabs(SD-Mean*Mean));
	}
}
/********************************************************************/
float TComponent::Slope (int BegPoint, int EndPoint)
{
	int j, jMin, jMax, jBeg, jEnd;
	float S,Extrem, X,Y;
	float	MX=0;	float MY=0;	float MXY=0;	float MX2=0;
	jBeg = BegPoint; jEnd = EndPoint;
	if (jBeg<1) jBeg=1;
	if (jEnd<1) jEnd=NPoint-1;
	if (jEnd>NPoint-1) jEnd=NPoint-1;
	Extrem= *(Data+jBeg-1);	jMin=jBeg-1;
	for (j=jBeg-1;j<=jEnd;j++)
   	if (*(Data+j)<Extrem)
      	{
         Extrem= *(Data+j);	jMin=j;
         }
	Extrem= *(Data+jBeg-1);	jMax=jBeg-1;
	for (j=jBeg-1;j<jMin;j++)
   		if (*(Data+j)>Extrem)
      	{
			Extrem= *(Data+j);	jMax=j;
        }
   jBeg=(int)(jMax+0.5*(jMin-jMax));
   jEnd=(int)(0.2*(jMin-jMax));
   if (jEnd<3)  jEnd=3;
   if (jEnd>10)	jEnd=10;
   jMax=jBeg+2*jEnd;
   // Mean square root procedure (Kramer's rule)
      for (j=jBeg;j<jMax;j++)
   	{
      X= *(Base+j);	Y= *(Data+j);
      MX+=X;	MX2+=X*X; MY+=Y; MXY+=X*Y;
      }
	S=(MX*MY-2*jEnd*MXY)/(2*jEnd*MX2-MX*MX);
	return S;
}
/********************************************************************/
float TComponent::Slope (float BegX, float EndX)
{
	int jBeg=(NPoint*(BegX-(*Base))/(*(Base+NPoint-1)-(*Base)));
	int jEnd=(NPoint*(EndX-(*Base))/(*(Base+NPoint-1)-(*Base)));
	if (jBeg<1) jBeg=1;
	if (jEnd>NPoint-1) jEnd=NPoint-1;
	return Slope(jBeg, jEnd);

}

/********************************************************************/
TComponent::TComponent(float* PtrData, float*  PtrBase, int DestN)
{
	StimPoint=101;
	NPoint=DestN;
	Base=PtrBase;
	Data=PtrData;
	CreateMode=MEM_NONE;
	if (NPoint<N_Spline)
		{
		//Splines=Data+NPoint+1;
		Splines=new float[DestN+1];
		IsSplined=1;
		CreateMode=MEM_SPLINE;
		}
		else IsSplined=0;
   NA=0;   
}
/********************************************************************/
TComponent::TComponent(float* PtrBase, int DestN)
{
	NPoint=DestN;
	StimPoint=101;
	Base=PtrBase;
	if (NPoint<N_Spline)
    	{
		Data=new float [2*NPoint+1];
		Splines=Data+NPoint;
		IsSplined=1;
		}
	else
		{
        Data=new float[NPoint];
		IsSplined=0;
		}
	CreateMode=MEM_DATA;
   NA=0;
}
/********************************************************************/
TComponent::TComponent(int DestN)
{
	NPoint=DestN;
	StimPoint=101;
	if (NPoint<N_Spline)
    	{
		Data=new float[2*NPoint+1];
		Splines=Data+NPoint;
		IsSplined=1;
		}
	else
		{
        Data=new float[NPoint];
		IsSplined=0;
		}
    Base=new float[NPoint];
	CreateMode=MEM_BASE;
	NA=0;
	
}
/********************************************************************/
TComponent::~TComponent()
{
	if (CreateMode==MEM_DATA)	delete[] Data;
	if (CreateMode==MEM_SPLINE)	delete[] Splines;
	if (CreateMode==MEM_BASE)	{	delete[] Data;	delete[] Base;	}
}
/********************************************************************/
void TComponent::Set(float far* PtrData, float far*  PtrBase, int DestN)
	{
	NPoint=DestN;
	Base=PtrBase;
	Data=PtrData;
	if (NPoint<N_Spline)
		{
		Splines=Data+NPoint;
		IsSplined=1;
		}
		else IsSplined=0;
}
/*********************************************************************/
float far* TComponent::GetData()
{
	return Data;
}
/********************************************************************/
void TComponent::SetData(float far* Data2Set)
{
	for (int j=0;j<NPoint;j++)
		*(Data+j)=(*(Data2Set+j));
}
/********************************************************************/
void TComponent::InitData(float Value)
{
	for (int j=0;j<NPoint;j++)
		(*(Data+j))=Value;
}


/**********************************************************************/
void TComponent::SetFilter(TParam* NewPar)
{
	if (NewPar!=NULL)
   	{
		FilterPar=NewPar;
   		if ((FilterPar->Mode==CLAMP_ST)||(FilterPar->Mode==CLAMP_FST))
	  	FilterOn=1;
     else FilterOn=0;
	}
    else
	{
		FilterPar=NULL; 
		FilterOn=0;
	}

}
/**********************************************************************/
void TComponent::Reset(TParam* NewPar)
{
	char szNum[10];
	const int MaxNSite=200;
	float	Prb_K[MaxNSite+1];
	float	SitePrb[MaxNSite+1];
	float X_K[MaxNSite+1];
	short int SiteIndex[MaxNSite];
	short int SiteSel[MaxNSite+1];
	float Beta,Beta2,X,dX,p;
	float Wid,Max,Spl,Y,S,Z;
	float	tr,td,t0,tn,tm,Fmax;
	// Cable model
	float R,L,Roo,an,an2,An,a0,a1,f0,f1,tga,R2,NRelease;
	float Aeq, t,tau, dtau,D,D2,al,dl,dR;
	float Simps[50];
	int NSimps=20;
	//Kinetics model
	float KDes, K_Des, alp, bet,	KD, K1, K_1, K2, K_2;
	double Uptake,Fglu,glu, Kup,K12; // coeeficients for imporved uptake model
	
	float		Mediat;
	//float*	MediatX;
	//float*	MediatY;
	//int	NMediat=1000;
	float Crit=3;
	
	TParam MediatPar;
	int j,n,k,i,m,crIndex,NStem;
	div_t Div;
	float mix;
	if ( NewPar->Wid > NewPar->WidH ) Wid=NewPar->WidH;
		else
		if ( NewPar->Wid < NewPar->WidL ) Wid=NewPar->WidL;
			else
			Wid=NewPar->Wid;
	if ( NewPar->Mix > NewPar->MixH ) mix=NewPar->MixH;
		else
		if ( NewPar->Mix < NewPar->MixL ) mix=NewPar->MixL;
			else
			mix=NewPar->Mix;
	if ( NewPar->Time > NewPar->TimeH ) tm=NewPar->TimeH;
		else
		if ( NewPar->Time < NewPar->TimeL ) tm=NewPar->TimeL;
			else
			tm=NewPar->Time;
	
	if ((NewPar->Mode==CLAMP_ST)||(NewPar->Mode==CLAMP_FST)||(NewPar->Mode==CLAMP_FST2))
      {
      if ( NewPar->Max > 10*NewPar->MaxH ) Max=10*NewPar->MaxH;
			else
			if ( NewPar->Max < NewPar->MaxL ) Max=NewPar->MaxL;
				else
				Max=NewPar->Max;
      }
	else
   	{
		if ( NewPar->Max > NewPar->MaxH ) Max=NewPar->MaxH;
			else
			if ( NewPar->Max < NewPar->MaxL ) Max=NewPar->MaxL;
				else
				Max=NewPar->Max;
      }
	if ( NewPar->Split > NewPar->SplitH ) Spl=NewPar->SplitH;
		else
		if ( NewPar->Split < NewPar->SplitL ) Spl=NewPar->SplitL;
			else
			Spl=NewPar->Split;
   //Max=40;
   //Wid=10;
   //Spl=100;
	if (fabs(Wid)<MinFloat) 
		if (Wid>0)	Wid=MinFloat;
		else		Wid=-MinFloat;
	NewPar->Wid=Wid;
	NewPar->Max=Max;
	NewPar->Split=Spl;
	NewPar->Time=tm;
	NewPar->Mix=mix;
	//sprintf(szNum, "%i", NewPar->Mode);
	//AfxMessageBox(szNum);
	
	//NewPar->Mode=RIDEC_I;
	NStem=NewPar->Mult;
	if (NStem < 1) NStem=1;
	if (FilterPar!=NULL)
	{
   	if ((FilterPar->Mode==CLAMP_ST)||(FilterPar->Mode==CLAMP_FST)||(FilterPar->Mode==CLAMP_FST2))
	{
		NStem=FilterPar->Mult;
        tm=FilterPar->Time;
        FilterOn=1;
        R=FilterPar->Max;	L=FilterPar->Wid;	S=FilterPar->Split;	Roo=FilterPar->Mix;
   		//tr=0.001*tm;
        tr=0.1*(*(Base+NPoint-1)-(*Base))/NPoint;
      	crIndex=0;	R2=R*S+R+S+1;
        //NStem=1;
        mix=0;
        if (!(FilterPar->Mult))
		{
			k=2*(int)(L*sqrt(tm/tr)/6.283-0.25);
   			if (k>100)k=100;
      		if (k<16)k=16;
        }
        else
            k=FilterPar->Mult;
      	if (FilterPar->Mode==CLAMP_ST)
      	{
			do
      		{
				a0=0.5*(2*crIndex+0.9)*PI/L;
      			a1=0.5*(2*crIndex+1.1)*PI/L;
         		j=0;
		        f0=a0*Roo*sin(a0*L)-(R2-a0*a0)*cos(a0*L);
         		while ((fabs(a1-a0)>0.001*fabs(a0))&&(j<25))
         		{
					//f1=a1*Roo*R2*sin(a1*L)-R*(R2-a1*a1)*cos(a1*L);
					f1=a1*Roo*sin(a1*L)-(R2-a1*a1)*cos(a1*L);
   	         		tga=(f1-f0)/(a1-a0);
	   				if ((tga<0)&&(fabs(tga)<MinFloat))tga=-MinFloat;
   	   				if ((tga>0)&&(fabs(tga)<MinFloat))tga=MinFloat;
      	   			a0=a1;	f0=f1;  	a1-=f1/tga;
					if (a1<0.5*(2*crIndex+0.5)*PI/L) a1=0.5*(2*crIndex+0.5)*PI/L;
					if (a1>0.5*(2*crIndex+1.5)*PI/L) a1=0.5*(2*crIndex+1.5)*PI/L;
            		j++;
	            }
	         	an=fabs(a1);
		        *(Prb_K+crIndex)=an;
   				td=tm/(1+an*an);
      			*(X_K+crIndex)=td;
         		crIndex++;
				mix+=fabs((an-(0.5*(2*crIndex+1)*PI/L))/(0.5*(2*crIndex+1)*PI/L));
         	}
            while (crIndex<=k);
      		//while ((td>tr)&&(crIndex<MaxNSite));
            mix/=crIndex;
            NewPar->Mix=mix;
            n=(int)(L*sqrt(R2)/PI-0.5);	an=sqrt(R2);
            *(Prb_K+n)=an;
            td=tm/(1+an*an);
      		*(X_K+n)=td;
      	}
      	else
        if (FilterPar->Mode==CLAMP_FST)
      	{
            //NStem=1;
         	an=sqrt(R2);
      		*Prb_K=an;
      		*X_K=tm/(1+an*an);
      		crIndex++;
         	do
         	{
				an=0.5*(2*crIndex+1)*PI/L;
				a1=atan(an*Roo/(R2-an*an));
				an-=a1;
				*(Prb_K+crIndex)=an;
   				td=tm/(1+an*an);
      			*(X_K+crIndex)=td;
         		crIndex++;
            }
         	//while ((td>tr)&&(crIndex<MaxNSite));
            while (crIndex<=k);
         }
         else
         {
			//NStem=1;
			//an=sqrt(R2);
      		//*Prb_K=an;
      		//*X_K=tm/(1+an*an);
      		//crIndex++;
         	do
			{	
				//an=0.5*(2*crIndex-1)*PI/L;
                an=0.5*(2*crIndex+1)*PI/L;
                a1=atan(an*Roo/(R2-an*an));
               	an-=a1;
			    *(Prb_K+crIndex)=an;
   			    td=tm/(1+an*an);
      			*(X_K+crIndex)=td;
         		crIndex++;
            }
	        //while ((td>tr)&&(crIndex<MaxNSite));
            while (crIndex<=k);
         }
         NA=crIndex;
         for (j=0;j<NA;j++)
         	{
            *(EgV+j)= *(Prb_K+j);
            *(EgF+j)=0;
            }
		}
		else FilterOn=0;
    }
	else FilterOn=0;
	switch (NewPar->Mode)
	{
      case CLAMP_ST: case CLAMP_FST: case CLAMP_FST2:
      // X_K - tn, Prb_K - an,  Max - R, Wid - L, Split - tm, mix - Roo.
      R=Max; // R_soma/R_access
      L=Wid; //cable length
      S=Spl; //shunt to soma conductance ratio ;
      Roo=mix;  //R_soma/R_infinite dendritic cable ;
      t0=tm; // membrane time constant ;
      tr=0.2*(*(Base+NPoint-1)-(*Base))/NPoint; //minimal time conctant ;
      crIndex=0;
      R2=R*S+R+S+1;
      k=(int)(L*sqrt(tm/tr)/6.283-0.25);
      if (k>50)k=50;
      if (k<8)k=8;

      if (NewPar->Mode==CLAMP_ST)
      {
      do
      	{
         //crIndex++;
         //finding eigenvalues
         a0=0.5*(2*crIndex+0.9)*PI/L;
         a1=0.5*(2*crIndex+1.1)*PI/L;
         j=0;
         //f0=a0*Roo*sin(a0*L)-(R-a0*a0)*cos(a0*L);
         f0=a0*Roo*sin(a0*L)-(R2-a0*a0)*cos(a0*L);
         while ((fabs(a1-a0)>0.001*fabs(a0))&&(j<50))
         	{
            f1=a1*Roo*sin(a1*L)-(R2-a1*a1)*cos(a1*L);
            tga=(f1-f0)/(a1-a0);
            if ((tga<0)&&(tga>-MinFloat))tga=-MinFloat;
            if ((tga>0)&&(tga<MinFloat))tga=MinFloat;
            a0=a1;
            f0=f1;
            a1-=f1/tga;
            j++;
            }
         an=a1;

         //an=0.5*(2*crIndex+1)*PI/L;
         //an*=(1-Roo/(L*R-L*an*an+Roo));
         *(Prb_K+crIndex)=an;
         td=t0/(1+an*an);
         *(X_K+crIndex)=td;
         crIndex++;
         }
      while (crIndex<=4*k);
      }
      else
      if (NewPar->Mode==CLAMP_FST)
      	{
         //NStem=2;
         an=sqrt(R2);
      	*Prb_K=an;
      	*X_K=t0/(1+an*an);
      	crIndex++;
         do
         	{
            an=0.5*(2*crIndex+1)*PI/L;
	         *(Prb_K+crIndex)=an;
   	      td=t0/(1+an*an);
      	   *(X_K+crIndex)=td;
         	crIndex++;
            }
         while (crIndex<=4*k);
         }
         else
         	{
            //NStem=8;
            //an=sqrt(R2);
 	     		//*Prb_K=an;
	      	//*X_K=t0/(1+an*an);
   	   	//crIndex++;
      	   do
         		{
            	an=0.5*(2*crIndex+1)*PI/L;
		         *(Prb_K+crIndex)=an;
   		      td=t0/(1+an*an);
      		   *(X_K+crIndex)=td;
         		crIndex++;
            	}
	         while (crIndex<=4*k);
            }
      X=*(Base+StimPoint);
      for (j=0;j<StimPoint+1;j++)
      	*(Data+j)=1;
      for (j=StimPoint+1;j<NPoint;j++)
      	*(Data+j)=1-1/(1+R*R2/(R2+NStem*R*Roo*tanh(L)));
      for (n=0;n<crIndex;n++)
      	{
         td= *(X_K+n);
         an= *(Prb_K+n);
         an2=an*an;
         //An=2*an2*R/(an*(2*an+R-an2)+L*(R-an2)*(R-an2)/Roo);
         Beta=(R2-an2)/(an*Roo);
         Beta2=1+Beta*Beta;
         An=2*R2/((1+R)*(R2+an2)/(R*an2)+NStem*L*Roo*Beta2);
         An/=(1+an2);
         for(j=StimPoint;j<NPoint;j++)
         	{
            Beta=(*(Base+j)-X)/td;
            *(Data+j)-=An*exp(-Beta);
            }
         }
      	break;
		case GAUSLOR:
			for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				Beta=2*(X-Max)/Wid;
				Beta*=Beta;
				*(Data+j)=mix*exp(-0.6931*Beta)+(1-mix)/(1+Beta);
				}
			break;
      case UNIMODAL: //gaussian distribution
			for (j=0;j<NPoint;j++)
			{
				X= *(Base+j);
				Beta=(X-Max)/Wid;
				Beta*=Beta; Beta/=2;
				//*(Data+j)=0.3989*exp(-Beta)/Wid;
				*(Data+j)=exp(-Beta);
			}
			break;
		case BIMODAL: // double gaussian
			// first peak: max,wid; 2-nd : -shift=split, rel.fraction = mix, rel.width = tm;
			// 1st peak
			for (j=0;j<NPoint;j++)
			{
				X= *(Base+j);
				Beta=(X-Max)/Wid;
				Beta*=Beta; Beta/=2;
				//*(Data+j)=0.3989*exp(-Beta)/Wid;
				*(Data+j)=(1-mix)*exp(-Beta);
			}
			//2nd peak
			for (j=0;j<NPoint;j++)
			{
				X= *(Base+j);
				Beta=(X-Max+Spl)/(tm*Wid);
				Beta*=Beta; Beta/=2;
				//*(Data+j)=0.3989*exp(-Beta)/Wid;
				*(Data+j)+=mix*exp(-Beta);
			}
			break;
      case SKEW_UN:
			for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
            if (X<Max)
            	Beta=(X-Max)/Wid;
            else
               Beta=(X-Max)/(Spl*Wid);
            Beta*=Beta; Beta/=2;
				//*(Data+j)=0.3989*exp(-Beta)/Wid;
            *(Data+j)=exp(-Beta);
				}
			break;
		case ABSORB:
			if (mix>0.003)
			  for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				Beta=2*(X-Max-0.5*Spl)/Wid;
				Beta*=Beta;
				*(Data+j)=1-mix*exp(-0.6931*Beta)-(1-mix)/(1+Beta);
				Beta=2*(X-Max+0.5*Spl)/Wid;
				Beta*=Beta;
				*(Data+j)-=mix*exp(-0.6931*Beta)+(1-mix)/(1+Beta);
				}
			  else
				for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				Beta=2*(X-Max-0.5*Spl)/Wid;
				Beta*=Beta;
				*(Data+j)=1-1/(1+Beta);
				Beta=2*(X-Max+0.5*Spl)/Wid;
				Beta*=Beta;
				*(Data+j)-=1/(1+Beta);
				}
			break;
		case LORDIR:
		for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				Beta=2*(X-Max)/Wid;
				Beta2=Beta*Beta;
				*(Data+j)=0.5-1.5396*Beta/((1+Beta2)*(1+Beta2));
				}
		break;
		case GAUSDIR:
			 for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				Beta=2*(X-Max)/Wid;
				Beta2=Beta*Beta;
				*(Data+j)=-1.3862*Beta*exp(-0.6931*Beta2);
				}
			break;
		case SINUS:
			for (j=0;j<NPoint;j++)
				*(Data+j)=mix*sin(6.28312*(*(Base+j)+Max)/Wid);
			break;
		case RIDEC_O:
			 tr=Wid; td=Spl;
          X=0.00003*(*(Base+NPoint-1)-(*Base));
          if (tr<X)tr=X;
          X*=2;
          if (td<X)td=X;
			 //t0=log(td/tr)*tr*td/(td-tr);
          t0=log(td/tr)/(td-tr);
			 Fmax=exp(-t0*tr)-exp(-t0*td);
          if(FilterOn)
          	{
            for(j=0;j<NPoint;j++) *(Data+j)=0;
          	for (n=0;n<crIndex;n++)
            	{
	   	  	 	tn= *(X_K+n);
               if ((tn-td<0)&&(tn-td)>-MinFloat) tn=td-MinFloat;
               if ((tn-td>0)&&(tn-td)<MinFloat) tn=td+MinFloat;
               //if (tn-td>0) tn=td-MinFloat;
               if ((tn-tr<0)&&(tn-tr)>-MinFloat) tn=tr-MinFloat;
               if ((tn-tr>0)&&(tn-tr)<MinFloat) tn=tr+MinFloat;
	   	      an= *(Prb_K+n);
   	   	   an2=an*an;
               //tg(an):
               Beta2=(R2-an2)/(Roo*an);
               Beta2*=Beta2;
               Beta2+=1;
               Div=div(n,2);
               if(Div.rem)Beta=sqrt(fabs(Beta2));
               else
						Beta=-sqrt(fabs(Beta2));

               if ((!n)&&(FilterPar->Mode==CLAMP_FST))
                  {
                  Beta2=tan(an*L);Beta2*=Beta2;
                  if(fabs(cos(an*L))>MinFloat)
                  	Beta=1.0/cos(an*L);
                  }

               //if ((FilterPar->Mode==CLAMP_ST)&&(fabs(cos(an*L))>MinFloat))
					if (fabs(cos(an*L))>MinFloat)
               	{
               	Beta=1.0/cos(an*L);
                  Beta2=Beta*Beta;
                  }

               //An=-2*fabs(cos(0.5*an*L))/tm;
               An=-2*cos(0.5*an*L)/tm;
					//An=-2/tm;
               //An=-2*(td-tr)/tm;
               An*=R2*Beta;//tanh(L);
               An/=((1+1/R)*(R2+an2)/an2+Roo*L*Beta2);
               *(EgF+n)=An;
         		for(j=0;j<NPoint;j++)
         			{
                  X= *(Base+j);
						if (X<Max) X=Max;
                  Beta=(Max-X)/tr;
						Beta2=(Max-X)/td;
                  Y=(An/Fmax)*exp((Max-X)/tn)*(tn*tn*(tr-td))/((tn-tr)*(tn-td));
                  Y=+(An/Fmax)*exp(Beta)*(tn*tr)/(tr-tn);
                  Y-=(An/Fmax)*exp(Beta2)*(tn*td)/(td-tn);
                  *(Data+j)-=Y;

         		}
            //if ((FilterPar->Mode==CLAMP_FST2)||(FilterPar->Mode==CLAMP_FST))
            	for(j=0;j<NPoint;j++)
            		{
               	Y= *(Data+j);
                  //*(Data+j)=Y;
               	*(Data+j)=fabs(Y);
               	}


            }
          else
			 for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				if (X<Max) X=Max;
				Beta=(Max-X)/tr;
				Beta2=(Max-X)/td;
				*(Data+j)=(exp(Beta2)-exp(Beta))/Fmax;
				}
			break;
		case RIDEC_I:
			 tr=Wid; td=Spl;
			 X=0.00003*(*(Base+NPoint-1)-(*Base));
          if (tr<X)tr=X;
          X*=2;
          if (td<X)td=X;
			 //t0=log(td/tr)*tr*td/(td-tr);
          t0=log(td/tr)/(td-tr);
			 Fmax=exp(-t0*tr)-exp(-t0*td);
          if(FilterOn)
          	{
            for(j=0;j<NPoint;j++) *(Data+j)=0;
          	for (n=0;n<crIndex;n++)
            	{
	   	  	 	tn= *(X_K+n);
               if ((tn-td<0)&&(tn-td)>-MinFloat) tn=td-MinFloat;
               //if (tn-td>0) tn=td-MinFloat;
               if ((tn-td>0)&&(tn-td)<MinFloat) tn=td+MinFloat;
               if ((tn-tr<0)&&(tn-tr)>-MinFloat) tn=tr-MinFloat;
               if ((tn-tr>0)&&(tn-tr)<MinFloat) tn=tr+MinFloat;
	   	      an= *(Prb_K+n);
   	   	   an2=an*an;
               Beta2=(R2-an2)/(Roo*an);
               Beta2*=Beta2;
               Beta2+=1;
               Div=div(n,2);
               if(Div.rem)Beta=sqrt(fabs(Beta2));
               else
						Beta=-sqrt(fabs(Beta2));
               if ((!n)&&(FilterPar->Mode==CLAMP_FST))
                  {
                  Beta2=tan(an*L);Beta2*=Beta2;
                  if(fabs(cos(an*L))>MinFloat)
                  	Beta=1.0/cos(an*L);
                  }

               //if ((FilterPar->Mode==CLAMP_ST)&&(fabs(cos(an*L))>MinFloat))
               if (fabs(cos(an*L))>MinFloat)
                  {
               	Beta=1.0/cos(an*L);
                  Beta2=Beta*Beta;
                  }
               //Beta=sqrt(Beta2);
               An=-2*cos(0.5*an*L)/tm;
					//An=-2/tm;
               An*=R2*Beta;//tanh(L);
      		   //An/=((1+1/R)*(1+2*an2)/an2+Roo*L*Beta2/R);
               An/=((1+1/R)*(R2+an2)/an2+Roo*L*Beta2);
               *(EgF+n)=An;
         		for(j=0;j<NPoint;j++)
         			{
                  X= *(Base+j);
						if (X<Max) X=Max;
                  Beta=(Max-X)/tr;
						Beta2=(Max-X)/td;
                  Y=(An/Fmax)*exp((Max-X)/tn)*(tn*tn*(tr-td))/((tn-tr)*(tn-td));
                  Y+=(An/Fmax)*exp(Beta)*(tn*tr)/(tr-tn);
                  Y-=(An/Fmax)*exp(Beta2)*(tn*td)/(td-tn);
                  *(Data+j)+=Y;
      	      	}
         		}
            //if ((FilterPar->Mode==CLAMP_FST2)||(FilterPar->Mode==CLAMP_FST))
            	for(j=0;j<NPoint;j++)
            		{
               	Y= *(Data+j);
                  //*(Data+j)=1-Y;
               	*(Data+j)=1-fabs(Y);
               	}

            }
          else
			 for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				if (X<Max) X=Max;
				Beta=(Max-X)/tr;
				Beta2=(Max-X)/td;
				*(Data+j)=1-(exp(Beta2)-exp(Beta))/Fmax;
				}
			break;
      case	STEPRESP:
      	if ( NStem <=1)
         	{
            NStem=100;
            NewPar->Mult=NStem;
            }
         tr=Wid; td=Spl;
         X=0.001*(*(Base+NPoint-1)-(*Base));
         if (tr<X)tr=X;
         X*=3;
         if (td<X)td=X;
         t0=log(td/tr)/(td-tr);
		 	Fmax=exp(-t0*tr)-exp(-t0*td);
         if (mix>1) mix=1;
         if (mix<0) mix=0;
      	for (j=0;j<NPoint;j++)
         	{
				X= *(Base+j);
				if (X<Max) Beta=0;
            	else	Beta=(Max-X)/tr;
            if (X<Max+tm) Beta2=0;
					else Beta2=(Max+tm-X)/(0.01*NStem*td);
				*(Data+j)=(1-mix)*(1-exp(Beta2)+exp(Beta));
            //*(Data+j)=exp(Beta);
				}
         for (j=0;j<NPoint;j++)
				{
				X= *(Base+j);
				if (X<Max) X=Max;
				Beta=(Max-X)/tr;
				Beta2=(Max-X)/td;
				*(Data+j)+=mix*(1-(exp(Beta2)-exp(Beta))/Fmax);
				}
      	break;

      case COMPOUND:
      	 n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
         p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
         if (Wid>0.8*Spl) Wid=0.8*Spl;
         SitePrb[1]=p*(1-mix);
         if (SitePrb[1]<MinFloat)SitePrb[1]=MinFloat;
         SitePrb[n]=p*(1+mix);
         if(SitePrb[n]>0.99999)SitePrb[n]=0.99999;
         p=0.5*(SitePrb[n]+SitePrb[1]);
         mix=0.5*(SitePrb[n]-SitePrb[1])/p;
         NewPar->Max=p;
         NewPar->Mix=mix;
         for (i=2;i<n;i++)
         	SitePrb[i]=SitePrb[1]+i*(SitePrb[n]-SitePrb[1])/n;
         *Prb_K=1;
         for (i=1;i<=n;i++)
         	*(Prb_K)*=(1-SitePrb[i]);
         //computing compound probabilities
         *(Prb_K+n)=1;
         for (i=1;i<=n;i++)
         	*(Prb_K+n)*=SitePrb[i];
         for(k=1;k<n;k++)
         	{
            S=1;
            for (i=1;i<=k;i++)
            	S*=SitePrb[i];
            for (i=k+1;i<=n;i++)
            	S*=(1-SitePrb[i]);
            *(Prb_K+k)=S;
            for (i=1;i<=k;i++)
            	SiteIndex[i]=i;
            crIndex=k;
            SiteIndex[0]=0;
            do
            	{
               if (SiteIndex[crIndex]<n)
              		SiteIndex[crIndex]++;
              	else
               	{
                  while ((SiteIndex[crIndex]==n-k+crIndex)&&(crIndex>0))
                  	crIndex--;
                  if (crIndex)
                     {
                     SiteIndex[crIndex]++;
                     for (i=crIndex+1;i<=k;i++)
                  		SiteIndex[i]=SiteIndex[i-1]+1;
                     crIndex=k;
                     }
                  }
               for (i=1;i<=n;i++)
               	SiteSel[i]=0;
               for (i=1;i<=k;i++)
               	SiteSel[SiteIndex[i]]=1;
               S=1;
               for (i=1;i<=n;i++)
               	if (SiteSel[i])
                  	S*=SitePrb[i];
                  else
                  	S*=(1-SitePrb[i]);
               *(Prb_K+k)+=S;
              	}
            while (crIndex>=1);
            }
         *X_K=0;
         for (k=1;k<=n;k++)
            {
         	*(X_K+k)=k*Spl;
            //*(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
            }

         S=0;

         for (j=0;j<NPoint;j++)
         	{
            if(j) dX=(*(Base+j)-(*(Base+j-1)));
            else  dX=(*(Base+j+1)-(*(Base+j)));
            *(Data+j)=0;
            X= *(Base+j)-0.5*dX;
            for (i=0;i<10;i++)
               {
               Y=0;
               for (k=0;k<=n;k++)
                  {
                  Beta=(X-(*(X_K+k)))/Wid;
                  Beta*=Beta;	Beta/2;
               	Y+=0.3989*(*(Prb_K+k))*exp(-Beta)/Wid;
                  }
               *(Data+j)+=Y*0.1;
               X+=0.1*dX;
               }
            if (S<*(Data+j)) S= *(Data+j);
            }
         if (S<MinFloat)S=MinFloat;
         //NewPar->Zero=0;
         //NewPar->Amp=S;
         for (j=0;j<NPoint;j++)
         	(*(Data+j))/=S;
        break;
      case BINOM:case BINOM_QU:
      	n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
         p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
         if (NewPar->Mode==BINOM_QU)
         	Spl=Wid;
            //else
		      //   if (Wid>0.8*Spl) Wid=0.8*Spl;
         *Prb_K=1;
         for (i=1;i<=n;i++)
         	*(Prb_K)*=(1-p);
         //computing compound probabilities
         *(Prb_K+n)=1;
         *X_K=0;
         for (k=1;k<=n;k++)
            {
         	*(X_K+k)=k*Spl;
            *(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
            }

         S=0;
         for (j=0;j<NPoint;j++)
         	{
            if(j) dX=(*(Base+j)-(*(Base+j-1)));
            else  dX=(*(Base+j+1)-(*(Base+j)));
            *(Data+j)=0;
            X= *(Base+j)-0.5*dX;
            for (i=0;i<10;i++)
               {
               Y=0;
               for (k=0;k<=n;k++)
                  {
                  Beta=(X-(*(X_K+k)))/(Wid*(1+k*tm));
                  Beta*=Beta;	Beta/2;
               	Y+=0.3989*(*(Prb_K+k))*exp(-Beta)/(Wid*(1+k*tm));
                  }
               *(Data+j)+=Y*0.1;
               X+=0.1*dX;
               }
            if (S<*(Data+j)) S= *(Data+j);
            }
         if (S<MinFloat)S=MinFloat;
         //NewPar->Zero=0;
         //NewPar->Amp=S;
         for (j=0;j<NPoint;j++)
         	(*(Data+j))/=S;
        break;
		case BINOM_NZ:
      		n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
			p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
			*Prb_K=1;
			for (i=1;i<=n;i++)
         		*(Prb_K)*=(1-p);
			if (*Prb_K>0.999999)*Prb_K=0.999999;
				 //computing compound probabilities
			*(Prb_K+n)=1;
			*X_K=0;
			for (k=1;k<=n;k++)
			{
         		*(X_K+k)=k*Spl;
				*(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
				*(Prb_K+k)/=(1-(*(Prb_K)));
			}
			S=0;
			for (j=0;j<NPoint;j++)
         	{
				if(j) dX=(*(Base+j)-(*(Base+j-1)));
					else  dX=(*(Base+j+1)-(*(Base+j)));
				*(Data+j)=0;
				X= *(Base+j)-0.5*dX;
				for (i=0;i<10;i++)
				{
					Y=0;
					for (k=1;k<=n;k++)
					{
						Beta=(X-(*(X_K+k)))/(Wid*(1+k*tm));
						Beta*=Beta;	Beta/2;
               			Y+=0.3989*(*(Prb_K+k))*exp(-Beta)/(Wid*(1+k*tm));
					}
					*(Data+j)+=Y*0.1;
					X+=0.1*dX;
				}
				if (S<*(Data+j)) S= *(Data+j);
			}
			if (S<MinFloat)S=MinFloat;
			for (j=0;j<NPoint;j++)
         		(*(Data+j))/=S;
			break;
      case BINOM_EF:
      	n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
         p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
         //if (Wid>0.8*Spl) Wid=0.8*Spl;
         *Prb_K=1;
         for (i=1;i<=n;i++)
         	*(Prb_K)*=(1-p);
         //computing compound probabilities
         *(Prb_K+n)=1;
         *X_K=0;
         for (k=1;k<=n;k++)
            {
         	*(X_K+k)=k*Spl;
            *(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
            }

         S=0;
         for (k=1;k<=n;k++)
	            (*(Prb_K+k))*=(1-(1+mix)*(*Prb_K))/(1-(*Prb_K));
         (*Prb_K)*=(1+mix);
         for (j=0;j<NPoint;j++)
         	{
            if(j) dX=(*(Base+j)-(*(Base+j-1)));
            else  dX=(*(Base+j+1)-(*(Base+j)));
            *(Data+j)=0;
            X= *(Base+j)-0.5*dX;
            for (i=0;i<10;i++)
               {
               Y=0;
               for (k=0;k<=n;k++)
                  {
                  Beta=(X-(*(X_K+k)))/(Wid*(1+k*tm));
                  Beta*=Beta;	Beta/2;
               	Y+=0.3989*(*(Prb_K+k))*exp(-Beta)/(Wid*(1+k*tm));
                  }
               *(Data+j)+=Y*0.1;
               X+=0.1*dX;
               }
            if (S<*(Data+j)) S= *(Data+j);
            }
         if (S<MinFloat)S=MinFloat;
         //NewPar->Zero=0;
         //NewPar->Amp=S;
         for (j=0;j<NPoint;j++)
         	(*(Data+j))/=S;
        break;
      case BINOM_SUB:
      	n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
         p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
         if (mix<MinFloat)mix=MinFloat;
         //if (Wid>0.8*Spl) Wid=0.8*Spl;
         *Prb_K=1;
         for (i=1;i<=n;i++)
         	*(Prb_K)*=(1-p);
         //computing compound probabilities
         *(Prb_K+n)=1;
         *X_K=0;
         for (k=1;k<=n;k++)
            {
         	*(X_K+k)=k*Spl/(1+k*Spl/mix);
            *(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
            }

         S=0;
         for (j=0;j<NPoint;j++)
         	{
            if(j) dX=(*(Base+j)-(*(Base+j-1)));
            else  dX=(*(Base+j+1)-(*(Base+j)));
            *(Data+j)=0;
            X= *(Base+j)-0.5*dX;
            for (i=0;i<10;i++)
               {
               Y=0;
               for (k=0;k<=n;k++)
                  {
                  Beta=(X-(*(X_K+k)))/(Wid*(1+k*tm));
                  Beta*=Beta;	Beta/2;
               	Y+=0.3989*(*(Prb_K+k))*exp(-Beta)/(Wid*(1+k*tm));
                  }
               *(Data+j)+=Y*0.1;
               X+=0.1*dX;
               }
            if (S<*(Data+j)) S= *(Data+j);
            }
         if (S<MinFloat)S=MinFloat;
         //NewPar->Zero=0;
         //NewPar->Amp=S;
         for (j=0;j<NPoint;j++)
         	(*(Data+j))/=S;
        break;
      case DIFFUS_1:
      	// this version is improvement of last DIFFUS_2, with correction on dependence of Uptake on [Glu]
		SourceComp=new TComponent(DiffSourceData,DiffSourceBase,DiffSize);
		R=Max; D=Wid; Z=mix;
		if (NStem < 1)	NStem=1;
		MediatCalc(R,D,Z,Spl, NStem, Base, Data, NPoint);
		delete SourceComp;
		break;
      case DIFFUS_2:
		R=Max; D=Wid; Z=mix;
		al=0.1; dl=1.34;	D2=D/(dl*dl);
        if (NStem < 1)	NStem=1;
        NRelease=(1.59e-06)*4000*NStem;
        X= *(Base+StimPoint);
        for (i=1;i<=NSimps/2-1;i++)
			{
            Simps[i*2-1]=4;	Simps[i*2]=2;
            }
        Simps[0]=1;	Simps[NSimps-1]=1;
        for (j=0;j<=StimPoint;j++)
			*(Data+j)=0;
         //for (j=StimPoint+1;j<NPoint;j++)
		X=0;
		if (R>Rcleft)
		{
			
			
			
			Uptake=Spl*(3.14159*Rcleft*Rcleft*h)*1.0e-6;
			SourceComp=new TComponent(DiffSourceData,DiffSourceBase,DiffSize);
			
			if (DiffSourceOn)
			{
				for (j=0;j<DiffSize;j++)
					*(DiffSourceBase+j)=j*10.5/DiffSize;
				DiffSourceCalc(Rcleft, D, Uptake, NStem, DiffSourceBase, DiffSourceData, DiffSize);
				*(SourceComp)/=al;
				*(SourceComp)*=NRelease;
			}
			
			
			//Uptake=Spl*(3.14159*Rmax*Rmax*h)*1.0e-3;
			Uptake=Spl*1.0e-3; // units: mM/msec
            
			for (j=1;j<NPoint;j++)
			{
				t= *(Base+j);
				S=0;
				if (t>(3/Sigma)*(NSimps+1)/NSimps)
            	{
               		dtau=(3/Sigma)/NSimps;
					for (i=0;i<NSimps;i++)
					{
      	   				tau=(i+1)*dtau;
						Mediat=SourceComp->OutSpline(tau,1); //diffusion source
         	   			S+=(dtau/3)*Simps[i]*(Mediat-Uptake)*exp(-(Z*Z+(R-Rcleft)*(R-Rcleft))/(4*D2*(t-tau)))/exp(1.5*log(12.57*D2*(t-tau)));
						
					}
					dtau=(0.99*t-(3/Sigma)*(NSimps+1)/NSimps)/(NSimps-1);
   					for (i=0;i<NSimps;i++)
      				{
         				tau=(3/Sigma)*(NSimps+1)/NSimps+i*dtau;
            			Mediat=SourceComp->OutSpline(tau,1); //diffusion source
         	   			S+=(dtau/3)*Simps[i]*(Mediat-Uptake)*exp(-(Z*Z+(R-Rcleft)*(R-Rcleft))/(4*D2*(t-tau)))/exp(1.5*log(12.57*D2*(t-tau)));
						
					}
               	}
				else
            	{
					dtau=0.99*t/NSimps;
   					for (i=0;i<NSimps;i++)
      				{
         				tau=(i+1)*dtau;
            			Mediat=SourceComp->OutSpline(tau,1); //diffusion source
         	   			S+=(dtau/3)*Simps[i]*(Mediat-Uptake)*exp(-(Z*Z+(R-Rcleft)*(R-Rcleft))/(4*D2*(t-tau)))/exp(1.5*log(12.57*D2*(t-tau)));
						//S+=(dtau/3)*Simps[i]*(Mediat-Uptake)*Rcleft*exp(-(Z*Z+(R-Rcleft)*(R-Rcleft))/(4*D2*(t-tau)))/exp(0.5*log(12.57*D2*(t-tau)));
						//S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*(1/al)*NRelease*exp(-Sigma*tau)-Uptake)*exp(-(Z*Z+(R-Rcleft)*(R-Rcleft))/(4*D2*(t-tau)))/exp(1.5*log(12.57*D2*(t-tau)));
					}
               	}
            		//*(Data+j)=1000*S*Rcleft*Rcleft/(12.57*(h+R-Rcleft)*D2*D);
				if (S>0)
					//*(Data+j)=1000*S/(12.57*(h)*D);
					*(Data+j)=S;
				else
					*(Data+j)=0;
			   //*(Data+j)=1000*S/(12.57*h*D2);
			}
			delete SourceComp;
			
		}
		else // R<=Rcleft
		{

			//Uptake=Spl*(3.14159*Rcleft*Rcleft*h)*1.0e-3;
			Uptake=Spl*1.0e-4;
			for (j=1;j<NPoint;j++)
			{
				t= *(Base+j)-X;
				S=0;
				if (t>(3/Sigma)*(NSimps+1)/NSimps)
				{
            		dtau=(3/Sigma)/NSimps;
	   				for (i=0;i<NSimps;i++)
   	   				{
      	   				tau=(i+1)*dtau;
         				S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*NRelease*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
            	   	}
					dtau=(0.99*t-(3/Sigma)*(NSimps+1)/NSimps)/(NSimps-1);
   					for (i=0;i<NSimps;i++)
      				{
         				tau=(3/Sigma)*(NSimps+1)/NSimps+i*dtau;
            			S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*NRelease*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
               		}
               	}
            	else
            	{
					dtau=0.99*t/NSimps;
   					for (i=0;i<NSimps;i++)
      				{
         				tau=(i+1)*dtau;
            			S+=(dtau/3)*Simps[i]*(tau*Sigma*Sigma*NRelease*exp(-Sigma*tau)-Uptake)*exp(-R*R/(4*D*(t-tau)))/(t-tau);
               		}
               	}
				if (S>0)
					*(Data+j)=S/(12.57*h*D);
				else
					*(Data+j)=0;
				
				
			}
		
        }
		break;
	case SPILL_1N: case SPILL_2N:
      	// NMDA
         // uptake in and out the cleft is different, Rmax is variable
         // receptor density is constant
		alp=0.0738;
        bet=0.0465;	KDes=0.0084;	K_Des=0.0018;	KD=0.001;	K_1=10;   K1=10;
		D=NewPar->Wid;
		al=0.1; dl=1.34;	D2=D/(dl*dl);
        if (NStem < 1)	NStem=1;
        NRelease=Release*NStem;
        Aeq=Release/(3.14159*Rmax*Rmax*h);
        X=0;
		
        NSimps=20;
        Crit=3;
		//MediatPar.Paste(NewPar);
        Rmax=NewPar->Max;
		Zmax=NewPar->Mix;
		R2=NewPar->Time;
		
		NRelease=(1.59e-06)*4000*NStem;
        for (i=1;i<=NSimps/2-1;i++)
			{
            Simps[i*2-1]=4;	Simps[i*2]=2;
			}
        Simps[0]=1;	Simps[NSimps-1]=1;
        for (j=0;j<NPoint;j++)
			*(Data+j)=0;
        //AfxMessageBox("Check1");
        //MediatX=new float[NMediat];
        //MediatY=new float[NMediat];
        //MediatX=(float*)malloc(NMediat*sizeof(float));
		//MediatY=(float*)malloc(NMediat*sizeof(float));
		MediatComp=new TComponent(MediatY,MediatX,NMediat);
		SourceComp=new TComponent(DiffSourceData,DiffSourceBase,DiffSize);
        *(MediatX)=0;
        S=0;
		for (j=1;j<NMediat;j++)
		{
			*(MediatX+j)=*(Base+1)+j*(*(Base+NPoint-1)-(*(Base+1)))/(NMediat-1);
			
		}
        
		if (NewPar->Mode==SPILL_2N)
			MediatPar.Mode=DIFFUS_2;
		else
			MediatPar.Mode=DIFFUS_1;
        Uptake=Spl*(3.14159*Rcleft*Rcleft*h)*1.0e-6;
		DiffSourceOn=0;
		for (j=0;j<DiffSize;j++)
			*(DiffSourceBase+j)=j*10.5/DiffSize;
		DiffSourceCalc(Rcleft, D, Uptake, NStem, DiffSourceBase, DiffSourceData, DiffSize);
		for (j=0;j<DiffSize;j++)
		{
			X=*(DiffSourceData+j);
			*(DiffSourceData+j)=X*NRelease/al;
		}
	
		//Uptake=Spl*(3.14159*Rmax*Rmax*h)*1.0e-3;
		Uptake=Spl*1.0e-3; // units: mM/msec
		R=0.01;
        MediatPar.Max=Rmax;
		MediatPar.Mix=Zmax;
	    
		/*/MediatC=MediatComp;
		//AfxMessageBox("MediatC");
		
		for (j=1;j<NPoint;j++)
		{
			t=*(Base+j);
			*(Data+j)= MediatComp->OutSpline(t);
			//*(Data+j)=NMDArec(t);
		}
		//R=Rmax;
        
		*/
		
		S=0;
		dR=(Rmax-0.01)/(NSimps-1);
        for (k=0;k<NSimps;k++)
		{
			R=0.01+k*dR;
            MediatPar.Max=R;
			MediatPar.Mix=Zmax;
	        MediatCalc(R,D,Zmax,Spl, NStem, MediatX, MediatY, NMediat);
			for (j=1;j<NPoint;j++)
			{
         		t= *(Base+j);
	            //computing activity of receptors
				S=NMDArec(t);
				if (R>Rcleft) S*=R2;
				//*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet+K1*Mediat);
                //*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet);
				*(Data+j)+=2*R*dR*S/(Rcleft*Rcleft+2*R2*Rcleft*Zmax+R2*(Rmax*Rmax-Rcleft*Rcleft));
                  
           	}
         
		}
		dR=(Zmax-0.05)/(NSimps-1);
		for (k=0;k<NSimps;k++)
		{
			R=0.05+k*dR;
            MediatPar.Max=Rcleft+0.05;
			MediatPar.Mix=R;
	       	//MediatCalc(&MediatPar);
	        MediatCalc(Rcleft+0.05,D,R,Spl, NStem, MediatX, MediatY, NMediat);
   			for (j=1;j<NPoint;j++)
			{
         		t= *(Base+j);
	            //computing activity of receptors
				S=R2*NMDArec(t);
				
				//*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet+K1*Mediat);
                //*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet);
				*(Data+j)+=2*Rcleft*dR*S/(Rcleft*Rcleft+2*R2*Rcleft*Zmax+R2*(Rmax*Rmax-Rcleft*Rcleft));
                  
           	}
         
		}
        
        NewPar->Max=Max;
        NewPar->Split=Spl;
        
		//free(MediatX);	free(MediatY);	
		delete MediatComp;
		delete SourceComp;
		DiffSourceOn=1;
		break;
	case SPILL_1A: case SPILL_2A:
      	// NMDA
         // uptake in and out the cleft is different, Rmax is variable
         // receptor density is constant
		alp=0.9;
        bet=4.24;	KDes=0.017;	K_Des=0.0007;	KD=1;	K_1=4.26;  K1=4.6; K_2=3.26; K2=28.4;
		al=0.1; dl=1.34;	D2=D/(dl*dl);
		if (NStem < 1)	NStem=1;
        NRelease=Release*NStem;
        Aeq=Release/(3.14159*Rmax*Rmax*h);
        X=0;
		D=NewPar->Wid;
        NSimps=20;
        Crit=3;
		//MediatPar.Paste(NewPar);
        Rmax=NewPar->Max;
		Zmax=NewPar->Mix;
		R2=NewPar->Time;
		
		NRelease=(1.59e-06)*4000*NStem;
        for (i=1;i<=NSimps/2-1;i++)
			{
            Simps[i*2-1]=4;	Simps[i*2]=2;
			}
        Simps[0]=1;	Simps[NSimps-1]=1;
        for (j=0;j<NPoint;j++)
			*(Data+j)=0;
        
        //MediatX=(float*)malloc(NMediat*sizeof(float));
		//MediatY=(float*)malloc(NMediat*sizeof(float));
		MediatComp=new TComponent(MediatY,MediatX,NMediat);
		SourceComp=new TComponent(DiffSourceData,DiffSourceBase,DiffSize);
        *(MediatX)=0;
        S=0;
		for (j=1;j<NMediat;j++)
		{
			*(MediatX+j)=*(Base+1)+j*(*(Base+NPoint-1)-(*(Base+1)))/(NMediat-1);
			
		}
        
		if (NewPar->Mode==SPILL_2N)
			MediatPar.Mode=DIFFUS_2;
		else
			MediatPar.Mode=DIFFUS_1;
        Uptake=Spl*(3.14159*Rcleft*Rcleft*h)*1.0e-6;
		DiffSourceOn=0;
		for (j=0;j<DiffSize;j++)
			*(DiffSourceBase+j)=j*10.5/DiffSize;
		DiffSourceCalc(Rcleft, D, Uptake, NStem, DiffSourceBase, DiffSourceData, DiffSize);
		for (j=0;j<DiffSize;j++)
		{
			X=*(DiffSourceData+j);
			*(DiffSourceData+j)=X*NRelease/al;
		}
	
		//Uptake=Spl*(3.14159*Rmax*Rmax*h)*1.0e-3;
		Uptake=Spl*1.0e-3; // units: mM/msec
		R=0.01;
        MediatPar.Max=Rmax;
		MediatPar.Mix=Zmax;
	    
		S=0;
		dR=(Rmax-0.01)/(NSimps-1);
        for (k=0;k<NSimps;k++)
		{
			R=0.01+k*dR;
            MediatPar.Max=R;
			MediatPar.Mix=Zmax;
	        MediatCalc(R,D,Zmax,Spl, NStem, MediatX, MediatY, NMediat);
			for (j=1;j<NPoint;j++)
			{
         		t= *(Base+j);
	            //computing activity of receptors
				S=AMPArec(t);
				if (R>Rcleft) S*=R2;
				//*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet+K1*Mediat);
                //*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet);
				*(Data+j)+=2*R*dR*S/(Rcleft*Rcleft+2*R2*Rcleft*Zmax+R2*(Rmax*Rmax-Rcleft*Rcleft));
                  
           	}
         
		}
		dR=(Zmax-0.05)/(NSimps-1);
		for (k=0;k<NSimps;k++)
		{
			R=0.05+k*dR;
            MediatPar.Max=Rcleft+0.05;
			MediatPar.Mix=R;
	       	//MediatCalc(&MediatPar);
	        MediatCalc(Rcleft+0.05,D,R,Spl, NStem, MediatX, MediatY, NMediat);
   			for (j=1;j<NPoint;j++)
			{
         		t= *(Base+j);
	            //computing activity of receptors
				S=R2*AMPArec(t);
				
				//*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet+K1*Mediat);
                //*(Data+j)+=2*(dR*R/(Rmax*Rmax))*S*bet/(KDes+K_Des+alp+bet);
				*(Data+j)+=2*Rcleft*dR*S/(Rcleft*Rcleft+2*R2*Rcleft*Zmax+R2*(Rmax*Rmax-Rcleft*Rcleft));
                  
           	}
         
		}
        
        NewPar->Max=Max;
        NewPar->Split=Spl;
        //free(MediatX);	free(MediatY);	
		delete MediatComp;
		delete SourceComp;
		DiffSourceOn=1;
		break;

     /*
		case SPILL_1N:
     	
		 /* NMDA  OLD version
         // uptake in and out the cleft is different, Rmax is constant
         // receptor density is variable
         alp=0.0916;	bet=0.0465;	KDes=0.0084;	K_Des=0.0068;	KD=0.001;	K_1=10;	K1=10;
         if (NStem < 1)	NStem=1;
         NRelease=Release*NStem;
         Aeq=Release/(3.14159*Rmax*Rmax*h);
         X=0;
         NSimps=20;
         Crit=3;
         Rmax=1.5;
         for (i=1;i<=NSimps/2-1;i++)
         	{
            Simps[i*2-1]=4;	Simps[i*2]=2;
            }
         Simps[0]=1;	Simps[NSimps-1]=1;
         for (j=0;j<NPoint;j++)
	         *(Data+j)=0;
         NMediat=2000;
         MediatX=new float[NMediat];
         MediatY=new float[NMediat];
         MediatComp=new TComponent(MediatY,MediatX,NMediat);
         *(MediatX)=0;
         for (j=1;j<NMediat;j++)
         	*(MediatX+j)=*(Base+1)+j*(*(Base+NPoint-1)-(*(Base+1)))/(NMediat-1);
         NewPar->Mode=DIFFUS_1;
         R=0.01;
         //R=Rmax;
         dR=(Rmax-0.01)/(NSimps-1);
         for (k=0;k<NSimps;k++)
         	{
            if (R>Rcleft)
               {
	            NewPar->Split=tm*Spl;
               //dR=(Rmax-Rcleft)/(NSimps-1);
               }
            R=0.01+k*dR;
            NewPar->Max=R;
	         MediatComp->Reset(NewPar);
   	      for (j=1;j<NPoint;j++)
            	{
         		t= *(Base+j);
	            S=0;
   	         //if (t>=(Crit/(alp+bet))*(NSimps+1)/NSimps)
      	      if (t>=(Crit/Sigma)*(NSimps+1)/NSimps)
         		   {
		            //dtau=(Crit/(alp+bet))/NSimps;
      	   	   dtau=(Crit/(Sigma))/NSimps;
         	   	for (i=0;i<NSimps;i++)
		      	   	{
      		         tau=(i+1)*dtau;
                     Mediat=MediatComp->OutSpline(t-tau,1);
            		   Beta=Mediat/KD;	Beta*=Beta;
		      	      //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(exp((-tau)*(KDes+K_Des))-exp((-tau)*(alp+bet)));
      		         //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*(1+K_Des/(KDes+K_Des))*exp((-tau)*(alp+bet)));
          			   //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet+K1*Mediat)));
                     Beta2=1-exp((tau-t)*(K_1+K1*Mediat));
                     S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2*Beta2;
		               }
      		      //dtau=(0.99*t-(Crit/(alp+bet))*(NSimps+1)/NSimps)/(NSimps-1);
            		dtau=(0.99*t-(Crit/(Sigma))*(NSimps+1)/NSimps)/(NSimps-1);
		            for (i=0;i<NSimps;i++)
      			   	{
            		   //tau=((Crit/(alp+bet))*(NSimps+1)/NSimps)+i*dtau;
		               tau=((Crit/(Sigma))*(NSimps+1)/NSimps)+i*dtau;
      		         Mediat=MediatComp->OutSpline(t-tau,1);
            		   Beta=Mediat/KD;	Beta*=Beta;
            		   //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(exp((-tau)*(KDes+K_Des))-exp((-tau)*(alp+bet)));
		               //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*(1+K_Des/(KDes+K_Des))*exp((-tau)*(alp+bet)));
      		         //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet+K1*Mediat)));
                     Beta2=1-exp((tau-t)*(K_1+K1*Mediat));
                     S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2*Beta2;
            		   }
            		}
	         	else
   	      		{
      	         dtau=0.99*t/NSimps;
         	   	for (i=0;i<NSimps;i++)
      					{
	         			tau=(i+1)*dtau;
   	            	Mediat=MediatComp->OutSpline(t-tau,1);
            		   Beta=Mediat/KD;	Beta*=Beta;
		     	         //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(exp((-tau)*(KDes+K_Des))-exp((-tau)*(alp+bet)));
      	     	      //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*(1+K_Des/(KDes+K_Des))*exp((-tau)*(alp+bet)));
            		   //S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet+K1*Mediat)));
                     Beta2=1-exp((tau-t)*(K_1+K1*Mediat));
                     S+=(dtau/3)*Simps[i]*(Beta/(1+Beta))*(-(KDes+K_Des)*exp((-tau)*(KDes+K_Des))+(alp+bet)*exp((-tau)*(alp+bet)))*Beta2*Beta2;
               		}
               	}
            if (S>0)
               	{
                  if (R>Rcleft)
		            	//*(Data+j)+=2*Max*(dR*R/(Rcleft*Rcleft+Max*(Rmax*Rmax-Rcleft*Rcleft)))*S*bet/(KDes+K_Des+alp+bet+K1*Mediat);
                     *(Data+j)+=2*Max*(dR*R/(Rcleft*Rcleft+Max*(Rmax*Rmax-Rcleft*Rcleft)))*S*bet/(KDes+K_Des+alp+bet);
                  else
                  	//*(Data+j)+=2*(dR*R/(Rcleft*Rcleft+Max*(Rmax*Rmax-Rcleft*Rcleft)))*S*bet/(KDes+K_Des+alp+bet+K1*Mediat);
                     *(Data+j)+=2*(dR*R/(Rcleft*Rcleft+Max*(Rmax*Rmax-Rcleft*Rcleft)))*S*bet/(KDes+K_Des+alp+bet);
            		}
           	}
         }
         NewPar->Mode=SPILL_1N;
         NewPar->Max=Max;
         NewPar->Split=Spl;
         delete MediatX;	delete MediatY;	delete MediatComp;
         break;
		*/
		}
		ReSpline();
}
/***************************************************/
/******* ! Changes NewPar->Zero,Amp !  *************/
void TComponent::Randomize(TParam* NewPar)
{
	int j,n,n1,n2,k,v;
	float Y;
	float Wid=NewPar->Wid;		float Max=NewPar->Max;
	float Spl=NewPar->Split;	float mix=NewPar->Mix;
	float tm=NewPar->Time;
	int* Pos;
	n=NewPar->Mult;
   //randomize();
   switch (NewPar->Mode)
   	{
      case UNIMODAL:
      	for (j=0;j<NPoint;j++)
         	*(Data+j)=RandomNorm(Max,Wid);

      break;
      case BIMODAL:
		//randomize();
      	Y=0;
		/*
		Pos=(int*)malloc((NPoint+1)*sizeof(int));
		k=NPoint*mix;
		for(j=0;j<NPoint;j++) *(Pos+j)=1;
		for(j=0;j<k;j++)
		{
			do
			{
				n=NPoint*Rand();
			}
			while (*(Pos+n)); 
			*(Pos+n)=0;
			
		}
		*/
		for (j=0;j<NPoint;j++)
		{
		
			v=RandomBinom(1,mix);
			if (!v) 
			
			//if (*(Pos+j))
			// main Gussian
				*(Data+j)=RandomNorm(Max,Wid);
			else // additional Gaussian
				*(Data+j)=RandomNorm(Max-Spl,Wid*tm);
			
		}
		break;
	  case BINOM_QU:
      	//Spl=Wid;
      	for (j=0;j<NPoint;j++)
         	*(Data+j)=Wid*RandomBinom(n,Max)+RandomNorm(0,Wid);
      break;
      case BINOM:
      	for (j=0;j<NPoint;j++)
         	{
            v=RandomBinom(n,Max);
            Y=Spl*v+RandomNorm(0,Wid*v*tm);
            if (Y<0)Y=0;
         	*(Data+j)=Y+RandomNorm(0,Wid);
            }
      break;
      case BINOM_SUB:
      	if (n<10)
            for (j=0;j<NPoint;j++)
            	{
               v=RandomBinom(n,Max);
               Y=v+RandomNorm(0,sqrt(v)*tm);
               if (Y<0)Y=0;
         		*(Data+j)=Spl*Y*Y*mix*mix/(mix*mix+Y*Y)+RandomNorm(0,Wid);
            	}
         else
         {
         n2=fmod(n,10);
         if (n2<1)n2=10;
         n1=n/10;
      	for (j=0;j<NPoint;j++)
         	{
            *(Data+j)=0;
            for (k=0;k<n2;k++)
				{
				v=RandomBinom(n1,Max);
				Y=v+RandomNorm(0,sqrt(v)*tm);
				if (Y<0)Y=0;
				*(Data+j)+=Spl*Y*Y*mix*mix/(mix*mix+Y*Y);
               //Y=Spl*v+RandomNorm(0,Wid*(1+v*tm));
            	//*(Data+j)+=Y/(1+Y/mix);
               //*(Data+j)+=Y;
               }
			*(Data+j)+=RandomNorm(0,Wid);
            }
         }
      break;
      }
   NewPar->Zero=MinVal();
   NewPar->Amp=MaxVal()-NewPar->Zero;
   for (j=0;j<NPoint;j++)
      {
   	*(Data+j)-=NewPar->Zero;
		*(Data+j)/=NewPar->Amp;
      }
   ReSpline();

}
/***************************************************/
float TComponent::LikeHoodTest(TParam* NewPar)
{
	char szNum[100];
	float L;
	const int MaxNSite=200;
	float	Prb_K[MaxNSite+1];
	float	SitePrb[MaxNSite+1];
	float	Prb[50][MaxNSite+1];
	float	X_K[MaxNSite+1];
	short int SiteIndex[MaxNSite];
	short int SiteSel[MaxNSite+1];
	short int PrbFlag[50][MaxNSite];
	float	Beta,Beta2,X,dX,p;
	float	Wid,Max,Spl,Y,S;
	float	tr,td,t0,tm,Fmax;
	float	mix=NewPar->Mix;
	int j,n,k,i,crIndex,begN,endN,Num;
	Wid=NewPar->Wid;
	Max=NewPar->Max;
	Spl=NewPar->Split;
	tm=NewPar->Time;
	if ( NewPar->Wid > NewPar->WidH ) Wid=NewPar->WidH;
		else
		if ( NewPar->Wid < NewPar->WidL ) Wid=NewPar->WidL;
			else
			Wid=NewPar->Wid;
	if ( NewPar->Mix > NewPar->MixH ) mix=NewPar->MixH;
		else
		if ( NewPar->Mix < NewPar->MixL ) mix=NewPar->MixL;
			else
			mix=NewPar->Mix;
	if ( NewPar->Time > NewPar->TimeH ) tm=NewPar->TimeH;
		else
		if ( NewPar->Time < NewPar->TimeL ) tm=NewPar->TimeL;
			else
			tm=NewPar->Time;
	if ( NewPar->Max > NewPar->MaxH ) Max=NewPar->MaxH;
			else
			if ( NewPar->Max < NewPar->MaxL ) Max=NewPar->MaxL;
				else
				Max=NewPar->Max;
    if ( NewPar->Split > NewPar->SplitH ) Spl=NewPar->SplitH;
		else
		if ( NewPar->Split < NewPar->SplitL ) Spl=NewPar->SplitL;
			else
			Spl=NewPar->Split;
	L=0;
	switch (NewPar->Mode)
	{
		case UNIMODAL:
			for (j=0;j<NPoint;j++)
			{
				X=(*(Data+j));
				Beta=(X-Max)/Wid;
				Beta2=Beta*Beta/2;
				Y=0.3989*exp(-Beta2)/Wid;
				if (Y<MinFloat) Y=MinFloat;
				L-=2*log(Y);
			//L+=X/NPoint;
			//L+=fabs(Beta)/NPoint;
			}
		 //if (L<MinFloat) L=MinFloat;
			break;
		case BIMODAL: // double gaussian
			// first peak: max,wid; 2-nd : -shift=split, rel.fraction = mix, rel.width = tm;
			// 1st peak
			for (j=0;j<NPoint;j++)
			{
				X= *(Data+j);
				Beta=(X-Max)/Wid;
				Beta2=Beta*Beta/2;
				Y=0.3989*(1-mix)*exp(-Beta2)/Wid;
				Beta=(X-Max+Spl)/(tm*Wid);
				Beta2=Beta*Beta/2;
				Y+=0.3989*mix*exp(-Beta2)/(tm*Wid);
				if (Y<MinFloat) Y=MinFloat;
					L-=2*log(Y);
			}
			break;
		case COMPOUND:
			
      		n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
			 p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
			 if (Wid>0.8*Spl) Wid=0.8*Spl;
			 SitePrb[1]=p*(1-mix);
			 if (SitePrb[1]<MinFloat)SitePrb[1]=MinFloat;
			 SitePrb[n]=p*(1+mix);
			 if(SitePrb[n]>0.99999)SitePrb[n]=0.99999;
			 p=0.5*(SitePrb[n]+SitePrb[1]);
			 mix=0.5*(SitePrb[n]-SitePrb[1])/p;
			 NewPar->Max=p;
			 NewPar->Mix=mix;
			 for (i=2;i<n;i++)
         		SitePrb[i]=SitePrb[1]+i*(SitePrb[n]-SitePrb[1])/n;
			 *Prb_K=1;
			 for (i=1;i<=n;i++)
         		*(Prb_K)*=(1-SitePrb[i]);
			 //computing compound probabilities
			 *(Prb_K+n)=1;
			 for (i=1;i<=n;i++)
         		*(Prb_K+n)*=SitePrb[i];
			 for(k=1;k<n;k++)
         		{
				S=1;
				for (i=1;i<=k;i++)
            		S*=SitePrb[i];
				for (i=k+1;i<=n;i++)
            		S*=(1-SitePrb[i]);
				*(Prb_K+k)=S;
				for (i=1;i<=k;i++)
            		SiteIndex[i]=i;
				crIndex=k;
				SiteIndex[0]=0;
				do
            		{
				   if (SiteIndex[crIndex]<n)
              			SiteIndex[crIndex]++;
              		else
               		{
					  while ((SiteIndex[crIndex]==n-k+crIndex)&&(crIndex>0))
                  		crIndex--;
					  if (crIndex)
						 {
						 SiteIndex[crIndex]++;
						 for (i=crIndex+1;i<=k;i++)
                  			SiteIndex[i]=SiteIndex[i-1]+1;
						 crIndex=k;
						 }
					  }
				   for (i=1;i<=n;i++)
               		SiteSel[i]=0;
				   for (i=1;i<=k;i++)
               		SiteSel[SiteIndex[i]]=1;
				   S=1;
				   for (i=1;i<=n;i++)
               		if (SiteSel[i])
                  		S*=SitePrb[i];
					  else
                  		S*=(1-SitePrb[i]);
				   *(Prb_K+k)+=S;
              		}
				while (crIndex>=1);
				}
			 *X_K=0;
			 for (k=1;k<=n;k++)
				{
         		*(X_K+k)=k*Spl;
				//*(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
				}

			 S=0;
			 for (j=0;j<NPoint;j++)
         		{
				X= *(Data+j);
				Y=0;
				for (k=0;k<=n;k++)
					  {
					  Beta=(X-(*(X_K+k)))/Wid;
					  Beta2=Beta*Beta/2;
               		Y+=0.3989*(*(Prb_K+k))*exp(-Beta2)/Wid;
					  }
				if (Y<MinFloat) Y=MinFloat;
				L-=2*log(Y);
				}
			if (L<MinFloat) L=MinFloat;
			
			break;
	  case BINOM:case BINOM_QU:
      	n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
		p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
		if (NewPar->Mode==BINOM_QU)
         	Spl=Wid;
		 //if (Wid>0.8*Spl) Wid=0.8*Spl;
		*Prb_K=1;
		for (i=1;i<=n;i++)
         	*(Prb_K)*=(1-p);
		 //computing compound probabilities
		 *(Prb_K+n)=1;
		 *X_K=0;
		 for (k=1;k<=n;k++)
			{
         	*(X_K+k)=k*Spl;
			*(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
			}
		 for (j=0;j<NPoint;j++)
         	{
			X= *(Data+j);
			Y=0;
			for (k=0;k<=n;k++)
            	{
			   Beta=(X-(*(X_K+k)))/(Wid*(1+k*mix));
			   Beta2=Beta*Beta/2;
			   Y+=0.3989*(*(Prb_K+k))*exp(-Beta2)/(Wid*(1+k*mix));
			   }
			if (Y<MinFloat) Y=MinFloat;
			L-=2*log(Y);
			}
		 //if (L<MinFloat)L=MinFloat;

		 //NewPar->Zero=0;
		 //NewPar->Amp=S;
			break;
		case BINOM_NZ:
      		n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
			p=Max;	if (p<0.005) p=0.005;	if (p>0.99999)p=0.99999;
			*Prb_K=1;
			for (i=1;i<=n;i++)
         		*(Prb_K)*=(1-p);
			if (*Prb_K>0.99999) *Prb_K=0.99999;
			//computing compound probabilities
			*(Prb_K+n)=1;
			*X_K=0;
			for (k=1;k<=n;k++)
			{
         		*(X_K+k)=k*Spl;
				*(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
				*(Prb_K+k)/= (1-(*(Prb_K)));
			}
			for (j=0;j<NPoint;j++)
         	{
				X= *(Data+j);
				Y=0;
				for (k=1;k<=n;k++)
            		{
				   Beta=(X-(*(X_K+k)))/(Wid*(1+k*mix));
				   Beta2=Beta*Beta/2;
				   Y+=0.3989*(*(Prb_K+k))*exp(-Beta2)/(Wid*(1+k*mix));
				   }
				if (Y<MinFloat) Y=MinFloat;
				L-=2*log(Y);
			}
			 //if (L<MinFloat)L=MinFloat;

			 //NewPar->Zero=0;
			 //NewPar->Amp=S;
			break;
	case BINOM_EF:
		n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
		p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
		if (Wid>0.8*Spl) Wid=0.8*Spl;
		*Prb_K=1;
		for (i=1;i<=n;i++)
			*(Prb_K)*=(1-p);
		//computing compound probabilities
		*(Prb_K+n)=1;
		*X_K=0;
		for (k=1;k<=n;k++)
		{
			*(X_K+k)=k*Spl;
			*(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
		}
		for (k=1;k<=n;k++)
			(*(Prb_K+k))*=(1-(1+mix)*(*Prb_K))/(1-(*Prb_K));
		(*Prb_K)*=(1+mix);
		for (j=0;j<NPoint;j++)
        {
			X= *(Data+j);
			Y=0;
			for (k=0;k<=n;k++)
           	{
			   Beta=(X-(*(X_K+k)))/(Wid*(1+k*tm));
			   Beta2=Beta*Beta/2;
			   Y+=0.3989*(*(Prb_K+k))*exp(-Beta2)/(Wid*(1+k*tm));
			}
			if (Y<MinFloat) Y=MinFloat;
			L-=2*log(Y);
		}
		break;
	  case BINOM_SUB:
      	n=NewPar->Mult;	if (n<1) n=1;	if (n>MaxNSite)n=MaxNSite;
		p=Max;	if (p<MinFloat) p=MinFloat;	if (p>0.99999)p=0.99999;
		if (mix<MinFloat)mix=MinFloat;
		if (Wid>0.8*Spl) Wid=0.8*Spl;
		*Prb_K=1;
		for (i=1;i<=n;i++)
			*(Prb_K)*=(1-p);
		//computing compound probabilities
		*(Prb_K+n)=1;
		*X_K=0;
		for (k=1;k<=n;k++)
		{
        	*(X_K+k)=k*Spl/(1+k*Spl/mix);
			*(Prb_K+k)=(*(Prb_K+k-1))*(n-k+1)*p/(k*(1-p));
		}
		S=0;
		for (j=0;j<NPoint;j++)
        {
			X= *(Data+j);
			Y=0;
			for (k=0;k<=n;k++)
			{
				Beta=(X-(*(X_K+k)))/(Wid*(1+k*tm));
				Beta2=Beta*Beta/2;
				Y+=0.3989*(*(Prb_K+k))*exp(-Beta2)/(Wid*(1+k*tm));
			}
			if (Y<MinFloat) Y=MinFloat;
           	L-=2*log(Y);
		}
		//if (L<MinFloat)L=MinFloat;
		break;
	case GAUSLOR:
		td=Spl;
		tr=mix;
		for(i=0;i<mNum;i++)
			for(j=0;j<MaxNSite;j++)
			{
				Prb[i][j]=0.020408;
				PrbFlag[i][j]=0;
				
			}
		endN=NPoint/eNum;
		if(endN>MaxNSite)
			begN=endN-MaxNSite;
		else
			begN=0;
		//sprintf(szNum, "%i %i", begN, endN);
		//AfxMessageBox(szNum);
		L=0;
		for(j=begN;j<endN;j++)
		{
			S=0;
			for(i=0;i<mNum;i++) S+=Prb[i][j-begN];
			for(i=0;i<mNum;i++) Prb_K[i]=Prb[i][j-begN]/S;
			for(k=0;k<eNum;k++)
			{
				Num=(int)(*(Data+j*eNum+k));
				Y=Prb_K[Num-1];
				if (Y<MinFloat) Y=MinFloat;
           		L-=2*log(Y);
				//Prb[Num-1][j-begN+1]+=0.020408*Max;
				
				if (!PrbFlag[Num-1][j-begN])
				{
					
					PrbFlag[Num-1][j-begN+1]=1;
					for(i=1;(i<=endN-j)&&(i<=2*td);i++)
					{
						Beta=(i-0.99999)/td;
						Prb[Num-1][j-begN+i]+=0.020408*Max*exp(-Beta)/td;
					}
				}
				for(i=1;(i<=endN-j)&&(i<=tr);i++)
				{
					//Beta=(i-0.00001)/tr;
					//Prb[Num-1][j-begN+i]-=0.020408*Wid*exp(-Beta*Beta)/tr;
					Prb[Num-1][j-begN+i]-=0.020408*Wid/tr;
				}
				
				
			}
 
		}
		S=0;
		for(j=0;j<mNum;j++) S+=Prb[j][endN];
		for(j=0;j<mNum;j++) Prb_K[j]=Prb[j][endN]/S;
		for(j=0;j<mNum;j++)
		{
			*(Rating+j)=Prb_K[j];
			*(Rating+1000+j)=Prb_K[j];
		}
		break;
	default: L=-1;
	}
   return L;
}
/**********************************************************************/
void TComponent::ShowLabel(RECT* OutRect,HDC SpDC, TMark* Lab)
{
	int    j;
	float  ScaleX,ScaleY,minX,minY,maxY;
	int    Left=OutRect->left;
	int    Top=OutRect->bottom;
	minY=0;//MinVal();
	maxY=1;MaxVal();
	minX= *Base;
	ScaleX=(OutRect->right-OutRect->left)/(*(Base+NPoint-1)-minX);
	ScaleY=(OutRect->bottom-OutRect->top);
	//j=(int)(NPoint*(Lab->X-minX)/(*(Base+NPoint-1)-minX));
   j=(int)(NPoint*(Lab->X));
	if (j>NPoint-2)j=NPoint-2;
   if (j<1)j=1;
   if (Lab->Visible)
      TextOut(SpDC,(int)(OutRect->left+ScaleX*(*(Base+j)-minX)),(int)(OutRect->bottom-ScaleY*(*(Data+j)+Lab->Y)),
				  (LPSTR)Lab->Text,strlen(Lab->Text));
}
/**********************************************************************/
void TComponent::Dot(RECT* OutRect,HDC SpDC, PTMark ViewLabel, float* DataY)
{
	char crStr[64];
	int    j,SizeX,SizeY,jInc;
	float  ScaleX,ScaleY,minX,minY,maxY,dX;
   POINT	 TriPoint[4];
   HBRUSH crBrush,OldBrush;
	int    Left=OutRect->left;
	int    Top=OutRect->bottom;
	minY=0;//MinVal();
	maxY=1;MaxVal();
	minX= *Base;
	ScaleX=(OutRect->right-OutRect->left)/(*(Base+NPoint-1)-minX);
	ScaleY=(OutRect->bottom-OutRect->top)/(maxY-minY);
	SizeX=(int)(0.5*(OutRect->right-OutRect->left)*(ViewLabel->SizeX));
	SizeY=(int)(0.5*(OutRect->bottom-OutRect->top)*(ViewLabel->SizeY));
   jInc=(int)(ViewLabel->TextX);
   if (jInc<1)jInc=1;
	if ( SizeX<=0 )
		SizeX=2;
	if ( SizeY<=0 )
		SizeY=2;
   switch(ViewLabel->Style)
   	{
      case BAR_O:
      	for (j=0;j<NPoint;j+=jInc)
				Rectangle(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),
					      (int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),
							(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),
							(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));
      	break;
      case BAR_F:
      	crBrush=CreateSolidBrush(RGB(0,0,0));
         OldBrush=(HBRUSH)SelectObject(SpDC,crBrush);
         for (j=0;j<NPoint;j+=jInc)
				Rectangle(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),
					      (int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),
							(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),
							(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));

         SelectObject(SpDC,OldBrush);
         DeleteObject(crBrush);
      	break;
      case HIST_F:
      	crBrush=CreateSolidBrush(RGB(0,0,0));
         OldBrush=(HBRUSH)SelectObject(SpDC,crBrush);
         dX=0.5*ScaleX*(*(Base+1)-(*(Base)));
         for (j=0;j<NPoint;j+=jInc)
         	{
				Rectangle(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-dX),
					      (int)(Top-ScaleY*(*(Data+j)-minY)),
							(int)(Left+ScaleX*(*(Base+j)-minX)+dX),
							(int)(Top));
            }
         SelectObject(SpDC,OldBrush);
         DeleteObject(crBrush);
      	break;
      case HIST_H:
      	crBrush=CreateHatchBrush(HS_FDIAGONAL,RGB(0,0,0));
         OldBrush=(HBRUSH)SelectObject(SpDC,crBrush);
         dX=0.5*ScaleX*(*(Base+1)-(*(Base)));
         for (j=0;j<NPoint;j+=jInc)
         	{
				Rectangle(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-dX),
					      (int)(Top-ScaleY*(*(Data+j)-minY)),
							(int)(Left+ScaleX*(*(Base+j)-minX)+dX),
							(int)(Top));
            }
         SelectObject(SpDC,OldBrush);
         DeleteObject(crBrush);
      	break;
      case HIST_O:
         dX=0.5*ScaleX*(*(Base+1)-(*(Base)));
         for (j=0;j<NPoint;j+=jInc)
         	{
            TriPoint[0].x=(int)(Left+ScaleX*(*(Base+j)-minX)-dX);
            TriPoint[0].y=(int)(Top);
            TriPoint[1].x=(int)(Left+ScaleX*(*(Base+j)-minX)-dX);
            TriPoint[1].y=(int)(Top-ScaleY*(*(Data+j)-minY));
            TriPoint[2].x=(int)(Left+ScaleX*(*(Base+j)-minX)+dX);
            TriPoint[2].y=(int)(Top-ScaleY*(*(Data+j)-minY));
            TriPoint[3].x=(int)(Left+ScaleX*(*(Base+j)-minX)+dX);
            TriPoint[3].y=(int)(Top);
            Polyline(SpDC,TriPoint,4);
				}
      	break;
      case TRI_O:
         for (j=0;j<NPoint;j+=jInc)
         	{
            TriPoint[0].x=(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX);
            TriPoint[0].y=(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY);
            TriPoint[1].x=(int)(Left+ScaleX*(*(Base+j)-minX));
            TriPoint[1].y=(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY);
            TriPoint[2].x=(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX);
            TriPoint[2].y=(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY);
            Polygon(SpDC,TriPoint,3);
            }
      	break;
      case TRI_F:
      	crBrush=CreateSolidBrush(RGB(0,0,0));
         OldBrush=(HBRUSH)SelectObject(SpDC,crBrush);
			for (j=0;j<NPoint;j+=jInc)
         	{
            TriPoint[0].x=(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX);
            TriPoint[0].y=(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY);
            TriPoint[1].x=(int)(Left+ScaleX*(*(Base+j)-minX));
            TriPoint[1].y=(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY);
            TriPoint[2].x=(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX);
            TriPoint[2].y=(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY);
            Polygon(SpDC,TriPoint,3);
            }
         SelectObject(SpDC,OldBrush);
         DeleteObject(crBrush);
      	break;
      case CIRCLE_O:
      	for (j=0;j<NPoint;j+=jInc)
		   	Ellipse(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),
					      (int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),
			            (int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),
							(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));

      	break;
      case CIRCLE_F:
      	crBrush=CreateSolidBrush(RGB(0,0,0));
         OldBrush=(HBRUSH)SelectObject(SpDC,crBrush);
         for (j=0;j<NPoint;j+=jInc)
	         Ellipse(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),
					      (int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),
			            (int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),
							(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));
         SelectObject(SpDC,OldBrush);
         DeleteObject(crBrush);
      	break;
      case CROSS_R:
      	for (j=0;j<NPoint;j+=jInc)
         	{
				MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)),NULL);
				LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)));
				MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)),(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY),NULL);
				LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)),(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY));
				}
      	break;
      case CROSS_D:
      	for (j=0;j<NPoint;j+=jInc)
         	{
				MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),NULL);
				LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));
				MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),NULL);
				LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));
				}
      	break;
      case CROSS_T:
         for (j=0;j<NPoint;j+=jInc)
         	{
      		MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),NULL);
				LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY));
				MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)-SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY),NULL);
				LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)+SizeX),(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));
				MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)),(int)(Top-ScaleY*(*(Data+j)-minY)-SizeY),NULL);
				LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j)-minX)),(int)(Top-ScaleY*(*(Data+j)-minY)+SizeY));
            }
      	break;
      }
}
/**********************************************************************/
void TComponent::Draw(RECT* OutRect,HDC SpDC)
{
	int    j,i,nStep;

	int    totGraphPoint=300;
	int    nTot=0;
	POINT  Graph[600];
	POINT* crGraph;
	float  ScaleX,ScaleY,minX,maxY,minY,crX,dX;
	float  X01,X02,X21,f;
	int    Left=OutRect->left;
	int    Top=OutRect->bottom;
	char   crStr[64];
	minY=0;//MinVal();
	maxY=1;//MaxVal();
	minX= *Base;
	ScaleX=(OutRect->right-OutRect->left)/((*(Base+NPoint-1))-minX);
	ScaleY=(OutRect->bottom-OutRect->top)/(maxY-minY);
	if ( IsSplined )
   	{
   	nStep=(int)(totGraphPoint/NPoint-1);
   	if(nStep < 0) nStep=0;
   	crGraph=Graph;
   	for (j=1;j < NPoint;j++)
        {
		crGraph->x=(int)(Left+ScaleX*(*(Base+j-1)-minX));
		crGraph->y=(int)(Top-ScaleY*(*(Data+j-1)-minY));
		crGraph++;
           nTot++;
		crX= *(Base+j-1);
		dX= (*(Base+j)-(*(Base+j-1)))/(nStep+1);
		for (i=1;i <=nStep;i++)
           {
	       crX+=dX;
	       X01=crX-(*(Base+j-1));
	       X02=crX-(*(Base+j));
	       X21=X01-X02;
           f=(*(Splines+j))*X01*X01*X01/(6*X21)-(*(Splines+j-1))*X02*X02*X02/(6*X21);
	       f+=((*(Data+j))/X21-(X21*(*(Splines+j)))/6)*X01-((*(Data+j-1))/X21-(X21*(*(Splines+j-1)))/6)*X02;
		   f/=2.0;
		   f+=0.5*((*(Data+j))+X02*( (*(Data+j))- (*(Data+j-1)))/X21);
	       crGraph->x=(int)(Left+ScaleX*(crX-minX));
	       crGraph->y=(int)(Top-ScaleY*(f-minY));
	       crGraph++;
	       nTot++;
	       }
		}
		MoveToEx(SpDC,Graph[0].x,Graph[0].y,NULL);
		for (j=0;j<=nTot-2;j++)
		{
	   		LineTo(SpDC,Graph[j+1].x,Graph[j+1].y);
		}
	}
	else
	{
	MoveToEx(SpDC,(int)(Left+ScaleX*(*(Base)-minX)),(int)(Top-ScaleY*(*(Data)-minY)),NULL);
	for (j=0;j<NPoint-1;j++)
		{
		 LineTo(SpDC,(int)(Left+ScaleX*(*(Base+j+1)-minX)),(int)(Top-ScaleY*(*(Data+j+1)-minY)));
		}
    }
} //  drawing
/***********************************************************************/
void TComponent::PhrFilterHigh(TPhourier* Filter)
{
/*
int N,k,j,n;
float L,X0,X,Freq,Filt,AcF,BsF;
complex*	Cf=new complex[NPoint];
complex SumA, SumB;
N=NPoint;
X0=*(Base);
L= (*(Base+N-1)-X0)*N/((float)(N-1));
if (Filter->CutFreq<MinFloat)Filter->CutFreq=MinFloat;
for (j=0;j<N;j++)
	{
   (Cf+j)->re=0;	(Cf+j)->im=0;
   for (k=0;k<N;k++)
   	{
      X=6.2832*k*j/N;
      (Cf+j)->re+=(*(Data+k))*cos(X)/N;
		(Cf+j)->im-=(*(Data+k))*sin(X)/N;
      }
   }
for (j=0;j<N;j++)
	{
	Freq=6.2832*j/L;
   //for (k=0;k<Filter->Factor;k++)
   Filt=(Freq/Filter->CutFreq);
   SumA.re=1/(1+Filt*Filt);	SumA.im=-Filt/(1+Filt*Filt);
   (*(Cf+j))*=SumA;
   }
for (k=0;k<N;k++)
	{
   SumB.re=0; SumB.im=0;
   X=(*(Base+k))-X0;
   X*=(6.2832/L);
   for(j=0;j<N;j++)
   	{
      SumA=polar(1,j*X);
      SumB+=(*(Cf+j))*SumA;
      }
   *(Data+k)=real(SumB);

   }

/*
int N,k,j,n;
float L,X0,X,Freq,Filt,SumA,SumB,AcF,BsF;
float* Ac=new float[NPoint];
float* Bs=new float[NPoint];
N=NPoint;
n=floor(0.5*(N+1));
X0=*(Base);
L= (*(Base+N-1)-X0)*N/((float)(N-1));
if (Filter->CutFreq<MinFloat)Filter->CutFreq=MinFloat;
Freq=Filter->CutFreq;
//L= (*(Base+N-1)-X0);
for (j=0;j<=n;j++)
	{
   *(Ac+j)=0;	*(Bs+j)=0;
   for (k=0;k<N;k++)
   	{
      X=6.2832*k*j/N;
      Filt=k*L/N;
      *(Ac+j)+=2*(*(Data+k))*cos(X)/N;	*(Bs+j)+=2*(*(Data+k))*sin(X)/N;
		//*(AcF+j)+=2*(exp(-Freq*Filt))*cos(X)/N;	*(BsF+j)+=2*(exp(-Freq*Filt))*sin(X)/N;
      }
   }
*Bs=0; *(Bs+n)=0;
//filtering
for (j=0;j<=n;j++)
	{
	Freq=6.2832*j/L;
   //for (k=0;k<Filter->Factor;k++)
   Filt=(Freq/Filter->CutFreq);
   AcF=1/(1+Filt*Filt);	BsF=-Filt/(1+Filt*Filt);
   //SumA=(*(Ac+j))*AcF-(*(Bs+j))*BsF;
   //SumB=(*(Ac+j))*BsF+(*(Bs+j))*AcF;
   SumA=(*(Ac+j))*AcF;
   SumB=(*(Bs+j))*AcF;
   *(Ac+j)=SumA;	*(Bs+j)=SumB;
   //*(Ac+j)/=(1+Filt*Filt);	*(Bs+j)*=(Filt/(1+Filt*Filt));
   }

for (k=0;k<N;k++)
	{
   *(Data+k)=0.5*(*Ac);
   X=(*(Base+k))-X0;
   X*=(6.2832/L);
   for(j=0;j<n;j++)
   	{
      *(Data+k)+=(*(Ac+j))*cos(j*X)+(*(Bs+j))*sin(j*X);
      }
   if (fmod(N,2)<1)
	   *(Data+k)+=0.5*(*(Ac+n))*cos(n*X);
   }

delete Ac;	delete Bs;

//ReSpline();
delete Cf;
*/
}
/**********************************************************************/
int TComponent::ReSpline(void)
{
	int NS=N_Spline+1;
	float* pS;
	float* pB;
	float* pA;
	float Denom, X01,X02,Y02,Y01;
	int j,i;
	int N=NPoint-1;
	if (NPoint<N_Spline) IsSplined=1;
   	else IsSplined=0;
	if ( IsSplined )
	{
		pS=(float*)malloc(8*NS*sizeof(float));
		pB=pS+NS;
		pA=pB+NS;
		if ( (pS!=NULL))
		{
			// initialization of coefficients
			X01=(*(Base+1))-(*Base);
			X02=(*(Base+2))-(*Base);
			*(pA+NS)=((*(Base+1))-(*Base))/3;
			*(pA+2*NS)=((*(Base+1))-(*Base))/6;
			*(pA+N)=((*(Base+N))-(*(Base+N-1)))/6;
			*(pA+NS+N)=((*(Base+N))-(*(Base+N-1)))/3;
			*(pA+2*NS+N)=0;
			*pB=0;
			*(pB+N)=0;
			for (j=1;j < N;j++)
    		{
				*(pA+j)=((*(Base+j))-(*(Base+j-1)))/6;
				*(pA+NS+j)=((*(Base+j+1))-(*(Base+j-1)))/3;
				*(pA+2*NS+j)=((*(Base+j+1))-(*(Base+j)))/6;
				X01= *(Base+j) - *(Base+j-1);
				Y01= *(Data+j) - *(Data+j-1);
				X02= *(Base+j+1) - *(Base+j);
				Y02= *(Data+j+1) - *(Data+j);
    			*(pB+j)=(Y02/X02)-(Y01/X01);
			}
			// coefficient matrix transformation
			for (j=0;j < N;j++)
    		{
    			Denom=(*(pA+j+1))/(*(pA+NS+j));
    			for (i=0;i<2;i++)
        			*(pA+i*NS+j+1)-=Denom*(*(pA+(i+1)*NS+j));
    			*(pB+j+1)-=Denom*(*(pB+j));
    		}
			// computation of spline-coefficients
			*(pS+N)=(*(pB+N))/(*(pA+NS+N));
			for (j=N-1;j >=0;j--)
	    		*(pS+j)=((*(pB+j))-(*(pS+j+1))*(*(pA+2*NS+j)))/(*(pA+NS+j));
			//changing the data component
			for (j=0;j<=N;j++)
				 *(Splines+j) = *(pS+j);
			free(pS);
		}
	
	return N;
	}
	else
		return 0;
}
/*********************************************************************/
int TComponent::FindPoint(float X_Point)
{
	int j=0;
	while ( (j < NPoint-1)&&(X_Point > (*(Base+j))) )
		j++;
	return j;
} 
/*********************************************************************/
float TComponent::Area(int BegPoint, int EndPoint)
{
	int jBeg = BegPoint;
	int jEnd = EndPoint;
	if (jBeg<1) jBeg=1;
	if (jEnd<1) jEnd=NPoint-1;
	if (jEnd>NPoint-1) jEnd=NPoint-1;
	float S=0;
	for (int j=jBeg;j<jEnd;j++)
		S+= fabs(0.5*(*(Data+j)+ *(Data+j-1))*(*(Base+j)- *(Base+j-1)));
	return S;
}
/*********************************************************************/
float TComponent::Area(float xBeg, float xEnd)
{
	int jBeg = FindPoint(xBeg);
	int jEnd = FindPoint(xEnd);
	float S=Area(jBeg,jEnd);
	return S;
}
/*********************************************************************/
float TComponent::SMod()
{
	float S=0;
	for (int j=0;j<NPoint;j++)
		S+= fabs( *(Data+j) );
	return S/NPoint;
}
/*********************************************************************/
float TComponent::SSqu()
{
	float Y;
	float S=0;
	for (int j=0;j<NPoint;j++)
		{
		Y= *(Data+j);
		S+=Y*Y;
		}
	return sqrt(S/NPoint);
}

/********************************************************************/
float Mult(TComponent* Comp_A,TComponent* Comp_B)
{
	float S=0;
	int N=Comp_A->NPoint;
	for (int j=0;j<N;j++)
		S+= (*(Comp_A->Data+j))*(*(Comp_B->Data+j));
	return S;
}
/********************************************************************/
float Norm(TComponent* Comp_A,TComponent* Comp_B, int Mode)
{
	float S=0;	float S_Diff=0;   	float S_Aver=0;
   float Y_A,Y_B;
   int j;
	int N=Comp_A->NPoint;
	switch (Mode)
	{
      // mean square root //
    	case TEST_MSR:
    		for (j=0;j<N;j++)
				{
				Y_A= *(Comp_A->Data+j);
				Y_B= *(Comp_B->Data+j);
				S_Aver+=fabs(Y_A)+fabs(Y_B);
		      	Y_A-=Y_B;
				S_Diff+=Y_A*Y_A;
				}
         S_Aver+=MinFloat;
         S_Diff+=MinFloat;
			S=2*sqrt(S_Diff*N)/S_Aver;
			break;
      // log-likehood //
      case TEST_LLH:
        	for (j=0;j<N;j++)
         	{
            Y_A= *(Comp_A->Data+j);
				Y_B= *(Comp_B->Data+j);
            //Y_B*=0.99;
            //Y_B+=MinFloat+Y_A*0.01;                             `
            if (Y_B<10*MinFloat)Y_B=10*MinFloat;
            S+=Y_A*(-2)*log(Y_B);
            }
         break;
      //chi-square test //
      case TEST_CHI:
      	for (j=0;j<N;j++)
         	{
            Y_A= *(Comp_A->Data+j);
				Y_B= *(Comp_B->Data+j);
            //Y_B*=0.99;
            if (Y_B<10*MinFloat)Y_B=10*MinFloat;
            S+=N*(Y_A-Y_B)*(Y_A-Y_B)/Y_B;
            }
         break;
    }
	return S;
}
/********************************************************************/
void TComponent::operator =(TComponent* Comp_A)
{
	for (int j=0;j<NPoint;j++)
		(*(Data+j))= *(Comp_A->Data+j);
}
/********************************************************************/
void TComponent::operator +=(TComponent* Comp_A)
{
	for (int j=0;j<NPoint;j++)
		(*(Data+j))+= *(Comp_A->Data+j);
}
/********************************************************************/
void TComponent::operator -=(TComponent* Comp_A)
{
	for (int j=0;j<NPoint;j++)
		(*(Data+j))-= *(Comp_A->Data+j);
}
/********************************************************************/
void TComponent::operator +=(float A)
{
	for (int j=0;j<NPoint;j++)
		(*(Data+j))+= A;
}
/********************************************************************/
void TComponent::operator -=(float A)
{
	for (int j=0;j<NPoint;j++)
		(*(Data+j))-= A;
}
/********************************************************************/
void TComponent::operator *=(float A)
{
	for (int j=0;j<NPoint;j++)
		(*(Data+j))*= A;
}
/********************************************************************/
void TComponent::operator /=(float A)
{
   if (A>MinFloat)
	for (int j=0;j<NPoint;j++)
		(*(Data+j))/= A;
}
/********************************************************************/
float TComponent::NoiseVal(int StartPoint)
{
   int j,N;
	float BaseVal,Noise,S;
   N=1+0.7*StartPoint;
   BaseVal=0;
   for (j=0;j<N;j++)
   	BaseVal+= *(Data+j);
   BaseVal/=N;
   Noise=0;
   for (j=0;j<N;j++)
   	{
      S= *(Data+j)-BaseVal;
      Noise+=S*S;
      }
   Noise/=N;
   if (Noise<MinFloat) Noise=MinFloat;
   return (sqrt(Noise));
}   


/********************************************************************/
float TComponent::MinVal(int StartPoint, int EndPoint, int SmoothPoint)
{
	float Extrem=30000000.0;
	float DataX[101];
	float DataY[101];
	int beg, end,N,Smooth;
	int j,ExtremPoint,Count;
	TComponent* AuxComp;
	if(EndPoint<2) N = NPoint-1;
	else	N  = EndPoint;
	if (SmoothPoint<50)Smooth=SmoothPoint;
		else Smooth=50;
	if ((StartPoint>=0)&&(StartPoint<NPoint-2))
	{
		// Finding the minimum value and its location
		for (j=StartPoint;j<N;j++)
			if ( Extrem > *(Data+j) ) { Extrem = *(Data+j); ExtremPoint=j;}
		if ((StartPoint)&&(ExtremPoint>2))
		// finging the Min for the region starting from StartPoint
		if ((Smooth>2)&&(StartPoint<N-2*Smooth))
		// polinomial approximation near the minimum point 
		{
			if (StartPoint<=ExtremPoint-Smooth)	{beg=ExtremPoint-Smooth; end=ExtremPoint+Smooth;}
			else {beg=StartPoint; end=StartPoint+2*Smooth;}
			for (j=0;j<2*Smooth+1;j++) {DataX[j]= *(Base+j+beg);	DataY[j]= *(Data+j+beg);}
			AuxComp = new TComponent(DataY, DataX, 2*Smooth+1);
			AuxComp->Smooth(2*Smooth+1,2*Smooth+1);
			Extrem = AuxComp->MinVal();
			delete AuxComp;
		}
		else
		{
			Extrem=0;
			Count=0;
			// finging the average value around minimum 
			for (j=ExtremPoint-2;(j<ExtremPoint+2)&&(j<N-2);j++)
				{
				Extrem+= *(Data+j);
				Count++;
				}
			Extrem/=Count;
		}
	}
	return Extrem;
}
/*********************************************************************/
float TComponent::MaxVal(int StartPoint, int EndPoint, int SmoothPoint)
{
	float Extrem=-3000000000.0;
	int j,ExtremPoint,Count,N,Smooth;
	float DataX[101];
	float DataY[101];
	int beg, end;
	TComponent* AuxComp;
	if(EndPoint<2) N = NPoint-1;
	else	N  = EndPoint;
	if (SmoothPoint<50)Smooth=SmoothPoint;
		else Smooth=50;
	if ((StartPoint>=0)&&(StartPoint<NPoint-2))
	{
		// Finding the maximum value and its location
		for (j=StartPoint;j<N;j++)
			if ( Extrem < *(Data+j) ) { Extrem = *(Data+j); ExtremPoint=j;}
		if ((StartPoint)&&(ExtremPoint>2))
		// finging the Max for the region starting from StartPoint
		if ((Smooth>2)&&(StartPoint<N-2*Smooth))
		// polinomial approximation near the maximum point 
		{
			if (StartPoint<=ExtremPoint-Smooth)	{beg=ExtremPoint-Smooth; end=ExtremPoint+Smooth;}
			else {beg=StartPoint; end=StartPoint+2*Smooth;}
			for (j=0;j<2*Smooth+1;j++) {DataX[j]= *(Base+j+beg);	DataY[j]= *(Data+j+beg);}
			AuxComp = new TComponent(DataY, DataX, 2*Smooth+1);
			AuxComp->Smooth(2*Smooth+1,2*Smooth+1);
			Extrem = AuxComp->MaxVal();
			delete AuxComp;
		}
		else	
		{
			Extrem=0;
			Count=0;
			// finging the average value around maximum 
			for (j=ExtremPoint-2;(j<ExtremPoint+2)&&(j<N-2);j++)
				{
				Extrem+= *(Data+j);
				Count++;
				}
			Extrem/=Count;
		}
	}
	return Extrem;
}
/*********************************************************************/
 float   TComponent::ApproxZeroLeft(void)
{
	return *Data;
}
/*********************************************************************/
float   TComponent::ApproxZeroRight(void)
{
	return *(Data+NPoint-1);
}

/******             Smoothing the certain cut of the curve     *******/
void TComponent::SmoothCut(float Begin, float End, float Fraction)
{
	int BegPoint, EndPoint;
	BegPoint=FindPoint(Begin);
	EndPoint=FindPoint(End);
	SmoothCut(BegPoint,EndPoint,Fraction);
}

void TComponent::SmoothCut(int BegPoint,int EndPoint, float Fraction)
{
	double A[8];
	double B[64];
	double C[8];
	int k,m,i,j,p;
	int NBefore,NAfter; // the power of the curve before and after smoothing
	float far* crX;
	float far* crY;
	double  Xk,X;
	float  BegY,EndY,BegX,EndX,CfY;
	NBefore=EndPoint-BegPoint+1;
	BegY= *(Data+BegPoint);
	EndY= *(Data+EndPoint+1);
   /*
	BegX= *(Base+BegPoint);
	EndX= *(Base+EndPoint+1);
   CfY=(EndY-BegY)/(EndX-BegX);
	crX=Base+BegPoint;
	crY=Data+BegPoint;
	for(j=0;j<NBefore;j++)
		*(crY+j)=0.5*( (*(crY+j))+BegY+CfY*(*(crX+j)-BegX));
	*/

	NAfter=1+NBefore/5;
	if (NAfter<3) NAfter=3;
	if (NAfter>8)NAfter=8;
	//A=(double*)malloc((size_t)(8*sizeof(double)));	B=(double*)malloc((size_t)(64*sizeof(double)));	C=(double*)malloc((size_t)(8*sizeof(double)));
	crX=Base+BegPoint;
	crY=Data+BegPoint;
	for (k=0;k<NAfter;k++)
		{
		*(C+k)=0;
		for (i=0;i<NBefore;i++)
			{
			Xk= (*(crY+i));
			X= (*(crX+i));
			for (p=1;p<=k;p++)
				Xk*=X; 
			(*(C+k))+=Xk;
			}
		for (m=0;m<NAfter;m++)
			{
			*(B+k*NAfter+m)=0;
			for(i=0;i<NBefore;i++)
				{
				Xk=1.0;
				X= (*(crX+i));
				for (p=1;p<=(k+m);p++)
					Xk*=X;
				(*(B+NAfter*k+m))+=Xk;
				}
			}
		}
	EquSet2(A,B,C,NAfter);
	for (i=0;i<NBefore;i++)
		{
		*(crY+i)*=Fraction;
		//X=Beg+((End-Beg)*i/NAfter);
		//*(crX+i)=X;
		X= *(crX+i);
		for (k=0;k<NAfter;k++)
			{
			Xk=( *(A+k));
			for(p=1;p<=k;p++)
				Xk*=X;
			*(crY+i)+=(1-Fraction)*Xk;
			}
		}
	//free(A);	free(B);	free(C);

	ReSpline();
}

/**********************************************************************/
float TComponent::OutSpline(float X, int FastFlag)
{
	int j;
	//return X/10;
	float  X01,X02,X21,f;
	if ( X<= (*Base) )
		f = *Data ;
	else
		if ( X >=(*(Base+NPoint-1)) )
			f= (*(Data+NPoint-1));
		else
         {
         if (FastFlag)
         	j=(int)(NPoint*(X-(*Base))/(*(Base+NPoint-1)-(*Base)));
         else
         	{
				if ( X <= *(Base+1) )
					j=1;
				else
					{
					j=2;
					while ((X > *(Base+j))&&(j<NPoint-1))
						j++;
					}
            }
     if (j<1) j=1;
     if (j>NPoint-1) j=NPoint-1;
			X01=X-(*(Base+j-1));
			X02=X-(*(Base+j));
			X21=X01-X02;

			if ( (IsSplined)&&(j>1)&&(j<NPoint-1) )
				{
				f=(*(Splines+j))*X01*X01*X01/(6*X21)-(*(Splines+j-1))*X02*X02*X02/(6*X21);
				f+=((*(Data+j))/X21-(X21*(*(Splines+j)))/6)*X01-((*(Data+j-1))/X21-(X21*(*(Splines+j-1)))/6)*X02;
				}
			else
         	f= (*(Data+j))+X02*( (*(Data+j))- (*(Data+j-1)))/X21;
         }
         return f;
         //return (X/10);
}

/******************************************************/
void TComponent::Save(const char* FileName,float Dx,float Dy, int Format)
{
	
	ofstream outFile;
	int j;
		char crStr[80];
	outFile.open(FileName,out);
	if (Format==FILE_DATA)
		{
			for (j=0;j<6;j++)
				{
			sprintf((LPSTR)crStr,"%s","   ");
			strcat(crStr,"\n");
			outFile.write(crStr,strlen(crStr));
			}
		sprintf((LPSTR)crStr,"%f%s%f",Dx,"   ",Dy);
		strcat(crStr,"\n");
		outFile.write(crStr,strlen(crStr));
    	}
	for (j=0;j<NPoint;j++)
		{
		if (Format==FILE_DATA)
			sprintf((LPSTR)crStr,"%f%s%f",(*(Base+j)),"   ",(*(Data+j)));
		if (Format==FILE_ORG)
			sprintf((LPSTR)crStr,"%f\t%f",(*(Base+j)),(*(Data+j)));
		strcat(crStr,"\n");
		outFile.write(crStr,strlen(crStr));
		}
	outFile.close();
	
}
/******************************************************/
int TComponent::Read(const char* FileName, int Format)
{
	
	HGLOBAL HMemX;
	HGLOBAL HMemY;
	float far* DataX;
	float far* DataY;
	int N_Data;
	int SortEnd;
	float Buf1,Buf2;
 	char arStr[128];
  	char Str1[64];
	char Str2[64];
	char* cr_char;
	ifstream inpFile;
	int symb1,symb2,crL,j;
	HMemX=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,64000);
	DataX=(float far*)GlobalLock(HMemX);
	HMemY=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,64000);
	DataY=(float far*)GlobalLock(HMemY);
	N_Data=0;
	if ((DataX!=NULL)&&(DataY!=NULL))
	{
	inpFile.open( FileName, in);
	for (N_Data=0;N_Data<6;N_Data++)
    	inpFile.getline(arStr,128,'\n');
		// reading data from file
		N_Data=0;
   	while( (!inpFile.eof())&&(N_Data<=16000) )
   		{
		inpFile.getline(arStr,128,'\n');
      // Cycle of string analysis
    	symb1=0;
      	crL=0;
	   	cr_char=arStr;
	   	if (*cr_char!=';')
      	{
			while((((*cr_char==' ')&&(symb1>=1))||((*cr_char==',')&&(symb1>=1))||(crL>128))==0)
	        	{
	        	switch (*cr_char)
					{
					case '0':Str1[symb1]=*cr_char;symb1++;break;
					case '1':Str1[symb1]=*cr_char;symb1++;break;
					case '2':Str1[symb1]=*cr_char;symb1++;break;
					case '3':Str1[symb1]=*cr_char;symb1++;break;
					case '4':Str1[symb1]=*cr_char;symb1++;break;
					case '5':Str1[symb1]=*cr_char;symb1++;break;
					case '6':Str1[symb1]=*cr_char;symb1++;break;
					case '7':Str1[symb1]=*cr_char;symb1++;break;
					case '8':Str1[symb1]=*cr_char;symb1++;break;
					case '9':Str1[symb1]=*cr_char;symb1++;break;
					case '+':Str1[symb1]=*cr_char;symb1++;break;
					case '-':Str1[symb1]=*cr_char;symb1++;break;
					case '.':Str1[symb1]=*cr_char;symb1++;break;
					case 'e':Str1[symb1]=*cr_char;symb1++;break;
					case 'E':Str1[symb1]=*cr_char;symb1++;break;
               }
	         crL++;
	         cr_char++;
				}
         	Str1[symb1]='\0';
         	symb2=0;
			while (crL<=78)
				{
	         switch (*cr_char)
            	{
					case '0':Str2[symb2]=*cr_char;symb2++;break;
					case '1':Str2[symb2]=*cr_char;symb2++;break;
					case '2':Str2[symb2]=*cr_char;symb2++;break;
					case '3':Str2[symb2]=*cr_char;symb2++;break;
					case '4':Str2[symb2]=*cr_char;symb2++;break;
					case '5':Str2[symb2]=*cr_char;symb2++;break;
					case '6':Str2[symb2]=*cr_char;symb2++;break;
					case '7':Str2[symb2]=*cr_char;symb2++;break;
					case '8':Str2[symb2]=*cr_char;symb2++;break;
					case '9':Str2[symb2]=*cr_char;symb2++;break;
					case '+':Str2[symb2]=*cr_char;symb2++;break;
					case '-':Str2[symb2]=*cr_char;symb2++;break;
					case '.':Str2[symb2]=*cr_char;symb2++;break;
					case 'e':Str2[symb2]=*cr_char;symb2++;break;
					case 'E':Str2[symb2]=*cr_char;symb2++;break;
               }
	         	crL++;
	         	cr_char++;
	         }
				Str2[symb2]='\0';
         	if ( (symb1 > 0)&&(symb2 > 0) )
            	{
	    		*(DataX+N_Data)=(float)atof(Str1);
				*(DataY+N_Data)=(float)atof(Str2);
				N_Data++;
				}
			}
		} // end of reading data
		
	inpFile.close();
   }
   if (N_Data >NDATA_MAX+1)
	N_Data=NDATA_MAX+1;
   if (N_Data >=2 )
	{
   NPoint=N_Data-2;
   // sorting the data
   do
   	{
		SortEnd=1;
		for (j=1;j<NPoint;j++)
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
   for (j=0;j<NPoint;j++)
   	{
		*(Base+j)= *(DataX+j);
		*(Data+j)= *(DataY+j);
		}
   ReSpline();
   }
   GlobalUnlock(HMemX);
   GlobalFree(HMemX);
   GlobalUnlock(HMemY);
   GlobalFree(HMemY);
   return (N_Data);
  
}
/********************************************************************/
int TComponent::Smooth(int  NBefore,int NAfter)
{
	double A[5];
	double B[25];
	double C[5];
	int k,m,i,j,p;
	int NCut;
	float far* crX;
	float far* crY;
	double  Xk,X;
	float Beg,End;
	//A=(double*)malloc(5*sizeof(double));	B=(double*)malloc(25*sizeof(double)); C=(double*)malloc(5*sizeof(double));
	if ( NBefore>(NPoint/2)) NBefore=(int)(NPoint/2);
	if (NBefore<4) NBefore=4;
	if (NAfter>NBefore) NAfter=NBefore;
	NCut=(int)(NPoint/NBefore);
	for (j=1;j<=NCut;j++)
	{
		crX=Base+(NBefore*(j-1));
		Beg= *crX;
		End= *(Base+NBefore*j);
		crY=Data+(NBefore*(j-1));
		for (k=0;k<3;k++)
		{
			*(C+k)=0;
			for (i=0;i<NBefore;i++)
			{
				Xk= (*(crY+i));
				X= (*(crX+i));
				for (p=1;p<=k;p++)
					Xk*=X; 
				(*(C+k))+=Xk;
			}
			for (m=0;m<3;m++)
			{
				*(B+k*3+m)=0;
				for(i=0;i<NBefore;i++)
				{
					Xk=1.0;
					X= (*(crX+i));
					for (p=1;p<=(k+m);p++)
						Xk*=X;
					(*(B+3*k+m))+=Xk;
				}
			}
		}
		EquSet2(A,B,C,3);
		crX=Base+(NAfter*(j-1));
		crY=Data+(NAfter*(j-1));
		for (i=0;i<NAfter;i++)
		{
			*(crY+i)=0;
			X=Beg+((End-Beg)*i/NAfter);
			*(crX+i)=X;
			for (k=0;k<3;k++)
			{
				Xk=( *(A+k));
				for(p=1;p<=k;p++)
					Xk*=X;
				*(crY+i)+=Xk;
			}
		}
	}
	NPoint=NCut*NAfter;
	//free(A);	free(B);	free(C);
	return NPoint;
}
/****************************************************/
void TComponent::Randomize(float Amp,float Amp10)
{
	int j,i;
   float r;
   //randomize();
   for (j=0;j<NPoint-1;j++)
   	*(Data+j)+=Amp*(2*N_Spline*Rand()-N_Spline)/N_Spline;
   for (j=0;j<NPoint-1;j+=20)
      {
      r=0.5*Amp10*(2*N_Spline*Rand()-N_Spline)/N_Spline;
      for (i=0;(i<20)&&(j+i<NPoint-1);i++)
         *(Data+j+i)+=r*exp(-0.04*(i-10)*(i-10));

      }
   for (j=0;j<NPoint-1;j+=50)
      {
      r=0.5*Amp10*(2*N_Spline*Rand()-N_Spline)/N_Spline;
      for (i=0;(i<50)&&(j+i<NPoint-1);i++)
         *(Data+j+i)+=r*exp(-0.012*(i-10)*(i-10));

      }

}
/****************************************************/
float TComponent::GetBaseLevel(int jBeg, int jEnd)
{
	int j;
   float BaseLev=0;
   if ((jBeg>=0)&&(jEnd>jBeg))
   for (j=jBeg;j<jEnd;j++)
   	BaseLev+= *(Data+j);
   return (BaseLev/=(jEnd-jBeg));
}
void TComponent::SetStimPoint(int NewStimPoint)
{
	StimPoint=NewStimPoint;
}
/**************************************************************\
*						Realization of TKinetics class				 	*
\**************************************************************/
TKinetics::TKinetics(float* Base,	int nPoint)
{
   int j;
	NVar=0;	NStage=0;	NConst=0;	TagNum=0;	NTerm=0;	NVarMax=0;
   tmBase=Base;	NPoint=nPoint;
   NMem=0;
   HMem[NMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)64000);
   if ( HMem[NMem]!=NULL)
		DataBegin=(float far*)GlobalLock(HMem[NMem]);
	if (DataBegin!=NULL)
		{
		BytesRem=GlobalSize(HMem[NMem]);
		NMem++;
		}
   while	(BytesRem >= (NPoint+1)*sizeof(float))
   	{
      VarY[NVarMax]=DataBegin;
      DataBegin+=(NPoint+1);
      BytesRem-=(NPoint+1)*sizeof(float);
      NVarMax++;
      }
   for (j=0;j<N_StageMax;j++)
   	{
      strcpy((RateConst+j)->Name,"\0");	(RateConst+j)->Fixed=1;
		(RateConst+j)->Min=-1.0e-10;	(RateConst+j)->Max=1.0e+10;
      (RateConst+j)->Val=0;
      }
   for (j=0;j<N_ReagentMax;j++)
   	{
      strcpy((Reagent+j)->Name,"\0");	(Reagent+j)->Fixed=0;	(Reagent+j)->Init=0;
      }
}
/**********************************/
TKinetics::~TKinetics(void)
{
   int j=0;
	if (NMem!=0)
		for (j=0;j<NMem;j++)
			{
			GlobalUnlock(HMem[j]);
			GlobalFree(HMem[j]);
			}
}
/********************************************/
int	TKinetics::SetScheme(TStageStr*	DestStage, int nDestStage)
{
	int j,i,SetOK;
   NVar=0;	NStage=0;	NConst=0;
   NStage=nDestStage;
   for (j=0;j<NStage;j++)
   	{
      (Stage+j)->iR1=-1;	(Stage+j)->iR2=-1;
      (Stage+j)->iP1=-1;	(Stage+j)->iP2=-1;
      (Stage+j)->iRC=-1;	(Stage+j)->cfRC=1;
      }
   //analysing	user data
   for (j=0;j<NStage;j++)
      //seeking reagent and constant names
      {
      if (!strcmp((DestStage+j)->nmR1,"\0"))
         {
         i=0;
         while((strcmp((Reagent+i)->Name,(DestStage+j)->nmR1)!=0)&&(i<NVar))
         	i++;
         if (i==NVar)
         	//new name!
            {
            strcpy((Reagent+i)->Name,(DestStage+j)->nmR1);
            NVar++;
            }
         (Stage+j)->iR1=i;
         }
      if (!strcmp((DestStage+j)->nmR2,"\0"))
         {
         i=0;
         while((strcmp((Reagent+i)->Name,(DestStage+j)->nmR2)!=0)&&(i<NVar))
         	i++;
         if (i==NVar)
         	//new name!
            {
            strcpy((Reagent+i)->Name,(DestStage+j)->nmR2);
            NVar++;
            }
         (Stage+j)->iR2=i;
         }
      if (!strcmp((DestStage+j)->nmP1,"\0"))
         {
         i=0;
         while((strcmp((Reagent+i)->Name,(DestStage+j)->nmP1)!=0)&&(i<NVar))
         	i++;
         if (i==NVar)
         	//new name!
            {
            strcpy((Reagent+i)->Name,(DestStage+j)->nmP1);
            NVar++;
            }
         (Stage+j)->iP1=i;
         }
      if (!strcmp((DestStage+j)->nmP2,"\0"))
         {
         i=0;
         while((strcmp((Reagent+i)->Name,(DestStage+j)->nmP2)!=0)&&(i<NVar))
         	i++;
         if (i==NVar)
         	//new name!
            {
            strcpy((Reagent+i)->Name,(DestStage+j)->nmP2);
            NVar++;
            }
         (Stage+j)->iP2=i;
         }

      if (!strcmp((DestStage+j)->nmRC,"\0"))
         {
         i=0;
         while((strcmp((RateConst+i)->Name,(DestStage+j)->nmRC)!=0)&&(i<NConst))
         	i++;
         if (i==NConst)
         	//new name!
            {
            strcpy((RateConst+i)->Name,(DestStage+j)->nmRC);
            NConst++;
            }
         (Stage+j)->iRC=i;
         }
      if (!strcmp((DestStage+j)->cfRC,"\0"))
         (Stage+j)->cfRC=atof((DestStage+j)->cfRC);
      }
   NTerm=NStage;
   SetOK=1;
   for (j=0;j<NVar;j++)
   	for (i=0;i<NTerm;i++)
      	*((Reagent+j)->cfTerm+i)=0;
   //preparing the equation terms
   for (j=0;j<NStage;j++)
      if ((Stage+j)->iRC!=-1)
      	{
	      if ((Stage+j)->iR1!=-1)
   	   	*((Reagent+j)->cfTerm+(Stage+j)->iR1)=-1;
      	if ((Stage+j)->iR2!=-1)
      		*((Reagent+j)->cfTerm+(Stage+j)->iR2)=-1;
	      if ((Stage+j)->iP1!=-1)
   	   	*((Reagent+j)->cfTerm+(Stage+j)->iP1)=1;
      	if ((Stage+j)->iP2!=-1)
      		*((Reagent+j)->cfTerm+(Stage+j)->iR2)=1;
         if (((Stage+j)->iR1!=-1)||((Stage+j)->iR2!=-1))
         	SetOK*=1;
         else
         	SetOK*=0;
         if (((Stage+j)->iP1!=-1)||((Stage+j)->iP2!=-1))
         	SetOK*=1;
         else
         	SetOK*=0;
       	}
       else
          SetOK*=0;
   if (SetOK)
   while (NVarMax<NVar)
   	{
      HMem[NMem]=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,(DWORD)64000);
   	if ( HMem[NMem]!=NULL)
			DataBegin=(float far*)GlobalLock(HMem[NMem]);
		if (DataBegin!=NULL)
			{
			BytesRem=GlobalSize(HMem[NMem]);
			NMem++;
			}
	   while	(BytesRem >= (NPoint+1)*sizeof(float))
   		{
      	VarY[NVarMax]=DataBegin;
	      DataBegin+=(NPoint+1);
   	   BytesRem-=(NPoint+1)*sizeof(float);
      	NVarMax++;
      	}
      }
   return SetOK;
}
/********************************************/
int	TKinetics::GetScheme(TStageStr* AStageStr)
{
return 1;
}

/********************************************/
int	TKinetics::CheckRConst()
{
return 1;
}

/********************************************/
int	TKinetics::CheckRConstLimits()
{
return 1;
}

/********************************************/
int	TKinetics::CheckInitVal()
{
return 1;
}

/********************************************/
int	TKinetics::Simulate()
{
return 1;
}

/********************************************/
float*	TKinetics::GetData(int nVar)
{
   if (nVar>=0)
   	return VarY[nVar];
   else
   	return NULL;
}
