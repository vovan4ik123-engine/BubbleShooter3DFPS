#version 300 es

precision highp float; // highp mediump lowp

in vec2 textureCoords;

out vec4 outColor;

uniform sampler2D diffuseTexture;
uniform float loadingProgressInScreenSpace;
uniform int displayPortraitOrientation; // 1 = PORTRAIT, 0 = LANDSCAPE

void main()
{
    if(displayPortraitOrientation == 1)
    {
        if(gl_FragCoord.y < loadingProgressInScreenSpace)
        {
            outColor = texture(diffuseTexture, textureCoords);
        }
        else
        {
            outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    else
    {
        if(gl_FragCoord.x < loadingProgressInScreenSpace)
        {
            outColor = texture(diffuseTexture, textureCoords);
        }
        else
        {
            outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}
