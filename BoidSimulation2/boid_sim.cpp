#include <pybind11/pybind11.h>
#include "boid.h"

namespace py = pybind11;

PYBIND11_MODULE(boid_sim, m) {
    py::class_<Vector2>(m, "Vector2")
        .def(py::init<float, float>())
        .def_readwrite("x", &Vector2::x)
        .def_readwrite("y", &Vector2::y);

    py::class_<Boid>(m, "Boid")
        .def(py::init<float, float>())
        .def("updatePosition", &Boid::updatePosition)
        .def("getPosition", &Boid::getPosition);
}
