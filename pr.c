#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
/* On the web, read a number asynchronously from the browser terminal */
EM_ASYNC_JS(int, web_read_int, (), {
    const line = await Module.readLine();
    const n = parseInt(line, 10);
    return (line === null || String(line).trim() === "" || Number.isNaN(n)) ? -999999 : n;
});
#endif

#define C_row 6
#define C_col 7
#define C_max_players 2

int dropPiece(int board[C_row][C_col], int col, int piece)
{
    if (col < 1 || col > C_col)
        return -1;
    for (int y = 0; y < C_row; y++)
        if (board[y][col - 1] == 0)
            return board[y][col - 1] = piece;
    return -1;
}

void initBoard(int board[C_row][C_col])
{
    for (int x = 0; x < C_row; x++)
        for (int y = 0; y < C_col; y++)
            board[x][y] = 0;
}

int checkWinners(int s[C_row][C_col])
{
    int i, j;
    for (int p = 1; p <= C_max_players; p++)
    {
        for (i = 0; i < C_row; i++)
            for (j = 0; j <= C_col - 4; j++)
                if (s[i][j] == p && s[i][j+1] == p && s[i][j+2] == p && s[i][j+3] == p) return p;
        for (i = 0; i <= C_row - 4; i++)
            for (j = 0; j < C_col; j++)
                if (s[i][j] == p && s[i+1][j] == p && s[i+2][j] == p && s[i+3][j] == p) return p;
        for (i = 3; i < C_row; i++)
            for (j = 0; j <= C_col - 4; j++)
                if (s[i][j] == p && s[i-1][j+1] == p && s[i-2][j+2] == p && s[i-3][j+3] == p) return p;
        for (i = 0; i <= C_row - 4; i++)
            for (j = 0; j <= C_col - 4; j++)
                if (s[i][j] == p && s[i+1][j+1] == p && s[i+2][j+2] == p && s[i+3][j+3] == p) return p;
    }
    for (i = 0; i < C_col; i++)
        if (s[C_row - 1][i] == 0) return 0;
    return -1;
}

void displayBoard(int board[][C_col])
{
    int i, j, k, m;
    const char vert_line = '|';
    const char horz_line_segment[] = "---+";   /* FIX: was [4], no room for '\0' */
    const char horz_line_segment_prefix = '+';
    const char playerColor1[] = "\033[30;43m";
    const char playerColor2[] = "\033[30;41m";
    const char resetColor[] = "\033[0m";

    printf("\033[2J\033[H");   /* clear screen (works in terminal AND browser) */

    printf("\n ");
    for (i = 0; i < C_col; i++)
        printf(" %d  ", i + 1);
    printf("\n");

    for (i = C_row - 1; i >= 0; i--)
    {
        printf("%c", vert_line);
        for (j = 0; j < C_col; j++)
        {
            switch (board[i][j])
            {
            case 1: printf(" %s%d%s %c", playerColor1, board[i][j], resetColor, vert_line); break;
            case 2: printf(" %s%d%s %c", playerColor2, board[i][j], resetColor, vert_line); break;
            default: printf(" %d %c", board[i][j], vert_line);
            }
        }
        printf("\n%c", horz_line_segment_prefix);
        for (k = 0; k < C_col; k++)
            printf("%s", horz_line_segment);
        printf("\n");
    }

    for (m = 1; m <= C_col; m++)
        (m > 9) ? printf(" ") : printf("  ");
    printf("\n");
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);   /* flush output immediately */
    int board[C_row][C_col];
    int player = 1;

    initBoard(board);
    displayBoard(board);

    while (1)
    {
        int possible = -1;
        int pos;

        do
        {
            printf("Player %d - Drop Piece (1-7):\n", player);   /* newline so it shows before input */
#ifdef __EMSCRIPTEN__
            pos = web_read_int();
            if (pos == -999999) { possible = -1; continue; }
#else
            if (scanf("%d", &pos) != 1) { while (getchar() != '\n'); possible = -1; continue; }
#endif
            possible = dropPiece(board, pos, player);
            if (possible == -1)
                printf("Invalid move! Try another column.\n");
        } while (possible == -1);

        displayBoard(board);

        int gameover = checkWinners(board);
        if (gameover != 0)
        {
            if (gameover == -1) printf("Nobody won. Sad :(\n");
            else if (gameover == 1) printf("Yellow Player Won!\n");
            else if (gameover == 2) printf("Red Player Won!\n");
            break;
        }
        player = (player % C_max_players) + 1;
    }
    return 0;
}
