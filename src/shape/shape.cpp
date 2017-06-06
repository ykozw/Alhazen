#include "pch.hpp"
#include "shape/shape.hpp"

//-------------------------------------------------
// Shape()
//-------------------------------------------------
Shape::Shape( const ObjectProp& prop)
{
    // マテリアル名を取得(単体のマテリアルのみの場合)
    if (!hasMultiBSDFs())
    {
        bsdfName_ = prop.findChildBy("name", "bsdf").asString("none");
    }
}

//-------------------------------------------------
// Shape()
//-------------------------------------------------
void Shape::setBSDF(AllBSDFList bsdfs)
{
    // 単体のマテリアルのみの場合はこの場で設定する
    if (!hasMultiBSDFs())
    {
        bsdf_ = bsdfs.find(bsdfName_);
    }
    // setBSDFs()は複数のBSDFを明示的に設定する必要がある場合のみ
    else
    {
        setBSDFs(bsdfs);
    }
}

//-------------------------------------------------
// Shape()
//-------------------------------------------------
void Shape::setBSDFs(AllBSDFList bsdfs)
{
    /*
    setBSDFs()は複数のマテリアルがあるクラスのみで、
    そのようなクラスはsetBSDFs()をオーバーロードする必要がある
    */
    AL_ASSERT_DEBUG(false);
}
