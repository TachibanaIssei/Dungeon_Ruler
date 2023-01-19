#pragma once

class Player;

class MagicThunder : public IGameObject
{
public:
	enum EnMagician {
		enMagician_None,
		enMagician_Player,	//術者がプレイヤー
		enMagician_Enemy	//術者が敵
	};

public:
	MagicThunder();
	~MagicThunder();
	bool Start();
	void Update();

	/// <summary>
	/// 術者を設定
	/// </summary>
	/// <param name="enMagician">術者</param>
	void SetEnMagician(const EnMagician enMagician)
	{
		m_enMagician = enMagician;
	}

private:
	Vector3				m_position;						//座標
	Quaternion			m_rotation;						//回転
	Vector3				m_scale = Vector3::One;			//大きさ
	EffectEmitter*		m_effectEmitter = nullptr;		//エフェクト
	float				m_timer = 0.0f;					//タイマー
	CollisionObject*	m_collisionObject;				//コリジョンオブジェクト
	EnMagician			m_enMagician = enMagician_None;	//術者
	Player*				m_player = nullptr;				//プレイヤー
};

