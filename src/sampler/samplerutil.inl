//#pragma once
#include "pch.hpp"
#include "core/math.hpp"



/*
------------------------------------------------
球面座標系を単位ベクトルにする
Z-upであることに注意
------------------------------------------------
*/
inline Vec3 remapSphericalCoordToDir(Vec2 tp)
{
    //
    AL_ASSERT_DEBUG(0.0f <= tp.x() && tp.x() <= 1.0f);
    AL_ASSERT_DEBUG(0.0f <= tp.y() && tp.y() <= 1.0f);
    //
    const float theta = 1.0f * PI * tp.x();
    const float phi = 2.0f * PI * tp.y();
    const float sinPhi = std::sinf(phi);
    const float cosPhi = std::cosf(phi);
    const float cosTheta = std::cosf(theta);
    const float sinTheta = std::sinf(theta);;
    const Vec3 r = { sinTheta * cosPhi, sinTheta * sinPhi, cosTheta };
    AL_ASSERT_DEBUG(std::fabsf(Vec3::dot(r, r) - 1.0f) < 0.01f);
    return r;
}

/*
------------------------------------------------
単位ベクトルを球面座系にする
------------------------------------------------
*/
inline Vec2 remapDirToSphericalCoord(Vec3 xyz)
{
    //
    AL_ASSERT_DEBUG(std::fabsf(xyz.length() - 1.0f) < 0.001f);
    // 
    const float theta = std::acosf(xyz.z());
    const float sinTheta = std::sinf(theta);
    const float tmp = std::min(std::max(xyz.x() / sinTheta, 0.0f), 1.0f);
    const float phi = std::acosf(tmp);
    //
    AL_ASSERT_DEBUG(!std::isnan(theta));
    AL_ASSERT_DEBUG(!std::isnan(phi));
    //
    return Vec2({ theta * INV_PI, phi * INV_PI * 0.5f });
}

/*
-------------------------------------------------
latinHypercube()
ラテン超方格
-------------------------------------------------
*/
INLINE void latinHypercube(RNG& rng, float* buf, uint32_t bufLen, size_t dim)
{
    AL_ASSERT_DEBUG(!(bufLen % dim));
    const int32_t sampleNum = static_cast<int32_t>(bufLen / dim);
    const float delta = 1 / (float)sampleNum;
    for (int32_t i = 0; i < sampleNum; ++i)
    {
        for (int32_t j = 0; j < dim; ++j)
        {
            buf[dim * i + j] = (i + rng.nextFloat()) * delta;
        }
    }
    for (int32_t i = 0; i < dim; ++i)
    {
        for (int32_t j = 0; j < sampleNum; ++j)
        {
            const uint32_t other = rng.nextSize(sampleNum);
            std::swap(buf[dim * j + i], buf[dim * other + i]);
        }
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float radicalInverseCore(int32_t base, int32_t index)
{
    int32_t n = index;
    float val = 0.0f;
    const float invBase = 1.0f / float(base);
    float invBi = invBase;
    while (n > 0)
    {
        int32_t d_i = (n % base);
        val += d_i * invBi;
        n = (int32_t)((float)n * invBase);
        invBi *= invBase;
    }
    return val;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float radicalInverseSlow(int32_t primNumberIndex, int32_t index)
{
    static const std::array<uint32_t,1229> primList =
    {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
        31, 37, 41, 43, 47, 53, 59, 61, 67,
        71, 73, 79, 83, 89, 97, 101, 103, 107, 109,
        113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
        173, 179, 181, 191, 193, 197, 199, 211, 223, 227,
        229, 233, 239, 241, 251, 257, 263, 269, 271, 277,
        281, 283, 293, 307, 311, 313, 317, 331, 337, 347,
        349, 353, 359, 367, 373, 379, 383, 389, 397, 401,
        409, 419, 421, 431, 433, 439, 443, 449, 457, 461,
        463, 467, 479, 487, 491, 499, 503, 509, 521, 523,
        541, 547, 557, 563, 569, 571, 577, 587, 593, 599,
        601, 607, 613, 617, 619, 631, 641, 643, 647, 653,
        659, 661, 673, 677, 683, 691, 701, 709, 719, 727,
        733, 739, 743, 751, 757, 761, 769, 773, 787, 797,
        809, 811, 821, 823, 827, 829, 839, 853, 857, 859,
        863, 877, 881, 883, 887, 907, 911, 919, 929, 937,
        941, 947, 953, 967, 971, 977, 983, 991, 997, 1009,
        1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,
        1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,
        1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,
        1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,
        1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,
        1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,
        1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,
        1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,
        1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,
        1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,
        1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,
        1811,1823,1831,1847,1861,1867,1871,1873,1877,1879,
        1889,1901,1907,1913,1931,1933,1949,1951,1973,1979,
        1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,
        2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,
        2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,
        2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,
        2287,2293,2297,2309,2311,2333,2339,2341,2347,2351,
        2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,
        2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,
        2531,2539,2543,2549,2551,2557,2579,2591,2593,2609,
        2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,
        2687,2689,2693,2699,2707,2711,2713,2719,2729,2731,
        2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,
        2819,2833,2837,2843,2851,2857,2861,2879,2887,2897,
        2903,2909,2917,2927,2939,2953,2957,2963,2969,2971,
        2999,3001,3011,3019,3023,3037,3041,3049,3061,3067,
        3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,
        3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,
        3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,
        3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,
        3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,
        3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,
        3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,
        3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,
        3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,
        3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,
        3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,
        3989,4001,4003,4007,4013,4019,4021,4027,4049,4051,
        4057,4073,4079,4091,4093,4099,4111,4127,4129,4133,
        4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,
        4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,
        4297,4327,4337,4339,4349,4357,4363,4373,4391,4397,
        4409,4421,4423,4441,4447,4451,4457,4463,4481,4483,
        4493,4507,4513,4517,4519,4523,4547,4549,4561,4567,
        4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,
        4657,4663,4673,4679,4691,4703,4721,4723,4729,4733,
        4751,4759,4783,4787,4789,4793,4799,4801,4813,4817,
        4831,4861,4871,4877,4889,4903,4909,4919,4931,4933,
        4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,
        5003,5009,5011,5021,5023,5039,5051,5059,5077,5081,
        5087,5099,5101,5107,5113,5119,5147,5153,5167,5171,
        5179,5189,5197,5209,5227,5231,5233,5237,5261,5273,
        5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,
        5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,
        5443,5449,5471,5477,5479,5483,5501,5503,5507,5519,
        5521,5527,5531,5557,5563,5569,5573,5581,5591,5623,
        5639,5641,5647,5651,5653,5657,5659,5669,5683,5689,
        5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,
        5791,5801,5807,5813,5821,5827,5839,5843,5849,5851,
        5857,5861,5867,5869,5879,5881,5897,5903,5923,5927,
        5939,5953,5981,5987,6007,6011,6029,6037,6043,6047,
        6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,
        6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,
        6221,6229,6247,6257,6263,6269,6271,6277,6287,6299,
        6301,6311,6317,6323,6329,6337,6343,6353,6359,6361,
        6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,
        6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,
        6571,6577,6581,6599,6607,6619,6637,6653,6659,6661,
        6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,
        6761,6763,6779,6781,6791,6793,6803,6823,6827,6829,
        6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,
        6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,
        6997,7001,7013,7019,7027,7039,7043,7057,7069,7079,
        7103,7109,7121,7127,7129,7151,7159,7177,7187,7193,
        7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,
        7297,7307,7309,7321,7331,7333,7349,7351,7369,7393,
        7411,7417,7433,7451,7457,7459,7477,7481,7487,7489,
        7499,7507,7517,7523,7529,7537,7541,7547,7549,7559,
        7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,
        7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,
        7723,7727,7741,7753,7757,7759,7789,7793,7817,7823,
        7829,7841,7853,7867,7873,7877,7879,7883,7901,7907,
        7919,7927,7933,7937,7949,7951,7963,7993,8009,8011,
        8017,8039,8053,8059,8069,8081,8087,8089,8093,8101,
        8111,8117,8123,8147,8161,8167,8171,8179,8191,8209,
        8219,8221,8231,8233,8237,8243,8263,8269,8273,8287,
        8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,
        8387,8389,8419,8423,8429,8431,8443,8447,8461,8467,
        8501,8513,8521,8527,8537,8539,8543,8563,8573,8581,
        8597,8599,8609,8623,8627,8629,8641,8647,8663,8669,
        8677,8681,8689,8693,8699,8707,8713,8719,8731,8737,
        8741,8747,8753,8761,8779,8783,8803,8807,8819,8821,
        8831,8837,8839,8849,8861,8863,8867,8887,8893,8923,
        8929,8933,8941,8951,8963,8969,8971,8999,9001,9007,
        9011,9013,9029,9041,9043,9049,9059,9067,9091,9103,
        9109,9127,9133,9137,9151,9157,9161,9173,9181,9187,
        9199,9203,9209,9221,9227,9239,9241,9257,9277,9281,
        9283,9293,9311,9319,9323,9337,9341,9343,9349,9371,
        9377,9391,9397,9403,9413,9419,9421,9431,9433,9437,
        9439,9461,9463,9467,9473,9479,9491,9497,9511,9521,
        9533,9539,9547,9551,9587,9601,9613,9619,9623,9629,
        9631,9643,9649,9661,9677,9679,9689,9697,9719,9721,
        9733,9739,9743,9749,9767,9769,9781,9787,9791,9803,
        9811,9817,9829,9833,9839,9851,9857,9859,9871,9883,
        9887,9901,9907,9923,9929,9931,9941,9949,9967,9973
    };
    const uint32_t primNumber = primList[primNumberIndex];
    return radicalInverseCore(primNumber, index);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<uint32_t base>
INLINE float radicalInverseFastCore(int32_t index)
{
    int32_t n = index;
    float val = 0.0f;
    const float invBase = 1.0f / float(base);
    float invBi = invBase;
    while (n > 0)
    {
        int32_t d_i = (n % base);
        val += d_i * invBi;
        n = (int32_t)((float)n * invBase);
        invBi *= invBase;
    }
    return val;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float radicalInverseFast(int32_t primNumberIndex, int32_t index)
{
    switch (primNumberIndex)
    {
    case 0: return radicalInverseFastCore<2>(index);
    case 1: return radicalInverseFastCore<3>(index);
    case 2: return radicalInverseFastCore<5>(index);
    case 3: return radicalInverseFastCore<7>(index);
    case 4: return radicalInverseFastCore<11>(index);
    case 5: return radicalInverseFastCore<13>(index);
    case 6: return radicalInverseFastCore<17>(index);
    case 7: return radicalInverseFastCore<19>(index);
    case 8: return radicalInverseFastCore<23>(index);
    case 9: return radicalInverseFastCore<29>(index);
    case 10: return radicalInverseFastCore<31>(index);
    case 11: return radicalInverseFastCore<37>(index);
    case 12: return radicalInverseFastCore<41>(index);
    case 13: return radicalInverseFastCore<43>(index);
    case 14: return radicalInverseFastCore<47>(index);
    case 15: return radicalInverseFastCore<53>(index);
    case 16: return radicalInverseFastCore<59>(index);
    case 17: return radicalInverseFastCore<61>(index);
    case 18: return radicalInverseFastCore<67>(index);
    case 19: return radicalInverseFastCore<71>(index);
    case 20: return radicalInverseFastCore<73>(index);
    case 21: return radicalInverseFastCore<79>(index);
    case 22: return radicalInverseFastCore<83>(index);
    case 23: return radicalInverseFastCore<89>(index);
    case 24: return radicalInverseFastCore<97>(index);
    case 25: return radicalInverseFastCore<101>(index);
    case 26: return radicalInverseFastCore<103>(index);
    case 27: return radicalInverseFastCore<107>(index);
    case 28: return radicalInverseFastCore<109>(index);
    case 29: return radicalInverseFastCore<113>(index);
    case 30: return radicalInverseFastCore<127>(index);
    case 31: return radicalInverseFastCore<131>(index);
    case 32: return radicalInverseFastCore<137>(index);
    case 33: return radicalInverseFastCore<139>(index);
    case 34: return radicalInverseFastCore<149>(index);
    case 35: return radicalInverseFastCore<151>(index);
    case 36: return radicalInverseFastCore<157>(index);
    case 37: return radicalInverseFastCore<163>(index);
    case 38: return radicalInverseFastCore<167>(index);
    case 39: return radicalInverseFastCore<173>(index);
    case 40: return radicalInverseFastCore<179>(index);
    case 41: return radicalInverseFastCore<181>(index);
    case 42: return radicalInverseFastCore<191>(index);
    case 43: return radicalInverseFastCore<193>(index);
    case 44: return radicalInverseFastCore<197>(index);
    case 45: return radicalInverseFastCore<199>(index);
    case 46: return radicalInverseFastCore<211>(index);
    case 47: return radicalInverseFastCore<223>(index);
    case 48: return radicalInverseFastCore<227>(index);
    case 49: return radicalInverseFastCore<229>(index);
    case 50: return radicalInverseFastCore<233>(index);
    case 51: return radicalInverseFastCore<239>(index);
    case 52: return radicalInverseFastCore<241>(index);
    case 53: return radicalInverseFastCore<251>(index);
    case 54: return radicalInverseFastCore<257>(index);
    case 55: return radicalInverseFastCore<263>(index);
    case 56: return radicalInverseFastCore<269>(index);
    case 57: return radicalInverseFastCore<271>(index);
    case 58: return radicalInverseFastCore<277>(index);
    case 59: return radicalInverseFastCore<281>(index);
    case 60: return radicalInverseFastCore<283>(index);
    case 61: return radicalInverseFastCore<293>(index);
    case 62: return radicalInverseFastCore<307>(index);
    case 63: return radicalInverseFastCore<311>(index);
    case 64: return radicalInverseFastCore<313>(index);
    case 65: return radicalInverseFastCore<317>(index);
    case 66: return radicalInverseFastCore<331>(index);
    case 67: return radicalInverseFastCore<337>(index);
    case 68: return radicalInverseFastCore<347>(index);
    case 69: return radicalInverseFastCore<349>(index);
    case 70: return radicalInverseFastCore<353>(index);
    case 71: return radicalInverseFastCore<359>(index);
    case 72: return radicalInverseFastCore<367>(index);
    case 73: return radicalInverseFastCore<373>(index);
    case 74: return radicalInverseFastCore<379>(index);
    case 75: return radicalInverseFastCore<383>(index);
    case 76: return radicalInverseFastCore<389>(index);
    case 77: return radicalInverseFastCore<397>(index);
    case 78: return radicalInverseFastCore<401>(index);
    case 79: return radicalInverseFastCore<409>(index);
    case 80: return radicalInverseFastCore<419>(index);
    case 81: return radicalInverseFastCore<421>(index);
    case 82: return radicalInverseFastCore<431>(index);
    case 83: return radicalInverseFastCore<433>(index);
    case 84: return radicalInverseFastCore<439>(index);
    case 85: return radicalInverseFastCore<443>(index);
    case 86: return radicalInverseFastCore<449>(index);
    case 87: return radicalInverseFastCore<457>(index);
    case 88: return radicalInverseFastCore<461>(index);
    case 89: return radicalInverseFastCore<463>(index);
    case 90: return radicalInverseFastCore<467>(index);
    case 91: return radicalInverseFastCore<479>(index);
    case 92: return radicalInverseFastCore<487>(index);
    case 93: return radicalInverseFastCore<491>(index);
    case 94: return radicalInverseFastCore<499>(index);
    case 95: return radicalInverseFastCore<503>(index);
    case 96: return radicalInverseFastCore<509>(index);
    case 97: return radicalInverseFastCore<521>(index);
    case 98: return radicalInverseFastCore<523>(index);
    case 99: return radicalInverseFastCore<541>(index);
    case 100: return radicalInverseFastCore<547>(index);
    case 101: return radicalInverseFastCore<557>(index);
    case 102: return radicalInverseFastCore<563>(index);
    case 103: return radicalInverseFastCore<569>(index);
    case 104: return radicalInverseFastCore<571>(index);
    case 105: return radicalInverseFastCore<577>(index);
    case 106: return radicalInverseFastCore<587>(index);
    case 107: return radicalInverseFastCore<593>(index);
    case 108: return radicalInverseFastCore<599>(index);
    case 109: return radicalInverseFastCore<601>(index);
    case 110: return radicalInverseFastCore<607>(index);
    case 111: return radicalInverseFastCore<613>(index);
    case 112: return radicalInverseFastCore<617>(index);
    case 113: return radicalInverseFastCore<619>(index);
    case 114: return radicalInverseFastCore<631>(index);
    case 115: return radicalInverseFastCore<641>(index);
    case 116: return radicalInverseFastCore<643>(index);
    case 117: return radicalInverseFastCore<647>(index);
    case 118: return radicalInverseFastCore<653>(index);
    case 119: return radicalInverseFastCore<659>(index);
    case 120: return radicalInverseFastCore<661>(index);
    case 121: return radicalInverseFastCore<673>(index);
    case 122: return radicalInverseFastCore<677>(index);
    case 123: return radicalInverseFastCore<683>(index);
    case 124: return radicalInverseFastCore<691>(index);
    case 125: return radicalInverseFastCore<701>(index);
    case 126: return radicalInverseFastCore<709>(index);
    case 127: return radicalInverseFastCore<719>(index);
    case 128: return radicalInverseFastCore<1009>(index);
    case 129: return radicalInverseFastCore<1013>(index);
    case 130: return radicalInverseFastCore<1019>(index);
    case 131: return radicalInverseFastCore<1021>(index);
    case 132: return radicalInverseFastCore<1031>(index);
    case 133: return radicalInverseFastCore<1033>(index);
    case 134: return radicalInverseFastCore<1039>(index);
    case 135: return radicalInverseFastCore<1049>(index);
    case 136: return radicalInverseFastCore<1051>(index);
    case 137: return radicalInverseFastCore<1061>(index);
    case 138: return radicalInverseFastCore<1063>(index);
    case 139: return radicalInverseFastCore<1069>(index);
    case 140: return radicalInverseFastCore<1087>(index);
    case 141: return radicalInverseFastCore<1091>(index);
    case 142: return radicalInverseFastCore<1093>(index);
    case 143: return radicalInverseFastCore<1097>(index);
    case 144: return radicalInverseFastCore<1103>(index);
    case 145: return radicalInverseFastCore<1109>(index);
    case 146: return radicalInverseFastCore<1117>(index);
    case 147: return radicalInverseFastCore<1123>(index);
    case 148: return radicalInverseFastCore<1129>(index);
    case 149: return radicalInverseFastCore<1151>(index);
    case 150: return radicalInverseFastCore<1153>(index);
    case 151: return radicalInverseFastCore<1163>(index);
    case 152: return radicalInverseFastCore<1171>(index);
    case 153: return radicalInverseFastCore<1181>(index);
    case 154: return radicalInverseFastCore<1187>(index);
    case 155: return radicalInverseFastCore<1193>(index);
    case 156: return radicalInverseFastCore<1201>(index);
    case 157: return radicalInverseFastCore<1213>(index);
    case 158: return radicalInverseFastCore<1217>(index);
    case 159: return radicalInverseFastCore<1223>(index);
    case 160: return radicalInverseFastCore<1229>(index);
    case 161: return radicalInverseFastCore<1231>(index);
    case 162: return radicalInverseFastCore<1237>(index);
    case 163: return radicalInverseFastCore<1249>(index);
    case 164: return radicalInverseFastCore<1259>(index);
    case 165: return radicalInverseFastCore<1277>(index);
    case 166: return radicalInverseFastCore<1279>(index);
    case 167: return radicalInverseFastCore<1283>(index);
    case 168: return radicalInverseFastCore<1289>(index);
    case 169: return radicalInverseFastCore<1291>(index);
    case 170: return radicalInverseFastCore<1297>(index);
    case 171: return radicalInverseFastCore<1301>(index);
    case 172: return radicalInverseFastCore<1303>(index);
    case 173: return radicalInverseFastCore<1307>(index);
    case 174: return radicalInverseFastCore<1319>(index);
    case 175: return radicalInverseFastCore<1321>(index);
    case 176: return radicalInverseFastCore<1327>(index);
    case 177: return radicalInverseFastCore<1361>(index);
    case 178: return radicalInverseFastCore<1367>(index);
    case 179: return radicalInverseFastCore<1373>(index);
    case 180: return radicalInverseFastCore<1381>(index);
    case 181: return radicalInverseFastCore<1399>(index);
    case 182: return radicalInverseFastCore<1409>(index);
    case 183: return radicalInverseFastCore<1423>(index);
    case 184: return radicalInverseFastCore<1427>(index);
    case 185: return radicalInverseFastCore<1429>(index);
    case 186: return radicalInverseFastCore<1433>(index);
    case 187: return radicalInverseFastCore<1439>(index);
    case 188: return radicalInverseFastCore<1447>(index);
    case 189: return radicalInverseFastCore<1451>(index);
    case 190: return radicalInverseFastCore<1453>(index);
    case 191: return radicalInverseFastCore<1459>(index);
    case 192: return radicalInverseFastCore<1471>(index);
    case 193: return radicalInverseFastCore<1481>(index);
    case 194: return radicalInverseFastCore<1483>(index);
    case 195: return radicalInverseFastCore<1487>(index);
    case 196: return radicalInverseFastCore<1489>(index);
    case 197: return radicalInverseFastCore<1493>(index);
    case 198: return radicalInverseFastCore<1499>(index);
    case 199: return radicalInverseFastCore<1511>(index);
    case 200: return radicalInverseFastCore<1523>(index);
    case 201: return radicalInverseFastCore<1531>(index);
    case 202: return radicalInverseFastCore<1543>(index);
    case 203: return radicalInverseFastCore<1549>(index);
    case 204: return radicalInverseFastCore<1553>(index);
    case 205: return radicalInverseFastCore<1559>(index);
    case 206: return radicalInverseFastCore<1567>(index);
    case 207: return radicalInverseFastCore<1571>(index);
    case 208: return radicalInverseFastCore<1579>(index);
    case 209: return radicalInverseFastCore<1583>(index);
    case 210: return radicalInverseFastCore<1597>(index);
    case 211: return radicalInverseFastCore<1601>(index);
    case 212: return radicalInverseFastCore<1607>(index);
    case 213: return radicalInverseFastCore<1609>(index);
    case 214: return radicalInverseFastCore<1613>(index);
    case 215: return radicalInverseFastCore<1619>(index);
    case 216: return radicalInverseFastCore<1621>(index);
    case 217: return radicalInverseFastCore<1627>(index);
    case 218: return radicalInverseFastCore<1637>(index);
    case 219: return radicalInverseFastCore<1657>(index);
    case 220: return radicalInverseFastCore<1663>(index);
    case 221: return radicalInverseFastCore<1667>(index);
    case 222: return radicalInverseFastCore<1669>(index);
    case 223: return radicalInverseFastCore<1693>(index);
    case 224: return radicalInverseFastCore<1697>(index);
    case 225: return radicalInverseFastCore<1699>(index);
    case 226: return radicalInverseFastCore<1709>(index);
    case 227: return radicalInverseFastCore<1721>(index);
    case 228: return radicalInverseFastCore<1723>(index);
    case 229: return radicalInverseFastCore<1733>(index);
    case 230: return radicalInverseFastCore<1741>(index);
    case 231: return radicalInverseFastCore<1747>(index);
    case 232: return radicalInverseFastCore<1753>(index);
    case 233: return radicalInverseFastCore<1759>(index);
    case 234: return radicalInverseFastCore<1777>(index);
    case 235: return radicalInverseFastCore<1783>(index);
    case 236: return radicalInverseFastCore<1787>(index);
    case 237: return radicalInverseFastCore<1789>(index);
    case 238: return radicalInverseFastCore<1801>(index);
    case 239: return radicalInverseFastCore<1811>(index);
    case 240: return radicalInverseFastCore<1823>(index);
    case 241: return radicalInverseFastCore<1831>(index);
    case 242: return radicalInverseFastCore<1847>(index);
    case 243: return radicalInverseFastCore<1861>(index);
    case 244: return radicalInverseFastCore<1867>(index);
    case 245: return radicalInverseFastCore<1871>(index);
    case 246: return radicalInverseFastCore<1873>(index);
    case 247: return radicalInverseFastCore<1877>(index);
    case 248: return radicalInverseFastCore<1879>(index);
    case 249: return radicalInverseFastCore<1889>(index);
    case 250: return radicalInverseFastCore<1901>(index);
    case 251: return radicalInverseFastCore<1907>(index);
    case 252: return radicalInverseFastCore<1913>(index);
    case 253: return radicalInverseFastCore<1931>(index);
    case 254: return radicalInverseFastCore<1933>(index);
    case 255: return radicalInverseFastCore<1949>(index);
    case 256: return radicalInverseFastCore<1951>(index);
    case 257: return radicalInverseFastCore<1973>(index);
    case 258: return radicalInverseFastCore<1979>(index);
    case 259: return radicalInverseFastCore<1987>(index);
    case 260: return radicalInverseFastCore<1993>(index);
    case 261: return radicalInverseFastCore<1997>(index);
    case 262: return radicalInverseFastCore<1999>(index);
    case 263: return radicalInverseFastCore<2003>(index);
    case 264: return radicalInverseFastCore<2011>(index);
    case 265: return radicalInverseFastCore<2017>(index);
    case 266: return radicalInverseFastCore<2027>(index);
    case 267: return radicalInverseFastCore<2029>(index);
    case 268: return radicalInverseFastCore<2039>(index);
    case 269: return radicalInverseFastCore<2053>(index);
    case 270: return radicalInverseFastCore<2063>(index);
    case 271: return radicalInverseFastCore<2069>(index);
    case 272: return radicalInverseFastCore<2081>(index);
    case 273: return radicalInverseFastCore<2083>(index);
    case 274: return radicalInverseFastCore<2087>(index);
    case 275: return radicalInverseFastCore<2089>(index);
    case 276: return radicalInverseFastCore<2099>(index);
    case 277: return radicalInverseFastCore<2111>(index);
    case 278: return radicalInverseFastCore<2113>(index);
    case 279: return radicalInverseFastCore<2129>(index);
    case 280: return radicalInverseFastCore<2131>(index);
    case 281: return radicalInverseFastCore<2137>(index);
    case 282: return radicalInverseFastCore<2141>(index);
    case 283: return radicalInverseFastCore<2143>(index);
    case 284: return radicalInverseFastCore<2153>(index);
    case 285: return radicalInverseFastCore<2161>(index);
    case 286: return radicalInverseFastCore<2179>(index);
    case 287: return radicalInverseFastCore<2203>(index);
    case 288: return radicalInverseFastCore<2207>(index);
    case 289: return radicalInverseFastCore<2213>(index);
    case 290: return radicalInverseFastCore<2221>(index);
    case 291: return radicalInverseFastCore<2237>(index);
    case 292: return radicalInverseFastCore<2239>(index);
    case 293: return radicalInverseFastCore<2243>(index);
    case 294: return radicalInverseFastCore<2251>(index);
    case 295: return radicalInverseFastCore<2267>(index);
    case 296: return radicalInverseFastCore<2269>(index);
    case 297: return radicalInverseFastCore<2273>(index);
    case 298: return radicalInverseFastCore<2281>(index);
    case 299: return radicalInverseFastCore<2287>(index);
    case 300: return radicalInverseFastCore<2293>(index);
    case 301: return radicalInverseFastCore<2297>(index);
    case 302: return radicalInverseFastCore<2309>(index);
    case 303: return radicalInverseFastCore<2311>(index);
    case 304: return radicalInverseFastCore<2333>(index);
    case 305: return radicalInverseFastCore<2339>(index);
    case 306: return radicalInverseFastCore<2341>(index);
    case 307: return radicalInverseFastCore<2347>(index);
    case 308: return radicalInverseFastCore<2351>(index);
    case 309: return radicalInverseFastCore<2357>(index);
    case 310: return radicalInverseFastCore<2371>(index);
    case 311: return radicalInverseFastCore<2377>(index);
    case 312: return radicalInverseFastCore<2381>(index);
    case 313: return radicalInverseFastCore<2383>(index);
    case 314: return radicalInverseFastCore<2389>(index);
    case 315: return radicalInverseFastCore<2393>(index);
    case 316: return radicalInverseFastCore<2399>(index);
    case 317: return radicalInverseFastCore<2411>(index);
    case 318: return radicalInverseFastCore<2417>(index);
    case 319: return radicalInverseFastCore<2423>(index);
    case 320: return radicalInverseFastCore<2437>(index);
    case 321: return radicalInverseFastCore<2441>(index);
    case 322: return radicalInverseFastCore<2447>(index);
    case 323: return radicalInverseFastCore<2459>(index);
    case 324: return radicalInverseFastCore<2467>(index);
    case 325: return radicalInverseFastCore<2473>(index);
    case 326: return radicalInverseFastCore<2477>(index);
    case 327: return radicalInverseFastCore<2503>(index);
    case 328: return radicalInverseFastCore<2521>(index);
    case 329: return radicalInverseFastCore<2531>(index);
    case 330: return radicalInverseFastCore<2539>(index);
    case 331: return radicalInverseFastCore<2543>(index);
    case 332: return radicalInverseFastCore<2549>(index);
    case 333: return radicalInverseFastCore<2551>(index);
    case 334: return radicalInverseFastCore<2557>(index);
    case 335: return radicalInverseFastCore<2579>(index);
    case 336: return radicalInverseFastCore<2591>(index);
    case 337: return radicalInverseFastCore<2593>(index);
    case 338: return radicalInverseFastCore<2609>(index);
    case 339: return radicalInverseFastCore<2617>(index);
    case 340: return radicalInverseFastCore<2621>(index);
    case 341: return radicalInverseFastCore<2633>(index);
    case 342: return radicalInverseFastCore<2647>(index);
    case 343: return radicalInverseFastCore<2657>(index);
    case 344: return radicalInverseFastCore<2659>(index);
    case 345: return radicalInverseFastCore<2663>(index);
    case 346: return radicalInverseFastCore<2671>(index);
    case 347: return radicalInverseFastCore<2677>(index);
    case 348: return radicalInverseFastCore<2683>(index);
    case 349: return radicalInverseFastCore<2687>(index);
    case 350: return radicalInverseFastCore<2689>(index);
    case 351: return radicalInverseFastCore<2693>(index);
    case 352: return radicalInverseFastCore<2699>(index);
    case 353: return radicalInverseFastCore<2707>(index);
    case 354: return radicalInverseFastCore<2711>(index);
    case 355: return radicalInverseFastCore<2713>(index);
    case 356: return radicalInverseFastCore<2719>(index);
    case 357: return radicalInverseFastCore<2729>(index);
    case 358: return radicalInverseFastCore<2731>(index);
    case 359: return radicalInverseFastCore<2741>(index);
    case 360: return radicalInverseFastCore<2749>(index);
    case 361: return radicalInverseFastCore<2753>(index);
    case 362: return radicalInverseFastCore<2767>(index);
    case 363: return radicalInverseFastCore<2777>(index);
    case 364: return radicalInverseFastCore<2789>(index);
    case 365: return radicalInverseFastCore<2791>(index);
    case 366: return radicalInverseFastCore<2797>(index);
    case 367: return radicalInverseFastCore<2801>(index);
    case 368: return radicalInverseFastCore<2803>(index);
    case 369: return radicalInverseFastCore<2819>(index);
    case 370: return radicalInverseFastCore<2833>(index);
    case 371: return radicalInverseFastCore<2837>(index);
    case 372: return radicalInverseFastCore<2843>(index);
    case 373: return radicalInverseFastCore<2851>(index);
    case 374: return radicalInverseFastCore<2857>(index);
    case 375: return radicalInverseFastCore<2861>(index);
    case 376: return radicalInverseFastCore<2879>(index);
    case 377: return radicalInverseFastCore<2887>(index);
    case 378: return radicalInverseFastCore<2897>(index);
    case 379: return radicalInverseFastCore<2903>(index);
    case 380: return radicalInverseFastCore<2909>(index);
    case 381: return radicalInverseFastCore<2917>(index);
    case 382: return radicalInverseFastCore<2927>(index);
    case 383: return radicalInverseFastCore<2939>(index);
    case 384: return radicalInverseFastCore<2953>(index);
    case 385: return radicalInverseFastCore<2957>(index);
    case 386: return radicalInverseFastCore<2963>(index);
    case 387: return radicalInverseFastCore<2969>(index);
    case 388: return radicalInverseFastCore<2971>(index);
    case 389: return radicalInverseFastCore<2999>(index);
    case 390: return radicalInverseFastCore<3001>(index);
    case 391: return radicalInverseFastCore<3011>(index);
    case 392: return radicalInverseFastCore<3019>(index);
    case 393: return radicalInverseFastCore<3023>(index);
    case 394: return radicalInverseFastCore<3037>(index);
    case 395: return radicalInverseFastCore<3041>(index);
    case 396: return radicalInverseFastCore<3049>(index);
    case 397: return radicalInverseFastCore<3061>(index);
    case 398: return radicalInverseFastCore<3067>(index);
    case 399: return radicalInverseFastCore<3079>(index);
    case 400: return radicalInverseFastCore<3083>(index);
    case 401: return radicalInverseFastCore<3089>(index);
    case 402: return radicalInverseFastCore<3109>(index);
    case 403: return radicalInverseFastCore<3119>(index);
    case 404: return radicalInverseFastCore<3121>(index);
    case 405: return radicalInverseFastCore<3137>(index);
    case 406: return radicalInverseFastCore<3163>(index);
    case 407: return radicalInverseFastCore<3167>(index);
    case 408: return radicalInverseFastCore<3169>(index);
    case 409: return radicalInverseFastCore<3181>(index);
    case 410: return radicalInverseFastCore<3187>(index);
    case 411: return radicalInverseFastCore<3191>(index);
    case 412: return radicalInverseFastCore<3203>(index);
    case 413: return radicalInverseFastCore<3209>(index);
    case 414: return radicalInverseFastCore<3217>(index);
    case 415: return radicalInverseFastCore<3221>(index);
    case 416: return radicalInverseFastCore<3229>(index);
    case 417: return radicalInverseFastCore<3251>(index);
    case 418: return radicalInverseFastCore<3253>(index);
    case 419: return radicalInverseFastCore<3257>(index);
    case 420: return radicalInverseFastCore<3259>(index);
    case 421: return radicalInverseFastCore<3271>(index);
    case 422: return radicalInverseFastCore<3299>(index);
    case 423: return radicalInverseFastCore<3301>(index);
    case 424: return radicalInverseFastCore<3307>(index);
    case 425: return radicalInverseFastCore<3313>(index);
    case 426: return radicalInverseFastCore<3319>(index);
    case 427: return radicalInverseFastCore<3323>(index);
    case 428: return radicalInverseFastCore<3329>(index);
    case 429: return radicalInverseFastCore<3331>(index);
    case 430: return radicalInverseFastCore<3343>(index);
    case 431: return radicalInverseFastCore<3347>(index);
    case 432: return radicalInverseFastCore<3359>(index);
    case 433: return radicalInverseFastCore<3361>(index);
    case 434: return radicalInverseFastCore<3371>(index);
    case 435: return radicalInverseFastCore<3373>(index);
    case 436: return radicalInverseFastCore<3389>(index);
    case 437: return radicalInverseFastCore<3391>(index);
    case 438: return radicalInverseFastCore<3407>(index);
    case 439: return radicalInverseFastCore<3413>(index);
    case 440: return radicalInverseFastCore<3433>(index);
    case 441: return radicalInverseFastCore<3449>(index);
    case 442: return radicalInverseFastCore<3457>(index);
    case 443: return radicalInverseFastCore<3461>(index);
    case 444: return radicalInverseFastCore<3463>(index);
    case 445: return radicalInverseFastCore<3467>(index);
    case 446: return radicalInverseFastCore<3469>(index);
    case 447: return radicalInverseFastCore<3491>(index);
    case 448: return radicalInverseFastCore<3499>(index);
    case 449: return radicalInverseFastCore<3511>(index);
    case 450: return radicalInverseFastCore<3517>(index);
    case 451: return radicalInverseFastCore<3527>(index);
    case 452: return radicalInverseFastCore<3529>(index);
    case 453: return radicalInverseFastCore<3533>(index);
    case 454: return radicalInverseFastCore<3539>(index);
    case 455: return radicalInverseFastCore<3541>(index);
    case 456: return radicalInverseFastCore<3547>(index);
    case 457: return radicalInverseFastCore<3557>(index);
    case 458: return radicalInverseFastCore<3559>(index);
    case 459: return radicalInverseFastCore<3571>(index);
    case 460: return radicalInverseFastCore<3581>(index);
    case 461: return radicalInverseFastCore<3583>(index);
    case 462: return radicalInverseFastCore<3593>(index);
    case 463: return radicalInverseFastCore<3607>(index);
    case 464: return radicalInverseFastCore<3613>(index);
    case 465: return radicalInverseFastCore<3617>(index);
    case 466: return radicalInverseFastCore<3623>(index);
    case 467: return radicalInverseFastCore<3631>(index);
    case 468: return radicalInverseFastCore<3637>(index);
    case 469: return radicalInverseFastCore<3643>(index);
    case 470: return radicalInverseFastCore<3659>(index);
    case 471: return radicalInverseFastCore<3671>(index);
    case 472: return radicalInverseFastCore<3673>(index);
    case 473: return radicalInverseFastCore<3677>(index);
    case 474: return radicalInverseFastCore<3691>(index);
    case 475: return radicalInverseFastCore<3697>(index);
    case 476: return radicalInverseFastCore<3701>(index);
    case 477: return radicalInverseFastCore<3709>(index);
    case 478: return radicalInverseFastCore<3719>(index);
    case 479: return radicalInverseFastCore<3727>(index);
    case 480: return radicalInverseFastCore<3733>(index);
    case 481: return radicalInverseFastCore<3739>(index);
    case 482: return radicalInverseFastCore<3761>(index);
    case 483: return radicalInverseFastCore<3767>(index);
    case 484: return radicalInverseFastCore<3769>(index);
    case 485: return radicalInverseFastCore<3779>(index);
    case 486: return radicalInverseFastCore<3793>(index);
    case 487: return radicalInverseFastCore<3797>(index);
    case 488: return radicalInverseFastCore<3803>(index);
    case 489: return radicalInverseFastCore<3821>(index);
    case 490: return radicalInverseFastCore<3823>(index);
    case 491: return radicalInverseFastCore<3833>(index);
    case 492: return radicalInverseFastCore<3847>(index);
    case 493: return radicalInverseFastCore<3851>(index);
    case 494: return radicalInverseFastCore<3853>(index);
    case 495: return radicalInverseFastCore<3863>(index);
    case 496: return radicalInverseFastCore<3877>(index);
    case 497: return radicalInverseFastCore<3881>(index);
    case 498: return radicalInverseFastCore<3889>(index);
    case 499: return radicalInverseFastCore<3907>(index);
    case 500: return radicalInverseFastCore<3911>(index);
    case 501: return radicalInverseFastCore<3917>(index);
    case 502: return radicalInverseFastCore<3919>(index);
    case 503: return radicalInverseFastCore<3923>(index);
    case 504: return radicalInverseFastCore<3929>(index);
    case 505: return radicalInverseFastCore<3931>(index);
    case 506: return radicalInverseFastCore<3943>(index);
    case 507: return radicalInverseFastCore<3947>(index);
    case 508: return radicalInverseFastCore<3967>(index);
    case 509: return radicalInverseFastCore<3989>(index);
    case 510: return radicalInverseFastCore<4001>(index);
    case 511: return radicalInverseFastCore<4003>(index);
    case 512: return radicalInverseFastCore<4007>(index);
    case 513: return radicalInverseFastCore<4013>(index);
    case 514: return radicalInverseFastCore<4019>(index);
    case 515: return radicalInverseFastCore<4021>(index);
    case 516: return radicalInverseFastCore<4027>(index);
    case 517: return radicalInverseFastCore<4049>(index);
    case 518: return radicalInverseFastCore<4051>(index);
    case 519: return radicalInverseFastCore<4057>(index);
    case 520: return radicalInverseFastCore<4073>(index);
    case 521: return radicalInverseFastCore<4079>(index);
    case 522: return radicalInverseFastCore<4091>(index);
    case 523: return radicalInverseFastCore<4093>(index);
    case 524: return radicalInverseFastCore<4099>(index);
    case 525: return radicalInverseFastCore<4111>(index);
    case 526: return radicalInverseFastCore<4127>(index);
    case 527: return radicalInverseFastCore<4129>(index);
    case 528: return radicalInverseFastCore<4133>(index);
    case 529: return radicalInverseFastCore<4139>(index);
    case 530: return radicalInverseFastCore<4153>(index);
    case 531: return radicalInverseFastCore<4157>(index);
    case 532: return radicalInverseFastCore<4159>(index);
    case 533: return radicalInverseFastCore<4177>(index);
    case 534: return radicalInverseFastCore<4201>(index);
    case 535: return radicalInverseFastCore<4211>(index);
    case 536: return radicalInverseFastCore<4217>(index);
    case 537: return radicalInverseFastCore<4219>(index);
    case 538: return radicalInverseFastCore<4229>(index);
    case 539: return radicalInverseFastCore<4231>(index);
    case 540: return radicalInverseFastCore<4241>(index);
    case 541: return radicalInverseFastCore<4243>(index);
    case 542: return radicalInverseFastCore<4253>(index);
    case 543: return radicalInverseFastCore<4259>(index);
    case 544: return radicalInverseFastCore<4261>(index);
    case 545: return radicalInverseFastCore<4271>(index);
    case 546: return radicalInverseFastCore<4273>(index);
    case 547: return radicalInverseFastCore<4283>(index);
    case 548: return radicalInverseFastCore<4289>(index);
    case 549: return radicalInverseFastCore<4297>(index);
    case 550: return radicalInverseFastCore<4327>(index);
    case 551: return radicalInverseFastCore<4337>(index);
    case 552: return radicalInverseFastCore<4339>(index);
    case 553: return radicalInverseFastCore<4349>(index);
    case 554: return radicalInverseFastCore<4357>(index);
    case 555: return radicalInverseFastCore<4363>(index);
    case 556: return radicalInverseFastCore<4373>(index);
    case 557: return radicalInverseFastCore<4391>(index);
    case 558: return radicalInverseFastCore<4397>(index);
    case 559: return radicalInverseFastCore<4409>(index);
    case 560: return radicalInverseFastCore<4421>(index);
    case 561: return radicalInverseFastCore<4423>(index);
    case 562: return radicalInverseFastCore<4441>(index);
    case 563: return radicalInverseFastCore<4447>(index);
    case 564: return radicalInverseFastCore<4451>(index);
    case 565: return radicalInverseFastCore<4457>(index);
    case 566: return radicalInverseFastCore<4463>(index);
    case 567: return radicalInverseFastCore<4481>(index);
    case 568: return radicalInverseFastCore<4483>(index);
    case 569: return radicalInverseFastCore<4493>(index);
    case 570: return radicalInverseFastCore<4507>(index);
    case 571: return radicalInverseFastCore<4513>(index);
    case 572: return radicalInverseFastCore<4517>(index);
    case 573: return radicalInverseFastCore<4519>(index);
    case 574: return radicalInverseFastCore<4523>(index);
    case 575: return radicalInverseFastCore<4547>(index);
    case 576: return radicalInverseFastCore<4549>(index);
    case 577: return radicalInverseFastCore<4561>(index);
    case 578: return radicalInverseFastCore<4567>(index);
    case 579: return radicalInverseFastCore<4583>(index);
    case 580: return radicalInverseFastCore<4591>(index);
    case 581: return radicalInverseFastCore<4597>(index);
    case 582: return radicalInverseFastCore<4603>(index);
    case 583: return radicalInverseFastCore<4621>(index);
    case 584: return radicalInverseFastCore<4637>(index);
    case 585: return radicalInverseFastCore<4639>(index);
    case 586: return radicalInverseFastCore<4643>(index);
    case 587: return radicalInverseFastCore<4649>(index);
    case 588: return radicalInverseFastCore<4651>(index);
    case 589: return radicalInverseFastCore<4657>(index);
    case 590: return radicalInverseFastCore<4663>(index);
    case 591: return radicalInverseFastCore<4673>(index);
    case 592: return radicalInverseFastCore<4679>(index);
    case 593: return radicalInverseFastCore<4691>(index);
    case 594: return radicalInverseFastCore<4703>(index);
    case 595: return radicalInverseFastCore<4721>(index);
    case 596: return radicalInverseFastCore<4723>(index);
    case 597: return radicalInverseFastCore<4729>(index);
    case 598: return radicalInverseFastCore<4733>(index);
    case 599: return radicalInverseFastCore<4751>(index);
    case 600: return radicalInverseFastCore<4759>(index);
    case 601: return radicalInverseFastCore<4783>(index);
    case 602: return radicalInverseFastCore<4787>(index);
    case 603: return radicalInverseFastCore<4789>(index);
    case 604: return radicalInverseFastCore<4793>(index);
    case 605: return radicalInverseFastCore<4799>(index);
    case 606: return radicalInverseFastCore<4801>(index);
    case 607: return radicalInverseFastCore<4813>(index);
    case 608: return radicalInverseFastCore<4817>(index);
    case 609: return radicalInverseFastCore<4831>(index);
    case 610: return radicalInverseFastCore<4861>(index);
    case 611: return radicalInverseFastCore<4871>(index);
    case 612: return radicalInverseFastCore<4877>(index);
    case 613: return radicalInverseFastCore<4889>(index);
    case 614: return radicalInverseFastCore<4903>(index);
    case 615: return radicalInverseFastCore<4909>(index);
    case 616: return radicalInverseFastCore<4919>(index);
    case 617: return radicalInverseFastCore<4931>(index);
    case 618: return radicalInverseFastCore<4933>(index);
    case 619: return radicalInverseFastCore<4937>(index);
    case 620: return radicalInverseFastCore<4943>(index);
    case 621: return radicalInverseFastCore<4951>(index);
    case 622: return radicalInverseFastCore<4957>(index);
    case 623: return radicalInverseFastCore<4967>(index);
    case 624: return radicalInverseFastCore<4969>(index);
    case 625: return radicalInverseFastCore<4973>(index);
    case 626: return radicalInverseFastCore<4987>(index);
    case 627: return radicalInverseFastCore<4993>(index);
    case 628: return radicalInverseFastCore<4999>(index);
    case 629: return radicalInverseFastCore<5003>(index);
    case 630: return radicalInverseFastCore<5009>(index);
    case 631: return radicalInverseFastCore<5011>(index);
    case 632: return radicalInverseFastCore<5021>(index);
    case 633: return radicalInverseFastCore<5023>(index);
    case 634: return radicalInverseFastCore<5039>(index);
    case 635: return radicalInverseFastCore<5051>(index);
    case 636: return radicalInverseFastCore<5059>(index);
    case 637: return radicalInverseFastCore<5077>(index);
    case 638: return radicalInverseFastCore<5081>(index);
    case 639: return radicalInverseFastCore<5087>(index);
    case 640: return radicalInverseFastCore<5099>(index);
    case 641: return radicalInverseFastCore<5101>(index);
    case 642: return radicalInverseFastCore<5107>(index);
    case 643: return radicalInverseFastCore<5113>(index);
    case 644: return radicalInverseFastCore<5119>(index);
    case 645: return radicalInverseFastCore<5147>(index);
    case 646: return radicalInverseFastCore<5153>(index);
    case 647: return radicalInverseFastCore<5167>(index);
    case 648: return radicalInverseFastCore<5171>(index);
    case 649: return radicalInverseFastCore<5179>(index);
    case 650: return radicalInverseFastCore<5189>(index);
    case 651: return radicalInverseFastCore<5197>(index);
    case 652: return radicalInverseFastCore<5209>(index);
    case 653: return radicalInverseFastCore<5227>(index);
    case 654: return radicalInverseFastCore<5231>(index);
    case 655: return radicalInverseFastCore<5233>(index);
    case 656: return radicalInverseFastCore<5237>(index);
    case 657: return radicalInverseFastCore<5261>(index);
    case 658: return radicalInverseFastCore<5273>(index);
    case 659: return radicalInverseFastCore<5279>(index);
    case 660: return radicalInverseFastCore<5281>(index);
    case 661: return radicalInverseFastCore<5297>(index);
    case 662: return radicalInverseFastCore<5303>(index);
    case 663: return radicalInverseFastCore<5309>(index);
    case 664: return radicalInverseFastCore<5323>(index);
    case 665: return radicalInverseFastCore<5333>(index);
    case 666: return radicalInverseFastCore<5347>(index);
    case 667: return radicalInverseFastCore<5351>(index);
    case 668: return radicalInverseFastCore<5381>(index);
    case 669: return radicalInverseFastCore<5387>(index);
    case 670: return radicalInverseFastCore<5393>(index);
    case 671: return radicalInverseFastCore<5399>(index);
    case 672: return radicalInverseFastCore<5407>(index);
    case 673: return radicalInverseFastCore<5413>(index);
    case 674: return radicalInverseFastCore<5417>(index);
    case 675: return radicalInverseFastCore<5419>(index);
    case 676: return radicalInverseFastCore<5431>(index);
    case 677: return radicalInverseFastCore<5437>(index);
    case 678: return radicalInverseFastCore<5441>(index);
    case 679: return radicalInverseFastCore<5443>(index);
    case 680: return radicalInverseFastCore<5449>(index);
    case 681: return radicalInverseFastCore<5471>(index);
    case 682: return radicalInverseFastCore<5477>(index);
    case 683: return radicalInverseFastCore<5479>(index);
    case 684: return radicalInverseFastCore<5483>(index);
    case 685: return radicalInverseFastCore<5501>(index);
    case 686: return radicalInverseFastCore<5503>(index);
    case 687: return radicalInverseFastCore<5507>(index);
    case 688: return radicalInverseFastCore<5519>(index);
    case 689: return radicalInverseFastCore<5521>(index);
    case 690: return radicalInverseFastCore<5527>(index);
    case 691: return radicalInverseFastCore<5531>(index);
    case 692: return radicalInverseFastCore<5557>(index);
    case 693: return radicalInverseFastCore<5563>(index);
    case 694: return radicalInverseFastCore<5569>(index);
    case 695: return radicalInverseFastCore<5573>(index);
    case 696: return radicalInverseFastCore<5581>(index);
    case 697: return radicalInverseFastCore<5591>(index);
    case 698: return radicalInverseFastCore<5623>(index);
    case 699: return radicalInverseFastCore<5639>(index);
    case 700: return radicalInverseFastCore<5641>(index);
    case 701: return radicalInverseFastCore<5647>(index);
    case 702: return radicalInverseFastCore<5651>(index);
    case 703: return radicalInverseFastCore<5653>(index);
    case 704: return radicalInverseFastCore<5657>(index);
    case 705: return radicalInverseFastCore<5659>(index);
    case 706: return radicalInverseFastCore<5669>(index);
    case 707: return radicalInverseFastCore<5683>(index);
    case 708: return radicalInverseFastCore<5689>(index);
    case 709: return radicalInverseFastCore<5693>(index);
    case 710: return radicalInverseFastCore<5701>(index);
    case 711: return radicalInverseFastCore<5711>(index);
    case 712: return radicalInverseFastCore<5717>(index);
    case 713: return radicalInverseFastCore<5737>(index);
    case 714: return radicalInverseFastCore<5741>(index);
    case 715: return radicalInverseFastCore<5743>(index);
    case 716: return radicalInverseFastCore<5749>(index);
    case 717: return radicalInverseFastCore<5779>(index);
    case 718: return radicalInverseFastCore<5783>(index);
    case 719: return radicalInverseFastCore<5791>(index);
    case 720: return radicalInverseFastCore<5801>(index);
    case 721: return radicalInverseFastCore<5807>(index);
    case 722: return radicalInverseFastCore<5813>(index);
    case 723: return radicalInverseFastCore<5821>(index);
    case 724: return radicalInverseFastCore<5827>(index);
    case 725: return radicalInverseFastCore<5839>(index);
    case 726: return radicalInverseFastCore<5843>(index);
    case 727: return radicalInverseFastCore<5849>(index);
    case 728: return radicalInverseFastCore<5851>(index);
    case 729: return radicalInverseFastCore<5857>(index);
    case 730: return radicalInverseFastCore<5861>(index);
    case 731: return radicalInverseFastCore<5867>(index);
    case 732: return radicalInverseFastCore<5869>(index);
    case 733: return radicalInverseFastCore<5879>(index);
    case 734: return radicalInverseFastCore<5881>(index);
    case 735: return radicalInverseFastCore<5897>(index);
    case 736: return radicalInverseFastCore<5903>(index);
    case 737: return radicalInverseFastCore<5923>(index);
    case 738: return radicalInverseFastCore<5927>(index);
    case 739: return radicalInverseFastCore<5939>(index);
    case 740: return radicalInverseFastCore<5953>(index);
    case 741: return radicalInverseFastCore<5981>(index);
    case 742: return radicalInverseFastCore<5987>(index);
    case 743: return radicalInverseFastCore<6007>(index);
    case 744: return radicalInverseFastCore<6011>(index);
    case 745: return radicalInverseFastCore<6029>(index);
    case 746: return radicalInverseFastCore<6037>(index);
    case 747: return radicalInverseFastCore<6043>(index);
    case 748: return radicalInverseFastCore<6047>(index);
    case 749: return radicalInverseFastCore<6053>(index);
    case 750: return radicalInverseFastCore<6067>(index);
    case 751: return radicalInverseFastCore<6073>(index);
    case 752: return radicalInverseFastCore<6079>(index);
    case 753: return radicalInverseFastCore<6089>(index);
    case 754: return radicalInverseFastCore<6091>(index);
    case 755: return radicalInverseFastCore<6101>(index);
    case 756: return radicalInverseFastCore<6113>(index);
    case 757: return radicalInverseFastCore<6121>(index);
    case 758: return radicalInverseFastCore<6131>(index);
    case 759: return radicalInverseFastCore<6133>(index);
    case 760: return radicalInverseFastCore<6143>(index);
    case 761: return radicalInverseFastCore<6151>(index);
    case 762: return radicalInverseFastCore<6163>(index);
    case 763: return radicalInverseFastCore<6173>(index);
    case 764: return radicalInverseFastCore<6197>(index);
    case 765: return radicalInverseFastCore<6199>(index);
    case 766: return radicalInverseFastCore<6203>(index);
    case 767: return radicalInverseFastCore<6211>(index);
    case 768: return radicalInverseFastCore<6217>(index);
    case 769: return radicalInverseFastCore<6221>(index);
    case 770: return radicalInverseFastCore<6229>(index);
    case 771: return radicalInverseFastCore<6247>(index);
    case 772: return radicalInverseFastCore<6257>(index);
    case 773: return radicalInverseFastCore<6263>(index);
    case 774: return radicalInverseFastCore<6269>(index);
    case 775: return radicalInverseFastCore<6271>(index);
    case 776: return radicalInverseFastCore<6277>(index);
    case 777: return radicalInverseFastCore<6287>(index);
    case 778: return radicalInverseFastCore<6299>(index);
    case 779: return radicalInverseFastCore<6301>(index);
    case 780: return radicalInverseFastCore<6311>(index);
    case 781: return radicalInverseFastCore<6317>(index);
    case 782: return radicalInverseFastCore<6323>(index);
    case 783: return radicalInverseFastCore<6329>(index);
    case 784: return radicalInverseFastCore<6337>(index);
    case 785: return radicalInverseFastCore<6343>(index);
    case 786: return radicalInverseFastCore<6353>(index);
    case 787: return radicalInverseFastCore<6359>(index);
    case 788: return radicalInverseFastCore<6361>(index);
    case 789: return radicalInverseFastCore<6367>(index);
    case 790: return radicalInverseFastCore<6373>(index);
    case 791: return radicalInverseFastCore<6379>(index);
    case 792: return radicalInverseFastCore<6389>(index);
    case 793: return radicalInverseFastCore<6397>(index);
    case 794: return radicalInverseFastCore<6421>(index);
    case 795: return radicalInverseFastCore<6427>(index);
    case 796: return radicalInverseFastCore<6449>(index);
    case 797: return radicalInverseFastCore<6451>(index);
    case 798: return radicalInverseFastCore<6469>(index);
    case 799: return radicalInverseFastCore<6473>(index);
    case 800: return radicalInverseFastCore<6481>(index);
    case 801: return radicalInverseFastCore<6491>(index);
    case 802: return radicalInverseFastCore<6521>(index);
    case 803: return radicalInverseFastCore<6529>(index);
    case 804: return radicalInverseFastCore<6547>(index);
    case 805: return radicalInverseFastCore<6551>(index);
    case 806: return radicalInverseFastCore<6553>(index);
    case 807: return radicalInverseFastCore<6563>(index);
    case 808: return radicalInverseFastCore<6569>(index);
    case 809: return radicalInverseFastCore<6571>(index);
    case 810: return radicalInverseFastCore<6577>(index);
    case 811: return radicalInverseFastCore<6581>(index);
    case 812: return radicalInverseFastCore<6599>(index);
    case 813: return radicalInverseFastCore<6607>(index);
    case 814: return radicalInverseFastCore<6619>(index);
    case 815: return radicalInverseFastCore<6637>(index);
    case 816: return radicalInverseFastCore<6653>(index);
    case 817: return radicalInverseFastCore<6659>(index);
    case 818: return radicalInverseFastCore<6661>(index);
    case 819: return radicalInverseFastCore<6673>(index);
    case 820: return radicalInverseFastCore<6679>(index);
    case 821: return radicalInverseFastCore<6689>(index);
    case 822: return radicalInverseFastCore<6691>(index);
    case 823: return radicalInverseFastCore<6701>(index);
    case 824: return radicalInverseFastCore<6703>(index);
    case 825: return radicalInverseFastCore<6709>(index);
    case 826: return radicalInverseFastCore<6719>(index);
    case 827: return radicalInverseFastCore<6733>(index);
    case 828: return radicalInverseFastCore<6737>(index);
    case 829: return radicalInverseFastCore<6761>(index);
    case 830: return radicalInverseFastCore<6763>(index);
    case 831: return radicalInverseFastCore<6779>(index);
    case 832: return radicalInverseFastCore<6781>(index);
    case 833: return radicalInverseFastCore<6791>(index);
    case 834: return radicalInverseFastCore<6793>(index);
    case 835: return radicalInverseFastCore<6803>(index);
    case 836: return radicalInverseFastCore<6823>(index);
    case 837: return radicalInverseFastCore<6827>(index);
    case 838: return radicalInverseFastCore<6829>(index);
    case 839: return radicalInverseFastCore<6833>(index);
    case 840: return radicalInverseFastCore<6841>(index);
    case 841: return radicalInverseFastCore<6857>(index);
    case 842: return radicalInverseFastCore<6863>(index);
    case 843: return radicalInverseFastCore<6869>(index);
    case 844: return radicalInverseFastCore<6871>(index);
    case 845: return radicalInverseFastCore<6883>(index);
    case 846: return radicalInverseFastCore<6899>(index);
    case 847: return radicalInverseFastCore<6907>(index);
    case 848: return radicalInverseFastCore<6911>(index);
    case 849: return radicalInverseFastCore<6917>(index);
    case 850: return radicalInverseFastCore<6947>(index);
    case 851: return radicalInverseFastCore<6949>(index);
    case 852: return radicalInverseFastCore<6959>(index);
    case 853: return radicalInverseFastCore<6961>(index);
    case 854: return radicalInverseFastCore<6967>(index);
    case 855: return radicalInverseFastCore<6971>(index);
    case 856: return radicalInverseFastCore<6977>(index);
    case 857: return radicalInverseFastCore<6983>(index);
    case 858: return radicalInverseFastCore<6991>(index);
    case 859: return radicalInverseFastCore<6997>(index);
    case 860: return radicalInverseFastCore<7001>(index);
    case 861: return radicalInverseFastCore<7013>(index);
    case 862: return radicalInverseFastCore<7019>(index);
    case 863: return radicalInverseFastCore<7027>(index);
    case 864: return radicalInverseFastCore<7039>(index);
    case 865: return radicalInverseFastCore<7043>(index);
    case 866: return radicalInverseFastCore<7057>(index);
    case 867: return radicalInverseFastCore<7069>(index);
    case 868: return radicalInverseFastCore<7079>(index);
    case 869: return radicalInverseFastCore<7103>(index);
    case 870: return radicalInverseFastCore<7109>(index);
    case 871: return radicalInverseFastCore<7121>(index);
    case 872: return radicalInverseFastCore<7127>(index);
    case 873: return radicalInverseFastCore<7129>(index);
    case 874: return radicalInverseFastCore<7151>(index);
    case 875: return radicalInverseFastCore<7159>(index);
    case 876: return radicalInverseFastCore<7177>(index);
    case 877: return radicalInverseFastCore<7187>(index);
    case 878: return radicalInverseFastCore<7193>(index);
    case 879: return radicalInverseFastCore<7207>(index);
    case 880: return radicalInverseFastCore<7211>(index);
    case 881: return radicalInverseFastCore<7213>(index);
    case 882: return radicalInverseFastCore<7219>(index);
    case 883: return radicalInverseFastCore<7229>(index);
    case 884: return radicalInverseFastCore<7237>(index);
    case 885: return radicalInverseFastCore<7243>(index);
    case 886: return radicalInverseFastCore<7247>(index);
    case 887: return radicalInverseFastCore<7253>(index);
    case 888: return radicalInverseFastCore<7283>(index);
    case 889: return radicalInverseFastCore<7297>(index);
    case 890: return radicalInverseFastCore<7307>(index);
    case 891: return radicalInverseFastCore<7309>(index);
    case 892: return radicalInverseFastCore<7321>(index);
    case 893: return radicalInverseFastCore<7331>(index);
    case 894: return radicalInverseFastCore<7333>(index);
    case 895: return radicalInverseFastCore<7349>(index);
    case 896: return radicalInverseFastCore<7351>(index);
    case 897: return radicalInverseFastCore<7369>(index);
    case 898: return radicalInverseFastCore<7393>(index);
    case 899: return radicalInverseFastCore<7411>(index);
    case 900: return radicalInverseFastCore<7417>(index);
    case 901: return radicalInverseFastCore<7433>(index);
    case 902: return radicalInverseFastCore<7451>(index);
    case 903: return radicalInverseFastCore<7457>(index);
    case 904: return radicalInverseFastCore<7459>(index);
    case 905: return radicalInverseFastCore<7477>(index);
    case 906: return radicalInverseFastCore<7481>(index);
    case 907: return radicalInverseFastCore<7487>(index);
    case 908: return radicalInverseFastCore<7489>(index);
    case 909: return radicalInverseFastCore<7499>(index);
    case 910: return radicalInverseFastCore<7507>(index);
    case 911: return radicalInverseFastCore<7517>(index);
    case 912: return radicalInverseFastCore<7523>(index);
    case 913: return radicalInverseFastCore<7529>(index);
    case 914: return radicalInverseFastCore<7537>(index);
    case 915: return radicalInverseFastCore<7541>(index);
    case 916: return radicalInverseFastCore<7547>(index);
    case 917: return radicalInverseFastCore<7549>(index);
    case 918: return radicalInverseFastCore<7559>(index);
    case 919: return radicalInverseFastCore<7561>(index);
    case 920: return radicalInverseFastCore<7573>(index);
    case 921: return radicalInverseFastCore<7577>(index);
    case 922: return radicalInverseFastCore<7583>(index);
    case 923: return radicalInverseFastCore<7589>(index);
    case 924: return radicalInverseFastCore<7591>(index);
    case 925: return radicalInverseFastCore<7603>(index);
    case 926: return radicalInverseFastCore<7607>(index);
    case 927: return radicalInverseFastCore<7621>(index);
    case 928: return radicalInverseFastCore<7639>(index);
    case 929: return radicalInverseFastCore<7643>(index);
    case 930: return radicalInverseFastCore<7649>(index);
    case 931: return radicalInverseFastCore<7669>(index);
    case 932: return radicalInverseFastCore<7673>(index);
    case 933: return radicalInverseFastCore<7681>(index);
    case 934: return radicalInverseFastCore<7687>(index);
    case 935: return radicalInverseFastCore<7691>(index);
    case 936: return radicalInverseFastCore<7699>(index);
    case 937: return radicalInverseFastCore<7703>(index);
    case 938: return radicalInverseFastCore<7717>(index);
    case 939: return radicalInverseFastCore<7723>(index);
    case 940: return radicalInverseFastCore<7727>(index);
    case 941: return radicalInverseFastCore<7741>(index);
    case 942: return radicalInverseFastCore<7753>(index);
    case 943: return radicalInverseFastCore<7757>(index);
    case 944: return radicalInverseFastCore<7759>(index);
    case 945: return radicalInverseFastCore<7789>(index);
    case 946: return radicalInverseFastCore<7793>(index);
    case 947: return radicalInverseFastCore<7817>(index);
    case 948: return radicalInverseFastCore<7823>(index);
    case 949: return radicalInverseFastCore<7829>(index);
    case 950: return radicalInverseFastCore<7841>(index);
    case 951: return radicalInverseFastCore<7853>(index);
    case 952: return radicalInverseFastCore<7867>(index);
    case 953: return radicalInverseFastCore<7873>(index);
    case 954: return radicalInverseFastCore<7877>(index);
    case 955: return radicalInverseFastCore<7879>(index);
    case 956: return radicalInverseFastCore<7883>(index);
    case 957: return radicalInverseFastCore<7901>(index);
    case 958: return radicalInverseFastCore<7907>(index);
    case 959: return radicalInverseFastCore<7919>(index);
    case 960: return radicalInverseFastCore<7927>(index);
    case 961: return radicalInverseFastCore<7933>(index);
    case 962: return radicalInverseFastCore<7937>(index);
    case 963: return radicalInverseFastCore<7949>(index);
    case 964: return radicalInverseFastCore<7951>(index);
    case 965: return radicalInverseFastCore<7963>(index);
    case 966: return radicalInverseFastCore<7993>(index);
    case 967: return radicalInverseFastCore<8009>(index);
    case 968: return radicalInverseFastCore<8011>(index);
    case 969: return radicalInverseFastCore<8017>(index);
    case 970: return radicalInverseFastCore<8039>(index);
    case 971: return radicalInverseFastCore<8053>(index);
    case 972: return radicalInverseFastCore<8059>(index);
    case 973: return radicalInverseFastCore<8069>(index);
    case 974: return radicalInverseFastCore<8081>(index);
    case 975: return radicalInverseFastCore<8087>(index);
    case 976: return radicalInverseFastCore<8089>(index);
    case 977: return radicalInverseFastCore<8093>(index);
    case 978: return radicalInverseFastCore<8101>(index);
    case 979: return radicalInverseFastCore<8111>(index);
    case 980: return radicalInverseFastCore<8117>(index);
    case 981: return radicalInverseFastCore<8123>(index);
    case 982: return radicalInverseFastCore<8147>(index);
    case 983: return radicalInverseFastCore<8161>(index);
    case 984: return radicalInverseFastCore<8167>(index);
    case 985: return radicalInverseFastCore<8171>(index);
    case 986: return radicalInverseFastCore<8179>(index);
    case 987: return radicalInverseFastCore<8191>(index);
    case 988: return radicalInverseFastCore<8209>(index);
    case 989: return radicalInverseFastCore<8219>(index);
    case 990: return radicalInverseFastCore<8221>(index);
    case 991: return radicalInverseFastCore<8231>(index);
    case 992: return radicalInverseFastCore<8233>(index);
    case 993: return radicalInverseFastCore<8237>(index);
    case 994: return radicalInverseFastCore<8243>(index);
    case 995: return radicalInverseFastCore<8263>(index);
    case 996: return radicalInverseFastCore<8269>(index);
    case 997: return radicalInverseFastCore<8273>(index);
    case 998: return radicalInverseFastCore<8287>(index);
    case 999: return radicalInverseFastCore<8291>(index);
    case 1000: return radicalInverseFastCore<8293>(index);
    case 1001: return radicalInverseFastCore<8297>(index);
    case 1002: return radicalInverseFastCore<8311>(index);
    case 1003: return radicalInverseFastCore<8317>(index);
    case 1004: return radicalInverseFastCore<8329>(index);
    case 1005: return radicalInverseFastCore<8353>(index);
    case 1006: return radicalInverseFastCore<8363>(index);
    case 1007: return radicalInverseFastCore<8369>(index);
    case 1008: return radicalInverseFastCore<8377>(index);
    case 1009: return radicalInverseFastCore<8387>(index);
    case 1010: return radicalInverseFastCore<8389>(index);
    case 1011: return radicalInverseFastCore<8419>(index);
    case 1012: return radicalInverseFastCore<8423>(index);
    case 1013: return radicalInverseFastCore<8429>(index);
    case 1014: return radicalInverseFastCore<8431>(index);
    case 1015: return radicalInverseFastCore<8443>(index);
    case 1016: return radicalInverseFastCore<8447>(index);
    case 1017: return radicalInverseFastCore<8461>(index);
    case 1018: return radicalInverseFastCore<8467>(index);
    case 1019: return radicalInverseFastCore<8501>(index);
    case 1020: return radicalInverseFastCore<8513>(index);
    case 1021: return radicalInverseFastCore<8521>(index);
    case 1022: return radicalInverseFastCore<8527>(index);
    case 1023: return radicalInverseFastCore<8537>(index);
    case 1024: return radicalInverseFastCore<8539>(index);
    case 1025: return radicalInverseFastCore<8543>(index);
    case 1026: return radicalInverseFastCore<8563>(index);
    case 1027: return radicalInverseFastCore<8573>(index);
    case 1028: return radicalInverseFastCore<8581>(index);
    case 1029: return radicalInverseFastCore<8597>(index);
    case 1030: return radicalInverseFastCore<8599>(index);
    case 1031: return radicalInverseFastCore<8609>(index);
    case 1032: return radicalInverseFastCore<8623>(index);
    case 1033: return radicalInverseFastCore<8627>(index);
    case 1034: return radicalInverseFastCore<8629>(index);
    case 1035: return radicalInverseFastCore<8641>(index);
    case 1036: return radicalInverseFastCore<8647>(index);
    case 1037: return radicalInverseFastCore<8663>(index);
    case 1038: return radicalInverseFastCore<8669>(index);
    case 1039: return radicalInverseFastCore<8677>(index);
    case 1040: return radicalInverseFastCore<8681>(index);
    case 1041: return radicalInverseFastCore<8689>(index);
    case 1042: return radicalInverseFastCore<8693>(index);
    case 1043: return radicalInverseFastCore<8699>(index);
    case 1044: return radicalInverseFastCore<8707>(index);
    case 1045: return radicalInverseFastCore<8713>(index);
    case 1046: return radicalInverseFastCore<8719>(index);
    case 1047: return radicalInverseFastCore<8731>(index);
    case 1048: return radicalInverseFastCore<8737>(index);
    case 1049: return radicalInverseFastCore<8741>(index);
    case 1050: return radicalInverseFastCore<8747>(index);
    case 1051: return radicalInverseFastCore<8753>(index);
    case 1052: return radicalInverseFastCore<8761>(index);
    case 1053: return radicalInverseFastCore<8779>(index);
    case 1054: return radicalInverseFastCore<8783>(index);
    case 1055: return radicalInverseFastCore<8803>(index);
    case 1056: return radicalInverseFastCore<8807>(index);
    case 1057: return radicalInverseFastCore<8819>(index);
    case 1058: return radicalInverseFastCore<8821>(index);
    case 1059: return radicalInverseFastCore<8831>(index);
    case 1060: return radicalInverseFastCore<8837>(index);
    case 1061: return radicalInverseFastCore<8839>(index);
    case 1062: return radicalInverseFastCore<8849>(index);
    case 1063: return radicalInverseFastCore<8861>(index);
    case 1064: return radicalInverseFastCore<8863>(index);
    case 1065: return radicalInverseFastCore<8867>(index);
    case 1066: return radicalInverseFastCore<8887>(index);
    case 1067: return radicalInverseFastCore<8893>(index);
    case 1068: return radicalInverseFastCore<8923>(index);
    case 1069: return radicalInverseFastCore<8929>(index);
    case 1070: return radicalInverseFastCore<8933>(index);
    case 1071: return radicalInverseFastCore<8941>(index);
    case 1072: return radicalInverseFastCore<8951>(index);
    case 1073: return radicalInverseFastCore<8963>(index);
    case 1074: return radicalInverseFastCore<8969>(index);
    case 1075: return radicalInverseFastCore<8971>(index);
    case 1076: return radicalInverseFastCore<8999>(index);
    case 1077: return radicalInverseFastCore<9001>(index);
    case 1078: return radicalInverseFastCore<9007>(index);
    case 1079: return radicalInverseFastCore<9011>(index);
    case 1080: return radicalInverseFastCore<9013>(index);
    case 1081: return radicalInverseFastCore<9029>(index);
    case 1082: return radicalInverseFastCore<9041>(index);
    case 1083: return radicalInverseFastCore<9043>(index);
    case 1084: return radicalInverseFastCore<9049>(index);
    case 1085: return radicalInverseFastCore<9059>(index);
    case 1086: return radicalInverseFastCore<9067>(index);
    case 1087: return radicalInverseFastCore<9091>(index);
    case 1088: return radicalInverseFastCore<9103>(index);
    case 1089: return radicalInverseFastCore<9109>(index);
    case 1090: return radicalInverseFastCore<9127>(index);
    case 1091: return radicalInverseFastCore<9133>(index);
    case 1092: return radicalInverseFastCore<9137>(index);
    case 1093: return radicalInverseFastCore<9151>(index);
    case 1094: return radicalInverseFastCore<9157>(index);
    case 1095: return radicalInverseFastCore<9161>(index);
    case 1096: return radicalInverseFastCore<9173>(index);
    case 1097: return radicalInverseFastCore<9181>(index);
    case 1098: return radicalInverseFastCore<9187>(index);
    case 1099: return radicalInverseFastCore<9199>(index);
    case 1100: return radicalInverseFastCore<9203>(index);
    case 1101: return radicalInverseFastCore<9209>(index);
    case 1102: return radicalInverseFastCore<9221>(index);
    case 1103: return radicalInverseFastCore<9227>(index);
    case 1104: return radicalInverseFastCore<9239>(index);
    case 1105: return radicalInverseFastCore<9241>(index);
    case 1106: return radicalInverseFastCore<9257>(index);
    case 1107: return radicalInverseFastCore<9277>(index);
    case 1108: return radicalInverseFastCore<9281>(index);
    case 1109: return radicalInverseFastCore<9283>(index);
    case 1110: return radicalInverseFastCore<9293>(index);
    case 1111: return radicalInverseFastCore<9311>(index);
    case 1112: return radicalInverseFastCore<9319>(index);
    case 1113: return radicalInverseFastCore<9323>(index);
    case 1114: return radicalInverseFastCore<9337>(index);
    case 1115: return radicalInverseFastCore<9341>(index);
    case 1116: return radicalInverseFastCore<9343>(index);
    case 1117: return radicalInverseFastCore<9349>(index);
    case 1118: return radicalInverseFastCore<9371>(index);
    case 1119: return radicalInverseFastCore<9377>(index);
    case 1120: return radicalInverseFastCore<9391>(index);
    case 1121: return radicalInverseFastCore<9397>(index);
    case 1122: return radicalInverseFastCore<9403>(index);
    case 1123: return radicalInverseFastCore<9413>(index);
    case 1124: return radicalInverseFastCore<9419>(index);
    case 1125: return radicalInverseFastCore<9421>(index);
    case 1126: return radicalInverseFastCore<9431>(index);
    case 1127: return radicalInverseFastCore<9433>(index);
    case 1128: return radicalInverseFastCore<9437>(index);
    case 1129: return radicalInverseFastCore<9439>(index);
    case 1130: return radicalInverseFastCore<9461>(index);
    case 1131: return radicalInverseFastCore<9463>(index);
    case 1132: return radicalInverseFastCore<9467>(index);
    case 1133: return radicalInverseFastCore<9473>(index);
    case 1134: return radicalInverseFastCore<9479>(index);
    case 1135: return radicalInverseFastCore<9491>(index);
    case 1136: return radicalInverseFastCore<9497>(index);
    case 1137: return radicalInverseFastCore<9511>(index);
    case 1138: return radicalInverseFastCore<9521>(index);
    case 1139: return radicalInverseFastCore<9533>(index);
    case 1140: return radicalInverseFastCore<9539>(index);
    case 1141: return radicalInverseFastCore<9547>(index);
    case 1142: return radicalInverseFastCore<9551>(index);
    case 1143: return radicalInverseFastCore<9587>(index);
    case 1144: return radicalInverseFastCore<9601>(index);
    case 1145: return radicalInverseFastCore<9613>(index);
    case 1146: return radicalInverseFastCore<9619>(index);
    case 1147: return radicalInverseFastCore<9623>(index);
    case 1148: return radicalInverseFastCore<9629>(index);
    case 1149: return radicalInverseFastCore<9631>(index);
    case 1150: return radicalInverseFastCore<9643>(index);
    case 1151: return radicalInverseFastCore<9649>(index);
    case 1152: return radicalInverseFastCore<9661>(index);
    case 1153: return radicalInverseFastCore<9677>(index);
    case 1154: return radicalInverseFastCore<9679>(index);
    case 1155: return radicalInverseFastCore<9689>(index);
    case 1156: return radicalInverseFastCore<9697>(index);
    case 1157: return radicalInverseFastCore<9719>(index);
    case 1158: return radicalInverseFastCore<9721>(index);
    case 1159: return radicalInverseFastCore<9733>(index);
    case 1160: return radicalInverseFastCore<9739>(index);
    case 1161: return radicalInverseFastCore<9743>(index);
    case 1162: return radicalInverseFastCore<9749>(index);
    case 1163: return radicalInverseFastCore<9767>(index);
    case 1164: return radicalInverseFastCore<9769>(index);
    case 1165: return radicalInverseFastCore<9781>(index);
    case 1166: return radicalInverseFastCore<9787>(index);
    case 1167: return radicalInverseFastCore<9791>(index);
    case 1168: return radicalInverseFastCore<9803>(index);
    case 1169: return radicalInverseFastCore<9811>(index);
    case 1170: return radicalInverseFastCore<9817>(index);
    case 1171: return radicalInverseFastCore<9829>(index);
    case 1172: return radicalInverseFastCore<9833>(index);
    case 1173: return radicalInverseFastCore<9839>(index);
    case 1174: return radicalInverseFastCore<9851>(index);
    case 1175: return radicalInverseFastCore<9857>(index);
    case 1176: return radicalInverseFastCore<9859>(index);
    case 1177: return radicalInverseFastCore<9871>(index);
    case 1178: return radicalInverseFastCore<9883>(index);
    case 1179: return radicalInverseFastCore<9887>(index);
    case 1180: return radicalInverseFastCore<9901>(index);
    case 1181: return radicalInverseFastCore<9907>(index);
    case 1182: return radicalInverseFastCore<9923>(index);
    case 1183: return radicalInverseFastCore<9929>(index);
    case 1184: return radicalInverseFastCore<9931>(index);
    case 1185: return radicalInverseFastCore<9941>(index);
    case 1186: return radicalInverseFastCore<9949>(index);
    case 1187: return radicalInverseFastCore<9967>(index);
    case 1188: return radicalInverseFastCore<9973>(index);
    default:
        AL_ASSERT_DEBUG(false);
        return 0.0f;
        break;
    }
}

/*
-------------------------------------------------
基数2のradicalInverse()
-------------------------------------------------
*/
INLINE float radicalInverseBase2(uint32_t n, uint32_t scramble)
{
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    //
    n = (n >> (32 - 24)) ^ (scramble & ~- (1 << 24));

    return (float)n / (float)(1U << 24);
}

/*
-------------------------------------------------
sobol2
基数2のsobol
-------------------------------------------------
*/
INLINE float sobol2(uint32_t n, uint32_t scramble)
{
    for (uint32_t v = 1U << 31; n != 0; n >>= 1, v ^= v >> 1)
    {
        if (n & 1)
        {
            scramble ^= v;
        }
    }
    return (float)scramble / (float)(1ULL << 32);
}

/*
-------------------------------------------------
次の2乗に移動
-------------------------------------------------
*/
INLINE uint32_t roundupNextSquareRoot(uint32_t v)
{
#define NEXT_RET(C) if(v<=(C*C)){ return C;}
    NEXT_RET(1);
    NEXT_RET(2);
    NEXT_RET(3);
    NEXT_RET(4);
    NEXT_RET(5);
    NEXT_RET(6);
    NEXT_RET(7);
    NEXT_RET(8);
    NEXT_RET(9);
    NEXT_RET(10);
    NEXT_RET(11);
    NEXT_RET(12);
    NEXT_RET(13);
    NEXT_RET(14);
    NEXT_RET(15);
    NEXT_RET(16);
    NEXT_RET(17);
    NEXT_RET(18);
    NEXT_RET(19);
    NEXT_RET(20);
    NEXT_RET(21);
    NEXT_RET(22);
    NEXT_RET(23);
    NEXT_RET(24);
    NEXT_RET(25);
    NEXT_RET(26);
    NEXT_RET(27);
    NEXT_RET(28);
    NEXT_RET(29);
    NEXT_RET(30);
    NEXT_RET(31);
    NEXT_RET(32);
    NEXT_RET(33);
    NEXT_RET(34);
    NEXT_RET(35);
    NEXT_RET(36);
    NEXT_RET(37);
    NEXT_RET(38);
    NEXT_RET(39);
    NEXT_RET(40);
    NEXT_RET(41);
    NEXT_RET(42);
    NEXT_RET(43);
    NEXT_RET(44);
    NEXT_RET(45);
    NEXT_RET(46);
    NEXT_RET(47);
    NEXT_RET(48);
    NEXT_RET(49);
    NEXT_RET(50);
    return 51 * 51;
}
