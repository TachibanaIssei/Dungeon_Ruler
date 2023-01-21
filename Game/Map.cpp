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
	const float BOSS_SPRITE_WIDTH = 78.0f;	//ボスのマップ画像の横サイズ
	const float BOSS_SPRITE_HEIGHT = 86.0f;	//ボスのマップ画像の縦サイズ
	const float MAP_FRAME_WIDTH = 370.0f;	//マップの枠の横サイズ
	const float MAP_FRAME_HEIGHT = 370.0f;	//マップの枠の縦サイズ

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
	//y軸はマップの座標に関係ないので0.0fにする
	worldCenterPosition.y = 0.0f;
	worldPosition.y = 0.0f;

	Vector3 diff = worldPosition - worldCenterPosition;

	//マップの中心とするオブジェクトとの距離が一定以上離れていたら
	if (diff.LengthSq() >= LIMITED_RANGE_IMAGE * LIMITED_RANGE_IMAGE)
	{
		return false;
	}

	//ベクトルの長さを取得
	float length = diff.Length();

	//カメラの前方向ベクトルからクォータニオンを作成
	Vector3 forward = g_camera3D->GetForward();
	Quaternion rot;
	rot.SetRotationY(atan2(-forward.x, forward.z));

	//ベクトルにカメラの回転を適用
	rot.Apply(diff);

	diff.Normalize();

	//マップの大きさ/距離制限で
	//ベクトルをマップ座標系を変換する
	diff *= length * MAP_RADIUS / LIMITED_RANGE_IMAGE;

	//マップの中央座標と上記ベクトルを加算する
	mapPosition = Vector3(m_mapCenterPosition.x + diff.x, m_mapCenterPosition.y + diff.z, 0.0f);

	return true;
}

void Map::PlayerMap()
{
	//プレイヤーの画像を回転
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

	//マップに表示する範囲に敵がいたら。
	if (WorldPositionConvertToMapPosition(playerPosition, enemyPosition, mapPosition))
	{
		//マップに表示するように設定する、
		m_isImage = true;
		//SpriteRenderに座標を設定。
		m_bossSprite.SetPosition(mapPosition);
	}
	//マップに表示する範囲に敵がいなかったら。
	else
	{
		//マップに表示しない。
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
			//マップの背景
			m_spriteRender.Draw(rc);

			//マッシュルーム
			const auto& mushrooms = FindGOs<MushRoomMan>("mushroomman");
			const int mushSize = mushrooms.size();

			for (int i = 0; i < mushSize; i++)
			{
				mushrooms[i]->MushMap(rc);
			}

			//ストーンモンスター
			const auto& stonemonsters = FindGOs<StoneMonster>("stonemonster");
			const int stoneSize = stonemonsters.size();

			for (int i = 0; i < stoneSize; i++)
			{
				stonemonsters[i]->StoneMap(rc);
			}

			//ボス
			if (m_isImage)
			{
				m_bossSprite.Draw(rc);
			}

			//炎ギミック
			const auto& fireGimmics = FindGOs<FireGimmic>("firegimmic");
			const int fireGimmicSize = fireGimmics.size();

			for (int i = 0; i < fireGimmicSize; i++)
			{
				fireGimmics[i]->FireGimmicMap(rc);
			}

			//回転する床ギミック
			const auto& seesawFloors = FindGOs<SeesawFloor>("seesawfloor");
			const int seesawFloorSize = seesawFloors.size();

			for (int i = 0; i < seesawFloorSize; i++)
			{
				seesawFloors[i]->SeeasawMap(rc);
			}

			//X軸に動く床
			MovingFloor* movingFloorX = FindGO<MovingFloor>("movingfloorX");
			movingFloorX->FloorXMap(rc);

			//Z軸に動く床
			const auto& movingFloorZs = FindGOs<MovingFloorZ>("movingfloorZ");
			const int movingFloorZSize = movingFloorZs.size();

			for (int i = 0; i < movingFloorZSize; i++)
			{
				movingFloorZs[i]->FloorZMap(rc);
			}
			//プレイヤー
			m_playerSprite.Draw(rc);
			//マップの枠
			m_mapFrame.Draw(rc);
		}
	}
}
