#pragma once

#include "resource.h"


// WinMAin Class
class winmain
{
public:
	HINSTANCE hInst;
	WCHAR lpCmdLine[1024];
	HWND  hWnd; 
//	HWND hStatusWnd;
	HDC   hDC; 
	HGLRC hRC; 
};


void glPrint(const WCHAR *fmt, ...);