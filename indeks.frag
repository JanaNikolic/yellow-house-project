#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;

void main()
{
	outCol = texture(uTex, chTex);
	if(outCol.a < 0.2) //ukoliko radimo sa png koji moze da ima alfa kanal(prozirnost)
		discard;
}
