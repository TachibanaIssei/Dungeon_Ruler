#pragma once

class Game;
class Fade;
class MovingFloor;
class CheckPoint;

class Player : public IGameObject
{
public:
	//プレイヤーのステート
	enum EnPlayerState {
		enPlayerState_Idle,			//待機状態
		enPlayerState_Walk,			//歩き
		enPlayerState_Run,			//走る
		enPlayerState_Jump,			//ジャンプ
		enPlayerState_Attack,		//近接攻撃
		enPlayerState_Magic,		//魔法攻撃
		enPlayerState_ReceiveDamage,//被ダメージ
		enPlayerState_Down,			//ダウン
		enPlayerState_Clear			//クリア
	};

	Player();
	~Player();
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
		m_respawnPosition = position;
		m_modelRender.SetPosition(m_position);
	}

	/// <summary>
	/// 座標を取得
	/// </summary>
	/// <returns>座標</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}

	/// <summary>
	/// 回転を取得
	/// </summary>
	/// <returns>回転</returns>
	const Quaternion& GetRotation() const
	{
		return m_rotationZ;
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
	/// 回転を設定
	/// </summary>
	/// <param name="rotation">回転</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_respawnRotation = rotation;
		m_modelRender.SetRotation(m_rotation);
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
		m_modelRender.SetScale(m_scale);
	}

	/// <summary>
	/// リスポーンポジションを設定
	/// </summary>
	/// <param name="position">リスポーンポジション</param>
	/// <param name="i">配列の番号</param>
	void SetinRespawnPosition(const Vector3& position,int i)
	{
		m_inRespawnPosition[i] = position;
	}

	/// <summary>
	/// 行動制限の座標を設定
	/// </summary>
	/// <param name="position">範囲の座標</param>
	/// <param name="i">配列の番号</param>
	void SetinMoveLimitPosition(const Vector3& position, int i)
	{
		m_moveLimit[i] = position;
	}

	/// <summary>
	/// 動ける状態(ステート)かどうかを取得
	/// </summary>
	/// <returns>動ける状態ならtrue</returns>
	bool IsEnableMove() const
	{
		return m_playerState != enPlayerState_Attack &&
			m_playerState != enPlayerState_Magic &&
			m_playerState != enPlayerState_ReceiveDamage &&
			m_playerState != enPlayerState_Down &&
			m_playerState != enPlayerState_Clear;
	}

	/// <summary>
	/// プレイヤーリスポーン
	/// </summary>
	void PlayerRespawn();

	/// <summary>
	/// サンダー魔法の当たりフラグをリセットする
	/// </summary>
	void ResetThunderHit()
	{
		m_thunderHit = false;
	}

	/// <summary>
	/// マッシュルームの攻撃の当たりフラグをリセットする
	/// </summary>
	void ResetMushAttackHit()
	{
		m_mushAttackHit = false;
	}

	/// <summary>
	/// プレイヤーの近接攻撃のエフェクトを再生する
	/// </summary>
	void isPlayerAttackHit()
	{
		MakeSlashingEffect();
	}

	/// <summary>
	/// プレイヤーが死亡している？
	/// </summary>
	/// <returns>HPが0以下ならtrue</returns>
	const bool isPlayerDead() const
	{
		return m_hp <= 0;
	}

	/// <summary>
	/// 画像表示フラグを取得する
	/// </summary>
	/// <returns>falseなら表示しない</returns>
	const bool GetSpriteFlag() const
	{
		return m_spriteFlag;
	}

	/// <summary>
	/// ボス部屋フラグを取得する
	/// </summary>
	/// <returns>falseなら表示しない</returns>
	const bool GetBossRoomFlag() const
	{
		return m_bossRoomFlag;
	}

	/// <summary>
	/// 目的の番号を取得する
	/// </summary>
	/// <returns>目的の番号(1.すべての敵を倒す,2.奥に進む,3.支配者を倒す)</returns>
	const int GetMokutekiFlag()
	{
		return m_mokutekiFlag;
	}

private:
	/// <summary>
	/// オブジェクトを見つける
	/// </summary>
	void FindObject();
	/// <summary>
	/// 動く床の移動速度を加算する
	/// </summary>
	void AddMoveSpeed();
	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();
	/// <summary>
	/// 行動制限
	/// </summary>
	void LimitMove();
	/// <summary>
	/// 回転処理
	/// </summary>
	void Turn();
	/// <summary>
	/// アニメーションの再生
	/// </summary>
	void PlayAnimation();
	/// <summary>
	/// 各ステートの遷移処理
	/// </summary>
	void ManageState();
	/// <summary>
	/// アニメーションイベント用の関数
	/// </summary>
	/// <param name="clipName">アニメーションの名前</param>
	/// <param name="eventName">アニメーションイベントキーの名前</param>
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);
	/// <summary>
	/// 敵との当たり判定処理
	/// </summary>
	void Collision();
	/// <summary>
	/// 攻撃中の処理
	/// </summary>
	void Attack();
	/// <summary>
	/// 攻撃の際の当たり判定用のコリジョンを作成する
	/// </summary>
	void MakeAttackCollision();
	/// <summary>
	/// 攻撃した際のエフェクトを作成する
	/// </summary>
	void MakeSlashingEffect();
	/// <summary>
	/// 魔法攻撃ヒットのエフェクトを再生する
	/// </summary>
	void MakeMagicHitEffect();
	/// <summary>
	/// 魔法攻撃を作成する
	/// </summary>
	void MakeMagicBall();
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
	/// 走りステートの遷移処理
	/// </summary>
	void ProcessRunStateTransition();
	/// <summary>
	/// 近接攻撃ステートの遷移処理
	/// </summary>
	void ProcessAttackStateTransition();
	/// <summary>
	/// 魔法攻撃ステートの遷移処理
	/// </summary>
	void ProcessMagicStateTransition();
	/// <summary>
	/// ジャンプステートの遷移処理
	/// </summary>
	void ProcessJumpStateTransition();
	/// <summary>
	/// 被ダメージステートの遷移処理
	/// </summary>
	void ProcessReceiveDamageStateTransition();
	/// <summary>
	/// 死亡ステートの遷移処理
	/// </summary>
	void ProcessDownStateTransition();
	/// <summary>
	/// クリアステートの遷移処理
	/// </summary>
	void ProcessClearStateTransition();
	/// <summary>
	/// HPバーの表示
	/// </summary>
	void HPBar();
	/// <summary>
	/// MPバーの表示
	/// </summary>
	void MPBar();

	//アニメーション
	enum EnAnimationClip {
		enAnimationClip_Idle,
		enAnimationClip_Walk,
		enAnimationClip_Run,
		enAnimationClip_Jump,
		enAnimationClip_Attack,
		enAnimationClip_Magic,
		enAnimationClip_ReceiveDamage,
		enAnimationClip_Down,
		enAnimationClip_Clear,
		enAnimationClip_Num,
	};

	Game*			m_game = nullptr;
	Fade*			m_fade = nullptr;
	MovingFloor*	m_movingFloorX		= nullptr;
	CheckPoint*		m_checkPoint		= nullptr;

	AnimationClip			m_animationClips[enAnimationClip_Num];							//アニメーションクリップ
	ModelRender				m_modelRender;													//モデルレンダー
	Vector3					m_position								= Vector3::Zero;		//座標
	Vector3					m_respawnPosition						= Vector3::Zero;		//リスポーン座標
	Vector3					m_inRespawnPosition[6];											//6つのリスポーンポジションを管理する
	Vector3					m_moveLimit[2]			= {Vector3::Zero,Vector3::Zero};		//制限する移動範囲を保持
	Vector3					m_moveSpeed								= Vector3::Zero;		//移動速度
	Vector3					m_oldMoveSpeed							= Vector3::Zero;		//移動速度を保持する
	Vector3					m_movingFloorSpeed						= Vector3::Zero;		//動く床の移動する方向を取る変数
	Vector3					m_forward								= Vector3::AxisZ;		//プレイヤーの正面ベクトル
	Vector3					m_scale									= Vector3::One;			//大きさ
	Quaternion				m_rotation								= Quaternion::Identity;	//クォータニオン
	Quaternion				m_rotationZ								= Quaternion::Identity;	//Z軸周りのクォータニオン
	Quaternion				m_respawnRotation						= Quaternion::Identity;	//リスポーンクォータニオン
	CharacterController		m_charCon;														//キャラクターコントローラー
	EnPlayerState			m_playerState							= enPlayerState_Idle;	//プレイヤーステート
	SpriteRender			m_hpBar;														//HPバーの画像
	SpriteRender			m_mpBar;														//MPバーの画像
	SpriteRender			m_statusBar;													//ステータスの画像
	SpriteRender			m_playerFaceFrame;												//プレイヤーの顔画像の枠
	SpriteRender			m_playerFaceBack;												//プレイヤーの顔画像の背景
	SpriteRender			m_playerFace;													//プレイヤーの顔
	int						m_swordBoneId							= -1;					//剣のボーンID
	int						m_hp = 0;														//現在のプレイヤーHP
	int						m_mp = 0;														//現在のプレイヤーMP
	int						checkPointCount							= 0;					//通過したチェックポイントの数を数える
	int						m_mokutekiFlag							= 1;					//目的画像の番号
	bool					m_isUnderAttack							= false;				//攻撃ならtrue
	bool					m_onMovingFloor							= false;				//動く床にいたらtrue
	bool					m_thunderHit							= false;				//サンダー魔法が当たったらtrue
	bool					m_mushAttackHit							= false;				//マッシュの攻撃が当たったらtrue
	bool					m_bossRoomFlag							= false;				//ボスのいる場所に行くとtrue
	bool					m_spriteFlag							= true;					//クリアしたらfalse
	float					m_playerSpeed							= 120.0f;				//プレイヤーの移動速度変更
	float					m_mpHealTimer							= 0.0f;					//MP回復の時間計測
	float					m_fireDamageTimer						= 0.0f;					//炎ギミックダメージのクールタイム用のタイマー
};

