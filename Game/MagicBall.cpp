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
	//�G�t�F�N�g�̍Đ����~����B
	m_effectEmitter->Stop();

	DeleteGO(m_effectEmitter);
	DeleteGO(m_collisionObject);
}

bool MagicBall::Start()
{
	//�G�t�F�N�g��ǂݍ��ށB
	EffectEngine::GetInstance()->ResistEffect(7, u"Assets/effect/efk/magic_fire.efk");
	EffectEngine::GetInstance()->ResistEffect(8, u"Assets/effect/efk/magic_wind.efk");

	//SE��ǂݍ���
	g_soundEngine->ResistWaveFileBank(5, "Assets/sound/magic_attack_wind.wav");
	g_soundEngine->ResistWaveFileBank(6, "Assets/sound/magic_attack_fire.wav");

	//�ړ����x���v�Z
	m_moveSpeed = Vector3::AxisZ;
	m_rotation.Apply(m_moveSpeed);
	m_position += m_moveSpeed * 50.0f;
	
	m_rotation.AddRotationDegY(360.0f);

	//�R���W�����I�u�W�F�N�g���쐬����
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateSphere(m_position,
		Quaternion::Identity,
		35.0f * m_scale.z
	);

	//�p�҂��v���C���[
	if (m_enMagician == enMagician_Player)
	{
		m_moveSpeed *= 1200.0f;

		m_collisionObject->SetName("player_magicball");
		
		//�G�t�F�N�g�̃I�u�W�F�N�g���쐬����B
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(8);
		m_effectEmitter->SetScale(m_scale * 10.0f);
		//�G�t�F�N�g�ɉ�]��ݒ肷��
		m_effectEmitter->SetRotation(m_rotation);

		//SE
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(5);
		se->SetVolume(SE_VOLUME);

		m_effectEmitter->Play();
		se->Play(false);
	}
	//�p�҂��G
	else if (m_enMagician == enMagician_Enemy)
	{
		m_moveSpeed *= 800.0f;

		m_collisionObject->SetName("enemy_magicball");

		//�G�t�F�N�g�̃I�u�W�F�N�g���쐬����B
		m_effectEmitter = NewGO <EffectEmitter>(0);
		m_effectEmitter->Init(7);
		m_effectEmitter->SetScale(m_scale * 20.0f);
		//�G�t�F�N�g�ɉ�]��ݒ肷��
		m_effectEmitter->SetRotation(m_rotation);

		//SE
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(6);
		se->SetVolume(SE_VOLUME);

		m_effectEmitter->Play();
		se->Play(false);
	}

	//�R���W�����I�u�W�F�N�g�������ō폜����Ȃ��悤�ɂ���
	m_collisionObject->SetIsEnableAutoDelete(false);

	return true;
}

void MagicBall::Update()
{
	//���W���ړ�������
	m_position += m_moveSpeed * (1.0f/60.0f);
	//�G�t�F�N�g�̍��W��ݒ肷��B
	m_effectEmitter->SetPosition(m_position);
	//�R���W�����I�u�W�F�N�g�ɍ��W��ݒ肷��
	m_collisionObject->SetPosition(m_position);

	//�^�C�}�[�����Z
	m_timer += g_gameTime->GetFrameDeltaTime();
	//�^�C�}�[��0.7�ȏ�ɂȂ�����
	if (m_timer >= 0.3f)
	{
		//���g���폜����
		DeleteGO(this);
	}
}
