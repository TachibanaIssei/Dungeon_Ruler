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
	/// 座標と視点を更新する
	/// </summary>
	void UpdatePositionAndTarget();

	/// <summary>
	/// 注視点を更新する
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
	/// カメラを動かないようにする
	/// </summary>
	void SetStopCamera()
	{
		m_cameraStopFlag = true;
	}

	/// <summary>
	/// 注視点のみを動かすフラグをON
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
	bool				m_cameraStopFlag = false;		//カメラが動かないようにする
	bool				m_cameraMoveTarget = false;		//注視点のみを動くようにする
};

