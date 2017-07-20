// IFNtmDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IFNtm.h"
#include "IFNtmDlg.h"
#include ".\ifntmdlg.h"
#include "oracl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIFNtmDlg �Ի���



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


// CIFNtmDlg ��Ϣ�������

BOOL CIFNtmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CIFNtmDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CIFNtmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIFNtmDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
			// �����ǰ����ݶ��� buffer ��
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
