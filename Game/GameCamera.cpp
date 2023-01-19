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
		*g_camera3D,	//�΂˃J�����̏������s���J�������w�肷��
		1000.0f,		//�J�����̈ړ����x�̍ő�l
		true,			//�J�����ƒn�`�Ƃ̓����蔻�����邩�ǂ����̃t���O
		5.0f			//�J�����ɐݒ肳��鋅�̃R���W�����̔���
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
	//�p�b�h�̓��͂��g���ăJ��������
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();
	//Y������̉�]
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 1.5f * x);
	qRot.Apply(m_toCameraPos);
	//X������̉�]
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 1.5 * y);
	qRot.Apply(m_toCameraPos);
	//�J�����̉�]������`�F�b�N����
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y < -0.5f) {
		//�J���������������
		m_toCameraPos = toCameraPosOld;
	}
	else if (toPosDir.y > 0.8f) {
		//�J������������
		m_toCameraPos = toCameraPosOld;
	}

	//���_���v�Z����
	m_pos = m_target + m_toCameraPos;

	//�΂˃J�����ɒ����_�Ǝ��_��ݒ肷��
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


