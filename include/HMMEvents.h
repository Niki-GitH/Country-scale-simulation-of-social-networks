#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include "Util.h"

class Observation {
public:
	int age;
	std::string financial_status;
};

// Define a struct to represent the emission probabilities
struct EmissionProb {
	std::unordered_map<std::string, double> probs;
};

class CHMMEvents {
public:
	// Constructor to initialize the CHMMEvents with the given parameters
	CHMMEvents(std::vector<std::string> hidden_states, std::vector<int> age_range,
		std::vector<std::string> financial_status);

	// Compute the most likely sequence of hidden states given the observations using the Viterbi algorithm
	std::vector<std::string> predict(std::vector<Observation>& observations);


	/*    
	1. Initialize the emission probabilities, transition probabilities, and initial probabilities.
    2. Loop over the training data and perform the following steps for each training example:
       a. Compute the forward and backward probabilities.
       b. Compute the posterior probabilities.
       c. Compute the joint probabilities.
       d. Update the transition probabilities and emission probabilities.
       e. Update the counts for each transition and emission.
    3. Update the initial probabilities and transition probabilities based on the counts.
    4. Repeat step 2 and step 3 for the specified number of iterations.
	*/
	void train(const std::vector<std::pair<std::vector<std::pair<int, std::string>>, std::vector<std::string>>>& training_data, int num_iterations);

	void savemodel();

	void readmodel(std::string modelname);
private:
	

	std::vector<std::string> hidden_states_; // list of hidden states
	std::vector<int> age_range_; // range of values for age
	std::vector<std::string> financial_status_; // list of possible financial statuses
	std::unordered_map<std::string, int> state_to_index_; // mapping of state names to indices
	std::unordered_map<int, int> age_to_index_; // mapping of age values to indices
	std::unordered_map<std::string, int> finstatus_to_index_; // mapping of financial status names to indices
	std::vector<EmissionProb> emit_probs_; // emission probabilities
	std::unordered_map<std::string, double> init_probs_;
	std::vector<Observation> observation_space_;
	std::unordered_map<std::string, std::unordered_map<std::string, double>> trans_probs_;

	// Get the index of the given hidden state
	int getStateIndex(const std::string& state) const;

	// Get the index of the given age value
	int getAgeIndex(int age) const;

	// Get the index of the given financial status
	int getStatusIndex(const std::string& status) const;

	// Compute the emission probabilities for the given observation
	double computeObsProb(const std::string& state, const Observation& obs) const;

	double computeObsSeqProb(const std::vector<Observation>& obs_sequence) const;

	// Compute the forward probabilities for the given sequence of observations
	void computeForwardProbs(const std::vector<Observation>& obs_sequence, std::vector<std::vector<double>>& forward_probs) const;

	// Compute the backward probabilities for the given sequence of observations
	void computeBackwardProbs(const std::vector<Observation>& obs_sequence, std::vector<std::vector<double>>& backward_probs) const;

	// Compute the most likely sequence of hidden states given the forward and backward probabilities using the Viterbi algorithm
	std::vector<std::string> computeViterbiPath(const std::vector<std::vector<double>>& forward_probs,
		const std::vector<std::vector<double>>& backward_probs);

	double logSumExp(std::vector<double> x) const;
	
	//compute gamma
	std::vector<std::vector<double>> compute_posterior_probabilities(const std::vector<std::vector<double>>& alpha,	const std::vector<std::vector<double>>& beta) const;
	//compute xi
	std::vector<std::vector<std::vector<double>>> compute_joint_probabilities(const std::vector<Observation>& obs, const std::vector<std::vector<double>>& alpha, const std::vector<std::vector<double>>& beta);
};

