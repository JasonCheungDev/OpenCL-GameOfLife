#version 450 core

in vec2 fragUv;

uniform sampler2D tex;

out vec4 color;

void main()
{
	vec3 col = texture(tex, fragUv).rgb;
	color = vec4(col, 1.0);
}
