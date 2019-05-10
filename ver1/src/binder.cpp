#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

//
class DummyRenderer
{
public:
    DummyRenderer(int32_t width, int32_t height)
    {
        printf("AlhazenRender Build %s %s\n", __DATE__, __TIME__);
        //
        width_ = width;
        height_ = height;
        buffer_.resize(width_*height_);
        for (int32_t y = 0; y < height_; ++y)
        {
            for (int32_t x = 0; x < width_; ++x)
            {
                auto& p = buffer_[x + y * width_];
                p[0] = float(x)/float(width_);
                p[1] = float(y) / float(height_);
                p[2] = 0.0f;
                p[3] = 1.0f;
            }
        }
    }
    int32_t width()
    {
        return width_;
    }
    int32_t height()
    {
        return height_;
    }
    const std::vector<std::array<float,4>>& getBuffer() const
    {
        return buffer_;
    }
private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    std::vector<std::array<float, 4>> buffer_;
};

//
namespace py = pybind11;
PYBIND11_MODULE(AlhazenPy, m)
{
    m.doc() = "pybind11 example plugin";
    //
    py::class_<DummyRenderer>(m, "Render")
        .def(py::init<int32_t,int32_t>())
        .def("width", &DummyRenderer::width)
        .def("height", &DummyRenderer::height)
        .def("getBuffer", &DummyRenderer::getBuffer)
        ;
}
