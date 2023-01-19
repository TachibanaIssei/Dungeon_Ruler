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
	//�G�t�F�N�g��ǂݍ���
	EffectEngine::GetInstance()->ResistEffect(2, u"Assets/effect/magic_Thunder.efk");
	//�G�t�F�N�g�̃I�u�W�F�N�g�̍쐬
	m_effectEmitter = NewGO<EffectEmitter>(0);
	m_effectEmitter->Init(2);
	//�G�t�F�N�g�̑傫����ݒ�
	m_effectEmitter->SetScale(m_scale * 4.0f);

	m_player = FindGO<Player>("player");
	Boss* boss = FindGO<Boss>("boss");


	m_position = boss->GetOldPos();
	m_position.y = 261.564f;

	//���ʉ�
	g_soundEngine->ResistWaveFileBank(11, "Assets/sound/magic_thunder_don.wav");
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(11);
	se->SetVolume(THUNDERSE_VOLUME);

	//�Đ�
	m_effectEmitter->SetPosition(m_position);
	m_effectEmitter->Play();
	se->Play(false);

	//�R���W�����I�u�W�F�N�g���쐬
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

	//�R���W�����I�u�W�F�N�g�������ō폜����Ȃ��悤�ɂ���
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
