
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>


////////////////////////////////////////////////////////////
/// Project wong kito
///
/// Prabumulih, Sumsel
///
////////////////////////////////////////////////////////////
int main()
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    // Define some constants
    const float pi = 3.14159f;
    const int gameWidth = 800;
    const int gameHeight = 600;
    sf::Vector2f paddleSize(25, 100);
    float ballRadius = 10.f;

    // buat jendela aplikasi
    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML Pong",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    // Load suara yg di gunoke dalam game
    sf::SoundBuffer ballSoundBuffer;
    if (!ballSoundBuffer.loadFromFile("resources/ball.wav"))
        return EXIT_FAILURE;
    sf::Sound ballSound(ballSoundBuffer);

    // buat paddle kiri
    sf::RectangleShape leftPaddle;
    leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    leftPaddle.setOutlineThickness(3);
    leftPaddle.setOutlineColor(sf::Color::Black);
    leftPaddle.setFillColor(sf::Color(100, 100, 200));
    leftPaddle.setOrigin(paddleSize / 2.f);

    // buat paddle kanan
    sf::RectangleShape rightPaddle;
    rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
    rightPaddle.setOutlineThickness(3);
    rightPaddle.setOutlineColor(sf::Color::Black);
    rightPaddle.setFillColor(sf::Color(200, 100, 100));
    rightPaddle.setOrigin(paddleSize / 2.f);

    // buat bola
    sf::CircleShape ball;
    ball.setRadius(ballRadius - 3);
    ball.setOutlineThickness(3);
    ball.setOutlineColor(sf::Color::Black);
    ball.setFillColor(sf::Color::White);
    ball.setOrigin(ballRadius / 2, ballRadius / 2);

    // memuat font teks
    sf::Font font;
    if (!font.loadFromFile("resources/sansation.ttf"))
        return EXIT_FAILURE;

    // Inisialisasi jeda pesan
    sf::Text pauseMessage;
    pauseMessage.setFont(font);
    pauseMessage.setCharacterSize(40);
    pauseMessage.setPosition(170.f, 150.f);
    pauseMessage.setFillColor(sf::Color::White);
    pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game");

    // definisikan properti paddles
    sf::Clock AITimer;
    const sf::Time AITime   = sf::seconds(0.1f);
    const float paddleSpeed = 400.f;
    float rightPaddleSpeed  = 0.f;
    const float ballSpeed   = 400.f;
    float ballAngle         = 0.f; // untuk di ubah nanti

    sf::Clock clock;
    bool isPlaying = false;
    while (window.isOpen())
    {
        // menangani acara
        sf::Event event;
        while (window.pollEvent(event))
        {
            // jendela tertutup atau tekan tombol escape: keluar
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }

            // tekan tombol space: bermain
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
            {
                if (!isPlaying)
                {
                    // (re)start the game
                    isPlaying = true;
                    clock.restart();

                    // atur ulang posisi paddle dan bola
                    leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
                    rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
                    ball.setPosition(gameWidth / 2, gameHeight / 2);

                    // atur ulang sudut bola
                    do
                    {
                        // memastikan sudut awal bola tidak terlalu vertikal
                        ballAngle = (std::rand() % 360) * 2 * pi / 360;
                    }
                    while (std::abs(std::cos(ballAngle)) < 0.7f);
                }
            }
        }

        if (isPlaying)
        {
            float deltaTime = clock.restart().asSeconds();

            // Pindahkan paddle pemain
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
               (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
            {
                leftPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
               (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
            {
                leftPaddle.move(0.f, paddleSpeed * deltaTime);
            }

            //gerakkan paddle komputer
            if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
                ((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
            {
                rightPaddle.move(0.f, rightPaddleSpeed * deltaTime);
            }

            //Perbarui arah paddle komputer sesuai dengan posisi bola
            if (AITimer.getElapsedTime() > AITime)
            {
                AITimer.restart();
                if (ball.getPosition().y + ballRadius > rightPaddle.getPosition().y + paddleSize.y / 2)
                    rightPaddleSpeed = paddleSpeed;
                else if (ball.getPosition().y - ballRadius < rightPaddle.getPosition().y - paddleSize.y / 2)
                    rightPaddleSpeed = -paddleSpeed;
                else
                    rightPaddleSpeed = 0.f;
            }

            // gerakkan bola
            float factor = ballSpeed * deltaTime;
            ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

            //Periksa tabrakan antara bola dan layar
            if (ball.getPosition().x - ballRadius < 0.f)
            {
                isPlaying = false;
                pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
            }
            if (ball.getPosition().x + ballRadius > gameWidth)
            {
                isPlaying = false;
                pauseMessage.setString("You won!\nPress space to restart or\nescape to exit");
            }
            if (ball.getPosition().y - ballRadius < 0.f)
            {
                ballSound.play();
                ballAngle = -ballAngle;
                ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
            }
            if (ball.getPosition().y + ballRadius > gameHeight)
            {
                ballSound.play();
                ballAngle = -ballAngle;
                ball.setPosition(ball.getPosition().x, gameHeight - ballRadius - 0.1f);
            }

            //Periksa tumbukan antara bola dan paddles
            // paddle kiri
            if (ball.getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x / 2 &&
                ball.getPosition().x - ballRadius > leftPaddle.getPosition().x &&
                ball.getPosition().y + ballRadius >= leftPaddle.getPosition().y - paddleSize.y / 2 &&
                ball.getPosition().y - ballRadius <= leftPaddle.getPosition().y + paddleSize.y / 2)
            {
                if (ball.getPosition().y > leftPaddle.getPosition().y)
                    ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
                else
                    ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

                ballSound.play();
                ball.setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball.getPosition().y);
            }

            // paddle kanan
            if (ball.getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x / 2 &&
                ball.getPosition().x + ballRadius < rightPaddle.getPosition().x &&
                ball.getPosition().y + ballRadius >= rightPaddle.getPosition().y - paddleSize.y / 2 &&
                ball.getPosition().y - ballRadius <= rightPaddle.getPosition().y + paddleSize.y / 2)
            {
                if (ball.getPosition().y > rightPaddle.getPosition().y)
                    ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
                else
                    ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

                ballSound.play();
                ball.setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball.getPosition().y);
            }
        }

        // bersihkan jendela
        window.clear(sf::Color(50, 200, 50));

        if (isPlaying)
        {
            // gambar paddles dan bola
            window.draw(leftPaddle);
            window.draw(rightPaddle);
            window.draw(ball);
        }
        else
        {
            // gambar jeda pesan
            window.draw(pauseMessage);
        }

        // Tampilkan berbagai hal di layar
        window.display();
    }

    return EXIT_SUCCESS;
}
