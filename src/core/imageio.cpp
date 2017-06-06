#include "pch.hpp"
#include "core/math.hpp"
#include "core/image.hpp"
#include "core/logging.hpp"
#include "core/util.hpp"

//-------------------------------------------------
// readBmp()
//-------------------------------------------------
void Image::readBmp(const std::string& fileName, float gamma)
{
    // 拡張子チェック
    if (strcmp(getExt(fileName.c_str()), ".bmp") != 0)
    {
        loggingError("Faild readBmp(). %s is not bmp.", fileName.c_str());
        return;
    }

    //
    logging("Read image [%s].", fileName.c_str());
    // 指定されたファイルを開く
    FILE* file = NULL;
    fopen_s(&file, fileName.c_str(), "rb");
    loggingErrorIf(!file, "File not found.[%s]", fileName.c_str());
    fseek(file, 0, SEEK_END);
    uint32_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    std::vector<uint8_t> data;
    data.resize(fileSize + 1);
    fread(&data[0], fileSize, 1, file);
    fclose(file);
    //
    BITMAPFILEHEADER* header = (BITMAPFILEHEADER*)data.data();
    BITMAPINFOHEADER* infoHeader = (BITMAPINFOHEADER*)(data.data() + sizeof(BITMAPFILEHEADER));
    //
    const int32_t w = infoHeader->biWidth;
    const int32_t h = infoHeader->biHeight;
    const int32_t base = header->bfOffBits;

    // HACK: なぜか2の累乗でない場合にロード結果がおかしくなるのでここで警告
    if (!alIsPowerOfTwo(w) || !alIsPowerOfTwo(h))
    {
        loggingWarning("BMP [%s] size is not power of 2.", fileName.c_str() );
    }

    // 対応しているフォーマットかチェック
    AL_ASSERT_DEBUG(header->bfType == ('B' | ('M' << 8)));
    AL_ASSERT_DEBUG(infoHeader->biPlanes == 1); // 1plane?
    AL_ASSERT_DEBUG(infoHeader->biBitCount == 24);// 24bpp?
    AL_ASSERT_DEBUG(infoHeader->biCompression == 0); // 0(無圧縮)?
    AL_ASSERT_DEBUG(infoHeader->biClrUsed == 0); // パレット数。0?
    AL_ASSERT_DEBUG(infoHeader->biClrImportant == 0); // 重要なパレットのインデックス
    //AL_ASSERT_DEBUG(infoHeader->biSizeImage == w * h * 3);

    // 4byteアライメント
    const int32_t lineSizeRaw = w * 3;
    const int32_t lineSize = (lineSizeRaw + 3) & ~3;

    //
    resize(w, h);
    //
    for (int32_t y = 0; y < h; ++y)
    {
        for (int32_t x = 0; x < w; ++x)
        {
            auto& sp = pixel(x, y);
            const int32_t index = (x * 3) + y * lineSize;
            const uint8_t r = data[base + index + 0];
            const uint8_t g = data[base + index + 1];
            const uint8_t b = data[base + index + 2];
            float b2 = alClamp((float)r / 255.0f, 0.0f, 1.0f);
            float g2 = alClamp((float)g / 255.0f, 0.0f, 1.0f);
            float r2 = alClamp((float)b / 255.0f, 0.0f, 1.0f);

            // HACK: cheap convert
            b2 = powf(b2, 1.0f / gamma);
            g2 = powf(g2, 1.0f / gamma);
            r2 = powf(r2, 1.0f / gamma);

            // BMPなので反射率としてロード
            sp = Spectrum::createFromRGB({{r2, g2, b2}}, false );
        }
    }
    return;
}

//-------------------------------------------------
// writeBmp()
//-------------------------------------------------
void ImageLDR::writeBmp(const std::string& fileName) const
{
    //
    logging("Write image [%s].", fileName.c_str() );
    //
    const auto& pixels = pixels_;
    const int32_t width = this->width();
    const int32_t height = this->height();
    //
    int success = 0;
    FILE* file = 0;
    BITMAPFILEHEADER* header = 0;
    BITMAPINFOHEADER* infoHeader = 0;
    unsigned int scanLineLengthInBytes = 0;
    int totalFileLength = 0;
    unsigned char* bmpMemoryStart = 0;
    unsigned char* bmpMemoryCursor = 0;
    // スキャンライン長計算
    scanLineLengthInBytes = width * 3;
    // ファイル全体の長さを計算
    totalFileLength = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + scanLineLengthInBytes*height;
    // メモリを確保
    bmpMemoryStart = (unsigned char*)malloc(totalFileLength);
    if (!bmpMemoryStart)
    {
        goto EXIT;
    }
    bmpMemoryCursor = bmpMemoryStart;
    // BITMAPFILEHEADERを作成
    header = (BITMAPFILEHEADER*)bmpMemoryCursor; //
    header->bfType = 'B' | ('M' << 8); // ファイルタイプ
    header->bfSize = totalFileLength;// ファイルサイズ (byte)
    header->bfReserved1 = 0; // 予約領域
    header->bfReserved2 = 0; // 予約領域
    header->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // ファイル先頭から画像データまでのオフセット (byte)
    bmpMemoryCursor += sizeof(BITMAPFILEHEADER);
    // BITMAPINFOHEADERを作成
    infoHeader = (BITMAPINFOHEADER*)bmpMemoryCursor;
    infoHeader->biSize = sizeof(BITMAPINFOHEADER); // 情報ヘッダのサイズ
    infoHeader->biWidth = width; // 画像の幅 (ピクセル)
    infoHeader->biHeight = height; // 画像の高さ (ピクセル)
    infoHeader->biPlanes = 1; // プレーン数
    infoHeader->biBitCount = 24; // 1 画素あたりのデータサイズ (bit)
    infoHeader->biCompression = 0; // 圧縮形式(無圧縮)
    infoHeader->biSizeImage = width*height * 3; // 画像データ部のサイズ (byte)
    infoHeader->biClrUsed = 0; // 格納されているパレット数 (使用色数)
    //infoHeader->biClrImporant = 0; // 重要なパレットのインデックス
    bmpMemoryCursor += sizeof(BITMAPINFOHEADER);
    // 全てのデータを書き込む
    for (int32_t y = height - 1; y >= 0; --y)
    {
        for (int32_t x = 0; x<width; ++x)
        {
            const uint32_t srcBase = (x + (height - y - 1)*width);
            const uint32_t dstBase = x * 3;
            const PixelLDR& rgb = pixels[srcBase];
            bmpMemoryCursor[dstBase + 0] = rgb.b;
            bmpMemoryCursor[dstBase + 1] = rgb.g;
            bmpMemoryCursor[dstBase + 2] = rgb.r;
        }
        bmpMemoryCursor += scanLineLengthInBytes;
    }
    // ファイルに書き込む
    fopen_s(&file, fileName.c_str(), "wb");
    if (!file)
    {
        goto EXIT;
    }
    if (fwrite(bmpMemoryStart, totalFileLength, 1, file) != 1)
    {
        goto EXIT;
    }
    if (fclose(file) == EOF)
    {
        goto EXIT;
    }
    // 最後まで到達したので成功
    success = 1;
EXIT:
    if (bmpMemoryStart)
    {
        free(bmpMemoryStart);
    }
    if (file)
    {
        fclose(file);
    }
    return;
}

//-------------------------------------------------
// readHdr()
//-------------------------------------------------
void Image::readHdr(const std::string& fileName)
{
    // ファイルを開く
    std::ifstream file(fileName, std::ios_base::binary);
    loggingErrorIf(file.fail(), "File open failed.[%s]", fileName.c_str());
    // 読み取り速度を上げるためにバッファを大きめにして設定する
    std::vector<char> ifstreamBuffer;
    ifstreamBuffer.resize(1024 * 1024 * 16); // 16MB
    file.rdbuf()->pubsetbuf(ifstreamBuffer.data(), ifstreamBuffer.size());
    // ヘッダの終わりまで読み続ける
    bool isRadiance = false;
    bool isRleRGBE = false;
    std::string line;
    while (std::getline(file, line) && line != "")
    {
        // ラディアンスのフォーマットか
        if (line == "#?RADIANCE")
        {
            isRadiance = true;
        }
        else if (line == "FORMAT=32-bit_rle_rgbe")
        {
            isRleRGBE = true;
        }
    }
    loggingErrorIf((!isRadiance || !isRleRGBE), "File format is not supported.[%s]", fileName.c_str());
    // "-Y 4096 +X 8192"
    int32_t width = 0;
    int32_t height = 0;
    std::getline(file, line);
    int32_t numScan = sscanf_s(line.c_str(), "-Y %d +X %d", &height, &width);
    loggingErrorIf(numScan != 2, "File format is not supported.[%s]", fileName.c_str());
    resize( width, height );
    // 本体の読み込み
    std::vector<uint8_t> imageLineBuffer;
    for (int32_t y = 0; y < height; ++y)
    {
        imageLineBuffer.clear();
        // 行始まり
        uint8_t ch;
        ch = file.get();
        AL_ASSERT_DEBUG(ch == 0x02);
        ch = file.get();
        AL_ASSERT_DEBUG(ch == 0x02);
        // 行幅の取得
        int32_t imageWidthTmp = 0;
        imageWidthTmp = file.get();
        imageWidthTmp <<= 8;
        imageWidthTmp |= file.get();
        AL_ASSERT_DEBUG(imageWidthTmp == width);
        //
        do
        {
            //
            ch = static_cast<uint8_t>( file.get() );
            // 無圧縮部
            if (ch <= 128)
            {
                const uint8_t numNoCompressNum = ch;
                for (int32_t i = 0; i < numNoCompressNum; ++i)
                {
                    ch = static_cast<uint8_t>( file.get() );
                    imageLineBuffer.push_back(ch);
                }
            }
            // ランレングス部
            else
            {
                const uint8_t numCompress = ch - 128;
                ch = static_cast<uint8_t>( file.get() );
                for (int32_t i = 0; i < numCompress; ++i)
                {
                    imageLineBuffer.push_back(ch);
                }
            }
        }
        while (imageLineBuffer.size() < width * 4);
        AL_ASSERT_DEBUG(imageLineBuffer.size() >= width * 4);
        // RGBEからRGBを復元する
        for (int32_t x = 0; x < width; ++x)
        {
            auto& p = pixel(x, y);
            uint8_t r = imageLineBuffer[x + 0];
            uint8_t g = imageLineBuffer[x + width * 1];
            uint8_t b = imageLineBuffer[x + width * 2];
            uint8_t e = imageLineBuffer[x + width * 3];
            const float scale = powf(2,(float)e - 128.0f) / 256.0f;
            // HDRファイルなので、光源としてロードする
            p = Spectrum::createFromRGB({ { float(r), float(g), float(b) } }, true);
            p *= scale;
        }
    }
}

//-------------------------------------------------
// writeHdr()
// HACK: 何かHDR感がない出力になる。修正する
//-------------------------------------------------
void Image::writeHdr(const std::string& fileName) const
{
    //
    const int32_t width = this->width();
    const int32_t height = this->height();
    std::vector<uint8_t> lineR(width);
    std::vector<uint8_t> lineG(width);
    std::vector<uint8_t> lineB(width);
    std::vector<uint8_t> lineE(width);
    // ファイルを開く
    std::ofstream file(fileName, std::ios_base::binary);
    loggingErrorIf(file.fail(), "File open failed.[%s]", fileName.c_str());
    // ヘッダを書き込む
    file << "#?RADIANCE" << std::endl;
    file << "FORMAT=32-bit_rle_rgbe" << std::endl;
    file << "EXPOSURE = 1.0" << std::endl;
    file << std::endl;
    file << "-Y " << height << " +X " << width << std::endl;

    // 全てのデータを書き込む
    for (int32_t y = 0; y < height; ++y)
    {
        // 行はじめを書く
        uint8_t b = 0;
        b = 0x02;
        file.write((const char*)&b, sizeof(b));
        file.write((const char*)&b, sizeof(b));
        // 行幅を書く
        b = ((width & 0xFF00) >> 8);
        file.write((char*)&b, sizeof(b));
        b = (width & 0xFF);
        file.write((char*)&b, sizeof(b));
        //
        lineR.clear();
        lineG.clear();
        lineB.clear();
        lineE.clear();
        lineR.resize(width);
        lineG.resize(width);
        lineB.resize(width);
        lineE.resize(width);
        // RGBのデータに分解する
        for (int32_t x = 0; x < width; ++x)
        {
            auto p = Spectrum(pixel(x, y));
            SpectrumRGB rgb;
            p.toRGB(rgb);
            // RGBEの算出
            const float maxValue = std::max({ rgb.r, rgb.g, rgb.b });
            const uint8_t e = uint8_t(128.0f + std::ceil(std::log2f(maxValue)));
            const float invE = 255.0f / powf(2.0f, float(e - 128));
            const uint8_t r = uint8_t(rgb.r * invE);
            const uint8_t g = uint8_t(rgb.g * invE);
            const uint8_t b = uint8_t(rgb.b * invE);
            // RGBEを格納していく
            lineR[x] = r;
            lineG[x] = g;
            lineB[x] = b;
            lineE[x] = e;
        }
        // 行データを書く
        const std::array<std::vector<uint8_t>, 4> lines =
        {
            lineR,
            lineG,
            lineB,
            lineE
        };
        AL_ASSERT_DEBUG( !(width % 64) );
        for (auto line : lines )
        {
            for (int32_t x = 0; x < width;)
            {
                // HACK: 全てのデータを無圧縮として書き込む
                b = 64;
                file.write((char*)&b, sizeof(b));
                for (int32_t i = 0; i < 64; ++i)
                {
                    b = line[x + i];
                    file.write((char*)&b, sizeof(b));
                }
                x += 64;
            }
        }
    }
}
