#version 330 core

in vec4 channelCol;
out vec4 outCol;

uniform float sunColor;
uniform bool useSunColor;

void main()
{
	if (useSunColor) {
        outCol = vec4(1.0, max(0.6, sunColor), 0.0, 1.0);
    } else {
        outCol = channelCol;
    }
}