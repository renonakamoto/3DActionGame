// VertexShader�ɑ����Ă���f�[�^�\��
struct VS_IN
{
        float4 pos : POSITION0;
        float4 nor : NORMAL0;
        float4 texture_pos : TEXTURE0;
};
 
// VertexShader����o�͂���f�[�^�\��
struct VS_OUT
{
        float4 pos : SV_POSITION;
        float4 nor : NORMAL;
        float4 texture_pos : TEXTURE0;
};
 
cbuffer ConstantBuffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4   CameraPos;
    float4   LightVector;
    float4   LightColor;
    float4   MaterialAmbient;
    float4   MaterialDiffuse;
    float4   MaterialSpecular;
};
 
VS_OUT vs_main( VS_IN input )
{
        VS_OUT output;

        // ���[�J�����W * ���[���h���W�ϊ��s��
        output.pos = mul(input.pos,  World);
        // ���[���h���W * �r���[���W�ϊ��s��
        output.pos = mul(output.pos, View);
        // �r���[���W * �v���W�F�N�V�������W�ϊ��s��
        output.pos = mul(output.pos, Projection);

        float4 normal;
        
        input.nor.w = 0.0;

        normal = mul(input.nor, World).xyzw;
        normal = normalize(normal);

        output.nor = saturate(dot(normal, LightVector));

        return output;
}