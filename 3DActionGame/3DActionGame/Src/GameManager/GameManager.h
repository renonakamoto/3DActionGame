#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_

/**
* @brief �Q�[�����̊Ǘ����s���N���X
*/
class GameManager
{
public:
	/**
	* @fn static GameManager* GetInstance()
	* @brief �C���X�^���X��Ԃ��֐�
	* @return GameManager* �C���X�^���X�̃|�C���^
	*/
	static GameManager* GetInstance()
	{
		static GameManager instance;
		return &instance;
	}

	/**
	* @fn void GameClear()
	* @brief GameClear�V�[���̑J�ڂ���֐�
	* @details �Q�[���N���A�����ۂɌĂ�
	*/
	void GameClear();

	/**
	* @fn void GameOver()
	* @brief GameOver�V�[���̑J�ڂ���֐�
	* @details �Q�[���I�[�o�[�����ۂɌĂ�
	*/
	void GameOver();

	/**
	* @fn void QuitGame()
	* @brief �A�v���P�[�V�������I������֐�
	* @details �A�v���P�[�V�����I���t���O���I���ɂ���
	*/
	void QuitGame() { m_Quit = true; }

	/**
	* @fn bool HasQuit()
	* @brief �A�v���P�[�V�����I���t���O��Ԃ��֐�
	* @return bool �A�v���P�[�V�����I���t���O
	*/
	bool HasQuit()const { return m_Quit; }
	
private:
	/**
	* @brief �R���X�g���N�^
	*/
	GameManager():
		m_Quit(false)
	{}

	/**
	* @brief �f�X�g���N�^
	*/
	~GameManager()
	{}

private:
	bool m_Quit;	//! �A�v���P�[�V�������I���������ǂ���

};

#endif