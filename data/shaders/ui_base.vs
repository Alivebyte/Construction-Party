#version 120

uniform mat4 u_projectionMatrix;

attribute vec2 a_position;
attribute vec2 a_texcoord;
attribute vec4 a_color;

varying vec2 v_texcoord;
varying vec4 v_color;

void main() {
	v_texcoord = a_texcoord;
    v_color = a_color;
    gl_Position = u_projectionMatrix * vec4(a_position.xy, 0, 1);
}