#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdbool.h>
#include<string.h>
// #include<ev.h>
#include<unistd.h>

#define COLUMN 9
#define ROW 9
#define R(piece) "\033[31m"#piece"\033[0m" //敵棋(紅色)
#define B(piece) "\033[34m"#piece"\033[0m" //本棋(藍色)
#define MAXSTACK 200 //定義最大堆疊容量

// ev_io stdin_watcher;
// ev_timer timeout_watcher1;
// ev_timer timeout_watcher2;
// int now;
// int clk1 = 0;
// int clk2 = 0;


char* r_board[ROW][COLUMN];
char* board[ROW][COLUMN]; //9x9的棋盤
int xi, yi; //要移動的棋子
int xj, yj; //要移動到的目標位置
bool restart = 0; //初始值=0
bool isStandard = 1; //是否符合規則，初始值1，符合
bool gameOver = 1; //遊戲是否結束，0結束
int turn = -1;

void chessboardBuilding();
void printChessboard();
int blueOrRed(int x,int y);
void blueMove();
void redMove();
void rulesofAllKindsOfChess();
void isGameOver();
int isEmpty(); //判斷是否為空堆疊
int push(int xi, int yi, int xj, int yj); //push，將指定的資料存入堆疊
int pop(); //pop，從堆疊取出資料
void record_board();
int choose_option(); //選擇繼續遊戲或悔棋
// int game(void);

//Linked list的陣列宣告
struct node
{
	int from[2];
	int to[2];
	char* from_chess;
	char* to_chess;
	struct node *next;
};
typedef struct node NODE;
NODE *stack = NULL;

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;
int getopt(int argc, char *const *argv, const char *optstring);



int main(int argc, char **argv) 
{

	int game_mode=0;
	int ch;
	FILE *fptr;
	
	//開始getopt
	opterr = 0;
	while((ch = getopt(argc, argv, "ns:l:")) != -1)
	{
		switch(ch)
		{
			case 'n':
				game_mode = 1;
				break;

			case 's':
				if((fptr = fopen(optarg, "w")) == NULL)
				{
					printf("File could not be open.\n");
					return 0;
				}
				game_mode = 1;
				
				//fclose(fptr);
				break;

			case 'l':
				if((fptr = fopen(optarg, "r")) == NULL)
				{
					printf("File could not be open.\n");
					return 0;
				}
				game_mode = 2;
				fclose(fptr);
				break;
		}
	}

	if(game_mode == 1)
	{
		//fptr = fopen(optarg, "w");
		int turn = -1;
	
	chessboardBuilding(); //生成棋盤
	record_board(); //紀錄每局
	printChessboard(); //印出棋盤
	
	printf("Blue's turn\n"); //本棋先下

	while(gameOver){
		isStandard = 1;
		turn *= (-1); //雙方交替下棋
		switch(turn)
		{
			case 1:
				blueMove();
				turn = (restart) ? (turn*(-1)) : turn;
				break;
			case -1:
				redMove();
				turn = (restart) ? (turn*(-1)) : turn;
				break;
		}
		isGameOver();
	}

	if(turn == 1){
		printf("Game Over, Blue Win!\n");
	}
	if(turn == -1){
		printf("Game Over, Red Win!\n");
	}
	}

	if(game_mode == 2)
	{
		int turn = -1;
	
	chessboardBuilding(); //生成棋盤
	record_board(); //紀錄每局
	printChessboard(); //印出棋盤
	
	printf("Blue's turn\n"); //本棋先下

	while(gameOver){
		isStandard = 1;
		turn *= (-1); //雙方交替下棋
		switch(turn)
		{
			case 1:
				blueMove();
				turn = (restart) ? (turn*(-1)) : turn;
				break;
			case -1:
				redMove();
				turn = (restart) ? (turn*(-1)) : turn;
				break;
		}
		isGameOver();
	}

	if(turn == 1){
		printf("Game Over, Blue Win!\n");
	}
	if(turn == -1){
		printf("Game Over, Red Win!\n");
	}
	}

	/*fptr = fopen("game.txt", "w");
	int turn = -1;
	
	chessboardBuilding(); //生成棋盤
	record_board(); //紀錄每局
	printChessboard(); //印出棋盤
	
	printf("Blue's turn\n"); //本棋先下
	while(gameOver){
		isStandard = 1;
		turn *= (-1); //雙方交替下棋
	 	switch(turn)
	 	{
	 		case 1:
	 			blueMove();
	 			turn = (restart) ? (turn*(-1)) : turn;
	 			break;
	 		case -1:
	 			redMove();
	 			turn = (restart) ? (turn*(-1)) : turn;
	 			break;
	 	}
	 	isGameOver();
	 }
	 if(turn == 1){
	 	printf("Game Over, Blue Win!\n");
	 }
	 if(turn == -1){
	 	printf("Game Over, Red Win!\n");
	 }
	fclose(fptr);*/
}


//push，將指定的資料存入Linked list
int push(int xi, int yi, int xj, int yj)
{
	NODE *tmp;
	tmp = (NODE *)malloc(sizeof(NODE));
	if(tmp != NULL){
		tmp->from[0] = xi;
		tmp->from[1] = yi;
		tmp->to[0] = xj;
		tmp->to[1] = yj;
		tmp->from_chess = board[xi-1][yi-1];
		tmp->to_chess = board[xj-1][yj-1];
		tmp->next = stack;
		stack = tmp;
		return 1;
	}
}

//pop，從Linked list取出資料
int pop()
{
	NODE *tmp = NULL;
	if(stack == NULL){
		return -1;
	}
	tmp = stack;
	stack = stack->next;
	board[tmp->from[0]-1][tmp->from[1]-1] = tmp->from_chess;
	board[tmp->to[0]-1][tmp->to[1]-1] = tmp->to_chess;
	free(tmp);
	return 0;
}

void record_board(void)
{
	for(int i=0 ; i<ROW ; i++){
		for(int j=0 ; j<COLUMN ; j++){
			r_board[i][j] = board[i][j];
		}
	}
}

NODE *inverse()
{
	NODE *front, *fnext,*list;
	list = stack;
	front = NULL;
	while(list){
		fnext = front;
		front = list;
		list = list->next;
		front->next = fnext;
	}
	return (front);
}

//印出開始遊戲後的所有步伐
void replay()
{
	NODE *tmp;
	int t=0;
	for(int i=0 ; i<ROW ; i++){
		for(int j=0 ; j<COLUMN ; j++){
			board[i][j] = r_board[i][j];
		}
	}
	tmp = inverse();
	do{
		board[tmp->to[0]-1][tmp->to[1]-1] = board[tmp->from[0]-1][tmp->from[1]-1];
		board[tmp->from[0]-1][tmp->from[1]-1] = "　";
		printf("---------- Round%d ---------\n",t+1);
		printChessboard();
		tmp = tmp->next;
		t++;
	}while(tmp);
}

int choose_option()
{
	char option[10];
	fflush(stdin);
	while(1)
	{
		printf("Choose the option: (0.Back to last step / 1.Play / 2.Replay the chessboard)");
		scanf("%s",option);
		if(strlen(option) != 1)
		{
			printf("Wrong enter, please enter again!\n");
			continue;
		}
		if(option[0] == '1')
		{
			printf("Continue\n");
			return 0;
		}
		if(option[0] == '0')
		{
			if(pop()){
				printf("can't go back\n");
				continue;
			}
			else{
				printChessboard();
				// turn = turn*(-1);
				// printf("%d",turn);
				// printf("%s's turn\n", turn ? "BLUE" : "RED");
				// clk1 = clk2 = 0;
				continue;
			}
		}
		if(option[0] == '2')
		{
			replay();
			continue;
		}
	}
}


//生成棋盤
void chessboardBuilding()
{
	for(int i=0;i<ROW;i++){
		for(int j=0;j<COLUMN;j++){
			board[i][j] = "　";
		}
		printf("\n");
	}
	//布置敵棋
	board[0][0] = board[0][8] = R(香);
	board[0][1] = board[0][7] = R(桂);
	board[0][2] = board[0][6] = R(銀);
	board[0][3] = board[0][5] = R(金);
	board[0][4] = R(王);
	board[1][1] = R(飛);
	board[1][7] = R(角);
	board[2][0] = board[2][1] = board[2][2] = board[2][3] = board[2][4] = board[2][5] = board[2][6] = board[2][7] = board[2][8] = R(步);

	//布置本棋
	board[8][0] = board[8][8] = B(香);
	board[8][1] = board[8][7] = B(桂);
	board[8][2] = board[8][6] = B(銀);
	board[8][3] = board[8][5] = B(金);
	board[8][4] = B(王);
	board[7][1] = B(角);
	board[7][7] = B(飛);
	board[6][0] = board[6][1] = board[6][2] = board[6][3] = board[6][4] = board[6][5] = board[6][6] = board[6][7] = board[6][8] = B(步);

}

//印出棋盤
void printChessboard()
{
	// system("clear");
	printf(" 1 2 3 4 5 6 7 8 9\n");
	for(int i=0;i<ROW;i++){
		printf("%d",i+1);
		for(int j=0;j<COLUMN;j++){
			printf("%s",board[i][j]);
		}
		printf("%d",i+1);
		printf("\n");
	}
	printf(" 1 2 3 4 5 6 7 8 9\n");
}

//判斷是藍棋還是紅棋,藍棋返回1,紅棋返回-1,否則返回0
int blueOrRed(int x,int y)
{
    if (board[x-1][y-1] == B(香) || board[x-1][y-1] == B(王) || board[x-1][y-1] == B(金) || board[x-1][y-1] == B(銀) || board[x-1][y-1] == B(桂) || board[x-1][y-1] == B(角) || board[x-1][y-1] == B(飛) || board[x-1][y-1] == B(步))
    {
        return  1;
    }
    else if (board[x-1][y-1] == R(香) || board[x-1][y-1] == R(王) || board[x-1][y-1] == R(金) || board[x-1][y-1] == R(銀) || board[x-1][y-1] == R(桂) || board[x-1][y-1] == R(角) || board[x-1][y-1] == R(飛) || board[x-1][y-1] == R(步))
    {
        return -1;
    }
    else
        return 0;
}

//本棋移動
void blueMove()
{
	if(restart){
		printf("違反遊戲規則，請重新輸入\n");
		restart = 0;
	}
	choose_option();
	printf("[藍棋]輸入你要移動的棋子:\n");
	scanf("%d %d",&xi,&yi);
	printf("[藍棋]輸入你要放的位置:\n");
	scanf("%d %d",&xj,&yj);
	//fprintf("game.txt", "Blue move: %d %d %d %d", xi, yi, xj, yj);
	rulesofAllKindsOfChess();
	printChessboard();
}

//敵棋移動
void redMove()
{
	if(restart){
		printf("違反遊戲規則，請重新輸入\n");
		restart = 0;
	}
	choose_option();
	printf("[紅棋]輸入你要移動的棋子:\n");
	scanf("%d %d",&xi,&yi);
	printf("[紅棋]輸入你要放的位置:\n");
	scanf("%d %d",&xj,&yj);
	//fprintf("game.txt", "Red move: %d %d %d %d", xi, yi, xj, yj);
	rulesofAllKindsOfChess();
	printChessboard();
}

//判斷遊戲結束
void isGameOver()
{
	bool sign_b = 0;
	bool sign_r = 0;
	for(int i=0;i<ROW;i++)
	{
		for(int j=0;j<COLUMN;j++)
		{
			if(board[i][j] == B(王))
			{
				sign_b = 1;
			}
			else if(board[i][j] == R(王))
			{
				sign_r = 1;
			}
		}
	}	
	if((sign_b == 0 || sign_r == 0))
	{
		gameOver = 0;
	}
}

//棋子移動的規則
void rulesofAllKindsOfChess()
{
	//B(步)，只能向前行一格，不能後退
	if(board[xi-1][yi-1] == B(步))
	{
		if(xi < xj)
			isStandard = 0; //如果倒退，不符合規則
		
	
		if( isStandard && blueOrRed(xj,yj) != 1 && (xj == xi - 1) && (yj == yi))
		{
			push(xi,yi,xj,yj);
			board[xi-1][yi-1] = "　";
			board[xj-1][yj-1] = B(步);
		}
		else 
		{
			restart = 1;
		}
		
	}
	
	//R(步)，只能向前行一格，不能後退
	else if(board[xi-1][yi-1] == R(步))
	{
		if(xi > xj)
			isStandard = 0; //如果倒退，不符合規則
	
		if( isStandard && blueOrRed(xj,yj) != -1 && (xj == xi + 1) && (yi == yj) )
		{
			push(xi,yi,xj,yj);
			board[xi-1][yi-1] = "　";
			board[xj-1][yj-1] = R(步);
		}
		else 
		{
			restart = 1;
		}
		
	}

	//B(香)，可向前行任何一格，不能後退
	else if(board[xi-1][yi-1] == B(香))
	{

		if(xj>xi){
			isStandard = 0; //如果倒退，不符合規則
		}

		int diff = xi - xj;
		for(int i = 1;i<diff;i++){
			if(board[xi-1-i][yi] != "　"){
				isStandard = 0;
			}
		}//判斷路徑中有無其他棋子，若有則不符合規則


		if( isStandard && (yi==yj) && blueOrRed(xj,yj) != 1 ){
			push(xi,yi,xj,yj);
			board[xi-1][yi-1] = "　";
			board[xj-1][yj-1] = B(香);
		}
		else
		{
			restart = 1;
		}
	}

	//R(香)，可向前行任何一格，不能後退
	else if(board[xi-1][yi-1] == R(香))
	{
		if(xj<xi){
			isStandard = 0; //如果倒退，不符合規則
		}

		int diff = xj - xi;
		for(int i = 1;i<diff;i++){
			if(board[xi-1+i][yi] != "　"){
				isStandard = 0;
			}
		}//判斷路徑中有無其他棋子，若有則不符合規則

		if( isStandard && (yi==yj) && blueOrRed(xj,yj) != -1 ){
			push(xi,yi,xj,yj);
			board[xi-1][yi-1] = "　";
			board[xj-1][yj-1] = R(香);
		}
		else
		{
			restart = 1;
		}
	}

	//B(桂)，可走右上格或左上格對上的一格
	else if(board[xi-1][yi-1] == B(桂))
	{
		if(xi<xj){
			isStandard = 0; //如果倒退，不符合規則
		}
		if(isStandard && blueOrRed(xj,yj) != 1 && (xj == xi-2) && ( (yj == yi-1) || (yj == yi+1 ) ) ){
			push(xi,yi,xj,yj);
			board[xi-1][yi-1] = "　";
			board[xj-1][yj-1] = B(桂);
		}
		else{
			restart = 1;
		}
	}

	//R(桂)，可走右上格或左上格對上的一格
	else if(board[xi-1][yi-1] == R(桂))
	{
		if(xi>xj){
			isStandard = 0; //如果倒退，不符合規則
		}
		if(isStandard && blueOrRed(xj,yj) != -1 && (xj == xi+2) && ( (yj == yi-1) || (yj == yi+1 ) ) ){
			push(xi,yi,xj,yj);
			board[xi-1][yi-1] = "　";
			board[xj-1][yj-1] = R(桂);
		}
		else{
			restart = 1;
		}
	}

	//B(銀)，可走前面、右上、右下、左上、左下的一格
	else if(board[xi-1][yi-1] == B(銀))
	{
		if(xj == xi){
			isStandard = 0; //如果要移動到同一ROW，不符合規則
		}
		if(isStandard && blueOrRed(xj,yj) != 1 )
		{
			if((xj == xi-1) && ((yj == yi)||(yj == yi-1)||(yj == yi+1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(銀);
			}
			else if(((xj == xi+1) && ((yj == yi+1)|| (yj == yi-1)))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(銀);
			}
			
		}
		else
		{
			restart = 1;
		}
	}

	//R(銀)，可走前面、右上、右下、左上、左下的一格
	else if(board[xi-1][yi-1] == R(銀))
	{
		if(xj == xi){
			isStandard = 0; //如果要移動到同一ROW，不符合規則
		}
		if(isStandard && blueOrRed(xj,yj) != -1 )
		{
			if((xj == xi+1) && ((yj == yi)||(yj == yi-1)||(yj == yi+1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(銀);
			}
			else if(((xj == xi-1) && ((yj == yi+1)|| (yj == yi-1)))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(銀);
			}
			
		}
		else
		{
			restart = 1;
		}
	}

	//B(金)，可走前面、右上、右面、左上、左面、下面的一格
	else if(board[xi-1][yi-1] == B(金))
	{
		// if((xj!=xi) || (xj!=xi+1) || (xj!=xi-1)){
		// 	isStandard = 0; //不符合金的移動規則
		// }
		if(isStandard && blueOrRed(xj,yj) != 1 ){
			if((xj == xi-1) && ((yj == yi)||(yj == yi+1)||(yj == yi-1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(金);
			}
			if((xj == xi) && ((yj == yi+1)||(yj == yi-1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(金);
			}
			if((xj == xi+1) && (yj == yi)){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(金);
			}
		}
		else
		{
			restart = 1;
		}
		
	}

	//R(金)，可走前面、右上、右面、左上、左面、下面的一格
	else if(board[xi-1][yi-1] == R(金))
	{
		// if((xj!=xi) || (xj!=xi+1) || (xj!=xi-1)){
		// 	isStandard = 0; //不符合金的移動規則
		// }
		if(isStandard && blueOrRed(xj,yj) != 1 ){
			if((xj == xi+1) && ((yj == yi)||(yj == yi+1)||(yj == yi-1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(金);
			}
			if((xj == xi) && ((yj == yi+1)||(yj == yi-1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(金);
			}
			if((xj == xi-1) && (yj == yi)){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(金);
			}
		}
		else
		{
			restart = 1;
		}
		
	}

	//B(王)，可走九宮格除了中心的一格
	else if(board[xi-1][yi-1] == B(王))
	{
		
		if( isStandard && blueOrRed(xj,yj) != 1 ){
			if((xj == xi-1) && ((yj == yi-1) || (yj == yi) || (yj == yi+1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(王);
			}
			if((xj == xi) && ((yj == yi+1)||(yj == yi-1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(王);
			}
			if((xj == xi+1) && ((yj == yi-1)||(yj == yi)||(yj == yi+1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(王);
			}
		}
		else {
			restart = 1;
		}
	}

	//R(王)，可走九宮格除了中心的一格
	else if(board[xi-1][yi-1] == R(王))
	{
		
		if( isStandard && blueOrRed(xj,yj) != -1 ){
			if((xj == xi-1) && ((yj == yi-1) || (yj == yi) || (yj == yi+1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(王);
			}
			if((xj == xi) && ((yj == yi+1) || (yj == yi-1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(王);
			}
			if((xj == xi+1) && ((yj == yi-1) || (yj == yi) || (yj == yi+1))){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(王);
			}
		}
		else {
			restart = 1;
		}
	}
	
	//B(角)，走兩對角線的任何一格
	else if(board[xi-1][yi-1] == B(角))
	{
		
		//目標位置在棋子右上方
		if((xj<xi) && (yj>yi)){
			
			int diff = xi - xj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1-i][yi-1+i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則
		
			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(角);
			}
			else{
				restart = 1;
			}

		}

		//目標位置在棋子左上方
		if((xj<xi) && (yj<yi)){

			int diff = xi - xj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1-i][yi-1-i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則

			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(角);
			}
			else{
				restart = 1;
			}

		}

		//目標位置在棋子左下方
		if((xj>xi) && (yj<yi)){

			int diff = xj - xi;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1+i][yi-1-i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則

			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(角);
			}
			else{
				restart = 1;
			}
		}

		//目標位置在棋子右下方
		if((xj>xi) && (yj>yi)){

			int diff = xj - xi;
			for(int i =1 ; i<diff ; i++){
				if(board[xi-1+i][yi-1+i]){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則

			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(角);
			}
			else{
				restart = 1;
			}
		}
	
	 	}

	//R(角)，走兩對角線的任何一格
	else if(board[xi-1][yi-1] == R(角))
	{
		
		//目標位置在棋子右上方
		if((xj<xi) && (yj>yi)){
			
			int diff = xi - xj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1-i][yi-1+i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則
		
			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(角);
			}
			else{
				restart = 1;
			}

		}

		//目標位置在棋子左上方
		if((xj<xi) && (yj<yi)){

			int diff = xi - xj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1-i][yi-1-i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則

			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(角);
			}
			else{
				restart = 1;
			}

		}

		//目標位置在棋子左下方
		if((xj>xi) && (yj<yi)){

			int diff = xj - xi;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1+i][yi-1-i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則

			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(角);
			}
			else{
				restart = 1;
			}
		}

		//目標位置在棋子右下方
		if((xj>xi) && (yj>yi)){

			int diff = xj - xi;
			for(int i =1 ; i<diff ; i++){
				if(board[xi-1+i][yi-1+i]){
					isStandard = 0;
				}
			}//若路徑上有別的棋子則不符合規則

			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(角);
			}
			else{
				restart = 1;
			}
		}
		

	}

	//B(飛)，走十字
	else if(board[xi-1][yi-1] == B(飛))
	{
		
		//目標位置在棋子正上方
		if((xj<xi) && (yj==yi)){

			int diff = xi - xj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1-i][yi-1] != "　"){
					isStandard = 0;
				}	
			}//若路徑上有其他棋子，則不符合規則
			
			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(飛);
			}
			else{
				restart = 1;
			}

		}
			
		//目標位置在棋子正下方
		if((xj>xi) && (yj==yi)){

			int diff = xj - xi;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1+i][yi-1] != "　"){
					isStandard = 0;
				}
			}//若路徑上有其他棋子，則不符合規則

			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(飛);
			}
			else{
				restart = 1;
			}
		}

		//目標位置在棋子正左方
		if((xj==xi) && (yj<yi)){
			int diff = yi -yj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1][yi-1-i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有其他棋子，不符合規則

			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(飛);
			}
			else{
				restart = 1;
			}
		}

		//目標位置在棋子正右方
		if((xj==xi) && (yj>yi)){
			int diff = yj - yi;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1][yi-1+i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有其他棋子，則不符合規則

			if(isStandard && blueOrRed(xj,yj) != 1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = B(飛);
			}
			else{
				restart = 1;
			}
		}

	}

	//R(飛)，走十字
	else if(board[xi-1][yi-1] == R(飛))
	{
		
		//目標位置在棋子正上方
		if((xj<xi) && (yj==yi)){

			int diff = xi - xj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1-i][yi-1] != "　"){
					isStandard = 0;
				}	
			}//若路徑上有其他棋子，則不符合規則
			
			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(飛);
			}
			else{
				restart = 1;
			}

		}
			
		//目標位置在棋子正下方
		if((xj>xi) && (yj==yi)){

			int diff = xj - xi;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1+i][yi-1] != "　"){
					isStandard = 0;
				}
			}//若路徑上有其他棋子，則不符合規則

			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(飛);
			}
			else{
				restart = 1;
			}
		}

		//目標位置在棋子正左方
		if((xj==xi) && (yj<yi)){
			int diff = yi -yj;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1][yi-1-i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有其他棋子，不符合規則

			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(飛);
			}
			else{
				restart = 1;
			}
		}

		//目標位置在棋子正右方
		if((xj==xi) && (yj>yi)){
			int diff = yj - yi;
			for(int i=1 ; i<diff ; i++){
				if(board[xi-1][yi-1+i] != "　"){
					isStandard = 0;
				}
			}//若路徑上有其他棋子，則不符合規則

			if(isStandard && blueOrRed(xj,yj) != -1){
				push(xi,yi,xj,yj);
				board[xi-1][yi-1] = "　";
				board[xj-1][yj-1] = R(飛);
			}
			else{
				restart = 1;
			}
		}

	}
}



// // //計時
// static void stdin_cb (struct ev_loop *loop, ev_io *w, int revents)
// {
// 	ev_timer_stop(loop, &timeout_watcher1);
// 	ev_timer_stop(loop, &timeout_watcher2);
// 	game();
// 	if(turn == 1){
// 		ev_timer_stop(loop, &timeout_watcher2);
// 		ev_timer_start(loop, &timeout_watcher1);
// 	}
// 	else{
// 		ev_timer_stop(loop, &timeout_watcher1);
// 		ev_timer_start(loop, &timeout_watcher2);
// 	}
// }

// static void timeout1_cb (struct ev_loop *loop, ev_timer *w, int revents)
// {
// 	printf("Total time:%d, BLUE's time:%d \nFrom:\n",(int)ev_now(loop)-now,clk1); //印出計時，總計時與個別計時
// 	if(clk1 > 30){
// 		turn *= (-1);
// 		printf("Time is up, now it's RED's turn.\n"); //BLUE計時超過30秒，就換RED移動棋子
// 		clk1 = clk2 = 0; //把個別計時歸零
// 		ev_timer_stop(loop, &timeout_watcher1);
// 		ev_timer_start(loop, &timeout_watcher2);

// 	}
// 	clk1 = clk1 + 5; //以5秒為間隔印出時間
// }

// static void timeout2_cb (struct ev_loop *loop, ev_timer *w, int revents)
// {
// 	printf("Total time:%d, RED's time:%d \nFrom:\n",(int)ev_now(loop)-now,clk2);
// 	if(clk2 > 30){
// 		turn *= (-1);
// 		printf("Time is up, now it's BLUE's turn.\n"); //RED計時超過30秒，就換BLUE移動棋子
// 		clk1 = clk2 = 0;
// 		ev_timer_stop(loop, &timeout_watcher2);
// 		ev_timer_start(loop, &timeout_watcher1);
// 	}
// 	clk2 = clk2 + 5;
// }
// int main()
// {
	
// 	chessboardBuilding();
// 	record_board();
// 	printChessboard();
// 	printf("Blue's turn!\n"); //藍棋先下
// 	choose_option();
// 	int i=0;
// 	struct ev_loop *loop = EV_DEFAULT;
// 	now = ev_now(loop);

// 	ev_io_init(&stdin_watcher, stdin_cb, 0, EV_READ);
// 	ev_io_start(loop, &stdin_watcher);
// 	ev_timer_init(&timeout_watcher1, timeout1_cb, 0, 5); //以五秒為間隔，印出計時
// 	ev_timer_init(&timeout_watcher2, timeout2_cb, 0, 5); //以五秒為間隔，印出計時
// 	if(turn == 1){
// 		ev_timer_start(loop, &timeout_watcher1);
// 	}
// 	else{
// 		ev_timer_start(loop, &timeout_watcher2);
// 	}
// 	ev_timer_start(loop, &timeout_watcher1);
// 	ev_run(loop, 0);
// 	return 0;

// }