#pragma once
// CNewModelDlg �Ի���
class CNewModelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewModelDlg)

public:
	CNewModelDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewModelDlg();
	CString GetModelName() const;
	CString GetModelMaterial() const;

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEW_MODEL_DLG };
#endif

private:
	CEdit m_modelNameEdit;
	CEdit m_modelMaterialEdit;
	CString m_strModelName;
	CString m_strModelMaterial;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
