/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* OpenGL²ÄÖÊäÖÈ¾
\*************************************************************************/
#ifndef __GL_MATERIAL_RENDERER_H__
#define __GL_MATERIAL_RENDERER_H__

#include "GLSupport.h"
#include "VayoLog.h"
#include "Vayo3dMaterialRenderer.h"
#include "GLRenderSystem.h"

class GLMaterialRenderer : public MaterialRenderer
{
public:
	GLMaterialRenderer(GLRenderSystem* rs) : _glRenderSys(rs) {}
protected:
	GLRenderSystem* _glRenderSys;
};
//------------------------------------------------------------------------
class GLMaterialRenderer_SOLID : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_SOLID(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(1);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_SOLID_2_LAYER : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_SOLID_2_LAYER(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(2);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			glActiveTexture(GL_TEXTURE1);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
		}
	}

	virtual void onUnsetMaterial()
	{
		glActiveTexture(GL_TEXTURE1);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
		glActiveTexture(GL_TEXTURE0);
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_TRANSPARENT_ADD_COLOR : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_TRANSPARENT_ADD_COLOR(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(1);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glEnable(GL_BLEND);
		}
	}

	virtual void onUnsetMaterial()
	{
		glDisable(GL_BLEND);
	}

	virtual bool isTransparent() const
	{
		return true;
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(1);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
		}
	}

	virtual void onUnsetMaterial()
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
		glDisable(GL_BLEND);
	}

	virtual bool isTransparent() const
	{
		return true;
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(1);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates
			|| material._alphaRef != lastMaterial._alphaRef)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, material._alphaRef);
		}
	}

	virtual void onUnsetMaterial()
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
	}

	virtual bool isTransparent() const
	{
		return true;
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_LIGHTMAP : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_LIGHTMAP(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(2);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			// diffuse map

			switch (material._materialType)
			{
			case EMT_LIGHTMAP_LIGHTING:
			case EMT_LIGHTMAP_LIGHTING_M2:
			case EMT_LIGHTMAP_LIGHTING_M4:
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				break;
			case EMT_LIGHTMAP_ADD:
			case EMT_LIGHTMAP:
			case EMT_LIGHTMAP_M2:
			case EMT_LIGHTMAP_M4:
			default:
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				break;
			}

			// lightmap

			glActiveTexture(GL_TEXTURE1);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

			if (material._materialType == EMT_LIGHTMAP_ADD)
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
			else
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_PREVIOUS);

			switch (material._materialType)
			{
			case EMT_LIGHTMAP_M4:
			case EMT_LIGHTMAP_LIGHTING_M4:
				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 4.0f);
				break;
			case EMT_LIGHTMAP_M2:
			case EMT_LIGHTMAP_LIGHTING_M2:
				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 2.0f);
				break;
			default:
				glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
			}

			glActiveTexture(GL_TEXTURE0);
		}
	}

	virtual void onUnsetMaterial()
	{
		glActiveTexture(GL_TEXTURE1);
		glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0f);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glActiveTexture(GL_TEXTURE0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_SPHERE_MAP : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_SPHERE_MAP(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(1);
		// texture needs to be flipped for OpenGL
		Matrix4x4 tmp = _glRenderSys->getTransform(ETS_TEXTURE_0);
		tmp[5] *= -1;
		_glRenderSys->setTransform(ETS_TEXTURE_0, tmp);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		}
	}

	virtual void onUnsetMaterial()
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_REFLECTION_2_LAYER : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_REFLECTION_2_LAYER(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(2);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			glActiveTexture(GL_TEXTURE1);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		}
	}

	virtual void onUnsetMaterial()
	{
		glActiveTexture(GL_TEXTURE1);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glActiveTexture(GL_TEXTURE0);
	}
};
//------------------------------------------------------------------------
class GLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER : public GLMaterialRenderer
{
public:
	GLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(GLRenderSystem* rs) : GLMaterialRenderer(rs) {}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		_glRenderSys->disableTextures(2);
		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);

			glActiveTexture(GL_TEXTURE1);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);

			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
		}
	}

	virtual void onUnsetMaterial()
	{
		glActiveTexture(GL_TEXTURE1);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_BLEND);
	}

	virtual bool isTransparent() const
	{
		return true;
	}
};
//------------------------------------------------------------------------
class GLShaderMaterialRenderer : public GLMaterialRenderer, public MaterialRendererServices
{
public:
	GLShaderMaterialRenderer(GLRenderSystem* rs, bool& succ,
		const char* vertexShaderProgram,
		const char* pixelShaderProgram,
		ShaderConstantSetCallback* callback, MaterialRenderer* baseMaterial)
		: GLMaterialRenderer(rs)
		, _callback(callback)
		, _baseMaterial(baseMaterial)
		, _shaderProgram(0)
	{
		succ = false;

		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexShaderProgram, NULL);
		glCompileShader(vertex);
		bool vertexSucc = checkCompileErrors(vertex, "VERTEX");
		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &pixelShaderProgram, NULL);
		glCompileShader(fragment);
		bool fragmentSucc = checkCompileErrors(fragment, "FRAGMENT");
		_shaderProgram = glCreateProgram();
		glAttachShader(_shaderProgram, vertex);
		glAttachShader(_shaderProgram, fragment);
		glLinkProgram(_shaderProgram);
		bool linkSucc = checkCompileErrors(_shaderProgram, "PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		succ = (vertexSucc && fragmentSucc && linkSucc);
	}

	~GLShaderMaterialRenderer()
	{
		glDeleteProgram(_shaderProgram);
	}

	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates)
	{
		glUseProgram(_shaderProgram);

		if (material._materialType != lastMaterial._materialType || resetAllRenderstates)
		{
			if (_callback && (*_callback))
				(*_callback)(this);

			if (_baseMaterial)
				_baseMaterial->onSetMaterial(material, material, true);
		}

		_glRenderSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
	}

	virtual void onUnsetMaterial()
	{
		glUseProgram(0);
		if (_baseMaterial)
			_baseMaterial->onUnsetMaterial();
	}

	virtual bool isTransparent() const
	{
		return _baseMaterial ? _baseMaterial->isTransparent() : false;
	}

	void setShaderConstant(const string &name, bool value)
	{
		glUniform1i(glGetUniformLocation(_shaderProgram, name.c_str()), (int)value);
	}

	void setShaderConstant(const string &name, int value)
	{
		glUniform1i(glGetUniformLocation(_shaderProgram, name.c_str()), value);
	}

	void setShaderConstant(const string &name, float value)
	{
		glUniform1f(glGetUniformLocation(_shaderProgram, name.c_str()), value);
	}

	void setShaderConstant(const string &name, double value)
	{
		glUniform1d(glGetUniformLocation(_shaderProgram, name.c_str()), value);
	}

	void setShaderConstant(const string &name, const Vector2df& value)
	{
		glUniform2fv(glGetUniformLocation(_shaderProgram, name.c_str()), 1, &value._x);
	}

	void setShaderConstant(const string &name, float x, float y)
	{
		glUniform2f(glGetUniformLocation(_shaderProgram, name.c_str()), x, y);
	}

	void setShaderConstant(const string &name, const Vector3df& value)
	{
		glUniform3fv(glGetUniformLocation(_shaderProgram, name.c_str()), 1, &value._x);
	}

	void setShaderConstant(const string &name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(_shaderProgram, name.c_str()), x, y, z);
	}

	void setShaderConstant(const string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(_shaderProgram, name.c_str()), x, y, z, w);
	}

	void setShaderConstant(const string &name, const Matrix4x4& value)
	{
		glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, name.c_str()), 1, GL_FALSE, value.pointer());
	}

private:
	bool checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				Log::print(ELL_ERROR, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s", type.c_str(), infoLog);
				return false;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				Log::print(ELL_ERROR, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s", type.c_str(), infoLog);
				return false;
			}
		}
		return true;
	}

protected:
	GLuint                     _shaderProgram;
	MaterialRenderer*          _baseMaterial;
	ShaderConstantSetCallback* _callback;
};

#endif // __GL_MATERIAL_RENDERER_H__
