#include <sstream>
#include "GameScene.h"
#include "SceneManager.h"
#include "../Model/FbxStorage.h"
#include "../Engine/DirectGraphics.h"

DirectX::XMFLOAT3 g_pos(0.f, 0.f, 0.f);
DirectX::XMFLOAT3 g_scale(1.f, 1.f, 1.f);
DirectX::XMFLOAT3 g_degree(0.f, 0.f, 0.f);


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
}

GameScene::~GameScene()
{
    
}

void GameScene::Load()
{
    if (WaitForSingleObject(m_ThreadHandle, 0) == WAIT_OBJECT_0)
    {
        FbxModel* model = FbxStorage::GetInstance()->GetModel("Ekard");
        model->Play("Run");

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
    FbxModel* model = FbxStorage::GetInstance()->GetModel("Ekard");
    model->Animate();
    
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
    FbxStorage::GetInstance()->GetModel("Ekard")->Render(DirectGraphics::GetInstance(), g_pos, g_scale, g_degree);
}