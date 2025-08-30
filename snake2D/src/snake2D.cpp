#include <deque>
#include <chrono>
#include <thread>

#define RLGL_IMPLEMENTATION
#define RLGL_SHOW_GL_DETAILS_INFO
#define RLGL_ENABLE_OPENGL_DEBUG_CONTEXT
#define GRAPHICS_API_OPENGL_43
#define SUPPORT_TRACELOG

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


  typedef std::vector<Vector2> segment;


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800


void CustomLog(int msgType, const char *text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    switch (msgType)
    {
        case LOG_INFO: printf("[INFO] : "); break;
        case LOG_ERROR: printf("[ERROR]: "); break;
        case LOG_WARNING: printf("[WARN] : "); break;
        case LOG_DEBUG: printf("[DEBUG]: "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

void render_scene_to_rendertarget(RenderTexture2D render_texture){

    BeginTextureMode(render_texture);
}

#define sizeofFloat4 sizeof(float)*4
#define flip_y(y) (WINDOW_HEIGHT-y)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main ()
{
    // Set custom logger
    SetTraceLogCallback(CustomLog);
    /*
        WARNING image coordinates goes like this

        _____________________________________________________
        |                                                w,h|
        |                                                   |
        |                                                   |
        |0,0________________________________________________| 
    
    */

    // all segments corresponding to walls
std::vector<std::vector<float>> segments = {
    // first polygon
    { 10 ,flip_y(10),800,flip_y(10)},
    {800,flip_y( 10), 800, flip_y( 200 )},
    {800,flip_y( 200), 60, flip_y( 200 )},
    {60 ,flip_y( 200), 60, flip_y( 1600 )},
    {60 ,flip_y( 1600),1000, flip_y( 1600 )},
    {1000,flip_y( 1600),1000, flip_y( 1800 )},
    {1000,flip_y( 1800),10,  flip_y( 1800 )},
    {10 ,flip_y( 1800),10,  flip_y( 10 )}, 
    // second polygon
    {  400 ,flip_y(600),1000,flip_y(600)},
    { 1000 ,flip_y( 600), 1000, flip_y( 800 )},
    { 1000 ,flip_y( 800), 400, flip_y( 800 )},
    { 400  ,flip_y( 800), 400, flip_y( 600 )},
    // third polygon
    {  1400 ,flip_y(600),2000,flip_y(600)},
    { 2000 ,flip_y( 600), 2000, flip_y( 800 )},
    { 2000 ,flip_y( 800), 1400, flip_y( 800 )},
    { 1400  ,flip_y( 800), 1400, flip_y( 600 )},
    // fourth polygon
     { 2400 ,flip_y(10),3200,flip_y(10)},
    {3200,flip_y( 10), 3200, flip_y( 1800 )},
    {3200,flip_y( 1800), 2400, flip_y( 1800 )},
    {2400 ,flip_y( 1800), 2400, flip_y( 1600 )},
    {2400 ,flip_y( 1600),2900, flip_y( 1600 )},
    {2900,flip_y( 1600),2900, flip_y( 200 )},
    {2900,flip_y( 200),2400,  flip_y( 200 )},
    {2400 ,flip_y( 200),2400,  flip_y( 10 )}

}; 
std::vector<std::vector<segment>> walls= 
{  
        // first 
        {
            { {.x = 10 ,.y = flip_y(10)},   {  .x = 800, .y = flip_y(10)} },
            { {.x = 800,.y = flip_y( 10) }, {  .x = 800, .y = flip_y( 200 )} },
            { {.x = 800,.y = flip_y( 200)}, {  .x = 60,  .y = flip_y( 200 )} },
            { {.x = 60 ,.y = flip_y( 200)}, {  .x = 60,  .y = flip_y( 1600 )} },
            { {.x = 60 ,.y = flip_y( 1600)},{  .x = 1000, .y = flip_y( 1600 )} },
            { {.x = 1000,.y = flip_y( 1600)},{  .x = 1000, .y = flip_y( 1800 )} },
            { {.x = 1000,.y = flip_y( 1800)},{  .x = 10,  .y = flip_y( 1800 )} },
            { {.x = 10 ,.y = flip_y( 1800)},{  .x = 10,  .y = flip_y( 10  )} }
        },
        {
            // second polygon
                {{  .x = 400 ,  .y = flip_y(600)},{   .x = 1000, .y = flip_y(600)}},
                {{  .x = 1000 , .y = flip_y( 600)},{  .x = 1000, .y = flip_y( 800 )}},
                {{  .x = 1000 , .y = flip_y( 800)}, { .x = 400,  .y = flip_y( 800 )}},
                {{  .x = 400  , .y = flip_y( 800)}, { .x = 400,  .y = flip_y( 600 )}}
        },
        // third polygon
        {
            { {.x = 1400 , .y =  flip_y(600)} , { .x = 2000,  .y =  flip_y(600) } },
            { {.x = 2000,  .y = flip_y( 600) }, { .x = 2000,  .y =   flip_y( 800 ) } },
            { {.x = 2000,  .y = flip_y( 800) }, { .x = 1400,  .y =   flip_y( 800 ) } },
            { {.x = 1400 , .y = flip_y( 800) }, { .x = 1400,  .y =   flip_y( 600 ) } }
        },
        // fourth polygon
        {
            { { .x = 2400 , .y = flip_y(10)},{     .x = 3200,  .y = flip_y(10)}},
            { { .x = 3200,  .y = flip_y( 10)},{    .x = 3200,  .y = flip_y( 1800 )}},
            { { .x = 3200,  .y = flip_y( 1800)},{  .x = 2400,  .y = flip_y( 1800 )}},
            { { .x = 2400 , .y = flip_y( 1800)},{  .x = 2400,  .y = flip_y( 1600 )}},
            { { .x = 2400 , .y = flip_y( 1600)},{  .x = 2900,  .y = flip_y( 1600 )}},
            { { .x = 2900,  .y = flip_y( 1600)},{  .x = 2900,  .y = flip_y( 200 )}},
            { { .x = 2900,  .y = flip_y( 200)},{  .x = 2400,  .y = flip_y( 200 )}},
            { { .x = 2400 , .y = flip_y( 200)},{  .x = 2400,    .y = flip_y( 10 )}}
        }
 
}; 
 
   // Initialization 
 //--------------------------------------------------------------------------------------
      int screenWidth = WINDOW_WIDTH;
     int screenHeight = WINDOW_HEIGHT;

    InitWindow(screenWidth, screenHeight, "SnAkE");


    RenderTexture2D offscreen_render_target = LoadRenderTexture(screenWidth, screenHeight);
    RenderTexture2D final_target = LoadRenderTexture(screenWidth, screenHeight);
    BeginTextureMode(final_target);
    ClearBackground(BLUE);
    EndTextureMode();
    
    // get compute shader code and compile

    char *compute_shader_raycast2d_code = LoadFileText("cs_ray_cast_2d.comp");
    unsigned int compiled_raycast2d_compute_shader = rlCompileShader(compute_shader_raycast2d_code, RL_COMPUTE_SHADER);
    if (compiled_raycast2d_compute_shader == 0 ){
        printf("error occurred during compute shader compilation. \n");
    }
    unsigned int compute_shader_program = rlLoadComputeShaderProgram(compiled_raycast2d_compute_shader);

    Shader cs_shader = {.id = compiled_raycast2d_compute_shader};

     int teinte_Loc = rlGetLocationUniform(compute_shader_program,"teinte");
     int world2screen_matrix_Loc = rlGetLocationUniform(compute_shader_program,"world2screen_matrix");

    
    UnloadFileText(compute_shader_raycast2d_code);
    // Load shader storage buffer object (SSBO), id returned

    float all_segments[segments.size()*4]={0};
    int kkk = 0;
    for (auto iii = segments.begin();iii != segments.end();iii++){

        for (auto jjj = iii->begin();jjj != iii->end();jjj++){
            all_segments[kkk++] = *jjj;
        }
    }

    unsigned int ssboWalls = rlLoadShaderBuffer(segments.size()*4*sizeof(float), (void *)all_segments, RL_DYNAMIC_READ);

    // fruits 
	fruits = {{10,10,30},{30,50,20},{100,150,10},{180,150,30}};


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

    the_snake.speed = {speed,};
    int zoomMode = 0;   // 0-Mouse Wheel, 1-Mouse Move
    double dangle = 0;

    SetTargetFPS(120);    // Set our game to run at 60 frames-per-second

    char fpsStr[128];
    //--------------------------------------------------------------------------------------
    Vector2 direction = {std::sqrt(2),std::sqrt(2)};

    bool fast_turn_enabled = true;

    bool keyLeftPressed = false;
    bool keyRightPressed = false;


    Matrix world2screen_matrix ;
    float world2screen[7];
    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        camera.target = the_snake.current_position;
        world2screen_matrix = GetCameraMatrix2D(camera);

        world2screen[0]=world2screen_matrix.m0;world2screen[1]=world2screen_matrix.m4;world2screen[2]=world2screen_matrix.m12;
        world2screen[3]=world2screen_matrix.m1;world2screen[4]=world2screen_matrix.m5;world2screen[5]=world2screen_matrix.m13;
        world2screen[6] = camera.zoom;
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

        // render screen offline 
        //----------------------------------------------------------------------------------
        BeginTextureMode(offscreen_render_target);
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


                //draw walls
                for ( auto polygons = walls.begin();polygons != walls.end();polygons++)
                {
                    for (auto polygon = polygons->begin(); polygon != polygons->end(); polygon ++){
                    DrawLineStrip(polygon->data(),polygon->size(),MAGENTA);
                    }
                }
            EndMode2D();

            // Draw mouse reference
            //Vector2 mousePos = GetWorldToScreen2D(GetMousePosition(), camera)
            DrawCircleV(GetMousePosition(), 4, DARKGRAY);
            Vector2 mouse_position = GetMousePosition();
            Vector2 world_mouse_position = GetScreenToWorld2D(mouse_position,camera);
            DrawTextEx(GetFontDefault(), TextFormat("[%i, %i] => [%i, %i]",(int)world_mouse_position.x,(int)world_mouse_position.y, (int)mouse_position.x, (int)mouse_position.y),
                Vector2Add(GetMousePosition(), (Vector2){ -44, -24 }), 20, 2, BLACK);

            sprintf(fpsStr, " fps %i ",GetFPS() );

            DrawText(fpsStr, 20, 20, 20, DARKGRAY);
        EndTextureMode();
        float teinte[4]={1,1,1,segments.size()};

        // we use compute shader to render to this texture
        
        rlEnableShader(compute_shader_program);


            rlBindShaderBuffer(ssboWalls, 0);
            rlSetUniform(3,(void *)teinte,RL_SHADER_UNIFORM_VEC4,1);
            rlSetUniform(4,(void *)world2screen,RL_SHADER_UNIFORM_FLOAT,7);

            int output_image_location= rlGetLocationUniform(compute_shader_program,"u_output_image");
            int input_image_location= rlGetLocationUniform(compute_shader_program,"u_input_image");

            rlActiveTextureSlot(input_image_location);
            rlEnableTexture(input_image_location);
            rlBindImageTexture(offscreen_render_target.texture.id,0,offscreen_render_target.texture.format,false);

            rlActiveTextureSlot(output_image_location);
            rlEnableTexture(output_image_location);
            rlBindImageTexture(final_target.texture.id,1,final_target.texture.format,false);

            rlComputeShaderDispatch(WINDOW_WIDTH/8, WINDOW_HEIGHT/8, 1); // Each GPU unit will process a command!
            rlDisableShader();

        // we draw the final image
        BeginDrawing();

            ClearBackground(BLANK);
            DrawTexture(final_target.texture, 0, 0, WHITE);
            DrawFPS(GetScreenWidth() - 100, 10);

        EndDrawing();

        //std::this_thread::sleep_for(std::chrono::milliseconds(10));

        //----------------------------------------------------------------------------------
    }

    

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}
