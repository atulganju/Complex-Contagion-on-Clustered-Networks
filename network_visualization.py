import sys
import getopt
import plotly.graph_objects as go
import networkx as nx
import json
import matplotlib.pyplot as plt
import numpy as np

help_text = 'network_visualization.py -i <input json> [-n <# nodes> -s <scale>]'


def gen_graph(output, num_nodes):
    G = nx.empty_graph(n=num_nodes)

    color_map = []
    for node in output['nodes'][:num_nodes]:
        if node['is_active']:
            color_map.append('red')
        else:
            color_map.append('black')
        for neighbor in node['neighbors']:
            if neighbor >= num_nodes:
                continue
            G.add_edge(node['id'], neighbor)

    return G, color_map


def simple_depth_init_pos(G, dtree):
    pos = {}
    for node in G.nodes():
        depth = dtree[node]
        pos[node] = (np.random.rand(), depth)
    return pos


def depth_tree(output, num_nodes):
    tree = {0: 0}
    for node in output['nodes'][:num_nodes]:
        id = node['id']
        depth = tree[id] if id in tree else 0
        for neighbor in node['neighbors']:
            if neighbor >= num_nodes:
                continue
            if neighbor in tree and tree[neighbor] < depth+1:
                continue
            tree[neighbor] = depth+1
    return tree


def incdepth_edges(G, dtree):
    edges = list(G.edges())
    valid_edges = []
    for (v, w) in edges:
        if dtree[w] > dtree[v]:
            valid_edges.append((v, w))
    return valid_edges


def color_incdepth_edges(G, dtree, scale):
    edges = list(G.edges())
    colors = []
    weights = []
    for (v, w) in edges:
        if dtree[w] > dtree[v]:
            colors.append('black')
            weights.append(scale)
        else:
            colors.append('grey')
            weights.append(scale * 0.5)
    return colors, weights


def main(argv):
    file = ''
    num_nodes = None
    scale = 1.0
    width = 12
    # print(argv)
    try:
        opts, args = getopt.getopt(argv, 'hi:n:s:w:')
    except getopt.GetoptError:
        print(help_text)
    # print(opts, args)
    for opt, arg in opts:
        if opt == '-h':
            print(help_text)
            return
        if opt in ('-i', '--ifile'):
            file = arg
        elif opt in ('-n', '--nodes'):
            num_nodes = None if int(arg) < 1 else int(arg)
        elif opt in ('-s', '--scale'):
            scale = float(arg)
        elif opt in ('-w', '--width'):
            width = float(arg)

    output = json.load(open(file))

    if num_nodes is None:
        num_nodes = len(output['nodes'])

    dtree = depth_tree(output, num_nodes)
    G, color_map = gen_graph(output, num_nodes)
    valid_edges = incdepth_edges(G, dtree)
    edge_colors, edge_weights = color_incdepth_edges(G, dtree, scale)

    pos = simple_depth_init_pos(G, dtree)
    pos = nx.spring_layout(G, pos=pos)
    plt.figure(figsize=(width, width))
    nx.draw(G,  # pos=pos,  # edgelist=valid_edges,
            edge_color=edge_colors,
            node_color=color_map, node_size=30*scale, width=edge_weights)
    plt.savefig('network.png')
    plt.show()


if __name__ == "__main__":
    main(sys.argv[1:])
