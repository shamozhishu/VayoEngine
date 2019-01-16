#pragma once
#include "GridData.h"

class CGridDataManager
{
public:
	static CGridDataManager& GetIns();
	static void Destroy();
	CGridDataManager();
	~CGridDataManager();
	bool OpenTessgridFile(CString filePath);
	bool SaveTessgridFile(CString filePath);
	bool GeneratingModel(CString modelName, bool display);
	void HideOtherModel(const wstring& excludedModel = L"");
	void ClearAllModel();
	CGridData* GetGridData(CString modelName);
	const CGridData* GetGridData(CString modelName) const;
	unsigned int GetGridDataCnt() const;
	list<CGridData>& GetGridDataset();
	const list<CGridData>& GetGridDataset() const;

private:
	bool BuildModelData(const CGridData& gridData, bool isBuildManulObject);
	bool ParseTessgridFile(wstringstream& filestream);
	bool ParseProp(wstringstream& strin);
	bool ParseCapAndBody(wstringstream& strin, const wstring& tag);
	bool ParseCap(wstringstream& strin, bool topCap);
	bool ParsePlace(wstringstream& strin);
	bool ParseStretchingBody(wstringstream& strin);
	bool ParsePolygon(wstringstream& strin);
	bool ParseCircle(wstringstream& strin);
	bool ParseGridding(wstringstream& strin);
	enum EGRIDDATA_OP { EGRIDDATA_OP_TOPCAP, EGRIDDATA_OP_BOTTOMCAP, EGRIDDATA_OP_STRETCH };
	bool OperateCurGridData(unsigned idx, EGRIDDATA_OP opType, const CGridShapeOp::SCap& capData, const CGridShapeOp::SStretchingBody& bodyData);

private:
	CGridData m_currentGridData;
	list<CGridData> m_gridDataset;
	TessGridBuilder m_gridBuilder;
	TessGridHandler m_gridHandler;
};
