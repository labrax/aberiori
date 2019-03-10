/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

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
};

void print_instructions();
