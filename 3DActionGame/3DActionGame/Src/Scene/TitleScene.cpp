#include <thread>
#include <sstream>
#include "TitleScene.h"
#include "SceneManager.h"
#include "../Engine/Engine.h"
#include "../Engine/Texture/TextureManager.h"
#include "../Objects/2DObject/BackGround.h"
#include "../Objects/2DObject/Button.h"
#include "../GameManager/GameManager.h"


TitleScene::TitleScene(SceneChanger* sceneChanger_) : 
    Scene(sceneChanger_)
{
    m_ThreadHandle = CreateThread(
        nullptr,                    // �Z�L�����e�B����
        0,                          // �X�^�b�N�T�C�Y
        this->LoadResources,        // �X���b�h�֐�
        nullptr,                    // �X���b�h�֐��ɓn������
        0,                          // �쐬�I�v�V����
        &m_dwThreadID);             // �X���b�hID

    m_CurrentState = SceneState::Load;
}

TitleScene::~TitleScene()
{
    ObjectManager::GetInstance()->AllRelease();
    TEX_MANAGER->AllRelease();
}

void TitleScene::Load()
{
    if (WaitForSingleObject(m_ThreadHandle, 0) == WAIT_OBJECT_0)
    {
        // �I�u�W�F�N�g�̓o�^
        ObjectManager::GetInstance()->Register(new Background("bg", DirectX::XMFLOAT3(0.f, 0.f, 1.f)));
        ObjectManager::GetInstance()->Register(new Button<TitleScene>("start_ui", "quit_ui", DirectX::XMFLOAT3(200.f, 360.f, 0.f), this, &TitleScene::NextScene));
        ObjectManager::GetInstance()->Register(new Button<GameManager>("quit_ui", "start_ui", DirectX::XMFLOAT3(800.f, 360.f, 0.f), GameManager::GetInstance(), &GameManager::QuitGame));

        // �I�u�W�F�N�g�̏�����
        ObjectManager::GetInstance()->Init();

        // ���̓��[�h�ύX
        INPUT->SetInputMode(InputMode::MODE_UI);
        m_CurrentState = SceneState::Main;
    }
}

void TitleScene::NextScene()
{
    m_SceneChanger->ChangeScene(SceneID::Game);
}

DWORD WINAPI TitleScene::LoadResources(LPVOID lpParam_)
{
    TEX_MANAGER->Load("Res/Textures/title_bg.png", "bg");
    TEX_MANAGER->Load("Res/Textures/title_ui_quit.png", "quit_ui");
    TEX_MANAGER->Load("Res/Textures/title_ui_start.png", "start_ui");

    return 0;
}

void TitleScene::Main()
{
    ObjectManager::GetInstance()->Update();
}

void TitleScene::Update()
{
	switch (m_CurrentState)
	{
    case SceneState::Load:
        Load();
        break;
    case SceneState::Main:
        Main();
        break;
    default:
        break;
	}
}

void TitleScene::Draw()
{
    switch (m_CurrentState)
    {
    case SceneState::Load:
        break;
    case SceneState::Main:
        ObjectManager::GetInstance()->Draw();
        break;
    default:
        break;
    }
}