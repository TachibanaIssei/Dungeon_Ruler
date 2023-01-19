#pragma once

class MagicBall : public IGameObject
{
public:
	enum EnMagician {
		enMagician_None,
		enMagician_Player,	//�p�҂��v���C���[
		enMagician_Enemy	//�p�҂��G
	};
public:
	MagicBall();
	~MagicBall();
	bool Start();
	void Update();

	/// <summary>
	/// ���W��ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// ��]��ݒ�
	/// </summary>
	/// <param name="rotation">��]</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// �傫����ݒ�
	/// </summary>
	/// <param name="scale">�傫��</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	void SetEnMagician(const EnMagician enMagician)
	{
		m_enMagician = enMagician;
	}

private:
	EffectEmitter*			m_effectEmitter = nullptr;		//�G�t�F�N�g
	Vector3					m_position;						//���W
	Quaternion				m_rotation;						//��]
	Vector3					m_scale = Vector3::One;			//�傫��
	Vector3					m_moveSpeed;					//�ړ����x
	float					m_timer = 0.0f;					//�^�C�}�[
	CollisionObject*		m_collisionObject;				//�R���W�����I�u�W�F�N�g
	EnMagician				m_enMagician = enMagician_None;	//�p��
};

