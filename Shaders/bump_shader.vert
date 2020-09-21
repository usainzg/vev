#version 120

// Bump mapping with many lights.

// all attributes in model space
attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec2 v_texCoord;
attribute vec3 v_TBN_t;
attribute vec3 v_TBN_b;

uniform mat4 modelToCameraMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToClipMatrix;

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)

uniform struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
} theLights[4];     // MG_MAX_LIGHTS

// All bump computations are performed in tangent space; therefore, we need to
// convert all light (and spot) directions and view directions to tangent space
// and pass them the fragment shader.
varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space
varying vec3 f_lightDirection[4]; // tangent space
varying vec3 f_spotDirection[4];  // tangent space

void main() {

	// pasamos la posicion del vertice a coordenadas de la camara
	vec3 f_position = vec3(modelToCameraMatrix * vec4(v_position, 1.0));
	
	// pasamos los vectores que forman la matriz TBN a coordenadas de la camara
	// dicha matriz nos pasa de espacio tangente -> modelo
	vec3 t = normalize(vec3(modelToCameraMatrix * vec4(v_TBN_t, 0.0)));
	vec3 b = normalize(vec3(modelToCameraMatrix * vec4(v_TBN_b, 0.0)));
	vec3 n = normalize(vec3(modelToCameraMatrix * vec4(v_normal, 0.0)));

	// construimos la matriz que pasa de modelo -> espacio tangente
	// en este caso al haber pasado los vectores al sistema de la camara...
	// la matriz TBN pasará de espacio de la camara -> espacio tangente
	mat3 TBN = transpose(mat3(t, b, n));

	vec3 v;
	v = -(TBN * f_position); // obtenemos el viewdirection en el espacio tangente
	
	// lo seteamos en la variable varying correspondiente
	// no normalizamos aqui porque con la interpolación se estropearia
	f_viewDirection = v;
	
	// pasamos los vectores de la luz al sistema del espacio tangente
	for (int i = 0; i < active_lights_n; i++) {
		vec3 l_dir = vec3(0.0);
		
		if (theLights[i].position.w == 0.0) {
			l_dir = -theLights[i].position.xyz;
		} else {
			l_dir = theLights[i].position.xyz - f_position;
			
			// solamente realizamos los calculos del spotDirection
			// cuando tenemos una luz de tipo spotLight
			if (theLights[i].cosCutOff > 0.0) {
				f_spotDirection[i] = TBN * theLights[i].spotDir;
			}
		}
		
		f_lightDirection[i] = TBN * l_dir;
	}
	f_texCoord = v_texCoord;
	gl_Position = modelToClipMatrix * vec4(v_position, 1.0);
}
