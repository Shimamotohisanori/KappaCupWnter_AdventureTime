#pragma once
class Player;
class Game;
class GoalPost : public IGameObject
{
public:
	GoalPost();
	~GoalPost();
	bool Start();
	void Update();
	void TouchOnGoalPost();
	void Render(RenderContext& rc);
	Player* m_player;
	Game* m_game;

	ModelRender m_goalPostRender;//モデルレンダー
	Vector3 m_goalPostPosition = Vector3::Zero;//位置
	PhysicsStaticObject m_goalPostPhysicsStaticObject;//静的オブジェクト
	PhysicsGhostObject m_goalPostPhysicsGhostObject;//ゴーストオブジェクト

	SoundSource* m_warpSE;//ワープする音
private:
	
};

