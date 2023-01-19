#pragma once

#include "sound/SoundSource.h"

class Game;
class Player;
class Map;
class FireGimmic :public IGameObject
{
public:
	FireGimmic();
	~FireGimmic();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	/// <summary>
	/// �G�t�F�N�g�̍Đ��Ԋu��ݒ肷��
	/// </summary>
	/// <param name="time">�Đ��Ԋu�̎���</param>
	void SetEffectInterval(const float& time)
	{
		m_effectInterval = time;
	}

	/// <summary>
	/// �X�s�[�h�ƈړ�������ݒ肷��
	/// </summary>
	/// <param name="speed">���x</param>
	/// <param name="limit">�ړ�����</param>
	void SetSppedLimit(const float& speed, const float& limit)
	{
		m_speed = speed;
		m_limit = limit;
	}

	/// <summary>
	/// �ړ�����t���O��ݒ肷��
	/// </summary>
	/// <param name="flag">�t���O</param>
	void SetMoveFlag(const bool& flag)
	{
		m_moveFlag = flag;
	}

	/// <summary>
	/// �}�b�v��̉摜�̓���
	/// </summary>
	/// <param name="rc"></param>
	void FireGimmicMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_mapSprite.Draw(rc);
		}
	}

private:
	void Move();												//�ړ�
	void PlayEffect();											//�G�t�F�N�g�̍Đ�
	void Collision();											//�G�t�F�N�g�Đ����̂݃R���W�������쐬
	void PlaySE();												//���ʉ����Đ�����
	void MapMove();												//�}�b�v�ړ��̏���
	ModelRender				m_modelRender;
	Vector3					m_position	= Vector3::Zero;
	Vector3					m_firstPosition = Vector3::Zero;
	EffectEmitter*			m_fire = nullptr;					//���̃M�~�b�N�G�t�F�N�g
	Player*					m_player = nullptr;
	Game*					m_game = nullptr;
	SoundSource*			m_se;								//��SE
	Map*					m_map = nullptr;					//�}�b�v

	SpriteRender			m_mapSprite;						//�}�b�v�̉摜

	enum enMovingState																//�����̃X�e�[�g
	{
		enMovingState_MovingRight,													//�E�Ɉړ�
		enMovingState_MovingLeft													//���Ɉړ�
	};
	enMovingState			m_MovingFloorState = enMovingState_MovingRight;	//�X�e�[�g�̕ϐ�

	bool					m_moveFlag = false;					//true�������瓮��
	bool					m_isMapImage = false;				//�}�b�v�\������Ƃ�true

	float					m_effectIntervalTimer = 0.0f;		//�G�t�F�N�g�̊Ԋu����̃^�C�}�[
	float					m_effectInterval = 5.0f;			//�G�t�F�N�g���~������Ԋu

	float	m_limit = 100.0f;
	float	m_speed = 20.0f;
};