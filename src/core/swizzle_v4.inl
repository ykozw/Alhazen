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
 printf("   return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(%d, %d, %d, %d));\n", i3, i2, i1, i0);
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
*/

INLINE Vec4 xxxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 0, 0));
}

INLINE Vec4 yxxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 0, 1));
}

INLINE Vec4 zxxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 0, 2));
}

INLINE Vec4 wxxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 0, 3));
}

INLINE Vec4 xyxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 1, 0));
}

INLINE Vec4 yyxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 1, 1));
}

INLINE Vec4 zyxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 1, 2));
}

INLINE Vec4 wyxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 1, 3));
}

INLINE Vec4 xzxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 2, 0));
}

INLINE Vec4 yzxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 2, 1));
}

INLINE Vec4 zzxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 2, 2));
}

INLINE Vec4 wzxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 2, 3));
}

INLINE Vec4 xwxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 3, 0));
}

INLINE Vec4 ywxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 3, 1));
}

INLINE Vec4 zwxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 3, 2));
}

INLINE Vec4 wwxx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 0, 3, 3));
}

INLINE Vec4 xxyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 0, 0));
}

INLINE Vec4 yxyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 0, 1));
}

INLINE Vec4 zxyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 0, 2));
}

INLINE Vec4 wxyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 0, 3));
}

INLINE Vec4 xyyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 1, 0));
}

INLINE Vec4 yyyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 1, 1));
}

INLINE Vec4 zyyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 1, 2));
}

INLINE Vec4 wyyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 1, 3));
}

INLINE Vec4 xzyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 2, 0));
}

INLINE Vec4 yzyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 2, 1));
}

INLINE Vec4 zzyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 2, 2));
}

INLINE Vec4 wzyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 2, 3));
}

INLINE Vec4 xwyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 3, 0));
}

INLINE Vec4 ywyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 3, 1));
}

INLINE Vec4 zwyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 3, 2));
}

INLINE Vec4 wwyx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 1, 3, 3));
}

INLINE Vec4 xxzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 0, 0));
}

INLINE Vec4 yxzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 0, 1));
}

INLINE Vec4 zxzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 0, 2));
}

INLINE Vec4 wxzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 0, 3));
}

INLINE Vec4 xyzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 1, 0));
}

INLINE Vec4 yyzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 1, 1));
}

INLINE Vec4 zyzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 1, 2));
}

INLINE Vec4 wyzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 1, 3));
}

INLINE Vec4 xzzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 2, 0));
}

INLINE Vec4 yzzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 2, 1));
}

INLINE Vec4 zzzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 2, 2));
}

INLINE Vec4 wzzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 2, 3));
}

INLINE Vec4 xwzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 3, 0));
}

INLINE Vec4 ywzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 3, 1));
}

INLINE Vec4 zwzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 3, 2));
}

INLINE Vec4 wwzx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 2, 3, 3));
}

INLINE Vec4 xxwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 0, 0));
}

INLINE Vec4 yxwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 0, 1));
}

INLINE Vec4 zxwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 0, 2));
}

INLINE Vec4 wxwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 0, 3));
}

INLINE Vec4 xywx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 1, 0));
}

INLINE Vec4 yywx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 1, 1));
}

INLINE Vec4 zywx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 1, 2));
}

INLINE Vec4 wywx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 1, 3));
}

INLINE Vec4 xzwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 2, 0));
}

INLINE Vec4 yzwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 2, 1));
}

INLINE Vec4 zzwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 2, 2));
}

INLINE Vec4 wzwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 2, 3));
}

INLINE Vec4 xwwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 3, 0));
}

INLINE Vec4 ywwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 3, 1));
}

INLINE Vec4 zwwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 3, 2));
}

INLINE Vec4 wwwx() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0, 3, 3, 3));
}

INLINE Vec4 xxxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 0, 0));
}

INLINE Vec4 yxxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 0, 1));
}

INLINE Vec4 zxxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 0, 2));
}

INLINE Vec4 wxxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 0, 3));
}

INLINE Vec4 xyxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 1, 0));
}

INLINE Vec4 yyxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 1, 1));
}

INLINE Vec4 zyxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 1, 2));
}

INLINE Vec4 wyxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 1, 3));
}

INLINE Vec4 xzxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 2, 0));
}

INLINE Vec4 yzxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 2, 1));
}

INLINE Vec4 zzxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 2, 2));
}

INLINE Vec4 wzxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 2, 3));
}

INLINE Vec4 xwxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 3, 0));
}

INLINE Vec4 ywxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 3, 1));
}

INLINE Vec4 zwxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 3, 2));
}

INLINE Vec4 wwxy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 0, 3, 3));
}

INLINE Vec4 xxyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 0, 0));
}

INLINE Vec4 yxyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 0, 1));
}

INLINE Vec4 zxyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 0, 2));
}

INLINE Vec4 wxyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 0, 3));
}

INLINE Vec4 xyyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 1, 0));
}

INLINE Vec4 yyyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 1, 1));
}

INLINE Vec4 zyyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 1, 2));
}

INLINE Vec4 wyyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 1, 3));
}

INLINE Vec4 xzyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 2, 0));
}

INLINE Vec4 yzyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 2, 1));
}

INLINE Vec4 zzyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 2, 2));
}

INLINE Vec4 wzyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 2, 3));
}

INLINE Vec4 xwyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 3, 0));
}

INLINE Vec4 ywyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 3, 1));
}

INLINE Vec4 zwyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 3, 2));
}

INLINE Vec4 wwyy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 1, 3, 3));
}

INLINE Vec4 xxzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 0, 0));
}

INLINE Vec4 yxzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 0, 1));
}

INLINE Vec4 zxzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 0, 2));
}

INLINE Vec4 wxzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 0, 3));
}

INLINE Vec4 xyzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 1, 0));
}

INLINE Vec4 yyzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 1, 1));
}

INLINE Vec4 zyzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 1, 2));
}

INLINE Vec4 wyzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 1, 3));
}

INLINE Vec4 xzzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 2, 0));
}

INLINE Vec4 yzzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 2, 1));
}

INLINE Vec4 zzzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 2, 2));
}

INLINE Vec4 wzzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 2, 3));
}

INLINE Vec4 xwzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 3, 0));
}

INLINE Vec4 ywzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 3, 1));
}

INLINE Vec4 zwzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 3, 2));
}

INLINE Vec4 wwzy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 2, 3, 3));
}

INLINE Vec4 xxwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 0, 0));
}

INLINE Vec4 yxwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 0, 1));
}

INLINE Vec4 zxwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 0, 2));
}

INLINE Vec4 wxwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 0, 3));
}

INLINE Vec4 xywy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 1, 0));
}

INLINE Vec4 yywy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 1, 1));
}

INLINE Vec4 zywy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 1, 2));
}

INLINE Vec4 wywy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 1, 3));
}

INLINE Vec4 xzwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 2, 0));
}

INLINE Vec4 yzwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 2, 1));
}

INLINE Vec4 zzwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 2, 2));
}

INLINE Vec4 wzwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 2, 3));
}

INLINE Vec4 xwwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 3, 0));
}

INLINE Vec4 ywwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 3, 1));
}

INLINE Vec4 zwwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 3, 2));
}

INLINE Vec4 wwwy() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(1, 3, 3, 3));
}

INLINE Vec4 xxxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 0, 0));
}

INLINE Vec4 yxxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 0, 1));
}

INLINE Vec4 zxxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 0, 2));
}

INLINE Vec4 wxxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 0, 3));
}

INLINE Vec4 xyxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 1, 0));
}

INLINE Vec4 yyxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 1, 1));
}

INLINE Vec4 zyxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 1, 2));
}

INLINE Vec4 wyxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 1, 3));
}

INLINE Vec4 xzxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 2, 0));
}

INLINE Vec4 yzxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 2, 1));
}

INLINE Vec4 zzxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 2, 2));
}

INLINE Vec4 wzxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 2, 3));
}

INLINE Vec4 xwxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 3, 0));
}

INLINE Vec4 ywxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 3, 1));
}

INLINE Vec4 zwxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 3, 2));
}

INLINE Vec4 wwxz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 0, 3, 3));
}

INLINE Vec4 xxyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 0, 0));
}

INLINE Vec4 yxyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 0, 1));
}

INLINE Vec4 zxyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 0, 2));
}

INLINE Vec4 wxyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 0, 3));
}

INLINE Vec4 xyyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 1, 0));
}

INLINE Vec4 yyyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 1, 1));
}

INLINE Vec4 zyyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 1, 2));
}

INLINE Vec4 wyyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 1, 3));
}

INLINE Vec4 xzyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 2, 0));
}

INLINE Vec4 yzyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 2, 1));
}

INLINE Vec4 zzyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 2, 2));
}

INLINE Vec4 wzyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 2, 3));
}

INLINE Vec4 xwyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 3, 0));
}

INLINE Vec4 ywyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 3, 1));
}

INLINE Vec4 zwyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 3, 2));
}

INLINE Vec4 wwyz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 1, 3, 3));
}

INLINE Vec4 xxzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 0, 0));
}

INLINE Vec4 yxzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 0, 1));
}

INLINE Vec4 zxzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 0, 2));
}

INLINE Vec4 wxzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 0, 3));
}

INLINE Vec4 xyzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 1, 0));
}

INLINE Vec4 yyzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 1, 1));
}

INLINE Vec4 zyzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 1, 2));
}

INLINE Vec4 wyzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 1, 3));
}

INLINE Vec4 xzzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 2, 0));
}

INLINE Vec4 yzzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 2, 1));
}

INLINE Vec4 zzzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 2, 2));
}

INLINE Vec4 wzzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 2, 3));
}

INLINE Vec4 xwzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 3, 0));
}

INLINE Vec4 ywzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 3, 1));
}

INLINE Vec4 zwzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 3, 2));
}

INLINE Vec4 wwzz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 2, 3, 3));
}

INLINE Vec4 xxwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 0, 0));
}

INLINE Vec4 yxwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 0, 1));
}

INLINE Vec4 zxwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 0, 2));
}

INLINE Vec4 wxwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 0, 3));
}

INLINE Vec4 xywz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 1, 0));
}

INLINE Vec4 yywz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 1, 1));
}

INLINE Vec4 zywz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 1, 2));
}

INLINE Vec4 wywz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 1, 3));
}

INLINE Vec4 xzwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 2, 0));
}

INLINE Vec4 yzwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 2, 1));
}

INLINE Vec4 zzwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 2, 2));
}

INLINE Vec4 wzwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 2, 3));
}

INLINE Vec4 xwwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 3, 0));
}

INLINE Vec4 ywwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 3, 1));
}

INLINE Vec4 zwwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 3, 2));
}

INLINE Vec4 wwwz() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(2, 3, 3, 3));
}

INLINE Vec4 xxxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 0, 0));
}

INLINE Vec4 yxxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 0, 1));
}

INLINE Vec4 zxxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 0, 2));
}

INLINE Vec4 wxxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 0, 3));
}

INLINE Vec4 xyxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 1, 0));
}

INLINE Vec4 yyxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 1, 1));
}

INLINE Vec4 zyxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 1, 2));
}

INLINE Vec4 wyxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 1, 3));
}

INLINE Vec4 xzxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 2, 0));
}

INLINE Vec4 yzxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 2, 1));
}

INLINE Vec4 zzxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 2, 2));
}

INLINE Vec4 wzxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 2, 3));
}

INLINE Vec4 xwxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 3, 0));
}

INLINE Vec4 ywxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 3, 1));
}

INLINE Vec4 zwxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 3, 2));
}

INLINE Vec4 wwxw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 0, 3, 3));
}

INLINE Vec4 xxyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 0, 0));
}

INLINE Vec4 yxyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 0, 1));
}

INLINE Vec4 zxyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 0, 2));
}

INLINE Vec4 wxyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 0, 3));
}

INLINE Vec4 xyyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 1, 0));
}

INLINE Vec4 yyyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 1, 1));
}

INLINE Vec4 zyyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 1, 2));
}

INLINE Vec4 wyyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 1, 3));
}

INLINE Vec4 xzyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 2, 0));
}

INLINE Vec4 yzyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 2, 1));
}

INLINE Vec4 zzyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 2, 2));
}

INLINE Vec4 wzyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 2, 3));
}

INLINE Vec4 xwyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 3, 0));
}

INLINE Vec4 ywyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 3, 1));
}

INLINE Vec4 zwyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 3, 2));
}

INLINE Vec4 wwyw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 1, 3, 3));
}

INLINE Vec4 xxzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 0, 0));
}

INLINE Vec4 yxzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 0, 1));
}

INLINE Vec4 zxzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 0, 2));
}

INLINE Vec4 wxzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 0, 3));
}

INLINE Vec4 yyzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 1, 1));
}

INLINE Vec4 zyzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 1, 2));
}

INLINE Vec4 wyzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 1, 3));
}

INLINE Vec4 xzzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 2, 0));
}

INLINE Vec4 yzzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 2, 1));
}

INLINE Vec4 zzzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 2, 2));
}

INLINE Vec4 wzzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 2, 3));
}

INLINE Vec4 xwzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 3, 0));
}

INLINE Vec4 ywzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 3, 1));
}

INLINE Vec4 zwzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 3, 2));
}

INLINE Vec4 wwzw() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 2, 3, 3));
}

INLINE Vec4 xxww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 0, 0));
}

INLINE Vec4 yxww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 0, 1));
}

INLINE Vec4 zxww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 0, 2));
}

INLINE Vec4 wxww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 0, 3));
}

INLINE Vec4 xyww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 1, 0));
}

INLINE Vec4 yyww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 1, 1));
}

INLINE Vec4 zyww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 1, 2));
}

INLINE Vec4 wyww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 1, 3));
}

INLINE Vec4 xzww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 2, 0));
}

INLINE Vec4 yzww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 2, 1));
}

INLINE Vec4 zzww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 2, 2));
}

INLINE Vec4 wzww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 2, 3));
}

INLINE Vec4 xwww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 3, 0));
}

INLINE Vec4 ywww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 3, 1));
}

INLINE Vec4 zwww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 3, 2));
}

INLINE Vec4 wwww() const
{
    return _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(3, 3, 3, 3));
}
