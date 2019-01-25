// NewModelDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "TessgridBuilder.h"
#include "NewModelDlg.h"
#include "afxdialogex.h"
#include "GridDataManager.h"

// CNewModelDlg 对话框

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

// CNewModelDlg 消息处理程序

void CNewModelDlg::OnBnClickedOk()
{
	m_modelNameEdit.GetWindowText(m_strModelName);
	m_modelMaterialEdit.GetWindowText(m_strModelMaterial);

	if (m_strModelName.IsEmpty())
	{
		AfxMessageBox(_T("模型名称不能为空！"));
		m_strModelName.Empty();
		m_strModelMaterial.Empty();
		return;
	}

	const CGridData* pGridData = CGridDataManager::GetIns().GetGridData(m_strModelName);
	if (pGridData)
	{
		CString strTip;
		strTip.Format(_T("模型[%s]已经存在！"), m_strModelName);
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
