// spacesim.cpp
// Copyright (c) 2026 rocketmike12
// Licensed under the MIT License (see LICENSE)

#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

const int WIDTH = 512;
const int HEIGHT = 512;

struct Vector2D {
	float x;
	float y;
};

struct Body {
	float x;
	float y;
	float m;
	Vector2D v;
};

const float G = 10.0f;
const float EPSILON = 30.0f;
const float dT = 0.01667f;

int main (void) {
    InitWindow(WIDTH, HEIGHT, "space");
    SetTargetFPS(60);

	vector<Body> system = {
		{ 240, 240, 5000, { 10.0f, 10.0f }},
		{ 272, 272, 5000, { -10.0f, -10.0f }},
		{ 282, 256, 5000, { -10.0f, -2.0f }}
	};

    while (!WindowShouldClose()) {
		for (int i = 0; i < (int)system.size(); i++) {
			float total_aX = 0.0f;
			float total_aY = 0.0f;

			for (int j = 0; j < (int)system.size(); j++) {
				if (i == j) continue;

				float dX = system[j].x - system[i].x;
				float dY = system[j].y - system[i].y;
				float r = sqrt((dX*dX) + (dY*dY));

				float F = G * ((system[i].m * system[j].m) / ((r*r) + EPSILON));

				total_aX += (F * dX) / (system[i].m * r);
				total_aY += (F * dY) / (system[i].m * r);
			}

			cout << "{ " << total_aX << " " << total_aY << " }" << "\n";

			system[i].v.x += total_aX * dT;
			system[i].v.y += total_aY * dT;
		}

		for (auto &body: system) {
			body.x += body.v.x * dT;
			body.y += body.v.y * dT;
		}

        BeginDrawing();
        ClearBackground(BLACK);

		for (const auto body : system) {
            // DrawPixel(body.x, body.y, WHITE);
			DrawRectangle(body.x, body.y, 2, 2, WHITE);
		}

        EndDrawing();
	}

    CloseWindow();

	return 0;
}
