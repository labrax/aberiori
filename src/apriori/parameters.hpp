/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#ifndef __PARAMETERS_H_
#define __PARAMETERS_H_

//#include <iostream>
#include <string>

using std::string;

class Parameters {
	private:
		bool use_thread;
		bool use_ontology;
		
		bool preprocessed;
		
		bool gen_new_transaction_file;
		
		string file;
		string ontologies_file;
		string output_file;
		double min_support, max_support, confidence;
		
		bool itemset_filtering;
		bool rules_filtering;
		unsigned int maximum_iteration;
	public:
		Parameters(int argc, char * argv[]);
		~Parameters();
		
		static bool verbose;
		static bool debug;
		static unsigned int thread_number;
		
		static bool dont_append_ontologies;
		static bool filter_results;
		
		const bool & useThread();
		const bool & useOntology();
		const bool & isPreprocessed();
		const bool & genNewTransactionFile();

		const string & phenotypesFile();
		const string & ontologiesFile();
		
		const string & outputFile(); //for new transaction added with rules
		
		const double & getMinSupport();
		const double & getMaxSupport();
		
		const double & getConfidence();
		
		const bool & useItemsetFiltering();
		const bool & useRulesFiltering();
		const unsigned int & getMaximumIteration();
};

void print_instructions();

#endif
