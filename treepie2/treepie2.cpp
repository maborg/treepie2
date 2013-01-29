// treepie2.cpp : Defines the entry point for the application.
//
#include <windows.h>

#include "treepie2.h"
#include "OpenfileBox.h"	
#include "AllDir.h"	

#define MAX_LOADSTRING 100

#include <GL/gl.h> 
#include <GL/glu.h> 
#include <vector>

#include <commctrl.h>
#include <stdio.h>
#include <math.h>
bool DrawSplash();
#define SWAPBUFFERS SwapBuffers(gW.hDC) 
winmain gW ;
bool gScanning = false;
GLvoid resize(GLsizei, GLsizei,GLsizei, GLsizei); 
GLvoid initializeGL(); 
GLvoid drawScene(GLvoid); 
DWORD WINAPI createObjects(LPVOID  );
double pi=3.141592654;
AllDir* g_icFoundPtr=0;
AllDir* g_icCurrentPtr=0;
HANDLE ghThread = 0; 
bool gbCreate= FALSE;
/*AllDir Globals*/
AllDir g_icAllDir((AllDir *)0);


BOOL bSetupPixelFormat(HDC hdc) 
{ 
	PIXELFORMATDESCRIPTOR pfd, *ppfd; 
	int pixelformat; 

	ppfd = &pfd; 

	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
	ppfd->nVersion = 1; 
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |  PFD_DOUBLEBUFFER;
	// PFD_DOUBLEBUFFER; PFD_SUPPORT_GDI
	ppfd->dwLayerMask = PFD_MAIN_PLANE; 
	ppfd->iPixelType = PFD_TYPE_RGBA;//PFD_TYPE_COLORINDEX; 
	ppfd->cColorBits = 8; 
	ppfd->cDepthBits = 16; 
	ppfd->cAccumBits = 0; 
	ppfd->cStencilBits = 0; 

	pixelformat = ChoosePixelFormat(hdc, ppfd); 

	if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
	{ 
		MessageBox(NULL, L"ChoosePixelFormat failed", L"Error", MB_OK); 
		return FALSE; 
	} 

	if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
	{ 
		MessageBox(NULL, L"SetPixelFormat failed", L"Error", MB_OK); 
		return FALSE; 
	} 

	return TRUE; 
} 

void manage_WM_CREATE(HWND hWnd) 
{
	RECT rect;
	gW.hDC = GetDC(hWnd); 

	if (!bSetupPixelFormat(gW.hDC)) 
		MessageBox (NULL, L"bSetupPixelFormat failed!", NULL, MB_OK );
	gW.hRC = wglCreateContext(gW.hDC); 
	wglMakeCurrent(gW.hDC, gW.hRC); 
	GetClientRect(hWnd, &rect); 
	resize(0,0,rect.right, rect.bottom); 
	initializeGL();


}



void manage_WM_COMPLETE(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rect;

	GetClientRect(hWnd, &rect); 
	resize(0,0,rect.right, rect.bottom); 

	drawScene();

}
void manage_WM_SIZE(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	resize(0,0,rect.right, rect.bottom); 

	drawScene();

}


void manage_WM_LBUTTONDOWN(HWND hWnd,WPARAM  wParam, LPARAM  lParam)
{

	double nAngle,nX,nY,Dummy;
	RECT rect;
	GLdouble mp[16], mw[16];
	GLint vw[4];

	GetClientRect(hWnd,&rect);

	// calcolo nX e ny rispetto al centro
	nX = (double) LOWORD(lParam);
	nY = (double) HIWORD(lParam);

	glGetDoublev( GL_MODELVIEW_MATRIX, mw);
	glGetDoublev( GL_PROJECTION_MATRIX, mp);   
	glGetIntegerv(GL_VIEWPORT,vw);

	//punti nella view !
	gluUnProject( nX, nY,0.0f,mw,mp,vw,&nX ,&nY,&Dummy);

	// angolo
	nAngle = 360-((atan2( nX ,nY )+pi)/(2*pi)) *360.0 ; 

	AllDir *icFound;
	static AllDir *icOldFound=0;

	g_icFoundPtr =0;
	//cerco nella lista distanza e angolo
	if ( g_icCurrentPtr->Find( sqrt(nX*nX+nY*nY),nAngle,(AllDir**)&icFound)>=0 )
	{
		gluUnProject( 0., 30.,0.0f,mw,mp,vw,&nX ,&nY,&Dummy);

		g_icFoundPtr = icFound;

		drawScene();		
	} else
	{
		/*	WCHAR sbuffer[2000];
		sprintf(sbuffer,"%f %f nAngle %f %s ",nX,nY,nAngle," ok");
		SendMessage(gW.hStatusWnd, SB_SETTEXT, (WPARAM) 0,(LPARAM) sbuffer);*/
	}
}


void startup()
{
	glClearColor(0.4,0.4,0.9,0.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glPushMatrix();
		glPushMatrix();
			glScalef(0.09,0.09,0.09);	
			glRotatef (-45.0 + rand()%20, 1.0, 0.1, 0.1);
			glTranslated(-7.0,0.0,-2.0);	

			DrawSplash();
		glPopMatrix();

		glColor3ub(220,220,220);
		glTranslated(0.0,0.0,-0.1);	
		glRasterPos2f(-2.0, 1.2);
		glPrint(L"working ");

		if(wcslen(gW.lpCmdLine)>0)
		{								
			glRasterPos2f(-2.0, 1.0);
			WCHAR sBuffer[1024];
			wcscpy(sBuffer,gW.lpCmdLine);
			glPrint(sBuffer);
		}
	glPopMatrix();
	glFlush ();

	SWAPBUFFERS; 

	if ( !gScanning ){ //&& !ghThread
		g_icFoundPtr = 0;
		DWORD dwThreadId; 

		ghThread = CreateThread( 
			NULL,                       // default security attributes 
			0,                          // use default stack size  
			createObjects,              // thread function 
			0,							// argument to thread function 
			0,                          // use default creation flags 
			&dwThreadId);               // returns the thread identifier 
	}
	SetThreadPriority(ghThread, THREAD_PRIORITY_ABOVE_NORMAL);
}





// NEHE :D
GLuint	base;
GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	base = glGenLists(96);								// Storage For 96 WCHARacters

	font = CreateFont(	-12,							// Height Of Font
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_BOLD,						// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// WCHARacter Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
		L"Arial");						// Font Name

	oldfont = (HFONT)SelectObject(gW.hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(gW.hDC, 32, 96, base);				// Builds 96 WCHARacters Starting At WCHARacter 32
	SelectObject(gW.hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

GLvoid KillFont(GLvoid)									// Delete The Font List
{
	glDeleteLists(base, 96);							// Delete All 96 WCHARacters
}


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TREEPIE2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	if (lstrlen(lpCmdLine)>0) // sandro manni fix
	{
		//parametri
		int len = lstrlen(lpCmdLine);
		if (len>=2 && lpCmdLine[0]=='"' && lpCmdLine[len-1]=='"')
			lstrcpyn(gW.lpCmdLine, &lpCmdLine[1],len-1);
		else
			lstrcpy(gW.lpCmdLine, lpCmdLine);
	}
	else
	{
		CBrowseForFolder sb(gW.hWnd, 0,L"TreePie.sf.net:\n Choose a folder or a drive to scan:");
		sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT );
		sb.SetSelection((WCHAR *)NULL);
		if (sb.SelectFolder())
		{
			lstrcpy(gW.lpCmdLine, sb.GetSelectedFolder());
		}else{
			PostQuitMessage(0);
		}
	}


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TREEPIE2));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TREEPIE2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TREEPIE2);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


DWORD WINAPI deleteObjects( ) 
{ 	
	g_icCurrentPtr->FreeAll();
	return 0;
} 

GLvoid initializeGL() 
{ 
	glClearColor( .9f,.9f,.9f,1.f); 
	glClearDepth( 1.0 ); 
	glEnable(GL_DEPTH_TEST); 
	BuildFont();
}

GLvoid drawScene(GLvoid) 
{ 
	glClearColor(1,1,1,0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glPushMatrix(); 
	//
	glTranslated(0.0,0.0,-0.9);	


	g_icCurrentPtr->Paint(360.0f,0,1,0,FALSE);    
	// todo fix this double call and use vertex buffer


	glPopMatrix();  

	if (g_icFoundPtr){

		g_icFoundPtr->Repaint(TRUE, L"",L"");

		WCHAR sbuffer[2040];
		wsprintf(sbuffer,L"current root: '%s'",g_icCurrentPtr->GetName() );

		if (g_icCurrentPtr != &g_icAllDir)
			g_icFoundPtr->StatusMessage(sbuffer,L"UP");
		else
			g_icFoundPtr->StatusMessage(sbuffer,L" ");




	}else {

		WCHAR sbuffer[2040];
		wsprintf(sbuffer,L"current root: '%s'",g_icCurrentPtr->GetName()  );

		g_icCurrentPtr->StatusMessage(sbuffer,L" ");

	}

	SWAPBUFFERS; 
} 


GLvoid resize( GLsizei x, GLsizei y,GLsizei width, GLsizei height ) 
{ 
	GLfloat aspect;

	/*	char s[1024];
	mb_log("resize: \n");
	mb_log("\tglViewport( x, y, width, height ): \n");

	sprintf(s,"\tx: %d y: %d w: %d h: %d\n",x,y,width,height);
	mb_log(s);
	*/	
	glViewport( x, y, width, height );
	if ( width > height ){
		aspect = (GLfloat) width / height; 
		glMatrixMode( GL_PROJECTION ); 
		glLoadIdentity(); 
		gluOrtho2D( -3.0*aspect, 3.0*aspect, -3.0, 3.0);
	}else{
		aspect = (GLfloat) height / width; 
		glMatrixMode( GL_PROJECTION ); 
		glLoadIdentity(); 
		gluOrtho2D( -3.0, 3.0, -3.0*aspect, 3.0*aspect);	
	}	 
	glMatrixMode( GL_MODELVIEW ); 
}     


DWORD WINAPI createObjects(LPVOID lpParam ) 
{ 	
	gScanning =true;
	g_icCurrentPtr = &g_icAllDir;
	if (lstrlen(gW.lpCmdLine)>0){
		WCHAR sBuffer[1024];

		lstrcpy(sBuffer,gW.lpCmdLine);
		lstrcat(sBuffer,L"\\");

		g_icCurrentPtr->ListAllDir(sBuffer,gW.lpCmdLine);

	}else{
		g_icCurrentPtr->ListAllDir(L"c:\\",L"c:");
	}

	gbCreate= true;
	gScanning =false;

	DWORD lTime = GetCurrentTime () ;
	//PostMessage (gW.hWnd, WM_COMPLETE, 0, lTime);


	return 0;
} 




GLvoid glPrint(const WCHAR *fmt, ...)					// Custom GL "Print" Routine
{
	WCHAR		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	vswprintf (text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base WCHARacter to 32
	glCallLists(wcslen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}




// DEBUG FUNCTIONS
// ---------------

void mb_log(WCHAR * sL)
{
	/*
	FILE *f;
	f=fopen("c:\\easylog.txt","a+");
	fprintf(f,sL);
	fclose(f);*/
}

