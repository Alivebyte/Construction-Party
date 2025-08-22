#version 120

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v_finalColor;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewProjection;

vec3 CalcOmniLight()
{
	vec3 lightPos = vec3(0.1, 2.1, 0.1);
	float d = distance(lightPos, v_position);
	vec3 L = normalize(lightPos-v_position);
	vec3 N = normalize(v_normal);
	vec3 col = vec3( max(0, dot(N, L) / d) );	
	col = col * 0.8 + 0.2;
	return col;
}

vec3 CalcDirLight()
{
	vec3 lightPos = vec3(5.0, 10.0, 1.0);
	//lightPos = -lightPos;
	
	vec3 L = normalize(lightPos);
	vec3 N = normalize(v_normal);
	vec3 col = vec3( max(0, dot(N, L)) );	
	col = col * 0.8 + 0.2;
	return col;
}

void main() {
	v_position = vec3( u_modelMatrix * vec4(a_position, 1.0) );
	v_normal = vec3( mat3(u_modelMatrix) * a_normal );
	v_texcoord = a_texcoord;
	v_finalColor = CalcDirLight();
	gl_Position = u_modelViewProjection * vec4(a_position, 1);
}