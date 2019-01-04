// NewModelDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "TessgridBuilder.h"
#include "NewModelDlg.h"
#include "afxdialogex.h"
#include "GridDataManager.h"

// CNewModelDlg �Ի���

IMPLEMENT_DYNAMIC(CNewModelDlg, CDialogEx)
CNewModelDlg::CNewModelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NEW_MODEL_DLG, pParent)
{
}

CNewModelDlg::~CNewModelDlg()
{
}

CString CNewModelDlg::GetModelName() const
{
	return m_strModelName;
}

CString CNewModelDlg::GetModelMaterial() const
{
	return m_strModelMaterial;
}

void CNewModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MODELNAME, m_modelNameEdit);
	DDX_Control(pDX, IDC_EDIT_MODELMATERIAL, m_modelMaterialEdit);
}

BEGIN_MESSAGE_MAP(CNewModelDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &OnBnClickedCancel)
END_MESSAGE_MAP()

// CNewModelDlg ��Ϣ�������

void CNewModelDlg::OnBnClickedOk()
{
	m_modelNameEdit.GetWindowText(m_strModelName);
	m_modelMaterialEdit.GetWindowText(m_strModelMaterial);

	if (m_strModelName.IsEmpty())
	{
		AfxMessageBox(_T("ģ�����Ʋ���Ϊ�գ�"));
		m_strModelName.Empty();
		m_strModelMaterial.Empty();
		return;
	}

	const CGridData* pGridData = CGridDataManager::GetIns().GetGridData(m_strModelName);
	if (pGridData)
	{
		CString strTip;
		strTip.Format(_T("ģ��[%s]�Ѿ����ڣ�"), m_strModelName);
		AfxMessageBox(strTip);
		m_strModelName.Empty();
		m_strModelMaterial.Empty();
		return;
	}

	CDialogEx::OnOK();
}

void CNewModelDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
