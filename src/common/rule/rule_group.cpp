/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "rule_group.hpp"
#include "../parameters.hpp"

#include <iostream>

using std::cout;
using std::endl;

Ontology * RuleGroup::ontologies = NULL;

RuleGroup::RuleGroup(RuleNode * rule) { //initiate group, allocating proper memory
	//RuleNode * new_rule = new RuleNode(*rule);
	heads = new vector<RuleNode *>();
	heads->insert(heads->end(), rule);
	ancestor_antecedent = new ItemSet(rule->getItemSetAntecedent());
	ancestor_consequent = new ItemSet(rule->getItemSetConsequent());
	hasBeenUsed = false;
}

RuleGroup::~RuleGroup() {
	for(auto & r: rules_added) {
		delete(r);
	}
	if(hasBeenUsed == false) { //if they were not used, free
		for(auto & r: *heads) {
			delete(r);
		}
	}
	delete(heads);
	delete(ancestor_antecedent);
	delete(ancestor_consequent);
}

bool RuleGroup::ruleBelong(RuleNode * rule) { //this function returns true if the rule belong to this group (match antecedent and consequent hierarchy)
	for(auto & a : rule->getItemSetAntecedent()->getItemSet()) {//check the antecedent part
		bool match = false;
		for(auto & ta : ancestor_antecedent->getItemSet()) {
			if(ontologies->checkAncestorOneAnother(a.first, ta.first)) {
				match = true;
				break;
			}
		}
		if(match == false)
			return false;
	}
	for(auto & c : rule->getItemSetConsequent()->getItemSet()) {//check the consequent part
		bool match = false;
		for(auto & tc : ancestor_consequent->getItemSet()) {
			if(ontologies->checkAncestorOneAnother(c.first, tc.first)) {
				match = true;
				break;
			}
		}
		if(match == false)
			return false;
	}
	return true;
}

bool RuleGroup::addRule(RuleNode * rule) {
	//this function tests if the rule belongs to the group, if it does then add, if not, return false
	//also modify descendent_ontologies
	
	if(this->ruleBelong(rule) == false) //the rule does not belong to this group
		return false;
	else {
		/** the approach to solve this problem uses mostly the relation of the ontologies
		 * 
		 *  another way, that could be faster (and simpler) is using the sumHeight of each element,
		 *  after checking that they belong to the same hierarchy (and not checking if is son again, multiple times)
		 */
		for(auto & a : rule->getItemSetAntecedent()->getItemSet()) { //update the youngest for each element
			for(auto & ta : ancestor_antecedent->getItemSet()) {
				if(ontologies->checkSon(ta.first, a.first)) {
					ancestor_antecedent->remove(ta.first);
					ancestor_antecedent->insert(a.first);
					break;
				}
			}
		}
		for(auto & c : rule->getItemSetConsequent()->getItemSet()) {
			for(auto & tc : ancestor_consequent->getItemSet()) {
				if(ontologies->checkSon(tc.first, c.first)) {
					ancestor_consequent->remove(tc.first);
					ancestor_consequent->insert(c.first);
					break;
				}
			}
		}
		
		bool added = false; //check if the element was added
		bool may_be_head = false; //check if the element can be head: to the verification, if a rule is in the same level of another one
		
		/**TODO: check if it is valid to add the following to RuleGroup:
		 * may_be_head: a rule may be head from a group
		 * may_not_be_head: a rule may not (it is a son-rule from another one)
		 * 
		 * right now in the 'heads' are only the better than actual ones and the 'match-ranking' ones
		 * 
		 * wouldn't it be better if in the head were also the ones that win in one element from any of the head?
		 *       or only the ones that win in all?
		**/
		for(unsigned int i=0; i < heads->size(); i++) { //check if the new rule is better than any of the heads
			auto & h = heads->at(i);
			int rank = 0;
			for(auto & t : rule->getItemSetAntecedent()->getItemSet()) {
				for(auto & fh : h->getItemSetAntecedent()->getItemSet()) {
					if(t.first == fh.first)
						break;
					else if(ontologies->checkAncestorOneAnother(t.first, fh.first)) {
						if(ontologies->checkSon(t.first, fh.first))
							rank++;
						else
							rank--;
						break;
					}
				}
			}
			for(auto & t : rule->getItemSetConsequent()->getItemSet()) {
				for(auto & fh : h->getItemSetConsequent()->getItemSet()) {
					if(t.first == fh.first)
						break;
					else if(ontologies->checkAncestorOneAnother(t.first, fh.first)) {
						if(ontologies->checkSon(t.first, fh.first))
							rank++;
						else
							rank--;
						break;
					}
				}
			}
			if(rank > 0) { //TODO: modify heuristic for rules choosing? (any rule that beat on maiority is better right now)
				rules_added.insert(rules_added.end(), h); //move the head rule to the other ones
				
				heads->erase(heads->begin() + i); //remove the rule from the head

				heads->insert(heads->end(), rule); //add new head
				added = true;
				break;
			}
			else if(rank == 0) { //if there is a rule which is not whole parent of another it may be another head of the group
				may_be_head = true;
			}
		}
		if(added == false && may_be_head == true) {
			heads->insert(heads->end(), rule);
		}
		else if(added == false) { //if it is not better than the heads, add in the other struct
			rules_added.insert(rules_added.end(), rule);
		}
	}
	return true;
}

vector<RuleNode *> * RuleGroup::getHeads() {
	hasBeenUsed = true;
	return heads;
}

bool RuleGroup::wasUsed() {
	return hasBeenUsed;
}

void RuleGroup::mergeGroup(RuleGroup * rule_group) {
	if(rule_group->wasUsed() == false) {
		bool ok = true;
		for(auto & e : *(rule_group->getHeads())) {
			if(ruleBelong(e) == false) {
				ok = false;
				break;
			}
		}
		if(ok == true) {
			for(auto & e : *(rule_group->getHeads())) {
				if(addRule(e) == false) {
					cout << ">> Error merging group. Probably error in logic: " __FILE__ << " " << __LINE__ << endl;
				}
			}
			rule_group->getHeads()->clear();
		}
		else
			rule_group->cancelMerge();
	}
}

void RuleGroup::cancelMerge() {
	hasBeenUsed = false;
}

void RuleGroup::print() {
	if(hasBeenUsed == true)
		cout << "--this group's heads have already been used--" << endl;
		
	cout << "group ancestors: ";
	ancestor_antecedent->printWithOntology(ontologies);
	cout << "===>\t";
	ancestor_consequent->printWithOntology(ontologies);
	cout << endl;
	cout << "printing heads" << endl;
	for(auto & h : *heads) {
		cout << "   "
			 << h->getConfidence() << "\t" << h->getSumDepth() << "\t" << h->getSumHeight() << "\t";
		h->getItemSetAntecedent()->printWithOntology(ontologies);
		cout << "===>\t";
		h->getItemSetConsequent()->printWithOntology(ontologies);
		cout << endl;
	}
	if(rules_added.size() > 0) {
		cout << "  printing others" << endl;
		for(auto & o : rules_added) {
			cout << "     "
				 << o->getConfidence() << "\t" << o->getSumDepth() << "\t" << o->getSumHeight() << "\t";
			o->getItemSetAntecedent()->printWithOntology(ontologies);
			cout << "===>\t";
			o->getItemSetConsequent()->printWithOntology(ontologies);
			cout << endl;
		}
	}
	
	if(hasBeenUsed == true)
		cout << "!!this group's head has already been used!!" << endl;
}
