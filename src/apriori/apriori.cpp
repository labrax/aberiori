/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */
 
#include "apriori.hpp"

#include "../common/itemset/itemset.hpp"
#include "candidate_apriori/candidate.hpp"
#include "../common/rule/rules.hpp"

#include "../common/new_transactions/genNewTransaction.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <cstdio>
//#include <cstdlib>

using namespace std;

#ifndef _TEST_
int main(int argc, char *argv[]) {
	Apriori program(argc, argv);
	
	program.setup();
	program.run();
	
	return 0;
}
#endif

Apriori::Apriori(int argc, char * argv[]) : parameters(argc, argv) {
	
}

Apriori::~Apriori() {
	
}

void Apriori::setup() {
	database = new DatabaseNormalized(parameters.phenotypesFile(), parameters.isPreprocessed());
	
	if(parameters.useOntology()) {
		ontologies = new Ontology(parameters.ontologiesFile());
		ontologies->processOntologies();
		if(!Parameters::dont_append_ontologies)
			ontologies->appendOntologies(&database->getNormalizedTransactions());
			
		if(Parameters::debug)
			cout << "ontologies have been loaded and inserted into transactions" << endl;
		if(Parameters::verbose)
			cout << database->getNormalizedTransactions().size() << " normalized transactions obtained with ontologies added" << endl;
	}
	else {
		ontologies = new Ontology();
		if(Parameters::debug)
			cout << "ontologies are not being used" << endl;
		if(Parameters::verbose)
			cout << database->getNormalizedTransactions().size() << " normalized transactions obtained" << endl;
	}
	
	std::sort(database->getNormalizedTransactions().begin(), database->getNormalizedTransactions().end(), normalizedCompare); //probably it will already be sorted, just in case this function is still here
	if(Parameters::debug)
		cout << "transactions sorted" << endl;
		
	//database->printNormalizedTransactions();
		
	/*a.removeDuplicates(); //TOO SLOW, USING uniq IN TERMINAL
	cout << "removed duplicates!" << endl;*/
	
	if(Parameters::verbose)
		cout << "Amount of transactions is " << database->getAmountTransactions() << endl;
	
	min_transactions = database->getAmountTransactions()*parameters.getMinSupport();
	if(min_transactions == 0)
		min_transactions = 1;
	if(Parameters::verbose)
		cout << "The minimum support is obtained with " << min_transactions  << " transactions" << endl;
		
	max_transactions = database->getAmountTransactions()*parameters.getMaxSupport();
	if(max_transactions == 0)
		max_transactions = 1;
	if(Parameters::verbose)
		cout << "The maximum support is obtained with " << max_transactions  << " transactions" << endl;
}

void Apriori::run() {
	//!OBTAINING 1-ITEMSETS
	std::map<string, uint64_t> itemset_1;
	for(auto &i : database->getNormalizedTransactions()) {
		std::map<string, uint64_t>::iterator it = itemset_1.find(i.second);
		if(it != itemset_1.end()) {
			itemset_1.find(i.second)->second++;
		}
		else {
			itemset_1.insert(pair<string, uint64_t> (i.second, 1));
		}
	}
	
	LargeItemSet * large_1 = new LargeItemSet(1);
	//this will print the count and transfer each item that /survived/
	for(auto &i : itemset_1) {
		if(i.second < min_transactions || i.second > max_transactions) //will skip it if it shouldn't be in large(1)
			continue;
		//cout << i.first << " #SUP:" << i.second << " " << i.second/(double) a.getAmountTransactions() << endl;
		ItemSet * a = new ItemSet();
		a->insert(i.first);
		a->setSupportCount(i.second);
		
		large_1->insertSet(a);
	}
	//!OBTAINING 1-ITEMSETS
	
	if(Parameters::verbose)
		large_1->printinfo();
	if(Parameters::debug)
		large_1->print();
	
	unsigned int iteration = 1;
	
	Rules rules(database->getAmountTransactions(), parameters.getConfidence(), ontologies, itemset_1, "SCTacELDH12X", true);
	LargeItemSet * large_obtained = large_1; //every large obtained will be passed to Rules::addLarge; where it will be destroyed with the object
	do {
		if(parameters.useItemsetFiltering())
			large_obtained->filterSet(ontologies);
		rules.addLarge(large_obtained);
		rules.computeRules(iteration);
		if(parameters.useRulesFiltering())
			rules.filterRules(iteration);
		//rules.print(iteration);
		
		CandidateItemSet cis(*ontologies);
		LargeItemSet * large_temp;
		
		if(parameters.useThread())
			large_temp = cis.apriori_genThreaded(large_obtained);
		else
			large_temp = cis.apriori_gen(large_obtained);
		
		if(Parameters::verbose)
			cout << "large_temp->size() = " << large_temp->getItemSets().size() << endl;

		if(parameters.useThread())
			large_obtained = cis.subsetThreaded(large_temp, &database->getNormalizedTransactions(), min_transactions, max_transactions);
		else
			large_obtained = cis.subset(large_temp, &database->getNormalizedTransactions(), min_transactions, max_transactions);
		
		delete(large_temp);
		
		if(Parameters::debug)
			cout << "sorting results" << endl;
		large_obtained->sort();
		
		if(Parameters::verbose)
			large_obtained->printinfo();
		if(Parameters::debug)
			large_obtained->print();
			
		iteration++;
	} while(large_obtained->getItemSets().size() > 0 && (parameters.getMaximumIteration() == 0 || iteration <= parameters.getMaximumIteration()));
	
	if(parameters.useItemsetFiltering())
		large_obtained->filterSet(ontologies);
	rules.addLarge(large_obtained);
	
	//rules.computeAllRules();
	//rules.print();
	
	rules.computeRules(iteration);
	//rules.print(iteration);
	
	if(parameters.useRulesFiltering())
		rules.filterRules(iteration);
	
	rules.migrateIterationRulesToRules();
	rules.print();
	
	if(parameters.genNewTransactionFile()) {
		genNewTransaction gNT(&rules, database, ontologies);
		gNT.generateNewData(parameters.useThread());
		gNT.toFile(parameters.outputFile());
	}
	
	if(ontologies)
		delete(ontologies);
	
	delete(database);
}
