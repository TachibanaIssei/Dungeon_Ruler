#pragma once

class MagicBall : public IGameObject
{
public:
	enum EnMagician {
		enMagician_None,
		enMagician_Player,	//術者がプレイヤー
		enMagician_Enemy	//術者が敵
	};
public:
	MagicBall();
	~MagicBall();
	bool Start();
	void Update();

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="rotation">回転</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// 大きさを設定
	/// </summary>
	/// <param name="scale">大きさ</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	void SetEnMagician(const EnMagician enMagician)
	{
		m_enMagician = enMagician;
	}

private:
	EffectEmitter*			m_effectEmitter = nullptr;		//エフェクト
	Vector3					m_position;						//座標
	Quaternion				m_rotation;						//回転
	Vector3					m_scale = Vector3::One;			//大きさ
	Vector3					m_moveSpeed;					//移動速度
	float					m_timer = 0.0f;					//タイマー
	CollisionObject*		m_collisionObject;				//コリジョンオブジェクト
	EnMagician				m_enMagician = enMagician_None;	//術者
};

