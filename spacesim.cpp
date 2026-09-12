// spacesim.cpp
// Copyright (c) 2026 rocketmike12
// Licensed under the MIT License (see LICENSE)

#include <bits/stdc++.h>
#include <cstddef>
#include <cstring>
#include <filesystem>

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

#include <raylib.h>
#include <vector>

using json = nlohmann::json;
using namespace std;

enum class Mode {
	PIXEL,
	UNIT
};

NLOHMANN_JSON_SERIALIZE_ENUM(Mode, {
	{ Mode::PIXEL, "pixel" },
	{ Mode::UNIT, "unit" }
});

struct Vector2D {
	double x;
	double y;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vector2D, x, y);
};

struct Body {
	double x;
	double y;
	double m;
	double R;
	Vector2D v;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Body, x, y, m, R, v);
};

struct System {
	enum Mode mode;	
	int width;
	int height;
	int fps = 0;
	double S = 1.0f;
	double dT;
	double G;
	double epsilon;
	vector<Body> bodies;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(System, mode, width, height, fps, S, G, epsilon, bodies);
};

bool validate_json(const json& j) {
	const vector<string> required_fields = {
		"mode", "width", "height", "fps", "G", "epsilon", "bodies"
	};

	for (const auto& field : required_fields) {
		if (!j.contains(field)) {
			cout << "ERROR: field missing: " << field << '\n';
			return false;
		}
	}

	if (j.at("mode") == Mode::UNIT) {
		if (!j.contains("S")) {
			cout << "ERROR: mode is set to 'unit' but no scaling factor (S) is provided\n";
			return false;
		}
		if (!j.contains("dT")) {
			cout << "ERROR: mode is set to 'unit' but no time delta (dT) is provided\n";
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

	j.at("mode").get_to(s.mode);
	j.at("width").get_to(s.width);
	j.at("height").get_to(s.height);
	j.at("fps").get_to(s.fps);
	if (s.mode == Mode::UNIT) {
		j.at("S").get_to(s.S);
		j.at("dT").get_to(s.dT);
	};
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

	cout << _system.bodies[0].m << "\n";

	const Mode mode = _system.mode;

	const double G = _system.G;
	const double EPSILON = _system.epsilon;

	double dT = 1.0f / _system.fps;

	if (_system.mode == Mode::UNIT) {
		dT = _system.dT;
	}

	const int centerX = _system.width / 2;
	const int centerY = _system.height / 2;

	InitWindow(_system.width, _system.height, "spacesim");
	SetTargetFPS(_system.fps);

	vector<Body> bodies = _system.bodies;

	while (!WindowShouldClose()) {
		vector<Vector2D> acc(bodies.size(), { 0.0f, 0.0f });

		for (int i = 0; i < (int)bodies.size(); i++) {
			for (int j = 0; j < (int)bodies.size(); j++) {
				if (i == j) continue;

				double dX = bodies[j].x - bodies[i].x;
				double dY = bodies[j].y - bodies[i].y;
				double r2 = (dX*dX) + (dY*dY);
				double r = sqrt(r2);

				double a = G * bodies[j].m / ((r*r) + EPSILON);
				acc[i].x += a * (dX / r);
				acc[i].y += a * (dY / r);
			}

		}

		for (size_t i = 0; i < bodies.size(); i++) {
			bodies[i].v.x += acc[i].x * dT;
			bodies[i].v.y += acc[i].y * dT;

			cout << bodies[i].v.x << " " << bodies[i].v.y << "\n";
			
			bodies[i].x += bodies[i].v.x * dT;
			bodies[i].y += bodies[i].v.y * dT;

			if (_warp && bodies[i].x > _system.width) {
				bodies[i].x = bodies[i].x - _system.width;
			}

			if (_warp && bodies[i].x < 0) {
				bodies[i].x = bodies[i].x + _system.width;
			}

			if (_warp && bodies[i].y > _system.height) {
				bodies[i].y = bodies[i].y - _system.height;
			}

			if (_warp && bodies[i].y < 0) {
				bodies[i].y = bodies[i].y + _system.height;
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (const auto body : bodies) {
			DrawRectangle(centerX + ((body.x - body.R) * _system.S), centerY - ((body.y - body.R) * _system.S), body.R * 2 * _system.S, body.R * 2 * _system.S, WHITE);
		}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
