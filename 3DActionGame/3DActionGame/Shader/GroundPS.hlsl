
/****************************************
           ���̓p�����[�^
****************************************/

// PixelShader�ɑ����Ă���f�[�^�\��
struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 posw : POSITION0;
    float3 norw : NORMAL0;
    float2 texture_pos : TEXCOORD0;
    float3 light : LIGHT0;
    float3 eye_vec : EYE0;
    float4 light_tex_coord : TEXCOORD1;
    float4 light_view_pos : LIGHT_VIEW_POS0;
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

Texture2D    Texture        : register(t0);
SamplerState Sampler        : register(s0);
Texture2D    TextureDepth   : register(t1);
SamplerComparisonState ShadowSampler  : register(s1);


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
    float4 specular = pow(saturate(dot(R, input.eye_vec)), 60);
    
    //float4 specular = Phong(input.norw, input.eye_vec, input.light);
    
    float4 vertical = float4(0.0, 1.0, 0.0, 0.0);
    float v_d_n = saturate(dot(vertical, N));
    
    // ���F
    float4 brown = float4(0.45, 0.31, 0.18, 1.0);
    // ��
    float4 green = float4(0.54, 0.67, 0.22, 1.0);
    
   // �g�U��
    float4 diffuse   = lerp(brown, green, v_d_n);
    float half_ndl   = NL * 0.5 + 0.5;
    float square_ndl = half_ndl * half_ndl;
    diffuse *= square_ndl;
    
    //float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
    //float4 diffuse = HalfLambert(saturate(NL)) + tex_color;
   
   // ����
    float4 ambient = diffuse / 2.0;
   
    float4 color = ambient + diffuse + specular;
    
   // �e
   //input.light_tex_coord.xyz /= input.light_tex_coord.w;
   //float max_depth_slope = max(abs(ddx(input.light_tex_coord.z)), abs(ddy(input.light_tex_coord.z)));
   //float tex_value = TextureDepth.Sample(ShadowSampler, input.light_tex_coord.xy).r;
   //float light_length = input.light_view_pos.z / input.light_view_pos.w;
   //if ((tex_value) < light_length - 0.00003)
   //{
   //    color /= 3;
   //}
   
    //input.light_tex_coord.xyz /= input.light_tex_coord.w;
    //float light_length = input.light_view_pos.z / input.light_view_pos.w;
    //float shadow = TextureDepth.SampleCmpLevelZero(ShadowSampler, input.light_tex_coord.xy, light_length);
    //
    //float4 shadow_color = color / 3.0;
    //
    //color = lerp(color, shadow_color, shadow);

    float2 sm_uv = input.light_tex_coord.xy / input.light_tex_coord.w;
    if (sm_uv.x > 0.0 && sm_uv.x < 1.0
        && sm_uv.y > 0.0 && sm_uv.y < 1.0)
    {   
        float light_length = input.light_view_pos.z / input.light_view_pos.w;
        float shadow = TextureDepth.SampleCmpLevelZero(ShadowSampler, sm_uv, light_length - 0.0003);
        float4 shadow_color = color / 3.0;
        
        color = lerp(color, shadow_color, shadow);
    }
    
    return color;
}