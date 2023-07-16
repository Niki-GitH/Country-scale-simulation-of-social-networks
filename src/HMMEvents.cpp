#include "../include/HMMEvents.h"
#include <algorithm>
#include <fstream>
#include <string.h>

CHMMEvents::CHMMEvents(std::vector<std::string> hidden_states, std::vector<int> age_range, std::vector<std::string> financial_status)
{
	hidden_states_ = hidden_states;
	age_range_ = age_range;
	financial_status_ = financial_status;
	
	// Initialize the observation space
	for (int age : age_range_) {
		for (const std::string& status : financial_status_) {
			Observation obs = { age, status };
			observation_space_.push_back(obs);
		}
	}

	// Initialize the transition probabilities to 0.0
	for (int i = 0; i < hidden_states_.size();++i) {
		for (int to_state = 0; to_state < hidden_states_.size(); ++to_state) {
			//trans_probs_[i][to_state] = 0.0;
		}
	}
	

	// Initialize the mapping of state names to indices
	for (int i = 0; i < hidden_states.size(); i++) {
		state_to_index_[hidden_states[i]] = i;
	}

	// Initialize the mapping of age values to indices
	for (int i = 0; i < age_range.size(); i++) {
		age_to_index_[age_range[i]] = i;
	}

	// Initialize the mapping of financial status names to indices
	for (int i = 0; i < financial_status.size(); i++) {
		//finstatus_to_index_[financial_status[i]] = i;
	}

	//"GRADUATION", "NEW_CHILD", "MARRIAGE", "HOME_PURCHASE", "NEW_JOB", "DEATH"
	std::vector<std::vector<double>> transition_matrixLife = {
	  {0.1, 0.05, 0.14, 0.01, 0.6, 0.1},
	  {0.0, 0.0,  0.5,  0.3,  0.2, 0.0},
	  {0.1, 0.4,  0.0,  0.2,  0.2, 0.1},
	  {0.0, 0.5,  0.3,  0.005,0.1, 0.095},
	  {0.005, 0.3,  0.3,  0.3,  0.0, 0.095},
	  {0.0, 0.0,  0.0,  0.00, 0.0, 0.0}
	};
	
	//age_0_14,age_15_24,age_25_54,age_55_64,age_65_above
	std::vector<std::vector<double>> emission_matrixage = {
	  {0.9, 0.6,    0.1, 0.0,  0.0},
	  {0.0, 0.0001, 0.2, 0.1,  0.0},
	  {0.0, 0.05,   0.2, 0.05, 0.0005},
	  {0.0, 0.1,    0.2, 0.4,  0.4},
	  {0.0, 0.2499, 0.2, 0.005,0.0},
	  {0.1, 0.1,    0.1, 0.445,  0.5995}
	};

	//"POOR", "LOWER_MIDDLE", "MIDDLE", "UPPER_MIDDLE", "RICH", "ULTRA_RICH"
	std::vector<std::vector<double>> emission_matrixFsts = {
	  {0.3,   0.25,  0.35, 0.4, 0.2, 0.05},
	  {0.1,   0.1,   0.08, 0.15, 0.2, 0.25},
	  {0.1,   0.2,   0.15, 0.15, 0.2, 0.25},
	  {0.001, 0.01,  0.05, 0.1, 0.2, 0.3},
	  {0.3,   0.3,   0.27, 0.1, 0.1, 0.05},
	  {0.199, 0.14,  0.1, 0.1, 0.1, 0.1}
	};

	//trans_probs_
	int i = 0;
	for (auto r : transition_matrixLife)
	{
		int j = 0;
		for (auto c : r)
		{
			trans_probs_[hidden_states_[i]][hidden_states_[j]] = c;
			j++;
		}
		i++;
	}
	
	// Initialize emission probabilities
	i = 0;
	for (const std::string& state : hidden_states_) {
		EmissionProb ep;
		int j = 0;
		
		for (const std::string& status : financial_status_) {
			for (int age : age_range_) {
				Observation obs{ age, status };
				double ageProb = 0.0;
				if (age < 18)
				{
					if(age<15)
					{
						ageProb = emission_matrixage[i][0];
					}
					else
					{
						ageProb = emission_matrixage[i][1];
					}
				}
				else if (age >= 18 && age < 26)
				{
					ageProb = emission_matrixage[i][2];
					if(age<25)
					{
						ageProb = emission_matrixage[i][1];
					}
				}
				else if (age > 25 && age <= 55)
				{
					ageProb = emission_matrixage[i][2];
				}
				else if (age > 55 && age <= 65)
				{
					ageProb = emission_matrixage[i][3];
				}
				else
				{
					ageProb = emission_matrixage[i][4];
				}
				double finaprob = emission_matrixFsts[i][j];
				double obs_prob = ageProb * finaprob;
				if((ageProb==0.0)||(finaprob==0.0))
				{
					obs_prob=ageProb+finaprob;
				}
				ep.probs[status + "_" + std::to_string(age)] = obs_prob;
			}
			j++;
		}
		emit_probs_.push_back(ep);
		i++;
	}

	// Initialize observation space
	for (const std::string& status : financial_status_) {
		for (int age : age_range_) {
			Observation obs{ age, status };
			observation_space_.push_back(obs);
		}
	}

	// Initialize initial probabilities
	double dval = 1.0 / hidden_states_.size();
	for (int i = 0; i < hidden_states_.size(); i++) {
		init_probs_[hidden_states_[i]] = dval;
	}

	std::vector<double> pi = { 0.18666666666666,0.18666666666666,0.18666666666666,0.18666666666666,0.18666666666666,0.06666666666666 };
	for (int i = 0; i < hidden_states_.size(); i++) {
		init_probs_[hidden_states_[i]] = pi[i];
	}
	/*std::vector<std::vector<double>> tmat1 = transition_matrixLife;
	for (int i = 0; i < 1000; ++i)
	{
		tmat1 = CUtil::multiplymat(transition_matrixLife, tmat1);
	}
	for (int i = 0; i < hidden_states_.size(); i++) {
		init_probs_[hidden_states_[i]] = tmat1[0][i];
	}*/
}

CHMMEvents::CHMMEvents(std::string modelname)
{
	readmodel(modelname);
}


std::vector<std::string> CHMMEvents::predict(std::vector<Observation>& observations) {
	// Initialize the forward and backward probability matrices
	//std::vector<std::vector<double>> forward_probs(observation_space_.size(), std::vector<double>(hidden_states_.size()));
	//std::vector<std::vector<double>> backward_probs(observation_space_.size(), std::vector<double>(hidden_states_.size()));

	std::vector<std::vector<double>> forward_probs(observations.size(), std::vector<double>(hidden_states_.size()));
	std::vector<std::vector<double>> backward_probs(observations.size(), std::vector<double>(hidden_states_.size()));

	
	// Compute the forward probabilities
	computeForwardProbs(observations, forward_probs);

	// Compute the backward probabilities
	computeBackwardProbs(observations, backward_probs);

	// Compute the viterbi path
	std::vector<std::string> viterbi_path = computeViterbiPath(forward_probs, backward_probs);
	viterbi_path.erase(std::unique(viterbi_path.begin(), viterbi_path.end()), viterbi_path.end());
	return viterbi_path;
}

void CHMMEvents::computeForwardProbs(const std::vector<Observation>& observations, std::vector<std::vector<double>>& forward_probs) const {

	// Compute forward probabilities for first observation
	for (int i = 0; i < hidden_states_.size(); ++i) {
		double init_prob = init_probs_.at(hidden_states_[i]);
		const EmissionProb& emit_prob = emit_probs_[i];
		Observation ob = observations[0];
		double obs_prob = emit_prob.probs.at(ob.financial_status + "_" + std::to_string(ob.age));
		forward_probs[0][i] = init_prob * obs_prob;
	}

	// Compute forward probabilities for subsequent observations
	for (int t = 1; t < observations.size(); ++t) {
		for (int i = 0; i < hidden_states_.size(); ++i) {
			double sum = 0.0;
			const EmissionProb& emit_prob = emit_probs_[i];
			double obs_prob = emit_prob.probs.at(observations[t].financial_status + "_" + std::to_string(observations[t].age));
			for (int j = 0; j < hidden_states_.size(); ++j) {
				double trans_prob = trans_probs_.at(hidden_states_[j]).at(hidden_states_[i]);
				sum += forward_probs[t - 1][j] * trans_prob;
			}
			forward_probs[t][i] = obs_prob * sum;
		}
	}

}


void CHMMEvents::computeBackwardProbs(const std::vector<Observation>& observations, std::vector<std::vector<double>>& backward_probs) const {
	const size_t num_obs = observations.size();
	//std::vector<std::vector<double>> backward_probs(num_obs, std::vector<double>(hidden_states_.size(), 0.0));

	for (int k = 0; k < observations.size(); ++k)
	{
		std::vector<double> initvect(hidden_states_.size(), 0.0);
		backward_probs.push_back(initvect);
	}

	// Initialize backward_probs for the last observation
	for (int i = 0; i < hidden_states_.size(); ++i) {
		const std::string& state = hidden_states_[i];
		backward_probs[num_obs - 1][i] = 1.0;
	}

	// Compute backward probabilities for the remaining observations
	for (int t = num_obs - 2; t >= 0; --t) {
		const Observation& obs_t1 = observations[t + 1];

		// Compute the emission probabilities for the observation at time t+1
		std::vector<double> emit_probs(hidden_states_.size(), 0.0);
		for (int j = 0; j < hidden_states_.size(); ++j) {
			emit_probs[j] = emit_probs_[j].probs.at(obs_t1.financial_status + "_" + std::to_string(obs_t1.age));
		}

		// Compute the backward probabilities for each state at time t
		for (int i = 0; i < hidden_states_.size(); ++i) {
			const std::string& state_i = hidden_states_[i];
			double sum = 0.0;
			for (int j = 0; j < hidden_states_.size(); ++j) {
				const std::string& state_j = hidden_states_[j];
				double trans_prob = trans_probs_.at(state_i).at(state_j);
				double emit_prob = emit_probs[j];
				double backward_prob_t1 = backward_probs[t + 1][j];
				sum += trans_prob * emit_prob * backward_prob_t1;
			}
			backward_probs[t][i] = sum;
		}
	}

	//return backward_probs;
}


std::vector<std::string> CHMMEvents::computeViterbiPath(const std::vector<std::vector<double>>& forward_probs,
	const std::vector<std::vector<double>>& backward_probs) {
	const size_t T = forward_probs.size();
	const size_t N = hidden_states_.size();
	std::vector<std::string> path(T);

	// Find the state with maximum probability at time T-1
	double max_prob = 0.0;
	int max_state_idx = 0;
	for (int i = 0; i < N; ++i) {
		double prob = forward_probs[T - 1][i] * init_probs_[hidden_states_[i]];
		if (prob > max_prob) {
			max_prob = prob;
			max_state_idx = i;
		}
		else
		{
			if (prob != 0.0)
			{
				double ddiff = max_prob - prob;
				if (ddiff < (10 * std::numeric_limits<double>::epsilon()))
				{
					max_prob = prob;
					max_state_idx = i;
				}
			}
		}
	}
	path[T - 1] = hidden_states_[max_state_idx];

	// Backtrack to find the most likely sequence of hidden states
	for (int t = T - 2; t >= 0; --t) {
		double max_prob = 0.0;
		int max_state_idx = 0;
		for (int i = 0; i < N; ++i) {
			double prob = forward_probs[t][i] * backward_probs[t + 1][i] * trans_probs_[hidden_states_[i]][path[t + 1]];
			if (prob > max_prob) {
				max_prob = prob;
				max_state_idx = i;
			}
		}
		path[t] = hidden_states_[max_state_idx];
	}

	return path;
}



// Get the index of the given hidden state
int CHMMEvents::getStateIndex(const std::string& state) const {
	auto it = state_to_index_.find(state);
	if (it != state_to_index_.end()) {
		return it->second;
	}
	else {
		return -1;
	}
}

// Get the index of the given age value
int CHMMEvents::getAgeIndex(int age) const {
	auto it = age_to_index_.find(age);
	if (it != age_to_index_.end()) {
		return it->second;
	}
	else {
		return -1;
	}
}

// Get the index of the given financial status
int CHMMEvents::getStatusIndex(const std::string& status) const {
	auto it = finstatus_to_index_.find(status);
	if (it != finstatus_to_index_.end()) {
		return it->second;
	}
	else {
		return -1;
	}
}

// Compute the observation probabilities for the given hidden state and observation
double CHMMEvents::computeObsProb(const std::string& state, const Observation& obs) const {
	int state_idx = getStateIndex(state);
	int age_idx = getAgeIndex(obs.age);
	int status_idx = getStatusIndex(obs.financial_status);
	if (state_idx == -1 || age_idx == -1 || status_idx == -1) {
		return 1.0;
	}
	else {
		//double prob = emit_probs_[state_idx].probs.at(std::to_string(age_idx) + "_" + std::to_string(status_idx));
		//return prob;
		double prob = 1.0;
		const EmissionProb& ep = emit_probs_[getStateIndex(state)];
		prob *= ep.probs.at(obs.financial_status + "_" + std::to_string(obs.age));
		return prob;
	}
}

// Compute the log of a sum of exponentials
double CHMMEvents::logSumExp(std::vector<double> x) const {
	double max_val = *std::max_element(x.begin(), x.end());
	double sum_exp = 0.0;
	for (const auto& val : x) {
		sum_exp += std::exp(val - max_val);
	}
	return std::log(sum_exp) + max_val;
}


std::vector<std::vector<double>> CHMMEvents::compute_posterior_probabilities(const std::vector<std::vector<double>>& alpha,const std::vector<std::vector<double>>& beta) const {
	size_t T = alpha.size();
	std::vector<std::vector<double>> posterior_probs(T, std::vector<double>(hidden_states_.size()));

	for (int t = 0; t < T; ++t) {
		double normalizer = 0.0;
		for (int i = 0; i < hidden_states_.size(); ++i) {
			posterior_probs[t][i] = alpha[t][i] * beta[t][i];
			normalizer += posterior_probs[t][i];
		}
		for (int i = 0; i < hidden_states_.size(); ++i) {
			if (normalizer < (10 * std::numeric_limits<double>::epsilon()))
			{
				break;
			}
			posterior_probs[t][i] /= normalizer;
		}
	}
	return posterior_probs;
}


std::vector<std::vector<std::vector<double>>> CHMMEvents::compute_joint_probabilities(
	const std::vector<Observation>& obs,
	const std::vector<std::vector<double>>& alpha,
	const std::vector<std::vector<double>>& beta
)
{
	
	std::vector<std::vector<std::vector<double>>> joint_probs(obs.size() - 1, std::vector<std::vector<double>>(hidden_states_.size(), std::vector<double>(hidden_states_.size(), 0.0)));

	// Compute the denominator for normalization
	double denominator = 0.0;
	for (int i = 0; i < hidden_states_.size(); i++) {
		for (int j = 0; j < hidden_states_.size(); j++) {
			denominator += alpha[obs.size() - 1][i] * trans_probs_[hidden_states_[i]][hidden_states_[j]] * emit_probs_[i].probs[obs.back().financial_status + "_" + std::to_string(obs.back().age)] * beta[0][j];
		}
	}

	// Compute the joint probabilities
	for (int t = 0; t < obs.size() - 1; t++) {
		if (denominator < (10 * std::numeric_limits<double>::epsilon()))
		{
			break;
		}
		for (int i = 0; i < hidden_states_.size(); i++) {
			for (int j = 0; j < hidden_states_.size(); j++) {
				joint_probs[t][i][j] = alpha[t][i] * trans_probs_[hidden_states_[i]][hidden_states_[j]] * emit_probs_[j].probs[obs[t + 1].financial_status + "_" + std::to_string(obs[t + 1].age)] * beta[t + 1][j] / denominator;
			}
		}
	}


	return joint_probs;
}


double CHMMEvents::computeObsSeqProb(const std::vector<Observation>& obs_sequence) const
{
	size_t num_states = hidden_states_.size();
	size_t T = obs_sequence.size();
	std::vector<std::vector<double>> alpha(T, std::vector<double>(num_states));

	// Compute alpha values for the first observation
	for (int i = 0; i < num_states; i++) {
		alpha[0][i] = init_probs_.at(hidden_states_[i]) * computeObsProb(hidden_states_[i], obs_sequence[0]);
	}

	// Compute alpha values for the rest of the observations
	for (int t = 1; t < T; t++) {
		for (int j = 0; j < num_states; j++) {
			double sum = 0.0;
			for (int i = 0; i < num_states; i++) {
				sum += alpha[t - 1][i] * trans_probs_.at(hidden_states_[i]).at(hidden_states_[j]);
			}
			alpha[t][j] = sum * computeObsProb(hidden_states_[j], obs_sequence[t]);
		}
	}

	// Compute the probability of the observation sequence
	double prob = 0.0;
	for (int i = 0; i < num_states; i++) {
		prob += alpha[T - 1][i];
	}
	return prob;
}

void CHMMEvents::train(const std::vector<std::pair<std::vector<std::pair<int, std::string>>, std::vector<std::string>>>& training_data, int num_iterations)
{
	// Initialize emission probabilities, transition probabilities, and initial probabilities
	//emit_probs_.resize(hidden_states_.size());
	trans_probs_.clear();
	init_probs_.clear();
	emit_probs_.clear();

	// Initialize emission probabilities
	for (const std::string& state : hidden_states_) {
		EmissionProb ep;
		for (const std::string& status : financial_status_) {
			for (int age : age_range_) {
				Observation obs{ age, status };
				double obs_prob = computeObsProb(state, obs);
				ep.probs[status + "_" + std::to_string(age)] = obs_prob;
			}
		}
		emit_probs_.push_back(ep);
	}

	// Initialize transition probabilities
	for (const std::string& from_state : hidden_states_) {
		std::unordered_map<std::string, double> to_probs;
		for (const std::string& to_state : hidden_states_) {
			to_probs[to_state] = 1.0 / hidden_states_.size();
		}
		trans_probs_[from_state] = to_probs;
	}


	// Initialize initial probabilities
	for (const std::string& state : hidden_states_) {
		init_probs_[state] = 1.0 / hidden_states_.size();
	}

	// Initialize observation space
	for (const auto& data : training_data) {
		for (const auto& obs_pair : data.first) {
			Observation obs{ obs_pair.first, obs_pair.second };
			observation_space_.push_back(obs);
		}
	}

	// Initialize the mapping of financial status names to indices
	for (int i = 0; i < financial_status_.size(); i++) {
		finstatus_to_index_[financial_status_[i]] = i;
	}

	size_t num_states_ = hidden_states_.size();
	for (int iter = 0; iter < num_iterations; iter++) {
		double log_likelihood = 0.0;
		std::unordered_map<std::string, double> state_count;
		std::unordered_map<std::string, std::unordered_map<int, double>> age_count;
		std::unordered_map<std::string, std::unordered_map<std::string, double>> emit_count;
		std::unordered_map<std::string, std::unordered_map<std::string, double>> trans_count;
		for (const auto& state : hidden_states_) {
			state_count[state] = 0.0;
		}

		// Step 1: Loop over the training data
		for (const auto& example : training_data) {

			const auto& obs_sequence = example.first;
			const auto& state_sequence = example.second;

			// Convert the observation sequence into a vector of Observations
			std::vector<Observation> observations;
			for (const auto& obs : obs_sequence) {
				Observation observation = { obs.first, obs.second };
				observations.push_back(observation);
			}

			// Compute the forward and backward probabilities
			std::vector<std::vector<double>> forward_probs;
			computeForwardProbs(observations, forward_probs);
			std::vector<std::vector<double>> backward_probs;
			computeBackwardProbs(observations, backward_probs);

			// Compute the posterior probabilities
			std::vector<std::vector<double>> post_probs = compute_posterior_probabilities(forward_probs, backward_probs);

			// Compute the joint probabilities
			auto joint_probs = compute_joint_probabilities(observations, forward_probs, backward_probs);

			// Update the transition probabilities and emission probabilities
			for (int t = 0; t < observations.size(); t++) {
				const auto& obs = observations[t];
				const auto& state = state_sequence[t];
				state_count[state] += post_probs[t][getStateIndex(state)];
				age_count[state][obs.age] += post_probs[t][getStateIndex(state)];
				emit_count[state][obs.financial_status] += post_probs[t][getStateIndex(state)];

				if (t > 0) {
					const auto& prev_state = state_sequence[t - 1];
					trans_count[prev_state][state] += joint_probs[t - 1][getStateIndex(prev_state)][getStateIndex(state)];
				}
			}

			log_likelihood += std::log10(computeObsSeqProb(observations));
		}

		// Update the initial probabilities
		for (const auto& state : hidden_states_) {
			init_probs_[state] = state_count[state] / observation_space_.size();
		}

		// Update the transition probabilities
		for (const auto& from_state : hidden_states_) {
			double count_sum = 0.0;
			for (const auto& to_state : hidden_states_) {
				count_sum += trans_count[from_state][to_state];
			}
			for (const auto& to_state : hidden_states_) {
				if (count_sum < (10 * std::numeric_limits<double>::epsilon()))
				{
					break;
				}
				trans_probs_[from_state][to_state] = trans_count[from_state][to_state] / count_sum;
			}
		}

		// Update the emission probabilities
		for (const auto& state : hidden_states_) {
			double count_sum = state_count[state];
			if (count_sum < (10 * std::numeric_limits<double>::epsilon()))
			{
				continue;
			}
			for (const auto& financial_status : financial_status_) {
				for (const auto& age : age_range_) {
					std::string obs_key = financial_status + "_" + std::to_string(age);
					emit_probs_[getStateIndex(state)].probs[obs_key] = (emit_count[state][financial_status]) / count_sum; //emit_count[state][financial_status][age]
				}
			}
		}

		// Reset the counters
		state_count.clear();
		age_count.clear();
		emit_count.clear();
		trans_count.clear();
	}
	
}

void CHMMEvents::savemodel()
{
	std::remove("HMMEvents.model");
	std::ofstream outfile;
	outfile.open("HMMEvents.model", std::ios::out | std::ios::app);

	std::string temp = "S :" + std::to_string(hidden_states_.size()) + "\n";
	outfile << temp;
	temp = "O1 :" + std::to_string(age_range_.size()) + "\n";
	outfile << temp;
	temp = "O2 :" + std::to_string(financial_status_.size()) + "\n";
	outfile << temp;

	outfile << "SN :";
	for (auto x : hidden_states_)
	{
		outfile << x + ", ";
	}
	outfile << "\n";

	outfile << "O1N :";
	for (auto age : age_range_)
	{
		outfile << std::to_string(age) + ", ";
	}
	outfile << "\n";


	outfile << "O2N :";
	for (auto f : financial_status_)
	{
		outfile << f + ", ";
	}
	outfile << "\n";

	outfile << "Tansition probabilty matrix [";
	for (auto x : hidden_states_)
	{
		outfile << x + ", ";
	}
	outfile << "] : $\n";
	for (auto var : trans_probs_)
	{
		outfile << var.first + ",\t";
		for (auto var1 : var.second)
		{
			outfile << std::to_string(var1.second) + ", ";
		}
		outfile << "\n";
	}
	//

	outfile << "\nEmission probabilty matrix : $\n";
	int i = 0;
	for (auto var : emit_probs_)
	{
		for (auto var1 : var.probs)
		{
			outfile << std::to_string(i) + ", " + var1.first + ", " + std::to_string(var1.second) + "\n";
		}
		i++;
	}


	outfile << "\nInit probabilty matrix : $\n";
	for (auto var : init_probs_)
	{
		outfile << var.first + ", " + std::to_string(var.second) + "\n";
	}


	outfile << "\nObservable space : $\n";
	for (auto var : observation_space_)
	{
		outfile << std::to_string(var.age) + ", " + var.financial_status + "\n";
	}
	outfile.flush();
	outfile.close();
}

void CHMMEvents::readmodel(std::string modelname)
{
	std::ifstream infile(modelname.c_str());;
	if (!infile) {
		throw std::runtime_error("Unable to open file " + modelname);
	}

	std::string line;
	int statscount = 0;
	int ob1count = 0;
	int ob2count = 0;
	bool bTransi = false;
	bool bEmsn = false;
	bool bInit = false;
	bool bObsSpace = false;
	const std::string trnas = "Tansition"; 
	const std::string emsn = "Emission"; 
	const std::string inits = "Init";
	const std::string obsbl = "Observable";
	const std::string delim = ":";
	const std::string delim1 = "$";
	const std::string delim2 = ",";
	while (std::getline(infile, line)) 
	{
		std::istringstream iss(line);
		if (line.find(delim) != std::string::npos) 
		{
			std::vector<std::string> output = CUtil::split(line, ':');
			std::string t0 = CUtil::trim(output[0]);
			std::string t1 = CUtil::trim(output[1]);
			if (t0 == "S")
			{
				statscount = std::stoi(t1);
			}
			else if (t0 == "O1")
			{
				ob1count = std::stoi(t1);
			}
			else if (t0 == "O2")
			{
				ob2count = std::stoi(t1);
			}
			else if (t0 == "SN")
			{
				hidden_states_.clear();
				std::vector<std::string> vls = CUtil::split(t1, ',');
				for (auto x : vls)
				{
					auto x1 = CUtil::trim(x);
					if (x1 != "")
					{
						hidden_states_.push_back(x1);
					}
				}
			}
			else if (t0 == "O1N")
			{
				age_range_.clear();
				std::vector<std::string> vls = CUtil::split(t1, ',');
				for (auto x : vls)
				{
					auto x1 = CUtil::trim(x);
					if (x1 != "")
					{
						age_range_.push_back(std::stoi(x1));
					}
				}
			}
			else if (t0 == "O2N")
			{
				financial_status_.clear();
				std::vector<std::string> vls = CUtil::split(t1, ',');
				for (auto x : vls)
				{
					auto x1 = CUtil::trim(x);
					if (x1 != "")
					{
						financial_status_.push_back(x1);
					}
				}
			}
			else
			{	
				if (t0.find(trnas) != std::string::npos)
				{
					bTransi = true;
					bEmsn = false;
					bInit = false;
					bObsSpace = false;
					trans_probs_.clear();
					for (const std::string& from_state : hidden_states_) {
						std::unordered_map<std::string, double> to_probs;
						for (const std::string& to_state : hidden_states_) {
							to_probs[to_state] = 1.0 / hidden_states_.size();
						}
						trans_probs_[from_state] = to_probs;
					}
					continue;
				}
				else if (t0.find(emsn) != std::string::npos)
				{
					bTransi = false;
					bEmsn = true;
					bInit = false;
					bObsSpace = false;
					//clear
					emit_probs_.clear();
					// Initialize emission probabilities
					for (const std::string& state : hidden_states_) {
						EmissionProb ep;
						for (const std::string& status : financial_status_) {
							for (int age : age_range_) {
								Observation obs{ age, status };
								double obs_prob = computeObsProb(state, obs);
								ep.probs[status + "_" + std::to_string(age)] = obs_prob;
							}
						}
						emit_probs_.push_back(ep);
					}
					continue;
				}
				else if (t0.find(inits) != std::string::npos)
				{
					bTransi = false;
					bEmsn = false;
					bInit = true;
					bObsSpace = false;
					init_probs_.clear();
					for (int i = 0; i < hidden_states_.size(); i++) {
						init_probs_[hidden_states_[i]] = 0.0;
					}
					continue;
				}
				else if (t0.find(obsbl) != std::string::npos)
				{
					bTransi = false;
					bEmsn = false;
					bInit = false;
					bObsSpace = true;
					observation_space_.clear();
					continue;
				}
				else
				{
					bTransi = false;
					bEmsn = false;
					bInit = false;
					bObsSpace = false;
				}
			}
		}

		if (bTransi)
		{
			std::vector<std::string> vls = CUtil::split(line, ',');
			int i = 0;
			int j = 0;
			std::string state;
			for (auto x : vls)
			{
				auto x1 = CUtil::trim(x);
				if (x1 != "")
				{
					if (0 == i)
					{
						state = x1;
					}
					else
					{
						trans_probs_[state][hidden_states_[j]] = std::stod(x1);
						j++;
					}
				}
				i++;
			}
		}

		if (bEmsn)
		{
			std::vector<std::string> vls = CUtil::split(line, ',');
			if (vls.size() < 3)
			{
				continue;
			}
			auto x1 = CUtil::trim(vls[0]);
			auto x2 = CUtil::trim(vls[1]);
			auto x3 = CUtil::trim(vls[2]);
			if ( (x1 != "") && (x2 != "") && (x3 != ""))
			{
				emit_probs_[std::stoi(x1)].probs[x2] = std::stod(x3);
			}
		}

		if (bInit)
		{
			std::vector<std::string> vls = CUtil::split(line, ',');
			if (vls.size() < 2)
			{
				continue;
			}
			auto x1 = CUtil::trim(vls[0]);
			auto x2 = CUtil::trim(vls[1]);
			if ((x1 != "") && (x2 != ""))
			{
				init_probs_[x1] = std::stod(x2);
			}
		}

		if (bObsSpace)
		{
			std::vector<std::string> vls = CUtil::split(line, ',');
			if (vls.size() < 2)
			{
				continue;
			}
			auto x1 = CUtil::trim(vls[0]);
			auto x2 = CUtil::trim(vls[1]);
			if ((x1 != "") && (x2 != ""))
			{
				Observation obs = { std::stoi(x1), x2 };
				observation_space_.push_back(obs);
			}
		}
	};
	infile.close();
}

