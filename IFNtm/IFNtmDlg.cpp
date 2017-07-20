// IFNtmDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IFNtm.h"
#include "IFNtmDlg.h"
#include ".\ifntmdlg.h"
#include "oracl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIFNtmDlg 对话框



CIFNtmDlg::CIFNtmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIFNtmDlg::IDD, pParent)
	, m_strColbText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIFNtmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CLOBTEXT, m_strColbText);
}

BEGIN_MESSAGE_MAP(CIFNtmDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CIFNtmDlg 消息处理程序

BOOL CIFNtmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIFNtmDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CIFNtmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIFNtmDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	char * database=(char *)malloc(15);
	char * uid=(char *)malloc(15);
	char * pwd=(char *)malloc(15);
	::GetPrivateProfileString("DataBase","database","fffff",database,15,"./Config.ini");
	::GetPrivateProfileString("DataBase","uid","",uid,15,"./Config.ini");
	::GetPrivateProfileString("DataBase","pwd","",pwd,15,"./Config.ini");
	OStartup();
	ODatabase odb(database, uid, pwd);
	ODynaset odyn(odb, "select * from clobtest");
	OClob oclob;
	odyn.GetFieldValue("text", &oclob);
	unsigned char *buffer = 0;
	try
	{
		unsigned long size = oclob.GetSize();

		buffer = (unsigned char *)malloc(size);
		memset(buffer, 0, size);
		oclob.EnableStreaming(size);
		short status = OLOB_NEED_DATA;
		unsigned long amtread = 0;
		while (status == OLOB_NEED_DATA)
		{
			// 下面是把数据读到 buffer 中
			amtread = oclob.Read(&status, buffer, 1000);
		}
		oclob.DisableStreaming();
	}
	catch(OException Exc)
	{
		m_strColbText = (CString)Exc.GetFailedMethodName() + " Error: " + (CString)Exc.GetErrorText() ;
	}
	m_strColbText = (CString)buffer;
	UpdateData(FALSE);
    free(buffer);
	free(database);
	free(uid);
	free(pwd);
	//OnOK();
}
