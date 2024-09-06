#version 300 es

precision highp float; // highp mediump lowp

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoords;

out vec2 textureCoords;
out vec3 normal;
out vec3 fragPos;
out vec4 fragPosLightPerspective;

uniform mat4 MVPMatrix;
uniform mat4 MVPLightMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

void main()
{
    textureCoords = inTextureCoords;
    normal = normalMatrix * inNormal;

    vec4 inPosVec4 = vec4(inPosition, 1.0f);
    fragPos = (modelMatrix * inPosVec4).xyz;
    fragPosLightPerspective = MVPLightMatrix * inPosVec4; // transform to view space then to clip space
    // Perspective divide to transform vertex from clip space to NDC(-1.0f 1.0f), then to (0.0f 1.0f)
    fragPosLightPerspective.xyz = (fragPosLightPerspective.xyz / fragPosLightPerspective.w) * 0.5f + 0.5f;

    gl_Position = MVPMatrix * inPosVec4;
}
