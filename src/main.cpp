#include <iostream>
#include <string>
#include <random>
#include <cmath>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "ResourcePath.hpp"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define PADDLE_SIZE sf::Vector2f(WINDOW_WIDTH * 0.025f, WINDOW_HEIGHT * 0.17f)
#define PADDLE_VELOCITY 4

#define BALL_RADIUS 20

int p1Score;
int p2Score;
int p1Velocity;
int p2Velocity;
sf::Vector2f ballVelocity(0, 0);

sf::Font defaultFont;
sf::Text p1ScoreText;
sf::Text p2ScoreText;
sf::RectangleShape p1Paddle;
sf::RectangleShape p2Paddle;
sf::CircleShape ball;
sf::Sound sound;
sf::SoundBuffer collisionSoundBuffer;
sf::SoundBuffer scoreSoundBuffer;

enum Direction
{
    DirectionLeft,
    DirectionRight
};

void spawnBall(Direction direction)
{
    ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
    std::random_device rd;
    std::mt19937 re(rd());
    
    std::uniform_real_distribution<float> ufx(2, 4);
    
    ballVelocity.x = ufx(re);
    
    std::uniform_real_distribution<float> ufy(1, 2);
    
    ballVelocity.y = ufy(re);
    
    if (direction == DirectionLeft) {
        ballVelocity.x = -ballVelocity.x;
    }
}

void load()
{
    p1Score = 0;
    p2Score = 0;
    p1Velocity = 0;
    p2Velocity = 0;
    
    p1ScoreText.setFont(defaultFont);
    p1ScoreText.setCharacterSize(50);
    p1ScoreText.setColor(sf::Color::Red);
    p1ScoreText.setPosition(WINDOW_WIDTH * 0.25f, WINDOW_HEIGHT * 0.08f);
    
    p2ScoreText.setFont(defaultFont);
    p2ScoreText.setCharacterSize(50);
    p2ScoreText.setColor(sf::Color::Blue);
    p2ScoreText.setPosition(WINDOW_WIDTH * 0.75f, WINDOW_HEIGHT * 0.08f);
    
    float positionY = WINDOW_HEIGHT / 2 - PADDLE_SIZE.y / 2;
    
    p1Paddle.setFillColor(sf::Color::Red);
    p1Paddle.setSize(PADDLE_SIZE);
    p1Paddle.setPosition(0, positionY);
    
    p2Paddle.setFillColor(sf::Color::Blue);
    p2Paddle.setSize(PADDLE_SIZE);
    p2Paddle.setPosition(WINDOW_WIDTH - PADDLE_SIZE.x, positionY);
    
    ball.setFillColor(sf::Color::White);
    ball.setOutlineThickness(2.0f);
    ball.setOutlineColor(sf::Color(50, 50, 50));
    ball.setRadius(BALL_RADIUS);
    ball.setOrigin(BALL_RADIUS, BALL_RADIUS);
}

void keyDown(sf::Keyboard::Key &code, sf::Event::KeyEvent &event, sf::RenderWindow &window)
{
    if (code == sf::Keyboard::Escape) {
        window.close();
    
    } else if (code == sf::Keyboard::W) {
        p1Velocity -= PADDLE_VELOCITY;
        
    } else if (code == sf::Keyboard::S) {
        p1Velocity += PADDLE_VELOCITY;
    
    } else if (code == sf::Keyboard::Up) {
        p2Velocity -= PADDLE_VELOCITY;
        
    } else if (code == sf::Keyboard::Down) {
        p2Velocity += PADDLE_VELOCITY;
    }
}

void keyUp(sf::Keyboard::Key &code, sf::Event::KeyEvent &event, sf::RenderWindow &window)
{
    if (code == sf::Keyboard::W) {
        p1Velocity += PADDLE_VELOCITY;
        
    } else if (code == sf::Keyboard::S) {
        p1Velocity -= PADDLE_VELOCITY;
        
    } else if (code == sf::Keyboard::Up) {
        p2Velocity += PADDLE_VELOCITY;
        
    } else if (code == sf::Keyboard::Down) {
        p2Velocity -= PADDLE_VELOCITY;
    }
}

void update()
{
    sf::Vector2f p1Position = p1Paddle.getPosition();
    p1Position.y += p1Velocity;
    
    if (p1Position.y < 0.0f || p1Position.y > (WINDOW_HEIGHT - PADDLE_SIZE.y)) {
        p1Position.y += -p1Velocity;
    }
    
    p1Paddle.setPosition(p1Position);
    
    sf::Vector2f p2Position = p2Paddle.getPosition();
    p2Position.y += p2Velocity;
    
    if (p2Position.y < 0.0f || p2Position.y > (WINDOW_HEIGHT - PADDLE_SIZE.y)) {
        p2Position.y += -p2Velocity;
    }
    
    p2Paddle.setPosition(p2Position);
    
    sf::Vector2f ballPosition = ball.getPosition();
    ballPosition.x += ballVelocity.x;
    ballPosition.y += ballVelocity.y;
    ball.setPosition(ballPosition);
    
    bool collided = false;
    bool scored = false;
    
    if ((ballPosition.y - BALL_RADIUS) <= 0.0f ||
        (ballPosition.y + BALL_RADIUS) >= WINDOW_HEIGHT) {
        ballVelocity.y = -ballVelocity.y;
        collided = true;
    }
    
    if (ballVelocity.x < 0) {
        
        sf::Vector2f collision(ballPosition.x - BALL_RADIUS, ballPosition.y);
        
        if (collision.x <= PADDLE_SIZE.x) {
            
            if (collision.y >= p1Position.y &&
                collision.y <= (p1Position.y + PADDLE_SIZE.y)) {
                ballVelocity.x = -ballVelocity.x;
                
                if (fabs(ballVelocity.x) < 10.0f) {
                    ballVelocity *= 1.1f;
                }
                
                collided = true;
                
            } else {
                p2Score += 1;
                scored = true;
                spawnBall(DirectionRight);
            }
            
        }
        
    } else {
        
        sf::Vector2f collision(ballPosition.x + BALL_RADIUS, ballPosition.y);
        
        if (collision.x >= (WINDOW_WIDTH - PADDLE_SIZE.x)) {
            
            if (collision.y >= p2Position.y &&
                collision.y <= (p2Position.y + PADDLE_SIZE.y)) {
                ballVelocity.x = -ballVelocity.x;
                
                if (fabs(ballVelocity.x) < 10.0f) {
                    ballVelocity *= 1.1f;
                }
                
                collided = true;
                
            } else {
                p1Score += 1;
                scored = true;
                spawnBall(DirectionLeft);
            }
            
        }
        
    }
    
    if (collided) {
        sound.setBuffer(collisionSoundBuffer);
        sound.setVolume(100);
        sound.play();
    }
    
    if (scored) {
        sound.setBuffer(scoreSoundBuffer);
        sound.setVolume(10);
        sound.play();
    }
    
    p1ScoreText.setString(std::to_string(p1Score));
    p2ScoreText.setString(std::to_string(p2Score));
}

void draw(sf::RenderWindow &window)
{
    sf::RectangleShape line;
    line.setFillColor(sf::Color(50, 50, 50));
    line.setSize(sf::Vector2f(1, WINDOW_HEIGHT));
    line.setPosition(PADDLE_SIZE.x, 0);
    
    window.draw(line);
    
    line.setFillColor(sf::Color::White);
    line.setPosition(WINDOW_WIDTH / 2, 0);
    
    window.draw(line);
    
    line.setFillColor(sf::Color(50, 50, 50));
    line.setPosition(WINDOW_WIDTH - PADDLE_SIZE.x, 0);
    
    window.draw(line);
    
    sf::CircleShape circle;
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineThickness(1.0f);
    circle.setOutlineColor(sf::Color::White);
    circle.setRadius(BALL_RADIUS * 2.5f);
    circle.setOrigin(circle.getRadius(), circle.getRadius());
    circle.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
    window.draw(circle);
    
    window.draw(p1Paddle);
    window.draw(p2Paddle);
    
    window.draw(ball);
    
    window.draw(p1ScoreText);
    window.draw(p2ScoreText);
}

int main(int, char const**)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    
    sf::Uint32 style = sf::Style::Titlebar | sf::Style::Close;
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pong", style, settings);
    
    sf::Image icon;
    
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);
    
    if (!defaultFont.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    
    if (!collisionSoundBuffer.loadFromFile(resourcePath() + "collision.ogg")) {
        return EXIT_FAILURE;
    }
    
    if (!scoreSoundBuffer.loadFromFile(resourcePath() + "score.ogg")) {
        return EXIT_FAILURE;
    }
    
    sf::Music music;
    
    if (!music.openFromFile(resourcePath() + "music.ogg")) {
        return EXIT_FAILURE;
    }
    
    music.setLoop(true);
    
    load();
    
    music.play();
    
    std::random_device rd;
    std::mt19937 re(rd());
    
    std::uniform_int_distribution<int> uf(0, 1);
    
    Direction initialDirection = DirectionLeft;
    
    if (uf(re) == 1) {
        initialDirection = DirectionRight;
    }
    
    spawnBall(initialDirection);
    
    while (window.isOpen()) {
        
        sf::Event event;
        
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                keyDown(event.key.code, event.key, window);
            
            } else if (event.type == sf::Event::KeyReleased) {
                keyUp(event.key.code, event.key, window);
            }
        }
        
        update();

        window.clear();

        draw(window);
        
        window.display();
    }

    return EXIT_SUCCESS;
}
