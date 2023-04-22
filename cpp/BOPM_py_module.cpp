#include "BOPM.h"
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(BOPM)
{
    class_<Option>("Option", init<std::string>())
        .def(init<>())
    ;

    class_<European>("European", init<std::string>())
        .def(init<>())
        
        .def("printProperties", &European::printProperties)
        .def("", &European::getExerciseValue)
    ;

}
