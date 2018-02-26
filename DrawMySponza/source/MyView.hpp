#pragma once

#include <sponza/sponza_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

class MyView : public tygra::WindowViewDelegate
{
public:

	MyView();

	~MyView();

	void setScene(const sponza::Context * scene);

private:

	void windowViewWillStart(tygra::Window * window) override;

	void windowViewDidReset(tygra::Window * window,
		int width,
		int height) override;

	void windowViewDidStop(tygra::Window * window) override;

	void windowViewRender(tygra::Window * window) override;

private:

	const sponza::Context * scene_;
	float m_aspect_ratio{ 0 };
	GLuint m_sponza_shader_program{ 0 };

	//Structure that contains all data for each mesh vertex
	struct VertexData {
		sponza::Vector3 m_position;
		sponza::Vector3 m_normal;
		sponza::Vector3 m_tangent;
		sponza::Vector2 m_texCord;
	};
	//Mesh struct to store ids of the VBOs and the VAO
	GLuint m_vertex_vbo{ 0 };
	GLuint m_instances_modelView_vbo{ 0 };
	GLuint m_element_vbo{ 0 };
	GLuint m_vao{ 0 };
	GLuint m_texture{ 0 };
	unsigned int m_maxInstancesNum{ 0 };
	struct MeshGL {
		int element_count{ 0 };
		int instanceCount{ 0 };
		int id { 0 };
		std::vector<glm::mat4> transforms;
		GLuint first_element_index{ 0 };
		GLuint first_vertex_index{ 0 };
	};
	std::vector<MeshGL> m_meshes;

	/*
	* Constant number identifiers
	*/
	const static GLuint S_NullID = 0;
	enum eVertexShaderAttributesIndexes
	{
		VertexPositionID = 0,
		VertexNormalID = 1,
		VertexTangentID = 2,
		VertexTexCordID = 3,
		InstanceModelFormFirstcolumn = 4,
		InstanceModelFormSecondcolumn = 5,
		InstanceModelFormThirdcolumn = 6,
		InstanceModelFormForthcolumn = 7
	};
	enum eFragmentShaderShaderDataIndexes
	{
		FragmentColourID = 0
	};
	enum eTextureSamplersIndexes
	{
		HEXSampler = 0
	};
};
