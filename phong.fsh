#define VERSION 100
#define MAX_LIGHTS 1

varying vec3 N;
varying vec3 V;

void main(void)
{
	vec3 L = normalize(gl_LightSource[0].position.xyz - V);
	vec3 E = normalize(-V);
	vec3 R = normalize(-reflect(L, N));

	//Ambient Term	
	vec4 Iamb = 0.5 * gl_LightSource[0].ambient * gl_Color;
	
	//Diffused Term
	vec4 Idif = (gl_LightSource[0].diffuse * gl_Color) * max(dot(N,L), 0.0);
	
	//Specular Term
	vec4 Ispec = (gl_LightSource[0].specular * (vec4(0.8, 0.8, 0.8, 0.8) + 0.2 * gl_Color)) * pow(max(dot(R, E), 0.0), 32.0);
	
	gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idif + Ispec;	
	
	//gl_FragColor = gl_Color;
}
