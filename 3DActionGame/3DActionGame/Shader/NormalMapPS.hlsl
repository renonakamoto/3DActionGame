
/****************************************
           ���̓p�����[�^
****************************************/

// PixelShader�ɑ����Ă���f�[�^�\��
struct PS_IN
{
    float4 pos                     : SV_POSITION;
    float4 posw                    : POSITION0;
    float3 norw                    : NORMAL0;
    float2 texture_pos             : TEXCOORD0;
    float3 light                   : LIGHT0;
    float3 eye_vec                 : EYE0;
    float4 light_tex_coord         : TEXCOORD1;
    float4 light_view_pos          : LIGHT_VIEW_POS0;
    float4 light_tangent_direct    : TANGENT_LIGHT;
};

/****************************************
          �萔�o�b�t�@
****************************************/
 
cbuffer ConstantBuffer : register(b0)
{
    float4x4 World; //! ���[���h�s��
    float4x4 View; //! �r���[�s��
    float4x4 Projection; //! �v���W�F�N�V�����s��
    float4x4 LightView; //! ���C�g���猩���r���[�s��
    float4x4 LightProjection; //! ���C�g���猩���v���W�F�N�V�����s��
    float4x4 ClipUV; //! UV�ϊ��p�s��
    float4 CameraPos; //! �J�������W
    float4 Light; //! ���C�g�̕���
    float4 MaterialAmbient; //! �A���r�G���g��
    float4 MaterialDiffuse; //! �f�B�q���[�Y��
    float4 MaterialSpecular; //! �X�y�L�����[��
};

/****************************************
          �e�N�X�`��
****************************************/

Texture2D Texture : register(t0);           // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
SamplerState Sampler : register(s0);        // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�
Texture2D TextureDepth : register(t1);
SamplerState ShadowSampler : register(s1);
Texture2D NormalTexture : register(t2);


/****************************************
          ���ʊ֐�
****************************************/

float4 HalfLambert(float ndl)
{
    float half_ndl = ndl * 0.5 + 0.5;
    float square_ndl = half_ndl * half_ndl;
	
    return MaterialDiffuse * square_ndl * MaterialDiffuse.w;
}

float Phong(float3 normal, float3 eye_vec, float3 lig_dir)
{
    // ���C�g�̔��˃x�N�g�������߂�
    float ndl = saturate(dot(normal, lig_dir));
    float3 ref_vec = normalize(-lig_dir + 2.0 * normal * ndl);
    
    // ���ʔ��˂̋������Z�o
    float r_d_e = saturate(dot(ref_vec, eye_vec));
    
    // �������i��
    r_d_e = pow(r_d_e, 60.0);
    
    return r_d_e;
}
/****************************************
            �G���g���[�֐�
****************************************/
float4 ps_main(PS_IN input) : SV_Target
{
    // ���ʔ��ˌ�
    float4 specular = Phong(input.norw, input.eye_vec, input.light);
   
   // �g�U��
    float n_d_l = saturate(dot(input.norw, input.light));
    float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
    float4 diffuse = HalfLambert(n_d_l) + tex_color;
   
   // ����
    float4 ambient = diffuse / 2.0;
    
    // �������C�g
    float3 normal_in_view = mul(input.norw, (float3x3) View);
    float  power1         = 1.0 - max(0.0, dot((float3)-Light, input.norw));
    float  power2         = 1.0 - max(0.0, normal_in_view.z * -1.0);
    float  lim_power = power1 * power2;
    lim_power = pow(lim_power, 4.0);
   
    float4 color = ambient + diffuse + specular + lim_power;
    
    // �o��
    return color + float4(0.1, 0.1, 0.1, 0.0);
}