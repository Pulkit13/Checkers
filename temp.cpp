#include <iostream>
using namespace std;

class MyPoint
{
public:	
	int r;
	int c;
	MyPoint(){r=0;c=0;}
	void setPoint(int x, int y)  //can be accessed by list->p1.setPoint(coord1, coord2)
	{
		r=x;
		c=y;
	}
};

class list:public MyPoint  //each node in the linked list has 2 points (r1,c1) and (r2,c2)
{

public:
	
	list* next;
	MyPoint* p1;
	MyPoint* p2;

	list(){}
	list(int x1, int y1, int x2, int y2)
	{
		p1=new MyPoint;
		p2=new MyPoint;
		p1->setPoint(x1,y1);
		p2->setPoint(x2,y2);
	}
};


class CheckerBoard {
public:
	char board[8][8];
	char flag;
	int depth;
	int utility_value;
	
	CheckerBoard* child;
	CheckerBoard* next;
	
	list *moves;  //each game CheckerBoard has an associated linked list of possible jumps and moves, this is the head pointer of the move list
	list *jumps;   //head pointer of the jump linked list
	
	list* currentJump;
	list* currentMove;
	
	//constructor
	CheckerBoard(void);
	
	/*functions given in the spec*/
	void getAllJumps (void);
	void getAllMoves (void);
	void getForwardJumps (int, int);
	void getBackwardJumps (int, int);
	void getForwardMoves (int, int);
	void getBackwardMoves (int, int);
	void add(int, int, int, int, int);
};

CheckerBoard::CheckerBoard(void)
{
	moves=NULL;
	jumps=NULL;
	child=NULL;
	currentJump=NULL;
	currentMove=NULL;
	depth=0;
}

int hasGameEnded(int,int,int);
int MaxValue (CheckerBoard *, int, int, int);
int MinValue (CheckerBoard *, int, int, int);
int eval_function (CheckerBoard *);
void spaces (CheckerBoard *);
void Prune (list *);
void alpha_beta (int, int);
void readBoard(char a[][8]);
int findMax(int,int);
int findMin(int,int);

int main (int argc, char * const argv[]) {
	char input_board[8][8];
	int depth;
	CheckerBoard *start = new CheckerBoard;
	readBoard(input_board);  //read board as input by the user
	cin>>depth;	//read the depth
	
	//reading board into start state
	
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
			start->board[i][j]=input_board[i][j];
	
	for(int k=0;k<8;k++)
	{
		cout<<endl;
		for(int m=0;m<8;m++)
			printf("%c",start->board[k][m]);
	}
	cout<<endl<<"Depth ="<<depth<<endl;
	
	start->flag = 'A';
	
	MaxValue (start, -10000, 10000, depth);
	cout<<endl<<"Expansions Completed."<<endl;
	
	CheckerBoard* answer;
	CheckerBoard* ptr;
	ptr=start->child;
	int max_util=ptr->utility_value;
	answer=ptr;
	while(ptr!=NULL)
	{
		if(ptr->utility_value > max_util)
		{
		max_util=ptr->utility_value;
		answer=ptr;
		}
		ptr=ptr->next;
	}
	if(answer->currentJump!=NULL)
	printf("Answer : Player A moves piece from (%d,%d) to (%d,%d)\n",(answer->currentJump->p1->r)+1,(answer->currentJump->p1->c)+1,(answer->currentJump->p2->r)+1,(answer->currentJump->p2->c)+1);

	else
	printf("Answer : Player A moves piece from (%d,%d) to (%d,%d)\n",(answer->currentMove->p1->r)+1,(answer->currentMove->p1->c)+1,(answer->currentMove->p2->r)+1,(answer->currentMove->p2->c)+1);
	
	cout<<endl;
	return 0;
}

int MaxValue (CheckerBoard *x, int alpha, int beta, int depthlimit) {
	int r1, c1, r2, c2;
	int e=eval_function(x);
	if(hasGameEnded(x->depth,depthlimit,e))
	{
		x->utility_value = e;
		return e;
	}
	
	x->getAllJumps();
	if (x->jumps != NULL) {
		while (x->jumps != NULL) {
			r1 = x->jumps->p1->r;
			c1 = x->jumps->p1->c;
			r2 = x->jumps->p2->r;
			c2 = x->jumps->p2->c;
			CheckerBoard *t;
			//creating a new CheckerBoard t, resulting after executing the move
			//t is child of x, append t to x's list of CheckerBoards. x has CheckerBoard* child and CheckerBoard* next
			//t = xchild
			//append to x's child
			if(x->child==NULL)
			{
				t = new CheckerBoard;
				x->child=t;
				*t = *x;
				t->next=NULL;
			}
			else
			{
				t=x->child;
				CheckerBoard* t1;
				while(t!=NULL)	
				{
					t1=t;
					t=t->next;
				}
				t=new CheckerBoard;
				*t=*x;
				t->next=NULL;
				t1->next=t;
			}
			t->child=NULL;
			t->currentJump = x->jumps; 
			(t->depth)++;
			
			//advancing the jumps linked list head pointer(jumps) by one
			x->jumps = (x->jumps)->next;

			if (r2 == 0) {
				t->board[r2][c2] = 'k';
				t->board[r1][c1] = '.';
				t->board[(r1+r2)/2][(c1+c2)/2] = '.';
			} else {
				t->board[r2][c2] = t->board[r1][c1];
				t->board[r1][c1] = '.';
				t->board[(r1+r2)/2][(c1+c2)/2] = '.';
			}
			
			spaces(x);
			printf("Depth %d: Player A Jumps the piece at (%d,%d) to (%d,%d).\n", x->depth, r1+1, c1+1, r2+1, c2+1);
			
			t->flag = 'B';
			t->currentMove=NULL;
			t->jumps=NULL;
			t->moves=NULL;
			
			alpha = findMax (alpha, MinValue(t, alpha, beta, depthlimit));
			t->utility_value = alpha;

			if (alpha >= beta) {
				if (x->jumps != NULL) {
					spaces(x);
					printf("Depth %d: Pruning Player A's moves", x->depth);
					Prune(x->jumps);
					alpha_beta(alpha, beta);
				}
				return beta;
			}
		}
		return alpha;
	} else {
		x->getAllMoves();
		//we examine all possible moves in sequence as branches of our game tree
		while (x->moves != NULL) {
			
			r1 = x->moves->p1->r;
			r2 = x->moves->p2->r;
			c1 = x->moves->p1->c;
			c2 = x->moves->p2->c;
			
			CheckerBoard *t;
			//creating a new CheckerBoard t, resulting after executing the move
			//t is child of x, append t to x's list of CheckerBoards. x has CheckerBoard* child and CheckerBoard* next
			 //t = xchild
			//append to x's child
			if(x->child==NULL)
			{
				t = new CheckerBoard();
				x->child=t;
				*t = *x;
				t->next=NULL;
			}
			else
			{
				t=x->child;
				CheckerBoard* t1;
				while(t!=NULL)	
				{
					t1=t;
					t=t->next;
				}
				t=new CheckerBoard();
				*t=*x;
				t->next=NULL;
				t1->next=t;
			}
			t->child=NULL;
			t->currentMove = x->moves;
			(t->depth)++;
			
			x->moves = (x->moves)->next;
			
			if (r2 == 0) {
				t->board[r2][c2] = 'k';
				t->board[r1][c1] = '.';
			} else {
				//executing the jump;
				t->board[r2][c2] = t->board[r1][c1];
				t->board[r1][c1] = '.';
				//t->board[(r1+r2)/2][(c1+c2)/2] = '->';
			}		
			spaces(x);
			
			
			t->flag = 'B';
			t->currentJump=NULL;
			t->jumps=NULL;
			t->moves=NULL;
			
			printf("Depth %d: Player A moves the piece at (%d,%d) to (%d,%d).\n", x->depth, r1+1, c1+1, r2+1, c2+1);
			
			alpha = findMax (alpha, MinValue(t, alpha, beta, depthlimit));
			t->utility_value = alpha;
			if (alpha >= beta) {
				if (x->moves != NULL) {
					spaces(x);
					cout<<"Depth "<<x->depth<<": Pruning Player A's moves";
					Prune(x->moves);
					alpha_beta(alpha, beta);
				}
				return beta;
			}
		}
		return alpha;
	}	
}

int MinValue (CheckerBoard *x, int alpha, int beta, int depthlimit) {
	int r1, c1, r2, c2;
	int e=eval_function(x);
	if(hasGameEnded(x->depth,depthlimit,e))
	{
		x->utility_value = e;
		return e;
	}

	x->getAllJumps();
	if (x->jumps != NULL) {
		while (x->jumps != NULL) {
			r1 = x->jumps->p1->r;
			c1 = x->jumps->p1->c;
			r2 = x->jumps->p2->r;
			c2 = x->jumps->p2->c;
			
			CheckerBoard *t;
			//creating a new CheckerBoard t, resulting after executing the move
			//t is child of x, append t to x's list of CheckerBoards. x has CheckerBoard* child and CheckerBoard* next
			 //t = xchild
			//append to x's child
			if(x->child==NULL)
			{
				t = new CheckerBoard();
				x->child=t;
				*t = *x;
				t->next=NULL;
				t->child=NULL;
			}
			else
			{
				t=x->child;
				CheckerBoard* t1;
				while(t!=NULL)	
				{
					t1=t;
					t=t->next;
				}
				t=new CheckerBoard();
				t->child=NULL;
				*t=*x;
				t->next=NULL;
				t1->next=t;
			}
			
			//x->child = t;	
			t->child=NULL;		
			t->currentJump = x->jumps; 
			t->currentMove=NULL;
			(t->depth)++;
			x->jumps = (x->jumps)->next;
			
			if (r2 == 7) {
				t->board[r2][c2] = 'K';
				t->board[r1][c1] = '.';
				t->board[(r1+r2)/2][(c1+c2)/2] = '.';
			} else {
				t->board[r2][c2] = t->board[r1][c1];
				t->board[r1][c1] = '.';
				t->board[(r1+r2)/2][(c1+c2)/2] = '.';
			}
			spaces(x);
			
			t->flag = 'A';
			t->jumps = NULL;
			t->moves = NULL;
			
			printf("Depth %d: Player B Jumps the piece at (%d,%d) to (%d,%d).\n", x->depth, r1+1, c1+1, r2+1, c2+1);
			
			beta = findMin (beta, MaxValue(t, alpha, beta, depthlimit));
			t->utility_value = beta;
			
			if (beta <= alpha) {
				if (x->jumps != NULL) {
					spaces(x);
					cout<<"Depth "<<x->depth<<": Pruning Player B's moves";
					Prune (x->jumps);
					alpha_beta(alpha, beta);
				}
				return alpha;
			}
		}
		return beta;
	} else {
		x->getAllMoves();
		while (x->moves != NULL) {
			r1 = x->moves->p1->r;
			c1 = x->moves->p1->c;
			r2 = x->moves->p2->r;
			c2 = x->moves->p2->c;
			
			CheckerBoard *t;
			//creating a new CheckerBoard t, resulting after executing the move
			//t is child of x, append t to x's list of CheckerBoards. x has CheckerBoard* child and CheckerBoard* next
			 //t = xchild
			//append to x's child
			if(x->child==NULL)
			{
				t = new CheckerBoard();
				x->child=t;
				*t = *x;
				t->child=NULL;
				t->next=NULL;
			}
			else
			{
				t=x->child;
				CheckerBoard* t1;
				while(t!=NULL)	
				{
					t1=t;
					t=t->next;
				}
				t=new CheckerBoard();
				*t=*x;
				t->child=NULL;
				t->next=NULL;
				t1->next=t;
			}
			
			//x->child = t;
			t->currentMove = x->moves;
			(t->depth)++;
			x->moves = (x->moves)->next;
			
			if (r2 == 7) {
				t->board[r2][c2] = 'K';
				t->board[r1][c1] = '.';
			} else {
				t->board[r2][c2] = t->board[r1][c1];
				t->board[r1][c1] = '.';
			}
			spaces(x);
			printf("Depth %d: Player B moves the piece at (%d,%d) to (%d,%d).\n", x->depth, r1+1, c1+1, r2+1, c2+1);
			
			t->flag = 'A';
			t->jumps = NULL;
			t->moves = NULL;
			t->currentJump=NULL;
			
			beta = findMin (beta, MaxValue(t, alpha, beta, depthlimit));
			t->utility_value = beta;
			
			if (beta <= alpha) {
				if (x->moves != NULL) {
					spaces(x);
					cout<<"Depth "<<x->depth<<": Pruning Player B's moves";
					Prune (x->moves);
					alpha_beta(alpha, beta);
				}
				return alpha;
			}
		}
		return beta;
	}
}

void alpha_beta (int alpha, int beta) {
	
	
	if (alpha == -10000 && beta == 10000) {
		cout<<"; Alpha = -infinity; Beta = infinity."<<endl;
	} else if (alpha != -10000 && beta == 10000) {
		cout<<"; Alpha = "<<alpha<<"; Beta = infinity."<<endl;
	} else if (alpha == -10000 && beta != 10000) {
		cout<<"; Alpha = -infinity; Beta = "<<beta<<"."<<endl;
	} else {
		cout<<"; Alpha = "<<alpha<<"; Beta = "<<beta<<"."<<endl;
	}
}


					   
void spaces (CheckerBoard *t) {
	for(int i=0;i<t->depth;i++)
	printf("\t");
}
					   

void Prune (list *l) {
	while (l != NULL) {
		printf(", (%d,%d) to (%d,%d)", (l->p1->r) +1, (l->p1->c) +1, (l->p2->r) +1, (l->p2->c) + 1);
		l = l->next;
	}
}
					   
int eval_function (CheckerBoard *x) {
	int i=0, j=0;
	int sumA = 0, sumB = 0;
	CheckerBoard temp = *x;	//temp is just a copy, it doesn't modify x
	while(i <= 7) {
		j=0;
		while(j <= 7) {
			/*normal pieces*/
			if (temp.board[i][j] == 'o') {
				sumA++;
			}
			if (temp.board[i][j] == '*') {
				sumB++;
			}
			
			/* king pieces */
			if (temp.board[i][j] == 'k') {
				sumA += 2;
			}
			
			if (temp.board[i][j] == 'K') {
				sumB += 2;
			}
			j++;
		}
		i++;
	}
	
	temp.getAllJumps();
	temp.getAllMoves();
	
	if (sumA == 0 || sumB == 0 || (temp.moves == NULL && temp.jumps == NULL)) {
		switch(temp.flag)
		{
			case 'A':
				return -100;
				break;
				
			case 'B':
				return 100;
				break;
				
			default:
				break;
		}
		
	}
	
	return (sumA - sumB);	
}


void CheckerBoard::getForwardMoves (int x, int y) {
	if (!x) {
		return;
	}
	if (!y) {
		if (board[x - 1][y + 1] == '.') {
			add(x, y, x - 1, y + 1,0);
			return;
		}
	}
	if (y == 7) {
		if (board[x - 1][y - 1] == '.') {
			add(x, y, x - 1, y - 1,0);
			return;
		}
	}
	if (board[x - 1][y + 1] == '.') {
		add(x, y, x - 1, y + 1,0);
	}
	if (board[x - 1][y - 1] == '.') {
		add(x, y, x - 1, y - 1,0);
	}
}


void CheckerBoard::getBackwardMoves (int x, int y) {
	if (x == 7) {
		return;
	}
	if (!y) {
		if (board[x + 1][y + 1] == '.') {
			add(x, y, x + 1, y + 1,0);
			return;
		}
	}
	if (y == 7) {
		if (board[x + 1][y - 1] == '.') {
			add(x, y, x + 1, y - 1,0);
			return;
		}
	}
	if (board[x + 1][y + 1] == '.') {
		add(x, y, x + 1, y + 1,0);
	}
	if (board[x + 1][y - 1] == '.') {
		add(x, y, x + 1, y - 1,0);
	}
}

void CheckerBoard::getForwardJumps (int i, int j) {
	if (i == 0 || i == 1) {
		return;
	}
	
	char other_player, other_player_king;
	if (flag == 'A') {
		other_player = '*';
		other_player_king = 'K';
	} else {
		other_player = 'o';
		other_player_king = 'k';
	}

	if (j == 0 || j == 1) {
		if ((board[i - 1][j + 1] == other_player || board[i - 1][j + 1] == other_player_king))
		{
			if(board[i - 2][j + 2] == '.'){
			add(i, j, i - 2, j + 2,1);
			return;}
		}
	}
	if (j == 6 || j == 7) {
		if ((board[i - 1][j - 1] == other_player || board[i - 1][j - 1] == other_player_king)) {
			if(board[i - 2][j - 2] == '.'){
			add (i, j, i - 2, j - 2,1);
			return;
			}
		}
	}
	if ((board[i - 1][j + 1] ==  other_player || board[i - 1][j + 1] == other_player_king) && board[i - 2][j + 2] == '.') {
		add(i, j, i - 2, j + 2,1);
	}
	if ((board[i - 1][j - 1] == other_player || board[i - 1][j - 1] == other_player_king) && board[i - 2][j - 2] == '.') {
		add(i, j, i - 2, j - 2,1);
	}
}


void CheckerBoard::getBackwardJumps (int i, int j) {
	if (i == 6 || i == 7) {
		return;
	}
	char other_player;
	char other_player_king;
	if (flag == 'A') {
		other_player = '*';
		other_player_king = 'K';
	} else {
		other_player = 'o';
		other_player_king = 'k';
	}
	
	if (j == 0 || j == 1) {
		if ((board[i + 1][j + 1] == other_player || board[i + 1][j + 1] == other_player_king) ) {
			if(board[i + 2][j + 2] == '.') 
			{
			add(i, j, i + 2, j + 2,1);
			return;
			}
		}
	}
	if (j == 6 || j == 7) {
		if ((board[i + 1][j - 1] == other_player || board[i + 1][j - 1] == other_player_king) ) {
			if(board[i + 2][j - 2] == '.') {
			add (i, j, i + 2, j - 2,1);
			return;
			}
		}
	}
	if ((board[i + 2][j + 2] == '.') && (board[i + 1][j + 1] == other_player || board[i + 1][j + 1] == other_player_king)) {
		add(i, j, i + 2, j + 2,1);
	}
	if ((board[i + 2][j - 2] == '.') && (board[i + 1][j - 1] == other_player || board[i + 1][j - 1] == other_player_king)) {
		add(i, j, i + 2, j - 2,1);
	}
}


void CheckerBoard::getAllMoves () {
	int i=0, j=0;
	switch(flag)
	{
		case 'A':
			while(i<8)
			{
				j=0;	
				while(j<8)
				{
					if (board[i][j] == 'o') {
						getForwardMoves(i, j);
					}
					if (board[i][j] == 'k') {
						getForwardMoves(i, j);
						getBackwardMoves(i, j);
					}
					j++;
				}
				i++;
			}
			break;
			
		case 'B':
			while(i<8)
			{
				j=0;
				while(j<8)
				{
					if (board[i][j] == '*') {
						getBackwardMoves(i, j);
					}
					if (board[i][j] == 'K') {
						getForwardMoves(i, j);
						getBackwardMoves(i, j);
					}
					j++;
				}
				i++;
			}
			break;
			
		default:
			break;
	}
	
}

void CheckerBoard::getAllJumps () {
	int i=0, j=0;
	
	switch(flag)
	{
		case 'A':
			while(i<8)
			{
				j=0;
				while(j<8)
				{
					if (board[i][j] == 'o') {
						getForwardJumps(i, j);
					}
					if (board[i][j] == 'k') {
						getForwardJumps(i, j);
						getBackwardJumps(i, j);
					}
					j++;
				}
			
				i++;
			}
			break;
	
		case 'B':
			while(i<8)
			{
				j=0;
				while(j<8)
				{
					if (board[i][j] == '*') {
						getBackwardJumps(i, j);
					}
					if (board[i][j] == 'K') {
						getForwardJumps(i, j);
						getBackwardJumps(i, j);
					}
					j++;
				}
				i++;
			}
			break;
			
		default:
			break;
		
	}
}


void CheckerBoard::add(int r1, int c1, int r2, int c2, int flag) {
	list *j = new list(r1,c1,r2,c2);
	j->next = NULL;
	if(flag==0)	//move
	{
		if (moves == NULL) {
		moves = j;
		} else {
		j->next = moves;
		moves = j;
		}
	}
	else if(flag==1) //jump
	{
	if (jumps == NULL) {
		jumps = j;
	} else {
		j->next = jumps;
		jumps = j;
	}
	}
}

void readBoard(char board[][8])
{
	char ch;
	int i,j;
	for(i=0;i<8;i++)
	{
		j=0;
		while((ch=getchar())!='\n')
		{
			board[i][j]=ch;
			j++;
		}
	}
	
}

int hasGameEnded(int a, int b, int e)
{
	if (a == b || e == 100 || e == -100) {
		while(a!=0) { printf("\t"); a--; }
		printf("Depth %d: Heuristic value of the current board = ", a);
		switch(e)
		{
		case 100:
			printf("infinity\n");
			break;
			
		case -100:
			printf("-infinity\n");
			break;
			
		default:
			printf("%d\n",e);
			break;
		}
		return 1;
	}
	return 0;
}

int findMax(int a,int b)
{
	int x;
	x= (a>b)?a:b;
	return x;
}

int findMin(int a,int b)
{
	int x;
	x=(a>b)?b:a;
	return x;
}