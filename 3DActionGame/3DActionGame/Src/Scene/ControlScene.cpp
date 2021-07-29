#include "ControlScene.h"
#include "../Engine/Engine.h"
#include "../ObjectManager/ObjectManager.h"
#include "../Objects/2DObject/BackGround.h"


ControlScene::ControlScene(SceneChanger* sceneChanger_) : Scene(sceneChanger_)
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

ControlScene::~ControlScene()
{
    ObjectManager::GetInstance()->AllRelease();
    TEX_MANAGER->AllRelease();
}

void ControlScene::Load()
{
    if (WaitForSingleObject(m_ThreadHandle, 0) == WAIT_OBJECT_0)
    {
        ObjectManager::GetInstance()->Register(new Background("control_bg"));

        ObjectManager::GetInstance()->Init();

        // ���̓��[�h�ύX
        INPUT_MANAGER->SetInputMode(InputMode::MODE_UI);
        m_CurrentState = SceneState::Main;
    }
}

DWORD WINAPI ControlScene::LoadResources(LPVOID lpParam_)
{
    TEX_MANAGER->Load("Res/Textures/control_bg.png", "control_bg");

    return 0;
}


void ControlScene::Main()
{
    if (INPUT_MANAGER->GetMouseDown(MouseButton::Left))
    {
        m_SceneChanger->ChangeScene(SceneID::Tilte);
        return;
    }

    ObjectManager::GetInstance()->Update();
}

void ControlScene::Update()
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

void ControlScene::Draw()
{
    GRAPHICS->StartRendering(KindRT::RT_ON_SCREEN);
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
    GRAPHICS->FinishRendering();
}