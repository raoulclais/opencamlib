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


#include <list>
// uncomment to disable assert() calls
// #define NDEBUG
#include <cassert>

#include <boost/foreach.hpp>

#include "point.hpp"
#include "triangle.hpp"
#include "numeric.hpp"
#include "octnode.hpp"
#include "volume.hpp"


namespace ocl
{

//**************** Octnode ********************/

// this defines the position of each octree-vertex with relation to the center of the node
// this also determines in which direction the center of a child node is
const Point Octnode::direction[8] = {
                     Point( 1, 1,-1),   // 0
                     Point(-1, 1,-1),   // 1
                     Point(-1,-1,-1),   // 2
                     Point( 1,-1,-1),   // 3
                     Point( 1, 1, 1),   // 4
                     Point(-1, 1, 1),   // 5
                     Point(-1,-1, 1),   // 6
                     Point( 1,-1, 1)    // 7
                    };
// surface enumeration
// surf     vertices  vertices
// 0:       2,3,7     2,6,7
// 1:       0,4,7     0,3,7
// 2:       0,1,4     1,4,5
// 3:       1,5,6     1,2,6
// 4:       0,2,3     0,1,2
// 5:       4,6,7     4,5,6

const char Octnode::octant[8] = {
                    1,
                    2,
                    4,
                    8,
                    16,
                    32,
                    64,
                    128
                };

Octnode::Octnode(Octnode* nodeparent, unsigned int index, double nodescale, unsigned int nodedepth) {
    parent = nodeparent;
    idx = index;
    child.resize(8);
    vertex.resize(8);
    f.resize(8);
    
    if (parent) {
        center = parent->childcenter(idx);
        inside = parent->inside;
        outside = parent->outside;
    } else {
        outside = true;
        inside = false;
        center = new Point(0,0,0); // default center for root is (0,0,0)
    }
    scale = nodescale;
    depth = nodedepth;
    
    for ( int n=0;n<8;++n) {
        vertex[n] = new Point(*center + scale*direction[n] ) ;
        f[n] = 1e6;
    }
    bb.clear();
    bb.addPoint( *vertex[2] ); // vertex[2] has the minimum x,y,z coordinates
    bb.addPoint( *vertex[4] ); // vertex[4] has the max x,y,z
    
    
    isosurface_valid = false;
    evaluated = false;
    childcount = 0;
    childStatus = 0;
}

// call delete on children, vertices, and center
Octnode::~Octnode() {
    for(int n=0;n<8;++n) {
        //if (child[n]) {
            delete child[n];
            child[n] = 0;
        //}
        //if (vertex[n]) {
            delete vertex[n];
            vertex[n] = 0;
        //}
    }
    //if (center) {
        delete center;
        center = 0;
    //}
}

// create the 8 children of this node
void Octnode::subdivide() {
    if (this->childcount==0) {
        for( int n=0;n<8;++n ) {
            this->child[n] = new Octnode( this, n , scale/2.0 , depth+1 ); // parent,  idx, scale,   depth
            ++childcount;
            // optimization: inherit one f[n] from the corner?
        }
    } else {
        std::cout << " DON'T subdivide a non-leaf node \n";
        assert(0); 
    }
}

// evaluate vol->dist() at all the vertices and store in f[]
// set the insinde/outside flags based on the sings of dist()
void Octnode::evaluate(const OCTVolume* vol) {
    //assert( childcount==0 );
    outside = true;
    inside = true;
    for ( int n=0;n<8;++n) { // go through the 8 corners of the cube
        double newf = vol->dist( *(vertex[n]) );
        if ( !evaluated ) {
            f[n] = newf;
            setInValid();
            //isosurface_valid = false; 
        } else if( (newf < f[n] )   ) { // only update distance field if new distance is smaller than old stored distance
            f[n] = newf;
            setInValid();
            //isosurface_valid = false; 
        } 
        
        // set the flags
        if ( f[n] <= 0.0 ) {// if one vertex is inside
            outside = false; // then it's not an outside-node
        } else { // if one vertex is outside
            assert( f[n] > 0.0 );
            inside = false; // then it's not an inside node anymore
        }
    }
    evaluated = true;
}

// return centerpoint of child with index n
Point* Octnode::childcenter(int n) {
    return  new Point(*center + (0.5*scale * direction[n]));
}

// string repr
std::ostream& operator<<(std::ostream &stream, const Octnode &n) {
    stream << " c=" << *(n.center) << " depth=" << n.depth ;     
    return stream;
}

// string repr
std::string Octnode::str() const {
    std::ostringstream o;
    o << *this;
    return o.str();
}

} // end namespace
// end of file octnode.cpp
