#pragma once

#include "../EventArgs.h"

class OpenLogEventArgs
	: public EventArgs
{
	LPCTSTR _logFilename;
	bool _append;

public:
	OpenLogEventArgs(LPCTSTR szFilename, bool append)
		: _logFilename(szFilename)
		, _append(append)
	{
	}

	bool GetAppend() { return _append; }
	LPCTSTR GetFileName() { return _logFilename; }
};