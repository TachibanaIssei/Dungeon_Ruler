#include "stdafx.h"
#include "CheckPoint.h"

#include "Player.h"

bool CheckPoint::Start()
{
    m_player = FindGO<Player>("player");

    m_levelRender.Init("Assets/level3D/checkpointLevel.tkl", [&](LevelObjectData& objData) {
        if (objData.ForwardMatchName(L"checkpoint") == true) {
            if (objData.number == 0)
            {
                m_physicsGhostObject[0].CreateBox(
                    Vector3(objData.position),
                    Quaternion::Identity,
                    Vector3::One * 200.0f);

                m_player->SetinRespawnPosition(Vector3(objData.position), objData.number);
            }
            if (objData.number == 1)
            {
                m_physicsGhostObject[1].CreateBox(
                    Vector3(objData.position),
                    Quaternion::Identity,
                    Vector3::One * 200.0f);
                m_player->SetinRespawnPosition(Vector3(objData.position), objData.number);
            }
            if (objData.number == 2)
            {
                m_physicsGhostObject[2].CreateBox(
                    Vector3(objData.position),
                    Quaternion::Identity,
                    Vector3::One * 200.0f);
                m_player->SetinRespawnPosition(Vector3(objData.position), objData.number);
            }
            if (objData.number == 3)
            {
                m_physicsGhostObject[3].CreateBox(
                    Vector3(objData.position),
                    Quaternion::Identity,
                    Vector3::One * 200.0f);
                m_player->SetinRespawnPosition(Vector3(objData.position), objData.number);
            }
            if (objData.number == 4)
            {
                m_physicsGhostObject[4].CreateBox(
                    Vector3(objData.position),
                    Quaternion::Identity,
                    Vector3::One * 200.0f);
                m_player->SetinRespawnPosition(Vector3(objData.position), objData.number);
            }
            if (objData.number == 5)
            {
                m_physicsGhostObject[5].CreateBox(
                    Vector3(objData.position),
                    Quaternion::Identity,
                    Vector3::One * 300.0f);
                m_player->SetinRespawnPosition(Vector3(objData.position), objData.number);
            }
        }

        if (objData.ForwardMatchName(L"movelimit") == true)
        {
            if (objData.number == 0)
            {
                m_player->SetinMoveLimitPosition(Vector3(objData.position), objData.number);
            }
            if (objData.number == 1)
            {
                m_player->SetinMoveLimitPosition(Vector3(objData.position), objData.number);
            }
        }
        return true;
        });    
    return true;
}

void CheckPoint::Update()
{
}
