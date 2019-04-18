// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPipelineSettings.hpp>
#include <Nazara/Renderer/UberShaderPreprocessor.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_basicFragmentShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/Basic/core.frag.h>
		};

		const UInt8 r_basicVertexShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/Basic/core.vert.h>
		};

		const UInt8 r_phongLightingFragmentShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhongLighting/core.frag.h>
		};

		const UInt8 r_phongLightingVertexShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhongLighting/core.vert.h>
		};

		void OverrideShader(const String& path, String* source)
		{
			ErrorFlags errFlags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled);

			File shaderFile(path, Nz::OpenMode_ReadOnly | Nz::OpenMode_Text);
			if (shaderFile.IsOpen())
			{
				StringStream shaderSource;

				while (!shaderFile.EndOfFile())
				{
					shaderSource << shaderFile.ReadLine();
					shaderSource << '\n';
				}

				*source = shaderSource;

				NazaraNotice(path + " will be used to override built-in shader");
			}
		}
	}

	/*!
	* \ingroup graphics
	* \class Nz::MaterialPipeline
	*
	* \brief Graphics class used to contains all rendering states that are not allowed to change individually on rendering devices
	*/

	/*!
	* \brief Returns a reference to a MaterialPipeline built with MaterialPipelineInfo
	*
	* This function is using a cache, calling it multiples times with the same MaterialPipelineInfo will returns references to a single MaterialPipeline
	*
	* \param pipelineInfo Pipeline informations used to build/retrieve a MaterialPipeline object
	*/
	MaterialPipelineRef MaterialPipeline::GetPipeline(const MaterialPipelineInfo& pipelineInfo)
	{
		auto it = s_pipelineCache.find(pipelineInfo);
		if (it == s_pipelineCache.end())
			it = s_pipelineCache.insert(it, PipelineCache::value_type(pipelineInfo, New(pipelineInfo)));

		return it->second;
	}

	void MaterialPipeline::GenerateRenderPipeline(UInt32 flags) const
	{
		NazaraAssert(m_pipelineInfo.uberShader, "Material pipeline has no uber shader");

		ParameterList list;
		for (std::size_t i = 0, texCount = m_pipelineInfo.settings->textures.size(); i < texCount; ++i)
		{
			const auto& texture = m_pipelineInfo.settings->textures[i];
			String parameterName = "HAS_" + texture.name.ToUpper() + "_TEXTURE";

			list.SetParameter(parameterName, (m_pipelineInfo.textures & (1 << i)) != 0);
		}

		list.SetParameter("ALPHA_TEST",         m_pipelineInfo.alphaTest);
		list.SetParameter("REFLECTION_MAPPING", m_pipelineInfo.reflectionMapping);
		list.SetParameter("SHADOW_MAPPING",     m_pipelineInfo.shadowReceive);
		list.SetParameter("TEXTURE_MAPPING",    m_pipelineInfo.textures != 0 || 
		                                        m_pipelineInfo.reflectionMapping || flags & ShaderFlags_TextureOverlay);
		list.SetParameter("TRANSFORM",          true);

		list.SetParameter("FLAG_BILLBOARD",      static_cast<bool>((flags & ShaderFlags_Billboard) != 0));
		list.SetParameter("FLAG_DEFERRED",       static_cast<bool>((flags & ShaderFlags_Deferred) != 0));
		list.SetParameter("FLAG_INSTANCING",     static_cast<bool>((flags & ShaderFlags_Instancing) != 0));
		list.SetParameter("FLAG_TEXTUREOVERLAY", static_cast<bool>((flags & ShaderFlags_TextureOverlay) != 0));
		list.SetParameter("FLAG_VERTEXCOLOR",    m_pipelineInfo.hasVertexColor || static_cast<bool>((flags & ShaderFlags_VertexColor) != 0));

		Instance& instance = m_instances[flags];
		instance.uberInstance = m_pipelineInfo.uberShader->Get(list);

		RenderPipelineInfo renderPipelineInfo;
		static_cast<RenderStates&>(renderPipelineInfo).operator=(m_pipelineInfo); // Not my proudest line

		renderPipelineInfo.shader = instance.uberInstance->GetShader();

		instance.renderPipeline.Create(renderPipelineInfo);

		// Send texture units (those never changes)
		instance.bindings = renderPipelineInfo.shader->ApplyLayout(m_pipelineInfo.pipelineLayout);

		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("ReflectionMap"), Material::GetTextureUnit(TextureMap_ReflectionCube));
		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("TextureOverlay"), Material::GetTextureUnit(TextureMap_Overlay));

		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("DirectionalSpotLightShadowMap[0]"), Material::GetTextureUnit(TextureMap_Shadow2D_1));
		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("DirectionalSpotLightShadowMap[1]"), Material::GetTextureUnit(TextureMap_Shadow2D_2));
		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("DirectionalSpotLightShadowMap[2]"), Material::GetTextureUnit(TextureMap_Shadow2D_3));

		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("PointLightShadowMap[0]"), Material::GetTextureUnit(TextureMap_ShadowCube_1));
		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("PointLightShadowMap[1]"), Material::GetTextureUnit(TextureMap_ShadowCube_2));
		renderPipelineInfo.shader->SendInteger(renderPipelineInfo.shader->GetUniformLocation("PointLightShadowMap[2]"), Material::GetTextureUnit(TextureMap_ShadowCube_3));
	}

	bool MaterialPipeline::Initialize()
	{
		// Basic shader
		{
			UberShaderPreprocessorRef uberShader = UberShaderPreprocessor::New();

			String fragmentShader(reinterpret_cast<const char*>(r_basicFragmentShader), sizeof(r_basicFragmentShader));
			String vertexShader(reinterpret_cast<const char*>(r_basicVertexShader), sizeof(r_basicVertexShader));

			#ifdef NAZARA_DEBUG
			OverrideShader("Shaders/Basic/core.frag", &fragmentShader);
			OverrideShader("Shaders/Basic/core.vert", &vertexShader);
			#endif

			uberShader->SetShader(ShaderStageType_Fragment, fragmentShader, "FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING TEXTURE_MAPPING");
			uberShader->SetShader(ShaderStageType_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_INSTANCING FLAG_VERTEXCOLOR TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

			UberShaderLibrary::Register("Basic", uberShader);
		}

		// PhongLighting shader
		{
			UberShaderPreprocessorRef uberShader = UberShaderPreprocessor::New();

			String fragmentShader(reinterpret_cast<const char*>(r_phongLightingFragmentShader), sizeof(r_phongLightingFragmentShader));
			String vertexShader(reinterpret_cast<const char*>(r_phongLightingVertexShader), sizeof(r_phongLightingVertexShader));

			#ifdef NAZARA_DEBUG
			OverrideShader("Shaders/PhongLighting/core.frag", &fragmentShader);
			OverrideShader("Shaders/PhongLighting/core.vert", &vertexShader);
			#endif

			uberShader->SetShader(ShaderStageType_Fragment, fragmentShader, "FLAG_DEFERRED FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING EMISSIVE_MAPPING NORMAL_MAPPING PARALLAX_MAPPING REFLECTION_MAPPING SHADOW_MAPPING SPECULAR_MAPPING");
			uberShader->SetShader(ShaderStageType_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_DEFERRED FLAG_INSTANCING FLAG_VERTEXCOLOR COMPUTE_TBNMATRIX PARALLAX_MAPPING SHADOW_MAPPING TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

			UberShaderLibrary::Register("PhongLighting", uberShader);
		}

		// Once the base shaders are registered, we can now set some default materials
		MaterialPipelineInfo pipelineInfo;
		pipelineInfo.uberShader = UberShaderLibrary::Get("Basic");

		// Basic 2D - No depth write/face culling with scissoring
		pipelineInfo.depthWrite = false;
		pipelineInfo.faceCulling = false;
		pipelineInfo.scissorTest = true;

		MaterialPipelineLibrary::Register("Basic2D", GetPipeline(pipelineInfo));

		// Translucent 2D - Alpha blending with no depth write/face culling and scissoring
		pipelineInfo.blending = true;
		pipelineInfo.depthWrite = false;
		pipelineInfo.faceCulling = false;
		pipelineInfo.depthSorting = false;
		pipelineInfo.scissorTest = true;
		pipelineInfo.dstBlend = BlendFunc_InvSrcAlpha;
		pipelineInfo.srcBlend = BlendFunc_SrcAlpha;

		MaterialPipelineLibrary::Register("Translucent2D", GetPipeline(pipelineInfo));

		// Translucent 3D - Alpha blending with depth buffer and no depth write/face culling
		pipelineInfo.blending = true;
		pipelineInfo.depthBuffer = true;
		pipelineInfo.depthWrite = false;
		pipelineInfo.faceCulling = false;
		pipelineInfo.depthSorting = true;
		pipelineInfo.scissorTest = false;
		pipelineInfo.dstBlend = BlendFunc_InvSrcAlpha;
		pipelineInfo.srcBlend = BlendFunc_SrcAlpha;

		MaterialPipelineLibrary::Register("Translucent3D", GetPipeline(pipelineInfo));

		return true;
	}

	void MaterialPipeline::Uninitialize()
	{
		s_pipelineCache.clear();
		UberShaderLibrary::Unregister("PhongLighting");
		UberShaderLibrary::Unregister("Basic");
		MaterialPipelineLibrary::Uninitialize();
	}

	MaterialPipelineLibrary::LibraryMap MaterialPipeline::s_library;
	MaterialPipeline::PipelineCache MaterialPipeline::s_pipelineCache;
}
