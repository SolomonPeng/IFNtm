// IFNtmDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#pragma comment(lib, "ORACLM32")

// CIFNtmDlg �Ի���
class CIFNtmDlg : public CDialog
{
// ����
public:
	CIFNtmDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IFNTM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strColbText;
};
