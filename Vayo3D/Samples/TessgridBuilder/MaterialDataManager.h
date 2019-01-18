#pragma once
#include "MaterialData.h"

class CMaterialDataManager
{
public:
	static CMaterialDataManager& GetIns();
	static void Destroy();
	CMaterialDataManager();
	~CMaterialDataManager();
	bool OpenMaterialFile(CString filePath);
	bool SaveMaterialFile(CString filePath);
	void ClearAllMaterial();

private:
	bool ParseMaterialFile(stringstream& filestream);

private:
	CMaterialData m_currentMaterialData;
	list<CMaterialData> m_materialDataset;
};
