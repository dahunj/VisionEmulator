
// MFCTestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCTest.h"
#include "MFCTestDlg.h"
#include "afxdialogex.h"

#include "CUdpManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCTestDlg 대화 상자



CMFCTestDlg::CMFCTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_MESSAGE(UM_UDP_RECEIVE, &CMFCTestDlg::OnUdpReceive)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


//afx_msg LRESULT CMFCTestDlg::OnUdpReceive(WPARAM wParam, LPARAM lParam)
//{
//	// wParam = 수신 길이, lParam = this 포인터(CUdpSocketCS*)
//	auto pSock = reinterpret_cast<CUdpSocketCS*>(lParam);
//	if (!pSock) return 0;
//
//	BYTE buf[2048];
//	CString fromIp;
//	UINT fromPort = 0;
//	int n = pSock->Read(buf, sizeof(buf), fromIp, fromPort);
//	if (n > 0) {
//		// TODO: buf[0..n-1] 처리
//	}
//	return 0;
//}

// CMFCTestDlg 메시지 처리기

BOOL CMFCTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	UdpSockets_EnsureInit(); // 안전 차원


	g_objUDPManager.Create(NULL, NULL, WS_CHILD, CRect(0, 0, 0, 0), this, 0);

	g_objUDPManager.Initialize();

	g_objUDPManager.m_sJudge[0][0][2] = "N1";
	g_objUDPManager.m_sCode[0][0][2] = "MC";

	g_objUDPManager.m_sJudge[0][0][3] = "G";
	g_objUDPManager.m_sCode[0][0][3] = "G";

	//m_pSock = new CUdpSocketCS;
	//if (m_pSock->Open(9000, this, UM_UDP_RECEIVE) != UdpSockErr::Ok) {
	//	AfxMessageBox(L"UDP 소켓 열기 실패");
	//}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCTestDlg::OnDestroy()
{
	
	CDialogEx::OnDestroy();

	g_objUDPManager.DestroyWindow();

	//if (m_pSock) {
	//	m_pSock->Close();
	//	delete m_pSock;
	//	m_pSock = nullptr;
	//}


	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMFCTestDlg::OnBnClickedButton1()
{
	g_objUDPManager.Set_LotReady(1, gData.sLotID[0]);

	/*gData.sPortNo = "1";
	gData.sTNo = "1";
	gData.sCNo = "3";

	g_objUDPManager.m_sJudge[0][0][2] = "G";
	g_objUDPManager.m_sCode[0][0][2] = "G";

	g_objUDPManager.Set_InspectComplete(3, "T2", "JJJJNNNN", "1", "1", "3");*/
}
