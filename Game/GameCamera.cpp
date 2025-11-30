#include "stdafx.h"
#include "GameCamera.h"
#include "Player.h"
#include "Game.h"
namespace
{
	const Vector3 CAMERA_OFFSET_POS(0.0f, 100.0f, -600.0f);

	constexpr float CAMERA_NERA = 1.0f;
	constexpr float CAMERA_FAR = 100000.0f;
}

GameCamera::GameCamera()
{
}


GameCamera::~GameCamera()
{
}
bool GameCamera::Start()
{
	//注視点から視点までのベクトルを設定。
	m_toCameraPos.Set(CAMERA_OFFSET_POS);
	//プレイヤーのインスタンスを探す。
	m_player = FindGO<Player>("player");
	p_game = FindGO<Game>("game");

	//カメラのニアクリップとファークリップを設定する。
	g_camera3D->SetNear(CAMERA_NERA);
	g_camera3D->SetFar(CAMERA_FAR);

	//カメラを180°回転させる
	m_toCameraPos.z = -m_toCameraPos.z;
	return true;
}
void GameCamera::Update()
{
	CameraPosManager();
	
}

void GameCamera::CameraPosManager()
{
	if (!p_game->m_isPause)
	{
		//カメラを更新。
		//注視点を計算する。
		Vector3 target = m_player->m_playerPosition;
		//プレイヤの足元からちょっと上を注視点とする。
		target.y += 80.0f;

		Vector3 toCameraPosOld = m_toCameraPos;
		//パッドの入力を使ってカメラを回す。
		float x = g_pad[0]->GetRStickXF();
		float y = g_pad[0]->GetRStickYF();
		//Y軸周りの回転
		Quaternion qRot;
		qRot.SetRotationDeg(Vector3::AxisY, 2.0f * x);
		qRot.Apply(m_toCameraPos);
		//X軸周りの回転。
		Vector3 axisX;
		axisX.Cross(Vector3::AxisY, m_toCameraPos);
		axisX.Normalize();
		qRot.SetRotationDeg(axisX, 2.0f * y);
		qRot.Apply(m_toCameraPos);
		//カメラの回転の上限をチェックする。
		//注視点から視点までのベクトルを正規化する。
		//正規化すると、ベクトルの大きさが１になる。
		//大きさが１になるということは、ベクトルから強さがなくなり、方向のみの情報となるということ。
		Vector3 toPosDir = m_toCameraPos;
		toPosDir.Normalize();
		if (toPosDir.y < -0.1f) {
			//カメラが上向きすぎ。
			m_toCameraPos = toCameraPosOld;
		}
		else if (toPosDir.y > 0.9f) {
			//カメラが下向きすぎ。
			m_toCameraPos = toCameraPosOld;
		}

		//視点を計算する。
		Vector3 pos = target + m_toCameraPos;
		//メインカメラに注視点と視点を設定する。
		g_camera3D->SetTarget(target);
		g_camera3D->SetPosition(pos);

		//カメラの更新。
		g_camera3D->Update();
	}
}
