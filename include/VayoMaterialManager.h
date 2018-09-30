/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 材质管理器
\*************************************************************************/
#ifndef __VAYO_MATERIAL_MANAGER_H__
#define __VAYO_MATERIAL_MANAGER_H__

#include "VayoSupport.h"
#include "VayoMaterial.h"
#include "VayoMaterialRenderer.h"

#define VAYO_MAX_SHADER_CALLBACK_NUM (500)

NS_VAYO_BEGIN

class _VayoExport MaterialManager
{
	DISALLOW_COPY_AND_ASSIGN(MaterialManager)
public:
	MaterialManager();
	~MaterialManager();
	bool init();
	MaterialPtr createMaterial(const wstring& name = L"");
	MaterialPtr findMaterial(const wstring& name);
	void        destroyMaterial(const wstring& name);
	void        destroyMaterial(const MaterialPtr& ptr);
	MaterialPtr getDefaultMaterial() const;
	void registerCallback(unsigned int idx, ShaderConstantSetCallback callback);
	void unregisterCallback(unsigned int idx);

private:
	bool parseMaterial(const wstring& filePath);

private:
	MaterialPtr               _defaultMaterial;
	map<wstring, MaterialPtr> _materialPool;
	ShaderConstantSetCallback _materialCallback[VAYO_MAX_SHADER_CALLBACK_NUM];
};

NS_VAYO_END

#endif // __VAYO_MATERIAL_MANAGER_H__
