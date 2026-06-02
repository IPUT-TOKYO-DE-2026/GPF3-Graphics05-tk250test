#include "FrameBufferEmulator.h"

int N = 6;
constexpr int MAX_CNT = 10000; // 操作回数の最大許容値
int b[MAX_CNT] = { 1, 4, 1, 3, 2, 4 }; // 現在のブロックの各列の高さを格納する配列
int tmp[MAX_CNT]; // 操作の途中、一時的に新しい列の高さを格納するための作業用配列
int cnt;
unsigned char rectColor[] = { 0, 200, 0 };
unsigned char gridColor[] = { 200, 200, 200 };

// 長方形を描画する
void drawRectangle(unsigned char* buff, int width, int height, int sx, int sy, int w, int h, unsigned char color[3])
{
	int ex = sx + w;
	int ey = sy + h;
	unsigned char* pixel;
	for (int i = sy; i < ey; i++) { // 縦方向のループ
		pixel = buff + i * width * 3 + sx * 3; // フレームバッファ上で1行書き始める位置
		for (int j = sx; j < ex; j++) { // 横方向のループ
			*pixel++ = color[0]; // B
			*pixel++ = color[1]; // G
			*pixel++ = color[2]; // R
		}
	}
}

// 格子を描画する
void drawGrids(unsigned char* buff, int width, int height, int w, int h, unsigned char color[3])
{
	// 横線
	for (int y = 0; y < height; y += h) { // 縦方向のループ
		unsigned char* p = buff + (y * width * 3); // 書き込み始める位置を求める（行）
		for (int x = 0; x < width; x++) { // 横方向のループ
			*p++ = color[0];
			*p++ = color[1];
			*p++ = color[2];
		}
	}

	// 縦線
	for (int x = 0; x < width; x += w) { // 横方向のループ
		for (int y = 0; y < height; y++) { // 縦方向のループ
			unsigned char* p = buff + ((y * width + x) * 3); // 書き込み始める位置を求める（列）
			*p++ = color[0];
			*p++ = color[1];
			*p++ = color[2];
		}
	}
}

void FrameBufferEmulator::initUser()
{
	cnt = 0;
}

void FrameBufferEmulator::drawUser(unsigned char* buff, int mode, int keyLevel, int keyTrigger)
{
	bool isTriangle = true;
	if (b[0] != 1) { // 最初の列の高さが1でなければ、「三角形」ではない
		isTriangle = false;
	}
	// 2列目以降を確認
	for (int i = 1; i < N; i++) {
		// i番目の列の高さが、(i-1)番目の列の高さに1を加えたものでなければ、「三角形」ではない
		if (b[i] != b[i - 1] + 1) {
			isTriangle = false;
			break; // 条件を満たさないものが見つかった時点で2列目以降の確認を終了
		}
	}

	int sx = 0;  // 一番左の位置
	int rectSize = 50;
	for (int i = 0; i < N; i++) { // 横方向のループ
		int sy = 450;  // 一番下の段の位置
		for (int j = 0; j < b[i]; j++) { // 縦（段）方向のループ
			drawRectangle(buff, width, height, sx, sy, rectSize, rectSize, rectColor);
			sy -= rectSize; // 1段上の位置にずらす
		}
		sx += rectSize; // ひとつ右にずらす
	}

	drawGrids(buff, width, height, rectSize, rectSize, gridColor); // グリッド描画

	if (isTriangle) {
		return;  // もし配列bが「三角形」の形になっていれば終了
	}

	if (keyTrigger == SDLK_RIGHT) { // 右矢印キーが押されたらステップを進める
		if (cnt > MAX_CNT) { return; }

		// ブロックの数が1より多い列で新しい列を作る
		int m = 0; // 高さ1より大きい列の数をカウントする変数。これが新しいtmp配列の要素数になる
		// 配列bの各列をチェック
		for (int i = 0; i < N; i++) {
			if (b[i] > 1) { // 列の高さが1より大きい場合
				tmp[m++] = b[i] - 1;  // その列の高さから1を引いた値をtmp配列に格納する
			}
		}
		// この時点で、tmp配列には高さが1減らされた列（元の高さが1だった列は消滅）が格納され、mはその数となる
		// tmp配列の内容（新しい列の状態）をb配列にコピー
		for (int i = 0; i < m; i++) {
			b[i] = tmp[i];
		}
		b[m] = N; // 元の列の数 N（この操作を行う前のNの値）を、新しい列としてb配列の末尾に追加
		N = m + 1; // 新しい列の数を更新します (高さ1より大きかった列の数 m + 新しく追加した1列)
		cnt++;
	}
}
