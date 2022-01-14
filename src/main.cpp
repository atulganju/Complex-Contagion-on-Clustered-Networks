#include "global_defs.hpp"
#include "graph/graph.hpp"
#include <fstream>
//#define DEBUG

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cout << "Invalid args" << std::endl;
        std::cout << "Expected format: ./run_sim -n=[clique size] -m=[number of cliques] -d=[threshold distribution]"
                  << std::endl;
        return -1;
    }

    int n, m;
    std::string d;

    try
    {
        std::string n_string(argv[1]);
        std::string m_string(argv[2]);
        std::string d_string(argv[3]);

        n = std::stoi(n_string.substr(3));
        m = std::stoi(m_string.substr(3));
        d = d_string.substr(3);
        if (d != "b" && d != "u" && d != "p")
        {
            throw std::invalid_argument("invalid distribution type");
        }
    }
    catch (std::invalid_argument)
    {
        std::cout << "Invalid args" << std::endl;
        std::cout << "Expected format: ./run_sim -n=[clique size] -m=[number of cliques] -d=[threshold distribution]"
                  << std::endl;
        return -1;
    }

    srand(0);

    std::unordered_map<klik::uint64, klik::Node> node_map;

    klik::Graph g(node_map, n, m, d);

#ifdef DEBUG

    std::cout << "Dumping infected nodes" << std::endl;
    for (auto &[id, node] : node_map)
    {
        if (node.get_active())
            std::cout << id << ", ";
    }
    std::cout << std::endl;

#ifdef DUMP_CONNECTIONS
    for (auto &[id, node] : node_map)
    {
        std::cout << node.dump_connections() << std::endl;
    }
#endif
#endif

    g.run_sim();

    std::ofstream json_rep("json_output.json", std::ofstream::out);
    json_rep << to_string(g.dump_graph());
    json_rep.close();

    return 0;
}