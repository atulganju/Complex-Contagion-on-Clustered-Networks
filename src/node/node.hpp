//
// Created by Adith Ramachandran on 11/30/21.
//

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include "../global_defs.hpp"
#include "../json/json.hpp"

namespace klik
{
    class Node
    {
    public:
        Node(const klik::uint64 id,
             std::unordered_map<klik::uint64, Node> &node_map,
             const std::string d) : id(id),
                                    node_map(node_map),
                                    threshold(klik::rand_minmax(d))
        {
            is_active = false;
        }

        void add_neighbor(klik::uint64 new_neighbor) { neighbors.insert(new_neighbor); };
        bool has_neighbor(klik::uint64 possible_neighbor) const { return neighbors.find(possible_neighbor) != neighbors.end(); };

        void set_active() { is_active = true; };
        bool get_active() const { return is_active; };

        bool should_infect() const;

        std::string dump_connections() const;
        nlohmann::json dump_json() const;

    private:
        const klik::uint64 id;
        const int threshold;
        bool is_active;

        std::unordered_map<klik::uint64, Node> &node_map;
        std::unordered_set<klik::uint64> neighbors;
    };
}
