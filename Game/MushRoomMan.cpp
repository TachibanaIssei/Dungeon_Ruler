#include "stdafx.h"
#include "MushRoomMan.h"

#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "GameCamera.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

#include <time.h>
#include <stdlib.h>

namespace {
	const float SEARCH_LENGTH = 400.0f;			//プレイヤーを発見する距離
	const float MAGIC_HITEFFECT_SIZE = 50.0f;	//魔法攻撃ヒットエフェクトの大きさ
	const float DEAD_EFFECT_SIZE = 25.0f;		//死亡エフェクトの大きさ

	const float MUSH_MAPSPRITE_WIDTH = 52.0f;	//マップ画像の横サイズ
	const float MUSH_MAPSPRITE_HEIGHT = 60.0f;	//マップ画像の縦サイズ

	const float SE_VOLUME = 1.0f;
	const float MOVESE_VOLUME = 0.5f;

	const float HP_WINDOW_WIDTH = 1152.0f;
	const float HP_WINDOW_HEIGHT = 648.0f;
	const float HP_BER_WIDTH = 178.0f;
	const float HP_BER_HEIGHT = 22.0f;
	
	const Vector3 HP_BER_SIZE = Vector3(HP_BER_WIDTH, HP_BER_HEIGHT, 0.0f);

	const Vector2 PIVOT = Vector2(0.0f, 0.5f);

	const int LOST_HP_ATTACK = 1;
	const int LOST_HP_MAGIC = 2;
}

MushRoomMan::MushRoomMan()
{
}

MushRoomMan::~MushRoomMan()
{

}

bool MushRoomMan::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/mushroomman/MM_Idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/mushroomman/MM_Walk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/mushroomman/MM_Attack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/mushroomman/MM_Damage.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/mushroomman/MM_Dead.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);

	m_modelRender.Init("Assets/modelData/mushroomMan/MushroomMan.tkm", m_animationClips, enAnimationClip_Num);

	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);

	m_charCon.Init(
		40.0f,
		40.0f,
		m_position
	);

	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
		});

	//エフェクトのロード
	EffectEngine::GetInstance()->ResistEffect(6, u"Assets/effect/mush_impact.efk");
	EffectEngine::GetInstance()->ResistEffect(11, u"Assets/effect/magicwind_hit.efk");
	EffectEngine::GetInstance()->ResistEffect(12, u"Assets/effect/enemy_dead.efk");

	//音のロード
	g_soundEngine->ResistWaveFileBank(8, "Assets/sound/move_sound_munyu.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");

	//画像のロード
	m_mushMapSprite.Init("Assets/sprite/mushroom_image.DDS", MUSH_MAPSPRITE_WIDTH, MUSH_MAPSPRITE_HEIGHT);

	m_HPBar.Init("Assets/sprite/zako_HP_bar.DDS", HP_BER_WIDTH, HP_BER_HEIGHT);
	//m_HPBar.SetPivot(PIVOT);

	m_HPBack.Init("Assets/sprite/zako_HP_background.DDS", HP_WINDOW_WIDTH, HP_WINDOW_HEIGHT);

	m_HPFrame.Init("Assets/sprite/HP_flame_mushroom.DDS", HP_WINDOW_WIDTH, HP_WINDOW_HEIGHT);

	m_player = FindGO<Player>("player");
	m_map = FindGO<Map>("map");
	m_gameCamera = FindGO<GameCamera>("gameCamera");
	//乱数を初期化
	srand((unsigned)time(NULL));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);

	return true;
}

void MushRoomMan::Update()
{
	Chase();
	Rotation();
	Collision();
	Attack();
	HPBar();
	MapMove();
	PlayAnimation();
	ManageState();

	m_modelRender.Update();
}

void MushRoomMan::Chase()
{
	//追跡ステートでないなら、処理しない
	if (m_mushState != enMushState_Chase)
	{
		return;
	}

	m_moveSpeed.y = 0.0f;
	
	m_position = m_charCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	Vector3 modelPosition = m_position;
	m_modelRender.SetPosition(modelPosition);
}

void MushRoomMan::Rotation()
{
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001) {
		return;
	}

	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);
	m_rotation.SetRotationY(-angle);

	m_modelRender.SetRotation(m_rotation);

	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void MushRoomMan::Attack()
{
	//攻撃ステートでないなら処理をしない
	if (m_mushState != enMushState_Attack)
	{
		return;
	}

	//攻撃中であれば
	if (m_isUnderAttack == true)
	{
		//攻撃用のコリジョンを作成
		MakeAttackCollision();
	}
}

void MushRoomMan::Collision()
{
	//被ダメージ、あるいはダウンステートの時は
	//当たり判定をしない
	if (m_mushState == enMushState_ReceiveDamage ||
		m_mushState == enMushState_Down)
	{
		return;
	}

	{
		//プレイヤーの攻撃用のコリジョンを取得する
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_attack");
		//コリジョンの配列をfor文で回す
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら
			if (collision->IsHit(m_charCon))
			{
				m_player->isPlayerAttackHit();

				//HPを1減らす
				m_hp -= LOST_HP_ATTACK;
				//HPが0になったら
				if (m_hp <= 0)
				{
					//死亡ステートに遷移する
					m_mushState = enMushState_Down;
				}
				else {
					//被ダメージステートに遷移する
					m_mushState = enMushState_ReceiveDamage;
				}

				return;
			}
		}
	}

	{
		//プレイヤーのマジックボール用のコリジョンを取得する
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_magicball");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charCon))
			{
				//HPを減らす
				m_hp -= LOST_HP_MAGIC;

				//魔法攻撃ヒットエフェクト再生
				MakeMagicHitEffect();

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HPが0になったら
				if (m_hp <= 0)
				{
					//死亡ステートに遷移する
					m_mushState = enMushState_Down;
				}
				else
				{
					//被ダメージステートに遷移
					m_mushState = enMushState_ReceiveDamage;
				}
				return;
			}
		}
	}
}

const bool MushRoomMan::SearchPlayer() const
{
	Vector3 diff = m_player->GetPosition() - m_position;

	//プレイヤーにある程度近かったら
	if (diff.LengthSq() <= SEARCH_LENGTH * SEARCH_LENGTH)
	{
		//エネミーからプレイヤーに向かうベクトルを正規化する
		diff.Normalize();
		//エネミーの正面のベクトルと、エネミーからプレイヤーに向かう
		//ベクトルの内積(cosθ)を求める
		float cos = m_forward.Dot(diff);
		//内積(cosθ)から角度(θ)を求める
		float angle = acosf(cos);
		//角度(θ)が180°より小さければ
		if (angle <= (Math::PI / 180.0f) * 120.0f)
		{
			//プレイヤーを見つけた
			return true;
		}
	}
	//プレイヤーを見つけられなかった
	return false;
}

void MushRoomMan::MakeAttackCollision()
{
	auto collisionObject = NewGO<CollisionObject>(0);
	Vector3 collisionPosition = m_position;
	collisionPosition += m_forward * 70.0f;
	collisionPosition.y += 30.0f;
	collisionObject->CreateSphere(
		collisionPosition,
		Quaternion::Identity,
		30.0f
	);
	collisionObject->SetName("mush_attack");

	if (collisionObject->IsHit(m_player->GetCharacterController()))
	{
		MakeAttackEffect();
	}
}

void MushRoomMan::MakeMagicHitEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Quaternion rotation;

	effectEmitter->Init(11);
	effectEmitter->SetScale(Vector3::One * MAGIC_HITEFFECT_SIZE);
	effectPosition.y += 20.0f;
	rotation = m_rotation;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);
	effectEmitter->Play();
}

void MushRoomMan::ProcessCommonStateTransition()
{
	//各タイマーを初期化
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	if (m_player->isPlayerDead())
	{
		m_mushState = enMushState_Idle;
		return;
	}

	//エネミーからプレイヤーに向かうベクトルを計算する
	Vector3 diff = m_player->GetPosition() - m_position;

	//プレイヤーを見つけたら
	if (SearchPlayer() == true)
	{
		//ベクトルを正規化する
		diff.Normalize();
		//移動速度を設定
		m_moveSpeed = diff * 130.0f;
		//攻撃できる距離なら
		if (IsCanAttack() == true)
		{
			//乱数によって、攻撃するか待機させるかを決定する
			int ram = rand() % 100;
			if (ram > 70)
			{
				//攻撃ステートに遷移
				m_mushState = enMushState_Attack;
				m_isUnderAttack = false;
				m_player->ResetMushAttackHit();
				return;
			}
			else
			{
				//待機ステートに遷移
				m_mushState = enMushState_Idle;
				return;
			}
		}
		//攻撃できない距離だったら
		else
		{
			//追跡ステートに遷移
			m_mushState = enMushState_Chase;
			return;
		}
	}

	//プレイヤーを見つけられなければ
	else
	{
		//待機ステートに遷移
		m_mushState = enMushState_Idle;
		return;
	}
}

void MushRoomMan::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//待機時間がある程度経過したら
	if (m_idleTimer >= 1.0f)
	{
		//他のステートに遷移
		ProcessCommonStateTransition();
	}
}

void MushRoomMan::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void MushRoomMan::ProcessChaseStateTransition()
{
	//攻撃できる距離なら
	if (IsCanAttack() == true)
	{
		//他のステートに遷移
		ProcessCommonStateTransition();
		return;
	}
	m_chaseTimer += g_gameTime->GetFrameDeltaTime();
	//追跡時間がある程度経過したら
	if (m_chaseTimer >= 0.8f)
	{
		ProcessCommonStateTransition();
	}
}

void MushRoomMan::ProcessAttackStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ProcessCommonStateTransition();
	}
}

void MushRoomMan::ProcessReceiveDamageStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//攻撃されたら距離関係なしに、追跡させる
		m_mushState = enMushState_Chase;
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		m_moveSpeed = diff * 250.0f;
	}
}

void MushRoomMan::ProcessDownStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		Game* game = FindGO<Game>("game");
		//倒されたエネミー数を＋1する
		game->AddDefeatedEnemyNumber();

		//死亡エフェクトを再生
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		Vector3 effectPosition = m_position;
		Quaternion rotation;

		effectEmitter->Init(12);
		effectEmitter->SetScale(Vector3::One * DEAD_EFFECT_SIZE);
		effectPosition.y += 10.0f;
		rotation = m_rotation;
		effectEmitter->SetPosition(effectPosition);
		effectEmitter->SetRotation(rotation);
		effectEmitter->Play();

		//自身を削除
		DeleteGO(this);
	}
}

const bool MushRoomMan::IsCanAttack() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	//エネミーとプレイヤーの距離が近かったら
	if (diff.LengthSq() <= 100.0f * 100.0f)
	{
		//攻撃できる
		return true;
	}
	//攻撃できない
	return false;
}

void MushRoomMan::MakeAttackEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Vector3 flont = Vector3::AxisZ;
	Quaternion rotation;

	effectEmitter->Init(6);
	effectEmitter->SetScale(Vector3::One * 3.0f);
	effectPosition.y += 50.0f;
	rotation = m_rotation;
	m_rotation.Apply(flont);
	flont *= 100.0f;
	effectPosition += flont;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);
	effectEmitter->Play();
}

void MushRoomMan::HPBar()
{
	//HPが0を下回ったら
	if (m_hp < 0)
	{
		//HPを0にする
		m_hp = 0;
	}
	//HPが最大HPを上回ったら
	if (m_hp > m_maxHP)
	{
		//HPを最大HPにする
		m_hp = m_maxHP;
	}

	Vector3 scale = Vector3::One;
	scale.x = float(m_hp) / float(m_maxHP);
	m_HPBar.SetScale(scale);

	Vector3 BerPosition = m_position;
	BerPosition.y += 130.0f;

	//座標を変換する
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBerPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPWindowPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBackPos, BerPosition);

	//HPバー画像を左寄せに表示する
	Vector3 BerSizeSubtraction = HPBerSend(HP_BER_SIZE,scale);	//画像の元の大きさ
	m_HPBerPos.x -= BerSizeSubtraction.x;

	m_HPBar.SetPosition(Vector3(m_HPBerPos.x, m_HPBerPos.y, 0.0f));
	m_HPFrame.SetPosition(Vector3(m_HPWindowPos.x, m_HPWindowPos.y, 0.0f));
	m_HPBack.SetPosition(Vector3(m_HPBackPos.x, m_HPBackPos.y, 0.0f));

	m_HPBar.Update();
	m_HPFrame.Update();
	m_HPBack.Update();
}

Vector3 MushRoomMan::HPBerSend(Vector3 size, Vector3 scale)
{
	Vector3 hpBerSize = size;								//画像の元の大きさ
	Vector3 changeBerSize = Vector3::Zero;					//画像をスケール変換したあとの大きさ
	Vector3 BerSizeSubtraction = Vector3::Zero;				//画像の元と変換後の差

	changeBerSize.x = hpBerSize.x * scale.x;
	BerSizeSubtraction.x = hpBerSize.x - changeBerSize.x;
	BerSizeSubtraction.x /= 2.0f;

	return BerSizeSubtraction;
}

bool MushRoomMan::DrawHP()
{
	Vector3 toCameraTarget = m_gameCamera->GetTarget() - m_gameCamera->GetPosition();
	Vector3 toMush = m_position - m_gameCamera->GetPosition();
	toCameraTarget.y = 0.0f;
	toMush.y = 0.0f;
	toCameraTarget.Normalize();
	toMush.Normalize();

	float cos = Dot(toCameraTarget, toMush);
	float angle = acos(cos);

	if (fabsf(angle) < Math::DegToRad(45.0f))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MushRoomMan::MapMove()
{
	Vector3 playerPosition = m_player->GetPosition();
	Vector3 enemyPosition = m_position;

	Vector3 mapPosition;

	//マップに表示する範囲に敵がいたら。
	if (m_map->WorldPositionConvertToMapPosition(playerPosition, enemyPosition, mapPosition))
	{
		//マップに表示するように設定する、
		m_isMapImage = true;
		//SpriteRenderに座標を設定。
		m_mushMapSprite.SetPosition(mapPosition);
	}
	//マップに表示する範囲に敵がいなかったら。
	else
	{
		//マップに表示しない。
		m_isMapImage = false;
	}

	m_mushMapSprite.Update();
}

void MushRoomMan::ReStart()
{
	m_position = m_firstPosition;
	m_rotation = m_firstRotation;
	m_moveSpeed = Vector3::Zero;
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_charCon.SetPosition(m_position);
	m_charCon.Execute(m_moveSpeed, 0.0f);
	m_modelRender.Update();
}

void MushRoomMan::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	//(void)clipName;
	//キーネームが「attack_start」のとき
	if (wcscmp(eventName, L"attack_start") == 0) {
		m_isUnderAttack = true;
	}
	else if (wcscmp(eventName, L"attack_end") == 0) {
		m_isUnderAttack = false;
	}
	
	if (wcscmp(eventName, L"moveSE") == 0) {
		//歩き効果音再生
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(8);
		se->SetVolume(MOVESE_VOLUME);
		se->Play(false);
	}
}

void MushRoomMan::ManageState()
{
	switch (m_mushState)
	{
		//待機ステートの時
	case enMushState_Idle:
		//待機ステートの遷移処理
		ProcessIdleStateTransition();
		break;
		//追跡ステートの時
	case enMushState_Chase:
		//追跡ステートの遷移処理
		ProcessChaseStateTransition();
		break;
		//攻撃ステートの時
	case enMushState_Attack:
		//攻撃ステートの遷移処理
		ProcessAttackStateTransition();
		break;
		//被ダメージステートの時
	case enMushState_ReceiveDamage:
		//被ダメージステートの遷移処理
		ProcessReceiveDamageStateTransition();
		break;
		//死亡ステートの時
	case enMushState_Down:
		//死亡ステートの遷移処理
		ProcessDownStateTransition();
		break;
	}
}

void MushRoomMan::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_mushState)
	{
		//待機ステート
	case enMushState_Idle:
		//待機アニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//追跡ステート
	case enMushState_Chase:
		//歩きアニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
		//攻撃ステート
	case enMushState_Attack:
		//攻撃アニメーション再生
		m_modelRender.SetAnimationSpeed(0.6f);
		m_modelRender.PlayAnimation(enAnimationClip_Attack);
		break;
		//被ダメージステート
	case enMushState_ReceiveDamage:
		//被ダメージアニメーション再生
		m_modelRender.SetAnimationSpeed(0.9f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage);
		break;
		//死亡ステート
	case enMushState_Down:
		//死亡アニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Down);
		break;
	default:
		break;
	}
}

void MushRoomMan::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	if (m_player->GetSpriteFlag())
	{
		if (DrawHP())
		{
			m_HPBack.Draw(rc);
			m_HPBar.Draw(rc);
			m_HPFrame.Draw(rc);
		}
	}
}