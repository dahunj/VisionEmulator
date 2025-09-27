#pragma once
#include <afxwin.h>
#include <afxmt.h>
#include <afxsock.h>

// 내보내기/가져오기 매크로
#ifdef UDPSOCKETDLL_EXPORTS
#define UDPSOCKET_API __declspec(dllexport)
#else
#define UDPSOCKET_API __declspec(dllimport)
#endif
// 수신 알림 메시지 (호스트 앱이 이 메시지를 받아 처리)
#ifndef UM_UDP_RECEIVE
#define UM_UDP_RECEIVE   (WM_USER + 9004)
#endif
// 간단한 상태/오류 코드
enum class UdpSockErr {
    Ok = 0,
    NotInited,
    AlreadyOpen,
    CreateFail,
    BindFail,
    Other
};
class UDPSOCKET_API CUdpSocketCS : public CAsyncSocket
{
public:
    CUdpSocketCS();
    virtual ~CUdpSocketCS();
    // 초기화(로컬 포트 바인드). parent는 알림 받을 대상창
    UdpSockErr Open(UINT nLocalPort, CWnd* pParent, UINT notifyMsg = UM_UDP_RECEIVE);
    void Close();
    // 송신 (상대 IP/Port로 데이터 전송)
    int SendToHost(LPCTSTR hostIp, UINT hostPort, const BYTE* pData, int len);
    // 수신 버퍼 읽기 (OnReceive 발생 후, 외부에서 호출)
        // 반환: 수신된 바이트 수 (음수면 오류)
    int Read(BYTE* pOut, int outLen, CString& fromIp, UINT& fromPort);
    // 상태 확인용
    BOOL IsOpen() const { return m_bOpen; }
    UINT LocalPort() const { return m_nLocalPort; }

    BOOL Open_Socket(UINT nLocalPort, UINT nHostPort, CString strHostIp, CWnd* pParent);
    void Close_Socket();
   
    BOOL Write_Socket(BYTE* pBuffer, int nLength);
protected:
    // CAsyncSocket 콜백
    virtual void OnReceive(int nErrorCode) override;
    virtual void OnClose(int nErrorCode) override;
private:
    BOOL    m_bOpen = FALSE;
    UINT    m_nLocalPort = 0;
    UINT	m_nHostPort  = 0;	// Host Port
    CString	m_strHostIp;	    // Host IP

    CWnd* m_pParent = nullptr;
    UINT    m_uNotifyMsg = UM_UDP_RECEIVE;
    // 마지막 수신 패킷 저장소
    CByteArray m_recvBuf;
    CString    m_fromIp;
    UINT       m_fromPort = 0;
    CCriticalSection m_cs; // 간단 동기화
};
// DLL 전역 초기화/해제 함수 (선택적으로 외부에서 호출 가능)
extern "C" {
    UDPSOCKET_API BOOL __stdcall UdpSockets_IsInitialized();
    UDPSOCKET_API BOOL __stdcall UdpSockets_EnsureInit();   // AfxSocketInit 강제 수행
}
