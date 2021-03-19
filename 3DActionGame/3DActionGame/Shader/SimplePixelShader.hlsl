// PixelShader�ɑ����Ă���f�[�^�\��
struct PS_IN
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

Texture2D    Texture : register(s0[0]); // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
SamplerState Sampler : register(s0[0]); // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�

float4 ps_main(PS_IN input) : SV_Target
{
	//float material_ambient_power = 1.0;
	//float material_diffuse_power = 1.0;
	//float total_power = material_ambient_power + material_diffuse_power;
	//
	//if (total_power == 0)
	//{
	//	total_power = 1.0;
	//}
	//
	//float ambient_power = material_ambient_power / total_power;
	//float diffuse_power = material_diffuse_power / total_power;
	//
	//float4 ambient_color = MaterialAmbient * ambient_power;
	float4 diffuse_color = (LightColor * MaterialDiffuse * input.nor);

	// �A���r�G���g�J���[ + �f�B�t���[�Y�J���[
	float4 out_color = diffuse_color;

	float4 tex_color = Texture.Sample(Sampler, input.texture_pos);

	out_color += tex_color;

	return out_color;
}