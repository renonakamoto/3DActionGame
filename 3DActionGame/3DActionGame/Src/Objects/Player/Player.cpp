﻿#include "Player.h"
#include "../../Engine/Engine.h"
#include "../../Utility/Calculation.h"
#include "PlayerState/PlayerIdleState.h"
#include "../../ObjectManager/ObjectManager.h"
#include "../../CollisionManager/CollisionManager.h"
#include "../../Objects/Enemy/Enemy.h"
#include "../../GameManager/GameManager.h"

void Player::Init()
{
	// モデル情報の取得
	m_Model = new SkeletalModel(FbxStorage::GetInstance()->GetModel("Ekard"));
	
	// プレイヤーステートの初期化
	m_State = PlayerIdleState::GetInstance();
	m_State->Enter(this);

	// 参照用オブジェクトの取得
	// ステージオブジェクトの取得
	ObjectBase* stage = ObjectManager::GetInstance()->GetObj("Stage");
	if (stage) m_Stage = dynamic_cast<Stage*>(stage);
	if (m_Stage)
	{
		m_Pos.y = m_Stage->GetPolygonHeight(m_Pos);
	}
	
	// カメラオブジェクトの取得
	ObjectBase* camera = ObjectManager::GetInstance()->GetObj("FollowCamera");
	if (camera) m_Camera = dynamic_cast<FollowCamera*>(camera);
	if (m_Camera)
	{
		m_Camera->SetFollowObject(this);
		m_Camera->SetDistance(m_CameraDistance);
		m_Camera->SetOffset(m_CameraLookAtOffset);
		m_Camera->SetHeight(m_CameraHeight);
	}

	
	DirectX::XMFLOAT3 shape_pos = m_Pos;
	shape_pos.y = m_Pos.y + (m_PlayerSize.y / 2.f);
	m_Shape = new ShapeOBB(shape_pos, m_PlayerSize.x / 2.f, m_PlayerSize.y / 2.f, m_PlayerSize.z / 2.f);
	m_OBB = dynamic_cast<ShapeOBB*>(m_Shape);

	// コリジョンマネージャーに登録
	CollisionManager::GetInstance()->Register(this);

	m_AttackVolume = new ShapeOBB(shape_pos, m_AttackVolumLength.x, m_AttackVolumLength.y, m_AttackVolumLength.z);
}

void Player::Update()
{
#ifdef _DEBUG
	if (INPUT_MANAGER->GetKeyDown(KeyInfo::Key_8))
	{
		Damage(5);
	}
#endif

	// もし死亡したらゲームマネージャーに伝える
	if (m_IsDeath == true) {
		GameManager::GetInstance()->GameOver();
		return;
	}
	
	// プレイヤーの状態を更新する
	PlayerState* state = m_State->CheckState(this);

	// 前のフレームの状態と違った場合は更新する
	if (state != m_State)
	{
		m_State = state;
		m_State->Enter(this);
	}

	// ステートの更新
	m_State->Update(this);
	
	// モーションのフレームを進める
	m_Model->Animate();
}

void Player::Draw()
{	
	GRAPHICS->SetRasterizerMode(RasterizerMode::MODE_CULL_NONE);
	m_Model->Render(m_Pos, m_Scale, m_Rot);
#ifdef _DEBUG
	if (m_AttackVolume)m_AttackVolume->Draw();
#endif
}

void Player::Damage(int damageNum_)
{
	m_Hp -= damageNum_;

	m_IsHit = true;
}

void Player::Release()
{
	delete m_AttackVolume;
	m_AttackVolume = nullptr;
}

void Player::Attack()
{
	// 当たり判定の更新
	DirectX::XMFLOAT3 shape_pos = Calculation::Add(m_Pos, Calculation::Mul(m_DirectionVec, m_AttackForwardDistance));
	shape_pos.y = m_Pos.y + (m_PlayerSize.y / 2.f);
	m_AttackVolume->m_Pos = shape_pos;
	m_AttackVolume->m_NormalDirect[0] = DirectX::XMFLOAT3(m_DirectionVec.z, m_DirectionVec.y, -m_DirectionVec.x);
	m_AttackVolume->m_NormalDirect[2] = m_DirectionVec;
	
	// 敵と当たっているかを調べる
	std::vector<Object3D*> hit_list;
	CollisionManager::GetInstance()->CheckHitObjects(*m_AttackVolume, &hit_list);
	for (auto obj : hit_list) {
		// 敵に当たっていたら
		if (obj->GetTag() == "Enemy") {
			// 敵にダウンキャスト
			Enemy* enemy = dynamic_cast<Enemy*>(obj);
			if (enemy) {
				// 敵にダメージを与える
				enemy->Damage(m_AttackPower);
			}
		}
	}
}

void Player::Move(float x_, float z_)
{
	// 移動ベクトルを正規化
	m_Velocity = DirectX::XMFLOAT3(x_, 0.f, z_);
	if (m_Velocity.x != 0.f || m_Velocity.z != 0.f)
	{
		m_Velocity = Calculation::Normalize(m_Velocity);
	}

	// 前回の速度と今回の速度で線形補間を行う
	m_Velocity    = Calculation::Lerp(m_OldVelocity, m_Velocity, (m_RotateSpeed / 60.0f));
	// 今回の速度を保存する
	m_OldVelocity = m_Velocity;

	if (Calculation::Length(m_Velocity) > 0.1f)
	{
		// 移動する前に座標を保存する
		m_OldPos = m_Pos;
		// 移動方向を算出
		float angle = atan2f(m_Velocity.x, m_Velocity.z);
		m_Rot.y = DirectX::XMConvertToDegrees(angle);

		// 移動する
		m_Pos.x += m_Velocity.x * m_Speed * (1.f / 60.f);
		m_Pos.z += m_Velocity.z * m_Speed * (1.f / 60.f);

		// 移動地点でのステージのポリゴンの高さを算出
		m_Pos.y = m_Stage->GetPolygonHeight(m_Pos);

		// 当たり判定の更新を行う
		DirectX::XMFLOAT3 shape_pos = m_Pos;
		shape_pos.y = m_Pos.y + (m_PlayerSize.y / 2.f);
		m_OBB->m_Pos = shape_pos;
		m_OBB->m_NormalDirect[0] = Calculation::Normalize(DirectX::XMFLOAT3(m_Velocity.z, m_Velocity.y, -m_Velocity.x));
		m_OBB->m_NormalDirect[2] = Calculation::Normalize(m_Velocity);

		// 移動
		m_DirectionVec = Calculation::Normalize(m_Velocity);
	}
}