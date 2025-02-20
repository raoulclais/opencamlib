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

#include <iostream>
#include <cassert>
#include <set>
#include <vector>

#include "gldata.hpp"

namespace ocl
{

unsigned int GLData::addVertex(float x, float y, float z, float r, float g, float b) {
    return addVertex( GLVertex(x,y,z,r,g,b) );
}
    
unsigned int GLData::addVertex(GLVertex v) {
    // add vertex with empty polygon-list.
    unsigned int idx = vertexArray.size();
    vertexArray.append(v);
    vertexDataArray.append( VertexData() );
    assert( vertexArray.size() == vertexDataArray.size() );
    return idx; // return index of newly appended vertex
}

unsigned int GLData::addVertex(float x, float y, float z, float r, float g, float b, Octnode* n) {
    unsigned int id = addVertex(x,y,z,r,g,b);
    vertexDataArray[id].node = n;
    return id;
}


void GLData::removeVertex( unsigned int vertexIdx ) {
    //std::cout << "GLData::removeVertex( " << vertexIdx << " )\n";
    
    // i) for each polygon of this vertex, call remove_polygon:
    BOOST_FOREACH( unsigned int polygonIdx, vertexDataArray[vertexIdx].polygons ) {
        removePolygon( polygonIdx );
    }
    // ii) overwrite with last vertex:
    unsigned int lastIdx = vertexArray.size()-1;
    if (vertexIdx != lastIdx) {
        vertexArray[vertexIdx] = vertexArray[lastIdx];
        vertexDataArray[vertexIdx] = vertexDataArray[lastIdx];
        // notify octree-node with new index here!
        // vertex that was at lastIdx is now at vertexIdx
        vertexDataArray[vertexIdx].node->swapIndex( lastIdx, vertexIdx );
        
        // request each polygon to re-number this vertex.
        BOOST_FOREACH( unsigned int polygonIdx, vertexDataArray[vertexIdx].polygons ) {
            unsigned int idx = polygonIdx*polyVerts;
            for (int m=0;m<polyVerts;++m) {
                if ( indexArray[ idx+m ] == lastIdx )
                    indexArray[ idx+m ] = vertexIdx;
            }
        }
    }
    // shorten array
    vertexArray.resize( vertexArray.size()-1 );
    vertexDataArray.resize( vertexDataArray.size()-1 );
    assert( vertexArray.size() == vertexDataArray.size() );
}

int GLData::addPolygon( std::vector<GLuint>& verts) {
    // append to indexArray, request each vertex to update
    unsigned int polygonIdx = indexArray.size()/polyVerts;
    BOOST_FOREACH( GLuint vertex, verts ) {
        indexArray.append(vertex);
        vertexDataArray[vertex].addPolygon(polygonIdx); // add index to vertex i1
    }
    return polygonIdx;
}

void GLData::removePolygon( unsigned int polygonIdx) {
    //std::cout << " removePolygon( " << polygonIdx << " )\n";
    unsigned int idx = polyVerts*polygonIdx; // start-index for polygon
    // i) request remove for each vertex in polygon:
    for (int m=0; m<polyVerts ; ++m)
        vertexDataArray[ indexArray[idx+m]   ].removePolygon(polygonIdx);
    
    // check for orphan vertices (?), and delete them (?)
    
    unsigned int last_index = (indexArray.size()-polyVerts);
    // if deleted polygon is last on the list, do nothing??
    if (idx!=last_index) { 
        // ii) remove from polygon-list by overwriting with last element
        for (int m=0; m<polyVerts ; ++m)
            indexArray[idx+m  ] = indexArray[ last_index+m   ];
        // iii) for the moved polygon, request that each vertex update the polygon number
        for (int m=0; m<polyVerts ; ++m) {
            vertexDataArray[ indexArray[idx+m   ] ].addPolygon( idx/polyVerts ); // this is the new polygon index
            vertexDataArray[ indexArray[idx+m   ] ].removePolygon( last_index/polyVerts ); // this polygon is no longer there!
        }
    }
    indexArray.resize( indexArray.size()-polyVerts ); // shorten array
} 

void GLData::genVBO() {
    vertexBuffer = makeBuffer(  QGLBuffer::VertexBuffer, vertexArray );
    indexBuffer = makeBuffer( QGLBuffer::IndexBuffer, indexArray );
}

void GLData::updateVBO() {
    updateBuffer( vertexBuffer, vertexArray );
    updateBuffer( indexBuffer, indexArray );

}

bool GLData::bind() {
    return (vertexBuffer->bind() && indexBuffer->bind());
}
/// release the vertex and index buffers
void GLData::release() {
    vertexBuffer->release();
    indexBuffer->release();
}

// ? replace with transformation-matrix
void GLData::setPosition(float x, float y, float z) {
    pos = GLVertex(x,y,z);
}
    
    
void GLData::print() {
    std::cout << "GLData vertices: \n";
    //int n = 0;
    for( int n = 0; n < vertexArray.size(); ++n ) {
        std::cout << n << " : ";
        vertexArray[n].str();
        std::cout << " polys: "; 
        vertexDataArray[n].str();
        std::cout << "\n";
    }
    std::cout << "GLData polygons: \n";
    int polygonIndex = 0;
    for( int n=0; n< indexArray.size(); n=n+polyVerts) {
        std::cout << polygonIndex << " : ";
        for (int m=0;m<polyVerts;++m)
            std::cout << indexArray[n+m] << " "; 
        std::cout << "\n";
        ++polygonIndex;
    }
}


} // end ocl namespace

