#version 300 es

precision highp float; // highp mediump lowp

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoords;
layout(location = 3) in ivec4 inBoneIDs; // INT pointer here
layout(location = 4) in vec4 inWeights;

out vec2 textureCoords;

const int MAX_BONES = 31; // 31 bones should be enough for most models

uniform mat4 bonesMatrices[MAX_BONES];
uniform mat4 MVPMatrix;

void main()
{
    textureCoords = inTextureCoords;

    // animation must contain minimum 0 bones per vertex. Maximum = 4
    mat4 boneTransf;
    if(inBoneIDs[0] != -1)
    {
        //boneTransf = bonesMatrices[inBoneIDs[0]] * inWeights[0];

        boneTransf[0][0] = bonesMatrices[inBoneIDs[0]][0][0] * inWeights[0];
        boneTransf[0][1] = bonesMatrices[inBoneIDs[0]][0][1] * inWeights[0];
        boneTransf[0][2] = bonesMatrices[inBoneIDs[0]][0][2] * inWeights[0];

        boneTransf[1][0] = bonesMatrices[inBoneIDs[0]][1][0] * inWeights[0];
        boneTransf[1][1] = bonesMatrices[inBoneIDs[0]][1][1] * inWeights[0];
        boneTransf[1][2] = bonesMatrices[inBoneIDs[0]][1][2] * inWeights[0];

        boneTransf[2][0] = bonesMatrices[inBoneIDs[0]][2][0] * inWeights[0];
        boneTransf[2][1] = bonesMatrices[inBoneIDs[0]][2][1] * inWeights[0];
        boneTransf[2][2] = bonesMatrices[inBoneIDs[0]][2][2] * inWeights[0];

        boneTransf[3][0] = bonesMatrices[inBoneIDs[0]][3][0] * inWeights[0];
        boneTransf[3][1] = bonesMatrices[inBoneIDs[0]][3][1] * inWeights[0];
        boneTransf[3][2] = bonesMatrices[inBoneIDs[0]][3][2] * inWeights[0];

        boneTransf[3][3] = bonesMatrices[inBoneIDs[0]][3][3] * inWeights[0];

        if(inBoneIDs[1] != -1)
        {
            //boneTransf += bonesMatrices[inBoneIDs[1]] * inWeights[1];

            boneTransf[0][0] += bonesMatrices[inBoneIDs[1]][0][0] * inWeights[1];
            boneTransf[0][1] += bonesMatrices[inBoneIDs[1]][0][1] * inWeights[1];
            boneTransf[0][2] += bonesMatrices[inBoneIDs[1]][0][2] * inWeights[1];

            boneTransf[1][0] += bonesMatrices[inBoneIDs[1]][1][0] * inWeights[1];
            boneTransf[1][1] += bonesMatrices[inBoneIDs[1]][1][1] * inWeights[1];
            boneTransf[1][2] += bonesMatrices[inBoneIDs[1]][1][2] * inWeights[1];

            boneTransf[2][0] += bonesMatrices[inBoneIDs[1]][2][0] * inWeights[1];
            boneTransf[2][1] += bonesMatrices[inBoneIDs[1]][2][1] * inWeights[1];
            boneTransf[2][2] += bonesMatrices[inBoneIDs[1]][2][2] * inWeights[1];

            boneTransf[3][0] += bonesMatrices[inBoneIDs[1]][3][0] * inWeights[1];
            boneTransf[3][1] += bonesMatrices[inBoneIDs[1]][3][1] * inWeights[1];
            boneTransf[3][2] += bonesMatrices[inBoneIDs[1]][3][2] * inWeights[1];

            boneTransf[3][3] += bonesMatrices[inBoneIDs[1]][3][3] * inWeights[1];

            if(inBoneIDs[2] != -1)
            {
                //boneTransf += bonesMatrices[inBoneIDs[2]] * inWeights[2];

                boneTransf[0][0] += bonesMatrices[inBoneIDs[2]][0][0] * inWeights[2];
                boneTransf[0][1] += bonesMatrices[inBoneIDs[2]][0][1] * inWeights[2];
                boneTransf[0][2] += bonesMatrices[inBoneIDs[2]][0][2] * inWeights[2];

                boneTransf[1][0] += bonesMatrices[inBoneIDs[2]][1][0] * inWeights[2];
                boneTransf[1][1] += bonesMatrices[inBoneIDs[2]][1][1] * inWeights[2];
                boneTransf[1][2] += bonesMatrices[inBoneIDs[2]][1][2] * inWeights[2];

                boneTransf[2][0] += bonesMatrices[inBoneIDs[2]][2][0] * inWeights[2];
                boneTransf[2][1] += bonesMatrices[inBoneIDs[2]][2][1] * inWeights[2];
                boneTransf[2][2] += bonesMatrices[inBoneIDs[2]][2][2] * inWeights[2];

                boneTransf[3][0] += bonesMatrices[inBoneIDs[2]][3][0] * inWeights[2];
                boneTransf[3][1] += bonesMatrices[inBoneIDs[2]][3][1] * inWeights[2];
                boneTransf[3][2] += bonesMatrices[inBoneIDs[2]][3][2] * inWeights[2];

                boneTransf[3][3] += bonesMatrices[inBoneIDs[2]][3][3] * inWeights[2];

                if(inBoneIDs[3] != -1)
                {
                    //boneTransf += bonesMatrices[inBoneIDs[3]] * inWeights[3];

                    boneTransf[0][0] += bonesMatrices[inBoneIDs[3]][0][0] * inWeights[3];
                    boneTransf[0][1] += bonesMatrices[inBoneIDs[3]][0][1] * inWeights[3];
                    boneTransf[0][2] += bonesMatrices[inBoneIDs[3]][0][2] * inWeights[3];

                    boneTransf[1][0] += bonesMatrices[inBoneIDs[3]][1][0] * inWeights[3];
                    boneTransf[1][1] += bonesMatrices[inBoneIDs[3]][1][1] * inWeights[3];
                    boneTransf[1][2] += bonesMatrices[inBoneIDs[3]][1][2] * inWeights[3];

                    boneTransf[2][0] += bonesMatrices[inBoneIDs[3]][2][0] * inWeights[3];
                    boneTransf[2][1] += bonesMatrices[inBoneIDs[3]][2][1] * inWeights[3];
                    boneTransf[2][2] += bonesMatrices[inBoneIDs[3]][2][2] * inWeights[3];

                    boneTransf[3][0] += bonesMatrices[inBoneIDs[3]][3][0] * inWeights[3];
                    boneTransf[3][1] += bonesMatrices[inBoneIDs[3]][3][1] * inWeights[3];
                    boneTransf[3][2] += bonesMatrices[inBoneIDs[3]][3][2] * inWeights[3];

                    boneTransf[3][3] += bonesMatrices[inBoneIDs[3]][3][3] * inWeights[3];
                }
            }
        }
    }

    //vec4 transformedByBone = boneTransf * vec4(inPosition, 1.0f);

    gl_Position = MVPMatrix * boneTransf * vec4(inPosition, 1.0f);
}
