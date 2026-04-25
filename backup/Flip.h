#pragma once
#include <graphics.h>

IMAGE img_player_left_flip[6];
IMAGE img_player_right_flip[6];
IMAGE img_background_flip;
IMAGE img_shadow_flip;

inline void putimage_alpha_flip(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();

	AlphaBlend(
		GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h,
		{ AC_SRC_OVER,0,255,AC_SRC_ALPHA }
	);
}

void Flip()
{
	//动画加载
	for (int i = 0; i < 6; i++)
	{
		static TCHAR img_path[256];
		_stprintf_s(img_path, _T("img/player_left_%d.png"), i);
		loadimage(&img_player_left_flip[i], img_path);
	}

	//生成玩家向右动画
	for (int i = 0; i < 6; i++)
	{
		//调整向右动画大小
		int width = img_player_left_flip[i].getwidth();
		int height = img_player_left_flip[i].getheight();

		Resize(&img_player_right_flip[i], width, height);

		//遍历图片的色彩缓冲区，逐行水平翻转拷贝
		DWORD* color_buffer_left_img = GetImageBuffer(&img_player_left_flip[i]);
		DWORD* color_buffer_right_img = GetImageBuffer(&img_player_right_flip[i]);

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int idx_left_img = y * width + x;  //源像素索引
				int idx_right_img = y * width + (width - x - 1); //目标像素索引

				color_buffer_right_img[idx_right_img] = color_buffer_left_img[idx_left_img];

			}
		}
	}
}


int FlipEnter()
{
	initgraph(1280, 720);



	loadimage(&img_background_flip, _T("img/background.png"));
	loadimage(&img_shadow_flip, _T("img/shadow_player.png"));

	//动画加载
	for (int i = 0; i < 6; i++)
	{
		static TCHAR img_path[256];
		_stprintf_s(img_path, _T("img/player_left_%d.png"), i);
		loadimage(&img_player_left_flip[i], img_path);
	}

	BeginBatchDraw();

	int current_frame = 0;  // 当前帧索引（0-5，关键：控制单帧显示）
	const int frame_delay = 100;  // 每帧延时300ms（保持你的需求）

	Flip();

	while (1)
	{
		cleardevice();

		putimage(0, 0, &img_background_flip);

		static TCHAR text[64];
		_stprintf_s(text, _T("当前帧：%d"), current_frame);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 0, 0));
		outtextxy(10, 10, text);

		putimage_alphas(1075 + (80 - 32) / 2, 345 + 80, &img_shadow_flip);
		putimage_alphas(1000 + (80 - 32) / 2, 345 + 80, &img_shadow_flip);
		putimage_alpha_flip(1075, 345, &img_player_left_flip[current_frame]);
		putimage_alpha_flip(1000, 345, &img_player_right_flip[current_frame]);
		

		FlushBatchDraw();

		current_frame = (current_frame + 1) % 6;

		Sleep(frame_delay);  // 延时300ms（每帧停留300ms）
	}



	EndBatchDraw();
	closegraph();

	return 0;
}