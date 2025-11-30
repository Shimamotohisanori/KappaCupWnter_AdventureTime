#pragma once
#include "sound/SoundSource.h"

//ファイルパスを追加する際は、
//cppのnamespaceと下記のenumを使用してください。
enum Sound {
	enSound_GameTitleBGM,//ゲームタイトルのBGM
	enSound_GameClearBGM,//ゲームクリアのBGM
	enSound_GameOverBGM,//ゲームオーバーのBGM
	enSound_InGameBGM,//ゲーム中のBGM
	enSound_BossBGM,//ボス戦のBGM
	enSound_DecisionSE,//決定音
	enSound_JumpSE,//ジャンプ音
	enSound_PlayerHitDamageSE,//プレイヤーがダメージを食らったときの音
	enSound_EnemyDieSE,//敵(クリボー)が死んだときの音
	enSound_GimmickenemyHitAndDieSE,//敵(ノコノコ)が踏まれたもしくは死んだ時の音
	enSound_BossHitAndDieSE,//ボスが踏まれたもしくは死んだ時の音
	enSound_BossShakeUpSE,//ボスが振りかぶった時の音
	enSound_WarpSE,//ワープの音
	enSound_PauseStart,//ポーズスタート時のSE
	enSound_PauseEnd,//ポーズ終了時のSE
	enSound_PauseChooseSE,//ポーズ画面でメニューを選んでいる時の音
	enSound_Num//このステータスは、サウンドの総数を表しているため、この下には追加しないでください。
};
class SoundManager : public IGameObject
{
public:
	SoundManager();
	~SoundManager() {};
	///<summary>
	///指定したサウンドを再生し、再生中のサウンドソースを返します。
	/// </summary>
	/// <param name="number">再生するサウンドを指定します。</param>
	/// <param name="isLoop">サウンドをループ再生するかどうかをしていします。デフォルトは true です。</param>
	/// <param name="volume">再生音量を指定します。デフォルトは1.0fです。</param>
	/// <returns>再生中のサウンドソースへのポインタ。</returns>
	SoundSource* PlayingSound(Sound number, bool isLoop = true, float volume = 1.0f);
};
