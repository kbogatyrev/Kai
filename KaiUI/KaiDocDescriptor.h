//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:.
//
//    $Id: KaiDocDescriptor.h,v 1.47 2008/08/05 16:56:01 kostya Exp $
//    
//==============================================================================

#pragma once

#include "KaiFontDescriptor.h"
#include "KaiParagraph.h"

class CT_TextPos;
class CT_UndoStack;

class CT_DocDescriptor
{
friend class CKaiDoc;

protected:

enum et_Encoding
{
    ec_KaiInternal,
    ec_UTF16,
    ec_UTF16BigEndian,
    ec_UTF8,
    ec_ASCII,
    ec_Unknown 

} eo_Encoding;

protected:
    std::map<unsigned int, CT_FontDescriptor *> map_Fonts_;

    template class KAI_API std::vector<CT_Paragraph *>;
    std::vector<CT_Paragraph *> vpo_Paragraphs_;

    CT_UndoStack * pco_UndoStack_;

    bool b_Imported_;

    et_Encoding eo_SaveAsMode_;

    bool b_Unrenderable_;

    int i_PageWidth_;        // 100ths of an inch
    bool b_InvalidateSearch_;

    STL_STRING str_Title_;

    void v_Null_();

public:
    CT_DocDescriptor();
    ~CT_DocDescriptor();

    bool b_Load (CFile& co_file);
    bool b_Store (CFile& co_file);
    bool b_StoreAsPlainText (CFile& co_file);

    void v_ResetPrintFonts()
    {
        std::map<unsigned int, CT_FontDescriptor *>::iterator it_ = map_Fonts_.begin();
        for (; it_ != map_Fonts_.end(); ++it_)
        {
            (*it_).second->v_ResetPrintFont();
        }
    }

    bool b_GetDefaultFont (STL_STRING& str_typeface, 
                           int& i_size, 
                           ET_Charset& eo_charset);

    bool b_GetDefaultParStyle (int& i_lineWidth,
                               int& i_leftIndent,
                               int& i_rightIndent,
                               ET_Alignment& eo_alignment);
    bool b_SearchInvalid()
    {
        return b_InvalidateSearch_;
    }

    void v_ValidateSearch()
    {
        b_InvalidateSearch_ = false;
    }

    unsigned int ui_GetDocLength();

    int i_NParagraphs() const
    {
        return vpo_Paragraphs_.size();
    }

    CT_Paragraph * pco_GetParagraph (int i_p) const
    {
        return vpo_Paragraphs_[i_p]; 
    }

    void v_RemoveBreaks (STL_STRING&);

    std::vector<CT_Paragraph *>& vpo_GetParagraphs();

    CT_FontDescriptor * pco_GetFontDescriptor (unsigned int ui_key);

    CT_FontDescriptor * pco_GetDefaultFontDescriptor();

    void v_AddParagraph (CT_Paragraph * pco_paragraph, int i_insertAfter)
    {
        if (++i_insertAfter < 0)
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Bad insertion postion"));
            return;
        }
        vpo_Paragraphs_.insert (vpo_Paragraphs_.begin() + i_insertAfter, pco_paragraph);
    }

    void v_RemoveParagraph (int i_at)
    {
        vpo_Paragraphs_.erase (vpo_Paragraphs_.begin()+i_at); 
    }

    void v_DeleteText (CT_TextPos& co_first, 
                       CT_TextPos& co_last, 
                       bool b_caretAtStart,           // needed for undoing
                       bool b_selection);             //      -- " --

    void v_InsertText (CT_TextPos * pco_at, 
                       CT_KaiString& kstr_text, 
                       bool b_replace);

    bool b_CheckDocFormat (CFile& co_inFile, unsigned int& ui_version);

    bool b_ImportTextFile (CFile& co_inFile);

    bool b_ReadUTF16 (CFile& co_inFile);

//    bool b_ReadUTF16BigEndian (CFile& co_inFile);

//    bool b_ReadUTF8 (CFile& co_inFile);
    
    bool b_ReadNonNativeUTF (CFile& co_inFile);

    bool b_ReadANSI (CFile& co_inFile, bool bUsesForeignChars);

	bool b_FileHasForeignCharacters (CFile& co_inFile);

    bool b_RegisterFont (CT_FontDescriptor& rco_fd, unsigned int& ui_fontKey);

    bool b_UnRegisterFont (unsigned int& ui_fontKey);

    void v_CreateDefaultParagraph (int i_leftIndent,
                                   int i_rightIndent,
                                   ET_Alignment eo_defaultAlignment,
                                   const STL_STRING& str_in,
                                   CT_FontDescriptor& rco_fd,
                                   CT_Paragraph *& pco_paragraph);

    bool b_ModifyFontDescriptor (UINT ui_old, 
                                 ET_FontModifier eo_property,
                                 bool b_addOrRemove,
                                 UINT& rui_new);

    bool b_StoreFontMap (CFile&);

    bool b_LoadFontMap (CFile&);

    void v_SetMaxParagraphWidth (int i_width)
    {
        i_PageWidth_ = i_width;
    }

    int i_GetPageWidth()
    {
        return i_PageWidth_;
    }

    bool b_IsImported()
    {
        return b_Imported_;
    }

    void v_SetUnrenderable()
    {
        b_Unrenderable_ = true;
    }

    bool b_IsUnrenderable()
    {
        return b_Unrenderable_;
    }

};
