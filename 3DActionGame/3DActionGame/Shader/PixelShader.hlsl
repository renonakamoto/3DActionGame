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
    float4   CameraPos;
    float4   Light;
    float4   MaterialAmbient;
    float4   MaterialDiffuse;
    float4   MaterialSpecular;
};

Texture2D    Texture       : register(t0[0]); // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
Texture2D    TextureDepth  : register(t1[0]);
SamplerState Sampler       : register(s0[0]); // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�
SamplerState ShadowSampler : register(s1[0]);

float4 ps_main(PS_IN input) : SV_Target
{
    // �@���x�N�g��
    float4 N = float4(input.norw, 0.0);
    // ���C�g�x�N�g��
    float4 L = float4(input.light, 0.0);
    // �@���ƃ��C�g�̓��ςŌ��̓��������Z�o
    float NL = saturate(dot(N, L));
    // ���˃x�N�g�����Z�o
    float4 R = normalize(-L + 2.0 * N * NL);

    // ���ʔ��ˌ�
    float4 specular = pow(saturate(dot(R, input.eye_vec)), 200) * MaterialSpecular;

    // �g�U��
    float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
    float4 diffuse   = (tex_color * tex_color.w) + (MaterialDiffuse * MaterialDiffuse.w);  
    
    // ����
    float4 ambient = diffuse / 2.0;

    // �t�H���V�F�[�f�B���O(�A���r�G���g�� + �f�B�q���[�Y�� + �X�y�L�����[��)
    float4 color = ambient + (diffuse * NL) + specular;

    
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