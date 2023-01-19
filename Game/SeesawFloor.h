#pragma once

class Player;
class Map;

class SeesawFloor : public IGameObject
{
public:
	SeesawFloor();
	~SeesawFloor();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	//回転方向のステートを設定する
	void SetState()
	{
		m_RotationFloorState = enRotationFloorState_RotationLeftDown;
	}

	/// <summary>
	/// 回転の増加力と最大値の設定
	/// </summary>
	/// <param name="rot">増加度</param>
	/// <param name="rotLimit">最大値</param>
	void SetRot(const float rot, const float rotLimit)
	{
		m_addRot = rot;
		m_rotLimit = rotLimit;
	}

	/// <summary>
	/// 床の番号を設定
	/// </summary>
	/// <param name="number">番号</param>
	void SetFloorNumber(int number)
	{
		m_floorNumber = number;
	}

	const int GetFloorNumber()
	{
		return m_floorNumber;
	}

	/// <summary>
	/// 回転を取得する
	/// </summary>
	/// <returns>回転</returns>
	const float& GetRotation() const
	{
		return m_rot;
	}

	/// <summary>
	/// 動く床にいるかどうか(いたらtrue)
	/// </summary>
	/// <returns>trueかfalse</returns>
	const bool& GetFlag() const
	{
		return m_onSeesawFloorFlag;
	}

	/// <summary>
	/// マップ上の画像の動作
	/// </summary>
	/// <param name="rc"></param>
	void SeeasawMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_mapSprite.Draw(rc);
		}
	}

private:
	void Rotation();
	/// <summary>
	/// 画像の移動処理
	/// </summary>
	void MapMove();
	Vector3						m_position = Vector3::Zero;
	Vector3						m_scale = Vector3::One;
	Quaternion					m_rotation = Quaternion::Identity;

	ModelRender					m_modelRender;

	Player*						m_player = nullptr;
	Map*						m_map = nullptr;

	SpriteRender				m_mapSprite;
	PhysicsStaticObject			m_physicsStaticObject;
	CollisionObject*			m_collisionObject = nullptr;

	enum enRotationFloorState
	{
		enRotationFloorState_RotationRightDown,
		enRotationFloorState_RotationLeftDown
	};
	enRotationFloorState m_RotationFloorState = enRotationFloorState_RotationRightDown;

	int		m_floorNumber = 0;						//床の番号
	float	m_rot = 0.0f;							//回転の角度
	float	m_addRot = 0.003;						//回転の増加度
	float	m_rotLimit = 0.4f;						//回転の上限
	bool	m_onSeesawFloorFlag = false;			//trueであれば床の上にいる
	bool	m_isMapImage = false;					//マップに表示するときtrue

};

