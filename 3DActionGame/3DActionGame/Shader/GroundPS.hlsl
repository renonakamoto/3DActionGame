
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
SamplerState ShadowSampler : register(s1);


/****************************************
          ���ʊ֐�
****************************************/

float HalfLambert(float3 lig_dir, float3 normal)
{
    float ndl = saturate(dot(normal, lig_dir));
    
    float half_ndl = ndl * 0.5 + 0.5;
    float square_ndl = half_ndl * half_ndl;
	
    return square_ndl;
}

float4 Phong(float3 normal, float3 eye_vec, float3 lig_dir)
{
    // ���C�g�̔��˃x�N�g�������߂�
    float ndl = saturate(dot(normal, lig_dir));
    float3 ref_vec = normalize(-lig_dir + 2.0 * normal * ndl);
    
    // ���ʔ��˂̋������Z�o
    float t = saturate(dot(ref_vec, eye_vec));
    
    // �������i��
    t = pow(t, 60.0);
    
    return t;
}
/****************************************
            �G���g���[�֐�
****************************************/
float4 ps_main(PS_IN input) : SV_Target
{    
    // ������̒P�ʃx�N�g��
    float3 up_n = float3(0.0, 1.0, 0.0);
    // ������̃x�N�g���Ɩ@���̓��ς��s��
    float v_d_n = saturate(dot(up_n, input.norw));
    
    // ��
    float3 brown = float3(0.45, 0.31, 0.18);
    // ��
    float3 green = float3(0.54, 0.67, 0.22);

    /*
        ����̏ꍇ�A���ς̌��ʂ�
        �Ζʂɋ߂��ق�[0.0]�ɂȂ�
        ���ʂɋ߂��ق�[1.0]�ɂȂ�
        ���̕�Ԓl�Ɏg�p���邱�ƂŁA
        ���ʂł́A[��]
        �Ζʂł́A[��]
        �ɂȂ�B
    */
    float3 diffuse = lerp(brown, green, v_d_n);
    
    // ���˗������߂�
    float t = HalfLambert(Light.xyz, input.norw);
    diffuse *= t;  
   
    // ����
    float3 ambient = diffuse / 2.0;
    
    // �X�y�L�����[�J���[
    float3 specular = Phong(input.norw, input.eye_vec, Light.xyz);
   
    float3 color = ambient + diffuse + specular;
    
    // �X�N���[���̉���
    float SCREEN_WIDTH  = 1280.0;
    // �X�N���[���̏c��
    float SCREEN_HEIGHT = 720.0;
    // ���炷�s�N�Z����
    float OFFSET_PIXEL  = 1.0;
    
    // �e
    float2 sm_uv = input.light_tex_coord.xy / input.light_tex_coord.w;
    if (sm_uv.x >= 0.0 && sm_uv.x <= 1.0
        && sm_uv.y >= 0.0 && sm_uv.y <= 1.0)
    {   
        // �V���h�E�A�N�l�΍�Ńo�C�A�X���|����
        float bias = 0.0003;
        
        // ����̃��C�g���猩���[�x�l���Z�o
        float z_light_view = input.light_view_pos.z;
        
        // �s�N�Z���̃I�t�Z�b�g�l���v�Z
        float offset_u = OFFSET_PIXEL / SCREEN_WIDTH;
        float offset_v = OFFSET_PIXEL / SCREEN_HEIGHT;
        
        // ��ƂȂ�e�N�Z���܂ދ߂��̃e�N�Z�����T���v�����O����
        float z_shadow_map_0 = TextureDepth.Sample(ShadowSampler, sm_uv).r;                                                    
        float z_shadow_map_1 = TextureDepth.Sample(ShadowSampler, sm_uv + float2(offset_u,      0.0)).r;
        float z_shadow_map_2 = TextureDepth.Sample(ShadowSampler, sm_uv + float2(offset_u, offset_v)).r;
        float z_shadow_map_3 = TextureDepth.Sample(ShadowSampler, sm_uv + float2(     0.0, offset_v)).r;
        
        // �V���h�E�}�b�v�ɕۑ�����Ă���[�x�l���������ꍇ�A�e�ɂȂ�̂Ŏˉe���� 1 ���Z����        
        float shadow_rate = 0.0;
        if (z_light_view > z_shadow_map_0 + bias){
            shadow_rate += 1.0;
        }
        if (z_light_view > z_shadow_map_1 + bias){
            shadow_rate += 1.0;
        }
        if (z_light_view > z_shadow_map_2 + bias){
            shadow_rate += 1.0;
        }
        if (z_light_view > z_shadow_map_3 + bias){
            shadow_rate += 1.0;
        }
        // �ˉe���̕��ς����߂� 0 �` 1 (0% �` 100%)�ɂȂ�
        shadow_rate /= 4.0;
        
        // ���`��ԂŎˉe���ɉ����ĐF�����炩�ɕω�������
        float3 shadow_color = color / 3.0;
        color = lerp(color, shadow_color, shadow_rate);
    }
    
    return float4(color, 1.0);
}