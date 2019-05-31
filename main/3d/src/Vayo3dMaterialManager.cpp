#include "Vayo3dMaterialManager.h"
#include "Vayo3dTextureManager.h"
#include "Vayo3dRenderSystem.h"
#include "VayoConfigManager.h"
#include "Vayo3dRoot.h"
#include "VayoFileIO.h"
#include "VayoUtils.h"
#include "VayoLog.h"

NS_VAYO3D_BEGIN

MaterialManager::MaterialManager()
{
	memset(_materialCallback, 0, sizeof(_materialCallback));
	int i = 0;
	while (materialScriptAttribs[i].TheKey)
	{
		_attribsWordMap[materialScriptAttribs[i].TheKey] = materialScriptAttribs[i].TheValue;
		++i;
	}
	i = 0;
	while (materialScriptAttribs[i].TheKey)
	{
		_attribsWordInvMap[materialScriptAttribs[i].TheValue] = materialScriptAttribs[i].TheKey;
		++i;
	}
}

MaterialManager::~MaterialManager()
{
}

bool MaterialManager::init()
{
	_materialPool.clear();
	vector<wstring> allFilePath;
	findFileDir(allFilePath, ConfigManager::getSingleton().getConfig()._3d.materialsPath, L"material");
	unsigned len = allFilePath.size();
	for (unsigned i = 0; i < len; ++i)
		parseMaterial(allFilePath[i], true);
	return true;
}

MaterialPtr MaterialManager::createMaterial(const wstring& name /*= L""*/)
{
	static unsigned long long idx = 0;
	wstring materialName;
	if (name == L"" || name == L"default_material")
	{
		std::wstringstream ss;
		ss << L"Material" << idx;
		++idx;
		materialName = ss.str();
	}
	else
	{
		map<wstring, MaterialPtr>::iterator it = _materialPool.find(name);
		if (it != _materialPool.end())
			return it->second;
		materialName = name;
	}

	MaterialPtr materialPtr(new Material(materialName));
	_materialPool[materialPtr->_materialName] = materialPtr;
	return materialPtr;
}

MaterialPtr MaterialManager::findMaterial(const wstring& name)
{
	if (name == L"" || name == L"default_material")
		return nullptr;

	map<wstring, MaterialPtr>::iterator it = _materialPool.find(name);
	if (it != _materialPool.end())
		return it->second;

	return nullptr;
}

void MaterialManager::destroyMaterial(const wstring& name)
{
	if (name == L"")
		return;

	map<wstring, MaterialPtr>::iterator it = _materialPool.find(name);
	if (it != _materialPool.end())
		_materialPool.erase(it);
}

void MaterialManager::destroyMaterial(MaterialPtr material)
{
	if (material)
		destroyMaterial(material->_materialName);
}

void MaterialManager::clearAllMaterials()
{
	_materialPool.clear();
}

bool MaterialManager::parseMaterial(const wstring& filename, bool fullPath /*= false*/)
{
	wstring fileName = filename;
	trim(fileName);
	if (fileName == L"" || fileName.substr(fileName.rfind(L'.')) != L".material")
	{
		Log::wprint(ELL_ERROR, L"材质脚本[%s]文件扩展名必须是[.material]", fileName.c_str());
		return false;
	}

	wstring filePath;
	if (fullPath)
		filePath = fileName;
	else
		filePath = ConfigManager::getSingleton().getConfig()._3d.materialsPath + fileName;

	std::ifstream fin(filePath);
	if (!fin)
	{
		Log::wprint(ELL_ERROR, L"材质脚本[%s]打开失败", filePath.c_str());
		return false;
	}

	fin.seekg(0, ios_base::end);
	if ((int)fin.tellg() == 0)
	{
		Log::wprint(ELL_ERROR, L"材质脚本[%s]为空", filePath.c_str());
		return false;
	}

	fin.seekg(0, ios_base::beg);
	stringstream filestream;
	filestream << fin.rdbuf();
	return parseMaterial(filestream);
}

bool MaterialManager::parseMaterial(stringstream& filestream)
{
	MaterialPtr    materialPtr;
	string         materialName;
	string         strTag;
	vector<string> container;
	int            curSelTexLayer = -1;
	int            singleMaterialParseEndMark = 0;
	wstring        shadersPath = ConfigManager::getSingleton().getConfig()._3d.shadersPath;

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
				materialName = container[1];
				materialPtr = MaterialPtr(new Material(utf8ToUnicode(materialName)));
				continue;
			}
		}

		if (0 == strTag.substr(0, 4).compare("type"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					materialPtr->_materialType = (EMaterialType)it->second;
					continue;
				}
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

		if (0 == strTag.substr(0, 13).compare("anti_aliasing"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			unsigned int antiAliasCnt = container.size();
			if (antiAliasCnt >= 2)
			{
				bool isok = true;
				materialPtr->_antiAliasing = EAAM_OFF;
				for (int i = antiAliasCnt - 1; i >= 1; --i)
				{
					unordered_map<string, int>::iterator it = _attribsWordMap.find(container[i]);
					if (it != _attribsWordMap.end())
						materialPtr->_antiAliasing |= (unsigned char)it->second;
					else
					{
						isok = false;
						break;
					}
				}

				if (isok)
					continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_mask"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilMask = (unsigned char)atoi(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 17).compare("stencil_func_mask"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_stencilFuncMask = (unsigned char)atoi(container[1].c_str());
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

		if (0 == strTag.substr(0, 12).compare("stencil_func"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					materialPtr->_stencilFunc = (EStencilFunc)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 12).compare("stencil_fail"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					materialPtr->_stencilFail = (EStencilOp)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 10).compare("depth_fail"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					materialPtr->_depthFail = (EStencilOp)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 18).compare("stencil_depth_pass"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					materialPtr->_stencilDepthPass = (EStencilOp)it->second;
					continue;
				}
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

			if (!hasError && curSelTexLayer >= 0 && curSelTexLayer < MATERIAL_MAX_TEXTURES)
				continue;
		}

		if (0 == strTag.substr(0, 7).compare("texture"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				materialPtr->_textureLayer[curSelTexLayer]._texture =
					Root::getSingleton().getTextureManager()->getTexture(utf8ToUnicode(container[1].c_str()));
				continue;
			}
		}

		if (0 == strTag.substr(0, 9).compare("filtering"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					bool isok = false;
					int mark = it->second;
					switch (mark)
					{
					case 1:
						materialPtr->_textureLayer[curSelTexLayer]._trilinearFilter = false;
						materialPtr->_textureLayer[curSelTexLayer]._bilinearFilter = false;
						isok = true;
						break;
					case 2:
						materialPtr->_textureLayer[curSelTexLayer]._trilinearFilter = false;
						materialPtr->_textureLayer[curSelTexLayer]._bilinearFilter = true;
						isok = true;
						break;
					case 3:
						materialPtr->_textureLayer[curSelTexLayer]._trilinearFilter = true;
						materialPtr->_textureLayer[curSelTexLayer]._bilinearFilter = false;
						isok = true;
						break;
					default:
						break;
					}

					if (isok)
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
				for (int i = 0; i < ETC_MIRROR_CLAMP_TO_BORDER + 1; ++i)
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

		Log::wprint(ELL_ERROR, L"材质脚本解析失败：[%s:%s]", utf8ToUnicode(materialName).c_str(), utf8ToUnicode(strTag).c_str());
		return false;

	} while (!filestream.eof());

	return true;
}

void MaterialManager::saveMaterial(const wstring& name, stringstream& filestream)
{
	saveMaterial(findMaterial(name), filestream);
}

void MaterialManager::saveMaterial(MaterialPtr material, stringstream& filestream)
{
	if (!material)
		return;

	filestream << "material " << w2a_(material->_materialName);
	filestream << "\r\n{";

	if (material->_materialType != EMT_SOLID)
		filestream << "\r\n\ttype " << _attribsWordInvMap[material->_materialType];

	if (material->_ambientColor != Colour(255, 255, 255, 255))
		filestream << "\r\n\tambient_color " << material->_ambientColor.getRed()
		<< " " << material->_ambientColor.getGreen() << " "
		<< material->_ambientColor.getBlue() << " " << material->_ambientColor.getAlpha();

	if (material->_diffuseColor != Colour(255, 255, 255, 255))
		filestream << "\r\n\tdiffuse_color " << material->_diffuseColor.getRed()
		<< " " << material->_diffuseColor.getGreen() << " "
		<< material->_diffuseColor.getBlue() << " " << material->_diffuseColor.getAlpha();

	if (material->_specularColor != Colour(0, 0, 0, 0))
		filestream << "\r\n\tspecular_color " << material->_specularColor.getRed()
		<< " " << material->_specularColor.getGreen() << " "
		<< material->_specularColor.getBlue() << " " << material->_specularColor.getAlpha();

	if (material->_emissiveColor != Colour(0, 0, 0, 0))
		filestream << "\r\n\temissive_color " << material->_emissiveColor.getRed()
		<< " " << material->_emissiveColor.getGreen() << " "
		<< material->_emissiveColor.getBlue() << " " << material->_emissiveColor.getAlpha();

	if (material->_shininess != 0.0f)
		filestream << "\r\n\tshininess " << material->_shininess;

	if (material->_thickness != 1.0f)
		filestream << "\r\n\tthickness " << material->_thickness;

	if (material->_alphaRef != 0.0f)
		filestream << "\r\n\talpha_ref " << material->_alphaRef;

	if (material->_antiAliasing != EAAM_ON)
		filestream << "\r\n\tanti_aliasing " << _attribsWordInvMap[material->_antiAliasing];

	if (material->_stencilMask != 0xFF)
		filestream << "\r\n\tstencil_mask " << material->_stencilMask;

	if (material->_stencilFuncMask != 0xFF)
		filestream << "\r\n\tstencil_func_mask " << material->_stencilFuncMask;

	if (material->_stencilRef != 0)
		filestream << "\r\n\tstencil_ref " << material->_stencilRef;

	if (material->_stencilFunc != ESF_NEVER)
		filestream << "\r\n\tstencil_func " << _attribsWordInvMap[material->_stencilFunc];

	if (material->_stencilFail != ESO_KEEP)
		filestream << "\r\n\tstencil_fail " << _attribsWordInvMap[material->_stencilFail];

	if (material->_depthFail != ESO_KEEP)
		filestream << "\r\n\tdepth_fail " << _attribsWordInvMap[material->_depthFail];

	if (material->_stencilDepthPass != ESO_KEEP)
		filestream << "\r\n\tstencil_depth_pass " << _attribsWordInvMap[material->_stencilDepthPass];

	if (material->_wireframe != false)
		filestream << "\r\n\twireframe " << "on";

	if (material->_clockwise != false)
		filestream << "\r\n\tclockwise " << "on";

	if (material->_gouraudShading != true)
		filestream << "\r\n\tgouraud_shading " << "off";

	if (material->_lighting != true)
		filestream << "\r\n\tlighting " << "off";

	if (material->_zBuffer != true)
		filestream << "\r\n\tzbuffer " << "off";

	if (material->_zWriteEnable != true)
		filestream << "\r\n\tzwrite_enable " << "off";

	if (material->_stencilBuffer != false)
		filestream << "\r\n\tstencil_buffer " << "on";

	if (material->_backfaceCulling != true)
		filestream << "\r\n\tbackface_culling " << "off";

	if (material->_useMipMaps != true)
		filestream << "\r\n\tuse_mip_maps " << "off";

	if (material->_fogEnable != false)
		filestream << "\r\n\tfog_enable " << "on";

	if (material->_vertexSource != L"")
		filestream << "\r\n\tvertex_program " << w2a_(material->_vertexSource);

	if (material->_fragmentSource != L"")
		filestream << "\r\n\tfragment_program " << w2a_(material->_fragmentSource);

	if (material->_shaderCallback != 0)
		filestream << "\r\n\tshader_constant_set_callback " << material->_shaderCallback;

	for (int i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		if (material->_textureLayer[i]._texture)
		{
			filestream << "\r\n\r\n\ttexture_unit " << i;
			filestream << "r\n\t{\r\n\t\ttexture " << w2a_(material->_textureLayer[i]._texture->getFileName());

			if (material->_textureLayer[i]._trilinearFilter)
				filestream << "\r\n\t\tfiltering " << "trilinear_filter";
			else if (!material->_textureLayer[i]._bilinearFilter)
				filestream << "\r\n\t\tfiltering " << "nearest_filter";

			if (material->_textureLayer[i]._textureWrapU != ETC_REPEAT)
				filestream << "\r\n\t\ttex_address_mode " << aTextureClampNames[material->_textureLayer[i]._textureWrapU];

			filestream << "\r\n\t}";
		}
	}

	filestream << "\r\n}\r\n";
}

void MaterialManager::saveMaterial(const wstring& name, const wstring& filename, bool append /*= true*/, bool fullPath /*= false*/)
{
	saveMaterial(findMaterial(name), filename, append, fullPath);
}

void MaterialManager::saveMaterial(MaterialPtr material, const wstring& filename, bool append /*= true*/, bool fullPath /*= false*/)
{
	if (!material)
		return;

	wstring fileName = filename;
	trim(fileName);
	if (fileName == L"" || fileName.substr(fileName.rfind(L'.')) != L".material")
	{
		Log::wprint(ELL_ERROR, L"材质脚本[%s]文件扩展名必须是[.material]", fileName.c_str());
		return;
	}

	wstring filePath;
	if (fullPath)
		filePath = fileName;
	else
		filePath = ConfigManager::getSingleton().getConfig()._3d.materialsPath + fileName;

	WriteFile fout(filePath, append);
	stringstream ss;
	saveMaterial(material, ss);
	string string = ansiToUtf8(ss.str());
	fout.write(string.data(), string.size());
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

NS_VAYO3D_END
