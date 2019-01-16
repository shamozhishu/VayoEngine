#include "stdafx.h"
#include "MaterialDataManager.h"

static CMaterialDataManager* g_pMaterialDataMgr = NULL;

CMaterialDataManager& CMaterialDataManager::GetIns()
{
	if (!g_pMaterialDataMgr)
		g_pMaterialDataMgr = new CMaterialDataManager();
	return *g_pMaterialDataMgr;
}

void CMaterialDataManager::Destroy()
{
	SAFE_DELETE(g_pMaterialDataMgr);
}

CMaterialDataManager::CMaterialDataManager()
{
}

CMaterialDataManager::~CMaterialDataManager()
{
}

bool CMaterialDataManager::OpenMaterialFile(CString filePath)
{
	if (filePath.IsEmpty() || filePath.Find(_T(".material")) == -1)
		return false;

	std::ifstream fin(filePath);
	if (!fin)
	{
		Log::wprint(ELL_ERROR, L"材质脚本[%s]打开失败", filePath.GetString());
		return false;
	}

	fin.seekg(0, ios_base::end);
	if ((int)fin.tellg() == 0)
	{
		Log::wprint(ELL_ERROR, L"材质脚本[%s]为空", filePath.GetString());
		return false;
	}

	fin.seekg(0, ios_base::beg);
	stringstream filestream;
	filestream << fin.rdbuf();
	return ParseMaterialFile(filestream);
}

bool CMaterialDataManager::SaveMaterialFile(CString filePath)
{
	return true;
}

void CMaterialDataManager::ClearAllMaterial()
{
	Root::getSingleton().getMaterialManager()->clearAllMaterials();
}

bool CMaterialDataManager::ParseMaterialFile(stringstream& filestream)
{
	string         strTag;
	vector<string> container;
	int            curSelTexLayer = -1;
	int            singleMaterialParseEndMark = 0;

	do
	{
		std::getline(filestream, strTag);
		strTag = trim(strTag);

		if (0 == strTag.find('//') || 0 == strTag.compare(""))
			continue;

		if (0 == strTag.compare("{"))
		{
			++singleMaterialParseEndMark;
			continue;
		}

		if (0 == strTag.substr(0, 8).compare("material"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.name = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 4).compare("type"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.type = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 13).compare("ambient_color"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 5)
			{
				int r = atoi(container[1].c_str());
				int g = atoi(container[2].c_str());
				int b = atoi(container[3].c_str());
				int a = atoi(container[4].c_str());
				m_currentMaterialData.ambient_color[0] = r;
				m_currentMaterialData.ambient_color[1] = g;
				m_currentMaterialData.ambient_color[2] = b;
				m_currentMaterialData.ambient_color[3] = a;
				continue;
			}
		}

		if (0 == strTag.substr(0, 13).compare("diffuse_color"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 5)
			{
				int r = atoi(container[1].c_str());
				int g = atoi(container[2].c_str());
				int b = atoi(container[3].c_str());
				int a = atoi(container[4].c_str());
				m_currentMaterialData.diffuse_color[0] = r;
				m_currentMaterialData.diffuse_color[1] = g;
				m_currentMaterialData.diffuse_color[2] = b;
				m_currentMaterialData.diffuse_color[3] = a;
				continue;
			}
		}

		if (0 == strTag.substr(0, 14).compare("specular_color"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 5)
			{
				int r = atoi(container[1].c_str());
				int g = atoi(container[2].c_str());
				int b = atoi(container[3].c_str());
				int a = atoi(container[4].c_str());
				m_currentMaterialData.specular_color[0] = r;
				m_currentMaterialData.specular_color[1] = g;
				m_currentMaterialData.specular_color[2] = b;
				m_currentMaterialData.specular_color[3] = a;
				continue;
			}
		}

		if (0 == strTag.substr(0, 14).compare("emissive_color"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 5)
			{
				int r = atoi(container[1].c_str());
				int g = atoi(container[2].c_str());
				int b = atoi(container[3].c_str());
				int a = atoi(container[4].c_str());
				m_currentMaterialData.emissive_color[0] = r;
				m_currentMaterialData.emissive_color[1] = g;
				m_currentMaterialData.emissive_color[2] = b;
				m_currentMaterialData.emissive_color[3] = a;
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("shininess"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.shininess = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("thickness"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.thickness = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("alpha_ref"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.alpha_ref = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 13).compare("anti_aliasing"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			unsigned int antiAliasCnt = container.size();
			if (antiAliasCnt >= 2)
			{
				m_currentMaterialData.anti_aliasing = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_mask"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.stencil_mask = (unsigned char)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 17).compare("stencil_func_mask"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.stencil_func_mask = (unsigned char)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 11).compare("stencil_ref"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.stencil_ref = atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_func"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.stencil_func = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_fail"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.stencil_fail = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 10).compare("depth_fail"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.depth_fail = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 18).compare("stencil_depth_pass"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.stencil_depth_pass = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("wireframe"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.wireframe = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("clockwise"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.clockwise = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 15).compare("gouraud_shading"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.gouraud_shading = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 8).compare("lighting"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.lighting = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 7).compare("zbuffer"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.zbuffer = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 13).compare("zwrite_enable"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.zwrite_enable = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 14).compare("stencil_buffer"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.stencil_buffer = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 16).compare("backface_culling"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.backface_culling = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("use_mip_maps"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.use_mip_maps = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 10).compare("fog_enable"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.fog_enable = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 14).compare("vertex_program"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.vertex_program = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 16).compare("fragment_program"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.fragment_program = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 28).compare("shader_constant_set_callback"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.shader_constant_set_callback = (unsigned short)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("texture_unit"))
		{
			container.clear();
			stringtok(container, strTag, " ");

			bool hasError = false;

			if (container.size() == 1)
				curSelTexLayer = 0;
			else if (container.size() == 2)
				curSelTexLayer = atoi(container[1].c_str());
			else
				hasError = true;

			if (!hasError && curSelTexLayer >= 0 && curSelTexLayer < MATERIAL_MAX_TEXTURES)
				continue;
		}

		if (0 == strTag.substr(0, 7).compare("texture"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.texture_unit[curSelTexLayer].texture = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("filtering"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.texture_unit[curSelTexLayer].filtering = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.substr(0, 16).compare("tex_address_mode"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				m_currentMaterialData.texture_unit[curSelTexLayer].tex_address_mode = utf8ToUnicode(container[1]).c_str();
				continue;
			}
		}

		if (0 == strTag.compare("}"))
		{
			--singleMaterialParseEndMark;

			if (0 == singleMaterialParseEndMark)
			{
				if (!m_currentMaterialData.name.IsEmpty())
				{
					m_materialDataset.push_back(m_currentMaterialData);
				}

				m_currentMaterialData.reset();
				continue;
			}
			else if (singleMaterialParseEndMark > 0)
			{
				curSelTexLayer = -1;
				continue;
			}
		}

		Log::wprint(ELL_ERROR, L"材质脚本解析失败：[%s:%s]", m_currentMaterialData.name.GetString(), utf8ToUnicode(strTag).c_str());
		return false;

	} while (!filestream.eof());

	return true;
}
