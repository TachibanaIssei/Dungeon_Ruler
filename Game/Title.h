#pragma once
#include "sound/SoundSource.h"

class Fade;

class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	SoundSource*	m_titleBGM;					//BGM
	SpriteRender	m_spriteRender;				//スプライトレンダー
	SpriteRender	m_startSprite;				//START画像
	Fade*			m_fade = nullptr;			//フェード
	float			m_alpha = 0.0f;				//アルファ値
	bool			m_isWaitFadeout = false;	//フェード待ちフラグ
};

