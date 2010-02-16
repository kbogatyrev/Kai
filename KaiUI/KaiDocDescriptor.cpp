//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: Kai-specific document properties.
//
//    $Id: KaiDocDescriptor.cpp,v 1.84 2009-01-18 02:31:04 kostya Exp $
//
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiTextPos.h"
#include "KaiUndoStack.h"
#include "KaiMainFrm.h"
#include "KaiDocDescriptor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Security::Cryptography;
using namespace System::Runtime::InteropServices;

#pragma unmanaged
CT_DocDescriptor::CT_DocDescriptor()
{
    pco_UndoStack_ = new CT_UndoStack;  // consider storing/loading it
    b_Imported_ = false;
    b_Unrenderable_ = false;
    i_PageWidth_ = 0;
    eo_Encoding = ec_Unknown;
}

CT_DocDescriptor::~CT_DocDescriptor()
{
    v_Null_();
}

void CT_DocDescriptor::v_Null_()
{
    map<unsigned int, CT_FontDescriptor *>::iterator it_fd;
    for (it_fd = map_Fonts_.begin(); it_fd != map_Fonts_.end(); ++it_fd)
    {
        CT_FontDescriptor * pco_fd = it_fd->second;;
        delete pco_fd;
    }
    map_Fonts_.clear();

    for (unsigned int ui_at = 0; ui_at < vpo_Paragraphs_.size(); ++ui_at)
    {
        CT_Paragraph * pco_p = vpo_Paragraphs_[ui_at];
        delete pco_p;
    }
    vpo_Paragraphs_.clear();

    if (pco_UndoStack_)
    {
        delete pco_UndoStack_;
        pco_UndoStack_ = NULL;
    }

    str_Title_.clear();

    b_Unrenderable_ = false;

}    //  v_Null()

bool CT_DocDescriptor::b_Load (CFile& co_file)
{
//    v_Null_();

    b_Unrenderable_ = false;

    ((CMainFrame *)AfxGetApp()->GetMainWnd())->v_SetIdleMessage (_T("Loading document..."));

    bool b_ = b_LoadFontMap (co_file);
    if (!b_)
    {
        return false;
    }

    V_LoadItem (co_file, i_PageWidth_);

    int i_paragraphs = 0;
    V_LoadItem (co_file, i_paragraphs);
    if (i_paragraphs <= 0)
    {
        ERROR_TRACE (_T("Num. of paragraphs zero or negative."));
        return false;
    }

    int i_progress = 0;
    vpo_Paragraphs_.reserve (i_paragraphs);
    for (int i_p = 0; i_p < i_paragraphs; ++i_p)
    {
        CT_Paragraph * pco_p = new CT_Paragraph (this);
        bool b_ = pco_p->b_Load (co_file);
        if (!b_)
        {
            STL_STRING str_msg (_T("Error loading paragraph "));
            str_msg += STR_IToStr (i_p);
            str_msg += _T (".");
            ERROR_TRACE (str_msg);
            delete pco_p;
            return false;
        }
        if (i_p > 0)
        {
            CT_Paragraph * pco_prev = vpo_Paragraphs_[i_p - 1];
            pco_prev->v_SetNext (pco_p);
            pco_p->v_SetPrevious (pco_prev);
        }

        vpo_Paragraphs_.push_back (pco_p);

        int i_done = (int)(((double)vpo_Paragraphs_.size()/(double)i_paragraphs) * 100);
        if (i_done > i_progress)
        {
            i_progress = i_done;
            AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, i_progress);
        }
    }

    return true;

}   //  b_Load (...)

#pragma managed
bool CT_DocDescriptor::b_Store (CFile& co_file)
{
#pragma push_macro ("new")
#undef new

    CString str_headerText;
    str_headerText.LoadString (IDR_WATERMARK);
    unsigned int ui_length = str_headerText.GetLength();
    LPTSTR pchr_headerText = str_headerText.GetBuffer();
    array<Byte> ^ barr_source = gcnew array<Byte>(ui_length * sizeof (TCHAR));
    for (unsigned int ui_byte = 0; ui_byte < ui_length * sizeof (TCHAR); ++ui_byte)
    {
        barr_source[ui_byte] = ((BYTE *)pchr_headerText)[ui_byte];
    }
    
    MD5CryptoServiceProvider ^ co_md5 = gcnew MD5CryptoServiceProvider();
    array<Byte> ^ barr_hash = co_md5->ComputeHash (barr_source);
    char puchr_hash[16];
    for (unsigned int ui_byte = 0; ui_byte < 16; ++ui_byte)
    {
        puchr_hash[ui_byte] = barr_hash[ui_byte];
    }

    co_file.Write (puchr_hash, 16);

    unsigned int ui_versionMajor1 = 0;
    unsigned int ui_versionMinor1 = 0;
    unsigned int ui_ignore1 = 0;
    unsigned int ui_ignore2 = 0;

    ((CKaiApp *)AfxGetApp())->v_GetVersion (ui_versionMajor1, 
                                            ui_versionMinor1,
                                            ui_ignore1,
                                            ui_ignore2);
    co_file.Write (&ui_versionMajor1, sizeof (unsigned int));

    bool b_ = b_StoreFontMap (co_file);
    if (!b_)
    {
        return false;
    }

    V_StoreItem (co_file, i_PageWidth_);

    int i_size = vpo_Paragraphs_.size();
    V_StoreItem (co_file, i_size);
    for (unsigned int ui_p = 0; ui_p < vpo_Paragraphs_.size(); ++ui_p)
    {
        CT_Paragraph * pco_p = vpo_Paragraphs_[ui_p];
        bool b_ = pco_p->b_Store (co_file);
        if (!b_)
        {
            return false;
        }
    }

    return true;

#pragma pop_macro ("new")

}   //  b_Store()

#pragma unmanaged

bool CT_DocDescriptor::b_StoreAsPlainText (CFile& co_file)
{
    co_file.Write ("\xFF\xFE", 2);  // BOM

    int i_size = vpo_Paragraphs_.size();
    for (unsigned int ui_p = 0; ui_p < vpo_Paragraphs_.size(); ++ui_p)
    {
        CT_Paragraph * pco_p = vpo_Paragraphs_[ui_p];
        bool b_ = pco_p->b_StoreAsPlainText (co_file);
        if (!b_)
        {
            return false;
        }
    }

    co_file.Flush();

    return true;

}   //  b_StoreAsPlainText()

bool CT_DocDescriptor::b_GetDefaultFont (STL_STRING& str_typeface, 
                                         int& i_size, 
                                         ET_Charset& eo_charset)
{
    str_typeface = _T("");
    STL_STRING str_charset = _T(" ");
    i_size = 0;
    eo_charset = ec_CharsetFront;

    try
    {
        bool b_typeface = B_GetConfigValue (STL_STRING(_T("Default Font.Typeface")),
                                            str_typeface);
        bool b_size = B_GetConfigValue (STL_STRING (_T("Default Font.Size")), i_size);
        bool b_charset = B_GetConfigValue (STL_STRING (_T("Default Font.Charset")), str_charset);
        if (!(b_typeface && b_size && b_charset))
        {
            ERROR_TRACE (_T("unable to obtain config value(s)"));
            return false;
        }
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }

    CT_KaiString kstr_charset (str_charset);
    kstr_charset.v_ToUpper();
    map<CT_KaiString, ET_Charset> mo_names;
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("ANSI"), ec_ANSICharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("BALTIC"), ec_BalticCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("CHINESEBIG5"), 
                                                   ec_ChineseBig5Charset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("DEFAULT"), 
                                                   ec_DefaultCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("EASTEUROPE"), 
                                                   ec_EastEuropeCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("GB2312"), ec_GB2312Charset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("GREEK"), ec_GreekCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("HANGUL"), ec_HangulCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("MAC"), ec_MacCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("OEM"), ec_OEMCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("RUSSIAN"), ec_RussianCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("SHIFTJIS"), ec_ShiftjisCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("SYMBOL"), ec_SymbolCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("TURKISH"), ec_TurkishCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("JOHAB"), ec_JohabCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("HEBREW"), ec_HebrewCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("ARABIC"), ec_ArabicCharset));
    mo_names.insert (pair<STL_STRING, ET_Charset> (_T("THAI"), ec_ThaiCharset));
    
    map<CT_KaiString, ET_Charset>::iterator it_charset = mo_names.find (kstr_charset);
    if (it_charset == mo_names.end())
    {            
//        B_WriteTraceMsg (ec_InfoTraceMsg, 
//                         STR_CurrentLocation(),
//                         "unable to obtain charset value");
        return false;
    }
    eo_charset = it_charset->second;

    return true;

}    //  b_GetDefaultFont

bool CT_DocDescriptor::b_GetDefaultParStyle (int& i_lineWidth,
                                             int& i_leftIndent,
                                             int& i_rightIndent,
                                             ET_Alignment& eo_alignment)
{
    i_lineWidth = 0;
    eo_alignment = ec_AlignmentFront;

    try
    {
        bool b_style = B_GetConfigValue (STL_STRING (_T("Default Paragraph Style.Line Width")), 
                                         i_lineWidth);
        b_style = B_GetConfigValue (STL_STRING (_T("Default Paragraph Style.Left Indent")), 
                                    i_leftIndent);
        b_style = B_GetConfigValue (STL_STRING (_T("Default Paragraph Style.Right Indent")), 
                                    i_rightIndent);
        unsigned int ui_alignment = 0;
        bool b_alignment = B_GetConfigValue (STL_STRING (_T("Default Paragraph Style.Alignment")), 
                                             ui_alignment);
        if (b_alignment)
        {
            if (ec_AlignmentFront >= static_cast <ET_Alignment>(ui_alignment))
            {
                b_alignment = false;
            }
            if (ec_AlignmentBack <= static_cast <ET_Alignment>(ui_alignment))
            {
                b_alignment = false;
            }
        }
        if (b_alignment)
        {
            eo_alignment = static_cast <ET_Alignment> (ui_alignment);
        }
        if (!(b_style && b_alignment))
        {
            return false;
        }
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }

    return true;

}    //  b_GetDefaultParStyle

unsigned int CT_DocDescriptor::ui_GetDocLength()
{
    unsigned int i_size = 0;
    for (unsigned int ui_at = 0; ui_at < vpo_Paragraphs_.size(); ++ui_at)
    {
        i_size += (vpo_Paragraphs_[ui_at])->length();
    }
    return i_size;

}   // ui_GetDocLength()

std::vector<CT_Paragraph *>& CT_DocDescriptor::vpo_GetParagraphs() 
{ 
    return vpo_Paragraphs_; 
}

CT_FontDescriptor * CT_DocDescriptor::pco_GetFontDescriptor (unsigned int ui_key)
{
    map<unsigned int, CT_FontDescriptor *>::iterator it_font = map_Fonts_.find (ui_key);
    if (it_font == map_Fonts_.end())
    {
        ERROR_TRACE (_T("Font descriptor not found"));
        throw CT_KaiException (CT_KaiException::ec_FontIndexOutOfBounds,
                               _T("KaiDocFont.cpp"),
                               _T("CT_DocDescriptor"),
                               _T("co_GetFontDescriptor"),
                               _T("Font descriptor not found"));
    }

    return it_font->second;

}    //  co_GetFontDescriptor (int i_font)

CT_FontDescriptor * CT_DocDescriptor::pco_GetDefaultFontDescriptor()
{
    if (map_Fonts_.empty())
    {
        ERROR_TRACE (_T("Font descriptor not found"));
        throw CT_KaiException (CT_KaiException::ec_FontIndexOutOfBounds,
                               _T("KaiDocFont.cpp"),
                               _T("CT_DocDescriptor"),
                               _T("co_GetFontDescriptor"),
                               _T("Font descriptor not found"));
    }

    map<unsigned int, CT_FontDescriptor *>::iterator it_f = map_Fonts_.begin();
    return it_f->second;

}

void CT_DocDescriptor::v_RemoveBreaks (STL_STRING& str_)
{
    unsigned int ui_pos = str_.find (_T("\r"));
    if (ui_pos != STL_STRING::npos)
    {
        str_.erase (ui_pos, 1);
    }
    ui_pos = str_.find (_T("\n"));
    if (ui_pos != STL_STRING::npos)
    {
        str_.erase (ui_pos, 1);
    }

    //
    // Replace line feed with paragraph-terminating char
    //
    _TCHAR chrarr_endMark[2];
    chrarr_endMark[0] = _T('\xB6');
    chrarr_endMark[1] = _T('\0');
    str_ += STL_STRING (chrarr_endMark);
}

void CT_DocDescriptor::v_DeleteText (CT_TextPos& co_first, 
                                     CT_TextPos& co_last,
                                     bool b_caretAtStart,
                                     bool b_selection)
{
    pco_UndoStack_->v_SetEventType (ec_UndoDeleteText);
    pco_UndoStack_->v_SetSelection (b_selection);
    pco_UndoStack_->v_SetParagraph (co_first.i_GetParagraph());
    pco_UndoStack_->v_SetOffset (co_first.i_GetParagraphOffset());
    pco_UndoStack_->v_SetStartVisibleOffset (co_first.i_GetVisibleParagraphOffset());
    pco_UndoStack_->v_SetEndParagraph (co_last.i_GetParagraph());
    pco_UndoStack_->v_SetEndVisibleOffset (co_last.i_GetVisibleParagraphOffset());
    pco_UndoStack_->v_SetCaretAtStart (b_caretAtStart);

    int i_p = co_first.i_GetParagraph();
    for (; i_p <= co_last.i_GetParagraph(); ++i_p)
    {
        pco_UndoStack_->v_PushParagraph (pco_GetParagraph (i_p));
    }

    pco_UndoStack_->v_AddEvent();

    STL_STRING str_deleted;
    if (co_first.pco_GetParagraph() == co_last.pco_GetParagraph())
    {
        CT_Paragraph * pco_p = co_first.pco_GetParagraph();
        bool b_ = pco_p->b_Delete (co_first, co_last, str_deleted);
        return;
    }

    //
    //    2 or more $$ affected
    //
    CT_Paragraph * pco_firstPar = co_first.pco_GetParagraph();
    CT_Paragraph * pco_prevPar = pco_firstPar->pco_GetPrevious(); 
    CT_Paragraph * pco_lastPar = co_last.pco_GetParagraph();
    CT_Paragraph * pco_nextPar = pco_lastPar->pco_GetNext();

    int i_firstPar = pco_firstPar->i_At();
    int i_lastPar = pco_lastPar->i_At();

    //
    // 1st paragraph
    //
    if ((co_first.i_GetLineOffset() == 0) && (co_first.i_GetLine() == 0))
    {
        delete pco_firstPar;
        pco_firstPar = NULL; 
    }
    else
    {
        int i_deleteAt = co_first.i_GetParagraphOffset();
        ST_Token st_t = pco_firstPar->st_GetTokenFromOffset (i_deleteAt);
        int i_t = pco_firstPar->i_GetTokenNum (st_t);
        while ((i_t > 0) && 
           (pco_firstPar->eo_GetTokenType (i_t) == ec_TokenDiacritics))
        {
            const ST_Token& rst_t = pco_firstPar->rst_GetToken (--i_t);
            i_deleteAt = rst_t.i_Offset;
        }
        str_deleted = pco_firstPar->substr (i_deleteAt);
        pco_firstPar->assign (pco_firstPar->substr (0, i_deleteAt));

        if (co_last.i_GetVisibleParagraphOffset() == 0)
        {
            pco_UndoStack_->v_SetMerged();
        }
    }

    //
    // Delete whole $$$ between 1st and last
    //
    for (int i_p = i_firstPar + 1; i_p < i_lastPar; ++i_p)
    {
        CT_Paragraph * pco_p = vpo_Paragraphs_[i_p];
        delete pco_p;
    }

    //
    // Last paragraph
    //
    if (co_last.i_GetVisibleParagraphOffset() > 0)
    {
        CT_TextPos co_lastParStart (co_last);
        co_lastParStart.v_SetVisibleParagraphOffset (0);
        pco_lastPar->b_Delete (co_lastParStart, co_last, str_deleted);
    }


    //
    // Remove all affected paragraphs (including 1st and last)
    // from the paragraph vector
    //
    for (int i_p = 0; i_p < i_lastPar - i_firstPar + 1; ++i_p)
    {
        v_RemoveParagraph (i_firstPar);
    }

    //
    // Construct a new paragraph out of remnants of the first and last ones
    //
    CT_Paragraph * pco_newPar = NULL;
    if (pco_firstPar && pco_lastPar)
    {
        pco_newPar = pco_firstPar;

        pco_newPar->append (*pco_lastPar);
        pco_newPar->v_SetLastParagraph (pco_lastPar->b_IsLastParagraph());
        pco_newPar->v_SetNext (pco_nextPar);
        if (pco_nextPar != NULL)
        {
            pco_nextPar->v_SetPrevious (pco_newPar);
        }
        pco_newPar->v_FdCleanup();
        pco_newPar->v_Invalidate();
        vpo_Paragraphs_.insert (vpo_Paragraphs_.begin() + i_firstPar, 
                                pco_newPar);
        delete pco_lastPar;
        pco_lastPar = NULL;

        return;
    }

    if (!pco_firstPar && !pco_lastPar)
    {
        if (pco_prevPar)
        {
            pco_prevPar->v_SetNext (pco_nextPar);
        }
        if (pco_nextPar)
        {
            pco_nextPar->v_SetPrevious (pco_prevPar);
        }
        pco_nextPar->v_Invalidate();

        return;
    }

    if (!pco_firstPar) 
    {
        pco_lastPar->v_SetPrevious (pco_prevPar);
        if (pco_prevPar)
        {
            pco_prevPar->v_SetNext (pco_lastPar);
        }
        pco_lastPar->v_Invalidate();
        vpo_Paragraphs_.insert (vpo_Paragraphs_.begin() + i_firstPar, pco_lastPar);
    }

    if (!pco_lastPar)
    {
        ERROR_TRACE (_T("Empty last paragraph")); 
    }

}    //  v_DeleteText (...)

void CT_DocDescriptor::v_InsertText (CT_TextPos * pco_at, 
                                     CT_KaiString& kstr_text,
                                     bool b_replace)
{
    CT_Paragraph * pco_paragraph = pco_at->pco_GetParagraph();

    pco_UndoStack_->v_SetParagraph (pco_paragraph->i_At());
    pco_UndoStack_->v_SetStartVisibleOffset (pco_at->i_GetVisibleParagraphOffset());

    CT_Paragraph * pco_afterLast = pco_paragraph->pco_GetNext();
    CT_DocDescriptor * pco_doc = pco_paragraph->pco_GetDocument();
    STL_STRING str_left = pco_paragraph->substr (0, pco_at->i_GetParagraphOffset());
    STL_STRING str_right = pco_paragraph->substr (pco_at->i_GetParagraphOffset());
    pco_paragraph->assign (str_left);

    int i_visibleOffset = 0;

    int i_insertAt = pco_paragraph->i_At();
    STL_STRING str_eop = pco_paragraph->str_GetEndOfParagraphChars();
    unsigned int ui_parStart = 0;
    unsigned int ui_nl = kstr_text.find (str_eop, 0);
    if (ui_nl != STL_STRING::npos)
    {
        pco_paragraph->append (kstr_text.substr (0, ui_nl + 1));
        ui_parStart = ui_nl + 1;
    }
    else
    {
        pco_paragraph->append (kstr_text);
        pco_UndoStack_->v_SetLength (kstr_text.length());
    }

    CT_Paragraph * pco_previous = pco_paragraph->pco_GetPrevious();
    while (ui_nl != STL_STRING::npos)
    {
        pco_paragraph->v_FdCleanup();
        pco_paragraph->v_Invalidate();

        ui_nl = kstr_text.find (str_eop, ui_parStart);
        pco_previous = pco_paragraph;
        pco_paragraph = new CT_Paragraph (pco_doc);
        pco_paragraph->v_CopyAttributes (*pco_previous);
        unsigned int ui_font = 0;
        bool b_ = pco_previous->b_GetFontKey (pco_previous->length() - 1, ui_font);
        STL_STRING str_fd;
        b_ = pco_paragraph->b_CreateFDString (ui_font, str_fd);
        pco_paragraph->assign (str_fd);

        if (ui_nl != STL_STRING::npos)
        {
            pco_paragraph->append (kstr_text.substr (ui_parStart, ui_nl - ui_parStart + 1));
            ui_parStart = ui_nl + 1;
        }
        else
        {
            if (ui_parStart < kstr_text.length())
            {
                pco_paragraph->append (kstr_text.substr (ui_parStart));
            }
        }
        pco_paragraph->v_SetPrevious (pco_previous);
        if (pco_previous != NULL)
        {
            pco_previous->v_SetNext (pco_paragraph);
        }
        pco_paragraph->v_SetLastParagraph (pco_previous->b_IsLastParagraph());
        pco_previous->v_SetLastParagraph (false);
        pco_doc->v_AddParagraph (pco_paragraph, i_insertAt++);
    }

    i_visibleOffset += pco_paragraph->i_GetVisibleLength();

    pco_UndoStack_->v_SetEndParagraph (pco_paragraph->i_At());
    pco_UndoStack_->v_SetEndVisibleOffset (i_visibleOffset);
    if (b_replace)
    {
        pco_UndoStack_->v_LinkToPrevious();
    }
    pco_UndoStack_->v_AddEvent();

    pco_paragraph->append (str_right);
    if (pco_previous != NULL)
    {
        pco_previous->v_SetNext (pco_paragraph);
    }
    pco_paragraph->v_SetNext (pco_afterLast);
    if (pco_afterLast != NULL)
    {
        pco_afterLast->v_SetPrevious (pco_paragraph);
    }
    pco_paragraph->v_FdCleanup();
    pco_paragraph->v_Invalidate();

    pco_at->v_SetParagraph (pco_paragraph);
    pco_at->v_SetVisibleParagraphOffset (i_visibleOffset);

}    //  v_InsertText()

#pragma managed
bool CT_DocDescriptor::b_CheckDocFormat (CFile& co_inFile, 
                                         unsigned int& ui_version)
{
#pragma push_macro ("new")
#undef new

    ui_version = 0;

    //
    // Read first 16 bytes -- compare to watermark hash
    //
    unsigned char puchr_watermark[16];
    try
    {
        UINT ui_bytesRead = co_inFile.Read (puchr_watermark, 16);
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR_WATERMARK);
        co_inFile.Close();
        return false;
    }

    array<Byte> ^ barr_waterMark = gcnew array<Byte>(16);
    for (unsigned int ui_byte = 0; ui_byte < 16; ++ui_byte)
    {
        barr_waterMark[ui_byte] = puchr_watermark[ui_byte];
    }

    CString str_headerText;
    str_headerText.LoadString (IDR_WATERMARK);
    unsigned int ui_length = str_headerText.GetLength();
    unsigned char * pbyte_headerText = (unsigned char *)LPCTSTR(str_headerText);
    array<Byte> ^ barr_source = gcnew array<Byte>(ui_length * sizeof (TCHAR));
    for (unsigned int ui_byte = 0; ui_byte < ui_length * sizeof (TCHAR); ++ui_byte)
    {
        barr_source[ui_byte] = pbyte_headerText[ui_byte];
    }

    MD5CryptoServiceProvider ^ co_md5 = gcnew MD5CryptoServiceProvider();
    array<Byte> ^ barr_hash = co_md5->ComputeHash (barr_source);

    unsigned int ui_matched = 0;
    for (; 
         (ui_matched < 16) && (barr_hash[ui_matched] == barr_waterMark[ui_matched]); 
         ++ui_matched);

    if (ui_matched < 16)
    {
        return false;
    }

    char pchr_version[4];
    try
    {
        unsigned int ui_bytesRead = co_inFile.Read (pchr_version, sizeof (unsigned int));
        if (ui_bytesRead < sizeof (unsigned int))
        {
            return false;
        }
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, IMPORT_ERROR_SEEK);
        return false;
    }
    unsigned int * pui_docVersion = reinterpret_cast<unsigned int *>(pchr_version);
    unsigned int ui_versionMajor1 = 0;
    unsigned int ui_versionMinor1 = 0;
    unsigned int ui_versionMajor2 = 0;
    unsigned int ui_versionMinor2 = 0;
    ((CKaiApp *)AfxGetApp())->v_GetVersion (ui_versionMajor1, 
                                            ui_versionMinor1,
                                            ui_versionMajor2,
                                            ui_versionMinor2);
    if (*pui_docVersion <= ui_versionMajor1)
    {
        return true;
    }
    else
    {
        return false;
    }

#pragma pop_macro ("new")

}    //  b_CheckDocFormat
#pragma unmanaged

bool CT_DocDescriptor::b_ImportTextFile (CFile& co_inFile)
{

//    pco_UndoStack_ = new CT_UndoStack;

    b_Unrenderable_ = false;
    b_Imported_ = true;
    eo_Encoding = ec_Unknown;

    BYTE byte_buf[1000];
    UINT ui_bytesRead = 0;
    try
    {
        co_inFile.SeekToBegin();

        ui_bytesRead = co_inFile.Read (&byte_buf, 1000);
        if (0 == ui_bytesRead)
        {
            ERROR_TRACE (_T("Unable to read input file; file may be empty."));
            return false;
        }
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
        return false;
    }


    //
    // Look for Byte Order Mark (BOM)
    //
//    BYTE byte_bom[3];

//    UINT ui_bytesRead = co_inFile.Read (&byte_bom, 3);

    if (ui_bytesRead >= 2)
    {
        if ((byte_buf[0] == static_cast <BYTE> ('\xFF')) &&
            (byte_buf[1] == static_cast <BYTE> ('\xFE')))
        {
            eo_Encoding = ec_UTF16;
        }
        if ((byte_buf[0] == static_cast <BYTE> ('\xFE')) &&
            (byte_buf[1] == static_cast <BYTE> ('\xFF')))
        {
            eo_Encoding = ec_UTF16BigEndian;
        }
        if ((ui_bytesRead > 2) &&
            (byte_buf[0] == static_cast <BYTE> ('\xEF')) &&
            (byte_buf[1] == static_cast <BYTE> ('\xBB')) &&
            (byte_buf[2] == static_cast <BYTE> ('\xBF')))
        {
            eo_Encoding = ec_UTF8;
        }
    }

    if (ec_Unknown == eo_Encoding)
    {
        int i_mask = IS_TEXT_UNICODE_UNICODE_MASK;
        BOOL mfcb_ = IsTextUnicode ((VOID *)byte_buf, ui_bytesRead, &i_mask);
        if (mfcb_)
        {
            ATLASSERT (i_mask);
            eo_Encoding = ec_UTF16;
        }
        else
        {
            i_mask = IS_TEXT_UNICODE_REVERSE_MASK;
            mfcb_ = IsTextUnicode ((VOID *)byte_buf, ui_bytesRead, &i_mask);
            if (mfcb_)
            {
                ATLASSERT (i_mask);
                eo_Encoding = ec_UTF16BigEndian;
            }
            else
            {
                i_mask = IS_TEXT_UNICODE_NOT_UNICODE_MASK;
                mfcb_ = IsTextUnicode ((VOID *)byte_buf, ui_bytesRead, &i_mask);
                if (mfcb_)
                {
                    ATLASSERT (i_mask);
                    eo_Encoding = ec_ASCII;
                }
                else
                {
                    i_mask = IS_TEXT_UNICODE_NOT_ASCII_MASK;
                    mfcb_ = IsTextUnicode ((VOID *)byte_buf, ui_bytesRead, &i_mask);
                    if (mfcb_)
                    {
                        ERROR_TRACE (_T("Warning: unable to determine encoding."));
                        return false;
                    }
                }
            }
        }
    }       //  if (ec_Unknown == eo_Encoding)

    bool b_r = true;
    switch (eo_Encoding)
    {
        case ec_UTF16:
        {
            b_r = b_ReadUTF16 (co_inFile);
            break;
        }
        case ec_UTF16BigEndian:
        {
            b_r = b_ReadNonNativeUTF (co_inFile);
            break;
        }
        case ec_UTF8:
        {
            b_r = b_ReadNonNativeUTF (co_inFile);
            break;
        }
        case ec_Unknown:
        {
			if (b_FileHasForeignCharacters (co_inFile))
			{
			    CString cstr_msg1 
				    (_T("This document appears to be using an unknown ANSI encoding. "));
				CString cstr_msg2
					(_T("Would you like to run the decoding utility?"));
				int i_ret = AfxMessageBox (cstr_msg1 + _T("\n") + cstr_msg2, 
										   MB_YESNO | MB_ICONEXCLAMATION);
                if (IDYES == i_ret)
                {
                    b_r = b_ReadANSI (co_inFile, true);
                }
                else
                {
                    return false;
                }
            }
            else
            {
                b_r = b_ReadANSI (co_inFile, false);
            }
            break;
        }
        default:
        {
            return false;
            break;
        }
    };

    return b_r;

}    //  b_ImportTextFile

bool CT_DocDescriptor::b_FileHasForeignCharacters (CFile& co_inFile)
{
	BYTE byte_buf[10000];
    UINT ui_bytesRead = 0;

	try
    {
        co_inFile.SeekToBegin();
        do
        {
            ui_bytesRead = co_inFile.Read (&byte_buf, 10000);
            if (0 == ui_bytesRead)
            {
                ERROR_TRACE (_T("Unable to read input file; file may be empty."));
                return false;
            }
            
            for (UINT ui_ = 0; ui_ <= ui_bytesRead; ++ui_)
            {
                if (byte_buf[ui_] > 127)
                {
                    return true;
                }
            }

        }  while (!co_inFile.end);
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
        return false;
    }

    return false;
}

bool CT_DocDescriptor::b_ReadUTF16 (CFile& co_inFile)
{
    ULONGLONG qw_length = co_inFile.GetLength();
    if (qw_length > INT_MAX)
    {
        ERROR_TRACE (_T("File too long."));
        return false;
    }

    int i_fileLength = (int)qw_length;

    STL_STRING str_in;
    _TCHAR chrarr_buffer[MAX_PARAGRAPH_LENGTH+1];
    CT_Paragraph * pco_previous = NULL;
    CT_Paragraph * pco_next = NULL;
    LPTSTR p_result = NULL;

    ((CMainFrame *)AfxGetApp()->GetMainWnd())->v_SetIdleMessage (_T("Importing a UTF16 file..."));

    int i_defaultLeftIndent = 0;
    int i_defaultRightIndent = 0;
    int i_defaultSpacing = 0;
    ET_Alignment eo_defaultAlignment = ec_AlignmentFront;

    bool b_ = b_GetDefaultParStyle (i_PageWidth_,
                                    i_defaultLeftIndent,
                                    i_defaultRightIndent,
                                    eo_defaultAlignment);
    if (!b_)
    {
        ERROR_TRACE (_T("b_GetDefaultParStyle returned \"false\"")); 
        i_PageWidth_ = 800;
        i_defaultLeftIndent = 0;
        i_defaultRightIndent = 0;
        i_defaultSpacing = 10;
        eo_defaultAlignment = ec_Left;
    }

    //
    // Get the default font
    //
    STL_STRING str_typeface;
    int i_size = 0;
    ET_Charset eo_charset = ec_CharsetFront;
    b_ = b_GetDefaultFont (str_typeface, i_size, eo_charset);
    if (!b_)
    {
        ERROR_TRACE (_T("b_GetDefaultFont_ returned \"false\""));
        str_typeface = _T("Courier New");
        i_size = 10;
        eo_charset = ec_RussianCharset;
    }

    //
    // Create a default font descriptor
    //
    CT_FontDescriptor co_fd (CT_KaiString (str_typeface),
                             eo_charset,
                             i_size);

    STL_STRING str_fileName = co_inFile.GetFilePath();
    co_inFile.Close();
    try
    {
        //
        // Reopen as a text file
        //
        CStdioFile co_textFile;
        co_textFile.Open (str_fileName.data(), 
                          CFile::modeRead | CFile::typeBinary);
        //
        // Remove BOM
        //
        wchar_t    chr_dummy;
        co_textFile.Read (&chr_dummy, sizeof (wchar_t));

        int i_charactersRead = 0;
        
        do
        {
            p_result = co_textFile.ReadString (chrarr_buffer, 
                                               MAX_PARAGRAPH_LENGTH+1);
            if (NULL == p_result)

            {
                continue;
            }
                
            //
            // Eliminate CR+LF
            //
            str_in = chrarr_buffer;
            i_charactersRead += str_in.length();
            v_RemoveBreaks (str_in);
            CT_Paragraph * pco_paragraph = NULL;
            v_CreateDefaultParagraph (i_defaultLeftIndent,
                                      i_defaultRightIndent,
                                      eo_defaultAlignment,
                                      str_in,
                                      co_fd,
                                      pco_paragraph);
            pco_paragraph->v_SetPrevious (pco_previous);
            if (pco_previous != NULL)
            {
                pco_previous->v_SetNext (pco_paragraph);
            }
            pco_paragraph->v_SetLastParagraph (true);
            if (!vpo_Paragraphs_.empty())
            {
                vpo_Paragraphs_.back()->v_SetLastParagraph (false);
            }
            vpo_Paragraphs_.push_back (pco_paragraph);
            pco_previous = pco_paragraph;

            double d_done = (double)i_charactersRead/(double)i_fileLength;
            AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, (int)(100*d_done));

        }    while (p_result);

        AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, 0);

        co_textFile.Close();
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, IMPORT_ERROR_READ);
        return false;
    }

    //
    // Reopen to please MFC
    //
    co_inFile.Open (str_fileName.data(), CFile::modeRead);

    return true;

}    //  bool CT_DocDescriptor::b_ReadUTF16 (CFile& co_inFile)

#pragma managed
bool CT_DocDescriptor::b_ReadNonNativeUTF (CFile& co_inFile)
{
#pragma push_macro ("new")
#undef new

    ULONGLONG qw_length = co_inFile.GetLength();
    if (qw_length > INT_MAX)
    {
        ERROR_TRACE (_T("File too long."));
        return false;
    }

    int i_fileLength = (int)qw_length;

    Encoding ^ co_decoder;
    if (ec_UTF8 == eo_Encoding)
    {
        co_decoder = Encoding::UTF8;
        ((CMainFrame *)AfxGetApp()->
            GetMainWnd())->v_SetIdleMessage (_T("Importing a UTF8 file..."));
    }
    else if (ec_UTF16BigEndian == eo_Encoding)
    {
        co_decoder = Encoding::BigEndianUnicode;
        ((CMainFrame *)AfxGetApp()->
            GetMainWnd())->v_SetIdleMessage (_T("Importing a UTF16 (big endian) file..."));
    }
    else
    {
        ERROR_TRACE (_T("Illegal encoding"));
        return false;
    }
    
    CT_Paragraph * pco_previous = NULL;
    CT_Paragraph * pco_next = NULL;
    LPTSTR p_result = NULL;

    int i_defaultLeftIndent = 0;
    int i_defaultRightIndent = 0;
    int i_defaultSpacing = 0;
    ET_Alignment eo_defaultAlignment = ec_AlignmentFront;
    bool b_ = b_GetDefaultParStyle (i_PageWidth_,
                                    i_defaultLeftIndent,
                                    i_defaultRightIndent,
                                    eo_defaultAlignment);
    if (!b_)
    {
        ERROR_TRACE (_T("b_GetDefaultParStyle returned \"false\"")); 
        i_PageWidth_ = 800;
        i_defaultLeftIndent = 0;
        i_defaultRightIndent = 0;
        i_defaultSpacing = 10;
        eo_defaultAlignment = ec_Left;
    }

    //
    // Get the default font
    //
    STL_STRING str_typeface;
    int i_size = 0;
    ET_Charset eo_charset = ec_CharsetFront;
    b_ = b_GetDefaultFont (str_typeface, i_size, eo_charset);
    if (!b_)
    {
        ERROR_TRACE (_T("b_GetDefaultFont_ returned \"false\""));
        str_typeface = _T("Courier New");
        i_size = 10;
        eo_charset = ec_RussianCharset;
    }

    //
    // Create a default font descriptor
    //
    CT_FontDescriptor co_fd (CT_KaiString (str_typeface), eo_charset, i_size);

    //
    // Read and decode
    //
    int i_charactersRead = 0;
    CString cstr_path = co_inFile.GetFilePath();
    try
    {
        //
        // Reopen as a .NET binary reader
        //
        String ^ nstr_fileName = gcnew String (cstr_path);
        co_inFile.Close();
        StreamReader ^ co_reader = gcnew StreamReader (nstr_fileName, co_decoder);
        array<Byte> ^ barr_source = gcnew array<Byte>(i_fileLength);

        while (co_reader->Peek() >= 0)
        {
            String ^ nstr_paragraph = co_reader->ReadLine();
            CString cstr_paragraph (nstr_paragraph);
            STL_STRING str_paragraph (cstr_paragraph.GetBuffer());
            i_charactersRead += str_paragraph.length();

            _TCHAR chrarr_endMark[2];
            chrarr_endMark[0] = _T('\xB6');
            chrarr_endMark[1] = _T('\0');
            str_paragraph += STL_STRING (chrarr_endMark);

            CT_Paragraph * pco_paragraph = NULL;
            v_CreateDefaultParagraph (i_defaultLeftIndent,
                                      i_defaultRightIndent,
                                      eo_defaultAlignment,
                                      str_paragraph,
                                      co_fd,
                                      pco_paragraph);
            pco_paragraph->v_SetPrevious (pco_previous);
            if (pco_previous != NULL)
            {
                pco_previous->v_SetNext (pco_paragraph);
            }
            pco_paragraph->v_SetLastParagraph (true);
            if (!vpo_Paragraphs_.empty())
            {
                vpo_Paragraphs_.back()->v_SetLastParagraph (false);
            }
            vpo_Paragraphs_.push_back (pco_paragraph);
            pco_previous = pco_paragraph;

            double d_done = (double)i_charactersRead/(double)i_fileLength;
            AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, (int)(100*d_done));
        }
        
        co_reader->Close();
    }
    catch (Exception ^ ex)
    {
        CString cstr_msg (ex->Message);
        cstr_msg += _T("IO error: ");
        ERROR_TRACE (cstr_msg.GetBuffer());
        return false;
    }

    AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, 0);

    //
    // Reopen to please MFC
    //
    co_inFile.Open (cstr_path, CFile::modeRead);

    return true;

#pragma pop_macro ("new")

}   //  bool CT_DocDescriptor::b_ReadNonNativeUTF (...)
#pragma unmanaged

#pragma managed
bool CT_DocDescriptor::b_ReadANSI (CFile& co_inFile, bool bUsesForeignChars)
{
#pragma push_macro ("new")
#undef new

    ULONGLONG qw_length = co_inFile.GetLength();
    if (qw_length > INT_MAX)
    {
        ERROR_TRACE (_T("File too long."));
        return false;
    }

    STL_STRING str_fileName = co_inFile.GetFilePath();
    String ^ nstr_path = gcnew String (co_inFile.GetFilePath());
    co_inFile.Close();

    AsciiDecoder::AsciiDecoderForm ^ co_decoder;    
    if (bUsesForeignChars)
    {
        co_decoder = gcnew AsciiDecoder::AsciiDecoderForm (true);
        co_decoder->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
        co_decoder->SourcePath = nstr_path;
        co_decoder->SetSourceData();
    }
    else
    {
        co_decoder = gcnew AsciiDecoder::AsciiDecoderForm (nstr_path);
    }

    if (bUsesForeignChars)
    {
        CWnd * pco_main = AfxGetApp()->GetMainWnd();
        CRect co_wndRect;
        pco_main->GetWindowRect (&co_wndRect);
        co_decoder->SetBounds (co_wndRect.TopLeft().x + co_wndRect.Width()/6, 
                               co_wndRect.TopLeft().y + co_wndRect.Height()/4, 
                               co_decoder->Width, 
                               co_decoder->Height); 

        KaiLibCS::DotNetWindowHandle ^ co_handle = 
            gcnew KaiLibCS::DotNetWindowHandle ((int)pco_main->m_hWnd);
        co_decoder->ShowDialog (co_handle);
    }

    ((CMainFrame *)AfxGetApp()->GetMainWnd())->v_SetIdleMessage (_T("Conversion in progress..."));

    // Reopen for consistency
    BOOL ui_r = co_inFile.Open (str_fileName.data(), CFile::modeRead);

    CT_Paragraph * pco_previous = NULL;
    CT_Paragraph * pco_next = NULL;
    LPTSTR p_result = NULL;

    int i_defaultLeftIndent = 0;
    int i_defaultRightIndent = 0;
    int i_defaultSpacing = 0;
    ET_Alignment eo_defaultAlignment = ec_AlignmentFront;
    bool b_ = b_GetDefaultParStyle (i_PageWidth_,
                                    i_defaultLeftIndent,
                                    i_defaultRightIndent,
                                    eo_defaultAlignment);
    if (!b_)
    {
        ERROR_TRACE (_T("b_GetDefaultParStyle returned \"false\"")); 
        i_PageWidth_ = 800;
        i_defaultSpacing = 10;
        eo_defaultAlignment = ec_Left;
    }

    //
    // Get the default font
    //
    STL_STRING str_typeface;
    int i_size = 0;
    ET_Charset eo_charset = ec_CharsetFront;
    b_ = b_GetDefaultFont (str_typeface, i_size, eo_charset);
    if (!b_)
    {
        ERROR_TRACE (_T("b_GetDefaultFont returned \"false\""));
        str_typeface = _T("Courier New");
        i_size = 10;
        eo_charset = ec_RussianCharset;
    }

    CT_FontDescriptor co_fd (CT_KaiString (str_typeface), eo_charset, i_size);
    CString cstr_in (co_decoder->DecodedString);
    if (cstr_in.GetLength() < 1)
    {
        return false;
    }

//    AfxGetApp()->GetMainWnd()->Invalidate();
//    AfxGetApp()->GetMainWnd()->UpdateWindow();

    int i_paragraphs = 0;
    int i_start = 0;
    do
    {
        i_start = cstr_in.Find (_T("\r\n"), i_start);
        if (i_start < 0)
        {
            continue;
        }
        ++i_paragraphs;
        ++i_start;
    
    }   while (i_start > -1);

    i_start = 0;
    do
    {
        CString cstr_paragraph = cstr_in.Tokenize (_T("\r\n"), i_start);
        if (-1 == i_start)
        {
            continue;
        }

        STL_STRING str_in (cstr_paragraph);
        _TCHAR chrarr_endMark[2];
        chrarr_endMark[0] = _T('\xB6');
        chrarr_endMark[1] = _T('\0');
        str_in += STL_STRING (chrarr_endMark);

        CT_Paragraph * pco_paragraph = NULL;
        v_CreateDefaultParagraph (i_defaultLeftIndent,
                                  i_defaultRightIndent,
                                  eo_defaultAlignment,
                                  str_in,
                                  co_fd,
                                  pco_paragraph);

        pco_paragraph->v_SetPrevious (pco_previous);
        if (pco_previous != NULL)
        {
            pco_previous->v_SetNext (pco_paragraph);
        }
        pco_paragraph->v_SetLastParagraph (true);
        if (!vpo_Paragraphs_.empty())
        {
            vpo_Paragraphs_.back()->v_SetLastParagraph (false);
        }
//                if (pco_paragraph->i_GetPageWidth() > i_PageWidth_)
//                {
//                    i_PageWidth_ = pco_paragraph->i_GetPageWidth();
//                }
        vpo_Paragraphs_.push_back (pco_paragraph);
        pco_previous = pco_paragraph;

        double d_done = (double)vpo_Paragraphs_.size()/(double)i_paragraphs;
        AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, (int)(100*d_done));

    } while (-1 != i_start);

    AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, 0);

    return true;

#pragma pop_macro ("new")

}    //  bool CT_DocDescriptor::b_ReadANSI (CFile& co_inFile)
#pragma unmanaged

bool CT_DocDescriptor::b_RegisterFont (CT_FontDescriptor& rco_fd, 
                                       unsigned int& ui_key)
{
    ui_key = rco_fd.ui_Hash();
    map<unsigned int, CT_FontDescriptor *>::iterator it_font;
    it_font = map_Fonts_.find (ui_key);
    if (it_font == map_Fonts_.end())
    {
        CT_FontDescriptor * pco_fd = new CT_FontDescriptor (rco_fd);
        pco_fd->v_SetReferenceCount (1);
        map_Fonts_[ui_key] = pco_fd;
    }
    else
    {
        unsigned int ui_rc = it_font->second->ui_GetReferenceCount();
        it_font->second->v_SetReferenceCount (++ui_rc);
    }

    return true;

}  //  b_RegisterFont (...)

bool CT_DocDescriptor::b_UnRegisterFont (unsigned int& ui_fontKey)
{
//
// Don't do anything for now
//
return true;

    map<unsigned int, CT_FontDescriptor *>::iterator it_font;
    it_font = map_Fonts_.find (ui_fontKey);
    if (it_font != map_Fonts_.end())
    {
        CT_FontDescriptor * pco_fd = (*it_font).second;
        unsigned int ui_rc = pco_fd->ui_GetReferenceCount();
        if (ui_rc > 1)
        {
            pco_fd->v_SetReferenceCount (--ui_rc);
        }
        else
        {
            CT_FontDescriptor * pco_fd = it_font->second;
            delete pco_fd;
            map_Fonts_.erase (it_font);
        }
    }
    else
    {
        STL_STRING str_ (_T("Font descriptor not found in the font table."));
        ERROR_TRACE (str_);
        return false;
    }

    return true;

}  //  b_UnRegisterFont (...)

void CT_DocDescriptor::v_CreateDefaultParagraph (int i_leftIndent,
                                                 int i_rightIndent,
                                                 ET_Alignment eo_alignment,
                                                 const STL_STRING& str_text,
                                                 CT_FontDescriptor& rco_fd,
                                                 CT_Paragraph *& pco_paragraph)
{
    pco_paragraph = new CT_Paragraph (this);

    pco_paragraph->v_SetLeftIndent (i_leftIndent);
    pco_paragraph->v_SetRigthIndent (i_rightIndent);
    pco_paragraph->v_SetSpacing (ec_LineSpacingAuto);

    unsigned int ui_fontKey = 0;
    bool b_ = b_RegisterFont (rco_fd, ui_fontKey);
    if (!b_)
    {
        return;
    }

    pco_paragraph->v_SetAlignment (eo_alignment);
    pco_paragraph->v_SetBreakChars (_T(" "));

    _TCHAR chrarr_tab[2];
    chrarr_tab[0] = _T('\x9');    // TAB
    chrarr_tab[1] = _T('\0');
    pco_paragraph->v_SetTabChars (STL_STRING (chrarr_tab));

    _TCHAR chrarr_esc[2];
    chrarr_esc[0] = _T('\x1B');   // ESC = decimal 27
    chrarr_esc[1] = _T('\0');
    pco_paragraph->v_SetEscChars (STL_STRING (chrarr_esc));

    pco_paragraph->v_SetPunctChars (_T(".,;:/?<>[]{}~!()-_\'\"\\"));

    _TCHAR chrarr_endMark[2];
    chrarr_endMark[0] = _T('\xB6');
    chrarr_endMark[1] = _T('\0');
    pco_paragraph->v_SetEndOfParagraphChars (STL_STRING (chrarr_endMark));

    STL_STRING str_fd;
    b_ = pco_paragraph->b_CreateFDString (ui_fontKey, str_fd);
    pco_paragraph->assign (str_fd + str_text);

}    //  v_CreateDefaultParagraph (...)

bool CT_DocDescriptor::b_ModifyFontDescriptor (UINT ui_old, 
                                               ET_FontModifier eo_changeTo,
                                               bool b_addOrRemove,
                                               UINT& rui_new)
{
    CT_FontDescriptor * pco_old = NULL;
    
    try
    {
        pco_old = pco_GetFontDescriptor (ui_old);
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }
    ATLASSERT (pco_old);

    CT_FontDescriptor co_new (pco_old->kstr_GetTypeface(),
                              pco_old->eo_GetCharset(),
                              pco_old->ui_GetPointSize(),
                              pco_old->b_GetBold(),
                              pco_old->b_GetItalic(),
                              pco_old->b_GetUnderLine(),
                              pco_old->b_GetStrikeOut());
    switch (eo_changeTo)
    {
        case ec_Bold:
        { 
            co_new.v_SetBold (b_addOrRemove);
            break;
        }
        case ec_Italic:
        {
            co_new.v_SetItalic (b_addOrRemove);
            break;
        }
        case ec_Underline:
        {
            co_new.v_SetUnderline (b_addOrRemove);
            break;
        }
        default:
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Unknow/illegal font modifier."));
            return false;
        }
    }

    bool b_ = b_UnRegisterFont (ui_old);
    if (!b_)
    {
        return false;
    }
    b_ = b_RegisterFont (co_new, rui_new);
    if (!b_)
    {
        return false;
    }

    return true;

}   //  v_ModifyFontDescriptor (...)

bool CT_DocDescriptor::b_StoreFontMap (CFile& co_file)
{
    unsigned int ui_items = map_Fonts_.size();
    try
    {
        co_file.Write (&ui_items, sizeof (unsigned int));

        map<unsigned int, CT_FontDescriptor *>::iterator it_ = map_Fonts_.begin();
        for (; it_ != map_Fonts_.end(); ++it_)
        {
            unsigned int ui_key = (*it_).first;
            co_file.Write (&ui_key, sizeof (unsigned int));

            CT_FontDescriptor * pco_fd = (*it_).second;
            bool b_ = pco_fd->b_Store (co_file);
            if (!b_)
            {
                return false;
            }
        }
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, WRITE_ERROR);
        return false;
    }

    return true;

}    //  b_StoreFontMap()

bool CT_DocDescriptor::b_LoadFontMap (CFile& co_file)
{
    LOCATION ("bool CT_DocDescriptor::b_LoadFontMap()")
//    START_TRACE

    try
    {
        unsigned int ui_items = 0;
        V_LoadItem (co_file, ui_items);

        for (unsigned int ui_item = 0; ui_item < ui_items; ++ui_item)
        {
            unsigned int ui_key = 0;
            V_LoadItem (co_file, ui_key);

            CT_FontDescriptor * pco_fd = new CT_FontDescriptor;
            bool b_ = pco_fd->b_Load (co_file);
            if (!b_)
            {
                return false;
            }

            map_Fonts_.insert 
                (pair<unsigned int, CT_FontDescriptor *>(ui_key, pco_fd));
        }
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
        return false;
    }

    return true;

}    //  b_LoadFontMap()
