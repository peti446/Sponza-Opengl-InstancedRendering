#include "MyView.hpp"
#include <sponza/sponza.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const sponza::Context * scene)
{
	scene_ = scene;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
	assert(scene_ != nullptr);
#pragma region Shaders and Program Creations
	{
		GLint shader_comile_status = GL_FALSE;

#pragma region Vertex Shader
		//Create vertex shader and load data in
		GLuint sponzaVertex_shader = glCreateShader(GL_VERTEX_SHADER);

		std::string vertex_shaderStr = tygra::createStringFromFile("resource:///sponza_vs.glsl");
		const char * vertex_shader_code = vertex_shaderStr.c_str();

		//Compile the shader
		glShaderSource(sponzaVertex_shader, 1, (const GLchar**)&vertex_shader_code, NULL);
		glCompileShader(sponzaVertex_shader);
		glGetShaderiv(sponzaVertex_shader, GL_COMPILE_STATUS, &shader_comile_status);

		//Check if compiled
		if (GL_FALSE == shader_comile_status) {
			//Did not compile, print error msg
			const int string_length = 1024;
			GLchar log[string_length] = "";
			glGetShaderInfoLog(sponzaVertex_shader, string_length, NULL, log);
			std::cerr << log << std::endl;
		}
#pragma endregion

#pragma region Fragmen Shader
		//Create fragment shader and load data in
		GLuint sponzaFragmen_shader = glCreateShader(GL_FRAGMENT_SHADER);
		std::string sponzaFragmen_shader_string = tygra::createStringFromFile("resource:///sponza_fs.glsl");
		const char * sponzaFragment_shader_code = sponzaFragmen_shader_string.c_str();

		//Compile the shader
		glShaderSource(sponzaFragmen_shader, 1, (const GLchar**)&sponzaFragment_shader_code, NULL);
		glCompileShader(sponzaFragmen_shader);
		glGetShaderiv(sponzaFragmen_shader, GL_COMPILE_STATUS, &shader_comile_status);

		//Check if compiled
		if (GL_FALSE == shader_comile_status) {
			//Did not compile, print error msg
			const int string_length = 1024;
			GLchar log[string_length] = "";
			glGetShaderInfoLog(sponzaVertex_shader, string_length, NULL, log);
			std::cerr << log << std::endl;
		}
#pragma endregion

#pragma region Create shader Program
		//Create the porgram
		this->m_sponza_shader_program = glCreateProgram();

		//Atach the vertex shader
		glAttachShader(this->m_sponza_shader_program, sponzaVertex_shader);
		glBindAttribLocation(this->m_sponza_shader_program, VertexPositionID, "vertex_position");
		glBindAttribLocation(this->m_sponza_shader_program, VertexNormalID, "vertex_normal");
		glBindAttribLocation(this->m_sponza_shader_program, VertexTangentID, "vertex_tangent");
		glBindAttribLocation(this->m_sponza_shader_program, VertexTexCordID, "vertex_texCord");
		glBindAttribLocation(this->m_sponza_shader_program, InstanceModelFormFirstcolumn, "model_matrix");
		glDeleteShader(sponzaVertex_shader);

		//Atach the fragmet shader
		glAttachShader(this->m_sponza_shader_program, sponzaFragmen_shader);
		glBindFragDataLocation(this->m_sponza_shader_program, FragmentColourID, "fragment_colour");
		glDeleteShader(sponzaFragmen_shader);
		glLinkProgram(this->m_sponza_shader_program);

		//Check if the shader program linked correctly
		GLint program_link_status = GL_FALSE;
		glGetProgramiv(this->m_sponza_shader_program, GL_LINK_STATUS, &program_link_status);
		if (GL_TRUE != program_link_status) {
			const int string_length = 1024;
			GLchar log[string_length] = "";
			glGetProgramInfoLog(this->m_sponza_shader_program, string_length, NULL, log);
			std::cerr << log << std::endl;
		}
#pragma endregion
	}
#pragma endregion
#pragma region Get the data from each mesh and instance data
	//Data for the meshes
	sponza::GeometryBuilder sponzaBuilder;
	std::vector<sponza::Mesh> allSponzaMeshes = sponzaBuilder.getAllMeshes();
	//Reserve data in the meshes vector
	this->m_meshes.reserve(allSponzaMeshes.size());

	//Prepare the vectors for the vertecis and element data
	std::vector<VertexData> vertex_data_array;
	std::vector<unsigned int> element_data_array;

	for (const sponza::Mesh& mesh : allSponzaMeshes) {
		unsigned int instancesCount = (unsigned int)this->scene_->getInstancesByMeshId(mesh.getId()).size();
		if (m_maxInstancesNum < instancesCount) {
			m_maxInstancesNum = instancesCount;
		}

		//Get the data from 
		std::vector<sponza::Vector3> positionData = mesh.getPositionArray();
		std::vector<sponza::Vector3> normalData = mesh.getNormalArray();
		std::vector<sponza::Vector3> tangentData = mesh.getTangentArray();
		std::vector<sponza::Vector2> textureCoordinates = mesh.getTextureCoordinateArray();
		std::vector<unsigned int> elementData = mesh.getElementArray();

		//Create a mes instance
		MeshGL meshIsntance;
		meshIsntance.element_count = (int)elementData.size();
		meshIsntance.first_element_index = (int)element_data_array.size();
		meshIsntance.instanceCount = instancesCount;
		meshIsntance.id = mesh.getId();
		meshIsntance.first_vertex_index = (int)vertex_data_array.size();
		this->m_meshes.push_back(meshIsntance);


		//Add the current mes element data to the big element data array
		element_data_array.insert(element_data_array.end(), elementData.begin(), elementData.end());
		//Add each vertex data from this mesh to the big array that contains all mesh data
		for (size_t i = 0; i < positionData.size(); i++) {
			VertexData vd;
			vd.m_position = positionData.at(i);
			vd.m_normal = normalData.at(i);
			vd.m_tangent = tangentData.at(i);
			vd.m_texCord = textureCoordinates.at(i);
			vertex_data_array.push_back(vd);
		}
	}
#pragma endregion
#pragma region VBO Generaiton
	//All meshes vertex VBO
	glGenBuffers(1, &this->m_vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
				 vertex_data_array.size() * sizeof(VertexData),
				 vertex_data_array.data(),
				 GL_STATIC_DRAW
				);
	glBindBuffer(GL_ARRAY_BUFFER, S_NullID);

	//Instanced data VBO
	glGenBuffers(1, &this->m_instances_modelView_vbo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->m_instances_modelView_vbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * this->m_maxInstancesNum, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, S_NullID);

	//All meshes Element VBO
	glGenBuffers(1, &this->m_element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		element_data_array.size() * sizeof(unsigned int),
		element_data_array.data(),
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, S_NullID);

#pragma endregion
#pragma region VAO Generation and Convfiguration
	glGenVertexArrays(1, &this->m_vao);
	glBindVertexArray(this->m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_element_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_vertex_vbo);

	//Active and configure the vertex data inputs
	glEnableVertexAttribArray(VertexPositionID);
	glVertexAttribPointer(VertexPositionID, 3, GL_FLOAT, GL_FALSE,
		sizeof(VertexData), TGL_BUFFER_OFFSET_OF(VertexData, m_position));

	glEnableVertexAttribArray(VertexNormalID);
	glVertexAttribPointer(VertexNormalID, 3, GL_FLOAT, GL_FALSE,
		sizeof(VertexData), TGL_BUFFER_OFFSET_OF(VertexData, m_normal));

	glEnableVertexAttribArray(VertexTangentID);
	glVertexAttribPointer(VertexTangentID, 3, GL_FLOAT, GL_FALSE,
		sizeof(VertexData), TGL_BUFFER_OFFSET_OF(VertexData, m_tangent));

	glEnableVertexAttribArray(VertexTexCordID);
	glVertexAttribPointer(VertexTexCordID, 2, GL_FLOAT, GL_FALSE,
		sizeof(VertexData), TGL_BUFFER_OFFSET_OF(VertexData, m_texCord));

	//Add the instanced data VBO
	glBindBuffer(GL_ARRAY_BUFFER, S_NullID);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_instances_modelView_vbo);

	//Activate and configure the instanced matrix data (divisor of 1 to use one matrix for each instance)
	glEnableVertexAttribArray(InstanceModelFormFirstcolumn);
	glVertexAttribPointer(InstanceModelFormFirstcolumn, 4, GL_FLOAT, GL_FALSE,
		sizeof(glm::mat4), TGL_BUFFER_OFFSET(0));
	glVertexAttribDivisor(InstanceModelFormFirstcolumn, 1);

	glEnableVertexAttribArray(InstanceModelFormSecondcolumn);
	glVertexAttribPointer(InstanceModelFormSecondcolumn, 4, GL_FLOAT, GL_FALSE,
		sizeof(glm::mat4), TGL_BUFFER_OFFSET(sizeof(glm::vec4) * 1));
	glVertexAttribDivisor(InstanceModelFormSecondcolumn, 1);

	glEnableVertexAttribArray(InstanceModelFormThirdcolumn);
	glVertexAttribPointer(InstanceModelFormThirdcolumn, 4, GL_FLOAT, GL_FALSE,
		sizeof(glm::mat4), TGL_BUFFER_OFFSET(sizeof(glm::vec4) * 2));
	glVertexAttribDivisor(InstanceModelFormThirdcolumn, 1);

	glEnableVertexAttribArray(InstanceModelFormForthcolumn);
	glVertexAttribPointer(InstanceModelFormForthcolumn, 4, GL_FLOAT, GL_FALSE,
		sizeof(glm::mat4), TGL_BUFFER_OFFSET(sizeof(glm::vec4) * 3));
	glVertexAttribDivisor(InstanceModelFormForthcolumn, 1);
	glBindBuffer(GL_ARRAY_BUFFER, S_NullID);
	glBindVertexArray(S_NullID);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#pragma endregion
#pragma region Texture generation
	//Get the texture from the resources
	tygra::Image texture_image = tygra::createImageFromPngFile("resource:///hex.png");
	//Check if loaded
	if (texture_image.doesContainData()) {
		//Generate and configure the image
		glGenTextures(1, &this->m_texture);
		glBindTexture(GL_TEXTURE_2D, this->m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 texture_image.width(),
					 texture_image.height(),
					 0,
					 pixel_formats[texture_image.componentsPerPixel()],
					 texture_image.bytesPerComponent() == 1 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
				     texture_image.pixelData());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, S_NullID);
	}
#pragma endregion
}

void MyView::windowViewDidReset(tygra::Window * window,
	int width,
	int height)
{
	//Rezise the viewport
	glViewport(0, 0, width, height);
	//Store the aspect ratio
	this->m_aspect_ratio = (float)width / (float)height;
}

void MyView::windowViewDidStop(tygra::Window * window)
{
	//Delete all the VBOs and VAOs so no memory leaks!
	glDeleteProgram(this->m_sponza_shader_program);
	glDeleteBuffers(1, &this->m_vertex_vbo);
	glDeleteBuffers(1, &this->m_element_vbo);
	glDeleteBuffers(1, &this->m_instances_modelView_vbo);
	glDeleteVertexArrays(1, &this->m_vao);
}

void MyView::windowViewRender(tygra::Window * window)
{
	assert(scene_ != nullptr);

	//Enable Depth test and cull face
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Clear tje color and the buffers
	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Create projection and view matrices
	const sponza::Camera& camera = this->scene_->getCamera();
	glm::mat4 projection_matrix = glm::perspective(glm::radians(camera.getVerticalFieldOfViewInDegrees()), this->m_aspect_ratio, camera.getNearPlaneDistance(), camera.getFarPlaneDistance());
	glm::mat4 view_matrix = glm::lookAt((glm::vec3&)camera.getPosition(), (glm::vec3&)camera.getPosition() + (glm::vec3&)camera.getDirection(), (glm::vec3&)this->scene_->getUpDirection());

	//Activate the shader program
	glUseProgram(this->m_sponza_shader_program);

	//Specify the 3 uniforms we are sending to the shaders (The projection and view matrix could been multiplied together, but i left them separate for debugin perpuses and in case i just want the view or projection matrix)
	//Projection matrix
	GLuint projection_model_id = glGetUniformLocation(this->m_sponza_shader_program, "projection_matrix");
	glUniformMatrix4fv(projection_model_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	//View matrix
	GLuint view_model_id = glGetUniformLocation(this->m_sponza_shader_program, "view_matrix");
	glUniformMatrix4fv(view_model_id, 1, GL_FALSE, glm::value_ptr(view_matrix));
	//Camera position
	GLuint camera_pos_id = glGetUniformLocation(this->m_sponza_shader_program, "camera_position");
	glUniform3fv(this->m_sponza_shader_program, 1, glm::value_ptr((glm::vec3&)this->scene_->getCamera().getPosition()));
	//light info
	//Ambient
	GLuint ambien_light_intensity = glGetUniformLocation(this->m_sponza_shader_program, "ambient_light_intensity");
	glUniform3fv(ambien_light_intensity, 1, glm::value_ptr((glm::vec3&)this->scene_->getAmbientLightIntensity()));

	//Light
	std::vector<sponza::Light> lights = this->scene_->getAllLights();
	GLuint count_id = glGetUniformLocation(this->m_sponza_shader_program, "lightsCount");
	glUniform1i(count_id, lights.size());
	for (int i = 0; i < lights.size(); i++) {
		sponza::Light& l = lights[i];
		GLuint pos = glGetUniformLocation(this->m_sponza_shader_program, ("light_sources["+std::to_string(i)+"].position").c_str());
		GLuint range = glGetUniformLocation(this->m_sponza_shader_program, ("light_sources[" + std::to_string(i) + "].range").c_str());
		GLuint intensity = glGetUniformLocation(this->m_sponza_shader_program, ("light_sources[" + std::to_string(i) + "].intensity").c_str());
	    glUniform3fv(pos, 1, glm::value_ptr((glm::vec3&)l.getPosition()));
		glUniform3fv(intensity, 1, glm::value_ptr((glm::vec3&)l.getIntensity()));
		glUniform1f(range, l.getRange());
	}
	

	//Textyre unit (need to activate it first)
	glActiveTexture(GL_TEXTURE0 + HEXSampler);
	glBindTexture(GL_TEXTURE_2D, this->m_texture);
	GLuint hexTextureSampler = glGetUniformLocation(this->m_sponza_shader_program, "hexTexture_sampler");
	glUniform1i(hexTextureSampler, HEXSampler);

	//Get the uniform location for the material
	GLuint material_diffuse = glGetUniformLocation(this->m_sponza_shader_program, "material_data.diffuse_colour");
	GLuint material_specular = glGetUniformLocation(this->m_sponza_shader_program, "material_data.specular_colour");
	GLuint material_shininess = glGetUniformLocation(this->m_sponza_shader_program, "material_data.shininess");


	//Draw meshes
	glBindVertexArray(this->m_vao);
	//For each mesh (so 27 times)
	for (const MeshGL& meshInfo : this->m_meshes)
	{
		//Check if the instance count is correct
		unsigned int instancecount = meshInfo.instanceCount;
		if (instancecount >= this->m_maxInstancesNum)
			instancecount = this->m_maxInstancesNum;

		//Get the data fro that mesh so animation works, in case there is some
		std::vector<unsigned int> instancesIDs = this->scene_->getInstancesByMeshId(meshInfo.id);
		std::vector<glm::mat4> instancesTransformations;
		instancesTransformations.reserve(instancesIDs.size());
		for (unsigned int instanceID : instancesIDs) {
			instancesTransformations.push_back(glm::mat4((glm::mat4x3&)this->scene_->getInstanceById(instanceID).getTransformationMatrix()));
		}

		//Fill the instance data matrix with the current instance data
		glBindBuffer(GL_UNIFORM_BUFFER, this->m_instances_modelView_vbo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * instancecount, instancesTransformations.data());
		glBindBuffer(GL_UNIFORM_BUFFER, S_NullID);

		//Pass material
		sponza::Material material = this->scene_->getMaterialById(this->scene_->getInstanceById(instancesIDs.front()).getMaterialId());
		glUniform3fv(material_diffuse, 1, glm::value_ptr((glm::vec3&)material.getDiffuseColour()));
		glUniform3fv(material_shininess, 1, glm::value_ptr((glm::vec3&)material.getSpecularColour()));
		glUniform1f(material_shininess, material.getShininess());

		//Draw instanceCount of the current mesh usng the matrix data
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, meshInfo.element_count, GL_UNSIGNED_INT, (const void*)(sizeof(unsigned int) * meshInfo.first_element_index), instancecount, meshInfo.first_vertex_index);
	}
	glBindVertexArray(S_NullID);
}
