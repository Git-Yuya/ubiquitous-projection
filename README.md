# ユビキタスプロジェクション

https://github.com/Git-Yuya/ubiquitous-projection/assets/84259422/efe82fbe-1060-47fe-afa7-0d6cb20c8e63

## 機能
![key_event](https://github.com/Git-Yuya/ubiquitous-projection/assets/84259422/7d044df2-5c52-4a58-a26d-f07c40a15e83)

### 線の描写
マウスの右クリックを押している状態で、マウスを動かすと線を描写する。

### 線の色変更
パソコンの特定のキーを押すことで、線の色を変更する。描写画面は黒色なので、線の色を黒色にした場合、消しゴムの役割を果たす。
- eキー：黒 (eraser)
- wキー：白 (white)
- gキー：緑 (green)
- bキー：青 (blue)

### 線の太さ変更
特定のキーを押すことで、線の太さを変更。
- pキー：太く (plus)
- mキー：細く (minus)

### 描写画面のリセット (reset)
rキーを押すと、描写画面をリセットする。

### 描写画面の保存 (save)
sキーを押すと、描写画面を"output.png"として保存する。

### 通信終了 (quit)
qキーを押すと、Socketを破棄しプログラムを正常に終了する。

## 実装
- 言語：
  <img src="https://img.shields.io/badge/-C++-00599C.svg?logo=cplusplus&style=plastic">
- 画像処理ライブラリ：
  <img src="https://img.shields.io/badge/-OpenCV-5C3EE8.svg?logo=opencv&style=plastic">
- 統合開発環境：
  <img src="https://img.shields.io/badge/-Visual%20Studio-5C2D91.svg?logo=visualstudio&style=plastic">

## 各ファイルの説明
- Rx.cpp：受信側
- Tx.cpp：送信側
