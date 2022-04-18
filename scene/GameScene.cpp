#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

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

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	for (size_t i = 0; i < _countof(worldTransform_); i++) 
	{
		// X,Y,Z方向のスケーリングを設定
		worldTransform_[i].scale_ = {5.0f, 5.0f, 5.0f};

		// X,Y,Z軸周りの回転角を設定
		// XM_PI/4.0fはラジアンの計算 XMConvertToRadians(45.0f)で度数法で書ける
		worldTransform_[i].rotation_ = {0, 0, XM_PI / -2.0f};

		// X,Y,Z軸周りの平行移動を設定
		if (i < 50) {

			float a = -50+ 10*i ;

			worldTransform_[i].translation_ = {-20 ,a , 0};
		} else {

			float a = 10 * (i-50);
			worldTransform_[i].translation_ = {20, a, 0};
		}

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}

	//カメラ始点座標を設定
	viewProjection_.eye = {0, 50, -50};

	//カメラ注視点座標を設定
	viewProjection_.target = {0, 50, 0};

	//カメラ上方向ベクトルを設定(右上45度指定)
	viewProjection_.up = {cosf(XM_PI / 4.0f), sinf(XM_PI / 4.0f), 0.0f};

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();


}

void GameScene::Update() 
{ 
	#pragma region 視点移動処理


	{
		//視点の移動ベクトル
		XMFLOAT3 move = {0, 0, 0};

		//視点の移動の速さ
		const float kEyeSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		/*if (input_->PushKey(DIK_W)) {
			move = {0, 0, kEyeSpeed};
		} else if (input_->PushKey(DIK_S)) {
			move = {0, 0, -kEyeSpeed};
		}*/

		//視点移動(ベクトルの加算)
		XMFLOAT3XYZadd(viewProjection_.eye, move.x, move.y, move.z);

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		/*debugText_->SetPos(50, 50);
		debugText_->Printf( "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);*/
	}
#pragma endregion

		
#pragma region 注視点移動処理
	{
		//注視点の移動ベクトル
		XMFLOAT3 move = {0, 0, 0};

		//注視点の移動の速さ
		const float kTargetSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		/*if (input_->PushKey(DIK_LEFT)) {
			move = {-kTargetSpeed, 0, 0};
		} else if (input_->PushKey(DIK_RIGHT)) {
			move = {kTargetSpeed, 0, 0};
		}*/

		//注視点移動(ベクトルの加算)
		XMFLOAT3XYZadd(viewProjection_.target, move.x, move.y, move.z);

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		/*debugText_->SetPos(50, 70);
		debugText_->Printf("target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);*/
	}
	#pragma endregion

	#pragma region 上方向回転処理
	{
		//上方向の回転の速さ[ラジアン/frame]
		const float kUpRotSpeed = 0.05f;

		//押した方向で移動ベクトルを変更
		/*if (input_->PushKey(DIK_SPACE)) {
			viewAngle += kUpRotSpeed;
			viewAngle = fmodf(viewAngle, XM_2PI);
		}*/

		//上方向ベクトルを計算(半径1の円周上の座標)
		viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		/*debugText_->SetPos(50, 90);
		debugText_->Printf("up:%f,%f,%f", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);*/

	}
#pragma endregion



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
	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}


void GameScene::XMFLOAT3XYZadd(DirectX::XMFLOAT3 &a, float moveX, float moveY, float moveZ) { 
	a.x += moveX; 
	a.y += moveY; 
	a.z += moveZ; 
}