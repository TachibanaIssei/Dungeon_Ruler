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
	const float SEARCH_LENGTH = 400.0f;			//�v���C���[�𔭌����鋗��
	const float MAGIC_HITEFFECT_SIZE = 50.0f;	//���@�U���q�b�g�G�t�F�N�g�̑傫��
	const float DEAD_EFFECT_SIZE = 25.0f;		//���S�G�t�F�N�g�̑傫��

	const float MUSH_MAPSPRITE_WIDTH = 52.0f;	//�}�b�v�摜�̉��T�C�Y
	const float MUSH_MAPSPRITE_HEIGHT = 60.0f;	//�}�b�v�摜�̏c�T�C�Y

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

	//�G�t�F�N�g�̃��[�h
	EffectEngine::GetInstance()->ResistEffect(6, u"Assets/effect/mush_impact.efk");
	EffectEngine::GetInstance()->ResistEffect(11, u"Assets/effect/magicwind_hit.efk");
	EffectEngine::GetInstance()->ResistEffect(12, u"Assets/effect/enemy_dead.efk");

	//���̃��[�h
	g_soundEngine->ResistWaveFileBank(8, "Assets/sound/move_sound_munyu.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");

	//�摜�̃��[�h
	m_mushMapSprite.Init("Assets/sprite/mushroom_image.DDS", MUSH_MAPSPRITE_WIDTH, MUSH_MAPSPRITE_HEIGHT);

	m_HPBar.Init("Assets/sprite/zako_HP_bar.DDS", HP_BER_WIDTH, HP_BER_HEIGHT);
	//m_HPBar.SetPivot(PIVOT);

	m_HPBack.Init("Assets/sprite/zako_HP_background.DDS", HP_WINDOW_WIDTH, HP_WINDOW_HEIGHT);

	m_HPFrame.Init("Assets/sprite/HP_flame_mushroom.DDS", HP_WINDOW_WIDTH, HP_WINDOW_HEIGHT);

	m_player = FindGO<Player>("player");
	m_map = FindGO<Map>("map");
	m_gameCamera = FindGO<GameCamera>("gameCamera");
	//������������
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
	//�ǐՃX�e�[�g�łȂ��Ȃ�A�������Ȃ�
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
	//�U���X�e�[�g�łȂ��Ȃ珈�������Ȃ�
	if (m_mushState != enMushState_Attack)
	{
		return;
	}

	//�U�����ł����
	if (m_isUnderAttack == true)
	{
		//�U���p�̃R���W�������쐬
		MakeAttackCollision();
	}
}

void MushRoomMan::Collision()
{
	//��_���[�W�A���邢�̓_�E���X�e�[�g�̎���
	//�����蔻������Ȃ�
	if (m_mushState == enMushState_ReceiveDamage ||
		m_mushState == enMushState_Down)
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

				//HP��1���炷
				m_hp -= LOST_HP_ATTACK;
				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//���S�X�e�[�g�ɑJ�ڂ���
					m_mushState = enMushState_Down;
				}
				else {
					//��_���[�W�X�e�[�g�ɑJ�ڂ���
					m_mushState = enMushState_ReceiveDamage;
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
					m_mushState = enMushState_Down;
				}
				else
				{
					//��_���[�W�X�e�[�g�ɑJ��
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
	//�e�^�C�}�[��������
	m_idleTimer = 0.0f;
	m_chaseTimer = 0.0f;

	if (m_player->isPlayerDead())
	{
		m_mushState = enMushState_Idle;
		return;
	}

	//�G�l�~�[����v���C���[�Ɍ������x�N�g�����v�Z����
	Vector3 diff = m_player->GetPosition() - m_position;

	//�v���C���[����������
	if (SearchPlayer() == true)
	{
		//�x�N�g���𐳋K������
		diff.Normalize();
		//�ړ����x��ݒ�
		m_moveSpeed = diff * 130.0f;
		//�U���ł��鋗���Ȃ�
		if (IsCanAttack() == true)
		{
			//�����ɂ���āA�U�����邩�ҋ@�����邩�����肷��
			int ram = rand() % 100;
			if (ram > 70)
			{
				//�U���X�e�[�g�ɑJ��
				m_mushState = enMushState_Attack;
				m_isUnderAttack = false;
				m_player->ResetMushAttackHit();
				return;
			}
			else
			{
				//�ҋ@�X�e�[�g�ɑJ��
				m_mushState = enMushState_Idle;
				return;
			}
		}
		//�U���ł��Ȃ�������������
		else
		{
			//�ǐՃX�e�[�g�ɑJ��
			m_mushState = enMushState_Chase;
			return;
		}
	}

	//�v���C���[���������Ȃ����
	else
	{
		//�ҋ@�X�e�[�g�ɑJ��
		m_mushState = enMushState_Idle;
		return;
	}
}

void MushRoomMan::ProcessIdleStateTransition()
{
	m_idleTimer += g_gameTime->GetFrameDeltaTime();
	//�ҋ@���Ԃ�������x�o�߂�����
	if (m_idleTimer >= 1.0f)
	{
		//���̃X�e�[�g�ɑJ��
		ProcessCommonStateTransition();
	}
}

void MushRoomMan::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void MushRoomMan::ProcessChaseStateTransition()
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
		//�U�����ꂽ�狗���֌W�Ȃ��ɁA�ǐՂ�����
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

const bool MushRoomMan::IsCanAttack() const
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

	//���W��ϊ�����
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBerPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPWindowPos, BerPosition);
	g_camera3D->CalcScreenPositionFromWorldPosition(m_HPBackPos, BerPosition);

	//HP�o�[�摜�����񂹂ɕ\������
	Vector3 BerSizeSubtraction = HPBerSend(HP_BER_SIZE,scale);	//�摜�̌��̑傫��
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
	Vector3 hpBerSize = size;								//�摜�̌��̑傫��
	Vector3 changeBerSize = Vector3::Zero;					//�摜���X�P�[���ϊ��������Ƃ̑傫��
	Vector3 BerSizeSubtraction = Vector3::Zero;				//�摜�̌��ƕϊ���̍�

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

	//�}�b�v�ɕ\������͈͂ɓG��������B
	if (m_map->WorldPositionConvertToMapPosition(playerPosition, enemyPosition, mapPosition))
	{
		//�}�b�v�ɕ\������悤�ɐݒ肷��A
		m_isMapImage = true;
		//SpriteRender�ɍ��W��ݒ�B
		m_mushMapSprite.SetPosition(mapPosition);
	}
	//�}�b�v�ɕ\������͈͂ɓG�����Ȃ�������B
	else
	{
		//�}�b�v�ɕ\�����Ȃ��B
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
	//�L�[�l�[�����uattack_start�v�̂Ƃ�
	if (wcscmp(eventName, L"attack_start") == 0) {
		m_isUnderAttack = true;
	}
	else if (wcscmp(eventName, L"attack_end") == 0) {
		m_isUnderAttack = false;
	}
	
	if (wcscmp(eventName, L"moveSE") == 0) {
		//�������ʉ��Đ�
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
		//�ҋ@�X�e�[�g�̎�
	case enMushState_Idle:
		//�ҋ@�X�e�[�g�̑J�ڏ���
		ProcessIdleStateTransition();
		break;
		//�ǐՃX�e�[�g�̎�
	case enMushState_Chase:
		//�ǐՃX�e�[�g�̑J�ڏ���
		ProcessChaseStateTransition();
		break;
		//�U���X�e�[�g�̎�
	case enMushState_Attack:
		//�U���X�e�[�g�̑J�ڏ���
		ProcessAttackStateTransition();
		break;
		//��_���[�W�X�e�[�g�̎�
	case enMushState_ReceiveDamage:
		//��_���[�W�X�e�[�g�̑J�ڏ���
		ProcessReceiveDamageStateTransition();
		break;
		//���S�X�e�[�g�̎�
	case enMushState_Down:
		//���S�X�e�[�g�̑J�ڏ���
		ProcessDownStateTransition();
		break;
	}
}

void MushRoomMan::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_mushState)
	{
		//�ҋ@�X�e�[�g
	case enMushState_Idle:
		//�ҋ@�A�j���[�V�����Đ�
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//�ǐՃX�e�[�g
	case enMushState_Chase:
		//�����A�j���[�V�����Đ�
		m_modelRender.PlayAnimation(enAnimationClip_Walk);
		break;
		//�U���X�e�[�g
	case enMushState_Attack:
		//�U���A�j���[�V�����Đ�
		m_modelRender.SetAnimationSpeed(0.6f);
		m_modelRender.PlayAnimation(enAnimationClip_Attack);
		break;
		//��_���[�W�X�e�[�g
	case enMushState_ReceiveDamage:
		//��_���[�W�A�j���[�V�����Đ�
		m_modelRender.SetAnimationSpeed(0.9f);
		m_modelRender.PlayAnimation(enAnimationClip_Damage);
		break;
		//���S�X�e�[�g
	case enMushState_Down:
		//���S�A�j���[�V�����Đ�
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