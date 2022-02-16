
/*
 *  Wave defaults and constants
 *  Copyright (C) 2022 Everett Gaius S. Vergara
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *	
 */

#ifndef _DROPLET_HPP_
#define _DROPLET_HPP_

#include "Dimensions.hpp"

namespace g80 {
    struct Droplet {
        Point point{0, 0};
        int animation_ix{0};
        int ixn{1};
        int stepper{0};
        int stepper_max{3};
    };
}

#endif 