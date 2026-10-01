#version 330

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
