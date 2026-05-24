#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 900
#define HEIGHT 600

#define NUM_PARTICLES 100
#define GRAVITY 0.1f
#define DAMPENING_FACTOR 0.98f
#define SPEED 15

typedef struct {
    float x, y;
    float r;
    float vx, vy;
} Particle;

Particle particles[NUM_PARTICLES];

void DrawParticle(Particle* particle)
{
    DrawCircle((int)particle->x, (int)particle->y, particle->r, RAYWHITE);
}

void UpdateParticle(Particle* particle)
{
    particle->vy += GRAVITY;

    particle->x += particle->vx;
    particle->y += particle->vy;

    float r = particle->r;

    bool collision = false;

    // left wall
    if (particle->x - r < 0)
    {
        particle->x = r;
        particle->vx = -particle->vx;
        collision = true;
    }

    // right wall
    if (particle->x + r > WIDTH)
    {
        particle->x = WIDTH - r;
        particle->vx = -particle->vx;
        collision = true;
    }

    // bottom wall
    if (particle->y + r > HEIGHT)
    {
        particle->y = HEIGHT - r;
        particle->vy = -particle->vy;

        // helps particles settle
        if (fabsf(particle->vy) < 0.2f)
            particle->vy = 0;

        collision = true;
    }

    // top wall
    if (particle->y - r < 0)
    {
        particle->y = r;
        particle->vy = -particle->vy;
        collision = true;
    }

    if (collision)
    {
        particle->vx *= DAMPENING_FACTOR;
        particle->vy *= DAMPENING_FACTOR;
    }
}

void DrawParticles()
{
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        DrawParticle(&particles[i]);
    }
}

void UpdateParticles()
{
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        UpdateParticle(&particles[i]);
    }
}

void CollideAllParticles()
{
    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        for (int j = i + 1; j < NUM_PARTICLES; j++)
        {
            Particle* p1 = &particles[i];
            Particle* p2 = &particles[j];

            float dx = p1->x - p2->x;
            float dy = p1->y - p2->y;

            float distance = sqrtf(dx * dx + dy * dy);

            if (distance == 0.0f)
                continue;

            float minDistance = p1->r + p2->r;

            if (distance < minDistance)
            {
                // normalized collision normal
                float nx = dx / distance;
                float ny = dy / distance;

                // separate overlapping particles
                float overlap = minDistance - distance;

                p1->x += nx * overlap * 0.5f;
                p1->y += ny * overlap * 0.5f;

                p2->x -= nx * overlap * 0.5f;
                p2->y -= ny * overlap * 0.5f;

                // relative velocity
                float rvx = p1->vx - p2->vx;
                float rvy = p1->vy - p2->vy;

                // velocity along collision normal
                float velocityAlongNormal = rvx * nx + rvy * ny;

                // already separating
                if (velocityAlongNormal > 0)
                    continue;

                // tangent vector
                float tx = -ny;
                float ty = nx;

                // decompose velocities
                float v1n = p1->vx * nx + p1->vy * ny;
                float v1t = p1->vx * tx + p1->vy * ty;

                float v2n = p2->vx * nx + p2->vy * ny;
                float v2t = p2->vx * tx + p2->vy * ty;

                // swap normal components (equal masses)
                float temp = v1n;
                v1n = v2n;
                v2n = temp;

                // reconstruct velocities
                p1->vx = tx * v1t + nx * v1n;
                p1->vy = ty * v1t + ny * v1n;

                p2->vx = tx * v2t + nx * v2n;
                p2->vy = ty * v2t + ny * v2n;

                // dampening
                p1->vx *= DAMPENING_FACTOR;
                p1->vy *= DAMPENING_FACTOR;

                p2->vx *= DAMPENING_FACTOR;
                p2->vy *= DAMPENING_FACTOR;
            }
        }
    }
}

void InitParticles()
{
    SetRandomSeed((unsigned int)time(NULL));

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        float radius = (float)GetRandomValue(10, 20);

        particles[i].r = radius;

        particles[i].x = (float)GetRandomValue((int)radius, WIDTH - (int)radius);
        particles[i].y = (float)GetRandomValue((int)radius, HEIGHT - (int)radius);

        particles[i].vx = (float)GetRandomValue(-SPEED, SPEED);
        particles[i].vy = (float)GetRandomValue(-SPEED, SPEED);
    }
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Particle Simulation in C");

    SetTargetFPS(60);

    InitParticles();

    while (!WindowShouldClose())
    {
        UpdateParticles();
        CollideAllParticles();

        BeginDrawing();

        ClearBackground(BLACK);

        DrawParticles();

        DrawFPS(5, 5);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}