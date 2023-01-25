#include "stdafx.h"
#include "Player.h"

#include "Game.h"
#include "GameCamera.h"
#include "Fade.h"
#include "MagicBall.h"
#include "MovingFloor.h"
#include "MovingFloorZ.h"
#include "SeesawFloor.h"
#include "CheckPoint.h"
#include "Fade.h"

#include "collision/CollisionObject.h"
#include "graphics/effect/EffectEmitter.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

namespace
{
	const int PLAYER_MAXHP = 1;	//プレイヤーの最大HP
	const int PLAYER_MAXMP = 100;	//プレイヤーの最大MP
	const int PLAYER_LOSTMP = 33;	//魔法を使ったときに減少するMP
	const int PLAYER_HEALMP = 5;	//MPの自動回復

	const float JUMP_POWER = 280.0f;	//ジャンプ力

	const float SE_VOLUME = 1.0f;
	const float FIREHIT_SE_VOLUME = 0.8f;		//炎ギミックの効果音ボリューム
	const float MUSHATTACK_SE_VOLUME = 1.2f;	//マッシュルーム攻撃ヒットSEボリューム
	const float DEAD_VOICE_VOLUME = 1.5f;		//死亡時の声のボリューム

	const int	LOST_HP_MUSH_ATTACK = 10;		//マッシュルームの攻撃のダメージ
	const int	LOST_HP_STONE_ATTACK = 10;		//ストーンモンスターの攻撃のダメージ
	const int	LOST_HP_BOSS_ATTACK = 20;		//ボスの近接攻撃のダメージ
	const int	LOST_HP_BOSS_MAGIC = 30;		//ボスの魔法攻撃のダメージ
	const float LOST_HP_FIRE = 0.8f;			//炎ギミックのダメージ

	const Vector2 GAUGE_PIVOT			= Vector2(0.0f, 0.5f);				//ゲージのピボット
	const Vector3 STATUS_BAR_POS		= Vector3(-393.0f, -407.0f, 0.0f);	//ステータスバーポジション
	const Vector3 PLAYER_FACE_BAR_POS	= Vector3(-580.0, -418.0f, 0.0f);	//プレイヤーの顔の枠のポジション
	const Vector3 HP_BAR_POS			= Vector3(-726.0f, -375.0f, 0.0f);	//HPバーポジション
	const Vector3 MP_BAR_POS			= Vector3(-720.0, -446.0, 0.0f);	//MPバーポジション

}

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	//アニメーションをロードする
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/player/playerIdle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Walk].Load("Assets/animData/player/playerWalk.tka");
	m_animationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/player/playerRun.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Jump].Load("Assets/animData/player/playerJump.tka");
	m_animationClips[enAnimationClip_Jump].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Attack].Load("Assets/animData/player/playerAttack.tka");
	m_animationClips[enAnimationClip_Attack].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Magic].Load("Assets/animData/player/playerMagicAttack.tka");
	m_animationClips[enAnimationClip_Magic].SetLoopFlag(false);
	m_animationClips[enAnimationClip_ReceiveDamage].Load("Assets/animData/player/playerReceiveDamage.tka");
	m_animationClips[enAnimationClip_ReceiveDamage].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Down].Load("Assets/animData/player/playerDown.tka");
	m_animationClips[enAnimationClip_Down].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Clear].Load("Assets/animData/player/playerVictory.tka");
	m_animationClips[enAnimationClip_Clear].SetLoopFlag(true);

	//キャラクターモデルの読み込み
	m_modelRender.Init("Assets/modelData/player/player.tkm",m_animationClips,enAnimationClip_Num, enModelUpAxisZ);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);

	//剣のボーンを探す
	m_swordBoneId = m_modelRender.FindBoneID(L"mixamorig:Sword_joint");

	//HPゲージの画像を読み込む
	m_statusBar.Init("Assets/sprite/HP_gauge.dds", 744, 244);
	m_statusBar.SetPosition(STATUS_BAR_POS);
	m_statusBar.SetScale(g_vec3One);

	m_playerFaceFrame.Init("Assets/sprite/HP_window.dds", 744, 244);
	m_playerFaceFrame.SetPosition(PLAYER_FACE_BAR_POS);
	m_playerFaceFrame.SetScale(g_vec3One);

	m_playerFaceBack.Init("Assets/sprite/HP_windowBack.dds", 744, 244);
	m_playerFaceBack.SetPosition(PLAYER_FACE_BAR_POS);
	m_playerFaceBack.SetScale(g_vec3One);

	//HPゲージの画像読み込み
	m_hpBar.Init("Assets/sprite/uf_bar_b_health.dds", 565,58.8);
	//ピボットを設定する
	m_hpBar.SetPivot(GAUGE_PIVOT);
	m_hpBar.SetPosition(HP_BAR_POS);

	//MPゲージの画像読み込み
	m_mpBar.Init("Assets/sprite/uf_bar_sm_mana.dds", 432.3, 34.06);
	//ピボットを設定する
	m_mpBar.SetPivot(GAUGE_PIVOT);
	m_mpBar.SetPosition(MP_BAR_POS);

	//更新処理
	m_statusBar.Update();
	m_hpBar.Update();
	m_hp = PLAYER_MAXHP;
	m_mpBar.Update();
	m_mp = PLAYER_MAXMP;
	m_playerFaceFrame.Update();
	m_playerFaceBack.Update();

	//アニメーションイベント用の関数を設定する
	m_modelRender.AddAnimationEvent([&](const wchar_t*
		clipName, const wchar_t* eventName) {
			OnAnimationEvent(clipName, eventName);
		});

	m_charCon.Init(
		15.0f,
		35.0f,
		m_position
	);

	//エフェクトのロード
	EffectEngine::GetInstance()->ResistEffect(0, u"Assets/effect/playerSword_slash.efk");
	EffectEngine::GetInstance()->ResistEffect(9, u"Assets/effect/efk/cast_wind.efk");
	EffectEngine::GetInstance()->ResistEffect(10, u"Assets/effect/magicfire_hit.efk");
	EffectEngine::GetInstance()->ResistEffect(10, u"Assets/effect/magicfire_hit.efk");
	
	//音のロード
	g_soundEngine->ResistWaveFileBank(3,"Assets/sound/sword_slash_shan.wav");
	g_soundEngine->ResistWaveFileBank(4, "Assets/sound/magic_circle_sound.wav");
	g_soundEngine->ResistWaveFileBank(7, "Assets/sound/hitheavy_sound_bogo.wav");
	g_soundEngine->ResistWaveFileBank(9, "Assets/sound/hit_sound_beshi.wav");
	g_soundEngine->ResistWaveFileBank(12, "Assets/sound/fire_hit_yakedo.wav");
	g_soundEngine->ResistWaveFileBank(14, "Assets/sound/otoko_voice_guha.wav");
	g_soundEngine->ResistWaveFileBank(16, "Assets/sound/button_sound_piroron.wav");
	g_soundEngine->ResistWaveFileBank(17, "Assets/sound/magic_huhatu.wav");

	m_game = FindGO<Game>("game");

	m_movingFloorX = FindGO<MovingFloor>("movingfloorX");
	m_checkPoint = FindGO<CheckPoint>("checkPoint");

	return true;
}

void Player::Update()
{
	FindObject();
	Move();
	LimitMove();
	Turn();
	Collision();
	Attack();
	PlayAnimation();
	ManageState();
	HPBar();
	MPBar();

	m_modelRender.Update();
}

void Player::FindObject()
{
	if (m_game == nullptr)
	{
		m_game = FindGO<Game>("game");
		return;
	}

	if (m_movingFloorX == nullptr)
	{
		m_movingFloorX = FindGO<MovingFloor>("movingfloorX");
		return;
	}
}

void Player::AddMoveSpeed()
{
	//X軸に動く床の上にいたら床の移動速度を加算する
	if (m_movingFloorX->GetFlag() == true)
	{
		m_oldMoveSpeed += m_movingFloorX->GetMoveSpeed();
	}

	//Z軸に動く床の上にいたら床の移動速度を加算する
	const auto& movingFloorZs = FindGOs<MovingFloorZ>("movingfloorZ");
	for (auto movingFloorZ : movingFloorZs)
	{
		if (movingFloorZ->GetFlag())
		{
			m_oldMoveSpeed += movingFloorZ->GetMoveSpeed();
		}
	}

	//回転する床の上にいたら
	const auto& seesawFloors = FindGOs<SeesawFloor>("seesawfloor");
	for (auto seesawFloor : seesawFloors)
	{
		if (seesawFloor->GetFlag())
		{
			if (seesawFloor->GetFloorNumber() == 1)
			{
				float rot = seesawFloor->GetRotation() * 500.0f;
				m_oldMoveSpeed.z += rot;
			}
			else if (seesawFloor->GetFloorNumber() == 2)
			{
				float rot = seesawFloor->GetRotation() * 550.0f;
				m_oldMoveSpeed.z += rot;
			}
		}
	}
}

void Player::Move()
{
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;
	m_oldMoveSpeed.x = m_moveSpeed.x;
	m_oldMoveSpeed.z = m_moveSpeed.z;

	//移動できない状態のときは、処理しない
	if (IsEnableMove() == false)
	{
		m_moveSpeed.y -= 980.0f * 1.0f/60.0f;

		m_oldMoveSpeed = m_moveSpeed;

		AddMoveSpeed();

		m_position = m_charCon.Execute(m_oldMoveSpeed, 1.0f / 60.0f);

		if (m_charCon.IsOnGround()) {
			//地面についた
			m_moveSpeed.y = 0.0f;
		}

		m_modelRender.SetPosition(m_position);
		return;
	}

	if (g_pad[0]->IsPress(enButtonRB1))
	{
		m_playerSpeed = 200.0f;
	}
	else
	{
		m_playerSpeed = 120.0f;
	}

	//左スティックの入力量を受け取る
	float lStick_x = g_pad[0]->GetLStickXF();
	float lStick_y = g_pad[0]->GetLStickYF();
	//カメラの前方向と右方向を取得
	Vector3 cameraForward = g_camera3D->GetForward();
	Vector3 cameraRight = g_camera3D->GetRight();
	//XZ平面での前方方向、右方向に変換する
	cameraForward.y = 0.0f;
	cameraForward.Normalize();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();
	//XZ成分の移動速度をクリア
	m_moveSpeed += cameraForward * lStick_y * m_playerSpeed;
	m_moveSpeed += cameraRight * lStick_x * m_playerSpeed;
	
	if (g_pad[0]->IsTrigger(enButtonA)	//Aボタンが押されたら
		&& m_charCon.IsOnGround()		//かつ、地上にいたら
		) {
		//ジャンプする
		m_moveSpeed.y = JUMP_POWER;
	}

	m_moveSpeed.y -= 980.0f * g_gameTime->GetFrameDeltaTime();

	m_oldMoveSpeed = m_moveSpeed;

	AddMoveSpeed();

	//キャラクターコントローラーを使用して、座標を更新
	m_position = m_charCon.Execute(m_oldMoveSpeed, g_gameTime->GetFrameDeltaTime());
	
	if (m_charCon.IsOnGround()) {
		//地面についた
		m_moveSpeed.y = 0.0f;
	}

	Vector3 modelPosition = m_position;
	m_modelRender.SetPosition(modelPosition);
}

void Player::LimitMove()
{
	if (m_game->IsWannhilationZako())
	{
		if (m_position.z <= m_moveLimit[0].z)
		{
			m_position.z = m_moveLimit[0].z;
			m_charCon.SetPosition(m_position);
			m_modelRender.SetPosition(m_position);
		}
	}

	else if (!m_game->IsWannhilationZako() && m_mokutekiFlag < 3)
	{
		m_mokutekiFlag = 2;
	}

	if (m_bossRoomFlag == true)
	{
		if (m_position.z >= m_moveLimit[1].z)
		{
			m_position.z = m_moveLimit[1].z;
			m_charCon.SetPosition(m_position);
			m_modelRender.SetPosition(m_position);
		}
	}
}

void Player::Turn()
{
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001f) {
		return;
	}

	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);
	m_rotation.SetRotationY(-angle);

	//マップ表示用の回転
	float angleMap = atan2(m_moveSpeed.x, -m_moveSpeed.z);
	m_rotationZ.SetRotationZ(angleMap);

	m_modelRender.SetRotation(m_rotation);

	//プレイヤーの正面ベクトルを計算する
	m_forward = Vector3::AxisZ;
	m_rotation.Apply(m_forward);
}

void Player::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	//近接攻撃
	if (wcscmp(eventName, L"attack_start") == 0)
	{
		m_isUnderAttack = true;
	}
	else if (wcscmp(eventName, L"attack_end") == 0)
	{
		m_isUnderAttack = false;
	}

	//魔法攻撃
	if (wcscmp(eventName, L"magic_attack") == 0)
	{
		//魔法攻撃を作成
		MakeMagicBall();

	}
}

void Player::Collision()
{
	//被ダメージ、ダウン中はダメージ判定をしない
	if (m_playerState == enPlayerState_ReceiveDamage ||
		m_playerState == enPlayerState_Down)
	{
		return;
	}

	{
		//敵の攻撃用のコリジョンの配列を取得する
		const auto& collisions =
			g_collisionObjectManager->FindCollisionObjects("mush_attack");
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら
			if (collision->IsHit(m_charCon))
			{
				//攻撃に重複して当たらないようにする
				if (m_mushAttackHit == true)
				{
					return;
				}

				m_mushAttackHit = true;

				//HPを減らす
				m_hp -= LOST_HP_MUSH_ATTACK;

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(7);
				se->SetVolume(MUSHATTACK_SE_VOLUME);
				se->Play(false);

				//HPが0になったら
				if (m_hp <= 0)
				{
					//ダウンさせる
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UIの表示
					m_spriteFlag = false;
				}
				//HPが0ではなかったら
				else
				{
					m_playerState = enPlayerState_ReceiveDamage;
				}
			}
		}
	}

	{
		//敵の攻撃用のコリジョンの配列を取得する
		const auto& collisions =
			g_collisionObjectManager->FindCollisionObjects("Boss_attack");
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら
			if (collision->IsHit(m_charCon))
			{
				//HPを減らす
				m_hp -= LOST_HP_BOSS_ATTACK;

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(3);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HPが0になったら
				if (m_hp <= 0)
				{
					//ダウンさせる
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UIの表示
					m_spriteFlag = false;
				}
				//HPが0ではなかったら
				else
				{
					m_playerState = enPlayerState_ReceiveDamage;
				}
			}
		}
	}

	{
		//敵のマジックボール用のコリジョンを取得する
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_magicball");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charCon))
			{
				//HPを減らす
				m_hp -= LOST_HP_STONE_ATTACK;

				//魔法攻撃ヒットエフェクトの再生
				MakeMagicHitEffect();

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HPが0になったら
				if (m_hp <= 0)
				{
					//死亡ステートに遷移する
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UIの表示
					m_spriteFlag = false;
				}
				else
				{
					//被ダメージステートに遷移
					m_playerState = enPlayerState_ReceiveDamage;
				}
				return;
			}
		}
	}

	{
		//敵のマジックサンダー用のコリジョンを取得する
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("enemy_magicThunder");
		for (auto collision : collisions)
		{
			if (collision->IsHit(m_charCon))
			{
				//攻撃に重複して当たらないようにする
				if (m_thunderHit == true)
				{
					return;
				}

				m_thunderHit = true;

				//HPを減らす
				m_hp -= LOST_HP_BOSS_MAGIC;

				SoundSource* se = NewGO<SoundSource>(0);
				se->Init(9);
				se->SetVolume(SE_VOLUME);
				se->Play(false);

				//HPが0になったら
				if (m_hp <= 0)
				{
					//死亡ステートに遷移する
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UIの表示
					m_spriteFlag = false;
				}
				else
				{
					//被ダメージステートに遷移
					m_playerState = enPlayerState_ReceiveDamage;
				}
				return;
			}
		}
	}

	{
		//炎ギミックのコリジョンを取得する
		const auto& collisions =
			g_collisionObjectManager->FindCollisionObjects("fire_gimmic");
		for (auto collision : collisions)
		{
			//コリジョンとキャラコンが衝突したら
			if (collision->IsHit(m_charCon))
			{
				
				m_fireDamageTimer += g_gameTime->GetFrameDeltaTime();
				if (m_fireDamageTimer >= 0.5)
				{
					//HPを減らす
					m_hp -= LOST_HP_FIRE;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(12);
					se->SetVolume(FIREHIT_SE_VOLUME);
					se->Play(false);

					m_fireDamageTimer = 0.0f;
				}

				//HPが0になったら
				if (m_hp <= 0)
				{
					//ダウンさせる
					m_playerState = enPlayerState_Down;

					SoundSource* se = NewGO<SoundSource>(0);
					se->Init(14);
					se->SetVolume(DEAD_VOICE_VOLUME);
					se->Play(false);

					//UIの表示
					m_spriteFlag = false;
				}
			}
		}
	}

	//チェックポイント通過したときの処理
	PhysicsWorld::GetInstance()->ContactTest
		(m_charCon,[&](const btCollisionObject& contactObject)
		{
				//1つ目のチェックポイント通過
				if (m_checkPoint->m_physicsGhostObject[0].IsSelf(contactObject) == true && checkPointCount < 1)
				{
					m_respawnPosition = m_inRespawnPosition[0];
					checkPointCount = 1;
				}
				//2つ目
				else if (m_checkPoint->m_physicsGhostObject[1].IsSelf(contactObject) == true && checkPointCount < 2)
				{
					m_respawnPosition = m_inRespawnPosition[1];
					checkPointCount = 2;
				}
				//3つ目
				else if (m_checkPoint->m_physicsGhostObject[2].IsSelf(contactObject) == true && checkPointCount < 3)
				{
					m_respawnPosition = m_inRespawnPosition[2];
					checkPointCount = 3;
				}
				//4つ目
				else if (m_checkPoint->m_physicsGhostObject[3].IsSelf(contactObject) == true && checkPointCount < 4)
				{
					m_respawnPosition = m_inRespawnPosition[3];
					checkPointCount = 4;
				}
				//5つ目
				else if (m_checkPoint->m_physicsGhostObject[4].IsSelf(contactObject) == true && checkPointCount < 5)
				{
					m_respawnPosition = m_inRespawnPosition[4];
					checkPointCount = 5;
				}
				//6つ目
				else if (m_checkPoint->m_physicsGhostObject[5].IsSelf(contactObject) == true && checkPointCount < 6)
				{
					m_respawnPosition = m_inRespawnPosition[5];
					checkPointCount = 6;
					m_bossRoomFlag = true;
					m_mokutekiFlag = 3;
				}
		});
}

void Player::Attack()
{
	//攻撃中でないなら、処理をしない
	if (m_playerState != enPlayerState_Attack)
	{
		return;
	}

	//攻撃判定中であれば
	if (m_isUnderAttack == true)
	{
		MakeAttackCollision();
	}
}

void Player::MakeAttackCollision()
{
	//コリジョンオブジェクトを作成
	auto collisionObject = NewGO<CollisionObject>(0);
	Matrix matrix = m_modelRender.GetBone(m_swordBoneId)->GetWorldMatrix();
	collisionObject->CreateBox(
		m_position,
		Quaternion::Identity,
		Vector3(55.0f,10.0f,10.0f)
	);
	collisionObject->SetWorldMatrix(matrix);
	collisionObject->SetName("player_attack");
}

void Player::MakeSlashingEffect()
{
	//エフェクト
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Vector3 flont = Vector3::AxisZ;
	Quaternion rotation;

	effectEmitter->Init(0);
	effectEmitter->SetScale(Vector3::One * 20.0f);
	effectPosition.y += 50.0f;
	rotation = m_rotation;
	m_rotation.Apply(flont);
	flont *= 50.0f;
	effectPosition += flont;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);

	//SE
	SoundSource* se = NewGO<SoundSource>(0);
	se->Init(3);
	se->SetVolume(SE_VOLUME);

	//再生
	effectEmitter->Play();
	se->Play(false);

}

void Player::MakeMagicHitEffect()
{
	EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
	Vector3 effectPosition = m_position;
	Quaternion rotation;

	effectEmitter->Init(10);
	effectEmitter->SetScale(Vector3::One * 20.0f);
	effectPosition.y += 50.0f;
	rotation = m_rotation;
	effectEmitter->SetPosition(effectPosition);
	effectEmitter->SetRotation(rotation);
	effectEmitter->Play();
}

void Player::MakeMagicBall()
{
	//MPを減少させる
	m_mp -= PLAYER_LOSTMP;

	//魔法攻撃を作成する
	MagicBall* magicBall = NewGO<MagicBall>(0);
	Vector3 magicBallPosition = m_position;
	magicBallPosition.y += 40.0f;
	magicBall->SetPosition(magicBallPosition);
	magicBall->SetRotation(m_rotation);
	//術者をプレイヤーにする
	magicBall->SetEnMagician(MagicBall::enMagician_Player);
}

void Player::ProcessCommonStateTransition()
{
	//敵を全滅させたら
	if (m_game->IsWannhilationEnemy())
	{
		//BGMをクリアBGMに変える
		m_game->ChangeClearBGM();
		//UIを見えないようにする
		m_spriteFlag = false;
		//ボスのHPゲージを見えないようにする
		m_bossRoomFlag = false;

		//プレイヤーをカメラの向きにする
		//カメラに向かうベクトルを求める
		GameCamera* gameCamera = FindGO<GameCamera>("gameCamera");
		gameCamera->SetStopCamera();

		Vector3 rotyou = m_position - gameCamera->GetPosition();
		rotyou.y = 0.0f;
		rotyou *= -1.0f;
		rotyou.Normalize();

		Quaternion qRot;
		qRot.SetRotationYFromDirectionXZ(rotyou);

		m_modelRender.SetRotation(qRot);
		m_modelRender.Update();

		m_game->SetGameClearSpriteFlag(true);

		//クリアステートに移行する
		m_playerState = enPlayerState_Clear;
		return;
	}

	//穴に落ちたときの処理
	if (m_position.y - m_respawnPosition.y <= -300.0f)
	{
		GameCamera* gameCamera = FindGO<GameCamera>("gameCamera");
		gameCamera->SetMoveTargetFlag();

		m_playerState = enPlayerState_Down;

		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(14);
		se->SetVolume(MUSHATTACK_SE_VOLUME);
		se->Play(false);

		//UIの表示
		m_spriteFlag = false;
		return;
	}

	//プレイヤーがジャンプ中だったら
	if (m_charCon.IsJump() == true)
	{
		//ステートジャンプ
		m_playerState = enPlayerState_Jump;
		return;
	}

	//Bボタンが押されたら
	if (g_pad[0]->IsTrigger(enButtonB))
	{
		//ステート攻撃
		m_playerState = enPlayerState_Attack;
		m_isUnderAttack = false;
		return;
	}

	if (g_pad[0]->IsTrigger(enButtonX))
	{
		//現在のMPが消費MPより少ない場合は処理しない
		if (m_mp < PLAYER_LOSTMP)
		{
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(17);
			se->SetVolume(SE_VOLUME);
			se->Play(false);
			return;
		}

		//ステート魔法攻撃
		m_playerState = enPlayerState_Magic;

		//魔法陣エフェクトの再生
		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		Vector3 effectPosition = m_position;
		effectPosition.y += 1.0f;
		Quaternion rotation;

		effectEmitter->Init(9);
		effectEmitter->SetScale(Vector3::One * 7.0f);
		rotation = m_rotation;
		effectEmitter->SetPosition(effectPosition);
		effectEmitter->SetRotation(rotation);

		//魔法陣SE
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(4);
		se->SetVolume(SE_VOLUME);

		effectEmitter->Play();
		se->Play(false);
		return;
	}

	//スティックの入力があったら
	if (fabsf(m_moveSpeed.x) >= 0.001f
		|| fabsf(m_moveSpeed.z) >= 0.001f)
	{
		//RBボタンが押されたら
		if (g_pad[0]->IsPress(enButtonRB1))
		{
			//ステート走り
			m_playerState = enPlayerState_Run;
			return;
		}
		else
		{
			//ステート歩き
			m_playerState = enPlayerState_Walk;
			return;
		}
	}
	//スティックの入力がなかったら
	else
	{
		//ステート待機
		m_playerState = enPlayerState_Idle;
		return;
	}
}

void Player::ProcessIdleStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessRunStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessAttackStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ProcessCommonStateTransition();
	}
}

void Player::ProcessMagicStateTransition()
{
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		ProcessCommonStateTransition();
	}
}

void Player::ProcessJumpStateTransition()
{
		ProcessCommonStateTransition();
}

void Player::ProcessReceiveDamageStateTransition()
{
	//被ダメージアニメーションの再生が終わったら
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		//ステートを遷移する
		ProcessCommonStateTransition();
	}
}

void Player::ProcessDownStateTransition()
{
	//ダウンアニメーションの再生がおわったら
	if (m_modelRender.IsPlayingAnimation() == false)
	{
		m_game->SetGameState(m_game->enGameState_PlayerDead);
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(16);
			se->SetVolume(SE_VOLUME);
			se->Play(false);

			//リスポーンさせる
			Fade* fade = FindGO<Fade>("fade");
			fade->StartFadeOut();
			m_game->SetisWaitRespown();
		}
	}
}

void Player::ProcessClearStateTransition()
{
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		SoundSource* se = NewGO<SoundSource>(0);
		se->Init(16);
		se->SetVolume(SE_VOLUME);
		se->Play(false);

		m_game->NotifyClear();
	}
}

void Player::HPBar()
{
	/*if (g_pad[0]->IsPress(enButtonUp))
	{
		m_hp += 1;
	}
	if (g_pad[0]->IsPress(enButtonDown))
	{
		m_hp -= 1;
	}*/

	//HPが0を下回ったら
	if (m_hp < 0)
	{
		//HPを0にする
		m_hp = 0;
	}
	//HPが最大HPを上回ったら
	if (m_hp > PLAYER_MAXHP)
	{
		//HPを最大HPにする
		m_hp = PLAYER_MAXHP;
	}

	Vector3 scale = Vector3::One;
	scale.x = float(m_hp) / float(PLAYER_MAXHP);
	m_hpBar.SetScale(scale);
	m_hpBar.Update();
}

void Player::MPBar()
{
	//MPが0を下回ったら
	if (m_mp < 0)
	{
		//MPを0にする
		m_mp = 0;
	}
	//MPが最大MPを上回ったら
	if (m_mp > PLAYER_MAXMP)
	{
		//MPを最大MPにする
		m_mp = PLAYER_MAXMP;
	}

	//MPが最大MPを下回ったら
	if (m_mp < PLAYER_MAXMP)
	{
		m_mpHealTimer += g_gameTime->GetFrameDeltaTime();

		//1秒ごとに
		if (m_mpHealTimer >= 2.0f)
		{
			//MPを回復する
			m_mp += PLAYER_HEALMP;
			m_mpHealTimer = 0.0f;
		}
	}

	Vector3 scale = Vector3::One;
	scale.x = float(m_mp) / float(PLAYER_MAXMP);
	m_mpBar.SetScale(scale);
	m_mpBar.Update();
}

void Player::PlayerRespawn()
{
	m_position = m_respawnPosition;
	m_rotation = m_respawnRotation;
	m_moveSpeed = Vector3::Zero;
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_charCon.SetPosition(m_position);
	m_hp = PLAYER_MAXHP;
	m_mp = PLAYER_MAXMP;

	//UIの表示
	m_spriteFlag = true;

	ProcessCommonStateTransition();
}

void Player::ManageState()
{
	switch (m_playerState)
	{
	//待機ステートの時
	case enPlayerState_Idle:
		ProcessIdleStateTransition();
		break;
	//歩きステートの時
	case enPlayerState_Walk:
		ProcessWalkStateTransition();
		break;
	//走りステートの時
	case enPlayerState_Run:
		ProcessRunStateTransition();
		break;
	//ジャンプステートの時
	case enPlayerState_Jump:
		ProcessJumpStateTransition();
		break;
	//攻撃ステートの時
	case enPlayerState_Attack:
		ProcessAttackStateTransition();
		break;
	//魔法攻撃ステートの時
	case enPlayerState_Magic:
		ProcessMagicStateTransition();
		break;
	//被ダメージステートの時
	case enPlayerState_ReceiveDamage:
		ProcessReceiveDamageStateTransition();
		break;
	//ダウンステートの時
	case enPlayerState_Down:
		ProcessDownStateTransition();
		break;
	case enPlayerState_Clear:
		ProcessClearStateTransition();
		break;
	}
}

void Player::PlayAnimation()
{
	m_modelRender.SetAnimationSpeed(1.0f);
	switch (m_playerState)
	{
	case enPlayerState_Idle:
		m_modelRender.PlayAnimation(enAnimationClip_Idle, 0.1f);
		break;
	case enPlayerState_Walk:
		m_modelRender.SetAnimationSpeed(1.1f);
		m_modelRender.PlayAnimation(enAnimationClip_Walk, 0.1f);
		break;
	case enPlayerState_Run:
		m_modelRender.PlayAnimation(enAnimationClip_Run, 0.1f);
		break;
	case enPlayerState_Jump:
		m_modelRender.PlayAnimation(enAnimationClip_Jump, 0.2f);
		break;
	case enPlayerState_Attack:
		m_modelRender.PlayAnimation(enAnimationClip_Attack, 0.4f);
		break;
	case enPlayerState_Magic:
		m_modelRender.SetAnimationSpeed(1.2f);
		m_modelRender.PlayAnimation(enAnimationClip_Magic, 0.4f);
		break;
	case enPlayerState_ReceiveDamage:
		m_modelRender.SetAnimationSpeed(0.8f);
		m_modelRender.PlayAnimation(enAnimationClip_ReceiveDamage, 0.4f);
		break;
	case enPlayerState_Down:
		m_modelRender.PlayAnimation(enAnimationClip_Down);
		break;
	case enPlayerState_Clear:
		m_modelRender.PlayAnimation(enAnimationClip_Clear);
		break;
	default:
		break;
	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	if (m_spriteFlag == true)
	{
		m_statusBar.Draw(rc);
		m_hpBar.Draw(rc);
		m_mpBar.Draw(rc);
		m_playerFaceBack.Draw(rc);
		m_playerFaceFrame.Draw(rc);
	}
}


