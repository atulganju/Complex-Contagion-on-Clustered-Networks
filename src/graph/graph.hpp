//
// Created by Adith Ramachandran on 11/30/21.
//

#pragma once

#include "../node/node.hpp"
#include "../global_defs.hpp"
#include "../json/json.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <numeric>

namespace klik
{
    struct graph_properties
    {
        int n;
        int m;
        std::string d;
    };

    typedef struct graph_properties graph_properties_t;

    class Graph
    {
    public:
        Graph(std::unordered_map<klik::uint64, klik::Node> &node_map,
              const int n,
              const int m,
              const std::string d) : node_map(node_map)
        {
            properties.n = n;
            properties.m = m;
            properties.d = d;
            infected_nodes = 0;
            generate_graph();
        }

        int get_infected_nodes() const { return infected_nodes; };

        void run_sim();

        nlohmann::json dump_graph() const;

    private:
        /*  members  */
        std::unordered_map<klik::uint64, klik::Node> &node_map;
        graph_properties_t properties;
        std::atomic<int> infected_nodes;

        /*  functions  */
        void generate_graph();
    };
}
