#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

//
int add(int i, int j)
{
    return i + j;
}

//
struct Pet
{
    Pet(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }
    std::vector<int32_t> getInts() const
    {
        return { 0,1,2 };
    }

    std::string name;
};

//
namespace py = pybind11;
PYBIND11_MODULE(example, m)
{
    m.doc() = "pybind11 example plugin";
    m.def("add2", &add, "A function which adds two numbers");

    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())
        .def("setName", &Pet::setName)
        .def("setName", &Pet::setName)
        .def("getINts", &Pet::getInts)
        ;

}
