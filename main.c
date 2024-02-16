#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

#define CELL_SIZE 20
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MAX_SNAKE_LENGTH 100
#define WIDTH_IN_CELLS (SCREEN_WIDTH / CELL_SIZE)
#define HEIGHT_IN_CELLS (SCREEN_HEIGHT / CELL_SIZE)
#define INITIAL_SNAKE_LENGTH 3

static int board[HEIGHT_IN_CELLS][WIDTH_IN_CELLS];

static uint32_t score = 0;
static uint8_t foodCount = 0;

typedef struct SnakeCell SnakeCell;

typedef struct SnakeCell {
    uint32_t cell_x;
    uint32_t cell_y;
    SnakeCell* head;
    SnakeCell* tail;
} SnakeCell;

typedef struct {
    SnakeCell* head;
    SnakeCell* tail;
    uint32_t length;
    Vector2 forwardDirection;
} Snake;

static Snake* snake;


static void PlaceFood(int board[HEIGHT_IN_CELLS][WIDTH_IN_CELLS]) {
    // Clear the board of any food
    for(int i = 0; i < HEIGHT_IN_CELLS; i++) {
        for(int j = 0; j < WIDTH_IN_CELLS; j++) {
            if(board[i][j] == 1) {
                board[i][j] = 0;
            }
        }
    }
    
    // Place food on the board
    int x = GetRandomValue(0, WIDTH_IN_CELLS - 1);
    int y = GetRandomValue(0, HEIGHT_IN_CELLS - 1);
    board[y][x] = 1;
}

static void init() {

    score = 0;
    foodCount = 0;

    // Initialise 2d array to store the game board
    for(int i = 0; i < HEIGHT_IN_CELLS; i++) {
        for(int j = 0; j < WIDTH_IN_CELLS; j++) {
            board[i][j] = 0;
        }
    }

    // Initialise the snake
    SnakeCell* head = (SnakeCell*)malloc(sizeof(SnakeCell));
    head->cell_x = WIDTH_IN_CELLS / 2;
    head->cell_y = HEIGHT_IN_CELLS / 2;
    head->head = NULL;
    head->tail = NULL;

    snake = (Snake*)malloc(sizeof(Snake));
    snake->head = head;
    snake->tail = head;
    snake->length = 1;
    snake->forwardDirection = (Vector2){1, 0};

    // Initialize the body of the snake
    for(int i = 1; i < INITIAL_SNAKE_LENGTH; i++) {
        SnakeCell* newCell = (SnakeCell*)malloc(sizeof(SnakeCell));
        newCell->cell_x = head->cell_x - 1;
        newCell->cell_y = head->cell_y;
        newCell->tail = NULL;
        newCell->head = head;

        head->tail = newCell;
        head = newCell;
        snake->length++;
    }

    PlaceFood(board);
}

static void RestartGame() {

    // free the snake from memory
    SnakeCell* current = snake->head;
    while(current != NULL) {
        SnakeCell* temp = current;
        current = current->tail;
        free(temp);
    }

    init();
}

static void HandleInput() {
    if(IsKeyPressed(KEY_UP) && snake->forwardDirection.y != 1) {
        snake->forwardDirection = (Vector2){0, -1};
    } else if(IsKeyPressed(KEY_DOWN) && snake->forwardDirection.y != -1) {
        snake->forwardDirection = (Vector2){0, 1};
    } else if(IsKeyPressed(KEY_LEFT) && snake->forwardDirection.x != 1) {
        snake->forwardDirection = (Vector2){-1, 0};
    } else if(IsKeyPressed(KEY_RIGHT) && snake->forwardDirection.x != -1) {
        snake->forwardDirection = (Vector2){1, 0};
    }
}

static void Update() {
    // Update the snake's position
    SnakeCell* current = snake->head;
    uint32_t prev_x = current->cell_x;
    uint32_t prev_y = current->cell_y;
    while(current != NULL) {
        // if the head of the snake has no head (i.e. it's the only cell in the snake), move it in the forward direction
        if(current->head == NULL) {
            if(current->cell_x == 0 && snake->forwardDirection.x < 0) {
                current->cell_x = WIDTH_IN_CELLS;
            } else if(current->cell_x == WIDTH_IN_CELLS) {
                current->cell_x = -1;
            }

            if(current->cell_y == 0 && snake->forwardDirection.y < 0) {
                current->cell_y = HEIGHT_IN_CELLS;
            } else if(current->cell_y >= HEIGHT_IN_CELLS) {
                current->cell_y = -1;
            }

            current->cell_x += snake->forwardDirection.x;
            current->cell_y += snake->forwardDirection.y;
        } else {
            // if the current cell has a head, move it to the position of the head
            uint32_t temp_x = current->cell_x;
            uint32_t temp_y = current->cell_y;
            current->cell_x = prev_x;
            current->cell_y = prev_y;
            prev_x = temp_x;
            prev_y = temp_y;

            //if we are at the tail of the snake, we want to grow by the amount of food we have eaten
            if(current->tail == NULL & foodCount > 0) {
                SnakeCell* newCell = (SnakeCell*)malloc(sizeof(SnakeCell));
                newCell->cell_x = prev_x;
                newCell->cell_y = prev_y;
                newCell->tail = NULL;
                newCell->head = current;
                current->tail = newCell;
                snake->length++;
                foodCount--;
            }
        }

        current = current->tail;
    }

    //Check if snake head is on food
    if(board[snake->head->cell_y][snake->head->cell_x] == 1) {
        // Increase the score
        score++;
        foodCount++;

        // Place new food
        PlaceFood(board);
    }

    // Check if snake head is on snake body
    current = snake->head->tail;
    while(current != NULL) {
        if(snake->head->cell_x == current->cell_x && snake->head->cell_y == current->cell_y) {
            RestartGame();
            break;
        }
        current = current->tail;
    }
    
}

static void Draw() {
    // Draw the game board
    for(int i = 0; i < HEIGHT_IN_CELLS; i++) {
        for(int j = 0; j < WIDTH_IN_CELLS; j++) {
            if(board[i][j] == 1) {
                DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
            } else {
                DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK);
            }
        }
    }

    // Draw the snake
    SnakeCell* current = snake->head;
    while(current != NULL) {
        DrawRectangle(current->cell_x * CELL_SIZE, current->cell_y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
        current = current->tail;
    }
}

int main (int argc, char* argvp[]) {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");

    SetTargetFPS(15);

    init();

    // Main game loop
    while (!WindowShouldClose()) 
    {
        BeginDrawing();
                HandleInput();
                Update();    
                Draw();
                DrawText(TextFormat("Score: %d", score), 10, 10, 20, LIGHTGRAY);
                printf("Snake head position: %d, %d\n", snake->head->cell_x, snake->head->cell_y);
                printf("Snake length: %d\n", snake->length);
                printf("Food count: %d\n", foodCount);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}