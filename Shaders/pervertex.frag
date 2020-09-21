#version 120

varying vec4 f_color;
varying vec2 f_texCoord;

uniform sampler2D texture0;

void main() {
	vec4 texColor;
	texColor = texture2D(texture0, f_texCoord);
	vec4 color = f_color * texColor;
	
	gl_FragColor = color;
}
