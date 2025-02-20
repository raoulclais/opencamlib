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

#ifndef OCTREE_H
#define OCTREE_H

#include <iostream>
#include <list>
#include <cassert>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "point.hpp"
#include "triangle.hpp"
#include "bbox.hpp"
#include "gldata.hpp"
#include "marching_cubes.hpp"

namespace ocl
{

class Octnode;
class OCTVolume;

// the tree uses an iso-surface algorithm that produces vertices and polygons.
// this defines types for the callback fuctions called on GLData

// addVertex

// addPolygon( vertexIdx0, vertexIdx1, vertexIdx2 )   
// (polygons are removed automatically by GLData when vertices are removed)
//typedef boost::function3< void, unsigned int, unsigned int, unsigned int> Void3UIntCallBack;
// void removeVertex( vertexIdx )
//typedef boost::function1< void, unsigned int> VoidUIntCallBack;

/// Octree class for cutting simulation
/// see http://en.wikipedia.org/wiki/Octree
/// The root node is divided into eight sub-octants, and each sub-octant
/// is recursively further divided into octants.
/// The side-length of the root node is root_scale
/// The dept of the root node is zero.
/// Subdivision is continued unti max_depth is reached.
/// A node at tree-dept n is a cube with side-length root_scale/pow(2,n)
///
/// This class stores the root Octnode and allows operations on the tree
///
class Octree {
    public:
        Octree() { 
            assert(0); 
        }
        virtual ~Octree();
        /// create an octree with a root node with scale=root_scale, maximum
        /// tree-depth of max_depth and centered at centerp.
        Octree(double root_scale, unsigned int max_depth, Point& centerPoint);
        /// subtract vol from tree
        void diff_negative(const OCTVolume* vol);
        /// find all leaf-nodes
        void get_leaf_nodes( std::vector<Octnode*>& nodelist) const {
            get_leaf_nodes( root,  nodelist);
        }
        /// find the leaf-nodes under Octnode* current
        void get_leaf_nodes(Octnode* current, std::vector<Octnode*>& nodelist) const;
        
        /// find the leaf-nodes under Octnode* current that are invalid.
        void get_invalid_leaf_nodes(std::vector<Octnode*>& nodelist) const;
        void get_invalid_leaf_nodes( Octnode* current, std::vector<Octnode*>& nodelist) const;
        
        /// return all nodes in tree
        void get_all_nodes(Octnode* current, std::vector<Octnode*>& nodelist) const;
        
        /// initialize by recursively calling subdivide() on all nodes n times
        void init(const unsigned int n);
        /// return the maximum depth of the tree
        unsigned int get_max_depth() const;
        /// return the root scale
        double get_root_scale() const;
        /// return the leaf scale (the minimum resolution of the tree)
        double leaf_scale() const;
        /// string output
        std::string str() const;
        Octnode* getRoot() {return root;}
        void setGLData(GLData* gdata) {
            g=gdata;
        }
        void updateGL() { updateGL(root); }
        void setIsoSurf(MarchingCubes* m) {mc = m;}
        bool debug;
    protected:
        
        // run isosurface-algorithm on current Octnode, and push gl-data to GLData
        void updateGL(Octnode* current);
        
        /// recursively traverse the tree subtracting vol
        void diff_negative(Octnode* current, const OCTVolume* vol);
        
        /// remove vertices associated with the current node
        void remove_node_vertices(Octnode* current );
    // DATA
        /// the root scale
        double root_scale;
        /// the maximum tree-depth
        unsigned int max_depth;
        /// pointer to the root node
        Octnode* root;
        GLData* g;
        MarchingCubes* mc;
        
};

} // end namespace
#endif
// end file octree.h
