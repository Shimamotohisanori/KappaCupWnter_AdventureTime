#include "stdafx.h"
#include "BossStage.h"
#include "Player.h"
#include "SoundManager.h"
#include "Game.h"
BossStage::BossStage()
{
}


BossStage::~BossStage()
{
	DeleteGO(p_bossBGM);
}


bool BossStage::Start()
{
	//m_bossStageRender.Init("Assets/modelData/bossStage.tkm");
	m_bossStageRender.Init("Assets/modelData/stage3.tkm");
	m_bossStagePhysicsStaticObject.CreateFromModel(m_bossStageRender.GetModel(),
		m_bossStageRender.GetModel().GetWorldMatrix());

	for (int i = 0; i < _countof(BOSS_STAGE_GHOSTOBJECT_INFOMATIONS); i++)
	{
		m_bossStageGhostObjects[i].CreateBox(
			BOSS_STAGE_GHOSTOBJECT_INFOMATIONS[i].bossStageGhostObjectPos,
			Quaternion::Identity,
			BOSS_STAGE_GHOSTOBJECT_INFOMATIONS[i].bossStageGhostObjectScale);
	}


	SoundManager* sound = FindGO<SoundManager>("soundmanager");

	p_bossBGM = sound->PlayingSound(enSound_BossBGM);

	p_player = FindGO<Player>("player");

	p_game = FindGO<Game>("game");

	return true;
}


void BossStage::Update()
{
	if (p_game->m_isPause)
	{
		p_bossBGM->Pause();
	}
	else
	{
		p_bossBGM->Play(true);
	}

	TouchOnBossStageGhostObject();
}

void BossStage::TouchOnBossStageGhostObject()
{

	//早期リターン
	if (p_player == nullptr)
	{
		return;
	}

	//プレイヤーのキャラコンとステージゴーストオブジェクトのすべてを見る
	PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
		{
			for (int i = 0; i < _countof(BOSS_STAGE_GHOSTOBJECT_INFOMATIONS); i++)
			{
				//もしどこかのゴーストオブジェクトにプレイヤーのキャラコンが触れていたら
				if (m_bossStageGhostObjects[i].IsSelf(contactObject))
				{
					//プレイヤーのポジションをリセットして、初期位置に戻す
					p_player->ResetPosition();
				}
			}
		});
}

void BossStage::Render(RenderContext& rc)
{
	m_bossStageRender.Draw(rc);
}
