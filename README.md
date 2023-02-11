# Dungeon Ruler
### 作成者
河原電子ビジネス専門学校<br>
1年 橘 一生
# 目次
- [Dungeon Ruler](#dungeon-ruler)
    - [作成者](#作成者)
- [目次](#目次)
  - [__1. ゲーム概要__](#1-ゲーム概要)
  - [__2. 操作説明__](#2-操作説明)
  - [__3. ソースコード__](#3-ソースコード)
  - [__4. ステージ__](#4-ステージ)
  - [__5.ギミック紹介__](#5ギミック紹介)
    - [1. 動く床](#1-動く床)
    - [2. 氷の床](#2-氷の床)
    - [3. 炎の柱](#3-炎の柱)
  - [__6.技術的要素__](#6技術的要素)
    - [1. 静的物理オブジェクトの回転](#1-静的物理オブジェクトの回転)
    - [2.プレイヤーの滑る処理](#2プレイヤーの滑る処理)
    - [3.グレースケール](#3グレースケール)
    - [4.スロー演出](#4スロー演出)

## __1. ゲーム概要__
▼第11回全国専門学校ゲームコンペティション応募動画<br>
[![](https://img.youtube.com/vi/IK2ury61ALo/0.jpg)](https://www.youtube.com/watch?v=IK2ury61ALo)

▼プレイ動画<br>
[![プレイ動画](https://img.youtube.com/vi/qhhHg3uN57c/0.jpg)](https://www.youtube.com/watch?v=qhhHg3uN57c)
***
* __ゲームジャンル__<br>
  3Dアクションゲーム
* __ゲーム内容__<br>
  &emsp;騎士を操作してダンジョン内の敵を倒し、ギミックをクリアして、ダンジョンの奥にいる支配者を倒す。
***
* __使用ゲームエンジン__<br>
  学校内製のゲームエンジンを使用
* __使用ツール__
  * Visual Studio 2022
  * 3dsMax 2023
  * Adobe Photoshop 2023
  * Effekseer
* __使用言語__
  * C++ 
* __開発環境__
  * Windows11
  * DirectX12
* __開発期間__<br>
  2022年9月～2023年1月
* __制作人数__<br>
  1人

⇧[目次に戻る](#目次)

## __2. 操作説明__
|操作|ボタン|
|:---:|:---:|
|ジャンプ|A|
|近接攻撃|B|
|魔法攻撃|X|
|移動|Lスティック|
|ダッシュ|Lスティック+RB|
|カメラ操作|Rスティック|

⇧[目次に戻る](#目次)

## __3. ソースコード__
<details>
  <summary>作成したソースコード</summary>

  * Background.cpp
    * Background.h
  * Boss.cpp
    * Boss.h
  * CheckPoint.cpp
    * CheckPoint.h
  * Fade.cpp
    * Fade.h
  * FireGimmic.cpp
    * FireGimmic.h
  * Game.cpp
    * Game.h
  * GameCamera.cpp
    * GameCamera.h
  * MagicBall.cpp
    * MagicBall.h
  * MagicThunder.cpp
    * MagicThunder.h
  * Map.cpp
    * Map.h
  * MovingFloor.cpp
    * MovingFloor.h
  * MovingFloorZ.cpp
    * MovingFloorZ.h
  * MushRoomMan.cpp
    * MushRoomMan.h
  * Player.cpp
    * Player.h
  * SeesawFloor.cpp
    * SeesawFloor.h
  * StoneMonster.cpp
    * StoneMonster.h
  * Title.cpp
    * Title.h
</details>
<details>
<summary>改造したソースコード</summary>

  * PhysicsStaticObject.h
    * SetRotation関数を追加
  * RenderingEngine.h
    * m_grayscale変数を追加
  * DeferredLighting.fx
    * 406行目～412行目のグレースケールの処理を追加
  * PBRLighting_struct.h
    * 59行目のgrayscale変数を追加

</details>

⇧[目次に戻る](#目次)

## __4. ステージ__
  ステージは以下の3つで構成されています。
 ![ステージ構成](PortfolioImage/stage.png "ステージ構成")
 ① 雑魚敵との戦闘<br>
 ② ステージギミックのあるアスレチックコース<br>
 ③ ボス戦

 ## __5.ギミック紹介__

  ### 1. 動く床<br>
   * 床が前後や左右に移動します<br>
   * 床の上に乗るとプレイヤーも一緒に移動します<br>
   * 床の移動速度をプレイヤーの移動速度に加算して実装しました<br>

▼動画<br>
[![ギミック動く床](https://img.youtube.com/vi/U87YqJSWFL8/0.jpg)](https://www.youtube.com/watch?v=U87YqJSWFL8)

---
  ### 2. 氷の床<br>
  *  傾く床の上に乗ると傾きによってプレイヤーが滑ります

▼動画<br>
  [![ギミック氷の床](https://img.youtube.com/vi/FPwZ4YPGl8Y/0.jpg)](https://www.youtube.com/watch?v=FPwZ4YPGl8Y)

---
  ### 3. 炎の柱
  * 一定間隔で炎が吹き出します
  * プレイヤーが当たるとHPが減少していきます<br>

▼動画<br>
[![ギミック炎の柱](https://img.youtube.com/vi/pE7BFnn8gUU/0.jpg)](https://www.youtube.com/watch?v=pE7BFnn8gUU)

⇧[目次に戻る](#目次)

## __6.技術的要素__
  ### 1. 静的物理オブジェクトの回転
  &emsp;ステージとプレイヤーの当たり判定は静的物理オブジェクトとキャラクターコントローラーで取っています。<br>
  &emsp;そのため、モデルだけ回転させると、意図した挙動にならないため、静的物理オブジェクトも回転させる必要があります。<br>
  &emsp;しかし、既存のエンジンには静的物理オブジェクトを回転させるための処理がなかったため、PhysicsStaticObject.hのSetPosition関数を参考にSetRotation関数を作成しました。<br>
  ```C++
  void SetRotation(const Quaternion& rot)
		{
		auto& btTrans = m_rigidBody.GetBody()->getWorldTransform();
		btQuaternion btRot;
		btRot = btQuaternion(rot.x,rot.y,rot.z,rot.w);
		btTrans.setRotation(btRot);
		}
  ```
  ***
  ### 2.プレイヤーの滑る処理
  &emsp;床の回転の値(seesawFloor->GetRotation())を取得して、床の回転の値にスカラーを乗算し、プレイヤーの移動速度(m_oldMoveSpeed)に加算することで実装しました。
  ```
  float rot = seesawFloor->GetRotation() * 500.0f;
m_oldMoveSpeed.z += rot;
  ```
  ***
  ### 3.グレースケール
  &emsp;プレイヤーが死亡したときに、3Dモデルがグレースケールになるようにしました。<br>
  &emsp;徐々にグレースケールになるようにするために、線形補間を使って実装しました。<br>
```HLSL
float Y = finalColor.r * 0.29891f + finalColor.g * 0.58661f + finalColor.b * 0.11448f;
float fullcolor = 1.0f - light.grayscale;
    
finalColor.r = (finalColor.r * fullcolor) + (Y * light.grayscale);
finalColor.g = (finalColor.g * fullcolor) + (Y * light.grayscale);
finalColor.b = (finalColor.b * fullcolor) + (Y * light.grayscale);
```
▼動画<br>
[![グレースケール](https://img.youtube.com/vi/3g6UbGYMKmM/0.jpg)](https://www.youtube.com/watch?v=3g6UbGYMKmM)

  >参考にしたサイト<br>
  [DirectX11プログラミング-グレースケールシェーダー-](https://yun.cup.com/directx11032.html)
***
  ### 4.スロー演出
  &emsp;ボスを倒したときにスローの演出をいれました。
  これにより強敵を倒したということを感じやすくできたと思います。<br>
  FPSを一時的に落とすことで実装しました。

▼動画<br>
[![スローモーション](https://img.youtube.com/vi/GJv1pbQCk0Y/0.jpg)](https://www.youtube.com/watch?v=GJv1pbQCk0Y)

⇧[目次に戻る](#目次)


