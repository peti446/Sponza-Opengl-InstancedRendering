#version 330

#define MAX_LIGHTS 128

struct LightSource {
	vec3 position;
	vec3 intensity;
	float range;
};
struct MaterialSettings {
	vec3 diffuse_colour;
	vec3 specular_colour;
	float shininess;
};
uniform sampler2D hexTexture_sampler;
uniform LightSource light_sources[MAX_LIGHTS];
uniform int lightsCount;
uniform MaterialSettings material_data;
uniform vec3 ambient_light;
uniform vec3 camera_position;
uniform mat4 view_matrix;

in vec2 varying_tex_cordinates;
in vec3 varying_normal_vector;
in vec3 varying_position;

out vec4 fragment_colour;

//function prototypes
vec3 CalculateLight(vec3 P, vec3 N, vec3 V, LightSource light, MaterialSettings material);

void main(void)
{
	vec3 P = varying_position;
	vec3 N = normalize(varying_normal_vector);
	vec3 V = normalize(camera_position - P);
	vec3 sumOfLight = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < lightsCount; i++) {
		sumOfLight += CalculateLight(P, N, V, light_sources[i], material_data);
	}
	vec3 Ka = vec3(0.85, 0.80, 0.66);
	vec3 Ia = ambient_light;
	vec3 reflectedLight = (Ka * Ia) + sumOfLight;
	fragment_colour = vec4(reflectedLight, 1.0);
}

vec3 CalculateLight(vec3 P, vec3 N, vec3 V, LightSource light, MaterialSettings material)
{
	//Calculate the vectos nececarry
	vec3 LP = light.position;
	vec3 L = normalize(LP - P);
	vec3 RV = reflect(-V, N);

	//Calculate the diffuse intrensity
	float diffuse_intensity = max(0.0, dot(L, N));
	//Calculate the specular value
	float speculat_intensity = pow(max(0.0, dot(L, RV)), material.shininess) * sign(diffuse_intensity);

	//Calculate light range
	float distanceFromLight = distance(LP, P);
	float lightProcent = 1.0 - smoothstep(0, light.range, distanceFromLight);

	//Return value ((I*attenuationPorcent)*(Kd * (L . N) + Ks * (L . Rv)^shininess))
	return (light.intensity) * ((lightProcent * diffuse_intensity * material.diffuse_colour) + (lightProcent * speculat_intensity * material.specular_colour));
}