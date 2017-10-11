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
    OrthonormalBasis(Vec3 n);
    OrthonormalBasis(Vec3 n, Vec3 t);
    void set(Vec3 n);
    void set(Vec3 n, Vec3 t);
    void set(Vec3 n, Vec3 t, Vec3 s);
    Vec3 world2local(Vec3 world) const;
    Vec3 local2world(Vec3 local) const;
    Vec3 getN() const;
    Vec3 getT() const;
    Vec3 getS() const;
private:
    void updateNTS();
private:
    Matrix3x3 nts;
    Matrix3x3 ntsInv;
};

#include "orthonormalbasis.inl"
