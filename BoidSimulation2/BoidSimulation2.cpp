#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

// ------------------------------------
// Vector2 Class (2D Vektör İşlemleri)
// ------------------------------------
struct Vector2
{
    float x;
    float y;

    Vector2(float x = 0, float y = 0) : x(x), y(y) {}

    float length() const
    {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalize() const
    {
        float len = length();
        if (len > 0)
            return *this * (1.0f / len);
        return Vector2(0, 0);
    }

    float distance(const Vector2& other) const
    {
        return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }

    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const
    {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }
};

// ------------------------------------
// Boid Class (Boid Hareketi)
// ------------------------------------
class Boid
{
public:
    Vector2 position;
    Vector2 velocity;

    Boid(float x, float y) : position(x, y), velocity(0, 0) {}

    void updatePosition(const std::vector<Boid>& boids,
        float separationDistance,
        float alignmentDistance,
        float cohesionDistance)
    {
        Vector2 sepForce = separation(boids, separationDistance);
        Vector2 aliForce = alignment(boids, alignmentDistance);
        Vector2 cohForce = cohesion(boids, cohesionDistance);

        // Kuvvetleri ekle
        velocity = velocity + sepForce + aliForce + cohForce;

        // **Hız Sınırlandırma**
        float maxSpeed = 2.0f;
        if (velocity.length() > maxSpeed)
        {
            velocity = velocity.normalize() * maxSpeed;
        }

        // Konumu güncelle
        position = position + velocity;
    }

    Vector2 separation(const std::vector<Boid>& boids, float desiredSeparation);
    Vector2 alignment(const std::vector<Boid>& boids, float neighborDistance);
    Vector2 cohesion(const std::vector<Boid>& boids, float neighborDistance);
};

// **Separation (Boid'ler çarpışmasın)**
Vector2 Boid::separation(const std::vector<Boid>& boids, float desiredSeparation)
{
    Vector2 steer(0, 0);
    int count = 0;

    for (const auto& other : boids)
    {
        float d = position.distance(other.position);
        if (d > 0 && d < desiredSeparation)
        {
            Vector2 diff = position - other.position;
            diff = diff * (1.0f / d);
            steer = steer + diff;
            count++;
        }
    }

    if (count > 0)
    {
        steer = steer * (1.0f / count);
    }
    return steer;
}

// **Alignment (Boid'ler hizalansın)**
Vector2 Boid::alignment(const std::vector<Boid>& boids, float neighborDistance)
{
    Vector2 avgVelocity(0, 0);
    int count = 0;

    for (const auto& other : boids)
    {
        float d = position.distance(other.position);
        if (d > 0 && d < neighborDistance)
        {
            avgVelocity = avgVelocity + other.velocity;
            count++;
        }
    }

    if (count > 0)
    {
        avgVelocity = avgVelocity * (1.0f / count);
        avgVelocity = avgVelocity - velocity;
    }

    return avgVelocity * 0.02f;
}

// **Cohesion (Boid'ler birbirine yaklaşsın)**
Vector2 Boid::cohesion(const std::vector<Boid>& boids, float neighborDistance)
{
    Vector2 centerOfMass(0, 0);
    int count = 0;

    for (const auto& other : boids)
    {
        float d = position.distance(other.position);
        if (d > 0 && d < neighborDistance)
        {
            centerOfMass = centerOfMass + other.position;
            count++;
        }
    }

    if (count > 0)
    {
        centerOfMass = centerOfMass * (1.0f / count);
        Vector2 cohesionVector = centerOfMass - position;
        return cohesionVector * 0.02f;
    }

    return Vector2(0, 0);
}

// ------------------------------------
// SFML ile Görselleştirme
// ------------------------------------
int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Boids Simulation");
    window.setFramerateLimit(60);

    std::vector<Boid> flock;
    flock.push_back(Boid(100, 100));
    flock.push_back(Boid(200, 200));
    flock.push_back(Boid(300, 300));
    flock.push_back(Boid(400, 400));
    flock.push_back(Boid(500, 500));
    flock.push_back(Boid(600, 600));

    // Başlangıç hızlarını belirleyelim
    flock[0].velocity = Vector2(1.5f, -0.5f);
    flock[1].velocity = Vector2(-1.0f, 0.8f);
    flock[2].velocity = Vector2(0.3f, -1.2f);
    flock[3].velocity = Vector2(0.0f, 1.0f);
    flock[4].velocity = Vector2(0.5f, 0.5f);
    flock[5].velocity = Vector2(0.5f, 0.5f);

    sf::CircleShape boidShape(5.f);
    boidShape.setOrigin(5.f, 5.f);
    boidShape.setFillColor(sf::Color::Yellow);

    float separationDist = 20.0f;
    float alignmentDist = 50.0f;
    float cohesionDist = 70.0f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Boid'leri güncelle
        for (auto& boid : flock)
        {
            boid.updatePosition(flock, separationDist, alignmentDist, cohesionDist);

            // **Sınırları kontrol et (Duvara çarpınca yön değiştir)**
            if (boid.position.x < 0)
            {
                boid.position.x = 0;
                boid.velocity.x *= -1;
            }
            else if (boid.position.x > 800)
            {
                boid.position.x = 800;
                boid.velocity.x *= -1;
            }

            if (boid.position.y < 0)
            {
                boid.position.y = 0;
                boid.velocity.y *= -1;
            }
            else if (boid.position.y > 600)
            {
                boid.position.y = 600;
                boid.velocity.y *= -1;
            }
        }

        window.clear(sf::Color::Black);

        for (const auto& boid : flock)
        {
            boidShape.setPosition(boid.position.x, boid.position.y); // burada Boid'leri shape kazandırır.
            window.draw(boidShape);
        }

        window.display();
    }

    return 0;
}
