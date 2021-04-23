#ifndef PLAYER_HIT_STATE_H_
#define PLAYER_HIT_STATE_H_

#include "PlayerState.h"

/**
* @brief �v���C���[�̍U�����󂯂���Ԃ��Ǘ�����N���X
*/
class PlayerHitState : public PlayerState
{
public:
	/**
	* @fn static PlayerHitState* GetInstance()
	* @brief �C���X�^���X��Ԃ��֐�
	* @return PlayerHitState* �C���X�^���X�̃|�C���^
	*/
	static PlayerHitState* GetInstance()
	{
		static PlayerHitState instance;
		return &instance;
	}

	/**
	* @fn PlayerState* CheckState(Player* player_)
	* @brief ��Ԃ��m�F���ēK�؂ȏ�Ԃ�Ԃ��֐�
	* @param[in] player_ �v���C���[�N���X�̃|�C���^
	* @return Player* �v���C���[�̏�ԃN���X�̃|�C���^��Ԃ�
	*/
	PlayerState* CheckState(Player* player_) override;

	/**
	* @fn void Update(Player* player_)
	* @brief �v���C���[���U����Ԏ��̍X�V���s���֐�
	* @param[in] player_ �v���C���[�N���X�̃|�C���^
	*/
	void Update(Player* player_) override;

	/**
	* @fn void Enter(Player* player_)
	* @brief ��Ԃ��؂�ւ�����ۂ̏������֐�
	* @param[in] player_ �v���C���[�N���X�̃|�C���^
	* @details ��Ԃ��؂�ւ��������1�x�����Ăяo��
	*/
	void Enter(Player* player_) override;

private:
	/**
	* @brief �R���X�g���N�^
	*/
	PlayerHitState()
	{}

	/**
	* @brief �f�X�g���N�^
	*/
	~PlayerHitState() final
	{}

private:

};

#endif