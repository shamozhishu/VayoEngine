/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 网格管理器
\*************************************************************************/
#ifndef __VAYO_MESH_MANAGER_H__
#define __VAYO_MESH_MANAGER_H__

#include "VayoSupport.h"
#include "VayoSharedPtr.h"
#include "VayoMesh.h"
NS_VAYO_BEGIN

class _VayoExport MeshManager
{
	DISALLOW_COPY_AND_ASSIGN(MeshManager)
public:
	MeshManager();
	~MeshManager();
	bool    init();

	// 查找网格
	MeshPtr findMesh(const wstring& name);

	// 创建空网格
	MeshPtr createEmptyMesh(const wstring& name = L"");

	// 销毁指定网格
	void destroyMesh(const wstring& name);
	void destroyMesh(MeshPtr mesh);

	// 创建立方体网格
	MeshPtr createCubeMesh(const Vector3df& size, const wstring& materialName = L"");
	MeshPtr createCubeMesh(const wstring& name, const Vector3df& size, const wstring& materialName = L"");

	// 创建球体网格
	MeshPtr createSphereMesh(float radius, unsigned int polyCountX, unsigned int polyCountY,
		const wstring& materialName = L"");
	MeshPtr createSphereMesh(const wstring& name, float radius, unsigned int polyCountX,
		unsigned int polyCountY, const wstring& materialName = L"");

	// 创建圆柱网格
	MeshPtr createCylinderMesh(float radius, float length, unsigned int tesselation,
		const Colour& color = 0xffffffff,
		bool closeTop = true, float oblique = 0.0f, const wstring& materialName = L"");
	MeshPtr createCylinderMesh(const wstring& name, float radius, float length, unsigned int tesselation,
		const Colour& color = 0xffffffff,
		bool closeTop = true, float oblique = 0.0f, const wstring& materialName = L"");

	// 创建圆锥网格
	MeshPtr createConeMesh(float radius, float length, unsigned int tesselation,
		const Colour& colorTop = 0xffffffff, const Colour& colorBottom = 0xffffffff,
		float oblique = 0.0f, const wstring& materialName = L"");
	MeshPtr createConeMesh(const wstring& name, float radius, float length, unsigned int tesselation,
		const Colour& colorTop = 0xffffffff, const Colour& colorBottom = 0xffffffff,
		float oblique = 0.0f, const wstring& materialName = L"");

private:
	unordered_map<wstring, MeshPtr> _meshPool;
};

NS_VAYO_END

#endif // __VAYO_MESH_MANAGER_H__
