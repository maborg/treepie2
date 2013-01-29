/**********************************************************
FILE:	AllDir.h
AUTHOR:	Marco Borgna
DATE:	August 2001

note:

	Logic of search (some day would be extracted in a virtual class
	and memory data scheme creation)

***********************************************************
Copyright (c) 2001/2009, Marco Borgna (mailto:borgna.marco@gmail.com)
*/

// Windows Header Files:
#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include <stdio.h>
#include <GL/gl.h> 
#include <GL/glu.h> 
#include <time.h>
#include "treepie2.h"

//extern HWND ghStatusWnd;
#include "alldir.h"

extern void WatchDirectory1(LPTSTR lpDir);
extern void mb_log(WCHAR * );
extern winmain gW;
//funzione di confronto usata dal quicksort
int compare( const void *arg1, const void *arg2 ){
	AllDir **a1;
	AllDir **a2;
	a1=(AllDir**) arg1;
	a2=(AllDir**) arg2;

	if ( (*a1)->iv_nSize < (*a2)->iv_nSize )
		return 1;
	else
		return -1;
   };

void AllDir::setActivity( )
{
	iv_ActivityTime = time(0);
}


void AllDir::FreeAll( )
{
	// free childrens before
	for (int ia=0;ia<iv_nLevelDir;ia++){
		iv_aList[ia]->FreeAll();
	}
	// and this then
	free(iv_aList);
	return;
}

// scansione delle sotto directory di sFile 
//(deve essere una dir con lo slash finale)
INT64 AllDir::ListAllDir( WCHAR* sFile,WCHAR* sDirName)
{
	HANDLE hFirst;
	WIN32_FIND_DATA hFind;
	INT64 nSize =0;
	
	WCHAR sCompleteFile[MAX_PATH];
	WCHAR* sListFile = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR));
	long nLevelDir;

	// aggiungo "*.*" assume la DIR con \ finale
	wcscpy(sListFile, sFile); 
	wcscat(sListFile,L"*.*");

	// azzero il conto delle directory
	nLevelDir=0;
	
	//ciclo su tutti le dir della dir per contarle
	hFirst = FindFirstFile( sListFile , &hFind );
	if (hFirst == (HANDLE)0xffffffff) return 0;
	do {
		if ( (hFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
			!(hFind.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)  &&
			 ( hFind.cFileName[wcslen((WCHAR*)hFind.cFileName)-1]!='.') )
		{
			nLevelDir++;
			
		}  
	}while ( FindNextFile (hFirst,&hFind) );
	
	FindClose(hFirst);

	// first an array of pointer
	iv_aList= (AllDir **) malloc( sizeof(AllDir* ) * nLevelDir);

	//just a hack to create the right derived obj at runtime 
	for (int ia=0;ia<nLevelDir;ia++){
		iv_aList[ia] = new AllDir();
	}
	
	// copio nome e numero di subdir
	wcscpy(iv_sDirName, sFile); 
	iv_nLevelDir = nLevelDir;

	// calling virtual function 
//	AddTreeItem(sDirName);

	//ciclo sulle subdir
    int j = 0;
	hFirst = FindFirstFile( sListFile , &hFind );
	delete(sListFile);
	do {
		// se è una dir
		if ( (hFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			!(hFind.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
			 ( hFind.cFileName[wcslen(hFind.cFileName)-1]!='.') )
		{
			// aggiungo lo slash e mi richiamo
			lstrcpy(sCompleteFile, sFile); 
			lstrcat(sCompleteFile,hFind.cFileName);
			lstrcat(sCompleteFile,L"\\");
			
			/*WCHAR buff[1024];
			sprintf(buff,"Now searching: %s (%d/%d)",sCompleteFile,j+1,iv_nLevelDir);
			*/
			nSize += iv_aList[j]->ListAllDir( sCompleteFile,hFind.cFileName);
			j++;
		}else if (!(hFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{ 

			if (hFind.nFileSizeHigh!=-858993460 && !(hFind.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) ) 
				//// ARRRGH!(system information folder)
			{
				//grandezza del file (aritmetica a 64 bit)
				nSize += (INT64)hFind.nFileSizeHigh * ((INT64)MAXDWORD+1) + hFind.nFileSizeLow ;
				//if (nSize > MAXDWORD){
				//	mb_log("ListAllDir: nSize sorpassa MAXDWORD");
				//}

				//WCHAR sBuffer[1024];
				//sprintf(sBuffer,"%s(hFind.nFileSizeHigh  ) : %d (nSize)[%I64d]\n",sDirName,hFind.nFileSizeHigh,nSize );

				//mb_log(sBuffer);
			}else{
			 int kk=0;kk++;
			}

		}	
	}while ( FindNextFile (hFirst,&hFind) );
	FindClose(hFirst);
    iv_nSize = nSize ;
	
	// metto in ordine
	if ( iv_bSorted ==1 )
	{
		qsort( (void *)iv_aList, (size_t) iv_nLevelDir , sizeof( AllDir*  ), compare );
	}
	
	return nSize;
}

double AllDir::Repaint(bool bLit, WCHAR  * sbuffer,WCHAR  * sCenterMsg)
{

	glPushMatrix();
	//glTranslated(0.0,0.0,0.2);
	double pippo= Paint(iv_nSweepAngle,iv_nStartAngle,iv_nStep,iv_nSize,bLit);

	
	glPopMatrix();
	return pippo;
}

double AllDir::Paint(double nAngle,double nStartAngle,int nStep,INT64 nUpSize,bool bLit)
{
	// calcolo l'angolo a partire dall'angolo 
	// che mi è stato passato
	// in proporzione alle dimensioni di questa dir e quella sopra
	// ------------------------------------------------------------

	if (nStep!=1)
		nAngle = (iv_nSize * nAngle  / nUpSize) ;

	 
	// salvo i dati che verranno usati in ricerca ! 
	// ------------------------------------------------------------
	iv_nStartAngle = nStartAngle;
	iv_nSweepAngle = nAngle;
	iv_nStep = nStep;

	// calcolo il raggio interno e esterno
	// ------------------------------------------------------------
	double nStartRadius;
	double nRadius;

  GetRangeFromStep ( nStep, nStartRadius, nRadius );

		
	//escludo le dir troppo piccole
	if (fabs(nAngle)>0.3 )
	{
		// Disegno il settore
		DrawSector(nStartAngle,nAngle,nStartRadius,nRadius,bLit); 
	}
	else
	{
		return nAngle;
	}
	nStep++;
	
	// ciclo nella lista
	for ( int i=0; i<iv_nLevelDir; i++)
	{
	  nStartAngle += iv_aList[i]->Paint(nAngle,  nStartAngle , nStep,iv_nSize,bLit);
	}

	return nAngle;
    
}

int AllDir::Find(double nRadius, double nAngle, AllDir** icFound)
{
	if ( (nAngle < iv_nStartAngle) || ( nAngle > iv_nStartAngle + iv_nSweepAngle) )
		return -1;
	double nStartRadius;
	double nEndRadius;
    GetRangeFromStep ( iv_nStep, nStartRadius, nEndRadius );
	if ( (nRadius >= nStartRadius ) && (nRadius <= nEndRadius )) {
		*icFound=this;
		return 1;
	}
	else {
		for (int i=0; i<iv_nLevelDir; i++)
			if ( iv_aList[i]->Find( nRadius,nAngle,icFound) > 0)
				return 1;
	}
	return -2;
}

int AllDir::Find( WCHAR* lpDir, AllDir** icFound)
{
	long currDir = 0;
	while( currDir < iv_nLevelDir ){
		unsigned int l = wcslen(lpDir);
		unsigned int p = 0;
		while (p<=l ){ //&& p<l2){
			if (iv_aList[currDir]->iv_sDirName[p]!=  lpDir[p]	  &&
				iv_aList[currDir]->iv_sDirName[p]!= (lpDir[p]+32) &&
				iv_aList[currDir]->iv_sDirName[p]!= (lpDir[p]-32)
				){
				break;
			}
		p++;	
		}
		// EQUAL --> FOUND!
		if (p==(l+1) || (p==l && iv_aList[currDir]->iv_sDirName[p]=='\\' ) ) {
			*icFound=iv_aList[currDir];
			return 1;
		}
		// still not foun but  partially matching
		if (p == wcslen(iv_aList[currDir]->iv_sDirName)) {
			return iv_aList[currDir]->Find(lpDir,icFound) ;
		}

		currDir++;

	}
	return 0;
	
}

int AllDir::GetRangeFromStep (int nStep,
						  double &nStartRadius,
						  double &nRadius )
{
	// calcolo nStartRadius
	nStartRadius=0.;
	nRadius=0.;
	for (int j=1; j<nStep; j++)
	{
		nStartRadius += (1/pow(1.1,j));
	}
	nStartRadius = fabs(nStartRadius) * RADIUS;
	nRadius = fabs(nStartRadius + (1/pow(1.1,nStep) )* RADIUS);
	return 0;
}
int palettelen = 28;
int curcolor[3] ;
int palette[] =
{ 
229,236,255,
255,229,249,
229,249,255,
168,191,255,
107,147,255,
255,229,236,
229,255,248,
255,216,107,
255,232,168,
255,235,229,
229,255,235,
236,255,229,
249,255,229,
255,248,229,
229,236,255,
255,229,249,
229,249,255,
168,191,255,
107,147,255,
255,229,236,
229,255,248,
255,216,111,
255,232,158,
255,235,221,
229,255,234,
236,255,220,
249,255,229,
255,248,228

};

boolean first=false;
int AllDir::GetColor( bool blit, int &r,int &g,int &b   )
{
	if (iv_nStep>0){

		r = 250;
		g = 250;
		b = 250;		
	}
	int pos =(int) iv_nStartAngle/360.0*palettelen;
	if (iv_nStep==2){
		r = palette[pos*3]*0.9;
		g = palette[pos*3+1]*0.9;
		b = palette[pos*3+2]*0.9;
		curcolor[0]=r*1.01;
		curcolor[1]=g*1.01;
		curcolor[2]=b*1.01;

	}
	if (iv_nStep>2){
		r = curcolor[0];
		g = curcolor[1];
		b = curcolor[2];
	}
	if (blit){ 	
		r = 10;
		g = 248;
		b = 10;
	}

	return 0;
}


void AllDir::StatusMessage(WCHAR * sBuffer,WCHAR *  sCenterMsg){

		glColor3ub(155,67,120);
		
//		double s,w;
//		GetRangeFromStep(iv_nStep+2,s,w);

		//glRasterPos2f(s*sin(iv_nStartAngle/180.0*3.14) , s*cos(iv_nStartAngle/180.0*3.14));		
		

		glMatrixMode(GL_PROJECTION);
		
		glPushMatrix(); 
			glLoadIdentity();

			RECT rect;
			GetClientRect( gW.hWnd, &rect); 
			glOrtho( rect.left, rect.right, rect.top, rect.bottom,-1,1);
			
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix(); 
				glLoadIdentity();

				glRasterPos2f(5.0f , rect.bottom - 20.0f );	
				if (iv_nSize > 1073741824 )
					glPrint(L"%d GB  ",iv_nSize/1073741824);	
				else
					glPrint(L"%d MB  ",iv_nSize/1048576);					
				glPrint(L"(%d KB  )",iv_nSize/1024);					

				glRasterPos2f(rect.right/2.0f , rect.bottom - 20.0f );		
				glPrint(sBuffer);					

				glColor3ub(55,67,120);

				glRasterPos2f(5.0f , rect.bottom - 40.0f );		

				glPrint(L"directory :");				
				glPrint(GetName());				

				glRasterPos2f(10,20);
				glPrint(L"rightclick explore | doubleclick expand | F5 full rescan");	
				
				glColor3ub(55,67,125);
				glTranslated(0.0,0.0,1.0);
				glRasterPos2f(rect.right/2.0f -5.0f,rect.bottom/2.0f-5.0f);
				glPrint(sCenterMsg);
				
			glPopMatrix();
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
};

void AllDir::DrawSector(double nStartAngle,double nAngle,double nStartRadius,double nRadius,bool bLit )
{	

		GLUquadricObj *quadObj; 
		quadObj= gluNewQuadric();
		if(quadObj)
		{
			glPushMatrix();
			long slices=(long)(nAngle/3.0);
			slices= (slices >1 ?slices:2);

			int r,g,b;
			
			GetColor(bLit,r,g,b);

			// GLU_SILHOUETTE
		  // ------------------------------------
			gluQuadricDrawStyle (quadObj, GLU_SILHOUETTE );
			long redComponent = 10 - (time(0) - iv_ActivityTime);
			if (redComponent <= 0)			
				redComponent = 0;
			else			
				redComponent=redComponent*5;

			if (bLit){
				glColor3ub(180 ,180,180);

			}
			else{
				glColor3d(r*0.55 ,r*0.55,r*0.55);//glColor3ub(55+ redComponent ,67,120);
			}
			
			gluPartialDisk( quadObj,  
				nStartRadius,  
				nRadius,  
				slices,
				1,
				nStartAngle,   
				nAngle    	);

			if (bLit){
				glColor3ub(148+ redComponent,169,201);
			}
			else{
				glColor3ub(255 ,255 - redComponent,255 - redComponent);
			}

			// GLU_FILL
			// ------------------------------------
			gluQuadricDrawStyle (quadObj, GLU_FILL );
			gluQuadricNormals(quadObj, GLU_NONE );
			
			// glTranslated(0.0,0.0,-0.1);		

			
			glColor3ub(r ,g - redComponent,b - redComponent);

			gluPartialDisk( quadObj,  
				nStartRadius,  
				nRadius,  
				slices,
				1,	          
				nStartAngle,   
				nAngle 	);
		

			glPopMatrix();
			gluDeleteQuadric(quadObj);

		}
	
}
