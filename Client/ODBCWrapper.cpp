#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace ODBC
{
 #include <sql.h> 
 #include <sqlext.h>
 #include <odbcinst.h>
 //--
 #pragma comment(lib,"odbc32.lib")
 #pragma comment(lib,"odbcbcp.lib")
 #pragma comment(lib,"OdbcCP32.Lib")
 //--
 #define IS_SQL_ERR !IS_SQL_OK
 #define IS_SQL_OK(res) (res==SQL_SUCCESS_WITH_INFO || res==SQL_SUCCESS)
 #define SAFE_STR(str) ((str==NULL) ? _T("") : str)
 //--
 enum sqlDataTypes
 {
  sqlDataTypeUnknown=SQL_UNKNOWN_TYPE,
  sqlDataTypeChar=SQL_CHAR,
  sqlDataTypeNumeric=SQL_NUMERIC,
  sqlDataTypeDecimal=SQL_DECIMAL,
  sqlDataTypeInteger=SQL_INTEGER,
  sqlDataTypeSmallInt=SQL_SMALLINT,
  sqlDataTypeFloat=SQL_FLOAT,
  sqlDataTypeReal=SQL_REAL,
  sqlDataTypeDouble=SQL_DOUBLE,
#if (ODBCVER >= 0x0300)
  sqlDataTypeDateTime=SQL_DATETIME,
#endif
  sqlDataTypeVarChar=SQL_VARCHAR
 };
 //--
 class DSNConnection
 {
 public:
  bool Connect( LPCTSTR svSource ,LPCTSTR szUsername, LPCTSTR szPassword)
  {
   int nConnect = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv );
   if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
    nConnect = SQLSetEnvAttr( m_hEnv, 
                 SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 );
    if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
     nConnect = SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDBC );
     if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
      SQLSetConnectOption( m_hDBC,SQL_LOGIN_TIMEOUT,5 );                
      nConnect=SQLConnect( m_hDBC,
         ( SQLTCHAR *)svSource,
         SQL_NTS, 
         ( SQLTCHAR *)szUsername,
         SQL_NTS,
         ( SQLTCHAR *)szPassword,
         SQL_NTS );
      if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO )
      {
       return 1;
      }
     }
    }
   }
   if( m_hDBC != NULL ) {
    m_nReturn = SQLDisconnect( m_hDBC );
    m_nReturn = SQLFreeHandle( SQL_HANDLE_DBC,  m_hDBC );
   }
   if( m_hEnv!=NULL )
    m_nReturn = SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
   m_hDBC              = NULL;
   m_hEnv              = NULL;
   m_nReturn           = SQL_ERROR;
   return 0;
  }
  DSNConnection()
  {
   m_hDBC              = NULL;
   m_hEnv              = NULL;
   m_nReturn           = SQL_ERROR;
  }
  virtual ~DSNConnection()
  {
   if( m_hDBC != NULL ) {
    m_nReturn = SQLFreeHandle( SQL_HANDLE_DBC,  m_hDBC );
   }
   if( m_hEnv!=NULL )
    m_nReturn = SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
  }
  void Disconnect()
  {
   if( m_hDBC != NULL )
   {
    m_nReturn = SQLDisconnect( m_hDBC );
    m_hDBC = NULL;
   }
  }
 public:
  operator HDBC()
  {
   return m_hDBC;
  }
 private:
  SQLRETURN       m_nReturn;      // Internal SQL Error code
  HENV            m_hEnv;         // Handle to environment
  HDBC            m_hDBC;         // Handle to database connection
 };
 //--
 class ODBCConnection
 {
 public:
  bool Connect(LPCTSTR svSource)
  {
   int nConnect = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv );
   if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
    nConnect = SQLSetEnvAttr( m_hEnv, 
                      SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 );
    if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
     nConnect = SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDBC );
     if( nConnect == SQL_SUCCESS || nConnect == SQL_SUCCESS_WITH_INFO ) {
      SQLSetConnectOption( m_hDBC,SQL_LOGIN_TIMEOUT,5 );                
      short shortResult = 0;
      SQLTCHAR szOutConnectString[ 1024 ];
      nConnect = SQLDriverConnect( m_hDBC,                // Connection Handle
          NULL,                           // Window Handle
          (SQLTCHAR*)svSource,  // InConnectionString
          _tcslen(svSource),             // StringLength1
          szOutConnectString,             // OutConnectionString
          sizeof( szOutConnectString ),   // Buffer length
          &shortResult,                   // StringLength2Ptr
          SQL_DRIVER_NOPROMPT );          // no User prompt
      return IS_SQL_OK(nConnect);
     }
    }
   }
   if( m_hDBC != NULL ) {
    m_nReturn = SQLDisconnect( m_hDBC );
    m_nReturn = SQLFreeHandle( SQL_HANDLE_DBC,  m_hDBC );
   }
   if( m_hEnv!=NULL )
    m_nReturn = SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
   m_hDBC              = NULL;
   m_hEnv              = NULL;
   m_nReturn           = SQL_ERROR;
   return( IS_SQL_OK(nConnect) );
  }
  ODBCConnection()
  {
   m_hDBC              = NULL;
   m_hEnv              = NULL;
   m_nReturn           = SQL_ERROR;
  }
  virtual ~ODBCConnection()
  {
   if( m_hDBC != NULL ) {
    m_nReturn = SQLFreeHandle( SQL_HANDLE_DBC,  m_hDBC );
   }
   if( m_hEnv!=NULL )
    m_nReturn = SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
  }
  void Disconnect()
  {
   if( m_hDBC != NULL )
   {
    m_nReturn = SQLDisconnect( m_hDBC );
    m_hDBC = NULL;
   }
  }
 public:
  operator HDBC()
  {
   return m_hDBC;
  }
 private:
  SQLRETURN       m_nReturn;      // Internal SQL Error code
  HENV            m_hEnv;         // Handle to environment
  HDBC            m_hDBC;         // Handle to database connection
 };
 //--
 class MSSQLConnection : public ODBCConnection
 {
 public:
  enum enumProtocols
  {
   protoNamedPipes,
   protoWinSock,
   protoIPX,
   protoBanyan,
   protoRPC
  };
 public:
  MSSQLConnection (){};
  virtual ~MSSQLConnection (){};
  BOOL Connect(LPCTSTR User=_T(""),LPCTSTR Pass=_T(""), 
         LPCTSTR Host=_T("(local)"),BOOL Trusted=1, 
         enumProtocols Proto=protoNamedPipes)
  {
   TCHAR str[512]=_T("");
   _stprintf(str,
     _T("Driver={SQL Server};Server=%s;Uid=%s;Pwd=%s;
     Trusted_Connection=%s;Network=%s;"),
    SAFE_STR(Host),SAFE_STR(User),SAFE_STR(Pass),
                   (Trusted ? _T("Yes") : _T("No")));
   switch(Proto)
   {
   case protoNamedPipes:
    _tcscat(str,_T("dbnmpntw"));
    break;
   case protoWinSock:
    _tcscat(str,_T("dbmssocn"));
    break;
   case protoIPX:
    _tcscat(str,_T("dbmsspxn"));
    break;
   case protoBanyan:
    _tcscat(str,_T("dbmsvinn"));
    break;
   case protoRPC:
    _tcscat(str,_T("dbmsrpcn"));
    break;
   default:
    _tcscat(str,_T("dbmssocn"));
    break;
   }
   _tcscat(str,_T(";"));
   return ODBCConnection::Connect(str);
  };
 };
 //--
 class DB2Connection : private ODBCConnection
 {
 public:
  DB2Connection(){};
  virtual ~DB2Connection(){};
  int Connect(LPCTSTR DBPath)
  {
   TCHAR str[512]=_T("");
   _stprintf(str,_T("Driver={Microsoft dBASE Driver 
          (*.dbf)};DriverID=277;Dbq=%s;"),SAFE_STR(DBPath));
   return ODBCConnection::Connect(str);
  };
 };
 //--
 class XLSConnection : private ODBCConnection
 {
 public:
  XLSConnection(){};
  virtual ~XLSConnection(){};
  int Connect(LPCTSTR XLSPath,LPCTSTR DefDir=_T(""))
  {
   TCHAR str[512]=_T("");
   _stprintf(str,_T("Driver={Microsoft Excel Driver 
         (*.xls)};DriverId=790;bq=%s;DefaultDir=%s;"),
         SAFE_STR(XLSPath),SAFE_STR(DefDir));
   return ODBCConnection::Connect(str);
  };
 };
 //--
 class TXTConnection : private ODBCConnection
 {
 public:
  TXTConnection(){};
  int Connect(LPCTSTR TXTPath)
  {
   TCHAR str[512]=_T("");
   _stprintf(str,_T("Driver={Microsoft Text Driver 
          (*.txt; *.csv)};Dbq=%s;Extensions=asc,csv,tab,txt;"),
          SAFE_STR(TXTPath));
   return ODBCConnection::Connect(str);
  };
 };
 //--
 class FOXConnection : public ODBCConnection
 {
 public:
  FOXConnection(){};
  virtual ~FOXConnection(){};
  int Connect(LPCTSTR DBPath,LPCTSTR User=_T(""),
          LPCTSTR Pass=_T(""),LPCTSTR Type=_T("DBF"),
          BOOL Exclusive=0)
  {
   TCHAR str[512]=_T("");
   _stprintf(str,_T("Driver={Microsoft Visual Foxpro Driver};
          Uid=%s;Pwd=%s;SourceDB=%s;SourceType=%s;Exclusive=%s;"),
          SAFE_STR(User),SAFE_STR(Pass),
          SAFE_STR(DBPath),
          SAFE_STR(Type),(Exclusive ? _T("yes") : _T("no")));
   return ODBCConnection::Connect(str);
  };
 };
 //--
 class MDBConnection : public ODBCConnection
 {
 public:
  MDBConnection(){};
  virtual ~MDBConnection(){};
  int Connect(LPCTSTR MDBPath,LPCTSTR User=_T(""),
            LPCTSTR Pass=_T(""),BOOL Exclusive=0)
  {
   TCHAR str[512]=_T("");
   _stprintf(
    _T("Driver={Microsoft Access Driver (*.mdb)};
               Dbq=%s;Uid=%s;Pwd=%s;Exclusive=%s;"),
               SAFE_STR(MDBPath),SAFE_STR(User),
               SAFE_STR(Pass),(Exclusive ? _T("yes") : _T("no")));
   return ODBCConnection::Connect(str);
  };
 };
 //--
 class mySQLConnection : public ODBCConnection
 {
 public:
  mySQLConnection(){};
  virtual ~mySQLConnection(){};
  int Connect(LPCTSTR User=_T(""),LPCTSTR Pass=_T(""),
         LPCTSTR Host=_T("localhost"),
         UINT Port=3306,UINT Option=0)
  {
   TCHAR str[512]=_T("");
   _stprintf(str,_T("Driver={MySQL ODBC 3.51 Driver};
         Uid=%s;Pwd=%s;Server=%s;Port=%d;"),
         SAFE_STR(User),SAFE_STR(Pass),
         SAFE_STR(Host),Port);
   return ODBCConnection::Connect(str);
  };
  int ConnectDB(LPCTSTR DB,LPCTSTR User=_T(""),
        LPCTSTR Pass=_T(""),LPCTSTR Host=_T("localhost"),
        UINT Port=3306,UINT Option=0)
  {
   TCHAR str[512]=_T("");
   _stprintf(str,_T("Driver={MySQL ODBC 3.51 Driver};
        Database=%s;Uid=%s;Pwd=%s;Server=%s;Port=%d;"),
        SAFE_STR(DB),SAFE_STR(User),SAFE_STR(Pass),
        SAFE_STR(Host),Port);
   return ODBCConnection::Connect(str);
  };
 };
 //--
 class ODBCStmt
 {
  HSTMT m_hStmt;
 public:
  operator HSTMT()
  {
   return m_hStmt;
  }
  ODBCStmt(HDBC hDBCLink)
  {
   SQLRETURN m_nReturn;
   m_nReturn = SQLAllocHandle( SQL_HANDLE_STMT, hDBCLink, &m_hStmt );
   SQLSetStmtAttr(m_hStmt, SQL_ATTR_CONCURRENCY, 
                       (SQLPOINTER) SQL_CONCUR_ROWVER, 0);
   SQLSetStmtAttr(m_hStmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER) 
                             SQL_CURSOR_KEYSET_DRIVEN, 0);
   if(!IS_SQL_OK(m_nReturn))
    m_hStmt=INVALID_HANDLE_VALUE;
  }
  virtual ~ODBCStmt()
  {
   if(m_hStmt!=INVALID_HANDLE_VALUE)
    SQLFreeHandle(SQL_HANDLE_STMT,m_hStmt);
  }
  BOOL IsValid()
  {
    return m_hStmt!=INVALID_HANDLE_VALUE;
  }
  USHORT GetColumnCount()
  {
   short nCols=0;
   if(!IS_SQL_OK(SQLNumResultCols(m_hStmt,&nCols)))
    return 0;
   return nCols;
  }
  DWORD GetChangedRowCount(void)
  {
   long nRows=0;
   if(!IS_SQL_OK(SQLRowCount(m_hStmt,&nRows)))
    return 0;
   return nRows;
  }
  BOOL Query( LPCTSTR strSQL)
  {
   SQLRETURN nRet=SQLExecDirect( m_hStmt, (SQLTCHAR *)strSQL, SQL_NTS );
   return IS_SQL_OK( nRet );
  }
  BOOL Fetch()
  {
   SQLRETURN nRet=SQLFetch(m_hStmt);
   return IS_SQL_OK( nRet );
  }
  BOOL FecthRow(UINT nRow)
  {
   return IS_SQL_OK(SQLSetPos(m_hStmt, nRow, SQL_POSITION, SQL_LOCK_NO_CHANGE));
  }
  BOOL FetchPrevious()
  {
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_PRIOR,0);
   return IS_SQL_OK(nRet);
  }
  BOOL FecthNext()
  {
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_NEXT,0);
   return IS_SQL_OK(nRet);
  }
  BOOL FetchRow(ULONG nRow,BOOL Absolute=1)
  {
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,
      (Absolute ? SQL_FETCH_ABSOLUTE : SQL_FETCH_RELATIVE),nRow);
   return IS_SQL_OK(nRet);
  }
  BOOL FetchFirst()
  {
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_FIRST,0);
   return IS_SQL_OK(nRet);
  }
  BOOL FetchLast()
  {
   SQLRETURN nRet=SQLFetchScroll(m_hStmt,SQL_FETCH_LAST,0);
   return IS_SQL_OK(nRet);
  }
  BOOL Cancel()
  {
   SQLRETURN nRet=SQLCancel(m_hStmt);
   return IS_SQL_OK(nRet);
  }
 };
 //--
 class ODBCRecord
 {
  HSTMT m_hStmt;
 public:
  ODBCRecord(HSTMT hStmt){m_hStmt=hStmt;};
  ~ODBCRecord(){};
  USHORT GetColumnCount()
  {
   short nCols=0;
   if(!IS_SQL_OK(SQLNumResultCols(m_hStmt,&nCols)))
    return 0;
   return nCols;
  }
  BOOL BindColumn(USHORT Column,LPVOID pBuffer,
        ULONG pBufferSize,LONG * pReturnedBufferSize=NULL,
        USHORT nType=SQL_C_TCHAR)
  {
   LONG pReturnedSize=0;
   SQLRETURN Ret=SQLBindCol(m_hStmt,Column,nType,
               pBuffer,pBufferSize,&pReturnedSize);
   if(*pReturnedBufferSize)
    *pReturnedBufferSize=pReturnedSize;
   return IS_SQL_OK(Ret);
  }
  USHORT GetColumnByName(LPCTSTR Column)
  {
   SHORT nCols=GetColumnCount();
   for(USHORT i=1;i<(nCols+1);i++)
   {
    TCHAR Name[256]=_T("");
    GetColumnName(i,Name,sizeof(Name));
    if(!_tcsicmp(Name,Column))
     return i;
   }
   return 0;
  }
  BOOL GetData(USHORT Column, LPVOID pBuffer, 
    ULONG pBufLen, LONG * dataLen=NULL, int Type=SQL_C_DEFAULT)
  {
   SQLINTEGER od=0;
   int Err=SQLGetData(m_hStmt,Column,Type,pBuffer,pBufLen,&od);
   if(IS_SQL_ERR(Err))
   { 
    return 0;
   } 
   if(dataLen)
    *dataLen=od;
   return 1;
  }
  int GetColumnType( USHORT Column )
  {
   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   UDWORD pcbColDef;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   nType=(int)swType;
   return( nType );
  }
  DWORD GetColumnSize( USHORT Column )
  {
   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   DWORD pcbColDef=0;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   return pcbColDef;
  }
  DWORD GetColumnScale( USHORT Column )
  {
   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   DWORD pcbColDef=0;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   return swScale;
  }
  BOOL GetColumnName( USHORT Column, LPTSTR Name, SHORT NameLen )
  {
   int nType=SQL_C_DEFAULT;
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   DWORD pcbColDef=0;
   SQLRETURN Ret=
    SQLDescribeCol( m_hStmt,            // Statement handle
       Column,               // ColumnNumber
       (SQLTCHAR*)Name,     // ColumnName
       NameLen,    // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   if(IS_SQL_ERR(Ret))
    return 0;
   return 1;
  }
  BOOL IsColumnNullable( USHORT Column )
  {
   int nType=SQL_C_DEFAULT;
   SQLTCHAR svColName[ 256 ]=_T("");
   SWORD swCol=0,swType=0,swScale=0,swNull=0;
   UDWORD pcbColDef;
   SQLDescribeCol( m_hStmt,            // Statement handle
       Column,             // ColumnNumber
       svColName,          // ColumnName
       sizeof( svColName), // BufferLength
       &swCol,             // NameLengthPtr
       &swType,            // DataTypePtr
       &pcbColDef,         // ColumnSizePtr
       &swScale,           // DecimalDigitsPtr
       &swNull );          // NullablePtr
   return (swNull==SQL_NULLABLE);
  }
 };
};