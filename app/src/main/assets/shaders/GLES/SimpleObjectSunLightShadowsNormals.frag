#version 300 es

precision highp float; // highp mediump lowp

in vec2 textureCoords;
in vec3 fragPosTangentSpace;
in vec3 sunLightDirTangentSpace;
in vec3 cameraPosTangentSpace;
in vec4 fragPosLightPerspective;

out vec4 outColor;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMapTexture;
uniform sampler2D normalMapTexture;

uniform float ambientLight;
uniform float specularLightStrength;

void main()
{
    // normal mapping
    vec3 normal = texture(normalMapTexture, textureCoords).rgb * 2.0f - 1.0f;   // this normal is in tangent space
    normal = normalize(normal);

    // diffuse
    vec3 sunTangentNorm = normalize(sunLightDirTangentSpace);
    float diffuse = max(dot(normal, -sunTangentNorm), 0.0f);

    // specular
    vec3 fragToCameraDir = normalize(cameraPosTangentSpace - fragPosTangentSpace);
    vec3 reflectDir = normalize(reflect(sunTangentNorm, normal));
    float specular = pow(max(dot(fragToCameraDir, reflectDir), 0.0f), 64.0f) * specularLightStrength; // 64.0f <- bigger number = smaller light dot

    // shadow
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMapTexture, fragPosLightPerspective.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = fragPosLightPerspective.z;
    float shadowMultiplier = 1.0f; // 1 means no shadow
    if(fragPosLightPerspective.z <= 1.0f)
    {
        shadowMultiplier = closestDepth < currentDepth - 0.00015f ? 0.0f : 1.0f; // currentDepth - 0.0... is bias offset
    }

    vec3 textureCollor = texture(diffuseTexture, textureCoords).rgb;
    float applyShadow = (diffuse + specular) * shadowMultiplier;
    outColor = vec4((ambientLight + applyShadow) * textureCollor, 1.0f);
}
