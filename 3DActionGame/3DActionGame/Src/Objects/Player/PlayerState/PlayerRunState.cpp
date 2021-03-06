#include "../Player.h"
#include "PlayerIdleState.h"
#include "PlayerRunState.h"
#include "PlayerAttack01State.h"
#include "PlayerHitState.h"
#include "../../../Engine/Engine.h"
#include "../../../Utility/Calculation.h"

PlayerState* PlayerRunState::CheckState(Player* player_)
{
	if (player_->m_IsHit)
	{
		return PlayerHitState::GetInstance();
	}

	// 左クリックが押された時は攻撃状態へ遷移
	if (INPUT_MANAGER->GetMouseDown(MouseButton::Left))
	{
		return PlayerAttack01State::GetInstance();
	}
	
	// 移動速度が0に近かったら待機状態へ
	if (Calculation::Length(player_->GetVelocity()) < 0.1f)
	{
		player_->SetVelocity(DirectX::XMFLOAT3(0.f, 0.f, 0.f));
		return PlayerIdleState::GetInstance();
	}
	
	// 移動状態
	return PlayerRunState::GetInstance();
}

void PlayerRunState::Update(Player* player_)
{
	// 移動ベクトル
	DirectX::XMFLOAT2 move_vec(0.f, 0.f);
	
	FollowCamera* camera = player_->m_Camera;

	// カメラの前向きベクトルを取得
	DirectX::XMFLOAT3 foward = camera->GetFowardNormal();
	// カメラの左向きベクトルを取得
	DirectX::XMFLOAT3 left   = camera->GetLeftNormal();

	// 前
	if (INPUT_MANAGER->GetKey(KeyInfo::Key_W)) {
		move_vec.x += foward.x;
		move_vec.y += foward.z;
	}
	// 後
	else if (INPUT_MANAGER->GetKey(KeyInfo::Key_S)) {
		move_vec.x -= foward.x;
		move_vec.y -= foward.z;
	}
	// 左
	if (INPUT_MANAGER->GetKey(KeyInfo::Key_A)) {
		move_vec.x -= left.x;
		move_vec.y += left.z;
	}
	// 右
	else if (INPUT_MANAGER->GetKey(KeyInfo::Key_D)) {
		move_vec.x += left.x;
		move_vec.y -= left.z;
	}

	player_->Move(move_vec.x, move_vec.y);
}

void PlayerRunState::Enter(Player* player_)
{
	player_->m_OldVelocity = player_->m_DirectionVec;
	player_->SetMotion("Run");
}