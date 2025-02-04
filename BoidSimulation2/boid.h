#ifndef BOID_H
#define BOID_H

struct Vector2 {
    float x, y;
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
};

class Boid {
public:
    Vector2 position;
    Boid(float x, float y) : position(x, y) {}
    void updatePosition() {
        position.x += 1;
        position.y += 1; // Basit bir hareket
    }
    Vector2 getPosition() { return position; }
};

#endif
