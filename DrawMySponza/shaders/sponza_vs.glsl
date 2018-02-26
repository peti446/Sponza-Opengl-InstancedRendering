#version 330
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 vertex_tangent;
in vec2 vertex_texCord;
in mat4 model_matrix;

out vec3 varying_normal_colour;
out vec2 varying_tex_cordinates;

void main(void)
{
	mat4 projection_model_view_matrix = projection_matrix * view_matrix * model_matrix;
	varying_tex_cordinates = vertex_texCord;
	varying_normal_colour = (model_matrix * vec4(0.5 + 0.5 * vertex_normal, 0.0)).xyz;
    gl_Position = projection_model_view_matrix * vec4(vertex_position,1.0);
}
