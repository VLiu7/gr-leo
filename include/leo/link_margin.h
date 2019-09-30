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

#ifndef INCLUDED_LEO_LINK_MARGIN_H
#define INCLUDED_LEO_LINK_MARGIN_H

#include <leo/api.h>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

namespace gr
{
  namespace leo
  {

    class LEO_API link_margin
    {

    public:

      ~link_margin ();

      typedef boost::shared_ptr<link_margin> link_margin_sptr;

      static link_margin::link_margin_sptr
      make ();

      float
      calc_link_margin (float total_loss_db, float satellite_antenna_gain,
                        float tracker_antenna_gain,
                        float transmission_power_dbw, float gs_receiver_bw);

    };
  } // namespace leo
} // namespace gr

#endif /* INCLUDED_LEO_LINK_MARGIN_H */
