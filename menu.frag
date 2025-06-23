#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D menuTexture;
 uniform float fadeAlpha;
void main() {
   
FragColor = texture(menuTexture, TexCoord) * fadeAlpha;

}