#ifndef SHADOW_MAP_OBJECT_H_
#define SHADOW_MAP_OBJECT_H_

#include "Object3D.h"

/**
* @brief �V���h�E�}�b�v���s���I�u�W�F�N�g�̊��N���X
*/
class ShadowMapObject : public Object3D
{
public:
	/**
	* @brief �R���X�g���N�^
	*/
	ShadowMapObject()
	{}

	/**
	* @brief �R���X�g���N�^
	*/
	ShadowMapObject(DirectX::XMFLOAT3 pos_):
		Object3D(pos_)
	{}

	/**
	* @brief �R���X�g���N�^
	*/
	ShadowMapObject(DirectX::XMFLOAT3 pos_, DirectX::XMFLOAT3 rot_, DirectX::XMFLOAT3 scale_):
		Object3D(pos_, rot_, scale_)
	{}

	/**
	* @brief �f�X�g���N�^
	*/
	virtual ~ShadowMapObject()
	{}

	/**
	* @fn void Init()
	* @brief �������֐�
	* @details �p����Œ�`���A�I�u�W�F�N�g�̏��������s��
	*/
	virtual void Init() = 0;

	/**
	* @fn void Update()
	* @brief �X�V�֐�
	* @details �p����Œ�`���A�I�u�W�F�N�g�̍X�V���s��
	*/
	virtual void Update() = 0;

	/**
	* @fn void Draw()
	* @brief �`��֐�
	* @details �p����Œ�`���A�I�u�W�F�N�g�̕`����s��
	*/
	virtual void Draw() = 0;

	/**
	* @fn void DrawShadowMap()
	* @brief �V���h�E�}�b�v�p�`��֐�
	* @details �p����Œ�`���A�V���h�E�}�b�v�p�����_�[�^�[�Q�b�g�ɃI�u�W�F�N�g�̕`����s��
	*/
	virtual void DrawShadowMap() = 0;

private:
	/**
	* @fn void Release()
	* @brief ����֐�
	* @details �p����Œ�`���A����������s��
	*/
	virtual void Release() = 0;

};

#endif