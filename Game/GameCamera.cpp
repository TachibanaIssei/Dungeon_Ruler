#include "stdafx.h"
#include "GameCamera.h"

#include "Player.h"

namespace
{
	const Vector3 START_CAMERAPOSITION = Vector3(0.0f, 40.0f, 180.0f);
}

GameCamera::GameCamera()
{
}

GameCamera::~GameCamera()
{
}

bool GameCamera::Start()
{
	m_toCameraPos.Set(START_CAMERAPOSITION);
	m_player = FindGO<Player>("player");

	m_springCamera.Init(
		*g_camera3D,	//ばねカメラの処理を行うカメラを指定する
		1000.0f,		//カメラの移動速度の最大値
		true,			//カメラと地形との当たり判定を取るかどうかのフラグ
		5.0f			//カメラに設定される球体コリジョンの判定
	);

	m_springCamera.SetFar(19000.0f);
	return true;
}

void GameCamera::UpdatePositionAndTarget()
{
	m_target = m_player->GetPosition();
	m_target.y += 80.0f;
	//m_target += g_camera3D->GetForward() * 20.0f;

	Vector3 toCameraPosOld = m_toCameraPos;
	//パッドの入力を使ってカメラを回す
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();
	//Y軸周りの回転
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 1.5f * x);
	qRot.Apply(m_toCameraPos);
	//X軸周りの回転
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 1.5 * y);
	qRot.Apply(m_toCameraPos);
	//カメラの回転上限をチェックする
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y < -0.5f) {
		//カメラが上向きすぎ
		m_toCameraPos = toCameraPosOld;
	}
	else if (toPosDir.y > 0.8f) {
		//カメラが下向き
		m_toCameraPos = toCameraPosOld;
	}

	//視点を計算する
	m_pos = m_target + m_toCameraPos;

	//ばねカメラに注視点と視点を設定する
	m_springCamera.SetPosition(m_pos);
	m_springCamera.SetTarget(m_target);
}

void GameCamera::UpdateTarget()
{
	m_target = m_player->GetPosition();
	m_target.y += 80.0f;
	
	m_springCamera.SetTarget(m_target);
}

void GameCamera::Update()
{
	if (m_cameraMoveTarget == true)
	{
		UpdateTarget();
		m_springCamera.Update();
		return;
	}

	if (m_cameraStopFlag == true)
	{
		return;
	}

	UpdatePositionAndTarget();
	m_springCamera.Update();
}

void GameCamera::ReStart()
{
	m_toCameraPos.Set(START_CAMERAPOSITION);
	m_springCamera.Refresh();
	m_cameraMoveTarget = false;
	UpdatePositionAndTarget();
}


