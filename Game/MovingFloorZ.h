#pragma once

class Player;
class Map;

class MovingFloorZ : public IGameObject
{
public:
	MovingFloorZ();
	~MovingFloorZ();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	/// <summary>
	/// ���̃X�s�[�h��ݒ�
	/// </summary>
	/// <param name="speed">�X�s�[�h</param>
	void SetSpeed(const float& speed)
	{
		m_speed = speed;
	}

	/// <summary>
	/// ���̓���������ݒ�
	/// </summary>
	/// <param name="limited">����</param>
	void SetLimited(const float& limited)
	{
		m_limited = limited;
	}

	/// <summary>
	/// �ړ����x���擾����
	/// </summary>
	/// <returns>�ړ����x</returns>
	const Vector3& GetMoveSpeed() const
	{
		return m_moveSpeed;
	}

	/// <summary>
	/// �������ɂ��邩�ǂ���(������true)
	/// </summary>
	/// <returns>true��false</returns>
	const bool& GetFlag() const
	{
		return m_onMovingFloorFlag;
	}

	/// <summary>
	/// �}�b�v�摜�\��
	/// </summary>
	/// <param name="rc"></param>
	void FloorZMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_mapSprite.Draw(rc);
		}
	}

private:
	void MoveZ();
	void MapMove();

	Vector3						m_position;												//���W
	Vector3						m_firstPosition;										//�������ꂽ���W
	Vector3						m_moveSpeed = Vector3::Zero;							//�ړ����x
	Vector3						m_scale;												//�傫��

	ModelRender					m_modelRender;											//���f�������_�[
	Player*						m_player = nullptr;										//�v���C���[
	Map*						m_map = nullptr;										//�}�b�v
	PhysicsStaticObject			m_physicsStaticObject;									//�ÓI�����I�u�W�F�N�g
	CollisionObject*			m_collisionObject = nullptr;							//�R���W����
	SpriteRender				m_mapSprite;											//�}�b�v�摜

	enum enMovingFloorState																//�����̃X�e�[�g
	{
		enMovingFloorState_MovingRight,													//�E�Ɉړ�
		enMovingFloorState_MovingLeft													//���Ɉړ�
	};

	enMovingFloorState			m_MovingFloorState	= enMovingFloorState_MovingRight;	//�X�e�[�g�̕ϐ�

	float						m_speed				= 100.0f;							//���̃X�s�[�h
	float						m_limited			= 170.0f;							//���̓�������
	float						m_stopTime			= 0.0f;								//�ꎞ��~���邽�߂̃^�C�}�[
	bool						m_onMovingFloorFlag = false;							//�������ɂ�����true
	bool						m_isMapImage		= false;							//�}�b�v�摜��\������Ƃ�true
};

