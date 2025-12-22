#include "DxLib.h"
#include "Library/Input.h"
#include <vector>
#include <string>
#include <map>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int CELL_SIZE = 64;
const int BOARD_TOTAL_SIZE = CELL_SIZE * 9;
const int BOARD_LEFT = (SCREEN_WIDTH - BOARD_TOTAL_SIZE) / 2;
const int BOARD_TOP = (SCREEN_HEIGHT - BOARD_TOTAL_SIZE) / 2;

enum class Turn { Sente, Gote };
enum class PieceType { EMPTY, PAWN, LANCE, KNIGHT, SILVER, GOLD, BISHOP, ROOK, KING };

struct Piece {
    PieceType type = PieceType::EMPTY;
    Turn owner = Turn::Sente;
    bool promoted = false;
    int img = -1;
};

Piece Board[9][9];
std::vector<Piece> Hand[2];
int selectedX = -1, selectedY = -1;
int selectedHandIdx = -1;
Turn currentTurn = Turn::Sente;
int bgImg = -1;
int moveCount = 1; // 手数（1手目から開始）

// 駒名の取得
std::string GetPieceName(Piece p) {
    if (p.promoted) {
        if (p.type == PieceType::BISHOP) return "馬";
        if (p.type == PieceType::ROOK) return "竜";
        return "と";
    }
    static std::map<PieceType, std::string> names = {
        {PieceType::PAWN,"歩"}, {PieceType::LANCE,"香"}, {PieceType::KNIGHT,"桂"},
        {PieceType::SILVER,"銀"}, {PieceType::GOLD,"金"}, {PieceType::BISHOP,"角"},
        {PieceType::ROOK,"飛"}, {PieceType::KING,"王"}
    };
    return names[p.type];
}

// 駒画像の読み込み
void LoadPieceImage(Piece& p) {
    if (p.type == PieceType::EMPTY) return;
    if (p.img != -1) { DeleteGraph(p.img); p.img = -1; }
    std::string turnStr = (p.owner == Turn::Sente) ? "Sente" : "Gote";
    std::string promStr = p.promoted ? "True" : "False";
    static std::map<PieceType, std::string> typeNames = {
        {PieceType::PAWN,"Pawn"}, {PieceType::LANCE,"Lance"}, {PieceType::KNIGHT,"Knight"},
        {PieceType::SILVER,"Silver"}, {PieceType::GOLD,"Gold"}, {PieceType::BISHOP,"Bishop"},
        {PieceType::ROOK,"Rook"}, {PieceType::KING,"King"}
    };
    std::string path = "data/" + turnStr + "_" + typeNames[p.type] + "_" + promStr + ".png";
    p.img = LoadGraph(path.c_str());
}

// 進路妨害チェック
bool IsPathClear(int fx, int fy, int tx, int ty) {
    int dx = (tx > fx) ? 1 : (tx < fx) ? -1 : 0;
    int dy = (ty > fy) ? 1 : (ty < fy) ? -1 : 0;
    int currX = fx + dx, currY = fy + dy;
    while (currX != tx || currY != ty) {
        if (Board[currY][currX].type != PieceType::EMPTY) return false;
        currX += dx; currY += dy;
    }
    return true;
}

// 二歩チェック
bool IsNifu(int tx, Turn owner, PieceType droppingType) {
    if (droppingType != PieceType::PAWN) return false;
    for (int y = 0; y < 9; y++) {
        if (Board[y][tx].type == PieceType::PAWN && Board[y][tx].owner == owner && !Board[y][tx].promoted) return true;
    }
    return false;
}

// 行き所のない駒チェック
bool IsInvalidDrop(int ty, PieceType type, Turn owner) {
    if (owner == Turn::Sente) {
        if (type == PieceType::PAWN || type == PieceType::LANCE) return ty == 0;
        if (type == PieceType::KNIGHT) return ty <= 1;
    }
    else {
        if (type == PieceType::PAWN || type == PieceType::LANCE) return ty == 8;
        if (type == PieceType::KNIGHT) return ty >= 7;
    }
    return false;
}

// 移動可能判定
bool CanMove(int fx, int fy, int tx, int ty, Piece p) {
    if (tx < 0 || tx >= 9 || ty < 0 || ty >= 9) return false;
    int dx = tx - fx, dy = ty - fy;
    int adx = abs(dx), ady = abs(dy);
    int dir = (p.owner == Turn::Sente) ? -1 : 1;
    if (Board[ty][tx].type != PieceType::EMPTY && Board[ty][tx].owner == p.owner) return false;

    if (p.promoted) {
        if (p.type == PieceType::BISHOP) {
            if ((adx == ady && IsPathClear(fx, fy, tx, ty)) || (adx <= 1 && ady <= 1)) return true;
        }
        if (p.type == PieceType::ROOK) {
            if (((dx == 0 || dy == 0) && IsPathClear(fx, fy, tx, ty)) || (adx <= 1 && ady <= 1)) return true;
        }
        if (p.type != PieceType::BISHOP && p.type != PieceType::ROOK) {
            return (adx <= 1 && ady <= 1) && !(adx == 1 && dy == -dir);
        }
    }

    switch (p.type) {
    case PieceType::PAWN:   return (dx == 0 && dy == dir);
    case PieceType::KNIGHT: return (adx == 1 && dy == dir * 2);
    case PieceType::LANCE:  return (dx == 0 && (p.owner == Turn::Sente ? dy < 0 : dy > 0)) && IsPathClear(fx, fy, tx, ty);
    case PieceType::SILVER: return (ady == 1 && adx == 1) || (dx == 0 && dy == dir);
    case PieceType::GOLD:   return (adx <= 1 && ady <= 1) && !(adx == 1 && dy == -dir);
    case PieceType::KING:   return (adx <= 1 && ady <= 1);
    case PieceType::BISHOP: return (adx == ady) && IsPathClear(fx, fy, tx, ty);
    case PieceType::ROOK:   return (dx == 0 || dy == 0) && IsPathClear(fx, fy, tx, ty);
    }
    return false;
}

void InitializeBoard() {
    for (int y = 0; y < 9; y++) for (int x = 0; x < 9; x++) Board[y][x].type = PieceType::EMPTY;
    Hand[0].clear(); Hand[1].clear();
    auto Place = [](int x, int y, PieceType t, Turn o) {
        Board[y][x] = { t, o, false, -1 }; LoadPieceImage(Board[y][x]);
        };
    for (int i : {0, 1}) {
        Turn o = (i == 0) ? Turn::Gote : Turn::Sente;
        int y = (i == 0) ? 0 : 8;
        Place(0, y, PieceType::LANCE, o); Place(8, y, PieceType::LANCE, o);
        Place(1, y, PieceType::KNIGHT, o); Place(7, y, PieceType::KNIGHT, o);
        Place(2, y, PieceType::SILVER, o); Place(6, y, PieceType::SILVER, o);
        Place(3, y, PieceType::GOLD, o); Place(5, y, PieceType::GOLD, o);
        Place(4, y, PieceType::KING, o);
    }
    Place(7, 1, PieceType::ROOK, Turn::Gote); Place(1, 1, PieceType::BISHOP, Turn::Gote);
    Place(1, 7, PieceType::ROOK, Turn::Sente); Place(7, 7, PieceType::BISHOP, Turn::Sente);
    for (int i = 0; i < 9; i++) { Place(i, 2, PieceType::PAWN, Turn::Gote); Place(i, 6, PieceType::PAWN, Turn::Sente); }
    bgImg = LoadGraph("data/background.png");
    currentTurn = Turn::Sente; selectedX = -1; selectedHandIdx = -1;
    moveCount = 1; // 手数をリセット
}

void DrawGame() {
    // 背景
    if (bgImg != -1) DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bgImg, FALSE);
    else DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GetColor(220, 180, 120), TRUE);

    // 盤面線
    for (int i = 0; i <= 9; i++) {
        DrawLine(BOARD_LEFT + i * CELL_SIZE, BOARD_TOP, BOARD_LEFT + i * CELL_SIZE, BOARD_TOP + BOARD_TOTAL_SIZE, 0);
        DrawLine(BOARD_LEFT, BOARD_TOP + i * CELL_SIZE, BOARD_LEFT + BOARD_TOTAL_SIZE, BOARD_TOP + i * CELL_SIZE, 0);
    }

    // 選択ハイライトと移動可能範囲
    if (selectedX != -1) {
        DrawBox(BOARD_LEFT + selectedX * CELL_SIZE, BOARD_TOP + selectedY * CELL_SIZE,
            BOARD_LEFT + (selectedX + 1) * CELL_SIZE, BOARD_TOP + (selectedY + 1) * CELL_SIZE, GetColor(255, 255, 0), TRUE);

        for (int ty = 0; ty < 9; ty++) {
            for (int tx = 0; tx < 9; tx++) {
                if (CanMove(selectedX, selectedY, tx, ty, Board[selectedY][selectedX])) {
                    DrawBox(BOARD_LEFT + tx * CELL_SIZE + 4, BOARD_TOP + ty * CELL_SIZE + 4,
                        BOARD_LEFT + (tx + 1) * CELL_SIZE - 4, BOARD_TOP + (ty + 1) * CELL_SIZE - 4, GetColor(0, 100, 255), FALSE);
                }
            }
        }
    }

    // 駒描画
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            Piece& p = Board[y][x]; if (p.type == PieceType::EMPTY) continue;
            int px = BOARD_LEFT + x * CELL_SIZE, py = BOARD_TOP + y * CELL_SIZE;
            if (p.img != -1) DrawExtendGraph(px, py, px + CELL_SIZE, py + CELL_SIZE, p.img, TRUE);

            unsigned int tc = (p.owner == Turn::Sente) ? GetColor(0, 0, 0) : GetColor(200, 0, 0);
            unsigned int out = GetColor(255, 255, 255);
            DrawFormatString(px + 43, py + 3, out, "%s", GetPieceName(p).c_str());
            DrawFormatString(px + 42, py + 2, tc, "%s", GetPieceName(p).c_str());
            DrawFormatString(px + 3, py + 3, out, p.owner == Turn::Sente ? "▲" : "▽");
            DrawFormatString(px + 2, py + 2, tc, p.owner == Turn::Sente ? "▲" : "▽");
        }
    }

    // 持ち駒
    for (int i = 0; i < 2; i++) {
        int baseX = (i == 0) ? 20 : BOARD_LEFT + BOARD_TOTAL_SIZE + 40;
        DrawFormatString(baseX, BOARD_TOP, 0, i == 0 ? "【先手 持】" : "【後手 持】");
        for (int j = 0; j < (int)Hand[i].size(); j++) {
            unsigned int c = (selectedHandIdx == j && (int)currentTurn == i) ? GetColor(255, 0, 0) : 0;
            DrawFormatString(baseX + (j / 12) * 80, BOARD_TOP + 30 + (j % 12) * 25, c, "- %s", GetPieceName(Hand[i][j]).c_str());
        }
    }
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE hp, LPSTR lp, int n) {
    ChangeWindowMode(TRUE); SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK); InitializeBoard();

    while (ProcessMessage() == 0 && ClearDrawScreen() == 0) {
        Input::KeyStateUpdate(); Input::MouseStateUpdate();
        int mx = Input::GetMouseX(), my = Input::GetMouseY();

        // ウィンドウタイトルの更新
        std::string turnName = (currentTurn == Turn::Sente) ? "先手" : "後手";
        char title[128];
        sprintf_s(title, "将棋ゲーム - %d手目 [%s の番]", moveCount, turnName.c_str());
        SetMainWindowText(title);

        if (Input::IsMouseDown(MOUSE_INPUT_LEFT)) {
            int bx = (mx - BOARD_LEFT) / CELL_SIZE, by = (my - BOARD_TOP) / CELL_SIZE;
            bool hitHand = false;
            for (int i = 0; i < 2; i++) {
                int baseX = (i == 0) ? 20 : BOARD_LEFT + BOARD_TOTAL_SIZE + 40;
                if (mx > baseX && mx < baseX + 150) {
                    int idx = (my - (BOARD_TOP + 30)) / 25;
                    if (idx >= 0 && idx < (int)Hand[i].size() && (int)currentTurn == i) { selectedHandIdx = idx; selectedX = -1; hitHand = true; }
                }
            }
            if (!hitHand && bx >= 0 && bx < 9 && by >= 0 && by < 9) {
                if (selectedHandIdx != -1) {
                    Piece p = Hand[(int)currentTurn][selectedHandIdx];
                    if (Board[by][bx].type == PieceType::EMPTY && !IsNifu(bx, currentTurn, p.type) && !IsInvalidDrop(by, p.type, currentTurn)) {
                        Board[by][bx] = p; Hand[(int)currentTurn].erase(Hand[(int)currentTurn].begin() + selectedHandIdx);
                        currentTurn = (currentTurn == Turn::Sente ? Turn::Gote : Turn::Sente);
                        moveCount++; // 手数を増やす
                    }
                    else if (IsNifu(bx, currentTurn, p.type)) MessageBox(NULL, "二歩です", "反則", MB_OK);
                    selectedHandIdx = -1;
                }
                else if (selectedX == -1) {
                    if (Board[by][bx].owner == currentTurn && Board[by][bx].type != PieceType::EMPTY) { selectedX = bx; selectedY = by; }
                }
                else {
                    if (CanMove(selectedX, selectedY, bx, by, Board[selectedY][selectedX])) {
                        if (Board[by][bx].type == PieceType::KING) {
                            DrawGame(); ScreenFlip(); MessageBox(NULL, (currentTurn == Turn::Sente ? "先手勝利！" : "後手勝利！"), "終局", MB_OK);
                            InitializeBoard();
                        }
                        else {
                            if (Board[by][bx].type != PieceType::EMPTY) {
                                Piece t = Board[by][bx]; t.owner = currentTurn; t.promoted = false; LoadPieceImage(t); Hand[(int)currentTurn].push_back(t);
                            }
                            Board[by][bx] = Board[selectedY][selectedX]; Board[selectedY][selectedX].type = PieceType::EMPTY;
                            if (!Board[by][bx].promoted && Board[by][bx].type != PieceType::KING && Board[by][bx].type != PieceType::GOLD && (currentTurn == Turn::Sente ? by <= 2 : by >= 6)) {
                                if (MessageBox(NULL, "成りますか？", "昇格", MB_YESNO) == IDYES) {
                                    Board[by][bx].promoted = true; LoadPieceImage(Board[by][bx]);
                                }
                            }
                            currentTurn = (currentTurn == Turn::Sente ? Turn::Gote : Turn::Sente);
                            moveCount++; // 手数を増やす
                        }
                    }
                    selectedX = -1;
                }
            }
        }
        DrawGame(); ScreenFlip();
    }
    DxLib_End(); return 0;
}