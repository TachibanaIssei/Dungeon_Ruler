#pragma once

class Player;
class Map;

class SeesawFloor : public IGameObject
{
public:
	SeesawFloor();
	~SeesawFloor();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	//��]�����̃X�e�[�g��ݒ肷��
	void SetState()
	{
		m_RotationFloorState = enRotationFloorState_RotationLeftDown;
	}

	/// <summary>
	/// ��]�̑����͂ƍő�l�̐ݒ�
	/// </summary>
	/// <param name="rot">�����x</param>
	/// <param name="rotLimit">�ő�l</param>
	void SetRot(const float rot, const float rotLimit)
	{
		m_addRot = rot;
		m_rotLimit = rotLimit;
	}

	/// <summary>
	/// ���̔ԍ���ݒ�
	/// </summary>
	/// <param name="number">�ԍ�</param>
	void SetFloorNumber(int number)
	{
		m_floorNumber = number;
	}

	const int GetFloorNumber()
	{
		return m_floorNumber;
	}

	/// <summary>
	/// ��]���擾����
	/// </summary>
	/// <returns>��]</returns>
	const float& GetRotation() const
	{
		return m_rot;
	}

	/// <summary>
	/// �������ɂ��邩�ǂ���(������true)
	/// </summary>
	/// <returns>true��false</returns>
	const bool& GetFlag() const
	{
		return m_onSeesawFloorFlag;
	}

	/// <summary>
	/// �}�b�v��̉摜�̓���
	/// </summary>
	/// <param name="rc"></param>
	void SeeasawMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_mapSprite.Draw(rc);
		}
	}

private:
	void Rotation();
	/// <summary>
	/// �摜�̈ړ�����
	/// </summary>
	void MapMove();
	Vector3						m_position = Vector3::Zero;
	Vector3						m_scale = Vector3::One;
	Quaternion					m_rotation = Quaternion::Identity;

	ModelRender					m_modelRender;

	Player*						m_player = nullptr;
	Map*						m_map = nullptr;

	SpriteRender				m_mapSprite;
	PhysicsStaticObject			m_physicsStaticObject;
	CollisionObject*			m_collisionObject = nullptr;

	enum enRotationFloorState
	{
		enRotationFloorState_RotationRightDown,
		enRotationFloorState_RotationLeftDown
	};
	enRotationFloorState m_RotationFloorState = enRotationFloorState_RotationRightDown;

	int		m_floorNumber = 0;						//���̔ԍ�
	float	m_rot = 0.0f;							//��]�̊p�x
	float	m_addRot = 0.003;						//��]�̑����x
	float	m_rotLimit = 0.4f;						//��]�̏��
	bool	m_onSeesawFloorFlag = false;			//true�ł���Ώ��̏�ɂ���
	bool	m_isMapImage = false;					//�}�b�v�ɕ\������Ƃ�true

};

