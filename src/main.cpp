#include "raylib.h"

void WelcomeTab() {
    DrawText("raylib", GetRenderWidth() / 2, GetRenderHeight() / 2, 50, RAYWHITE);
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
    Rectangle tabBarBackground = {0.00, 0.00, static_cast<float>(GetRenderWidth()), GetRenderHeight() * tabsBarSizePercent};
    Rectangle sidebarBackground = {0.00, tabBarBackground.height, GetRenderHeight() * tabsBarSizePercent * 2, GetRenderHeight() - welcomeTabButton.height};

    while (!WindowShouldClose()) {
        /* Update values every run */
        cursor = {static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY()), 1, 1};

        // Update button values now
        welcomeTabButton = {0.00, 0.00, static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 4.20), GetRenderHeight() * tabsBarSizePercent};
        imageTabButton = {welcomeTabButton.width, 0.00, static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 2.90), GetRenderHeight() * tabsBarSizePercent};
        settingsTabButton = {welcomeTabButton.width + imageTabButton.width, 0.00, static_cast<float>(GetRenderHeight() * tabsBarSizePercent * 4.40), GetRenderHeight() * tabsBarSizePercent};
        increaseTabButton = {GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent, 0, GetRenderHeight() * tabsBarSizePercent, GetRenderHeight() * tabsBarSizePercent};
        decreaseTabButton = {GetRenderWidth() - (GetRenderHeight() * tabsBarSizePercent + increaseTabButton.width), 0, GetRenderHeight() * tabsBarSizePercent, GetRenderHeight() * tabsBarSizePercent};

        // Now Drawn UI elements
        tabBarBackground = {0.00, 0.00, static_cast<float>(GetRenderWidth()), GetRenderHeight() * tabsBarSizePercent};
        sidebarBackground = {0.00, welcomeTabButton.height, GetRenderHeight() * tabsBarSizePercent * 2, GetRenderHeight() - welcomeTabButton.height};
        
        // Set a minimum width that the user can resize the window too depending on the hight of the window
        SetWindowMinSize(GetRenderHeight() * 0.70, 200);

        /* Tab changes */
        if (CheckCollisionRecs(cursor, welcomeTabButton) && IsMouseButtonPressed(0)) {
            currentTab = 0;
        } else if (CheckCollisionRecs(cursor, imageTabButton) && IsMouseButtonPressed(0)) {
            currentTab = 1;
        } else if (CheckCollisionRecs(cursor, settingsTabButton) && IsMouseButtonPressed(0)) {
            currentTab = 2;
        } else if (CheckCollisionRecs(cursor, increaseTabButton) && IsMouseButtonPressed(0)) {
            currentTab++;
        } else if (CheckCollisionRecs(cursor, decreaseTabButton) && IsMouseButtonPressed(0)) {
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