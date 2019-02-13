#pragma once

#include <bits/stdc++.h>
#include "DS.h"
#include "Models.h"
#include "defs.h"
#include "json.hpp"
#include "CImg.h"

Vector4f augment(const Vector3f &vec, float val);
Vector3f apply_transformation(const Vector3f &point, const Matrix4f &trans);
std::optional<std::pair<float, float>> solve_quadratic(float a, float b,
                                                       float c);

// Parsing Functions
using json = nlohmann::json;
using namespace std;

Vector3f get_vector3f(const json &j);
Model *parse_model(const json &j, unordered_map<string, Material *> &materials);
void get_models(const json &j, vector<Model *> &models,
                unordered_map<string, Material *> &materials);
pair<string, Material *> parse_material(const json &j);
void get_materials(const json &j, unordered_map<string, Material *> &materials);
Light *parse_light(const json &j);
void get_lights(const json &j, vector<Light *> &lights);
Camera *get_camera(const json &j);
State get_state(string filename);
