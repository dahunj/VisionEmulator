// LogFile.cpp: implementation of the CLogFile class.
//
#include "pch.h"
#include "LogFile.h"


// CLogFile
CLogFile g_objLogFile;

CCriticalSection g_csInspectorLog;


CLogFile::CLogFile()
{
}

CLogFile::~CLogFile()
{
}

///////////////////////////////////////////////////////////////////////////////

void CLogFile::Create_Folder(CString sPath)
{
	if (sPath == _T("")) return;
	if (sPath.Right(1) == _T("\\")) sPath = sPath.Left(sPath.GetLength() - 1);
	if (GetFileAttributes(sPath) != -1) return;	// Directory Exist!!!

	int nFound = sPath.ReverseFind('\\');
	Create_Folder(sPath.Left(nFound));

	CreateDirectory(sPath, NULL);
}


void CLogFile::Save_InspectorLog(CString sLog)
{
	g_csInspectorLog.Lock();

	SYSTEMTIME time;
	GetLocalTime(&time);

	CString strPath, strFile, strSave;
	strPath.Format("%s\\LOG\\Inspector\\%04d\\%02d\\%02d", gsCurrentDir, time.wYear, time.wMonth, time.wDay);
	Create_Folder(strPath);

	strFile.Format("%s\\%04d%02d%02d-%02d.csv", strPath, time.wYear, time.wMonth, time.wDay, time.wHour);

	CFile file;
	if (file.Open(strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyNone)) {
		try {
			file.SeekToEnd();

			strSave.Format("[%02d:%02d:%02d %03d], %s\r\n", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, sLog);

			file.Write(strSave, strSave.GetLength());
			file.Close();

		}
		catch (CFileException* pEx) {
			pEx->Delete();
		}
	}
	g_csInspectorLog.Unlock();
}


///////////////////////////////////////////////////////////////////////////////
