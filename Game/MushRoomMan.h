#pragma once

class Player;
class Map;
class GameCamera;

class MushRoomMan : public IGameObject
{
public:
	//マッシュルームマンステート
	enum EnMushState {
		enMushState_Idle,
		enMushState_Chase,
		enMushState_Attack,
		enMushState_ReceiveDamage,
		enMushState_Down,
	};
public:
	MushRoomMan();
	~MushRoomMan();
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
	}
	/// <summary>
	/// 大きさを設定
	/// </summary>
	/// <param name="scale">大きさ</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	/// <summary>
	/// キャラコンを取得
	/// </summary>
	/// <returns>キャラクターコントローラー</returns>
	CharacterController& GetCharacterController()
	{
		return m_charCon;
	}

	/// <summary>
	/// HP関連画像の座標を設定
	/// </summary>
	/// <param name="number">名前表示の番号</param>
	void SetHPSpritePosition(int number)
	{
		m_nameNumber = number;
	}

	/// <summary>
	/// マップ上の画像の動作
	/// </summary>
	/// <param name="rc"></param>
	void MushMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_mushMapSprite.Draw(rc);
		}
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
	/// 魔法攻撃ヒットエフェクト
	/// </summary>
	void MakeMagicHitEffect();
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
	/// 攻撃のエフェクトを再生する
	/// </summary>
	void MakeAttackEffect();

	/// <summary>
	/// HPのゲージの増減
	/// </summary>
	void HPBar();

	/// <summary>
	/// ゲージを左寄せする処理
	/// </summary>
	/// <param name="size">画像の元の大きさ</param>
	/// <param name="scale">現在のスケール倍率</param>
	/// <returns>変換前と変換後の差</returns>
	Vector3 HPBerSend(Vector3 size, Vector3 scale);
	
	/// <summary>
	/// HPゲージの描画フラグ
	/// </summary>
	/// <returns>描画できる範囲にあるときtrue</returns>
	bool DrawHP();

	/// <summary>
	/// マップの画像移動処理
	/// </summary>
	void MapMove();

	enum EnAnimationClip {			//アニメーション
		enAnimationClip_Idle,		//待機アニメーション
		enAnimationClip_Walk,		//歩きアニメーション
		enAnimationClip_Attack,		//攻撃アニメーション
		enAnimationClip_Damage,		//被ダメージアニメーション
		enAnimationClip_Down,		//死亡アニメーション
		enAnimationClip_Num,		//アニメーションの数
	};

	SpriteRender		m_HPBar;		//HPバー画像
	SpriteRender		m_HPFrame;		//HP枠画像
	SpriteRender		m_HPBack;		//HP背景画像
	SpriteRender		m_mushMapSprite;//マップ表示

	AnimationClip		m_animationClips[enAnimationClip_Num];	//アニメーションクリップ
	ModelRender			m_modelRender;							//モデルレンダー

	Vector2				m_HPBerPos = Vector2::Zero;				//HPバーのポジション
	Vector2				m_HPWindowPos = Vector2::Zero;			//HP枠のポジション
	Vector2				m_HPBackPos = Vector2::Zero;			//HP背景のポジション

	Vector3				m_position = Vector3::Zero;				//座標
	Vector3				m_firstPosition = Vector3::Zero;		//スタート時の座標
	Vector3				m_moveSpeed = Vector3::Zero;			//移動速度
	Vector3				m_forward = Vector3::AxisZ;				//マッシュルームの正面ベクトル

	Quaternion			m_rotation = Quaternion::Identity;		//回転
	Quaternion			m_firstRotation = Quaternion::Identity;	//初期回転
	Vector3				m_scale = Vector3::One;					//大きさ
	CharacterController	m_charCon;								//キャラコン
	EnMushState			m_mushState = enMushState_Idle;			//マッシュルームのステート
	bool				m_isUnderAttack = false;				//攻撃中か？
	bool				m_isMapImage = false;					//マップに表示するときtrue
	int					m_maxHP = 0;							//最大HP
	int					m_hp = 0;								//HP
	int					m_nameNumber = 0;						//名前表示の番号
	Player*				m_player = nullptr;						//プレイヤー
	Map*				m_map = nullptr;						//マップ
	GameCamera*			m_gameCamera = nullptr;					//ゲームカメラ
	float				m_chaseTimer = 0.0f;					//追跡タイマー
	float				m_idleTimer = 0.0f;						//待機タイマー
};

