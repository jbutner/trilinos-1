//@HEADER
/*
************************************************************************

              Isorropia: Partitioning and Load Balancing Package
                Copyright (2006) Sandia Corporation

Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
license for use of this work by or on behalf of the U.S. Government.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA
Questions? Contact Alan Williams (william@sandia.gov)
                or Erik Boman    (egboman@sandia.gov)

************************************************************************
*/
//@HEADER

#ifndef _Isorropia_EpetraCostDescriber_hpp_
#define _Isorropia_EpetraCostDescriber_hpp_

#include <Isorropia_ConfigDefs.hpp>
#include <Isorropia_CostDescriber.hpp>
#include <Teuchos_RefCountPtr.hpp>
#include <Teuchos_ParameterList.hpp>

#include <map>

#ifdef HAVE_EPETRA
class Epetra_Vector;
class Epetra_CrsMatrix;

namespace Isorropia {

/** The Isorropia::Epetra namespace implements a parallel partitioner
    that operates on Epetra objects.

    The objects to be partitioned may be the rows of an Epetra matrix
    or linear system.  There are two modes of operation.

    In one, the user supplies
    an Epetra object to Isorropia::Epetra::create_balanced_copy() and a
    new, rebalanced Epetra object is returned.

    In the other mode, the user supplies
    an Epetra object to Isorropia::Epetra::create_partitioner() and a
    Isorropia::Epetra::Partitioner object is returned.  This Partitioner
    object may be used to create an Isorropia::Epetra::Redistributor object.
    Finally the "redistribute" method of the Redistributor object
    may be used multiple times to redistribute Epetra objects with the
    same distribution as the Epetra object given to create_partitioner().

    In both cases, the application may supply row and edge weights with
    an Isorropia::Epetra::CostDescriber and may supply parameters with
    an Teuchos::ParameterList.

    If Trilinos has been built with the Zoltan parallel dynamic load
    balancing library (http://www.cs.sandia.gov/Zoltan),
    then Isorropia with use Zoltan to partition the Epetra object.
    The application can set Zoltan parameters with the ParameterList object.

    If Zoltan is not available, or the application has set the parameter
    "PARTITIONING_METHOD" to "SIMPLE_LINEAR", Isorropia will
    perform a simple linear partitioning
    of the object.  If the application supplied vertex weights with a
    CostDescriber, the linear partitioner will observe the vertex (row) weights.
    Otherwise the linear partitioner will use the number of non-zeroes in
    the row as the row weight.  The linear partitioner does not
    consider edge weights in the partitioning.
*/

namespace Epetra {

/** The CostDescriber class describes the vertex, edge and/or hyperedge weights.

    It is instantiated by the application to define
    weights, and then supplied to Isorropia with the
    Isorropia::Epetra::create_balanced_copy method or the.
    Isorropia::Epetra::create_partitioner method.

    The CostDescriber can hold vertex (row) weights.
    For graph partitioning with Zoltan it may
    also define graph edge (nonzero) weights.  For hypergraph partitioning
    with Zoltan it may describe hyperedge (column) weights.

    If the application does not provide weights, reasonable defaults will
    be used.  Zoltan parameters (supplied with an Isorropia::Epetra::ParameterList)
    are available to override those defaults.
*/

class CostDescriber : public Isorropia::CostDescriber {
public:
  /** Constructor */
  CostDescriber();

  /** Destructor */
  ~CostDescriber();

  /** For debugging purposes, print out the cost information 
   */
  void ShowCosts();

  /** Set parameters for the CostDescriber instance. The contents of the
     input paramlist object are copied into an internal ParameterList
     attribute. This class does not retain a reference
     to the input ParameterList after this method returns.
     Used by library, not application.
   */
  void setParameters(const Teuchos::ParameterList& paramlist);

  /** Vertex Weights; these typically correspond to matrix rows.
      Each vertex (row) weight should be supplied by exactly one process
      (the process that "owns" that row).
      Used by application
   */
  void setVertexWeights(Teuchos::RefCountPtr<const Epetra_Vector> vwts);

  /** Graph edge weights; these typically correspond to nonzeros of a
      square symmetric matrix.
     
      Edges belonging to a vertex are represented by non-zeroes in the
      row that corresponds to the vertex.  All of the edges for a given
      vertex should be supplied by the one process that "owns" that row.
      Used by application
   */
  void setGraphEdgeWeights(Teuchos::RefCountPtr<const Epetra_CrsMatrix> gewts);

  /** Hypergraph edge weights; these typically correspond to matrix columns.
     
      Matrices that represent hypergraphs are not in general square.
      There may be more or fewer hyperedges (columns) than vertices (rows).
     
      More than one process can supply a weight for the same column.  (So
      there is no concept of a process owning a hyperedge.)  Zoltan
      combines these weights according to the setting of the
      PHG_EDGE_WEIGHT_OPERATION parameter.
      Used by application
   */
  void setHypergraphEdgeWeights(Teuchos::RefCountPtr<const Epetra_Vector> hgewts);

  /** Supply a list of hypergraph edge weights and corresponding hypergraph
      global IDs.
     
      Used by application
   */
  void setHypergraphEdgeWeights(int numHGedges, const int *hgGIDs, const float *hgEwgts);

  /** Supply a list of hypergraph edge weights and corresponding hypergraph
      global IDs.
      Used by application
   */
  void setHypergraphEdgeWeights(int numHGedges, const int *hgGIDs, const double *hgEwgts);

  /** Query whether non-default vertex weights are present. If this
    function returns false, the caller can assume that vertex weights
    are all 1.0, or this process has no vertices.
     Used by library, not application.
  */
  bool haveVertexWeights() const;

  /** Get the number of vertices. Vertices typically correspond to
    matrix rows.
     Used by library, not application.
  */
  int getNumVertices() const;

  /** Create a map from each vertex global ID to its weight.  Return the
      number of vertices in the map.  If vertex weights are defined, there
      is one weight for each vertex owned by this process. 
     Used by library, not application.
  */
  int getVertexWeights(std::map<int, float> &wgtMap) const;

 /** Get the lists of vertex ids and weights.
     Used by library, not application.
  */
  void getVertexWeights(int numVertices,
                      int* global_ids, float* weights) const; 

  /** Query whether non-default graph edge weights are present.
     If this function returns false, the caller can assume that
     graph edge weights are all 1.0, or that this process has
     no rows.
     Used by library, not application.
  */
  bool haveGraphEdgeWeights() const;

  /** Get the number of graph edges for a specified vertex.
     Graph edges typically correspond to matrix nonzeros.
     Used by library, not application.
  */
  int getNumGraphEdges(int vertex_global_id) const;

  /** Create a map from neighbor global ID to edge weight, and return
      the number of neighbors.
     Used by library, not application.
  */
  int getGraphEdgeWeights(int vertex_global_id, std::map<int, float> &wgtMap) const;

  /** Get the graph edge weights for a specified vertex.
     Used by library, not application.
  */
  void getGraphEdgeWeights(int vertex_global_id,
                                   int num_neighbors,
                                   int* neighbor_global_ids,
                                   float* weights) const; 

  /** Query whether non-default hypergraph edge weights are present.
     If this function returns false, the caller can assume that
     hypergraph edge weights are all 1.0, or that all hyperedge
     weights were supplied by other processes.
     Used by library, not application.
  */
  bool haveHypergraphEdgeWeights() const;

  /** Get the number of Hypergraph edge weights. Hypergraph edges typically
     correspond to matrix columns.
     Used by library, not application.
  */
  int getNumHypergraphEdgeWeights() const;

  /** Get the hypergraph edge weights.  Used by library, not application.
  */
  void getHypergraphEdgeWeights(int numEdges,
                                        int* global_ids,
                                        float* weights) const;

  /**
     It's possible that (haveVertexWeights() == false) because this
     process has no rows of the matrix, but that
     the application overall has supplied vertex weights.
     Used by library, not application.
   */
  bool haveGlobalVertexWeights() const;
  /** to do
   */
  void setNumGlobalVertexWeights(int num);

  /**
     It's possible that (haveGraphEdgeWeights() == false) because this
     process has no rows of the matrix, but that
     the application overall has supplied graph edge weights.
     Used by library, not application.
   */
  bool haveGlobalGraphEdgeWeights() const;
  /** to do
   */
  void setNumGlobalGraphEdgeWeights(int num);

  /**
     It's possible that (haveHypergraphEdgeWeights() == false) because 
     the hypergraph edge weights were all supplied by other processes,
     but that the application overall has supplied hypergraph edge weights.
     Used by library, not application.
   */
  bool haveGlobalHypergraphEdgeWeights() const;
  /** to do
   */
  void setNumGlobalHypergraphEdgeWeights(int num);

private:
  /** to do
   */
  void allocate_hg_edge_weights_(int n);
  /** to do
   */
  void free_hg_edge_weights_();

  Teuchos::RefCountPtr<const Epetra_Vector> vertex_weights_;

  Teuchos::RefCountPtr<const Epetra_CrsMatrix> graph_edge_weights_;
  std::set<int> graph_self_edges_;

  int *hg_edge_gids_;
  float *hg_edge_weights_;
  int num_hg_edge_weights_;
  Teuchos::ParameterList paramlist_;

  int numGlobalVertexWeights_;
  int numGlobalGraphEdgeWeights_;
  int numGlobalHypergraphEdgeWeights_;

  // Create an array of the global IDs for the neighbors of the
  // given vertex, and also array of the edge weight for each edge.
  // Return the number of edges.  Self edges are not included.

  /** to do
   */
  int getEdges(int vertexGID, int len, int *nborGID, float *weights) const;


};//class CostDescriber

}//namespace Epetra
}//namespace Isorropia

#endif //HAVE_EPETRA

#endif

