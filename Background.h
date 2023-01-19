#pragma once
class Background:public IGameObject
{
public:
	Background();
	~Background();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// À•W‚ğİ’è‚·‚é
	/// </summary>
	/// <param name="position">À•W</param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
		m_modelRender.SetPosition(m_position);
	}
	/// <summary>
	/// ‰ñ“]‚ğİ’è‚·‚é
	/// </summary>
	/// <param name="rotation">‰ñ“]</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
		m_modelRender.SetRotation(m_rotation);
	}
	/// <summary>
	/// ‘å‚«‚³‚ğİ’è‚·‚é
	/// </summary>
	/// <param name="scale">‘å‚«‚³</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
		m_modelRender.SetScale(m_scale);
	}

private:
	ModelRender				m_modelRender;
	PhysicsStaticObject		m_physicsStaticObject;
	Vector3					m_position;
	Vector3					m_scale = Vector3::One;
	Quaternion				m_rotation;


};

