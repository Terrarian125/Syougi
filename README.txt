━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
将棋ゲーム 画像差し替えガイド
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

このゲームは、`data` フォルダ内に特定の名前で画像を置くことで、
駒や背景を自由に変更できます。

■ 背景画像の変更
・場所：data/background.png
・サイズ：1280 x 720 ピクセル推奨

■ 駒画像のルール
・場所：data/ フォルダ内
・形式：透過PNG形式推奨
・ファイル名：[陣営]_[種類]_[成判定].png

【陣営（先手後手）】
  Sente （自分側・下側）
  Gote  （相手側・上側）

【駒の種類】
  Pawn   (歩)    Lance  (香)    Knight (桂)    Silver (銀)
  Gold   (金)    Bishop (角)    Rook   (飛)    King   (王)

【成判定】
  False （通常の状態）
  True  （成った状態）

【具体例】
・先手の「歩」：Sente_Pawn_False.png
・先手の「と金」：Sente_Pawn_True.png
・先手の「竜（飛車が成ったもの）」：Sente_Rook_True.png
・後手の「王」：Gote_King_False.png

※重要：将棋のルール上、「王」と「金」は成ることができません。
  そのため、King_True や Gold_True という画像は使用されません。

■ 補足
・画像ファイルがない場合は、自動的に漢字でのテキスト表示になります。
・画像サイズは一辺 64ピクセル程度を想定しています。
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
将棋駒画像 ファイル名一覧（全28種）
■ 先手（Sente）- 14枚
Sente_Pawn_False.png (歩) 
Sente_Pawn_True.png (と金) 
Sente_Lance_False.png (香車) 
Sente_Lance_True.png (成香) 
Sente_Knight_False.png (桂馬) 
Sente_Knight_True.png (成桂) 
Sente_Silver_False.png (銀将) 
Sente_Silver_True.png (成銀) 
Sente_Gold_False.png (金将) 
Sente_Bishop_False.png (角行) 
Sente_Bishop_True.png (龍馬) 
Sente_Rook_False.png (飛車) 
Sente_Rook_True.png (龍王) 
Sente_King_False.png (王将)

■ 後手（Gote）- 14枚
Gote_Pawn_False.png (歩)
Gote_Pawn_True.png (と金)
Gote_Lance_False.png (香車) 
Gote_Lance_True.png (成香) 
Gote_Knight_False.png (桂馬) 
Gote_Knight_True.png (成桂) 
Gote_Silver_False.png (銀将) 
Gote_Silver_True.png (成銀) 
Gote_Gold_False.png (金将) 
Gote_Bishop_False.png (角行) 
Gote_Bishop_True.png (龍馬) 
Gote_Rook_False.png (飛車) 
Gote_Rook_True.png (龍王) 
Gote_King_False.png (玉将)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
■ 今後の更新予定
・持ち時間
・対局ログ保存機能
・オンライン対戦機能
・AI対戦機能
・駒のカスタムテーマ機能
・操作履歴可視化