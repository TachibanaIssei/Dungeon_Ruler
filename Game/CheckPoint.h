#pragma once

#include "physics/PhysicsGhostObject.h"
#include "Level3DRender/LevelRender.h"

class Player;

class CheckPoint : public IGameObject
{
public:
	bool Start();
	void Update();
	LevelRender				m_levelRender;
	PhysicsGhostObject		m_physicsGhostObject[6];

	Player* m_player = nullptr;
};

