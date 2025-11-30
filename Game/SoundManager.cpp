#include "stdafx.h"
#include "SoundManager.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"
#include <iostream>

namespace {
	//ファイル名のみ追加すれば
	//コンストラクタにて自動でg_soundEngineに登録されます。
	//例: "Assets/sound/"+"Title"+".wav
	const char* SOUND_FILEPATH = "Assets/sound/";
	const char* SOUND_EXTENSION = ".wav";
	const char* soundFileNameList[enSound_Num] =
	{
		//ここにファイル名を追加してください。
		//追加する際は、ヘッダのenum Soundにも追加してください
		"GameTitleBGM",
		"GameClearBGM",
		"GameOverBGM",
		"InGameBGM",
		"BossBGM",
		"Decision",
		"JumpSE",
		"PlayerHitDamageSE",
		"EnemyDieSE",
		"GimmickEnemyHitAndDieSE",
		"BossEnemyHitAndDieSE",
		"BossShakeUpSE",
		"WarpSE",
		"PauseStartSE",
		"PauseEndSE",
		"PauseChoose"
	};
}

SoundManager::SoundManager()
{
	//サウンドのファイルパスの登録。
	//サウンドの個数分（enSound_Numの数）、登録されます。
	for (int i = 0; i < enSound_Num; i++)
	{
		//std::stringを使用し、文字列を結合。
		std::string file = std::string(SOUND_FILEPATH) + soundFileNameList[i] + (SOUND_EXTENSION);//ここでファイルパスを作る
		//引数がconst char*なので、std::stringからconst char*に変換。
		const char* filepath = file.c_str();
		//g_soundEngineへ登録。
		g_soundEngine->ResistWaveFileBank(i, filepath);
	}
}

SoundSource* SoundManager::PlayingSound(Sound number, bool isLoop, float volume)
{
	SoundSource* sound = NewGO<SoundSource>(0);
	//引数で受け取ったnumberはsoundFileNameListの要素番号に対応しています。
	sound->Init(number);
	sound->SetVolume(volume);
	sound->Play(isLoop);
	//戻り値にSoundSourceのインスタンスを設定。
	//呼び出し元で、
	//SoundSource* sound = sound.PlayingSound(Sound::enSound_TitleBGM);
	//と書くとインスタンスのアドレスを受け取ることができます。
	return sound;
}