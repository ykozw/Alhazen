#pragma once

#include "pch.hpp"

void
vdbmt_begin();
void
vdbmt_end();
void
vdbmt_point(Vec3 p);
void
vdbmt_line(Vec3 p0, Vec3 p1);
void
vdbmt_normal(Vec3 p, Vec3 n);
void
vdbmt_triangle(Vec3 v0, Vec3 v1, Vec3 v2);
void
vdbmt_aabb(Vec3 mn, Vec3 mx);
void
vdbmt_color(Vec3 color);
void
vdbmt_color_rnd();
void
vdbmt_label(const char* lbl);
void
vdbmt_label_i(int32_t i);
void
vdbmt_frame();
void
vdbmt_begin();
void
vdbmt_end();
