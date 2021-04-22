﻿#include <stdio.h>

#include "DirectGraphics.h"
#include "../Engine.h"

#pragma comment(lib,"d3d11.lib")

// MSAAの有効
//#define ENABLE_MSAA

bool DirectGraphics::Init()
{
    // デバイスとスワップチェインの作成 
    if (CreateDeviceAndSwapChain() == false)
    {
        return true;
    }

    // レンダーターゲットビューの作成
    if (CreateRenderTargetView() == false)
    {
        return false;
    }

    // デプスビューとステンシルビューの作成
    if (CreateDepthAndStencilView() == false)
    {
        return false;
    }
    
    // シェーダーの作成
    if (CreateShader() == false)
    {
        return false;
    }

    // コンスタントバッファの作成
    if (CreateConstantBuffer() == false)
    {
        return false;
    }

    // テクスチャサンプラの作成
    if (CreateTextureSampler() == false)
    {
        return false;
    }

    // ラスタライザの作成
    if (CreateRasterizer() == false)
    {
        return false;
    }

    // シャドウマップ用の
    if (CreateDepthDSVAndRTV() == false)
    {
        return false;
    }

    // ビューポート設定
    SetUpViewPort();

    // ライト設定
    SetUpLight();
    
    // UV変換行列の設定
    DirectX::XMMATRIX tex_uv = DirectX::XMMatrixSet(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f,-0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    DirectX::XMStoreFloat4x4(&m_ConstantBufferData.ClipUV, DirectX::XMMatrixTranspose(tex_uv));

    
    return true;
}

void DirectGraphics::Release()
{
    if (m_Device != nullptr)
    {
        m_Device->Release();
        m_Device = nullptr;
    }

    if (m_Context != nullptr)
    {
        m_Context->ClearState();
        m_Context->Release();
        m_Context = nullptr;
    }

    if (m_SwapChain != nullptr)
    {
        m_SwapChain->Release();
        m_SwapChain = nullptr;
    }

    if (m_RenderTargetView != nullptr)
    {
        m_RenderTargetView->Release();
        m_RenderTargetView = nullptr;
    }

    if (m_DepthStencilTexture != nullptr)
    {
        m_DepthStencilTexture->Release();
        m_DepthStencilTexture = nullptr;
    }

    if (m_DepthStencilView != nullptr)
    {
        m_DepthStencilView->Release();
        m_DepthStencilView = nullptr;
    }

    if (m_SamplerState != nullptr)
    {
        m_SamplerState->Release();
        m_SamplerState = nullptr;
    }

    if (m_ConstantBuffer != nullptr)
    {
        m_ConstantBuffer->Release();
        m_ConstantBuffer = nullptr;
    }

    if (m_ConstBoneBuffer != nullptr)
    {
        m_ConstBoneBuffer->Release();
        m_ConstBoneBuffer = nullptr;
    }

    if (m_ShadowSamplerState != nullptr)
    {
        m_ShadowSamplerState->Release();
        m_ShadowSamplerState = nullptr;
    }

    for (int i = 0; i < static_cast<int>(RasterizerMode::MODE_NUM); ++i)
    {
        if (m_RasterizerState[i] == nullptr) continue;
        m_RasterizerState[i]->Release();
        m_RasterizerState[i] = nullptr;
    }

    if (m_DepthTextureView != nullptr)
    {
        m_DepthTextureView->Release();
        m_DepthTextureView = nullptr;
    }

    if (m_DepthRenderTargetView != nullptr)
    {
        m_DepthRenderTargetView->Release();
        m_DepthRenderTargetView = nullptr;
    }

    if (m_DepthDepthStencilView != nullptr)
    {
        m_DepthDepthStencilView->Release();
        m_DepthDepthStencilView = nullptr;
    }

    if (m_DepthDepthStencilTexture != nullptr)
    {
        m_DepthDepthStencilTexture->Release();
        m_DepthDepthStencilTexture = nullptr;
    }

    if (m_DepthTexture != nullptr)
    {
        m_DepthTexture->Release();
        m_DepthTexture = nullptr;
    }

    if (m_ShadowSamplerState != nullptr)
    {
        m_ShadowSamplerState->Release();
        m_ShadowSamplerState = nullptr;
    }
}

/*
    描画開始関数
*/
void DirectGraphics::StartRendering()
{
    /*
        DirectX11は描画の開始を宣言する必要はないがビューのクリアを毎フレーム
        行わないと描画内容がおかしいことになる
    */

    float clear_color[4] = { 0.0f,0.0f,1.0f,1.0f };

    // レンダーターゲットビューのクリアw
    m_Context->ClearRenderTargetView(
                m_RenderTargetView, // 対象のレンダーターゲットビュー
                clear_color         // クリアするビューのカラー
                );

    // デプスステンシルビューのクリア
    m_Context->ClearDepthStencilView(
                m_DepthStencilView,                      // 対象のビュー
                D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, // クリアフラグ
                1.0f,                                    // 深度クリア値
                0U                                       // ステンシルクリア値
                );

    /*
        ビューポートの設定
    */
    //SetUpViewPort();

    /*
       出力先の設定
    */
    m_Context->OMSetRenderTargets(1U, &m_RenderTargetView, m_DepthStencilView);

}

/*
    描画終了関数
    バックバッファに書き込んだ情報をフロントバッファに渡す
*/
void DirectGraphics::FinishRendering()
{
    // バックバッファをフロントバッファに送信する   
    m_SwapChain->Present(1, 0);
}

void DirectGraphics::SetRasterizerMode(RasterizerMode mode_)
{
    m_Context->RSSetState(m_RasterizerState[static_cast<int>(mode_)]);
}

void DirectGraphics::SetTexture(ID3D11ShaderResourceView* texture_)
{
    m_Context->PSSetSamplers(
        0U,
        1U,
        &m_SamplerState);

    m_Context->PSSetShaderResources(
        0U,
        1U,
        &texture_);
    
}

void DirectGraphics::SetMaterial(ObjMaterial* material_)
{
    // nullptrなら黒にする
    if (!material_) {
        m_ConstantBufferData.MaterialAmbient  = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        m_ConstantBufferData.MaterialDiffuse  = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        m_ConstantBufferData.MaterialSpecular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    // アンビエント
    m_ConstantBufferData.MaterialAmbient  = DirectX::XMFLOAT4(material_->Ambient[0], material_->Ambient[1],
                                                              material_->Ambient[2], material_->Ambient[3]);
    // ディヒューズ
    m_ConstantBufferData.MaterialDiffuse  = DirectX::XMFLOAT4(material_->Diffuse[0],  material_->Diffuse[1],
                                                              material_->Diffuse[2],  material_->Diffuse[3]);
    // スペキュラ
    m_ConstantBufferData.MaterialSpecular = DirectX::XMFLOAT4(material_->Specular[0], material_->Specular[1], 
                                                              material_->Specular[2], material_->Specular[3]);
}

void DirectGraphics::SetUpDxgiSwapChanDesc(DXGI_SWAP_CHAIN_DESC* dxgi_)
{
    // スワップチェイン作成時に必要な設定
    ZeroMemory(dxgi_, sizeof(DXGI_SWAP_CHAIN_DESC));
    
    // バッファの数
    dxgi_->BufferCount = 1U;
    // バッファの横幅
    dxgi_->BufferDesc.Width  = static_cast<UINT>(WINDOW->GetClientWidth());
    // バッファの縦幅
    dxgi_->BufferDesc.Height = static_cast<UINT>(WINDOW->GetClientHeight());
    // バッファのカラーフォーマット
    dxgi_->BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // リフレッシュレートの分子
    dxgi_->BufferDesc.RefreshRate.Numerator   = 60U;
    // リフレッシュレートの分母
    dxgi_->BufferDesc.RefreshRate.Denominator = 1U;
    // スキャンラインの方法
    // バックバッファをフリップしたときにハードウェアがパソコンのモニターに
    // 点をどう描くかを設定する
    // 特に理由がなければデフォルト値であるDXGI_MODE_SCANLINE_ORDER_UNSPECIFIEDでOK
    dxgi_->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    // ウィンドウのサイズに応じてスケーリングするかどうかの設定
    // スケーリングする場合   DXGI_MODE_SCALING_STRETCHED 
    // スケーリングしない場合 DXGI_MODE_SCALING_CENTERED
    dxgi_->BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    // バッファの使用方法
    // レンダーターゲットとして使用する場合は、DXGI_USAGE_RENDER_TARGET_OUTPUT 
    // シェーダー入力用として使用する場合はDXGI_USAGE_SHADER_INPUT
    dxgi_->BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // 出力対象のウィンドウハンドル
    dxgi_->OutputWindow = WINDOW->GetWindowHandle();
    // マルチサンプリングのサンプル数(未使用は1)
    dxgi_->SampleDesc.Count = 1U;
    // マルチサンプリングの品質(未使用は0)
    dxgi_->SampleDesc.Quality = 0U;
    // ウィンドウモード指定
    dxgi_->Windowed = true;
    // スワップチェインの動作オプション
    // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCHを指定した場合
    // フルスクリーンとウィンドウモードの切り替えが可能
    dxgi_->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
}

bool DirectGraphics::CreateDeviceAndSwapChain()
{
#ifdef ENABLE_MSAA
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


    // 使用できるサンプルレベルを調べる
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

    // インターフェースを取得
    IDXGIDevice1* dxgi = nullptr;
    if (FAILED(m_Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgi)))
    {
        return false;
    }

    // アダプターを取得
    IDXGIAdapter* adapter = nullptr;
    if (FAILED(dxgi->GetAdapter(&adapter)))
    {
        return false;
    }

    // ファクトリーを取得
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

    // スワップチェイン作成時に必要な設定
    ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

    // バッファの数
    swap_chain_desc.BufferCount = 1;
    // バッファの横幅
    swap_chain_desc.BufferDesc.Width = static_cast<UINT>(rect.right - rect.left);
    // バッファの縦幅
    swap_chain_desc.BufferDesc.Height = static_cast<UINT>(rect.bottom - rect.top);
    // バッファのカラーフォーマット
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // リフレッシュレートの分子
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    // リフレッシュレートの分母
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    // スキャンラインの方法
    // バックバッファをフリップしたときにハードウェアがパソコンのモニターに
    // 点をどう描くかを設定する
    // 特に理由がなければデフォルト値であるDXGI_MODE_SCANLINE_ORDER_UNSPECIFIEDでOK
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    // ウィンドウのサイズに応じてスケーリングするかどうかの設定
    // スケーリングする場合   DXGI_MODE_SCALING_STRETCHED 
    // スケーリングしない場合 DXGI_MODE_SCALING_CENTERED
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // バッファの使用方法
    // レンダーターゲットとして使用する場合は、DXGI_USAGE_RENDER_TARGET_OUTPUT 
    // シェーダー入力用として使用する場合はDXGI_USAGE_SHADER_INPUT
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // 出力対象のウィンドウハンドル
    swap_chain_desc.OutputWindow = window_handle;
    swap_chain_desc.SampleDesc = m_SampleDesc;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    // ウィンドウモード指定
    swap_chain_desc.Windowed = true;
    // スワップチェインの動作オプション
    // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCHを指定した場合
    // フルスクリーンとウィンドウモードの切り替えが可能
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if (FAILED(factory->CreateSwapChain(m_Device, &swap_chain_desc, &m_SwapChain)))
    {
        return false;
    }
#else
DXGI_SWAP_CHAIN_DESC swap_chain_desc;
SetUpDxgiSwapChanDesc(&swap_chain_desc);

// 適応されたFutureレベルも取得できる
if (FAILED(D3D11CreateDeviceAndSwapChain(
    // デバイス作成時に使用するビデオアダプタの指定
    // nullptrで既定のアダプタを使用する
    nullptr,
    // ドライバのタイプ
    // D3D_DRIVER_TYPEのいずれかを指定
    // 通常はD3D_DRIVER_TYPE_HARDWAREで問題ない
    D3D_DRIVER_TYPE_HARDWARE,
    // D3D_DRIVER_TYPE_SOFTWARE指定時に使用
    nullptr,
    // ランタイムレイヤーのフラグ指定
    0,
    // D3D_FEATURE_LEVEL指定で自分で定義した配列を指定可能
    nullptr,
    // 上のD3D_FEATURE_LEVEL配列の要素数
    0,
    // SDKバージョン
    D3D11_SDK_VERSION,
    // 設定済みのDXGI_SWAP_CHAIN_DESC
    &swap_chain_desc,
    // 初期化が完了したSwapChainの出力先 
    &m_SwapChain,
    // 初期化が完了したDeviceの出力先 
    &m_Device,
    // 最終的に決定したFutureレベルの出力先
    &m_FeatureLevel,
    // 作成されたコンテキストを受け取るためのID3D11DeviceContextポインタアドレス
    &m_Context)))
{
    return false;
}

#endif


    /*
        D3D11CreateDeviceAndSwapChainを使用するとレンダリング用のContextが作成される
        Contextは描画コマンドの追加、送信などの処理を行う
        CPU側で追加された描画コマンドをGPU側に送信するのが主な役目
        Contextnには「Immediate」と「Deferred」の2種類あり
        Immediateは生成したコマンドを即時実行する
        Deferredは生成した描画コマンドをバッファにためておき、
        実行命令を受けたらたまっているコマンドを実行する
        マルチスレッドによる描画処理に有効とされている

        D3D11CreateDeviceAndSwapChainで作成した場合、Immediateで作成される
    */

    
    return true;
}

bool DirectGraphics::CreateRenderTargetView()
{
    /*
        RenderTargetViewはレンダリングパイプラインからアウトプットされる
        ピクセル情報を保存するViewのことで、このViewの内容をゲーム画面に反映させる。
    */

    // RenderViewはSwapChainがもつバッファを使用して作成するので
    // GetBufferを使用してバッファを取得する
    ID3D11Texture2D* back_buffer;
    if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer)))
    {
        return false;
    }
    
    // RenderTargetViewの作成
    if (FAILED(m_Device->CreateRenderTargetView(back_buffer, nullptr, &m_RenderTargetView)))
    {
        return false;
    }

    // これ以降バッファを使用しないので、解放しておく
    back_buffer->Release();
    
    return true;
}

bool DirectGraphics::CreateDepthAndStencilView()
{
    /*
        CreateTexture2Dで作成されたテクスチャをバッファとして使用するので
        D3D11_TEXTURE2D_DESCでテクスチャの設定を行う
    */
    
    // 深度ステンシルバッファの作成
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    // バッファの横幅
    texture_desc.Width  = static_cast<UINT>(WINDOW->GetClientWidth());
    // バッファの横幅
    texture_desc.Height = static_cast<UINT>(WINDOW->GetClientHeight());
    // ミップマップレベル
    texture_desc.MipLevels = 1U;
    // テクスチャ配列のサイズ
    texture_desc.ArraySize = 1U;
    // テクスチャのフォーマット
    // DXGI_FORMAT_D24_UNORM_S8_UINT
    // Depth24bit, Stencil8bitとなる
    texture_desc.Format = DXGI_FORMAT_D32_FLOAT;
    // 
#ifdef ENABLE_MSAA
    texture_desc.SampleDesc = m_SampleDesc;
#else
    texture_desc.SampleDesc.Count   = 1U;
    texture_desc.SampleDesc.Quality = 0U;
#endif
    // テクスチャの使用方法
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    // Bind設定
    // D3D11_BIND_DEPTH_STENCILで問題ない
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    // リソースへのCPUのアクセス権限
    // 0で問題ない
    texture_desc.CPUAccessFlags = 0U;
    // リソースオプションのフラグ
    // 必要なければ0
    texture_desc.MiscFlags = 0U;
    
    // テクスチャの作成
    if (FAILED(m_Device->CreateTexture2D(&texture_desc, NULL, &m_DepthStencilTexture)))
    {
        return false;
    }

    // DepthStencilViewの設定を行う
    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    // Viewのフォーマット
    dsv_desc.Format = texture_desc.Format;
    // テクスチャの種類
#ifdef ENABLE_MSAA
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
#else
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
#endif
    dsv_desc.Texture2D.MipSlice = 0U;
    
    // DepthStencilViewの作成
    if (FAILED(m_Device->CreateDepthStencilView(
        m_DepthStencilTexture,
        &dsv_desc,
        &m_DepthStencilView)))
    {
        return false;
    }

    m_Context->OMSetRenderTargets(1U, &m_RenderTargetView, m_DepthStencilView);

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

    m_DepthVertexShader = new VertexShader();
    if (m_DepthVertexShader->Create(m_Device, "Res/Shader/DepthVertexShader.cso") == false)
    {
        return false;
    }

    m_DepthPixelShader = new PixelShader();
    if (m_DepthPixelShader->Create(m_Device, "Res/Shader/DepthPixelShader.cso") == false)
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
    buffer_desc.CPUAccessFlags      = 0U;
    buffer_desc.MiscFlags           = 0U;
    buffer_desc.StructureByteStride = 0U;

    if (FAILED(m_Device->CreateBuffer(&buffer_desc, nullptr, &m_ConstantBuffer)))
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

    sampler_desc.Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_desc.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_desc.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampler_desc.BorderColor[0] = 1.f;
    sampler_desc.BorderColor[1] = 1.f;
    sampler_desc.BorderColor[2] = 1.f;
    sampler_desc.BorderColor[3] = 1.f;
    sampler_desc.MaxAnisotropy  = 1U;
    sampler_desc.MipLODBias     = 0.f;
    sampler_desc.MinLOD         = -FLT_MAX;
    sampler_desc.MaxLOD         = +FLT_MAX;

    if (FAILED(m_Device->CreateSamplerState(&sampler_desc, &m_ShadowSamplerState)))
    {
        return false;
    }

    return true;
}

bool DirectGraphics::CreateRasterizer()
{
    /*
        カリングなし
    */
    // ラスタライザ
    D3D11_RASTERIZER_DESC ras_desc;
    ID3D11RasterizerState* RasBackCulling;
    ZeroMemory(&ras_desc, sizeof(ras_desc));
    ras_desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;  //! レンダリングするポリゴンを塗りつぶす
    ras_desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;   //! カリングなし
    ras_desc.FrontCounterClockwise = FALSE;                 //! 時計周りが表、反時計回りが裏
    if (FAILED(m_Device->CreateRasterizerState(&ras_desc, &m_RasterizerState[static_cast<int>(RasterizerMode::MODE_CULL_NONE)])))
    {
        return false;
    }

    /*
        背面カリング
    */
    ras_desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;   //! 背面カリング
    if (FAILED(m_Device->CreateRasterizerState(&ras_desc, &m_RasterizerState[static_cast<int>(RasterizerMode::MODE_CULL_BACK)])))
    {
        return false;
    }

    /*
        ワイヤーフレーム
    */
    ras_desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;  //! ポリゴンを塗りつぶさない
    if (FAILED(m_Device->CreateRasterizerState(&ras_desc, &m_RasterizerState[static_cast<int>(RasterizerMode::MODE_WIREFRAME)])))
    {
        return false;
    }

    // デフォルトは背面カリング
    SetRasterizerMode(RasterizerMode::MODE_CULL_BACK);
    return true;
}

bool DirectGraphics::CreateDepthDSVAndRTV()
{
    // 深度ステンシルビューの作成
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width              = static_cast<UINT>(WINDOW->GetClientWidth() * 2);
    texture_desc.Height             = static_cast<UINT>(WINDOW->GetClientHeight() * 2);
    texture_desc.MipLevels          = 1U;
    texture_desc.ArraySize          = 1U;
    texture_desc.MiscFlags          = 0U;
    texture_desc.Format             = DXGI_FORMAT_R32_FLOAT;
    texture_desc.SampleDesc.Count   = 1U;
    texture_desc.SampleDesc.Quality = 0U;
    texture_desc.Usage              = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags     = 0U;

    if (FAILED(m_Device->CreateTexture2D(&texture_desc, nullptr, &m_DepthTexture)))
    {
        return false;
    }

    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format             = texture_desc.Format;
    rtv_desc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0U;

    if (FAILED(m_Device->CreateRenderTargetView(m_DepthTexture, &rtv_desc, &m_DepthRenderTargetView)))
    {
        return false;
    }
    
    texture_desc.Format    = DXGI_FORMAT_D32_FLOAT;
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    if (FAILED(m_Device->CreateTexture2D(&texture_desc, nullptr, &m_DepthDepthStencilTexture)))
    {
        return false;
    }
    if (FAILED(m_Device->CreateDepthStencilView(m_DepthDepthStencilTexture, nullptr, &m_DepthDepthStencilView)))
    {
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format              = DXGI_FORMAT_R32_FLOAT;
    srv_desc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1U;

    if (FAILED(m_Device->CreateShaderResourceView(m_DepthTexture, &srv_desc, &m_DepthTextureView)))
    {
        return false;
    }

    return true;
}

void DirectGraphics::SetUpViewPort()
{
    D3D11_VIEWPORT view_port;
    // 画面左上のX座標
    view_port.TopLeftX = 0.f;
    // 画面左上のY座標
    view_port.TopLeftY = 0.f;
    // 横幅
    view_port.Width  = static_cast<FLOAT>(WINDOW->GetClientWidth());
    // 縦幅
    view_port.Height = static_cast<FLOAT>(WINDOW->GetClientHeight());
    // 深度値の最小値
    view_port.MinDepth = 0.f;
    // 深度値の最大値
    view_port.MaxDepth = 1.f;

    // ViewPortの設定
    m_Context->RSSetViewports(1U, &view_port);
}

void DirectGraphics::SetUpLight()
{
    DirectX::XMStoreFloat4(&m_ConstantBufferData.Light, DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.0f, 0.5f, -1.0f, 0.0f)));

    DirectX::XMMATRIX light_view = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(m_ConstantBufferData.Light.x, m_ConstantBufferData.Light.y, m_ConstantBufferData.Light.z, 0.0f),
        DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    DirectX::XMStoreFloat4x4(&m_ConstantBufferData.LightView, DirectX::XMMatrixTranspose(light_view));
}
