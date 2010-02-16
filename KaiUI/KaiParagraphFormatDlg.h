//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2006
//    
//    Facility: Kai 1.2
//
//    Module description: Interface of the paragraph formatting dialog
//
//    $Id: KaiParagraphFormatDlg.h,v 1.4 2007-03-25 00:44:40 kostya Exp $
//
//==============================================================================
#pragma once

#include "KaiParagraph.h"
#include "afxwin.h"

class CT_ParagraphFormatDlg : public CDialog
{
    DECLARE_DYNAMIC(CT_ParagraphFormatDlg)

public:
    CT_ParagraphFormatDlg (CT_Paragraph * pco_paragraph,
                           CWnd * pco_parent = NULL);
    virtual ~CT_ParagraphFormatDlg();

// Dialog Data
    enum { IDD = IDD_FORMAT_PARAGRAPH };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

protected:
// Class variables
    CT_Paragraph * pco_Paragraph_;
    ET_LineSpacingType eo_CurrentSpacingMode_;
    ET_Alignment eo_CurrentAlignment_;
    int i_CurrentSpacingSize_;
    int i_CurrentLeftIndent_;
    int i_CurrentRightIndent_;

protected:
    void v_ShowLineSpacing();

public:
    afx_msg void OnDeltaposLineSpaceSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposLeftIndentSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDeltaposRightIndentSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCbnCloseupLineSpaceCombo();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnCbnSelchangeAlignmentCombo();

public:
    ET_LineSpacingType eo_CurrentSpacingMode() { return eo_CurrentSpacingMode_; };
    ET_Alignment eo_CurrentAlignment() { return eo_CurrentAlignment_; };
    int i_CurrentSpacingSize() { return i_CurrentSpacingSize_; };
    int i_CurrentLeftIndent() { return i_CurrentLeftIndent_; };
    int i_CurrentRightIndent() { return i_CurrentRightIndent_; };
};
