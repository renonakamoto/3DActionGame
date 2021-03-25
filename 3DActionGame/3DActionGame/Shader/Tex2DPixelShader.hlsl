
// PixelShader�ɑ����Ă���f�[�^�\��
struct PS_IN
{
    float4 pos         : SV_POSITION;
    float2 texture_pos : TEXCOORD0;
};

// �e�N�X�`�����
Texture2D    Texture : register(t0);
SamplerState Sampler : register(s0);


// �G���g���[�|�C���g
float4 ps_main(PS_IN input) : SV_TARGET
{
    // ���̂܂܏o��
    return Texture.Sample(Sampler, input.texture_pos);
}