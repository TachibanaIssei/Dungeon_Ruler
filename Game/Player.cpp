#include "stdafx.h"
#include "Player.h"

#include "Game.h"
#include "GameCamera.h"
#include "Fade.h"
#include "MagicBall.h"
#include "MovingFloor.h"
#include "MovingFloorZ.h"
#include "SeesawFloor.h"
#include "CheckPoint.h"
#include "Fade.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

namespace
{
	const int PLAYER_MAXHP = 1;	//�v���C���[�̍ő�HP
	const int PLAYER_MAXMP = 100;	//�v���C���[�̍ő�MP
	const int PLAYER_LOSTMP = 33;	//���@���g�����Ƃ��Ɍ�������MP
	const int PLAYER_HEALMP = 5;	//MP�̎�����

	const float JUMP_POWER = 280.0f;	//�W�����v��

	const float SE_VOLUME = 1.0f;
	const float FIREHIT_SE_VOLUME = 0.8f;		//���M�~�b�N�̌��ʉ��{�����[��
	const float MUSHATTACK_SE_VOLUME = 1.2f;	//�}�b�V�����[���U���q�b�gSE�{�����[��
	const float DEAD_VOICE_VOLUME = 1.5f;		//���S���̐��̃{�����[��

	const int	LOST_HP_MUSH_ATTACK = 10;		//�}�b�V�����[���̍U���̃_���[�W
	const int	LOST_HP_STONE_ATTACK = 10;		//�X�g�[�������X�^�[�̍U���̃_���[�W
	const int	LOST_HP_BOSS_ATTACK = 20;		//�{�X�̋ߐڍU���̃_���[�W
	const int	LOST_HP_BOSS_MAGIC = 30;		//�{�X�̖��@�U���̃_���[�W
	const float LOST_HP_FIRE = 0.8f;			//���M�~�b�N�̃_���[�W

	const Vector2 GAUGE_PIVOT			= Vector2(0.0f, 0.5f);				//�Q�[�W�̃s�{�b�g
	const Vector3 STATUS_BAR_POS		= Vector3(-393.0f, -407.0f, 0.0f);	//�X�e�[�^�X�o�[�|�W�V����
	const Vector3 PLAYER_FACE_BAR_POS	= Vector3(-580.0, -418.0f, 0.0f);	//�v���C���[�̊�̘g�̃|�W�V����
	const Vector3 HP_BAR_POS			= Vector3(-726.0f, -375.0f, 0.0f);	//HP�o�[�|�W�V����
	const Vector3 MP_BAR_POS			= Vector3(-720.0, -446.0, 0.0f);	//MP�o�[�|�W�V����

}

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	//�A�j���[�V���������[�h����
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/player/playerIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/player/playerWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/player/playerRun.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Jump].Load("Assets/animData/player/playerJump.tka");
	m_animationClips[enAnimationClip_Jump].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/player/playerAttack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Magic].Load("Assets/animData/player/playerMagicAttack.tka");
	m_animationClips[enAnimationClip_Magic].SetLoopFlag(false);
	m_animationClips[enAnimationClip_ReceiveDamage].Load("Assets/animData/player/playerReceiveDamage.tka");
	m_animationClips[enAnimationClip_ReceiveDamage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/player/playerDown.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Clear].Load("Assets/animData/player/playerVictory.tka");
	m_animationClips[enAnimationClip_Clear].SetLoopFlag(true);

	//�L�����N�^�[���f���̓ǂݍ���
	m_modelRender.Init("Assets/modelData/player/player.tkm",m_animationClips,enAnimationClip_Num, enModelUpAxisZ);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);

	//���̃{�[����T��
	m_swordBoneId = m_modelRender.FindBoneID(L"mixamorig:Sword_joint");

	//HP�Q�[�W�̉摜��ǂݍ���
	m_statusBar.Init("Assets/sprite/HP_gauge.dds", 744, 244);
	m_statusBar.SetPosition(STATUS_BAR_POS);
	m_statusBar.SetScale(g_vec3One);

	m_playerFaceFrame.Init("Assets/sprite/HP_window.dds", 744, 244);
	m_playerFaceFrame.SetPosition(PLAYER_FACE_BAR_POS);
	m_playerFaceFrame.SetScale(g_vec3One);

	m_playerFaceBack.Init("Assets/sprite/HP_windowBack.dds", 744, 244);
	m_playerFaceBack.SetPosition(PLAYER_FACE_BAR_POS);
	m_playerFaceBack.SetScale(g_vec3One);

	//HP�Q�[�W�̉摜�ǂݍ���
	m_hpBar.Init("Assets/sprite/uf_bar_b_health.dds", 565,58.8);
	//�s�{�b�g��ݒ肷��
	m_hpBar.SetPivot(GAUGE_PIVOT);
	m_hpBar.SetPosition(HP_BAR_POS);

	//MP�Q�[�W�̉摜�ǂݍ���
	m_mpBar.Init("Assets/sprite/uf_bar_sm_mana.dds", 432.3, 34.06);
	//�s�{�b�g��ݒ肷��
	m_mpBar.SetPivot(GAUGE_PIVOT);
	m_mpBar.SetPosition(MP_BAR_POS);

	//�X�V����
	m_statusBar.Update();
	m_hpBar.Update();
	m_hp = PLAYER_MAXHP;
	m_mpBar.Update();
	m_mp = PLAYER_MAXMP;
	m_playerFaceFrame.Update();
	m_playerFaceBack.Update();

	//�A�j���[�V�����C�x���g�p�̊֐���ݒ肷��
	m_modelRender.AddAnimationEvent([&](const wchar_t*
		clipName, const wchar_t* eventName) {
			OnAnimationEvent(clipName, eventName);
		});

	m_charCon.Init(
		15.0f,
		35.0f,
		m_position
	);

	//�G�t�F�N�g�̃��[�h
	EffectEngine::GetInstance()->ResistEffect(0, u"Assets/effect/playerSword_slash.efk");
	EffectEngine::GetInstance()->ResistEffect(9, u"Assets/effect/efk/cast_wind.efk");
	EffectEngine::GetInstance()->ResistEffect(10, u"Assets/effect/magicfire_hit.efk");
	EffectEngine::GetInstance()->ResistEffect(10, u"Assets/effect/magicfire_hit.efk");
	
	//���̃��[�h
	g_soundEngine->ResistWaveFileBank(3,"Assets/sound/sword_slash_shan.wav");
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/magic_circle_sound.wav");
	g_soundEngine->ResistWaveFileBank(7, "Assets/sound/hitheavy_sound_bogo.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");
	g_soundEngine->ResistWaveFileBank(12, "Assets/sound/fire_hit_yakedo.wav");
	g_soundEngine->ResistWaveFileBank(14, "Assets/sound/otoko_voice_guha.wav");
	g_soundEngine->ResistWaveFileBank(16, "Assets/sound/button_sound_piroron.wav");
	g_soundEngine->ResistWaveFileBank(17, "Assets/sound/magic_huhatu.wav");

	m_game = FindGO<Game>("game");

	m_movingFloorX = FindGO<MovingFloor>("movingfloorX");
	m_checkPoint = FindGO<CheckPoint>("checkPoint");

	return true;
}

void Player::Update()
{
	FindObject();
	Move();
	LimitMove();
	Turn();
	Collision();
	Attack();
	PlayAnimation();
	ManageState();
	HPBar();
	MPBar();

	m_modelRender.Update();
}

void Player::FindObject()
{
	if (m_game == nullptr)
	{
		m_game = FindGO<Game>("game");
		return;
	}

	if (m_movingFloorX == nullptr)
	{
		m_movingFloorX = FindGO<MovingFloor>("movingfloorX");
		return;
	}
}

void Player::AddMoveSpeed()
{
	//X���ɓ������̏�ɂ����珰�̈ړ����x�����Z����
	if (m_movingFloorX->GetFlag() == true)
	{
		m_oldMoveSpeed += m_movingFloorX->GetMoveSpeed();
	}

	//Z���ɓ������̏�ɂ����珰�̈ړ����x�����Z����
	const auto& movingFloorZs = FindGOs<MovingFloorZ>("movingfloorZ");
	for (auto movingFloorZ : movingFloorZs)
	{
		if (movingFloorZ->GetFlag())
		{
			m_oldMoveSpeed += movingFloorZ->GetMoveSpeed();
		}
	}

	//��]���鏰�̏�ɂ�����
	const auto& seesawFloors = FindGOs<SeesawFloor>("seesawfloor");
	for (auto seesawFloor : seesawFloors)
	{
		if (seesawFloor->GetFlag())
		{
			if (seesawFloor->GetFloorNumber() == 1)
			{
				float rot = seesawFloor->GetRotation() * 500.0f;
				m_oldMoveSpeed.z += rot;
			}
			else if (seesawFloor->GetFloorNumber() == 2)
			{
				float rot = seesawFloor->GetRotation() * 550.0f;
				m_oldMoveSpeed.z += rot;
			}
		}
	}
}

void Player::Move()
{
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;
	m_oldMoveSpeed.x = m_moveSpeed.x;
	m_oldMoveSpeed.z = m_moveSpeed.z;

	//�ړ��ł��Ȃ���Ԃ̂Ƃ��́A�������Ȃ�
	if (IsEnableMove() == false)
	{
		m_moveSpeed.y -= 980.0f * 1.0f/60.0f;

		m_oldMoveSpeed = m_moveSpeed;

		AddMoveSpeed();

		m_position = m_charCon.Execute(m_oldMoveSpeed, 1.0f / 60.0f);

		if (m_charCon.IsOnGround()) {
			//�n�ʂɂ���
			m_moveSpeed.y = 0.0f;
		}

		m_modelRender.SetPosition(m_position);
		return;
	}

	if (g_pad[0]->IsPress(enButtonRB1))
	{
		m_playerSpeed = 200.0f;
	}
	else
	{
		m_playerSpeed = 120.0f;
	}

	//���X�e�B�b�N�̓��͗ʂ��󂯎��
	float lStick_x = g_pad[0]->GetLStickXF();
	float lStick_y = g_pad[0]->GetLStickYF();
	//�J�����̑O�����ƉE�������擾
	Vector3 cameraForward = g_camera3D->GetForward();
	Vector3 cameraRight = g_camera3D->GetRight();
	//XZ���ʂł̑O�������A�E�����ɕϊ�����
	cameraForward.y = 0.0f;
	cameraForward.Normalize();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();
	//XZ�����̈ړ����x���N���A
	m_moveSpeed += cameraForward * lStick_y * m_playerSpeed;
	m_moveSpeed += cameraRight * lStick_x * m_playerSpeed;
	
	if (g_pad[0]->IsTrigger(enButtonA)	//A�{�^���������ꂽ��
		&& m_charCon.IsOnGround()		//���A�n��ɂ�����
		) {
		//�W�����v����
		m_moveSpeed.y = JUMP_POWER;
	}

	m_moveSpeed.y -= 980.0f * g_gameTime->GetFrameDeltaTime();

	m_oldMoveSpeed = m_moveSpeed;

	AddMoveSpeed();

	//�L�����N�^�[�R���g���[���[���g�p���āA���W���X�V
	m_position = m_charCon.Execute(m_oldMoveSpeed, g_gameTime->GetFrameDeltaTime());
	
	if (m_charCon.IsOnGround()) {
		//�n�ʂɂ���
		m_moveSpeed.y = 0.0f;
	}

	Vector3 modelPosition = m_position;
	m_modelRender.SetPosition(modelPosition);
}

void Player::LimitMove()
{
	if (m_game->IsWannhilationZako())
	{
		if (m_position.z <= m_moveLimit[0].z)
		{
			m_position.z = m_moveLimit[0].z;
			m_charCon.SetPosition(m_position);
			m_modelRender.SetPosition(m_position);
		}
	}

	else if (!m_game->IsWannhilationZako() && m_mokutekiFlag < 3)
	{
		m_mokutekiFlag = 2;
	}

	if (m_bossRoomFlag == true)
	{
		if (m_position.z >= m_moveLimit[1].z)
		{
			m_position.z = m_moveLimit[1].z;
			m_charCon.SetPosition(m_position);
			m_modelRender.SetPosition(m_position);
		}
	}
}

void Player::Turn()
{
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001f) {
		return;
	}

	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);
	m_rotation.SetRotationY(-angle);

	//�}�b�v�\���p�̉�]
	float angleMap = atan2(m_moveSpeed.x, -m_moveSpeed.z);
	m_rotationZ.SetRotationZ(angleMap);

	m_modelRender.SetRotation(m_rotation);

	//�v���C���[�̐��ʃx�N�g�����v�Z����
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void Player::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	//�ߐڍU��
	if (wcscmp(eventName, L"attack_start") == 0)
	{
		m_isUnderAttack = true;
	}
	else if (wcscmp(eventName, L"attack_end") == 0)
	{
		m_isUnderAttack = false;
	}

	//���@�U��
	if (wcscmp(eventName, L"magic_attack") == 0)
	{
		//���@�U�����쐬
		MakeMagicBall();

	}
}

void Player::Collision()
{
	//��_���[�W�A�_�E�����̓_���[�W��������Ȃ�
	if (m_playerState == enPlayerState_ReceiveDamage ||
		m_playerState == enPlayerState_Down)
	{
		return;
	}

	{
		//�G�̍U���p�̃R���W�����̔z����擾����
		const auto& collisions =
			g_collisionObjectManager->FindCollisionObjects("mush_attack");
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����
			if (collision->IsHit(m_charCon))
			{
				//�U���ɏd�����ē�����Ȃ��悤�ɂ���
				if (m_mushAttackHit == true)
				{
					return;
				}

				m_mushAttackHit = true;

				//HP�����炷
				m_hp -= LOST_HP_MUSH_ATTACK;

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(7);
				se->SetVolume(MUSHATTACK_SE_VOLUME);
				se->Play(false);

				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//�_�E��������
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UI�̕\��
					m_spriteFlag = false;
				}
				//HP��0�ł͂Ȃ�������
				else
				{
					m_playerState = enPlayerState_ReceiveDamage;
				}
			}
		}
	}

	{
		//�G�̍U���p�̃R���W�����̔z����擾����
		const auto& collisions =
			g_collisionObjectManager->FindCollisionObjects("Boss_attack");
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����
			if (collision->IsHit(m_charCon))
			{
				//HP�����炷
				m_hp -= LOST_HP_BOSS_ATTACK;

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(3);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//�_�E��������
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UI�̕\��
					m_spriteFlag = false;
				}
				//HP��0�ł͂Ȃ�������
				else
				{
					m_playerState = enPlayerState_ReceiveDamage;
				}
			}
		}
	}

	{
		//�G�̃}�W�b�N�{�[���p�̃R���W�������擾����
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_magicball");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charCon))
			{
				//HP�����炷
				m_hp -= LOST_HP_STONE_ATTACK;

				//���@�U���q�b�g�G�t�F�N�g�̍Đ�
				MakeMagicHitEffect();

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//���S�X�e�[�g�ɑJ�ڂ���
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UI�̕\��
					m_spriteFlag = false;
				}
				else
				{
					//��_���[�W�X�e�[�g�ɑJ��
					m_playerState = enPlayerState_ReceiveDamage;
				}
				return;
			}
		}
	}

	{
		//�G�̃}�W�b�N�T���_�[�p�̃R���W�������擾����
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_magicThunder");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charCon))
			{
				//�U���ɏd�����ē�����Ȃ��悤�ɂ���
				if (m_thunderHit == true)
				{
					return;
				}

				m_thunderHit = true;

				//HP�����炷
				m_hp -= LOST_HP_BOSS_MAGIC;

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//���S�X�e�[�g�ɑJ�ڂ���
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UI�̕\��
					m_spriteFlag = false;
				}
				else
				{
					//��_���[�W�X�e�[�g�ɑJ��
					m_playerState = enPlayerState_ReceiveDamage;
				}
				return;
			}
		}
	}

	{
		//���M�~�b�N�̃R���W�������擾����
		const auto& collisions =
			g_collisionObjectManager->FindCollisionObjects("fire_gimmic");
		for (auto collision : collisions)
		{
			//�R���W�����ƃL�����R�����Փ˂�����
			if (collision->IsHit(m_charCon))
			{
				
				m_fireDamageTimer += g_gameTime->GetFrameDeltaTime();
				if (m_fireDamageTimer >= 0.5)
				{
					//HP�����炷
					m_hp -= LOST_HP_FIRE;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(12);
					se->SetVolume(FIREHIT_SE_VOLUME);
					se->Play(false);

					m_fireDamageTimer = 0.0f;
				}

				//HP��0�ɂȂ�����
				if (m_hp <= 0)
				{
					//�_�E��������
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UI�̕\��
					m_spriteFlag = false;
				}
			}
		}
	}

	//�`�F�b�N�|�C���g�ʉ߂����Ƃ��̏���
	PhysicsWorld::GetInstance()->ContactTest
		(m_charCon,[&](const btCollisionObject& contactObject)
		{
				//1�ڂ̃`�F�b�N�|�C���g�ʉ�
				if (m_checkPoint->m_physicsGhostObject[0].IsSelf(contactObject) == true && checkPointCount < 1)
				{
					m_respawnPosition = m_inRespawnPosition[0];
					checkPointCount = 1;
				}
				//2��
				else if (m_checkPoint->m_physicsGhostObject[1].IsSelf(contactObject) == true && checkPointCount < 2)
				{
					m_respawnPosition = m_inRespawnPosition[1];
					checkPointCount = 2;
				}
				//3��
				else if (m_checkPoint->m_physicsGhostObject[2].IsSelf(contactObject) == true && checkPointCount < 3)
				{
					m_respawnPosition = m_inRespawnPosition[2];
					checkPointCount = 3;
				}
				//4��
				else if (m_checkPoint->m_physicsGhostObject[3].IsSelf(contactObject) == true && checkPointCount < 4)
				{
					m_respawnPosition = m_inRespawnPosition[3];
					checkPointCount = 4;
				}
				//5��
				else if (m_checkPoint->m_physicsGhostObject[4].IsSelf(contactObject) == true && checkPointCount < 5)
				{
					m_respawnPosition = m_inRespawnPosition[4];
					checkPointCount = 5;
				}
				//6��
				else if (m_checkPoint->m_physicsGhostObject[5].IsSelf(contactObject) == true && checkPointCount < 6)
				{
					m_respawnPosition = m_inRespawnPosition[5];
					checkPointCount = 6;
					m_bossRoomFlag = true;
					m_mokutekiFlag = 3;
				}
		});
}

void Player::Attack()
{
	//�U�����łȂ��Ȃ�A���������Ȃ�
	if (m_playerState != enPlayerState_Attack)
	{
		return;
	}

	//�U�����蒆�ł����
	if (m_isUnderAttack == true)
	{
		MakeAttackCollision();
	}
}

void Player::MakeAttackCollision()
{
	//�R���W�����I�u�W�F�N�g���쐬
	auto collisionObject = NewGO<CollisionObject>(0);
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	collisionObject->CreateBox(
		m_position,
		Quaternion::Identity,
		Vector3(55.0f,10.0f,10.0f)
	);
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("player_attack");
}

void Player::MakeSlashingEffect()
{
	//�G�t�F�N�g
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Vector3 flont = Vector3::AxisZ;
	Quaternion rotation;

	effectEmitter->Init(0);
	effectEmitter->SetScale(Vector3::One * 20.0f);
	effectPosition.y += 50.0f;
	rotation = m_rotation;
	m_rotation.Apply(flont);
	flont *= 50.0f;
	effectPosition += flont;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);

	//SE
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(3);
	se->SetVolume(SE_VOLUME);

	//�Đ�
	effectEmitter->Play();
	se->Play(false);

}

void Player::MakeMagicHitEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Quaternion rotation;

	effectEmitter->Init(10);
	effectEmitter->SetScale(Vector3::One * 20.0f);
	effectPosition.y += 50.0f;
	rotation = m_rotation;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);
	effectEmitter->Play();
}

void Player::MakeMagicBall()
{
	//MP������������
	m_mp -= PLAYER_LOSTMP;

	//���@�U�����쐬����
	MagicBall* magicBall = NewGO<MagicBall>(0);
	Vector3 magicBallPosition = m_position;
	magicBallPosition.y += 40.0f;
	magicBall->SetPosition(magicBallPosition);
	magicBall->SetRotation(m_rotation);
	//�p�҂��v���C���[�ɂ���
	magicBall->SetEnMagician(MagicBall::enMagician_Player);
}

void Player::ProcessCommonStateTransition()
{
	//�G��S�ł�������
	if (m_game->IsWannhilationEnemy())
	{
		//BGM���N���ABGM�ɕς���
		m_game->ChangeClearBGM();
		//UI�������Ȃ��悤�ɂ���
		m_spriteFlag = false;
		//�{�X��HP�Q�[�W�������Ȃ��悤�ɂ���
		m_bossRoomFlag = false;

		//�v���C���[���J�����̌����ɂ���
		//�J�����Ɍ������x�N�g�������߂�
		GameCamera* gameCamera = FindGO<GameCamera>("gameCamera");
		gameCamera->SetStopCamera();

		Vector3 rotyou = m_position - gameCamera->GetPosition();
		rotyou.y = 0.0f;
		rotyou *= -1.0f;
		rotyou.Normalize();

		Quaternion qRot;
		qRot.SetRotationYFromDirectionXZ(rotyou);

		m_modelRender.SetRotation(qRot);
		m_modelRender.Update();

		m_game->SetGameClearSpriteFlag(true);

		//�N���A�X�e�[�g�Ɉڍs����
		m_playerState = enPlayerState_Clear;
		return;
	}

	//���ɗ������Ƃ��̏���
	if (m_position.y - m_respawnPosition.y <= -300.0f)
	{
		GameCamera* gameCamera = FindGO<GameCamera>("gameCamera");
		gameCamera->SetMoveTargetFlag();

		m_playerState = enPlayerState_Down;

		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(14);
		se->SetVolume(MUSHATTACK_SE_VOLUME);
		se->Play(false);

		//UI�̕\��
		m_spriteFlag = false;
		return;
	}

	//�v���C���[���W�����v����������
	if (m_charCon.IsJump() == true)
	{
		//�X�e�[�g�W�����v
		m_playerState = enPlayerState_Jump;
		return;
	}

	//B�{�^���������ꂽ��
	if (g_pad[0]->IsTrigger(enButtonB))
	{
		//�X�e�[�g�U��
		m_playerState = enPlayerState_Attack;
		m_isUnderAttack = false;
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonX))
	{
		//���݂�MP������MP��菭�Ȃ��ꍇ�͏������Ȃ�
		if (m_mp < PLAYER_LOSTMP)
		{
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(17);
			se->SetVolume(SE_VOLUME);
			se->Play(false);
			return;
		}

		//�X�e�[�g���@�U��
		m_playerState = enPlayerState_Magic;

		//���@�w�G�t�F�N�g�̍Đ�
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		Vector3 effectPosition = m_position;
		effectPosition.y += 1.0f;
		Quaternion rotation;

		effectEmitter->Init(9);
		effectEmitter->SetScale(Vector3::One * 7.0f);
		rotation = m_rotation;
		effectEmitter->SetPosition(effectPosition);
		effectEmitter->SetRotation(rotation);

		//���@�wSE
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(4);
		se->SetVolume(SE_VOLUME);

		effectEmitter->Play();
		se->Play(false);
		return;
	}

	//�X�e�B�b�N�̓��͂���������
	if (fabsf(m_moveSpeed.x) >= 0.001f
		|| fabsf(m_moveSpeed.z) >= 0.001f)
	{
		//RB�{�^���������ꂽ��
		if (g_pad[0]->IsPress(enButtonRB1))
		{
			//�X�e�[�g����
			m_playerState = enPlayerState_Run;
			return;
		}
		else
		{
			//�X�e�[�g����
			m_playerState = enPlayerState_Walk;
			return;
		}
	}
	//�X�e�B�b�N�̓��͂��Ȃ�������
	else
	{
		//�X�e�[�g�ҋ@
		m_playerState = enPlayerState_Idle;
		return;
	}
}

void Player::ProcessIdleStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessRunStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessAttackStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ProcessCommonStateTransition();
	}
}

void Player::ProcessMagicStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ProcessCommonStateTransition();
	}
}

void Player::ProcessJumpStateTransition()
{
		ProcessCommonStateTransition();
}

void Player::ProcessReceiveDamageStateTransition()
{
	//��_���[�W�A�j���[�V�����̍Đ����I�������
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//�X�e�[�g��J�ڂ���
		ProcessCommonStateTransition();
	}
}

void Player::ProcessDownStateTransition()
{
	//�_�E���A�j���[�V�����̍Đ������������
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		m_game->SetGameState(m_game->enGameState_PlayerDead);
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(16);
			se->SetVolume(SE_VOLUME);
			se->Play(false);

			//���X�|�[��������
			Fade* fade = FindGO<Fade>("fade");
			fade->StartFadeOut();
			m_game->SetisWaitRespown();
		}
	}
}

void Player::ProcessClearStateTransition()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(16);
		se->SetVolume(SE_VOLUME);
		se->Play(false);

		m_game->NotifyClear();
	}
}

void Player::HPBar()
{
	/*if (g_pad[0]->IsPress(enButtonUp))
	{
		m_hp += 1;
	}
	if (g_pad[0]->IsPress(enButtonDown))
	{
		m_hp -= 1;
	}*/

	//HP��0�����������
	if (m_hp < 0)
	{
		//HP��0�ɂ���
		m_hp = 0;
	}
	//HP���ő�HP����������
	if (m_hp > PLAYER_MAXHP)
	{
		//HP���ő�HP�ɂ���
		m_hp = PLAYER_MAXHP;
	}

	Vector3 scale = Vector3::One;
	scale.x = float(m_hp) / float(PLAYER_MAXHP);
	m_hpBar.SetScale(scale);
	m_hpBar.Update();
}

void Player::MPBar()
{
	//MP��0�����������
	if (m_mp < 0)
	{
		//MP��0�ɂ���
		m_mp = 0;
	}
	//MP���ő�MP����������
	if (m_mp > PLAYER_MAXMP)
	{
		//MP���ő�MP�ɂ���
		m_mp = PLAYER_MAXMP;
	}

	//MP���ő�MP�����������
	if (m_mp < PLAYER_MAXMP)
	{
		m_mpHealTimer += g_gameTime->GetFrameDeltaTime();

		//1�b���Ƃ�
		if (m_mpHealTimer >= 2.0f)
		{
			//MP���񕜂���
			m_mp += PLAYER_HEALMP;
			m_mpHealTimer = 0.0f;
		}
	}

	Vector3 scale = Vector3::One;
	scale.x = float(m_mp) / float(PLAYER_MAXMP);
	m_mpBar.SetScale(scale);
	m_mpBar.Update();
}

void Player::PlayerRespawn()
{
	m_position = m_respawnPosition;
	m_rotation = m_respawnRotation;
	m_moveSpeed = Vector3::Zero;
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_charCon.SetPosition(m_position);
	m_hp = PLAYER_MAXHP;
	m_mp = PLAYER_MAXMP;

	//UI�̕\��
	m_spriteFlag = true;

	ProcessCommonStateTransition();
}

void Player::ManageState()
{
	switch (m_playerState)
	{
	//�ҋ@�X�e�[�g�̎�
	case enPlayerState_Idle:
		ProcessIdleStateTransition();
		break;
	//�����X�e�[�g�̎�
	case enPlayerState_Walk:
		ProcessWalkStateTransition();
		break;
	//����X�e�[�g�̎�
	case enPlayerState_Run:
		ProcessRunStateTransition();
		break;
	//�W�����v�X�e�[�g�̎�
	case enPlayerState_Jump:
		ProcessJumpStateTransition();
		break;
	//�U���X�e�[�g�̎�
	case enPlayerState_Attack:
		ProcessAttackStateTransition();
		break;
	//���@�U���X�e�[�g�̎�
	case enPlayerState_Magic:
		ProcessMagicStateTransition();
		break;
	//��_���[�W�X�e�[�g�̎�
	case enPlayerState_ReceiveDamage:
		ProcessReceiveDamageStateTransition();
		break;
	//�_�E���X�e�[�g�̎�
	case enPlayerState_Down:
		ProcessDownStateTransition();
		break;
	case enPlayerState_Clear:
		ProcessClearStateTransition();
		break;
	}
}

void Player::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_playerState)
	{
	case enPlayerState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.1f);
		break;
	case enPlayerState_Walk:
		m_modelRender.SetAnimationSpeed(1.1f);
		m_modelRender.PlayAnimation(enAnimationClip_Walk, 0.1f);
		break;
	case enPlayerState_Run:
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
	case enPlayerState_Jump:
		m_modelRender.PlayAnimation(enAnimationClip_Jump, 0.2f);
		break;
	case enPlayerState_Attack:
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.4f);
		break;
	case enPlayerState_Magic:
		m_modelRender.SetAnimationSpeed(1.2f);
		m_modelRender.PlayAnimation(enAnimationClip_Magic, 0.4f);
		break;
	case enPlayerState_ReceiveDamage:
		m_modelRender.SetAnimationSpeed(0.8f);
		m_modelRender.PlayAnimation(enAnimationClip_ReceiveDamage, 0.4f);
		break;
	case enPlayerState_Down:
		m_modelRender.PlayAnimation(enAnimationClip_Down);
		break;
	case enPlayerState_Clear:
		m_modelRender.PlayAnimation(enAnimationClip_Clear);
		break;
	default:
		break;
	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	if (m_spriteFlag == true)
	{
		m_statusBar.Draw(rc);
		m_hpBar.Draw(rc);
		m_mpBar.Draw(rc);
		m_playerFaceBack.Draw(rc);
		m_playerFaceFrame.Draw(rc);
	}
}


