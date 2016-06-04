/*PGR-GNU*****************************************************************
 *

Copyright (c) 2015 Celia Virginia Vergara Castillo
vicky_vergara@hotmail.com

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/

#pragma once
#ifdef __MINGW32__
#include <winsock2.h>
#include <windows.h>
#ifdef open
#undef open
#endif
#endif



#include <boost/graph/iteration_macros.hpp>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <deque>
#include <vector>
#include <set>
#include <map>
#include <limits>

#include "./pgr_types.h" // for pgr_edge_t 

#include "./ch_vertex.h"
#include "./ch_edge.h"
#include "./basic_vertex.h"
#include "./xy_vertex.h"

#include "./basic_edge.h"
//#include "../../contraction/src/edge.h"

#include "./pgr_assert.h"

/*! @brief boost::graph simplified to pgRouting needs

  This class gives the handling basics of a boost::graph of kind G
  where G:
  can be an undirected graph or a directed graph.

Requiremets:
============

A vertex class T_V
------------------

Current Available vertex classes:
- Basic_vertex
- XY_vertex


An edge class T_E
-----------------

Current Available edge classes:
- Basic_edge

extract_vertices function
-------------------------

Data obtained from postgresql is stored in
A C array of pgr_edge_t type.

~~~~{.c}
std::vector< T_V > 
extract_vertices(pgr_edge_t *, size_t)
~~~~


Data obtained from postgresql is stored in
o a vector container.

~~~~{.c}
std::vector< T_V > 
extract_vertices(std::vector< pgr_edge_t >)
~~~~

Boost Graph
-------------

The code is prepared to be used for:
- boost::adjacency_list graph type
- boost::undirectedS when the graph is UNDIRECTED
- boost::bidirectionalS when the graph is DIRECTED


~~~~{.c}
boost::adjacency_list
< boost::vecS,  // not tested with other values
boost::vecS,  // not tested with other values
boost::undirectedS,  // USING UNDIRECTED 
Basic_vertex,  // the vertex class
Basic_edge >   // the edge class
~~~~



Eample Usage:
=============

For this example we will use:
- Basic_vertex
- Basic_edge 
- pgr_edge_t


Create Graph type
-----------------

~~~~{.c}
typedef typename
graph::Pgr_base_graph <
boost::adjacency_list < 
boost::vecS,
    boost::vecS,
    boost::bidirectionalS,
    Basic_vertex,
    Basic_edge >,
    Basic_vertex,
    Basic_edge >

    DirectedGraph;
~~~~


Initializing the graph
------------------------------

Graph initialization is for seting the Vertices of the graph.

//TODO discuss if also the edges

Vector of unique vertices of the graph
~~~~{.c}
size_t total_edges;
pgr_edge_t *my_edges = NULL;
pgr_get_edges(edges_sql, &my_edges, &total_tuples); 
std::vector< Basic_Vertex > vertices(pgRouting::extract_vertices(my_edges));
~~~~

There are several ways to initialize the graph

~~~~{.c}
// 1. Initializes an empty graph
pgRouting::DirectedGraph digraph(gType);

// 2. Initializes a graph based on the vertices
pgRouting::DirectedGraph digraph(
    verices,
    gType);
vertices.clear();

3. Initializes a graph based on the extracted vertices
pgRouting::DirectedGraph digraph(
    pgRouting::extract_vertices(my_edges, total_edges);
    gType);

4. Initializes a graph based on the extracted vertices
pgRouting::DirectedGraph digraph(
    pgRouting::extract_vertices(my_edges);
    gType);
~~~~

1. Initializes an empty graph
  - vertices vector size is 0

2. Initializes a graph based on the vertices:
  - vertices vector size is vertices.size()
  - the vertices are inserted
  - vertices container can be clared to free memory

3. Initializes a graph based on the vertices extracted
  - from edges stored on a C array
  - the vertices are inserted

4. Initializes a graph based on the vertices extracted
  - from edges stored on a vector
  - the vertices are inserted


Fill the graph 
---------------------

After initializing the graph with the vertices, the edges can be added.

~~~~{.c}
// inserting edges from a C array
digraph.graph_insert_data(my_edges, total_edges);

// adding more edges to the graph from a vector container
digraph.graph_insert_data(new_edges);
~~~~

*/
namespace pgRouting {

namespace graph{
template <class G, typename Vertex, typename Edge>
class Pgr_base_graph;

} // namespace graph


/** @name Graph types 
  Type      |   pgRouting
  :---------: | :---------------------
  UndirectedGraph | Basic undirected graph
  DirectedGraph | Basic directed graph
  xyUndirectedGraph | X & Y values stored on the vertex 
  xyDirectedGraph | X & Y values stored on the vertex 
  */
//@{
typedef typename graph::Pgr_base_graph <
boost::adjacency_list < boost::vecS, boost::vecS,
    boost::undirectedS,
    Basic_vertex, Basic_edge >,
    Basic_vertex, Basic_edge > UndirectedGraph;

typedef typename graph::Pgr_base_graph <
boost::adjacency_list < boost::vecS, boost::vecS,
    boost::bidirectionalS,
    Basic_vertex, Basic_edge >,
    Basic_vertex, Basic_edge > DirectedGraph;

typedef typename graph::Pgr_base_graph <
boost::adjacency_list < boost::listS, boost::vecS,
    boost::undirectedS,
    XY_vertex, Basic_edge >,
    XY_vertex, Basic_edge > xyUndirectedGraph;

typedef typename graph::Pgr_base_graph <
boost::adjacency_list < boost::listS, boost::vecS,
    boost::bidirectionalS,
    XY_vertex, Basic_edge >,
    XY_vertex, Basic_edge > xyDirectedGraph;

// TODO (Rohith) add this to the comment above
typedef typename graph::Pgr_base_graph <
boost::adjacency_list < boost::listS, boost::vecS,
    boost::undirectedS,
    contraction::Vertex, Basic_edge >,
    contraction::Vertex, Basic_edge > CUndirectedGraph;

typedef typename graph::Pgr_base_graph <
boost::adjacency_list < boost::listS, boost::vecS,
    boost::bidirectionalS,
    contraction::Vertex, Basic_edge >,
    contraction::Vertex, Basic_edge > CDirectedGraph;
//@}


namespace graph{

template <class G, typename T_V, typename T_E>
class Pgr_base_graph {
 public:
     /** @name Graph related types
       Type      |     boost meaning     |   pgRouting meaning
       :---------: | :-------------------- | :----------------------
       G        | boost::adjacency_list |   Graph
       V        | vertex_descriptor     |   Think of it as local ID of a vertex
       E        | edge_descriptor       |   Think of it as local ID of an edge
       V_i      | vertex_iterator       |   To cycle the vertices of the Graph
       E_i      | edge_iterator         |   To cycle the edges of the Graph
       EO_i     | out_edge_iterator     |   To cycle the out going edges of a vertex
       EI_i     | in_edge_iterator      |   To cycle the in coming edges of a vertex (only in bidirectional graphs)
       */
     //@{
     typedef G B_G;
     typedef typename boost::graph_traits < G >::vertex_descriptor V;
     typedef typename boost::graph_traits < G >::edge_descriptor E;
     typedef typename boost::graph_traits < G >::vertex_iterator V_i;
     typedef typename boost::graph_traits < G >::edge_iterator E_i;
     typedef typename boost::graph_traits < G >::out_edge_iterator EO_i;
     typedef typename boost::graph_traits < G >::in_edge_iterator EI_i;

     typedef typename boost::graph_traits < G >::vertices_size_type     vertices_size_type;
     typedef typename boost::graph_traits < G >::edges_size_type        edges_size_type;
     typedef typename boost::graph_traits < G >::degree_size_type       degree_size_type;

     //@}

     /** @name Id handling related types
       Type      |  Meaning       |   pgRouting Meaning
       :---------: | :------------- | :----------------------
       id_to_V  | maps id -> V   | given an id store the V
       LI       | Left Iterator  | iterates over id_to_V
       */
     //@{

     typedef typename std::map< int64_t, V > id_to_V;
     typedef typename id_to_V::const_iterator LI;

     //@}

     //! @name The Graph
     //@{
     G graph;                //!< The graph
     size_t m_num_vertices;  //!< local count.
     graphType m_gType;      //!< type (DIRECTED or UNDIRECTED)
     //@}

     //! @name Id mapping handling
     //@{

     id_to_V  vertices_map;   //!< id -> graph id

     //@}

     //! @name Graph Modification
     //@{
     //! Used for storing the removed_edges

     std::deque< T_E > removed_edges;

     //@}



     //! @name The Graph
     //@{
     //! \brief Constructor
     /*!
       - Prepares the graph to be of type gtype
       - inserts the vertices
       - The vertices must be checked (if necessary)  before calling the constructor
       */
     Pgr_base_graph< G , T_V, T_E >(const std::vector< T_V > &vertices, graphType gtype)
         : graph(vertices.size()),
         m_num_vertices(vertices.size()),
         m_gType(gtype) {
             pgassert(boost::num_vertices(graph) == num_vertices());
             pgassert(boost::num_vertices(graph) == vertices.size());
#if 0
             pgassert(pgRouting::check_vertices(vertices) == 0);
#endif
             size_t i = 0;
             for (auto vi = boost::vertices(graph).first; vi != boost::vertices(graph).second; ++vi) {
                 vertices_map[vertices[i].id] = (*vi);
                 graph[(*vi)].cp_members(vertices[i++]);
             }
         }

     /*!
       Prepares the _graph_ to be of type gtype with 0 vertices
       */
     explicit Pgr_base_graph< G , T_V, T_E >(graphType gtype)
         : graph(0),
         m_num_vertices(0),
         m_gType(gtype) {
         }


     //! @name Insert data
     //@{
     /*! \brief Inserts *count* edges of type *T* into the graph
      *  
      *  Converts the edges to a std::vector<T> & calls the overloaded
      *  twin function.
      *
      *  @param edges
      *  @param count
      */
     template < typename T >
         void graph_insert_data(const T *edges, int64_t count) {
             graph_insert_data(std::vector < T >(edges, edges + count));
         }
     /*! @brief Inserts *count* edges of type *pgr_edge_t* into the graph

        The set of edges should not have an illegal vertex defined
        
        When the graph is empty calls:
        - \bextract_vertices
        and throws an exeption if there are illegal vertices.
        
        
        When developing:
          - if an illegal vertex is found an exeption is thrown
          - That means that the set of vertices should be checked in the
            code that is being developed
        
        No edge is inserted when there is an error on the vertices

        @param edges
      */
     template < typename T >
         void graph_insert_data(const std::vector < T > &edges) {
             for (const auto edge : edges) {
                 graph_add_edge(edge);
             }
         }
     //@}

    private:
     /*! @brief adds the vertices into the graph
      *
      * PRECONDITIONS:
      * ~~~~~{.c}
      * precondition(boost::num_vertices(graph) == 0);
      * for (vertex : vertices) 
      *    precondition(!has_vertex(vertex.id));
      * ~~~~~
      *
      * POSTCONDITIONS:
      * ~~~~~{.c}
      * postcondition(boost::num_vertices(graph) == vertices.size());
      * for (vertex : vertices) 
      *    precondition(has_vertex(vertex.id));
      * ~~~~~
      */
     void add_vertices(std::vector< T_V > vertices);

    public:



     //! @name boost wrappers
     //@{
     //! \brief get the out-degree  of a vertex

     /*!
       \returns 0: The out degree of a vertex that its not in the graph

       @param [IN] *vertex_id* original vertex id
       */
     degree_size_type out_degree(int64_t vertex_id) const{
         if (!has_vertex(vertex_id)) {
             return 0;
         }
         return out_degree(get_V(vertex_id));
     }


     /*! @brief get the vertex descriptor of the vertex

       When the vertex does not exist
       - creates a new vetex

       @return V: The vertex descriptor of the vertex
       */
     V get_V(const T_V &vertex) {
         auto vm_s(vertices_map.find(vertex.id));
         if (vm_s == vertices_map.end()) {
             auto v =  add_vertex(graph);
             graph[v].cp_members(vertex);
             vertices_map[vertex.id] =  v;
             return v;
         }
         return vm_s->second;
     }

     /*! @brief get the vertex descriptor of the vid

       Call has_vertex(vid) before calling this function

       @return V: The vertex descriptor of the vertex
       */
     V get_V(int64_t vid) const {
         pgassert(has_vertex(vid));
         return vertices_map.find(vid)->second;
     }

     //! @brief True when vid is in the graph
     bool has_vertex(int64_t vid) const{
         return vertices_map.find(vid) != vertices_map.end();
     }

     //! @brief True when vid is in the graph
     degree_size_type in_degree(V &v) const {
         return boost::in_degree(v, graph);
     }

     //! @brief True when vid is in the graph
     degree_size_type out_degree(V &v) const {
         return boost::out_degree(v, graph);
     }

     //@}


     //! @name edge disconection/reconnection 
     //@{
     //! \brief Disconnects all edges from p_from to p_to
     /*!

       - No edge is disconnected if the vertices id's do not exist in the graph
       - All removed edges are stored for future reinsertion
       - All parallel edges are disconnected (automatically by boost)

       ![disconnect_edge(2,3) on an UNDIRECTED graph](disconnectEdgeUndirected.png)
       ![disconnect_edge(2,3) on a DIRECTED graph](disconnectEdgeDirected.png)

       @param [IN] *p_from* original vertex id of the starting point of the edge
       @param [IN] *p_to*   original vertex id of the ending point of the edge
       */
     void disconnect_edge(int64_t p_from, int64_t p_to);


     //! \brief Disconnects the outgoing edges with a particular original id from a vertex
     /*!

       - No edge is disconnected if it doesn't exist in the graph
       - Removed edges are stored for future reinsertion
       - all outgoing edges with the edge_id are removed if they exist

       @param [IN] *vertex_id* original vertex
       @param [IN] *edge_id* original edge_id
       */
     void disconnect_out_going_edge(int64_t vertex_id, int64_t edge_id);




     //! \brief Disconnects all incomming and outgoing edges from the vertex
     /*!
       boost::graph doesn't recommend th to insert/remove vertices, so a vertex removal is
       simulated by disconnecting the vertex from the graph

       - No edge is disconnected if the vertices id's do not exist in the graph
       - All removed edges are stored for future reinsertion
       - All parallel edges are disconnected (automatically by boost)

       ![disconnect_vertex(2) on an UNDIRECTED graph](disconnectVertexUndirected.png)
       ![disconnect_vertex(2) on a DIRECTED graph](disconnectVertexDirected.png)

       @param [IN] *p_vertex* original vertex id of the starting point of the edge
       */
     void disconnect_vertex(int64_t p_vertex);
     void disconnect_vertex(V vertex);


     //! \brief Reconnects all edges that were removed
     void restore_graph();

     //@}

     //! @name only for stand by program
     //@{

     friend std::ostream& operator<<(std::ostream &log, const Pgr_base_graph< G, T_V, T_E > &g) {

         typename Pgr_base_graph< G, T_V, T_E >::EO_i out, out_end;

         for (auto vi = vertices(g.graph).first; vi != vertices(g.graph).second; ++vi) {
             if ((*vi) >= g.m_num_vertices) break;
             log << (*vi) << ": "  << " out_edges_of(" << g.graph[(*vi)] << "):";
             for (boost::tie(out, out_end) = out_edges(*vi, g.graph);
                     out != out_end; ++out) {
                 log << ' ' << g.graph[*out].id << "=(" << g.graph[source(*out, g.graph)].id
                     << ", " << g.graph[target(*out, g.graph)].id << ") = "
                     <<  g.graph[*out].cost <<"\t";
             }
             log << std::endl;
         }
         return log;
     }

     //@}


     int64_t get_edge_id(V from, V to, double &distance) const;

     size_t num_vertices() const { return boost::num_vertices(graph);}

     T_V operator[](V v) const {
         return graph[v];
     }


     void graph_add_edge(const T_E &edge);

     template < typename T >
         void graph_add_edge(const T &edge);
};




template < class G, typename T_V, typename T_E >
void
Pgr_base_graph< G, T_V, T_E >::disconnect_edge(int64_t p_from, int64_t p_to) {
    T_E d_edge;

    // nothing to do, the vertex doesnt exist
    if (!has_vertex(p_from) || !has_vertex(p_to)) return;

    EO_i out, out_end;
    V g_from(get_V(p_from));
    V g_to(get_V(p_to));

    // store the edges that are going to be removed
    for (boost::tie(out, out_end) = out_edges(g_from, graph);
            out != out_end; ++out) {
        if (target(*out, graph) == g_to) {
            d_edge.id = graph[*out].id;
            d_edge.source = graph[source(*out, graph)].id;
            d_edge.target = graph[target(*out, graph)].id;
            d_edge.cost = graph[*out].cost;
            removed_edges.push_back(d_edge);
        }
    }
    // the actual removal
    boost::remove_edge(g_from, g_to, graph);
}



template < class G, typename T_V, typename T_E >
void
Pgr_base_graph< G, T_V, T_E >::disconnect_out_going_edge(int64_t vertex_id, int64_t edge_id) {
    T_E d_edge;

    // nothing to do, the vertex doesnt exist
    if (!has_vertex(vertex_id)) return;
    auto v_from(get_V(vertex_id));

    EO_i out, out_end;
    bool change = true;
    // store the edge that are going to be removed
    while (change) {
        change = false;
        for (boost::tie(out, out_end) = out_edges(v_from, graph);
                out != out_end; ++out) {
            if (graph[*out].id  == edge_id) {
                d_edge.id = graph[*out].id;
                d_edge.source = graph[source(*out, graph)].id;
                d_edge.target = graph[target(*out, graph)].id;
                d_edge.cost = graph[*out].cost;
                removed_edges.push_back(d_edge);
                boost::remove_edge((*out), graph);
                change = true;
                break;
            }
        }
    }
}


template < class G, typename T_V, typename T_E >
void
Pgr_base_graph< G, T_V, T_E >::disconnect_vertex(int64_t vertex) {
    if (!has_vertex(vertex)) return;
    disconnect_vertex(get_V(vertex));
}

template < class G, typename T_V, typename T_E >
void
Pgr_base_graph< G, T_V, T_E >::disconnect_vertex(V vertex) {
    T_E d_edge;

    EO_i out, out_end;
    // store the edges that are going to be removed
    for (boost::tie(out, out_end) = out_edges(vertex, graph);
            out != out_end; ++out) {
        d_edge.id = graph[*out].id;
        d_edge.source = graph[source(*out, graph)].id;
        d_edge.target = graph[target(*out, graph)].id;
        d_edge.cost = graph[*out].cost;
        removed_edges.push_back(d_edge);
    }

    // special case
    if (m_gType == DIRECTED) {
        EI_i in, in_end;
        for (boost::tie(in, in_end) = in_edges(vertex, graph);
                in != in_end; ++in) {
            d_edge.id = graph[*in].id;
            d_edge.source = graph[source(*in, graph)].id;
            d_edge.target = graph[target(*in, graph)].id;
            d_edge.cost = graph[*in].cost;
            removed_edges.push_back(d_edge);
        }
    }

    // delete incomming and outgoing edges from the vertex
    boost::clear_vertex(vertex, graph);
}

template < class G, typename T_V, typename T_E >
void
Pgr_base_graph< G, T_V, T_E >::restore_graph() {
    while (removed_edges.size() != 0) {
        graph_add_edge(removed_edges[0]);
        removed_edges.pop_front();
    }
}


template < class G, typename T_V, typename T_E >
int64_t
Pgr_base_graph< G, T_V, T_E >::get_edge_id(V from, V to, double &distance) const {
    E e;
    EO_i out_i, out_end;
    V v_source, v_target;
    double minCost =  std::numeric_limits<double>::max();
    int64_t minEdge = -1;
    for (boost::tie(out_i, out_end) = boost::out_edges(from, graph);
            out_i != out_end; ++out_i) {
        e = *out_i;
        v_target = target(e, graph);
        v_source = source(e, graph);
        if ((from == v_source) && (to == v_target)
                && (distance == graph[e].cost))
            return graph[e].id;
        if ((from == v_source) && (to == v_target)
                && (minCost > graph[e].cost)) {
            minCost = graph[e].cost;
            minEdge = graph[e].id;
        }
    }
    distance = minEdge == -1? 0: minCost;
    return minEdge;
}


template < class G, typename T_V, typename T_E >
void
Pgr_base_graph< G, T_V, T_E >::graph_add_edge(const T_E &edge ) {
    bool inserted;
    typename Pgr_base_graph< G, T_V, T_E >::LI vm_s, vm_t;
    typename Pgr_base_graph< G, T_V, T_E >::E e;

    vm_s = vertices_map.find(edge.source);
    if (vm_s == vertices_map.end()) {
        vertices_map[edge.source]=  m_num_vertices;
        vm_s = vertices_map.find(edge.source);
    }

    vm_t = vertices_map.find(edge.target);
    if (vm_t == vertices_map.end()) {
        vertices_map[edge.target]=  m_num_vertices;
        vm_t = vertices_map.find(edge.target);
    }

    if (edge.cost >= 0) {
        boost::tie(e, inserted) =
            boost::add_edge(vm_s->second, vm_t->second, graph);
        graph[e].cp_members(edge);
    }
}


template < class G, typename T_V, typename T_E >
template < typename T>
void
Pgr_base_graph< G, T_V, T_E >::graph_add_edge(const T &edge) {
    bool inserted;
    typename Pgr_base_graph< G, T_V, T_E >::E e;
    if ((edge.cost < 0) && (edge.reverse_cost < 0))
        return;

    /*
     * true: for source
     * false: for target
     */
    auto vm_s = get_V(T_V(edge, true));
    auto vm_t = get_V(T_V(edge, false));

    pgassert(vertices_map.find(edge.source) != vertices_map.end());
    pgassert(vertices_map.find(edge.target) != vertices_map.end());
    if (edge.cost >= 0) {
        boost::tie(e, inserted) =
            boost::add_edge(vm_s, vm_t, graph);
        graph[e].cost = edge.cost;
        graph[e].id = edge.id;
        graph[e].first = true;
    }

    if (edge.reverse_cost >= 0) {
        boost::tie(e, inserted) =
            boost::add_edge(vm_t, vm_s, graph);

        graph[e].cost = edge.reverse_cost;
        graph[e].id = edge.id;
        graph[e].first = false;
    }
}

/******************  PRIVATE *******************/

template < class G, typename T_V, typename T_E >
void
Pgr_base_graph< G, T_V, T_E >::add_vertices(
        std::vector< T_V > vertices) {
    pgassert(m_num_vertices == 0);
    for (const auto vertex : vertices) {
        pgassert(vertices_map.find(vertex.id) == vertices_map.end());

        auto v =  add_vertex(graph);
        vertices_map[vertex.id] =  m_num_vertices++;
        graph[v].cp_members(vertex);

        pgassert(boost::num_vertices(graph) == num_vertices());
    }
    return;
}

} // namespace graph
}  // namespace pgRouting
