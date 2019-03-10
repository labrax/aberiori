/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */
 
#pragma once

#include <cstdint>

#include "../common/database_normalized.hpp"
#include "large.hpp"
#include "parameters.hpp"

#include "../common/ontologies/ontology.hpp"

class Apriori {
	private:
		Parameters parameters;

		DatabaseNormalized * database;
		vector<LargeItemSet *> largesets;
		
		Ontology * ontologies;

		uint64_t min_transactions;
		uint64_t max_transactions;

	public:		
		Apriori(int argc, char * argv[]);
		~Apriori();
		
		void setup();
		void run();
};
