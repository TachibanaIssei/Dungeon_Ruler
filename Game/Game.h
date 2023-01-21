#pragma once

#include "Level3DRender/LevelRender.h"
#include "sound/SoundSource.h"

class GameCamera;
class Player;
class Background;
class MovingFloor;
class MovingFloorZ;
class SeesawFloor;
class FireGimmic;
class Map;
class CheckPoint;
class Fade;

class Game : public IGameObject
{
public:
	enum EnGameState
	{
		enGameState_DuringGamePlay,		//ゲームプレイ中
		enGameState_PlayerDead,			//プレイヤー死亡
		enGameState_GameClear,			//ゲームクリア
		enGameState_GameClear_Idle		//ゲームクリア(待機中)
	};

public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// 倒された敵の数を＋１する
	/// </summary>
	void AddDefeatedEnemyNumber()
	{
		m_numDefeatedEnemy++;
	}

	/// <summary>
	/// 雑魚敵をすべて倒した？
	/// </summary>
	/// <returns>雑魚敵が全滅したらfalse</returns>
	const bool IsWannhilationZako()
	{
		return m_numDefeatedEnemy < 5;
	}

	/// <summary>
	/// 敵を全滅させた？
	/// </summary>
	/// <returns>敵が全滅していたらtrue</returns>
	const bool IsWannhilationEnemy() const
	{
		return m_numEnemy == m_numDefeatedEnemy;
	}

	const void SetisWaitRespown()
	{
		m_isWaitRespown = true;
	}

	/// <summary>
	/// ゲームクリア画像の表示フラグを設定する
	/// </summary>
	/// <param name="flag">trueかfalse</param>
	void SetGameClearSpriteFlag(bool flag)
	{
		m_gameClearSpriteFlag = flag;
	}

	/// <summary>
	/// ゲームステートを設定する
	/// </summary>
	/// <param name="state">ステート</param>
	void SetGameState(EnGameState state)
	{
		m_gameState = state;
	}

	/// <summary>
	/// クリアBGMに変える
	/// </summary>
	void ChangeClearBGM();

	void NotifyReStart();
	void NotifyClear();

private:
	/// <summary>
	/// 空を初期化
	/// </summary>
	void InitSky();

private:
	LevelRender				m_levelRender;
	GameCamera*				m_gameCamera = nullptr;
	Player*					m_player = nullptr;
	Background*				m_backGround = nullptr;
	MovingFloor*			m_movingFloorX = nullptr;
	Map*					m_map = nullptr;
	CheckPoint*				m_checkPoint = nullptr;
	SkyCube*				m_skyCube = nullptr;
	Fade*					m_fade = nullptr;
	SoundSource* m_playBGM;
	SoundSource* m_clearBGM;

	SpriteRender			m_deadSprite;	//プレイヤー死亡画像
	SpriteRender			m_attackIcon;	//攻撃アイコン
	SpriteRender			m_magicIcon;	//魔法アイコン
	SpriteRender			m_buttonBIcon;	//Bボタンアイコン
	SpriteRender			m_buttonXIcon;	//Xボタンアイコン
	SpriteRender			m_gameClear;	//ゲームクリア
	SpriteRender			m_pressAbutton;	//PRESS A BUTTON
	SpriteRender			m_mokuteki;		//目的画像
	SpriteRender			m_mokuteki_1;	//目的1画像
	SpriteRender			m_mokuteki_2;	//目的2画像
	SpriteRender			m_mokuteki_3;	//目的3画像


	EnGameState m_gameState = enGameState_DuringGamePlay;	//ゲームステート

	std::vector<FireGimmic*>	m_fireGimmicVector;			//炎ギミック
	std::vector<SeesawFloor*>	m_seesawFloorVector;		//回転する床
	std::vector<MovingFloorZ*>	m_movingFloorZVector;		//Z軸に移動する床

	bool					m_isWaitFadeout = false;		//フェード待ち
	bool					m_isWaitRespown = false;		//リスポーン待ち
	bool					m_gameClearSpriteFlag = false;	//ゲームクリア画像の表示フラグ
	int						m_numEnemy = 0;					//敵の数
	int						m_numDefeatedEnemy = 0;			//倒した敵の数
	float					m_gameClearAlpha = 0.0f;		//ゲームクリア画像のアルファ値
	float					m_pressAbuttonAlpha = 0.0f;		//PRESS A BUTTON画像のアルファ値
};

/*
エフェクトエミッターInit番号
0:プレイヤー剣攻撃
1:炎ギミック
2:雷魔法
3:敵キャラの剣攻撃
4:火炎ボール魔法陣
5:雷魔法陣
6:マッシュルームの攻撃
7:魔法攻撃(炎)
8:魔法攻撃(水)
9:プレイヤーの魔法陣
10:火魔法のヒット
11:風魔法のヒット
12:敵の死亡エフェクト
*/

/*
音楽のInit番号
0:タイトルBGM
1:ゲーム中BGM
2:クリア時BGM
3:剣ヒットSE
4:魔法陣SE
5:風魔法SE
6:炎魔法SE
7:マッシュルーム攻撃SE
8:マッシュルーム移動SE
9:魔法ヒットSE
10:ボス叫びSE
11:雷魔法SE
12:炎ギミックヒットSE
13:炎ギミックSE
14:プレイヤー死亡SE
15:タイトルボタンSE
16:クリア後のボタンSE
17:魔法不発SE
18:ボスを倒した時のSE
*/