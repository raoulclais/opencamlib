/*  
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
#ifndef WEAVE_HPP
#define WEAVE_HPP

#include <vector>

#include "point.hpp"
#include "fiber.hpp"
#include "weave_typedef.hpp"
#include "halfedgediagram.hpp"

namespace ocl
{

namespace weave2
{



                 
/// weave-graph, 2nd impl. based on HEDIGraph
/// see http://www.anderswallin.net/2011/05/weave-notes/
class Weave {
    public:
        Weave() {}
        virtual ~Weave() {}

        /// add Fiber f to the graph
        /// each fiber should be either in the X or Y-direction
        /// FIXME: seprate addXFiber and addYFiber methods?
        void addFiber(Fiber& f);
        
        /// from the list of fibers, build a graph
        void build();

        /// run planar_face_traversal to get the waterline loops
        void face_traverse();
        
        /// retrun list of loops
        std::vector< std::vector<Point> > getLoops() const;
        
        /// string representation
        std::string str() const;
        
        /// print out information about the graph
        void printGraph() const;
        
    protected:       
    
        /// add CL vertex to weave
        /// sets position, type, and inserts the VertexPair into Interval::intersections
        /// also adds the CL-vertex to clVertices, a list of cl-verts to be processed during face_traverse()
        Vertex add_cl_vertex( Point position, Interval& interv, double ipos);
        
        /// given a vertex in the graph, find it's upper and lower neighbor vertices
        std::pair<Vertex,Vertex> find_neighbor_vertices( VertexPair v_pair, Interval& ival);
         
// DATA
        /// the weave-graph
        WeaveGraph g;
        /// output: list of loops in this weave
        std::vector< std::vector<Vertex> > loops;
        /// the X-fibers
        std::vector<Fiber> xfibers;
        /// the Y-fibers
        std::vector<Fiber> yfibers;
        /// set of CL-points
        std::set<Vertex> clVertices;
};

} // end weave2 namespace

} // end ocl namespace
#endif
// end file weave2.h
