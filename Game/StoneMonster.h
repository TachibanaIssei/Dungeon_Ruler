#pragma once

class Player;
class Map;
class GameCamera;

class StoneMonster : public IGameObject
{
public:
	enum EnStoneState {
		enStoneState_Idle,
		enStoneState_Chase,
		enStoneState_Magic,
		enStoneState_ReceiveDamage,
		enStoneState_Down,
	};

public:
	StoneMonster() {};
	~StoneMonster();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
		m_firstPosition = position;
	}
	/// <summary>
	/// 座標を取得
	/// </summary>
	/// <returns></returns>
	const Vector3 GetPosition() const
	{
		return m_position;
	}
	//HPを設定
	void SetHP(const int hp)
	{
		m_hp = hp;
		m_maxHP = hp;
	}
	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// 大きさを設定
	/// </summary>
	/// <param name="scale"></param>
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
	void StoneMap(RenderContext& rc)
	{
		if (m_isMapImage)
		{
			m_stoneMapSprite.Draw(rc);
		}
	}

	/// <summary>
	/// リスタート処理
	/// </summary>
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
	/// プレイヤーの攻撃との当たり判定処理
	/// </summary>
	void Collision();
	/// <summary>
	/// プレイヤーを探索
	/// </summary>
	const bool SearchPlayer() const;
	/// <summary>
	/// 魔法攻撃を作成
	/// </summary>
	void MakeMagicBall();
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
	/// 共通のステートの遷移処理
	/// </summary>
	void ProcessCommonStateTransition();
	/// <summary>
	/// 待機ステートの遷移処理
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// 走りステートの遷移処理
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// 追跡ステートの遷移処理
	/// </summary>
	void ProcessChaseStateTransition();
	/// <summary>
	/// 魔法攻撃ステートの遷移処理
	/// </summary>
	void ProcessMagicStateTransition();
	/// <summary>
	/// 被ダメージステートの遷移処理
	/// </summary>
	void ProcessReceiveDamageTransition();
	/// <summary>
	/// 死亡ステートの遷移処理
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// 攻撃できる距離かどうか調べる
	/// </summary>
	/// <returns></returns>
	const bool IsCanAttack() const;
	/// <summary>
	/// HPゲージを増減する
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
	/// <returns>描画できる範囲にいるときtrue</returns>
	bool DrawHP();
	/// <summary>
	/// マップの画像移動処理
	/// </summary>
	void MapMove();

	enum EnAnimationClip {
		enAnimationClip_Idle,			//待機アニメーション
		enAnimationClip_Run,			//走るアニメーション
		enAnimationClip_Magic,			//攻撃アニメーション
		enAnimationClip_Damage,			//被ダメージアニメーション
		enAnimationClip_Down,			//死亡アニメーション
		enAnimationClip_Num				//アニメーションの数
	};

	SpriteRender			m_HPBar;								//HPバー画像
	SpriteRender			m_HPFrame;								//HP枠画像
	SpriteRender			m_HPBack;								//HP背景画像
	SpriteRender			m_stoneMapSprite;						//マップ表示


	AnimationClip			m_animationClips[enAnimationClip_Num];	//アニメーションクリップ
	ModelRender				m_modelRender;							//モデルレンダー

	Vector2					m_HPBerPos = Vector2::Zero;				//HPバーのポジション
	Vector2					m_HPWindowPos = Vector2::Zero;			//HP枠のポジション
	Vector2					m_HPBackPos = Vector2::Zero;			//HP背景のポジション

	Vector3					m_position = Vector3::Zero;				//座標
	Vector3					m_firstPosition = Vector3::Zero;		//スタート時の座標
	Vector3					m_moveSpeed;							//移動速度
	Vector3					m_forward = Vector3::AxisZ;				//ストーンモンスターの正面ベクトル
	Quaternion				m_rotation;								//回転
	Quaternion				m_firstRotation = Quaternion::Identity;	//初期回転
	Vector3					m_scale = Vector3::One;					//大きさ
	CharacterController		m_charCon;								//キャラコン
	EnStoneState			m_stoneState = enStoneState_Idle;		//ストーンモンスターのステート
	bool					m_isMapImage = false;					//マップ表示するときtrue
	int						m_maxHP = 0;							//最大HP
	int						m_hp = 0;								//ストーンモンスターのHP
	int						m_nameNumber = 0;						//名前表示の番号
	Player*					m_player = nullptr;						//プレイヤー
	Map*					m_map = nullptr;						//マップ
	GameCamera*				m_gameCamera = nullptr;					//ゲームカメラ
	float					m_chaseTimer = 0.0f;					//追跡タイマー
	float					m_idleTimer = 0.0f;						//待機タイマー
};

