﻿#ifndef DIRECTGRAPHICS_H_
#define DIRECTGRAPHICS_H_

#include <d3d11.h>
#include "VertexShader.h"
#include "PixelShader.h"
#include "../../Utility/GraphicsUtility.h"

/**
* @brief ラスタライズのモード
*/
enum class RasterizerMode
{
	MODE_CULL_BACK,	// 背面カリングモード
	MODE_CULL_NONE,	// カリングなしモード
	MODE_WIREFRAME,	// ワイヤーフレームモード

	MODE_NUM,		// モードの数
};

/**
* @brief DirectXのグラフィック関連を扱うクラス
*/
class DirectGraphics
{
public:
	/**
	* @brief コンストラクタ
	*/
	DirectGraphics() :
#pragma region MemberInit
		m_Device(nullptr),
		m_Context(nullptr),
		m_SwapChain(nullptr),
		m_RenderTargetView(nullptr),
		m_DepthStencilTexture(nullptr),
		m_DepthStencilView(nullptr),
		m_ConstantBuffer(nullptr),
		m_ConstantBufferData(),
		m_ConstBoneBuffer(nullptr),
		m_ConstBoneBufferData(),
		m_VertexShader(nullptr),
		m_PixelShader(nullptr),
		m_SimpleVertexShader(nullptr),
		m_FeatureLevel(D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0),
		m_SamplerState(nullptr),
		m_SampleDesc{ 0 },
		m_RasterizerState{ nullptr },
		m_DepthTextureView(nullptr),
		m_DepthRenderTargetView(nullptr),
		m_DepthDepthStencilView(nullptr),
		m_DepthDepthStencilTexture(nullptr),
		m_DepthTexture(nullptr),
		m_ShadowSamplerState(nullptr),
		m_DepthVertexShader(nullptr),
		m_DepthPixelShader(nullptr)
#pragma endregion
	{}

	/**
	* @brief デストラクタ
	*/
	~DirectGraphics()
	{}

	/**
	* @fn bool Init()
	* @brief 初期化関数
	* @details DirectX関連の初期化を行う
	*/
	bool Init();

	/**
	* @fn void Release()
	* @brief 解放関数
	* @details ゲーム終了時に行う
	*/
	void Release();
	
	/**
	* @fn void StartRendering()
	* @brief 描画開始関数
	* @details バックバッファの情報をクリアする
	*/
	void StartRendering();

	/**
	* @fn void FinishRendering()
	* @brief 描画終了関数
	* @details バックバッファの情報をフロントバッファに送る
	*/
	void FinishRendering();

public:
	/**
	* @fn ID3D11Device* GetDevice()
	* @brief Device取得関数
	* @return ID3D11Device* Deviceポインタ
	*/
	ID3D11Device* GetDevice() { return m_Device; }

	/**
	* @fn ID3D11DeviceContext* GetContext()
	* @brief Context取得関数
	* @return ID3D11DeviceContext* Contextのポインタ
	*/
	ID3D11DeviceContext* GetContext() { return m_Context; }

	/**
	* @fn VertexShader* GetVertexShader()
	* @brief スキンメッシュ用のVertexShader取得関数
	* @return VertexShader* VertexShaderのポインタ
	*/
	VertexShader* GetVertexShader()	{ return m_VertexShader; }

	/**
	* @fn PixelShader* GetPixelShader()
	* @brief PixelShader取得関数
	* @return PixelShader* PixelShaderのポインタ
	*/
	PixelShader* GetPixelShader() { return m_PixelShader; }

	/**
	* @fn VertexShader* GetSimpleVertexShader()
	* @brief スタティックメッシュ用のVertexShader取得関数
	* @return VertexShader* VertexShaderのポインタ
	*/
	VertexShader* GetSimpleVertexShader() { return m_SimpleVertexShader; }

	/**
	* @fn ID3D11Buffer* GetConstantBuffer()
	* @brief モデル毎に必要な汎用バッファの取得関数
	* @return ID3D11Buffer* ID3D11Bufferのポインタ
	*/
	ID3D11Buffer* GetConstantBuffer()  { return m_ConstantBuffer; }
	
	/**
	* @fn ConstantBuffer* GetConstantBufferData()
	* @brief モデル毎に必要な定数バッファ取得関数
	* @return ConstantBuffer* ConstantBufferのポインタ
	*/
	ConstantBuffer* GetConstantBufferData(){ return &m_ConstantBufferData; }

	/**
	* @fn ID3D11Buffer* GetPixelShader()
	* @brief 姿勢行列格納用の汎用バッファ取得関数
	* @return ID3D11Buffer* ID3D11Bufferのポインタ
	*/
	ID3D11Buffer* GetConstBoneBuffer() { return m_ConstBoneBuffer; }

	/**
	* @fn ConstBoneBuffer* GetConstBoneBufferData()
	* @brief 姿勢行列を格納する定数バッファ
	* @return ConstBoneBuffer* ConstBoneBufferのポインタ
	*/
	ConstBoneBuffer* GetConstBoneBufferData() { return &m_ConstBoneBufferData; }

	/**
	* @fn void SetRasterizerMode(RasterizerMode mode_)
	* @brief ラスタライズのモードを変更する関数
	* @param[in] mode_ ラスタライズモード
	*/
	void SetRasterizerMode(RasterizerMode mode_);

	/**
	* @fn void SetTexture(ID3D11ShaderResourceView* texture_)
	* @brief テクスチャをピクセルシェーダに送る関数
	* @param[in] texture_ テクスチャデータ
	* @details 設定しないときはnullptr
	*/
	void SetTexture(ID3D11ShaderResourceView* texture_);

	/**
	* @fn void SetMaterial(ObjMaterial* material_)
	* @brief マテリアルを定数バッファにセットする関数
	* @param[in] material_ マテリアルデータ
	* @details 設定しないときはnullptr
	*/
	void SetMaterial(ObjMaterial* material_);

private:
	/**
	* @fn void SetUpDxgiSwapChanDesc(DXGI_SWAP_CHAIN_DESC* dxgi)
	* @brief スワップチェインの設定を行う関数
	* @param[in] dxgi_ DXGI_SWAP_CHAIN_DESC構造体のポインタ
	*/
	void SetUpDxgiSwapChanDesc(DXGI_SWAP_CHAIN_DESC* dxgi_);

	/**
	* @fn bool CreateDeviceAndSwapChain()
	* @brief デバイスオブジェクトとスワップチェインオブジェクトの作成関数
	* @return bool 成功したかどうかを真偽で返す
	*/
	bool CreateDeviceAndSwapChain();

	/**
	* @fn bool CreateRenderTargetView()
	* @brief レンダーターゲットビューの作成関数
	* @return bool 成功したかどうかを真偽で返す
	*/
	bool CreateRenderTargetView();

	/**
	* @fn bool CreateDepthAndStencilView()
	* @brief 深度ステンシルビューの作成関数
	* @return bool 成功したかどうかを真偽で返す
	*/
	bool CreateDepthAndStencilView();

	/**
	* @fn bool CreateShader()
	* @brief シェーダーオブジェクト作成関数
	* @details 使用するシェーダーオブジェクトの作成を行う
	*/
	bool CreateShader();

	/**
	* @fn bool CreateShader()
	* @brief コンスタントバッファの作成関数
	* @details 使用するコンスタントバッファの作成を行う
	*/
	bool CreateConstantBuffer();

	/**
	* @fn bool CreateTextureSampler()
	* @brief テクスチャサンプラの作成関数
	* @details 使用するテクスチャサンプラの作成を行う
	*/
	bool CreateTextureSampler();

	/**
	* @fn bool CreateRasterizer()
	* @brief ラスタライザの作成関数
	* @details 使用するラスタライザの作成を行う
	*/
	bool CreateRasterizer();

	bool CreateDepthDSVAndRTV();

	/**
	* @fn void SetUpViewPort()
	* @brief ビューポートを設定する関数
	*/
	void SetUpViewPort();

	/**
	* @fn void SetUpLight()
	* @brief ライトを設定する関数
	*/
	void SetUpLight();

private:
	ID3D11Device*			m_Device;				//! デバイス
	ID3D11DeviceContext*	m_Context;				//! コンテキスト
	D3D_FEATURE_LEVEL 		m_FeatureLevel;			//! フューチャーレベル
	IDXGISwapChain*			m_SwapChain;			//! スワップチェイン
	ID3D11RenderTargetView* m_RenderTargetView;		//! レンダーターゲットビュー
	ID3D11Texture2D*		m_DepthStencilTexture;	//! 深度ステンシル用のテクスチャ
	ID3D11DepthStencilView* m_DepthStencilView;		//! 深度ステンシルビュー
	DXGI_SAMPLE_DESC		m_SampleDesc;			//! MSAA使用時に使うマルチサンプリングのパラメータ変数

	ID3D11RasterizerState* m_RasterizerState[static_cast<int>(RasterizerMode::MODE_NUM)];
	
	ID3D11SamplerState*		m_SamplerState;			//! モデルのテクスチャ用のテクスチャサンプラー

	ID3D11Buffer*			m_ConstantBuffer;		//! モデル用のコンストバッファ
	ConstantBuffer			m_ConstantBufferData;	//!	モデル用のコンストバッファ
	ID3D11Buffer*			m_ConstBoneBuffer;		//! ボーン用のコンストバッファ
	ConstBoneBuffer			m_ConstBoneBufferData;	//! ボーン用のコンストバッファ
	
	VertexShader*			m_VertexShader;			//! スキンメッシュ用の頂点シェーダ
	VertexShader*			m_SimpleVertexShader;	//!	スタティックメッシュ用の頂点シェーダ
	PixelShader*			m_PixelShader;			//! 通常描画用のピクセルシェーダ

	/* シャドウマップ用 */
	ID3D11ShaderResourceView* m_DepthTextureView;			//! 
	ID3D11RenderTargetView*   m_DepthRenderTargetView;
	ID3D11DepthStencilView*   m_DepthDepthStencilView;
	ID3D11Texture2D*		  m_DepthDepthStencilTexture;
	ID3D11Texture2D*		  m_DepthTexture;				//! 
	ID3D11SamplerState*		  m_ShadowSamplerState;			//! シャドウマップ用のテクスチャサンプラー
	VertexShader*			  m_DepthVertexShader;			//! 頂点シェーダ
	PixelShader*			  m_DepthPixelShader;			//! ピクセルシェーダ

};

#endif