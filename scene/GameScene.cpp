﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}


GameScene::~GameScene() 
{ 
	delete model_; 
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");


	//3Dモデルの生成
	model_ = Model::Create();

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.scale_ = {5.0f, 5.0f, 5.0f};

	// X,Y,Z軸周りの回転角を設定
	//XM_PI/4.0fはラジアンの計算 XMConvertToRadians(45.0f)で度数法で書ける
	worldTransform_.rotation_ = {XM_PI / 4.0f, XM_PI / 4.0f, 0.0f};

	// X,Y,Z軸周りの平行移動を設定
	worldTransform_.translation_ = {10.0f, 10.0f, 10.0f};

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//ビュープロジェクションの初期化

	viewProjection_.Initialize();


}

void GameScene::Update() 
{ 
	debugText_->SetPos(50, 50);

	debugText_->Printf("translation:(%lf,%f,%f)", worldTransform_.translation_.x, worldTransform_.translation_.y,worldTransform_.translation_.z); 

	debugText_->SetPos(50, 65);

	debugText_->Printf("rotation:(%lf,%f,%f)", worldTransform_.rotation_.x, worldTransform_.rotation_.y,worldTransform_.rotation_.z); 

	debugText_->SetPos(50, 80);

	debugText_->Printf("scale:(%lf,%f,%f)", worldTransform_.scale_.x, worldTransform_.scale_.y,worldTransform_.scale_.z);

	viewProjecion_.Initialize();

	//サウンドデータの読み込み
	soundDataHandle_ = audio_->LoadWave("se_sad03.wav");

	//音声再生
	audio_->PlayWave(soundDataHandle_);

	//音声再生
	voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);
}


}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>


	//3Dモデル描画
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	sprite_->Draw();



	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
