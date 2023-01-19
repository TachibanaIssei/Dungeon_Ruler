#pragma once

class Player;
class MushRoomMan;
class StoneMonster;
class Boss;

class Map : public IGameObject
{
public:
	Map();
	~Map();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// マップの真ん中を取得する
	/// </summary>
	/// <returns></returns>
	const Vector3& GetMapCenterPosition() const
	{
		return m_mapCenterPosition;
	}

	/// <summary>
	/// ワールド座標をマップ用の座標に変換する
	/// </summary>
	/// <param name="worldCenterPosition">真ん中の座標</param>
	/// <param name="worldPosition">変換したい座標</param>
	/// <param name="mapPosition">変換した値が入る</param>
	/// <returns></returns>
	const bool WorldPositionConvertToMapPosition(Vector3 worldCenterPosition, Vector3 worldPosition, Vector3& mapPosition);

private:
	void PlayerMap();
	void BossMap();

	SpriteRender	m_spriteRender;
	SpriteRender	m_mapFrame;
	SpriteRender	m_playerSprite;
	SpriteRender	m_bossSprite;

	Vector3			m_mapCenterPosition		= Vector3(765.0f, 205.0f, 0.0f);	//マップのセンターポジション

	Player*			m_player = nullptr;
	Boss*			m_boss = nullptr;

	bool	m_isImage = false;
};

