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
	const float SEARCH_LENGTH = 400.0f;			//�v���C���[�𔭌����鋗��
	const float MAGIC_HITEFFECT_SIZE = 30.0f;	//���@�U���q�b�g�G�t�F�N�g�̑傫��
	const float DEAD_EFFECT_SIZE = 20.0f;		//���S�G�t�F�N�g�̑傫��

	const float STONE_MAPSPRITE_WIDTH = 57.0f;	//�}�b�v�摜�̉��T�C�Y
	const float STONE_MAPSPRITE_HEIGHT = 61.0f;	//�}�b�v�摜�̏c�T�C�Y

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

	//�G�t�F�N�g�̃��[�h
	EffectEngine::GetInstance()->ResistEffect(4, u"Assets/effect/efk/cast_fire.efk");
	EffectEngine::GetInstance()->ResistEffect(11, u"Assets/effect/magicwind_hit.efk");
	EffectEngine::GetInstance()->ResistEffect(12, u"Assets/effect/enemy_dead.efk");

	//SE
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/magic_circle_sound.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");

	//�摜�̃��[�h
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

	//������������
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
	//�ǐՃX�e�[�g�łȂ��Ȃ�A�������Ȃ�
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
	//��_���[�W�A���邢�̓_�E���X�e�[�g�̎���
	//�����蔻������Ȃ�
	if (m_stoneState == enStoneState_ReceiveDamage ||
		m_stoneState == enStoneState_Down)
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
					m_stoneState = enStoneState_Down;
				}
				else {
					//��_���[�W�X�e�[�g�ɑJ�ڂ���
					m_stoneState = enStoneState_ReceiveDamage;
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
				//HP���P���炷
				m_hp -= LOST_HP_MAGIC;

				//���@�U���q�b�g�G�t�F�N�g�Đ�
				MakeMagicHitEffect();

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//���S�X�e�[�g�ɑJ�ڂ���
					m_stoneState = enStoneState_Down;
				}
				else
				{
					//��_���[�W�X�e�[�g�ɑJ��
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

void StoneMonster::MakeMagicBall()
{
	//���@�U�����쐬����
	MagicBall* magicBall = NewGO<MagicBall>(0);
	Vector3 magicBallPosition = m_position;
	magicBallPosition.y += 40.0f;
	magicBall->SetPosition(magicBallPosition);
	magicBall->SetRotation(m_rotation);
	//�p�҂�G�ɂ���
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
			if (ram > 80)
			{
				//�U���X�e�[�g�ɑJ��
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
				//�ҋ@�X�e�[�g�ɑJ��
				m_stoneState = enStoneState_Idle;
				return;
			}
		}
		//�U���ł��Ȃ�������������
		else
		{
			//�ǐՃX�e�[�g�ɑJ��
			m_stoneState = enStoneState_Chase;
			return;
		}
	}

	//�v���C���[���������Ȃ����
	else
	{
		//�ҋ@�X�e�[�g�ɑJ��
		m_stoneState = enStoneState_Idle;
		return;
	}
}

void StoneMonster::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//�ҋ@���Ԃ�������x�o�߂�����
	if (m_idleTimer >= 1.0f)
	{
		//���̃X�e�[�g�ɑJ��
		ProcessCommonStateTransition();
	}
}

void StoneMonster::ProcessRunStateTransition()
{
	ProcessCommonStateTransition();
}

void StoneMonster::ProcessChaseStateTransition()
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
		//�U�����ꂽ�狗���֌W�Ȃ��ɁA�ǐՂ�����
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
		//�|���ꂽ�G�l�~�[�����{1����
		game->AddDefeatedEnemyNumber();

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

const bool StoneMonster::IsCanAttack() const
{
	Vector3 diff = m_player->GetPosition() - m_position;
	//�G�l�~�[�ƃv���C���[�̋������߂�������
	if (diff.LengthSq() <= 300.0f * 300.0f)
	{
		//�U���ł���
		return true;
	}
	//�U���ł��Ȃ�
	return false;
}

void StoneMonster::HPBar()
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
	m_HPBar.SetScale(scale);

	Vector3 BerPosition = m_position;
	BerPosition.y += 130.0f;
	Vector3 OldBerPos = BerPosition;
	OldBerPos.y += 30.0f;

	//���W��ϊ�����
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBerPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPWindowPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBackPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_namePos, OldBerPos);

	//HP�o�[�摜�����񂹂ɕ\������
	Vector3 BerSizeSubtraction = HPBerSend(HP_BER_SIZE, scale);	//�摜�̌��̑傫��
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
	Vector3 hpBerSize = size;								//�摜�̌��̑傫��
	Vector3 changeBerSize = Vector3::Zero;					//�摜���X�P�[���ϊ��������Ƃ̑傫��
	Vector3 BerSizeSubtraction = Vector3::Zero;				//�摜�̌��ƕϊ���̍�

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

	//�}�b�v�ɕ\������͈͂ɓG��������B
	if (m_map->WorldPositionConvertToMapPosition(playerPosition, enemyPosition, mapPosition))
	{
		//�}�b�v�ɕ\������悤�ɐݒ肷��A
		m_isMapImage = true;
		//SpriteRender�ɍ��W��ݒ�B
		m_stoneMapSprite.SetPosition(mapPosition);
	}
	//�}�b�v�ɕ\������͈͂ɓG�����Ȃ�������B
	else
	{
		//�}�b�v�ɕ\�����Ȃ��B
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
		//���@�w�G�t�F�N�g�̍Đ�
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		Vector3 effectPosition = m_position;
		Quaternion rotation;

		effectEmitter->Init(4);
		effectEmitter->SetScale(Vector3::One * 7.0f);
		rotation = m_rotation;
		effectEmitter->SetPosition(effectPosition);
		effectEmitter->SetRotation(rotation);

		//���@�w���ʉ�
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(4);
		se->SetVolume(SE_VOLUME);

		effectEmitter->Play();
		se->Play(false);
	}

	else if (wcscmp(eventName, L"magic_attack") == 0)
	{
		//���@�U�����쐬
		MakeMagicBall();
	}
}

void StoneMonster::ManageState()
{
	switch (m_stoneState)
	{
		//�ҋ@�X�e�[�g�̎�
	case enStoneState_Idle:
		//�ҋ@�X�e�[�g�̑J�ڏ���
		ProcessIdleStateTransition();
		break;
		//�ǐՃX�e�[�g�̎�
	case enStoneState_Chase:
		//�ǐՃX�e�[�g�̑J�ڏ���
		ProcessChaseStateTransition();
		break;
		//�U���X�e�[�g�̎�
	case enStoneState_Magic:
		//�U���X�e�[�g�̑J�ڏ���
		ProcessMagicStateTransition();
		break;
		//��_���[�W�X�e�[�g�̎�
	case enStoneState_ReceiveDamage:
		//��_���[�W�X�e�[�g�̑J�ڏ���
		ProcessReceiveDamageTransition();
		break;
		//���S�X�e�[�g�̎�
	case enStoneState_Down:
		//���S�X�e�[�g�̑J�ڏ���
		ProcessDownStateTransition();
		break;
	}
}

void StoneMonster::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_stoneState)
	{
		//�ҋ@�X�e�[�g
	case enStoneState_Idle:
		//�ҋ@�A�j���[�V�����Đ�
		m_modelRender.PlayAnimation(enAnimationClip_Idle,0.1f);
		break;
		//�ǐՃX�e�[�g
	case enStoneState_Chase:
		//�����A�j���[�V�����Đ�
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
		//�U���X�e�[�g
	case enStoneState_Magic:
		//�U���A�j���[�V�����Đ�
		m_modelRender.SetAnimationSpeed(0.7f);
		m_modelRender.PlayAnimation(enAnimationClip_Magic, 0.2f);
		break;
		//��_���[�W�X�e�[�g
	case enStoneState_ReceiveDamage:
		//��_���[�W�A�j���[�V�����Đ�
		m_modelRender.SetAnimationSpeed(0.7f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage, 0.3f);
		break;
		//���S�X�e�[�g
	case enStoneState_Down:
		//���S�A�j���[�V�����Đ�
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
