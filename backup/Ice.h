#pragma once
#include <graphics.h>

inline void putimage_alphas(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();

	AlphaBlend(
		GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h,
		{ AC_SRC_OVER,0,255,AC_SRC_ALPHA }
	);
}

IMAGE img_player_left_ice[6];
IMAGE img_shadow_ice;
IMAGE img_ice;
IMAGE img_background_ice;

void RenderFrozenPlayer()
{
    static const POINT position = { 1075, 345 };

    static int counter = 0;			// 动画帧索引
    static int anim_timer = 0;		// 动画计时器
    static int frozen_timer = 0;	// 冰冻状态计时器
    static const int THICKNESS = 5;	// 扫描线宽度
    static int highlight_pos_y = 0;	// 修正：hightlight_pos_y → highlight_pos_y
    static bool is_frozen = false;	// 当前是否正在冰冻

    // 如果没有处于冰冻状态则更新动画计时器
    if ((!is_frozen) && (++anim_timer % 3 == 0))
        counter = (counter + 1) % 6;

    // 更新冻结计时器并重置扫描线位置
    if (++frozen_timer % 35 == 0)
    {
        is_frozen = !is_frozen;
        highlight_pos_y = -THICKNESS;
    }

    // 绘制玩家脚底阴影
    putimage_alphas(position.x + (80 - 32) / 2, position.y + 80, &img_shadow_ice);

    // 根据当前是否处于冰冻状态渲染不同的动画帧
    if (is_frozen)
    {
        // 拷贝当前帧用于后续处理
        IMAGE img_current_frame(img_player_left_ice[counter]);	// 修正：img_curent_frame → img_current_frame
        int width = img_current_frame.getwidth();			// 同步修正变量名
        int height = img_current_frame.getheight();		// 同步修正变量名

        // 更新高亮扫描线竖直坐标
        highlight_pos_y = (highlight_pos_y + 2) % height;

        // 获取当前帧图片和冰冻图片的色彩缓冲区
        DWORD* color_buffer_ice_img = GetImageBuffer(&img_ice);
        DWORD* color_buffer_frame_img = GetImageBuffer(&img_current_frame);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int idx = y * width + x;
                static const float RATIO = 0.25f;		// 混叠比例
                static const float THRESHOLD = 0.02f;	// 高亮阈值（正确拼写）
                DWORD color_ice_img = color_buffer_ice_img[idx];
                DWORD color_frame_img = color_buffer_frame_img[idx];
                if ((color_frame_img & 0xFF000000) >> 24)	// 0xFF000000为透明通道
                {
                    // 注意：色彩缓冲区中颜色存储的顺序是BGR，所以获取时需要调换B和R的位置
                    BYTE r = (BYTE)(GetBValue(color_frame_img)
                        * RATIO + GetBValue(color_ice_img) * (1 - RATIO));
                    BYTE g = (BYTE)(GetGValue(color_frame_img)
                        * RATIO + GetGValue(color_ice_img) * (1 - RATIO));
                    BYTE b = (BYTE)(GetRValue(color_frame_img)
                        * RATIO + GetRValue(color_ice_img) * (1 - RATIO));

                    // 修正：TRESHOLD → THRESHOLD；y < = → y <=；hightlight_pos_y → highlight_pos_y
                    if ((y >= highlight_pos_y && y <= highlight_pos_y + THICKNESS)
                        && ((r / 255.0f) * 0.2126f + (g / 255.0f) * 0.7152f + (b / 255.0f) * 0.0722f
                            >= THRESHOLD))
                    {
                        color_buffer_frame_img[idx]= (BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24));
                        continue;
                    }
                    color_buffer_frame_img[idx] = (BGR(RGB(r, g, b)) | (((DWORD)(BYTE)(255)) << 24));
                }
            }
        }
        putimage_alphas(position.x, position.y, &img_current_frame);
    }
    else putimage_alphas(position.x, position.y, &img_player_left_ice[counter]);
}

int IceEnter()
{
	initgraph(1280, 720);



	loadimage(&img_background_ice, _T("img/background.png"));
    loadimage(&img_shadow_ice, _T("img/shadow_player.png"));
    loadimage(&img_ice, _T("img/ice.jpg"));

    //动画加载
    for (int i = 0; i < 6; i++)
    {
        static TCHAR img_path[256];
        _stprintf_s(img_path, _T("img/player_left_%d.png"), i);
        loadimage(&img_player_left_ice[i], img_path);
    }

	BeginBatchDraw();
	
	int current_frame = 0;  // 当前帧索引（0-5，关键：控制单帧显示）
	const int frame_delay = 100;  // 每帧延时300ms（保持你的需求）

	while (1)
	{
		cleardevice();

		putimage(0, 0, &img_background_ice);

		static TCHAR text[64];
		_stprintf_s(text, _T("当前帧：%d"), current_frame);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(255, 0, 0));
		outtextxy(10, 10, text);

        RenderFrozenPlayer();


		FlushBatchDraw();
	
		current_frame = (current_frame + 1) % 6;

		Sleep(frame_delay);  // 延时300ms（每帧停留300ms）
	}
	
	EndBatchDraw();
	closegraph();

	return 0;
}