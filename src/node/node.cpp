//
// Created by Adith Ramachandran on 11/30/21.
//

#include "node.hpp"

std::string klik::Node::dump_connections() const
{
    std::ostringstream neighbor_string;

    neighbor_string << "node " << this->id << " is connected to ";

    for (auto n : this->neighbors)
    {
        neighbor_string << n << " ";
    }

    return neighbor_string.str();
}



bool klik::Node::should_infect() const {
    if (is_active) return false;
    int infected_neighbors = 0;

    for (auto neighbor : neighbors)
    {
        if (node_map.at(neighbor).get_active()) infected_neighbors++;
        if (infected_neighbors >= threshold) return true;
    }

    return false;
}

nlohmann::json klik::Node::dump_json() const {
    nlohmann::json ret_val;

    ret_val["id"] = id;
    ret_val["threshold"] = threshold;
    ret_val["neighbors"] = std::vector<klik::uint64>(neighbors.begin(), neighbors.end());
    ret_val["is_active"] = is_active;

    return ret_val;
}