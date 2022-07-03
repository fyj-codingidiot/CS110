#include "lcd/lcd.h"
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#include "img.h"
#include <math.h>

// Click Event
#define LEFT_BUTTON_CLICK (Get_Button(0))
#define RIGHT_BUTTON_CLICK (Get_Button(1))
#define BOOT0_BUTTON_CLICK (Get_BOOT0())
// Color setting
#define COLOR_BACKGROUND BLACK

typedef enum _RexState {
    Runing,
    Crouching,
    Jumping_Up,
    Jumping_Down,
    Dead
} RexState;
typedef enum _ObstacleType {
    Bird,
    Plant0,
    Plant1,
} ObstacleType;
typedef struct _Rex
{
    int PosX;
    int PosY;
    int Height;
    int Width;
    RexState State;
    bool footState;
} Rex;

typedef struct _Obstacle
{
    int PosX;
    int PosY;
    int Height;
    int Width;
    ObstacleType Type;
} Obstacle;
// Game variables
Rex rex;
Obstacle obs0, obs1;
int ObstacleSpeed = 2;
int obsWidth[3] = {20,12,12};
int cloud0X,cloud0Y,cloud1X,cloud1Y;
int groundpointX[5] = {15, 57, 89, 123, 156};
int groundpointY[5] = {73, 75, 76, 71, 72};
double score;
int jump_count = 0;
int difficulty = 0;
void Inp_init(void)
{
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

void Adc_init(void) 
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1);
    RCU_CFG0|=(0b10<<14)|(1<<28);
    rcu_periph_clock_enable(RCU_ADC0);
    ADC_CTL1(ADC0)|=ADC_CTL1_ADCON;
}

void IO_init(void)
{
    Inp_init(); // inport init
    Adc_init(); // A/D init
    Lcd_Init(); // LCD init
}
void GameInitialize() {
    
    rex.PosX = 15;
    rex.PosY = 50;
    rex.State = Runing;
    rex.Height = 20;
    rex.Width = 15;

    obs0.PosX = 79;
    obs0.PosY = 55;
    obs0.Type = Plant0;
    obs0.Width = 12;
    obs0.Height = 20;

    obs1.PosX = 159;
    obs1.PosY = 55;
    obs1.Type = Plant1;
    obs1.Width = 12;
    obs1.Height = 20;

    cloud0X = 100;
    cloud0Y = 20;
    cloud1X = 40;
    cloud1Y = 25;
    score = 0;
}
void RexUpdate() {
    score += 0.1;
    rex.footState = 1 - rex.footState;
    if (rex.State == Jumping_Up) {
        // rex.PosY -= 3; // TODO: set value
        // if (rex.PosY <= 15) {
        //     rex.PosY = 15;
        //     rex.State = Jumping_Down;
        // }
        if(!difficulty){
            rex.PosY = 80-(-23*(jump_count-13)*(jump_count-13)/169+53);
            jump_count += 1;
            if(jump_count >= 13){
            rex.PosY = 27;
            rex.State = Jumping_Down;
        }
        }
        else{
            rex.PosY = 80-(-23*(jump_count-7)*(jump_count-7)/49+53);
            jump_count += 1;
            if(jump_count >= 7){
            rex.PosY = 27;
            rex.State = Jumping_Down;
        }
        }
    }
    else if (rex.State == Jumping_Down) {
        if(!difficulty){
            rex.PosY = 80-(-23*(jump_count-13)*(jump_count-13)/144+53);
            jump_count += 1;
            if(jump_count >= 25){
            jump_count = 0;
            rex.PosY = 50;
            rex.State = Runing;
        }
        }
        else{
            rex.PosY = 80-(-23*(jump_count-7)*(jump_count-7)/36+53);
            jump_count += 1;
            if(jump_count >= 13){
            jump_count = 0;
            rex.PosY = 50;
            rex.State = Runing;
        }
        }
    }
    else if (rex.State == Crouching) {
        rex.State = Runing;
        rex.PosX += 3;
        rex.PosY -= 8;
        rex.Width = 15;
    }
}
void CloudUpdate(){
    cloud0X -= 1;
    cloud1X -= 1;
    if(cloud0X <= -30){
        cloud0X = 130;
    }
    if(cloud1X <= -30){
        cloud1X = 130;
    }
}
void ObstacleUpdate(Obstacle* obs) {
    obs->PosX -= ObstacleSpeed; // TODO： set value
    if (obs->PosX <= -obs->Width) {
        obs->PosX = 160 + rand() % 10; // right edge
        score ++;
        obs->Type = rand() % 3;
        obs->PosY = 55;
        obs->Width = obsWidth[obs->Type];
        if (obs->Type == 0) {
            obs->PosY = 35;
        }
    }
}
bool PointCollision(int x, int y) {
    int rex_X1, rex_Y1;
    rex_X1 = rex.PosX + rex.Width - 6;
    rex_Y1 = rex.PosY + rex.Height - 1;
    if (x >= rex.PosX && x <= rex_X1 && y >= rex.PosY && y <= rex_Y1) {
        return true;
    }
    return false;
}
bool ObstacleCollision(Obstacle* obs) {
    int obs_X0, obs_Y0, obs_X1, obs_Y1;
    if (obs->Type == 0) {
        obs_X0 = obs->PosX + 3;
        obs_Y0 = obs->PosY;
        obs_X1 = obs_X0 + 8;
        obs_Y1 = obs->PosY + obs->Height - 1;
    }
    else if (obs->Type == 1) {
        obs_X0 = obs->PosX + 2;
        obs_Y0 = obs->PosY;
        obs_X1 = obs_X0 + 10;
        obs_Y1 = obs->PosY + obs->Height - 1;
    }
    else {
        obs_X0 = obs->PosX + 3;
        obs_Y0 = obs->PosY;
        obs_X1 = obs_X0 + 7;
        obs_Y1 = obs->PosY + obs->Height - 1;
    }
    if (PointCollision(obs_X0,obs_Y0) || PointCollision(obs_X0,obs_Y1) || 
        PointCollision(obs_X1,obs_Y0) || PointCollision(obs_X1,obs_Y1)) {
        return true;
    }
    return false;
}
void GetInput() {
    if (LEFT_BUTTON_CLICK) {
        if (rex.State == Runing || rex.State == Crouching) {
            rex.State = Jumping_Up;
        }
    }
    if (RIGHT_BUTTON_CLICK) {
        if (rex.State == Runing) {
            rex.State = Crouching;
            rex.PosX -= 3;
            rex.PosY += 8;
            rex.Width = 27;
        }
    }
}
void RexDraw() {
    // LCD_ShowNum(0,0,rex.State,3,GREEN);
    LCD_ShowNum(120,0,score,3,YELLOW);
    if (rex.State == Crouching) {
        if (rex.footState)
            //LCD_ShowPic(rex.PosX,rex.PosY-8,rex.PosX+rex.Width-1,rex.PosY+rex.Height -9,trex4);
            draw_dino_crouching(rex.PosX,rex.PosY-8,trex4);
        else 
            //LCD_ShowPic(rex.PosX,rex.PosY-8,rex.PosX+rex.Width-1,rex.PosY+rex.Height -9,trex5);
            draw_dino_crouching(rex.PosX,rex.PosY-8,trex5);
    }
    else if (rex.State == Runing) {
        if (rex.footState)
            //LCD_ShowPic(rex.PosX - 5,rex.PosY,rex.PosX+rex.Width-1,rex.PosY+rex.Height-1,trex1);
            draw_dino_running(rex.PosX-5,rex.PosY,trex1);
        else
            //LCD_ShowPic(rex.PosX - 5,rex.PosY,rex.PosX+rex.Width-1,rex.PosY+rex.Height-1,trex2);
            draw_dino_running(rex.PosX-5,rex.PosY,trex2);
    }
    else if (rex.State == Jumping_Up || rex.State == Jumping_Down) {
        //LCD_ShowPic(rex.PosX - 5,rex.PosY,rex.PosX+rex.Width-1,rex.PosY+rex.Height-1,trex3);
        draw_dino_jumping(rex.PosX-5,rex.PosY,trex3);
    }
    else if(rex.State == Dead){
        drawdino(rex.PosX-5,rex.PosY,rex.PosX+14,rex.PosY+19,trexdie);
    }
}
void ObstacleDraw() {
    if (obs0.PosX < 160 - obs0.Width) {
        switch (obs0.Type)
        {
        case 0:
            if(rex.footState){
                draw_bird(obs0.PosX,obs0.PosY,pter1);
            }
            else{
                draw_bird(obs0.PosX,obs0.PosY,pter2);
            }
            break;
        case 1:
            draw_plant_single(obs0.PosX,obs0.PosY,cactus1);
            break;
        case 2:
            draw_plant_single(obs0.PosX,obs0.PosY,cactus2);
            break;
        }
    }
    if (obs1.PosX < 160 - obs1.Width) {
        switch (obs1.Type)
        {
        case 0:
            if(rex.footState){
                draw_bird(obs1.PosX,obs1.PosY,pter1);
            }
            else{
                draw_bird(obs1.PosX,obs1.PosY,pter2);
            }
            break;
        case 1:
            draw_plant_single(obs1.PosX,obs1.PosY,cactus1);
            break;
        case 2:
            draw_plant_single(obs1.PosX,obs1.PosY,cactus2);
            break;
        }
    }
}
void GroudUpdate() {
    for (int i = 0; i < 5; i++) {
        groundpointX[i] -= ObstacleSpeed;
        if (groundpointX[i] < 0) {
            groundpointX[i] = 160 + rand() % 5;
            groundpointY[i] = 71 + rand() % 9;
        }
    }
}
void GroundDraw() {
    LCD_DrawLine(0,70,159,70,WHITE);
    LCD_DrawLine(0,71,159,71,WHITE);
    for (int i = 0; i < 5; i++) {
        LCD_DrawPoint(groundpointX[i],groundpointY[i],WHITE);
    }
}
void CloudDraw() {
    draw_clouds(cloud0X,cloud0Y,clouds);
    draw_clouds(cloud1X,cloud1Y,clouds);
}
int main(void)
{
    IO_init();         // init OLED
    LCD_Clear(COLOR_BACKGROUND);
    
    // Menu loop
    while (1)
    {
        // TODO: Draw Menu
        //menuUI(10,55,29,74,trex3);
        //startUI();
        if(!difficulty){
            startUI(RED,WHITE);
            if(RIGHT_BUTTON_CLICK){
                difficulty = 1;
                ObstacleSpeed = 5;
            }
        }
        if(difficulty){
            //LCD_DrawLine(35,45,40,45,WHITE);
            startUI(WHITE,RED);
            if(LEFT_BUTTON_CLICK){
                //startUI(WHITE,RED);
                difficulty = 0;
                ObstacleSpeed = 2;
            }
        }
        if(BOOT0_BUTTON_CLICK){
            break;
        }
        delay_1ms(33);
    }
    LCD_Clear(COLOR_BACKGROUND);
    while(1){
        menuUI(10,55,29,74,trex3);
        if(RIGHT_BUTTON_CLICK)
            break;
    }

    // Game Initialization
    GameInitialize();
    // Game loop
    while (1)
    {
        if (rex.State == Dead)
        {
            break;
        }
        if (ObstacleCollision(&obs0) || ObstacleCollision(&obs1)) {
            rex.State = Dead;
            continue;
        }
        LCD_Clear(COLOR_BACKGROUND);   
        CloudDraw();
        RexDraw();
        ObstacleDraw();
        GroundDraw();
        RexUpdate();
        CloudUpdate();
        GroudUpdate();
        ObstacleUpdate(&obs0);
        ObstacleUpdate(&obs1);
        GetInput();
        delay_1ms(33);
    }
    LCD_Clear(COLOR_BACKGROUND);
    // Dead loop
    while (1)
    {   
        GroundDraw();
        //RexDraw();
        draw_clouds(cloud0X,cloud0Y,clouds);
        draw_clouds(cloud1X,cloud1Y,clouds);
        //RexDraw();
        ObstacleDraw();
        RexDraw();
        endUI(end);
        break;
    }
    
    // TODO: Draw dead
    
}
