#pragma once
class Player:public IGameObject
{
public:
	//ƒƒ“ƒoŠÖ”
	Player();
	~Player();
	bool Start() override;
	void Update() override;
	void Render(RenderContext& renderContext) override;
private:
	//ƒƒ“ƒo•Ï”
	ModelRender m_modelRender;
};