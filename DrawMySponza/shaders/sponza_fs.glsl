#version 330
uniform sampler2D hexTexture_sampler;

in vec3 varying_normal_colour;
in vec2 varying_tex_cordinates;

out vec4 fragment_colour;

void main(void)
{
	fragment_colour = vec4(texture(hexTexture_sampler, varying_tex_cordinates).rgb * varying_normal_colour, 1.0);
}
