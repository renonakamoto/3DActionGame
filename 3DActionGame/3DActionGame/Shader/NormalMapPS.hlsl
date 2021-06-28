
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

float4 Phong(float3 nw, float3 eye_vec, float3 light)
{
    // �@���x�N�g��
    float4 n = float4(nw, 0.0);
    // ���C�g�x�N�g��
    float4 l = float4(light, 0.0);
    // �@���ƃ��C�g�̓��ςŌ��̓��������Z�o
    float ndl = dot(n, l);
    // ���˃x�N�g�����Z�o
    float4 reflect = normalize(-l + 2.0 * n * ndl);
    
    return MaterialSpecular * pow(saturate(dot(reflect.xyz, eye_vec)), 60) * MaterialSpecular.z;
}


/****************************************
          �C���^�[�t�F�[�X
****************************************/

/*
    ���C�e�B���O�̃C���^�[�t�F�[�X
*/
interface BaseLight
{
    float4 GetColor(PS_IN input);
};

/****************************************
          �N���X
****************************************/

class SimpleShading : BaseLight
{
    float4 GetColor(PS_IN input)
    {
        /* AmbientColor */
        float4 ambient = MaterialAmbient * MaterialAmbient.w;
        
        /* DiffuseColor */
        float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
        float ndl = saturate(dot(input.norw, input.light));
        float4 diffuse = HalfLambert(saturate(ndl)) + tex_color;
        
        /* SpecularColor */
        float4 specular = Phong(input.norw, input.eye_vec, input.light);
        
        return ambient + diffuse + specular;
    }
};

class PhongShading : BaseLight
{
    float4 GetColor(PS_IN input)
    {
        float ndl = saturate(dot(input.norw, input.light));
        float4 diffuse = HalfLambert(ndl);
        
        float4 specular = Phong(input.norw, input.eye_vec, input.light);
        
        return diffuse + specular;

    }
};

/****************************************
            �G���g���[�֐�
****************************************/
float4 ps_main(PS_IN input) : SV_Target
{
   // �@���x�N�g��
    float4 N = float4(input.norw, 0.0);
   // ���C�g�x�N�g��
    float4 L = float4(input.light, 0.0);
   // �@���ƃ��C�g�̓��ςŌ��̓��������Z�o
    float NL = dot(N, L);
   // ���˃x�N�g�����Z�o
    float4 R = normalize(-L + 2.0 * N * NL);
   
    // ���ʔ��ˌ�
    float4 specular = pow(saturate(dot(R, input.eye_vec)), 50);
    
    //float4 specular = Phong(input.norw, input.eye_vec, input.light);
   
    float3 normal_color = NormalTexture.Sample(Sampler, input.texture_pos);
    float3 normal_vec = 2 * normal_color - 1.0f;
    normal_vec = normalize(normal_vec);
   
    float3 bright = dot(input.light_tangent_direct.xyz, normal_vec);
    bright = max(0.0, bright);
   
   // �g�U��
    float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
    float4 diffuse = HalfLambert(saturate(NL)) + tex_color;
   
   // ����
    float4 ambient = diffuse / 2.0;
    
    float3 normal_in_view = mul(input.norw, (float3x3) View);
    float  power1 = 1.0 - max(0.0, dot((float3)-Light, input.norw));
    float  power2 = 1.0 - max(0.0, normal_in_view.z * -1.0);
    float  lim_power = power1 * power2;
    lim_power = pow(lim_power, 4.0);
   
    float4 color = ambient + diffuse + specular + lim_power;
    
    // �e
    //input.light_tex_coord.xyz /= input.light_tex_coord.w;
    //float max_depth_slope = max(abs(ddx(input.light_tex_coord.z)), abs(ddy(input.light_tex_coord.z)));
    //float tex_value = TextureDepth.Sample(ShadowSampler, input.light_tex_coord.xy).r;
    //float light_length = input.light_view_pos.z / input.light_view_pos.w;
    //if ((tex_value + 0.0005) > light_length)
    //{
    //    color /= 3;
    //}

    
    return color + float4(0.1, 0.1, 0.1, 0.0);
    //return color;
}