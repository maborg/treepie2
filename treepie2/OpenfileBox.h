///http://www.codeguru.com/Cpp/COM-Tech/shell/print.php/c1325/

#include <memory>

#include <shlobj.h>

#ifndef __SHELLBROWSER_H__
#define __SHELLBROWSER_H__
class CBrowseForFolder
{
public:

	CBrowseForFolder(const HWND hParent, const LPITEMIDLIST pidl, const WCHAR* strTitle);
//	virtual ~CBrowseForFolder() = 0;
	void SetOwner(const HWND hwndOwner);
	void SetRoot(const LPITEMIDLIST pidl);
	const WCHAR* GetTitle() const;
	void SetTitle(const WCHAR* strTitle);
	
	UINT GetFlags() const;
	void SetFlags(const UINT ulFlags);
	WCHAR* GetSelectedFolder() const;
	
	int GetImage() const;
	bool SelectFolder();
	void SetSelection(const WCHAR* strPath) const;
protected:
	virtual void OnInit() const;
	virtual void OnSelChanged(const LPITEMIDLIST pidl) const;
	void EnableOK(const bool bEnable) const;
	void SetSelection(const LPITEMIDLIST pidl) const;

	void SetStatusText(const char* strText) const;

private:
	static int __stdcall BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

	typedef std::auto_ptr<WCHAR> AUTO_STR;
	AUTO_STR m_pchTitle;

	BROWSEINFO m_bi;
	WCHAR m_szSelected[MAX_PATH];
	WCHAR* m_strPath;
	HWND m_hwnd;
};

inline UINT CBrowseForFolder::GetFlags() const
{
	return m_bi.ulFlags;
}

inline int CBrowseForFolder::GetImage() const
{
	return m_bi.iImage;
}

#endif 
