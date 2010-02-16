// SetupDll.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "msiquery.h"
#include "shlobj.h"
#include "shlwapi.h"
#include <string>
#include <iostream>
#include <sstream>
#include <strstream>

std::wstring str_Cause (unsigned int ui_errorCode);

UINT __stdcall CreateFolder (MSIHANDLE hInstall)
{
    WCHAR sz_path[MAX_PATH + 1] = {0};
    DWORD dw_len = MAX_PATH;
    HRESULT h_r = SHGetFolderPath (NULL, 
                                   CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, 
                                   NULL, 
                                   0, 
                                   sz_path);
    if (S_OK != h_r)
    {
        std::wstring str_msg (L"Unable to obtain application data folder path. Aborting setup.");
        MessageBox (NULL, 
                    str_msg.data(),
                    L"Kai Installation Error", 
                    MB_ICONERROR);
        return ERROR_DIRECTORY;
    }

    WCHAR sz_folderName[MAX_PATH + 1] = {0};
    UINT ui_ret = MsiGetProperty (hInstall, L"CustomActionData", sz_folderName, &dw_len);
    if (ui_ret != ERROR_SUCCESS)
    {
        std::wstring str_msg (L"Unable to access MSI custom action property. Aborting setup.");
        MessageBox (NULL, str_msg.data(), L"Kai Installation Error", MB_ICONERROR);
        return ui_ret;
    }

    ui_ret = PathAppend (sz_path, sz_folderName);
    if (!ui_ret)
    {
        std::wstring str_msg (L"Unable to create target path. Aborting setup.");
        MessageBox (NULL, str_msg.data(), L"Kai Installation Error", MB_ICONERROR);
        return ui_ret;
    }

    ui_ret = CreateDirectory (sz_path, NULL);
    if (!ui_ret)
    {
        UINT err = GetLastError();
        std::basic_stringstream<WCHAR> io_;
        io_ << err;
        if (GetLastError() != ERROR_ALREADY_EXISTS)
        {            
            std::wstring str_msg (L"Unable to create application data folder. Aborting setup.");
            str_msg += L"\n" + std::wstring (io_.str());
            MessageBox (NULL, 
                         str_msg.data(),
                         L"Kai Installation Error", 
                         MB_ICONERROR);
            return ui_ret;
        }
    }

    return 0;

}   //  CreateFolder()

/* Currently not needed
std::wstring str_Cause (unsigned int ui_errorCode)
{
    std::wstring str_cause;
    switch (ui_errorCode)
    {
        case ERROR_DIRECTORY:
        {
            str_cause = L"ERROR_DIRECTORY";
            break;
        }
        case ERROR_INVALID_HANDLE:
        {
            str_cause = L"ERROR_INVALID_HANDLE";
            break;
        }
        case ERROR_INVALID_PARAMETER:
        {
            str_cause = L"ERROR_INVALID_PARAMETER";
            break;
        }
        case ERROR_MORE_DATA:
        { 
            str_cause = L"ERROR_MORE_DATA";
            break;
        }
        case ERROR_SUCCESS:
        {
            str_cause = L"ERROR_SUCCESS";
            break;
        }
        default:
        {
            str_cause = L"unknown error";
        }
    }

    return str_cause;
}
*/
