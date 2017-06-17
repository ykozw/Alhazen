/*
 void genSwizzle()
 {
 const auto writeCode = [](int i0,int i1, int i2)
 {
 //
 const auto xyzstr = [] (int i)
 {
 switch(i)
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
 printf("   return Vec3(%s,%s,%s));\n", xyzstr(i0), xyzstr(i1), xyzstr(i2));
 printf("#else\n");
 // SIMD コード
 printf("   return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(%d, %d, %d, %d));\n", 0, i2, i1, i0);
 printf("#endif\n");
 printf("}\n");
 printf("\n");
 };
 for(int i=0;i<27;++i)
 {
 const int i0 = (i/1) % 3;
 const int i1 = (i/3) % 3;
 const int i2 = (i/9) % 3;
 writeCode(i0,i1,i2);
 }
 exit(0);
 }
 */

INLINE Vec3 xxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,x,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 0));
#endif
}

INLINE Vec3 yxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,x,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 1));
#endif
}

INLINE Vec3 zxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,x,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 2));
#endif
}

INLINE Vec3 xyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,y,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 0));
#endif
}

INLINE Vec3 yyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,y,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 1));
#endif
}

INLINE Vec3 zyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,y,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 1, 2));
#endif
}

INLINE Vec3 xzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,z,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 0));
#endif
}

INLINE Vec3 yzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,z,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 1));
#endif
}

INLINE Vec3 zzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,z,x));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 2, 2));
#endif
}

INLINE Vec3 xxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,x,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 0, 0));
#endif
}

INLINE Vec3 yxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,x,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 0, 1));
#endif
}

INLINE Vec3 zxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,x,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 0, 2));
#endif
}

INLINE Vec3 xyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,y,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 1, 0));
#endif
}

INLINE Vec3 yyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,y,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 1, 1));
#endif
}

INLINE Vec3 zyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,y,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 1, 2));
#endif
}

INLINE Vec3 xzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,z,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 2, 0));
#endif
}

INLINE Vec3 yzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,z,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 2, 1));
#endif
}

INLINE Vec3 zzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,z,y));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 1, 2, 2));
#endif
}

INLINE Vec3 xxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,x,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 0, 0));
#endif
}

INLINE Vec3 yxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,x,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 0, 1));
#endif
}

INLINE Vec3 zxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,x,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 0, 2));
#endif
}

INLINE Vec3 xyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,y,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 1, 0));
#endif
}

INLINE Vec3 yyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,y,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 1, 1));
#endif
}

INLINE Vec3 zyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,y,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 1, 2));
#endif
}

INLINE Vec3 xzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(x,z,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 2, 0));
#endif
}

INLINE Vec3 yzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(y,z,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 2, 1));
#endif
}

INLINE Vec3 zzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(z,z,z));
#else
    return _mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 2, 2, 2));
#endif
}
