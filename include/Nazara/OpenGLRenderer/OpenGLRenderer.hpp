// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_HPP
#define NAZARA_OPENGLRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Renderer/RendererImpl.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <memory>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderer : public RendererImpl
	{
		public:
			OpenGLRenderer() = default;
			~OpenGLRenderer();

			std::unique_ptr<RenderSurface> CreateRenderSurfaceImpl() override;
			std::unique_ptr<RenderWindowImpl> CreateRenderWindowImpl() override;

			std::shared_ptr<RenderDevice> InstanciateRenderDevice(std::size_t deviceIndex) override;

			bool IsBetterThan(const RendererImpl* other) const override;

			RenderAPI QueryAPI() const override;
			std::string QueryAPIString() const override;
			UInt32 QueryAPIVersion() const override;
			std::vector<RenderDeviceInfo> QueryRenderDevices() const override;

			bool Prepare(const ParameterList& parameters) override;

		private:
			DynLib m_opengl32Lib;
			std::shared_ptr<OpenGLDevice> m_device;
			std::unique_ptr<GL::Loader> m_loader;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderer.inl>

#endif // NAZARA_OPENGLRENDERER_HPP