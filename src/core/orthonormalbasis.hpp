#pragma once

#include "pch.hpp"
#include "core/math.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
enum ONB_METHOD
{
    // ナイーブな実装
    Naive,
    // Tomas Möller, “Efficiently Building a Matrix to Rotate One Vector to Another,”
    Moller99,
    // J. R. Frisvad, “Building an Orthonormal Basis from a 3D Unit Vector Without Normalization,”
    Frisvad12,
    // Tom Duff, “Building an Orthonormal Basis, Revisited,”
    Duff17,
    // Nelson Max, “Improved Accuracy when Building an Orthonormal Basis,”
    Nelson17
};
/*
-------------------------------------------------
正規直交基底
-------------------------------------------------
*/
template<ONB_METHOD method = ONB_METHOD::Frisvad12>
class OrthonormalBasis
{
public:
	OrthonormalBasis();
    OrthonormalBasis(const Vec3& n);
    OrthonormalBasis(const Vec3& n, const Vec3& t);
	void set(const Vec3& n);
	void set(const Vec3& n, const Vec3& t);
    void set(const Vec3& n, const Vec3& t, const Vec3& s);
    Vec3 n() const { return n_; }
    Vec3 t() const { return t_; }
    Vec3 s() const { return s_; }
    Vec3 world2local(Vec3 world) const;
    Vec3 local2world(Vec3 local) const;
private:
    void updateNTS();
private:
    Vec3 n_;
    Vec3 t_;
    Vec3 s_;
    // ntsをtransposeしたもの
    Vec3 ntsX_;
    Vec3 ntsY_;
    Vec3 ntsZ_;
};

#include "orthonormalbasis.inl"
