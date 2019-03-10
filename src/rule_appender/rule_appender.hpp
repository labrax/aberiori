/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include "../common/ontologies/ontology.hpp"
#include "../common/database_normalized.hpp"
#include "../common/rule/rules.hpp"

#include <string>

using std::string;

class RuleAppender {
	private:
		double confidence;
		double min_support, max_support;

		bool use_thread;
		bool preprocessed;
		bool gen_new_transaction_file;

		string rulesFile;
		string oldTransactionsFile;
		string newTransactionsFile;
		bool use_ontology;
		string ontologiesFile;
		
		Ontology * ontologies;
		DatabaseNormalized * database;
		Rules * rules;
		
		void readRules();
		void readOldTransactions();
		void writeNewTransactions();
	public:
		static bool debug;
		static bool verbose;
		static unsigned int thread_number;
		static bool filter_results;
		
		RuleAppender(int argc, char * argv[]);
		~RuleAppender();
		void print_instructions(void);
};
