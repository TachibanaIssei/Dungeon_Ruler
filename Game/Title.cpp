#include "stdafx.h"
#include "Title.h"

#include "Game.h"
#include "Fade.h"

#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

namespace
{
	const Vector3 START_SPRITE_POS = Vector3(0.0f, -270.0f, 0.0f);

	const float TITLE_BGM_VOLUME = 0.5f;
	const float SE_VOLUME = 1.0f;
}

Title::Title()
{
}

Title::~Title()
{
	DeleteGO(m_titleBGM);
}

bool Title::Start()
{
	m_spriteRender.Init("Assets/sprite/title.DDS",1920, 1080);
	m_startSprite.Init("Assets/sprite/title_START.dds", 1920, 1080);
	
	m_startSprite.SetPosition(START_SPRITE_POS);

	//BGMのロード
	g_soundEngine->ResistWaveFileBank(0, "Assets/sound/BGM_title.wav");

	//SEのロード
	g_soundEngine->ResistWaveFileBank(15, "Assets/sound/ken_sound_nukuoto.wav");

	m_titleBGM = NewGO<SoundSource>(0);
	m_titleBGM->Init(0);
	m_titleBGM->SetVolume(TITLE_BGM_VOLUME);

	m_fade = FindGO<Fade>("fade");
	m_fade->StartFadeIn();

	m_titleBGM->Play(true);

	return true;
}

void Title::Update()
{
	if (m_isWaitFadeout)
	{
		if (!m_fade->IsFade())
		{
			NewGO<Game>(0, "game");
			DeleteGO(this);
		}
	}
	else
	{
		if (g_pad[0]->IsTrigger(enButtonA)) {
			SoundSource* se = NewGO<SoundSource>(0);
			se->Init(15);
			se->SetVolume(SE_VOLUME);
			se->Play(false);

			m_isWaitFadeout = true;
			m_fade->StartFadeOut();
		}
	}

	//α値を変化させる。
	if (m_isWaitFadeout)
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 20.5f;
	}
	else
	{
		m_alpha += g_gameTime->GetFrameDeltaTime() * 1.2f;
	}

	m_startSprite.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, fabsf(sinf(m_alpha))));


	m_spriteRender.Update();
	m_startSprite.Update();
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	m_startSprite.Draw(rc);
}
