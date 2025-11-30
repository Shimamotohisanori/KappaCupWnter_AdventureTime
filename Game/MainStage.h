#pragma once
class MainStage : public IGameObject
{
public:
	MainStage();
	~MainStage();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	ModelRender m_mainStageRender;//モデルレンダー
	PhysicsStaticObject m_mainStagePhysicsStaticObject;//静的オブジェクト
};

