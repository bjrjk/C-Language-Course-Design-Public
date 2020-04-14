#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<graphics.h>
#include<mmsystem.h>
#pragma comment(lib, "WINMM.LIB")
//宏常量
#define NINF 0x80000000
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GAME_AREA_WIDTH WINDOW_WIDTH * 4 / 5
#define GAME_AREA_HEIGHT WINDOW_HEIGHT * 7 / 8
#define FISH_WIDTH 100
#define FISH_HEIGHT 62
#define FISH_CNT 7
#define TIME_DELAY 100 //单位为毫秒(ms)
#define BUTTON_CNT 8
#define PROGRAM_TITLE "捕鱼能手"
//结构体、枚举定义
struct __node_ans { //小鱼链表节点的定义
	int v;
	__node_ans* nxt;
	int x, y;
	int color;
};
typedef struct __node_ans node;
struct __coordinate { //坐标结构体
	int x, y;
};
typedef struct __coordinate coordinate;
struct __node_problem { //数学题结构体
	int a, b;
	char op;
};
typedef struct __node_problem problem;
struct __rank_node{ //排行榜信息结构体
	int id;
	char name[100];
	int time, score;
	double scorePerTime;
	__rank_node() {
		id = 0;
		strcpy(name, "NULL");
		time = 0;
		score = 0;
		scorePerTime = 0;
	}
};
typedef struct __rank_node rank;
enum ARRAY_OPERATION_TYPE { //数组操作类型枚举
	TOP, PUSH, POP, EMPTY, FULL, CLEAR, INDEX, CNT, CHANGE
};
//程序类全局变量函数
bool VARRunningUI(const bool* status = NULL) { //界面UI状态
	static bool UI = false;
	if (status)UI = *status;
	return UI;
}
bool VARRunning(const bool* status = NULL) { //全局运行状态
	static bool running = false;
	if (status)running = *status;
	return running;
}
int VARRunningTime(const int* t = NULL) { //游戏运行时间(以1ms为单位)
	static int RunningTime = 0;
	if (t)RunningTime = *t;
	return RunningTime;
}
const char* VARMessage(const char* msg = NULL) { //游戏状态提示信息
	static char message[500] = "";
	if (msg)strcpy(message, msg);
	return message;
}
const char* VARPlayerName(const char* player = NULL) { //玩家昵称
	static char name[200] = "";
	if (player)strcpy(name, player);
	return name;
}
int VARPlayerScore(const int* num = NULL) { //玩家分数
	static int score = 0;
	if (num)score = *num;
	return score;
}
bool VARButtonClicked(int index, const bool* clk = NULL) { //按钮点击状态
	static bool clicked[BUTTON_CNT] = {}; //下标从0开始
	if (clk)clicked[index] = *clk;
	return clicked[index];
}
int VARButtonPosition(int i, int j, bool update = false) { //按钮显示位置
	static int buttonPosition[BUTTON_CNT][4];
	if (!update)return buttonPosition[i][j];
	int DrawRange_Height_Begin = 5;
	int DrawRange_Height_End = WINDOW_HEIGHT * 3 / 5 - 5;
	const int ButtonBlanking = 5;
	int DrawButtonHeight = (DrawRange_Height_End - DrawRange_Height_Begin
		- (BUTTON_CNT - 1) * ButtonBlanking) / BUTTON_CNT;
	for (int i = 0; i < BUTTON_CNT; i++) {
		buttonPosition[i][0] = GAME_AREA_WIDTH + 10;
		buttonPosition[i][2] = WINDOW_WIDTH - 10;
		buttonPosition[i][1] = DrawRange_Height_Begin + i * (DrawButtonHeight + ButtonBlanking);
		buttonPosition[i][3] = buttonPosition[i][1] + DrawButtonHeight;
	}
	return buttonPosition[i][j];
}
const char* VARButtonContent(int i, const char* str = NULL) { //按钮显示内容
	static char buttonContent[BUTTON_CNT][100] = {
		"开始","暂停","增加","退出","存档","读档","下一题","主界面"
	};
	if (str)strcpy(buttonContent[i], str);
	return buttonContent[i];
}
const problem* VARProblemArray(ARRAY_OPERATION_TYPE oper, const problem* problemPtr = NULL,
	int index = 0) { //数学题存储数组
	const int MAX_ARR_SIZE = 50;
	static problem STORAGE_ARR[MAX_ARR_SIZE + 1];
	static int ARRAY_SIZE = 0;
	switch (oper) {
	case TOP:return ARRAY_SIZE ? STORAGE_ARR + ARRAY_SIZE - 1 : NULL; break;
	case PUSH:
		if (VARProblemArray(FULL) || !problemPtr)return NULL;
		STORAGE_ARR[ARRAY_SIZE++] = *problemPtr;
		return STORAGE_ARR + ARRAY_SIZE - 1;
		break;
	case POP:
		if (VARProblemArray(EMPTY))return NULL;
		ARRAY_SIZE--;
		return (const problem*)1;
		break;
	case EMPTY:return (const problem*)(ARRAY_SIZE == 0); break;
	case FULL:return (const problem*)(ARRAY_SIZE == MAX_ARR_SIZE); break;
	case CLEAR:
		ARRAY_SIZE = 0;
		return (const problem*)1;
		break;
	case INDEX:
		return index > 0 && index <= ARRAY_SIZE ? STORAGE_ARR + index - 1 : NULL;
		break;
	case CNT:
		return (const problem*)ARRAY_SIZE;
		break;
	case CHANGE:
		for (int i = ARRAY_SIZE - 1; i >= 0; i--) {
			STORAGE_ARR[i + 1] = STORAGE_ARR[i];
		}
		STORAGE_ARR[0] = STORAGE_ARR[ARRAY_SIZE];
		break;
	}
	return NULL;
}
const coordinate* VARMouseCoordinates(const coordinate* x = NULL) { //鼠标位置坐标
	static coordinate coord = { 0,0 };
	if (x)coord = *x;
	return &coord;
}
//基础函数
char* Int2Str(int x) { //将数字转化为字符串
	//调用Int2Str函数后，请及时释放返回的字符串空间，避免内存泄漏
	char* result = (char*)malloc(sizeof(char) * 100);
	if (!result)return NULL;
	sprintf(result, "%d", x);
	return result;
}
int Random(int l, int r) { //生成[l,r]范围内的随机数
	return l + rand() % (r - l + 1);
}
void PlayMusic(const char* musicPath) { //播放音乐
	PlaySound(_T(musicPath), NULL, SND_ASYNC | SND_FILENAME);
}
char* __InputBox(const char* message,const char* defaultContent=NULL) { //信息输入框
	//调用__InputBox函数后，请及时释放返回的字符串空间，避免内存泄漏
	char* result = (char*)malloc(sizeof(char) * 100);
	InputBox(_T(result), 50, message, PROGRAM_TITLE, defaultContent);
	return result;
}
int WriteTextFile(const char* filePath, const char* str) { //写入文本文件
	FILE* file = fopen(filePath, "w");
	if (!file)return 0;
	fprintf(file, "%s", str);
	fclose(file);
	return 1;
}
int ReadTextFile(const char* filePath, char* str) { //读入文本文件
	FILE* file = fopen(filePath, "r");
	if (!file)return 0;
	while (1) {
		char c = fgetc(file);
		if (c == -1)break;
		*(str++) = c;
	}
	*str = '\0';
	fclose(file);
	return 1;
}
int cmp(const void * ptr1,const void * ptr2) { //降序排序比较函数
	return (int)(1000*(((rank*)ptr2)->scorePerTime - ((rank*)ptr1)->scorePerTime));
}
//小鱼链表部分
node* List_GetHead() { //获得链表头节点
	//本链表为带头节点的链表，头节点不存储数据
	static node head = { NINF,NULL };
	return &head;
}
node* List_CreateNode(int v, int x = -10, int y = -10, int color = -1) { //建立一个新的小鱼链表节点
	node* ptr = (node*)malloc(sizeof(node));
	if (!ptr)return NULL;
	ptr->v = v;
	ptr->x = x;
	ptr->y = y;
	if (color == -1)ptr->color = Random(0, FISH_CNT - 1);
	else ptr->color = color;
	ptr->nxt = NULL;
	return ptr;
}
void List_Destory(node* cur, int cnter) { //释放链表中除头节点外的所有节点及其空间
	//传入头节点时，形参cnter传入的对应实参为0
	if (cur->nxt)List_Destory(cur->nxt, cnter + 1);
	if (cnter)free(cur);
	else cur->nxt = NULL;
}
void List_InsertAfter(node* insertAfterPtr, node* newNode) { //在指定节点后插入已建好的节点
	newNode->nxt = insertAfterPtr->nxt;
	insertAfterPtr->nxt = newNode;
}
void List_DeleteAfter(node* deleteAfterPtr) { //删除指定节点后的节点
	if (!deleteAfterPtr || !deleteAfterPtr->nxt)return;
	node* tmpPtr = deleteAfterPtr->nxt;
	deleteAfterPtr->nxt = deleteAfterPtr->nxt->nxt;
	free(tmpPtr);
}
node* List_GenerateRandomNodePtr() { //在链表中随机选择节点返回指针
	node* pre = List_GetHead();
	while (pre->nxt) {
		if (Random(0, 1))return pre->nxt;
	}
	return pre;
}
bool List_Traversal(bool (*fun)(node* prePtr)) { //链表的遍历，利用传入的函数指针完成对链表的操作
	node* cur = List_GetHead();
	bool flag = false;
	while (cur->nxt) {
		flag |= fun(cur);
		cur = cur->nxt;
	}
	return flag;
}
int List_Size() { //获得链表节点个数
	node* cur = List_GetHead();
	int cnter = 0;
	while (cur->nxt) {
		cnter++;
		cur = cur->nxt;
	}
	return cnter;
}
bool List_UpdateFishesPositions(node* prePtr) { //链表遍历传入函数——更新单个小鱼位置
	node* cur = prePtr->nxt;
	cur->x -= 10;
	return true;
}
void List_RecoverFishesPositions() { //链表——复原所有小鱼位置
	const static int posY[3] = { GAME_AREA_HEIGHT * 3 / 7,GAME_AREA_HEIGHT * 4 / 7,GAME_AREA_HEIGHT * 5 / 7 };
	const int posX = GAME_AREA_WIDTH - FISH_WIDTH - 10;
	node* pre = List_GetHead();
	int cnter = 0;
	while (pre->nxt) {
		node* cur = pre->nxt;
		cur->y = posY[cnter % 3] + (Random(0, 1) ? 1 : -1) * Random(0, GAME_AREA_HEIGHT / 25);
		cur->x = posX + cnter / 3 * FISH_WIDTH + (Random(0, 1) ? 1 : -1) * Random(0, GAME_AREA_HEIGHT / 25);
		cnter++;
		pre = pre->nxt;
	}
}
bool List_QueryAns(int ans) { //小鱼链表中查找小鱼值
	node* pre = List_GetHead();
	while (pre->nxt) {
		node* cur = pre->nxt;
		if (ans == cur->v)return true;
		pre = pre->nxt;
	}
	return false;
}
//图形界面部分
void DrawFullScreenImage(const char* picturePath) { //绘制全屏图片
	IMAGE img(WINDOW_WIDTH, WINDOW_HEIGHT);
	loadimage(&img, _T(picturePath), WINDOW_WIDTH, WINDOW_HEIGHT);
	putimage(0, 0, &img);
}
void DrawMessage(const char* message, RECT area,
	UINT drawFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE, COLORREF textColor = RED) { //绘制文字信息
	//调用该函数绘制信息前，请先检查原信息是否已被清空
	settextcolor(textColor);
	setbkmode(TRANSPARENT);
	drawtext(_T(message), &area, drawFlags);
}
void DrawLine(int x1, int y1, int x2, int y2, COLORREF lineColor = BLUE) { //绘制线条
	setlinecolor(lineColor);
	setlinestyle(PS_SOLID, 3);
	line(x1, y1, x2, y2);
}
void DrawButton(int x1, int y1, int x2, int y2, const char* str, int index = -1) { //绘制按钮
	if (index != -1) {
		if (VARButtonClicked(index)) {
			solidroundrect(x1 + 4, y1 + 4, x2 - 4, y2 - 4, 50, 50);
			bool clked = false;
			VARButtonClicked(index, &clked);
		}
		else solidroundrect(x1, y1, x2, y2, 50, 50);
	}
	else solidroundrect(x1, y1, x2, y2, 50, 50);
	DrawMessage(str, RECT{ x1,y1,x2,y2 }, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
		WHITE);
}
void DrawAlpha(IMAGE* workingSpace, IMAGE* picture, //绘制透明图片（网上引用）
	int picture_x, int picture_y) { // 载入PNG图并去透明部分
	// x为载入图片的X坐标，y为Y坐标
	DWORD* dst = GetImageBuffer(workingSpace);    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = workingSpace ? workingSpace->getwidth() : getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = workingSpace ? workingSpace->getheight() : getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标
	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++) {
		for (int ix = 0; ix < picture_width; ix++) {
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				dst[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
void DrawFish(int v, int x, int y, int color) { //绘制小鱼
	const static char picturePath[FISH_CNT][100] = {
		"Resources/Pictures/Fish1.png",
		"Resources/Pictures/Fish2.png",
		"Resources/Pictures/Fish3.png",
		"Resources/Pictures/Fish4.png",
		"Resources/Pictures/Fish5.png",
		"Resources/Pictures/Fish6.png",
		"Resources/Pictures/Fish7.png",
	};
	IMAGE fish(FISH_WIDTH, FISH_HEIGHT);
	loadimage(&fish, _T(picturePath[color]));
	DrawAlpha(NULL, &fish, x, y);
	char* tmpStr = Int2Str(v);
	DrawMessage(tmpStr,
		RECT{ x,y,x + FISH_WIDTH,y + FISH_HEIGHT },
		DT_CENTER | DT_VCENTER | DT_SINGLELINE,
		WHITE);
	free(tmpStr);
}
bool DrawSingleFish(node* prePtr) { //链表遍历传入函数——绘制小鱼
	// 注意此处的WorkingSpace问题
	node* cur = prePtr->nxt;
	if (cur->x < 0 || cur->y < 0)return false;
	if (cur->x > GAME_AREA_WIDTH - FISH_WIDTH ||
		cur->y > GAME_AREA_HEIGHT - FISH_HEIGHT)
		return true;
	DrawFish(cur->v, cur->x, cur->y, cur->color);
	return true;
}
void DrawMousePointer() { //绘制渔网
	IMAGE mouse(100, 48);
	loadimage(&mouse, _T("Resources/Pictures/Pointer.png"));
	coordinate coord = *VARMouseCoordinates();
	if (0 < coord.x - 50 && coord.x - 50 + 100 < GAME_AREA_WIDTH &&
		0 < coord.y - 24 && coord.y - 24 + 48 < GAME_AREA_HEIGHT)
		DrawAlpha(NULL, &mouse, coord.x - 50, coord.y - 24);
}
//程序内部函数
void RefreshStatus() {//刷新状态信息
	//绘制作者信息
	RECT authorArea = { 0,GAME_AREA_HEIGHT + 1,GAME_AREA_WIDTH * 1 / 2,WINDOW_HEIGHT };
	DrawMessage("https://github.com/bjrjk/", authorArea);
	//绘制运行时间
	RECT runTimeArea = { GAME_AREA_WIDTH * 1 / 2,GAME_AREA_HEIGHT + 1,
		GAME_AREA_WIDTH ,WINDOW_HEIGHT };
	char runTime[100];
	sprintf(runTime, "游戏时间：%02d:%02d.%03d", VARRunningTime() / 1000 / 60,
		VARRunningTime() / 1000 % 60, VARRunningTime() % 1000);
	DrawMessage(runTime, runTimeArea);
	//绘制分割线
	DrawLine(0, GAME_AREA_HEIGHT, GAME_AREA_WIDTH, GAME_AREA_HEIGHT);
	DrawLine(GAME_AREA_WIDTH, 0, GAME_AREA_WIDTH, WINDOW_HEIGHT);
	DrawLine(GAME_AREA_WIDTH, WINDOW_HEIGHT * 3 / 5, WINDOW_WIDTH, WINDOW_HEIGHT * 3 / 5);
	//绘制按钮
	for (int i = 0; i < BUTTON_CNT; i++)DrawButton(VARButtonPosition(i, 0),
		VARButtonPosition(i, 1), VARButtonPosition(i, 2), VARButtonPosition(i, 3),
		VARButtonContent(i), i);
	//绘制状态信息
	RECT Message1Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 10 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 11 / 16 };
	DrawMessage("玩家昵称：", Message1Area);
	RECT Message2Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 11 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 12 / 16 };
	DrawMessage(VARPlayerName(), Message2Area);
	RECT Message3Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 12 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 13 / 16 };
	DrawMessage("玩家分数：", Message3Area);
	RECT Message4Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 13 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 14 / 16 };
	char* scoreStr = Int2Str(VARPlayerScore());
	DrawMessage(scoreStr, Message4Area);
	free(scoreStr);
	//刷新状态提示信息
	DrawMessage(VARMessage(), RECT{ GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 14 / 16,
		WINDOW_WIDTH - 2,WINDOW_HEIGHT * 16 / 16 },
		DT_CENTER);
}
bool RefreshFishes() { //刷新小鱼图形
	return List_Traversal(DrawSingleFish);
}
void UpdateFishesPositions() { //更新所有小鱼位置
	List_Traversal(List_UpdateFishesPositions);
}
int CalculateProblem(problem p) { //计算数学题答案
	if (p.op == '/' && p.b == 0)return NINF;
	switch (p.op) {
	case '+':return p.a + p.b; break;
	case '-':return p.a - p.b; break;
	case '*':return p.a * p.b; break;
	case '/':return p.a / p.b; break;
	}
	return NINF;
}
void GenerateProblem() { //生成数学题
	const char symbol[] = "+-*/";
	problem p;
	p.a = Random(0, 10);
	p.b = Random(0, 10);
	p.op = symbol[Random(0, 3)];
	int result = CalculateProblem(p);
	if (result == NINF || List_QueryAns(result))GenerateProblem();
	else {
		VARProblemArray(PUSH, &p);
		List_InsertAfter(List_GenerateRandomNodePtr(), List_CreateNode(result));
	}
}
void GenerateWrongAns() { //生成错误随机答案
	int ans = Random(-99, 99);
	if (List_QueryAns(ans))GenerateWrongAns();
	else List_InsertAfter(List_GetHead(), List_CreateNode(ans));
}
void RefreshProblem() { //刷新数学题
	char problemStr[100];
	if (!VARProblemArray(TOP))return;
	problem p = *VARProblemArray(TOP);
	sprintf(problemStr, "%d%c%d", p.a, p.op, p.b);
	IMAGE leaf;
	loadimage(&leaf, "Resources/Pictures/leaf.png");
	DrawAlpha(NULL, &leaf, GAME_AREA_WIDTH * 5 / 8, GAME_AREA_HEIGHT * 1 / 15);
	DrawMessage(problemStr, RECT{ GAME_AREA_WIDTH * 5 / 8, GAME_AREA_HEIGHT * 1 / 15,
		GAME_AREA_WIDTH * 7 / 8, GAME_AREA_HEIGHT * 3 / 15 }, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
		RED);
}
/*
	游戏存档二进制文件结构：
	int Problem_N;
	struct node_problem{
		int a,b;
		char op;
	}problems[Problem_N];
	int Fish_N;
	struct fish{
		int v,x,y,color;
	}[Fish_N];
	int playerName_Size;
	char playerName[playerName_Size+1];
	int playerScore;
	int playTime;
*/
bool SaveCurrentState() { //保存当前游戏状态
	FILE* f = fopen("GameState.dat", "wb");
	if (!f)return false;
	int Problem_N = (int)VARProblemArray(CNT);
	fwrite(&Problem_N, sizeof(int), 1, f);
	for (int i = 1; i <= Problem_N; i++)
		fwrite(VARProblemArray(INDEX, NULL, i), sizeof(problem), 1, f);
	int Fish_N = List_Size();
	fwrite(&Fish_N, sizeof(int), 1, f);
	node* prePtr = List_GetHead();
	while (prePtr->nxt) {
		node* cur = prePtr->nxt;
		fwrite(&cur->v, sizeof(int), 1, f);
		fwrite(&cur->x, sizeof(int), 1, f);
		fwrite(&cur->y, sizeof(int), 1, f);
		fwrite(&cur->color, sizeof(int), 1, f);
		prePtr = prePtr->nxt;
	}
	int playerName_Size = strlen(VARPlayerName());
	fwrite(&playerName_Size, sizeof(int), 1, f);
	fwrite(VARPlayerName(), sizeof(char), playerName_Size + 1, f);
	int playerScore = VARPlayerScore();
	fwrite(&playerScore, sizeof(int), 1, f);
	int playTime = VARRunningTime();
	fwrite(&playTime, sizeof(int), 1, f);
	fclose(f);
	return true;
}
bool LoadState() { //从文件中加载游戏状态
	FILE* f = fopen("GameState.dat", "rb");
	if (!f)return false;
	int Problem_N;
	fread(&Problem_N, sizeof(int), 1, f);
	for (int i = 1; i <= Problem_N; i++) {
		problem p;
		fread(&p, sizeof(p), 1, f);
		VARProblemArray(PUSH, &p);
	}
	int Fish_N;
	fread(&Fish_N, sizeof(int), 1, f);
	for (int i = 1; i <= Fish_N; i++) {
		int v, x, y, color;
		fread(&v, sizeof(int), 1, f);
		fread(&x, sizeof(int), 1, f);
		fread(&y, sizeof(int), 1, f);
		fread(&color, sizeof(int), 1, f);
		List_InsertAfter(List_GenerateRandomNodePtr(), List_CreateNode(v, x, y, color));
	}
	int playerName_Size;
	fread(&playerName_Size, sizeof(int), 1, f);
	char playerName[100];
	fread(&playerName, sizeof(char), playerName_Size + 1, f);
	VARPlayerName(playerName);
	int playerScore;
	fread(&playerScore, sizeof(int), 1, f);
	VARPlayerScore(&playerScore);
	int playTime;
	fread(&playTime, sizeof(int), 1, f);
	VARRunningTime(&playTime);
	fclose(f);
	return true;
}
void Update_Rank() { //更新排行榜信息
	rank arr[9];
	FILE* file = fopen("Rank.txt", "r");
	if (file) {
		for (int i = 0; i < 8; i++)fscanf(file, "%d %s %d %d %lf", &arr[i].id, arr[i].name,
			&arr[i].score, &arr[i].time, &arr[i].scorePerTime);
		fclose(file);
	}
	arr[8].score = VARPlayerScore();
	arr[8].time = VARRunningTime() / 1000;
	arr[8].scorePerTime = 1.0 * arr[8].score / arr[8].time;
	strcpy(arr[8].name, VARPlayerName());
	qsort(arr, 9, sizeof(rank), cmp);
	file = fopen("Rank.txt", "w");
	for (int i = 0; i < 8; i++)fprintf(file, "%d\t%s\t%d\t%d\t%.3lf\t\n", i + 1, arr[i].name, arr[i].score,
		arr[i].time, arr[i].scorePerTime);
	fclose(file);
}
void Status_Clear() { //清除当前游戏状态
	List_Destory(List_GetHead(), 0);
	int tmpScore = 0;
	VARPlayerScore(&tmpScore);
	VARRunningTime(&tmpScore);
	VARMessage("");
	VARProblemArray(CLEAR);
}
void Button_Start() { //开始按钮事件
	Status_Clear();
	for (int i = 0; i < 5; i++)GenerateProblem();
	for (int i = 0; i < 3; i++)GenerateWrongAns();
	bool tmpFlag = true;
	VARRunning(&tmpFlag);
	VARButtonContent(1, "暂停");
}
void Button_Pause() { //暂停按钮事件
	bool tmpFlag = false;
	VARRunning(&tmpFlag);
	VARButtonContent(1, "继续");
}
void Button_Continue() { //继续按钮事件
	bool tmpFlag = true;
	VARRunning(&tmpFlag);
	VARButtonContent(1, "暂停");
}
void Button_Add() { //增加按钮事件
	for (int i = 0; i < 5; i++)
		if (!VARProblemArray(FULL))GenerateProblem();
		else VARMessage("存储空间已满！\n不能增加更多问题\n！");
	List_RecoverFishesPositions();
}
void Button_Exit() { //退出按钮事件
	char str[200] = "";
	sprintf(str, "玩家%s，用时%02d:%02d.%03d，得分%d分！", VARPlayerName(),
		VARRunningTime() / 1000 / 60, VARRunningTime() / 1000 % 60,
		VARRunningTime() % 1000, VARPlayerScore());
	WriteTextFile("Score.txt", str);
	Update_Rank();
	if (VARPlayerScore() > 0) {
		DrawFullScreenImage("Resources/Pictures/Good.png");
		PlayMusic("Resources/Musics/Win.wav");
	}
	else {
		DrawFullScreenImage("Resources/Pictures/Encourage.png");
		PlayMusic("Resources/Musics/Failed.wav");
	}
	char* tmpScore = Int2Str(VARPlayerScore());
	char* tmpTime = Int2Str(VARRunningTime() / 1000);
	DrawMessage(tmpScore, RECT{ WINDOW_WIDTH * 17 / 20,WINDOW_HEIGHT * 15 / 20,
		WINDOW_WIDTH * 18 / 20,WINDOW_HEIGHT * 16 / 20 });
	DrawMessage(tmpTime, RECT{ WINDOW_WIDTH * 17 / 20,WINDOW_HEIGHT * 17 / 20,
		WINDOW_WIDTH * 18 / 20,WINDOW_HEIGHT * 19 / 20 });
	FlushBatchDraw();
	free(tmpScore);
	free(tmpTime);
	Sleep(12000);
	closegraph();
	exit(0);
}
void Button_Save() { //存档按钮事件
	bool runningStatus = VARRunning();
	Button_Pause();
	if (SaveCurrentState())VARMessage("存档成功！");
	else VARMessage("存档失败！");
	if (runningStatus)Button_Continue();
}
bool Button_Load() { //读档按钮事件
	Status_Clear();
	bool runningStatus = VARRunning();
	Button_Pause();
	if (LoadState())VARMessage("读档成功！");
	else {
		Status_Clear();
		VARMessage("读档失败！");
		return false;
	}
	if (runningStatus)Button_Continue();
	return true;
}
void Button_Next() { //下一题按钮事件
	VARProblemArray(CHANGE);
}
void ButtonClickEvent(int event) { //按钮点击事件
	bool flag = true;
	VARButtonClicked(event, &flag);
	switch (event) {
	case 0:Button_Start(); break;
	case 1:
		if (strcmp("暂停", VARButtonContent(1)) == 0)Button_Pause();
		else Button_Continue();
		break;
	case 2:Button_Add(); break;
	case 3:Button_Exit(); break;
	case 4:Button_Save(); break;
	case 5:Button_Load(); break;
	case 6:Button_Next(); break;
	case 7:
		bool tmp = false;
		VARRunningUI(&tmp);
		break;
	}
}
int List_PerformAnsClick(int x, int y, bool del = false) { //链表——实现小鱼点击操作
	node* pre = List_GetHead();
	while (pre->nxt) {
		node* cur = pre->nxt;
		if (cur->x < x && x < cur->x + FISH_WIDTH &&
			cur->y < y && y < cur->y + FISH_HEIGHT)
		{
			if (del) {
				List_DeleteAfter(pre);
				return 1;
			}
			problem p = *VARProblemArray(TOP);
			if (CalculateProblem(p) == cur->v) {
				List_DeleteAfter(pre);
				return 1;
			}
			else return 0;
		}
		pre = pre->nxt;
	}
	return NINF;
}
void CheckAnsClick(MOUSEMSG msg) { //检测小鱼点击操作
	int x = msg.x, y = msg.y;
	if (x > GAME_AREA_WIDTH || y > GAME_AREA_HEIGHT)return;
	if (msg.mkRButton) {
		if (VARProblemArray(EMPTY))List_PerformAnsClick(x, y, true);
		return;
	}
	if (VARProblemArray(EMPTY)) {
		VARMessage("当前没有题目，\n请添加题目！");
		return;
	}
	int result = List_PerformAnsClick(x, y);
	if (result == 1) {
		VARProblemArray(POP);
		int tmpScore = VARPlayerScore();
		tmpScore++;
		VARPlayerScore(&tmpScore);
		VARMessage("算对了！\n加1分！");
		PlayMusic("Resources/Musics/Right.wav");
	}
	else if (result == 0) {
		int tmpScore = VARPlayerScore();
		tmpScore--;
		VARPlayerScore(&tmpScore);
		VARMessage("算错了！\n再想想！\n减1分！");
		PlayMusic("Resources/Musics/Wrong.wav");
	}
}
void MouseLRTrigger(MOUSEMSG cur) { //检测鼠标左、右击事件
	bool flag = false;
	if (cur.mkLButton)
		for (int i = 0; i < BUTTON_CNT; i++) {
			if (VARButtonPosition(i, 0) < cur.x && cur.x < VARButtonPosition(i, 2) &&
				VARButtonPosition(i, 1) < cur.y && cur.y < VARButtonPosition(i, 3)) {
				PlayMusic("Resources/Musics/Click.wav");
				ButtonClickEvent(i);
				flag = true;
			}
		}
	//检测点击答案事件
	if (!flag && VARRunning())CheckAnsClick(cur);
}
void InitMouseClick(MOUSEMSG msg) { //程序主界面鼠标事件
	int x = msg.x, y = msg.y;
	if (WINDOW_WIDTH * 4 / 10 < x && x < WINDOW_WIDTH * 6 / 10 &&
		WINDOW_HEIGHT * 6 / 11 + 5 < y && y < WINDOW_HEIGHT * 7 / 11 - 5) {
		PlayMusic("Resources/Musics/Click.wav");
		char* playerName = __InputBox("请输入玩家昵称（请勿输入空格）：","User");
		VARPlayerName(playerName);
		free(playerName);
		Status_Clear();
		bool tmp = true;
		VARRunningUI(&tmp);
		Button_Start();
	}
	if (WINDOW_WIDTH * 4 / 10 < x && x < WINDOW_WIDTH * 6 / 10 &&
		WINDOW_HEIGHT * 7 / 11 +5< y && y < WINDOW_HEIGHT * 8 / 11-5) {
		PlayMusic("Resources/Musics/Click.wav");
		if (Button_Load()) {
			bool tmp = true;
			VARRunningUI(&tmp);
			Button_Continue();
		}
		else {
			MessageBox(NULL, "存档文件不存在！", PROGRAM_TITLE, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
		}
	}
	if (WINDOW_WIDTH * 4 / 10 < x && x < WINDOW_WIDTH * 6 / 10 &&
		WINDOW_HEIGHT * 8 / 11+5 < y && y < WINDOW_HEIGHT * 9 / 11-5) {
		PlayMusic("Resources/Musics/Click.wav");
		char* buffer = (char *)malloc(1000);
		char* buffer2 = (char*)malloc(1100);
		if(!ReadTextFile("Rank.txt", buffer))strcpy(buffer2, "当前暂无排行榜信息！");
		else {
			strcpy(buffer2, "排名\t用户名\t分数\t时间\t单位时间分数\t\n");
			strcat(buffer2, buffer);
		}
		MessageBox(NULL, buffer2, "排行榜信息", MB_OK | MB_SYSTEMMODAL);
		free(buffer);
		free(buffer2);
	}
	if (WINDOW_WIDTH * 4 / 10 < x && x < WINDOW_WIDTH * 6 / 10 &&
		WINDOW_HEIGHT * 9 / 11+5 < y && y < WINDOW_HEIGHT * 10 / 11-5) {
		PlayMusic("Resources/Musics/Click.wav");
		exit(0);
	}
}
void MouseTrigger() { //鼠标事件触发器
	bool flag = false;
	MOUSEMSG cur;
	while (MouseHit()) {
		flag = true;
		cur = GetMouseMsg(); //x是横向,y是纵向
		if (VARRunningUI() && (cur.mkLButton || cur.mkRButton)) MouseLRTrigger(cur);
		if (!VARRunningUI() && cur.mkLButton) InitMouseClick(cur);
	}
	if (!flag)return;
	coordinate tmpCoord = { cur.x,cur.y };
	VARMouseCoordinates(&tmpCoord);
}
void ProgramInit() { //程序初始化
	srand(time(NULL));
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetWindowText(GetHWnd(), PROGRAM_TITLE);
	LOGFONT font;
	gettextstyle(&font);
	font.lfHeight += 2;
	font.lfWeight = 570;
	_tcscpy(font.lfFaceName, "幼圆");
	font.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&font);
	VARButtonPosition(0, 0, true); //更新按钮位置
	VARMessage("欢迎来到本游戏！");
	int tmpScore = 0;
	VARPlayerScore(&tmpScore);
	setlinecolor(BLUE);
	setlinestyle(PS_SOLID, 3);
	setfillcolor(RGB(51,122,183));
}
void GameMain() { //游戏界面主函数
	BeginBatchDraw();
	while (1) {
		DrawFullScreenImage("Resources/Pictures/BackGround.png");
		RefreshStatus();
		bool flag = false;
		flag = RefreshFishes();
		RefreshProblem();
		if (VARRunning()) {
			int tmpTime = VARRunningTime();
			tmpTime += TIME_DELAY;
			VARRunningTime(&tmpTime);
		}
		Sleep(TIME_DELAY);
		MouseTrigger();
		DrawMousePointer();
		if (!VARRunningUI())break;
		FlushBatchDraw();
		if (VARRunning()) {
			if (flag)UpdateFishesPositions();
			else List_RecoverFishesPositions();
		}
	}
	EndBatchDraw();
}
void InitMain() { //程序开始界面
	BeginBatchDraw();
	while (1) {
		DrawFullScreenImage("Resources/Pictures/InitBackGround.png");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 6 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 7 / 11 - 5, "开始游戏");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 7 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 8 / 11 - 5, "读取存档");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 8 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 9 / 11 - 5, "排行榜");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 9 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 10 / 11 - 5, "退出游戏");
		FlushBatchDraw();
		Sleep(100);
		MouseTrigger();
		if (VARRunningUI())break;
	}
	EndBatchDraw();
}
void ProgramMain() { //程序主过程
	while (1) {
		if (VARRunningUI())GameMain();
		else InitMain();
	}
}
int main() { //主函数
	ProgramInit();
	ProgramMain();
}