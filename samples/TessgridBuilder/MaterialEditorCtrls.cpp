#include "stdafx.h"
#include "MaterialEditorCtrls.h"
#include "TessgridBuilder.h"
#include "TessgridBuilderDlg.h"

CMaterialEditorCtrls::CMaterialEditorCtrls()
{
}

bool CMaterialEditorCtrls::Init(CTessgridBuilderDlg* dlg)
{
	m_staPropGridBg.Init(dlg);
	m_staPropGridBg.ShowWindow(SW_HIDE);
	m_listCtrl.ShowWindow(SW_HIDE);

	if (m_listImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2))
	{
		COLORREF co = GetSysColor(COLOR_WINDOW);
		m_listImg.SetBkColor(co);
		m_listCtrl.SetImageList(&m_listImg, LVSIL_SMALL);

		HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MATERIAL), IMAGE_ICON, 16, 16, 0);
		m_listImg.Add(hIcon);

		m_listCtrl.SetFont(&afxGlobalData.fontRegular);
		m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_COLUMNOVERFLOW);
		m_listCtrl.InsertColumn(0, _T("���ʽű�"), LVCFMT_CENTER, 192, 0);
		m_listCtrl.SetColumnWidth(0, 192);
		LVCOLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.iImage = 0;
		lvcol.cchTextMax = 256;
		lvcol.cx = 192;
		lvcol.pszText = _T("���ʽű�");
		m_listCtrl.SetColumn(0, &lvcol);
	}

	m_propGridCtrl.SetParent(&m_staPropGridBg);
	m_gridCtrlItem.cxy = 120;
	m_gridCtrlItem.mask = HDI_WIDTH;
	m_propGridCtrl.GetHeaderCtrl().SetItem(0, &m_gridCtrlItem);
	m_propGridCtrl.EnableHeaderCtrl(FALSE);
	m_propGridCtrl.EnableDescriptionArea();
	m_propGridCtrl.SetVSDotNetLook();
	m_propGridCtrl.MarkModifiedProperties();

	COleVariant oleVarStr, oleVarFlt, oleVarInt, oleVarUint, oleVarByte;
	oleVarStr.SetString(_T(""), VT_BSTR);
	oleVarFlt.ChangeType(VT_R4);
	oleVarInt.ChangeType(VT_INT);
	oleVarUint.ChangeType(VT_UINT);
	oleVarByte.ChangeType(VT_UI2);

	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("material"), oleVarStr, _T("��������")));

	oleVarStr.SetString(_T("solid"), VT_BSTR);
	CMFCPropertyGridProperty* pValueProp = new CMFCPropertyGridProperty(_T("type"), oleVarStr, _T("��������"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 24; i < 39; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarByte.bVal = 255;
	CMFCPropertyGridProperty* pGroupProp = new CMFCPropertyGridProperty(_T("������"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.r"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.g"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.b"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.a"), oleVarByte, _T("alpha")));

	pGroupProp = new CMFCPropertyGridProperty(_T("�����"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.r"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.g"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.b"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.a"), oleVarByte, _T("alpha")));

	oleVarByte.bVal = 0;
	pGroupProp = new CMFCPropertyGridProperty(_T("�����"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.r"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.g"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.b"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.a"), oleVarByte, _T("alpha")));

	pGroupProp = new CMFCPropertyGridProperty(_T("�Է���"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.r"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.g"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.b"), oleVarByte, _T("��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.a"), oleVarByte, _T("alpha")));

	pGroupProp = new CMFCPropertyGridProperty(_T("����Ԫ"));
	m_propGridCtrl.AddProperty(pGroupProp);

	oleVarStr.SetString(_T(""), VT_BSTR);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("texture"), oleVarStr, _T("����ͼƬ")));

	oleVarStr.SetString(_T("bilinear_filter"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("filtering"), oleVarStr, _T("�������"));
	pGroupProp->AddSubItem(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 0; i < 3; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("repeat"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("tex_address_mode"), oleVarStr, _T("����Ѱַģʽ"));
	pGroupProp->AddSubItem(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 0; i < 8; ++i)
		pValueProp->AddOption(a2w_(aTextureClampNames[i]).c_str());

	oleVarFlt.fltVal = 0.0f;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("shininess"), oleVarFlt, _T("����ָ��")));
	oleVarFlt.fltVal = 1.0f;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("thickness"), oleVarFlt, _T("���ߺ��")));
	oleVarFlt.fltVal = 0.0f;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("alpha_ref"), oleVarFlt, _T("alpha����")));

	oleVarStr.SetString(_T("on"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("anti_aliasing"), oleVarStr, _T("�����"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 19; i < 24; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarByte.bVal = 255;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("stencil_mask"), oleVarByte, _T("ģ������")));
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("stencil_func_mask"), oleVarByte, _T("ģ�庯������")));
	oleVarInt.intVal = 0;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("stencil_ref"), oleVarInt, _T("ģ��ο�ֵ")));

	oleVarStr.SetString(_T("never"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_func"), oleVarStr, _T("ģ�庯��"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 3; i < 11; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("keep"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_fail"), oleVarStr, _T("ģ�����ʧ��"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 11; i < 19; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("keep"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("depth_fail"), oleVarStr, _T("��Ȳ���ʧ��"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 11; i < 19; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("keep"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_depth_pass"), oleVarStr, _T("ģ��-��Ȳ��Գɹ�"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 11; i < 19; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("off"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("wireframe"), oleVarStr, _T("�߿�ģʽ"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("clockwise"), oleVarStr, _T("���㻷�Ʒ�ʽ��˳ʱ�����ʱ�룩"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("on"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("gouraud_shading"), oleVarStr, _T("ƽ����ɫ"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("lighting"), oleVarStr, _T("���ù���"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("zbuffer"), oleVarStr, _T("������Ȳ���"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("zwrite_enable"), oleVarStr, _T("��Ȼ�������д"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("off"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_buffer"), oleVarStr, _T("����ģ�����"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("on"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("backface_culling"), oleVarStr, _T("�����޳�"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("use_mip_maps"), oleVarStr, _T("���ö༶��������"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("off"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("fog_enable"), oleVarStr, _T("������"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T(""), VT_BSTR);
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("vertex_program"), oleVarStr, _T("������ɫ��")));
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("fragment_program"), oleVarStr, _T("Ƭ����ɫ��")));
	oleVarUint.uintVal = 0u;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("shader_constant_set_callback"), oleVarUint, _T("��ɫ���������ûص���������")));

	return true;
}
