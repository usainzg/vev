#version 120

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient; // Scene ambient light

struct material_t {
	vec3  diffuse;
	vec3  specular;
	float alpha;
	float shininess;
};

struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
};

uniform light_t theLights[4];
uniform material_t theMaterial;

uniform sampler2D texture0;
uniform sampler2D bumpmap;

varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space
varying vec3 f_lightDirection[4]; // tangent space
varying vec3 f_spotDirection[4];  // tangent space

// n es el vector normal
// l es el vector de la luz
float lambert_factor(vec3 n, const vec3 l) {
	return max(0, dot(n, l));
}

// n es el vector normal
// l es el vector de la luz
// v es el vector que va a la cámara
// m es el brillo del material (theMaterial.shininess)
float specular_factor(const vec3 n, const vec3 l, const vec3 v, float m) {
	vec3 r = normalize(2 * (dot(n, l)) * n - l);
	float r_dot_v = dot(r, v);

	return (r_dot_v > 0.0) ? pow(r_dot_v, m) : 0.0;
}

void dir_light(const in int i, const in vec3 l, const in vec3 normal, inout vec3 diffuse, inout vec3 specular, const in vec3 v) {
	vec3 L = normalize(l);
	
	float d_factor = lambert_factor(normal, L);
	float s_factor = specular_factor(normal, L, v, theMaterial.shininess);
	diffuse += theLights[i].diffuse * d_factor;
	specular += theLights[i].specular * s_factor * d_factor;
}

void pos_light(const in int i, const in vec3 l, const in vec3 normal, inout vec3 diffuse, inout vec3 specular, const in vec3 v, const in float dis) {
	float d_factor = lambert_factor(normal, l);
	float s_factor = specular_factor(normal, l, v, theMaterial.shininess);
	
	float att = 1.0;
	float denomin = (theLights[i].attenuation[0] + theLights[i].attenuation[1] * dis + theLights[i].attenuation[2] * (dis*dis));
	
	// si el denominador no es 0.0 hacemos la division
	// en caso contrario (denomin == 0.0), dejamos la atenuacion en 1.0
	if (denomin > 0.0) att = 1.0 / denomin;

	diffuse += d_factor * theLights[i].diffuse * att;
	specular += s_factor * theLights[i].specular * att * d_factor;
}

void spot_light(const in int i, const in vec3 l, const in vec3 normal, inout vec3 diffuse, inout vec3 specular, const in vec3 v) {
	vec3 L = l;
	vec3 D = normalize(f_spotDirection[i]);

	float d_factor = lambert_factor(normal, L);
	float s_factor = specular_factor(normal, L, v, theMaterial.shininess);

	// si el spot_cos es mayor o igual al cosCutOff
	// el pixel a iluminar está dentro del cono
	float spot_cos = max(0.0, dot(-L, D)); // spot_cos no puede ser negativo (pow)
	float spot_factor = 0.0;
	if (spot_cos >= theLights[i].cosCutOff) { // inside?
		spot_factor = pow(spot_cos, theLights[i].exponent);
	}

	// si no hemos cambiado los valores del spot_factor,
	// quiere decir que no está dentro del spot y por lo tanto no
	// le afecta el spot_factor. Nos evitamos calculos innecesarios.
	if (spot_factor > 0.0) {
		diffuse += d_factor * theLights[i].diffuse * spot_factor;
		specular += s_factor * theLights[i].specular * spot_factor * d_factor;
	}
}

void main() {
	vec4 texColor;
	texColor = texture2D(texture0, f_texCoord);
	
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	
	// pasamos del rango [0, 1] -> [-1, 1]
	// primero pasamos el rango [0, 1] a [0, 2] (multiplicando por 2)
	// y restamos 1 para obtener el rango [-1, 1]
	vec3 normal = 2.0 * texture2D(bumpmap, f_texCoord).rgb - 1;
	normal = normalize(normal);
	
	vec3 v = normalize(f_viewDirection);

	for(int i = 0; i < active_lights_n; i++) {
		if (theLights[i].position.w == 0.0) { // directional
			dir_light(i, -theLights[i].position.xyz, normal, diffuse, specular, v);
		} else {
			float dis = length(f_lightDirection[i]);
			if (dis > 0) {
				vec3 l = normalize(f_lightDirection[i]);
				if (theLights[i].cosCutOff > 0.0) { // spotlight
					spot_light(i, l, normal, diffuse, specular, v);
				} else { // positional
					pos_light(i, l, normal, diffuse, specular, v, dis);
				}
			}
		}
	}
	vec4 color;
	color.rgb = scene_ambient + diffuse * theMaterial.diffuse + specular * theMaterial.specular;
	color.a = theMaterial.alpha;
	color *= texColor;

	gl_FragColor = color;
}
