#pragma once

class CLogFile
{
public:
	static CLogFile &Instance();

	HRESULT Open(LPCTSTR pszFilename, bool append);
	HRESULT Log(LPCTSTR pszText);
	HRESULT Close();

	bool Timestamp() const			{ return _timestamp;}
	void Timestamp(bool timestamp)	{ _timestamp = timestamp; }

	bool IsLogging() const	{ return _logging; }

private:
	bool _timestamp;
	bool _logging;
	int  _bytesWritten;
	CAtlFile _file;

	CLogFile(void);
	~CLogFile(void);
	CLogFile(const CLogFile &/*src*/){}
	CLogFile &operator=(const CLogFile &/*src*/);
};
