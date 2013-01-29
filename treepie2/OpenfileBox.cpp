///http://www.codeguru.com/Cpp/COM-Tech/shell/print.php/c1325/
//////////////////////////////////////////////////////////////////////
//
// ShellBrowser.cpp: implementation of the CShellBrowser class.
//

#include "stdafx.h"
#include "OpenfileBox.h"

//////////////////////////////////////////////////////////////////////
//
// Construction/Destruction
//



CBrowseForFolder::CBrowseForFolder(const HWND hParent, const LPITEMIDLIST pidl, const WCHAR* strTitle)
{
	m_hwnd = NULL;
	SetOwner(hParent);
	SetRoot(pidl);
	SetTitle(strTitle);
	m_bi.lpfn = BrowseCallbackProc;
	m_bi.lParam = reinterpret_cast<long>(this);
	m_bi.pszDisplayName = m_szSelected;
}


//////////////////////////////////////////////////////////////////////
//
// Implementation
//

void CBrowseForFolder::SetOwner(const HWND hwndOwner)
{
	if (m_hwnd != NULL)
		return;

	m_bi.hwndOwner = hwndOwner;
}

void CBrowseForFolder::SetRoot(const LPITEMIDLIST pidl)
{
	if (m_hwnd != NULL)
		return;

	m_bi.pidlRoot = pidl;
}

const WCHAR* CBrowseForFolder::GetTitle() const
{
	return   m_bi.lpszTitle; //copied ???
}

void CBrowseForFolder::SetTitle(const WCHAR* strTitle)
{
	if (m_hwnd != NULL)
		return;
	m_bi.lpszTitle = strTitle;
}


void CBrowseForFolder::SetFlags(const UINT ulFlags)
{
	if (m_hwnd != NULL)
		return;

	m_bi.ulFlags = ulFlags;
}

WCHAR* CBrowseForFolder::GetSelectedFolder() const
{
	return (WCHAR *)m_szSelected;
}



bool CBrowseForFolder::SelectFolder()
{
	bool bRet = false;

	LPITEMIDLIST pidl;
	if ((pidl = ::SHBrowseForFolder(&m_bi)) != NULL)
	{

		if (SUCCEEDED(::SHGetPathFromIDList(pidl, m_szSelected)))
		{
			bRet = true;
			m_strPath = m_szSelected;
		}

		LPMALLOC pMalloc;
		//Retrieve a pointer to the shell's IMalloc interface
		if (SUCCEEDED(SHGetMalloc(&pMalloc)))
		{
			// free the PIDL that SHBrowseForFolder returned to us.
			pMalloc->Free(pidl);
			// release the shell's IMalloc interface
			(void)pMalloc->Release();
		}
	}
	m_hwnd = NULL;

	return bRet;
}

void CBrowseForFolder::OnInit() const
{

}

void CBrowseForFolder::OnSelChanged(const LPITEMIDLIST pidl) const
{
	(void)pidl;
}

void CBrowseForFolder::EnableOK(const bool bEnable) const
{
	if (m_hwnd == NULL)
		return;

	SendMessage(m_hwnd, BFFM_ENABLEOK, (long)bEnable, NULL);
}

void CBrowseForFolder::SetSelection(const LPITEMIDLIST pidl) const
{
	if (m_hwnd == NULL)
		return;

	SendMessage(m_hwnd, BFFM_SETSELECTION, FALSE, (long)(pidl));
}

void CBrowseForFolder::SetSelection(const WCHAR* strPath) const
{
	if (m_hwnd == NULL)
		return;

	SendMessage(m_hwnd, BFFM_SETSELECTION, TRUE, (long)(strPath));
}

void CBrowseForFolder::SetStatusText(const char* strText) const
{
	if (m_hwnd == NULL)
		return;

	SendMessage(m_hwnd, BFFM_SETSTATUSTEXT, NULL, reinterpret_cast<long>(strText));
}

int __stdcall CBrowseForFolder::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CBrowseForFolder* pbff = reinterpret_cast<CBrowseForFolder*>(lpData);
	pbff->m_hwnd = hwnd;
	if (uMsg == BFFM_INITIALIZED)
		pbff->OnInit();
	else if (uMsg == BFFM_SELCHANGED)
		pbff->OnSelChanged((struct _ITEMIDLIST *const)lParam);

	return 0;
}
