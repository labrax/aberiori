/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "rule_appender.hpp"

#include "../common/new_transactions/genNewTransaction.hpp"

#include <iostream>
#include <thread>

#include <algorithm>

#include <getopt.h>
#define no_argument 0
#define required_argument 1
#define optional_argument 2

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char * argv[]) {
	RuleAppender a = RuleAppender(argc, argv);
	return 0;
}

bool RuleAppender::debug = false;
bool RuleAppender::verbose = false;
unsigned int RuleAppender::thread_number = 0;
bool RuleAppender::filter_results = false;

RuleAppender::RuleAppender(int argc, char * argv[]) {
	static struct option long_options[] = {
		{"help",			no_argument, 0, 'h'},
		{"preprocessed",	no_argument, 0, 'p'},
		{"verbose",			no_argument, 0, 'v'},
		{"debug",			no_argument, 0, 'd'},
		{"filter-results", no_argument, 0, 'r'},

		{"transactions-file", required_argument, 0, 'f'},
		{"new-transactions-file", required_argument, 0, 'x'},
		{"rules-file",		required_argument, 0, 'e'},
		{"ontologies-file", required_argument, 0, 'o'},

		{"confidence", 		required_argument, 0, 'c'},
		{"minsupport", 		required_argument, 0, 'l'},
		{"maxsupport", 		required_argument, 0, 'u'},

		{"thread",			optional_argument, 0, 't'},
		{0, 0, 0, 0},
	};
	int option_index = 0;
    
    rulesFile.assign("");
    oldTransactionsFile.assign("");
    newTransactionsFile.assign("");
    use_ontology = false;
    ontologiesFile.assign("");

    use_thread = false;
    thread_number = 0;
    preprocessed = false;
    gen_new_transaction_file = false;
    min_support = 0.5;
    max_support = 1;
    confidence = 0.8;
    
    int c = getopt_long(argc, argv, "hpvdrf:x:e:o:c:l:u:t::", long_options, &option_index);
    
    while(c != -1) {
		switch(c) {
			case 0:
				if(long_options[option_index].flag != 0)
					break;
				cerr << "option " << long_options[option_index].name;
				if(optarg)
					cerr << " with arg " << std::boolalpha << optarg << endl;
				break;
			case 'h':
				print_instructions();
				break;
			case 'p':
				preprocessed = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'd':
				debug = true;
				break;
			case 'r':
				filter_results = true;
				break;
			case 'f':
				oldTransactionsFile.assign(optarg);
				break;
			case 'x':
				gen_new_transaction_file = true;
				newTransactionsFile.assign(optarg);
				break;
			case 'e':
				newTransactionsFile.assign(optarg);
				break;
			case 'o':
				use_ontology = true;
				ontologiesFile.assign(optarg);
				break;
			case 'c':
				confidence = atof(optarg);
				break;
			case 'l':
				min_support = atof(optarg);
				break;
			case 'u':
				max_support = atof(optarg);
				break;
			case 't':
				use_thread = true;				
				if(optarg) {
					int getpos;
					for(getpos = 0; (optarg[getpos] < '0' || optarg[getpos] > '9') && optarg[getpos] != ' '; ++getpos);
								
					if(isdigit(optarg[getpos]))
						thread_number = atoi(optarg + getpos);
				}
				break;
			default:
				print_instructions();
				break;
		}
		c = getopt_long(argc, argv, "hpvdrf:x:e:o:c:l:u:t::", long_options, &option_index);
	}
	
	if(optind < argc) {
		cerr << "Guessing the remaining parameters: " << endl;
		while(optind < argc) {
			string par(argv[optind++]);
			if(par.find(".obo") != string::npos) {
				cerr << "\tconsidering " << par << " as the ontologies file" << endl;
				ontologiesFile = par;
				use_ontology = true;
			}
			else if(par.find("rules") != string::npos) {
				cerr << "\tconsidering " << par << " as the rules file" << endl;
				rulesFile = par;
			}
			else if(par.find(".txt") != string::npos) {
				if(oldTransactionsFile == "") {
					cerr << "\tconsidering " << par << " as the transactions-file" << endl;
					oldTransactionsFile = par;
				}
				else if(newTransactionsFile == "") {
					cerr << "\tconsidering " << par << " as the new-transactions-file" << endl;
					gen_new_transaction_file = true;
					newTransactionsFile = par;
				}
			}
			else {
				cerr << "\tdoesn't know what " << par << " means" << endl;
			}
		}
	}

	if(confidence < 0 || confidence > 1) {
		print_instructions();
	}
	
	if(min_support < 0 || min_support > 1) {
		print_instructions();
	}
	
	if(max_support < 0 || max_support > 1) {
		print_instructions();
	}
	
	if(oldTransactionsFile == "") {
		print_instructions();
	}
	
	if(rulesFile == "") {
		print_instructions();
	}
	
	if(gen_new_transaction_file && newTransactionsFile == "") {
		print_instructions();
	}
	
	cerr << std::boolalpha;
	cerr << "running with" << endl
		 << "transactions-file: " << oldTransactionsFile << " preprocessed: " << preprocessed << endl;
	cerr << "rules-file: " << rulesFile << endl;
	cerr << "min-support: " << min_support << endl
		 << "max-support: " << max_support << endl
		 << "confidence: " << confidence << endl;
	cerr << "threads: " << use_thread;
	if(thread_number != 0)
		cerr << " with " << thread_number << " threads" << endl;
	else
		cerr << endl;
		
	cerr << "verbose: " << verbose << endl;
	cerr << "debug: " << debug << endl;
	
	cerr << "ontologies: ";
	if(use_ontology)
		cerr << ontologiesFile << endl;
	else
		cerr << "false" << endl;
	
	if(use_thread)
		if(thread_number == 0) {
			thread_number = std::thread::hardware_concurrency();
			if(thread_number == 0) {
				cerr << "Error initializing thread_number, running without threads!" << endl;
				use_thread = false;
				thread_number = 0;
			}
		}
		
	cerr << "filter-results: " << filter_results << endl;
	
	cerr << "new-transactions-file: ";
	if(gen_new_transaction_file)
		cerr << rulesFile << endl;
	else
		cerr << "false" << endl;
}

RuleAppender::~RuleAppender() {
	delete(database);
	delete(rules);
}

void RuleAppender::readRules() { //TODO: implement RuleAppender::readRules()
	rules = new Rules(rulesFile);
	
	//TODO: create removeRules
	//rules->filterRules(); //TODO: check filterRules from RuleAppender::readRules()
	rules->print();
}

void RuleAppender::readOldTransactions() {
	database = new DatabaseNormalized(oldTransactionsFile, preprocessed);
	if(verbose)
		cout << database->getNormalizedTransactions().size() << " normalized transactions obtained" << endl;
		
	std::sort(database->getNormalizedTransactions().begin(), database->getNormalizedTransactions().end(), normalizedCompare); //probably it will already be sorted, just in case this function is still here
	if(debug)
		cout << "transactions sorted" << endl;
		
	if(verbose)
		cout << "Amount of transactions is " << database->getAmountTransactions() << endl;
}

void RuleAppender::writeNewTransactions() {
	if(gen_new_transaction_file) {
		genNewTransaction gNT(rules, database, ontologies);
		gNT.generateNewData(use_thread);
		gNT.toFile(newTransactionsFile);
	}
}

void RuleAppender::print_instructions(void) {
	cerr << "use format:" << endl
		 << "-r	filter-results: will remove rules in the format A+B=>C where A=>C exists and has higher support" << endl
		 << endl
		 << "-h	help: will print this instruction of use" << endl
		 << "-p	preprocessed: will not use 'sort -u' in the file before reading" << endl
		 << "-v	verbose: will print some information of execution" << endl
		 << "-d	debug: will print the sets information" << endl
		 << endl
		 << "-f	transactions-file <file>: input file" << endl
		 << "-o	ontologies-file <file>: input file" << endl
		 << "-e	rules-file <file>: 'input file" << endl
		 << endl
		 << "-x	new-transactions-file <file>: create a new 'transactions' file using the rules" << endl
		 << endl
		 << "-c	confidence <number 0 to 1>: confidence value - default 0.8" << endl
		 << "-l	minsupport <number 0 to 1>: minimum support value - default 0.5" << endl
		 << "-u	maxsupport <number 0 to 1>: maximum support value - default 1" << endl
		 << endl
		 << "-t	thread <optional: number of threads>: to use parallelization" << endl
		 << endl
		 << "options can be guessed, if:" << endl
		 << "the transaction files is .txt it doesn't need -f" << endl
		 << "the ontologies file is .obo it doesn't need -o" << endl
		 << "the rules file has 'rules' in the name it doesn't need -o" << endl
		 << endl;

	cerr << "example of use: " << endl
		 << "./rule_appender -f mousephenotypes.txt -e rules.txt -x new_mousephenotypes.txt -t=20" << endl
		 << "./rule_appender mousephenotypes.txt rules.txt new_mousephenotypes.txt -t -s 0.01 -c 0.3" << endl
		 << endl;
		 
	exit(-1);
}
