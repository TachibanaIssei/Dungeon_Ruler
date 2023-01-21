#pragma once

class Player;
class Map;
class GameCamera;

class StoneMonster : public IGameObject
{
public:
	enum EnStoneState {
		enStoneState_Idle,
		enStoneState_Chase,
		enStoneState_Magic,
		enStoneState_ReceiveDamage,
		enStoneState_Down,
	};

public:
	StoneMonster() {};
	~StoneMonster();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// ���W��ݒ�
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
		m_firstPosition = position;
	}
	/// <summary>
	/// ���W���擾
	/// </summary>
	/// <returns></returns>
	const Vector3 GetPosition() const
	{
		return m_position;
	}
	//HP��ݒ�
	void SetHP(const int hp)
	{
		m_hp = hp;
		m_maxHP = hp;
	}
	/// <summary>
	/// ��]��ݒ�
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// �傫����ݒ�
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
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
	/// HP�֘A�摜�̍��W��ݒ�
	/// </summary>
	/// <param name="number">���O�\���̔ԍ�</param>
	void SetHPSpritePosition(int number)
	{
		m_nameNumber = number;
	}

	/// <summary>
	/// �}�b�v��̉摜�̓���
	/// </summary>
	/// <param name="rc"></param>
	void StoneMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_stoneMapSprite.Draw(rc);
		}
	}

	/// <summary>
	/// ���X�^�[�g����
	/// </summary>
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
	/// �v���C���[�̍U���Ƃ̓����蔻�菈��
	/// </summary>
	void Collision();
	/// <summary>
	/// �v���C���[��T��
	/// </summary>
	const bool SearchPlayer() const;
	/// <summary>
	/// ���@�U�����쐬
	/// </summary>
	void MakeMagicBall();
	/// <summary>
	/// ���@�U���q�b�g�G�t�F�N�g
	/// </summary>
	void MakeMagicHitEffect();
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
	/// ���ʂ̃X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// �ҋ@�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// ����X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// �ǐՃX�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessChaseStateTransition();
	/// <summary>
	/// ���@�U���X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessMagicStateTransition();
	/// <summary>
	/// ��_���[�W�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessReceiveDamageTransition();
	/// <summary>
	/// ���S�X�e�[�g�̑J�ڏ���
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// �U���ł��鋗�����ǂ������ׂ�
	/// </summary>
	/// <returns></returns>
	const bool IsCanAttack() const;
	/// <summary>
	/// HP�Q�[�W�𑝌�����
	/// </summary>
	void HPBar();
	/// <summary>
	/// �Q�[�W�����񂹂��鏈��
	/// </summary>
	/// <param name="size">�摜�̌��̑傫��</param>
	/// <param name="scale">���݂̃X�P�[���{��</param>
	/// <returns>�ϊ��O�ƕϊ���̍�</returns>
	Vector3 HPBerSend(Vector3 size, Vector3 scale);
	/// <summary>
	/// HP�Q�[�W�̕`��t���O
	/// </summary>
	/// <returns>�`��ł���͈͂ɂ���Ƃ�true</returns>
	bool DrawHP();
	/// <summary>
	/// �}�b�v�̉摜�ړ�����
	/// </summary>
	void MapMove();

	enum EnAnimationClip {
		enAnimationClip_Idle,			//�ҋ@�A�j���[�V����
		enAnimationClip_Run,			//����A�j���[�V����
		enAnimationClip_Magic,			//�U���A�j���[�V����
		enAnimationClip_Damage,			//��_���[�W�A�j���[�V����
		enAnimationClip_Down,			//���S�A�j���[�V����
		enAnimationClip_Num				//�A�j���[�V�����̐�
	};

	SpriteRender			m_HPBar;								//HP�o�[�摜
	SpriteRender			m_HPFrame;								//HP�g�摜
	SpriteRender			m_HPBack;								//HP�w�i�摜
	SpriteRender			m_stoneMapSprite;						//�}�b�v�\��


	AnimationClip			m_animationClips[enAnimationClip_Num];	//�A�j���[�V�����N���b�v
	ModelRender				m_modelRender;							//���f�������_�[

	Vector2					m_HPBerPos = Vector2::Zero;				//HP�o�[�̃|�W�V����
	Vector2					m_HPWindowPos = Vector2::Zero;			//HP�g�̃|�W�V����
	Vector2					m_HPBackPos = Vector2::Zero;			//HP�w�i�̃|�W�V����

	Vector3					m_position = Vector3::Zero;				//���W
	Vector3					m_firstPosition = Vector3::Zero;		//�X�^�[�g���̍��W
	Vector3					m_moveSpeed;							//�ړ����x
	Vector3					m_forward = Vector3::AxisZ;				//�X�g�[�������X�^�[�̐��ʃx�N�g��
	Quaternion				m_rotation;								//��]
	Quaternion				m_firstRotation = Quaternion::Identity;	//������]
	Vector3					m_scale = Vector3::One;					//�傫��
	CharacterController		m_charCon;								//�L�����R��
	EnStoneState			m_stoneState = enStoneState_Idle;		//�X�g�[�������X�^�[�̃X�e�[�g
	bool					m_isMapImage = false;					//�}�b�v�\������Ƃ�true
	int						m_maxHP = 0;							//�ő�HP
	int						m_hp = 0;								//�X�g�[�������X�^�[��HP
	int						m_nameNumber = 0;						//���O�\���̔ԍ�
	Player*					m_player = nullptr;						//�v���C���[
	Map*					m_map = nullptr;						//�}�b�v
	GameCamera*				m_gameCamera = nullptr;					//�Q�[���J����
	float					m_chaseTimer = 0.0f;					//�ǐՃ^�C�}�[
	float					m_idleTimer = 0.0f;						//�ҋ@�^�C�}�[
};
