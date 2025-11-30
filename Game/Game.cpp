#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "Stage.h"
#include "GameCamera.h"
#include "Enemy.h"
#include "MainStage.h"
#include "GoalPost.h"
#include "GimmickEnemy.h"
#include "BossStage.h"
#include "Boss.h"
#include "GameClear.h"
#include "GameOver.h"
#include "GameTitle.h"
#include "SoundManager.h"


namespace
{
	const char* MAXHP_UI_FILE_PATH = "Assets/sprite/MaxHPUI.dds";
	const char* HALFHP_UI_FILE_PATH = "Assets/sprite/HalfHPUI.dds";
	const char* DYINGHP_UI_FILE_PATH = "Assets/sprite/DyingHPUI.dds";
	const char* HP_BAR_BACK_FILE_PATH = "Assets/sprite/HPBar1.dds";
	const char* GAME_TIMER_SPRITE_FILE_PATH = "Assets/sprite/Time.dds";
	const char* PAUSE_SPRITE_FILE_PATH = "Assets/sprite/Pause.dds";
	const char* PAUSE_MENU_SPRITE_FILE_PATH = "Assets/sprite/PauseMenu.dds";

	const Vector4 PAUSE_SCREEN_COLOR = { 1.0f,1.0f,1.0f,0.4f };//ポーズ画面の色
	const Vector4 RETURN_TITLE_FONT_COLOR = { 1.0f,0.0f,0.0f,1.0f };//タイトルに戻るフォントを見ているときの色
	const Vector4 CONTINUE_FONT_COLOR = { 1.0f,0.0f,0.0f,1.0f };//ゲームに戻るフォントを見ているときの色

	const Vector3 BOSS_NEWGO_POS = { 0.0f,0.0f,3000.0f };//ボスがNewGOされるときに出てくるポジション
	const Vector3 GAME_TIMER_FONT_POSITION = { 600.0f, 500.0f, 0.0f };//ゲームタイマーフォントのポジション
	const Vector3 HP_BAR_POSITION = { -700.0f, -410.0f, 0.0f };//HPBarのポジション
	const Vector3 HP_BAR_BACK_POSITION = { -700.0f,-400.0f, 0.0f };//HPBarの背景画像の位置
	const Vector3 FIRST_GOALPOST_POS = { 0.0f,0.0f,-5500.0f };//最初に出てくるゴールポストの位置
	const Vector3 SECOND_GOALPOST_POS = { 0.0f,0.0f,-5500.0f };//2回目に出てくるゴールポストの位置
	const Vector3 GAME_TIME_SPRITE_POSITION = { 725.0f,460.0f,0.0f };//ゲームタイマーの背景画像の位置
	const Vector3 RETURN_TITLE_FONT_POSITION = { -245.0f, 80.0f, 0.0f };//タイトルに戻るフォントの位置
	const Vector3 CONTINUE_FONT_POSITION = { -215.0f, 0.0f, 0.0f };//コンティニューフォントの位置

	const float GAME_TIMER_FONT_SCALE = 2.0f;//ゲームタイマーフォントのスケール
	const float RETURN_TITLE_FONT_SCALE = 1.5f;//タイトルに戻るフォントのスケール
	const float CONTINUE_FONT_SCALE = 1.5f;//コンティニューフォントのスケール

	const int GAME_TIMER_LIMIT = 0;//ゲームタイマーが止まる時間
	const int FIRST_CLEARCOUNT = 3;
	const int SECOND_CLEARCOUNT = 7;
	const int THREE_CLEARCOUNT = 8;
}


Game::~Game()
{
	DeleteGO(p_player);
	DeleteGO(p_gameCamera);

	if (p_pauseChooseSE != nullptr)
	{
		DeleteGO(p_pauseChooseSE);
	}

	if (p_pauseStartSE != nullptr)
	{
		DeleteGO(p_pauseStartSE);
	}

	if (p_pauseEndSE != nullptr)
	{
		DeleteGO(p_pauseEndSE);
	}

	if (p_goalPost != nullptr)
	{
		DeleteGO(p_goalPost);
		p_goalPost = nullptr;
	}

	if (p_tutorialStage != nullptr)
	{
		DeleteGO(p_tutorialStage);
		p_tutorialStage = nullptr;
	}

	if (p_enemy != nullptr)
	{
		for (int i = 0; i < _countof(ENEMY_INFOMATIONS); i++)
		{
			if (p_enemy[i] == nullptr)
			{
				continue;
			}

			DeleteGO(FindGO<Enemy>(ENEMY_INFOMATIONS[i].enemyName.c_str()));
			p_enemy[i] = nullptr;
		}
	}

	if (p_mainStage != nullptr)
	{
		DeleteGO(p_mainStage);
		p_mainStage = nullptr;
	}

	if (p_gimmickEnemy != nullptr)
	{
		for (int i = 0; i < _countof(GIMMICK_ENEMY_INFOMATIONS); i++)
		{
			if (p_gimmickEnemy[i] == nullptr)
			{
				continue;
			}

			DeleteGO(FindGO<GimmickEnemy>(GIMMICK_ENEMY_INFOMATIONS[i].gimmickEnemyName.c_str()));
			p_gimmickEnemy[i] = nullptr;
		}
	}

	if (p_bossStage != nullptr)
	{
		DeleteGO(p_bossStage);
		p_bossStage = nullptr;
	}

	if (p_boss != nullptr)
	{
		DeleteGO(p_boss);
		p_boss = nullptr;
	}

}


bool Game::Start()
{
	p_player = NewGO<Player>(0, "player");

	p_gameCamera = NewGO<GameCamera>(0, "gamecamera");

	p_tutorialStage = NewGO<Stage>(0, "stage");

	p_maxHPSpriteRnder.Init(MAXHP_UI_FILE_PATH, 300.0f, 100.0f);
	p_maxHPSpriteRnder.SetPosition(HP_BAR_POSITION);

	p_halfHPSpriteRender.Init(HALFHP_UI_FILE_PATH, 300.0f, 100.0f);
	p_halfHPSpriteRender.SetPosition(HP_BAR_POSITION);

	p_dyingHPSpriteRender.Init(DYINGHP_UI_FILE_PATH, 300.0f, 100.0f);
	p_dyingHPSpriteRender.SetPosition(HP_BAR_POSITION);

	p_hpBarBackSpriteRender.Init(HP_BAR_BACK_FILE_PATH, 500.0f, 275.0f);
	p_hpBarBackSpriteRender.SetPosition(HP_BAR_BACK_POSITION);

	p_timeSpriteRender.Init(GAME_TIMER_SPRITE_FILE_PATH, 400.0f, 200.0f);
	p_timeSpriteRender.SetPosition(GAME_TIME_SPRITE_POSITION);

	p_pauseSpriteRender.Init(PAUSE_SPRITE_FILE_PATH, 1920.0f, 1080.0f);

	p_pauseMenuSpriteRender.Init(PAUSE_MENU_SPRITE_FILE_PATH, 800.0f, 500.0f);
	
	p_returnTitleFontRender.SetText(L"タイトルに戻る");
	p_returnTitleFontRender.SetPosition(RETURN_TITLE_FONT_POSITION);
	p_returnTitleFontRender.SetScale(RETURN_TITLE_FONT_SCALE);

	p_continueFontRender.SetText(L"ゲームに戻る");
	p_continueFontRender.SetPosition(CONTINUE_FONT_POSITION);
	p_continueFontRender.SetScale(CONTINUE_FONT_SCALE);

	//クリボーの生成
	CreateEnemy();

	InitSky();

	return true;
}


void Game::Update()
{
	
	CreateGoal();//ゴールポストの生成
	CreateMainStage();//メインステージの生成
	CreateBossStage();//ボスステージの生成
	CreateGameClear();//ゲームクリア画面の生成
	CreateGameOver();//ゲームオーバー画面の生成
	GameTimer();//タイマー
	HPBarManager();//HP管理の関数
	PauseGame();//ポーズ画面を出す関数
	PauseSceneManager();//ポーズ画面の管理
	//当たり判定を可視化する。
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();
}


void Game::InitSky()
{
	if (m_isNewBossStage)
	{
		DeleteGO(p_skyCube);

		p_skyCube = NewGO<SkyCube>(0, "SkyCube");
		p_skyCubeType = enSkyCubeType_Night;
		p_skyCube->SetType((EnSkyCubeType)p_skyCubeType);

		//環境光の計算のためのIBLテクスチャの設定
		g_renderingEngine->SetAmbientByIBLTexture(p_skyCube->GetTextureFilePath(), 0.5f);
		return;
	}

	DeleteGO(p_skyCube);

	p_skyCube = NewGO<SkyCube>(0, "SkyCube");
	
	p_skyCube->SetType((EnSkyCubeType)p_skyCubeType);

	//環境光の計算のためのIBLテクスチャの設定
	g_renderingEngine->SetAmbientByIBLTexture(p_skyCube->GetTextureFilePath(), 0.5f);
}


void Game::CreateEnemy()
{
	for (int i = 0; i < _countof(ENEMY_INFOMATIONS); i++)
	{
		p_enemy[i] = NewGO<Enemy>(0, ENEMY_INFOMATIONS[i].enemyName.c_str());
		p_enemy[i]->m_enemyPos = ENEMY_INFOMATIONS[i].enemyPos;
		p_enemy[i]->m_enemyInitalPos = ENEMY_INFOMATIONS[i].enemyPos;
	}
}


void Game::CreateGimmickEnemy()
{
	for (int i = 0; i < _countof(GIMMICK_ENEMY_INFOMATIONS); i++)
	{
		p_gimmickEnemy[i] = NewGO<GimmickEnemy>(0, GIMMICK_ENEMY_INFOMATIONS[i].gimmickEnemyName.c_str());
		p_gimmickEnemy[i]->m_gimmickEnemyPos = GIMMICK_ENEMY_INFOMATIONS[i].gimmickEnemyPos;
		p_gimmickEnemy[i]->m_gimmickEnemyInitalPos = GIMMICK_ENEMY_INFOMATIONS[i].gimmickEnemyPos;
	}
}


void Game::CreateGoal()
{
	if (p_player->m_clearCount == FIRST_CLEARCOUNT && !m_isNewGoalPost1)
	{
		p_goalPost = NewGO<GoalPost>(0, "goalpost");
		p_goalPost->m_goalPostPosition = FIRST_GOALPOST_POS;
		m_isNewGoalPost1 = true;
		m_isTutorialClear = true;
	}

	if (p_player->m_clearCount == SECOND_CLEARCOUNT && !m_isNewGoalPost2)
	{
		p_goalPost = NewGO<GoalPost>(0, "goalpost");
		p_goalPost->m_goalPostPosition = SECOND_GOALPOST_POS;
		m_isNewGoalPost2 = true;
		m_isMainClear = true;
	}

	if (p_player->m_clearCount == THREE_CLEARCOUNT && !m_isNewGoalPost3)
	{
		p_goalPost = NewGO<GoalPost>(0, "goalpost");
		m_isNewGoalPost3 = true;
	}
}


void Game::CreateMainStage()
{
	if (m_isNewMainStage && p_player->m_clearCount == FIRST_CLEARCOUNT)
	{
		p_player->ResetPosition();
		DeleteGO(p_goalPost);
		p_goalPost = nullptr;
		m_isNewMainStage = false;
		m_isTutorialClear = false;
		CreateGimmickEnemy();
	}
}


void Game::CreateBossStage()
{
	if (m_isNewBossStage && p_player->m_clearCount == SECOND_CLEARCOUNT)
	{
		p_bossStage = NewGO<BossStage>(0, "bossstage");
		InitSky();
		p_player->ResetPosition();
		DeleteGO(p_tutorialStage);
		p_tutorialStage = nullptr;
		DeleteGO(p_goalPost);
		//ゴールポストは複数回デリートするので、消した後は必ずnullptrにしておく
		p_goalPost = nullptr;
		m_isNewBossStage = false;
		m_isMainClear = false;
		p_boss = NewGO<Boss>(0, "boss");
		p_boss->m_bossPos = BOSS_NEWGO_POS;
		p_boss->m_bossInitalPos = BOSS_NEWGO_POS;
	}
}


void Game::CreateGameClear()
{
	if (m_isNewGameClear)
	{
		p_gameClear = NewGO<GameClear>(0, "gameclear");
		m_isNewGameClear = false;
		DeleteGO(p_goalPost);
		//ゴールポストは複数回デリートするので、消した後は必ずnullptrにしておく
		p_goalPost = nullptr;
		DeleteGO(p_bossStage);
		p_goalPost = nullptr;
		DeleteGO(this);
	}
}


void Game::CreateGameOver()
{
	if (p_player->m_isPlayerDead)
	{
		p_gameOver = NewGO<GameOver>(0, "gameover");
		p_player->m_isPlayerDead = false;
		
		DeleteGO(this);
	}
}


void Game::GameTimer()
{
	if (m_isPause)
	{
		return;
	}
	p_gameTimer -= g_gameTime->GetFrameDeltaTime();
	p_gameTimeMinute = (int)p_gameTimer / 60;
	p_gameTimeSecond = (int)p_gameTimer % 60;
	wchar_t t_timer[256];
	swprintf_s(t_timer, 256, L"%02d:%02d", p_gameTimeMinute,p_gameTimeSecond);
	p_timerFontRender.SetText(t_timer);
	p_timerFontRender.SetPosition(GAME_TIMER_FONT_POSITION);
	p_timerFontRender.SetScale(GAME_TIMER_FONT_SCALE);
	p_timerFontRender.SetColor(g_vec4White);

	p_timeSpriteRender.Update();

	if (p_gameTimer < GAME_TIMER_LIMIT)
	{
		p_gameTimer = 0.0f;
		p_player->m_isPlayerDead = true;
	}
}


void Game::HPBarManager()
{
	p_hpBarBackSpriteRender.Update();
	p_dyingHPSpriteRender.Update();
	p_halfHPSpriteRender.Update();
	p_maxHPSpriteRnder.Update();
}


void Game::PauseGame()
{
	//もう一度Enterキーが押されたらポーズ解除
	if (g_pad[0]->IsTrigger(enButtonStart) && m_isPause)
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_pauseEndSE = sound->PlayingSound(enSound_PauseEnd,false);
		m_isPause = false;
		return;
	}

	//Enterキーが押されたらポーズフラグをオンにして、ポーズ画面表示
	if (g_pad[0]->IsTrigger(enButtonStart) &&!m_isPause)
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_pauseStartSE = sound->PlayingSound(enSound_PauseStart,false);
		m_isPause = true;
	}
	p_pauseMenuSpriteRender.Update();
	p_pauseSpriteRender.Update();
}


void Game::PauseSceneManager()
{

	if (g_pad[0]->IsTrigger(enButtonDown))
	{
		SoundManager* choosesound = FindGO<SoundManager>("soundmanager");
		p_pauseChooseSE = choosesound->PlayingSound(enSound_PauseChooseSE,false);
		p_isLookContinueFont = true;
		p_isLookReturnTitleFont = false;
	}

	if (g_pad[0]->IsTrigger(enButtonUp))
	{
		SoundManager* choosesound = FindGO<SoundManager>("soundmanager");
		p_pauseChooseSE = choosesound->PlayingSound(enSound_PauseChooseSE, false);
		p_isLookReturnTitleFont = true;
		p_isLookContinueFont = false;
	}

	if (m_isPause && p_isLookReturnTitleFont && g_pad[0]->IsTrigger(enButtonY))
	{
		p_gameTitle = NewGO<GameTitle>(0, "gametitle");
		DeleteGO(this);
	}

	if (m_isPause && p_isLookContinueFont && g_pad[0]->IsTrigger(enButtonY))
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_pauseEndSE = sound->PlayingSound(enSound_PauseEnd, false);
		m_isPause = false;
	}
}


void Game::Render(RenderContext& rc)
{
	p_timerFontRender.Draw(rc);
	p_hpBarBackSpriteRender.Draw(rc);
	p_timeSpriteRender.Draw(rc);
	
	switch (p_player->m_playerHP)
	{
	case 1:
		p_dyingHPSpriteRender.Draw(rc);
		break;
	case 2:
		p_halfHPSpriteRender.Draw(rc);
		break;
	case 3:
		p_maxHPSpriteRnder.Draw(rc);
		break;
	default:
		break;
	}
	if (m_isPause)
	{
		p_pauseSpriteRender.SetMulColor(PAUSE_SCREEN_COLOR);
		p_pauseSpriteRender.Draw(rc);
		p_pauseMenuSpriteRender.Draw(rc);
		p_returnTitleFontRender.SetColor(g_vec4White);
		p_continueFontRender.SetColor(g_vec4White);
		p_returnTitleFontRender.Draw(rc);
		p_continueFontRender.Draw(rc);

		//もしタイトルに戻るフォントを見ていたら赤くする
		if (p_isLookReturnTitleFont)
		{
			p_returnTitleFontRender.SetColor(RETURN_TITLE_FONT_COLOR);
		}

		//もしコンティニューフォントを見ていたら赤くする
		if (p_isLookContinueFont)
		{
			p_continueFontRender.SetColor(CONTINUE_FONT_COLOR);
		}
	}
}