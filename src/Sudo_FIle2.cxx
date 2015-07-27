// Sudo_File2.cxx

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifdef _MSC_VER
#pragma warning ( disable : 4996 4995 )
#endif

#include "Sudoku.hxx"

static const char *module = "Sudo_FIle2";

void out_Sudo_FIle2()
{
    SPRTF("%s: Another windows file interface\n", module );
}

// WOW, NOT SURE I LIKE ALL, OR ANY OF THIS!!!
// *******************************************
// #define TRY_FILE2_INTERFACE

#ifdef TRY_FILE2_INTERFACE
#include <Windows.h>
#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Propsys")
#pragma comment(lib, "shlwapi")

// ==========================================================================
// GET A DIRECTORY
/* -----------------------------
HRESULT SHBindToParent(
  _In_   PCIDLIST_ABSOLUTE pidl,
  _In_   REFIID riid,
  _Out_  VOID **ppv,
  _Out_  PCUITEMID_CHILD *ppidlLast
);
   ----------------------------- */
// Macros for interface casts
#ifdef __cplusplus
#define IID_PPV_ARG(IType, ppType) IID_##IType, reinterpret_cast(static_cast(ppType))
#else
#define IID_PPV_ARG(IType, ppType) &IID_##IType, (void**)(ppType)
#endif

// Retrieves the UIObject interface for the specified full PIDL
STDAPI SHGetUIObjectFromFullPIDL(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv)
{
    LPCITEMIDLIST pidlChild;
    IShellFolder* psf = 0;
    REFIID riid2 = IID_IShellFolder;

    *ppv = NULL;

    //HRESULT hr = SHBindToParent(pidl, IID_PPV_ARG(IShellFolder, &psf), &pidlChild);
    HRESULT hr = SHBindToParent((PCIDLIST_ABSOLUTE)pidl, riid2, (void **)psf, (PCUITEMID_CHILD *)&pidlChild);
    if (SUCCEEDED(hr))
    {
        hr = psf->GetUIObjectOf(hwnd, 1, (PCUITEMID_CHILD_ARRAY) &pidlChild, riid, NULL, ppv);
        psf->Release();
    }
    return hr;
}

// #define ILSkip(pidl, cb)        ((PUIDLIST_RELATIVE)VOID_OFFSET((pidl), (cb)))
//#define ILSkip(pidl, cb)       ((LPITEMIDLIST)(((BYTE*)(pidl))+cb))
//#define ILNext(pidl)           ILSkip(pidl, (pidl)->mkid.cb)
// #define VOID_OFFSET(pv, cb)     ((void*)(((BYTE*)(pv))+(cb)))
 
HRESULT SHILClone(LPCITEMIDLIST pidl, LPITEMIDLIST *ppidl)
{
    DWORD cbTotal = 0;

    if (pidl)
    {
        LPCITEMIDLIST pidl_temp = pidl;
        cbTotal += sizeof (pidl_temp->mkid.cb);

        while (pidl_temp->mkid.cb) 
        {
            cbTotal += pidl_temp->mkid.cb;
            //pidl_temp += ILNext (pidl_temp);
            //pidl_temp += ILSkip( pidl_temp, (pidl_temp)->mkid.cb );
            //pidl_temp += (PUIDLIST_RELATIVE)VOID_OFFSET((pidl_temp), ((pidl_temp)->mkid.cb));
            //pidl_temp += VOID_OFFSET(pidl_temp, pidl_temp->mkid.cb);
            //pidl_temp += (PUIDLIST_RELATIVE)((void*)(((BYTE*)(pidl_temp))+(pidl_temp->mkid.cb)));
            void *vp = ((void*)(((BYTE*)(pidl_temp))+(pidl_temp->mkid.cb)));
            //pidl_temp += ((void*)(((BYTE*)(pidl_temp))+(pidl_temp->mkid.cb)));
            pidl_temp = (LPCITEMIDLIST)vp;

        }
    }
    
    *ppidl = (LPITEMIDLIST)CoTaskMemAlloc(cbTotal);
    
    if (*ppidl)
        CopyMemory(*ppidl, pidl, cbTotal);
 
    return  *ppidl ? S_OK: E_OUTOFMEMORY;
}
 
// Get the target PIDL for a folder PIDL. This also deals with cases of a folder  
// shortcut or an alias to a real folder.
STDAPI SHGetTargetFolderIDList(LPCITEMIDLIST pidlFolder, LPITEMIDLIST *ppidl)
{
    IShellLink *psl;
    REFIID riid = IID_IShellFolder;
	
    *ppidl = NULL;
    
    //HRESULT hr = SHGetUIObjectFromFullPIDL(pidlFolder, NULL, IID_PPV_ARG(IShellLink, &psl));
    HRESULT hr = SHGetUIObjectFromFullPIDL(pidlFolder, NULL, riid, (void **)&psl );
    
    if (SUCCEEDED(hr))
    {
        hr = psl->GetIDList((PIDLIST_ABSOLUTE *)ppidl);
        psl->Release();
    }
    
    // It's not a folder shortcut so get the PIDL normally.
    if (FAILED(hr))
        hr = SHILClone(pidlFolder, ppidl);
    
    return hr;
}

// Get the target folder for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, the My Documents folder, and 
// other items of that nature.
STDAPI SHGetTargetFolderPath2(LPCITEMIDLIST pidlFolder, LPWSTR pszPath, UINT cchPath)
{
    LPITEMIDLIST pidlTarget;
	
    *pszPath = 0;

    HRESULT hr = SHGetTargetFolderIDList(pidlFolder, &pidlTarget);
    
    if (SUCCEEDED(hr))
    {
        SHGetPathFromIDListW((PCIDLIST_ABSOLUTE)pidlTarget, pszPath);   // Make sure it is a path
        CoTaskMemFree(pidlTarget);
    }
    
    return *pszPath ? S_OK : E_FAIL;
}



// Retrieves the UIObject interface for the specified full PIDLstatic 
HRESULT SHGetUIObjectFromFullPIDL2(LPCITEMIDLIST pidl, HWND hwnd, REFIID riid, void **ppv)
{    
    LPCITEMIDLIST pidlChild;    
    IShellFolder* psf;    
    *ppv = NULL;    
    
    HRESULT hr = SHBindToParent((PCIDLIST_ABSOLUTE)pidl, IID_IShellFolder, (LPVOID*)&psf,
        (PCUITEMID_CHILD *)&pidlChild);    
    if (SUCCEEDED(hr))    
    {        
        hr = psf->GetUIObjectOf(hwnd, 1, (PCUITEMID_CHILD_ARRAY)&pidlChild, riid, NULL, ppv);        
        psf->Release();    
    }    
    return hr;
}

// Get the target folder for a folder PIDL. This deals with cases where a folder
// is an alias to a real folder, folder shortcuts, the My Documents folder, 
// and so on.
STDAPI SHGetTargetFolderPath(LPCITEMIDLIST pidlFolder, LPWSTR pszPath, UINT cchPath)
{    
    LPITEMIDLIST pidlTarget;    
    *pszPath = 0;    
    
    HRESULT hr = SHGetTargetFolderIDList(pidlFolder, &pidlTarget);    
    if (SUCCEEDED(hr))    
    {        
        SHGetPathFromIDListW((PCIDLIST_ABSOLUTE)pidlTarget, pszPath);   
        
        // Make sure it is a path        
        CoTaskMemFree(pidlTarget);    
    }    
    
    return *pszPath ? S_OK : E_FAIL;
}


HRESULT Set_Options( IFileDialog *pfd, DWORD flag )
{
    // Set the options on the dialog.
    DWORD dwFlags;
    // Before setting, always get the options first in order 
    // not to override existing options.
    HRESULT hr = pfd->GetOptions(&dwFlags);
    if (SUCCEEDED(hr))
    {
        // In this case, get shell items only for file system items.
        hr = pfd->SetOptions(dwFlags | flag ); // like say FOS_FORCEFILESYSTEM);
    }
    return hr;
}

const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
    {L"Word Document (*.doc)",       L"*.doc"},
    {L"Web Page (*.htm; *.html)",    L"*.htm;*.html"},
    {L"Text Document (*.txt)",       L"*.txt"},
    {L"All Documents (*.*)",         L"*.*"}
};

// Indices of file types
#define INDEX_WORDDOC 1
#define INDEX_WEBPAGE 2
#define INDEX_TEXTDOC 3

// Controls
#define CONTROL_GROUP           2000
#define CONTROL_RADIOBUTTONLIST 2
#define CONTROL_RADIOBUTTON1    1
#define CONTROL_RADIOBUTTON2    2       // It is OK for this to have the same ID as CONTROL_RADIOBUTTONLIST,
                                        // because it is a child control under CONTROL_RADIOBUTTONLIST

// IDs for the Task Dialog Buttons
#define IDC_BASICFILEOPEN                       100
#define IDC_ADDITEMSTOCUSTOMPLACES              101
#define IDC_ADDCUSTOMCONTROLS                   102
#define IDC_SETDEFAULTVALUESFORPROPERTIES       103
#define IDC_WRITEPROPERTIESUSINGHANDLERS        104
#define IDC_WRITEPROPERTIESWITHOUTUSINGHANDLERS 105

/* File Dialog Event Handler *****************************************************************************************************/

class CDialogEventHandler : public IFileDialogEvents,
                            public IFileDialogControlEvents
{
public:
    // IUnknown methods
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] = {
            QITABENT(CDialogEventHandler, IFileDialogEvents),
            QITABENT(CDialogEventHandler, IFileDialogControlEvents),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&_cRef);
        if (!cRef)
            delete this;
        return cRef;
    }

    // IFileDialogEvents methods
    IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) { return S_OK; };
    IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnSelectionChange(IFileDialog *) { return S_OK; };
    IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; };
    IFACEMETHODIMP OnTypeChange(IFileDialog *pfd);
    IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; };

    // IFileDialogControlEvents methods
    IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem);
    IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD) { return S_OK; };
    IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL) { return S_OK; };
    IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD) { return S_OK; };

    CDialogEventHandler() : _cRef(1) { };
private:
    ~CDialogEventHandler() { };
    long _cRef;
};

// IFileDialogEvents methods
// This method gets called when the file-type is changed (combo-box selection changes).
// For sample sake, let's react to this event by changing the properties show.
HRESULT CDialogEventHandler::OnTypeChange(IFileDialog *pfd)
{
    IFileSaveDialog *pfsd;
    HRESULT hr = pfd->QueryInterface(&pfsd);
    if (SUCCEEDED(hr))
    {
        UINT uIndex;
        hr = pfsd->GetFileTypeIndex(&uIndex);   // index of current file-type
        if (SUCCEEDED(hr))
        {
            IPropertyDescriptionList *pdl = NULL;

            switch (uIndex)
            {
            case INDEX_WORDDOC:
                // When .doc is selected, let's ask for some arbitrary property, say Title.
                hr = PSGetPropertyDescriptionListFromString(L"prop:System.Title", IID_PPV_ARGS(&pdl));
                if (SUCCEEDED(hr))
                {
                    // FALSE as second param == do not show default properties.
                    hr = pfsd->SetCollectedProperties(pdl, FALSE);
                    pdl->Release();
                }
                break;

            case INDEX_WEBPAGE:
                // When .html is selected, let's ask for some other arbitrary property, say Keywords.
                hr = PSGetPropertyDescriptionListFromString(L"prop:System.Keywords", IID_PPV_ARGS(&pdl));
                if (SUCCEEDED(hr))
                {
                    // FALSE as second param == do not show default properties.
                    hr = pfsd->SetCollectedProperties(pdl, FALSE);
                    pdl->Release();
                }
                break;

            case INDEX_TEXTDOC:
                // When .txt is selected, let's ask for some other arbitrary property, say Author.
                hr = PSGetPropertyDescriptionListFromString(L"prop:System.Author", IID_PPV_ARGS(&pdl));
                if (SUCCEEDED(hr))
                {
                    // TRUE as second param == show default properties as well, but show Author property first in list.
                    hr = pfsd->SetCollectedProperties(pdl, TRUE);
                    pdl->Release();
                }
                break;
            }
        }
        pfsd->Release();
    }
    return hr;
}

// IFileDialogControlEvents
// This method gets called when an dialog control item selection happens (radio-button selection. etc).
// For sample sake, let's react to this event by changing the dialog title.
HRESULT CDialogEventHandler::OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem)
{
    IFileDialog *pfd = NULL;
    HRESULT hr = pfdc->QueryInterface(&pfd);
    if (SUCCEEDED(hr))
    {
        if (dwIDCtl == CONTROL_RADIOBUTTONLIST)
        {
            switch (dwIDItem)
            {
            case CONTROL_RADIOBUTTON1:
                hr = pfd->SetTitle(L"Longhorn Dialog");
                break;

            case CONTROL_RADIOBUTTON2:
                hr = pfd->SetTitle(L"Vista Dialog");
                break;
            }
        }
        pfd->Release();
    }
    return hr;
}


// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
{
    *ppv = NULL;
    CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
    HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pDialogEventHandler->QueryInterface(riid, ppv);
        pDialogEventHandler->Release();
    }
    return hr;
}


HRESULT BasicFileOpen()
{
    // CoCreate the File Open Dialog object.
    IFileDialog *pfd = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, 
                      NULL, 
                      CLSCTX_INPROC_SERVER, 
                      IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        // Create an event handling object, and hook it up to the dialog.
        IFileDialogEvents *pfde = NULL;
        hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
        if (SUCCEEDED(hr))
        {
            // Hook up the event handler.
            DWORD dwCookie;
            hr = pfd->Advise(pfde, &dwCookie);
            if (SUCCEEDED(hr))
            {
                // Set the options on the dialog.
                DWORD dwFlags;

                // Before setting, always get the options first in order 
                // not to override existing options.
                hr = pfd->GetOptions(&dwFlags);
                if (SUCCEEDED(hr))
                {
                    // In this case, get shell items only for file system items.
                    hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
                    if (SUCCEEDED(hr))
                    {
                        // Set the file types to display only. 
                        // Notice that this is a 1-based array.
                        hr = pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
                        if (SUCCEEDED(hr))
                        {
                            // Set the selected file type index to Word Docs for this example.
                            hr = pfd->SetFileTypeIndex(INDEX_WORDDOC);
                            if (SUCCEEDED(hr))
                            {
                                // Set the default extension to be ".doc" file.
                                hr = pfd->SetDefaultExtension(L"doc;docx");
                                if (SUCCEEDED(hr))
                                {
                                    // Show the dialog
                                    hr = pfd->Show(NULL);
                                    if (SUCCEEDED(hr))
                                    {
                                        // Obtain the result once the user clicks 
                                        // the 'Open' button.
                                        // The result is an IShellItem object.
                                        IShellItem *psiResult;
                                        hr = pfd->GetResult(&psiResult);
                                        if (SUCCEEDED(hr))
                                        {
                                            // We are just going to print out the 
                                            // name of the file for sample sake.
                                            PWSTR pszFilePath = NULL;
                                            hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, 
                                                               &pszFilePath);
                                            if (SUCCEEDED(hr))
                                            {
                                                TaskDialog(NULL,
                                                           NULL,
                                                           L"CommonFileDialogApp",
                                                           pszFilePath,
                                                           NULL,
                                                           TDCBF_OK_BUTTON,
                                                           TD_INFORMATION_ICON,
                                                           NULL);
                                                CoTaskMemFree(pszFilePath);
                                            }
                                            psiResult->Release();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                // Unhook the event handler.
                pfd->Unadvise(dwCookie);
            }
            pfde->Release();
        }
        pfd->Release();
    }
    return hr;
}

// *********************************************
#endif // #ifdef TRY_FILE2_INTERFACE

// eof - Sudo_File2.cxx
