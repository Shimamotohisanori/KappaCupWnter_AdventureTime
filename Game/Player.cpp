#include "stdafx.h"
#include "Player.h"
#include "Game.h"
#include "SoundManager.h"
namespace
{
	const char* PLAYER_MODEL_FILE_PATH = "Assets/modelData/unityChan.tkm";
	const char* PLAYER_IDLE_ANIMATION_FILE_PATH = "Assets/animData/idle.tka";
	const char* PLAYER_RUN_ANIMATION_FILE_PATH = "Assets/animData/run.tka";
	const char* PLAYER_JUMP_ANIMATION_FILE_PATH = "Assets/animData/jump.tka";
	const char* PRAYER_WALK_ANIMATION_FILE_PATH = "Assets/animData/walk.tka";

	const Vector3 PLAYER_HEAD_POSITION = { 0.0f, 60.0f, 0.0f };
	const Quaternion PLAYER_FIRST_ROTATION = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);

	const float CHARACTERCONTROLLER_RADIUS = 35.0f;
	const float CHARACTERCONTROLLER_HEIGTH = 70.0f;
	const float COOLTIMECOUNT_LIMIT = 0.0f;
	const float COOLTIMESECOND = 1.0f;
	const float COOLTIMEMINUTE = 60.0f;
	const float FIRE_SE_VOLUME = 1.5f;
	const float GRAVITATIONAL_ACCELERATION = 9.8f;
	const float JUMP_POWER = 1900.0f;
	const float STICK_MULTIPLICATION_VALUE = 450.0f;
	const float STICK_INPUT = 0.001f;
	const float MOVE_SECOND = 1.0f;
	const float MOVE_MINUTE = 60.0f;
	const float RESPAWN_LIMIT = -1800.0f;
}


Player::Player()
{
	
}


Player::~Player()
{
	DeleteGO(m_jumpSE);
}


bool Player::Start()
{
	/** アニメーションクリップをロードする */
	m_playerAnimationClips[enAnimationClip_Idle].Load(PLAYER_IDLE_ANIMATION_FILE_PATH);
	m_playerAnimationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_playerAnimationClips[enAnimationClip_Walk].Load(PRAYER_WALK_ANIMATION_FILE_PATH);
	m_playerAnimationClips[enAnimationClip_Walk].SetLoopFlag(true);
	m_playerAnimationClips[enAnimationClip_Run].Load(PLAYER_RUN_ANIMATION_FILE_PATH);
	m_playerAnimationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_playerAnimationClips[enAnimationClip_Jump].Load(PLAYER_JUMP_ANIMATION_FILE_PATH);
	m_playerAnimationClips[enAnimationClip_Jump].SetLoopFlag(false);
	
	
	/** ユニティちゃんのモデルを読み込む */
	m_playerModelRender.Init(PLAYER_MODEL_FILE_PATH, m_playerAnimationClips, enAnimationClip_Num, enModelUpAxisY);
	m_playerModelRender.SetScale(m_playerScale);
	m_playerRotation.SetRotationDegY(180.0f);
	m_playerModelRender.SetRotation(m_playerRotation);
	
	
	/** キャラコンを初期化する */
	m_playerCharacterController.Init(CHARACTERCONTROLLER_RADIUS, CHARACTERCONTROLLER_HEIGTH, m_playerPosition);


	p_game = FindGO<Game>("game");

	return true;
}


void Player::Update()
{
	//ポーズ中なら何もしない。
	if (p_game->m_isPause)
	{
		return;
	}
	//ステート管理
	ManageState();

	//アニメーションの再生
	PlayAnimation();

	//移動処理
	Move();

	//回転処理
	Rotation();
	
	//プレイヤーのHP管理
	PlayerHPManager();

	//更新処理
	m_playerModelRender.Update();
}


void Player::Move()
{
	//xzの移動速度をリセットする
	m_playerMoveSpeed.x = 0.0f;
	m_playerMoveSpeed.z = 0.0f;

	//左スティックの入力量を取得。
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();

	//カメラの前方向と右方向のベクトルを持ってくる。
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();
	//y方向には移動させない。
	forward.y = 0.0f;
	forward.Normalize();
	right.y = 0.0f;
	right.Normalize();

	//もしプレイヤーが下に落ちすぎたら
	if (m_playerPosition.y <= RESPAWN_LIMIT)
	{
		m_isPlayerDead = true;
	}

	//ダッシュの処理
	if (g_pad[0]->IsPress(enButtonB))
	{
		//スティックの入力量を2倍して速度を計算する
		right *= stickL.x * STICK_MULTIPLICATION_VALUE * 2.0f;
		forward *= stickL.y * STICK_MULTIPLICATION_VALUE * 2.0f;
	}
	//歩きの処理
	else
	{
		//左スティックの入力量と720.0fを乗算。
		right *= stickL.x * STICK_MULTIPLICATION_VALUE;
		forward *= stickL.y * STICK_MULTIPLICATION_VALUE;
	}

	//移動速度にスティックの入力量を加算する。
	m_playerMoveSpeed += right + forward;

	//地面に付いていたら。
	if (m_playerCharacterController.IsOnGround() && g_pad[0]->IsTrigger(enButtonA))
	{
		m_gravityFrameTime = 0.0f;//重力のフレーム時間を0.0fにする。

		SoundManager* sound = FindGO<SoundManager>("soundmanager");
		m_jumpSE = sound->PlayingSound(enSound_JumpSE, false);

		//ジャンプさせる。
		m_playerMoveSpeed.y += JUMP_POWER;
	}
	//地面に付いていなかったら。
	else
	{
		//重力を発生させる。
		m_playerMoveSpeed.y -= GRAVITATIONAL_ACCELERATION * GRAVITATIONAL_ACCELERATION + GRAVITATIONAL_ACCELERATION * AddGameFrame(m_gravityFrameTime);
	}

	m_playerPosition = m_playerCharacterController.Execute(m_playerMoveSpeed, g_gameTime->GetFrameDeltaTime());


	//絵描きさんに座標を教える。
	m_playerModelRender.SetPosition(m_playerPosition);
}


void Player::Rotation()
{
		//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(m_playerMoveSpeed.x) >= STICK_INPUT || fabsf(m_playerMoveSpeed.z) >= STICK_INPUT)
	{
		//キャラクターの方向を変える。
		m_playerRotation.SetRotationYFromDirectionXZ(m_playerMoveSpeed);
		//絵描きさんに回転を教える。
		m_playerModelRender.SetRotation(m_playerRotation);
	}
}


//ステート管理。
void Player::ManageState()
{
	//地面に付いていなかったら。
	if (m_playerCharacterController.IsOnGround() == false)
	{
		//ステートを1(ジャンプ中)にする。
		m_playerState = enPlayerState_Jump;
		//ここでManageStateの処理を終わらせる。
		return;
	}

	//Bボタンを押してダッシュ処理をしたら
	if (g_pad[0]->IsPress(enButtonB))
	{
		m_playerState = enPlayerState_Run;
		//走るアニメーションだけ再生したいので早期リターンをする。
		return;
	}

	//地面に付いていたら。
	//xかzの移動速度があったら(スティックの入力があったら)。
	if (fabsf(m_playerMoveSpeed.x) >= STICK_INPUT || fabsf(m_playerMoveSpeed.z) >= STICK_INPUT)
	{
		//ステートを(歩き)にする。
		m_playerState = enPlayerState_Walk;
	}
	
	//xとzの移動速度が無かったら(スティックの入力が無かったら)。
	else
	{
		//ステートを(待機)にする。
		m_playerState = enPlayerState_Idle;
	}
}


//アニメーションの再生。
void Player::PlayAnimation()
{
		//switch文。
	switch (m_playerState) {
		//プレイヤーステートが(待機)だったら。
	case enPlayerState_Idle:
		//待機アニメーションを再生する。
		m_playerModelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//プレイヤーステートが(歩き)だったら。
	case enPlayerState_Walk:
		//歩きアニメーションを再生する。
		m_playerModelRender.PlayAnimation(enAnimationClip_Walk);
		break;
		//プレイヤーステートが(走り)だったら。
	case enPlayerState_Run:
		//走りアニメーションを再生する。
		m_playerModelRender.PlayAnimation(enAnimationClip_Run);
		break;
		//プレイヤーステートが(ジャンプ中)だったら。
	case enPlayerState_Jump:
		//ジャンプアニメーションを再生する。
		m_playerModelRender.PlayAnimation(enAnimationClip_Jump);
		break;
	}
}


void Player::PlayerHPManager()
{
	/** もしプレイヤーのHPが0よりも下だったら */
	if (m_playerHP == 0)
	{
		/** プレイヤーが死ぬ */
		m_isPlayerDead = true;
	}
}


void Player::ResetPosition()
{
	m_playerPosition = m_playerInitialPosition;//初期位置に戻す
	m_playerMoveSpeed = Vector3::Zero;//移動速度もリセット
	m_playerCharacterController.SetPosition(m_playerPosition);//物理エンジンにも更新させる
	m_playerModelRender.SetPosition(m_playerPosition);//モデルにも位置を更新させる
}


/** 描画処理 */
void Player::Render(RenderContext& rc)
{
	m_playerModelRender.Draw(rc);
}