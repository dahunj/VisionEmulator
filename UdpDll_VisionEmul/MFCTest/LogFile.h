// LogFile.h : 헤더 파일
//
#pragma once

class CLogFile  
{
public:
	CLogFile();
	virtual ~CLogFile();

private:
	CString sLogLotID[30];
	int		nLotSeqNo[30];
	CString m_sPCID;
	CString m_sMachineCode;

public:
	void Create_Folder(CString sPath);
		
	void Save_InspectorLog(CString sLog);
	

	void Set_ID(CString sPCID, CString sMCode) { m_sPCID = sPCID; m_sMachineCode = sMCode; }
};

extern CLogFile g_objLogFile;

///////////////////////////////////////////////////////////////////////////////
