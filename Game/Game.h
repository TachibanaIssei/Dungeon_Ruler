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
		enGameState_DuringGamePlay,		//�Q�[���v���C��
		enGameState_PlayerDead,			//�v���C���[���S
		enGameState_GameClear,			//�Q�[���N���A
		enGameState_GameClear_Idle		//�Q�[���N���A(�ҋ@��)
	};

public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// �|���ꂽ�G�̐����{�P����
	/// </summary>
	void AddDefeatedEnemyNumber()
	{
		m_numDefeatedEnemy++;
	}

	/// <summary>
	/// �G���G�����ׂē|�����H
	/// </summary>
	/// <returns>�G���G���S�ł�����false</returns>
	const bool IsWannhilationZako()
	{
		return m_numDefeatedEnemy < 5;
	}

	/// <summary>
	/// �G��S�ł������H
	/// </summary>
	/// <returns>�G���S�ł��Ă�����true</returns>
	const bool IsWannhilationEnemy() const
	{
		return m_numEnemy == m_numDefeatedEnemy;
	}

	const void SetisWaitRespown()
	{
		m_isWaitRespown = true;
	}

	/// <summary>
	/// �Q�[���N���A�摜�̕\���t���O��ݒ肷��
	/// </summary>
	/// <param name="flag">true��false</param>
	void SetGameClearSpriteFlag(bool flag)
	{
		m_gameClearSpriteFlag = flag;
	}

	/// <summary>
	/// �Q�[���X�e�[�g��ݒ肷��
	/// </summary>
	/// <param name="state">�X�e�[�g</param>
	void SetGameState(EnGameState state)
	{
		m_gameState = state;
	}

	/// <summary>
	/// �N���ABGM�ɕς���
	/// </summary>
	void ChangeClearBGM();

	void NotifyReStart();
	void NotifyClear();

private:
	/// <summary>
	/// ���������
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

	SpriteRender			m_deadSprite;	//�v���C���[���S�摜
	SpriteRender			m_attackIcon;	//�U���A�C�R��
	SpriteRender			m_magicIcon;	//���@�A�C�R��
	SpriteRender			m_buttonBIcon;	//B�{�^���A�C�R��
	SpriteRender			m_buttonXIcon;	//X�{�^���A�C�R��
	SpriteRender			m_gameClear;	//�Q�[���N���A
	SpriteRender			m_pressAbutton;	//PRESS A BUTTON
	SpriteRender			m_mokuteki;		//�ړI�摜
	SpriteRender			m_mokuteki_1;	//�ړI1�摜
	SpriteRender			m_mokuteki_2;	//�ړI2�摜
	SpriteRender			m_mokuteki_3;	//�ړI3�摜


	EnGameState m_gameState = enGameState_DuringGamePlay;	//�Q�[���X�e�[�g

	std::vector<FireGimmic*>	m_fireGimmicVector;			//���M�~�b�N
	std::vector<SeesawFloor*>	m_seesawFloorVector;		//��]���鏰
	std::vector<MovingFloorZ*>	m_movingFloorZVector;		//Z���Ɉړ����鏰

	bool					m_isWaitFadeout = false;		//�t�F�[�h�҂�
	bool					m_isWaitRespown = false;		//���X�|�[���҂�
	bool					m_gameClearSpriteFlag = false;	//�Q�[���N���A�摜�̕\���t���O
	int						m_numEnemy = 0;					//�G�̐�
	int						m_numDefeatedEnemy = 0;			//�|�����G�̐�
	float					m_gameClearAlpha = 0.0f;		//�Q�[���N���A�摜�̃A���t�@�l
	float					m_pressAbuttonAlpha = 0.0f;		//PRESS A BUTTON�摜�̃A���t�@�l
};

/*
�G�t�F�N�g�G�~�b�^�[Init�ԍ�
0:�v���C���[���U��
1:���M�~�b�N
2:�����@
3:�G�L�����̌��U��
4:�Ή��{�[�����@�w
5:�����@�w
6:�}�b�V�����[���̍U��
7:���@�U��(��)
8:���@�U��(��)
9:�v���C���[�̖��@�w
10:�Ζ��@�̃q�b�g
11:�����@�̃q�b�g
12:�G�̎��S�G�t�F�N�g
*/

/*
���y��Init�ԍ�
0:�^�C�g��BGM
1:�Q�[����BGM
2:�N���A��BGM
3:���q�b�gSE
4:���@�wSE
5:�����@SE
6:�����@SE
7:�}�b�V�����[���U��SE
8:�}�b�V�����[���ړ�SE
9:���@�q�b�gSE
10:�{�X����SE
11:�����@SE
12:���M�~�b�N�q�b�gSE
13:���M�~�b�NSE
14:�v���C���[���SSE
15:�^�C�g���{�^��SE
16:�N���A��̃{�^��SE
17:���@�s��SE
18:�{�X��|��������SE
*/