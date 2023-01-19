#include "stdafx.h"
#include "MagicThunder.h"

#include "Player.h"
#include "Boss.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

namespace {
	const float THUNDERSE_VOLUME = 1.0f;
}

MagicThunder::MagicThunder()
{
}

MagicThunder::~MagicThunder()
{
	DeleteGO(m_effectEmitter);
	DeleteGO(m_collisionObject);
}

bool MagicThunder::Start()
{
	//エフェクトを読み込む
	EffectEngine::GetInstance()->ResistEffect(2, u"Assets/effect/magic_Thunder.efk");
	//エフェクトのオブジェクトの作成
	m_effectEmitter = NewGO<EffectEmitter>(0);
	m_effectEmitter->Init(2);
	//エフェクトの大きさを設定
	m_effectEmitter->SetScale(m_scale * 4.0f);

	m_player = FindGO<Player>("player");
	Boss* boss = FindGO<Boss>("boss");


	m_position = boss->GetOldPos();
	m_position.y = 261.564f;

	//効果音
	g_soundEngine->ResistWaveFileBank(11, "Assets/sound/magic_thunder_don.wav");
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(11);
	se->SetVolume(THUNDERSE_VOLUME);

	//再生
	m_effectEmitter->SetPosition(m_position);
	m_effectEmitter->Play();
	se->Play(false);

	//コリジョンオブジェクトを作成
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateCapsule(m_position,
		Quaternion::Identity,
		30.0f * m_scale.z,
		100.0f);

	m_collisionObject->SetPosition(m_position);

	if (m_enMagician == enMagician_Player)
	{
		m_collisionObject->SetName("player_magicThunder");
	}
	else if (m_enMagician == enMagician_Enemy)
	{
		m_collisionObject->SetName("enemy_magicThunder");
	}

	//コリジョンオブジェクトが自動で削除されないようにする
	m_collisionObject->SetIsEnableAutoDelete(false);

	m_player->ResetThunderHit();

	return true;
}

void MagicThunder::Update()
{
	m_effectEmitter->SetPosition(m_position);
	m_collisionObject->SetPosition(m_position);

	m_timer += g_gameTime->GetFrameDeltaTime();

	if (m_timer > 1.0f)
	{
		DeleteGO(this);
	}
}
