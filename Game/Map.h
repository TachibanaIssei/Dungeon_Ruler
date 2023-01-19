#pragma once

class Player;
class MushRoomMan;
class StoneMonster;
class Boss;

class Map : public IGameObject
{
public:
	Map();
	~Map();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// �}�b�v�̐^�񒆂��擾����
	/// </summary>
	/// <returns></returns>
	const Vector3& GetMapCenterPosition() const
	{
		return m_mapCenterPosition;
	}

	/// <summary>
	/// ���[���h���W���}�b�v�p�̍��W�ɕϊ�����
	/// </summary>
	/// <param name="worldCenterPosition">�^�񒆂̍��W</param>
	/// <param name="worldPosition">�ϊ����������W</param>
	/// <param name="mapPosition">�ϊ������l������</param>
	/// <returns></returns>
	const bool WorldPositionConvertToMapPosition(Vector3 worldCenterPosition, Vector3 worldPosition, Vector3& mapPosition);

private:
	void PlayerMap();
	void BossMap();

	SpriteRender	m_spriteRender;
	SpriteRender	m_mapFrame;
	SpriteRender	m_playerSprite;
	SpriteRender	m_bossSprite;

	Vector3			m_mapCenterPosition		= Vector3(765.0f, 205.0f, 0.0f);	//�}�b�v�̃Z���^�[�|�W�V����

	Player*			m_player = nullptr;
	Boss*			m_boss = nullptr;

	bool	m_isImage = false;
};

