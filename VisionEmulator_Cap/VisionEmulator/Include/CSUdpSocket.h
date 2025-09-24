// UdpSocketCS.h : 헤더 파일
//
#pragma once

#include <afxsock.h>		// MFC socket extensions

#define UM_UDP_RECEIVE	WM_USER+9004

class CUdpSocketCS : public CAsyncSocket
{
public:
	CUdpSocketCS();
	virtual ~CUdpSocketCS();
	virtual void OnReceive(int nErrorCode);

private:
	UINT	m_nLocalPort;	// Local Port
	UINT	m_nHostPort;	// Host Port
	CString	m_strHostIp;	// Host IP
	UINT	m_nRecvPort;	// Receive Port
	CString m_strRecvIp;	// Receive IP

	CWnd*	m_pParent;
	int		m_nReadLen;
	BYTE*	m_pRecvBuff;

public:
	BOOL Open_Socket(UINT nLocalPort, UINT nHostPort, CString strHostIp, CWnd* pParent = NULL);
	void Close_Socket();

	int  Read_Socket(BYTE* pBuffer);
	BOOL Write_Socket(BYTE* pBuffer, int nLength);

	UINT	Get_HostPort() { return m_nHostPort; }
	CString	Get_HostIp() { return m_strHostIp; }
	UINT	Get_RecvPort() { return m_nRecvPort; }
	CString	Get_RecvIp() { return m_strRecvIp; }
	void    Reset_Host(UINT nPort, CString sIp) { m_nHostPort = nPort; m_strHostIp = sIp; }
};

///////////////////////////////////////////////////////////////////////////////
