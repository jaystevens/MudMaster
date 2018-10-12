#pragma once

class CSession;
class CReplaceVariables
{
public:
	CReplaceVariables( 
		CSession *pSession,
		CString &line,
		bool quoteStrings );

	~CReplaceVariables(void);

	void execute();

private:
	enum ParseState
	{
		ParseStart,
		ParseEscape,
		ParseVar,
		ParseVarName,
		ParseProc,
		ParseProcParams,
		ParseProcEscape,
	};

	void DoParseStart( ParseState &state );
	void DoParseEscape( ParseState &state );
	void DoParseVar( ParseState &state );
	void DoGlobalVar();
	void DoParseVarName( ParseState &state );
	void DoHandleVarName();
	void ProcessVar();
	void QuoteVar( const std::string &var );
	void DoParseProc( ParseState &state );
	void DoParseProcParams( ParseState &state );
	void DoParseProcEscape( ParseState &state );
	void ProcessProc();

	CSession *_pSession;
	CString &_line;
	bool _quoteStrings;

	bool _foundVar;
	bool _foundProc;
	bool _foundProcParen;
	int _procParenCount;
	std::string _text;
	std::string _varName;
	std::string _procName;
	std::string _temp;
	int _index;
	int _length;
};
