#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define SUDOKU_SIZE 9
#define SNAKE_BOARD_SIZE 100
#define NUM_PLAYERS 4

/* ----------------- Leaderboard Structure ----------------- */
typedef struct {
    char name[50];
    int score;
    double time_taken; // in seconds
} Leaderboard;

/* ----------------- Utility Functions ----------------- */
void show_rules(int choice) {
    if (choice == 1)
        printf("\n--- Sudoku Rules ---\nFill the 9x9 grid so that each row, column, and 3x3 box contains numbers 1–9.\n");
    else
        printf("\n--- Snake & Ladder Rules ---\nRoll dice, move forward. Snakes pull you down, ladders push you up. First to 100 wins.\n");
}

void save_leaderboard(const char *filename, Leaderboard lb) {
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        printf("Error saving leaderboard!\n");
        return;
    }
    fprintf(fp, "%s %d %.2f\n", lb.name, lb.score, lb.time_taken);
    fclose(fp);
}

void show_leaderboard(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("\nNo leaderboard records found!\n");
        return;
    }

    printf("\n--- Leaderboard ---\n");
    Leaderboard lb;
    while (fscanf(fp, "%s %d %lf", lb.name, &lb.score, &lb.time_taken) == 3) {
        printf("Player: %-10s | Score: %-3d | Time: %.2fs\n", lb.name, lb.score, lb.time_taken);
    }
    fclose(fp);
}

/* ----------------- Sudoku Functions ----------------- */
void displaySudoku(int board[SUDOKU_SIZE][SUDOKU_SIZE]) {
    printf("\n+-------+-------+-------+\n");
    for (int r = 0; r < SUDOKU_SIZE; r++) {
        printf("| ");
        for (int c = 0; c < SUDOKU_SIZE; c++) {
            if (board[r][c] == 0) printf(". ");
            else printf("%d ", board[r][c]);
            if ((c+1)%3==0) printf("| ");
        }
        printf("\n");
        if ((r+1)%3==0) printf("+-------+-------+-------+\n");
    }
}

bool canPlace(int board[SUDOKU_SIZE][SUDOKU_SIZE], int row, int col, int num) {
    for(int i=0;i<SUDOKU_SIZE;i++){
        if(board[row][i]==num || board[i][col]==num) return false;
    }
    int startRow=row-row%3, startCol=col-col%3;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(board[startRow+i][startCol+j]==num) return false;
    return true;
}

void sudoku_game() {
    int board[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5,3,0, 0,7,0, 0,0,0},
        {6,0,0, 1,9,5, 0,0,0},
        {0,9,8, 0,0,0, 0,6,0},
        {8,0,0, 0,6,0, 0,0,3},
        {4,0,0, 8,0,3, 0,0,1},
        {7,0,0, 0,2,0, 0,0,6},
        {0,6,0, 0,0,0, 2,8,0},
        {0,0,0, 4,1,9, 0,0,5},
        {0,0,0, 0,8,0, 0,7,9}
    };

    int choice;
    char name[50];
    time_t start, end;
    start = time(NULL);

    while(1){
        displaySudoku(board);
        printf("\n1. Enter number\n2. Exit Sudoku\nChoose: ");
        scanf("%d",&choice);
        if(choice==1){
            int r,c,num;
            printf("Enter row(1-9), col(1-9), number(1-9): ");
            scanf("%d %d %d",&r,&c,&num);
            r--;c--;
            if(r<0||r>=SUDOKU_SIZE||c<0||c>=SUDOKU_SIZE||num<1||num>9){
                printf("Invalid input!\n");
                continue;
            }
            if(board[r][c]!=0){
                printf("Cell already filled!\n");
                continue;
            }
            if(canPlace(board,r,c,num)){
                board[r][c]=num;
                printf("Placed %d at (%d,%d)\n",num,r+1,c+1);
            } else {
                printf("Cannot place %d at (%d,%d)\n",num,r+1,c+1);
            }
        } else break;
    }

    end = time(NULL);
    double time_taken = difftime(end,start);
    printf("Enter your name for leaderboard: ");
    scanf("%s",name);
    Leaderboard lb;
    strcpy(lb.name,name);
    lb.score = 100; // Dummy score
    lb.time_taken = time_taken;
    save_leaderboard("sudoku_leaderboard.txt", lb);
}

/* ----------------- Snake & Ladder Functions ----------------- */
struct Player {
    int id;
    int position;
    struct Player *next;
};

struct Player* createPlayer(int id){
    struct Player* p = (struct Player*)malloc(sizeof(struct Player));
    p->id=id;
    p->position=0;
    p->next=NULL;
    return p;
}

void snake_game(){
    int board[SNAKE_BOARD_SIZE+1]={0};
    int dice,winner=-1;
    board[3]=22; board[5]=8; board[11]=26; board[20]=29; // ladders
    board[27]=1; board[21]=9; board[17]=4; board[19]=7; board[95]=24; board[99]=78; // snakes

    srand(time(0));

    struct Player *head=NULL,*tail=NULL;
    for(int i=1;i<=NUM_PLAYERS;i++){
        struct Player *p=createPlayer(i);
        if(!head) head=tail=p;
        else { tail->next=p; tail=p; }
    }
    tail->next=head;

    struct Player *current=head;
    char name[50];
    time_t start,end;
    start = time(NULL);

    while(winner==-1){
        printf("Player %d's turn. Press Enter to roll dice...",current->id);
        getchar(); getchar(); // wait for input
        dice = rand()%6+1;
        printf("Player %d rolled %d\n",current->id,dice);
        current->position += dice;
        if(current->position>100) current->position-=dice;
        else if(board[current->position]!=0) {
            if(board[current->position]>current->position)
                printf("Ladder! Player %d climbs from %d to %d\n",current->id,current->position,board[current->position]);
            else
                printf("Snake! Player %d slides from %d to %d\n",current->id,current->position,board[current->position]);
            current->position=board[current->position];
        }
        printf("Player %d is at position %d\n\n",current->id,current->position);
        if(current->position==100) winner=current->id;
        current=current->next;
    }

    end=time(NULL);
    double time_taken = difftime(end,start);
    printf("Player %d wins!\n",winner);
    printf("Enter your name for leaderboard: ");
    scanf("%s",name);
    Leaderboard lb;
    strcpy(lb.name,name);
    lb.score=150; // dummy score
    lb.time_taken = time_taken;
    save_leaderboard("snake_leaderboard.txt",lb);
}

/* ----------------- Main Menu ----------------- */
int main(){
    int choice;
    while(1){
        printf("\n=== MAIN MENU ===\n");
        printf("1. Play Sudoku\n");
        printf("2. Play Snake & Ladder\n");
        printf("3. Show Rules\n");
        printf("4. Show Leaderboard\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d",&choice);

        if(choice==1) sudoku_game();
        else if(choice==2) snake_game();
        else if(choice==3){
            int g;
            printf("1=Sudoku, 2=Snake & Ladder: ");
            scanf("%d",&g);
            show_rules(g);
        }
        else if(choice==4){
            int g;
            printf("Leaderboard for (1=Sudoku, 2=Snake & Ladder): ");
            scanf("%d",&g);
            if(g==1) show_leaderboard("sudoku_leaderboard.txt");
            else show_leaderboard("snake_leaderboard.txt");
        }
        else if(choice==5){
            printf("Goodbye!\n");
            break;
        }
        else printf("Invalid choice!\n");
    }
    return 0;
}