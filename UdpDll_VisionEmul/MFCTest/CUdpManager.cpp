// CUdpManager.cpp: 구현 파일
//

#include "pch.h"
#include "MFCTest.h"
#include "CUdpManager.h"
#include "LogFile.h"

// CUdpManager
CUdpManager g_objUDPManager;


CCriticalSection g_csInspector;	// Send_Command 문제 해결하기 위함



IMPLEMENT_DYNAMIC(CUdpManager, CWnd)

CUdpManager::CUdpManager()
{
	
}

CUdpManager::~CUdpManager()
{
}


BEGIN_MESSAGE_MAP(CUdpManager, CWnd)
	ON_MESSAGE(UM_UDP_RECEIVE, &CUdpManager::OnUdpReceive)
END_MESSAGE_MAP()



// CUdpManager 메시지 처리기


afx_msg LRESULT CUdpManager::OnUdpReceive(WPARAM wParam, LPARAM lParam)
{
	// wParam = 수신 길이, lParam = this 포인터(CUdpSocketCS*)
	auto pSock = reinterpret_cast<CUdpSocketCS*>(lParam);
	if (!pSock) return 0;

	BYTE buf[2048];
	CString fromIp;
	UINT fromPort = 0;
	int nInspector = 0;

	int nLen = pSock->Read(buf, sizeof(buf), fromIp, fromPort);

	if (fromPort == 21000) { nInspector = INSPECTOR_PC1;  }
	if (fromPort == 22000) { nInspector = INSPECTOR_PC2;  }
	if (fromPort == 23000) { nInspector = INSPECTOR_PC3;  }
	if (fromPort == 24000) { nInspector = INSPECTOR_PC4;  }

	CString strLog;

	if (nInspector == 0 || nLen < 1) {
		strLog.Format("[H<-V%d] : Local Port (%d) Mismatch or Receive Data Zero (%d)", nInspector, fromPort, nLen);
		g_objLogFile.Save_InspectorLog(strLog);
		return 0;
	}

	CString strRecvSocket;
	strRecvSocket.Format("%s", buf);
	m_strRecvCmd += strRecvSocket;

	while (!m_strRecvCmd.IsEmpty()) {
		int nStart = m_strRecvCmd.Find("@");
		int nEnd = m_strRecvCmd.Find("\n");

		if (nEnd < 0) break;	// 버퍼에 들어오는 중...

		if (nStart < 0 || nStart > nEnd) {
			strLog.Format("[H<-V%d] : <<Error>> %s : Start(%d), End(%d)", nInspector, m_strRecvCmd, nStart, nEnd);
			g_objLogFile.Save_InspectorLog(strLog);
			m_strRecvCmd.Delete(0, nEnd + 1);	// 쓰레기값이 채워져 있어서...
			continue;
		}

		CString strRecv = m_strRecvCmd.Mid(nStart + 1, nEnd - nStart - 1);
		m_strRecvCmd.Delete(0, nEnd + 1);

		char chSep = ',';
		CString strCmd, strOp;

		AfxExtractSubString(strCmd, strRecv, 0, chSep);
		AfxExtractSubString(strOp, strRecv, 1, chSep);

		// Inspector Log ////////////////////////////////////////
		if (strCmd != "HEART" && strOp != "BEAT") {
			strLog.Format("[H<-V%d] : %s", nInspector, strRecv);
			g_objLogFile.Save_InspectorLog(strLog);
		}
		/////////////////////////////////////////////////////////

		CString strArg[12];
		for (int i = 0; i < 12; i++) AfxExtractSubString(strArg[i], strRecv, i + 2, chSep);

		if (strCmd == "CONNECT")
		{
			if (strOp == "REQUEST")	Get_ConnectRequest(nInspector);
			else if (strOp == "REPLY") Get_ConnectReply(nInspector);
			else if (strOp == "END") Get_ConnectEnd(nInspector);
		}
		else if (strCmd == "STATUS")
		{
			if (strOp == "REQUEST")	Get_StatusRequest(nInspector);
			else if (strOp == "REPLY") Get_StatusReply(nInspector, strArg[0]);
			else if (strOp == "UPDATE") Get_StatusUpdate(nInspector, strArg[0]);
		}
		else if (strCmd == "LOAD")
		{
			//						Get_LoadComplete    			sGBn,    LotID,     portNo,     TrayNo1,   CmNo1,    TrayNo2,   CmNo2,     TrayNo3,    CmNo3,    TrayNo4,   CmNo4
			if (strOp == "COMPLETE") Get_LoadComplete(strArg[0], strArg[1], strArg[2], strArg[3], strArg[4], strArg[5], strArg[6]);
		}
		else if (strCmd == "LOT")
		{
			if (strOp == "START") Get_LotStart(strArg[0]);
		}
	}
	return 1;
}




void CUdpManager::DoEvents(int nSleep)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (nSleep > 0) Sleep(nSleep);
}


void CUdpManager::Get_ConnectRequest(int nInspector)
{
	if (nInspector == INSPECTOR_PC1) m_bConnectPC1 = TRUE;
	if (nInspector == INSPECTOR_PC2) m_bConnectPC2 = TRUE;
	if (nInspector == INSPECTOR_PC3) m_bConnectPC3 = TRUE;
	if (nInspector == INSPECTOR_PC4) m_bConnectPC4 = TRUE;
	Set_ConnectReply(nInspector);
}

void CUdpManager::Get_ConnectReply(int nInspector)
{
	if (nInspector == INSPECTOR_PC1) m_bConnectPC1 = TRUE;
	if (nInspector == INSPECTOR_PC2) m_bConnectPC2 = TRUE;
	if (nInspector == INSPECTOR_PC3) m_bConnectPC3 = TRUE;
	if (nInspector == INSPECTOR_PC4) m_bConnectPC4 = TRUE;
}

void CUdpManager::Get_ConnectEnd(int nInspector)
{
	if (nInspector == INSPECTOR_PC1) m_bConnectPC1 = FALSE;
	if (nInspector == INSPECTOR_PC2) m_bConnectPC2 = FALSE;
	if (nInspector == INSPECTOR_PC3) m_bConnectPC3 = FALSE;
	if (nInspector == INSPECTOR_PC4) m_bConnectPC4 = FALSE;
}


void CUdpManager::Get_StatusRequest(int nInspector)
{
	//BOOL bStatus = g_objSequenceMain.Is_MainThreadRun();
	//if (bStatus) Set_StatusReply(nInspector, 1);
	//else Set_StatusReply(nInspector, 0);
}

void CUdpManager::Get_StatusReply(int nInspector, CString sStatus)
{
	if (nInspector == INSPECTOR_PC1) m_nStatusPC1 = atoi(sStatus);
	if (nInspector == INSPECTOR_PC2) m_nStatusPC2 = atoi(sStatus);
	if (nInspector == INSPECTOR_PC3) m_nStatusPC3 = atoi(sStatus);
	if (nInspector == INSPECTOR_PC4) m_nStatusPC4 = atoi(sStatus);
}

void CUdpManager::Get_StatusUpdate(int nInspector, CString sStatus)
{
	Set_StatusReply(nInspector);
}

void CUdpManager::Get_LotStart(CString sLotID)
{
	gData.sLotID[0] = sLotID;

	Set_LotReady(1, gData.sLotID[0]);
	Set_LotReady(2, gData.sLotID[0]);
	Set_LotReady(3, gData.sLotID[0]);
	Set_LotReady(4, gData.sLotID[0]);
}



void CUdpManager::Send_Command(int nInspector, CString strSend)
{
	// Inspector Log //////////////////////////////////////
	CString strLog;
	strLog.Format("[H->V%d] : %s", nInspector, strSend);
	//g_objLogFile.Save_InspectorLog(strLog);
	///////////////////////////////////////////////////////

	g_csInspector.Lock();	// Critical Section

	CString strSendSocket;
	strSendSocket.Format("@%s\n", strSend);

	char chSend[1024] = { 0 };
	int nLength = strSendSocket.GetLength();
	memcpy(chSend, (LPSTR)(LPCSTR)strSendSocket, nLength);

	if (nInspector == INSPECTOR_ALL || nInspector == INSPECTOR_PC1) m_UdpVisionPC1.Write_Socket((BYTE*)chSend, nLength);
	if (nInspector == INSPECTOR_ALL || nInspector == INSPECTOR_PC2) m_UdpVisionPC2.Write_Socket((BYTE*)chSend, nLength);
	if (nInspector == INSPECTOR_ALL || nInspector == INSPECTOR_PC3) m_UdpVisionPC3.Write_Socket((BYTE*)chSend, nLength);
	if (nInspector == INSPECTOR_ALL || nInspector == INSPECTOR_PC4) m_UdpVisionPC4.Write_Socket((BYTE*)chSend, nLength);

	g_csInspector.Unlock();	// Critical Section
}

void CUdpManager::Initialize()
{
	BOOL bOpenedPC1 = m_UdpVisionPC1.Open_Socket(21001, 21000, "127.0.0.1", this);
	BOOL bOpenedPC2 = m_UdpVisionPC2.Open_Socket(22001, 22000, "127.0.0.1", this);
	BOOL bOpenedPC3 = m_UdpVisionPC3.Open_Socket(23001, 23000, "127.0.0.1", this);
	BOOL bOpenedPC4 = m_UdpVisionPC4.Open_Socket(24001, 24000, "127.0.0.1", this);

}

void CUdpManager::Terminate()
{
	m_UdpVisionPC1.Close_Socket();
	m_UdpVisionPC2.Close_Socket();
	m_UdpVisionPC3.Close_Socket();
	m_UdpVisionPC4.Close_Socket();
}



void CUdpManager::Get_LoadComplete( CString sType, CString sLotID, CString nPortNo, CString sTNo1, CString sTNo2, CString sCNo1,  CString sCNo2)
{
	int nTNo1, nTNo2;
	int nCNo1, nCNo2;

	nTNo1 = atoi(sTNo1);
	nTNo2 = atoi(sTNo2);
	

	nCNo1 = atoi(sCNo1);
	nCNo2 = atoi(sCNo2);	

	if (nTNo1 != -1 && nCNo1 != -1) Set_ScanComplete(1, sType, sLotID, nPortNo, sTNo1, sCNo1);
	Sleep(10);
	if (nTNo2 != -1 && nCNo2 != -1) Set_ScanComplete(1, sType, sLotID, nPortNo, sTNo2, sCNo2);
	Sleep(10);

	if (nTNo1 != -1 && nCNo1 != -1) Set_InspectComplete(1, sType, sLotID, nPortNo, sTNo1, sCNo1);
	Sleep(10);
	if (nTNo2 != -1 && nCNo2 != -1) Set_InspectComplete(1, sType, sLotID, nPortNo, sTNo2, sCNo2);
	Sleep(10);

}

void CUdpManager::Set_ScanComplete(int nInspector, CString sType, CString sLotID, CString sPortNo, CString sTrayNo, CString sCMNo)
{
	CString	strSendCmd;
	strSendCmd.Format("SCAN,COMPLETE,%s,%s,%s,%s,%s", sType, sLotID, sPortNo, sTrayNo, sCMNo);
	Send_Command(nInspector, strSendCmd);
}

int CUdpManager::Get_Random(int nStart, int nEnd)
{
	static BOOL bSeed = FALSE;
	if (nStart >= nEnd) return 0;
	if (!bSeed) { srand((unsigned)time(NULL)); bSeed = TRUE; }
	return (rand() % (nEnd - nStart + 1) + nStart);
}

void CUdpManager::Set_InspectComplete(int nInspector, CString sGbn, CString sLotID, CString sPortNo, CString sTNo, CString sCNo)
{
	CString	strSendCmd;

	int nPortNo, nTNo, nCNo;
	nPortNo = atoi(sPortNo);
	nTNo = atoi(sTNo);
	nCNo = atoi(sCNo);

	int nRand = Get_Random(0, 99);
	int nNGPercent1 = 35;
	int nNGPercent2 = nNGPercent1 +10;
	int nNGPercent3 = nNGPercent2 + 10;
	int nNGPercent4 = nNGPercent3 + 10;

	int nJudgeNo = 0;

	nJudgeNo = (nRand < nNGPercent1 ? 4 : (nRand < nNGPercent2 ? 5 : (nRand < nNGPercent3 ? 6 : (nRand < nNGPercent4 ? 7 : 2))));

	if (nJudgeNo == 2)
	{
		//m_sJudge[nPortNo - 1][nTNo - 1][nCNo - 1].Format("%d", nJudgeNo);
		m_sJudge[nPortNo - 1][nTNo - 1][nCNo - 1] = "G";
		m_sCode[nPortNo - 1][nTNo - 1][nCNo - 1] = "";
	}
	else 
	{
		//m_sJudge[nPortNo - 1][nTNo - 1][nCNo - 1].Format("%d", nJudgeNo);
		m_sJudge[nPortNo - 1][nTNo - 1][nCNo - 1] = "R";

		if (nJudgeNo == 4)
		{
			m_sCode[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-GLUECT1";
		}
		else if (nJudgeNo == 5)
		{
			m_sCode[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-GLUECTPISMO";
		}
		else if (nJudgeNo == 6)
		{
			m_sCode[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-GLUECTSLOPE";
		}
		else if (nJudgeNo == 7)
		{
			m_sCode[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-DRIVERICDA";
		}
		
	}			

	int nRand2 = Get_Random(0, 99);
	int nNGPercent1_2 = 0;
	int nNGPercent2_2 = nNGPercent1_2 + 0;
	int nNGPercent3_2 = nNGPercent2_2 + 0;
	int nNGPercent4_2 = nNGPercent3_2 + 0;

	nJudgeNo = (nRand < nNGPercent1_2 ? 4 : (nRand < nNGPercent2_2 ? 5 : (nRand < nNGPercent3_2 ? 6 : (nRand < nNGPercent4_2 ? 7 : 2))));

	//m_sJudge[nPortNo - 1][nTNo - 1][nCNo - 1] = "G";
	if (nJudgeNo == 4)
	{		
		m_sCodeV[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-GLUECT1";
	}
	else if (nJudgeNo == 5)
	{		
		m_sCodeV[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-GLUECTPISMO";
	}
	else if (nJudgeNo == 6)
	{		
		m_sCodeV[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-GLUECTSLOPE";
	}
	else if (nJudgeNo == 7)
	{		
		m_sCodeV[nPortNo - 1][nTNo - 1][nCNo - 1] = "FS-S-DRIVERICDA";
	}


	strSendCmd.Format("INSPECT,COMPLETE,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", sGbn, sLotID, sPortNo, sTNo, sCNo, m_sJudge[nPortNo - 1][nTNo - 1][nCNo - 1], m_sCode[nPortNo - 1][nTNo - 1][nCNo - 1],"", "", "", m_sCodeV[nPortNo - 1][nTNo - 1][nCNo - 1], "");
	Send_Command(nInspector, strSendCmd);
}




void CUdpManager::Set_ConnectReply(int nInspector)
{
	CString	strSendCmd;
	strSendCmd.Format("CONNECT,REQUEST");
	Send_Command(nInspector, strSendCmd);
}

void CUdpManager::Set_StatusReply(int nInspector)
{
	CString	strSendCmd;
	strSendCmd.Format("STATUS,REPLY,1");
	Send_Command(nInspector, strSendCmd);
}

void CUdpManager::Set_LotReady(int nInspector, CString sLotID)
{
	CString	strSendCmd;
	strSendCmd.Format("LOT,READY,%s,1,1928,2059,MEM", sLotID);
	Send_Command(nInspector, strSendCmd);
}



void CUdpManager::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);
	switch (nIDEvent)
	{
	case INSPECTOR_PC1:	m_nStatusPC1 = 0; break;
	case INSPECTOR_PC2:	m_nStatusPC2 = 0; break;
	case INSPECTOR_PC3:	m_nStatusPC3 = 0; break;
	case INSPECTOR_PC4:	m_nStatusPC4 = 0; break;
	}
	CWnd::OnTimer(nIDEvent);
}