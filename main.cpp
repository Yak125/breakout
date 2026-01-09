# include "raylib.h"


enum GameState {
    MENU,       // = 0 automatically
    PLAYING,    // = 1
    PAUSED,     // = 2
    GAMEOVER,   // = 3
    WIN         // = 4
};


// ============ FUNCTIONS ============

int main() {

    // SCREEN
    constexpr int screenWidth { 800} ;
    constexpr int screenHeight { 600 };
    InitWindow(screenWidth, screenHeight, "BrickBreaker");

    // BRICKS
    constexpr int ROWS { 5 };
    constexpr int COLS { 20 };

    bool brickActive[ROWS][COLS];
    for (int i {0}; i < ROWS; ++i) {
        for (int j {0}; j < COLS; ++j) {
            brickActive[i][j] = true;
        }
    }
    constexpr float brickWidth { 2 * screenWidth / COLS };                    // 80
    constexpr float brickHeight { screenWidth / COLS };

    // PADDLE 
    constexpr float paddleWidth { 2 * brickWidth };                           // 160
    constexpr float paddleHeight { brickHeight / 2 };
    float paddleXpos { (screenWidth - paddleWidth) / 2 };
    constexpr float paddleYpos { screenHeight - paddleHeight };
    constexpr int paddleSpeed { 300 };
    Rectangle paddle { paddleXpos, paddleYpos, paddleWidth, paddleHeight };

    // BALL
    constexpr float ballRadius { 10.0f };
    Vector2 ballPos { screenWidth / 2.0f, screenHeight / 2.0f };
    Vector2 ballSpeed { 0, 400 };



    constexpr float maxoffset {paddleWidth / 2};
    constexpr float maxPaddleInfluence {300};


    GameState currentState { MENU };

    SetTargetFPS(60);

    while( !WindowShouldClose() ) {

        float deltaTime = GetFrameTime();

        // ***NON GAME USER INPUT PHASE***
        if (IsKeyPressed(KEY_R)) {

            // reset bricks
            for (int i {0}; i < ROWS; ++i) {
                for (int j {0}; j < COLS; ++j) {
                    brickActive[i][j] = true;
                }
            }

            ballPos = { screenWidth / 2.0f, screenHeight / 2.0f };
            ballSpeed = { 0, 400 };

            paddle.x = (screenWidth - paddleWidth) / 2;

        }
        // ***POSITION UPDATE PHASE***

        // PADDLE MOVEMENT
        if (IsKeyDown(KEY_D)) paddle.x += paddleSpeed * deltaTime;
        if (IsKeyDown(KEY_A)) paddle.x -= paddleSpeed * deltaTime;

        // BALL MOVEMENT
        ballPos.x += ballSpeed.x * deltaTime;
        ballPos.y += ballSpeed.y * deltaTime;


        // ***COLLISION CHECK PHASE***

        // BALL / PADDLE COLLISION
        if ( CheckCollisionCircleRec(ballPos, ballRadius, paddle) ) {
            // 1. flip vertical ball speed
            ballSpeed.y *= -1;

            // 2. find paddle inluence on horizontal ball speed
            float paddleCenter = paddle.x + paddleWidth / 2.0f;
            float offset = ballPos.x - paddleCenter;
            float paddleInfluence = (offset / maxoffset) * maxPaddleInfluence;

            // 3. adjust horizontal ball speed using combination of initial horizontal speed and paddle influence
            ballSpeed.x = ballSpeed.x * 0.5f + paddleInfluence * 0.5f;
        }

        // BALL / BRICK COLLISION
        bool brickHit { false };
        for ( int i { 0 }; i < ROWS; ++i ) {
            for ( int j { 0 }; j < COLS; ++j ) {
                if (brickActive[i][j] && !brickHit) {
                    Rectangle currentBrick { j * brickWidth, i * brickHeight, brickWidth, brickHeight };
                    if ( CheckCollisionCircleRec(ballPos, ballRadius, currentBrick) ) {
                        // 1. destroy brick
                        brickActive[i][j] = false; // destroy the brick

                        // 2. find side with smallest overlap
                        float overlapLeft = (ballPos.x + ballRadius) - currentBrick.x;
                        float overlapRight = (currentBrick.x + brickWidth) - (ballPos.x - ballRadius);
                        float overlapTop = (ballPos.y + ballRadius) - currentBrick.y;
                        float overlapBottom = (currentBrick.y + brickHeight) - (ballPos.y - ballRadius);

                        float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
                        float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;
                        
                        // 3. change ball direction based on brick side hit (side with smallest overlap)
                        if (minOverlapX < minOverlapY) {
                            ballSpeed.x *= -1;
                        } else {
                            ballSpeed.y *= -1;
                        }

                        // 4. skip all future brick checks
                        brickHit = true;
                    }
                }
            }
        }

        // BALL / SCREEN LEFT OR RIGHT SIDE COLLISION
        if (ballPos.x >= screenWidth || ballPos.x <= 0) {
            ballSpeed.x *= -1;
        }


        // ***DRAW PHASE***
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(paddle, GREEN);

        DrawCircleV(ballPos, ballRadius, BLACK);


        for ( int i {0}; i < ROWS; ++i ) {
            for ( int j {0}; j < COLS; ++j ) {
                if (brickActive[i][j]) {
                    float brickxpos {brickWidth * j};
                    float brickypos {brickHeight * i};
                    DrawRectangle(brickxpos, brickypos, brickWidth, brickHeight, GRAY); 
                    DrawRectangleLines(brickxpos, brickypos, brickWidth, brickHeight, DARKGRAY);
                }
            }
        }



        EndDrawing();


    }

    CloseWindow();
    return 0;
}