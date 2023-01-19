#include "stdafx.h"
#include "MagicBall.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

namespace
{
	const float SE_VOLUME = 1.0f;
}

MagicBall::MagicBall()
{
}

MagicBall::~MagicBall()
{
	//エフェクトの再生を停止する。
	m_effectEmitter->Stop();

	DeleteGO(m_effectEmitter);
	DeleteGO(m_collisionObject);
}

bool MagicBall::Start()
{
	//エフェクトを読み込む。
	EffectEngine::GetInstance()->ResistEffect(7, u"Assets/effect/efk/magic_fire.efk");
	EffectEngine::GetInstance()->ResistEffect(8, u"Assets/effect/efk/magic_wind.efk");

	//SEを読み込む
	g_soundEngine->ResistWaveFileBank(5, "Assets/sound/magic_attack_wind.wav");
	g_soundEngine->ResistWaveFileBank(6, "Assets/sound/magic_attack_fire.wav");

	//移動速度を計算
	m_moveSpeed = Vector3::AxisZ;
	m_rotation.Apply(m_moveSpeed);
	m_position += m_moveSpeed * 50.0f;
	
	m_rotation.AddRotationDegY(360.0f);

	//コリジョンオブジェクトを作成する
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateSphere(m_position,
		Quaternion::Identity,
		35.0f * m_scale.z
	);

	//術者がプレイヤー
	if (m_enMagician == enMagician_Player)
	{
		m_moveSpeed *= 1200.0f;

		m_collisionObject->SetName("player_magicball");
		
		//エフェクトのオブジェクトを作成する。
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(8);
		m_effectEmitter->SetScale(m_scale * 10.0f);
		//エフェクトに回転を設定する
		m_effectEmitter->SetRotation(m_rotation);

		//SE
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(5);
		se->SetVolume(SE_VOLUME);

		m_effectEmitter->Play();
		se->Play(false);
	}
	//術者が敵
	else if (m_enMagician == enMagician_Enemy)
	{
		m_moveSpeed *= 800.0f;

		m_collisionObject->SetName("enemy_magicball");

		//エフェクトのオブジェクトを作成する。
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(7);
		m_effectEmitter->SetScale(m_scale * 20.0f);
		//エフェクトに回転を設定する
		m_effectEmitter->SetRotation(m_rotation);

		//SE
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(6);
		se->SetVolume(SE_VOLUME);

		m_effectEmitter->Play();
		se->Play(false);
	}

	//コリジョンオブジェクトが自動で削除されないようにする
	m_collisionObject->SetIsEnableAutoDelete(false);

	return true;
}

void MagicBall::Update()
{
	//座標を移動させる
	m_position += m_moveSpeed * (1.0f/60.0f);
	//エフェクトの座標を設定する。
	m_effectEmitter->SetPosition(m_position);
	//コリジョンオブジェクトに座標を設定する
	m_collisionObject->SetPosition(m_position);

	//タイマーを加算
	m_timer += g_gameTime->GetFrameDeltaTime();
	//タイマーが0.7以上になったら
	if (m_timer >= 0.3f)
	{
		//自身を削除する
		DeleteGO(this);
	}
}
