import sys
import os
import getopt

from numpy.core.numeric import full
import plotly.graph_objects as go
import networkx as nx
import json
import matplotlib.pyplot as plt
import numpy as np

help_text = 'network_visualization.py -i <input json> [-n <# nodes> -s <scale>]'


def gen_graph(output, valid_nodes):
    # G = nx.empty_graph(n=len(valid_nodes))
    G = nx.Graph()
    for node in np.array(output['nodes'])[valid_nodes]:
        G.add_node(node['id'])

    color_map = []
    for node in np.array(output['nodes'])[valid_nodes]:
        if node['is_active']:
            color_map.append('red')
        else:
            color_map.append('black')
        for neighbor in node['neighbors']:
            if not neighbor in valid_nodes:
                continue
            G.add_edge(node['id'], neighbor)

    return G, color_map


def recolor_nodes(output, valid_nodes):
    return ['red' if node['is_active'] else 'black' for node in np.array(output['nodes'])[valid_nodes]]


def simple_depth_init_pos(G, dtree):
    pos = {}
    for node in G.nodes():
        depth = dtree[node]
        pos[node] = (np.random.rand(), depth)
    return pos


def position_nicely(output_jsons, dtree):
    output = output_jsons[0]
    max_depth = max(dtree.values())
    valid_nodes = [n for n, d in dtree.items() if d <= 2]
    G, _ = gen_graph(output, valid_nodes)
    full_pos = nx.spring_layout(G)
    for depth in range(2, max_depth+1):
        valid_nodes = [n for n, d in dtree.items() if d <= depth]
        for node in np.array(output['nodes'])[valid_nodes]:
            for neighbor in node['neighbors']:
                if not neighbor in valid_nodes:
                    continue
                # G.add_edge(node['id'], neighbor)
                if not neighbor in full_pos:
                    diff = [(1.5 + 0.5 * np.random.rand()) * (v - w)
                            for v, w in zip(full_pos[node['id']], full_pos[0])]
                    full_pos[neighbor] = np.array(diff)
        G, _ = gen_graph(output, valid_nodes)
        full_pos = nx.spring_layout(G, pos=full_pos)
    return full_pos


def depth_tree(output, valid_nodes):
    tree = {0: 0}
    for node in np.array(output['nodes'])[valid_nodes]:
        id = node['id']
        depth = tree[id]
        for neighbor in node['neighbors']:
            if not neighbor in valid_nodes:
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


def main():
    path = 'src/outputs/'
    scale = 0.5
    width = 6

    files = []
    for filename in os.listdir(path):
        if filename.endswith(".json"):
            files.append(filename)
    if len(files) == 0:
        print('No files found in directory', path)
        return
    output_jsons = [json.load(open(path + file)) for file in files]
    sorted_jsons = []
    for output in output_jsons:
        nodes = output['nodes']
        nodes.sort(key=lambda x: x['id'])
        sorted_jsons.append({'nodes': nodes})
    output_jsons = sorted_jsons
    print(output_jsons[0])

    for filename in os.listdir('visualizations/'):
        os.remove('visualizations/' + filename)

    valid_nodes = list(range(len(output_jsons[0]['nodes'])))
    # print(valid_nodes)
    # print(len(valid_nodes))
    dtree = depth_tree(output_jsons[0], valid_nodes)
    max_depth = max(dtree.values())
    # print(dtree.items())
    valid_nodes = [n for n, d in dtree.items() if d < max_depth]
    # print(len(valid_nodes))
    # print(valid_nodes)
    dtree = depth_tree(output_jsons[0], valid_nodes)
    # print(len(dtree.items()))

    G, color_map = gen_graph(output_jsons[0], valid_nodes)
    valid_edges = incdepth_edges(G, dtree)
    edge_colors, edge_weights = color_incdepth_edges(G, dtree, scale)

    # pos = simple_depth_init_pos(G, dtree)
    pos = nx.spring_layout(G)
    print(pos)
    pos = position_nicely(output_jsons, dtree)

    for i, output in enumerate(output_jsons[:-1]):
        color_map = recolor_nodes(output, valid_nodes)
        plt.figure(figsize=(width, width))
        print(len(color_map), len(edge_colors), len(edge_weights))
        nx.draw(G,  pos=pos,  # edgelist=valid_edges,
                edge_color=edge_colors,
                node_color=color_map,
                node_size=30, width=edge_weights)
        plt.savefig('visualizations/network_'+str(i)+'.png')
        # plt.show()


if __name__ == "__main__":
    main()
