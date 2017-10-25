#include "pch.hpp"
#include "core/vdbmt.hpp"
#include "core/math.hpp"
#include "core/util.hpp"

#include "vdb.h"

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename Type>
class DirtyableValue
{
public:
    Type value;
    bool dirty = false;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
struct TLS_Data
{
    // 前回の色
    DirtyableValue<Vec3> color;
    // 前回のラベル
    DirtyableValue<const char*> label;
    DirtyableValue<int32_t> label_i;
};
AL_TLS TLS_Data tls = {};

/*
SpinLockMutexを使うとむしろ遅くなるのでstd::mutexを使う
*/
#if 1
std::mutex global_vdmt_mutex;
#else
SpinLock global_vdmt_mutex;
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
static void setAdditionalAttributes()
{
    // 色
    auto& col = tls.color;
    if (col.dirty)
    {
        auto& c = col.value;
        vdb_color(c.x(), c.y(), c.z());
        col.dirty = false;
    }
    // ラベル
    auto& label = tls.label;
    if (label.dirty)
    {
        vdb_label(label.value);
        label.dirty = false;
    }
    auto& label_i = tls.label_i;
    if (label_i.dirty)
    {
        vdb_label_i(label_i.value);
        label_i.dirty = false;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_begin()
{
    // 実装がややこしくなるので非対応
    AL_ASSERT_ALWAYS(false);
    vdb_begin();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_end()
{
    // 実装がややこしくなるので非対応
    AL_ASSERT_ALWAYS(false);
    vdb_end();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_point(Vec3 p)
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    setAdditionalAttributes();
    vdb_point(p.x(), p.y(), p.z());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_line(Vec3 p0, Vec3 p1)
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    setAdditionalAttributes();
    vdb_line(
        p0.x(), p0.y(), p0.z(),
        p1.x(), p1.y(), p1.z());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_normal(Vec3 p, Vec3 n)
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    setAdditionalAttributes();
    vdb_normal(
        p.x(), p.y(), p.z(),
        n.x(), n.y(), n.z());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_triangle(Vec3 v0, Vec3 v1, Vec3 v2)
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    setAdditionalAttributes();
    vdb_triangle(
        v0.x(), v0.y(), v0.z(),
        v1.x(), v1.y(), v1.z(),
        v2.x(), v2.y(), v2.z());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_aabb(Vec3 mn, Vec3 mx)
{
    const float mnx = mn.x();
    const float mny = mn.y();
    const float mnz = mn.z();
    const float mxx = mx.x();
    const float mxy = mx.y();
    const float mxz = mx.z();
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    setAdditionalAttributes();
    vdb_line(mnx, mny, mnz, mxx, mny, mnz);
    vdb_line(mnx, mny, mnz, mnx, mxy, mnz);
    vdb_line(mnx, mny, mnz, mnx, mny, mxz);
    vdb_line(mxx, mny, mnz, mxx, mxy, mnz);
    vdb_line(mxx, mny, mnz, mxx, mny, mxz);
    vdb_line(mnx, mxy, mnz, mxx, mxy, mnz);
    vdb_line(mnx, mxy, mnz, mnx, mxy, mxz);
    vdb_line(mnx, mny, mxz, mxx, mny, mxz);
    vdb_line(mnx, mny, mxz, mnx, mxy, mxz);
    vdb_line(mnx, mxy, mxz, mxx, mxy, mxz);
    vdb_line(mxx, mny, mxz, mxx, mxy, mxz);
    vdb_line(mxx, mxy, mnz, mxx, mxy, mxz);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_color(Vec3 aColor)
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    auto& color = tls.color;
    color.value = aColor;
    color.dirty = true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_label(const char * lbl)
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    auto& label = tls.label;
    label.value = lbl;
    label.dirty = true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_label_i(int32_t i)
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    auto& label = tls.label_i;
    label.value = i;
    label.dirty = true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void vdbmt_frame()
{
    std::lock_guard<decltype(global_vdmt_mutex)> lock(global_vdmt_mutex);
    vdb_frame();
}

