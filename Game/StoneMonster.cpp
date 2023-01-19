#include "stdafx.h"
#include "StoneMonster.h"

#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "MagicBall.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

#include <time.h>
#include <stdlib.h>

namespace {
	const float SEARCH_LENGTH = 400.0f;			//プレイヤーを発見する距離
	const float MAGIC_HITEFFECT_SIZE = 30.0f;	//魔法攻撃ヒットエフェクトの大きさ
	const float DEAD_EFFECT_SIZE = 20.0f;		//死亡エフェクトの大きさ

	const float STONE_MAPSPRITE_WIDTH = 57.0f;	//マップ画像の横サイズ
	const float STONE_MAPSPRITE_HEIGHT = 61.0f;	//マップ画像の縦サイズ

	const float SE_VOLUME = 1.0f;

	const float HP_WINDOW_WIDTH = 1152.0f;
	const float HP_WINDOW_HEIGHT = 648.0f;
	const float HP_BER_WIDTH = 178.0f;
	const float HP_BER_HEIGHT = 22.0f;

	const Vector3 HP_BER_SIZE = Vector3(HP_BER_WIDTH, HP_BER_HEIGHT, 0.0f);

	const Vector3 NAME_A_POS = Vector3(-700.0f, 490.0f, 0.0f);
	const Vector3 NAME_B_POS = Vector3(700.0f, 490.0f, 0.0f);

	//const Vector2 PIVOT = Vector2(0.0f, 0.5f);

	const int LOST_HP_ATTACK = 2;
	const int LOST_HP_MAGIC = 1;
}

StoneMonster::~StoneMonster()
{
}

bool StoneMonster::Start()
{
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/stonemonster/StoneMonstorIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/stonemonster/StoneMonstorRun.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Magic].Load("Assets/animData/stonemonster/StoneMonstorAttack.tka");
	m_animationClips[enAnimationClip_Magic].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Damage].Load("Assets/animData/stonemonster/StoneMonstorDamage.tka");
	m_animationClips[enAnimationClip_Damage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/stonemonster/StoneMonstorDeath.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);

	m_modelRender.Init("Assets/modelData/stonemonster/StoneMonster.tkm", m_animationClips, enAnimationClip_Num);

	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);

	m_charCon.Init(
		35.0f,
		10.0f,
		m_position
	);

	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
		});

	m_player = FindGO<Player>("player");
	m_map = FindGO<Map>("map");

	//エフェクトのロード
	EffectEngine::GetInstance()->ResistEffect(4, u"Assets/effect/efk/cast_fire.efk");
	EffectEngine::GetInstance()->ResistEffect(11, u"Assets/effect/magicwind_hit.efk");
	EffectEngine::GetInstance()->ResistEffect(12, u"Assets/effect/enemy_dead.efk");

	//SE
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/magic_circle_sound.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");

	//画像のロード
	m_stoneMapSprite.Init("Assets/sprite/stonemonster_image.DDS", STONE_MAPSPRITE_WIDTH, STONE_MAPSPRITE_HEIGHT);

	m_HPBar.Init("Assets/sprite/zako_HP_bar.DDS", HP_BER_WIDTH, HP_BER_HEIGHT);
	//m_HPBar.SetPivot(PIVOT);

	m_HPBack.Init("Assets/sprite/zako_HP_background.DDS", HP_WINDOW_WIDTH, HP_WINDOW_HEIGHT);

	m_HPFrame.Init("Assets/sprite/zako_HP_waku.DDS", HP_WINDOW_WIDTH, HP_WINDOW_HEIGHT);

	m_nameA.Init("Assets/sprite/stonemonster_A.dds", 768, 432);
	m_nameA.SetPosition(NAME_A_POS);
	m_nameB.Init("Assets/sprite/stonemonster_B.dds", 768, 432);
	m_nameB.SetPosition(NAME_B_POS);

	m_nameA.Update();
	m_nameB.Update();

	//乱数を初期化
	srand((unsigned)time(NULL));
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);

	return true;
}

void StoneMonster::Update()
{
	Chase();
	Rotation();
	Collision();
	HPBar();
	MapMove();
	PlayAnimation();
	ManageState();

	m_modelRender.Update();
}


void StoneMonster::Chase()
{
	//追跡ステートでないなら、処理しない
	if (m_stoneState != enStoneState_Chase)
	{
		return;
	}

	m_moveSpeed.y = 0.0f;

	m_position = m_charCon.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//m_position.y = 35.0f;

	Vector3 modelPosition = m_position;
	m_modelRender.SetPosition(modelPosition);
}

void StoneMonster::Rotation()
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

void StoneMonster::Collision()
{
	//被ダメージ、あるいはダウンステートの時は
	//当たり判定をしない
	if (m_stoneState == enStoneState_ReceiveDamage ||
		m_stoneState == enStoneState_Down)
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
					m_stoneState = enStoneState_Down;
				}
				else {
					//被ダメージステートに遷移する
					m_stoneState = enStoneState_ReceiveDamage;
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
				//HPを１減らす
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
					m_stoneState = enStoneState_Down;
				}
				else
				{
					//被ダメージステートに遷移
					m_stoneState = enStoneState_ReceiveDamage;
				}
				return;
			}
		}
	}
}

const bool StoneMonster::SearchPlayer() const
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

void StoneMonster::MakeMagicBall()
{
	//魔法攻撃を作成する
	MagicBall* magicBall = NewGO<MagicBall>(0);
	Vector3 magicBallPosition = m_position;
	magicBallPosition.y += 40.0f;
	magicBall->SetPosition(magicBallPosition);
	magicBall->SetRotation(m_rotation);
	//術者を敵にする
	magicBall->SetEnMagician(MagicBall::enMagician_Enemy);
}

void StoneMonster::MakeMagicHitEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Quaternion rotation;

	effectEmitter->Init(11);
	effectEmitter->SetScale(Vector3::One * MAGIC_HITEFFECT_SIZE);
	effectPosition.y += 40.0f;
	rotation = m_rotation;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);
	effectEmitter->Play();
}

void StoneMonster::ProcessCommonStateTransition()
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
			if (ram > 80)
			{
				//攻撃ステートに遷移
				m_stoneState = enStoneState_Magic;
				return;
			}
			else if (ram > 40)
			{
				m_stoneState = enStoneState_Chase;
				return;
			}
			else
			{
				//待機ステートに遷移
				m_stoneState = enStoneState_Idle;
				return;
			}
		}
		//攻撃できない距離だったら
		else
		{
			//追跡ステートに遷移
			m_stoneState = enStoneState_Chase;
			return;
		}
	}

	//プレイヤーを見つけられなければ
	else
	{
		//待機ステートに遷移
		m_stoneState = enStoneState_Idle;
		return;
	}
}

void StoneMonster::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//待機時間がある程度経過したら
	if (m_idleTimer >= 1.0f)
	{
		//他のステートに遷移
		ProcessCommonStateTransition();
	}
}

void StoneMonster::ProcessRunStateTransition()
{
	ProcessCommonStateTransition();
}

void StoneMonster::ProcessChaseStateTransition()
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

void StoneMonster::ProcessMagicStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ProcessCommonStateTransition();
	}
}

void StoneMonster::ProcessReceiveDamageTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{			
		//攻撃されたら距離関係なしに、追跡させる
		m_stoneState = enStoneState_Chase;
		Vector3 diff = m_player->GetPosition() - m_position;
		diff.Normalize();
		m_moveSpeed = diff * 250.0f;
	}
}

void StoneMonster::ProcessDownStateTransition()
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

const bool StoneMonster::IsCanAttack() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	//エネミーとプレイヤーの距離が近かったら
	if (diff.LengthSq() <= 300.0f * 300.0f)
	{
		//攻撃できる
		return true;
	}
	//攻撃できない
	return false;
}

void StoneMonster::HPBar()
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
	Vector3 OldBerPos = BerPosition;
	OldBerPos.y += 30.0f;

	//座標を変換する
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBerPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPWindowPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBackPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_namePos, OldBerPos);

	//HPバー画像を左寄せに表示する
	Vector3 BerSizeSubtraction = HPBerSend(HP_BER_SIZE, scale);	//画像の元の大きさ
	m_HPBerPos.x -= BerSizeSubtraction.x;

	m_HPBar.SetPosition(Vector3(m_HPBerPos.x, m_HPBerPos.y, 0.0f));
	m_HPFrame.SetPosition(Vector3(m_HPWindowPos.x, m_HPWindowPos.y, 0.0f));
	m_HPBack.SetPosition(Vector3(m_HPBackPos.x, m_HPBackPos.y, 0.0f));
	m_nameA.SetPosition(Vector3(m_namePos.x, m_namePos.y, 0.0f));
	m_nameB.SetPosition(Vector3(m_namePos.x, m_namePos.y, 0.0f));

	m_HPBar.Update();
	m_HPFrame.Update();
	m_HPBack.Update();
	m_nameA.Update();
	m_nameB.Update();
}

Vector3 StoneMonster::HPBerSend(Vector3 size, Vector3 scale)
{
	Vector3 hpBerSize = size;								//画像の元の大きさ
	Vector3 changeBerSize = Vector3::Zero;					//画像をスケール変換したあとの大きさ
	Vector3 BerSizeSubtraction = Vector3::Zero;				//画像の元と変換後の差

	changeBerSize.x = hpBerSize.x * scale.x;
	BerSizeSubtraction.x = hpBerSize.x - changeBerSize.x;
	BerSizeSubtraction.x /= 2.0f;

	return BerSizeSubtraction;
}

void StoneMonster::MapMove()
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
		m_stoneMapSprite.SetPosition(mapPosition);
	}
	//マップに表示する範囲に敵がいなかったら。
	else
	{
		//マップに表示しない。
		m_isMapImage = false;
	}

	m_stoneMapSprite.Update();
}

void StoneMonster::ReStart()
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

void StoneMonster::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	if (wcscmp(eventName, L"magic_start") == 0) {
		//魔法陣エフェクトの再生
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		Vector3 effectPosition = m_position;
		Quaternion rotation;

		effectEmitter->Init(4);
		effectEmitter->SetScale(Vector3::One * 7.0f);
		rotation = m_rotation;
		effectEmitter->SetPosition(effectPosition);
		effectEmitter->SetRotation(rotation);

		//魔法陣効果音
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(4);
		se->SetVolume(SE_VOLUME);

		effectEmitter->Play();
		se->Play(false);
	}

	else if (wcscmp(eventName, L"magic_attack") == 0)
	{
		//魔法攻撃を作成
		MakeMagicBall();
	}
}

void StoneMonster::ManageState()
{
	switch (m_stoneState)
	{
		//待機ステートの時
	case enStoneState_Idle:
		//待機ステートの遷移処理
		ProcessIdleStateTransition();
		break;
		//追跡ステートの時
	case enStoneState_Chase:
		//追跡ステートの遷移処理
		ProcessChaseStateTransition();
		break;
		//攻撃ステートの時
	case enStoneState_Magic:
		//攻撃ステートの遷移処理
		ProcessMagicStateTransition();
		break;
		//被ダメージステートの時
	case enStoneState_ReceiveDamage:
		//被ダメージステートの遷移処理
		ProcessReceiveDamageTransition();
		break;
		//死亡ステートの時
	case enStoneState_Down:
		//死亡ステートの遷移処理
		ProcessDownStateTransition();
		break;
	}
}

void StoneMonster::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_stoneState)
	{
		//待機ステート
	case enStoneState_Idle:
		//待機アニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Idle,0.1f);
		break;
		//追跡ステート
	case enStoneState_Chase:
		//歩きアニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
		//攻撃ステート
	case enStoneState_Magic:
		//攻撃アニメーション再生
		m_modelRender.SetAnimationSpeed(0.7f);
		m_modelRender.PlayAnimation(enAnimationClip_Magic, 0.2f);
		break;
		//被ダメージステート
	case enStoneState_ReceiveDamage:
		//被ダメージアニメーション再生
		m_modelRender.SetAnimationSpeed(0.7f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.3f);
		break;
		//死亡ステート
	case enStoneState_Down:
		//死亡アニメーション再生
		m_modelRender.PlayAnimation(enAnimationClip_Down);
		break;
	default:
		break;
	}
}

void StoneMonster::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	if (m_player->GetSpriteFlag())
	{
		m_HPBack.Draw(rc);
		m_HPBar.Draw(rc);
		m_HPFrame.Draw(rc);

		if (m_nameNumber == 1)
		{
			//m_nameA.Draw(rc);
		}
		else if (m_nameNumber == 2)
		{
			//m_nameB.Draw(rc);
		}
	}
}
