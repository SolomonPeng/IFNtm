#include "StdAfx.h"
#include ".\xml.h"




CXml::CXml(void)
: m_strFilePath(_T(""))
, m_pDoc(NULL)
{
}

CXml::~CXml(void)
{
	Close();
}


//-------------------------------------------------------------------------
// Function Name    :IsFileExist	[static]
// Parameter(s)     :CString strFilePath	�ļ�·��������
// Return           :BOOL
// Memo             :�ж��ļ��Ƿ����
//-------------------------------------------------------------------------
BOOL CXml::IsFileExist(CString strFilePath)
{
	BOOL bExist = FALSE;
	HANDLE hFile = NULL;

	hFile = CreateFile( strFilePath
		, GENERIC_READ
		, FILE_SHARE_READ | FILE_SHARE_WRITE
		, NULL
		, OPEN_EXISTING
		, 0
		, 0
		);

	if( hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
		bExist = TRUE;
	}

	return (bExist);
}


//-------------------------------------------------------------------------
// Function Name    :Open
// Parameter(s)     :CString strXmlFilePath	�ļ�·��������
// Return           :BOOL	�Ƿ�ɹ�
// Memo             :��XML�ļ� ����������򴴽�֮
//-------------------------------------------------------------------------
BOOL CXml::Open( CString strXmlFilePath )
{
	HRESULT hr = S_OK;

	hr = m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument40) );
	if( !SUCCEEDED(hr) )
	{
		// ��û��CoInitialize������
		ASSERT(FALSE);
		return FALSE;
	}

	ASSERT( m_pDoc != NULL );
	m_pDoc->preserveWhiteSpace = VARIANT_FALSE;
	m_pDoc->validateOnParse = VARIANT_FALSE;
	m_strFilePath = strXmlFilePath;

	if( IsFileExist(strXmlFilePath) )
	{
		VARIANT_BOOL vbSuccessful = VARIANT_FALSE;

		try
		{			
			vbSuccessful = m_pDoc->load( COleVariant((LPCSTR)strXmlFilePath) );
		}
		catch(...)
		{
			vbSuccessful = VARIANT_FALSE;
		}
		
		return  vbSuccessful == VARIANT_TRUE;
	}
	else
	{
		MSXML2::IXMLDOMProcessingInstructionPtr pPI = NULL;
		pPI = m_pDoc->createProcessingInstruction( _bstr_t(_T("xml")), _bstr_t(_T("version=\"1.0\"")) );
		ASSERT( pPI != NULL );

		hr = m_pDoc->appendChild(pPI);
		ASSERT( SUCCEEDED(hr) );

		return SUCCEEDED(hr);
	}
}


//-------------------------------------------------------------------------
// Function Name    :Close
// Parameter(s)     :void
// Return           :void
// Memo             :�ر�
//-------------------------------------------------------------------------
void CXml::Close(void)
{
	m_pDoc = NULL;
}



//-------------------------------------------------------------------------
// Function Name    :GetLastError
// Parameter(s)     :void
// Return           :CString
// Memo             :��ȡ���ĳ�����Ϣ
//-------------------------------------------------------------------------
CString CXml::GetLastError(void)
{
	ASSERT( m_pDoc != NULL );

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMParseErrorPtr pIParseError = NULL;

	hr = m_pDoc->get_parseError(&pIParseError);
	ASSERT( SUCCEEDED(hr) );

	CString strRet;
	if( pIParseError != NULL )
	{
		LONG value;
		BSTR bstr = NULL;
		hr = pIParseError->get_errorCode(&value);
		ASSERT( SUCCEEDED(hr) );

        hr = pIParseError->get_reason(&bstr);
		ASSERT( SUCCEEDED(hr) );

		strRet = (LPCSTR)(LPCTSTR)_bstr_t( bstr, true);

		if( bstr != NULL )
		{
			SysFreeString(bstr);
			bstr = NULL;
		}	

		SAFE_RELEASE(pIParseError);		
	}

	return strRet;
}



//-------------------------------------------------------------------------
// Function Name    :GetRoot
// Parameter(s)     :
// Return           :
// Memo             :��ȡ���ڵ�
//-------------------------------------------------------------------------
CXmlNodePtr CXml::GetRoot(void)
{	
	ASSERT( m_pDoc != NULL );

	MSXML2::IXMLDOMElementPtr pElement = NULL;
	HRESULT hr = m_pDoc->get_documentElement(&pElement);
	ASSERT( SUCCEEDED(hr) );

	// ������ �� ����
	if( pElement == NULL )
	{
		pElement = m_pDoc->createElement( _bstr_t(_T("xmlRoot")) );
		ASSERT( pElement != NULL );

		m_pDoc->appendChild(pElement);
	}

	CXmlNodePtr pNode( new CXmlNode(pElement) );

	SAFE_RELEASE(pElement);
	
	return pNode;
}

//-------------------------------------------------------------------------
// Function Name    :Save
// Parameter(s)     :LPCTSTR lpszFilePath	[in] �����λ��
// Return           :BOOL
// Memo             :����Xml�ļ�
//-------------------------------------------------------------------------
BOOL CXml::Save(LPCTSTR lpszFilePath /* = NULL */)
{
	ASSERT( m_pDoc != NULL );

	HRESULT hr = S_OK;
	if( lpszFilePath == NULL )
		hr = m_pDoc->save( _variant_t((char*)(LPCSTR)m_strFilePath) );
	else
		hr = m_pDoc->save( _variant_t( lpszFilePath ) );

	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :SelectSingleNode
// Parameter(s)     :LPCTSTR strPath  ·����
// Return           :CXmlNodePtr
// Memo             :ȡ�����ڵ�
//-------------------------------------------------------------------------
CXmlNodePtr CXml::SelectSingleNode(LPCTSTR lpszPath)
{
	ASSERT( m_pDoc != NULL );

	MSXML2::IXMLDOMNodePtr pItem = NULL;
	pItem = m_pDoc->selectSingleNode( _bstr_t(lpszPath) );

	CXmlNodePtr pNode ( new CXmlNode(pItem) );
	SAFE_RELEASE(pItem);
	
	return pNode;
}



//-------------------------------------------------------------------------
// Function Name    :SelectNodes
// Parameter(s)     :LPCTSTR strPath	·����
// Return           :CXmlNodesPtr
// Memo             :ȡ��㼯��
//-------------------------------------------------------------------------
CXmlNodesPtr CXml::SelectNodes(LPCTSTR lpszPath)
{
	ASSERT( m_pDoc != NULL );

	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	pNodeList =	m_pDoc->selectNodes( _bstr_t(lpszPath) );

	CXmlNodesPtr pNodes ( new CXmlNodes(pNodeList) );
	SAFE_RELEASE(pNodeList);

	return pNodes;
}

//-------------------------------------------------------------------------
// Function Name    :CXmlNode
// Parameter(s)     : MSXML2::IXMLDOMNodePtr pNode	[in]
// Return           :
// Memo             :������
//-------------------------------------------------------------------------
CXmlNode::CXmlNode( MSXML2::IXMLDOMNodePtr pNode IN)
{
	m_pNode = pNode;
}


//-------------------------------------------------------------------------
// Function Name    :CXmlNode
// Parameter(s)     :void
// Return           :
// Memo             :������
//-------------------------------------------------------------------------
CXmlNode::CXmlNode(void)
{
	m_pNode = NULL;	
}


//-------------------------------------------------------------------------
// Function Name    :CXmlNode
// Parameter(s)     :CXmlNode & refNode [in]
// Return           :
// Memo             :�������캯��
//-------------------------------------------------------------------------
CXmlNode::CXmlNode( CXmlNode & refNode IN)
{
	m_pNode = refNode.m_pNode;
}


//-------------------------------------------------------------------------
// Function Name    :~CXmlNode
// Parameter(s)     :void
// Return           :
// Memo             :��������
//-------------------------------------------------------------------------
CXmlNode::~CXmlNode(void)
{
	m_pNode = NULL;
}





//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNodePtr pNode	[in]
// Return           :CXmlNodePtr
// Memo             :���� = ������
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNode::operator = ( CXmlNodePtr pNode IN)
{
	m_pNode = pNode->m_pNode;
	return pNode;
}


//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNode & refNode	[in]
// Return           :CXmlNode &
// Memo             :���� = ������
//-------------------------------------------------------------------------
CXmlNode & CXmlNode::operator = ( CXmlNode & refNode IN)
{
	m_pNode = refNode.m_pNode;
	return (*this);
}


//-------------------------------------------------------------------------
// Function Name    :IsNull
// Parameter(s)     :
// Return           :BOOL
// Memo             :�жϴ˽���Ƿ��ǿսڵ�
//-------------------------------------------------------------------------
BOOL CXmlNode::IsNull(void)
{
	return m_pNode == NULL;
}




//-------------------------------------------------------------------------
// Function Name    :GetChild
// Parameter(s)     :CString strName	�ڵ�����
// Return           :���ӽڵ�ָ��
// Memo             :��ȡ����
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNode::GetChild(CString strName IN)
{
	ASSERT( m_pNode != NULL );

	MSXML2::IXMLDOMNodePtr pChildNode = NULL;
	pChildNode = m_pNode->selectSingleNode(_bstr_t((char*)(LPCSTR)strName));

	// ������ �� ����
	if( pChildNode == NULL )
	{
		MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
		HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
		ASSERT( SUCCEEDED(hr) );

		pChildNode = pDoc->createElement( _bstr_t(strName) );
		ASSERT( pChildNode != NULL );

		m_pNode->appendChild(pChildNode);

		SAFE_RELEASE(pDoc);
	}

	CXmlNodePtr pChild( new CXmlNode(pChildNode) );

	SAFE_RELEASE(pChildNode);

	return pChild;
}





//-------------------------------------------------------------------------
// Function Name    :_GetValue
// Parameter(s)     :CString & strValue IN	ֵ
// Return           :BOOL	�Ƿ�ɹ�
// Memo             :��ȡ��ǰ�ڵ��ֵ
//-------------------------------------------------------------------------
BOOL CXmlNode::_GetValue(CString & strValue OUT)
{
	ASSERT( m_pNode != NULL );

	if( HasChildren() )
	{
		// �������ӽ��
		ASSERT( FALSE );
		return FALSE;
	}

	BSTR bstr = NULL;
	HRESULT hr = m_pNode->get_text( &bstr );
	ASSERT( SUCCEEDED(hr) );	
	strValue = (LPCSTR)(LPCTSTR)_bstr_t( bstr, true);

	if( bstr != NULL )
	{
		SysFreeString(bstr);
		bstr = NULL;
	}

	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :_SetValue
// Parameter(s)     :CString & strValue IN
// Return           :BOOL	�Ƿ�ɹ�
// Memo             :��ȡ��ǰ�ڵ��ֵ
//-------------------------------------------------------------------------
BOOL CXmlNode:: _SetValue(CString & strValue IN)
{
	ASSERT( m_pNode != NULL );

	if( HasChildren() )
	{
		// �������ӽ�� 
		ASSERT( FALSE );
		return FALSE;
	}

	HRESULT hr = S_OK;
	hr = m_pNode->put_text( _bstr_t(strValue) );
	ASSERT( SUCCEEDED(hr) );

	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :GetName
// Parameter(s)     :void
// Return           :CString
// Memo             :��ȡ��ǰ�ڵ�����
//-------------------------------------------------------------------------
CString CXmlNode::GetName(void)
{
	ASSERT( m_pNode != NULL );

	BSTR bstr = NULL;
	HRESULT hr = m_pNode->get_nodeName(&bstr);
	ASSERT( SUCCEEDED(hr) );	
	CString strRet( (LPCSTR)(LPCTSTR)_bstr_t( bstr, true) );

	if( bstr != NULL )
	{
		SysFreeString(bstr);
		bstr = NULL;
	}

	return strRet;
}






//-------------------------------------------------------------------------
// Function Name    :GetAttribute
// Parameter(s)     :CString strName		������
//					:LPCTSTR lpszDefault	Ĭ��ֵ
// Return           :CString
// Memo             :��ȡ����ֵ
//-------------------------------------------------------------------------
CString CXmlNode::GetAttribute( CString strName IN
							   , LPCTSTR lpszDefault /* = NULL */ IN)
{
	ASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
	hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
	ASSERT( SUCCEEDED(hr) );

	MSXML2::IXMLDOMNodePtr pIXMLDOMNode = NULL;
	pIXMLDOMNode = pIXMLDOMNamedNodeMap->getNamedItem( _bstr_t(strName) );

	CString strValue;
	if( pIXMLDOMNode == NULL )
	{
		strValue = lpszDefault;
		SetAttribute( strName, strValue);		
	}
	else
	{
		VARIANT varValue;
		hr = pIXMLDOMNode->get_nodeValue(&varValue);
		ASSERT( SUCCEEDED(hr) );

		strValue = (LPCTSTR)(_bstr_t)varValue;
	}

	pIXMLDOMNode = NULL;

	pIXMLDOMNamedNodeMap = NULL;
	return strValue;
}


//-------------------------------------------------------------------------
// Function Name    :SetAttribute
// Parameter(s)     :CString strValue	ֵ
// Return           :BOOL
// Memo             :��������ֵ
//-------------------------------------------------------------------------
BOOL CXmlNode::SetAttribute( CString strName IN
							, CString strValue IN)
{
	ASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMElementPtr pElement = NULL;
	pElement = static_cast<IXMLDOMElementPtr> (m_pNode);
	hr = pElement->setAttribute( _bstr_t(strName), _variant_t((LPCSTR)strValue) );
	ASSERT( SUCCEEDED(hr) );

	SAFE_RELEASE(pElement);

	return SUCCEEDED(hr);
}

//-------------------------------------------------------------------------
// Function Name    :DelAttribute
// Parameter(s)     :CString strName IN ��������
// Return           :BOOL
// Memo             :ɾ������
//-------------------------------------------------------------------------
BOOL CXmlNode::DelAttribute( CString strName IN )
{
	ASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
	hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
	ASSERT( SUCCEEDED(hr) );

	hr = SUCCEEDED(hr) ? 
		pIXMLDOMNamedNodeMap->removeNamedItem( _bstr_t(strName) )
		: hr;
	ASSERT( SUCCEEDED(hr) );

	SAFE_RELEASE(pIXMLDOMNamedNodeMap);
	return SUCCEEDED(hr);
}



//-------------------------------------------------------------------------
// Function Name    :HasChildren
// Parameter(s)     :
// Return           :
// Memo             :�Ƿ����ӽ��
//-------------------------------------------------------------------------
BOOL CXmlNode::HasChildren(void)
{
	ASSERT( m_pNode != NULL );

	BOOL bHasChildren = FALSE;

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::DOMNodeType NodeType;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	hr = m_pNode->get_childNodes(&pNodeList);
	ASSERT( SUCCEEDED(hr) );

	for( int i = 0; i < pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];

		hr = pNode->get_nodeType(&NodeType);
		ASSERT( SUCCEEDED(hr) );

		SAFE_RELEASE(pNode);

		if( NodeType == MSXML2::NODE_ELEMENT )
		{
			bHasChildren = TRUE;
			break;
		}
	}

	SAFE_RELEASE(pNodeList);

	return bHasChildren;
}



//-------------------------------------------------------------------------
// Function Name    :Remove
// Parameter(s)     :
// Return           :
// Memo             :ɾ���˽��
//-------------------------------------------------------------------------
BOOL CXmlNode::Remove(void)
{
	ASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	hr = m_pNode->get_parentNode(&pNode);
	ASSERT( SUCCEEDED(hr) );

	pNode->removeChild(m_pNode);

	return TRUE;
}

//-------------------------------------------------------------------------
// Function Name    :RemoveChildren
// Parameter(s)     :
// Return           :BOOL
// Memo             :ɾ���ӽ��
//-------------------------------------------------------------------------
BOOL CXmlNode::RemoveChildren(void)
{
	ASSERT( m_pNode != NULL );

	if( !HasChildren() )
		return FALSE;

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNodePtr pNode = NULL;
	MSXML2::DOMNodeType NodeType;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	hr = m_pNode->get_childNodes(&pNodeList);
	ASSERT( SUCCEEDED(hr) );

	for( int i = 0; i < pNodeList->length; i++)
	{
		pNode = pNodeList->item[i];

		hr = pNode->get_nodeType(&NodeType);
		ASSERT( SUCCEEDED(hr) );

		if( NodeType == MSXML2::NODE_ELEMENT )
		{
			m_pNode->removeChild(pNode);
		}

		SAFE_RELEASE(pNode);
	}

	SAFE_RELEASE(pNodeList);
	return TRUE;
}


//-------------------------------------------------------------------------
// Function Name    :GetChildren
// Parameter(s)     :void
// Return           :CXmlNodesPtr
// Memo             :��ȡ�ӽ��
//-------------------------------------------------------------------------
CXmlNodesPtr CXmlNode::GetChildren()
{
	ASSERT( m_pNode != NULL );

	HRESULT hr = S_OK;
	MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
	hr = m_pNode->get_childNodes(&pNodeList);
	ASSERT( SUCCEEDED(hr) );

	CXmlNodesPtr pNodes(new CXmlNodes(pNodeList));

	return pNodes;
}




//////////////////////////////////////////////////////////////////////////
// ����Ϊȡֵ�����ط���

CString CXmlNode::GetString(LPCTSTR lpszValue /* = NULL */ IN)
{
	CString strValue;
	_GetValue(strValue);

	if( strValue.IsEmpty() &&
		lpszValue != NULL )
	{
		strValue = lpszValue;
		_SetValue(strValue);
	}

	return strValue;
}



BOOL CXmlNode::GetBOOL( BOOL bDefault /* = FALSE */ IN)
{
	CString strValue;
	_GetValue(strValue);

	if( strValue.CompareNoCase(_T("true")) == 0 )
	{
		return TRUE;
	}
	else if( strValue.CompareNoCase(_T("false")) == 0 )
	{
		return FALSE;
	}
	else
	{
		strValue = bDefault ? "true" : "false";
		_SetValue(strValue);
		return bDefault;
	}	
}


INT CXmlNode::GetInt( INT nDefault /* = 0 */ IN)
{
	CString strValue;
	_GetValue(strValue);

	if( strValue.IsEmpty() )
	{
		strValue.Format( "%ld", nDefault);
		_SetValue(strValue);
	}

	return atoi(strValue);
}



DOUBLE CXmlNode::GetFloat( DOUBLE fDefault /* = 0.0 */)
{
	CString strValue;
	_GetValue(strValue);

	if( strValue.IsEmpty() )
	{
		strValue.Format( "%f", fDefault);
		_SetValue(strValue);
	}

	return atof(strValue);
}


//////////////////////////////////////////////////////////////////////////
// ����Ϊ��ֵ�����ط���

BOOL CXmlNode::SetString( LPCTSTR lpszValue )
{
	CString strValue(lpszValue);
	return _SetValue(strValue);
}


BOOL CXmlNode::SetBOOL( BOOL bValue )
{
	CString strValue;
	strValue = bValue ? "true" : "false";

	return _SetValue(strValue);
}

BOOL CXmlNode::SetInt( INT nValue )
{
	CString strValue;
	strValue.Format( "%ld", nValue);

	return _SetValue(strValue);
}

BOOL CXmlNode::SetFloat( FLOAT fValue )
{
	CString strValue;
	strValue.Format( "%f", fValue);

	return _SetValue(strValue);
}

CXmlNodes::CXmlNodes(MSXML2::IXMLDOMNodeListPtr pNodeList)
{
	m_pNodeList = pNodeList;
}

CXmlNodes::CXmlNodes( CXmlNodes & refNodes )
{
	m_pNodeList = refNodes.m_pNodeList;
}

CXmlNodes::CXmlNodes(void)
: m_pNodeList(NULL)
{
}

CXmlNodes::~CXmlNodes(void)
{
	SAFE_RELEASE(m_pNodeList);
}

//////////////////////////////////////////////////////////////////////////



//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNodesPtr pNodes
// Return           :CXmlNodesPtr
// Memo             :���� = ��
//-------------------------------------------------------------------------
CXmlNodesPtr CXmlNodes::operator = ( CXmlNodesPtr pNodes IN )
{
	SAFE_RELEASE(m_pNodeList);

	m_pNodeList = pNodes->m_pNodeList;
	return pNodes;
}


//-------------------------------------------------------------------------
// Function Name    :operator =
// Parameter(s)     :CXmlNodes & refNodes
// Return           :CXmlNodes &
// Memo             :���� = ��
//-------------------------------------------------------------------------
CXmlNodes & CXmlNodes::operator = (CXmlNodes & refNodes)
{
	SAFE_RELEASE(m_pNodeList);

	m_pNodeList = refNodes.m_pNodeList;
	return (*this);
}



//-------------------------------------------------------------------------
// Function Name    :GetCount
// Parameter(s)     :void
// Return           :LONG
// Memo             :��ȡ����
//-------------------------------------------------------------------------
LONG CXmlNodes::GetCount(void)
{
	ASSERT( m_pNodeList != NULL );

	LONG lCount = 0l;
	HRESULT hr = S_OK;
	MSXML2::DOMNodeType NodeType;
	MSXML2::IXMLDOMNodePtr pNode = NULL;

	for( int i = 0; i < m_pNodeList->length; i++)
	{
		pNode = m_pNodeList->item[i];

		hr = pNode->get_nodeType(&NodeType);
		ASSERT( SUCCEEDED(hr) );

		if( NodeType == MSXML2::NODE_ELEMENT )
		{
			lCount ++;
		}

		SAFE_RELEASE(pNode);
	}

	return lCount;
}



//-------------------------------------------------------------------------
// Function Name    :GetItem
// Parameter(s)     :LONG nIndex	����
// Return           :CXmlNodePtr
// Memo             :��ȡ�����еĽ��
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNodes::GetItem( LONG nIndex )
{
	ASSERT( nIndex >= 0 && nIndex < GetCount() );

	LONG lCount = 0l;
	HRESULT hr = S_OK;
	MSXML2::DOMNodeType NodeType;
	MSXML2::IXMLDOMNodePtr pItem = NULL;

	CXmlNodePtr pNode ( new CXmlNode() );

	for( int i = 0; i < m_pNodeList->length; i++)
	{
		pItem = m_pNodeList->item[i];

		hr = pItem->get_nodeType(&NodeType);
		ASSERT( SUCCEEDED(hr) );


		if( NodeType == MSXML2::NODE_ELEMENT )
		{
			if( lCount == nIndex )
			{
				pNode->m_pNode = pItem;
				SAFE_RELEASE(pItem);
				return pNode;
			}

			lCount ++;
		}

		SAFE_RELEASE(pItem);
	}

	return pNode;
}


//-------------------------------------------------------------------------
// Function Name    :operator[]
// Parameter(s)     :INT nIndex
// Return           :CXmlNodePtr
// Memo             :����ͬ GetItem( LONG nIndex )
//-------------------------------------------------------------------------
CXmlNodePtr CXmlNodes::operator[] ( LONG lIndex )
{
	return GetItem(lIndex);
}


CXmlNodePtr CXmlNodes::GetItem( LPCTSTR lpszName )
{
	ASSERT( lpszName != NULL );

	HRESULT hr = S_OK;
	CString strName;
	BSTR bstr = NULL;
	MSXML2::IXMLDOMNodePtr pItem = NULL;

	CXmlNodePtr pNode ( new CXmlNode() );

	for( int i = 0; i < m_pNodeList->length; i++)
	{
		pItem = m_pNodeList->item[i];

		hr = pItem->get_nodeName(&bstr);
		ASSERT( SUCCEEDED(hr) );

		strName = (LPCSTR)(LPCTSTR)_bstr_t( bstr, true);
		if( bstr != NULL )
		{
			SysFreeString(bstr);
			bstr = NULL;
		}

		// ���ҵ�һ����ȵ�
		if( strName.Compare(lpszName) == 0 )
		{
			pNode->m_pNode = pItem;
			SAFE_RELEASE(pItem);
			return pNode;
		}

		SAFE_RELEASE(pItem);
	}

	return pNode;
}

CXmlNodePtr CXmlNodes::operator[] ( LPCTSTR lpszName )
{
	return GetItem(lpszName);
}

