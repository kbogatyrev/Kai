//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2006
//    
//    Facility: Kai 1.2
//
//    Module description:    Implementation of paragraph formatting dialog.
//
//  $Id: KaiParagraphFormatDlg.cpp,v 1.6 2007-03-19 02:41:10 kostya Exp $
//
//==============================================================================

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiParagraphFormatDlg.h"

IMPLEMENT_DYNAMIC(CT_ParagraphFormatDlg, CDialog)

CT_ParagraphFormatDlg::CT_ParagraphFormatDlg (CT_Paragraph * pco_paragraph,
                                              CWnd * pco_parent /*=NULL*/)
    : CDialog (CT_ParagraphFormatDlg::IDD, pco_parent), pco_Paragraph_ (pco_paragraph)
{
}

CT_ParagraphFormatDlg::~CT_ParagraphFormatDlg()
{
}

void CT_ParagraphFormatDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CT_ParagraphFormatDlg, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDC_LINE_SPACE_SPIN, OnDeltaposLineSpaceSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_LEFT_INDENT_SPIN, OnDeltaposLeftIndentSpin)
    ON_NOTIFY(UDN_DELTAPOS, IDC_RIGHT_INDENT_SPIN, OnDeltaposRightIndentSpin)
    ON_CBN_CLOSEUP(IDC_LINE_SPACE_COMBO, OnCbnCloseupLineSpaceCombo)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_CBN_SELCHANGE(IDC_ALIGNMENT_COMBO, OnCbnSelchangeAlignmentCombo)
END_MESSAGE_MAP()


// CT_ParagraphFormatDlg message handlers

BOOL CT_ParagraphFormatDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    eo_CurrentSpacingMode_ = pco_Paragraph_->eo_GetSpacingType();
    i_CurrentSpacingSize_ = pco_Paragraph_->i_GetSpacingSize();
    eo_CurrentAlignment_ = pco_Paragraph_->eo_GetAlignment();
    i_CurrentLeftIndent_ = pco_Paragraph_->i_GetLeftIndent();
    i_CurrentRightIndent_ = pco_Paragraph_->i_GetRightIndent();

    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_ALIGNMENT_COMBO);

    CString cstr_text;
    cstr_text.LoadString (IDS_ALIGNMENT_LEFT);
    pco_combo->AddString (cstr_text);
    cstr_text.LoadString (IDS_ALIGNMENT_RIGHT_JUSTIFY);
    pco_combo->AddString (cstr_text);

    if (ec_RightJustified != pco_Paragraph_->eo_GetAlignment())
    {
        pco_combo->SetCurSel (0);
    }
    else
    {
        pco_combo->SetCurSel (1);
    }

    pco_combo = (CComboBox *)GetDlgItem (IDC_LINE_SPACE_COMBO);
    cstr_text.LoadString (IDS_LINE_SPACING_AUTO);
    pco_combo->AddString (cstr_text);
    cstr_text.LoadString (IDS_LINE_SPACING_SINGLE);
    pco_combo->AddString (cstr_text);
    cstr_text.LoadString (IDS_LINE_SPACING_1_5);
    pco_combo->AddString (cstr_text);
    cstr_text.LoadString (IDS_LINE_SPACING_DOUBLE);
    pco_combo->AddString (cstr_text);
    cstr_text.LoadString (IDS_LINE_SPACING_EXACT);
    pco_combo->AddString (cstr_text);
    cstr_text.LoadString (IDS_LINE_SPACING_MULTIPLE);
    pco_combo->AddString (cstr_text);

    v_ShowLineSpacing();

    CSpinButtonCtrl * pco_spinS = (CSpinButtonCtrl *)GetDlgItem (IDC_LINE_SPACE_SPIN);
    pco_spinS->SetPos (0);
    pco_spinS->SetRange (0, 100);

    CEdit * pco_editL = (CEdit *)GetDlgItem (IDC_LEFT_INDENT_EDIT);
    double d_lIndent = static_cast<double>(i_CurrentLeftIndent_)/100;
    CString cstr_displayValueL;
    cstr_displayValueL.Format (_T("%1.1f"), d_lIndent);
    pco_editL->SetWindowText (cstr_displayValueL);

    CSpinButtonCtrl * pco_spinL = (CSpinButtonCtrl *)GetDlgItem (IDC_LEFT_INDENT_SPIN);
    pco_spinL->SetPos (0);
    pco_spinL->SetRange (0, 100);

    CEdit * pco_editR = (CEdit *)GetDlgItem (IDC_RIGHT_INDENT_EDIT);
    double d_rIndent = static_cast<double>(i_CurrentRightIndent_)/100;
    CString cstr_displayValueR;
    cstr_displayValueR.Format (_T("%1.1f"), d_rIndent);
    pco_editR->SetWindowText (cstr_displayValueR);

    CSpinButtonCtrl * pco_spinR = (CSpinButtonCtrl *)GetDlgItem (IDC_RIGHT_INDENT_SPIN);
    pco_spinR->SetPos (0);
    pco_spinR->SetRange (0, 100);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CT_ParagraphFormatDlg::OnDeltaposLineSpaceSpin (NMHDR * pst_NMHDR, 
                                                     LRESULT *p_result)
{
    LPNMUPDOWN pst_NMUpDown = reinterpret_cast<LPNMUPDOWN>(pst_NMHDR);
    // TODO: Add your control notification handler code here
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_LINE_SPACE_EDIT);
    CString cstr_;
    pco_edit->GetWindowText (cstr_);

    if (ec_LineSpacingLines == eo_CurrentSpacingMode_)
    {
        double d_ = _tstof (cstr_);
        d_ += .5 * (double)pst_NMUpDown->iDelta;
        d_ = max (0.5, d_);
        i_CurrentSpacingSize_ = max (1, static_cast<int>(d_ * 100));
        cstr_.Format (_T("%1.1f"), d_);
    }
    if (ec_LineSpacingPoints == eo_CurrentSpacingMode_)
    {
        i_CurrentSpacingSize_ = max (1, 
                                     i_CurrentSpacingSize_ + 10*pst_NMUpDown->iDelta);
        cstr_.Format (_T("%1i pt"), i_CurrentSpacingSize_/10);
    }
    pco_edit->SetWindowText (cstr_);

    *p_result = 0;
}

void CT_ParagraphFormatDlg::OnDeltaposLeftIndentSpin (NMHDR * pst_NMHDR, 
                                                      LRESULT *p_result)
{
    LPNMUPDOWN pst_NMUpDown = reinterpret_cast<LPNMUPDOWN>(pst_NMHDR);
    // TODO: Add your control notification handler code here
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_LEFT_INDENT_EDIT);
    CString cstr_;
    pco_edit->GetWindowText (cstr_);
    double d_ = _tstof (cstr_);
    i_CurrentLeftIndent_ = max (0, static_cast<int>(100*d_) + 10*pst_NMUpDown->iDelta);
    cstr_.Format (_T("%1.1f"), static_cast<double>(i_CurrentLeftIndent_)/100);
    pco_edit->SetWindowText (cstr_ + CString(_T("\"")));

    *p_result = 0;
}

void CT_ParagraphFormatDlg::OnDeltaposRightIndentSpin (NMHDR * pst_NMHDR, 
                                                       LRESULT *p_result)
{
    LPNMUPDOWN pst_NMUpDown = reinterpret_cast<LPNMUPDOWN>(pst_NMHDR);
    // TODO: Add your control notification handler code here
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_RIGHT_INDENT_EDIT);
    CString cstr_;
    pco_edit->GetWindowText (cstr_);
    double d_ = _tstof (cstr_);
    i_CurrentRightIndent_ = max (0, static_cast<int>(100*d_) + 10*pst_NMUpDown->iDelta);
    cstr_.Format (_T("%1.1f"), static_cast<double>(i_CurrentRightIndent_)/100);
    pco_edit->SetWindowText (cstr_ + CString(_T("\"")));

    *p_result = 0;
}

void CT_ParagraphFormatDlg::OnCbnCloseupLineSpaceCombo()
{
    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_LINE_SPACE_COMBO);
    CString cstr_selection;
    int i_idx = pco_combo->GetCurSel();
    if (i_idx >= 0)
    {
        pco_combo->GetLBText (i_idx, cstr_selection);
    }

    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_LINE_SPACE_EDIT);

    BOOL B_enableSizeWnd = FALSE;

    CString cstr_type;
    UINT ui_ret = cstr_type.LoadString (IDS_LINE_SPACING_AUTO);
    if (cstr_type == cstr_selection)
    {
        eo_CurrentSpacingMode_ = ec_LineSpacingAuto;
        i_CurrentSpacingSize_ = 120;
        B_enableSizeWnd = FALSE;
    }
    ui_ret = cstr_type.LoadString (IDS_LINE_SPACING_SINGLE);
    if (cstr_type == cstr_selection)
    {
        eo_CurrentSpacingMode_ = ec_LineSpacingLines;
        i_CurrentSpacingSize_ = 100;
        B_enableSizeWnd = FALSE;
    }
    ui_ret = cstr_type.LoadString (IDS_LINE_SPACING_1_5);
    if (cstr_type == cstr_selection)
    {
        eo_CurrentSpacingMode_ = ec_LineSpacingLines;
        i_CurrentSpacingSize_ = 150;
        B_enableSizeWnd = FALSE;
    }
    ui_ret = cstr_type.LoadString (IDS_LINE_SPACING_DOUBLE);
    if (cstr_type == cstr_selection)
    {
        eo_CurrentSpacingMode_ = ec_LineSpacingLines;
        i_CurrentSpacingSize_ = 200;
        B_enableSizeWnd = FALSE;
    }
    ui_ret = cstr_type.LoadString (IDS_LINE_SPACING_EXACT);
    if (cstr_type == cstr_selection)
    {
        eo_CurrentSpacingMode_ = ec_LineSpacingPoints;
        CString cstr_displayValue;
        cstr_displayValue.Format (_T("%1i pt"), i_CurrentSpacingSize_/10);
        pco_edit->SetWindowText (cstr_displayValue);
        B_enableSizeWnd = TRUE;
    }
    ui_ret = cstr_type.LoadString (IDS_LINE_SPACING_MULTIPLE);
    if (cstr_type == cstr_selection)
    {
        eo_CurrentSpacingMode_ = ec_LineSpacingLines;
        double d_size = max (3., i_CurrentSpacingSize_/100);
        CString cstr_displayValue;
        cstr_displayValue.Format (_T("%1.0f"), d_size);
        pco_edit->SetWindowText (cstr_displayValue);
        B_enableSizeWnd = TRUE;
    }

    if ((eo_CurrentSpacingMode_ <= ec_LineSpacingFront) || 
        (eo_CurrentSpacingMode_ >= ec_LineSpacingBack))
    {
        eo_CurrentSpacingMode_ = ec_LineSpacingAuto;
        i_CurrentSpacingSize_ = 120;
    }

    pco_edit->EnableWindow (B_enableSizeWnd);

}   // OnCbnCloseupLineSpaceCombo()

void CT_ParagraphFormatDlg::OnCbnSelchangeAlignmentCombo()
{
    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_ALIGNMENT_COMBO);

    int i_idx = pco_combo->GetCurSel();
    CString cstr_alignment;
    if (i_idx >= 0)
    {
        pco_combo->GetLBText (i_idx, cstr_alignment);
        CString cstr_left;
        cstr_left.LoadString (IDS_ALIGNMENT_LEFT);
        if (cstr_left == cstr_alignment)
        {
            eo_CurrentAlignment_ = ec_Left;
        }
        CString cstr_justify;
        cstr_justify.LoadString (IDS_ALIGNMENT_RIGHT_JUSTIFY);
        if (cstr_justify == cstr_alignment)
        {
            eo_CurrentAlignment_ = ec_RightJustified;
        }
    }
}

void CT_ParagraphFormatDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    CClientDC co_dc (GetParent());
    int i_pixPerInch = co_dc.GetDeviceCaps (LOGPIXELSX);
    int i_marginsInLus = (i_pixPerInch * (i_CurrentLeftIndent_ + 
                                          i_CurrentRightIndent_))/100;
    int i_minPageWidth = i_pixPerInch/2;
    int i_pageWidth = (pco_Paragraph_->i_GetPageWidth() * i_pixPerInch)/100;
    if (i_pageWidth - i_marginsInLus <= 0)
    {
        AfxMessageBox (_T("Illegal indent size"), 
                       MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    OnOK();
}

void CT_ParagraphFormatDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    OnCancel();
}

//
// Helpers
//
void CT_ParagraphFormatDlg::v_ShowLineSpacing()
{
    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_LINE_SPACE_COMBO);
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_LINE_SPACE_EDIT);

    CString cstr_text;
    switch (eo_CurrentSpacingMode_)
    {
        case ec_LineSpacingAuto:
        {
            cstr_text.LoadString (IDS_LINE_SPACING_AUTO);
            pco_combo->SelectString (0, cstr_text);
            pco_edit->EnableWindow (FALSE);
            i_CurrentSpacingSize_ = 120;

            break;
        }
        case ec_LineSpacingLines:
        {
            if (i_CurrentSpacingSize_ <= 100)
            {
                cstr_text.LoadString (IDS_LINE_SPACING_SINGLE);
                pco_combo->SelectString (0, cstr_text);
            }
            if ((i_CurrentSpacingSize_ > 100) && (i_CurrentSpacingSize_ <= 150))
            {
                cstr_text.LoadString (IDS_LINE_SPACING_1_5);
                pco_combo->SelectString (0, cstr_text);
            }
            if ((i_CurrentSpacingSize_ > 150) && (i_CurrentSpacingSize_ <= 200))
            {
                cstr_text.LoadString (IDS_LINE_SPACING_DOUBLE);
                pco_combo->SelectString (0, cstr_text);
            }
            if (i_CurrentSpacingSize_ >= 250)
            {
                cstr_text.LoadString (IDS_LINE_SPACING_MULTIPLE);
                pco_combo->SelectString (0, cstr_text);
                double d_size = static_cast<double>(i_CurrentSpacingSize_)/100;
                CString cstr_displayValue;
                cstr_displayValue.Format (_T("%1.1f"), d_size);
                CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_LINE_SPACE_EDIT);
                pco_edit->SetWindowText (cstr_displayValue);
                pco_edit->EnableWindow (TRUE);
            }
            else
            {
                pco_edit->EnableWindow (FALSE);
            }

            break;
        }
        case ec_LineSpacingPoints:
        {
            cstr_text.LoadString (IDS_LINE_SPACING_EXACT);
            pco_combo->SelectString (0, cstr_text);
            pco_edit->EnableWindow (TRUE);
            CString cstr_displayValue;
            cstr_displayValue.Format (_T("%1i pt"), i_CurrentSpacingSize_/10);
            CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_LINE_SPACE_EDIT);
            pco_edit->SetWindowText (cstr_displayValue);
            pco_edit->SetWindowText (cstr_displayValue);

            break;
        }
        case ec_LineSpacingFront:
        case ec_LineSpacingBack:
        default:
        {
            break;
        }
    }
}       //  v_ShowLineSpacing()
