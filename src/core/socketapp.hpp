#pragma once

#include "core/socket.hpp"
#include "core/math.hpp"
#include "core/image.hpp"

#if defined(WINDOWS)
/*
-------------------------------------------------
アプリケーション固有のパケット情報付き
-------------------------------------------------
*/
class SocketApp
    :public Socket
{
public:
    void sendResize(int32_t w, int32_t h, int32_t tileSize);
    void sendTile(const Region2D& region, const ImageLDR& image);
};

#endif
