#pragma once

class Game;
class Fade;
class MovingFloor;
class CheckPoint;

class Player : public IGameObject
{
public:
	//�v���C���[�̃X�e�[�g
	enum EnPlayerState {
		enPlayerState_Idle,			//�ҋ@���
		enPlayerState_Walk,			//����
		enPlayerState_Run,			//����
		enPlayerState_Jump,			//�W�����v
		enPlayerState_Attack,		//�ߐڍU��
		enPlayerState_Magic,		//���@�U��
		enPlayerState_ReceiveDamage,//��_���[�W
		enPlayerState_Down,			//�_�E��
		enPlayerState_Clear			//�N���A
	};

	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// ���W��ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
		m_respawnPosition = position;
		m_modelRender.SetPosition(m_position);
	}

	/// <summary>
	/// ���W���擾
	/// </summary>
	/// <returns>���W</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}

	/// <summary>
	/// ��]���擾
	/// </summary>
	/// <returns>��]</returns>
	const Quaternion& GetRotation() const
	{
		return m_rotationZ;
	}

	/// <summary>
	/// �L�����R�����擾
	/// </summary>
	/// <returns>�L�����N�^�[�R���g���[���[</returns>
	CharacterController& GetCharacterController()
	{
		return m_charCon;
	}

	/// <summary>
	/// ��]��ݒ�
	/// </summary>
	/// <param name="rotation">��]</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_respawnRotation = rotation;
		m_modelRender.SetRotation(m_rotation);
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
		m_modelRender.SetScale(m_scale);
	}

	/// <summary>
	/// ���X�|�[���|�W�V������ݒ�
	/// </summary>
	/// <param name="position">���X�|�[���|�W�V����</param>
	/// <param name="i">�z��̔ԍ�</param>
	void SetinRespawnPosition(const Vector3& position,int i)
	{
		m_inRespawnPosition[i] = position;
	}

	/// <summary>
	/// �s�������̍��W��ݒ�
	/// </summary>
	/// <param name="position">�͈͂̍��W</param>
	/// <param name="i">�z��̔ԍ�</param>
	void SetinMoveLimitPosition(const Vector3& position, int i)
	{
		m_moveLimit[i] = position;
	}

	/// <summary>
	/// ��������(�X�e�[�g)���ǂ������擾
	/// </summary>
	/// <returns>�������ԂȂ�true</returns>
	bool IsEnableMove() const
	{
		return m_playerState != enPlayerState_Attack &&
			m_playerState != enPlayerState_Magic &&
			m_playerState != enPlayerState_ReceiveDamage &&
			m_playerState != enPlayerState_Down &&
			m_playerState != enPlayerState_Clear;
	}

	/// <summary>
	/// �v���C���[���X�|�[��
	/// </summary>
	void PlayerRespawn();

	/// <summary>
	/// �T���_�[���@�̓�����t���O�����Z�b�g����
	/// </summary>
	void ResetThunderHit()
	{
		m_thunderHit = false;
	}

	/// <summary>
	/// �}�b�V�����[���̍U���̓�����t���O�����Z�b�g����
	/// </summary>
	void ResetMushAttackHit()
	{
		m_mushAttackHit = false;
	}

	/// <summary>
	/// �v���C���[�̋ߐڍU���̃G�t�F�N�g���Đ�����
	/// </summary>
	void isPlayerAttackHit()
	{
		MakeSlashingEffect();
	}

	/// <summary>
	/// �v���C���[�����S���Ă���H
	/// </summary>
	/// <returns>HP��0�ȉ��Ȃ�true</returns>
	const bool isPlayerDead() const
	{
		return m_hp <= 0;
	}

	/// <summary>
	/// �摜�\���t���O���擾����
	/// </summary>
	/// <returns>false�Ȃ�\�����Ȃ�</returns>
	const bool GetSpriteFlag() const
	{
		return m_spriteFlag;
	}

	/// <summary>
	/// �{�X�����t���O���擾����
	/// </summary>
	/// <returns>false�Ȃ�\�����Ȃ�</returns>
	const bool GetBossRoomFlag() const
	{
		return m_bossRoomFlag;
	}

	/// <summary>
	/// �ړI�̔ԍ����擾����
	/// </summary>
	/// <returns>�ړI�̔ԍ�(1.���ׂĂ̓G��|��,2.���ɐi��,3.�x�z�҂�|��)</returns>
	const int GetMokutekiFlag()
	{
		return m_mokutekiFlag;
	}

private:
	/// <summary>
	/// �I�u�W�F�N�g��������
	/// </summary>
	void FindObject();
	/// <summary>
	/// �������̈ړ����x�����Z����
	/// </summary>
	void AddMoveSpeed();
	/// <summary>
	/// �ړ�����
	/// </summary>
	void Move();
	/// <summary>
	/// �s������
	/// </summary>
	void LimitMove();
	/// <summary>
	/// ��]����
	/// </summary>
	void Turn();
	/// <summary>
	/// �A�j���[�V�����̍Đ�
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// �e�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ManageState();
	/// <summary>
	/// �A�j���[�V�����C�x���g�p�̊֐�
	/// </summary>
	/// <param name="clipName">�A�j���[�V�����̖��O</param>
	/// <param name="eventName">�A�j���[�V�����C�x���g�L�[�̖��O</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// �G�Ƃ̓����蔻�菈��
	/// </summary>
	void Collision();
	/// <summary>
	/// �U�����̏���
	/// </summary>
	void Attack();
	/// <summary>
	/// �U���̍ۂ̓����蔻��p�̃R���W�������쐬����
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// �U�������ۂ̃G�t�F�N�g���쐬����
	/// </summary>
	void MakeSlashingEffect();
	/// <summary>
	/// ���@�U���q�b�g�̃G�t�F�N�g���Đ�����
	/// </summary>
	void MakeMagicHitEffect();
	/// <summary>
	/// ���@�U�����쐬����
	/// </summary>
	void MakeMagicBall();
	/// <summary>
	/// ���ʂ̃X�e�[�g�J�ڏ���
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// �ҋ@�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// �����X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessWalkStateTransition();
	/// <summary>
	/// ����X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// �ߐڍU���X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// ���@�U���X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessMagicStateTransition();
	/// <summary>
	/// �W�����v�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessJumpStateTransition();
	/// <summary>
	/// ��_���[�W�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// ���S�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// �N���A�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessClearStateTransition();
	/// <summary>
	/// HP�o�[�̕\��
	/// </summary>
	void HPBar();
	/// <summary>
	/// MP�o�[�̕\��
	/// </summary>
	void MPBar();

	//�A�j���[�V����
	enum EnAnimationClip {
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Run,
		enAnimationClip_Jump,
		enAnimationClip_Attack,
		enAnimationClip_Magic,
		enAnimationClip_ReceiveDamage,
		enAnimationClip_Down,
		enAnimationClip_Clear,
		enAnimationClip_Num,
	};

	Game*			m_game = nullptr;
	Fade*			m_fade = nullptr;
	MovingFloor*	m_movingFloorX		= nullptr;
	CheckPoint*		m_checkPoint		= nullptr;

	AnimationClip			m_animationClips[enAnimationClip_Num];							//�A�j���[�V�����N���b�v
	ModelRender				m_modelRender;													//���f�������_�[
	Vector3					m_position								= Vector3::Zero;		//���W
	Vector3					m_respawnPosition						= Vector3::Zero;		//���X�|�[�����W
	Vector3					m_inRespawnPosition[6];											//6�̃��X�|�[���|�W�V�������Ǘ�����
	Vector3					m_moveLimit[2]			= {Vector3::Zero,Vector3::Zero};		//��������ړ��͈͂�ێ�
	Vector3					m_moveSpeed								= Vector3::Zero;		//�ړ����x
	Vector3					m_oldMoveSpeed							= Vector3::Zero;		//�ړ����x��ێ�����
	Vector3					m_movingFloorSpeed						= Vector3::Zero;		//�������̈ړ�������������ϐ�
	Vector3					m_forward								= Vector3::AxisZ;		//�v���C���[�̐��ʃx�N�g��
	Vector3					m_scale									= Vector3::One;			//�傫��
	Quaternion				m_rotation								= Quaternion::Identity;	//�N�H�[�^�j�I��
	Quaternion				m_rotationZ								= Quaternion::Identity;	//Z������̃N�H�[�^�j�I��
	Quaternion				m_respawnRotation						= Quaternion::Identity;	//���X�|�[���N�H�[�^�j�I��
	CharacterController		m_charCon;														//�L�����N�^�[�R���g���[���[
	EnPlayerState			m_playerState							= enPlayerState_Idle;	//�v���C���[�X�e�[�g
	SpriteRender			m_hpBar;														//HP�o�[�̉摜
	SpriteRender			m_mpBar;														//MP�o�[�̉摜
	SpriteRender			m_statusBar;													//�X�e�[�^�X�̉摜
	SpriteRender			m_playerFaceFrame;												//�v���C���[�̊�摜�̘g
	SpriteRender			m_playerFaceBack;												//�v���C���[�̊�摜�̔w�i
	SpriteRender			m_playerFace;													//�v���C���[�̊�
	int						m_swordBoneId							= -1;					//���̃{�[��ID
	int						m_hp = 0;														//���݂̃v���C���[HP
	int						m_mp = 0;														//���݂̃v���C���[MP
	int						checkPointCount							= 0;					//�ʉ߂����`�F�b�N�|�C���g�̐��𐔂���
	int						m_mokutekiFlag							= 1;					//�ړI�摜�̔ԍ�
	bool					m_isUnderAttack							= false;				//�U���Ȃ�true
	bool					m_onMovingFloor							= false;				//�������ɂ�����true
	bool					m_thunderHit							= false;				//�T���_�[���@������������true
	bool					m_mushAttackHit							= false;				//�}�b�V���̍U��������������true
	bool					m_bossRoomFlag							= false;				//�{�X�̂���ꏊ�ɍs����true
	bool					m_spriteFlag							= true;					//�N���A������false
	float					m_playerSpeed							= 120.0f;				//�v���C���[�̈ړ����x�ύX
	float					m_mpHealTimer							= 0.0f;					//MP�񕜂̎��Ԍv��
	float					m_fireDamageTimer						= 0.0f;					//���M�~�b�N�_���[�W�̃N�[���^�C���p�̃^�C�}�[
};

