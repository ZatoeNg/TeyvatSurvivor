#pragma once
#include <graphics.h>

IMAGE img_player_left_sketch[6];
IMAGE img_player_sketch[6];
IMAGE img_background_sketch;
IMAGE img_shadow_sketch;

inline void putimage_alphas_sketch(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();

	AlphaBlend(
		GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h,
		{ AC_SRC_OVER,0,255,AC_SRC_ALPHA }
	);
}

void sketch()
{
	for (int i = 0; i < 6; i++)
	{
		int width = img_player_left_sketch[i].getwidth();
		int height = img_player_left_sketch[i].getheight();
		Resize(&img_player_sketch[i], width, height);

		//遍历图片的色彩缓冲区
		DWORD* color_buffer_raw_img = GetImageBuffer(&img_player_left_sketch[i]);
		DWORD* color_buffer_sketch_img = GetImageBuffer(&img_player_sketch[i]);

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int idx = y * width + x;

				if ((color_buffer_raw_img[idx] & 0xFF000000) >> 24)
					color_buffer_sketch_img[idx] = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);
			}
		}

	}
}

int SketchEnter()
{
	initgraph(1280, 720);



	loadimage(&img_background_sketch, _T("img/background.png"));
	loadimage(&img_shadow_sketch, _T("img/shadow_player.png"));

	//动画加载
	for (int i = 0; i < 6; i++)
	{
		static TCHAR img_path[256];
		_stprintf_s(img_path, _T("img/player_left_%d.png"), i);
		loadimage(&img_player_left_sketch[i], img_path);
	}

	BeginBatchDraw();

	int current_frame = 0;  // 当前帧索引（0-5，关键：控制单帧显示）
	const int frame_delay = 100;  // 每帧延时300ms（保持你的需求）

	sketch();

	while (1)
	{
		cleardevice();

		putimage(0, 0, &img_background_sketch);

		static TCHAR text[64];
		_stprintf_s(text, _T("当前帧：%d"), current_frame);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 0, 0));
		outtextxy(10, 10, text);

		putimage_alphas_sketch(1075 + (80 - 32) / 2, 345 + 80, &img_shadow_sketch);
		putimage_alphas_sketch(1075, 345, current_frame%3==0? &img_player_sketch[current_frame]: &img_player_left_sketch[current_frame]);


		FlushBatchDraw();

		current_frame = (current_frame + 1) % 6;

		Sleep(frame_delay);  // 延时300ms（每帧停留300ms）
	}



	EndBatchDraw();
	closegraph();

	return 0;
}