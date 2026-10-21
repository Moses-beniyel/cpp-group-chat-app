// CChatClientDlg.cpp : implementation file
//

#include "pch.h"
#include "ChatApplication.h"
#include "afxdialogex.h"
#include "CChatClientDlg.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <atlconv.h>


// CChatClientDlg dialog

IMPLEMENT_DYNAMIC(CChatClientDlg, CDialogEx)

CChatClientDlg::CChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CChatClientDlg::~CChatClientDlg()
{
}
#include <afxwin.h>
BOOL CChatClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetDlgItemText(IDC_EDIT5, L"Type your message here.....");

    return TRUE;
}
void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_DRAWITEM()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTNCONNECT, &CChatClientDlg::OnBnClickedBtnconnect)
    ON_BN_CLICKED(IDC_BTNDISCONNECT, &CChatClientDlg::OnBnClickedBtndisconnect)
    ON_BN_CLICKED(IDC_BTNSEND, &CChatClientDlg::OnBnClickedBtnsend)
    ON_EN_SETFOCUS(IDC_EDIT5,&CChatClientDlg::OnEnSetfocusEdit5)
    ON_EN_KILLFOCUS(IDC_EDIT5, &CChatClientDlg::OnEnKillfocusEdit5)
    

END_MESSAGE_MAP()
HBRUSH CChatClientDlg::OnCtlColor(
	CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}
void CChatClientDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (nIDCtl == IDC_BTNCONNECT)
    {
        CDC dc;
        dc.Attach(lpDrawItemStruct->hDC);

        CRect rect = lpDrawItemStruct->rcItem;

        dc.FillSolidRect(rect, RGB(0, 120, 215));

        dc.SetTextColor(RGB(255, 255, 255));
        dc.SetBkMode(TRANSPARENT);

        CString text = L"Connect";
        dc.DrawText(text, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        dc.Detach();
        return;
    }

    CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
void CChatClientDlg::AddLog(CString message)
{
    CTime currentTime = CTime::GetCurrentTime();

    CString timestamp =
        currentTime.Format(L"%Y-%m-%d %H:%M:%S");

    CString newLine;

    newLine.Format(
        L"%s  %s\r\n",
        timestamp.GetString(),
        message.GetString()
    );

    // Display log in the UI
    CString existingText;
    GetDlgItemText(IDC_EDIT4, existingText);

    existingText += newLine;

    SetDlgItemText(IDC_EDIT4, existingText);

    // Send log to the separate logging thread
    std::wstring logMessage =
        timestamp.GetString();

    logMessage += L"  ";
    logMessage += message.GetString();

    logger.Log(logMessage);
}
void CChatClientDlg::OnBnClickedBtnconnect()
{
    // TODO: Add your control notification handler code here
    CString serverIP;
    CString portText;
    CString userID;

    GetDlgItemText(IDC_SERVERIP, serverIP);
    GetDlgItemText(IDC_PORT, portText);
    GetDlgItemText(IDC_USERID, userID);

    if (serverIP.IsEmpty())
    {
        AfxMessageBox(L"Please enter Server IP");
        return;
    }

    if (portText.IsEmpty())
    {
        AfxMessageBox(L"Please enter Port");
        return;
    }

    if (userID.IsEmpty())
    {
        AfxMessageBox(L"Please enter User ID");
        return;
    }
    // Convert port text to number
    int port = _ttoi(portText);

    if (port <= 0 || port > 65535)
    {
        AfxMessageBox(L"Please enter a valid port number");
        return;
    }

    // Initialize Winsock
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        AfxMessageBox(L"Winsock initialization failed");
        return;
    }

    // Create TCP socket
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        AfxMessageBox(L"Socket creation failed");
        WSACleanup();
        return;
    }

    // Prepare server address
    sockaddr_in serverAddr = {};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short)port);

    if (InetPton(AF_INET, serverIP, &serverAddr.sin_addr) != 1)
    {
        AfxMessageBox(L"Invalid Server IP address");
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Connect to server
    if (connect(clientSocket,
        (sockaddr*)&serverAddr,
        sizeof(serverAddr)) == SOCKET_ERROR)
    {
        AddLog(L"Connection failed");
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    AddLog(L"Connected to server successfully");
    // Create UDP socket for heartbeat
heartbeatSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

if (heartbeatSocket == INVALID_SOCKET)
{
    AddLog(L"UDP socket creation failed");
}
else
{
    AddLog(L"UDP heartbeat socket created");
}
    SetTimer(1, 5000, NULL);
}
void CChatClientDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1 &&
        heartbeatSocket != INVALID_SOCKET)
    {
        CString userID;
        GetDlgItemText(IDC_USERID, userID);

        CTime currentTime = CTime::GetCurrentTime();
        CString timestamp = currentTime.Format(L"%Y-%m-%d %H:%M:%S");

        CString heartbeatMessage;
        heartbeatMessage.Format(
            L"%s|%s",
            userID.GetString(),
            timestamp.GetString()
        );

        CT2A packet(heartbeatMessage, CP_UTF8);

        sockaddr_in udpAddr = {};
        udpAddr.sin_family = AF_INET;
        udpAddr.sin_port = htons(9091);
        InetPton(AF_INET, L"127.0.0.1", &udpAddr.sin_addr);

        int result = sendto(
            heartbeatSocket,
            packet,
            (int)strlen(packet),
            0,
            (sockaddr*)&udpAddr,
            sizeof(udpAddr)
        );

        if (result == SOCKET_ERROR)
        {
            AddLog(L"Heartbeat sending failed");
        }
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CChatClientDlg::OnBnClickedBtndisconnect()
{
    KillTimer(1);
    // TODO: Add your control notification handler code here
        if (clientSocket != INVALID_SOCKET)
        {
           
            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);

            clientSocket = INVALID_SOCKET;
            if (heartbeatSocket != INVALID_SOCKET)
            {
                closesocket(heartbeatSocket);
                heartbeatSocket = INVALID_SOCKET;
            }

            WSACleanup();

            AddLog(L"Disconnected from server");
        }
}

void CChatClientDlg::OnBnClickedBtnsend()
{
    // TODO: Add your control notification handler code here
    CString message;
    CString userID;

    GetDlgItemText(IDC_EDIT5, message);
    GetDlgItemText(IDC_USERID, userID);

    if (message.IsEmpty() ||
        message == L"Type your message here...")
    {
        AfxMessageBox(L"Please enter a message");
        return;
    }

    if (clientSocket == INVALID_SOCKET)
    {
        AddLog(L"Not connected to server");
        return;
    }

    CTime currentTime = CTime::GetCurrentTime();
    CString timestamp = currentTime.Format(L"%H:%M:%S");

    CString formattedMessage;
    formattedMessage.Format(
        L"%s | %s | %s",
        userID.GetString(),
        timestamp.GetString(),
        message.GetString()
    );

    CT2A convertedMessage(formattedMessage, CP_UTF8);

    int result = send(
        clientSocket,
        convertedMessage,
        (int)strlen(convertedMessage),
        0
    );

    if (result == SOCKET_ERROR)
    {
        AddLog(L"Message sending failed");
        return;
    }

    CString displayMessage;
    displayMessage.Format(
        L"%s | %s: %s",
        timestamp.GetString(),
        userID.GetString(),
        message.GetString()
    );

    AddLog(displayMessage);
    SetDlgItemText(IDC_EDIT5, L"Type your message here...");
}
void CChatClientDlg::OnEnSetfocusEdit5()
{
    CString text;
    GetDlgItemText(IDC_EDIT5, text);

    if (text.Find ( L"Type your message here...")==0)
    {
        SetDlgItemText(IDC_EDIT5, L"");
    }
}

void CChatClientDlg::OnEnKillfocusEdit5()
{
    CString text;
    GetDlgItemText(IDC_EDIT5, text);

    if (text.IsEmpty())
    {
        SetDlgItemText(IDC_EDIT5, L"Type your message here...");
    }
}