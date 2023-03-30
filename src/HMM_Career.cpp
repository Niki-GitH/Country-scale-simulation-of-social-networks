//#include "pch.h"
#include "../include/HMM_Career.h"

CHMM_Career::CHMM_Career()
{
	n_states = CAREER_STATE_END;
	n_obs = FINCE_OBSV_END;
}

CHMM_Career::CHMM_Career(const std::vector<CareerState_>& states, const std::vector<FinacialObsSymbol>& symbols, const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B, const std::vector<double>& pi)
{
	m_states = states;
	m_symbols = symbols;
	m_A = A;
	m_B = B;
	m_pi = pi;
	n_states = CAREER_STATE_END;
	n_obs = FINCE_OBSV_END;
}

CHMM_Career::CHMM_Career(const std::vector<CareerState_>& states, const std::vector<FinacialObsSymbol>& symbols, const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B)
{
	m_states = states;
	m_symbols = symbols;
	m_A = A;
	m_B = B;
	n_states = CAREER_STATE_END;
	n_obs = FINCE_OBSV_END;
}



void CHMM_Career::set_initial(std::vector<double>& initial_probs) {
	if (initial_probs.size() != n_states) {
		std::cout << "Error: initial probability distribution size does not match number of hidden states." << std::endl;
		return;
	}
	m_pi = initial_probs;
}

void CHMM_Career::set_transition(std::vector<std::vector<double>>& trans_probs) {
	if (trans_probs.size() != n_states || trans_probs[0].size() != n_states) {
		std::cout << "Error: transition probability matrix size does not match number of hidden states." << std::endl;
		return;
	}
	m_A = trans_probs;
}

void CHMM_Career::set_emission(std::vector<std::vector<double>>& emiss_probs) {
	if (emiss_probs.size() != n_states || emiss_probs[0].size() != n_obs) {
		std::cout << "Error: emission probability matrix size does not match number of hidden states or number of observable states." << std::endl;
		return;
	}
	m_B = emiss_probs;
}


std::vector<double> CHMM_Career::forward_algorithm(std::vector<int>& observations) {
	int T = observations.size(); // length of observation sequence
	std::vector<std::vector<double>> alpha(T, std::vector<double>(n_states));
	std::vector<double> c(T);

	// initialize alpha
	for (int i = 0; i < n_states; i++) {
		if (i >= alpha[0].size())
		{
			break;
		}
		if (i >= m_pi.size())
		{
			break;
		}
		if (i >= m_B.size())
		{
			break;
		}
		alpha[0][i] = m_pi[i] * m_B[i][observations[0]];
		c[0] += alpha[0][i];
	}
	c[0] = 1.0 / c[0];
	for (int i = 0; i < n_states; i++) {
		alpha[0][i] *= c[0];
	}

	// perform forward algorithm
	for (int t = 1; t < T; t++) {
		for (int i = 0; i < n_states; i++) {
			double sum = 0;
			for (int j = 0; j < n_states; j++) {
				sum += alpha[t - 1][j] * m_A[j][i];
			}
			alpha[t][i] = sum * m_B[i][observations[t]];
			c[t] += alpha[t][i];
		}
		c[t] = 1.0 / c[t];
		for (int i = 0; i < n_states; i++) {
			alpha[t][i] *= c[t];
		}
	}

	// calculate probability of observation sequence
	double prob = 0;
	for (int t = 0; t < T; t++) {
		prob += log(c[t]);
	}
	prob = -prob;

	return alpha[T - 1]; // return alpha_T
}


std::vector<CareerState_> CHMM_Career::predict(const std::vector<FinacialObsSymbol>& observations, int num_steps)
{
	std::vector<CareerState_> state_sequence(num_steps); // Initialize state sequence
	int num_states = m_states.size();
	int T = observations.size();

	// Initialize delta and psi matrices
	std::vector<std::vector<double>> delta(T, std::vector<double>(num_states, 0));
	std::vector<std::vector<int>> psi(T, std::vector<int>(num_states, 0));

	// Initialization step
	for (int i = 0; i < num_states; i++) {
		delta[0][i] = log(m_pi[i]) + log(m_B[i][observations[0]]);
	}

	// Recursion step
	for (int t = 1; t < T; t++) {
		for (int j = 0; j < num_states; j++) {
			double max_prob = -INFINITY;
			int max_index = 0;
			for (int i = 0; i < num_states; i++) {
				double prob = delta[t - 1][i] + log(m_A[i][j]) + log(m_B[j][observations[t]]);
				if (prob > max_prob) {
					max_prob = prob;
					max_index = i;
				}
			}
			delta[t][j] = max_prob;
			psi[t][j] = max_index;
		}
	}

	// Termination step
	double max_prob = -INFINITY;
	int max_index = 0;
	for (int i = 0; i < num_states; i++) {
		if (delta[T - 1][i] > max_prob) {
			max_prob = delta[T - 1][i];
			max_index = i;
		}
	}
	state_sequence[T - 1] = m_states[max_index];

	// Backtracking step
	for (int t = T - 2; t >= 0; t--) {
		max_index = psi[t + 1][max_index];
		state_sequence[t] = m_states[max_index];
	}

	return state_sequence;
}
