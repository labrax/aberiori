/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "genNewTransaction.hpp"
#include "../parameters.hpp"
#include "../database_normalized.hpp"

#include <iostream>
#include <map>
#include <thread>

#include <algorithm>

#include <fstream>

using std::cerr;
using std::cout;
using std::endl;

using std::map;
using std::tuple;
using std::get;

using std::thread;

genNewTransaction::genNewTransaction(Rules * rules, DatabaseNormalized * database, Ontology * ontologies) : rules(rules), database(database), ontologies(ontologies) {
	
}

genNewTransaction::~genNewTransaction() {
	
}

vector <pair <string, string>> & genNewTransaction::generateNewData(bool use_thread) {
	map<string, vector<RuleNode *>> * rulesMap = &rules->getRulesMap(); //for the operations
	
	new_transactions = database->getNormalizedTransactions(); //will copy the vector data
	
	uint64_t increased_size=0;

	if(use_thread) {
		vector<tuple<uint64_t, uint64_t, vector<pair<string, string>> * >> thread_blocks;
		
		uint64_t initial=0;
		while(thread_blocks.size() < Parameters::thread_number-1) {
			uint64_t new_initial = new_transactions.size()*thread_blocks.size()/Parameters::thread_number;
			if(new_initial < initial)
				new_initial = initial;

			uint64_t new_end = new_transactions.size()*(thread_blocks.size()+1)/Parameters::thread_number;
			for(initial = new_end; initial < new_transactions.size() && (new_transactions)[initial].first == (new_transactions)[new_end].first; ++initial);
			
			thread_blocks.insert(thread_blocks.end(), tuple<uint64_t, uint64_t, vector<pair<string, string>> * >(new_initial, initial, new vector<pair<string, string>>()));
		}
		thread_blocks.insert(thread_blocks.end(), tuple<uint64_t, uint64_t, vector<pair<string, string>> * > (initial, new_transactions.size(), new vector<pair <string, string>>()));
		
		vector<thread *> threads;
		
		//cout << "0 to " << new_transactions.size() << endl;
		for(uint64_t i=0; i<thread_blocks.size(); i++) {
			//cout << "thread : " << get<0>(thread_blocks[i]) << " " << get<1>(thread_blocks[i]) << endl;
			threads.insert(threads.end(), new thread(run_threaded, get<0>(thread_blocks[i]), get<1>(thread_blocks[i]), get<2>(thread_blocks[i]), &new_transactions, rulesMap));
		}
		
		for(auto & t: threads)
			t->join();

		for(auto & t: threads)
			delete(t);
		
		for(auto & d : thread_blocks) {
			increased_size += get<2>(d)->size();
			for(auto & n : *(get<2>(d))) { //insert generated data
				new_transactions.insert(new_transactions.end(), n);
			}
			delete(get<2>(d));
		}
	}
	else {
		increased_size = new_transactions.size();
		run_threaded(0, new_transactions.size(), &new_transactions, &new_transactions, rulesMap);
		
		increased_size = new_transactions.size() - increased_size;
	}
	
	if(Parameters::verbose)
		cout << increased_size << " items added to transactions" << endl;
	
	std::sort(new_transactions.begin(), new_transactions.end(), normalizedCompare);
	
	for(auto & v : *rulesMap) {
		for(auto & r: v.second)
			delete(r);
	}
	
	return new_transactions;
}

void genNewTransaction::toFile(string file) {
	std::ofstream stream;
	stream.open(file, std::ofstream::out | std::ofstream::trunc);
	if(stream.is_open() == false) {
		cerr << "error opening " << file << " output file" << endl
			 << "outputing to standard output" << endl;
		cout << "------------------";
		for(auto & i : new_transactions) {
			stream << i.first << "\t" << i.second << endl;
		}
		cout << "------------------";
	}
	else {
		for(auto & i : new_transactions) {
			stream << i.first << "\t" << i.second << endl;
		}
		stream.close();
		if(Parameters::verbose)
			cerr << "new_transaction file generated to " << file << endl;
	}
}

void run_threaded(uint64_t tstart, uint64_t tend, vector<pair<string,string>> * tout, vector<pair <string, string>> * new_transactions, map<string, vector<RuleNode *>> * rulesMap) {	
	uint64_t end=0; //begin and end indicate the range of a transaction; increased_size the amount of new values inserted into normalized_transactions
	
	uint64_t initial_size = tend;
	for(uint64_t begin=tstart; begin < initial_size; begin=end) { //will scan transaction by transaction
		for(end=begin; end < new_transactions->size() && (*new_transactions)[end].first == (*new_transactions)[begin].first; ++end);
		
		vector<pair <string, string>>::const_iterator first = new_transactions->begin() + begin; //get the first element in a transaction
		vector<pair <string, string>>::const_iterator last = new_transactions->begin() + end; //get the last element in a transaction
		
		vector<pair <string, string>> transaction(first, last); //new sub-transaction
	
		vector<pair<string, string>> * newtra = getNewTransaction(transaction, rulesMap); //get new data
		
		for(auto & i: *newtra) {
			tout->insert(tout->end(), i);
		}
		
		delete(newtra);
	}
}

vector <pair <string, string>> * getNewTransaction(vector <pair <string, string>> & transaction, map<string, vector<RuleNode *>> * rulesMap) {	
	vector<pair <string, string>> * newTransactionData = new vector<pair <string, string>>();
	
	for(auto & e : transaction) {
		map<string, vector<RuleNode *>>::iterator it = rulesMap->find(e.second);
		if(it != rulesMap->end()) { //if the element has a rule associated to it (NOTE: this structure only store the first element for each rule, what should be enough, as the rules are in crescent order)
			for(auto & r : it->second) { //try each rule
				bool ok = false;
				
				for(auto & a : r->getItemSetAntecedent()->getItemSet()) { //will go through the antecedents
					bool sub_ok = false;
					
					for(auto & p : transaction) {
						if(p.second == a.first) { //if contain good, next element
							sub_ok = true;
							break;
						}
					}

					if(sub_ok == false) { //doesnt contain one of the antecedents
						ok = false;
						break;
					}
					else
						ok=true;
				}
				
				if(ok == true) { //add the consequent if the antecedents are present
					for(auto & c : r->getItemSetConsequent()->getItemSet()) { //for each of the consequents
						
						bool cool = true; //test for duplicates
						for(auto & t : transaction) {
							if(t.second == c.first) {
								cool = false;
								break;
							}
						}
						
						if(cool == true)
							newTransactionData->insert(newTransactionData->end(), pair<string, string>(e.first, c.first)); //e->first is the initial transaction identifier; c->first is the new item
					}
				}
			}
		}
	}
	
	return newTransactionData;
}
