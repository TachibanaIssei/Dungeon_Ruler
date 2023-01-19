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
	SpriteRender	m_spriteRender;				//�X�v���C�g�����_�[
	SpriteRender	m_startSprite;				//START�摜
	Fade*			m_fade = nullptr;			//�t�F�[�h
	float			m_alpha = 0.0f;				//�A���t�@�l
	bool			m_isWaitFadeout = false;	//�t�F�[�h�҂��t���O
};

