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
#include "rule_node.hpp"

#include <vector>

using std::vector;

class RuleGroup {
	private:		
		ItemSet * ancestor_antecedent;
		ItemSet * ancestor_consequent;
		
		vector<RuleNode *> * heads;
		vector<RuleNode *> rules_added;
		
		bool hasBeenUsed;
	public:
		static Ontology * ontologies;
		RuleGroup(RuleNode *);
		~RuleGroup();
		
		bool ruleBelong(RuleNode *);
		bool addRule(RuleNode *);
		vector<RuleNode *> * getHeads();
		
		bool wasUsed();
		
		void mergeGroup(RuleGroup *);
		void cancelMerge();
		
		void print();
};
