#include <sstream>
#include "GameScene.h"
#include "SceneManager.h"
#include "../Model/FbxStorage.h"
#include "../Engine/DirectGraphics.h"


GameScene::GameScene(SceneChanger* sceneChanger_) : 
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
    p = nullptr;
}

GameScene::~GameScene()
{
    
}

void GameScene::Load()
{
    if (WaitForSingleObject(m_ThreadHandle, 0) == WAIT_OBJECT_0)
    {
        p = new Player();

        m_CurrentState = SceneState::Main;
    }
}

DWORD WINAPI GameScene::LoadResources(LPVOID lpParam_)
{
    FbxStorage::GetInstance()->LoadModel("Res/Models/Ekard.fbx", "Ekard");
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Ekard_Run_01.fbx", "Ekard", "Run");
    
    return 0;
}

void GameScene::Main()
{
    if (p)p->Update();
}

void GameScene::Update()
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

void GameScene::Draw()
{
    switch (m_CurrentState)
    {
    case SceneState::Load:
        break;
    case SceneState::Main:
        if (p) p->Draw();
        break;
    default:
        break;
    }
}