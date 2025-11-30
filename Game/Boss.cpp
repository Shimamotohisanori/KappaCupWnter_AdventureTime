#include "stdafx.h"
#include "Boss.h"
#include "Player.h"
#include <thread>
#include "Game.h"
#include "SoundManager.h"
namespace
{
	const char* BOSS_MODEL_FILE_PATH = "Assets/modelData/boss.tkm";
	const char* BOSS_IDLE_ANIMATION_FILE_PATH = "Assets/animData/bossIdle.tka";
	const char* BOSS_WALK_ANIMATION_FILE_PATH = "Assets/animData/bossWalk.tka";
	const char* BOSS_HIT_ANIMATION_FILE_PATH = "Assets/animData/bossHit.tka";
	const char* BOSS_DIE_ANIMATION_FILE_PATH = "Assets/animData/bossDie.tka";
	const char* BOSS_ATTACK_ANIMATION_FILE_PATH = "Assets/animData/bossAttack.tka";
	
	const Vector3 BOSS_HEAD_GHOST_OBJECT_POS = { 0.0f,500.0f,0.0f };//敵の頭のゴーストオブジェクトの位置
	const Vector3 BOSS_HEAD_GHOST_OBJECT_SCALE = { 160.0f,50.0f,160.0f };//敵の頭のゴーストオブジェクトの大きさ
	const Vector3 BOSS_BODY_GHOST_OBJECT_POS = { 0.0f,300.0f,0.0f };//敵の体のゴーストオブジェクトの位置
	const Vector3 BOSS_BODY_GHOST_OBJECT_SCALE = { 400.0f,450.0f,400.0f };//敵の体のゴーストオブジェクトの大きさ
	const Vector3 BOSS_ATTACK_GHOST_OBJECT_POS = { 0.0f,320.0f,0.0f };//敵の攻撃専用のゴーストオブジェクトの位置
	const Vector3 BOSS_ATTACK_GHOST_OBJECT_SCALE = { 200.0f, 400.0f, 390.0f };//敵の攻撃専用のゴーストオブジェクトの大きさ

	const float CHARACON_RADIUS = 140.0f;//敵のキャラコン(半径)
	const float CHARACON_HEIGHT = 320.0f;//敵のキャラコン(高さ)
	const float BOSS_DISTANCE_CLOSE = 230.0f;//敵がプレイヤーに近づきすぎた時の距離
	const float DISTANCE = 2500.0f;//敵がプレイヤーを追いかけ始める距離
	const float GRAVITATIONAL_ACCELERATION = 9.8f;//重力
	const float BOSS_SPEED = 400.0f;//敵の移動速度
	const float REAPAWN_LIMIT = -1000.0f;//リスポーンする時の地点
	const float BOSS_ANIMATION_LIMIT = 2.5f;//敵のアニメーションが流れる時間の制限
	const float BOSS_CHANGE_ANIMATION_TIME_LIMIT = 1.5f;//ボスのアニメーションが切り替わる時間の制限

	const int BOSS_DEAD_COUNT = 5;//敵が死ぬまでの数
	const int BOSS_FIRST_HIT_COUNT = 1;//敵が攻撃を受けた時(1回目)
	const int BOSS_SECOND_HIT_COUNT = 2;//敵が攻撃を受けた時(2回目)
}


Boss::Boss()
{
}


Boss::~Boss()
{
	DeleteGO(p_bossHitAndDieSE);
}


bool Boss::Start()
{
	p_bossAnimationClips[enBossAnimationClip_Idle].Load(BOSS_IDLE_ANIMATION_FILE_PATH);
	p_bossAnimationClips[enBossAnimationClip_Idle].SetLoopFlag(true);
	p_bossAnimationClips[enBossAnimationClip_Walk].Load(BOSS_WALK_ANIMATION_FILE_PATH);
	p_bossAnimationClips[enBossAnimationClip_Walk].SetLoopFlag(true);
	p_bossAnimationClips[enBossAnimationClip_Hit].Load(BOSS_HIT_ANIMATION_FILE_PATH);
	p_bossAnimationClips[enBossAnimationClip_Hit].SetLoopFlag(false);
	p_bossAnimationClips[enBossAnimationClip_Die].Load(BOSS_DIE_ANIMATION_FILE_PATH);
	p_bossAnimationClips[enBossAnimationClip_Die].SetLoopFlag(false);
	p_bossAnimationClips[enBossAnimationClip_Attack].Load(BOSS_ATTACK_ANIMATION_FILE_PATH);
	p_bossAnimationClips[enBossAnimationClip_Attack].SetLoopFlag(false);

	p_bossModelRender.Init(BOSS_MODEL_FILE_PATH, p_bossAnimationClips, enBossAnimationClip_Num, enModelUpAxisZ);
	
	p_bossCharacterController.Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_bossPos);

	p_bossModelRender.SetPosition(m_bossPos);
	p_bossModelRender.SetRotation(p_bossRotation);

	//ゴーストオブジェクトの作成
	p_bossBodyGhostObject.CreateBox(m_bossPos + BOSS_BODY_GHOST_OBJECT_POS, Quaternion::Identity, BOSS_BODY_GHOST_OBJECT_SCALE);
	p_bossHeadGhostObject.CreateBox(m_bossPos + BOSS_HEAD_GHOST_OBJECT_POS, Quaternion::Identity, BOSS_HEAD_GHOST_OBJECT_SCALE);
	p_bossAttackGhostObject.CreateBox(m_bossPos + BOSS_ATTACK_GHOST_OBJECT_POS, Quaternion::Identity, BOSS_ATTACK_GHOST_OBJECT_SCALE);

	p_player = FindGO<Player>("player");
	
	p_game = FindGO<Game>("game");

	p_bossModelRender.AddAnimationEvent([&](const wchar_t* clipName , const wchar_t* eventName) {
		OnAnimationEvent(clipName, eventName);
		});
	return true;
}


void Boss::Update()
{
	//ポーズ中なら何もしない。
	if (p_game->m_isPause)
	{
		return;
	}

	BossManageState();
	BossMove();
	BossRotation();
	Respawn();
	StepOnBoss();
	ChangeBossAnimation();
	Attack();
	p_bossModelRender.SetScale(p_bossScale);
	//ゴーストオブジェクトを常に敵の位置の合わせる
	p_bossBodyGhostObject.SetPosition(m_bossPos + BOSS_BODY_GHOST_OBJECT_POS);
	p_bossHeadGhostObject.SetPosition(m_bossPos + BOSS_HEAD_GHOST_OBJECT_POS);
	p_bossModelRender.Update();
}


void Boss::BossMove()
{
	if (!p_isDead)
	{
		//プレイヤーがいないのなら何もしない。
		if (p_player == nullptr)
		{
			return;
		}

		//プレイヤーとの距離を測る。
		Vector3 diff = p_player->m_playerPosition - m_bossPos;

		if (diff.Length() < BOSS_DISTANCE_CLOSE)
		{
			//移動速度を初期化
			p_bossMoveSpeed.x = 0.0f;
			p_bossMoveSpeed.z = 0.0f;
			//フラグを動いてない状態にする
			p_isMove = false;
			return;
		}

		else if (diff.Length() <= DISTANCE)
		{
			//移動方向を正規化(距離を1にする)
			diff.Normalize();

			//敵の位置を更新
			p_bossMoveSpeed.x = diff.x * BOSS_SPEED;
			p_bossMoveSpeed.z = diff.z * BOSS_SPEED;

			//フラグを動く状態にする
			p_isMove = true;
			//フラグを待機じゃない状態にする
			p_isIdle = false;
		}

		else if (p_bossCharacterController.IsOnGround() == false)
		{
			p_bossMoveSpeed.y -= GRAVITATIONAL_ACCELERATION * GRAVITATIONAL_ACCELERATION * AddGameFrame(p_gravityFrameTime);
		}

		else
		{
			//移動速度を初期化
			p_bossMoveSpeed.x = 0.0f;
			p_bossMoveSpeed.z = 0.0f;
			//フラグを動いてない状態にする
			p_isMove = false;
			//フラグを待機状態にする
			p_isIdle = true;
		}

		//キャラクターコントローラーに位置を教える
		m_bossPos = p_bossCharacterController.Execute(p_bossMoveSpeed, g_gameTime->GetFrameDeltaTime());
	}
	//絵描きさんに位置を教える
	p_bossModelRender.SetPosition(m_bossPos);
}


void Boss::BossRotation()
{
	//xかzの移動速度があったら(スティックの入力があったら)。
	if (p_isMove)
	{
		//キャラクターの方向を変える。
		p_bossRotation.SetRotationYFromDirectionXZ(p_bossMoveSpeed);
		//絵描きさんに回転を教える。
		p_bossModelRender.SetRotation(p_bossRotation);
		return;
	}

	if (p_isIdle)
	{
		p_bossRotation.AddRotationDegY(0.5f);
		p_bossModelRender.SetRotation(p_bossRotation);
	}
}


void Boss::BossManageState()
{
	p_bossState = enBossState_Idle;

	
	if (p_isMove)
	{
		p_bossState = enBossState_Walk;
	}
	
	if (p_isAttack)
	{
		p_bossState = enBossState_Attack;
		if (p_bossModelRender.IsPlayingAnimation())
		{
			if (p_attackAnimationTime > 1.0f)
			{
				p_isPlayAttackAnimation = true;
			}
		}
	}

	if (p_isHitAnimation)
	{
		p_bossState = enBossState_Hit;
		if (p_bossModelRender.IsPlayingAnimation())
		{
			if (p_changeAnimationTime > 1.0f)
			{
				p_isHitAnimation = false;
			}
		}
		else
		{
			p_bossState = enBossState_Idle;
			p_isHitAnimation = false;
		}
	}

	if (p_isDead)
	{
		p_bossState = enBossState_Die;
	}

	p_bossModelRender.PlayAnimation(p_bossState);
}


void Boss::Respawn()
{
	if (m_bossPos.y <= REAPAWN_LIMIT)
	{
		m_bossPos = m_bossInitalPos;//初期位置に戻す
		p_bossMoveSpeed = Vector3::Zero;//移動速度もリセット
		p_bossCharacterController.SetPosition(m_bossPos);//物理エンジンにも更新させる
		p_bossModelRender.SetPosition(m_bossPos);//モデルにも位置を更新させる
	}
}


void Boss::StepOnBoss()
{
	if (p_player->m_isPlayerDead == false)
	{

		if (p_isHitAnimation)
		{
			p_bossHeadGhostObject.SetPosition({m_bossPos.x, -100.0f, m_bossPos.z});
		}

		if (p_isDead)
		{
			p_changeAnimationTime += g_gameTime->GetFrameDeltaTime();
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
			DeleteGO(this);
			return;
		}

		bool isTouching = false;

		PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
			{
				//敵の頭のゴーストオブジェクトと接触したら
				if (p_bossHeadGhostObject.IsSelf(contactObject))
				{
					isTouching = true;

					p_player->m_playerMoveSpeed.y = 1900.0f;//プレイヤーを跳ねさせる
				}
			});

		if (p_damageCount == BOSS_DEAD_COUNT)
		{
			p_isDead = true;
		}

		//前フレームが触れていない、次フレームが触れてると
		if (isTouching && !p_isDamage)
		{
			SoundManager* sound = FindGO<SoundManager>("soundmanager");
			p_bossHitAndDieSE = sound->PlayingSound(enSound_BossHitAndDieSE,false);
			p_isDamage = true;
			p_damageCount++;
			p_isHitAnimation = true;
		}

		//次フレームが触れていない、前フレームが触れてると
		if (!isTouching && p_isDamage)
		{
			p_isDamage = false;
		}

		p_isDamage = isTouching;
	}
}


void Boss::Attack()
{
	if (p_player->m_isPlayerDead == false)
	{
		Vector3 forward = Vector3::AxisZ;
		p_bossRotation.Apply(forward);
		forward.Normalize();

		p_bossAttackGhostObject.SetPosition(m_bossPos + BOSS_ATTACK_GHOST_OBJECT_POS + forward * 130.0f);
		p_bossAttackGhostObject.SetRotation(p_bossRotation);

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
			DeleteGO(p_bossShakeUpSE);
			return;
		}

		PhysicsWorld::GetInstance()->ContactTest(p_player->m_playerCharacterController, [&](const btCollisionObject& contactObject)
			{
				if (p_bossBodyGhostObject.IsSelf(contactObject))
				{
					if (!p_isAttack)
					{
						p_isAttack = true;
					}
				}

				if (p_bossAttackGhostObject.IsSelf(contactObject) 
					&& p_isPlayAttackAnimation
					&& !p_hasAlreadyDamaged)
				{
					SoundManager* sound = FindGO<SoundManager>("soundmanager");
					p_playerHitDamageSE = sound->PlayingSound(enSound_PlayerHitDamageSE, false , 1.5f);
					p_player->m_playerHP--;
					p_hasAlreadyDamaged = true;//ダメージを与えた
				}
			});
	}
}


void Boss::ChangeBossAnimation()
{
	if (p_bossState == enBossState_Hit)
	{
		float deltatime = g_gameTime->GetFrameDeltaTime();
		p_changeAnimationTime += deltatime;
		
		if (p_changeAnimationTime > BOSS_CHANGE_ANIMATION_TIME_LIMIT)
		{
			p_bossState = enBossState_Idle;
			p_changeAnimationTime = 0.0f;
		}
	}
}


void Boss::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//キーの名前が「attack_start」の時。
	if (wcscmp(eventName, L"attack_start") == 0)
	{
		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		p_bossShakeUpSE = sound->PlayingSound(enSound_BossShakeUpSE, false);
	}

	else if (wcscmp(eventName, L"attack_end") == 0)
	{
		DeleteGO(p_bossShakeUpSE);
	}
}


void Boss::Render(RenderContext& rc)
{
	p_bossModelRender.Draw(rc);
}


