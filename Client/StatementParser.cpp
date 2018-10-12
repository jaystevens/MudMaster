#include "stdafx.h"
#include ".\statementparser.h"
#include "Sess.h"
#include "ParseUtils.h"


CStatementParser::CStatementParser( CSession *pSession )
: _pSession( pSession )
{
}

CStatementParser::~CStatementParser(void)
{
}

bool CStatementParser::GetDBConnection(
									   CString &strIn,
									   ODBC::ODBCConnection &dbConn)
{

	CString strConnectString,strDBUser,strPassword, strDBtype;

	CParseUtils::FindStatement(_pSession,strIn, strConnectString);
	CParseUtils::FindStatement(_pSession,strIn, strDBUser);
	if(strDBUser.IsEmpty())
		strDBUser = "admin";
	CParseUtils::FindStatement(_pSession,strIn, strPassword);



	CParseUtils::ReplaceVariables(_pSession, strConnectString);
	CParseUtils::ReplaceVariables(_pSession, strDBUser);
	CParseUtils::ReplaceVariables(_pSession, strPassword);

	if(strConnectString.IsEmpty())
	{
		strConnectString ="@sessionpath()mm2k6Arrays.mdb"; 
		CParseUtils::ReplaceVariables(_pSession, strConnectString);
		//strConnectString ="C:\\mm2k6\\MudMaster\\builds\\debug\\db1.mdb";
		ODBC::MDBConnection m_DB;
		m_DB.Connect(strConnectString,strDBUser,strPassword,FALSE);
		dbConn.setHDBC(m_DB.getHDBC());
		dbConn.setHENV(m_DB.getHENV());
	}
	else
	{
		if(strConnectString.Find(":") == 3)
		{
			strDBtype = strConnectString.Left(3);
			strConnectString = strConnectString.Mid(4);
			if(strDBtype == "mdb")
			{
				ODBC::MDBConnection m_DB;
				m_DB.Connect(strConnectString,strDBUser,strPassword,FALSE);
				dbConn.setHDBC(m_DB.getHDBC());
			}
			if(strDBtype == "txt")
			{
				ODBC::TXTConnection m_DB;
				m_DB.Connect(strConnectString);
				dbConn.setHDBC(m_DB.getHDBC());
			}
			if(strDBtype == "mys")
			{
				ODBC::mySQLConnection  m_DB;
				CString strHost;
				int iPort, iIndex;
				iIndex = strConnectString.Find("Host=");
				if(iIndex != -1)
				{
					strHost = strConnectString.Mid(iIndex +5,strConnectString.Find(",") - (iIndex +4));
				}
				iPort=3306;
				iIndex = strConnectString.Find("Port=");
				if(iIndex != -1)
				{
					iPort = atoi(strConnectString.Mid(iIndex +5));
				}
				m_DB.Connect(strDBUser,strPassword,strHost,iPort);
				dbConn.setHDBC(m_DB.getHDBC());
			}
			if(strDBtype == "my5")
			{
				//for MySql 5.1 the fully formated connection string is passed straight through
				ODBC::mySQLConnection  m_DB;
				m_DB.ConnectDB5(strConnectString);
				dbConn.setHDBC(m_DB.getHDBC());
			}
			if(strDBtype == "xls")
			{
				ODBC::XLSConnection  m_DB;
				m_DB.Connect(strConnectString,strDBUser);
				dbConn.setHDBC(m_DB.getHDBC());
			}
			if(strDBtype == "mss")
			{
				CString strHost;
				int  iIndex;
				iIndex = strConnectString.Find("Host=") + 5;
				if(iIndex != -1)
				{
					strHost = strConnectString.Mid(iIndex);
				}

				ODBC::MSSQLConnection  m_DB;
				m_DB.Connect(strDBUser,strPassword,strHost);
				dbConn.setHDBC(m_DB.getHDBC());
			}
			if(strDBtype == "dsn")
			{
					ODBC::DSNConnection m_DB;
					m_DB.Connect(strConnectString,strDBUser,strPassword);
					dbConn.setHDBC(m_DB.getHDBC());
			}
			
		}
		else
		{
			ODBC::MDBConnection m_DB;
			m_DB.Connect(strConnectString,strDBUser,strPassword,FALSE);
			dbConn.setHDBC(m_DB.getHDBC());
		}
	}


	return true;

}

bool CStatementParser::GetOneStatement(
	CString &strIn, 
	CString &out, 
	bool preTrans)
{
	if( !CParseUtils::FindStatement( _pSession, strIn, out ) )
		return false;

	if(preTrans)
		CParseUtils::PretranslateString( _pSession, out );
	else
		CParseUtils::ReplaceVariables( _pSession, out );

	return true;
}

bool CStatementParser::GetTwoStatements(
	CString &strIn, 
	CString &out1,
	bool preTrans1,
	CString &out2, 
	bool preTrans2 )
{
	if( !CParseUtils::FindStatement( _pSession, strIn, out1 ) )
		return false;

	if( !CParseUtils::FindStatement( _pSession, strIn, out2 ) )
		return false;

	if(preTrans1)
		CParseUtils::PretranslateString( _pSession, out1 );
	else
		CParseUtils::ReplaceVariables( _pSession, out1 );

	if(preTrans2)
		CParseUtils::PretranslateString( _pSession, out2 );
	else
		CParseUtils::ReplaceVariables( _pSession, out2 );

	return true;
}

bool CStatementParser::GetThreeStatements(
	CString &strIn, 
	CString &out1,
	bool preTrans1,
	CString &out2, 
	bool preTrans2,
	CString &out3,
	bool preTrans3 )
{
	if( !CParseUtils::FindStatement( _pSession, strIn, out1 ) )
		return false;

	if( !CParseUtils::FindStatement( _pSession, strIn, out2 ) )
		return false;

	if( !CParseUtils::FindStatement( _pSession, strIn, out3 ) )
		return false;

	if(preTrans1)
		CParseUtils::PretranslateString( _pSession, out1 );
	else
		CParseUtils::ReplaceVariables( _pSession, out1 );

	if(preTrans2)
		CParseUtils::PretranslateString( _pSession, out2 );
	else
		CParseUtils::ReplaceVariables( _pSession, out2 );

	if(preTrans3)
		CParseUtils::PretranslateString( _pSession, out3 );
	else
		CParseUtils::ReplaceVariables( _pSession, out3 );

	return true;
}