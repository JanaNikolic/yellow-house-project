#version 330 core

struct Light {
    vec3 pos;
    vec3 kA;
    vec3 kD;
    vec3 kS;
};

struct DirLight {
    vec3 pos;
    vec3 kA;
    vec3 kD;
    vec3 kS;
};

struct PointLight {
    vec3 pos;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 kA;
    vec3 kD;
    vec3 kS;
};

struct SpotLight {
    vec3 pos;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 kA;
    vec3 kD;
    vec3 kS;      
};

#define NR_POINT_LIGHTS 4

struct Material {
    vec3 kA;
    vec3 kD;
    vec3 kS;
    float shine;
};

in vec3 chNor;
in vec3 chFragPos;
in vec2 chTex;

out vec4 outCol;

uniform Material uMaterial;
uniform vec3 uViewPos;

uniform DirLight dirLight;
uniform PointLight pointLight1;
uniform PointLight pointLight2;
uniform SpotLight spotLight;
//uniform Material material;

uniform sampler2D uTex;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D uDiffMap1;

uniform float uAlpha;
uniform bool grass;
uniform bool house;
uniform bool model;
uniform bool lego;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 normal = normalize(chNor);
    vec3 viewDirection = normalize(uViewPos - chFragPos);
    vec3 finalColor;
    vec3 textureColor = texture(uTex, chTex).rgb;

    if (grass) {
        vec3 diffuseColor = texture(diffuseMap, chTex).rgb;
        // Specular map
        vec3 specularColor = texture(specularMap, chTex).rgb;

        vec3 resA = dirLight.kA * diffuseColor;
        vec3 lightDirection = normalize(dirLight.pos);
        float nD = max(dot(normal, lightDirection), 0.0); 
        vec3 resD = dirLight.kD * (nD * diffuseColor);

        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float s = pow(max(dot(viewDirection, reflectionDirection), 0.0), 120.0);
        vec3 resS = dirLight.kS * (s * specularColor);

        finalColor = resA + resD + resS + CalcSpotLight(spotLight, normal, chFragPos, viewDirection);
        outCol = vec4(finalColor, uAlpha);
        return;

    } else if (house) {
        float dotProduct = dot(normal, viewDirection);
        
        vec3 resA = dirLight.kA * uMaterial.kA;
        vec3 lightDirection = normalize(dirLight.pos - chFragPos);
        float nD = max(dot(normal, lightDirection), 0.0);
        vec3 resD = dirLight.kD * (nD * uMaterial.kD);

        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float s = pow(max(dot(viewDirection, reflectionDirection), 0.0), uMaterial.shine);
        vec3 resS = dirLight.kS * (s * uMaterial.kS);

        textureColor = textureColor * resA;
        finalColor = resA + resD + resS;
        vec3 faceColor;
    
        if (dotProduct > 0.0) {
            //finalColor = resA;
            faceColor = gl_FrontFacing ? finalColor : textureColor;
            if (all(equal(faceColor, textureColor))) {
                faceColor += CalcPointLight(pointLight1, normal, chFragPos, viewDirection);
                faceColor += CalcPointLight(pointLight2, normal, chFragPos, viewDirection);
            }
            finalColor = faceColor;
            outCol = vec4(finalColor, uAlpha);
            return;
        } else if (dotProduct == 0.0) {
            finalColor = resA;
            finalColor += CalcPointLight(pointLight1, normal, chFragPos, viewDirection);
            finalColor += CalcPointLight(pointLight2, normal, chFragPos, viewDirection);
            outCol = vec4(finalColor, uAlpha);
            return;
        }
        else {
            
            faceColor = gl_FrontFacing ? textureColor : finalColor;
            //finalColor = resA;
            finalColor = faceColor;
            if (all(equal(faceColor, textureColor))) {
                finalColor += CalcPointLight(pointLight1, normal, chFragPos, viewDirection);
                finalColor += CalcPointLight(pointLight2, normal, chFragPos, viewDirection);
            }
            outCol = vec4(finalColor, uAlpha);
            return;
        }
    } else if (model) {
        

        vec3 ambientStrength = dirLight.kA;
        vec3 uLightColor = vec3(1.0, 1.0, 1.0);
        vec3 ambient = ambientStrength * uLightColor;
  	
        // diffuse 
        vec3 lightDir = normalize(dirLight.pos - chFragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * uLightColor * dirLight.kD;
    
        // specular
        vec3 specularStrength = dirLight.kS;
        vec3 viewDir = normalize(uViewPos - chFragPos);
        vec3 reflectDir = reflect(-lightDir, normal);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * uLightColor; 
        outCol = texture(uDiffMap1, chTex) * vec4(ambient + diffuse + specular, 1.0);
        return;
    } else if (lego) {
        vec3 ambientStrength = dirLight.kA;
        vec3 uLightColor = vec3(1.0, 1.0, 1.0);
        vec3 ambient = ambientStrength * uLightColor;
  	    ambient += CalcPointLight(pointLight1, normal, chFragPos, viewDirection);
        outCol = texture(uDiffMap1, chTex) * vec4(ambient, 1.0);
        return;
    }

    vec3 resA = dirLight.kA * uMaterial.kA;
    vec3 lightDirection = normalize(dirLight.pos - chFragPos);
    float nD = max(dot(normal, lightDirection), 0.0);
    vec3 resD = dirLight.kD * (nD * uMaterial.kD);

    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float s = pow(max(dot(viewDirection, reflectionDirection), 0.0), uMaterial.shine);
    vec3 resS = dirLight.kS * (s * uMaterial.kS);
    vec3 res = resA + resD + resS;
    res += CalcSpotLight(spotLight, normal, chFragPos, viewDirection);

    outCol = vec4(res, uAlpha);

}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shine);
    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.kA * uMaterial.kA;
    vec3 diffuse = light.kD * diff * uMaterial.kD;
    vec3 specular = light.kS * spec * uMaterial.kS;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shine);
    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.kA * uMaterial.kA;
    vec3 diffuse = light.kD * diff * uMaterial.kD;
    vec3 specular = light.kS * spec * uMaterial.kS;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}