#pragma once

#include "camera/SpringCamera.h"

class Player;

class GameCamera : public IGameObject 
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();
	void ReStart();

	/// <summary>
	/// ���W�Ǝ��_���X�V����
	/// </summary>
	void UpdatePositionAndTarget();

	/// <summary>
	/// �����_���X�V����
	/// </summary>
	void UpdateTarget();

	const Vector3& GetPosition() const
	{
		return m_pos;
	}

	const Vector3& GetTarget() const
	{
		return m_target;
	}

	/// <summary>
	/// �J�����𓮂��Ȃ��悤�ɂ���
	/// </summary>
	void SetStopCamera()
	{
		m_cameraStopFlag = true;
	}

	/// <summary>
	/// �����_�݂̂𓮂����t���O��ON
	/// </summary>
	void SetMoveTargetFlag()
	{
		m_cameraMoveTarget = true;
	}

	Player*				m_player = nullptr;
	Vector3				m_toCameraPos = Vector3::One;
	Vector3				m_pos = Vector3::Zero;
	Vector3				m_target = Vector3::Zero;
	SpringCamera		m_springCamera;
	bool				m_cameraStopFlag = false;		//�J�����������Ȃ��悤�ɂ���
	bool				m_cameraMoveTarget = false;		//�����_�݂̂𓮂��悤�ɂ���
};

