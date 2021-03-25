

// VertexShader�ɑ����Ă���f�[�^
struct VS_IN
{
    float4 pos          : POSITION0;
    float2 texture_pos  : TEXCOORD0;
};

// VertexShader����o�͂���f�[�^
struct VS_OUT
{
    float4 pos         : SV_POSITION;
    float2 texture_pos : TEXCOORD0;
};

// �萔�o�b�t�@
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix projection;
};


// �G���g���[�|�C���g
VS_OUT vs_main( VS_IN input )
{
    VS_OUT output = (VS_OUT)0;
    
    // �s��ϊ�
    matrix wp = world * projection;
    output.pos = mul(input.pos, wp);
    
    output.texture_pos = input.texture_pos;

	return output;
}