#pragma once

#ifndef NAZARA_SHADERNODES_SHADERNODE_HPP
#define NAZARA_SHADERNODES_SHADERNODE_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/NodeDataModel>
#include <QtGui/QPixmap>
#include <optional>

class QLabel;
class QFormLayout;
class ShaderGraph;

class ShaderNode : public QtNodes::NodeDataModel
{
	public:
		ShaderNode(ShaderGraph& graph);

		virtual void BuildNodeEdition(QFormLayout* layout);

		void EnablePreview(bool enable);

		virtual Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const = 0;
		inline ShaderGraph& GetGraph();
		inline const ShaderGraph& GetGraph() const;
		inline const std::string& GetVariableName() const;

		inline void SetPreviewSize(const Nz::Vector2i& size);
		inline void SetVariableName(std::string variableName);

		QWidget* embeddedWidget() final;

		void setInData(std::shared_ptr<QtNodes::NodeData>, int) override;

	protected:
		inline void DisableCustomVariableName();
		inline void EnableCustomVariableName(bool enable = true);
		void UpdatePreview();

	private:
		virtual bool ComputePreview(QPixmap& pixmap);

		Nz::Vector2i m_previewSize;
		QLabel* m_pixmapLabel;
		std::optional<QPixmap> m_pixmap;
		std::string m_variableName;
		ShaderGraph& m_graph;
		bool m_enableCustomVariableName;
		bool m_isPreviewEnabled;
};

#include <ShaderNode/DataModels/ShaderNode.inl>

#endif
