#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D myTexture;
uniform sampler2D myTexture2;
uniform float mixer;

void main()
{
   FragColor = mix(texture(myTexture, TexCoord), texture(myTexture2, vec2(1.0 - TexCoord.x, TexCoord.y)), mixer);
}