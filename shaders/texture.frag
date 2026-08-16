#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D brick;
uniform sampler2D scenery;

void main() {
    vec4 sceneryTexture = texture(scenery, texCoord);
    vec4 brickTexture = texture(brick, texCoord);
    FragColor = mix(brickTexture, sceneryTexture, 0.5);
};
