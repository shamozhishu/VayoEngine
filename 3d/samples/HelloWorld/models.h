#ifndef __MODELS_H__
#define __MODELS_H__

#include "MyApp.h"

// ������������
void bendWireway(const Vector3df& startCenterPos, const Vector3df& finishCenterPos, float circleRadius,
	float frontLength, SceneNode* parent, const wstring& modelName=L"");
// ���ģ��
void combineModel(const wstring& connectorName, const wstring& rearCoverName, const wstring& wirewayName);
// �������
void comWireway(float radius, float length, SceneNode* parent, const Vector3df& pos);
// ����ĳ��ģ��
void highlightModel(const wstring& modelName, bool hightlight);

// ����ģ��
class TestModel
{
public:
	TestModel(SceneManager* pSceneMgr);
	~TestModel();
	static void onSetConstants0(MaterialRendererServices* services);
	static void onSetConstants1(MaterialRendererServices* services);

private:
	SceneManager* m_sceneMgr;
};

// ����ģ��
class BoxModel
{
public:
	BoxModel(SceneManager* pSceneMgr);
	~BoxModel();

private:
	SceneManager* m_sceneMgr;
};

#endif // __MODELS_H__
