    /*
 void genSwizzle()
{
    const auto writeCode2 = [](int i0, int i1)
    {
        //
        const auto xyzstr = [](int i)
        {
            switch (i)
            {
            case 0:return "x";
            case 1:return "y";
            case 2:return "z";
            }
            return "*";
        };
        printf("INLINE Vec2 %s%s() const\n", xyzstr(i0), xyzstr(i1));
        printf("{\n");
        printf("#if defined(AL_MATH_USE_NO_SIMD)\n");
        // 普通のコード
        printf("   return Vec2(%s_,%s_));\n", xyzstr(i0), xyzstr(i1));
        printf("#else\n");
        // SIMD コード
        printf("   return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(%d, %d, %d, %d));\n", 0, 0, i1, i0);
        printf("#endif\n");
        printf("}\n");
        printf("\n");
    };
    for (int i = 0; i<9; ++i)
    {
        const int i0 = (i / 3) % 3;
        const int i1 = (i / 1) % 3;
        writeCode2(i0, i1);
    }

    const auto writeCode = [](int i0, int i1, int i2)
    {
        //
        const auto xyzstr = [](int i)
        {
            switch (i)
            {
            case 0:return "x";
            case 1:return "y";
            case 2:return "z";
            }
            return "*";
        };
        printf("INLINE Vec3 %s%s%s() const\n", xyzstr(i0), xyzstr(i1), xyzstr(i2));
        printf("{\n");
        printf("#if defined(AL_MATH_USE_NO_SIMD)\n");
        // 普通のコード
        printf("   return Vec3(%s_,%s_,%s_));\n", xyzstr(i0), xyzstr(i1), xyzstr(i2));
        printf("#else\n");
        // SIMD コード
        printf("   return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(%d, %d, %d, %d));\n", 0, i2, i1, i0);
        printf("#endif\n");
        printf("}\n");
        printf("\n");
    };
    for (int i = 0; i<27; ++i)
    {
        const int i0 = (i / 9) % 3;
        const int i1 = (i / 3) % 3;
        const int i2 = (i / 1) % 3;
        writeCode(i0, i1, i2);
    }
    exit(0);
}
 */
INLINE Vec2 xx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(x_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 0));
#endif
}

INLINE Vec2 xy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(x_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 0));
#endif
}

INLINE Vec2 xz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(x_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 0));
#endif
}

INLINE Vec2 yx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(y_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 1));
#endif
}

INLINE Vec2 yy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(y_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 1));
#endif
}

INLINE Vec2 yz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(y_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 1));
#endif
}

INLINE Vec2 zx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(z_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 2));
#endif
}

INLINE Vec2 zy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(z_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 2));
#endif
}

INLINE Vec2 zz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec2(z_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 2));
#endif
}

INLINE Vec3 xxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, x_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 0));
#endif
}

INLINE Vec3 xxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, x_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 0, 0));
#endif
}

INLINE Vec3 xxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, x_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 0, 0));
#endif
}

INLINE Vec3 xyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, y_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 0));
#endif
}

INLINE Vec3 xyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, y_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 1, 0));
#endif
}

INLINE Vec3 xyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, y_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 1, 0));
#endif
}

INLINE Vec3 xzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, z_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 0));
#endif
}

INLINE Vec3 xzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, z_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 2, 0));
#endif
}

INLINE Vec3 xzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x_, z_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 2, 0));
#endif
}

INLINE Vec3 yxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, x_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 1));
#endif
}

INLINE Vec3 yxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, x_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 0, 1));
#endif
}

INLINE Vec3 yxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, x_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 0, 1));
#endif
}

INLINE Vec3 yyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, y_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 1));
#endif
}

INLINE Vec3 yyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, y_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 1, 1));
#endif
}

INLINE Vec3 yyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, y_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 1, 1));
#endif
}

INLINE Vec3 yzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, z_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 1));
#endif
}

INLINE Vec3 yzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, z_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 2, 1));
#endif
}

INLINE Vec3 yzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y_, z_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 2, 1));
#endif
}

INLINE Vec3 zxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, x_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 2));
#endif
}

INLINE Vec3 zxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, x_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 0, 2));
#endif
}

INLINE Vec3 zxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, x_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 0, 2));
#endif
}

INLINE Vec3 zyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, y_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 2));
#endif
}

INLINE Vec3 zyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, y_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 1, 2));
#endif
}

INLINE Vec3 zyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, y_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 1, 2));
#endif
}

INLINE Vec3 zzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, z_, x_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 2));
#endif
}

INLINE Vec3 zzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, z_, y_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 2, 2));
#endif
}

INLINE Vec3 zzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z_, z_, z_));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 2, 2));
#endif
}

