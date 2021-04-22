// VertexShader�ɑ����Ă���f�[�^�\��
struct VS_IN
{
        float4 pos          : POSITION0;
        float4 nor          : NORMAL0;
        float4 texture_pos  : TEXTURE0;
};
 
// VertexShader����o�͂���f�[�^�\��
struct VS_OUT
{
    float4 pos                  : SV_POSITION;
    float4 posw                 : POSITION0;
    float3 norw                 : NORMAL0;
    float2 texture_pos          : TEXCOORD0;
    float3 light                : TEXCOORD1;
    float3 eye_vec              : TEXCOORD2;
    float4 light_tex_coord      : TEXCOORD3;
    float4 light_view_pos       : TEXCOORD4;
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
 
VS_OUT vs_main( VS_IN input )
{
        VS_OUT output = (VS_OUT)0;
        
        // ���[�J�����W * ���[���h���W�ϊ��s��
        output.posw = mul(input.pos,  World);
        // ���[���h���W * �r���[���W�ϊ��s��
        output.pos = mul(output.posw, View);
        // �r���[���W * �v���W�F�N�V�������W�ϊ��s��
        output.pos = mul(output.pos, Projection);

        // �e�N�X�`�����W
        output.texture_pos = input.texture_pos;

        // �@���x�N�g��
        output.norw = normalize ( mul(input.nor, (float3x3)World) );

        output.eye_vec = normalize(CameraPos - output.posw);
        
        output.light = normalize(Light);
        
        output.light_view_pos = mul(output.posw, LightView);
        output.light_view_pos = mul(output.light_view_pos, Projection);
        output.light_tex_coord = mul(output.light_view_pos, ClipUV);

        return output;
}