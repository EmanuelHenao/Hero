#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>

using namespace sf;

const int M = 20; // Altura de la matriz
const int N = 10; // Ancho de la matriz
const int MAX_BULLETS = 25; // Número máximo de proyectiles
const float EXPLOSION_TIMER = 1.0f; // Tiempo antes de la explosión en segundos
const float GRAVITY = 5.8f; // Aceleración debida a la gravedad

int field[M][N] = {0};

struct Point {
    int x, y;
} a[4], b[4];

struct Bullet {
    Sprite sprite;
    Vector2f velocity;
    bool active;
};

struct Explosive {
    Sprite sprite;
    Clock timer;
    bool active;
};

struct Enemy {
    Sprite sprite;
    Vector2f velocity;
    bool active;
};

bool check() {
    for (int i = 0; i < 4; i++) {
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return false;
        else if (field[a[i].y][a[i].x]) return false;
    }
    return true;
}


int main() {
    srand(time(0));

    RenderWindow window(VideoMode(968, 760), "H.E.R.O.");

    // Cargar texturas 
    Texture t1, t2, bodyTexture, headTexture, bulletTexture, explosiveTexture, enemyTexture;
    t1.loadFromFile("niveles/personaje.png");
    t2.loadFromFile("niveles/level_01.bmp");
    bodyTexture.loadFromFile("niveles/body.png");
    headTexture.loadFromFile("niveles/head.png");
    bulletTexture.loadFromFile("niveles/bullet.png");
    explosiveTexture.loadFromFile("niveles/explosive.png");
    enemyTexture.loadFromFile("niveles/enemy.png");
    

    Sprite s(t1), background(t2), bodySprite(bodyTexture), headSprite(headTexture);
    Sprite bulletSprite(bulletTexture);
    Sprite explosiveSprite(explosiveTexture);
    Sprite enemySprite(enemyTexture);
    
    sf::Texture textureBloque1, textureBloque2, textureBloque3, textureBloque4, textureBloque5, textureBloque6, textureBloque7, textureBloque8, textureBloque9;
    sf::Sprite spriteBloque1, spriteBloque2, spriteBloque3, spriteBloque4, spriteBloque5, spriteBloque6, spriteBloque7, spriteBloque8, spriteBloque9;

    textureBloque1.loadFromFile("niveles/pene1.png");
    textureBloque2.loadFromFile("niveles/pene2.png");
    textureBloque3.loadFromFile("niveles/pene3.png");
    textureBloque4.loadFromFile("niveles/bloqu         e4.png");
    textureBloque5.loadFromFile("niveles/bloque5.png");
    textureBloque6.loadFromFile("niveles/bloque6.png");
    textureBloque7.loadFromFile("niveles/bloque7.png");
    textureBloque8.loadFromFile("niveles/bloque8.png");
    textureBloque9.loadFromFile("niveles/bloque9.png");

    spriteBloque1.setTexture(textureBloque1);
    spriteBloque2.setTexture(textureBloque2);
    spriteBloque3.setTexture(textureBloque3);
    spriteBloque4.setTexture(textureBloque4);
    spriteBloque5.setTexture(textureBloque5);
    spriteBloque6.setTexture(textureBloque6);
    spriteBloque7.setTexture(textureBloque7);
    spriteBloque8.setTexture(textureBloque8);
    spriteBloque9.setTexture(textureBloque9);


    // Cargar sonidos
    SoundBuffer explosionBuffer, shootBuffer;
    explosionBuffer.loadFromFile("niveles/explosion.wav");
    shootBuffer.loadFromFile("niveles/shoot.wav");

    Sound explosionSound, shootSound;
    explosionSound.setBuffer(explosionBuffer);
    shootSound.setBuffer(shootBuffer);
    
    
    
    int dx = 0, dy = 0;
    float timer = 0, delay = 0.4;
    float bulletSpeed = 0.8f; // Velocidad del proyectil reducida

    Clock clock;// punto de donde sale el jugador
    sf::Vector2f position(150, 140);
    float velocityY = 0.0f; // Velocidad inicial en el eje Y

    Bullet bullets[MAX_BULLETS] = {};
    Explosive explosive;

    Enemy enemy;
    enemy.sprite.setTexture(enemyTexture);
    enemy.sprite.setPosition(600, 125); // Posición inicial del enemigo
    enemy.active = true;
    enemy.velocity = Vector2f(-50.5f, 0.0f); // Velocidad inicial del enemigo
    
    // Posicionamiento de los bloques
    spriteBloque1.setPosition(8, 225); // Bloque1 en la esquina superior izquierda
    spriteBloque2.setPosition(510, 225);//
    spriteBloque3.setPosition(8, 52);//
    spriteBloque4.setPosition(335, 52);
    spriteBloque5.setPosition(8, 140);//
    spriteBloque6.setPosition(335, 52);//
    spriteBloque7.setPosition(8, 475);//
	spriteBloque8.setPosition(813, 52);//
	spriteBloque9.setPosition(860, 140 );
    	
	
    
    

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {
                switch (e.key.code) {
                    case Keyboard::Up:
                        position.y -= 5; // Mover 5 píxeles hacia arriba
                        dy = -1;
                        dx = 0;
                        break;
                    case Keyboard::Left:
                        position.x -= 5; // Mover 5 píxeles hacia la izquierda
                        dx = -1;
                        dy = 0;
                        break;
                    case Keyboard::Right:
                        position.x += 5; // Mover 5 píxeles hacia la derecha
                        dx = 1;
                        dy = 0;
                        break;
                    case Keyboard::Down:
                        if (!explosive.active) {
                            explosive.sprite = explosiveSprite;
                            explosive.sprite.setPosition(position.x - bodySprite.getLocalBounds().width / 8, position.y + bodySprite.getLocalBounds().height / 1.7);
                            explosive.active = true;
                            explosive.timer.restart();
                            explosionSound.play(); // Reproducir sonido de explosión
                        }
                        break;
                    case Keyboard::Space:
                        // Crear un nuevo proyectil
                        for (int i = 0; i < MAX_BULLETS; i++) {
                            if (!bullets[i].active) {
                                // Ajustar la posición de origen del láser para que esté más cerca del borde del cuerpo del personaje
                                float laserOriginX = position.x + (dx == -8 ? bodySprite.getLocalBounds().width : 0 ); // Si se mueve hacia la derecha, el láser sale desde el borde derecho del cuerpo
                                float laserOriginY = position.y + bodySprite.getLocalBounds().height / 2; // El láser sale desde la mitad de la altura del cuerpo

                                bullets[i].sprite = bulletSprite;
                                bullets[i].sprite.setPosition(laserOriginX, laserOriginY);
                                bullets[i].velocity = Vector2f(bulletSpeed * dx, bulletSpeed * dy); // Velocidad dependiendo de la dirección del muñeco
                                bullets[i].active = true;
                                shootSound.play();
								shootSound.play(); // Reproducir sonido de disparo
                                break;
                            }
                        }
                        break;
                }
                
            }
        }

        // Simular la caída del cuerpo y la cabeza
        velocityY += GRAVITY * time; // Aplicar la aceleración de la gravedad
        position.y += velocityY * time; // Actualizar la posición en función de la velocidad

        // Verificar si el personaje ha tocado el suelo
        if (position.y > window.getSize().y - bodySprite.getLocalBounds().height) {
            position.y = window.getSize().y - bodySprite.getLocalBounds().height; // Asegurar que no se vaya debajo del suelo
            velocityY = 0.0f; // Detener la caída
        }
        //colision
    if(bodySprite.getGlobalBounds().intersects(spriteBloque1.getGlobalBounds())){
    	
	}
        // Hacer todos los bloques sólidos
        spriteBloque1.setColor(sf::Color::White);
        spriteBloque2.setColor(sf::Color::White);
        spriteBloque3.setColor(sf::Color::White);
        spriteBloque4.setColor(sf::Color::White);
        spriteBloque5.setColor(sf::Color::White);
        spriteBloque6.setColor(sf::Color::White);
        spriteBloque7.setColor(sf::Color::White);
        spriteBloque8.setColor(sf::Color::White);
        spriteBloque9.setColor(sf::Color::White);


        // Asegúrate de que el muñeco no se salga de los límites de la ventana
        if (position.x < 0) position.x = 0;
        if (position.y < 0) position.y = 0;
        if (position.x > window.getSize().x - bodySprite.getLocalBounds().width) 
            position.x = window.getSize().x - bodySprite.getLocalBounds().width;

        // Movimiento del enemigo
        if (enemy.active) {
          enemy.sprite.move(enemy.velocity * time);
          if (enemy.sprite.getPosition().x < 484 || enemy.sprite.getPosition().x > window.getSize().x - enemy.sprite.getLocalBounds().width) {
           // Si el enemigo alcanza los límites de la ventana, invierte su dirección
           enemy.velocity.x *= -1;
        }

        // Detección de colisiones entre láseres y enemigo
        for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active && bullets[i].sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
            // Si hay una colisión entre el láser y el enemigo
            bullets[i].active = false; // Desactivar el láser
            enemy.active = false; //Elimina el enemy
			enemy.sprite.setColor(sf::Color(255, 255, 255, 0)); // Opacidad completa (255) establecida a 0// Eliminar al enemigo
            
        }
    }
}



        // Voltear el sprite del cuerpo y la cabeza según la dirección
        if (dx < 0) {
            bodySprite.setScale(1, 1); // Restablecer la escala del cuerpo si se mueve hacia la izquierda
            headSprite.setScale(1, 1); // Restablecer la escala de la cabeza si se mueve hacia la  izquierda
        } else if (dx > 0) {
            bodySprite.setScale(-1, 1); // Voltear el cuerpo hacia la izquierda si se mueve hacia la derecha
            headSprite.setScale(-1, 1); // Voltear la cabeza hacia la izquierda si se mueve hacia la derecha
        }

        bodySprite.setPosition(position);
        headSprite.setPosition(position.x + 1, position.y - 1); // Ajusta la posición de la cabeza

        // Verificar si el explosivo está activo y ha pasado el tiempo de explosión
        if (explosive.active && explosive.timer.getElapsedTime().asSeconds() >= EXPLOSION_TIMER) {
            // Implementar la lógica de la explosión aquí
            explosive.active = false; // Desactivar el explosivo después de la explosión
        }

        window.clear(Color::White);
        window.draw(background);

        // Dibujar el muñeco
        window.draw(bodySprite);
        window.draw(headSprite);

        // Dibujar el explosivo si está activo
        if (explosive.active) {
            window.draw(explosive.sprite);
        }
        
       
        // Dibujar los bloques
        window.draw(spriteBloque1);
        window.draw(spriteBloque2);
        window.draw(spriteBloque3);
        window.draw(spriteBloque4);
        window.draw(spriteBloque5);
        window.draw(spriteBloque6);
        window.draw(spriteBloque7);
        window.draw(spriteBloque8);
        window.draw(spriteBloque9);
        
        
        // Dibujar al enemigo
        window.draw(enemy.sprite);

        // Dibujar los proyectiles
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].sprite.move(bullets[i].velocity);
                // Desactivar el proyectil si sale de la ventana
                if (bullets[i].sprite.getPosition().x > window.getSize().x || bullets[i].sprite.getPosition().y > window.getSize().y || bullets[i].sprite.getPosition().x < 0 || bullets[i].sprite.getPosition().y < 0) {
                    bullets[i].active = false;
                }
                window.draw(bullets[i].sprite);
            }
        }

        for (int i = 0; i < 4; i++) {
            s.setTextureRect(IntRect(0, 0, 18, 18));
            s.setPosition(a[i].x * 18, a[i].y * 18);
            s.move(28, 31); // offset
            window.draw(s);
        }

        window.display();
    }
    return 0;
}
