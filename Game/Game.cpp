#include "stdafx.h"
#include "Game.h"

#include <vector>

#include "nature/SkyCube.h"

#include "Title.h"
#include "GameCamera.h"
#include "Player.h"
#include "MushRoomMan.h"
#include "StoneMonster.h"
#include "Boss.h"
#include "Background.h"
#include "MovingFloor.h"
#include "MovingFloorZ.h"
#include "SeesawFloor.h"
#include "FireGimmic.h"
#include "Map.h"
#include "CheckPoint.h"
#include "Fade.h"

#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"
#include "graphics/RenderingEngine.h"

namespace
{
	const Vector3 ATTACK_ICON_POS = Vector3(640.0f, -385.0f, 0.0f);
	const Vector3 MAGIC_ICON_POS  = Vector3(850.0f, -385.0f, 0.0f);
	const Vector3 BUTTON_X_ICON_POS = Vector3(850.0f, -454.0f, 0.0f);
	const Vector3 BUTTON_B_ICON_POS = Vector3(640.0f, -454.0f, 0.0f);
	const Vector3 GAMECLEAR_POS = Vector3(0.0f,380.0f,0.0f);
	const Vector3 PRESSABUTTON_POS = Vector3(0.0f, -400.0f, 0.0f);

	//�v���C���[���S�摜���W
	const Vector3 DEAD_SPRITE_POS = Vector3(0.0f, 300.0f, 0.0f);

	//�}�b�V�����[����HP�摜���W
	const Vector3 MUSH_1_HP_SPRITE_POS = Vector3(0.0f,430.0f,0.0f);
	const Vector3 MUSH_2_HP_SPRITE_POS = Vector3(350.0f,430.0f,0.0f);
	const Vector3 MUSH_3_HP_SPRITE_POS = Vector3(-350.0,430.0f,0.0f);
	//�}�b�V�����[����HP�Q�[�W�摜���W
	const Vector3 MUSH_1_HP_BAR_POS = Vector3(-148.07692307692298f, 430.0f, 0.0f);
	const Vector3 MUSH_2_HP_BAR_POS = Vector3(201.9230769230769f, 430.0f, 0.0f);
	const Vector3 MUSH_3_HP_BAR_POS = Vector3(-498.0769230769231f, 430.0f, 0.0f);
	//�X�g�[�������X�^�[��HP�摜���W
	const Vector3 STONE_1_HP_SPRITE_POS = Vector3(-700.0f, 430.0f, 0.0f);
	const Vector3 STONE_2_HP_SPRITE_POS = Vector3(700.0f, 430.0f, 0.0f);
	//�X�g�[�������X�^�[��HP�Q�[�W�摜���W
	const Vector3 STONE_1_HP_BAR_POS = Vector3(-848.0769230769231, 430.0f, 0.0f);
	const Vector3 STONE_2_HP_BAR_POS = Vector3(551.9230769230769f, 430.0f, 0.0f);
	//�ړI�摜�̍��W
	const Vector3 MOKUTEKI_POS = Vector3(-700.0f, -175.0f, 0.0f);
	const Vector3 MOKUTEKI_1_POS = Vector3(-690.0f, -200.0f, 0.0f);
	const Vector3 MOKUTEKI_2_POS = Vector3(-690.0f, -200.0f, 0.0f);
	const Vector3 MOKUTEKI_3_POS = Vector3(-690.0f, -200.0f, 0.0f);

	const float PLAY_BGM_VOLUME = 0.3f;
	const float CLEAR_BGM_VOLUME = 0.5f;

	//�̗�
	const int MUSHROOM_HP = 5;
	const int STONEMONSTER_HP = 5;
	const int BOSS_HP = 10;
}

Game::Game()
{
}

Game::~Game()
{
	DeleteGO(m_gameCamera);
	DeleteGO(m_player);
	DeleteGO(m_backGround);
	DeleteGO(m_movingFloorX);
	DeleteGO(m_playBGM);
	DeleteGO(m_clearBGM);

	//���M�~�b�N�̍폜
	for (auto firegimmic : m_fireGimmicVector)
	{
		DeleteGO(firegimmic);
	}

	//��]���鏰�̍폜
	for (auto seesawFloor : m_seesawFloorVector)
	{
		DeleteGO(seesawFloor);
	}

	//Z���Ɉړ����鏰�̍폜
	for (auto movingFloorZ : m_movingFloorZVector)
	{
		DeleteGO(movingFloorZ);
	}

	//�}�b�V�����[���̍폜
	const auto& mushRoomMans = FindGOs<MushRoomMan>("mushroomman");
	for (auto mushRoomMan : mushRoomMans)
	{
		DeleteGO(mushRoomMan);
	}
	//�X�g�[�������X�^�[�̍폜
	const auto& stoneMonsters = FindGOs<StoneMonster>("stonemonster");
	for (auto stoneMonster : stoneMonsters)
	{
		DeleteGO(stoneMonster);
	}
	DeleteGO(FindGO<Boss>("boss"));	//�{�X�̍폜
	DeleteGO(m_map);
	DeleteGO(m_checkPoint);
	DeleteGO(m_skyCube);
}

void Game::InitSky()
{
	DeleteGO(m_skyCube);
	m_skyCube = NewGO<SkyCube>(0, "skycube");
	m_skyCube->SetScale(600.0f);
	g_renderingEngine->SetAmbientByIBLTexture(m_skyCube->GetTextureFilePath(), 2.0f);	
	//g_renderingEngine->DisableIBLTextureForAmbient();
}

bool Game::Start()
{
	Vector3 direction = { -0.25f,-1.5f,-0.5f };
	direction.Normalize();
	g_renderingEngine->SetDirectionLight(0, direction, Vector3::One * 5.0f);

	//�f�t�H���g��1.0f
	g_renderingEngine->SetBloomThreshold(2.0f);
	//�f�t�H���g��0.18f
	g_renderingEngine->SetSceneMiddleGray(0.18f);

	g_renderingEngine->SetCascadeNearAreaRates(0.05f, 0.5f, 1.0f);

	InitSky();

	m_map = NewGO<Map>(2, "map");

	//���x�����\�z����
	m_levelRender.Init("Assets/level3D/level.tkl", [&](LevelObjectData& objData) {
		//�o�b�N�O���E���h
		 if (objData.EqualObjectName(L"background") == true) {
			m_backGround = NewGO<Background>(0, "background");
			m_backGround->SetPosition(objData.position);
			m_backGround->SetScale(objData.scale);
			m_backGround->SetRotation(objData.rotation);
			return true;
		}
		//�v���C���[
		 if (objData.EqualObjectName(L"player") == true) {
			 m_player = NewGO<Player>(1, "player");
			 m_player->SetPosition(objData.position);
			 m_player->SetRotation(objData.rotation);
			 m_player->SetScale(objData.scale);
			 return true;
		 }
		//�}�b�V�����[���}��
		 if (objData.ForwardMatchName(L"MushroomMon") == true){
			 MushRoomMan* mushRoomMan = NewGO<MushRoomMan>(0, "mushroomman");
			 mushRoomMan->SetPosition(objData.position);
			 mushRoomMan->SetRotation(objData.rotation);
			 mushRoomMan->SetScale(objData.scale);
			 //�}�b�V�����[���}����HP��5�ɐݒ�
			 mushRoomMan->SetHP(MUSHROOM_HP);
			 //�G�̐���+1����
			 m_numEnemy++;

			 if (objData.number == 1)
			 {
				 mushRoomMan->SetHPSpritePosition(1);
			 }
			 if (objData.number == 2)
			 {
				 mushRoomMan->SetHPSpritePosition(2);
			 }
			 if (objData.number == 3)
			 {
				 mushRoomMan->SetHPSpritePosition(3);
			 }

			 return true;
		 }
		 //�X�g�[�������X�^�[
		 if (objData.ForwardMatchName(L"StoneMonster") == true) {
			 StoneMonster* stoneMonster = NewGO<StoneMonster>(0, "stonemonster");
			 stoneMonster->SetPosition(objData.position);
			 stoneMonster->SetRotation(objData.rotation);
			 stoneMonster->SetScale(objData.scale);
			 //�X�g�[�������X�^�[��HP��5�ɐݒ�
			 stoneMonster->SetHP(STONEMONSTER_HP);
			 //�G�̐���+1����
			 m_numEnemy++;

			 if (objData.number == 1)
			 {
				 stoneMonster->SetHPSpritePosition(1);
			 }
			 if (objData.number == 2)
			 {
				 stoneMonster->SetHPSpritePosition(2);
			 }

			 return true;
		 }
		 //�{�X
		 if (objData.EqualObjectName(L"Skeleton") == true) {
			 Boss* boss = NewGO<Boss>(0, "boss");
			 boss->SetPosition(objData.position);
			 boss->SetRotation(objData.rotation);
			 boss->SetScale(objData.scale);
			 //�{�X��HP��5�ɐݒ�
			 boss->SetHP(BOSS_HP);
			 //�G�̐���+1����
			 m_numEnemy++;
			 return true;
		 }

		 //������
		 if (objData.EqualObjectName(L"movefloorX") == true) {
			 m_movingFloorX = NewGO<MovingFloor>(0, "movingfloorX");
			 m_movingFloorX->SetPosition(objData.position);
			 m_movingFloorX->SetScale(objData.scale);
			 return true;
		 }

		 if (objData.ForwardMatchName(L"movefloorZ") == true) {
			 auto movingFloorZ = NewGO<MovingFloorZ>(0, "movingfloorZ");

			 movingFloorZ->SetPosition(objData.position);
			 movingFloorZ->SetScale(objData.scale);

			 if (objData.number == 2)
			 {
				 movingFloorZ->SetSpeed(150.0f);
			 }
			 if (objData.number == 3)
			 {
				 movingFloorZ->SetSpeed(130.0f);
			 }
			 m_movingFloorZVector.push_back(movingFloorZ);
			 return true;
		 }

		 //��]���鏰
		 if (objData.ForwardMatchName(L"seesawfloor") == true) {
			 auto seesawFloor = NewGO<SeesawFloor>(0, "seesawfloor");
			 if (objData.number == 1)
			 {
				 seesawFloor->SetPosition(objData.position);
				 seesawFloor->SetRotation(objData.rotation);
				 seesawFloor->SetRot(0.003f, 0.4f);
				 seesawFloor->SetFloorNumber(1);
			 }
			 else if (objData.number == 2)
			 {
				 seesawFloor->SetPosition(objData.position);
				 seesawFloor->SetRotation(objData.rotation);
				 seesawFloor->SetState();
				 seesawFloor->SetRot(0.003f, 0.4f);
				 seesawFloor->SetFloorNumber(2);
			 }
			 m_seesawFloorVector.push_back(seesawFloor);
			 return true;
		 }

		 //���̃M�~�b�N
		 if (objData.ForwardMatchName(L"fire_gimmic") == true) {
			 auto fireGimmic = NewGO<FireGimmic>(0, "firegimmic");
			 fireGimmic->SetPosition(objData.position);

			 //@�̌��1�����Ă���I�u�W�F�N�g��
			 if (objData.number == 1)
			 {
				 fireGimmic->SetMoveFlag(true);
				 //�X�s�[�h�ƈړ�������ݒ肷��
				 fireGimmic->SetSppedLimit(100.0f, 150.0f);
				 fireGimmic->SetEffectInterval(3.0f);
			 }
			 //@�̌��2�����Ă���I�u�W�F�N�g��
			 else if (objData.number == 2)
			 {
				 fireGimmic->SetMoveFlag(true);
				 fireGimmic->SetSppedLimit(140.0f, 261.0f);
				 fireGimmic->SetEffectInterval(3.0f);
			 }
			 //@�̌��3�����Ă���I�u�W�F�N�g��
			 else if (objData.number == 3)
			 {
				 fireGimmic->SetMoveFlag(true);
				 fireGimmic->SetSppedLimit(170.0f, 261.0f);
				 fireGimmic->SetEffectInterval(3.0f);
			 }
			 else
			 {
				 fireGimmic->SetEffectInterval(objData.number);
			 }
			 m_fireGimmicVector.push_back(fireGimmic);
			 return true;
		 }

		return true;
		});

	m_gameCamera = NewGO<GameCamera>(2, "gameCamera");
	m_checkPoint = NewGO<CheckPoint>(0, "checkPoint");
	

	//�摜��ǂݍ���
	m_deadSprite.Init("Assets/sprite/YouDead.dds", 1290.0, 420.0);
	m_deadSprite.SetPosition(DEAD_SPRITE_POS);

	m_attackIcon.Init("Assets/sprite/weapons_02.dds", 153.6, 153.6);
	m_attackIcon.SetPosition(ATTACK_ICON_POS);

	m_magicIcon.Init("Assets/sprite/magicball.dds", 153.6, 153.6);
	m_magicIcon.SetPosition(MAGIC_ICON_POS);

	m_buttonBIcon.Init("Assets/sprite/button_b.dds", 64, 64);
	m_buttonBIcon.SetPosition(BUTTON_B_ICON_POS);

	m_buttonXIcon.Init("Assets/sprite/button_x.dds", 64, 64);
	m_buttonXIcon.SetPosition(BUTTON_X_ICON_POS);

	m_gameClear.Init("Assets/sprite/game_clear.DDS", 1920, 1080);
	m_gameClear.SetPosition(GAMECLEAR_POS);

	m_pressAbutton.Init("Assets/sprite/PRESS_A_BUTTON.DDS", 1920, 1080);
	m_pressAbutton.SetPosition(PRESSABUTTON_POS);

	m_mokuteki.Init("Assets/sprite/mokuteki.dds", 1920, 1080);
	m_mokuteki.SetPosition(MOKUTEKI_POS);

	m_mokuteki_1.Init("Assets/sprite/mokuteki_1.dds", 1920, 1080);
	m_mokuteki_1.SetPosition(MOKUTEKI_1_POS);

	m_mokuteki_2.Init("Assets/sprite/mokuteki_2.dds", 1920, 1080);
	m_mokuteki_2.SetPosition(MOKUTEKI_2_POS);

	m_mokuteki_3.Init("Assets/sprite/mokuteki_3.dds", 1920, 1080);
	m_mokuteki_3.SetPosition(MOKUTEKI_3_POS);

	//���̃��[�h
	g_soundEngine->ResistWaveFileBank(1, "Assets/sound/BGM_play.wav");
	g_soundEngine->ResistWaveFileBank(2, "Assets/sound/BGM_clear.wav");

	m_playBGM = NewGO<SoundSource>(0);
	m_playBGM->Init(1);
	m_playBGM->SetVolume(PLAY_BGM_VOLUME);

	m_clearBGM = NewGO<SoundSource>(0);
	m_clearBGM->Init(2);
	m_clearBGM->SetVolume(CLEAR_BGM_VOLUME);

	//�摜�̍X�V����
	m_deadSprite.Update();
	m_attackIcon.Update();
	m_magicIcon.Update();
	m_buttonBIcon.Update();
	m_buttonXIcon.Update();
	m_gameClear.Update();
	m_pressAbutton.Update();
	m_mokuteki.Update();
	m_mokuteki_1.Update();
	m_mokuteki_2.Update();
	m_mokuteki_3.Update();

	m_fade = FindGO<Fade>("fade");
	m_fade->StartFadeIn();

	m_playBGM->Play(true);

	//�����蔻���L��������B
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
	return true;
}

void Game::ChangeClearBGM()
{
	m_playBGM->Stop();
	m_clearBGM->Play(true);
}

void Game::NotifyReStart()
{
	if (!m_fade->IsFade())
	{
		//�}�b�V�����[����T��
		const auto& mushRoomMans = FindGOs<MushRoomMan>("mushroomman");
		for (auto mushRoomMan : mushRoomMans)
		{
			//���Z�b�g����
			mushRoomMan->ReStart();
		}
		//�X�g�[�������X�^�[��T��
		const auto& StoneMonsters = FindGOs<StoneMonster>("stonemonster");
		for (auto stoneMonster : StoneMonsters)
		{
			//���Z�b�g����
			stoneMonster->ReStart();
		}
		//�{�X��T��
		Boss* boss = FindGO<Boss>("boss");
		//�{�X������������
		if (boss != nullptr)
		{
			//���Z�b�g����
			boss->ReStart();
		}
		m_player->PlayerRespawn();
		m_gameCamera->ReStart();
		m_gameState = enGameState_DuringGamePlay;

		m_fade->StartFadeIn();
		m_isWaitRespown = false;
	}
	
}

void Game::NotifyClear()
{
	m_gameClearSpriteFlag = false;
	m_isWaitFadeout = true;
	m_fade->StartFadeOut();
}

void Game::Update()
{
	 g_renderingEngine->DisableRaytracing();

	if (m_isWaitFadeout)
	{
		if (!m_fade->IsFade())
		{
			NewGO<Title>(0, "title");
			DeleteGO(this);
		}
	}

	if (m_isWaitRespown)
	{
		NotifyReStart();
	}

	if (m_gameClearSpriteFlag)
	{
		if (m_gameClearAlpha >= 1.0f)
		{
			m_gameClearAlpha = 1.0f;
		}
		else {
			m_gameClearAlpha += g_gameTime->GetFrameDeltaTime() * 1.2f;
		}

		m_pressAbuttonAlpha += g_gameTime->GetFrameDeltaTime() * 1.2f;
	}

	m_gameClear.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_gameClearAlpha));
	m_pressAbutton.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, fabsf(sinf(m_pressAbuttonAlpha))));
	m_gameClear.Update();
	m_pressAbutton.Update();
}

void Game::Render(RenderContext& rc)
{
	if (m_gameClearSpriteFlag)
	{
		m_gameClear.Draw(rc);
		m_pressAbutton.Draw(rc);
	}

	if (m_player->GetSpriteFlag())
	{
		m_attackIcon.Draw(rc);
		m_magicIcon.Draw(rc);
		m_buttonBIcon.Draw(rc);
		m_buttonXIcon.Draw(rc);
		m_mokuteki.Draw(rc);

		if (m_player->GetMokutekiFlag() == 1)
		{
			m_mokuteki_1.Draw(rc);
		}
		else if (m_player->GetMokutekiFlag() == 2)
		{
			m_mokuteki_2.Draw(rc);
		}
		else if (m_player->GetMokutekiFlag() == 3)
		{
			m_mokuteki_3.Draw(rc);
		}
	}
	if (m_gameState == enGameState_PlayerDead)
	{
		if (!m_fade->IsFade()) {
			m_deadSprite.Draw(rc);
		}
	}
}
