#pragma once

class Player;

class Boss : public IGameObject
{
public:
	//�{�X�X�e�[�g
	enum EnBossState {
		enBossState_Idle,
		enBossState_Chase,
		enBossState_Attack,
		enBossState_Magic,
		enBossState_ReceiveDamage,
		enBossState_Down,
	};

	Boss();
	~Boss();
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
		m_firstPosition = position;
		m_modelRender.SetPosition(m_position);
	}
	/// <summary>
	/// ���W���擾����
	/// </summary>
	/// <returns>���W</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// HP��ݒ�
	/// </summary>
	/// <param name="hp">HP</param>
	void SetHP(const int hp)
	{
		m_hp = hp;
		m_maxHP = hp;
	}
	/// <summary>
	/// ��]��ݒ�
	/// </summary>
	/// <param name="rotation">��]</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_firstRotation = rotation;
		m_modelRender.SetRotation(m_rotation);
	}
	/// <summary>
	/// �傫����ݒ�
	/// </summary>
	/// <param name="scale">�傫��</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
		m_modelRender.SetScale(m_scale);
	}

	/// <summary>
	/// ���@�U�����s�����W���擾����
	/// </summary>
	/// <returns>���@�̍��W</returns>
	const Vector3& GetOldPos() const
	{
		return m_magicOldPos;
	}

	/// <summary>
	/// �L�����R�����擾
	/// </summary>
	/// <returns>�L�����N�^�[�R���g���[���[</returns>
	CharacterController& GetCharacterController()
	{
		return m_charCon;
	}

	void ReStart();

private:
	/// <summary>
	/// �ǐՏ���
	/// </summary>
	void Chase();
	/// <summary>
	/// ��]����
	/// </summary>
	void Rotation();
	/// <summary>
	/// �U������
	/// </summary>
	void Attack();
	/// <summary>
	/// ���@�U�����쐬
	/// </summary>
	void MakeMagic();
	/// <summary>
	/// �v���C���[�̍U���Ƃ̓����蔻�菈��
	/// </summary>
	void Collision();
	/// <summary>
	/// �v���C���[��T������
	/// </summary>
	/// <returns>�v���C���[������������treu</returns>
	const bool SearchPlayer() const;
	/// <summary>
	/// �U���p�̓����蔻��R���W�������쐬
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// �U�������ۂ̃G�t�F�N�g���쐬����
	/// </summary>
	void MakeSlashingEffect();
	/// <summary>
	/// �A�j���[�V�����̍Đ�
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// �A�j���[�V�����C�x���g�p�̊֐�
	/// </summary>
	/// <param name="clipName">�A�j���[�V�����̖��O</param>
	/// <param name="eventName">�A�j���[�V�����C�x���g�L�[�̖��O</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// �e�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ManageState();
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
	/// �ǐՃX�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessChaseStateTransition();
	/// <summary>
	/// �U���X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// ���@�U���X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessMagicStateTransition();
	/// <summary>
	/// ��_���[�W�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// �_�E���X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// �U���ł��鋗�����ǂ������ׂ�
	/// </summary>
	/// <returns>�U���ł���Ȃ�true</returns>
	const bool IsCanAttack() const;
	/// <summary>
	/// HP�Q�[�W�摜�̑���������
	/// </summary>
	void HPBer();

	enum EnAnimationClip {			//�A�j���[�V����
		enAnimationClip_Idle,		//�ҋ@�A�j���[�V����
		enAnimationClip_Run,		//����A�j���[�V����
		enAnimationClip_Attack,		//�U���A�j���[�V����
		enAnimationClip_Magic,		//���@�U���A�j���[�V����
		enAnimationClip_Damage,		//��_���[�W�A�j���[�V����
		enAnimationClip_Down,		//���S�A�j���[�V����
		enAnimationClip_Num,		//�A�j���[�V�����̐�
	};

	SpriteRender m_bossName;		//�{�X�̖��O
	SpriteRender m_HPbar;			//HP�Q�[�W�摜
	SpriteRender m_HPframe;			//HP�Q�[�W�̘g
	SpriteRender m_HPback;			//HP�Q�[�W�̔w�i

	AnimationClip		m_animationClips[enAnimationClip_Num];	//�A�j���[�V�����N���b�v
	ModelRender			m_modelRender;							//���f�������_�[
	Vector3				m_position = Vector3::Zero;				//���W
	Vector3				m_firstPosition = Vector3::Zero;		//�������W
	Vector3				m_moveSpeed = Vector3::Zero;;			//�ړ����x
	Vector3				m_forward = Vector3::AxisZ;				//�{�X�̐��ʃx�N�g��
	Quaternion			m_rotation = Quaternion::Identity;		//��]
	Quaternion			m_firstRotation = Quaternion::Identity;	//������]
	Vector3				m_scale = Vector3::One;					//�傫��
	CharacterController	m_charCon;								//�L�����R��
	EnBossState			m_BossState = enBossState_Idle;			//�{�X�̃X�e�[�g
	bool				m_isUnderAttack = false;				//�U�������H
	int					m_maxHP = 0;							//�ő�HP
	int					m_hp = 0;								//HP
	int					m_swordBoneId = -1;						//�{�X�̌��̃{�[��ID
	Player*				m_player = nullptr;						//�v���C���[
	float				m_chaseTimer = 0.0f;					//�ǐՃ^�C�}�[
	float				m_idleTimer = 0.0f;						//�ҋ@�^�C�}�[
	Vector3 m_magicOldPos = Vector3::Zero;
};

