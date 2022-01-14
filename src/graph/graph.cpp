//
// Created by Adith Ramachandran on 12/3/21.
//

#include "graph.hpp"
//#define DEBUG
//#define DEBUG_WONKY

void klik::Graph::generate_graph()
{
    const int n = properties.n;
    const int m = properties.m;
    const std::string d = properties.d;

    std::mutex node_map_lock;

    const auto clique_choice = [&]()
    {
        for (klik::uint64 i = 0; i < TOTAL_NODES; i++)
        {
            node_map.insert({i, Node(i, node_map, properties.d)});
            if (klik::prob_true())
            {
                node_map.at(i).set_active();
                infected_nodes++;
            }
        }

        std::mutex counts_lock;
        int node_counts[TOTAL_NODES];

        for (auto i = 0; i < TOTAL_NODES; i++)
        {
            node_counts[i] = m;
        }

        std::atomic<int> n_threads(m);

        const auto gen_subcliques = [&]()
        {
            std::vector<size_t> indices_to_pick_from(TOTAL_NODES);
            std::iota(
                indices_to_pick_from.begin(),
                indices_to_pick_from.end(),
                0);

            std::vector<std::pair<klik::uint64, klik::uint64>> pairs_to_add;

            std::random_device rd;
            std::mt19937_64 mt(rd());

            for (int i = 0; i < (TOTAL_NODES / n); i++)
            {
                std::unordered_set<klik::uint64> curr_clique;

                for (int j = 0; j < n; j++)
                {
                    while (true)
                    {
                        std::unique_lock<std::mutex> l(counts_lock);
                        std::uniform_int_distribution<int> dist(0, indices_to_pick_from.size() - 1);
                        size_t pick = dist(mt);
                        if (node_counts[indices_to_pick_from[pick]] <= 0)
                        {
                            indices_to_pick_from.erase(indices_to_pick_from.begin() + pick);
                            l.unlock();
                            continue;
                        }

                        if (curr_clique.find(indices_to_pick_from[pick]) == curr_clique.end())
                        {
                            node_counts[indices_to_pick_from[pick]]--;
                            curr_clique.insert(indices_to_pick_from[pick]);
                            l.unlock();
                            break;
                        }
                        else
                        {
                            if (n_threads == 1)
                            {
                                curr_clique.clear();
                                l.unlock();
                                break;
                            }
                        }
                    }
                }

                std::vector<klik::uint64> clique_to_make(curr_clique.begin(), curr_clique.end());

                for (auto j = 0; j < clique_to_make.size(); j++)
                {
                    for (auto k = j + 1; k < clique_to_make.size(); k++)
                    {
                        pairs_to_add.push_back({clique_to_make[j], clique_to_make[k]});
                    }
                }
            }

            n_threads--;

            std::scoped_lock<std::mutex> l(node_map_lock);
            for (auto pair : pairs_to_add)
            {
                node_map.at(pair.first).add_neighbor(pair.second);
                node_map.at(pair.second).add_neighbor(pair.first);
            }
        };

        std::vector<std::thread> ts;

        for (int i = 0; i < m; i++)
        {
            ts.push_back(std::thread(gen_subcliques));
        }

        for (auto i = 0; i < m; i++)
        {
            ts[i].join();
        }
    };

    clique_choice();

    std::cout << "Generated graph with " << node_map.size() << " nodes" << std::endl;
}

void klik::Graph::run_sim()
{
    int curr_infected_nodes;

    std::cout << "Started simulation with " << infected_nodes << " infected nodes" << std::endl;

    std::ofstream json_rep("outputs/output_0.json", std::ofstream::out);
    json_rep << to_string(dump_graph());
    json_rep.close();

    std::mutex node_map_lock;

    int iteration = 1;
    do
    {
        curr_infected_nodes = infected_nodes;

        const int n_threads = 10;
        const auto calc_nodes_to_infect = [&](int thread_id)
        {
            std::vector<klik::uint64> local;
            for (int i = thread_id * (TOTAL_NODES / n_threads); i < ((thread_id + 1) * (TOTAL_NODES / n_threads)); i++)
            {
                if (node_map.at(i).should_infect())
                    local.push_back(i);
            }
            std::scoped_lock<std::mutex> l(node_map_lock);
            for (auto n : local)
            {
                node_map.at(n).set_active();
                infected_nodes++;
            }
        };

        std::vector<std::thread> ts;
        for (int i = 0; i < n_threads; i++)
        {
            ts.push_back(std::thread(calc_nodes_to_infect, i));
        }

        for (int i = 0; i < n_threads; i++)
        {
            ts[i].join();
        }

        // std::ofstream json_rep("outputs/output_" + std::to_string(iteration) + ".json", std::ofstream::out);
        // json_rep << to_string(dump_graph());
        // json_rep.close();
        iteration++;

        std::cout << (double)infected_nodes / node_map.size() << std::endl;
    } while (infected_nodes > curr_infected_nodes);

#ifdef DEBUG_WONKY
    for (auto &[id, node] : node_map)
    {
        if (!node.get_active())
        {
            std::cout << "Wonky" << std::endl;
            std::cout << node.dump_connections() << std::endl;
            std::cout << node.should_infect() << std::endl;
        }
    }
#endif

    std::cout << "Ended simulation with " << infected_nodes << " infected nodes" << std::endl;
}

nlohmann::json klik::Graph::dump_graph() const
{
    nlohmann::json ret_val;

    std::vector<nlohmann::json> jsons;

    for (auto &[id, node] : node_map)
    {
        jsons.push_back(node.dump_json());
    }

    ret_val["nodes"] = jsons;

    return ret_val;
}