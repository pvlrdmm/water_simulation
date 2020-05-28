#version 330 core
in vec3 ourColor;
in vec3 Normal;

out vec4 color;

void main()
{
	vec3 ambientLightIntensity = vec3(0.3f, 0.2f, 0.4f);
    vec3 sunLightIntensity = vec3(2.5f, 2.5f, 2.5f);
    vec3 sunLightDirection = normalize(vec3(-2.0f, -2.0f, 0.0f));

	vec3 lightIntensity = ambientLightIntensity + sunLightIntensity * max(dot(Normal, sunLightDirection), 0.0f);
	
	color = vec4(ourColor * lightIntensity, 1.0f);
}