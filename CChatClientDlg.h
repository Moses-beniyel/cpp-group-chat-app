#pragma once
#include "Logger.h"
#include "afxdialogex.h"


// CChatClientDlg dialog

class CChatClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatClientDlg)

public:
	CChatClientDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CChatClientDlg();
	SOCKET clientSocket = INVALID_SOCKET;
	SOCKET heartbeatSocket= INVALID_SOCKET;
	void AddLog(CString message);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent); 
private:
	Logger logger;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnconnect();
	afx_msg void OnBnClickedBtndisconnect();
	afx_msg void OnBnClickedBtnsend();
	afx_msg void OnEnSetfocusEdit5();
	afx_msg void OnEnKillfocusEdit5();

};
