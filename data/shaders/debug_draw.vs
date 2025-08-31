#version 120

attribute vec3 a_position;
attribute vec3 a_color;

varying vec3 v_color;

uniform mat4 u_modelViewProjection;

void main()
{
    gl_Position = u_modelViewProjection * vec4(a_position, 1.0f);

    v_color = a_color;
}