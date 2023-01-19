#pragma once

class Player;

class Boss : public IGameObject
{
public:
	//ボスステート
	enum EnBossState {
		enBossState_Idle,
		enBossState_Chase,
		enBossState_Attack,
		enBossState_Magic,
		enBossState_ReceiveDamage,
		enBossState_Down,
	};

	Boss();
	~Boss();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
		m_firstPosition = position;
		m_modelRender.SetPosition(m_position);
	}
	/// <summary>
	/// 座標を取得する
	/// </summary>
	/// <returns>座標</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// HPを設定
	/// </summary>
	/// <param name="hp">HP</param>
	void SetHP(const int hp)
	{
		m_hp = hp;
		m_maxHP = hp;
	}
	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="rotation">回転</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_firstRotation = rotation;
		m_modelRender.SetRotation(m_rotation);
	}
	/// <summary>
	/// 大きさを設定
	/// </summary>
	/// <param name="scale">大きさ</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
		m_modelRender.SetScale(m_scale);
	}

	/// <summary>
	/// 魔法攻撃を行う座標を取得する
	/// </summary>
	/// <returns>魔法の座標</returns>
	const Vector3& GetOldPos() const
	{
		return m_magicOldPos;
	}

	/// <summary>
	/// キャラコンを取得
	/// </summary>
	/// <returns>キャラクターコントローラー</returns>
	CharacterController& GetCharacterController()
	{
		return m_charCon;
	}

	void ReStart();

private:
	/// <summary>
	/// 追跡処理
	/// </summary>
	void Chase();
	/// <summary>
	/// 回転処理
	/// </summary>
	void Rotation();
	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();
	/// <summary>
	/// 魔法攻撃を作成
	/// </summary>
	void MakeMagic();
	/// <summary>
	/// プレイヤーの攻撃との当たり判定処理
	/// </summary>
	void Collision();
	/// <summary>
	/// プレイヤーを探索する
	/// </summary>
	/// <returns>プレイヤーが見つかったらtreu</returns>
	const bool SearchPlayer() const;
	/// <summary>
	/// 攻撃用の当たり判定コリジョンを作成
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// 攻撃した際のエフェクトを作成する
	/// </summary>
	void MakeSlashingEffect();
	/// <summary>
	/// アニメーションの再生
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// アニメーションイベント用の関数
	/// </summary>
	/// <param name="clipName">アニメーションの名前</param>
	/// <param name="eventName">アニメーションイベントキーの名前</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// 各ステートの遷移処理
	/// </summary>
	void ManageState();
	/// <summary>
	/// 共通のステート遷移処理
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// 待機ステートの遷移処理
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// 歩きステートの遷移処理
	/// </summary>
	void ProcessWalkStateTransition();
	/// <summary>
	/// 追跡ステートの遷移処理
	/// </summary>
	void ProcessChaseStateTransition();
	/// <summary>
	/// 攻撃ステートの遷移処理
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// 魔法攻撃ステートの遷移処理
	/// </summary>
	void ProcessMagicStateTransition();
	/// <summary>
	/// 被ダメージステートの遷移処理
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// ダウンステートの遷移処理
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// 攻撃できる距離かどうか調べる
	/// </summary>
	/// <returns>攻撃できるならtrue</returns>
	const bool IsCanAttack() const;
	/// <summary>
	/// HPゲージ画像の増減をする
	/// </summary>
	void HPBer();

	enum EnAnimationClip {			//アニメーション
		enAnimationClip_Idle,		//待機アニメーション
		enAnimationClip_Run,		//走りアニメーション
		enAnimationClip_Attack,		//攻撃アニメーション
		enAnimationClip_Magic,		//魔法攻撃アニメーション
		enAnimationClip_Damage,		//被ダメージアニメーション
		enAnimationClip_Down,		//死亡アニメーション
		enAnimationClip_Num,		//アニメーションの数
	};

	SpriteRender m_bossName;		//ボスの名前
	SpriteRender m_HPbar;			//HPゲージ画像
	SpriteRender m_HPframe;			//HPゲージの枠
	SpriteRender m_HPback;			//HPゲージの背景

	AnimationClip		m_animationClips[enAnimationClip_Num];	//アニメーションクリップ
	ModelRender			m_modelRender;							//モデルレンダー
	Vector3				m_position = Vector3::Zero;				//座標
	Vector3				m_firstPosition = Vector3::Zero;		//初期座標
	Vector3				m_moveSpeed = Vector3::Zero;;			//移動速度
	Vector3				m_forward = Vector3::AxisZ;				//ボスの正面ベクトル
	Quaternion			m_rotation = Quaternion::Identity;		//回転
	Quaternion			m_firstRotation = Quaternion::Identity;	//初期回転
	Vector3				m_scale = Vector3::One;					//大きさ
	CharacterController	m_charCon;								//キャラコン
	EnBossState			m_BossState = enBossState_Idle;			//ボスのステート
	bool				m_isUnderAttack = false;				//攻撃中か？
	int					m_maxHP = 0;							//最大HP
	int					m_hp = 0;								//HP
	int					m_swordBoneId = -1;						//ボスの剣のボーンID
	Player*				m_player = nullptr;						//プレイヤー
	float				m_chaseTimer = 0.0f;					//追跡タイマー
	float				m_idleTimer = 0.0f;						//待機タイマー
	Vector3 m_magicOldPos = Vector3::Zero;
};

