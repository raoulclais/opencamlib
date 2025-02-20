/*  $Id$
 * 
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WATERLINE_H
#define WATERLINE_H

#include <iostream>
#include <string>
#include <vector>

#include "point.hpp"
#include "fiber.hpp"
#include "batchpushcutter.hpp"
#include "operation.hpp"


namespace ocl
{


/// \brief a Waterline toolpath follows the shape of the model at a constant z-height in the xy-plane

/// The Waterline object is used for generating waterline or z-slice toolpaths
/// from an STL-model. Waterline uses two BatchPushCutter sub-operations to find out where the CL-points are located
/// and a Weave to split and order the CL-points correctly into loops.
class Waterline : public Operation {
    public:
        /// create an empty Waterline object
        Waterline(); 
        virtual ~Waterline();
        
        /// Set the z-coordinate for the waterline we generate
        void setZ(const double z) {
            zh = z;
        }
        /// run the Waterline algorithm. setSTL, setCutter, setSampling, and setZ must
        /// be called before a call to run()
        virtual void run();
        
        /// returns a vector< vector< Point > > with the resulting waterline loops
        std::vector< std::vector<Point> >  getLoops() const {
            return loops;
        }
        
    protected:
        /// from xfibers and yfibers, build the weave, run face-traverse, and write toolpaths to loops
        void weave2_process(); 
        /// initialization of fibers
        void init_fibers();
        /// x and y-coordinates for fiber generation
        std::vector<double> generate_range( double start, double end, int N) const;
        
    // DATA
        /// the z-height for this Waterline
        double zh;
        /// the results of this operation, a list of loops
        std::vector< std::vector<Point> >  loops; 
        
        /// x-fibers for this operation
        std::vector<Fiber> xfibers;
        /// y-fibers for this operation
        std::vector<Fiber> yfibers;
        
};



} // end namespace

#endif
