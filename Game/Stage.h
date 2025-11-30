#pragma once
struct TutorialStageGhostObjectInfo
{
	Vector3 ghostObjectPos = Vector3::Zero;
	Vector3 ghostObjectScale = Vector3::Zero;
};

const TutorialStageGhostObjectInfo TUTORIAL_STAGE_GHOSTOBJECT_INFOMATIONS[] =
{
	//このゴーストオブジェクトの位置はプレイヤーがステージの奥を見た時の位置をコメントとして起こしています。
	{{0.0f,600.0f,1600.0f},{2000.0f,2000.0f,500.0}},//スタート地点の一番後ろ正面のゴーストオブジェクト
	{{1200.0f,600.0f,-600.0f},{900.0f,2000.0f,3800.0}},//スタート地点の左にあるゴーストオブジェクト
	{{-1200.0f,600.0f,-600.0f},{900.0f,2000.0f,3800.0}},//スタート地点の→にあるゴーストオブジェクト
	{{1800.0f,600.0f,-2600.0f},{1500.0f,2000.0f,800.0}},//中間左にある手前側のゴーストオブジェクト
	{{-1800.0f,600.0f,-2400.0f},{1500.0f,2000.0f,800.0}},//中間右にある手前側のゴーストオブジェクト
	{{2300.0f,600.0f,-3900.0f},{600.0f,2000.0f,1800.0}},//中間左にある中間のゴーストオブジェクト
	{{-2300.0f,600.0f,-3600.0f},{600.0f,2000.0f,2000.0}},//中間右にある中間のゴーストオブジェクト
	{{1800.0f,600.0f,-4700.0f},{1500.0f,2000.0f,800.0}},//中間左にある奥側のゴーストオブジェクト
	{{-1800.0f,600.0f,-4700.0f},{1800.0f,2000.0f,800.0}},//中間右にある奥側のゴーストオブジェクト
	{{1200.0f,600.0f,-5700.0f},{900.0f,2000.0f,2900.0}},//ゴール手前左にあるゴーストオブジェクト
	{{-1200.0f,600.0f,-5700.0f},{900.0f,2000.0f,2900.0}},//ゴール手前右にあるゴーストオブジェクト
	{{0.0f,600.0f,-7150.0f},{2000.0f,2000.0f,500.0}},//ゴール正面にあるゴーストオブジェクト
};

class Player;
class Game;


class Stage : public IGameObject
{
public:
	Stage();
	~Stage();
	bool Start();
	void Update();
	void TouchOnGhostObject();//ステージ端にあるゴーストオブジェクトに触れたら
	void Render(RenderContext& rc);

private:
	Game* p_game;
	Player* p_player;
	ModelRender p_tutorialStageRender;//モデルレンダー
	PhysicsStaticObject p_tutorialStagePhysicsStaticObject;//静的オブジェクト
	enum EnGhostObjectInstance
	{
		enGhostObject_One,
		enGhostObject_Two,
		enGhostObject_three,
		enGhostObject_Four,
		enGhostObject_Five,
		enGhostObject_Six,
		enGhostObject_Seven,
		enGhostObject_Eight,
		enGhostObject_Nine,
		enGhostObject_Ten,
		enGhostObject_Eleven,
		enGhostObject_Twelve,
		enGhostObject_Num,
	};
	PhysicsGhostObject p_tutorialStageGhostObject[enGhostObject_Num];//ゴーストオブジェクト
	SoundSource* p_inGameBGM;
};

