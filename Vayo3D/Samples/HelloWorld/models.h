#ifndef __MODELS_H__
#define __MODELS_H__

#include "MyApp.h"

// 生成弯曲导线
void bendWireway(const Vector3df& startCenterPos, const Vector3df& finishCenterPos, float circleRadius,
	float frontLength, SceneNode* parent, const wstring& modelName=L"");
// 组合模型
void combineModel(const wstring& connectorName, const wstring& rearCoverName, const wstring& wirewayName);
// 线束组合
void comWireway(float radius, float length, SceneNode* parent, const Vector3df& pos);
// 高亮某个模型
void highlightModel(const wstring& modelName, bool hightlight);

// 测试模型
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

// 箱子模型
class BoxModel
{
public:
	BoxModel(SceneManager* pSceneMgr);
	~BoxModel();

private:
	SceneManager* m_sceneMgr;
};

#endif // __MODELS_H__
