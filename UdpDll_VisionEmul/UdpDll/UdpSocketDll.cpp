#include "pch.h"
#include "UdpSocketDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// ====== 내부 상태 ======
static volatile BOOL g_bSockInited = FALSE;
// ====== 내부 유틸 ======
static BOOL EnsureSockets()
{
    if (g_bSockInited)
        return TRUE;
    WSADATA wsa = {};
    if (!AfxSocketInit(&wsa)) {
        return FALSE;
    }
    g_bSockInited = TRUE;
    return TRUE;
}
// ====== Export C APIs ======
extern "C" UDPSOCKET_API BOOL __stdcall UdpSockets_IsInitialized()
{
    return g_bSockInited;
}
extern "C" UDPSOCKET_API BOOL __stdcall UdpSockets_EnsureInit()
{
    return EnsureSockets();
}
// ====== CUdpSocketCS ======
CUdpSocketCS::CUdpSocketCS()
{
}
CUdpSocketCS::~CUdpSocketCS()
{
    Close();
}
UdpSockErr CUdpSocketCS::Open(UINT nLocalPort, CWnd* pParent, UINT notifyMsg)
{
    if (!EnsureSockets())
        return UdpSockErr::NotInited;
    if (m_bOpen)
        return UdpSockErr::AlreadyOpen;
    m_pParent = pParent;
    m_uNotifyMsg = notifyMsg;
    m_nLocalPort = nLocalPort;
    if (!Create(nLocalPort, SOCK_DGRAM, FD_READ | FD_CLOSE)) {
        return UdpSockErr::CreateFail;
    }
    // UDP는 Bind가 Create에서 이뤄짐. 추가 옵션 필요 시 SetSockOpt 사용.
    m_bOpen = TRUE;
    return UdpSockErr::Ok;
}
void CUdpSocketCS::Close()
{
    if (m_bOpen) {
        try { ShutDown(); }
        catch (...) {}
        try { Close(); }
        catch (...) {}
        m_bOpen = FALSE;
    }
}

BOOL CUdpSocketCS::Open_Socket(UINT nLocalPort, UINT nHostPort, CString strHostIp, CWnd* pParent = NULL)
{
    m_nHostPort = nHostPort;
    m_strHostIp = strHostIp;
    if (Open(nLocalPort, pParent, UM_UDP_RECEIVE) != UdpSockErr::Ok) {
    	AfxMessageBox("UDP 소켓 열기 실패");
        return FALSE;
    }
    return TRUE;
}

void CUdpSocketCS::Close_Socket()
{
    Close();
}

BOOL CUdpSocketCS::Write_Socket(BYTE* pBuffer, int nLength)
{
    if (SendToHost(m_strHostIp, m_nHostPort, pBuffer, nLength) == SOCKET_ERROR)
    {
        int nError = GetLastError();
        return FALSE;
    }
    return TRUE;
}

int CUdpSocketCS::SendToHost(LPCTSTR hostIp, UINT hostPort, const BYTE* pData, int len)
{
    if (!m_bOpen || pData == nullptr || len <= 0)
        return -1;
    return CAsyncSocket::SendTo((LPVOID)pData, len, hostPort, hostIp);
}
int CUdpSocketCS::Read(BYTE* pOut, int outLen, CString& fromIp, UINT& fromPort)
{
    CSingleLock lock(&m_cs, TRUE);
    if (m_recvBuf.GetSize() <= 0 || pOut == nullptr || outLen <= 0)
        return 0;
    const INT sizeToCopy = (INT)min((INT)m_recvBuf.GetSize(), outLen);
    memcpy(pOut, m_recvBuf.GetData(), sizeToCopy);
    fromIp = m_fromIp;
    fromPort = m_fromPort;
    m_recvBuf.RemoveAll(); // 소비
    return sizeToCopy;
}
void CUdpSocketCS::OnReceive(int nErrorCode)
{
    if (nErrorCode != 0) {
        CAsyncSocket::OnReceive(nErrorCode);
        return;
    }
    BYTE buf[2048];
    int flags = 0;
    CString ip;
    UINT port = 0;
    int ret = ReceiveFrom(buf, sizeof(buf), ip, port, flags);
    if (ret > 0) {
        CSingleLock lock(&m_cs, TRUE);
        m_recvBuf.RemoveAll();
        m_recvBuf.SetSize(ret);
        memcpy(m_recvBuf.GetData(), buf, ret);
        m_fromIp = ip;
        m_fromPort = port;
    }
    // 호스트 앱으로 알림 보내기
    if (m_pParent && ::IsWindow(m_pParent->GetSafeHwnd())) {
        m_pParent->PostMessage(m_uNotifyMsg, (WPARAM)ret, (LPARAM)this);
    }
    CAsyncSocket::OnReceive(nErrorCode);
}
void CUdpSocketCS::OnClose(int nErrorCode)
{
    // 필요시 상태 갱신/알림
    CAsyncSocket::OnClose(nErrorCode);
}
