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
		m_listCtrl.InsertColumn(0, _T("材质脚本"), LVCFMT_CENTER, 192, 0);
		m_listCtrl.SetColumnWidth(0, 192);
		LVCOLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.iImage = 0;
		lvcol.cchTextMax = 256;
		lvcol.cx = 192;
		lvcol.pszText = _T("材质脚本");
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

	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("material"), oleVarStr, _T("材质名称")));

	oleVarStr.SetString(_T("solid"), VT_BSTR);
	CMFCPropertyGridProperty* pValueProp = new CMFCPropertyGridProperty(_T("type"), oleVarStr, _T("材质类型"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 24; i < 39; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarByte.bVal = 255;
	CMFCPropertyGridProperty* pGroupProp = new CMFCPropertyGridProperty(_T("环境光"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.r"), oleVarByte, _T("红")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.g"), oleVarByte, _T("绿")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.b"), oleVarByte, _T("蓝")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ambient_color.a"), oleVarByte, _T("alpha")));

	pGroupProp = new CMFCPropertyGridProperty(_T("漫射光"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.r"), oleVarByte, _T("红")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.g"), oleVarByte, _T("绿")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.b"), oleVarByte, _T("蓝")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("diffuse_color.a"), oleVarByte, _T("alpha")));

	oleVarByte.bVal = 0;
	pGroupProp = new CMFCPropertyGridProperty(_T("镜面光"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.r"), oleVarByte, _T("红")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.g"), oleVarByte, _T("绿")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.b"), oleVarByte, _T("蓝")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("specular_color.a"), oleVarByte, _T("alpha")));

	pGroupProp = new CMFCPropertyGridProperty(_T("自发光"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.r"), oleVarByte, _T("红")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.g"), oleVarByte, _T("绿")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.b"), oleVarByte, _T("蓝")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("emissive_color.a"), oleVarByte, _T("alpha")));

	pGroupProp = new CMFCPropertyGridProperty(_T("纹理单元"));
	m_propGridCtrl.AddProperty(pGroupProp);

	oleVarStr.SetString(_T(""), VT_BSTR);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("texture"), oleVarStr, _T("纹理图片")));

	oleVarStr.SetString(_T("bilinear_filter"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("filtering"), oleVarStr, _T("纹理过滤"));
	pGroupProp->AddSubItem(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 0; i < 3; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("repeat"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("tex_address_mode"), oleVarStr, _T("纹理寻址模式"));
	pGroupProp->AddSubItem(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 0; i < 8; ++i)
		pValueProp->AddOption(a2w_(aTextureClampNames[i]).c_str());

	oleVarFlt.fltVal = 0.0f;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("shininess"), oleVarFlt, _T("反射指数")));
	oleVarFlt.fltVal = 1.0f;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("thickness"), oleVarFlt, _T("点线厚度")));
	oleVarFlt.fltVal = 0.0f;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("alpha_ref"), oleVarFlt, _T("alpha测试")));

	oleVarStr.SetString(_T("on"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("anti_aliasing"), oleVarStr, _T("抗锯齿"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 19; i < 24; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarByte.bVal = 255;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("stencil_mask"), oleVarByte, _T("模板掩码")));
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("stencil_func_mask"), oleVarByte, _T("模板函数掩码")));
	oleVarInt.intVal = 0;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("stencil_ref"), oleVarInt, _T("模板参考值")));

	oleVarStr.SetString(_T("never"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_func"), oleVarStr, _T("模板函数"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 3; i < 11; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("keep"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_fail"), oleVarStr, _T("模板测试失败"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 11; i < 19; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("keep"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("depth_fail"), oleVarStr, _T("深度测试失败"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 11; i < 19; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("keep"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_depth_pass"), oleVarStr, _T("模板-深度测试成功"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	for (int i = 11; i < 19; ++i)
		pValueProp->AddOption(a2w_(materialScriptAttribs[i].TheKey).c_str());

	oleVarStr.SetString(_T("off"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("wireframe"), oleVarStr, _T("线框模式"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("clockwise"), oleVarStr, _T("顶点环绕方式（顺时针和逆时针）"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("on"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("gouraud_shading"), oleVarStr, _T("平滑着色"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("lighting"), oleVarStr, _T("启用光照"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("zbuffer"), oleVarStr, _T("启用深度测试"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("zwrite_enable"), oleVarStr, _T("深度缓冲区可写"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("off"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("stencil_buffer"), oleVarStr, _T("启用模板测试"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("on"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("backface_culling"), oleVarStr, _T("背面剔除"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	pValueProp = new CMFCPropertyGridProperty(_T("use_mip_maps"), oleVarStr, _T("启用多级渐进纹理"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T("off"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("fog_enable"), oleVarStr, _T("启用雾"));
	m_propGridCtrl.AddProperty(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("off"));
	pValueProp->AddOption(_T("on"));

	oleVarStr.SetString(_T(""), VT_BSTR);
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("vertex_program"), oleVarStr, _T("顶点着色器")));
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("fragment_program"), oleVarStr, _T("片段着色器")));
	oleVarUint.uintVal = 0u;
	m_propGridCtrl.AddProperty(new CMFCPropertyGridProperty(_T("shader_constant_set_callback"), oleVarUint, _T("着色器常量设置回调函数索引")));

	return true;
}
