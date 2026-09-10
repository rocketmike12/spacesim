// spacesim.cpp
// Copyright (c) 2026 rocketmike12
// Licensed under the MIT License (see LICENSE)

#include <algorithm>
#include <bits/stdc++.h>
#include <raylib.h>

using namespace std;

const int WIDTH = 1024;
const int HEIGHT = 1024;

struct Vector2D {
	float x;
	float y;
};

struct Body {
	float x;
	float y;
	float m;
	float R;
	Vector2D v;
};

const float G = 100.0f;
const float EPSILON = 700.0f;
const float dT = 0.01667f;

int main (int argc, char* argv[]) {
	bool warp = false;

	if (argc > 0) {
		for (int i = 0; i < argc; i++) {
			warp = warp || !strcmp(argv[i], "--warp");
		}
	}
	
    InitWindow(WIDTH, HEIGHT, "spacesim");
    SetTargetFPS(60);

	float halfWidth = (float)WIDTH / 2;
	float halfHeight = (float)HEIGHT / 2;

	vector<Body> system = {
		{ halfWidth - 64, halfHeight - 64, 10000, 2, { 20.0f, 20.0f }},
		{ halfWidth + 64, halfHeight + 64, 10000, 2, { -20.0f, -20.0f }},
		{ halfWidth + 104, halfHeight + 32, 10000, 2, { -20.0f, -4.0f }},
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

			// cout << "{ " << total_aX << " " << total_aY << " }" << "\n";

			system[i].v.x += total_aX * dT;
			system[i].v.y += total_aY * dT;
		}

		for (auto &body: system) {
			body.x += body.v.x * dT;
			body.y += body.v.y * dT;

			if (warp && body.x > WIDTH) {
				body.x = body.x - WIDTH;
			}

			if (warp && body.x < 0) {
				body.x = body.x + WIDTH;
			}

			if (warp && body.y > HEIGHT) {
				body.y = body.y - HEIGHT;
			}

			if (warp && body.y < 0) {
				body.y = body.y + HEIGHT;
			}
		}

        BeginDrawing();
        ClearBackground(BLACK);

		for (const auto body : system) {
			DrawRectangle(body.x - body.R, body.y - body.R, body.R * 2, body.R * 2, WHITE);
		}

        EndDrawing();
	}

    CloseWindow();

	return 0;
}
