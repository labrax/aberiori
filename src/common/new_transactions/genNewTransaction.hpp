/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include "../rule/rules.hpp"
#include "../database_normalized.hpp"
#include "../ontologies/ontology.hpp"

#include <string>
#include <vector>
#include <string>

using std::vector;
using std::pair;
using std::string;


class genNewTransaction {
	private:
		Rules * rules;
		DatabaseNormalized * database;
		Ontology * ontologies;
		
		vector<pair <string, string>> new_transactions;
	public:
		genNewTransaction(Rules *, DatabaseNormalized *, Ontology *);
		~genNewTransaction();
		
		vector<pair <string, string>> & generateNewData(bool);
		
		void toFile(string);
};

void run_threaded(uint64_t tstart, uint64_t tend, vector<pair<string,string>> * tout, vector<pair <string, string>> * new_transactions, map<string, vector<RuleNode *>> * rulesMap);
vector <pair <string, string>> * getNewTransaction(vector <pair <string, string>> & transaction, map<string, vector<RuleNode *>> * rulesMap);
