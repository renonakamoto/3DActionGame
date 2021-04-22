#include <sstream>
#include "GameScene.h"
#include "SceneManager.h"
#include "../Model/FbxStorage.h"
#include "../Model/ObjFileStrage.h"
#include "../Engine/Engine.h"
#include "../Objects/Player/Player.h"
#include "../Objects/Enemy/Enemy.h"
#include "../Objects/Enemy/EnemyManager.h"
#include "../Objects/Stage/Stage.h"
#include "../Objects/Camera/FollowCamera.h"
#include "../CollisionManager/CollisionManager.h"
#include "../GameManager/GameManager.h"
#include "../Model/FbxStorage.h"


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

    TEX_MANAGER->Load("Res/Textures/nowloading.png", "NowLoading");
}

GameScene::~GameScene()
{
    CollisionManager::GetInstance()->AllRelease();
    ObjectManager::GetInstance()->AllRelease();
    TEX_MANAGER->AllRelease();
    FbxStorage::GetInstance()->AllRelease();
    ObjFileStrage::GetInstance()->AllRelease();
}

void GameScene::Load()
{
    if (WaitForSingleObject(m_ThreadHandle, 0) == WAIT_OBJECT_0)
    {
        // ���\�[�X�̓ǂݍ��݂��I��������A�e�I�u�W�F�N�g�̃C���X�^���X���쐬
        ObjectManager::GetInstance()->Register(new Player(DirectX::XMFLOAT3(0.f, 100.f, 0.f)));
        ObjectManager::GetInstance()->Register(new EnemyManager());

        ObjectManager::GetInstance()->Register(new Stage());
        ObjectManager::GetInstance()->Register(new FollowCamera());
        
        // �e�I�u�W�F�N�g�̐�����A�e�I�u�W�F�N�g��Init���s��
        ObjectManager::GetInstance()->Init();
        // ���̓��[�h��ύX
       INPUT->SetInputMode(InputMode::MODE_GAME);
        m_CurrentState = SceneState::Main;
    }
}

DWORD WINAPI GameScene::LoadResources(LPVOID lpParam_)
{
    // �v���C���[���f���̓ǂݍ���
    FbxStorage::GetInstance()->LoadModel("Res/Models/Ekard.fbx",                  "Ekard");
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Ekard_Run_01.fbx",          "Ekard", "Run");
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Ekard_Attack_01.fbx",       "Ekard", "Attack01");
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Ekard_Attack_02.fbx",       "Ekard", "Attack02");
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Ekard_BattleIdle_01_h.fbx", "Ekard", "Idle");
    FbxStorage::GetInstance()->GetModel("Ekard")->AddMesh("Res/Models/Sword_12.fbx", DirectX::XMFLOAT3(22.9f, 0.0f, 40.0f), "Bip001 R Hand");

    // �G�l�~�[���f���̓ǂݍ���
    FbxStorage::GetInstance()->LoadModel("Res/Models/Enemy/Grenadier.fbx",              "Enemy");
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Enemy/@GrenadierWalk.fbx",        "Enemy", "Walk"  );
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Enemy/@GrenadierIdle.fbx",        "Enemy", "Idle"  );
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Enemy/@GrenadierMeleeAttack.fbx", "Enemy", "Attack");
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Enemy/@GrenadierDeath.fbx",       "Enemy", "Death" );
    FbxStorage::GetInstance()->LoadMotion("Res/Models/Enemy/@GrenadierHit.fbx",         "Enemy", "Hit"   );

    // �X�e�[�W���f���̓ǂݍ���
    ObjFileStrage::GetInstance()->LoadModel("Res/Models/Ground.obj",     "Stage");

    // �����蔻��p�̃L���[�u���f���̓ǂݍ���
    ObjFileStrage::GetInstance()->LoadModel("Res/Models/Shape/Cube.obj", "Cube" );
    return 0;
}

void GameScene::Main()
{
    // �I�u�W�F�N�g�̍X�V
    ObjectManager::GetInstance()->Update();
    
    
#ifdef _DEBUG
    // Esc�L�[�œ��̓��[�h�ؑ�
    if (INPUT->GetKeyDown(KeyInfo::Key_ESC))
    {
        if (INPUT->GetInputMode() == InputMode::MODE_GAME)
        {
            INPUT->SetInputMode(InputMode::MODE_UI);
        }
        else
        {
            INPUT->SetInputMode(InputMode::MODE_GAME);
        }

    }

#endif

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
        
        TEX_MANAGER->Render("NowLoading", DirectX::XMFLOAT3(0.f, 0.f, 0.f));
        break;
        
    case SceneState::Main:
        ObjectManager::GetInstance()->Draw();
        
#ifdef _DEBUG
        {
            static bool is_draw = true;
            if (INPUT->GetKeyDown(KeyInfo::Key_Return)) is_draw = !is_draw;
            if (is_draw) {
                CollisionManager::GetInstance()->Draw();
            }
        }
#endif
        break;
    default:
        break;
    }
}