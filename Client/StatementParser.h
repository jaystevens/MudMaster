#pragma once
#include "Sess.h"

class CSession;

class CStatementParser
{
public:
	CStatementParser( CSession *pSession );
	~CStatementParser(void);

	bool GetOneStatement(
		CString &strIn,
		CString &out,
		bool preTrans);
	
	bool GetDBConnection(
		CString &strIn,
		ODBC::ODBCConnection &dbConn);

	bool GetTwoStatements(
		CString &strIn, 
		CString &strOut1,
		bool preTrans1,
		CString &strOut2,
		bool preTrans2 );

	bool GetThreeStatements( 
		CString &strIn, 
		CString &strOut1,
		bool preTrans1,
		CString &strOut2,
		bool preTrans2,
		CString &strOut3,
		bool preTrans3 );

private:
	CSession *_pSession;
};
