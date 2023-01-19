#pragma once

class Player;
class Map;

class MovingFloorZ : public IGameObject
{
public:
	MovingFloorZ();
	~MovingFloorZ();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	/// <summary>
	/// 床のスピードを設定
	/// </summary>
	/// <param name="speed">スピード</param>
	void SetSpeed(const float& speed)
	{
		m_speed = speed;
	}

	/// <summary>
	/// 床の動く距離を設定
	/// </summary>
	/// <param name="limited">距離</param>
	void SetLimited(const float& limited)
	{
		m_limited = limited;
	}

	/// <summary>
	/// 移動速度を取得する
	/// </summary>
	/// <returns>移動速度</returns>
	const Vector3& GetMoveSpeed() const
	{
		return m_moveSpeed;
	}

	/// <summary>
	/// 動く床にいるかどうか(いたらtrue)
	/// </summary>
	/// <returns>trueかfalse</returns>
	const bool& GetFlag() const
	{
		return m_onMovingFloorFlag;
	}

	/// <summary>
	/// マップ画像表示
	/// </summary>
	/// <param name="rc"></param>
	void FloorZMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_mapSprite.Draw(rc);
		}
	}

private:
	void MoveZ();
	void MapMove();

	Vector3						m_position;												//座標
	Vector3						m_firstPosition;										//生成された座標
	Vector3						m_moveSpeed = Vector3::Zero;							//移動速度
	Vector3						m_scale;												//大きさ

	ModelRender					m_modelRender;											//モデルレンダー
	Player*						m_player = nullptr;										//プレイヤー
	Map*						m_map = nullptr;										//マップ
	PhysicsStaticObject			m_physicsStaticObject;									//静的物理オブジェクト
	CollisionObject*			m_collisionObject = nullptr;							//コリジョン
	SpriteRender				m_mapSprite;											//マップ画像

	enum enMovingFloorState																//動きのステート
	{
		enMovingFloorState_MovingRight,													//右に移動
		enMovingFloorState_MovingLeft													//左に移動
	};

	enMovingFloorState			m_MovingFloorState	= enMovingFloorState_MovingRight;	//ステートの変数

	float						m_speed				= 100.0f;							//床のスピード
	float						m_limited			= 170.0f;							//床の動く距離
	float						m_stopTime			= 0.0f;								//一時停止するためのタイマー
	bool						m_onMovingFloorFlag = false;							//動く床にいたらtrue
	bool						m_isMapImage		= false;							//マップ画像を表示するときtrue
};

