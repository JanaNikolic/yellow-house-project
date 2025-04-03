#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNor;
layout(location = 2) in vec2 inTex;

out vec3 chFragPos; 
out vec3 chNor;
out vec2 chTex;

uniform mat4 uM;  // Model matrix
uniform mat4 uV;  // View matrix
uniform mat4 uP;  // Projection matrix

uniform vec3 uTranslation;   // Translation vector (x, y)
uniform float uScale;         // Scaling factor
uniform bool uApplyTransform; // Flag to apply the transformation

void main()
{
    vec3 scaledAndTranslatedPos = inPos;

    if (uApplyTransform) {
        scaledAndTranslatedPos = (uScale * inPos) + uTranslation;
    }
    chFragPos = vec3(uM * vec4(scaledAndTranslatedPos, 1.0));
    gl_Position = uP * uV * vec4(chFragPos, 1.0);
    
    chNor = mat3(transpose(inverse(uM))) * inNor;
    chTex = inTex;
}
