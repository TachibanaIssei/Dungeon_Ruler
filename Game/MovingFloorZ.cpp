#include "stdafx.h"
#include "MovingFloorZ.h"

#include "Player.h"
#include "Map.h"
#include "collision/CollisionObject.h"

namespace
{
	Vector3 COLLISION_HEIGHT = Vector3(0.0f, 80.0f, -3.0f);
	Vector3 COLLISION_SIZE_Z = Vector3(200.0f, 150.0f, 200.0f);

	const float MAP_SPRITE_SIZE = 49.0f;	//�}�b�v�摜�̃T�C�Y
}

MovingFloorZ::MovingFloorZ()
{
}

MovingFloorZ::~MovingFloorZ()
{
	DeleteGO(m_collisionObject);
}

bool MovingFloorZ::Start()
{
	m_modelRender.Init("Assets/modelData/background/move_floorZ.tkm");
	m_modelRender.Update();
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	m_collisionObject = NewGO<CollisionObject>(0, "collisionObject");

	m_mapSprite.Init("Assets/sprite/Map_moveFloor.DDS", MAP_SPRITE_SIZE, MAP_SPRITE_SIZE);

	//�R���W�����I�u�W�F�N�g���B
	//�������̏�ɐݒu����(�L�����N�^�[����ɏ�����甽������悤�ɂ���������)�B
	m_collisionObject->CreateBox(
		m_position + COLLISION_HEIGHT,
		Quaternion::Identity,
		COLLISION_SIZE_Z
	);

	m_collisionObject->SetIsEnableAutoDelete(false);
	m_firstPosition = m_position;

	m_player	= FindGO<Player>("player");
	m_map		= FindGO<Map>("map");
	return true;
}

void MovingFloorZ::Update()
{
	if (m_player == nullptr)
	{
		m_player = FindGO<Player>("player");
		return;
	}
	MoveZ();
	MapMove();

	m_modelRender.SetScale(m_scale);
	m_modelRender.Update();
	m_physicsStaticObject.SetPosition(m_position);
	m_collisionObject->SetPosition(m_position + COLLISION_HEIGHT);
}

void MovingFloorZ::MoveZ()
{
	m_moveSpeed = Vector3::Zero;

	if (m_MovingFloorState == enMovingFloorState_MovingRight)
	{
		if (m_firstPosition.z - m_limited >= m_position.z)
		{
			m_stopTime += g_gameTime->GetFrameDeltaTime();
			if (m_stopTime <= 1.0f)
			{
				return;
			}
			m_MovingFloorState = enMovingFloorState_MovingLeft;
			m_stopTime = 0.0f;
		}
	}
	else if (m_MovingFloorState == enMovingFloorState_MovingLeft)
	{
		if (m_firstPosition.z + m_limited <= m_position.z)
		{
			m_stopTime += g_gameTime->GetFrameDeltaTime();
			if (m_stopTime <= 1.0f)
			{
				return;
			}
			m_MovingFloorState = enMovingFloorState_MovingRight;
			m_stopTime = 0.0f;
		}
	}

	if (m_MovingFloorState == enMovingFloorState_MovingRight)
	{
		m_moveSpeed.z = -m_speed;
	}
	else if (m_MovingFloorState == enMovingFloorState_MovingLeft)
	{
		m_moveSpeed.z = m_speed;
	}

	m_position += m_moveSpeed * g_gameTime->GetFrameDeltaTime();

	m_modelRender.SetPosition(m_position);

	//�R���W�����I�u�W�F�N�g�ƃv���C���[�̃L�����N�^�[�R���g���[���[���B
	//�Փ˂�����B(�L�����N�^�[���������̏�ɏ������)�B
	if (m_collisionObject->IsHit(m_player->GetCharacterController()) == true)
	{
		m_onMovingFloorFlag = true;
	}
	else
	{
		m_onMovingFloorFlag = false;
	}
}

void MovingFloorZ::MapMove()
{
	Vector3 playerPosition = m_player->GetPosition();
	Vector3 enemyPosition = m_position;

	Vector3 mapPosition;

	//�}�b�v�ɕ\������͈͂ɓG��������B
	if (m_map->WorldPositionConvertToMapPosition(playerPosition, enemyPosition, mapPosition))
	{
		//�}�b�v�ɕ\������悤�ɐݒ肷��A
		m_isMapImage = true;
		//SpriteRender�ɍ��W��ݒ�B
		m_mapSprite.SetPosition(mapPosition);
	}
	//�}�b�v�ɕ\������͈͂ɓG�����Ȃ�������B
	else
	{
		//�}�b�v�ɕ\�����Ȃ��B
		m_isMapImage = false;
	}

	m_mapSprite.Update();
}

void MovingFloorZ::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
