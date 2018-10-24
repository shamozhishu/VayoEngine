/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 手工对象(用于手动制作网格)
\*************************************************************************/
#ifndef __VAYO_MANUAL_OBJECT_H__
#define __VAYO_MANUAL_OBJECT_H__

#include "VayoMovableObject.h"
#include "VayoRenderable.h"
#include "VayoVector2d.h"
#include "VayoVector3d.h"
#include "VayoColour.h"
#include "VayoMesh.h"

NS_VAYO_BEGIN

/**
 * Serialized template:
 * <ManualObject name="球" model="Sphere,examples/sphere,5,100,100"/>
**/
class _VayoExport ManualObject : public MovableObject, public Renderable
{
public:
	ManualObject(const wstring& name);
	~ManualObject();
	void     update(float dt);
	void     render();
	void     setMaterial(const wstring& name);
	void     setMaterial(const MaterialPtr& material);
	void     getWorldTransform(Matrix4x4& mat) const;
	void     begin(EPrimitiveType primType, const wstring& materialName = L"");
	void     beginUpdate(unsigned int idx);
	void     end();
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

public:
	/* 生成模型 */
	void generatePlane(float extent, float step, const wstring& materialName);
	void generateSphere(float fRadius, int iSlices, int iStacks, const wstring& materialName);
	void generateTorus(float majorRadius, float minorRadius, int numMajor, int numMinor, const wstring& materialName);
	/* 属性序列化与反序列化 */
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);

protected:
	void submitDisplay();

private:
	bool         _needSubmit;
	unsigned int _lastVertNum;
	unsigned int _lastIdxNum;
	MeshPtr      _meshData;
	SubMesh*     _opSubMesh;
	DisplayList* _displayList;
};

NS_VAYO_END

#endif // __VAYO_MANUAL_OBJECT_H__
