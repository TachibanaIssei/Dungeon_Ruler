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
	const float SEARCH_LENGTH = 400.0f;		//�v���C���[�𔭌����鋗��
	const float FIRST_POSITION_Y = 261.564;
	const Vector3 ATTACK_COLLISION_SIZE = Vector3(100.0f, 50.0f, 50.0f);
	const float DEAD_EFFECT_SIZE = 30.0f;	//���S�G�t�F�N�g�̑傫��

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

//todo �A�j���[�V�����̒ǉ�
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

	//���̃{�[����T��
	m_swordBoneId = m_modelRender.FindBoneID(L"Bip001 R Hand");

	m_charCon.Init(
		35.0f,
		50.0f,
		m_position
	);

	//�G�t�F�N�g�̃��[�h
	EffectEngine::GetInstance()->ResistEffect(3, u"Assets/effect/efk/enemy_slash_01.efk");
	EffectEngine::GetInstance()->ResistEffect(5, u"Assets/effect/magic_circle.efk");
	EffectEngine::GetInstance()->ResistEffect(12, u"Assets/effect/enemy_dead.efk");

	//���̃��[�h
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");
	g_soundEngine->ResistWaveFileBank(10, "Assets/sound/sakebi_sound_kyuaa.wav");
	g_soundEngine->ResistWaveFileBank(18, "Assets/sound/attack_sound_kin.wav");

	//�摜�̃��[�h
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

	//������������
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
	//�ǐՃX�e�[�g�łȂ��Ȃ�A�������Ȃ�
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
	//�U���X�e�[�g�łȂ��Ȃ珈�������Ȃ�
	if (m_BossState != enBossState_Attack)
	{
		return;
	}

	//�U�����ł����
	if (m_isUnderAttack == true)
	{
		//�U���p�̃R���W�������쐬
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
	//��_���[�W�A���邢�̓_�E���X�e�[�g�̎���
	//�����蔻������Ȃ�
	if (m_BossState == enBossState_ReceiveDamage ||
		m_BossState == enBossState_Down)
	{
		return;
	}

	{
		//�v���C���[�̍U���p�̃R���W�������擾����
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_attack");
		//�R���W�����̔z���for���ŉ�
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����
			if (collision->IsHit(m_charCon))
			{
				m_player->isPlayerAttackHit();

				//HP�����炷
				m_hp -= LOST_HP_ATTACK;
				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//���S�X�e�[�g�ɑJ�ڂ���
					m_BossState = enBossState_Down;

					//���SSE
					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(18);
					se->SetVolume(DEAD_SE_VOLUME);
					se->Play(false);

					Game* game = FindGO<Game>("game");
					//�|���ꂽ�G�l�~�[�����{1����
					game->AddDefeatedEnemyNumber();
				}
				else {
					//��_���[�W�X�e�[�g�ɑJ�ڂ���
					m_BossState = enBossState_ReceiveDamage;
				}

				return;
			}
		}
	}

	{
		//�v���C���[�̃}�W�b�N�{�[���p�̃R���W�������擾����
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("player_magicball");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charCon))
			{
				//HP�����炷
				m_hp -= LOST_HP_MAGIC;

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(DEAD_SE_VOLUME);
				se->Play(false);

				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//���S�X�e�[�g�ɑJ�ڂ���
					m_BossState = enBossState_Down;

					//���SSE
					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(18);
					se->SetVolume(SE_VOLUME);
					se->Play(false);

					Game* game = FindGO<Game>("game");
					//�|���ꂽ�G�l�~�[�����{1����
					game->AddDefeatedEnemyNumber();
				}
				else
				{
					//��_���[�W�X�e�[�g�ɑJ��
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

	//�v���C���[�ɂ�����x�߂�������
	if (diff.LengthSq() <= SEARCH_LENGTH * SEARCH_LENGTH)
	{
		//�G�l�~�[����v���C���[�Ɍ������x�N�g���𐳋K������
		diff.Normalize();
		//�G�l�~�[�̐��ʂ̃x�N�g���ƁA�G�l�~�[����v���C���[�Ɍ�����
		//�x�N�g���̓���(cos��)�����߂�
		float cos = m_forward.Dot(diff);
		//����(cos��)����p�x(��)�����߂�
		float angle = acosf(cos);
		//�p�x(��)��180����菬�������
		if (angle <= (Math::PI / 180.0f) * 120.0f)
		{
			//�v���C���[��������
			return true;
		}
	}
	//�v���C���[���������Ȃ�����
	return false;
}

const bool Boss::IsCanAttack() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	//�G�l�~�[�ƃv���C���[�̋������߂�������
	if (diff.LengthSq() <= 100.0f * 100.0f)
	{
		//�U���ł���
		return true;
	}
	//�U���ł��Ȃ�
	return false;
}

void Boss::HPBer()
{
	//HP��0�����������
	if (m_hp < 0)
	{
		//HP��0�ɂ���
		m_hp = 0;
	}
	//HP���ő�HP����������
	if (m_hp > m_maxHP)
	{
		//HP���ő�HP�ɂ���
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
	
	//�{�b�N�X�R���W�����̍쐬
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
	//�L�[�̖��O���uattack_start�v�̂Ƃ�
	if (wcscmp(eventName, L"attack_start") == 0)
	{
		m_isUnderAttack = true;
	}
	//�L�[�̖��O���uattack_end�v�̂Ƃ�
	if (wcscmp(eventName, L"attack_end") == 0)
	{
		m_isUnderAttack = false;
	}

	if (wcscmp(eventName, L"posget") == 0)
	{
		m_magicOldPos = m_player->GetPosition();

		//���@�w�G�t�F�N�g�Đ�
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

	//�L�[�̖��O���umagic_attack�v�̂Ƃ�
	if (wcscmp(eventName, L"magic_attack") == 0)
	{
		MakeMagic();
	}

	//���S���̃X���[���[�V����
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
	//�e�^�C�}�[��������
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	//�G�l�~�[����v���C���[�Ɍ������x�N�g�����v�Z����
	Vector3 diff = m_player->GetPosition() - m_position;

	//�v���C���[����������
	if (SearchPlayer() == true)
	{
		//�x�N�g���𐳋K������
		diff.Normalize();
		//�ړ����x��ݒ�
		m_moveSpeed = diff * 150.0f;
		//�U���ł��鋗���Ȃ�
		if (IsCanAttack() == true)
		{
			//�����ɂ���āA�U�����邩�ҋ@�����邩�����肷��
			int ram = rand() % 100;
			if (ram > 60)
			{
				//�U���X�e�[�g�ɑJ��
				m_BossState = enBossState_Attack;
				m_isUnderAttack = false;
				return;
			}
			else if (ram > 50)
			{
				if (m_BossState == enBossState_Magic)
				{
					//�A���Ŗ��@���������Ȃ��悤��
					//�ҋ@�X�e�[�g�Ɉڍs����
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
				//�ҋ@�X�e�[�g�ɑJ��
				m_BossState = enBossState_Idle;
				return;
			}
		}
		//�U���ł��Ȃ�������������
		else
		{
			int ram = rand() % 100;
			if (ram > 50)
			{
				//�ǐՃX�e�[�g�ɑJ��
				m_BossState = enBossState_Chase;
				return;
			}
			else
			{
				if (m_BossState == enBossState_Magic)
				{
					//�A���Ŗ��@���������Ȃ��悤��
					//�ҋ@�X�e�[�g�Ɉڍs����
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
	//�v���C���[���������Ȃ����
	else
	{
		//�ҋ@�X�e�[�g�ɑJ��
		m_BossState = enBossState_Idle;
		return;
	}
}

void Boss::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//�ҋ@���Ԃ�������x�o�߂�����
	if (m_idleTimer >= 1.0f)
	{
		//���̃X�e�[�g�ɑJ��
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void Boss::ProcessChaseStateTransition()
{
	//�U���ł��鋗���Ȃ�
	if (IsCanAttack() == true)
	{
		//���̃X�e�[�g�ɑJ��
		ProcessCommonStateTransition();
		return;
	}
	m_chaseTimer += g_gameTime->GetFrameDeltaTime();
	//�ǐՎ��Ԃ�������x�o�߂�����
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
	//���@�U���A�j���[�V�����̍Đ����I�������B
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//���̃X�e�[�g�֑J�ڂ���B
		ProcessCommonStateTransition();
	}
}

void Boss::ProcessReceiveDamageStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//�U�����ꂽ�狗���֌W�Ȃ��ɁA�ǐՂ�����
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
		//���S�G�t�F�N�g���Đ�
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

		//���g���폜
		DeleteGO(this);
	}
}

void Boss::ManageState()
{
	switch (m_BossState)
	{
		//�ҋ@�X�e�[�g�̎�
	case enBossState_Idle:
		//�ҋ@�X�e�[�g�̑J�ڏ���
		ProcessIdleStateTransition();
		break;
		//�ǐՃX�e�[�g�̎�
	case enBossState_Chase:
		//�ǐՃX�e�[�g�̑J�ڏ���
		ProcessChaseStateTransition();
		break;
		//�U���X�e�[�g�̎�
	case enBossState_Attack:
		//�U���X�e�[�g�̑J�ڏ���
		ProcessAttackStateTransition();
		break;
		//���@�X�e�[�g�̎�
	case enBossState_Magic:
		//���@�X�e�[�g�̑J�ڏ���
		ProcessMagicStateTransition();
		break;
		//��_���[�W�X�e�[�g�̎�
	case enBossState_ReceiveDamage:
		//��_���[�W�X�e�[�g�̑J�ڏ���
		ProcessReceiveDamageStateTransition();
		break;
		//���S�X�e�[�g�̎�
	case enBossState_Down:
		//���S�X�e�[�g�̑J�ڏ���
		ProcessDownStateTransition();
		break;
	}
}

void Boss::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_BossState)
	{
	//�ҋ@�X�e�[�g
	case enBossState_Idle:
		//�ҋ@�A�j���[�V�����Đ�
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.1f);
		break;
	//�ǐՃX�e�[�g
	case enBossState_Chase:
		//����A�j���[�V�����Đ�
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
	//�U���X�e�[�g
	case enBossState_Attack:
		//�U���A�j���[�V�����Đ�
		m_modelRender.SetAnimationSpeed(1.0f);
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.3f);
		break;
	//���@�X�e�[�g
	case enBossState_Magic:
		//���@�A�j���[�V�����Đ�
		m_modelRender.SetAnimationSpeed(1.0f);
		m_modelRender.PlayAnimation(enAnimationClip_Magic, 0.3f);
		break;
	//��_���[�W�X�e�[�g
	case enBossState_ReceiveDamage:
		//��_���[�W�A�j���[�V�����Đ�
		m_modelRender.SetAnimationSpeed(0.9f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.3f);
		break;
	//���S�X�e�[�g
	case enBossState_Down:
		//���S�A�j���[�V�����Đ�
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
