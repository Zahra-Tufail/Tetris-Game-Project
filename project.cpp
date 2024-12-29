#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace sf;
using namespace std;

const int WIDTH = 10;      // Grid width
const int HEIGHT = 20;     // Grid height
const int BLOCK_SIZE = 30; // Block size in pixels

// Define Tetromino shapes using 4 blocks
int tetrominoShapes[7][4] = 
{
    {1, 3, 5, 7},       // I shape
    {2, 4, 5, 7},       // Z shape
    {3, 5, 4, 6},       // S shape
    {3, 5, 4, 7},       // T shape
    {2, 3, 5, 7},       // L shape
    {3, 5, 7, 6},       // J shape
    {2, 3, 4, 5}        // O shape
};

class Tetris
{
private:
    bool gameOver;
    RenderWindow window;
    vector<vector<int>> grid;
    RectangleShape block;

    int currentShape;
    int rotation;
    int x, y; // Position of the current piece
    int score; // Player's score

    // Colors for different Tetrominoes
    Color colors[7] = 
    {
        
        Color(144,238,144),  // I
        Color(127,0,255),   // Z
        Color(1,50,32), // S
        Color::Yellow, // T
        Color::Black,  // L
        Color(199, 110, 10), // J
        Color(72,60,72) // O
    };

    Clock fallClock; // Timer for controlling block fall speed
    float fallTime;   // Time between automatic falls
    const float fallSpeed = 0.5f; // Control the speed of block fall

    Font font;
    Text scoreText;

public:
    Tetris()
    {
        window.create(VideoMode(WIDTH * BLOCK_SIZE, HEIGHT * BLOCK_SIZE), "Tetris");
        window.setFramerateLimit(60);

        // Initialize the grid with zeros (empty spaces)
        grid.resize(HEIGHT, vector<int>(WIDTH, 0));

        block.setSize(Vector2f(BLOCK_SIZE - 1, BLOCK_SIZE - 1)); // Block size with margin
        block.setFillColor(Color::White);

        srand(time(0)); // Seed for random generation
        spawnNewPiece();

        fallTime = 0.0f;
        score = 0; // Initialize score
        gameOver = false;

        if (!font.loadFromFile("ELEPHNT.ttf")) 
        {
            cerr << "Error loading font" << endl;
            exit(1);
        }

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(10, 10);
        updateScoreText();
    }

    void updateScoreText() 
    {
        scoreText.setString("Score: " + to_string(score));
    }

    void spawnNewPiece() 
    {
        currentShape = rand() % 7; // Randomly select a shape
        x = WIDTH / 2 - 2; // Start at the middle of the grid
        y = 0; // Start at the top of the grid
        for (int i = 0; i < 4; ++i)
        {
            int px = x + (tetrominoShapes[currentShape][i] % 2);
            int py = y + (tetrominoShapes[currentShape][i] / 2);

            // Ensure only rows within the grid are checked
            if (py >= 0 && grid[py][px] != 0)
            {
                gameOver = true; // Trigger game over if collision is detected
                return;
            }
        }
    }
    bool checkCollision()
    {
        // Check if the current piece collides with the walls or other blocks
        for (int i = 0; i < 4; ++i) 
        {
            int px = x + (tetrominoShapes[currentShape][i] % 2);           // px is the current cordinate of block
            int py = y + (tetrominoShapes[currentShape][i] / 2);          // y is the veritcal position of the piece 
            if (px < 0 || px >= WIDTH || py >= HEIGHT || (py >= 0 && grid[py][px]))
                // px  Checks if the block is outside the left boundary ,py  Checks if the block is below the bottom boundary
            {                                                           //grid[py][px] checks if the grid is already occupied
                return true;
            }
        }
        return false;
    }

    void rotatePiece() {
        int oldRotation = rotation;
        rotation = (rotation + 1) % 4;

        // Check if the rotation causes a collision
        if (checkCollision()) {
            rotation = oldRotation; // Revert to the previous rotation
        }
    }


    void movePiece(int dx, int dy)
    {
        x += dx;
        y += dy;

        if (checkCollision())
        {
            x -= dx;
            y -= dy;
            if (dy > 0)
            { // Piece reached the bottom, so place it on the grid
                placePiece();
                spawnNewPiece();
            }
        }
    }
    void placePiece()
    {
        for (int i = 0; i < 4; ++i)
        {
            int px = x + (tetrominoShapes[currentShape][i] % 2);
            int py = y + (tetrominoShapes[currentShape][i] / 2);

            // Debugging output
            cout << "Placing block at: (" << px << ", " << py << ")" << endl;

            grid[py][px] = currentShape + 1; // Store the piece number
        }

        clearFullLines();

        // Check for game over (any block in the top row)
        for (int j = 0; j < WIDTH; ++j)
        {
            if (grid[0][j] != 0)
            { // If any cell in the top row is occupied
                cout << "Game over: Block at top row" << endl; // Debugging output
                gameOver = true;
                return;
            }
        }
    }


    void clearFullLines()
    {
        for (int i = HEIGHT - 1; i >= 0; --i)   // starts at bottom row and moves upward
        {
            bool fullLine = true;
            for (int j = 0; j < WIDTH; ++j)
            {
                if (grid[i][j] == 0)
                {
                    fullLine = false;
                    break;
                }
            }
            if (fullLine)
            {
                for (int k = i; k > 0; --k)
                {
                    grid[k] = grid[k - 1];
                }
                grid[0] = vector<int>(WIDTH, 0); // Clear the top row by setting all values to 0
                score += 10; // Increment score for clearing a line
                updateScoreText();
                i++; // Recheck this row
            }
        }
    }

    void draw()
    {
        window.clear(Color(201, 169, 166)); // adding background color.
        // Draw the grid
        for (int i = 0; i < HEIGHT; ++i) 
        {
            for (int j = 0; j < WIDTH; ++j)
            {
                if (grid[i][j] != 0) 
                {
                    block.setFillColor(colors[grid[i][j] - 1]);
                   
                    block.setOutlineColor(Color::White);    // Outline color for the falling piece
                    block.setOutlineThickness(1);           // Outline thickness for the falling piece
                    block.setPosition(j * BLOCK_SIZE, i * BLOCK_SIZE);
                    window.draw(block);

                }
            }
        }
        if (!gameOver)
        {
            for (int i = 0; i < 4; ++i) 
            {
                int px = x + (tetrominoShapes[currentShape][i] % 2);
                int py = y + (tetrominoShapes[currentShape][i] / 2);
                block.setFillColor(colors[currentShape]);
                block.setPosition(px * BLOCK_SIZE, py * BLOCK_SIZE);
                window.draw(block);
            }
        }
        else
        {
            // Draw game over message
            Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(36);
            gameOverText.setFillColor(Color::Red);
            gameOverText.setString("Game Over");
            gameOverText.setPosition(WIDTH * BLOCK_SIZE / 2 - 100, HEIGHT * BLOCK_SIZE / 2 - 50);
            window.draw(gameOverText);
        }
        window.draw(scoreText);

        window.display();
    }

    void run()
    {
        while (window.isOpen())  
        {
            Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == Event::Closed)
                    window.close();
             
                if (!gameOver && event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Left)
                        movePiece(-1, 0); // Move left
                    if (event.key.code == Keyboard::Right)
                        movePiece(1, 0);  // Move right
                    if (event.key.code == Keyboard::Down)
                        movePiece(0, 1);  // Move down
                    if (event.key.code == Keyboard::Up)
                        rotatePiece();    // Rotate piece
                }
            }

            if (!gameOver)
            {
                // Handle automatic falling with fallClock
                if (fallClock.getElapsedTime().asSeconds() >= fallSpeed)  //fallClock to track elapsed time since the last fall.
                {
                    movePiece(0, 1); // Move the piece down
                    fallClock.restart(); // Reset the clock for next fall
                }
            }
            else
            {
                cout << "Game Over: No further actions allowed." << endl; // Debugging output
            }

            draw();
        }
    }

};

int main() 
{
    Tetris game;
    game.run();
    return 0;
}

