// CLoginDialog.cpp : implementation file
//

#include "pch.h"
#include "ChatApplication.h"
#include "afxdialogex.h"
#include "CLoginDialog.h"
#include "CChatClientDlg.h"


// CLoginDialog dialog

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

CLoginDialog::CLoginDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CLoginDialog::~CLoginDialog()
{
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialogEx)
	ON_WM_DRAWITEM()
    ON_BN_CLICKED(IDC_BTNLOGIN, &CLoginDialog::OnBnClickedBtnlogin)
    ON_BN_CLICKED(IDC_BTNSIGNUP, &CLoginDialog::OnBnClickedBtnsignup)
END_MESSAGE_MAP()


// CLoginDialog message handlers
void CLoginDialog::OnDrawItem(
    int nIDCtl,
    LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (nIDCtl == IDC_BTNLOGIN || nIDCtl == IDC_BTNSIGNUP)
    {
        CDC dc;
        dc.Attach(lpDrawItemStruct->hDC);

        CRect rect = lpDrawItemStruct->rcItem;

        CBrush blueBrush(RGB(0, 120, 215));
        CPen bluePen(PS_SOLID, 1, RGB(0, 120, 215));

        CBrush* oldBrush = dc.SelectObject(&blueBrush);
        CPen* oldPen = dc.SelectObject(&bluePen);

        dc.RoundRect(rect, CPoint(12, 12));

        dc.SelectObject(oldBrush);
        dc.SelectObject(oldPen);

        dc.SetTextColor(RGB(255, 255, 255));
        dc.SetBkMode(TRANSPARENT);

        CString text;
        GetDlgItemText(nIDCtl, text);

        dc.DrawText(text, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        dc.Detach();
        return;
    }

    CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
void CLoginDialog::OnBnClickedBtnlogin()
    {
        CString username;
        CString password;

        GetDlgItemText(IDC_USERNAME1, username);
        GetDlgItemText(IDC_PASSWORD1, password);

        if (username.IsEmpty())
        {
            AfxMessageBox(L"Please enter username.");
            return;
        }
        else if (password.IsEmpty())
        {
            AfxMessageBox(L"Please enter password.");
            return;
        }
        else if (username == L"user1" && password == L"pass123")
        {
            CChatClientDlg* pDlg = new CChatClientDlg();

            if (pDlg->Create(IDD_DIALOG1))
            {
                pDlg->ShowWindow(SW_SHOW);
                EndDialog(IDOK);
            }
            else
            {
                delete pDlg;
                return;
            }
        }
        else
        {
            AfxMessageBox(L"Invalid username or password!");
        }
    }

void CLoginDialog::OnBnClickedBtnsignup()
{
    // TODO: Add your control notification handler code here
    CString username;
    CString password;
    CString confirmPassword;

    GetDlgItemText(IDC_USERNAME2, username);
    GetDlgItemText(IDC_PASSWORD2, password);
    GetDlgItemText(IDC_CONFRIMPASSWORD2, confirmPassword);

    if (username.IsEmpty() || password.IsEmpty() || confirmPassword.IsEmpty())
    {
        AfxMessageBox(L"Please fill the respective fields.");
        return;
    }
    else if (password != confirmPassword)
    {
        AfxMessageBox(L"Passwords do not match!");
        return;
    }
    else
    {
        CChatClientDlg* pDlg = new CChatClientDlg();

        if (pDlg->Create(IDD_DIALOG1))
        {
            pDlg->ShowWindow(SW_SHOW);
            EndDialog(IDOK);
        }
        else
        {
            delete pDlg;
            AfxMessageBox(L"Unable to open Server page.");
        }
    }
}
