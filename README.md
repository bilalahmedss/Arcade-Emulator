# Arcade Games Emulator

## Contributors
- **Bilal Ahmed**
- **Qazi Mustafa**
- **Simra Shamim**

## Overview
This project is a collection of popular classic games implemented in C++ as part of the **Object-Oriented Programming (OOP)** course. The project includes an emulator that allows users to play games such as **Snake**, **Tetris**, **Pong**, and **Brick Breaker**.

## Features

- **Multiple Games**: Includes **Snake**, **Tetris**, **Pong**, and **Brick Breaker**.
- **Graphics and Sound**: Utilizes **SDL** for handling images, sound files, and fonts to provide a rich user experience.


---

## **1. Pong**

### **Gameplay:**
- **Objective:** In **Pong**, two players control paddles and try to bounce a ball past the opponent. The player who fails to return the ball loses a point.
- **Player Controls:** Player 1 uses the **W** and **S** keys, while Player 2 uses **I** and **K** to control their paddles.
- **Ball Movement:** The ball moves across the screen, bouncing off the paddles and the top/bottom walls. The game keeps track of the scores, resetting the ball after each point.

### **OOP Concepts:**
- **Classes & Objects:** 
  - **Pong_Paddle** class for the paddles, which controls their movement and rendering.
  - **Pong_Ball** class for the ball's behavior, including its movement, collision detection, and resetting after scoring.
  - **Score** class to keep track of the scores for both players.

---

## **2. Brick Breaker**

### **Gameplay:**
- **Objective:** The player controls a paddle at the bottom of the screen, using it to bounce a ball that breaks the bricks above. The game ends when the player loses the ball.
- **Player Controls:** The paddle moves left and right with the **arrow keys**. The objective is to clear the screen by breaking all bricks.
- **Bricks:** Bricks are destroyed when the ball hits them, and the player scores points.

### **OOP Concepts:**
- **Classes & Objects:**
  - **Paddle** class to control the player’s paddle.
  - **Ball** class that handles the movement, collision with bricks and paddles, and scoring.
  - **Brick** class to represent individual bricks, including their position, color, and visibility.

---

## **3. Snake**

### **Gameplay:**
- **Objective:** The player controls a snake that grows longer as it eats apples. The game ends when the snake collides with itself.
- **Player Controls:** The snake is controlled with the **arrow keys**. Each apple eaten increases the snake's length.
- **Growth:** As the snake eats apples, it grows in size. The game uses a grid system, and the snake moves in segments, wrapping around the screen.

### **OOP Concepts:**
- **Linked List Implementation:** 
  - The snake is represented as a **linked list**, where each segment of the snake is a node. This allows for dynamic growth and movement of the snake.
  - The **Node** class represents each segment of the snake, containing the **x** and **y** positions.
  - The **Apple** class generates random apple positions and handles rendering.
  - **Direction Enum** defines the movement directions for better control over snake movement.

---

## **4. Tetris**

### **Gameplay:**
- **Objective:** The player controls falling tetrominoes and arranges them to complete horizontal lines, which are then cleared.
- **Player Controls:** The player can rotate and move the tetrominoes using the **arrow keys**.
- **Line Clearing:** When a line is filled with blocks, it clears, and the player earns points. The game ends when the blocks fill up to the top of the screen.

### **OOP Concepts:**
- **Classes & Objects:**
  - **Shape** class represents each type of tetromino and its rotation matrix.
  - **Board** class holds the grid, where shapes fall and checks for filled lines to clear.
  - **CheckMove** class handles movement logic, checking if a piece can move left, right, or down.
  - **LineFull** class checks for full lines and clears them.
  - **ShapePlacer** class places the piece onto the board and handles the game over condition.

---

## Usage

1. **Launch the Emulator** by running the `Emulator.exe` file.
2. **Choose a game** from the menu.
3. Use the **keyboard keys** to control the games:
   - For **Snake**: Arrow keys to control direction.
   - For **Tetris**: Arrow keys to move and rotate blocks.
   - For **Pong**: Use the paddle to move and hit the ball back.
   - For **Brick Breaker**: Use the paddle to bounce the ball and break blocks.

## Code Structure

- **Main Emulator**: `emulator.cpp`
  - Controls the game menu and game switching.
- **Game Implementations**: 
  - `brick_breaker.cpp`, `pong.cpp`, `snake.cpp`, `tetris.cpp` for individual game logic.
- **Game Header Files**: 
  - `brick_breaker.h`, `pong.h`, `snake.h`, `tetris.h` define the game classes and functions.
- **Utility Files**: 
  - `background.png`, `font.ttf`, `game.mp3` for game assets.

