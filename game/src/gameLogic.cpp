#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <bitset>
#include <random>
#include <gameLogic.h>
#include <input.h>
int score = 0;

struct GameData
{
    int positionX = 0;
    int positionY = 0;
    int fpsCounter = 0;
    float timer = 0;
};
static GameData data;

struct RGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Point
{
    int x;
    int y;
};
std::vector<Point> snakeVector;
std::vector<Point> snakeVectorTemp;
struct Snake
{
    int x_pos = 200;
    int y_pos = 100;
    int width = 50;
    int height = 50;

    int speed = 2500;
    float moveTimer = 0.0f;
    bool isMoving = true;
    int direction_x = 0;
    int direction_y = 0;

    bool up_can = true;
    bool down_can = true;
    bool right_can = true;
    bool left_can = true;

    bool can_click = true;

    RGB color = {50, 90, 252};

    void move(float dt, Input input)
    {
        // user input
        if (can_click)
        {
            if (input.keyBoard[Button::Up].pressed && up_can)
            {
                direction_y = -1;
                direction_x = 0;
                left_can = true;
                up_can = true;
                down_can = false;
                right_can = true;
                can_click = false;
            }
            else if (input.keyBoard[Button::Down].pressed && down_can)
            {
                direction_y = 1;
                direction_x = 0;
                left_can = true;
                up_can = false;
                down_can = true;
                right_can = true;
                can_click = false;
            }
            else if (input.keyBoard[Button::Left].pressed && left_can)
            {
                direction_y = 0;
                direction_x = -1;
                left_can = true;
                up_can = true;
                down_can = true;
                right_can = false;
                can_click = false;
            }
            else if (input.keyBoard[Button::Right].pressed && right_can)
            {
                direction_y = 0;
                direction_x = 1;
                left_can = false;
                up_can = true;
                down_can = true;
                right_can = true;
                can_click = false;
            }
        }

        // move snake every 0.5s
        moveTimer += dt;
        if (isMoving)
        {
            x_pos += 50 * direction_x;
            y_pos += 50 * direction_y;
            snakeVector[0].x = x_pos;
            snakeVector[0].y = y_pos;
            for (int segment_index = 0; segment_index < snakeVector.size(); segment_index++)
            {
                if (segment_index > 0)
                {
                    std::cout << "sex,drugs,money";
                    snakeVector[segment_index].x = snakeVectorTemp[segment_index - 1].x;
                    snakeVector[segment_index].y = snakeVectorTemp[segment_index - 1].y;

                    if (snakeVector[0].x == snakeVector[segment_index].x && snakeVector[0].y == snakeVector[segment_index].y)
                    {
                        endGame();
                    }
                }
            }
            isMoving = false;
        }
        if (moveTimer >= 0.3f)
        {
            isMoving = true;
            moveTimer -= 0.3f;
            can_click = true;
        }
    }
};
Snake snake;

struct Apple
{
    int pos_x;
    int pos_y;

    int min = 0;
    int max_x = 950;
    int max_y = 650;

    void changeLocation(bool increaseScore)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist_x(min, max_x);
        std::uniform_int_distribution<> dist_y(min, max_y);

        pos_x = (dist_x(gen) + 25) / 50 * 50;
        pos_y = (dist_y(gen) + 25) / 50 * 50;

        if (increaseScore)
        {
            score++;
            Point nextSegment = {snake.x_pos, snake.y_pos};
            snakeVector.push_back(nextSegment);
        }
    }
};
Apple apple;

struct Glyph
{
    int width;
    int height;
    int x_offset;
    int y_offset;
    std::vector<std::string> bitmap;
};

std::unordered_map<char, Glyph> loadBDF(const std::string &filePath)
{
    std::unordered_map<char, Glyph> fontData;
    std::ifstream file(filePath);
    std::string line;

    if (!file.is_open())
    {
        std::cout << "failed to open a file\n";
    }

    char currentChar = 0;
    Glyph glyph;

    while (std::getline(file, line))
    {
        if (line.find("STARTCHAR") != std::string::npos)
        {
            glyph = {};
        }
        else if (line.find("ENCODING") != std::string::npos)
        {
            std::istringstream iss(line);
            std::string temp;
            int value;
            iss >> temp >> value;
            currentChar = value;
        }
        else if (line.find("BBX") != std::string::npos)
        {
            std::istringstream iss(line);
            std::string temp;
            iss >> temp >> glyph.width >> glyph.height >> glyph.x_offset >> glyph.y_offset;
        }
        else if (line.find("BITMAP") != std::string::npos)
        {
            glyph.bitmap.clear();
            while (std::getline(file, line) && line.find("ENDCHAR") == std::string::npos)
            {
                glyph.bitmap.push_back(line);
            }
            fontData[currentChar] = glyph;
        }
    }

    return fontData;
}

void drawText(GameWindowBuffer &buffer, int x, int y, const std::string &text,
              const std::unordered_map<char, Glyph> &font, RGB color, int scale = 1)
{
    for (char c : text)
    {
        if (font.find(c) == font.end())
        {
            continue;
        }

        const Glyph &glyph = font.at(c);

        for (int row = 0; row < glyph.height; row++)
        {
            int drawY = y + row * scale;
            unsigned int rowBitmap = std::stoul(glyph.bitmap[row], nullptr, 16);

            for (int col = 0; col < glyph.width; col++)
            {
                int drawX = x + col * scale;

                if (rowBitmap & (1 << (8 - 1 - col)))
                {
                    for (int i = 0; i < scale; i++)
                    {
                        for (int j = 0; j < scale; j++)
                        {
                            buffer.drawASafe(drawX + i, drawY + j, color.r, color.g, color.b);
                        }
                    }
                }
            }
        }
        x += glyph.width * scale;
    }
}

std::unordered_map<char, Glyph> font = loadBDF("D:\\Code\\Projects\\Game Development\\SnakeCpp\\game\\static\\tom-thumb.bdf");
bool initGameplay()
{
    apple.changeLocation(false);
    Point snakeHead = {snake.x_pos, snake.y_pos};
    snakeVector.push_back(snakeHead);
    data = {};
    return true;
}

bool gameplayFrame(float deltaTime, int w, int h, Input &input, GameWindowBuffer &gameWindowBuffer)
{
    data.fpsCounter += 1;
    data.timer = deltaTime;
    if (data.timer > 1)
    {
        data.timer -= 1;
        std::cout << "FPS: " << data.fpsCounter << std::endl;
        data.fpsCounter = 0;
    }

    gameWindowBuffer.clear();

#pragma region update

    snakeVectorTemp = snakeVector;
    snake.move(deltaTime, input);
    // collision
    if (snake.x_pos == apple.pos_x && snake.y_pos == apple.pos_y)
    {
        apple.changeLocation(true);
    }
    if (snake.x_pos > w || snake.x_pos < 0 || snake.y_pos > h || snake.y_pos < 0)
    {
        endGame();
    }

    std::cout << snakeVector.size() << std::endl;

#pragma endregion update

#pragma region draw

    // tiles
    int tiles_in_row = w / 50;
    int tiles_in_column = h / 50;
    RGB tile_color_1 = {26, 156, 52};
    RGB tile_color_2 = {25, 97, 40};

    for (int y = 0; y < tiles_in_column; y++)
    {
        for (int x = 0; x < tiles_in_row; x++)
        {
            for (int i = 50 * x; i < 50 * x + 50; i++)
            {
                for (int j = 50 * y; j < 50 * y + 50; j++)
                {
                    if (y % 2 != 0)
                    {
                        if (x % 2 != 0)
                        {
                            gameWindowBuffer.drawASafe(i, j, tile_color_2.r, tile_color_2.g, tile_color_2.b);
                        }
                        else
                        {
                            gameWindowBuffer.drawASafe(i, j, tile_color_1.r, tile_color_1.g, tile_color_1.b);
                        }
                    }
                    else
                    {
                        if (x % 2 != 0)
                        {
                            gameWindowBuffer.drawASafe(i, j, tile_color_1.r, tile_color_1.g, tile_color_1.b);
                        }
                        else
                        {
                            gameWindowBuffer.drawASafe(i, j, tile_color_2.r, tile_color_2.g, tile_color_2.b);
                        }
                    }
                }
            }
        }
    }

    // snake segments
    for (int segment_index = 0; segment_index < snakeVector.size(); segment_index++)
    {
        for (int x = snakeVector[segment_index].x; x < snakeVector[segment_index].x + snake.width; x++)
        {
            for (int y = snakeVector[segment_index].y; y < snakeVector[segment_index].y + snake.height; y++)
            {
                gameWindowBuffer.drawASafe(x, y, snake.color.r, snake.color.g, snake.color.b);
            }
        }
    }

    // apple
    for (int i = apple.pos_x; i < apple.pos_x + 50; i++)
    {
        for (int j = apple.pos_y; j < apple.pos_y + 50; j++)
        {
            gameWindowBuffer.drawASafe(i, j, 255, 0, 0);
        }
    }

    // score
    drawText(gameWindowBuffer, 15, 15, "s c o r e : " + std::to_string(score), font, {0, 0, 0}, 7);

#pragma endregion draw

    return true;
}

void closeGameLogic()
{
}
