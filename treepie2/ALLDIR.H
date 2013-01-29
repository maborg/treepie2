/*
Copyright (c) 2001/2009, Marco Borgna (mailto:borgna.marco@gmail.com)
All rights reserved.

Redistribution and use in source and binary forms, 
with or without modification, are permitted provided 
that the following conditions are met:

Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 
Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 

  Neither the name of  Marco Borgna nor the names of its contributors 
  may be used to endorse or promote products derived from this software without 
  specific prior written permission. 

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. 
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 OF SUCH DAMAGE.
*/
#define CHECKMESSAGE MSG        msg;\
if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE  )\
if (GetMessage(&msg, NULL, 0, 0) ){ \
TranslateMessage(&msg); \
DispatchMessage(&msg); }





/******************************************************************************/
//
//								CLASSE ALLDIR 
//
/******************************************************************************/
const double RADIUS = 0.4f;

class AllDir
{
	AllDir* iv_parent;

	AllDir **iv_aList;
	WCHAR iv_sDirName[MAX_PATH];
	long iv_nLevelDir;
	int iv_bSorted;

protected:
	double iv_nStartAngle;
	double iv_nSweepAngle;
	int GetRangeFromStep (int nStep,
							  double &nStartRadius,
							  double &nRadius );
	 long iv_ActivityTime;

int GetColor( bool blit, int &r,int &g,int &b   );
public:

	int iv_nStep;

	INT64	iv_nSize;

	INT64	ListAllDir( WCHAR* sFile,WCHAR* sDirName);
	void	FreeAll( );

	void setActivity( );

	double	Paint(double nAngle,double nStartAngle,int nStep,INT64 nUpSize,bool bLit);
	int	Find(double radius, double nAngle,AllDir **icFound);
	int Find( WCHAR* lpDir, AllDir** icFound);


	WCHAR*	GetName(){return iv_sDirName;};
	double	Repaint(bool bLit,WCHAR * sbuffer,WCHAR  * sCenterMsg);

	// constructor ... 
	AllDir(AllDir *p){
			iv_parent=p;
			iv_ActivityTime = 0;

	}


	//for Array
	AllDir(){
		iv_bSorted = 1;
	}

//    HTREEITEM iv_hTreeItem;
	
	 void StatusMessage(WCHAR * sBuffer,WCHAR *  sCenterMsg);

	void DrawSector(double nStartAngle,double nAngle,
			   double nStartRadius,double nRadius,bool bLit );





};
