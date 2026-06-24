#include "raylib.h"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
extern "C" {
    #include "tinyfiledialogs.h"
}


/* Cursor related functions */
Rectangle cursor = {(float)(GetMouseX()), (float)(GetMouseY()), 1.00, 1.00};
Rectangle lastCursor = {(float)(GetMouseX()), (float)(GetMouseY()), 1.00, 1.00};

/* UI related */
int currentTab = 0;
float tabsBarSizePercent = 0.05;
bool changingContrastType = false;

// Image movement stuff
Vector2 imagePos = {0.00,0.00};
float imageScale = 0; // 0 covers the entire screen

// Actual clickable buttons
Rectangle welcomeTabButton;
Rectangle imageTabButton;
Rectangle settingsTabButton;
Rectangle increaseTabButton;
Rectangle decreaseTabButton;

// For sidebar
Rectangle toggleContrastViewButton;
Rectangle brightnessContrastButton;
Rectangle redContrastButton;
Rectangle greenContrastButton;
Rectangle blueContrastButton;
Rectangle lowerContrastSlider;
Rectangle upperContrastSlider;

Rectangle sortButton;
Rectangle resetSortButton;

// UI elements that should be drawn
Rectangle tabBarBackground;
Rectangle sidebarBackground;
Rectangle loadingBarBackground; 

// For Sidebar
Rectangle lowerContrastSliderBackround;
Rectangle upperContrastSliderBackround;

/* Image related */
bool hasImageLoaded = false;
bool imageContrastShown = false;
const char *imageFilePath;
Image originalImage;
Texture2D originalTex;
Image contrastImage;
Texture2D contrastTex;
Image sortedImage;
Texture2D sortedTex;

// Brighter goes towards vector
Vector2 sortDirectionVector = { 1, 0};

// Contrast stuffs
enum ContrastType {
    Brightness,
    Red,
    Green,
    Blue
};

ContrastType currentContrastType = Brightness;
int contrastLow = 50;
int contrastHigh = 200;

int GetBrightnessPerPixel(Color pixel) {
    switch (currentContrastType) {
        case Brightness:
            return (pixel.r + pixel.g + pixel.b) / 3;
            break;
        case Red:
            return pixel.r;
            break;
        case Green:
            return pixel.g;
            break;
        case Blue:
            return pixel.b;
            break;
    }
}

void ContrastMask() { 
    contrastImage = LoadImage(imageFilePath);

    for (int x = 0; x < contrastImage.width; x++) {
        for (int y = 0; y < contrastImage.height; y++) {
            Color currentPixelColor = GetImageColor(contrastImage, x, y);
            switch (currentContrastType) {
                case Brightness:
                    if ((currentPixelColor.r + currentPixelColor.g + currentPixelColor.b) / 3 >= contrastLow && (currentPixelColor.r + currentPixelColor.g + currentPixelColor.b) / 3 <= contrastHigh) {
                        ImageDrawPixel(&contrastImage, x, y, LIGHTGRAY);
                    } else {
                        ImageDrawPixel(&contrastImage, x, y, BLACK);
                    }
                    break;
                case Red:
                    if (currentPixelColor.r >= contrastLow && currentPixelColor.r <= contrastHigh) {
                        ImageDrawPixel(&contrastImage, x, y, LIGHTGRAY);
                    } else {
                        ImageDrawPixel(&contrastImage, x, y, BLACK);
                    }
                    break;
                case Green:
                    if (currentPixelColor.g >= contrastLow && currentPixelColor.g <= contrastHigh) {
                        ImageDrawPixel(&contrastImage, x, y, LIGHTGRAY);
                    } else {
                        ImageDrawPixel(&contrastImage, x, y, BLACK);
                    }
                    break;
                case Blue:
                    if (currentPixelColor.b >= contrastLow && currentPixelColor.b <= contrastHigh) {
                        ImageDrawPixel(&contrastImage, x, y, LIGHTGRAY);
                    } else {
                        ImageDrawPixel(&contrastImage, x, y, BLACK);
                    }
                    break;
            }
        }
    }
    UnloadTexture(contrastTex);
    contrastTex = LoadTextureFromImage(contrastImage);
}

void SortImage() {
    UnloadImage(sortedImage);
    sortedImage = LoadImage(imageFilePath);

    // Right or Left
    if (sortDirectionVector.x != 0) {
        for (int y = 0; y < sortedImage.height; y++) {
            Color rowColor[sortedImage.width];

            // Get current row into an array
            for (int x = 0; x < sortedImage.width; x++) {
                rowColor[x] = GetImageColor(sortedImage, x, y);
            }

            // Process and sort spars
            int sparStart = -1;
            for (int x = 0; x < sortedImage.width; x++) {
                bool inRange = x < sortedImage.width && ColorIsEqual(GetImageColor(contrastImage, x, y), LIGHTGRAY);

                if (inRange && sparStart == -1) {
                    // Start of new spar
                    sparStart = x;
                } else if ((!inRange || x == sortedImage.width - 1) && sparStart != -1) {
                    // End of current spar
                    int sparEnd = x - 1;
                    int sparLength = sparEnd - sparStart + 1;
                    
                    // Sort it
                    // Bubble sort
                    for (int i = 0; i < sparLength - 1; i++) {
                        for (int j = 0; j < sparLength - i - 1; j++) {
                            
                            // Right
                            if (sortDirectionVector.x > 0) {
                                if (GetBrightnessPerPixel(rowColor[sparStart + j]) > GetBrightnessPerPixel(rowColor[sparStart + j + 1])) {
                                    Color tmp = rowColor[sparStart + j];
                                    rowColor[sparStart + j] = rowColor[sparStart + j + 1];
                                    rowColor[sparStart + j + 1] = tmp;
                                }
                                
                            // Left
                            } else if (sortDirectionVector.x < 0) {
                                if (GetBrightnessPerPixel(rowColor[sparStart + j]) < GetBrightnessPerPixel(rowColor[sparStart + j + 1])) {
                                    Color tmp = rowColor[sparStart + j];
                                    rowColor[sparStart + j] = rowColor[sparStart + j + 1];
                                    rowColor[sparStart + j + 1] = tmp;
                                }
                            }
                        }
                    }

                    // Restart for next spar
                    sparStart = -1;
                }
            }

            // Get sorted row back into image
            for (int x = 0; x < sortedImage.width; x++) {
                ImageDrawPixel(&sortedImage, x, y, rowColor[x]);
            }
        }
    }

    // Up or down
    if (sortDirectionVector.y != 0) {
        for (int x = 0; x < sortedImage.width; x++) {
            Color colColor[sortedImage.height];

            // Get current row into an array
            for (int y = 0; y < sortedImage.height; y++) {
                colColor[y] = GetImageColor(sortedImage, x, y);
            }

            // Process and sort spars
            int sparStart = -1;
            for (int y = 0; y < sortedImage.height; y++) {
                bool inRange = y < sortedImage.height && ColorIsEqual(GetImageColor(contrastImage, x, y), LIGHTGRAY);

                if (inRange && sparStart == -1) {
                    // Start of new spar
                    sparStart = y;
                } else if ((!inRange || y == sortedImage.height) && sparStart != -1) {
                    // End of current spar
                    int sparEnd = y - 1;
                    int sparLength = sparEnd - sparStart + 1;

                    // Sort it
                    // Bubble sort
                    for (int i = 0; i < sparLength - 1; i++) {
                        for (int j = 0; j < sparLength - i - 1; j++) {
                            
                            // Up
                            if (sortDirectionVector.y > 0) {
                                if (GetBrightnessPerPixel(colColor[sparStart + j]) > GetBrightnessPerPixel(colColor[sparStart + j + 1])) {
                                    Color tmp = colColor[sparStart + j];
                                    colColor[sparStart + j] = colColor[sparStart + j + 1];
                                    colColor[sparStart + j + 1] = tmp;
                                }
                                
                            // Down
                            } else if (sortDirectionVector.y < 0) {
                                if (GetBrightnessPerPixel(colColor[sparStart + j]) < GetBrightnessPerPixel(colColor[sparStart + j + 1])) {
                                    Color tmp = colColor[sparStart + j];
                                    colColor[sparStart + j] = colColor[sparStart + j + 1];
                                    colColor[sparStart + j + 1] = tmp;
                                }
                            }
                        }
                    }

                    // Restart for next spar
                    sparStart = -1;
                }
            }

            // Get sorted row back into image
            for (int y = 0; y < sortedImage.width; y++) {
                ImageDrawPixel(&sortedImage, x, y, colColor[y]);
            }
        }
    }

    UnloadTexture(sortedTex);
    sortedTex = LoadTextureFromImage(sortedImage);
}

void WelcomeTab() {
    // Tutorial area
    Rectangle learnMoreButtion = {  (float)(GetRenderHeight() * tabsBarSizePercent * 2.30), 
                                    (float)(GetRenderHeight() * tabsBarSizePercent * 1.30), 
                                    (float)(GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent * 2.60), 
                                    (float)(GetRenderHeight() / 7.00 - GetRenderHeight() * tabsBarSizePercent * 1.60)};
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
    Rectangle loadImageButtion = {  (float)(GetRenderHeight() * tabsBarSizePercent * 2.30), 
                                    (float)(GetRenderHeight() * tabsBarSizePercent * 1.60) + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                                    (float)(GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent * 2.60), 
                                    (float)(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60)};
    DrawRectangleLinesEx(loadImageButtion, 1.00, RAYWHITE);
    if (!hasImageLoaded) {
        DrawText("Load an image", 
                GetRenderHeight() * tabsBarSizePercent * 2.60, 
                GetRenderHeight() * tabsBarSizePercent * 1.80 + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                RAYWHITE);
        if (CheckCollisionRecs(cursor, loadImageButtion) && IsMouseButtonPressed(0)) {
            imageFilePath = tinyfd_openFileDialog("Select an image", 
                                                        "", 
                                                        0, 
                                                        NULL,
                                                        NULL,
                                                        0);
            if (imageFilePath != NULL) {
                originalImage = LoadImage(imageFilePath);
                sortedImage = LoadImage(imageFilePath);
                ContrastMask();
                if (originalImage.data != NULL && IsImageValid(originalImage)) {
                    originalTex = LoadTextureFromImage(originalImage);
                    contrastTex = LoadTextureFromImage(contrastImage);
                    sortedTex = LoadTextureFromImage(sortedImage);
                    hasImageLoaded = true;
                }
            }
        } else {
            DrawText("No image loaded!", 
                    GetRenderHeight() * tabsBarSizePercent * 2.60, 
                    GetRenderHeight() * tabsBarSizePercent * 2.80 + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                    GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                    RAYWHITE);
        }
    // If image already loaded
    } else {
        DrawText("Image loaded!", 
                GetRenderHeight() * tabsBarSizePercent * 2.60, 
                GetRenderHeight() * tabsBarSizePercent * 1.80 + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                RAYWHITE);
        DrawText(imageFilePath, 
                GetRenderHeight() * tabsBarSizePercent * 2.60, 
                GetRenderHeight() * tabsBarSizePercent * 2.80 + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                RAYWHITE);
        if (CheckCollisionRecs(cursor, loadImageButtion) && IsMouseButtonPressed(0)) {
            UnloadImage(originalImage);
            UnloadTexture(originalTex);
            UnloadImage(contrastImage);
            UnloadTexture(contrastTex);
            UnloadImage(sortedImage);
            UnloadTexture(sortedTex);
            hasImageLoaded = false;
            imagePos = {0, 0};
            imageScale = 0;
        }
    }

    // Export image
    Rectangle exportImageButtion = {(float)(GetRenderHeight() * tabsBarSizePercent * 2.30), 
                                    (float)(GetRenderHeight() * tabsBarSizePercent * 1.90) + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60) + (float)(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                                    (float)(GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent * 2.60), 
                                    (float)(GetRenderHeight() / 3.50 - GetRenderHeight() * tabsBarSizePercent * 1.60)};
    DrawRectangleLinesEx(exportImageButtion, 1.00, RAYWHITE);
    DrawText("Export image", 
                    GetRenderHeight() * tabsBarSizePercent * 2.60, 
                    (float)(GetRenderHeight() * tabsBarSizePercent * 2.20) + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60) + (float)(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                    GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                    RAYWHITE);
    DrawText("Exported to: ", 
                    GetRenderHeight() * tabsBarSizePercent * 2.60, 
                    (float)(GetRenderHeight() * tabsBarSizePercent * 3.20) + (float)(GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.60) + (float)(GetRenderHeight() / 1.50 - GetRenderHeight() * tabsBarSizePercent * 1.60), 
                    GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
                    RAYWHITE);
}

void ImageTab() {
    // Move image around
    if (!CheckCollisionRecs(cursor, tabBarBackground) && !CheckCollisionRecs(cursor, sidebarBackground) && IsMouseButtonDown(0)) {
        Vector2 cursorDifference = {(float)((cursor.x - lastCursor.x) * 1.00), (float)((cursor.y - lastCursor.y) * 1.00)};
        imagePos = {imagePos.x + cursorDifference.x, imagePos.y + cursorDifference.y}; 
    }

    Vector2 newImagePos = {GetRenderHeight() * tabsBarSizePercent * 2 + imagePos.x, GetRenderHeight() * tabsBarSizePercent + imagePos.y};

    // Scale image
    imageScale = imageScale + GetMouseWheelMove() * 0.10;

    float renderSizePercentOfImage = (GetRenderHeight() - tabBarBackground.height - loadingBarBackground.height) / originalTex.height;
    float newImageScale = imageScale + renderSizePercentOfImage;

    if (newImageScale < 0) {
        newImageScale = 0;
    }

    // Image drawing
    if (hasImageLoaded && imageContrastShown) {
        DrawTextureEx(contrastTex, newImagePos, 0.00, newImageScale, WHITE);
    } else if (hasImageLoaded && !imageContrastShown) {
        DrawTextureEx(sortedTex, newImagePos, 0.00, newImageScale, WHITE);
    } else {
        DrawText("No image loaded!", 
            GetRenderHeight() * tabsBarSizePercent * 2.60, 
            GetRenderHeight() * tabsBarSizePercent * 1.40, 
            GetRenderHeight() / 7 - GetRenderHeight() * tabsBarSizePercent * 1.80, 
            RAYWHITE);
    }
    
    // Loading bar
    DrawRectangleRec(loadingBarBackground, GRAY);
    DrawLine(loadingBarBackground.x, loadingBarBackground.y, GetRenderWidth(), loadingBarBackground.y, RAYWHITE);

    // DEBUG
    printf("low: %d | high: %d\n", contrastLow, contrastHigh);
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
        cursor = {(float)(GetMouseX()), (float)(GetMouseY()), 1, 1};

        // Update button values now
        welcomeTabButton = {0.00, 
                            0.00, 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 4.20), 
                            GetRenderHeight() * tabsBarSizePercent};
        imageTabButton = {  welcomeTabButton.width, 
                            0.00, 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 2.90), 
                            GetRenderHeight() * tabsBarSizePercent};
        settingsTabButton = {welcomeTabButton.width + imageTabButton.width, 
                            0.00, 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 4.40), 
                            GetRenderHeight() * tabsBarSizePercent};
        increaseTabButton = {GetRenderWidth() - GetRenderHeight() * tabsBarSizePercent, 
                            0, 
                            GetRenderHeight() * tabsBarSizePercent, 
                            GetRenderHeight() * tabsBarSizePercent};
        decreaseTabButton = {GetRenderWidth() - (GetRenderHeight() * tabsBarSizePercent + increaseTabButton.width),    
                            0, 
                            GetRenderHeight() * tabsBarSizePercent, 
                            GetRenderHeight() * tabsBarSizePercent};
        toggleContrastViewButton = {0,    
                                    GetRenderHeight() * tabsBarSizePercent, 
                                    (float)(GetRenderHeight() * tabsBarSizePercent * 2.00) - 1, 
                                    (float)(GetRenderHeight() * tabsBarSizePercent * 2.00)};
        brightnessContrastButton = {0,    
                                    GetRenderHeight() * tabsBarSizePercent + toggleContrastViewButton.height, 
                                    (float)(GetRenderHeight() * tabsBarSizePercent * 2.00) - 1, 
                                    (float)(GetRenderHeight() * tabsBarSizePercent * 2.00)};
        redContrastButton = {0,    
                            brightnessContrastButton.height + brightnessContrastButton.y, 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 2.00) - 1, 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 2.00)};
        greenContrastButton = { 0,    
                                redContrastButton.height + redContrastButton.y, 
                                (float)(GetRenderHeight() * tabsBarSizePercent * 2.00) - 1, 
                                (float)(GetRenderHeight() * tabsBarSizePercent * 2.00)};
        blueContrastButton = {  0,    
                                greenContrastButton.height + greenContrastButton.y, 
                                (float)(GetRenderHeight() * tabsBarSizePercent * 2.00) - 1, 
                                (float)(GetRenderHeight() * tabsBarSizePercent * 2.00)};

        sortButton = {  0,    
                        (float)(GetRenderHeight() - GetRenderHeight() * tabsBarSizePercent * 4.00), 
                        (float)(GetRenderHeight() * tabsBarSizePercent * 2.00) - 1, 
                        (float)(GetRenderHeight() * tabsBarSizePercent * 4.00)};

        resetSortButton = { 0,    
                            (float)(GetRenderHeight() - GetRenderHeight() * tabsBarSizePercent * 6.00), 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 2.00) - 1, 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 2.00)};

        // Now Drawn UI elements
        tabBarBackground = {0.00, 
                            0.00, 
                            (float)(GetRenderWidth()), 
                            GetRenderHeight() * tabsBarSizePercent};
        sidebarBackground = {0.00, 
                            tabBarBackground.height, 
                            (float)(GetRenderHeight() * tabsBarSizePercent * 2.00), 
                            GetRenderHeight() - tabBarBackground.height};

        loadingBarBackground = {sidebarBackground.width, 
                                GetRenderHeight() - tabBarBackground.height,
                                GetRenderWidth() - sidebarBackground.width,
                                tabBarBackground.height};
        
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
        } 
        
        if (currentTab > 2) {
            currentTab = 2;
        } 
        
        if (currentTab < 0) {
            currentTab = 0;
        }

        /* Contrast and image manipulation */
        if (IsKeyPressed(KEY_C) || CheckCollisionRecs(cursor, toggleContrastViewButton) && IsMouseButtonPressed(0)) {
            if (imageContrastShown) {
                imageContrastShown = false;
            } else {
                imageContrastShown = true;
            }
        }

        if (changingContrastType) {
            if (CheckCollisionRecs(cursor, brightnessContrastButton) && IsMouseButtonPressed(0)) {
                changingContrastType = false;
                currentContrastType = Brightness;
            } else if (CheckCollisionRecs(cursor, redContrastButton) && IsMouseButtonPressed(0)) {
                changingContrastType = false;
                currentContrastType = Red;
            } else if (CheckCollisionRecs(cursor, greenContrastButton) && IsMouseButtonPressed(0)) {
                changingContrastType = false;
                currentContrastType = Green;
            } else if (CheckCollisionRecs(cursor, blueContrastButton) && IsMouseButtonPressed(0)) {
                changingContrastType = false;
                currentContrastType = Blue;
            }

            if (!changingContrastType) {
                ContrastMask();
            }
        } else {
            if (CheckCollisionRecs(cursor, brightnessContrastButton) && IsMouseButtonPressed(0)) {
                changingContrastType = true;
            }
        }

        /*if (IsKeyPressed(KEY_A)) {
            changingContrastType = false;
            currentContrastType = Brightness;
            ContrastMask();
        } else if (IsKeyPressed(KEY_S)) {
            changingContrastType = false;
            currentContrastType = Red;
            ContrastMask();
        } else if (IsKeyPressed(KEY_D)) {
            changingContrastType = false;
            currentContrastType = Green;
            ContrastMask();
        } else if (IsKeyPressed(KEY_F)) {
            changingContrastType = false;
            currentContrastType = Blue;
            ContrastMask();
        }*/

        if (IsKeyPressed(KEY_Q)) {
            contrastLow = contrastLow + 5;
            ContrastMask();
        } else if (IsKeyPressed(KEY_A)) {
            contrastLow = contrastLow - 5;
            ContrastMask();
        }

        if (IsKeyPressed(KEY_W)) {
            contrastHigh = contrastHigh + 5;
            ContrastMask();
        } else if (IsKeyPressed(KEY_S)) {
            contrastHigh = contrastHigh - 5;
            ContrastMask();
        }

        if (IsKeyPressed(KEY_R) || CheckCollisionRecs(cursor, resetSortButton) && IsMouseButtonPressed(0)) {
            UnloadImage(sortedImage);
            sortedImage = LoadImage(imageFilePath);
            UnloadTexture(sortedTex);
            sortedTex = LoadTextureFromImage(sortedImage);
        }

        if (sortDirectionVector.x > 1) {
            sortDirectionVector.x = 1;
        } else if (sortDirectionVector.x < -1) {
            sortDirectionVector.x = -1;
        }

        if (sortDirectionVector.y > 1) {
            sortDirectionVector.y = 1;
        } else if (sortDirectionVector.y < -1) {
            sortDirectionVector.y = -1;
        }

        if (IsKeyPressed(KEY_J)) {
            sortDirectionVector.x--;
        } else if (IsKeyPressed(KEY_L)) {
            sortDirectionVector.x++;
        } else if (IsKeyPressed(KEY_I)) {
            sortDirectionVector.y--;
        } else if (IsKeyPressed(KEY_K)) {
            sortDirectionVector.y++;
        }

        if (IsKeyPressed(KEY_ENTER) || CheckCollisionRecs(cursor, sortButton) && IsMouseButtonPressed(0)) {
            SortImage();
        }
        
        /* Drawing */
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Draw the tab
        switch (currentTab) {
            case 0:
                WelcomeTab();
                DrawRectangleRec(tabBarBackground, GRAY);
                DrawRectangleRec(welcomeTabButton, DARKGRAY);
                break;
            case 1:
                ImageTab();
                DrawRectangleRec(tabBarBackground, GRAY);
                DrawRectangleRec(imageTabButton, DARKGRAY);
                break;
            case 2:
                SettingsTab();
                DrawRectangleRec(tabBarBackground, GRAY);
                DrawRectangleRec(settingsTabButton, DARKGRAY);
                break;
        }

        // Draw the tab bar
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
        if (imageContrastShown) {
            DrawRectangleRec(toggleContrastViewButton, DARKGRAY);
        }

        if (changingContrastType) {
            DrawRectangleRec(brightnessContrastButton, LIGHTGRAY);
            DrawLine(0, redContrastButton.y, sidebarBackground.width, redContrastButton.y, RAYWHITE);
            DrawRectangleRec(redContrastButton, MAROON);
            DrawLine(0, greenContrastButton.y, sidebarBackground.width, greenContrastButton.y, RAYWHITE);
            DrawRectangleRec(greenContrastButton, DARKGREEN);
            DrawLine(0, blueContrastButton.y, sidebarBackground.width, blueContrastButton.y, RAYWHITE);
            DrawRectangleRec(blueContrastButton, DARKBLUE);
            DrawLine(0, tabBarBackground.height + blueContrastButton.height, sidebarBackground.width, tabBarBackground.height + blueContrastButton.height, RAYWHITE);
        } else {
            switch (currentContrastType) {
                case Brightness:
                    DrawRectangleRec(brightnessContrastButton, LIGHTGRAY);
                    DrawLine(0, redContrastButton.y, sidebarBackground.width, redContrastButton.y, RAYWHITE);
                    break;
                case Red:
                    DrawRectangleRec(brightnessContrastButton, MAROON);
                    DrawLine(0, redContrastButton.y, sidebarBackground.width, redContrastButton.y, RAYWHITE);
                    break;
                case Green:
                    DrawRectangleRec(brightnessContrastButton, DARKGREEN);
                    DrawLine(0, redContrastButton.y, sidebarBackground.width, redContrastButton.y, RAYWHITE);
                    break;
                case Blue:
                    DrawRectangleRec(brightnessContrastButton, DARKBLUE);
                    DrawLine(0, redContrastButton.y, sidebarBackground.width, redContrastButton.y, RAYWHITE);
                    break;
            }
        }

        DrawRectangleRec(resetSortButton, LIGHTGRAY);
        DrawRectangleRec(sortButton, DARKGREEN);

        DrawLine(0, tabBarBackground.height + toggleContrastViewButton.height, sidebarBackground.width, tabBarBackground.height + toggleContrastViewButton.height, RAYWHITE);
        EndDrawing();

        // For dragging stuff (kinda like delta or sumthin)
        lastCursor = cursor;
    }

    CloseWindow();
    return 0;
}