#pragma once
#include "afxdialogex.h"


// CLoginDialog dialog

class CLoginDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CLoginDialog)

public:
    CLoginDialog(CWnd* pParent = nullptr);   // standard constructor
    virtual ~CLoginDialog();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG2 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnPaint();
    afx_msg void OnDrawItem(
        int nIDCtl,
        LPDRAWITEMSTRUCT lpDrawItemStruct
    );

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnlogin();
    afx_msg void OnBnClickedBtnsignup();
};
