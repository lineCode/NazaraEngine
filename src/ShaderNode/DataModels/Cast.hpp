#pragma once

#ifndef NAZARA_SHADERNODES_CAST_HPP
#define NAZARA_SHADERNODES_CAST_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataModels/VecValue.hpp>
#include <array>

template<typename From, typename To>
class CastVec : public ShaderNode
{
	public:
		CastVec(ShaderGraph& graph);
		~CastVec() = default;

		void BuildNodeEdition(QVBoxLayout* layout) override;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		QString caption() const override;
		QString name() const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

	private:
		static constexpr std::size_t FromComponents = From::ComponentCount;
		static constexpr std::size_t ToComponents = To::ComponentCount;
		static constexpr std::size_t ComponentDiff = (ToComponents >= FromComponents) ? ToComponents - FromComponents : 0;

		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		VecType<ComponentDiff> m_overflowComponents;
		std::shared_ptr<From> m_input;
		std::shared_ptr<To> m_output;
};

using CastVec2ToVec3 = CastVec<Vec2Data, Vec3Data>;
using CastVec2ToVec4 = CastVec<Vec2Data, Vec4Data>;
using CastVec3ToVec2 = CastVec<Vec3Data, Vec2Data>;
using CastVec3ToVec4 = CastVec<Vec3Data, Vec4Data>;
using CastVec4ToVec2 = CastVec<Vec4Data, Vec2Data>;
using CastVec4ToVec3 = CastVec<Vec4Data, Vec3Data>;

#include <ShaderNode/DataModels/Cast.inl>

#endif