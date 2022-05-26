#include <iostream>

#include "raylib.h"

struct AnimData
{
    Rectangle rect;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
    bool shouldBeRendered{true};
    float height{125};
    float width{125};

    void ChangeRenderer(bool visible)
    {
        shouldBeRendered = visible;
    }
};

bool IsOnGround(AnimData player, int windowHeight)
{
    return player.pos.y >= windowHeight - player.rect.height;
}

AnimData UpdateAnimData(AnimData data, float deltaTime, int maxFrame, bool render = false)
{
    //Updating running time (animation time)
    data.runningTime += deltaTime;
    //Update Animation Frame (animData)
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0;
        data.rect.x = data.frame * data.rect.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    data.shouldBeRendered = render;
    return data;
}


int main(int argc, char* argv[])
{
    constexpr int windowDimensions[2]{512,380};
    
    float velocity{0};

    //Jump velocity (pixels/second) per second
    const float jumpVel{-1000};
    
    InitWindow(windowDimensions[0],windowDimensions[1],"Dapper dasher");

    //Acceleration due to gravity (pixels per second) per second
    const int gravity{2000};

    //Nebula setup
    Texture2D nebula {LoadTexture("textures/12_nebula_spritesheet.png")};
    //Animdata for neb

    const int sizeOfNebula {10};

    const int sizeOfCoins{10};

    AnimData nebulae[sizeOfNebula]{};
    AnimData coins[sizeOfCoins]{};

    AnimData* currentCoins[sizeOfCoins];

    const float spaceBetweenNebula {300};
    Texture2D goldCoin = LoadTexture("textures/Empty-Gold-Coin-Transparent.png");

    float timer {0};


    for(int i = 0; i < sizeOfNebula; i++)
    {
        nebulae[i].rect = {0,0,(float)nebula.width / 8,(float)nebula.height / 8};
        nebulae[i].pos.y = {windowDimensions[1] - (float)nebula.height / 8};
        nebulae[i].pos.x = windowDimensions[0] + spaceBetweenNebula * i;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0;
        nebulae[i].updateTime = 0;
        coins[i].rect = {0,0, static_cast<float>(goldCoin.width), static_cast<float>(goldCoin.height)};
        coins[i].pos.x -= windowDimensions[0] + 75 + spaceBetweenNebula * i;
        coins[i].pos.y = -50;
    }

    for (int i = 0; i < sizeOfCoins; i++)
    {
        currentCoins[i] = &coins[i];
    }

    float finishLine{nebulae[sizeOfNebula - 1].pos.x};
    
    //Nebula X velocity (pixels/second)
    float nebulaVelocity{-200};

    //Scarfy setup
    Texture2D scarfy {LoadTexture("textures/scarfy.png")};
    AnimData scarfyData{};
    scarfyData.rect.width = scarfy.width / 6;
    scarfyData.rect.height = scarfy.height;
    scarfyData.rect.x = 0;
    scarfyData.rect.y = 0;
    scarfyData.pos.x = windowDimensions[0] / 2 - scarfyData.rect.width / 2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rect.height;
    scarfyData.frame = 0;

    scarfyData.updateTime = 1.0 / 12.0;
    scarfyData.runningTime = 0;

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgPosX{0};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgPosX{0};
    Texture2D backbuildings = LoadTexture("textures/back-buildings.png");
    float bbPosX{0};

    
    bool collision{false};

    bool hasPickedCoin{false};

    int score{0};
    
    //SetTargetFPS(60);
    
    
    
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        
        const float deltaTime{GetFrameTime()};
        
        bgPosX -= 20 * deltaTime;

        bbPosX -= 40 * deltaTime;

        fgPosX -= 80 * deltaTime;

        //Scroll background
        if(bgPosX <= -background.width * 2)
        {
            bgPosX = 0;
        }
        //Scroll back buildings
        if(bbPosX <= -backbuildings.width*2)
        {
            bbPosX = 0;
        }
        //Scroll foreground
        if(fgPosX <= -foreground.width*2)
        {
            fgPosX = 0;
        }
        

        //BACKGROUND POS
        Vector2 bg1Pos{bgPosX,0};
        Vector2 bg2Pos{bgPosX + background.width * 2,0};
        //FOREGROUND POS
        Vector2 fgPos{fgPosX,0};
        Vector2 fg2Pos{fgPosX + foreground.width * 2,0};
        //BACK BUILDING POS
        Vector2 mgPos{bbPosX,0};
        Vector2 mg2Pos{bbPosX + backbuildings.width * 2,0};

        //DRAW BACKGROUND
        DrawTextureEx(background,bg1Pos,0,2,WHITE);
        DrawTextureEx(background,bg2Pos,0,2,WHITE);
        //DRAW BACK BUILDINGS
        DrawTextureEx(backbuildings,mgPos,0,2,WHITE);
        DrawTextureEx(backbuildings,mg2Pos,0,2,WHITE);
        //DRAW FOREGROUND
        DrawTextureEx(foreground,fgPos,0,2,WHITE);
        DrawTextureEx(foreground,fg2Pos,0,2,WHITE);
        
        //START GAME LOGIC
        
        //Ground check
        if(IsOnGround(scarfyData, windowDimensions[1]))
        {
            velocity = 0;
        }
        else
        {
            velocity += gravity * deltaTime;
        }

        //Check if on ground
        if(IsOnGround(scarfyData, windowDimensions[1]))
        {
            scarfyData = UpdateAnimData(scarfyData, deltaTime, 5);
        }

        //Updating nebula animation frame
        for (int i = 0; i < sizeOfNebula; i++)
        {
            nebulae[i] = UpdateAnimData(nebulae[i], deltaTime, 7);
        }
        
        if(IsKeyDown(KEY_SPACE) && IsOnGround(scarfyData, windowDimensions[1]))
        {
            velocity += jumpVel;
        }


        for (int i = 0; i < sizeOfNebula; i++)
        {
            nebulae[i].pos.x += nebulaVelocity * deltaTime;
            coins[i].pos.x -= nebulaVelocity * deltaTime;
        }

        //Update finish line
        finishLine += nebulaVelocity * deltaTime;

        for(AnimData neb : nebulae)
        {
            float padding{50};
            Rectangle nebRec{
                neb.pos.x + padding,
                neb.pos.y + padding,
                neb.rect.width - 2 * padding,
                neb.rect.height-2*padding
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rect.width,
                scarfyData.rect.height
            };
            if(CheckCollisionRecs(nebRec,scarfyRec))
            {
                collision = true;
            }
        }

        for(int i = 0; i < sizeOfCoins; i++)
        {
            Rectangle coinRec{
                currentCoins[i]->pos.x,
                currentCoins[i]->pos.y,
                150,
                150,
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rect.width,
                scarfyData.rect.height
            };
            if(CheckCollisionRecs(coinRec,scarfyRec) && !hasPickedCoin)
            {
                hasPickedCoin = true;
                score++;
                timer = 8;
                currentCoins[score]->ChangeRenderer(false);
            }
            if(timer < 0)
            {
                hasPickedCoin = false;
            }
            else
            {
                timer -= deltaTime;
            }
        }

        //Update scarfy Pos
        scarfyData.pos.y += velocity * deltaTime;

        if(collision)
        {
            int youLoseTextMeasurement = MeasureText("You Lose!", 30);
            DrawText("You Lose!", windowDimensions[0] / 2 - youLoseTextMeasurement / 2, windowDimensions[1] / 2, 30, RED);
        }
        else if(scarfyData.pos.x >= finishLine)
        {
            int youWinMeasurement = MeasureText("You Win!", 30);
            DrawText("You Win!", windowDimensions[0] / 2 - youWinMeasurement / 2, windowDimensions[1] / 2, 30, GREEN);
        }
        else
        {
            DrawText(TextFormat("Score: %i", score), 0, 0, 30, WHITE);
            
            for (int i = 0; i < sizeOfCoins; i++)
            {
                //Draw Nebula
                DrawTextureRec(nebula, nebulae[i].rect, nebulae[i].pos, WHITE);
                if(currentCoins[i]->shouldBeRendered)
                {
                    DrawTexturePro(goldCoin,currentCoins[i]->rect, {currentCoins[i]->rect.x,currentCoins[i]->rect.y, currentCoins[i]->width, currentCoins[i]->height},currentCoins[i]->pos,0,WHITE);
                }
            }

            //Draw Scarfy
            DrawTextureRec(scarfy, scarfyData.rect, scarfyData.pos, WHITE);
        }
        
        //END GAME LOGIC
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(foreground);
    UnloadTexture(backbuildings);
    UnloadTexture(goldCoin);
    
    CloseWindow();
    return 0;
}
