#include <stdio.h>

#include "DirectGraphics.h"
#include "Window.h"

#pragma comment(lib,"d3d11.lib")

//#define ENABLE_MASS

bool DirectGraphics::Init()
{
    // �f�o�C�X�ƃX���b�v�`�F�C���̍쐬 
    if (CreateDeviceAndSwapChain() == false)
    {
        return true;
    }

    // �����_�[�^�[�Q�b�g�r���[�̍쐬
    if (CreateRenderTargetView() == false)
    {
        return false;
    }

    // �f�v�X�r���[�ƃX�e���V���r���[�̍쐬
    if (CreateDepthAndStencilView() == false)
    {
        return false;
    }

    // �r���[�|�[�g�ݒ�
    SetUpViewPort();
    
    // �V�F�[�_�[�̍쐬
    if (CreateShader() == false)
    {
        return false;
    }

    // �R���X�^���g�o�b�t�@�̍쐬
    if (CreateConstantBuffer() == false)
    {
        return false;
    }

    // �e�N�X�`���T���v���̍쐬
    if (CreateTextureSampler() == false)
    {
        return false;
    }

    


    //// ���X�^���C�U
    //D3D11_RASTERIZER_DESC rasterizerDesc;
    //ID3D11RasterizerState* state;
    //ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    //rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    //rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    //rasterizerDesc.FrontCounterClockwise = TRUE;
    //if (FAILED(m_Device->CreateRasterizerState(&rasterizerDesc, &state)))
    //{
    //    return false;
    //}

    //m_Context->RSSetState(state);





    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);

    // View�s��ݒ�
    DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 50.0f, -300.0f, 0.0f);
    DirectX::XMVECTOR focus = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX view_matrix = DirectX::XMMatrixLookAtLH(eye, focus, up);

    // �v���W�F�N�V�����s��ݒ�
    constexpr float fov = DirectX::XMConvertToRadians(45.0f);
    float aspect = (float)(rect.right - rect.left) / (rect.bottom - rect.top);
    float near_z = 0.1f;
    float far_z = 500000.f;
    DirectX::XMMATRIX proj_matrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, near_z, far_z);

    DirectX::XMStoreFloat4x4(&m_ConstantBufferData.View, DirectX::XMMatrixTranspose(view_matrix));
    DirectX::XMStoreFloat4x4(&m_ConstantBufferData.Projection, DirectX::XMMatrixTranspose(proj_matrix));
    DirectX::XMStoreFloat4(&m_ConstantBufferData.CameraPos, eye);

    DirectX::XMStoreFloat4x4(&m_SimpleConstantBufferData.View, DirectX::XMMatrixTranspose(view_matrix));
    DirectX::XMStoreFloat4x4(&m_SimpleConstantBufferData.Projection, DirectX::XMMatrixTranspose(proj_matrix));
    DirectX::XMStoreFloat4(&m_SimpleConstantBufferData.CameraPos, eye);
    // ���C�g�ݒ�
    DirectX::XMVECTOR light = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 0.5f, -1.0f, 0.0f));
    DirectX::XMStoreFloat4(&m_SimpleConstantBufferData.LightVector, light);

    // ���C�g�̃J���[�ݒ�
    m_SimpleConstantBufferData.LightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    // ���C�g�̐ݒ�
    DirectX::XMStoreFloat4(&m_ConstantBufferData.Light, DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 0.5f, -1.0f, 0.0f)));
    //DirectX::XMMATRIX mat_rot = DirectX::XMMatrixIdentity();
    //
    //DirectX::XMMATRIX mat   =  DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(90.f));
    //DirectX::XMMATRIX trans =  DirectX::XMMatrixTranslation(m_ConstantBufferData.Light.x, m_ConstantBufferData.Light.y, m_ConstantBufferData.Light.z);
    //mat = mat * trans;
    //DirectX::XMMATRIX light_view = DirectX::XMMatrixInverse(nullptr, mat);
    //DirectX::XMStoreFloat4x4(&m_ConstantBufferData.LightView, DirectX::XMMatrixTranspose(light_view));

    DirectX::XMMATRIX light_view = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(m_ConstantBufferData.Light.x, m_ConstantBufferData.Light.y, m_ConstantBufferData.Light.z, 0.0f),
        DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    DirectX::XMStoreFloat4x4(&m_ConstantBufferData.LightView, DirectX::XMMatrixTranspose(light_view));

    m_ConstantBufferData.Attenuation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);


    //�e�N�X�`���s��̐ݒ�
    ZeroMemory(&m_ConstantBufferData.ClipUV, sizeof(DirectX::XMFLOAT4X4));
    m_ConstantBufferData.ClipUV._11 = 0.5f;
    m_ConstantBufferData.ClipUV._22 = -0.5f;
    m_ConstantBufferData.ClipUV._33 = 1.0f;
    m_ConstantBufferData.ClipUV._41 = 0.5f;
    m_ConstantBufferData.ClipUV._42 = 0.5f;
    m_ConstantBufferData.ClipUV._44 = 1.0f;

    DirectX::XMMATRIX tex_uv = DirectX::XMMatrixSet(
        0.5f, 0, 0, 0,
        0, -0.5f, 0, 0,
        0, 0, 1.0f, 0,
        0.5f, 0.5f, 0, 1.0f);

    DirectX::XMStoreFloat4x4(&m_ConstantBufferData.ClipUV, DirectX::XMMatrixTranspose(tex_uv));

    // ���X�^���C�U
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ID3D11RasterizerState* state;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
    rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = TRUE;
    if (FAILED(m_Device->CreateRasterizerState(&rasterizerDesc, &state)))
    {
        return false;
    }

    m_Context->RSSetState(state);


    return true;
}

void DirectGraphics::Release()
{
    
    if (m_Context != nullptr)
    {
        m_Context->ClearState();
        m_Context->Release();
    }

    if (m_SwapChain != nullptr)
    {
        m_SwapChain->Release();
    }

    if (m_Device != nullptr)
    {
        m_Device->Release();
    }

}

/*
    �`��J�n�֐�
*/
void DirectGraphics::StartRendering()
{
    /*
        DirectX11�͕`��̊J�n��錾����K�v�͂Ȃ����r���[�̃N���A�𖈃t���[��
        �s��Ȃ��ƕ`����e�������������ƂɂȂ�
    */

    float clear_color[4] = { 0.0f,0.0f,1.0f,1.0f };

    // �����_�[�^�[�Q�b�g�r���[�̃N���Aw
    m_Context->ClearRenderTargetView(
                m_RenderTargetView, // �Ώۂ̃����_�[�^�[�Q�b�g�r���[
                clear_color         // �N���A����r���[�̃J���[
                );

    // �f�v�X�X�e���V���r���[�̃N���A
    m_Context->ClearDepthStencilView(
                m_DepthStencilView,                      // �Ώۂ̃r���[
                D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, // �N���A�t���O
                1.0f,                                    // �[�x�N���A�l
                0                                        // �X�e���V���N���A�l
                );

    /*
        �r���[�|�[�g�̐ݒ�
    */
    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);

    D3D11_VIEWPORT vp;
    vp.Width  = (rect.right - rect.left);
    vp.Height = (rect.bottom - rect.top);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    m_Context->RSSetViewports(1, &vp);

    /*
       �o�͐�̐ݒ�
    */
    m_Context->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

}

/*
    �`��I���֐�
    �o�b�N�o�b�t�@�ɏ������񂾏����t�����g�o�b�t�@�ɓn��
*/
void DirectGraphics::FinishRendering()
{
    // �o�b�N�o�b�t�@���t�����g�o�b�t�@�ɑ��M����   
    m_SwapChain->Present(1, 0);
}

void DirectGraphics::SetUpContext()
{
    /*
    �v���~�e�B�u�̐ݒ�
    �`�悷��|���S���̍쐬���@��ݒ肷��
    */
    m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /*
        ���_�V�F�[�_�̐ݒ�
    */
    m_Context->VSSetShader(m_VertexShader->GetShaderInterface(), NULL, 0);
    /*
        �s�N�Z���V�F�[�_�̐ݒ�
    */
    m_Context->PSSetShader(m_PixelShader->GetShaderInterface(), NULL, 0);

    /*
       �o�͐�̐ݒ�
   */
    m_Context->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
}

void DirectGraphics::SetTexture(ID3D11ShaderResourceView* texture_)
{
    m_Context->PSSetSamplers(
        0,
        1,
        &m_SamplerState);

    m_Context->PSSetShaderResources(
        0,
        1,
        &texture_);
    
}

void DirectGraphics::SetMaterial(ObjMaterial* material_)
{
    m_ConstantBufferData.MaterialAmbient = DirectX::XMFLOAT4(material_->Ambient[0],
        material_->Ambient[1],
        material_->Ambient[2], 1);
    m_ConstantBufferData.MaterialDiffuse = DirectX::XMFLOAT4(material_->Diffuse[0], material_->Diffuse[1], material_->Diffuse[2], 1);
    m_ConstantBufferData.MaterialSpecular = DirectX::XMFLOAT4(material_->Specular[0], material_->Specular[1], material_->Specular[2], 1);
}

void DirectGraphics::SetUpDxgiSwapChanDesc(DXGI_SWAP_CHAIN_DESC* dxgi)
{
    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);

    // �X���b�v�`�F�C���쐬���ɕK�v�Ȑݒ�
    ZeroMemory(dxgi, sizeof(DXGI_SWAP_CHAIN_DESC));
    
    // �o�b�t�@�̐�
    dxgi->BufferCount = 1;
    // �o�b�t�@�̉���
    dxgi->BufferDesc.Width  = static_cast<UINT>(rect.right - rect.left);
    // �o�b�t�@�̏c��
    dxgi->BufferDesc.Height = static_cast<UINT>(rect.bottom - rect.top);
    // �o�b�t�@�̃J���[�t�H�[�}�b�g
    dxgi->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // ���t���b�V�����[�g�̕��q
    dxgi->BufferDesc.RefreshRate.Numerator   = 60;
    // ���t���b�V�����[�g�̕���
    dxgi->BufferDesc.RefreshRate.Denominator = 1;
    // �X�L�������C���̕��@
    // �o�b�N�o�b�t�@���t���b�v�����Ƃ��Ƀn�[�h�E�F�A���p�\�R���̃��j�^�[��
    // �_���ǂ��`������ݒ肷��
    // ���ɗ��R���Ȃ���΃f�t�H���g�l�ł���DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED��OK
    dxgi->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    // �E�B���h�E�̃T�C�Y�ɉ����ăX�P�[�����O���邩�ǂ����̐ݒ�
    // �X�P�[�����O����ꍇ   DXGI_MODE_SCALING_STRETCHED 
    // �X�P�[�����O���Ȃ��ꍇ DXGI_MODE_SCALING_CENTERED
    dxgi->BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    // �o�b�t�@�̎g�p���@
    // �����_�[�^�[�Q�b�g�Ƃ��Ďg�p����ꍇ�́ADXGI_USAGE_RENDER_TARGET_OUTPUT 
    // �V�F�[�_�[���͗p�Ƃ��Ďg�p����ꍇ��DXGI_USAGE_SHADER_INPUT
    dxgi->BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // �o�͑Ώۂ̃E�B���h�E�n���h��
    dxgi->OutputWindow = window_handle;
    // �}���`�T���v�����O�̃T���v����(���g�p��1)
    dxgi->SampleDesc.Count = 1;
    // �}���`�T���v�����O�̕i��(���g�p��0)
    dxgi->SampleDesc.Quality = 0;
    // �E�B���h�E���[�h�w��
    dxgi->Windowed = true;
    // �X���b�v�`�F�C���̓���I�v�V����
    // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH���w�肵���ꍇ
    // �t���X�N���[���ƃE�B���h�E���[�h�̐؂�ւ����\
    dxgi->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

}

bool DirectGraphics::CreateDeviceAndSwapChain()
{
#ifdef ENABLE_MASS
    if (FAILED(D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &m_Device,
        nullptr,
        &m_Context)))
    {
        return false;
    }


    for (int i = 0; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++i)
    {
        UINT quality;
        if (SUCCEEDED(m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &quality)))
        {
            if (0 < quality)
            {
                m_SampleDesc.Count = i;
                m_SampleDesc.Quality = quality - 1;
            }
        }
    }

    // �C���^�[�t�F�[�X���擾
    IDXGIDevice1* dxgi = nullptr;
    if (FAILED(m_Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgi)))
    {
        return false;
    }

    // �A�_�v�^�[���擾
    IDXGIAdapter* adapter = nullptr;
    if (FAILED(dxgi->GetAdapter(&adapter)))
    {
        return false;
    }

    // �t�@�N�g���[���擾
    IDXGIFactory* factory = nullptr;
    adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
    if (factory == nullptr)
    {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swap_chain_desc;

    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);

    // �X���b�v�`�F�C���쐬���ɕK�v�Ȑݒ�
    ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

    // �o�b�t�@�̐�
    swap_chain_desc.BufferCount = 1;
    // �o�b�t�@�̉���
    swap_chain_desc.BufferDesc.Width = static_cast<UINT>(rect.right - rect.left);
    // �o�b�t�@�̏c��
    swap_chain_desc.BufferDesc.Height = static_cast<UINT>(rect.bottom - rect.top);
    // �o�b�t�@�̃J���[�t�H�[�}�b�g
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // ���t���b�V�����[�g�̕��q
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    // ���t���b�V�����[�g�̕���
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    // �X�L�������C���̕��@
    // �o�b�N�o�b�t�@���t���b�v�����Ƃ��Ƀn�[�h�E�F�A���p�\�R���̃��j�^�[��
    // �_���ǂ��`������ݒ肷��
    // ���ɗ��R���Ȃ���΃f�t�H���g�l�ł���DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED��OK
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    // �E�B���h�E�̃T�C�Y�ɉ����ăX�P�[�����O���邩�ǂ����̐ݒ�
    // �X�P�[�����O����ꍇ   DXGI_MODE_SCALING_STRETCHED 
    // �X�P�[�����O���Ȃ��ꍇ DXGI_MODE_SCALING_CENTERED
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // �o�b�t�@�̎g�p���@
    // �����_�[�^�[�Q�b�g�Ƃ��Ďg�p����ꍇ�́ADXGI_USAGE_RENDER_TARGET_OUTPUT 
    // �V�F�[�_�[���͗p�Ƃ��Ďg�p����ꍇ��DXGI_USAGE_SHADER_INPUT
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // �o�͑Ώۂ̃E�B���h�E�n���h��
    swap_chain_desc.OutputWindow = window_handle;
    swap_chain_desc.SampleDesc = m_SampleDesc;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    // �E�B���h�E���[�h�w��
    swap_chain_desc.Windowed = true;
    // �X���b�v�`�F�C���̓���I�v�V����
    // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH���w�肵���ꍇ
    // �t���X�N���[���ƃE�B���h�E���[�h�̐؂�ւ����\
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if (FAILED(factory->CreateSwapChain(m_Device, &swap_chain_desc, &m_SwapChain)))
    {
        return false;
    }
#else
DXGI_SWAP_CHAIN_DESC swap_chain_desc;
SetUpDxgiSwapChanDesc(&swap_chain_desc);

// �K�����ꂽFuture���x�����擾�ł���
if (FAILED(D3D11CreateDeviceAndSwapChain(
    // �f�o�C�X�쐬���Ɏg�p����r�f�I�A�_�v�^�̎w��
    // nullptr�Ŋ���̃A�_�v�^���g�p����
    nullptr,
    // �h���C�o�̃^�C�v
    // D3D_DRIVER_TYPE�̂����ꂩ���w��
    // �ʏ��D3D_DRIVER_TYPE_HARDWARE�Ŗ��Ȃ�
    D3D_DRIVER_TYPE_HARDWARE,
    // D3D_DRIVER_TYPE_SOFTWARE�w�莞�Ɏg�p
    nullptr,
    // �����^�C�����C���[�̃t���O�w��
    0,
    // D3D_FEATURE_LEVEL�w��Ŏ����Œ�`�����z����w��\
    nullptr,
    // ���D3D_FEATURE_LEVEL�z��̗v�f��
    0,
    // SDK�o�[�W����
    D3D11_SDK_VERSION,
    // �ݒ�ς݂�DXGI_SWAP_CHAIN_DESC
    &swap_chain_desc,
    // ����������������SwapChain�̏o�͐� 
    &m_SwapChain,
    // ����������������Device�̏o�͐� 
    &m_Device,
    // �ŏI�I�Ɍ��肵��Future���x���̏o�͐�
    &m_FeatureLevel,
    // �쐬���ꂽ�R���e�L�X�g���󂯎�邽�߂�ID3D11DeviceContext�|�C���^�A�h���X
    &m_Context)))
{
    return false;
}

#endif


    /*
        D3D11CreateDeviceAndSwapChain���g�p����ƃ����_�����O�p��Context���쐬�����
        Context�͕`��R�}���h�̒ǉ��A���M�Ȃǂ̏������s��
        CPU���Œǉ����ꂽ�`��R�}���h��GPU���ɑ��M����̂���Ȗ��
        Contextn�ɂ́uImmediate�v�ƁuDeferred�v��2��ނ���
        Immediate�͐��������R�}���h�𑦎����s����
        Deferred�͐��������`��R�}���h���o�b�t�@�ɂ��߂Ă����A
        ���s���߂��󂯂��炽�܂��Ă���R�}���h�����s����
        �}���`�X���b�h�ɂ��`�揈���ɗL���Ƃ���Ă���

        D3D11CreateDeviceAndSwapChain�ō쐬�����ꍇ�AImmediate�ō쐬�����
    */

    
    return true;
}

bool DirectGraphics::CreateRenderTargetView()
{
    /*
        RenderTargetView�̓����_�����O�p�C�v���C������A�E�g�v�b�g�����
        �s�N�Z������ۑ�����View�̂��ƂŁA����View�̓��e���Q�[����ʂɔ��f������B
    */

    // RenderView��SwapChain�����o�b�t�@���g�p���č쐬����̂�
    // GetBuffer���g�p���ăo�b�t�@���擾����
    ID3D11Texture2D* back_buffer;
    if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer)))
    {
        return false;
    }
    
    // RenderTargetView�̍쐬
    if (FAILED(m_Device->CreateRenderTargetView(back_buffer, nullptr, &m_RenderTargetView)))
    {
        return false;
    }

    // ����ȍ~�o�b�t�@���g�p���Ȃ��̂ŁA������Ă���
    back_buffer->Release();
    
    return true;
}

bool DirectGraphics::CreateDepthAndStencilView()
{
    /*
        CreateTexture2D�ō쐬���ꂽ�e�N�X�`�����o�b�t�@�Ƃ��Ďg�p����̂�
        D3D11_TEXTURE2D_DESC�Ńe�N�X�`���̐ݒ���s��
    */
    
    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);
    
    // �[�x�X�e���V���o�b�t�@�̍쐬
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));
    // �o�b�t�@�̉���
    texture_desc.Width = (rect.right - rect.left);
    // �o�b�t�@�̉���
    texture_desc.Height = (rect.bottom - rect.top);
    // �~�b�v�}�b�v���x��
    texture_desc.MipLevels = 1;
    // �e�N�X�`���z��̃T�C�Y
    texture_desc.ArraySize = 1;
    // �e�N�X�`���̃t�H�[�}�b�g
    // DXGI_FORMAT_D24_UNORM_S8_UINT
    // Depth24bit, Stencil8bit�ƂȂ�
    texture_desc.Format = DXGI_FORMAT_D32_FLOAT;
    // 
#ifdef ENABLE_MASS
    texture_desc.SampleDesc = m_SampleDesc;
#else
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
#endif
    // �e�N�X�`���̎g�p���@
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    // Bind�ݒ�
    // D3D11_BIND_DEPTH_STENCIL�Ŗ��Ȃ�
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    // ���\�[�X�ւ�CPU�̃A�N�Z�X����
    // 0�Ŗ��Ȃ�
    texture_desc.CPUAccessFlags = 0;
    // ���\�[�X�I�v�V�����̃t���O
    // �K�v�Ȃ����0
    texture_desc.MiscFlags = 0;
    
    // �e�N�X�`���̍쐬
    if (FAILED(m_Device->CreateTexture2D(&texture_desc, NULL, &m_DepthStencilTexture)))
    {
        return false;
    }

    // DepthStencilView�̐ݒ���s��
    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    // View�̃t�H�[�}�b�g
    dsv_desc.Format = texture_desc.Format;
    // �e�N�X�`���̎��
#ifdef ENABLE_MASS
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
#else
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
#endif
    dsv_desc.Texture2D.MipSlice = 0;
    
    // DepthStencilView�̍쐬
    if (FAILED(m_Device->CreateDepthStencilView(
        m_DepthStencilTexture,
        &dsv_desc,
        &m_DepthStencilView)))
    {
        return false;
    }

    m_Context->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

    return true;
}

bool DirectGraphics::CreateShader()
{
    m_VertexShader = new VertexShader();
    if (m_VertexShader->Create(m_Device, "Res/Shader/VertexShader.cso") == false)
    {
        return false;
    }

    m_PixelShader = new PixelShader();
    if (m_PixelShader->Create(m_Device, "Res/Shader/PixelShader.cso") == false)
    {
        return false;
    }

    m_SimpleVertexShader = new VertexShader();
    if (m_SimpleVertexShader->Create(m_Device, "Res/Shader/SimpleVertexShader.cso") == false)
    {
        return false;
    }

    m_SimplePixelShader = new PixelShader();
    if (m_SimplePixelShader->Create(m_Device, "Res/Shader/SimplePixelShader.cso") == false)
    {
        return false;
    }

    return true;
}

bool DirectGraphics::CreateConstantBuffer()
{
    D3D11_BUFFER_DESC buffer_desc;
    buffer_desc.ByteWidth           = sizeof(ConstantBuffer);
    buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags      = 0;
    buffer_desc.MiscFlags           = 0;
    buffer_desc.StructureByteStride = 0;

    if (FAILED(m_Device->CreateBuffer(&buffer_desc, nullptr, &m_ConstantBuffer)))
    {
        return false;
    }
    
    buffer_desc.ByteWidth = sizeof(SimpleConstantBuffer);
    if (FAILED(m_Device->CreateBuffer(&buffer_desc, nullptr, &m_SimpleConstantBuffer)))
    {
        return false;
    }

    buffer_desc.ByteWidth = sizeof(ConstBoneBuffer);
    if (FAILED(m_Device->CreateBuffer(&buffer_desc, nullptr, &m_ConstBoneBuffer)))
    {
        return false;
    }    

    return true;
}

bool DirectGraphics::CreateTextureSampler()
{
    D3D11_SAMPLER_DESC sampler_desc;

    ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

    sampler_desc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;


    if (FAILED(m_Device->CreateSamplerState(&sampler_desc, &m_SamplerState)))
    {
        return false;
    }

    sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampler_desc.BorderColor[0] = 1.0f;
    sampler_desc.BorderColor[1] = 1.0f;
    sampler_desc.BorderColor[2] = 1.0f;
    sampler_desc.BorderColor[3] = 1.0f;
    sampler_desc.MaxAnisotropy = 1;
    sampler_desc.MipLODBias = 0;
    sampler_desc.MinLOD = -FLT_MAX;
    sampler_desc.MaxLOD = +FLT_MAX;

    if (FAILED(m_Device->CreateSamplerState(&sampler_desc, &m_ShadowSamplerState)))
    {
        return false;
    }

    return true;
}

void DirectGraphics::SetUpViewPort()
{
    HWND window_handle = FindWindow(Window::ClassName, nullptr);
    RECT rect;
    GetClientRect(window_handle, &rect);

    D3D11_VIEWPORT view_port;
    // ��ʍ����X���W
    view_port.TopLeftX = 0;
    // ��ʍ����Y���W
    view_port.TopLeftY = 0;
    // ����
    view_port.Width = (rect.right - rect.left);
    // �c��
    view_port.Height = (rect.bottom - rect.top);
    // �[�x�l�̍ŏ��l
    view_port.MinDepth = 0.0f;
    // �[�x�l�̍ő�l
    view_port.MaxDepth = 1.0f;

    // ViewPort�̐ݒ�
    m_Context->RSSetViewports(1, &view_port);
}