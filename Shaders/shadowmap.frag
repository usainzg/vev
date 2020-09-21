#version 120

varying vec4 f_color;
varying vec2 f_texCoord;
varying vec4 f_c;

uniform sampler2D texture0;
uniform sampler2D shadowmap;

void main() {
	vec4 texColor;
	texColor = texture2D(texture0, f_texCoord);

	// obtenemos las coordenadas fNDC 
	vec3 fndc = f_c.xyz / f_c.w;
	
	// pasamos de [-1, 1] -> [0, 1]
	fndc = (fndc * 0.5) + 0.5;
	// sacamos la profundidad del fragmento 
	// segun la camara colocada en la posicion de la luz
	float current_depth = fndc.z;

	// valor guardado en el mapa de sombras (x, y), solo usamos la R (RGB)
	float closest_depth = texture2D(shadowmap, fndc.xy).r;
	
	vec4 color;
	color = f_color * texColor;
	
	// para el shadow acne
	float bias = 0.005;
	
	// si el fragmento se encuentra en la zona de la sombra, oscurecemos
	if (current_depth-bias > closest_depth) {
		color.rgb *= 0.80;
	}

	color.a = 1.0;
	gl_FragColor = color;
}
