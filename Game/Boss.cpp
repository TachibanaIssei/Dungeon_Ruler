#include "stdafx.h"
#include "Boss.h"

#include "Game.h"
#include "Player.h"
#include "MagicThunder.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

#include <time.h>
#include <stdlib.h>

namespace {
	const float SEARCH_LENGTH = 400.0f;		//プレイヤーを発見する距離
	const float FIRST_POSITION_Y = 261.564;
	const Vector3 ATTACK_COLLISION_SIZE = Vector3(100.0f, 50.0f, 50.0f);
	const float DEAD_EFFECT_SIZE = 30.0f;	//死亡エフェクトの大きさ

	const float SE_VOLUME = 1.0f;
	const float DEAD_SE_VOLUME = 0.8f;

	const Vector2 HP_BER_PIVOT = Vector2(0.0f, 0.5f);
	const Vector3 HP_BER_POS = Vector3(-448.0f, 425.0f, 0.0f);
	const Vector3 HP_FRAME_POS = Vector3(0.0f, 425.0f, 0.0f);
	const Vector3 HP_BACK_POS = Vector3(0.0f, 425.0f, 0.0f);
	const Vector3 BOSS_NAME_POS = Vector3(0.0f, 495.0f, 0.0f);

	const int LOST_HP_ATTACK = 1;
	const int LOST_HP_MAGIC = 3;
}

Boss::Boss()
{
}

Boss::~Boss()
{
}

//todo アニメーションの追加
bool Boss::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/skeleton/SkeletonStand.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/skeleton/SkeletonRun.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/skeleton/SkeletonAttack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Magic].Load("Assets/animData/skeleton/SkeletonSkil.tka");
	m_animationClips[enAnimationClip_Magic].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/skeleton/SkeletonDamage.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/skeleton/SkeletonDeath.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);

	m_modelRender.Init("Assets/modelData/skeleton/skeleton.tkm", m_animationClips, enAnimationClip_Num);

	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);

	//剣のボーンを探す
	m_swordBoneId = m_modelRender.FindBoneID(L"Bip001 R Hand");

	m_charCon.Init(
		35.0f,
		50.0f,
		m_position
	);

	//エフェクトのロード
	EffectEngine::GetInstance()->ResistEffect(3, u"Assets/effect/efk/enemy_slash_01.efk");
	EffectEngine::GetInstance()->ResistEffect(5, u"Assets/effect/magic_circle.efk");
	EffectEngine::GetInstance()->ResistEffect(12, u"Assets/effect/enemy_dead.efk");

	//音のロード
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");
	g_soundEngine->ResistWaveFileBank(10, "Assets/sound/sakebi_sound_kyuaa.wav");
	g_soundEngine->ResistWaveFileBank(18, "Assets/sound/attack_sound_kin.wav");

	//画像のロード
	m_bossName.Init("Assets/sprite/boss_name.dds", 1536, 864);
	m_bossName.SetPosition(BOSS_NAME_POS);
	m_bossName.Update();

	m_HPbar.Init("Assets/sprite/boss_HP_bar.dds", 898, 46);
	m_HPbar.SetPivot(HP_BER_PIVOT);
	m_HPbar.SetPosition(HP_BER_POS);
	m_HPbar.Update();

	m_HPframe.Init("Assets/sprite/boss_HPwaku.dds", 1920, 1080);
	m_HPframe.SetPosition(HP_FRAME_POS);
	m_HPframe.Update();

	m_HPback.Init("Assets/sprite/boss_HP_background.dds", 1920, 1080);
	m_HPback.SetPosition(HP_BACK_POS);
	m_HPback.Update();

	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
		});

	m_player = FindGO<Player>("player");

	//乱数を初期化
	srand((unsigned)time(NULL));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
	return true;
}

void Boss::Update()
{
	Chase();
	Rotation();
	Collision();
	Attack();
	HPBer();
	PlayAnimation();
	ManageState();

	m_modelRender.Update();
}

void Boss::Chase()
{
	//追跡ステートでないなら、処理しない
	if (m_BossState != enBossState_Chase)
	{
		return;
	}

	m_moveSpeed.y = 0.0f;

	m_position = m_charCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	Vector3 modelPosition = m_position;
	m_modelRender.SetPosition(modelPosition);
}

void Boss::Rotation()
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

void Boss::Attack()
{
	//攻撃ステートでないなら処理をしない
	if (m_BossState != enBossState_Attack)
	{
		return;
	}

	//攻撃中であれば
	if (m_isUnderAttack == true)
	{
		//攻撃用のコリジョンを作成
		MakeAttackCollision();
		MakeSlashingEffect();
	}
}

void Boss::MakeMagic()
{
	MagicThunder* magicThunder = NewGO<MagicThunder>(0,"magicThunder");
	magicThunder->SetEnMagician(MagicThunder::enMagician_Enemy);
}

void Boss::Collision()
{
	//被ダメージ、あるいはダウンステートの時は
	//当たり判定をしない
	if (m_BossState == enBossState_ReceiveDamage ||
		m_BossState == enBossState_Down)
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

				//HPを減らす
				m_hp -= LOST_HP_ATTACK;
				//HPが0になったら
				if (m_hp <= 0)
				{
					//死亡ステートに遷移する
					m_BossState = enBossState_Down;

					//死亡SE
					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(18);
					se->SetVolume(DEAD_SE_VOLUME);
					se->Play(false);

					Game* game = FindGO<Game>("game");
					//倒されたエネミー数を＋1する
					game->AddDefeatedEnemyNumber();
				}
				else {
					//被ダメージステートに遷移する
					m_BossState = enBossState_ReceiveDamage;
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

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(DEAD_SE_VOLUME);
				se->Play(false);

				//HPが0になったら
				if (m_hp <= 0)
				{
					//死亡ステートに遷移する
					m_BossState = enBossState_Down;

					//死亡SE
					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(18);
					se->SetVolume(SE_VOLUME);
					se->Play(false);

					Game* game = FindGO<Game>("game");
					//倒されたエネミー数を＋1する
					game->AddDefeatedEnemyNumber();
				}
				else
				{
					//被ダメージステートに遷移
					m_BossState = enBossState_ReceiveDamage;
				}
				return;
			}
		}
	}
}

const bool Boss::SearchPlayer() const
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

const bool Boss::IsCanAttack() const
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

void Boss::HPBer()
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
	m_HPbar.SetScale(scale);

	m_HPbar.Update();
	m_HPframe.Update();
	m_HPback.Update();
}

void Boss::MakeAttackCollision()
{
	auto collisionObject = NewGO<CollisionObject>(0);
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	
	//ボックスコリジョンの作成
	collisionObject->CreateBox(m_position, Quaternion::Identity, ATTACK_COLLISION_SIZE);
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("Boss_attack");
}

void Boss::MakeSlashingEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Vector3 flont = Vector3::AxisZ;
	Quaternion rotation;

	effectEmitter->Init(3);
	effectEmitter->SetScale(Vector3::One * 15.0f);
	effectPosition.y += 50.0f;
	rotation = m_rotation;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);
	effectEmitter->Play();
}

void Boss::ReStart()
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

void Boss::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//キーの名前が「attack_start」のとき
	if (wcscmp(eventName, L"attack_start") == 0)
	{
		m_isUnderAttack = true;
	}
	//キーの名前が「attack_end」のとき
	if (wcscmp(eventName, L"attack_end") == 0)
	{
		m_isUnderAttack = false;
	}

	if (wcscmp(eventName, L"posget") == 0)
	{
		m_magicOldPos = m_player->GetPosition();

		//魔法陣エフェクト再生
		EffectEmitter* effect = NewGO<EffectEmitter>(0);
		effect->Init(5);
		Vector3 effectPosition = m_magicOldPos;
		effectPosition.y = FIRST_POSITION_Y;
		effectPosition.y += 1.0f;
		effect->SetPosition(effectPosition);
		effect->SetScale(Vector3::One * 50.0f);
		effect->Play();
	}

	if (wcscmp(eventName, L"call_out") == 0) {
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(10);
		se->SetVolume(SE_VOLUME);
		se->Play(false);
	}

	//キーの名前が「magic_attack」のとき
	if (wcscmp(eventName, L"magic_attack") == 0)
	{
		MakeMagic();
	}

	//死亡時のスローモーション
	if (wcscmp(eventName, L"slow_start") == 0)
	{
		g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, 20);
	}

	if (wcscmp(eventName, L"slow_end") == 0)
	{
		g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, 60);
	}
}

void Boss::ProcessCommonStateTransition()
{
	//各タイマーを初期化
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	//エネミーからプレイヤーに向かうベクトルを計算する
	Vector3 diff = m_player->GetPosition() - m_position;

	//プレイヤーを見つけたら
	if (SearchPlayer() == true)
	{
		//ベクトルを正規化する
		diff.Normalize();
		//移動速度を設定
		m_moveSpeed = diff * 150.0f;
		//攻撃できる距離なら
		if (IsCanAttack() == true)
		{
			//乱数によって、攻撃するか待機させるかを決定する
			int ram = rand() % 100;
			if (ram > 60)
			{
				//攻撃ステートに遷移
				m_BossState = enBossState_Attack;
				m_isUnderAttack = false;
				return;
			}
			else if (ram > 50)
			{
				if (m_BossState == enBossState_Magic)
				{
					//連続で魔法を撃たせないように
					//待機ステートに移行する
					m_BossState = enBossState_Idle;
					return;
				}
				else 
				{
					m_BossState = enBossState_Magic;
				}
				return;
			}
			else
			{
				//待機ステートに遷移
				m_BossState = enBossState_Idle;
				return;
			}
		}
		//攻撃できない距離だったら
		else
		{
			int ram = rand() % 100;
			if (ram > 50)
			{
				//追跡ステートに遷移
				m_BossState = enBossState_Chase;
				return;
			}
			else
			{
				if (m_BossState == enBossState_Magic)
				{
					//連続で魔法を撃たせないように
					//待機ステートに移行する
					m_BossState = enBossState_Chase;
					return;
				}
				else
				{
					m_BossState = enBossState_Magic;
					return;
				}
			}
		}
	}
	//プレイヤーを見つけられなければ
	else
	{
		//待機ステートに遷移
		m_BossState = enBossState_Idle;
		return;
	}
}

void Boss::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//待機時間がある程度経過したら
	if (m_idleTimer >= 1.0f)
	{
		//他のステートに遷移
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void Boss::ProcessChaseStateTransition()
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

void Boss::ProcessAttackStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessMagicStateTransition()
{
	//魔法攻撃アニメーションの再生が終わったら。
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//他のステートへ遷移する。
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessReceiveDamageStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//攻撃されたら距離関係なしに、追跡させる
		m_BossState = enBossState_Chase;
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		m_moveSpeed = diff * 250.0f;
	}
}

void Boss::ProcessDownStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
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

void Boss::ManageState()
{
	switch (m_BossState)
	{
		//待機ステートの時
	case enBossState_Idle:
		//待機ステートの遷移処理
		ProcessIdleStateTransition();
		break;
		//追跡ステートの時
	case enBossState_Chase:
		//追跡ステートの遷移処理
		ProcessChaseStateTransition();
		break;
		//攻撃ステートの時
	case enBossState_Attack:
		//攻撃ステートの遷移処理
		ProcessAttackStateTransition();
		break;
		//魔法ステートの時
	case enBossState_Magic:
		//魔法ステートの遷移処理
		ProcessMagicStateTransition();
		break;
		//被ダメージステートの時
	case enBossState_ReceiveDamage:
		//被ダメージステートの遷移処理
		ProcessReceiveDamageStateTransition();
		break;
		//死亡ステートの時
	case enBossState_Down:
		//死亡ステートの遷移処理
		ProcessDownStateTransition();
		break;
	}
}

void Boss::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_BossState)
	{
	//待機ステート
	case enBossState_Idle:
		//待機アニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.1f);
		break;
	//追跡ステート
	case enBossState_Chase:
		//走りアニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
	//攻撃ステート
	case enBossState_Attack:
		//攻撃アニメーション再生
		m_modelRender.SetAnimationSpeed(1.0f);
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.3f);
		break;
	//魔法ステート
	case enBossState_Magic:
		//魔法アニメーション再生
		m_modelRender.SetAnimationSpeed(1.0f);
		m_modelRender.PlayAnimation(enAnimationClip_Magic, 0.3f);
		break;
	//被ダメージステート
	case enBossState_ReceiveDamage:
		//被ダメージアニメーション再生
		m_modelRender.SetAnimationSpeed(0.9f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.3f);
		break;
	//死亡ステート
	case enBossState_Down:
		//死亡アニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Down);
		break;
	default:
		break;
	}
}

void Boss::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	if (m_player->GetBossRoomFlag())
	{
		if (m_player->GetSpriteFlag())
		{
			m_bossName.Draw(rc);
			m_HPback.Draw(rc);
			m_HPbar.Draw(rc);
			m_HPframe.Draw(rc);
		}
	}
}
