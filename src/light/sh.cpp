#include "pch.hpp"
#include "sh.hpp"

// routine generated programmatically for evaluating SH basis for degree 1
// inputs (x,y,z) are a point on the sphere (i.e., must be unit length)
// output is vector b with SH basis evaluated at (x,y,z).
//
/*
-------------------------------------------------
-------------------------------------------------
*/
inline static void sh_eval_basis_1(Vec3 dir, float b[4])
{
    //
    const float x = dir.x();
    const float y = dir.y();
    const float z = dir.z();
    // l=0,m=0
    b[0] = 0.282094791773878140f;
    // l=1,m=0
    b[2] = 0.488602511902919920f * z; 
    //
    const float s1 = y;
    const float c1 = x;
    // l=1
    const float p_1_1 = -0.488602511902919920f;
    // l=1,m=-1
    b[1] = p_1_1*s1;
    // l=1,m=+1
    b[3] = p_1_1*c1;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
inline static void sh_eval_basis_2(Vec3 dir, float b[9])
{
    //
    const float x = dir.x();
    const float y = dir.y();
    const float z = dir.z();
    //
    const float z2 = z*z;
    const float p_0_0 = 0.282094791773878140f;
    // l=0,m=0
    b[0] = p_0_0; 
    const float p_1_0 = 0.488602511902919920f*z;
    // l=1,m=0
    b[2] = p_1_0; 
    // l=2
    const float p_2_0 = 0.946174695757560080f * z2 + -0.315391565252520050f;
    // l=2,m=0
    b[6] = p_2_0;
    const float s1 = y;
    const float c1 = x;
    //
    const float p_1_1 = -0.488602511902919920f;
    // l=1,m=-1
    b[1] = p_1_1*s1;
    // l=1,m=+1
    b[3] = p_1_1*c1;
    const float p_2_1 = -1.092548430592079200f*z;
    // l=2,m=-1
    b[5] = p_2_1*s1;
    // l=2,m=+1
    b[7] = p_2_1*c1;
    //
    const float s2 = x*s1 + y*c1;
    const float c2 = x*c1 - y*s1;
    // l=2
    const float p_2_2 = 0.546274215296039590f;
    // l=2,m=-2
    b[4] = p_2_2*s2;
    // l=2,m=+2
    b[8] = p_2_2*c2;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
// input pF only consists of Yl0 values, normalizes coefficients for directional
// lights.
static inline float cosWtInt(const size_t order)
{
    const float fCW0 = 0.25f;
    const float fCW1 = 0.5f;
    const float fCW2 = 5.0f / 16.0f;
    //const float fCW3 = 0.0f;
    const float fCW4 = -3.0f / 32.0f;
    //const float fCW5 = 0.0f;

    // order has to be at least linear...

    float fRet = fCW0 + fCW1;

    if (order > 2) fRet += fCW2;
    if (order > 4) fRet += fCW4;

    // odd degrees >= 3 evaluate to zero integrated against cosine...

    return fRet;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void _evalDirection(float* values, int32_t order, Vec3 dir)
{
    // TODO: 
    switch (order)
    {
    case 2:
        sh_eval_basis_1(dir, values);
        break;
    case 3:
        sh_eval_basis_2(dir, values);
        break;
    default:
        assert(false);
        break;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void _evalDirectionalLight(float* values, int32_t order, Vec3 dir)
{
    _evalDirection(values, order, dir);

    // 方向成分をnormalizeする
    // now compute "normalization" and scale vector for each valid spectral band
    const float fNorm = PI / cosWtInt(order);
    const int32_t numCoeff = order * order;
    for (size_t i = 0; i < numCoeff; ++i)
    {
        values[i] *= fNorm;
    }
}
