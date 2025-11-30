#include "stdafx.h"
#include "GoalPost.h"
#include "Player.h"
#include "Game.h"
#include "SoundManager.h"
namespace
{
	const Vector3 GOAL_GHOSTOBJECT_POS = { 0.0f,600.0f,0.0f };
	const Vector3 GOAL_GHOSTOBJECT_SIZE = { 300.0f,800.0f,300.0f };

	const int GAME_END_CLEARCOUNT = 8;
}
GoalPost::GoalPost()
{
}

GoalPost::~GoalPost()
{
	DeleteGO(m_warpSE);
}

bool GoalPost::Start()
{
	m_goalPostRender.Init("Assets/modelData/goal.tkm");
	m_player = FindGO<Player>("player");

	m_game = FindGO<Game>("game");

	m_goalPostRender.SetPosition(m_goalPostPosition);
	m_goalPostRender.Update();

	m_goalPostPhysicsStaticObject.CreateFromModel(m_goalPostRender.GetModel(),
		m_goalPostRender.GetModel().GetWorldMatrix());

	m_goalPostPhysicsGhostObject.CreateBox(Vector3{( GOAL_GHOSTOBJECT_POS,GOAL_GHOSTOBJECT_POS,m_goalPostPosition )}, Quaternion::Identity, GOAL_GHOSTOBJECT_SIZE);
	return true;
}

void GoalPost::Update()
{
	TouchOnGoalPost();
}

void GoalPost::TouchOnGoalPost()
{
	PhysicsWorld::GetInstance()->ContactTest(m_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
		{
			//ゴールポストのゴーストオブジェクトに触れたら

			if (m_goalPostPhysicsGhostObject.IsSelf(contactObject) && m_game->m_isTutorialClear)
			{
				SoundManager* sound = FindGO<SoundManager>("soundmanger");
				m_warpSE = sound->PlayingSound(enSound_WarpSE, false);
				m_game->m_isNewMainStage = true;
				return;
			}

			if (m_goalPostPhysicsGhostObject.IsSelf(contactObject) && m_game->m_isMainClear)
			{
				m_game->m_isNewBossStage = true;
				SoundManager* sound = FindGO<SoundManager>("soundmanger");
				m_warpSE = sound->PlayingSound(enSound_WarpSE, false);
				return;
			}

			if (m_goalPostPhysicsGhostObject.IsSelf(contactObject) && m_player->m_clearCount == GAME_END_CLEARCOUNT)
			{
				m_game->m_isNewGameClear = true;
				SoundManager* sound = FindGO<SoundManager>("soundmanger");
				m_warpSE = sound->PlayingSound(enSound_WarpSE, false);
				return;
			}
		});
}

void GoalPost::Render(RenderContext& rc)
{
	m_goalPostRender.Draw(rc);
}
