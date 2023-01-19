#pragma once

class Player;

class MagicThunder : public IGameObject
{
public:
	enum EnMagician {
		enMagician_None,
		enMagician_Player,	//�p�҂��v���C���[
		enMagician_Enemy	//�p�҂��G
	};

public:
	MagicThunder();
	~MagicThunder();
	bool Start();
	void Update();

	/// <summary>
	/// �p�҂�ݒ�
	/// </summary>
	/// <param name="enMagician">�p��</param>
	void SetEnMagician(const EnMagician enMagician)
	{
		m_enMagician = enMagician;
	}

private:
	Vector3				m_position;						//���W
	Quaternion			m_rotation;						//��]
	Vector3				m_scale = Vector3::One;			//�傫��
	EffectEmitter*		m_effectEmitter = nullptr;		//�G�t�F�N�g
	float				m_timer = 0.0f;					//�^�C�}�[
	CollisionObject*	m_collisionObject;				//�R���W�����I�u�W�F�N�g
	EnMagician			m_enMagician = enMagician_None;	//�p��
	Player*				m_player = nullptr;				//�v���C���[
};

