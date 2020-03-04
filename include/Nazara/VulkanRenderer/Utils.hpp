// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILS_VULKAN_HPP
#define NAZARA_UTILS_VULKAN_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>

namespace Nz
{
	inline VkFormat ToVulkan(ComponentType componentType);
	inline VkCullModeFlagBits ToVulkan(FaceSide faceSide);
	inline VkPolygonMode ToVulkan(FaceFilling faceFilling);
	inline VkPrimitiveTopology ToVulkan(PrimitiveMode primitiveMode);
	inline VkCompareOp ToVulkan(RendererComparison comparison);
	inline VkShaderStageFlagBits ToVulkan(ShaderStageType stageType);
	inline VkStencilOp ToVulkan(StencilOperation stencilOp);
	inline VkVertexInputRate ToVulkan(VertexInputRate inputRate);

	NAZARA_VULKANRENDERER_API String TranslateVulkanError(VkResult code);
}

#include <Nazara/VulkanRenderer/Utils.inl>

#endif // NAZARA_UTILS_VULKAN_HPP