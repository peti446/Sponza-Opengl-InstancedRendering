#version 330
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform sampler2D hexTexture_sampler;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 vertex_tangent;
in vec2 vertex_texCord;
in mat4 model_matrix;

out vec2 varying_tex_cordinates;
out vec3 varying_normal_vector;
out vec3 varying_position;

void main(void)
{
	varying_tex_cordinates = vertex_texCord;
	varying_position = mat4x3(model_matrix) * vec4(vertex_position, 1.0);
	varying_normal_vector = mat3(model_matrix) * vertex_normal;

    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex_position,1.0);
}
