CSD1   d1138793a32d4bc23cddfb92cd4edae0    �     @      �  `   !  �  �   )    .  �   #version 130

uniform mat4 projection;
uniform mat4 modelview;
uniform vec4 color;

in vec4 in_position;
in vec2 in_texcoord;

out vec2 ps_texcoord;
out vec4 ps_color;

void main()
{
	ps_texcoord = in_texcoord;
	vec4 position = in_position*modelview;
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
#version 130

uniform mat4 projection;
uniform mat4 modelview;
uniform vec4 color;

in vec4 in_position;
in vec2 in_texcoord;

out vec2 ps_texcoord;
out vec4 ps_color;

void main()
{
	vec4 position = in_position*modelview;
	gl_Position = position*projection;
}
#version 130

uniform sampler2D texture0;
uniform vec4 color;

in vec2 ps_texcoord;

out vec4 oColor;

void main()
{
	oColor = color;
	}
mode                               �            