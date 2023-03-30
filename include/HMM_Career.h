#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include "defines.h"

class CHMM_Career {
private:
	int n_states; // number of hidden states
	int n_obs; // number of observable states
	std::vector<CareerState_> m_states;           // Vector of possible states
	std::vector<FinacialObsSymbol> m_symbols;         // Vector of possible observation symbols
	std::vector<std::vector<double>> m_A;       // Transition matrix
	std::vector<std::vector<double>> m_B;       // Emission matrix
	std::vector<double> m_pi;              // Initial state probabilities [initial probability distribution]

public:
	CHMM_Career();

	// Constructor that initializes the CHMM_Career with given parameters
	CHMM_Career(const std::vector<CareerState_>& states, const std::vector<FinacialObsSymbol>& symbols, const std::vector<std::vector<double>>& A,
		const std::vector<std::vector<double>>& B, const std::vector<double>& pi);

	CHMM_Career(const std::vector<CareerState_>& states, const std::vector<FinacialObsSymbol>& symbols, const std::vector<std::vector<double>>& A,
		const std::vector<std::vector<double>>& B);

	// Function that predicts the most likely sequence of states given a sequence of observations
	std::vector<CareerState_> predict(const std::vector<FinacialObsSymbol>& observations, int num_steps);


	void set_initial(std::vector<double>& initial_probs);
	void set_transition(std::vector<std::vector<double>>& trans_probs);
	void set_emission(std::vector<std::vector<double>>& emiss_probs);

	std::vector<double> forward_algorithm(std::vector<int>& observations);
};
/*
      // Define the states
  std::vector<CareerState_> states = {WORKING, STUDYING};

  // Define the observations
  std::vector<FinacialObsSymbol> observations = {LOW, MEDIUM, HIGH};

  // Define the transition matrix
  std::vector<std::vector<double>> transition_matrix = {
	{0.9, 0.1},
	{0.5, 0.5}
  };

  // Define the emission matrix
  std::vector<std::vector<double>> emission_matrix = {
	{0.1, 0.4, 0.5},
	{0.3, 0.4, 0.3}
  };

   std::vector<double> pi = {0.4,0.6};
  // Create an instance of the HMM class
  CHMM_Career hmm(states, observations, transition_matrix, emission_matrix,pi);

  // Predict the probability of going to higher education after working for 3 years
  //std::vector<int> sequence = {HIGH, HIGH, MEDIUM};
  std::vector<int> sequence = {LOW, LOW, LOW};
  //double probability = hmm.forward_algorithm(sequence)[STUDYING];
  std::vector<double> db = hmm.forward_algorithm(sequence);
  for(int i=0;i<db.size();++i)
  {
    std::cout << "The probability of going to higher education after working for 3 years is " << db[i] << std::endl;  
  }
  
  std::vector<FinacialObsSymbol> sequence1 = {LOW, LOW, MEDIUM};
  std::vector<CareerState_> res = hmm.predict(sequence1,3);
  for(int i=0;i<res.size();++i)
  {
    std::cout << "The probability of going to higher education after working for 3 years is " << res[i] << std::endl;  
  }
  
*/