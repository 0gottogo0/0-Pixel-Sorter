#include "raylib.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
extern "C" {
    #include "tinyfiledialogs.h"
}


/* Cursor related functions */
Rectangle cursor = {static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY()), 1.00, 1.00};

/* UI related */
int currentTab = 0;
float tabsBarSizePercent = 0.05;

// Actual clickable buttons
Rectangle welcomeTabButton;
Rectangle imageTabButton;
Rectangle settingsTabButton;
Rectangle increaseTabButton;
Rectangle decreaseTabButton;

// UI elements that should be drawn
Rectangle tabBarBackground;
Rectangle sidebarBackground;

/* Image related */
bool hasImageLoaded = false;
const char *imageFilePath;
Image originalImage;
Texture2D originalTex;
Texture2D contrastTex;
Image sortUpDown;
Image sortLeftRight;
Texture2D modifiedTex;

void WelcomeTab() {
    // Tutorial area
    Rectangle learnMoreButtion = {  static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 2.30), 
                                    static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 1.30), 
                                    static_cast<float>(GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent * 2.60), 
                                    static_cast<float>(GetRenderHeight() / 7.00 - GetRenderHeight() * tabsBarSizePercent * 1.60)};
    DrawRectangleLinesEx(learnMoreButtion, 1.00, RAYWHITE);
    DrawText("Click here to learn more!", 
            GetRenderHeight() * tabsBarSizePercent * 2.60, 
            GetRenderHeight() * tabsBarSizePercent * 1.40, 
            GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
            RAYWHITE);

    if (CheckCollisionRecs(cursor, learnMoreButtion) && IsMouseButtonPressed(0)) {
        #if defined(_WIN32) || defined(_WIN64)
    std::string command = std::string("start ") + "https://github.com/0gottogo0/0-Pixel-Sorter";
        #elif __APPLE__
    std::string command = std::string("open ") + "https://github.com/0gottogo0/0-Pixel-Sorter";
        #elif __linux__
    std::string command = std::string("xdg-open ") + "https://github.com/0gottogo0/0-Pixel-Sorter";
        #else
    #error "Unknown Operating System"
        #endif
    std::system(command.c_str());
    }

    // Load image
    Rectangle loadImageButtion = {  static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 2.30), 
                                    static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 1.60) + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                                    static_cast<float>(GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent * 2.60), 
                                    static_cast<float>(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60)};
    DrawRectangleLinesEx(loadImageButtion, 1.00, RAYWHITE);
    if (!hasImageLoaded) {
        DrawText("Load an image", 
                GetRenderHeight() * tabsBarSizePercent * 2.60, 
                GetRenderHeight() * tabsBarSizePercent * 1.80 + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                RAYWHITE);
        if (CheckCollisionRecs(cursor, loadImageButtion) && IsMouseButtonPressed(0)) {
            const char *filterPatterns[1] = { "*.png;*.jpg;*.bmp" };
            imageFilePath = tinyfd_openFileDialog("Select an image", 
                                                        "", 
                                                        0, 
                                                        NULL,
                                                        NULL,
                                                        0);
            if (imageFilePath != NULL) {
                originalImage = LoadImage(imageFilePath);
                if (originalImage.data != NULL && IsImageValid(originalImage)) {
                    originalTex = LoadTextureFromImage(originalImage);
                    hasImageLoaded = true;
                }
            }
        } else {
            DrawText("No image loaded!", 
                    GetRenderHeight() * tabsBarSizePercent * 2.60, 
                    GetRenderHeight() * tabsBarSizePercent * 2.80 + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                    GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                    RAYWHITE);
        }
    } else {
        DrawText("Image loaded!", 
                GetRenderHeight() * tabsBarSizePercent * 2.60, 
                GetRenderHeight() * tabsBarSizePercent * 1.80 + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                RAYWHITE);
        DrawText(imageFilePath, 
                GetRenderHeight() * tabsBarSizePercent * 2.60, 
                GetRenderHeight() * tabsBarSizePercent * 2.80 + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                RAYWHITE);
    }

    // Export image
    Rectangle exportImageButtion = {static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 2.30), 
                                    static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 1.90) + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60) + static_cast<float>(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                                    static_cast<float>(GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent * 2.60), 
                                    static_cast<float>(GetRenderHeight() / 3.50 - GetRenderHeight() * tabsBarSizePercent * 1.60)};
    DrawRectangleLinesEx(exportImageButtion, 1.00, RAYWHITE);
    DrawText("Export image", 
                    GetRenderHeight() * tabsBarSizePercent * 2.60, 
                    static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 2.20) + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60) + static_cast<float>(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                    GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                    RAYWHITE);
    DrawText("Exported to: ", 
                    GetRenderHeight() * tabsBarSizePercent * 2.60, 
                    static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 3.20) + static_cast<float>(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60) + static_cast<float>(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                    GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                    RAYWHITE);
}

void ImageTab() {
    DrawText("tab 1", GetRenderWidth() / 2, GetRenderHeight() / 2, 50, RAYWHITE);
}

void SettingsTab() {
    DrawText("settings 1", GetRenderWidth() / 2, GetRenderHeight() / 2, 50, RAYWHITE);
}


int main() {
    // Window related functions
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "testing window");
    SetWindowMinSize(100, 100);
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        /* Update values every run */
        cursor = {static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY()), 1, 1};

        // Update button values now
        welcomeTabButton = {0.00, 
                            0.00, 
                            static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 4.20), 
                            GetRenderHeight() * tabsBarSizePercent};
        imageTabButton = {  welcomeTabButton.width, 
                            0.00, 
                            static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 2.90), 
                            GetRenderHeight() * tabsBarSizePercent};
        settingsTabButton = {welcomeTabButton.width + imageTabButton.width, 
                            0.00, 
                            static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 4.40), 
                            GetRenderHeight() * tabsBarSizePercent};
        increaseTabButton = {GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent, 
                            0, 
                            GetRenderHeight() * tabsBarSizePercent, 
                            GetRenderHeight() * tabsBarSizePercent};
        decreaseTabButton = {GetRenderWidth() - (GetRenderHeight() * tabsBarSizePercent + increaseTabButton.width),    
                            0, 
                            GetRenderHeight() * tabsBarSizePercent, 
                            GetRenderHeight() * tabsBarSizePercent};

        // Now Drawn UI elements
        tabBarBackground = {0.00, 
                            0.00, 
                            static_cast<float>(GetRenderWidth()), 
                            GetRenderHeight() * tabsBarSizePercent};
        sidebarBackground = {0.00, 
                            welcomeTabButton.height, 
                            static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 2.00), 
                            GetRenderHeight() - welcomeTabButton.height};
        
        // Set a minimum width that the user can resize the window too depending on the hight of the window
        SetWindowMinSize(GetRenderHeight() * 0.70, 200);

        /* Tab changes */
        if (CheckCollisionRecs(cursor, welcomeTabButton) && IsMouseButtonPressed(0)) {
            currentTab = 0;
        } else if (CheckCollisionRecs(cursor, imageTabButton) && IsMouseButtonPressed(0)) {
            currentTab = 1;
        } else if (CheckCollisionRecs(cursor, settingsTabButton) && IsMouseButtonPressed(0)) {
            currentTab = 2;
        } else if (CheckCollisionRecs(cursor, increaseTabButton) && IsMouseButtonPressed(0) || IsKeyPressed(KEY_RIGHT_BRACKET)) {
            currentTab++;
        } else if (CheckCollisionRecs(cursor, decreaseTabButton) && IsMouseButtonPressed(0) || IsKeyPressed(KEY_LEFT_BRACKET)) {
            currentTab--;
        } else if (currentTab > 3) {
            currentTab = 3;
        } 
        
        if (currentTab < 0) {
            currentTab = 0;
        }
        
        /* Drawing */
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Tab bar
        DrawRectangleRec(tabBarBackground, GRAY);

        // draw the tabs contents
        switch (currentTab) {
            default:
                // thought this was funny
                DrawText("go back :(", GetRenderWidth() / 2, GetRenderHeight() / 2, 50, RAYWHITE);
                break;
            case 0:
                WelcomeTab();
                DrawRectangleRec(welcomeTabButton, DARKGRAY);
                break;
            case 1:
                ImageTab();
                DrawRectangleRec(imageTabButton, DARKGRAY);
                break;
            case 2:
                SettingsTab();
                DrawRectangleRec(settingsTabButton, DARKGRAY);
                break;
        }

        // Continue Drawing the tab bar
        DrawLine(imageTabButton.x, 0.00, imageTabButton.x, tabBarBackground.height, RAYWHITE);
        DrawLine(settingsTabButton.x, 0.00, settingsTabButton.x, tabBarBackground.height, RAYWHITE);
        DrawLine(settingsTabButton.x, 0.00, settingsTabButton.x, tabBarBackground.height, RAYWHITE);
        DrawLine(settingsTabButton.x + settingsTabButton.width, 0.00, settingsTabButton.x + settingsTabButton.width, tabBarBackground.height, RAYWHITE);
        DrawLine(decreaseTabButton.x, 0.00, decreaseTabButton.x, tabBarBackground.height, RAYWHITE);
        DrawLine(increaseTabButton.x + 1.00, 0.00, increaseTabButton.x + 1.00, tabBarBackground.height, RAYWHITE);
        
        DrawLine(0.00, tabBarBackground.height, GetRenderWidth(), tabBarBackground.height, RAYWHITE);
        
        DrawText("Welcome", welcomeTabButton.x + tabBarBackground.height * 0.12, 0.00, tabBarBackground.height, RAYWHITE);
        DrawText("Image", imageTabButton.x + tabBarBackground.height * 0.12, 0.00, tabBarBackground.height, RAYWHITE);
        DrawText("Settings", settingsTabButton.x + tabBarBackground.height * 0.12, 0.00, tabBarBackground.height, RAYWHITE);
        DrawText("<", decreaseTabButton.x + tabBarBackground.height * 0.35, 0.00, tabBarBackground.height, RAYWHITE);
        DrawText(">", increaseTabButton.x + tabBarBackground.height * 0.35, 0.00, tabBarBackground.height, RAYWHITE);

        // Side bar
        DrawRectangleRec(sidebarBackground, GRAY);
        DrawLine(sidebarBackground.width, tabBarBackground.height, sidebarBackground.width, GetRenderHeight(), RAYWHITE);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}