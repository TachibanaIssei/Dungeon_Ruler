#include "stdafx.h"
#include "Map.h"

#include "Player.h"
#include "MushRoomMan.h"
#include "StoneMonster.h"
#include "Boss.h"
#include "FireGimmic.h"
#include "SeesawFloor.h"
#include "MovingFloor.h"
#include "MovingFloorZ.h"
#include "Fade.h"

namespace
{
	const float MAP_SPRITE_SCALE = 358.4f;
	const float PLAYER_SPRITE_SCALE = 51.2f;
	const float BOSS_SPRITE_WIDTH = 78.0f;	//�{�X�̃}�b�v�摜�̉��T�C�Y
	const float BOSS_SPRITE_HEIGHT = 86.0f;	//�{�X�̃}�b�v�摜�̏c�T�C�Y
	const float MAP_FRAME_WIDTH = 370.0f;	//�}�b�v�̘g�̉��T�C�Y
	const float MAP_FRAME_HEIGHT = 370.0f;	//�}�b�v�̘g�̏c�T�C�Y

	const float MAP_RADIUS = 160.0f;
	const float LIMITED_RANGE_IMAGE = 400.0f;

	Vector3 MAP_FRAME_POS = Vector3(765.0f, 205.0f, 0.0f);
}

Map::Map()
{
}

Map::~Map()
{
}

bool Map::Start()
{
	m_spriteRender.Init("Assets/sprite/minimap.DDS", MAP_SPRITE_SCALE, MAP_SPRITE_SCALE);
	m_spriteRender.SetPosition(m_mapCenterPosition);

	m_mapFrame.Init("Assets/sprite/minimap_window.DDS", MAP_FRAME_WIDTH, MAP_FRAME_HEIGHT);
	m_mapFrame.SetPosition(MAP_FRAME_POS);

	m_playerSprite.Init("Assets/sprite/minimap_player.DDS", PLAYER_SPRITE_SCALE, PLAYER_SPRITE_SCALE);
	m_playerSprite.SetPosition(m_mapCenterPosition);	
	
	m_bossSprite.Init("Assets/sprite/boss_image.DDS", BOSS_SPRITE_WIDTH, BOSS_SPRITE_HEIGHT);


	m_player = FindGO<Player>("player");

	return true;
}

void Map::Update()
{
	PlayerMap();
	BossMap();

	m_spriteRender.Update();
	m_mapFrame.Update();
}

const bool Map::WorldPositionConvertToMapPosition(Vector3 worldCenterPosition, Vector3 worldPosition, Vector3& mapPosition)
{
	//y���̓}�b�v�̍��W�Ɋ֌W�Ȃ��̂�0.0f�ɂ���
	worldCenterPosition.y = 0.0f;
	worldPosition.y = 0.0f;

	Vector3 diff = worldPosition - worldCenterPosition;

	//�}�b�v�̒��S�Ƃ���I�u�W�F�N�g�Ƃ̋��������ȏ㗣��Ă�����
	if (diff.LengthSq() >= LIMITED_RANGE_IMAGE * LIMITED_RANGE_IMAGE)
	{
		return false;
	}

	//�x�N�g���̒������擾
	float length = diff.Length();

	//�J�����̑O�����x�N�g������N�H�[�^�j�I�����쐬
	Vector3 forward = g_camera3D->GetForward();
	Quaternion rot;
	rot.SetRotationY(atan2(-forward.x, forward.z));

	//�x�N�g���ɃJ�����̉�]��K�p
	rot.Apply(diff);

	diff.Normalize();

	//�}�b�v�̑傫��/����������
	//�x�N�g�����}�b�v���W�n��ϊ�����
	diff *= length * MAP_RADIUS / LIMITED_RANGE_IMAGE;

	//�}�b�v�̒������W�Ə�L�x�N�g�������Z����
	mapPosition = Vector3(m_mapCenterPosition.x + diff.x, m_mapCenterPosition.y + diff.z, 0.0f);

	return true;
}

void Map::PlayerMap()
{
	//�v���C���[�̉摜����]
	//m_playerSprite.SetRotation(m_player->GetRotation());

	Quaternion playerRotation;
	//playerRotation.SetRotationZ(atan2(g_camera3D->GetForward().x, -g_camera3D->GetForward().z));

	m_playerSprite.Update();
}

void Map::BossMap()
{
	m_boss = FindGO<Boss>("boss");
	if (m_boss == nullptr)
	{
		return;
	}

	Vector3 playerPosition = m_player->GetPosition();
	Vector3 enemyPosition = m_boss->GetPosition();

	Vector3 mapPosition;

	//�}�b�v�ɕ\������͈͂ɓG��������B
	if (WorldPositionConvertToMapPosition(playerPosition, enemyPosition, mapPosition))
	{
		//�}�b�v�ɕ\������悤�ɐݒ肷��A
		m_isImage = true;
		//SpriteRender�ɍ��W��ݒ�B
		m_bossSprite.SetPosition(mapPosition);
	}
	//�}�b�v�ɕ\������͈͂ɓG�����Ȃ�������B
	else
	{
		//�}�b�v�ɕ\�����Ȃ��B
		m_isImage = false;
	}

	m_bossSprite.Update();
}

void Map::Render(RenderContext& rc)
{
	if (m_player->GetSpriteFlag())
	{
		if (!m_fade->IsFade())
		{
			//�}�b�v�̔w�i
			m_spriteRender.Draw(rc);

			//�}�b�V�����[��
			const auto& mushrooms = FindGOs<MushRoomMan>("mushroomman");
			const int mushSize = mushrooms.size();

			for (int i = 0; i < mushSize; i++)
			{
				mushrooms[i]->MushMap(rc);
			}

			//�X�g�[�������X�^�[
			const auto& stonemonsters = FindGOs<StoneMonster>("stonemonster");
			const int stoneSize = stonemonsters.size();

			for (int i = 0; i < stoneSize; i++)
			{
				stonemonsters[i]->StoneMap(rc);
			}

			//�{�X
			if (m_isImage)
			{
				m_bossSprite.Draw(rc);
			}

			//���M�~�b�N
			const auto& fireGimmics = FindGOs<FireGimmic>("firegimmic");
			const int fireGimmicSize = fireGimmics.size();

			for (int i = 0; i < fireGimmicSize; i++)
			{
				fireGimmics[i]->FireGimmicMap(rc);
			}

			//��]���鏰�M�~�b�N
			const auto& seesawFloors = FindGOs<SeesawFloor>("seesawfloor");
			const int seesawFloorSize = seesawFloors.size();

			for (int i = 0; i < seesawFloorSize; i++)
			{
				seesawFloors[i]->SeeasawMap(rc);
			}

			//X���ɓ�����
			MovingFloor* movingFloorX = FindGO<MovingFloor>("movingfloorX");
			movingFloorX->FloorXMap(rc);

			//Z���ɓ�����
			const auto& movingFloorZs = FindGOs<MovingFloorZ>("movingfloorZ");
			const int movingFloorZSize = movingFloorZs.size();

			for (int i = 0; i < movingFloorZSize; i++)
			{
				movingFloorZs[i]->FloorZMap(rc);
			}
			//�v���C���[
			m_playerSprite.Draw(rc);
			//�}�b�v�̘g
			m_mapFrame.Draw(rc);
		}
	}
}
