#pragma once


const int INSPECTOR_PC		= 1;	// Angle, Align, Btm1



// CUDPManager

class CUDPManager : public CWnd
{
	DECLARE_DYNAMIC(CUDPManager)

public:
	CUDPManager();
	virtual ~CUDPManager();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUdpReceive(WPARAM wLocalPort, LPARAM lParam);

private:
	CUdpSocketCS	m_UdpVisionPC;
	
	CString m_strRecvCmd;

	BOOL	m_bConnectPC;
	
	int		m_nStatusPC;		// Vision PC1 ป๓ลย (0:Not Ready, 1:Ready)
	
	BOOL	m_bLotReady;

	void DoEvents(int nSleep = 0);


	void Get_ConnectRequest();
	void Get_ConnectReply();
	void Get_ConnectEnd();

	void Get_StatusRequest();
	void Get_StatusReply(CString sStatus);
	void Get_StatusUpdate(CString sStatus);

	void Get_LotStart(CString sLotId, CString sPortNo, CString sTrayCnt, CString sCmCnt, CString sRecipe);
	void Get_LoadComplete(CString sGbn, CString sLotID, CString sPortNo, CString sTNo1, CString sTNo2, CString sCNo1, CString sCNo2, CString sPickNo1, CString sPickNo2);

	void Send_Command(CString strSend);
	void Exception_Log(CString sFunc, CString sGbn, int nCase);	// Recevie Exception Log

public:
	void Initialize();
	void Terminate();

	void Set_ConnectRequest();
	void Set_ConnectReply();
	void Set_ConnectEnd();
	void Set_StatusRequest();
	void Set_StatusReply(int nStatus);
	void Set_StatusUpdate(int nStatus); 

	void Set_LotReady(CString sLotId, CString sPortNo);
	void Set_InspectComplete(CString sGbn, CString sLotId, CString sPortNo, CString sTrayNo, CString sCmNo, CString sJudge, CString sNgCode, CString sOffsetX, CString sOffsetY, CString sSizeX, CString sSizeY);


};
extern CUDPManager g_objUDPManager;

