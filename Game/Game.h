#pragma once

#include "Level3DRender/LevelRender.h"

class Player;
class Stage;
class GameCamera;
class Enemy;
class MainStage;
class GoalPost;
class GimmickEnemy;
class BossStage;
class Boss;
class GameClear;
class GameOver;
class GameTitle;

struct GimmickEnemyInfo
{
	std::string gimmickEnemyName;//敵の名前
	Vector3 gimmickEnemyPos = Vector3::Zero; //敵の位置
};

const GimmickEnemyInfo GIMMICK_ENEMY_INFOMATIONS[] =
{
	{"gimmickenemy1",{0.0f,0.0f,-5000.0f}},
	{"gimmickenemy2",{1700.0f,0.0f,-3500.0f}},
	{"gimmickenemy3",{-1700.0f,0.0f,-3500.0f}},
	{"gimmickenemy4",{0.0f,0.0f,-2300.f}},
};

struct EnemyInfo
{
	std::string enemyName;//敵の名前
	Vector3 enemyPos = Vector3::Zero; // 敵の位置
};

//敵の構造体の一覧
const EnemyInfo ENEMY_INFOMATIONS[] =
{
	{"enemy1",{0.0f,0.0f,-5000.0f}},
	{"enemy2",{1700.0f,0.0f,-3500.0f}},
	{"enemy3",{-1700.0f,0.0f,-3500.0f}},
};

class Game : public IGameObject
{
public:
	Game() {}
	~Game();
	bool Start();
	void Update();
	void InitSky();
	void CreateEnemy();
	void CreateGimmickEnemy();
	void CreateGoal();//ゴールポストの生成
	void CreateMainStage();//メインステージの生成
	void CreateBossStage();//ボスステージの生成
	void CreateGameClear();//ゲームクリア画面の生成
	void CreateGameOver();//ゲームオーバー画面の生成
	void GameTimer();//時間制限
	void HPBarManager();//HPを管理する関数
	void PauseGame();//ポーズ画面を出す関数
	void PauseSceneManager();//ポーズ画面の管理
	void Render(RenderContext& rc);

	bool m_isNewMainStage = false;//メインステージをNewGOできるかできないかのフラグ
	bool m_isNewBossStage = false;//ボスステージをNewGOできるかできないかのフラグ
	bool m_isNewGoalPost1 = false;//1回目のゴールポストを出すかどうかのフラグ
	bool m_isNewGoalPost2 = false;//2回目のゴールポストを出すかどうかのフラグ
	bool m_isNewGoalPost3 = false;//3回目のゴールポストを出すかどうかのフラグ
	bool m_isNewGameClear = false;//クリア画面を出すかどうかのフラグ
	bool m_isTutorialClear = false;//チュートリアルステージをクリアしたかどうか
	bool m_isMainClear = false;//メインステージをクリアしたかどうか
	bool m_isPause = false;//ポーズ中かどうかのフラグ
private:
	Player* p_player;
	Stage* p_tutorialStage;
	GameCamera* p_gameCamera;
	SkyCube* p_skyCube;
	MainStage* p_mainStage;
	BossStage* p_bossStage;
	GoalPost* p_goalPost;
	Boss* p_boss;
	GameClear* p_gameClear;
	GameOver* p_gameOver;
	GameTitle* p_gameTitle;
	FontRender p_timerFontRender;//タイマーのフォント
	FontRender p_returnTitleFontRender;//タイトルに戻るフォント
	FontRender p_continueFontRender;//コンティニューフォント
	SpriteRender p_pauseSpriteRender;//ポーズ画面の背景
	SpriteRender p_pauseMenuSpriteRender;//ポーズ画面のメニュー
	SpriteRender p_timeSpriteRender;//タイマーの背景
	SpriteRender p_hpBarBackSpriteRender;//HPバーの背景
	SpriteRender p_maxHPSpriteRnder;//満タンのHP
	SpriteRender p_halfHPSpriteRender;//半分のHP
	SpriteRender p_dyingHPSpriteRender;//瀕死のHP
	SoundSource* p_pauseStartSE;//ポーズスタート時のSE
	SoundSource* p_pauseEndSE;//ポーズスタート時のSE
	SoundSource* p_pauseChooseSE;//ポーズ画面のメニュー画面で選んでいるときの音

	enum EnEnemyInstance
	{
		enEnemyInstance_First,
		enEnemyInstance_Second,
		enEnemyInstance_Third,
		enEnemyInstance_Num,
	};
	Enemy* p_enemy[enEnemyInstance_Num];

	enum EnGimmickEnemyInstance
	{
		enGimmickEnemyInstance_First,
		enGimmickEnemyInstance_Second,
		enGimmickEnemyInstance_Third,
		enGimmickEnemyInstance_Four,
		enGimmickEnemyInstance_Num,
	};
	GimmickEnemy* p_gimmickEnemy[enGimmickEnemyInstance_Num];

	float p_gameTimer = 241.0f;//ゲーム時間

	int p_gameTimeMinute = 0;//分
	int p_gameTimeSecond = 0;//秒
	int p_skyCubeType = enSkyCubeType_Day; //ここで空の種類を選択

	bool p_isLookReturnTitleFont = false;//タイトルに戻るフォントを見ているかどうかのフラグ
	bool p_isLookContinueFont = false;//コンティニューフォントを見ているかどうかのフラグ
};
