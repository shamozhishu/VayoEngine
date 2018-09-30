#include "VayoMaterialManager.h"
#include "VayoTextureManager.h"
#include "VayoConfigManager.h"
#include "VayoRenderSystem.h"
#include "VayoUtils.h"
#include "VayoRoot.h"
#include "VayoLog.h"

NS_VAYO_BEGIN

MaterialManager::MaterialManager()
{
	memset(_materialCallback, 0, sizeof(_materialCallback));
}

MaterialManager::~MaterialManager()
{
}

bool MaterialManager::init()
{
	_materialPool.clear();
	MaterialPtr defaultMaterialPtr(new Material());
	_defaultMaterial = defaultMaterialPtr;

	wstring materialsPath = Root::getSingleton().getConfigManager()->getSceneConfig().MaterialsPath;
	vector<wstring> allFilePath;
	findFileDir(allFilePath, materialsPath, L"material");
	unsigned len = allFilePath.size();

	for (unsigned i = 0; i < len; i++)
	{
		parseMaterial(allFilePath[i]);
	}

	return true;
}

MaterialPtr MaterialManager::createMaterial(const wstring& name /*= L""*/)
{
	static unsigned short idx = 0;
	MaterialPtr materialPtr(new Material());
	if (0 == name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"Material" << idx;
		++idx;
		materialPtr->_materialName = ss.str();
	}
	_materialPool[materialPtr->_materialName] = materialPtr;
	return materialPtr;
}

MaterialPtr MaterialManager::findMaterial(const wstring& name)
{
	map<wstring, MaterialPtr>::iterator it = _materialPool.find(name);
	if (it != _materialPool.end())
		return it->second;
	return _defaultMaterial;
}

void MaterialManager::destroyMaterial(const wstring& name)
{
	if (name == L"")
		return;
	map<wstring, MaterialPtr>::iterator it = _materialPool.find(name);
	if (it != _materialPool.end())
		_materialPool.erase(it);
}

void MaterialManager::destroyMaterial(const MaterialPtr& ptr)
{
	if (ptr)
		destroyMaterial(ptr->_materialName);
}

MaterialPtr MaterialManager::getDefaultMaterial() const
{
	return _defaultMaterial;
}

void MaterialManager::registerCallback(unsigned int idx, ShaderConstantSetCallback callback)
{
	if (idx >= VAYO_MAX_SHADER_CALLBACK_NUM)
		return;
	_materialCallback[idx] = callback;
}

void MaterialManager::unregisterCallback(unsigned int idx)
{
	registerCallback(idx, NULL);
}

bool MaterialManager::parseMaterial(const wstring& filePath)
{
	std::ifstream fin(filePath);
	if (!fin)
	{
		Log::wprint(ELL_ERROR, L"打开材质脚本失败：%s", filePath.c_str());
		return false;
	}

	MaterialPtr    materialPtr;
	string         materialName;
	string         strTag;
	vector<string> container;
	int            curSelTexLayer = -1;
	int            singleMaterialParseEndMark = 0;
	wstring        texturePath = Root::getSingleton().getConfigManager()->getSceneConfig().TexturesPath;
	wstring        shadersPath = Root::getSingleton().getConfigManager()->getSceneConfig().ShadersPath;

	do
	{
		std::getline(fin, strTag);
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
				materialName = container[1];
				materialPtr = MaterialPtr(new Material());
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
				materialPtr->_ambientColor.set(a, r, g, b);
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
				materialPtr->_diffuseColor.set(a, r, g, b);
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
				materialPtr->_specularColor.set(a, r, g, b);
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
				materialPtr->_emissiveColor.set(a, r, g, b);
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("shininess"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_shininess = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("thickness"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_thickness = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("alpha_ref"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_alphaRef = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_func"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilFunc = (EStencilFunc)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 11).compare("stencil_ref"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilRef = atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_mask"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilMask = (unsigned int)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 17).compare("stencil_func_mask"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilFuncMask = (unsigned int)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_fail"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilFail = (EStencilOp)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 10).compare("depth_fail"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_depthFail = (EStencilOp)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 18).compare("stencil_depth_pass"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilDepthPass = (EStencilOp)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("wireframe"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_wireframe = (container[1] == "on" ? true : false);
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("clockwise"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_clockwise = (container[1] == "on" ? true : false);
				continue;
			}
		}

		if (0 == strTag.substr(0, 15).compare("gouraud_shading"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_gouraudShading = (container[1] == "off" ? false : true);
				continue;
			}
		}

		if (0 == strTag.substr(0, 8).compare("lighting"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_lighting = (container[1] == "off" ? false : true);
				continue;
			}
		}

		if (0 == strTag.substr(0, 7).compare("zbuffer"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_zBuffer = (container[1] == "off" ? false : true);
				continue;
			}
		}

		if (0 == strTag.substr(0, 13).compare("zwrite_enable"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_zWriteEnable = (container[1] == "off" ? false : true);
				continue;
			}
		}

		if (0 == strTag.substr(0, 14).compare("stencil_buffer"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilBuffer = (container[1] == "on" ? true : false);
				continue;
			}
		}

		if (0 == strTag.substr(0, 16).compare("backface_culling"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_backfaceCulling = (container[1] == "off" ? false : true);
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("use_mip_maps"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_useMipMaps = (container[1] == "off" ? false : true);
				continue;
			}
		}

		if (0 == strTag.substr(0, 10).compare("fog_enable"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_fogEnable = (container[1] == "on" ? true : false);
				continue;
			}
		}

		if (0 == strTag.substr(0, 14).compare("vertex_program"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_vertexSource = utf8ToUnicode(container[1]);
				continue;
			}
		}

		if (0 == strTag.substr(0, 16).compare("fragment_program"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_fragmentSource = utf8ToUnicode(container[1]);
				continue;
			}
		}

		if (0 == strTag.substr(0, 28).compare("shader_constant_set_callback"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_shaderCallback = (unsigned int)atoi(container[1].c_str());
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

			if (!hasError && curSelTexLayer >=0 && curSelTexLayer < MATERIAL_MAX_TEXTURES)
				continue;
		}

		if (0 == strTag.substr(0, 7).compare("texture"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_textureLayer[curSelTexLayer]._texture =
					Root::getSingleton().getTextureManager()->getTexture(texturePath + utf8ToUnicode(container[1].c_str()));
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("filtering"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				strTag = container[1];
				if (0 == strTag.compare("trilinear_filter"))
				{
					materialPtr->_textureLayer[curSelTexLayer]._trilinearFilter = true;
					materialPtr->_textureLayer[curSelTexLayer]._bilinearFilter = false;
					continue;
				}
				else if (0 == strTag.compare("bilinear_filter"))
				{
					materialPtr->_textureLayer[curSelTexLayer]._trilinearFilter = false;
					materialPtr->_textureLayer[curSelTexLayer]._bilinearFilter = true;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 16).compare("tex_address_mode"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				bool hasFinded = false;
				strTag = container[1];
				for (int i = 0; i < ETC_MIRROR_CLAMP_TO_BORDER+1; ++i)
				{
					if (0 == strTag.compare(aTextureClampNames[i]))
					{
						materialPtr->_textureLayer[curSelTexLayer]._textureWrapU = (ETextureClamp)i;
						materialPtr->_textureLayer[curSelTexLayer]._textureWrapV = (ETextureClamp)i;
						hasFinded = true;
						break;
					}
				}

				if (hasFinded)
					continue;
			}
		}

		if (0 == strTag.compare("}"))
		{
			--singleMaterialParseEndMark;

			if (0 == singleMaterialParseEndMark)
			{
				if (materialName != "")
				{
					materialPtr->_materialName = utf8ToUnicode(materialName);

					if (materialPtr->_vertexSource != L"" && materialPtr->_fragmentSource != L"")
					{
						RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
						int curMaterialType = pRenderer->addShaderMaterialFromFiles(
							shadersPath + materialPtr->_vertexSource,
							shadersPath + materialPtr->_fragmentSource,
							&(_materialCallback[materialPtr->_shaderCallback]),
							materialPtr->_materialType);

						if (curMaterialType != -1)
							materialPtr->_materialType = (EMaterialType)curMaterialType;
						else
							Log::wprint(ELL_WARNING, L"着色器材质创建失败,默认使用初始材质类型(%s)",
								materialPtr->_materialName.c_str());
					}

					_materialPool[materialPtr->_materialName] = materialPtr;
				}

				materialName.clear();
				materialPtr = NULL;
				continue;
			}
			else if (singleMaterialParseEndMark > 0)
			{
				curSelTexLayer = -1;
				continue;
			}
		}
		
		Log::wprint(ELL_ERROR, L"解析材质脚本失败：%s", filePath.c_str());
		return false;

	} while (!fin.eof());

	return true;
}

NS_VAYO_END
