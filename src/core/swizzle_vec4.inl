/*
 void genSwizzle()
 {
 const auto writeCode = [](int i0,int i1, int i2, int i3)
 {
 if(i0 == 0 && i1 == 1 && i2 == 2 && i3 == 3)
 {
 return;
 }
 //
 const auto xyzstr = [] (int i)
 {
 switch(i)
 {
 case 0:return "x";
 case 1:return "y";
 case 2:return "z";
 case 3:return "w";
 }
 return "";
 };
 printf("INLINE Vec4 %s%s%s%s() const\n", xyzstr(i0), xyzstr(i1), xyzstr(i2), xyzstr(i3));
 printf("{\n");
 printf("#if defined(AL_MATH_USE_NO_SIMD)\n");
 printf("   return Vec4(%s_,%s_,%s_,%s_);\n", xyzstr(i0), xyzstr(i1), xyzstr(i2), xyzstr(i3));
 printf("#else\n");
 printf("   return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(%d, %d, %d, %d));\n", i3, i2, i1, i0);
 printf("#endif\n");
 printf("}\n");
 printf("\n");
 };
 for(int i=0;i<256;++i)
 {
 const int i0 = (i/1) % 4;
 const int i1 = (i/4) % 4;
 const int i2 = (i/16) % 4;
 const int i3 = (i/64) % 4;
 writeCode(i0,i1,i2,i3);
 }
 exit(0);
 }
 }
*/

INLINE Vec4 xxxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 0, 0));
#endif
}

INLINE Vec4 yxxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 0, 1));
#endif
}

INLINE Vec4 zxxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 0, 2));
#endif
}

INLINE Vec4 wxxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 0, 3));
#endif
}

INLINE Vec4 xyxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 1, 0));
#endif
}

INLINE Vec4 yyxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 1, 1));
#endif
}

INLINE Vec4 zyxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 1, 2));
#endif
}

INLINE Vec4 wyxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 1, 3));
#endif
}

INLINE Vec4 xzxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 2, 0));
#endif
}

INLINE Vec4 yzxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 2, 1));
#endif
}

INLINE Vec4 zzxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 2, 2));
#endif
}

INLINE Vec4 wzxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 2, 3));
#endif
}

INLINE Vec4 xwxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 3, 0));
#endif
}

INLINE Vec4 ywxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 3, 1));
#endif
}

INLINE Vec4 zwxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 3, 2));
#endif
}

INLINE Vec4 wwxx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, x_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 3, 3));
#endif
}

INLINE Vec4 xxyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 0, 0));
#endif
}

INLINE Vec4 yxyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 0, 1));
#endif
}

INLINE Vec4 zxyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 0, 2));
#endif
}

INLINE Vec4 wxyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 0, 3));
#endif
}

INLINE Vec4 xyyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 1, 0));
#endif
}

INLINE Vec4 yyyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 1, 1));
#endif
}

INLINE Vec4 zyyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 1, 2));
#endif
}

INLINE Vec4 wyyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 1, 3));
#endif
}

INLINE Vec4 xzyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 2, 0));
#endif
}

INLINE Vec4 yzyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 2, 1));
#endif
}

INLINE Vec4 zzyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 2, 2));
#endif
}

INLINE Vec4 wzyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 2, 3));
#endif
}

INLINE Vec4 xwyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 3, 0));
#endif
}

INLINE Vec4 ywyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 3, 1));
#endif
}

INLINE Vec4 zwyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 3, 2));
#endif
}

INLINE Vec4 wwyx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, y_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 1, 3, 3));
#endif
}

INLINE Vec4 xxzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 0, 0));
#endif
}

INLINE Vec4 yxzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 0, 1));
#endif
}

INLINE Vec4 zxzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 0, 2));
#endif
}

INLINE Vec4 wxzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 0, 3));
#endif
}

INLINE Vec4 xyzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 1, 0));
#endif
}

INLINE Vec4 yyzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 1, 1));
#endif
}

INLINE Vec4 zyzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 1, 2));
#endif
}

INLINE Vec4 wyzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 1, 3));
#endif
}

INLINE Vec4 xzzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 2, 0));
#endif
}

INLINE Vec4 yzzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 2, 1));
#endif
}

INLINE Vec4 zzzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 2, 2));
#endif
}

INLINE Vec4 wzzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 2, 3));
#endif
}

INLINE Vec4 xwzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 3, 0));
#endif
}

INLINE Vec4 ywzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 3, 1));
#endif
}

INLINE Vec4 zwzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 3, 2));
#endif
}

INLINE Vec4 wwzx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, z_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 2, 3, 3));
#endif
}

INLINE Vec4 xxwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 0, 0));
#endif
}

INLINE Vec4 yxwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 0, 1));
#endif
}

INLINE Vec4 zxwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 0, 2));
#endif
}

INLINE Vec4 wxwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 0, 3));
#endif
}

INLINE Vec4 xywx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 1, 0));
#endif
}

INLINE Vec4 yywx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 1, 1));
#endif
}

INLINE Vec4 zywx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 1, 2));
#endif
}

INLINE Vec4 wywx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 1, 3));
#endif
}

INLINE Vec4 xzwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 2, 0));
#endif
}

INLINE Vec4 yzwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 2, 1));
#endif
}

INLINE Vec4 zzwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 2, 2));
#endif
}

INLINE Vec4 wzwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 2, 3));
#endif
}

INLINE Vec4 xwwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 3, 0));
#endif
}

INLINE Vec4 ywwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 3, 1));
#endif
}

INLINE Vec4 zwwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 3, 2));
#endif
}

INLINE Vec4 wwwx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, w_, x_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 3, 3, 3));
#endif
}

INLINE Vec4 xxxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 0, 0));
#endif
}

INLINE Vec4 yxxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 0, 1));
#endif
}

INLINE Vec4 zxxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 0, 2));
#endif
}

INLINE Vec4 wxxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 0, 3));
#endif
}

INLINE Vec4 xyxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 1, 0));
#endif
}

INLINE Vec4 yyxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 1, 1));
#endif
}

INLINE Vec4 zyxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 1, 2));
#endif
}

INLINE Vec4 wyxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 1, 3));
#endif
}

INLINE Vec4 xzxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 2, 0));
#endif
}

INLINE Vec4 yzxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 2, 1));
#endif
}

INLINE Vec4 zzxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 2, 2));
#endif
}

INLINE Vec4 wzxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 2, 3));
#endif
}

INLINE Vec4 xwxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 3, 0));
#endif
}

INLINE Vec4 ywxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 3, 1));
#endif
}

INLINE Vec4 zwxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 3, 2));
#endif
}

INLINE Vec4 wwxy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, x_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 0, 3, 3));
#endif
}

INLINE Vec4 xxyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 0, 0));
#endif
}

INLINE Vec4 yxyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 0, 1));
#endif
}

INLINE Vec4 zxyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 0, 2));
#endif
}

INLINE Vec4 wxyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 0, 3));
#endif
}

INLINE Vec4 xyyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 1, 0));
#endif
}

INLINE Vec4 yyyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 1, 1));
#endif
}

INLINE Vec4 zyyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 1, 2));
#endif
}

INLINE Vec4 wyyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 1, 3));
#endif
}

INLINE Vec4 xzyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 2, 0));
#endif
}

INLINE Vec4 yzyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 2, 1));
#endif
}

INLINE Vec4 zzyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 2, 2));
#endif
}

INLINE Vec4 wzyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 2, 3));
#endif
}

INLINE Vec4 xwyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 3, 0));
#endif
}

INLINE Vec4 ywyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 3, 1));
#endif
}

INLINE Vec4 zwyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 3, 2));
#endif
}

INLINE Vec4 wwyy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, y_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 3, 3));
#endif
}

INLINE Vec4 xxzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 0, 0));
#endif
}

INLINE Vec4 yxzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 0, 1));
#endif
}

INLINE Vec4 zxzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 0, 2));
#endif
}

INLINE Vec4 wxzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 0, 3));
#endif
}

INLINE Vec4 xyzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 1, 0));
#endif
}

INLINE Vec4 yyzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 1, 1));
#endif
}

INLINE Vec4 zyzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 1, 2));
#endif
}

INLINE Vec4 wyzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 1, 3));
#endif
}

INLINE Vec4 xzzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 2, 0));
#endif
}

INLINE Vec4 yzzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 2, 1));
#endif
}

INLINE Vec4 zzzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 2, 2));
#endif
}

INLINE Vec4 wzzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 2, 3));
#endif
}

INLINE Vec4 xwzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 3, 0));
#endif
}

INLINE Vec4 ywzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 3, 1));
#endif
}

INLINE Vec4 zwzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 3, 2));
#endif
}

INLINE Vec4 wwzy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, z_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 2, 3, 3));
#endif
}

INLINE Vec4 xxwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 0, 0));
#endif
}

INLINE Vec4 yxwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 0, 1));
#endif
}

INLINE Vec4 zxwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 0, 2));
#endif
}

INLINE Vec4 wxwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 0, 3));
#endif
}

INLINE Vec4 xywy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 1, 0));
#endif
}

INLINE Vec4 yywy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 1, 1));
#endif
}

INLINE Vec4 zywy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 1, 2));
#endif
}

INLINE Vec4 wywy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 1, 3));
#endif
}

INLINE Vec4 xzwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 2, 0));
#endif
}

INLINE Vec4 yzwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 2, 1));
#endif
}

INLINE Vec4 zzwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 2, 2));
#endif
}

INLINE Vec4 wzwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 2, 3));
#endif
}

INLINE Vec4 xwwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 3, 0));
#endif
}

INLINE Vec4 ywwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 3, 1));
#endif
}

INLINE Vec4 zwwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 3, 2));
#endif
}

INLINE Vec4 wwwy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, w_, y_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 3, 3, 3));
#endif
}

INLINE Vec4 xxxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 0, 0));
#endif
}

INLINE Vec4 yxxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 0, 1));
#endif
}

INLINE Vec4 zxxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 0, 2));
#endif
}

INLINE Vec4 wxxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 0, 3));
#endif
}

INLINE Vec4 xyxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 1, 0));
#endif
}

INLINE Vec4 yyxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 1, 1));
#endif
}

INLINE Vec4 zyxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 1, 2));
#endif
}

INLINE Vec4 wyxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 1, 3));
#endif
}

INLINE Vec4 xzxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 2, 0));
#endif
}

INLINE Vec4 yzxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 2, 1));
#endif
}

INLINE Vec4 zzxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 2, 2));
#endif
}

INLINE Vec4 wzxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 2, 3));
#endif
}

INLINE Vec4 xwxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 3, 0));
#endif
}

INLINE Vec4 ywxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 3, 1));
#endif
}

INLINE Vec4 zwxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 3, 2));
#endif
}

INLINE Vec4 wwxz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, x_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 0, 3, 3));
#endif
}

INLINE Vec4 xxyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 0, 0));
#endif
}

INLINE Vec4 yxyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 0, 1));
#endif
}

INLINE Vec4 zxyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 0, 2));
#endif
}

INLINE Vec4 wxyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 0, 3));
#endif
}

INLINE Vec4 xyyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 1, 0));
#endif
}

INLINE Vec4 yyyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 1, 1));
#endif
}

INLINE Vec4 zyyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 1, 2));
#endif
}

INLINE Vec4 wyyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 1, 3));
#endif
}

INLINE Vec4 xzyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 2, 0));
#endif
}

INLINE Vec4 yzyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 2, 1));
#endif
}

INLINE Vec4 zzyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 2, 2));
#endif
}

INLINE Vec4 wzyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 2, 3));
#endif
}

INLINE Vec4 xwyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 3, 0));
#endif
}

INLINE Vec4 ywyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 3, 1));
#endif
}

INLINE Vec4 zwyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 3, 2));
#endif
}

INLINE Vec4 wwyz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, y_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 1, 3, 3));
#endif
}

INLINE Vec4 xxzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 0, 0));
#endif
}

INLINE Vec4 yxzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 0, 1));
#endif
}

INLINE Vec4 zxzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 0, 2));
#endif
}

INLINE Vec4 wxzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 0, 3));
#endif
}

INLINE Vec4 xyzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 1, 0));
#endif
}

INLINE Vec4 yyzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 1, 1));
#endif
}

INLINE Vec4 zyzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 1, 2));
#endif
}

INLINE Vec4 wyzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 1, 3));
#endif
}

INLINE Vec4 xzzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 2, 0));
#endif
}

INLINE Vec4 yzzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 2, 1));
#endif
}

INLINE Vec4 zzzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 2, 2));
#endif
}

INLINE Vec4 wzzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 2, 3));
#endif
}

INLINE Vec4 xwzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 3, 0));
#endif
}

INLINE Vec4 ywzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 3, 1));
#endif
}

INLINE Vec4 zwzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 3, 2));
#endif
}

INLINE Vec4 wwzz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, z_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 3, 3));
#endif
}

INLINE Vec4 xxwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 0, 0));
#endif
}

INLINE Vec4 yxwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 0, 1));
#endif
}

INLINE Vec4 zxwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 0, 2));
#endif
}

INLINE Vec4 wxwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 0, 3));
#endif
}

INLINE Vec4 xywz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 1, 0));
#endif
}

INLINE Vec4 yywz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 1, 1));
#endif
}

INLINE Vec4 zywz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 1, 2));
#endif
}

INLINE Vec4 wywz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 1, 3));
#endif
}

INLINE Vec4 xzwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 2, 0));
#endif
}

INLINE Vec4 yzwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 2, 1));
#endif
}

INLINE Vec4 zzwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 2, 2));
#endif
}

INLINE Vec4 wzwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 2, 3));
#endif
}

INLINE Vec4 xwwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 3, 0));
#endif
}

INLINE Vec4 ywwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 3, 1));
#endif
}

INLINE Vec4 zwwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 3, 2));
#endif
}

INLINE Vec4 wwwz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, w_, z_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 3, 3, 3));
#endif
}

INLINE Vec4 xxxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 0, 0));
#endif
}

INLINE Vec4 yxxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 0, 1));
#endif
}

INLINE Vec4 zxxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 0, 2));
#endif
}

INLINE Vec4 wxxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 0, 3));
#endif
}

INLINE Vec4 xyxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 1, 0));
#endif
}

INLINE Vec4 yyxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 1, 1));
#endif
}

INLINE Vec4 zyxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 1, 2));
#endif
}

INLINE Vec4 wyxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 1, 3));
#endif
}

INLINE Vec4 xzxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 2, 0));
#endif
}

INLINE Vec4 yzxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 2, 1));
#endif
}

INLINE Vec4 zzxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 2, 2));
#endif
}

INLINE Vec4 wzxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 2, 3));
#endif
}

INLINE Vec4 xwxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 3, 0));
#endif
}

INLINE Vec4 ywxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 3, 1));
#endif
}

INLINE Vec4 zwxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 3, 2));
#endif
}

INLINE Vec4 wwxw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, x_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 0, 3, 3));
#endif
}

INLINE Vec4 xxyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 0, 0));
#endif
}

INLINE Vec4 yxyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 0, 1));
#endif
}

INLINE Vec4 zxyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 0, 2));
#endif
}

INLINE Vec4 wxyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 0, 3));
#endif
}

INLINE Vec4 xyyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 1, 0));
#endif
}

INLINE Vec4 yyyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 1, 1));
#endif
}

INLINE Vec4 zyyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 1, 2));
#endif
}

INLINE Vec4 wyyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 1, 3));
#endif
}

INLINE Vec4 xzyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 2, 0));
#endif
}

INLINE Vec4 yzyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 2, 1));
#endif
}

INLINE Vec4 zzyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 2, 2));
#endif
}

INLINE Vec4 wzyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 2, 3));
#endif
}

INLINE Vec4 xwyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 3, 0));
#endif
}

INLINE Vec4 ywyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 3, 1));
#endif
}

INLINE Vec4 zwyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 3, 2));
#endif
}

INLINE Vec4 wwyw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, y_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 1, 3, 3));
#endif
}

INLINE Vec4 xxzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 0, 0));
#endif
}

INLINE Vec4 yxzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 0, 1));
#endif
}

INLINE Vec4 zxzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 0, 2));
#endif
}

INLINE Vec4 wxzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 0, 3));
#endif
}

INLINE Vec4 yyzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 1, 1));
#endif
}

INLINE Vec4 zyzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 1, 2));
#endif
}

INLINE Vec4 wyzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 1, 3));
#endif
}

INLINE Vec4 xzzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 2, 0));
#endif
}

INLINE Vec4 yzzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 2, 1));
#endif
}

INLINE Vec4 zzzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 2, 2));
#endif
}

INLINE Vec4 wzzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 2, 3));
#endif
}

INLINE Vec4 xwzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 3, 0));
#endif
}

INLINE Vec4 ywzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 3, 1));
#endif
}

INLINE Vec4 zwzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 3, 2));
#endif
}

INLINE Vec4 wwzw() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, z_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 2, 3, 3));
#endif
}

INLINE Vec4 xxww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, x_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 0, 0));
#endif
}

INLINE Vec4 yxww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, x_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 0, 1));
#endif
}

INLINE Vec4 zxww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, x_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 0, 2));
#endif
}

INLINE Vec4 wxww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, x_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 0, 3));
#endif
}

INLINE Vec4 xyww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, y_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 1, 0));
#endif
}

INLINE Vec4 yyww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, y_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 1, 1));
#endif
}

INLINE Vec4 zyww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, y_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 1, 2));
#endif
}

INLINE Vec4 wyww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, y_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 1, 3));
#endif
}

INLINE Vec4 xzww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, z_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 2, 0));
#endif
}

INLINE Vec4 yzww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, z_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 2, 1));
#endif
}

INLINE Vec4 zzww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, z_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 2, 2));
#endif
}

INLINE Vec4 wzww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, z_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 2, 3));
#endif
}

INLINE Vec4 xwww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(x_, w_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 3, 0));
#endif
}

INLINE Vec4 ywww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(y_, w_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 3, 1));
#endif
}

INLINE Vec4 zwww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(z_, w_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 3, 2));
#endif
}

INLINE Vec4 wwww() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec4(w_, w_, w_, w_);
#else
    return _mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 3, 3));
#endif
}
