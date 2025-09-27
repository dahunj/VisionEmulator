
// MFCTestDlg.h: 헤더 파일
//

#pragma once



// CMFCTestDlg 대화 상자
class CMFCTestDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCTestDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


    //CUdpSocketCS* m_pSock = nullptr;
    //afx_msg LRESULT OnUdpReceive(WPARAM wParam, LPARAM lParam);

    
public:
    afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
};
