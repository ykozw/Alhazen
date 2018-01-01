#include "pch.hpp"
#include "app/alhazen.hpp"
#include "bsdf/bsdf.hpp"
#include "core/rng.hpp"
#include "core/util.hpp"
#include "core/unittest.hpp"

/*
 */

//
#include "EmbeddableWebServer.h"r


struct Response* createResponseForRequest(const struct Request* request,
                                          struct Connection* connection)
{
    // HomePage
    if (0 == strcmp(request->pathDecoded, "/"))
    {
        Response* response = responseAllocWithFormat(200, "OK", "text/html; charset=UTF-8",
                                                     "<html><title>ALhazen Viewer</title>"
                                                     "<body><h1>Alhazen Viewer</h1></body></html>");
        // テーブルを作成する
        const int32_t imageWidth = 640;
        const int32_t imageHeight = 640;
        const int32_t tileSize = 16;
        const int32_t numCol = imageWidth/tileSize;
        const int32_t numRow = imageHeight/tileSize;
        heapStringAppendFormat(&response->body, "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
        for(int32_t col=0;col<numCol;++col)
        {
            heapStringAppendFormat(&response->body, "<tr>\n");
            for(int32_t row=0;row<numRow;++row)
            {
                heapStringAppendFormat(&response->body, "<td><img src=\"3.gif\"></td>\n");
            }
            heapStringAppendFormat(&response->body, "</tr>\n");
        }
        heapStringAppendFormat(&response->body, "</table>\n");
        return response;
    }
    // ファイルアクセス
    else
    {
        return responseAllocServeFileFromRequestPath("/", request->path, request->pathDecoded, ".");
    }
}

//
class ViewerServer
{
public:
    //
    ViewerServer()
    {
    }
    
    std::thread serverThread;
    void init()
    {
        // スレッドを立てる
        serverThread = std::thread([this]() {
#if 0
            const uint16_t port = 80808;
            serverInit(&server);
            acceptConnectionsUntilStoppedFromEverywhereIPv4(&server, port);
            serverDeInit(&server);
#else
            acceptConnectionsUntilStoppedFromEverywhereIPv4(nullptr, 8080);
#endif
        });
    }
    
    void wait()
    {
        serverThread.join();
    }
    
    //
    void setImage(int32_t width,
                  int32_t height,
                  std::vector<float*> tilePtrs,
                  int32_t tileSize)
    {
        assert(width % tileSize == 0);
        assert(height % tileSize == 0);
        const int32_t numTile = (width*height)/(tileSize*tileSize);
        assert(tilePtrs.size() == numTile);
        //
        width_ = width;
        height_ = height;
        tilePtrs_ = tilePtrs;
        tileSize_ = tileSize;
    }
    
private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    std::vector<float*> tilePtrs_;
    int32_t tileSize_ = 0;
};

/*
-------------------------------------------------
main
-------------------------------------------------
*/
int32_t main(int32_t argc, char* argv[])
{
#if 0
    ViewerServer vs;
    vs.init();
    vs.wait();
    return 0;
#else
    Alhazen app;
    return app.run(argc, argv);
#endif
}
