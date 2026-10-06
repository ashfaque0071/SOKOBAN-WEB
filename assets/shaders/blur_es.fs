#version 300 es

/* GLSL ES 3.00 port of blur.fs for the WebGL2 build. The maths is identical
   to the desktop #version 330 shader; only the precision qualifier and the
   version header differ. It has to match the GLSL version of raylib's default
   vertex shader, which is 300 es when raylib is built for OpenGL ES 3 -- a
   #version 100 fragment shader will not link against it. */

precision mediump float;

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec2 texelSize;

out vec4 finalColor;

void main()
{
    vec2 offset = texelSize * 2.2;
    vec4 color = texture(texture0, fragTexCoord) * 0.20;

    color += texture(texture0, fragTexCoord + vec2( offset.x, 0.0)) * 0.12;
    color += texture(texture0, fragTexCoord + vec2(-offset.x, 0.0)) * 0.12;
    color += texture(texture0, fragTexCoord + vec2(0.0,  offset.y)) * 0.12;
    color += texture(texture0, fragTexCoord + vec2(0.0, -offset.y)) * 0.12;

    color += texture(texture0, fragTexCoord + vec2( offset.x,  offset.y)) * 0.08;
    color += texture(texture0, fragTexCoord + vec2(-offset.x,  offset.y)) * 0.08;
    color += texture(texture0, fragTexCoord + vec2( offset.x, -offset.y)) * 0.08;
    color += texture(texture0, fragTexCoord + vec2(-offset.x, -offset.y)) * 0.08;

    finalColor = color * colDiffuse * fragColor;
}
