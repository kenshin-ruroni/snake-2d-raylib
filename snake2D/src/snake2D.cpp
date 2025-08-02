#include <deque>
#include <chrono>
#include <thread>


#include "raylib.h"

#include "rlgl.h"
#include "raymath.h"

#include "Snake.h"
#include "Fruit.h"


#define max_delta_angle 0.0005f
#define max_delta_angle_steps 60.0f

#define delta_angle_step max_delta_angle/max_delta_angle_steps

Snake the_snake;

std::deque<Fruit> fruits;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main ()
{
	fruits = {{10,10,30},{30,50,20},{100,150,10},{180,150,30}};

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "SnAkE");

    float speed = 1;

    double currentTime = GetTime();
    double previousTime = currentTime;
    double deltaTime = 0.0;
    double dx=1,dy=0;
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    camera.target = the_snake.current_position;

    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };

    Vector2 p;
    the_snake.initialize(p = {(float)GetScreenWidth()/2, (float)GetScreenHeight()/2});

    the_snake.speed = {speed,speed};
    int zoomMode = 0;   // 0-Mouse Wheel, 1-Mouse Move
    double dangle = 0;

    SetTargetFPS(120);    // Set our game to run at 60 frames-per-second

    char fpsStr[128];
    //--------------------------------------------------------------------------------------
    Vector2 direction = {std::sqrt(2),std::sqrt(2)};

    bool fast_turn_enabled = true;

    bool keyLeftPressed = false;
    bool keyRightPressed = false;
    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {

    	keyRightPressed = IsKeyDown(KEY_RIGHT);
    	keyLeftPressed = IsKeyDown(KEY_LEFT);
    	previousTime = currentTime;
    	currentTime = GetTime();
    	deltaTime = 1; //currentTime - previousTime;

    	// the more we press a key the more the snake turns fast
    	if (fast_turn_enabled && ( keyRightPressed || keyLeftPressed ) ){

    		if( dangle > max_delta_angle )
    		{
    			dangle = 0;
    			fast_turn_enabled = false;
    		} else{
    			dangle+= delta_angle_step  ;
    		}

    	}else{
    		dangle = 0;fast_turn_enabled = true;
    	}

    	if (keyRightPressed) the_snake.angle += 0.01 + dangle;
    	if (keyLeftPressed) the_snake.angle -= 0.01 + dangle;



    	direction = {std::cos(the_snake.angle),std::sin(the_snake.angle)};
    	the_snake.direction = direction;
    	the_snake.perpendicular_direction = {direction.y,-direction.x};

    	the_snake.speed.x = speed * direction.x * deltaTime;
    	the_snake.speed.y = speed *direction.y * deltaTime;

    	the_snake.current_position +=the_snake.speed;

        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
        else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;

        // Translate based on mouse right click
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        if (zoomMode == 0)
        {
            // Zoom based on mouse wheel
            float wheel = GetMouseWheelMove();
            if (wheel != 0)
            {
                // Get the world point that is under the mouse
                //Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                // Set the offset to where the mouse is
                //camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point
                // under the cursor to the screen space point under the cursor at any zoom
                //camera.target = mouseWorldPos;

                // Zoom increment
                // Uses log scaling to provide consistent zoom speed
                float scale = 0.2f*wheel;
                camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
            }
        }
        else
        {
            // Zoom based on mouse right click
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                // Get the world point that is under the mouse
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                // Set the offset to where the mouse is
                camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point
                // under the cursor to the screen space point under the cursor at any zoom
                //camera.target = mouseWorldPos;
            }
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                // Zoom increment
                // Uses log scaling to provide consistent zoom speed
                float deltaX = GetMouseDelta().x;
                float scale = 0.005f*deltaX;
                camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
            }
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                // Draw the 3d grid, rotated 90 degrees and centered around 0,0
                // just so we have something in the XY plane
                rlPushMatrix();
                    rlTranslatef(0, 25*50, 0);
                    rlRotatef(90, 1, 0, 0);
                    DrawGrid(100, 50);
                rlPopMatrix();

                // Draw a reference circle


                the_snake.update(&fruits);

                for ( auto it = fruits.begin();it != fruits.end();it++){
                	DrawCircle(it->p.x,it->p.y,it->radius,RED);
                }
            EndMode2D();

            // Draw mouse reference
            //Vector2 mousePos = GetWorldToScreen2D(GetMousePosition(), camera)
            DrawCircleV(GetMousePosition(), 4, DARKGRAY);
            DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()),
                Vector2Add(GetMousePosition(), (Vector2){ -44, -24 }), 20, 2, BLACK);

            sprintf(fpsStr, " fps %i ",GetFPS() );

            DrawText(fpsStr, 20, 20, 20, DARKGRAY);


        EndDrawing();

        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        camera.target = the_snake.current_position;
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}
