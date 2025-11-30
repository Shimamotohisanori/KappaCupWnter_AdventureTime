#include "stdafx.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "Game.h"
namespace
{
	const char* TUTORIAL_STAGE_MODEL_FILE_PATH = "Assets/modelData/stage1.tkm";
}
Stage::Stage()
{
}

Stage::~Stage()
{
	DeleteGO(p_inGameBGM);
}

bool Stage::Start()
{
	p_tutorialStageRender.Init(TUTORIAL_STAGE_MODEL_FILE_PATH);
	//p_tutorialStageRender.Init("Assets/modelData/tutorialStage.tkm");
	p_tutorialStagePhysicsStaticObject.CreateFromModel(p_tutorialStageRender.GetModel(),
		p_tutorialStageRender.GetModel().GetWorldMatrix());

	for (int i = 0; i < _countof(TUTORIAL_STAGE_GHOSTOBJECT_INFOMATIONS); i++)
	{
		p_tutorialStageGhostObject[i].CreateBox(TUTORIAL_STAGE_GHOSTOBJECT_INFOMATIONS[i].ghostObjectPos, Quaternion::Identity, TUTORIAL_STAGE_GHOSTOBJECT_INFOMATIONS[i].ghostObjectScale);
	}

	p_player = FindGO<Player>("player");

	p_game = FindGO<Game>("game");

	SoundManager* sound = FindGO<SoundManager>("soundmanager");
	p_inGameBGM = sound->PlayingSound(enSound_InGameBGM,true,2.0f);

	return true;
}

void Stage::Update()
{
	if(p_game->m_isPause)
	{
		p_inGameBGM->Pause();
	}
	else
	{
		p_inGameBGM->Play(true);
	}
	TouchOnGhostObject();
}

void Stage::TouchOnGhostObject()
{
	//早期リターン
	if (p_player == nullptr)
	{
		return;
	}

	//プレイヤーのキャラコンとステージゴーストオブジェクトのすべてを見る
	PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
		{
			for (int i = 0; i < _countof(TUTORIAL_STAGE_GHOSTOBJECT_INFOMATIONS); i++)
			{
				//もしどこかのゴーストオブジェクトにプレイヤーのキャラコンが触れていたら
				if (p_tutorialStageGhostObject[i].IsSelf(contactObject))
				{
					//プレイヤーのポジションをリセットして、初期位置に戻す
					p_player->ResetPosition();
				}
			}
		});
}

void Stage::Render(RenderContext& rc)
{
	p_tutorialStageRender.Draw(rc);
}
