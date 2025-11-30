#include "stdafx.h"
#include "GimmickEnemy.h"
#include "Player.h"
#include "Game.h"
#include "SoundManager.h"
namespace
{
	const char* GIMMICK_ENEMY_MODEL_FILE_PATH = "Assets/modelData/gimmickEnemy.tkm";
	const char* GIMMICK_ENEMY_SHELL_FILE_PATH = "Assets/modelData/gimmickEnemyShell.tkm";
	const char* GIMMICK_ENEMY_IDLE_ANIMATION_FILE_PATH = "Assets/animData/gimmickEnemyIdle.tka";
	const char* GIMMICK_ENEMY_WALK_ANIMATION_FILE_PATH = "Assets/animData/gimmickEnemyWalk.tka";
	const char* GIMMICK_ENEMY_ATTACK_ANIMATION_FILE_PATH = "Assets/animData/gimmickEnemyAttack.tka";
	const char* GIMMICK_ENEMY_FIRST_HIT_ANIMATION_FILE_PATH = "Assets/animData/gimmickEnemyFirstHit.tka";
	const char* GIMMICK_ENEMY_DIE_ANIMATION_FILE_PATH = "Assets/animData/gimmickEnemyDie.tka";

	const Vector3 GIMMICK_ENEMY_BODY_GHOST_OBJECT_POS = { 0.0f,50.0f,0.0f };//敵の体のゴーストオブジェクトの位置
	const Vector3 GIMMICK_ENEMY_BODY_GHOST_OBJECT_SCALE = { 230.0f,160.0f,230.0f };//敵の体のゴーストオブジェクトの大きさ
	const Vector3 GIMMICK_ENEMY_HEAD_GHOST_OBJECT_POS = { 0.0f,230.0f,0.0f };//敵の頭のゴーストオブジェクトの位置
	const Vector3 GIMMICK_ENEMY_HEAD_GHOST_OBJECT_SCALE = { 110.0f,90.0f,110.0f };//敵の頭のゴーストオブジェクトの大きさ
	const Vector3 GIMMICK_ENEMY_ATTACK_GHOST_OBJECT_POS = { 0.0f,130.0f,0.0f };//敵の攻撃専用のゴーストオブジェクトの位置
	const Vector3 GIMMICK_ENEMY_ATTACK_GHOST_OBJECT_SCALE = { 170.0f, 180.0f, 170.0f };//敵の攻撃専用のゴーストオブジェクトの大きさ

	const float GIMMICK_ENEMY_DISTANCE_CLOSE = 80.0f;//敵がプレイヤーに近づきすぎた時の距離
	const float DISTANCE = 600.0f;//敵がプレイヤーを追いかけ始める距離
	const float GIMMICK_ENEMY_SPEED = 200.0f;//敵の移動速度
	const float CHARACON_RADIUS = 40.0f;//敵のキャラコン(半径)
	const float CHARACON_HEIGHT = 75.0f;//敵のキャラコン(高さ)
	const float BOSS_ANIMATION_LIMIT = 1.5f;//敵のアニメーションが流れる時間の制限
	const float GRAVITATIONAL_ACCELERATION = 9.8f;//重力
	const float REAPAWN_LIMIT = -1000.0f;//リスポーンする時の地点
}

GimmickEnemy::GimmickEnemy()
{
}

GimmickEnemy::~GimmickEnemy()
{
	DeleteGO(p_playerHitDamageSE);
}

bool GimmickEnemy::Start()
{
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Idle].Load(GIMMICK_ENEMY_IDLE_ANIMATION_FILE_PATH);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Idle].SetLoopFlag(true);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Walk].Load(GIMMICK_ENEMY_WALK_ANIMATION_FILE_PATH);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Walk].SetLoopFlag(true);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Attack].Load(GIMMICK_ENEMY_ATTACK_ANIMATION_FILE_PATH);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Attack].SetLoopFlag(false);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_FirstHit].Load(GIMMICK_ENEMY_FIRST_HIT_ANIMATION_FILE_PATH);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_FirstHit].SetLoopFlag(false);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Die].Load(GIMMICK_ENEMY_DIE_ANIMATION_FILE_PATH);
	p_gimmickEnemyAnimationClips[enGimmickEnemyAnimationClip_Die].SetLoopFlag(false);

	p_gimmickEnemyModelRender.Init(GIMMICK_ENEMY_MODEL_FILE_PATH, p_gimmickEnemyAnimationClips, enGimmickEnemyAnimationClip_Num, enModelUpAxisZ);
	p_gimmickEnemyModelRender.SetPosition(m_gimmickEnemyPos);
	p_gimmickEnemyModelRender.SetRotation(p_gimmickEnemyRotation);
	p_gimmickEnemyModelRender.SetScale(p_gimmickEnemyScale);

	p_gimmickEnemyShellModelRender.Init(GIMMICK_ENEMY_SHELL_FILE_PATH);
	p_gimmickEnemyShellModelRender.SetPosition(m_gimmickEnemyPos);
	p_gimmickEnemyShellModelRender.SetRotation(p_gimmickEnemyRotation);
	p_gimmickEnemyShellModelRender.SetScale(p_gimmickEnemyShellScale);
	
	p_gimmickEnemyCharacterController.Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_gimmickEnemyPos);

	//ゴーストオブジェクトの作成
	p_gimmickEnemyBodyGhostObject.CreateBox(m_gimmickEnemyPos + GIMMICK_ENEMY_BODY_GHOST_OBJECT_POS, Quaternion::Identity, GIMMICK_ENEMY_BODY_GHOST_OBJECT_SCALE);
	p_gimmickEnemyHeadGhostObject.CreateBox(m_gimmickEnemyPos + GIMMICK_ENEMY_HEAD_GHOST_OBJECT_POS, Quaternion::Identity, GIMMICK_ENEMY_HEAD_GHOST_OBJECT_SCALE);
	p_gimmickEnemyAttackGhostObject.CreateBox(m_gimmickEnemyPos + GIMMICK_ENEMY_ATTACK_GHOST_OBJECT_POS, Quaternion::Identity, GIMMICK_ENEMY_ATTACK_GHOST_OBJECT_SCALE);
	p_player = FindGO<Player>("player");

	p_game = FindGO<Game>("game");

	return true;
}

void GimmickEnemy::Update()
{
	//ポーズ中なら何もしない。
	if (p_game->m_isPause)
	{
		return;
	}
	GimmickEnemyMove();
	GimmickEnemyRotation();
	GimmickEnemyManageState();
	PlayGimmickEnemyAnimation();
	GimmickEnemyAttack();
	StepOnGimmickEnemy();
	GimmickEnemyGhostObjectManager();
	Respawn();
	p_gimmickEnemyModelRender.Update();
	p_gimmickEnemyShellModelRender.Update();
}

void GimmickEnemy::GimmickEnemyMove()
{
	if (p_damageCount != 0)
	{
		return;
	}

	if (!p_isDead)
	{
		//プレイヤーがいないのなら何もしない。
		if (p_player == nullptr)
		{
			return;
		}

		//プレイヤーとの距離を測る。
		Vector3 diff = p_player->m_playerPosition - m_gimmickEnemyPos;
		
		if (diff.Length() < GIMMICK_ENEMY_DISTANCE_CLOSE)
		{
			//重力を初期化
			p_gravityFrameTime = 0.0f;
			//移動速度を初期化
			p_gimmickEnemyMoveSpeed.x = 0.0f;
			p_gimmickEnemyMoveSpeed.z = 0.0f;
			//フラグを動いてない状態にする
			p_isMove = false;
			return;
		}
		
		else if (diff.Length() <= DISTANCE)
		{
			diff.y = 0.0f;
			//移動方向を正規化(距離を1にする)
			diff.Normalize();

			//敵の位置を更新
			p_gimmickEnemyMoveSpeed.x = diff.x * GIMMICK_ENEMY_SPEED;
			p_gimmickEnemyMoveSpeed.z = diff.z * GIMMICK_ENEMY_SPEED;

			//フラグを動く状態にする
			p_isMove = true;
			p_isIdle = false;
		}

		else if (p_gimmickEnemyCharacterController.IsOnGround() == false)
		{
			p_gimmickEnemyMoveSpeed.y -= GRAVITATIONAL_ACCELERATION * GRAVITATIONAL_ACCELERATION * AddGameFrame(p_gravityFrameTime);
		}

		else
		{
			//重力を初期化
			p_gravityFrameTime = 0.0f;
			//移動速度を初期化
			p_gimmickEnemyMoveSpeed.x = 0.0f;
			p_gimmickEnemyMoveSpeed.z = 0.0f;
			//フラグを動いてない状態にする
			p_isMove = false;
			p_isIdle = true;
		}

		//キャラクターコントローラーに位置を教える
		m_gimmickEnemyPos = p_gimmickEnemyCharacterController.Execute(p_gimmickEnemyMoveSpeed, g_gameTime->GetFrameDeltaTime());

	}
	//絵描きさんに位置を教える
	p_gimmickEnemyModelRender.SetPosition(m_gimmickEnemyPos);
	p_gimmickEnemyShellModelRender.SetPosition(m_gimmickEnemyPos);
}

void GimmickEnemy::GimmickEnemyRotation()
{
	if (p_isMove)
	{
		//キャラクターの方向を変える。
		p_gimmickEnemyRotation.SetRotationYFromDirectionXZ(p_gimmickEnemyMoveSpeed);
		//絵描きさんに回転を教える。
		p_gimmickEnemyModelRender.SetRotation(p_gimmickEnemyRotation);
		return;
	}

	if (p_isIdle)
	{
		p_gimmickEnemyRotation.AddRotationDegY(0.5f);
		p_gimmickEnemyModelRender.SetRotation(p_gimmickEnemyRotation);
	}
}

void GimmickEnemy::GimmickEnemyManageState()
{
	if (p_isDead)
	{
		p_gimmickEnemyState = enGimmickEnemyAnimationClip_Die;
		return;
	}
	if (p_damageCount == 1)
	{
		p_gimmickEnemyState = enGimmickEnemyAnimationClip_FirstHit;
		return;
	}

	else if (p_isMove)
	{
		p_gimmickEnemyState = enGimmickEnemyAnimationClip_Walk;
		return;
	}

	else if (p_isAttack)
	{
		p_gimmickEnemyState = enGimmickEnemyAnimationClip_Attack;
	}

	else
	{
		p_gimmickEnemyState = enGimmickEnemyAnimationClip_Idle;
	}
}

void GimmickEnemy::PlayGimmickEnemyAnimation()
{
	//switch文。
	switch (p_gimmickEnemyState) {
		//プレイヤーステートが(待機)だったら。
	case enGimmickEnemyState_Idle:
		//待機アニメーションを再生する。
		p_gimmickEnemyModelRender.PlayAnimation(enGimmickEnemyAnimationClip_Idle);
		break;
		//エネミーステートが(歩き)だったら
	case enGimmickEnemyState_Walk:
		//歩きアニメーションを再生する
		p_gimmickEnemyModelRender.PlayAnimation(enGimmickEnemyAnimationClip_Walk);
		break;
		//エネミーステートが(一回目のプレイヤーの攻撃を受けた)だったら
	case enGimmickEnemyAnimationClip_FirstHit:
		//一回目のヒットアニメーションを再生する
		p_gimmickEnemyModelRender.PlayAnimation(enGimmickEnemyAnimationClip_FirstHit);
		break;
		//エネミーステートが(死亡)だったら
	case enGimmickEnemyState_Die:
		//死亡アニメーションを再生する
		p_gimmickEnemyModelRender.PlayAnimation(enGimmickEnemyAnimationClip_Die);
		break;
		//エネミーステートが(攻撃)だったら
	case enGimmickEnemyState_Attack:
		//攻撃アニメーションを再生する
		p_gimmickEnemyModelRender.PlayAnimation(enGimmickEnemyAnimationClip_Attack);
		if (p_gimmickEnemyModelRender.IsPlayingAnimation())
		{
			if (p_attackAnimationTime > 0.7f)
			{
				p_isPlayAttackAnimation = true;
			}
		}
		break;
	}
}

void GimmickEnemy::StepOnGimmickEnemy()
{
	if (p_isDead)
	{
		p_deadAnimationTime += g_gameTime->GetFrameDeltaTime();
	}

	if (p_deadAnimationTime > BOSS_ANIMATION_LIMIT)
	{
		p_damageCount = 0;
		p_deadAnimationTime = 0.0f;
		p_isDead = false;
		//クリアカウントを増やす
		p_player->m_clearCount++;
		//敵を削除する
		DeleteGO(p_hitAndDieSE);
		DeleteGO(this);
		return;
	}

	bool isTouching = false;

	PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
		{
			//敵の頭のゴーストオブジェクトと接触したら
			if (p_gimmickEnemyHeadGhostObject.IsSelf(contactObject))
			{
				isTouching = true;

				p_player->m_playerMoveSpeed.y = 1900.0f;//プレイヤーを跳ねさせる
			}
		});

	if (p_damageCount == 2)
	{
		p_isDead = true;
	}

	if (isTouching && !p_isDamage)
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_hitAndDieSE = sound->PlayingSound(enSound_GimmickenemyHitAndDieSE, false);
		p_isDamage = true;
		p_damageCount++;
	}

	if (!isTouching && p_isDamage)
	{
		p_isDamage = false;
	}

	p_isDamage = isTouching;
}

void GimmickEnemy::GimmickEnemyGhostObjectManager()
{
	if (p_damageCount == 1)
	{
		p_gimmickEnemyBodyGhostObject.SetPosition(m_gimmickEnemyPos + GIMMICK_ENEMY_BODY_GHOST_OBJECT_POS);
		p_gimmickEnemyHeadGhostObject.SetPosition(m_gimmickEnemyPos);
		return;
	}

	else
	//ゴーストオブジェクトを常に敵の位置の合わせる
	p_gimmickEnemyBodyGhostObject.SetPosition(m_gimmickEnemyPos + GIMMICK_ENEMY_BODY_GHOST_OBJECT_POS);
	p_gimmickEnemyHeadGhostObject.SetPosition(m_gimmickEnemyPos + GIMMICK_ENEMY_HEAD_GHOST_OBJECT_POS);
}

void GimmickEnemy::GimmickEnemyAttack()
{
	if (p_damageCount == 0)
	{
		Vector3 forward = Vector3::AxisZ;
		p_gimmickEnemyRotation.Apply(forward);
		forward.Normalize();

		p_gimmickEnemyAttackGhostObject.SetPosition(m_gimmickEnemyPos + GIMMICK_ENEMY_ATTACK_GHOST_OBJECT_POS + forward * 100.0f);
		p_gimmickEnemyAttackGhostObject.SetRotation(p_gimmickEnemyRotation);


		if (p_isAttack)
		{
			p_attackAnimationTime += g_gameTime->GetFrameDeltaTime();
		}

		if (p_attackAnimationTime > BOSS_ANIMATION_LIMIT)
		{
			p_attackAnimationTime = 0.0f;
			p_isPlayAttackAnimation = false;
			p_hasAlreadyDamaged = false;
			p_isAttack = false;
			return;
		}


		PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
			{
				if (p_gimmickEnemyBodyGhostObject.IsSelf(contactObject))
				{
					if (!p_isAttack)
					{
						p_isAttack = true;
						p_attackAnimationTime = 0.0f;
						p_hasAlreadyDamaged = false;
					}
				}

				if (p_gimmickEnemyAttackGhostObject.IsSelf(contactObject)
					&& p_isPlayAttackAnimation
					&& !p_hasAlreadyDamaged)
				{
					SoundManager* sound = FindGO<SoundManager>("soundmanager");
					p_playerHitDamageSE = sound->PlayingSound(enSound_PlayerHitDamageSE, false, 1.5f);
					p_player->m_playerHP--;
					p_hasAlreadyDamaged = true;
				}
			});
	}
}

void GimmickEnemy::Respawn()
{
	if (m_gimmickEnemyPos.y <= REAPAWN_LIMIT)
	{
		m_gimmickEnemyPos = m_gimmickEnemyInitalPos;//初期位置に戻す
		p_gimmickEnemyMoveSpeed = Vector3::Zero;//移動速度もリセット
		p_gimmickEnemyCharacterController.SetPosition(m_gimmickEnemyPos);//物理エンジンにも更新させる
		p_gimmickEnemyModelRender.SetPosition(m_gimmickEnemyPos);//モデルにも位置を更新させる
	}
}

void GimmickEnemy::Render(RenderContext& rc)
{
	if (p_damageCount == 1)
	{
		p_gimmickEnemyShellModelRender.Draw(rc);
	}

	else
	p_gimmickEnemyModelRender.Draw(rc);

}
