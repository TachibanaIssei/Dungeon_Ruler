#include "stdafx.h"
#include "SeesawFloor.h"

#include "Player.h"
#include "Map.h"
#include "collision/CollisionObject.h"

namespace
{
	const Vector3 COLLISION_HEIGHT	= Vector3(0.0f, 0.0f, 0.0f);
	const Vector3 COLLISION_SIZE	= Vector3(200.0f, 150.0f, 300.0f);

	const float MAP_SPRITE_WIDTH	= 57.0f;	//�}�b�v�摜���T�C�Y
	const float MAP_SPRITE_HEIGHT	= 38.0f;	//�}�b�v�摜�c�T�C�Y
}

SeesawFloor::SeesawFloor()
{
}

SeesawFloor::~SeesawFloor()
{
}

bool SeesawFloor::Start()
{
	m_modelRender.Init("Assets/modelData/background/seesaw_floor.tkm");
	m_modelRender.Update();
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	
	m_collisionObject = NewGO<CollisionObject>(0,"collisionObject");
	m_collisionObject->CreateBox(
		m_position + COLLISION_HEIGHT,
		Quaternion::Identity,
		COLLISION_SIZE
	);

	m_mapSprite.Init("Assets/sprite/map_iceFloor.DDS", MAP_SPRITE_WIDTH, MAP_SPRITE_HEIGHT);

	m_collisionObject->SetIsEnableAutoDelete(false);

	m_player = FindGO<Player>("player");
	m_map = FindGO<Map>("map");
	return true;
}

void SeesawFloor::Update()
{
	if (m_player == nullptr)
	{
		m_player = FindGO<Player>("player");
		return;
	}

	Rotation();
	MapMove();

	m_physicsStaticObject.SetPosition(m_position);
	m_collisionObject->SetPosition(m_position + COLLISION_HEIGHT);
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
}

void SeesawFloor::Rotation()
{
	if (m_RotationFloorState == enRotationFloorState_RotationRightDown)
	{
		m_rot += -m_addRot;
	}
	else if (m_RotationFloorState == enRotationFloorState_RotationLeftDown)
	{
		m_rot += m_addRot;
	}

	m_rotation.SetRotationX(m_rot);
	m_physicsStaticObject.SetRotation(m_rotation);
	m_modelRender.SetRotation(m_rotation);

	if (m_RotationFloorState == enRotationFloorState_RotationRightDown)
	{
		if (m_rot <= -m_rotLimit) {
			m_RotationFloorState = enRotationFloorState_RotationLeftDown;
		}
	}
	else if (m_RotationFloorState == enRotationFloorState_RotationLeftDown)
	{
		if (m_rot >= m_rotLimit) {
			m_RotationFloorState = enRotationFloorState_RotationRightDown;
		}
	}

	//�v���C���[�����̏�ɂ�����true
	if (m_collisionObject->IsHit(m_player->GetCharacterController()) == true)
	{
		m_onSeesawFloorFlag = true;
	}
	else
	{
		m_onSeesawFloorFlag = false;
	}
}

void SeesawFloor::MapMove()
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

void SeesawFloor::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
