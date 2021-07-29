#ifndef RENDER_TARGET_H_
#define RENDER_TARGET_H_

#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class RenderTarget
{
public:
	bool Create(int width_, int height_, DXGI_FORMAT rtFormat_, DXGI_FORMAT dsFormat_, float clearColor_[4]);
	bool Create(int width_, int height_, ID3D11Texture2D* resouce_, DXGI_FORMAT dsFormat_, float clearColor_[4]);

	int GetWidth()  { return m_Width;  }
	int GetHeight() { return m_Height; }
	float* GetColor() { return m_ClearColor; }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_RenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView.Get(); }
	ID3D11ShaderResourceView* GetTexture() { return m_Texture.Get(); }

private:
	int   m_Width;
	int   m_Height;
	float m_ClearColor[4];

	ComPtr<ID3D11RenderTargetView>   m_RenderTargetView; //! �I�t�X�N���[���p�����_�[�^�[�Q�b�g�r���[
	ComPtr<ID3D11Texture2D>		     m_RTTexture;		 //! �I�t�X�N���[���p�����_�[�^�[�Q�b�g�e�N�X�`��
	ComPtr<ID3D11DepthStencilView>   m_DepthStencilView; //! �I�t�X�N���[���p�[�x�X�e���V���r��
	ComPtr<ID3D11Texture2D>			 m_DSTexture;		 //! �I�t�X�N���[���p�[�x�X�e���V���e�N�X�`��
	
	ComPtr<ID3D11ShaderResourceView> m_Texture;			 //! �V�F�[�_�[�ɑ���ׂ̃I�t�X�N���[���e�N�X�`��
};

#endif