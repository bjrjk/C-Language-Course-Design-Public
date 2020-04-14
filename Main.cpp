#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>
#include<graphics.h>
#include<mmsystem.h>
#pragma comment(lib, "WINMM.LIB")
//�곣��
#define NINF 0x80000000
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GAME_AREA_WIDTH WINDOW_WIDTH * 4 / 5
#define GAME_AREA_HEIGHT WINDOW_HEIGHT * 7 / 8
#define FISH_WIDTH 100
#define FISH_HEIGHT 62
#define FISH_CNT 7
#define TIME_DELAY 100 //��λΪ����(ms)
#define BUTTON_CNT 8
#define PROGRAM_TITLE "��������"
//�ṹ�塢ö�ٶ���
struct __node_ans { //С������ڵ�Ķ���
	int v;
	__node_ans* nxt;
	int x, y;
	int color;
};
typedef struct __node_ans node;
struct __coordinate { //����ṹ��
	int x, y;
};
typedef struct __coordinate coordinate;
struct __node_problem { //��ѧ��ṹ��
	int a, b;
	char op;
};
typedef struct __node_problem problem;
struct __rank_node{ //���а���Ϣ�ṹ��
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
enum ARRAY_OPERATION_TYPE { //�����������ö��
	TOP, PUSH, POP, EMPTY, FULL, CLEAR, INDEX, CNT, CHANGE
};
//������ȫ�ֱ�������
bool VARRunningUI(const bool* status = NULL) { //����UI״̬
	static bool UI = false;
	if (status)UI = *status;
	return UI;
}
bool VARRunning(const bool* status = NULL) { //ȫ������״̬
	static bool running = false;
	if (status)running = *status;
	return running;
}
int VARRunningTime(const int* t = NULL) { //��Ϸ����ʱ��(��1msΪ��λ)
	static int RunningTime = 0;
	if (t)RunningTime = *t;
	return RunningTime;
}
const char* VARMessage(const char* msg = NULL) { //��Ϸ״̬��ʾ��Ϣ
	static char message[500] = "";
	if (msg)strcpy(message, msg);
	return message;
}
const char* VARPlayerName(const char* player = NULL) { //����ǳ�
	static char name[200] = "";
	if (player)strcpy(name, player);
	return name;
}
int VARPlayerScore(const int* num = NULL) { //��ҷ���
	static int score = 0;
	if (num)score = *num;
	return score;
}
bool VARButtonClicked(int index, const bool* clk = NULL) { //��ť���״̬
	static bool clicked[BUTTON_CNT] = {}; //�±��0��ʼ
	if (clk)clicked[index] = *clk;
	return clicked[index];
}
int VARButtonPosition(int i, int j, bool update = false) { //��ť��ʾλ��
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
const char* VARButtonContent(int i, const char* str = NULL) { //��ť��ʾ����
	static char buttonContent[BUTTON_CNT][100] = {
		"��ʼ","��ͣ","����","�˳�","�浵","����","��һ��","������"
	};
	if (str)strcpy(buttonContent[i], str);
	return buttonContent[i];
}
const problem* VARProblemArray(ARRAY_OPERATION_TYPE oper, const problem* problemPtr = NULL,
	int index = 0) { //��ѧ��洢����
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
const coordinate* VARMouseCoordinates(const coordinate* x = NULL) { //���λ������
	static coordinate coord = { 0,0 };
	if (x)coord = *x;
	return &coord;
}
//��������
char* Int2Str(int x) { //������ת��Ϊ�ַ���
	//����Int2Str�������뼰ʱ�ͷŷ��ص��ַ����ռ䣬�����ڴ�й©
	char* result = (char*)malloc(sizeof(char) * 100);
	if (!result)return NULL;
	sprintf(result, "%d", x);
	return result;
}
int Random(int l, int r) { //����[l,r]��Χ�ڵ������
	return l + rand() % (r - l + 1);
}
void PlayMusic(const char* musicPath) { //��������
	PlaySound(_T(musicPath), NULL, SND_ASYNC | SND_FILENAME);
}
char* __InputBox(const char* message,const char* defaultContent=NULL) { //��Ϣ�����
	//����__InputBox�������뼰ʱ�ͷŷ��ص��ַ����ռ䣬�����ڴ�й©
	char* result = (char*)malloc(sizeof(char) * 100);
	InputBox(_T(result), 50, message, PROGRAM_TITLE, defaultContent);
	return result;
}
int WriteTextFile(const char* filePath, const char* str) { //д���ı��ļ�
	FILE* file = fopen(filePath, "w");
	if (!file)return 0;
	fprintf(file, "%s", str);
	fclose(file);
	return 1;
}
int ReadTextFile(const char* filePath, char* str) { //�����ı��ļ�
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
int cmp(const void * ptr1,const void * ptr2) { //��������ȽϺ���
	return (int)(1000*(((rank*)ptr2)->scorePerTime - ((rank*)ptr1)->scorePerTime));
}
//С��������
node* List_GetHead() { //�������ͷ�ڵ�
	//������Ϊ��ͷ�ڵ������ͷ�ڵ㲻�洢����
	static node head = { NINF,NULL };
	return &head;
}
node* List_CreateNode(int v, int x = -10, int y = -10, int color = -1) { //����һ���µ�С������ڵ�
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
void List_Destory(node* cur, int cnter) { //�ͷ������г�ͷ�ڵ�������нڵ㼰��ռ�
	//����ͷ�ڵ�ʱ���β�cnter����Ķ�Ӧʵ��Ϊ0
	if (cur->nxt)List_Destory(cur->nxt, cnter + 1);
	if (cnter)free(cur);
	else cur->nxt = NULL;
}
void List_InsertAfter(node* insertAfterPtr, node* newNode) { //��ָ���ڵ������ѽ��õĽڵ�
	newNode->nxt = insertAfterPtr->nxt;
	insertAfterPtr->nxt = newNode;
}
void List_DeleteAfter(node* deleteAfterPtr) { //ɾ��ָ���ڵ��Ľڵ�
	if (!deleteAfterPtr || !deleteAfterPtr->nxt)return;
	node* tmpPtr = deleteAfterPtr->nxt;
	deleteAfterPtr->nxt = deleteAfterPtr->nxt->nxt;
	free(tmpPtr);
}
node* List_GenerateRandomNodePtr() { //�����������ѡ��ڵ㷵��ָ��
	node* pre = List_GetHead();
	while (pre->nxt) {
		if (Random(0, 1))return pre->nxt;
	}
	return pre;
}
bool List_Traversal(bool (*fun)(node* prePtr)) { //����ı��������ô���ĺ���ָ����ɶ�����Ĳ���
	node* cur = List_GetHead();
	bool flag = false;
	while (cur->nxt) {
		flag |= fun(cur);
		cur = cur->nxt;
	}
	return flag;
}
int List_Size() { //�������ڵ����
	node* cur = List_GetHead();
	int cnter = 0;
	while (cur->nxt) {
		cnter++;
		cur = cur->nxt;
	}
	return cnter;
}
bool List_UpdateFishesPositions(node* prePtr) { //����������뺯���������µ���С��λ��
	node* cur = prePtr->nxt;
	cur->x -= 10;
	return true;
}
void List_RecoverFishesPositions() { //��������ԭ����С��λ��
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
bool List_QueryAns(int ans) { //С�������в���С��ֵ
	node* pre = List_GetHead();
	while (pre->nxt) {
		node* cur = pre->nxt;
		if (ans == cur->v)return true;
		pre = pre->nxt;
	}
	return false;
}
//ͼ�ν��沿��
void DrawFullScreenImage(const char* picturePath) { //����ȫ��ͼƬ
	IMAGE img(WINDOW_WIDTH, WINDOW_HEIGHT);
	loadimage(&img, _T(picturePath), WINDOW_WIDTH, WINDOW_HEIGHT);
	putimage(0, 0, &img);
}
void DrawMessage(const char* message, RECT area,
	UINT drawFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE, COLORREF textColor = RED) { //����������Ϣ
	//���øú���������Ϣǰ�����ȼ��ԭ��Ϣ�Ƿ��ѱ����
	settextcolor(textColor);
	setbkmode(TRANSPARENT);
	drawtext(_T(message), &area, drawFlags);
}
void DrawLine(int x1, int y1, int x2, int y2, COLORREF lineColor = BLUE) { //��������
	setlinecolor(lineColor);
	setlinestyle(PS_SOLID, 3);
	line(x1, y1, x2, y2);
}
void DrawButton(int x1, int y1, int x2, int y2, const char* str, int index = -1) { //���ư�ť
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
void DrawAlpha(IMAGE* workingSpace, IMAGE* picture, //����͸��ͼƬ���������ã�
	int picture_x, int picture_y) { // ����PNGͼ��ȥ͸������
	// xΪ����ͼƬ��X���꣬yΪY����
	DWORD* dst = GetImageBuffer(workingSpace);    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = workingSpace ? workingSpace->getwidth() : getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = workingSpace ? workingSpace->getheight() : getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�
	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++) {
		for (int ix = 0; ix < picture_width; ix++) {
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				dst[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}
void DrawFish(int v, int x, int y, int color) { //����С��
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
bool DrawSingleFish(node* prePtr) { //����������뺯����������С��
	// ע��˴���WorkingSpace����
	node* cur = prePtr->nxt;
	if (cur->x < 0 || cur->y < 0)return false;
	if (cur->x > GAME_AREA_WIDTH - FISH_WIDTH ||
		cur->y > GAME_AREA_HEIGHT - FISH_HEIGHT)
		return true;
	DrawFish(cur->v, cur->x, cur->y, cur->color);
	return true;
}
void DrawMousePointer() { //��������
	IMAGE mouse(100, 48);
	loadimage(&mouse, _T("Resources/Pictures/Pointer.png"));
	coordinate coord = *VARMouseCoordinates();
	if (0 < coord.x - 50 && coord.x - 50 + 100 < GAME_AREA_WIDTH &&
		0 < coord.y - 24 && coord.y - 24 + 48 < GAME_AREA_HEIGHT)
		DrawAlpha(NULL, &mouse, coord.x - 50, coord.y - 24);
}
//�����ڲ�����
void RefreshStatus() {//ˢ��״̬��Ϣ
	//����������Ϣ
	RECT authorArea = { 0,GAME_AREA_HEIGHT + 1,GAME_AREA_WIDTH * 1 / 2,WINDOW_HEIGHT };
	DrawMessage("https://github.com/bjrjk/", authorArea);
	//��������ʱ��
	RECT runTimeArea = { GAME_AREA_WIDTH * 1 / 2,GAME_AREA_HEIGHT + 1,
		GAME_AREA_WIDTH ,WINDOW_HEIGHT };
	char runTime[100];
	sprintf(runTime, "��Ϸʱ�䣺%02d:%02d.%03d", VARRunningTime() / 1000 / 60,
		VARRunningTime() / 1000 % 60, VARRunningTime() % 1000);
	DrawMessage(runTime, runTimeArea);
	//���Ʒָ���
	DrawLine(0, GAME_AREA_HEIGHT, GAME_AREA_WIDTH, GAME_AREA_HEIGHT);
	DrawLine(GAME_AREA_WIDTH, 0, GAME_AREA_WIDTH, WINDOW_HEIGHT);
	DrawLine(GAME_AREA_WIDTH, WINDOW_HEIGHT * 3 / 5, WINDOW_WIDTH, WINDOW_HEIGHT * 3 / 5);
	//���ư�ť
	for (int i = 0; i < BUTTON_CNT; i++)DrawButton(VARButtonPosition(i, 0),
		VARButtonPosition(i, 1), VARButtonPosition(i, 2), VARButtonPosition(i, 3),
		VARButtonContent(i), i);
	//����״̬��Ϣ
	RECT Message1Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 10 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 11 / 16 };
	DrawMessage("����ǳƣ�", Message1Area);
	RECT Message2Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 11 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 12 / 16 };
	DrawMessage(VARPlayerName(), Message2Area);
	RECT Message3Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 12 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 13 / 16 };
	DrawMessage("��ҷ�����", Message3Area);
	RECT Message4Area = { GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 13 / 16,WINDOW_WIDTH - 2,WINDOW_HEIGHT * 14 / 16 };
	char* scoreStr = Int2Str(VARPlayerScore());
	DrawMessage(scoreStr, Message4Area);
	free(scoreStr);
	//ˢ��״̬��ʾ��Ϣ
	DrawMessage(VARMessage(), RECT{ GAME_AREA_WIDTH + 2,WINDOW_HEIGHT * 14 / 16,
		WINDOW_WIDTH - 2,WINDOW_HEIGHT * 16 / 16 },
		DT_CENTER);
}
bool RefreshFishes() { //ˢ��С��ͼ��
	return List_Traversal(DrawSingleFish);
}
void UpdateFishesPositions() { //��������С��λ��
	List_Traversal(List_UpdateFishesPositions);
}
int CalculateProblem(problem p) { //������ѧ���
	if (p.op == '/' && p.b == 0)return NINF;
	switch (p.op) {
	case '+':return p.a + p.b; break;
	case '-':return p.a - p.b; break;
	case '*':return p.a * p.b; break;
	case '/':return p.a / p.b; break;
	}
	return NINF;
}
void GenerateProblem() { //������ѧ��
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
void GenerateWrongAns() { //���ɴ��������
	int ans = Random(-99, 99);
	if (List_QueryAns(ans))GenerateWrongAns();
	else List_InsertAfter(List_GetHead(), List_CreateNode(ans));
}
void RefreshProblem() { //ˢ����ѧ��
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
	��Ϸ�浵�������ļ��ṹ��
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
bool SaveCurrentState() { //���浱ǰ��Ϸ״̬
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
bool LoadState() { //���ļ��м�����Ϸ״̬
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
void Update_Rank() { //�������а���Ϣ
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
void Status_Clear() { //�����ǰ��Ϸ״̬
	List_Destory(List_GetHead(), 0);
	int tmpScore = 0;
	VARPlayerScore(&tmpScore);
	VARRunningTime(&tmpScore);
	VARMessage("");
	VARProblemArray(CLEAR);
}
void Button_Start() { //��ʼ��ť�¼�
	Status_Clear();
	for (int i = 0; i < 5; i++)GenerateProblem();
	for (int i = 0; i < 3; i++)GenerateWrongAns();
	bool tmpFlag = true;
	VARRunning(&tmpFlag);
	VARButtonContent(1, "��ͣ");
}
void Button_Pause() { //��ͣ��ť�¼�
	bool tmpFlag = false;
	VARRunning(&tmpFlag);
	VARButtonContent(1, "����");
}
void Button_Continue() { //������ť�¼�
	bool tmpFlag = true;
	VARRunning(&tmpFlag);
	VARButtonContent(1, "��ͣ");
}
void Button_Add() { //���Ӱ�ť�¼�
	for (int i = 0; i < 5; i++)
		if (!VARProblemArray(FULL))GenerateProblem();
		else VARMessage("�洢�ռ�������\n�������Ӹ�������\n��");
	List_RecoverFishesPositions();
}
void Button_Exit() { //�˳���ť�¼�
	char str[200] = "";
	sprintf(str, "���%s����ʱ%02d:%02d.%03d���÷�%d�֣�", VARPlayerName(),
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
void Button_Save() { //�浵��ť�¼�
	bool runningStatus = VARRunning();
	Button_Pause();
	if (SaveCurrentState())VARMessage("�浵�ɹ���");
	else VARMessage("�浵ʧ�ܣ�");
	if (runningStatus)Button_Continue();
}
bool Button_Load() { //������ť�¼�
	Status_Clear();
	bool runningStatus = VARRunning();
	Button_Pause();
	if (LoadState())VARMessage("�����ɹ���");
	else {
		Status_Clear();
		VARMessage("����ʧ�ܣ�");
		return false;
	}
	if (runningStatus)Button_Continue();
	return true;
}
void Button_Next() { //��һ�ⰴť�¼�
	VARProblemArray(CHANGE);
}
void ButtonClickEvent(int event) { //��ť����¼�
	bool flag = true;
	VARButtonClicked(event, &flag);
	switch (event) {
	case 0:Button_Start(); break;
	case 1:
		if (strcmp("��ͣ", VARButtonContent(1)) == 0)Button_Pause();
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
int List_PerformAnsClick(int x, int y, bool del = false) { //������ʵ��С��������
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
void CheckAnsClick(MOUSEMSG msg) { //���С��������
	int x = msg.x, y = msg.y;
	if (x > GAME_AREA_WIDTH || y > GAME_AREA_HEIGHT)return;
	if (msg.mkRButton) {
		if (VARProblemArray(EMPTY))List_PerformAnsClick(x, y, true);
		return;
	}
	if (VARProblemArray(EMPTY)) {
		VARMessage("��ǰû����Ŀ��\n�������Ŀ��");
		return;
	}
	int result = List_PerformAnsClick(x, y);
	if (result == 1) {
		VARProblemArray(POP);
		int tmpScore = VARPlayerScore();
		tmpScore++;
		VARPlayerScore(&tmpScore);
		VARMessage("����ˣ�\n��1�֣�");
		PlayMusic("Resources/Musics/Right.wav");
	}
	else if (result == 0) {
		int tmpScore = VARPlayerScore();
		tmpScore--;
		VARPlayerScore(&tmpScore);
		VARMessage("����ˣ�\n�����룡\n��1�֣�");
		PlayMusic("Resources/Musics/Wrong.wav");
	}
}
void MouseLRTrigger(MOUSEMSG cur) { //���������һ��¼�
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
	//��������¼�
	if (!flag && VARRunning())CheckAnsClick(cur);
}
void InitMouseClick(MOUSEMSG msg) { //��������������¼�
	int x = msg.x, y = msg.y;
	if (WINDOW_WIDTH * 4 / 10 < x && x < WINDOW_WIDTH * 6 / 10 &&
		WINDOW_HEIGHT * 6 / 11 + 5 < y && y < WINDOW_HEIGHT * 7 / 11 - 5) {
		PlayMusic("Resources/Musics/Click.wav");
		char* playerName = __InputBox("����������ǳƣ���������ո񣩣�","User");
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
			MessageBox(NULL, "�浵�ļ������ڣ�", PROGRAM_TITLE, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
		}
	}
	if (WINDOW_WIDTH * 4 / 10 < x && x < WINDOW_WIDTH * 6 / 10 &&
		WINDOW_HEIGHT * 8 / 11+5 < y && y < WINDOW_HEIGHT * 9 / 11-5) {
		PlayMusic("Resources/Musics/Click.wav");
		char* buffer = (char *)malloc(1000);
		char* buffer2 = (char*)malloc(1100);
		if(!ReadTextFile("Rank.txt", buffer))strcpy(buffer2, "��ǰ�������а���Ϣ��");
		else {
			strcpy(buffer2, "����\t�û���\t����\tʱ��\t��λʱ�����\t\n");
			strcat(buffer2, buffer);
		}
		MessageBox(NULL, buffer2, "���а���Ϣ", MB_OK | MB_SYSTEMMODAL);
		free(buffer);
		free(buffer2);
	}
	if (WINDOW_WIDTH * 4 / 10 < x && x < WINDOW_WIDTH * 6 / 10 &&
		WINDOW_HEIGHT * 9 / 11+5 < y && y < WINDOW_HEIGHT * 10 / 11-5) {
		PlayMusic("Resources/Musics/Click.wav");
		exit(0);
	}
}
void MouseTrigger() { //����¼�������
	bool flag = false;
	MOUSEMSG cur;
	while (MouseHit()) {
		flag = true;
		cur = GetMouseMsg(); //x�Ǻ���,y������
		if (VARRunningUI() && (cur.mkLButton || cur.mkRButton)) MouseLRTrigger(cur);
		if (!VARRunningUI() && cur.mkLButton) InitMouseClick(cur);
	}
	if (!flag)return;
	coordinate tmpCoord = { cur.x,cur.y };
	VARMouseCoordinates(&tmpCoord);
}
void ProgramInit() { //�����ʼ��
	srand(time(NULL));
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetWindowText(GetHWnd(), PROGRAM_TITLE);
	LOGFONT font;
	gettextstyle(&font);
	font.lfHeight += 2;
	font.lfWeight = 570;
	_tcscpy(font.lfFaceName, "��Բ");
	font.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&font);
	VARButtonPosition(0, 0, true); //���°�ťλ��
	VARMessage("��ӭ��������Ϸ��");
	int tmpScore = 0;
	VARPlayerScore(&tmpScore);
	setlinecolor(BLUE);
	setlinestyle(PS_SOLID, 3);
	setfillcolor(RGB(51,122,183));
}
void GameMain() { //��Ϸ����������
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
void InitMain() { //����ʼ����
	BeginBatchDraw();
	while (1) {
		DrawFullScreenImage("Resources/Pictures/InitBackGround.png");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 6 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 7 / 11 - 5, "��ʼ��Ϸ");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 7 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 8 / 11 - 5, "��ȡ�浵");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 8 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 9 / 11 - 5, "���а�");
		DrawButton(WINDOW_WIDTH * 4 / 10, WINDOW_HEIGHT * 9 / 11 + 5, WINDOW_WIDTH * 6 / 10,
			WINDOW_HEIGHT * 10 / 11 - 5, "�˳���Ϸ");
		FlushBatchDraw();
		Sleep(100);
		MouseTrigger();
		if (VARRunningUI())break;
	}
	EndBatchDraw();
}
void ProgramMain() { //����������
	while (1) {
		if (VARRunningUI())GameMain();
		else InitMain();
	}
}
int main() { //������
	ProgramInit();
	ProgramMain();
}