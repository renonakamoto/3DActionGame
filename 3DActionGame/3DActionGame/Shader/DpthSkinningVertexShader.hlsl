
// �V�F�[�_�[�ɑ����{�[���s��̍ő吔
#define MAX_BONE_MATRIX 255

// VertexShader�ɑ����Ă���f�[�^�\��
struct VS_IN
{
    float4 pos          : POSITION0;
    float4 nor          : NORMAL0;
    float4 color        : COLOR0;
    float2 texture_pos  : TEXCOORD0;
    uint4 bones         : BONE_INDEX;
    float4 weights      : BONE_WEIGHT;
};

// VertexShader����o�͂���f�[�^�\��
struct VS_OUT
{
    float4 pos   : SV_POSITION;
    float4 depth : POSITION0;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;             //! ���[���h�s��
    float4x4 View;              //! �r���[�s��
    float4x4 Projection;        //! �v���W�F�N�V�����s��
    float4x4 LightView;         //! ���C�g���猩���r���[�s��
    float4x4 LightProjection;   //! ���C�g���猩���v���W�F�N�V�����s��
    float4x4 ClipUV;            //! UV�ϊ��p�s��
    float4 CameraPos;           //! �J�������W
    float4 Light;               //! ���C�g�̕���
    float4 MaterialAmbient;     //! �A���r�G���g��
    float4 MaterialDiffuse;     //! �f�B�q���[�Y��
    float4 MaterialSpecular;    //! �X�y�L�����[��
};

cbuffer BoneBuffer : register(b1)
{
    matrix BoneWorld[MAX_BONE_MATRIX]; // �{�[���̎p���s��
};

struct Skin
{
    float4 pos; // ���_���W
    float3 nor; // �@���x�N�g��
};


Skin SkinVert(VS_IN input)
{
    Skin output = (Skin) 0;
    
    float4 pos = input.pos;
    float3 nor = input.nor.xyz;

    // �{�[��0
    uint bone = input.bones.x;
    float weight = input.weights.x;
    matrix m = BoneWorld[bone];
    output.pos += weight * mul(pos, m);
    output.nor += weight * mul(nor, (float3x3) m);

    // �{�[��1
    bone = input.bones.y;
    weight = input.weights.y;
    m = BoneWorld[bone];
    output.pos += weight * mul(pos, m);
    output.nor += weight * mul(nor, (float3x3) m);

    // �{�[��2
    bone = input.bones.z;
    weight = input.weights.z;
    m = BoneWorld[bone];
    output.pos += weight * mul(pos, m);
    output.nor += weight * mul(nor, (float3x3) m);

    // �{�[��3
    bone = input.bones.w;
    weight = input.weights.w;
    m = BoneWorld[bone];
    output.pos += weight * mul(pos, m);
    output.nor += weight * mul(nor, (float3x3) m);

    return output;
}


VS_OUT vs_main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // �X�L�����b�V�����s��
    Skin skinned = SkinVert(input);

    output.pos = mul(skinned.pos, World);
    output.pos = mul(output.pos, LightView);
    output.pos = mul(output.pos, LightProjection);
    output.depth = output.pos;

    return output;
}