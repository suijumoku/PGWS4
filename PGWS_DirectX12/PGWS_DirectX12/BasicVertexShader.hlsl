#include "BasicShaderHeader.hlsli"

Output BasicVS(
    float4 pos : POSITION,
    float4 normal : NORMAL,
    float2 uv : TEXCOORD,
    min16uint2 bonebo : BONE_NO,
    min16uint weight : WEIGHT
    )
{
    Output output; // �s�N�Z���V�F�[�_�[�ɓn���l
    output.svpos = mul(mul(viewproj,world),pos); // �V�F�[�_�[�ł͗�D��Ȃ̂Œ���
    normal.w = 0; // �������d�v�i���s�ړ������𖳌��ɂ���j
    output.normal = mul(world, normal); // �@���ɂ����[���h�ϊ����s��
    output.uv = uv;
    return output;
}


//#include"BasicType.hlsli"

//cbuffer cbuff0 : register(b0)
//{
//    matrix mat;
//};

//BasicType BasicVS(float4 pos : POSITION, float2 uv : TEXCOORD)
//{
//    BasicType output;
//    output.svpos = mul(mat, pos);
//    output.uv = uv;
//    return output;
//}