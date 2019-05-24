#include "models.h"
#include "pages.h"

//////////////////////////////////////////////////////////////////////////
// ¹«¹²º¯Êý
//////////////////////////////////////////////////////////////////////////

void bendWireway(const Vector3df& startCenterPos, const Vector3df& finishCenterPos, float circleRadius,
	float frontLength, SceneNode* parent,const wstring& modelName /*= L""*/)
{
	SceneManager* pSceneMgr = Root::getSingleton().getCurSceneMgr();
	if (pSceneMgr->findObject<ManualObject>(modelName))
		return;

	Vector3df tmpPos;
	unsigned int j, cnt = 60;
	const float angleStep = (VAYO_MATH_PI * 2.0f) / VayoDegToRad(cnt);
	Matrix4x4 transformMat;

	deque<VertIdxPair> vertList;
	Vertex tmpVert;
	tmpVert._normal.set(0, 0, 1);

	tmpPos.set(circleRadius, 0, 0);
	unsigned int idx = 0;
	vector<unsigned int> vecOutsideContour;
	vecOutsideContour.resize(60);
	vecOutsideContour[0] = idx++;
	tmpVert._position = tmpPos;
	vertList.push_back(tmpVert);
	transformMat.makeIdentity();
	transformMat.setRotationDegrees(angleStep, Vector3df::ZAxis);
	cnt -= 1;
	for (int j = 0; j < cnt; ++j)
	{
		transformMat.transformVect(tmpPos);
		tmpVert._position = tmpPos;
		vertList.push_back(tmpVert);
		vecOutsideContour[j + 1] = idx++;
	}

	TessGridHandler gridder(0, 1);
	gridder.getVertexList() = vertList;
	gridder.getContour(0) = vecOutsideContour;

	// Éú³É¹ì¼£Ïß
	vector<Vector3df> trailLine;
	if (isZero(frontLength))
	{
		trailLine.push_back(startCenterPos);
		trailLine.push_back(finishCenterPos);
	}
	else
	{
		float totalLengthZ = finishCenterPos._z - startCenterPos._z;
		float behindLengthZ = totalLengthZ - frontLength;

		Vector3df direction(0, 0, 1);
		Vector3df anchor1 = startCenterPos + direction * frontLength * 0.75f;
		trailLine.push_back(anchor1);
		Vector3df control1 = startCenterPos + direction * frontLength;
		trailLine.push_back(control1);
		Vector3df control2 = finishCenterPos - direction * behindLengthZ;
		trailLine.push_back(control2);
		Vector3df anchor2 = finishCenterPos - direction * behindLengthZ * 0.75f;
		trailLine.push_back(anchor2);

		vector<Vector3df> pts;
		gridder.bezierCurve3Order(pts, trailLine[0], trailLine[1], trailLine[2], trailLine[3]);
		trailLine = pts;
		trailLine.insert(trailLine.begin(), startCenterPos);
		trailLine.push_back(finishCenterPos);
	}

#if 0
	// ²âÊÔ¹ì¼£Ïß
	ManualObject* pTestObj = pSceneMgr->findObject<ManualObject>(modelName + L"_²âÊÔ¹ì¼£Ïß");
	if (NULL == pTestObj)
	{
		pTestObj = pSceneMgr->createObject<ManualObject>(modelName + L"_²âÊÔ¹ì¼£Ïß");
		parent->attachObject(pTestObj);
		pTestObj->setMaterial(L"examples/µãÔÆ");
		pTestObj->begin(EPT_LINE_STRIP, L"examples/µãÔÆ");
		pTestObj->colour(0, 255, 0);
		for (int i = 0; i < trailLine.size(); ++i)
			pTestObj->position(trailLine[i]);
		pTestObj->end();
	}
#endif

	// ½¨Ä£
	ManualObject* pLineObj = pSceneMgr->createObject<ManualObject>(modelName);
	pLineObj->setMaterial(L"examples/ÏßÊø");
	parent->attachObject(pLineObj);
	gridder.generatePipe(trailLine, pLineObj, 0, true, true, L"examples/ÏßÊø");
}

void combineModel(const wstring& connectorName, const wstring& rearCoverName, const wstring& wirewayName)
{
	SceneManager* pSceneMgr = Root::getSingleton().getCurSceneMgr();
	ManualObject* pConnectorObj = pSceneMgr->findObject<ManualObject>(connectorName);
	ManualObject* pRearCoverObj = pSceneMgr->findObject<ManualObject>(rearCoverName);
	ManualObject* pWirewayObj = pSceneMgr->findObject<ManualObject>(wirewayName);
	if (!pConnectorObj || !pRearCoverObj || !pWirewayObj)
	{
		return;
	}

	pConnectorObj->setVisible(true);
	pRearCoverObj->setVisible(true);
	pWirewayObj->setVisible(false);

	float connectorRadius = any_cast<float>(pConnectorObj->getUserDataBind().getUserData());
	float rearCoverRadius = any_cast<float>(pRearCoverObj->getUserDataBind().getUserData(L"interface_size"));
	float rearCoverHeight = any_cast<float>(pRearCoverObj->getUserDataBind().getUserData(L"height"));
	float wirewayRadius = any_cast<float>(pWirewayObj->getUserDataBind().getUserData());
	float scaleFactor = rearCoverRadius / connectorRadius;
	float zOffset = 0.0f;

	TableCSV* pTalbe = (TableCSV*)any_cast<void*>(pConnectorObj->getUserDataBind().getUserData(L"tablecsv"));
	if (pTalbe)
	{
		int num = pTalbe->getItemCount();
		wstring modelName;
		float comWirewayRadius = 0.0f;
		for (int i=1; i<=num; ++i)
		{
			modelName = pTalbe->item2str(i, L"name");
			Vector3df startPos = any_cast<Vector3df>(pConnectorObj->getUserDataBind().getUserData(modelName));
			Vector3df finishPos = startPos*Vector3df(scaleFactor, scaleFactor, 1);
			finishPos._z += rearCoverHeight;
			float frontLength = (startPos - Vector3df(0, 0, startPos._z)).getLength();
			if (frontLength > comWirewayRadius)
			{
				comWirewayRadius = frontLength;
			}
			bendWireway(startPos, finishPos, wirewayRadius, frontLength, pConnectorObj->getParentNode(), modelName);
			zOffset = startPos._z;
		}

		float h = rearCoverHeight - comWirewayRadius;
		comWirewayRadius += wirewayRadius;
		comWireway(comWirewayRadius, h, pConnectorObj->getParentNode(), Vector3df(0, 0, zOffset + comWirewayRadius - wirewayRadius));
	}

	zOffset += rearCoverHeight;
	zOffset /= 2.0f;
	pRearCoverObj->getMaterial()->_materialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
	SceneNode* pRearCoverNode = pRearCoverObj->getParentNode();
	pRearCoverNode->setRotation(Vector3df(180, 0, 0));
	pConnectorObj->getParentNode()->setPosition(Vector3df(0, 0, -zOffset));
}

void comWireway(float radius, float length, SceneNode* parent, const Vector3df& pos)
{
	SceneManager* pSceneMgr = Root::getSingleton().getCurSceneMgr();
	if (pSceneMgr->findObject<ManualObject>(L"comWireway()"))
		return;

	ManualObject* pObj = pSceneMgr->createObject<ManualObject>(L"comWireway()");
	parent->createChildSceneNode()->attachObject(pObj);
	pObj->getParentNode()->setPosition(pos);
	pObj->setMaterial(L"examples/plane");

	Vector3df tmpPos;
	unsigned int j, cnt = 60;
	const float angleStep = (VAYO_MATH_PI * 2.0f) / VayoDegToRad(cnt);
	Matrix4x4 transformMat;

	deque<VertIdxPair> vertList;
	Vertex tmpVert;
	tmpVert._normal.set(0, 0, 1);

	tmpPos.set(radius, 0, 0);
	unsigned int idx = 0;
	vector<unsigned int> vecOutsideContour;
	vecOutsideContour.resize(60);
	vecOutsideContour[0] = idx++;
	tmpVert._position = tmpPos;
	vertList.push_back(tmpVert);
	transformMat.makeIdentity();
	transformMat.setRotationDegrees(angleStep, Vector3df::ZAxis);
	cnt -= 1;
	for (int j = 0; j < cnt; ++j)
	{
		transformMat.transformVect(tmpPos);
		tmpVert._position = tmpPos;
		vertList.push_back(tmpVert);
		vecOutsideContour[j + 1] = idx++;
	}

	TessGridHandler gridder(0, 1);
	gridder.getVertexList() = vertList;
	gridder.getContour(0) = vecOutsideContour;

	transformMat.makeIdentity();
	transformMat.setTranslation(Vector3df(0, 0, length));
	gridder.beginStretch(pObj, 0, true);
	gridder.stretching(transformMat);
	gridder.endStretch();

	Matrix4x4 posMat, normMat;
	posMat.setTranslation(Vector3df(0, 0, length+0.01f));
	gridder.transformVertexList(posMat, normMat);
	gridder.tesselating(pObj, false);
}

void highlightModel(const wstring& modelName, bool hightlight)
{
	SceneManager* pSceneMgr = Root::getSingleton().getCurSceneMgr();
	ManualObject* pObj = pSceneMgr->findObject<ManualObject>(modelName);
	if (NULL == pObj)
		return;

	Any defaultMaterialName = pObj->getUserDataBind().getUserData(L"DefaultMaterialName");
	if (!defaultMaterialName.has_value())
	{
		pObj->getUserDataBind().setUserData(L"DefaultMaterialName", pObj->getMaterial()->_materialName);
		defaultMaterialName = pObj->getMaterial()->_materialName;
	}

	if (hightlight)
	{
		pObj->setMaterial(L"examples/Ô²Ì¨¸ßÁÁ");
	}
	else
	{
		pObj->setMaterial(any_cast<wstring>(defaultMaterialName));
	}
}

//////////////////////////////////////////////////////////////////////////
// TestModel
TestModel::TestModel(SceneManager* pSceneMgr)
	: m_sceneMgr(pSceneMgr)
{
	for (int i = 0; i < 1; ++i)
	{
		MeshPtr sphereMesh = Root::getSingleton().getMeshManager()->createSphereMesh(10, 1000, 1000, L"examples/torus");
		Entity* pEnt = m_sceneMgr->createObject<Entity>();
		m_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pEnt);
		pEnt->getParentNode()->setPosition(Vector3df(0, 0, -70 * i));
		pEnt->setMesh(sphereMesh);
	}

	ManualObject* pObj = m_sceneMgr->createObject<ManualObject>();
	pObj->begin(EPT_POLYGON, L"examples/TextureQuad");
	pObj->textureCoord(0, 0);
	pObj->position(0.5f, 0.75f, 0);
	pObj->textureCoord(0, 2);
	pObj->position(0.5f, 0.5f, 0);
	pObj->textureCoord(2, 2);
	pObj->position(0.75f, 0.5f, 0);
	pObj->textureCoord(2, 0);
	pObj->position(0.75f, 0.75f, 0);
	pObj->end();

	Entity* pEnt = m_sceneMgr->createObject<Entity>(L"TextureQuad");
	pEnt->setMesh(pObj->convertToMesh(EHM_STATIC));
	m_sceneMgr->getRootSceneNode()->attachObject(pEnt);
	m_sceneMgr->destroyObject(pObj);

	pEnt = m_sceneMgr->createObject<Entity>(L"TextureCube");
	MeshPtr cubeMesh = Root::getSingleton().getMeshManager()->createCubeMesh(Vector3df(10, 10, 10), L"examples/TextureCube");
	pEnt->setMesh(cubeMesh);
	m_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pEnt);
	pEnt->getParentNode()->setPosition(Vector3df(20, -20, -5));

	Root::getSingleton().getMaterialManager()->registerCallback(0, TestModel::onSetConstants0);
	Root::getSingleton().getMaterialManager()->registerCallback(1, TestModel::onSetConstants1);

	TessGridHandler gridder(0, 1);
	vector<Vector3df> pts;
	gridder.generateArc(pts, Vector3df(10, 10, -10), Vector3df(5, 5, -5), Vector3df(0, 0, 1), 90.0f, 10);
	pObj = m_sceneMgr->createObject<ManualObject>();
	pObj->begin(EPT_LINE_STRIP, L"examples/µãÔÆ");
	pObj->colour(255, 0, 0);
	for (int i = 0; i < pts.size(); ++i)
	{
		pObj->position(pts[i]);
	}
	pObj->end();

	m_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pObj);
	pObj->getParentNode()->setPosition(Vector3df(10, 10, 10));

	deque<VertIdxPair> vertList;
	Vertex tmpVert;
	tmpVert._position.set(-2, -2, 0);
	vertList.push_back(tmpVert);
	gridder.getContour(0).push_back(0);
	tmpVert._position.set(2, -2, 0);
	vertList.push_back(tmpVert);
	gridder.getContour(0).push_back(1);
	tmpVert._position.set(2, 2, 0);
	vertList.push_back(tmpVert);
	gridder.getContour(0).push_back(2);
	tmpVert._position.set(-2, 2, 0);
	vertList.push_back(tmpVert);
	gridder.getContour(0).push_back(3);
	gridder.getVertexList() = vertList;
	gridder.generatePipe(pts, pObj, 0, true, true, L"examples/outline");
}

TestModel::~TestModel()
{
	m_sceneMgr->destroyObject(L"TextureQuad");
	m_sceneMgr->destroyObject(L"TextureCube");
}

void TestModel::onSetConstants0(MaterialRendererServices* services)
{
	services->setShaderConstant("texture1", 0);
}

void TestModel::onSetConstants1(MaterialRendererServices* services)
{
	Vector3df pointLightPositions[] = { Vector3df(0.7f,  0.2f,  2.0f), Vector3df(2.3f, -3.3f, -4.0f),
		Vector3df(-4.0f,  2.0f, -12.0f), Vector3df(0.0f,  0.0f, -3.0f) };

	services->setShaderConstant("material.diffuse", 0);
	services->setShaderConstant("material.specular", 1);

	Camera* pCamera = Root::getSingleton().getCurSceneMgr()->getActiveCamera();
	Vector3df cameraPos, cameraFront;
	pCamera->getWorldPos(cameraPos);
	pCamera->getWorldLook(cameraFront);
	services->setShaderConstant("viewPos", cameraPos);
	services->setShaderConstant("material.shininess", 32.0f);
	// directional light
	services->setShaderConstant("dirLight.direction", -0.2f, -1.0f, -0.3f);
	services->setShaderConstant("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	services->setShaderConstant("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	services->setShaderConstant("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	services->setShaderConstant("pointLights[0].position", pointLightPositions[0]);
	services->setShaderConstant("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	services->setShaderConstant("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	services->setShaderConstant("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	services->setShaderConstant("pointLights[0].constant", 1.0f);
	services->setShaderConstant("pointLights[0].linear", 0.09f);
	services->setShaderConstant("pointLights[0].quadratic", 0.032f);
	// point light 2
	services->setShaderConstant("pointLights[1].position", pointLightPositions[1]);
	services->setShaderConstant("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	services->setShaderConstant("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	services->setShaderConstant("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	services->setShaderConstant("pointLights[1].constant", 1.0f);
	services->setShaderConstant("pointLights[1].linear", 0.09f);
	services->setShaderConstant("pointLights[1].quadratic", 0.032f);
	// point light 3
	services->setShaderConstant("pointLights[2].position", pointLightPositions[2]);
	services->setShaderConstant("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	services->setShaderConstant("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	services->setShaderConstant("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	services->setShaderConstant("pointLights[2].constant", 1.0f);
	services->setShaderConstant("pointLights[2].linear", 0.09f);
	services->setShaderConstant("pointLights[2].quadratic", 0.032f);
	// point light 4
	services->setShaderConstant("pointLights[3].position", pointLightPositions[3]);
	services->setShaderConstant("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	services->setShaderConstant("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	services->setShaderConstant("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	services->setShaderConstant("pointLights[3].constant", 1.0f);
	services->setShaderConstant("pointLights[3].linear", 0.09f);
	services->setShaderConstant("pointLights[3].quadratic", 0.032f);
	// spotLight
	services->setShaderConstant("spotLight.position", cameraPos);
	services->setShaderConstant("spotLight.direction", cameraFront);
	services->setShaderConstant("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	services->setShaderConstant("spotLight.diffuse", 1.0f, 0.0f, 0.0f);
	services->setShaderConstant("spotLight.specular", 1.0f, 0.0f, 0.0f);
	services->setShaderConstant("spotLight.constant", 1.0f);
	services->setShaderConstant("spotLight.linear", 0.0f);
	services->setShaderConstant("spotLight.quadratic", 0.0f);
	services->setShaderConstant("spotLight.cutOff", cosf(VayoDegToRad(12.5f)));
	services->setShaderConstant("spotLight.outerCutOff", cosf(VayoDegToRad(15.0f)));

	RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
	services->setShaderConstant("projection", pRenderer->getTransform(ETS_PROJECTION));
	services->setShaderConstant("view", pRenderer->getTransform(ETS_VIEW));
	services->setShaderConstant("model", pRenderer->getTransform(ETS_WORLD));
}

BoxModel::BoxModel(SceneManager* pSceneMgr)
	: m_sceneMgr(pSceneMgr)
{
	Entity* pEnt = m_sceneMgr->findObject<Entity>(Language::getSingleton().getLanguageByID(21));
	pEnt->setMesh(Root::getSingleton().getMeshManager()->createCubeMesh(Vector3df(10, 10, 10), L"examples/Ïä×Ó"));
	pEnt->getSubEntity(0)->setRenderPriority(0);
	pEnt = m_sceneMgr->findObject<Entity>(Language::getSingleton().getLanguageByID(22));
	pEnt->setMesh(Root::getSingleton().getMeshManager()->createCubeMesh(Vector3df(11, 11, 11), L"examples/Ãè±ß"));
	pEnt->getSubEntity(0)->setRenderPriority(1);
	pEnt = m_sceneMgr->findObject<Entity>(Language::getSingleton().getLanguageByID(23));
	pEnt->setMesh(Root::getSingleton().getMeshManager()->createCubeMesh(Vector3df(10, 10, 10), L"examples/Ïä×Ó"));
	pEnt->getSubEntity(0)->setRenderPriority(0);
	pEnt = m_sceneMgr->findObject<Entity>(Language::getSingleton().getLanguageByID(24));
	pEnt->setMesh(Root::getSingleton().getMeshManager()->createCubeMesh(Vector3df(11, 11, 11), L"examples/Ãè±ß"));
	pEnt->getSubEntity(0)->setRenderPriority(1);
	ManualObject* pObj = m_sceneMgr->findObject<ManualObject>(Language::getSingleton().getLanguageByID(25));
	pObj->setMaterial(L"examples/Ïä×Ó");
	pObj->generateSphere(6, 30, 30, L"examples/Ïä×Ó");
	pObj->setRenderPriority(0);
	pObj = m_sceneMgr->findObject<ManualObject>(Language::getSingleton().getLanguageByID(26));
	pObj->setMaterial(L"examples/Ãè±ß");
	pObj->generateSphere(6.5f, 30, 30, L"examples/Ãè±ß");
	pObj->setRenderPriority(1);
	pObj = m_sceneMgr->findObject<ManualObject>(Language::getSingleton().getLanguageByID(27));
	pObj->setMaterial(L"examples/Ïä×Ó");
	pObj->generateSphere(6, 30, 30, L"examples/Ïä×Ó");
	pObj->setRenderPriority(0);
	pObj = m_sceneMgr->findObject<ManualObject>(Language::getSingleton().getLanguageByID(28));
	pObj->setMaterial(L"examples/Ãè±ß");
	pObj->generateSphere(6.5f, 30, 30, L"examples/Ãè±ß");
	pObj->setRenderPriority(1);

	pObj = m_sceneMgr->createObject<ManualObject>(L"Èý½ÇÐÎ");
	pObj->getMaterial()->_lighting = false;
	pObj->getMaterial()->_gouraudShading = false;
	m_sceneMgr->getRootSceneNode()->attachObject(pObj);
	pObj->begin(EPT_TRIANGLES);
	pObj->colour(0xffff0000);
	pObj->position(0, 5, 0);
	pObj->colour(0xff00ff00);
	pObj->position(-5, 0, 0);
	pObj->colour(0xff0000ff);
	pObj->position(5, 0, 0);
	pObj->end();
}

BoxModel::~BoxModel()
{
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(21));
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(22));
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(23));
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(24));
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(25));
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(26));
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(27));
	m_sceneMgr->destroyObject(Language::getSingleton().getLanguageByID(28));
}
