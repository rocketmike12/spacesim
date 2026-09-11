// spacesim.cpp
// Copyright (c) 2026 rocketmike12
// Licensed under the MIT License (see LICENSE)

#include <bits/stdc++.h>
#include <cstring>
#include <filesystem>
#include <nlohmann/detail/macro_scope.hpp>
#include <raylib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

struct Vector2D {
	float x;
	float y;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vector2D, x, y);
};

struct Body {
	float x;
	float y;
	float m;
	float R;
	Vector2D v;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Body, x, y, m, R, v);
};

struct System {
	int width;
	int height;
	int fps = 0;
	float G;
	float epsilon;
	vector<Body> bodies;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(System, width, height, fps, G, epsilon, bodies);
};

bool validate_json(const json& j) {
    const vector<string> required_fields = {
        "width", "height", "fps", "G", "epsilon", "bodies"
    };
    
    for (const auto& field : required_fields) {
        if (!j.contains(field)) {
            cout << "ERROR: field missing: " << field << '\n';
            return false;
        }
    }

    if (!j["bodies"].is_array()) {
        cout << "ERROR: field 'bodies' must be an array\n";
        return false;
    }

    for (const auto& b : j.at("bodies")) {
        const vector<string> body_fields = {"x", "y", "m", "R", "v"};
        for (const auto& field : body_fields) {
            if (!b.contains(field)) {
                cout << "ERROR: field missing: bodies[i]." << field << '\n';
                return false;
            }
        }

        if (!b.at("v").contains("x")) {
            std::cout << "field missing: bodies[i].v.x\n";
            return false;
        }

        if (!b.at("v").contains("y")) {
            std::cout << "field missing: bodies[i].v.y\n";
            return false;
        }
    }

    return true;
}

int parse_system (const json& j, System& s) {
	if (!validate_json(j)) return -1;

	j.at("width").get_to(s.width);
	j.at("height").get_to(s.height);
	j.at("fps").get_to(s.fps);
	j.at("G").get_to(s.G);
	j.at("epsilon").get_to(s.epsilon);
	j.at("bodies").get_to(s.bodies);

	return 0;
}

int main (int argc, char* argv[]) {
	bool _warp = false;
	System _system;

	if (argc > 0) {
		for (int i = 1; i < argc; i++) {
			if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--warp")) {
				_warp = true;
				continue;
			}

			if (!_system.fps) {
				if (!filesystem::exists(argv[i])) {
					cout << "ERROR: " << string(argv[i]) << ": no such file\n";
					return -1;
				}

				ifstream f(argv[i]);
				if(parse_system(json::parse(f), _system) != 0) return -1;

				continue;
			}
		}

		if (!_system.fps) {
			cout << "ERROR: no system file provided\n";
			return -1;
		}
	}

	const float G = _system.G;
	const float EPSILON = _system.epsilon;
	const float dT = 1.0f / _system.fps;

	InitWindow(_system.width, _system.height, "spacesim");
	SetTargetFPS(_system.fps);

	vector<Body> bodies = _system.bodies;

	while (!WindowShouldClose()) {
		for (int i = 0; i < (int)bodies.size(); i++) {
			float total_aX = 0.0f;
			float total_aY = 0.0f;

			for (int j = 0; j < (int)bodies.size(); j++) {
				if (i == j) continue;

				float dX = bodies[j].x - bodies[i].x;
				float dY = bodies[j].y - bodies[i].y;
				float r = sqrt((dX*dX) + (dY*dY));

				float F = G * ((bodies[i].m * bodies[j].m) / ((r*r) + EPSILON));

				total_aX += (F * dX) / (bodies[i].m * r);
				total_aY += (F * dY) / (bodies[i].m * r);
			}

			// cout << "{ " << total_aX << " " << total_aY << " }" << "\n";

			bodies[i].v.x += total_aX * dT;
			bodies[i].v.y += total_aY * dT;
		}

		for (auto &body: bodies) {
			body.x += body.v.x * dT;
			body.y += body.v.y * dT;

			if (_warp && body.x > _system.width) {
				body.x = body.x - _system.width;
			}

			if (_warp && body.x < 0) {
				body.x = body.x + _system.width;
			}

			if (_warp && body.y > _system.height) {
				body.y = body.y - _system.height;
			}

			if (_warp && body.y < 0) {
				body.y = body.y + _system.height;
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (const auto body : bodies) {
			DrawRectangle(body.x - body.R, body.y - body.R, body.R * 2, body.R * 2, WHITE);
		}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
