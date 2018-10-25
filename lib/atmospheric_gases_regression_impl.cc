/* -*- c++ -*- */
/*
 * gr-leo: SatNOGS GNU Radio Out-Of-Tree Module
 *
 *  Copyright (C) 2018, Libre Space Foundation <https://libre.space/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atmospheric_gases_regression_impl.h"
#include <cmath>
#include <leo/log.h>

namespace gr
{
  namespace leo
  {
    namespace attenuation
    {

      generic_attenuation::generic_attenuation_sptr
      atmospheric_gases_regression::make (float frequency, float surface_watervap_density,
                                          float temperature)
      {
        return generic_attenuation::generic_attenuation_sptr (
            new atmospheric_gases_regression_impl (frequency, surface_watervap_density,
                                                   temperature));
      }

      atmospheric_gases_regression_impl::atmospheric_gases_regression_impl (
          float frequency, float watervap, float temperature) :
              generic_attenuation (),
              d_frequency (frequency),
              d_surface_watervap_density (watervap),
              d_temperature (temperature)

      {
        atmospheric_gases_regression_impl::atmo_coefficients_t tmp_coeff;
        tmp_coeff = get_atmo_coeff (frequency, &d_atmo_gases_coeff);

        d_af = std::get<1> (tmp_coeff);
        d_bf = std::get<2> (tmp_coeff);
        d_cf = std::get<3> (tmp_coeff);

        tmp_coeff = get_atmo_coeff (frequency, &d_atmo_gases_coeff_zenith);

        d_azf = std::get<1> (tmp_coeff);
        d_bzf = std::get<2> (tmp_coeff);
        d_czf = std::get<3> (tmp_coeff);

      }

      atmospheric_gases_regression_impl::~atmospheric_gases_regression_impl ()
      {
      }

      float
      atmospheric_gases_regression_impl::m (float y1, float y2, float f1,
                                            float f2)
      {
        return std::log10 (y1 / y2) / std::log10 (f1 / f2);
      }

      float
      atmospheric_gases_regression_impl::calc_coeff (float y1, float y2,
                                                     float f1, float f2,
                                                     float f0)
      {
        float _m = m (y1, y2, f1, f2);
        return std::pow (
            10, _m * std::log10 (f0) + (std::log10 (y2) - _m * std::log10 (f2)));
      }

      atmospheric_gases_regression_impl::atmo_coefficients_t
      atmospheric_gases_regression_impl::get_atmo_coeff (
          float frequency, std::vector<atmo_coefficients_t>* coeff_table)
      {
        float a_f, b_f, c_f = 0;

        if (frequency < std::get<0> ((*coeff_table)[0])) {
          a_f = std::get<1> ((*coeff_table)[0]);
          b_f = std::get<2> ((*coeff_table)[0]);
          c_f = std::get<3> ((*coeff_table)[0]);
        }
        else if (frequency
            > std::get<0> ((*coeff_table)[coeff_table->size () - 1])) {
          a_f = std::get<1> ((*coeff_table)[0]);
          b_f = std::get<2> ((*coeff_table)[0]);
          c_f = std::get<3> ((*coeff_table)[0]);
        }

        for (size_t i = 0; i < coeff_table->size (); i++) {
          if (std::get<0> ((*coeff_table)[i]) == frequency) {
            a_f = std::get<1> ((*coeff_table)[i]);
            b_f = std::get<2> ((*coeff_table)[i]);
            c_f = std::get<3> ((*coeff_table)[i]);
            break;
          }
          else if (std::get<0> ((*coeff_table)[i]) > frequency) {
            a_f = calc_coeff (std::get<1> ((*coeff_table)[i - 1]),
                              std::get<1> ((*coeff_table)[i]),
                              std::get<0> ((*coeff_table)[i - 1]),
                              std::get<0> ((*coeff_table)[i]), frequency);
            b_f = calc_coeff (std::get<2> ((*coeff_table)[i - 1]),
                              std::get<2> ((*coeff_table)[i]),
                              std::get<0> ((*coeff_table)[i - 1]),
                              std::get<0> ((*coeff_table)[i]), frequency);
            c_f = calc_coeff (std::get<3> ((*coeff_table)[i - 1]),
                              std::get<3> ((*coeff_table)[i]),
                              std::get<0> ((*coeff_table)[i - 1]),
                              std::get<0> ((*coeff_table)[i]), frequency);
            break;
          }
        }

        return atmo_coefficients_t (frequency, a_f, b_f, c_f);
      }

      float
      atmospheric_gases_regression_impl::get_attenuation (float elevation)
      {
        float elevation_rad = elevation * 3.141592 / 180;
        float gammaa = d_af + d_bf * d_surface_watervap_density - d_cf * d_temperature;
        float zenitha = d_azf + d_bzf * d_surface_watervap_density - d_czf * d_temperature;

        float ha = zenitha / gammaa;

        if (elevation >= 10) {
          return (ha * zenitha) / std::sin (elevation_rad);
        }
        else {
          return (2 * ha * zenitha)
              / (std::sqrt (
                  std::pow (std::sin (elevation_rad), 2)
                      + ((2 * ha) / EARTH_RADIUS)) + std::sin (elevation_rad));
        }
      }

    } /* namespace attenuation */
  } /* namespace leo */
} /* namespace gr */

