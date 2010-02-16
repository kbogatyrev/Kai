// CvsAddOn.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"

void ErrorMsg (DWORD dw_error)
{
    std::wstring str_out (L"Unable to determine encoding.");
    str_out += L":\n";
    str_out += L"TortoiseCVS may not be able to store the file correctly";
    str_out += L":\n";
    str_out += L"if it uses 16-bit Unicode";
    str_out += L":\n";
    std::basic_stringstream<WCHAR> io_;
    io_ << dw_error;
    str_out += io_.str();
    MessageBox (NULL, str_out.data(), L"Tortoise CVS AddOn", MB_ICONERROR | MB_OK);
}

int __stdcall UtfCheck (const char *filename)
{
MessageBox (NULL, L"GOGO", L"Tortoise CVS AddOn", MB_ICONINFORMATION | MB_OK);

/*
#   if (file_is_text(filename))
#     return 1;
#   if (file_is_unicode(filename))
#     return 2;
#   if (file_is_binary(filename))
#     return 3;
#   if (file_is_unknown(filename)) 
#     return 0; // Tortoise uses its built-in detection algorithm
# 
*/
    HANDLE h_file = CreateFileA (filename, 
                                 GENERIC_READ, 
                                 FILE_SHARE_READ, 
                                 NULL, 
                                 OPEN_EXISTING, 
                                 FILE_ATTRIBUTE_NORMAL, 
                                 NULL);    
    if (INVALID_HANDLE_VALUE == h_file)
    {
        ErrorMsg (GetLastError());
        return 0;
    }

    BYTE byte_buffer[2] = { 0 };
    DWORD w_bytesRead = 0;
    BOOL ui_ret = ReadFile (h_file, byte_buffer, 2, &w_bytesRead, NULL);
    if (!ui_ret)
    {
        ErrorMsg (GetLastError());
        return 0;
    }

    if (('\xFF' == byte_buffer[0]) && ('\xFE' == byte_buffer[1]))
    {
        MessageBox (NULL, L"UNICODE", L"Tortoise CVS AddOn", MB_ICONINFORMATION | MB_OK);
        return 2;
    }

    return 0;
}