#include "pch.hpp"
#include "core/socketapp.hpp"
#include "core/math.hpp"
#include "core/image.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
void SocketApp::sendResize(int32_t w, int32_t h, int32_t tileSize)
{
    SocketData data;
    data.pushUint8(0x00); // タグ
    data.pushUint32(w);
    data.pushUint32(h);
    data.pushUint32(tileSize);
    send(data);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SocketApp::sendTile(const Region2D& region, const ImageLDR& image)
{
    //
    SocketData data;
    data.pushUint8(0x01); // タグ
    data.pushUint32(region.left);
    data.pushUint32(region.top);
    data.pushUint32(region.width());
    data.pushUint32(region.height());
    const auto& ps = image.pixels();
    for (auto& p : ps)
    {
        data.pushUint8(p.b);
        data.pushUint8(p.g);
        data.pushUint8(p.r);
    }
    //
    send(data);
}
