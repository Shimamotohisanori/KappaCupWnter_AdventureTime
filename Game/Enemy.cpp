#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Game.h"
#include "SoundManager.h"
namespace
{
	const char* ENEMY_MODEL_FILE_PATH = "Assets/modelData/enemy.tkm";
	const char* ENEMY_IDLE_ANIMATION_FILE_PATH = "Assets/animData/enemyIdle1.tka";
	const char* ENEMY_WALK_ANIMATION_FILE_PATH = "Assets/animData/enemyWalk.tka";
	const char* ENEMY_DIE_ANIMATION_FILE_PATH = "Assets/animData/enemyDie.tka";
	const char* ENEMY_ATTACK_ANIMATION_FILE_PATH = "Assets/animData/enemyAttack.tka";

	const Vector3 ENEMY_BODY_GHOST_OBJECT_POS = { 0.0f,50.0f,0.0f };//敵の体のゴーストオブジェクトの位置
	const Vector3 ENEMY_BODY_GHOST_OBJECT_SCALE = { 250.0f,90.0f,250.0f };//敵の体のゴーストオブジェクトの大きさ
	const Vector3 ENEMY_HEAD_GHOST_OBJECT_POS = { 0.0f,180.0f,0.0f };//敵の頭のゴーストオブジェクトの位置
	const Vector3 ENEMY_HEAD_GHOST_OBJECT_SCALE = { 170.0f,40.0f,170.0f };//敵の頭のゴーストオブジェクトの大きさ
	const Vector3 ENEMY_ATTACK_GHOST_OBJECT_POS = { 0.0f,100.0f,0.0f };//敵の攻撃専用のゴーストオブジェクトの位置
	const Vector3 ENEMY_ATTACK_GHOST_OBJECT_SCALE = { 170.0f, 120.0f, 170.0f };//敵の攻撃専用のゴーストオブジェクトの大きさ

	const float DISTANCE = 600.0f;//敵がプレイヤーを追いかけ始める距離
	const float ENEMY_SPEED = 200.0f;//敵の移動速度
	const float CHARACON_RADIUS = 40.0f;//敵のキャラコン(半径)
	const float CHARACON_HEIGHT = 75.0f;//敵のキャラコン(高さ)
	const float ENEMY_ANIMATION_LIMIT = 1.5f;//敵のアニメーションが流れる時間の制限
	const float ENEMY_DISTANCE_CLOSE = 80.0f;//敵がプレイヤーに近づきすぎた時の距離
	const float GRAVITATIONAL_ACCELERATION = 9.8f;//重力
	const float REAPAWN_LIMIT = -1000.0f;//リスポーンする時の地点
}
Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	DeleteGO(p_playerHitDamageSE);
}

bool Enemy::Start()
{
	p_enemyAnimationClips[enEnemyAnimationClip_Idle].Load(ENEMY_IDLE_ANIMATION_FILE_PATH);
	p_enemyAnimationClips[enEnemyAnimationClip_Idle].SetLoopFlag(true);
	p_enemyAnimationClips[enEnemyAnimationClip_Walk].Load(ENEMY_WALK_ANIMATION_FILE_PATH);
	p_enemyAnimationClips[enEnemyAnimationClip_Walk].SetLoopFlag(true);
	p_enemyAnimationClips[enEnemyAnimationClip_Die].Load(ENEMY_DIE_ANIMATION_FILE_PATH);
	p_enemyAnimationClips[enEnemyAnimationClip_Die].SetLoopFlag(false);
	p_enemyAnimationClips[enEnemyAnimationClip_Attack].Load(ENEMY_ATTACK_ANIMATION_FILE_PATH);
	p_enemyAnimationClips[enEnemyAnimationClip_Attack].SetLoopFlag(false);
	p_enemyModelRender.Init(ENEMY_MODEL_FILE_PATH, p_enemyAnimationClips ,enEnemyAnimationClip_Num, enModelUpAxisZ);
	p_enemyModelRender.SetPosition(m_enemyPos);
	p_enemyModelRender.SetRotation(p_enemyRotation);

	p_enemyCharacterController.Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_enemyPos);

	//ゴーストオブジェクトの作成
	p_enemyBodyGhostObject.CreateBox(m_enemyPos + ENEMY_BODY_GHOST_OBJECT_POS, Quaternion::Identity, ENEMY_BODY_GHOST_OBJECT_SCALE);
	p_enemyHeadGhostObject.CreateBox(m_enemyPos + ENEMY_HEAD_GHOST_OBJECT_POS, Quaternion::Identity, ENEMY_HEAD_GHOST_OBJECT_SCALE);
	p_enemyAttackGhostObject.CreateBox(m_enemyPos + ENEMY_ATTACK_GHOST_OBJECT_POS, Quaternion::Identity, ENEMY_ATTACK_GHOST_OBJECT_SCALE);
	p_player = FindGO<Player>("player");

	p_game = FindGO<Game>("game");

	return true;
}

void Enemy::Update()
{
	//ポーズ中なら何もしない。
	if (p_game->m_isPause)
	{
		return;
	}
	EnemyMove();
	EnemyRotation();
	EnemyManageState();
	PlayEnemyAnimation();
	StepOnEnemy();
	Attack();
	Respawn();
	p_enemyModelRender.SetScale(p_enemyScale);
	//ゴーストオブジェクトを常に敵の位置の合わせる
	p_enemyBodyGhostObject.SetPosition(m_enemyPos + ENEMY_BODY_GHOST_OBJECT_POS);
	p_enemyHeadGhostObject.SetPosition(m_enemyPos + ENEMY_HEAD_GHOST_OBJECT_POS);

	
	p_enemyModelRender.Update();
}

void Enemy::EnemyMove()
{
	if (!p_isDead)
	{
		//プレイヤーがいないのなら何もしない。
		if (p_player == nullptr)
		{
			return;
		}

		
		//プレイヤーとの距離を測る。
		Vector3 diff = p_player->m_playerPosition - m_enemyPos;

		if (diff.Length() < ENEMY_DISTANCE_CLOSE)
		{
			//移動速度を初期化
			p_enemyMoveSpeed.x = 0.0f;
			p_enemyMoveSpeed.z = 0.0f;
			//フラグを動いてない状態にする
			p_isMove = false;
			return;
		}

		else if (diff.Length() <= DISTANCE)
		{
			//移動方向を正規化(距離を1にする)
			diff.Normalize();

			//敵の位置を更新
			p_enemyMoveSpeed.x = diff.x * ENEMY_SPEED;
			p_enemyMoveSpeed.z = diff.z * ENEMY_SPEED;

			//フラグを動く状態にする
			p_isMove = true;
			//フラグを待機じゃない状態にする
			p_isIdle = false;
		}

		else if (p_enemyCharacterController.IsOnGround() == false)
		{
			p_enemyMoveSpeed.y -= GRAVITATIONAL_ACCELERATION * GRAVITATIONAL_ACCELERATION * AddGameFrame(p_gravityFrameTime);
		}

		else
		{
			//移動速度を初期化
			p_enemyMoveSpeed.x = 0.0f;
			p_enemyMoveSpeed.z = 0.0f;
			//フラグを動いてない状態にする
			p_isMove = false;
			//フラグを待機状態にする
			p_isIdle = true;
		}

		//キャラクターコントローラーに位置を教える
		m_enemyPos = p_enemyCharacterController.Execute(p_enemyMoveSpeed, g_gameTime->GetFrameDeltaTime());

	}
		//絵描きさんに位置を教える
		p_enemyModelRender.SetPosition(m_enemyPos);
}

void Enemy::EnemyRotation()
{
	if (p_isMove)
	{
		//キャラクターの方向を変える。
		p_enemyRotation.SetRotationYFromDirectionXZ(p_enemyMoveSpeed);
		//絵描きさんに回転を教える。
		p_enemyModelRender.SetRotation(p_enemyRotation);
		return;
	}

	if (p_isIdle)
	{
		p_enemyRotation.AddRotationDegY(0.5f);
		p_enemyModelRender.SetRotation(p_enemyRotation);
	}
}

void Enemy::EnemyManageState()
{
	if (p_isDead)
	{
		p_enemyState = enEnemyState_Die;
		return;
	}
	else if (p_isMove)
	{
		p_enemyState = enEnemyState_Walk;
		return;
	}

	else if (p_isAttack)
	{
		p_enemyState = enEnemyState_Attack;
	}
	else
	{
		p_enemyState = enEnemyState_Idle;
	}

}

void Enemy::PlayEnemyAnimation()
{

	//switch文。
	switch (p_enemyState) {
		//プレイヤーステートが(待機)だったら。
	case enEnemyState_Idle:
		//待機アニメーションを再生する。
		p_enemyModelRender.PlayAnimation(enEnemyAnimationClip_Idle);
		break;
		//エネミーステートが(歩き)だったら
	case enEnemyState_Walk:
		//歩きアニメーションを再生する
		p_enemyModelRender.PlayAnimation(enEnemyAnimationClip_Walk);
		break;
		//エネミーステートが(死亡)だったら
	case enEnemyState_Die:
		//死亡アニメーションを再生する
		p_enemyModelRender.PlayAnimation(enEnemyAnimationClip_Die);
		break;
		//エネミーステートが(攻撃)だったら
	case enEnemyState_Attack:
		//攻撃アニメーションを再生する
		p_enemyModelRender.PlayAnimation(enEnemyAnimationClip_Attack);
		if (p_enemyModelRender.IsPlayingAnimation())
		{
			if (p_attackAnimationTime > 0.7f)
			{
				p_isPlayAttackAnimation = true;
			}
		}
		break;
	}
}

void Enemy::StepOnEnemy()
{
	if (p_isDead)
	{
		p_deadAnimationTime += g_gameTime->GetFrameDeltaTime();
	}

	if (p_deadAnimationTime > ENEMY_ANIMATION_LIMIT)
	{
	
		p_deadAnimationTime = 0.0f;
		p_isDead = false;
		//クリアカウントを増やす
		p_player->m_clearCount++;
		//敵を削除する
		DeleteGO(p_dieSE);
		DeleteGO(this);
		return;
	}

	PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
		{
			//敵の頭のゴーストオブジェクトと接触したら
			if (p_enemyHeadGhostObject.IsSelf(contactObject))
			{
				p_isDead = true;
				
				p_player->m_playerMoveSpeed.y = 1900.0f;//プレイヤーを跳ねさせる

				SoundManager* sound = FindGO<SoundManager>("soundmanager");
				p_dieSE = sound->PlayingSound(enSound_EnemyDieSE, false, 1.5f);
			}
		});
}

void Enemy::Attack()
{
	Vector3 forward = Vector3::AxisZ;
	p_enemyRotation.Apply(forward);
	forward.Normalize();

	p_enemyAttackGhostObject.SetPosition(m_enemyPos + ENEMY_ATTACK_GHOST_OBJECT_POS + forward * 100.0f);
	p_enemyAttackGhostObject.SetRotation(p_enemyRotation);

	if (p_isAttack)
	{
		p_attackAnimationTime += g_gameTime->GetFrameDeltaTime();
	}

	if (p_attackAnimationTime > ENEMY_ANIMATION_LIMIT)
	{
		p_attackAnimationTime = 0.0f;
		p_isPlayAttackAnimation = false;
		p_isAttack = false;
		p_hasAlreadyDamaged = false;
		return;
	}

	PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
		{
			if (p_enemyBodyGhostObject.IsSelf(contactObject))
			{
				if (!p_isAttack)
				{
					p_isAttack = true;
					p_attackAnimationTime = 0.0f;
					p_hasAlreadyDamaged = false;//まだダメージを与えていない
				}
			}

			//まだダメージを与えてない時だけココが働く
			if (p_enemyAttackGhostObject.IsSelf(contactObject)
				&& p_isPlayAttackAnimation
				&& !p_hasAlreadyDamaged)
			{
				SoundManager* sound = FindGO<SoundManager>("soundmanager");
				p_playerHitDamageSE = sound->PlayingSound(enSound_PlayerHitDamageSE, false, 1.5f);
				p_player->m_playerHP--;
				p_hasAlreadyDamaged = true;//ダメージを与えた
			}
		});
}

void Enemy::Respawn()
{
	if (m_enemyPos.y <= REAPAWN_LIMIT)
	{
		m_enemyPos = m_enemyInitalPos;//初期位置に戻す
		p_enemyMoveSpeed = Vector3::Zero;//移動速度もリセット
		p_enemyCharacterController.SetPosition(m_enemyPos);//物理エンジンにも更新させる
		p_enemyModelRender.SetPosition(m_enemyPos);//モデルにも位置を更新させる
	}
}

void Enemy::Render(RenderContext& rc)
{
	p_enemyModelRender.Draw(rc);
}
