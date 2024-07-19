CSD1   c1eac0f5e33c242a5bbbc30253c7f9a1            @      (  P   1  �  �   #version 130

uniform mat4 projection;
uniform mat4 modelview;

uniform vec2 offset;

in vec4 in_position;
in vec2 in_texcoord;

out vec2 ps_texcoord;

void main()
{
	ps_texcoord = in_texcoord;
	vec4 position = vec4(in_position.xy+offset, in_position.zw)*modelview;;
	gl_Position = position*projection;
}
#version 130

uniform sampler2D texture0;
uniform vec4 color;

in vec2 ps_texcoord;

out vec4 oColor;

void main()
{
	oColor = texture(texture0, ps_texcoord)*color;
}
