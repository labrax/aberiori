/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include "../itemset/itemset.hpp"
#include "../large.hpp"
#include "../ontologies/ontology.hpp"
#include "rule_node.hpp"
#include "rule_group.hpp"

#include <vector>
#include <tuple>

#include <map>
#include <string>

using std::vector;
using std::tuple;

using std::map;
using std::string;

class Rules {
	private:
		vector<RuleNode *> rules;
		map<unsigned int, vector<RuleNode*>*> rulesIteration;
		
		map<string, vector<RuleNode *>> rulesMap;
		
		vector<LargeItemSet *> larges;
		Ontology * ontologies;
		
		uint64_t amount_transactions;
		double confidence;
		std::map<string, uint64_t> itemset_1; //to calculate the amount of information
		
		char output_format[16];
		bool try_ontology_name;
	public:
		Rules(uint64_t amount_transactions, double confidence, Ontology * ontologies, map<string, uint64_t> & itemset_1, char output_format[16], bool try_ontology_name);
		Rules(string file);
		~Rules();
		
		void addLarge(LargeItemSet *);
		void computeAllRules();
		void print();
		
		void computeRules(unsigned int iteration);
		void filterRules(unsigned int iteration);
		void print(unsigned int iteration);
		void printHeader();
		void printNode(RuleNode*);
		
		void migrateIterationRulesToRules();		
		
		uint64_t getFrequency(ItemSet * itemset);
		void calculateSemanticSimilaritySide(RuleNode * r); //will calculate considering the grouping of the whole side
		void calculateSemanticSimilarityAll(RuleNode * r); //will calculate considering each element individually
		double informationMeasure(string identifier);
		
		vector<RuleNode *> & getRules();
		map<string, vector<RuleNode *>> & getRulesMap();
};

bool rulesSort(const RuleNode * a, const RuleNode * b);
