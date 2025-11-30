#pragma once


struct BossStageGhostObjectInfo
{
	Vector3 bossStageGhostObjectPos = Vector3::Zero;
	Vector3 bossStageGhostObjectScale = Vector3::Zero;
};


const BossStageGhostObjectInfo BOSS_STAGE_GHOSTOBJECT_INFOMATIONS[] =
{
	{{0.0f,2000.0f,3750.0f},{7000.0f,5000.0f,300.0f}},//ボスステージのボス側正面のゴーストオブジェクト
	{{0.0f,2000.0f,-3500.0f},{7000.0f,5000.0f,300.0f}},//ボスステージのボス側後方のゴーストオブジェクト
	{{3350.0f,2000.0f,0.0f},{300.0f,5000.0f,8000.0f}},//ボスステージのボス側右のゴーストオブジェクト
	{{-3100.0f,2000.0f,0.0f},{300.0f,5000.0f,8000.0f}},//ボスステージのボス側左のゴーストオブジェクト
};


class Player;
class Game;


class BossStage : public IGameObject
{
public:
	BossStage();
	~BossStage();
	bool Start();
	void Update();
	void TouchOnBossStageGhostObject();
	void Render(RenderContext& rc);

	ModelRender m_bossStageRender;//モデルレンダー
	PhysicsStaticObject m_bossStagePhysicsStaticObject;//静的オブジェクト

	enum EnGhostObjectInstance
	{
		enGhostObject_One,
		enGhostObject_Two,
		enGhostObject_Three,
		enGhostObject_Four,
		enGhostObject_Num,
	};
	PhysicsGhostObject m_bossStageGhostObjects[enGhostObject_Num];//ゴーストオブジェクト


private:
	Player* p_player;
	Game* p_game;
	SoundSource* p_bossBGM;
};

