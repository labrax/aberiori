/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */
 
#pragma once

#include <vector>
#include <string>

#include <cstdint>

#include "candidate_tree.hpp"
#include "../large.hpp"
#include "../../common/ontologies/ontology.hpp"

using std::vector;
using std::pair;
using std::string;

class CandidateItemSet {
	private:
		CandidateTree * root;
		
		Ontology & ontologies;
		
	public:
		CandidateItemSet(Ontology & ontologies);
		~CandidateItemSet();
		
		LargeItemSet * apriori_gen(LargeItemSet *);
		LargeItemSet * apriori_genThreaded(LargeItemSet *);
		LargeItemSet * subset(LargeItemSet * a, vector <pair <string, string>> * normalized_transactions, uint64_t, uint64_t);
		LargeItemSet * subsetThreaded(LargeItemSet * a, vector <pair <string, string>> * normalized_transactions, uint64_t, uint64_t);
};

void run_apriori_genThreaded(Ontology * ontologies, LargeItemSet *, LargeItemSet *, vector<ItemSet *> *, uint64_t, uint64_t);
inline uint64_t amount_op_aux(uint64_t a, uint64_t b); //to calculate how to separate the threaded operation
inline uint64_t amount_op(uint64_t amount_transactions, uint64_t block_init, uint64_t block_end); //as it is a quadractic function, to separate the threads

void run_subsetThreaded(CandidateTree *, vector <pair <string, string>> *, uint64_t, uint64_t);
