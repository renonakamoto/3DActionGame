#ifndef DIRECTGRAPHICS_H_
#define DIRECTGRAPHICS_H_

#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include "VertexShader.h"
#include "PixelShader.h"
#include "../../Utility/GraphicsUtility.h"
#include "RenderTarget.h"

using namespace Microsoft::WRL;

/**
* @brief ラスタライズのモード
*/
enum class RasterizerMode
{
	MODE_CULL_BACK,	 //! 背面カリングモード
	MODE_CULL_FRONT, //! 正面カリングモード
	MODE_CULL_NONE,	 //! カリングなしモード
	MODE_WIREFRAME,	 //! ワイヤーフレームモード

	MODE_NUM,		 //! モードの数
};

/**
* @brief レンダーターゲットの種類
*/
enum class KindRT
{
	RT_ON_SCREEN,	//! オンスクリーン
	RT_OFF_SCREEM,	//! オフスクリーン
	RT_SHADOWMAP,	//! シャドウマップ

	RT_NUM,			//! 種類の数
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
		m_ConstantBuffer(nullptr),
		m_ConstantBufferData(),
		m_ConstBoneBuffer(nullptr),
		m_ConstBoneBufferData(),
		m_VertexShader(nullptr),
		m_PixelShader(nullptr),
		m_SimpleVertexShader(nullptr),
		m_FeatureLevel(D3D_FEATURE_LEVEL_9_1),
		m_SamplerState(nullptr),
		m_SampleDesc{ 0 },
		m_RasterizerState{ nullptr },
		m_ShadowSamplerState(nullptr),
		m_DepthVertexShader(nullptr),
		m_DepthPixelShader(nullptr),
		m_DepthSkinningVertexShader(nullptr),
		m_NormalMapPS(nullptr),
		DepthViewSize(50.f)
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
	* @fn void StartRendering(KindRT rt_)
	* @brief 描画開始関数
	* @param[in] rt_ レンダリングターゲットの種類
	*/
	void StartRendering(KindRT rt_);

	/**
	* @fn void FinishRendering()
	* @brief 描画終了関数
	* @details バックバッファの情報をフロントバッファに送る
	*/
	void FinishRendering();
	
	/**
	* @fn void UpdateLight()
	* @brief ライトの更新関数
	*/
	void UpdateLight();

public:
	/**
	* @fn ID3D11Device* GetDevice()
	* @brief Device取得関数
	* @return ID3D11Device* Deviceポインタ
	*/
	ID3D11Device* GetDevice() { return m_Device.Get(); }

	/**
	* @fn ID3D11DeviceContext* GetContext()
	* @brief Context取得関数
	* @return ID3D11DeviceContext* Contextのポインタ
	*/
	ID3D11DeviceContext* GetContext() { return m_Context.Get(); }

	/**
	* @fn void SetRasterizerMode(RasterizerMode mode_)
	* @brief ラスタライズのモードを変更する関数
	* @param[in] mode_ ラスタライズモード
	*/
	void SetRasterizerMode(RasterizerMode mode_);

public:

	/**
	* @fn VertexShader* GetVertexShader()
	* @brief スキンメッシュ用のVertexShader取得関数
	* @return VertexShader* VertexShaderのポインタ
	*/
	VertexShader* GetVertexShader()	{ return m_VertexShader.get(); }

	/**
	* @fn PixelShader* GetPixelShader()
	* @brief PixelShader取得関数
	* @return PixelShader* PixelShaderのポインタ
	*/
	PixelShader* GetPixelShader() { return m_PixelShader.get(); }

	/**
	* @fn VertexShader* GetSimpleVertexShader()
	* @brief スタティックメッシュ用のVertexShader取得関数
	* @return VertexShader* VertexShaderのポインタ
	*/
	VertexShader* GetSimpleVertexShader() { return m_SimpleVertexShader.get(); }

	/**
	* @fn VertexShader* GetDepthVertexShader()
	* @brief シャドウマップ用のVertexShader取得関数
	* @return VertexShader* VertexShaderのポインタ
	*/
	VertexShader* GetDepthVertexShader() { return m_DepthVertexShader.get(); }

	/**
	* @fn PixelShader* GetDepthPixelShader()
	* @brief シャドウマップ用のPixelShader取得関数
	* @return PixelShader* PixelShaderのポインタ
	*/
	PixelShader* GetDepthPixelShader() { return m_DepthPixelShader.get(); }

	/**
	* @fn VertexShader* GetDepthSkinningVertexShader()
	* @brief スキニングするシャドウマップ用のVertexShader取得関数
	* @return VertexShader* VertexShaderのポインタ
	*/
	VertexShader* GetDepthSkinningVertexShader() { return m_DepthSkinningVertexShader.get(); }

	/**
	* @fn PixelShader* GetNormalMapPS()
	* @brief ノーマルマップ用のPixelShader取得関数
	* @return PixelShader* PixelShaderのポインタ
	*/
	PixelShader* GetNormalMapPS() { return m_NormalMapPS.get(); }

	/**
	* @fn PixelShader* GetNormalMapPS()
	* @brief ノーマルマップ用のPixelShader取得関数
	* @return PixelShader* PixelShaderのポインタ
	*/
	PixelShader* GetGroundPS() { return m_GroundPS.get(); }

	/**
	* @fn VertexShader* GetSpriteVertexShader()
	* @brief VertexShader取得関数
	* @return VertexShader* VertexShaderのポインタ
	*/
	VertexShader* GetSpriteVertexShader() { return m_SpriteVertexShader.get(); }

	/**
	* @fn PixelShader* GetBlurPixelShader()
	* @brief PixelShader取得関数
	* @return PixelShader* PixelShaderのポインタ
	*/
	PixelShader* GetBlurPixelShader() { return m_BlurPixelShader.get(); }

public:

	/**
	* @fn ID3D11Buffer* GetConstantBuffer()
	* @brief モデル毎に必要な汎用バッファの取得関数
	* @return ID3D11Buffer* ID3D11Bufferのポインタ
	*/
	ID3D11Buffer* GetConstantBuffer()  { return m_ConstantBuffer.Get(); }
	
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
	ID3D11Buffer* GetConstBoneBuffer() { return m_ConstBoneBuffer.Get(); }

	/**
	* @fn ConstBoneBuffer* GetConstBoneBufferData()
	* @brief 姿勢行列を格納する定数バッファ
	* @return ConstBoneBuffer* ConstBoneBufferのポインタ
	*/
	ConstBoneBuffer* GetConstBoneBufferData() { return &m_ConstBoneBufferData; }

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

	/**
	* @fn ID3D11SamplerState* GetSSamplerState()
	* @brief サンプラーステート取得関数
	* @return ID3D11SamplerState* m_SamplerStateのポインタ
	*/
	ID3D11SamplerState* GetSSamplerState() { return m_SamplerState.Get(); }

	/**
	* @fn ID3D11ShaderResourceView* GetDepthTextureView()
	* @brief シャドウマップ用テクスチャデータ取得関数
	* @return ID3D11ShaderResourceView* m_DepthTextureViewのポインタ
	*/
	ID3D11ShaderResourceView* GetDepthTextureView() { return GetRenderTarget(KindRT::RT_SHADOWMAP)->GetTexture(); }

	/**
	* @fn ID3D11SamplerState* GetShadowMapSamplerState()
	* @brief シャドウマップ用サンプラーステート取得関数
	* @return ID3D11SamplerState* m_ShadowSamplerStateのポインタ
	*/
	ID3D11SamplerState* GetShadowMapSamplerState() { return m_ShadowSamplerState.Get(); }

	/**
	* @fn RenderTarget* GetRenderTarget(KindRT rt_)
	* @brief レンダーターゲットオブジェクトの取得関数
	* @param[in] rt_ レンダーターゲットの種類
	* @return RenderTarget* GetRenderTargetのポインタ
	*/
	RenderTarget* GetRenderTarget(KindRT rt_) { return &m_RenderTargets[static_cast<int>(rt_)]; }

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
	* @fn bool CreateShader()
	* @brief シェーダーオブジェクト作成関数
	* @details 使用するシェーダーオブジェクトの作成を行う
	* @return bool 成功したかどうかを真偽で返す
	*/
	bool CreateShader();

	/**
	* @fn bool CreateShader()
	* @brief コンスタントバッファの作成関数
	* @details 使用するコンスタントバッファの作成を行う
	* @return bool 成功したかどうかを真偽で返す
	*/
	bool CreateConstantBuffer();

	/**
	* @fn bool CreateTextureSampler()
	* @brief テクスチャサンプラの作成関数
	* @details 使用するテクスチャサンプラの作成を行う
	* @return bool 成功したかどうかを真偽で返す
	*/
	bool CreateTextureSampler();

	/**
	* @fn bool CreateRasterizer()
	* @brief ラスタライザの作成関数
	* @details 使用するラスタライザの作成を行う
	* @return bool 成功したかどうかを真偽で返す
	*/
	bool CreateRasterizer();

	/**
	* @fn void SetUpLight()
	* @brief ライトを設定する関数
	*/
	void SetUpLight();
	
private:
	ComPtr<ID3D11Device>			m_Device;				//! デバイス
	ComPtr<ID3D11DeviceContext>		m_Context;				//! コンテキスト
	ComPtr<IDXGISwapChain>			m_SwapChain;			//! スワップチェイン
	D3D_FEATURE_LEVEL 				m_FeatureLevel;			//! フューチャーレベル
	DXGI_SAMPLE_DESC				m_SampleDesc;			//! MSAA使用時に使うマルチサンプリングのパラメータ変数
	ComPtr<ID3D11SamplerState>		m_SamplerState;			//! 通常テクスチャ用のテクスチャサンプラー

	ComPtr<ID3D11Buffer>			m_ConstantBuffer;		//! モデル用のコンストバッファ(シェーダに送る用)
	ConstantBuffer					m_ConstantBufferData;	//!	モデル用のコンストバッファ
	ComPtr<ID3D11Buffer>			m_ConstBoneBuffer;		//! ボーン用のコンストバッファ(シェーダに送る用)
	ConstBoneBuffer					m_ConstBoneBufferData;	//! ボーン用のコンストバッファ

	std::unique_ptr<VertexShader>   m_VertexShader;			//! スキンメッシュ用の頂点シェーダ
	std::unique_ptr<VertexShader>   m_SimpleVertexShader;	//!	スタティックメッシュ用の頂点シェーダ
	std::unique_ptr<PixelShader>	m_PixelShader;			//! 通常描画用のピクセルシェーダ
	
	RenderTarget m_RenderTargets[static_cast<int>(KindRT::RT_NUM)];									//! レンダーターゲット
	ComPtr<ID3D11RasterizerState>   m_RasterizerState[static_cast<int>(RasterizerMode::MODE_NUM)];	//! ラスタライザ配列

	std::unique_ptr<VertexShader>   m_SpriteVertexShader;		    //!	スプライト用頂点シェーダー
	std::unique_ptr<PixelShader>	m_BlurPixelShader;			    //! ぼかしシェーダー用ピクセルシェーダー
	
	ComPtr<ID3D11SamplerState>		 m_ShadowSamplerState;			//! シャドウマップ用のテクスチャサンプラー
	
	std::unique_ptr<VertexShader>    m_DepthSkinningVertexShader;	//! シャドウマップ用スキニング頂点シェーダ
	std::unique_ptr<VertexShader>	 m_DepthVertexShader;			//! シャドウマップ用通常頂点シェーダ
	std::unique_ptr<PixelShader>     m_DepthPixelShader;			//! シャドウマップ用ピクセルシェーダ
	std::unique_ptr<PixelShader>	 m_NormalMapPS;					//! 法線マップ用ピクセルシェーダ
	std::unique_ptr<PixelShader>	 m_GroundPS;					//! 地面用のシェーダー

	// ライト
	DirectX::XMFLOAT3 m_LightPos;	//! ライトの座標
	float DepthViewSize;			//! シャドウマップの適応範囲のサイズ

};

#endif