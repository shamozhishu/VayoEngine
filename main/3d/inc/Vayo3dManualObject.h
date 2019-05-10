/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 手工对象(用于手动制作网格)
\*************************************************************************/
#ifndef __VAYO3D_MANUAL_OBJECT_H__
#define __VAYO3D_MANUAL_OBJECT_H__

#include "Vayo3dMesh.h"
#include "Vayo3dColour.h"
#include "Vayo3dRenderable.h"
#include "Vayo3dMovableObject.h"
#include "VayoVector2d.h"
#include "VayoVector3d.h"

NS_VAYO3D_BEGIN

class _Vayo3dExport ManualObject : public MovableObject, public Renderable
{
	static Reflex<ManualObject, const wstring&, SceneManager*> _dynReflex;
public:
	enum
	{
		ECH_NEED_COMPUTE_NORM = ESTATE_01, // 在生成显示列表前自动计算法线信息
		ECH_GL_IMMEDIATE_MODE = ESTATE_02, // 在生成显示列表时使用OpenGL立即模式
		ECH_ONLY_DISPLAY_LIST = ESTATE_03  // 在生成显示列表后销毁使用过的子网格
	};
public:
	ManualObject(const wstring& name, SceneManager* oriSceneMgr);
	~ManualObject();
	void     update(float dt);
	void     render();
	void     setMaterial(const wstring& name);
	void     setMaterial(MaterialPtr material);
	void     getWorldTransform(Matrix4x4& mat) const;
	bool     begin(EPrimitiveType primType, const wstring& materialName = L"", bool sharedSubMesh = false);
	bool     beginUpdate(unsigned int idx, bool sharedSubMesh = false);
	void     end(bool endlist = false);
	void     position(const Vector3df& pos);
	void     position(float x, float y, float z);
	void     normal(const Vector3df& norm);
	void     normal(float x, float y, float z);
	void     textureCoord(const Vector2df& uv);
	void     textureCoord(float u, float v);
	void     colour(const Colour& col);
	void     colour(int r, int g, int b, int a = 255);
	void     index(unsigned int idx);
	void     triangle(unsigned int i1, unsigned int i2, unsigned int i3);
	void     quad(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4);
	MeshPtr  convertToMesh(EHardwareMapping mappingHint = EHM_NEVER) const;
	MeshPtr  getMesh() const;
	SubMesh* getOpSubMesh() const;
	void     resetSubmit();
	void     setCharacteristic(unsigned int character);
	bool     getCharacteristic(unsigned int character);

public:
	/* 生成模型 */
	void generatePlane(float extent, float step, const wstring& materialName);
	void generateSphere(float fRadius, int iSlices, int iStacks, const wstring& materialName);
	void generateTorus(float majorRadius, float minorRadius, int numMajor, int numMinor, const wstring& materialName);
	/* 属性序列化与反序列化 */
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
	bool parseCustomAttrib();

protected:
	void submitDisplayList();
	void setMaterialCB(SubMesh* mb);
	void setMaterialCB(SharedSubMesh* mb, unsigned idx);

private:
	bool          _needSubmit;
	bool     _isSharedSubMesh;
	unsigned int _lastVertNum;
	unsigned int  _lastIdxNum;
	BitState    _characterBit;
	MeshPtr         _meshData;
	SubMesh*       _opSubMesh;
	IndexBuffer* _opIdxBuffer;
	DisplayList* _displayList;
};

NS_VAYO3D_END

#endif // __VAYO3D_MANUAL_OBJECT_H__
