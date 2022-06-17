/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(generic_antenna.h)                                         */
/* BINDTOOL_HEADER_FILE_HASH(0)                                                    */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/leo/generic_antenna.h>

void bind_generic_antenna(py::module& m)
{

    using generic_antenna = ::gr::leo::generic_antenna;


    py::class_<generic_antenna, std::shared_ptr<generic_antenna>>(m, "generic_antenna")

        .def("unique_id", &generic_antenna::unique_id)

        .def("get_frequency",
             &generic_antenna::get_frequency)

        .def("set_pointing_error",
             &generic_antenna::set_pointing_error)

        .def("get_pointing_error",
             &generic_antenna::get_pointing_error)

        .def("get_polarization",
             &generic_antenna::get_polarization)

        .def("get_wavelength",
             &generic_antenna::get_wavelength)

        .def("get_gain",
             &generic_antenna::get_gain)

        .def("get_beamwidth",
             &generic_antenna::get_beamwidth)

        .def("get_gain_rolloff",
             &generic_antenna::get_gain_rolloff)

        .def("get_gain_rolloff",
             &generic_antenna::get_gain_rolloff)

        ;

}
