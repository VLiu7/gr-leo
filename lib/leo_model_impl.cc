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

#include "leo_model_impl.h"
#include <cstring>
#include <leo/log.h>
#include <volk/volk.h>

namespace gr
{
  namespace leo
  {
    namespace model
    {

      generic_model::generic_model_sptr
      leo_model::make ()
      {
        return generic_model::generic_model_sptr (
            new leo_model_impl ());
      }

      leo_model_impl::leo_model_impl () :
              generic_model ("leo_model"),
              d_nco ()
      {
        d_nco.set_freq (0);
      }

      leo_model_impl::~leo_model_impl ()
      {

      }

      float
      leo_model_impl::calculate_free_space_path_loss (double slant_range)
      {
        float wave_length = LIGHT_SPEED / get_frequency ();

        // Multiply slant_range to convert to meters/sec
        return 22.0 + 20 * std::log10 ((slant_range * 1e3) / wave_length);
      }

      float
      leo_model_impl::calculate_doppler_shift (double velocity)
      {
        return (1e3 * velocity * get_frequency ()) / LIGHT_SPEED;
      }

      void
      leo_model_impl::generic_work (const gr_complex *inbuffer,
                                    gr_complex *outbuffer, int noutput_items)
      {
        const gr_complex *in = (const gr_complex *) inbuffer;
        gr_complex *out = (gr_complex *) outbuffer;
        gr_complex* tmp = new gr_complex[noutput_items];

        float pl_attenuation_db;
        float atmo_attenuation_db;
        float doppler_shift;

        gr_complex attenuation_linear;

        /**
         * TODO: This object should be created at the constructor once.
         * The first argument that refers to the
         * frequency is taken by calling the get_frequency function from
         * the parent class. The problem is that in order to call this
         * function the channel_model_impl block must first set the transmission
         * mode, that accepts as a parameter, to the parent class.
         */
        d_atmosphere = new atmosphere (get_frequency ());

        d_tracker->add_elapsed_time ();
        double slant_range = d_tracker->get_slant_range ();
        float elevation_degrees = d_tracker->get_elevation_degrees();
        float elevation_radians = d_tracker->get_elevation_radians();
        d_atmosphere->set_elevation_angle(elevation_radians);

        if (slant_range) {
          doppler_shift = calculate_doppler_shift (d_tracker->get_velocity ());

          d_nco.set_freq (
              2 * M_PI * doppler_shift
                  / ((1e6 * noutput_items)
                      / d_tracker->get_time_resolution_us ()));
          d_nco.sincos (tmp, noutput_items, 1.0);
          volk_32fc_x2_multiply_32fc (tmp, tmp, inbuffer, noutput_items);

          /**
           * Calculate Free-Space Path Loss attenuation in db,
           * convert it to liner and multiply.
           */
          pl_attenuation_db = calculate_free_space_path_loss (slant_range)
              - (+get_tracker_antenna_gain () + get_satellite_antenna_gain ());
          attenuation_linear = gr_complex (pow (10, (-pl_attenuation_db / 10)));
          volk_32fc_s32fc_multiply_32fc (tmp, tmp, attenuation_linear,
                                         noutput_items);

          /**
           * Calculate atmospheric gases attenuation in db,
           * convert it to liner and multiply.
           */
          atmo_attenuation_db = d_atmosphere->get_gaseous_attenuation();
          attenuation_linear = gr_complex (pow (10, (-atmo_attenuation_db / 10)));
          volk_32fc_s32fc_multiply_32fc (outbuffer, tmp, attenuation_linear,
                                         noutput_items);
        }
        else {
          memset (outbuffer, 0, noutput_items * sizeof(gr_complex));
        }

        delete tmp;
        delete d_atmosphere;

        LEO_LOG_INFO(
            "Time: %s | Slant Range (km): %f | Elevation (degrees): %f | Path Loss (dB): %f | Atmospheric Loss (dB): %f | Doppler (Hz): %f",
            d_tracker->get_elapsed_time ().ToString ().c_str (), slant_range, elevation_degrees,
            pl_attenuation_db, atmo_attenuation_db, doppler_shift);

      }
    } /* namespace model */
  } /* namespace leo */
} /* namespace gr */

