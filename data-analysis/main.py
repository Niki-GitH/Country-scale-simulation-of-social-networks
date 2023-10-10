import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
import random
import numpy as np
import seaborn as sns
import pickle
import joblib
from matplotlib.widgets import Button
import pylab
from Bio import Phylo

POI_Name = ""

class MultilayerNetworkVisualizer:
    def __init__(self, population_data, family_edges_data):
        # Read population data
        self.population_data = pd.read_csv(population_data,low_memory=False)
        self.population_data['UniqueID'] = self.population_data['UniqueID'].str.strip()
        self.population_data['FName'] = self.population_data['FName'].str.strip()
        self.population_data['SchoolsAttended'] = self.population_data['SchoolsAttended'].str.strip()
        self.population_data['OfficeAddress'] = self.population_data['OfficeAddress'].str.strip()
        self.population_data['HomeAddress'] = self.population_data['HomeAddress'].str.strip()

        # Read family edges data
        self.family_edges_data = pd.read_csv(family_edges_data)
        self.family_edges_data['conn_from'] = self.family_edges_data['conn_from'].str.strip()
        self.family_edges_data['conn_to'] = self.family_edges_data['conn_to'].str.strip()
        self.family_edges_data['edge_type'] = self.family_edges_data['edge_type'].str.strip()

        # Create an empty multilayer network
        self.multilayer_network = nx.MultiGraph()

        # Add school connections layer
        school_connections = self.population_data.groupby('SchoolsAttended')['UniqueID'].apply(list)
        for connections in school_connections:
            self.multilayer_network.add_nodes_from(connections, layer='SchoolsAttended')
            self.multilayer_network.add_edges_from(
                [(conn1, conn2, {'layer': 'SchoolsAttended'}) for conn1 in connections for conn2 in connections if
                 conn1 != conn2])

        # Add office connections layer
        office_connections = self.population_data.groupby('OfficeAddress')['UniqueID'].apply(list)
        for connections in office_connections:
            self.multilayer_network.add_nodes_from(connections, layer='OfficeAddress')
            self.multilayer_network.add_edges_from(
                [(conn1, conn2, {'layer': 'OfficeAddress'}) for conn1 in connections for conn2 in connections if
                 conn1 != conn2])

        # Add close family layer
        close_family_edges = self.family_edges_data[
            self.family_edges_data['edge_type'].isin(['husband', 'wife', 'father', 'mother', 'sister', 'brother'])]
        for _, row in close_family_edges.iterrows():
            conn_from = row['conn_from']
            conn_to = row['conn_to']
            self.multilayer_network.add_edge(conn_from, conn_to, layer='CloseFamily')

        # Add extended family layer (using family_edges.csv)
        extended_family_edges = self.family_edges_data[
            ~self.family_edges_data['edge_type'].isin(['husband', 'wife', 'father', 'mother', 'sister', 'brother'])]

        for _, row in extended_family_edges.iterrows():
            conn_from = row['conn_from']  # Update the column name for the source node
            conn_to = row['conn_to']  # Update the column name for the target node
            edge_type = row['edge_type']
            self.multilayer_network.add_edge(conn_from, conn_to, layer='ExtendedFamily')


        # Add local area connections layer
        local_area_connections = self.population_data.groupby('HomeAddress')['UniqueID'].apply(list)
        for connections in local_area_connections:
            self.multilayer_network.add_nodes_from(connections, layer='LocalArea')
            self.multilayer_network.add_edges_from([(conn1, conn2, {'layer': 'LocalArea'}) for conn1 in connections for conn2 in connections if conn1 != conn2])

    def get_connections_list(self, person_of_interest):
        templist = []
        connections_list = []
        #print(f"Connections for {person_of_interest}:")
        for connection in self.multilayer_network.edges(person_of_interest, data=True):
            connection_to = connection[1]
            layer = connection[2]['layer']
            templist.append((connection_to, layer))
        [connections_list.append(x) for x in templist if x not in connections_list]
        #for x in connections_list:
        #    print(f"Connected to {x[0]} in layer: {x[1]}")
        return connections_list, person_of_interest

    def plot_degree_histogram(self, ax=None):
        if ax is None:
            fig, ax = plt.subplots()
        degrees = self.multilayer_network.degree()
        degree_values = [val for _, val in degrees]
        ax.hist(degree_values, bins=20, alpha=0.5, color='b')
        ax.set_xlabel('Degree')
        ax.set_ylabel('Frequency')
        ax.set_title('Degree Histogram')

    def plot_closeness_centrality(self, ax=None):
        if ax is None:
            fig, ax = plt.subplots()
        closeness_centrality = nx.closeness_centrality(self.multilayer_network)
        nodes, values = zip(*closeness_centrality.items())
        ax.bar(nodes, values, color='g', alpha=0.7)
        ax.set_xlabel('Nodes')
        ax.set_ylabel('Closeness Centrality')
        ax.set_title('Closeness Centrality')
        ax.set_xticks(nodes)
        ax.axes.xaxis.set_ticklabels([])
        ax.tick_params(axis='x', rotation=45)  # Use rotation instead of ha for tick labels

    def plot_shortest_path_distance(self, person_of_interest, ax=None):
        if ax is None:
            fig, ax = plt.subplots()
        shortest_path_distances = nx.single_source_shortest_path_length(self.multilayer_network, person_of_interest)
        nodes, values = zip(*shortest_path_distances.items())
        ax.bar(nodes, values, color='r', alpha=0.7)
        ax.set_xlabel('Nodes')
        ax.set_ylabel('Shortest Path Distance')
        ax.set_title('Shortest Path Distance from Person of Interest')
        ax.set_xticks(nodes)
        ax.axes.xaxis.set_ticklabels([])
        ax.tick_params(axis='x', rotation=45)

    def plot_combined_graphs(self, person_of_interest):
        fig, axs = plt.subplots(1, 2, figsize=(15, 5))

        self.plot_degree_histogram(axs[0])
        self.plot_closeness_centrality(axs[1])
        #self.plot_shortest_path_distance(person_of_interest, axs[2])

        plt.tight_layout()
        plt.title('combined graph plot')
        plt.show()
        plt.savefig("Degree-Closeness.png")

    def draw_connections(self, person_of_interest, connections_list):
        plt.figure(figsize=(12, 8))

        # Create a subgraph containing only the nodes and edges connected to the person of interest
        subgraph_nodes = [person_of_interest] + [node for node, _ in connections_list]
        subgraph = self.multilayer_network.subgraph(subgraph_nodes)

        # Compute positions of nodes using spring layout
        #pos = nx.spring_layout(subgraph, seed=42)
        pos = nx.fruchterman_reingold_layout(subgraph)


        # Draw connections from the person of interest to other nodes
        for connection_to, layer in connections_list:
            if connection_to in pos:
                x = [pos[person_of_interest][0], pos[connection_to][0]]
                y = [pos[person_of_interest][1], pos[connection_to][1]]
                color = 'red' if connection_to == person_of_interest else layer_color[layer]
                plt.plot(x, y, color=color, linestyle='dashed', alpha=0.4)

        # Update node_color dictionary with UniqueIDs as keys
        node_color = {}
        for node, layer in connections_list:
            node_color[node] = layer_color.get(layer, 'gray')  # Default color for unknown layers

        # Draw nodes (circles) with names and colors based on their layers
        rng = np.random.RandomState(0)
        colors = rng.rand(100)
        circle_radius = 0.03
        for node, (x, y) in pos.items():
            if node != person_of_interest:  # Skip the person of interest circle
                circle_color = node_color[node] if node in node_color else 'gray'  # Default color for unknown layers
                plt.scatter(x, y, s=100, color=circle_color, marker='o', edgecolors=circle_color, alpha=0.5)
                pname = self.population_data.loc[self.population_data['UniqueID'] == node, 'FName']
                plt.text(x, y - circle_radius, str(pname.values[0]), ha='center', va='center', fontsize=10)

        # Draw person of interest
        if person_of_interest in pos:
            plt.scatter(pos[person_of_interest][0], pos[person_of_interest][1], s=200, color='red', alpha=0.8, marker='o', edgecolors='red')
            plt.text(pos[person_of_interest][0], pos[person_of_interest][1], POI_Name, color='blue', ha='center', va='center', fontsize=10)

        # Add a legend for the layers
        legend_handles = []
        for layer, color in layer_color.items():
            legend_handles.append(
                plt.Line2D([0], [0], marker='o', color='w', label=layer, markerfacecolor=color, markersize=10))
        legend_handles.append(
            plt.Line2D([0], [0], marker='o', color='w', label="Person of Interest", markerfacecolor='red', markersize=10))
        #plt.legend(handles=legend_handles,bbox_to_anchor=(1.3, 1.0), loc='upper left')
        plt.legend(handles=legend_handles, loc=0, frameon=True)

        # Add a refresh button
        #refresh_button_ax = plt.axes([0.81, 0.02, 0.1, 0.05])  # [left, bottom, width, height]
        #refresh_button = Button(refresh_button_ax, 'Refresh', color='lightgray', hovercolor='lightblue')
        #refresh_button.on_clicked(self.on_refresh_button_clicked)

        plt.title("Individual's connection's from all layers")
        plt.tight_layout()
        plt.axis('off')
        plt.show()
        plt.savefig("Connections.png")

    def get_connections_list_depth(self, person_of_interest, visited_list, depth=1, current_depth=0):
        if current_depth >= depth:
            return []

        connections_list_depth = []

        if current_depth == 0:
            #connections_list_depth, person_of_interest = self.get_connections_list(person_of_interest)
            connections_list_depth.append((person_of_interest, 'Person of Interest', self.population_data.loc[self.population_data['UniqueID'] == person_of_interest, 'FName'].values[0]))

        for node, connection_data in self.multilayer_network[person_of_interest].items():
            for connection in connection_data.values():
                layer = connection['layer']
                if node != person_of_interest:
                    connections_list_depth.append((node, layer, self.population_data.loc[self.population_data['UniqueID'] == node, 'FName'].values[0] ))

        if current_depth >= 0 :
            for conne in connections_list_depth:
                if conne[0] not in visited_list:
                    visited_list.append(conne[0])
                    connections_list_depth.extend(self.get_connections_list_depth(conne[0], visited_list,depth, current_depth + 1))

        return connections_list_depth


    def draw_connections_depth(self, person_of_interest, depth=1):
        # Constants
        circle_radius = 0.03
        # Define a circular layout for visualization
        #pos = nx.spring_layout(self.multilayer_network)
        pos = nx.fruchterman_reingold_layout(self.multilayer_network)

        # Get the connections list for the person of interest
        visited_list = []
        visited_list.append(person_of_interest)
        connections_list = self.get_connections_list_depth(person_of_interest,visited_list, depth)

        # Draw the person of interest in red color
        plt.scatter(pos[person_of_interest][0], pos[person_of_interest][1], s=200, color='red', marker='o',
                    edgecolors='red', alpha=0.8)
        plt.style.use('ggplot')
        plt.text(pos[person_of_interest][0], pos[person_of_interest][1], person_of_interest, ha='center', va='center',
                 fontsize=10)

        # Initialize a set to keep track of visited nodes to avoid duplicates
        visited = set()

        # Draw circles and connections for each level of depth
        for node, layer_name, name in connections_list:
            if node in pos:  # Check if the node exists in the circular layout positions
                circle_color = layer_color.get(layer_name, 'gray')  # Default color for unknown layers
                x, y = pos[node]
                plt.scatter(x, y, s=200, color=circle_color, marker='o', edgecolors=circle_color, alpha=0.5)
                pname = self.population_data.loc[self.population_data['UniqueID'] == node, 'FName']
                plt.text(x, y - circle_radius - 0.02, str(pname.values[0]), ha='center', va='center', fontsize=10)

                # Draw a dotted line connection to the connected person
                plt.plot([pos[person_of_interest][0], x], [pos[person_of_interest][1], y], linestyle='dotted',
                         color=circle_color)

                visited.add(node)

        # Set the axis properties and display the visualization
        plt.axis('off')
        plt.show()

    def on_refresh_button_clicked(self, event):
        # Get a new random person from the population data for visualization
        person_of_interest = random.choice(list(self.multilayer_network.nodes()))

        # Clear the current plot and draw the new connections for the new person
        plt.clf()
        connections_list, person_of_interest = self.get_connections_list(person_of_interest)
        self.draw_connections(person_of_interest, connections_list)

        # Draw the connections for the new person and their connections at depth 2
        #self.draw_connections_depth(person_of_interest, depth=2)


    def plot_categorical_plot(self):
        # Clean the 'edge_type' values by removing leading and trailing spaces
        self.family_edges_data['edge_type'] = self.family_edges_data['edge_type'].str.strip()

        # Convert the 'Age' column to numeric data in the population_data
        self.population_data['Age'] = pd.to_numeric(self.population_data['Age'], errors='coerce')

        #data_to_plot = self.population_data[['Age', 'Gender']].dropna()
        #data_to_plot['Age'] = pd.to_numeric(data_to_plot['Age'], errors='coerce')
        #data_to_plot.dropna(subset=['Age'], inplace=True)

        # Plot the categorical plot
        sns.catplot(x='Age', hue='Gender', col='Gender', data=self.population_data, kind='count')
        plt.title('categorical plot')
        plt.show()

    def plot_edge_type_statistics(self):
        plt.figure(figsize=(8, 6))
        sns.countplot(y='edge_type', data=self.family_edges_data, palette='Set3')
        plt.ylabel('Edge Type')
        plt.xlabel('Count')
        plt.title('Edge Types statistics')
        plt.xticks(rotation=45)
        plt.show()
        plt.savefig("Edge-Type-Statistics.png")

    def plot_age_gender_distribution(self):
        sns.set_theme(style="whitegrid", palette="muted")
        # Draw a categorical scatterplot to show each observation
        ax = sns.swarmplot(data=self.population_data, x="Age", y="Gender", hue="Gender")
        ax.set(ylabel="")
        plt.title("Age and Gender distribution")
        plt.show()
        plt.savefig("Age-Gender-distribution.png")

    def plot_joint_kernel_density_estimate(self):
        # Check if 'Age' column exists in 'population_data'
        if 'Age' not in self.population_data.columns:
            raise ValueError("The 'Age' column does not exist in the 'population_data' DataFrame.")

        # Calculate the age distribution for each layer
        layer_age_distributions = {}
        for node, data in self.multilayer_network.nodes(data=True):
            layer = data['layer']
            print(layer)
            age = self.population_data.loc[
                self.population_data['UniqueID'] == node, 'Age'].values[0]
            #age = self.population_data.loc[node, 'Age']
            if layer not in layer_age_distributions:
                layer_age_distributions[layer] = []
            layer_age_distributions[layer].append(age)

        # Plot the KDE for each layer
        for layer, age_data in layer_age_distributions.items():
            sns.kdeplot(data=age_data, fill=True, label=layer, common_norm=False, common_grid=False)
        plt.legend()
        plt.xlabel('Age')
        plt.ylabel('Density')
        plt.title('Kernel Density Estimate by Layer')
        plt.show()

    def plot_age_distribution(self):
        sns.set_theme(style="whitegrid")
        cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
        g = sns.relplot(
            data=self.population_data,
            x="Age", y="DOB",
            hue="Age", size="Age",
            palette=cmap, sizes=(10, 100),
        )
        g.set( yscale="log")
        g.set(yticklabels=[])
        g.ax.xaxis.grid(True, "minor", linewidth=.25)
        g.ax.yaxis.grid(True, "minor", linewidth=.25)
        g.despine(left=True, bottom=True)
        plt.title("Age distribution")
        plt.show()
        plt.savefig("age-distribution.png")

    def plot_paragrid_for_population_data(self):
        sns.set_theme(style="white")
        newDF = self.population_data[
            ['UniqueID', 'Age', 'DOB', 'Gender', 'EducationLevel', 'MaritalStatus', 'SpouseName', 'NumOfChild',
             'IsEmployed']].copy()
        g = sns.PairGrid(newDF, diag_sharey=False)
        g.map_upper(sns.scatterplot, s=15)
        g.map_lower(sns.kdeplot)
        g.map_diag(sns.kdeplot, lw=2)
        plt.title("Population paragrid")
        plt.show()
        plt.savefig("Paragrid.png")


def visualize_hmm(model_file):
    with open(model_file, 'r') as f:
        lines = f.readlines()

    hidden_states_size = int(lines[0].split(':')[1].strip())
    age_range_size = int(lines[1].split(':')[1].strip())
    financial_status_size = int(lines[2].split(':')[1].strip())

    hidden_states = lines[3].split(':')[1].strip().split(', ')
    age_range = [int(age) for age in lines[4].split(':')[1].strip().split(', ')[:-1]]  # Fix here
    financial_status = lines[5].split(':')[1].strip().split(', ')

    transition_probs = []
    for i in range(hidden_states_size):
        row = [float(x.strip()) for x in lines[8 + i].split(',')[1:-1]]
        transition_probs.append(row)

    emission_probs = np.zeros((hidden_states_size, age_range_size, financial_status_size))
    for i in range(hidden_states_size):
        j = 0
        for line in lines[8 + hidden_states_size + i * (age_range_size * financial_status_size):(
                8 + hidden_states_size + (i + 1) * (age_range_size * financial_status_size))]:
            if line.strip() == "Emission probabilty matrix : $":  # Skip the header line
                continue
            if line.strip() == "Init probabilty matrix : $":  # Stop when reaching the next section
                break
            age, financial_status, prob = line.strip().split(', ')
            age_index = age_range.index(int(age))
            financial_status_index = financial_status.index(financial_status)
            emission_probs[i, age_index, financial_status_index] = float(prob)

    fig, axes = plt.subplots(1, 2, figsize=(12, 6))

    # Transition Probabilities Heatmap
    axes[0].imshow(transition_probs, cmap='viridis')
    axes[0].set_xticks(np.arange(len(hidden_states)))
    axes[0].set_yticks(np.arange(len(hidden_states)))
    axes[0].set_xticklabels(hidden_states)
    axes[0].set_yticklabels(hidden_states)
    axes[0].set_xlabel('To State')
    axes[0].set_ylabel('From State')
    axes[0].set_title('Transition Probabilities')
    for i in range(len(hidden_states)):
        for j in range(len(hidden_states)):
            axes[0].text(j, i, f'{transition_probs[i, j]:.2f}', ha='center', va='center', color='w')

    # Emission Probabilities Heatmap
    for i, state in enumerate(hidden_states):
        axes[1].imshow(emission_probs[i], cmap='viridis')
        axes[1].set_xticks(np.arange(len(financial_status)))
        axes[1].set_yticks(np.arange(len(age_range)))
        axes[1].set_xticklabels(financial_status)
        axes[1].set_yticklabels(age_range)
        axes[1].set_xlabel('Financial Status')
        axes[1].set_ylabel('Age')
        axes[1].set_title(f'Emission Probabilities for State: {state}')
        for k in range(len(age_range)):
            for l in range(len(financial_status)):
                axes[1].text(l, k, f'{emission_probs[i, k, l]:.2f}', ha='center', va='center', color='w')

    plt.tight_layout()
    plt.show()

def plot_HMM_model(modelpath):
    # Read the Newick tree from the .nwk file
    #tree_ = Phylo.parse(modelpath, 'newick')
    # Write the tree to the XML file
    #Phylo.write(tree_, "out.xml", 'phyloxml')

    trees = Phylo.parse("out.xml", "phyloxml")  # Returns a generator, not a single tree
    for tree in trees:
        # Set the figure size (adjust as needed)
        plt.figure(figsize=(8, 8))

        # Plot the tree in circular format
        Phylo.draw(tree, do_show=False, show_confidence=False, branch_labels=None)

        # Show the plot
        plt.show()
        #net = Phylo.to_networkx(tree)
        #nx.draw(net)
        #pylab.show()


if __name__ == '__main__':
    #plot_HMM_model("out.xml")
    #exit
    # Create an instance of the MultilayerNetworkVisualizer class with your data
    visualizer = MultilayerNetworkVisualizer('1k/population_database.csv', '1k/family_edges.csv')

    # Get a random person from the population data for visualization
    person_of_interest = random.choice(list(visualizer.multilayer_network.nodes()))

    POI_Name = str(visualizer.population_data.loc[visualizer.population_data['UniqueID'] == person_of_interest, 'FName'].values[0])

    # Define colors for each layer
    #'#47a0b3', '#a2d9a4', '#edf8a3', '#fee999', '#fca55d', '#e2514a'
    layer_color = {
        'SchoolsAttended': '#4D648D',
        'OfficeAddress': '#FB6542',
        'CloseFamily': '#E11EA9',
        'ExtendedFamily': '#E1B71E',
        'LocalArea': '#34A853'
    }


    # Get the connections list and person_of_interest for the person of interest
    connections_list, person_of_interest = visualizer.get_connections_list(person_of_interest)
    visualizer.draw_connections(person_of_interest, connections_list)

    # Draw the connections
    visualizer.plot_age_distribution()
    visualizer.plot_age_gender_distribution()
    visualizer.plot_paragrid_for_population_data()
    visualizer.plot_edge_type_statistics()
    visualizer.plot_combined_graphs(person_of_interest)

    #----------------------------------------------

    #visualize_hmm("HMMEvents.model")
    visualizer.plot_joint_kernel_density_estimate()
    # Draw the connections for the person of interest and their connections at depth 2
    #visualizer.draw_connections_depth(person_of_interest, depth=2)
