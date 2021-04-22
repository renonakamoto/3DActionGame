
// �V�F�[�_�[�ɑ����{�[���s��̍ő吔
#define MAX_BONE_MATRIX 255

// VertexShader�ɑ����Ă���f�[�^�\��
struct VS_IN
{
        float4 pos          : POSITION0;
        float4 nor          : NORMAL0;
        float4 color        : COLOR0;
        float2 texture_pos  : TEXCOORD0;
        uint4  bones        : BONE_INDEX;
        float4 weights      : BONE_WEIGHT;
};
 
// VertexShader����o�͂���f�[�^�\��
struct VS_OUT
{
        float4 pos             : SV_POSITION;   
        float4 posw            : POSITION0;
        float3 norw            : NORMAL0;
        float2 texture_pos     : TEXCOORD0;
        float3 light           : TEXCOORD1;
        float3 eye_vec         : TEXCOORD2;
        float4 light_tex_coord : TEXCOORD3;
        float4 light_view_pos  : TEXCOORD4;
};
 
cbuffer ConstantBuffer : register(b0)
{
    float4x4 World;             //! ���[���h�s��
    float4x4 View;              //! �r���[�s��
    float4x4 Projection;        //! �v���W�F�N�V�����s��
    float4x4 LightView;         //! ���C�g�̃r���[�s��
    float4x4 ClipUV;            //! UV�ϊ��p�s��
    float4   CameraPos;         //! �J�������W
    float4   Light;             //! ���C�g�̕���
    float4   MaterialAmbient;   //! �A���r�G���g��
    float4   MaterialDiffuse;   //! �f�B�q���[�Y��
    float4   MaterialSpecular;  //! �X�y�L�����[��
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
    Skin output = (Skin)0;
    
    float4 pos = input.pos;
    float3 nor = input.nor.xyz;

    // �{�[��0
    uint bone    = input.bones.x;
    float weight = input.weights.x;
    matrix m     = BoneWorld[bone];
    output.pos  += weight * mul(pos, m);
    output.nor  += weight * mul(nor, (float3x3)m);

    // �{�[��1
    bone        = input.bones.y;
    weight      = input.weights.y;
    m           = BoneWorld[bone];
    output.pos += weight * mul(pos, m);
    output.nor += weight * mul(nor, (float3x3)m);

    // �{�[��2
    bone        = input.bones.z;
    weight      = input.weights.z;
    m           = BoneWorld[bone];
    output.pos += weight * mul(pos, m);
    output.nor += weight * mul(nor, (float3x3)m);

    // �{�[��3
    bone        = input.bones.w;
    weight      = input.weights.w;
    m           = BoneWorld[bone];
    output.pos += weight * mul(pos, m);
    output.nor += weight * mul(nor, (float3x3)m);

    return output;
}

 
VS_OUT vs_main( VS_IN input )
{
        VS_OUT output = (VS_OUT)0;

        // �X�L�����b�V�����s��
        Skin skinned = SkinVert(input);
        
        // ���[���h���W�ɕϊ�
        output.posw = mul(skinned.pos, World);
        // ���[���h���W * �r���[���W�ϊ��s��
        output.pos = mul(output.posw, View);
        // �r���[���W * �v���W�F�N�V�������W�ϊ��s��
        output.pos = mul(output.pos, Projection);

        // �e�N�X�`�����W
        output.texture_pos = input.texture_pos;
        
        // �@���x�N�g��
        output.norw = normalize(mul(skinned.nor, (float3x3)World));
        //output.norw = saturate(dot(normal, Light));
        
        // �J�����̌���
        output.eye_vec = normalize(CameraPos - output.posw);

        // ���C�g�̕���
        output.light = normalize(Light);

        /*
            �V���h�E�}�b�v�p
        */
        
        // ���C�g���_�ł̃r���[���W�ϊ�
        output.light_view_pos = mul(output.posw, LightView);
        // ���C�g���_�ł̃v���W�F�N�V�������W�ϊ�
        output.light_view_pos = mul(output.light_view_pos, Projection);
        // �e�N�X�`��UV�̕ϊ�
        output.light_tex_coord = mul(output.light_view_pos, ClipUV);

        return output;
}