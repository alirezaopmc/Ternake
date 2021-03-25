#include <iostream>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <vector>
#include <cstdio>
#include <stdio.h>
#include <fstream>

using namespace std;


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

/**
 * Terminal utilities
 */
void line(int l = 40) { cout << string(l, '='); }

/**
 * ENUMS
 */
enum Cell { EMPTY = '-', WALL = '#', HEAD = 'X', BODY = '*', FOOD};
enum Direction { LEFT = 'a', RIGHT = 'd', UP = 'w', DOWN = 's', BOOM = 'p'};
enum State { NONE, SCORE, FAIL };

/**
 * Constants
 */
const int ROWS = 22;
const int COLS = 22;
const int WIN_SCORE = 60;
const int DELAY = 120;
const int INIT_X = 11;
const int INIT_Y = 11;

const bool LOG = false;

/**
 * Rendering
 */
const string foods = "=O";

/**
 * Helper Classes
 */
// Point
class Point { public: int x, y; Point(int x, int y) : x(x), y(y) {} };

/**
 * Unit Class
 */
class Unit {
    private:
    Point coords;
    Unit *next, *prev;

    public:
    Unit(Point coords, Unit* prev) : coords(coords), prev(prev) {};
    // void set(string property, Unit *object) { (property == "next" ? next : prev) = object; }
    // Unit *get(string property) { return (property == "next" ? next : prev); }
    static bool is(string attribute, Unit *unit) {
        return (attribute == "head" ? unit->prev == nullptr : unit->prev != nullptr);
    }
    int x() { return coords.x; }
    int y() { return coords.y; }
    // void setCoords(Point point) { coords = point; }

    friend class Snake;
    friend class SnakeGame;
};

/**
 * Snake Class
 */
class Snake {
    private:
    int length;
    Unit *head, *tail;

    public:
    Snake(int x, int y) : length(1), head(new Unit(Point(x, y), nullptr)), tail(head) {
        if (LOG) cout << "Instantiating a snake." << endl;
    }
    void push_back(int x, int y);
    void push_front(int x, int y);
    Unit *pop_back();
    Unit *pop_front();
    int size() { return length; }
    // Unit *getHead() { return head; }
    // void setHead(Unit *unit) { head = unit; }
    // Unit *getTail() { return tail; }
    // void setTail(Unit *unit) { tail = unit; }

    friend class SnakeGame;
};

/**
 * SnakeGame Class
 */
class SnakeGame {
    private:
    int rows, cols;
    int win_score;
    int delay;
    Snake *snake;
    Cell **grid;

    // States
    int turn;
    bool running;
    Direction direction;

    public:
    bool gameover() { return !running; }
    int snakeLength() { return snake->size(); }

    SnakeGame(int rows, int cols, int win_score, int delay, int initX, int initY) :
        rows(rows),
        cols(cols),
        win_score(win_score),
        delay(delay),
        snake(new Snake(initX, initY)),
        running(true),
        direction(UP),
        turn(0)
    {
        if (LOG) cout << "Instantiating a SnakeGame" << endl;
        // Memory allocation for grid; 2d array of characters
        if (LOG) cout << "Memory allocation for grid 2d array." << endl;

        cout << rows << endl;
        cout << cols << endl;

        grid = (Cell **) malloc(rows * sizeof(Cell *));
        for(int i = 0; i < rows; i++) {
            grid[i] = (Cell *) malloc(cols * sizeof(Cell));
        }
        if (LOG) cout << "|-> Done" << endl;
    };

    static SnakeGame loadMap(string mapPath) {
        if (LOG) line();
        if (LOG) cout << "Loading map: " << mapPath << endl;
        ifstream file;
        file.open(mapPath);

        int _rows;
        int _cols;
        int _win_score;
        int _delay;
        int _initX;
        int _initY;

        if (LOG) cout << "Reading data from the file..." << endl;
        file >> _rows;
        file >> _cols;
        file >> _win_score;
        file >> _delay;
        file >> _initX;
        file >> _initY;
        cout << "|-> Done." << endl;

        if (LOG) cout << "Creating a SnakeGame object.";
        SnakeGame game(_rows, _cols, _win_score, _delay, _initX, _initY);
        if (LOG) cout << "|-> Done" << endl;

        if (LOG) cout << "Assigning map tiles..." << endl;
        for(int i = 0; i < _rows; i++) {
            string s;
            file >> s;
            for(int j = 0; j < _cols; j++) {
                game.setCell(Point(i, j), s[j]);

            }
        }
        if (LOG) cout << "|-> Done" << endl;

        file.close();
        return game;
    }

    static SnakeGame defaultGame() {
        int _rows = 20;
        int _cols = 20;
        int _win_score = 60;
        int _delay = 150;
        int _initX = 10;
        int _initY = 10;

        SnakeGame game(_rows, _cols, _win_score, _delay, _initX, _initY);

        for(int i = 0; i < _rows; i++) {
            for(int j = 0; j < _cols; j++) {
                if (i == 0 || j == 0 || i == _rows-1 || j == _cols-1) {
                    game.setCell(Point(i, j), WALL);
                } else {
                    game.setCell(Point(i, j), EMPTY);
                }
            }
        }
        
        return game;
    }

    void execute();
    void init();
    void render();
    void input();
    void update();
    void move(Direction dir, bool food);
    Direction directionOfHead();
    Point getRelativeCoords(Direction dir, Unit *unit);
    Cell getCell(Point p) { return grid[p.x][p.y]; }
    void setCell(Point p, char c) { grid[p.x][p.y] = (Cell)c; }
    void putFood();
    void endGame();
};

/**
 * Utilities
 */
// Cursor Mover
void gotoXY(short x, short y) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursorCoord {x, y};
	SetConsoleCursorPosition(consoleHandle, cursorCoord);
}

// Opposite Directions
bool areOpposite(Direction a, Direction b) {
    return (
        (a == LEFT && b == RIGHT) ||
        (a == RIGHT && b == LEFT) ||
        (a == UP && b == DOWN) ||
        (a == DOWN && b == UP)
    );
}

int main() {
    // Game instance
    // string mapPath = "./SnakeRhenium/map1.txt";
    string mapPath = "./map1.txt";
    // SnakeGame game = SnakeGame::loadMap(mapPath);
    SnakeGame game = SnakeGame::defaultGame();
    game.execute();
}

/**
 * Snake Class Implementations
 */
void Snake::push_back(int x, int y) {
    Unit* unit = new Unit(Point(x, y), tail);
    // tail->set("next", unit);
    tail->next = unit;
    tail = unit;
    length++;
}

/**
 * 
 */
void Snake::push_front(int x, int y) {
    Unit *unit = new Unit(Point(x, y), nullptr);
    // unit->set("next", head);
    unit->next = head;
    // head->set("prev", unit);
    head->prev = unit;
    head = unit;
    length++;
}

Unit *Snake::pop_back() {
    Unit *unit = tail;
    // tail = tail->get("prev");
    tail = tail->prev;
    // unit->set("prev", nullptr);
    unit->prev = nullptr;
    // tail->set("next", nullptr);
    tail->next = nullptr;

    return unit;
}

Unit *Snake::pop_front() {
    Unit *unit = head;
    // head = head->get("next");
    head = head->next;
    // unit->set("next", nullptr);
    unit->next = nullptr;
    // head->set("prev", nullptr);
    head->prev = nullptr;
    return unit;
}

/**
 * SnakeGame Class Implementations
 */

/**
 * Executing the game
 */
void SnakeGame::execute() {
    if (LOG) cout << "Executing the game..." << endl;
    init();

    while(!gameover()) {
        render();
        input();
        update();
        Sleep(delay);
    }

    cout << RESET;

    cout << "Game has been executed for " << turn << " turns." << endl;
    endGame();

    if (LOG) cout << "|-> Game is finished" << endl;
}

/**
 * Initialization for the game system
 */
void SnakeGame::init() {
    system("MODE con");
    system("cls");
    putFood();
}

/**
 * Console output rendering
 */
void SnakeGame::render() {
    system("cls");
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {

            switch(grid[i][j]) {
                case FOOD:
                    cout << GREEN;
                    break;
                case WALL:
                    cout << BLUE;
                    break;
                case BODY: case HEAD:
                    cout << RED;
                    break;
                default:
                    cout << RESET;
            }

            if (grid[i][j] == FOOD) {
                cout << foods[turn % 2];
            } else {
                cout << (char)grid[i][j];
            }
            cout << " ";
        }
        cout << endl;
    }
    cout << "Score: " << snakeLength() << endl;
}

/**
 * Handle user's input
 */
void SnakeGame::input() {
    if (_kbhit()) {
        switch(_getch()) {
            case 'a':
                direction = LEFT;
                break;
            case 'd':
                direction = RIGHT;
                break;
            case 'w':
                direction = UP;
                break;
            case 's':
                direction = DOWN;
                break;
            case 'p':
                running = false;
                break;
            default:
                break;
        }
    }
}

/**
 * Logical check
 */
void SnakeGame::update() {
    if (gameover()) return; // Lose

    // Win
    if (snakeLength() >= win_score) {
        running = false;
        return;
    }

    Point target = getRelativeCoords(direction, snake->head);

    if (getCell(target) == WALL || getCell(target) == BODY) running = false;
    if (running) move(direction, getCell(target) == FOOD);

    turn++;
}

/**
 * Move Utilities
 */
void SnakeGame::move(Direction dir, bool food) {
    Direction headDirection = directionOfHead();
    int x = snake->head->x();
    int y = snake->head->y();

    int X = snake->tail->x();
    int Y = snake->tail->y();
    
    if (areOpposite(headDirection, dir)) {
        direction = headDirection;
    };

    Point p = getRelativeCoords(direction, snake->head);

    if (food) {
        grid[x][y] = BODY;

        snake->push_front(p.x, p.y);

    /*
        Unit *unit = new Unit(p, nullptr);
        // snake->getHead()->set("prev", unit);
        snake->head->prev = unit;
        // unit->set("next", snake->getHead());
        unit->next = snake->head;
        // snake->setHead(unit);
        snake->head = unit;

        int theX = snake->head->x();
        int theY = snake->head->y();
    */
        grid[p.x][p.y] = HEAD;
        putFood();
    } else {
        grid[X][Y] = EMPTY;

        if (snake->size() == 1) {
            grid[x][y] = EMPTY;
            grid[p.x][p.y] = HEAD;
            snake->head->coords = p;
            return;
        } else {
            grid[x][y] = BODY;
        }

        Unit *unit = snake->tail;
        unit->coords = p;
        snake->tail = snake->tail->prev;
        unit->prev = nullptr;
        unit->next = snake->head;
        snake->head->prev = unit;
        snake->head = unit;

        int theX = snake->head->x();
        int theY = snake->head->y();

        grid[theX][theY] = HEAD;
    }
}

/**
 * Put food
 */
void SnakeGame::putFood() {
    srand(time(0));
    int x = rand() % rows;
    int y = rand() % cols;

    while(grid[x][y] != EMPTY) {
        x = rand() % rows;
        y = rand() % cols;
    }
    
    grid[x][y] = FOOD;
}

/**
 * Direction of head
 */
Direction SnakeGame::directionOfHead() {
    if (snakeLength() < 2) return direction;

    Unit *next = snake->head->next;
    int nextX = next->x();
    int nextY = next->y();
    int currX = snake->head->x();
    int currY = snake->head->y();

    int deltaX = currX - nextX;
    int deltaY = currY - nextY;

    if (deltaX > 0) return DOWN;
    if (deltaX < 0) return UP;
    if (deltaY > 0) return RIGHT;
    if (deltaY < 0) return LEFT;

    return BOOM;
}

/**
 * Get the cell in which direction of the head
 */
Point SnakeGame::getRelativeCoords(Direction dir, Unit *unit) {
    int x = unit->x();
    int y = unit->y();
    
    switch(dir) {
        case LEFT:
            return Point(x, y-1);
        case RIGHT:
            return Point(x, y+1);
        case UP:
            return Point(x-1, y);
        case DOWN:
            return Point(x+1, y);
        default:
            return Point(x, y);
    }
}

/**
 * End Game
 */
void SnakeGame::endGame() {
    if (snakeLength() >= win_score) {
        cout << "You Won!" << endl;
    } else {
        cout << "You Lose!" << endl;
    }
}