// �V�F�[�_�[�ɑ����{�[���s��̍ő吔
#define MAX_BONE_MATRIX 255

// PixelShader�ɑ����Ă���f�[�^�\��
struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 posw : POSITION0;
    float3 norw : NORMAL0;
    float2 texture_pos : TEXCOORD0;
    float3 light : TEXCOORD1;
    float3 eye_vec : TEXCOORD2;
    float4 light_tex_coord : TEXCOORD3;
    float4 light_view_pos : TEXCOORD4;
};
 
cbuffer ConstantBuffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4x4 LightView;
    float4x4 ClipUV;
    matrix   BoneWorld[MAX_BONE_MATRIX];
    float4   CameraPos;
    float4   Light;
    float4   Attenuation;
    float4   MaterialAmbient;
    float4   MaterialDiffuse;
    float4   MaterialSpecular;
};

Texture2D    Texture : register(t0[0]); // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
Texture2D    TextureDepth : register(t1[0]);
SamplerState Sampler : register(s0[0]); // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�
SamplerState ShadowSampler : register(s1[0]);

float4 ps_main(PS_IN input) : SV_Target
{
    // ����
    float4 ambient = MaterialAmbient;
    // �g�U��
    float NL = saturate(dot(input.norw, input.light));
    float4 diffuse = Texture.Sample(Sampler, input.texture_pos);
    //float4 diffuse = (MaterialDiffuse / 2 + Texture.Sample(Sampler, input.texture_pos) / 2) * NL;
    // ���ʔ��ˌ�
    float3 reflect = normalize(2 * NL * input.norw - input.light);
    float4 specular = pow(saturate(dot(reflect, input.eye_vec)), 4) * MaterialSpecular;
    // �t�H��
    float4 color = diffuse;
    // �A���t�@�u�����h
    //color.a = MaterialDiffuse.a;

    // �e
    //input.light_tex_coord /= input.light_tex_coord.w;
    //float max_depth_slope = max(abs(ddx(input.light_tex_coord.z)), abs(ddy(input.light_tex_coord.z)));
    //float tex_value = TextureDepth.Sample(Sampler, input.light_tex_coord).r;
    //float light_length = input.light_view_pos.z / input.light_view_pos.w;
    //if ((tex_value) < light_length)
    //{
    //    color /= 3;
    //}

    return color;
}