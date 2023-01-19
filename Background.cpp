#include "stdafx.h"
#include "Background.h"

Background::Background()
{
}

Background::~Background()
{
}

bool Background::Start()
{
	m_modelRender.Init("Assets/modelData/background/dungeon01.tkm");
	
	//m_modelRender.SetPosition(m_position + Vector3(0.0f,30.0f,0.0f));
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetScale(m_scale);
	m_modelRender.Update();
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(),
		m_modelRender.GetModel().GetWorldMatrix());
	
	return true;
}

void Background::Update()
{
	m_modelRender.Update();
}

void Background::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
