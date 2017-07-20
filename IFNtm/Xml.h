#pragma once
#import "msxml4.dll"
#include <memory>


#define SAFE_RELEASE(x)	\
	if( x != NULL )		\
	{					\
	x.Release();	\
	x = NULL;		\
	}

//-------------------------------------------------------------------------
// Classes Name	    :CXml CXmlNode CXmlNodes
// Author			:csdn[我就是传说中的。。。。SB]	sky-sea-earth@21cn.com 
// CreateTime       :2005-10-07
// Memo             :Xml结点类库
// Version          :v1.0
//-------------------------------------------------------------------------

class CXmlNode;
typedef std::auto_ptr<CXmlNode>  CXmlNodePtr;

class CXmlNodes;
typedef std::auto_ptr<CXmlNodes>  CXmlNodesPtr;

class CXmlNode
{
	friend class CXml;
	friend class CXmlNode;
	friend class CXmlNodes;


public:	
	~CXmlNode(void);

protected:

	MSXML2::IXMLDOMNodePtr   m_pNode;

	CXmlNode( MSXML2::IXMLDOMNodePtr pNode);	
	BOOL _GetValue(CString & strValue);
	BOOL _SetValue(CString & strValue);

public:

	//		_________________
	//______|	构造函数	|___________________________________________________________________
	CXmlNode(void);
	CXmlNode( CXmlNode & refNode IN);


	//		_________________
	//______|	重载 = 号	|___________________________________________________________________
	CXmlNodePtr operator = ( CXmlNodePtr pNode);
	CXmlNode & operator = ( CXmlNode & refNode);



	//		_________________
	//______|  获取结点名称	|___________________________________________________________________
	CString GetName(void);



	//		_________________
	//______|	属性操作	|___________________________________________________________________
	CString GetAttribute( CString strName, LPCTSTR lpszDefault = NULL);
	BOOL SetAttribute(  CString strName, CString strValue);
	BOOL DelAttribute( CString strName );



	//		_________________
	//______|	子结点操作	|___________________________________________________________________
	CXmlNodePtr GetChild(CString strName);
	CXmlNodesPtr GetChildren();
	BOOL HasChildren(void);
	BOOL RemoveChildren(void);



	//		_________________
	//______|	删除结点	|___________________________________________________________________
	BOOL Remove(void);



	//		_________________
	//______|	取值操作	|___________________________________________________________________	
	CString GetString( LPCTSTR lpszDefault = NULL );
	BOOL GetBOOL( BOOL bDefault = FALSE );
	INT GetInt( INT nDefault = 0 );
	DOUBLE GetFloat( DOUBLE fDefault = 0.0f );



	//		_________________
	//______|	赋值操作	|___________________________________________________________________
	BOOL SetString( LPCTSTR lpszValue );
	BOOL SetBOOL( BOOL bValue );
	BOOL SetInt( INT nValue );
	BOOL SetFloat( FLOAT fValue );

	BOOL IsNull(void);
};

class CXmlNodes
{
	friend class CXml;
	friend class CXmlNode;
	friend class CXmlNodes;

public:
	~CXmlNodes(void);
	CXmlNodes(void);
	CXmlNodes( CXmlNodes & refNodes );

	CXmlNodesPtr operator = (CXmlNodesPtr pNodes);
	CXmlNodes & operator = (CXmlNodes & refNodes);
	CXmlNodePtr operator[] ( LONG lIndex );
	CXmlNodePtr operator[] ( LPCTSTR lpszName );


	LONG GetCount(void);

	CXmlNodePtr GetItem( LONG nIndex );
	CXmlNodePtr GetItem( LPCTSTR lpszName );

protected:
	CXmlNodes(MSXML2::IXMLDOMNodeListPtr pNodeList);
	MSXML2::IXMLDOMNodeListPtr m_pNodeList;

};

class CXml
{
public:
	CXml(void);
	~CXml(void);

protected:
	MSXML2::IXMLDOMDocument2Ptr m_pDoc; 
	CString						m_strFilePath;

public:
	BOOL Open(CString strXmlFilePath);
	BOOL Save(LPCTSTR lpszFilePath = NULL);
	void Close(void);

	CString GetLastError(void);

	CXmlNodePtr GetRoot(void);		


	static BOOL IsFileExist(CString strFilePath);
	CXmlNodePtr SelectSingleNode(LPCTSTR lpszPath);
	CXmlNodesPtr SelectNodes(LPCTSTR lpszPath);
};




