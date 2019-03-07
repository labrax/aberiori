/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "rules.hpp"
#include "../parameters.hpp"

#include <cstring>

#include <iostream>
#include <algorithm>

#include <iomanip>

#include <cmath>

using std::pair;

using std::cout;
using std::endl;

using std::setw;

//note: compiling defining SEMANTIC_SIMILARITY_ALL to compute the semantic similarity for all elements grouped individually (intercession set)

//example on how to:
//#define SEMANTIC_SIMILARITY_ALL

Rules::Rules(uint64_t amount_transactions, double confidence, Ontology * ontologies, map<string, uint64_t> & itemset_1, char output_format[16], bool try_ontology_name) { //NOTE: "SCTacELDH12X" will print every information with '\n'
	this->amount_transactions = amount_transactions;
	this->confidence = confidence;
	this->ontologies = ontologies;
	this->itemset_1 = itemset_1;
	strcpy(this->output_format, output_format);
	this->try_ontology_name = try_ontology_name;
}

Rules::Rules(string file) { //TODO: implement Rules::Rules(string file)
	
}

Rules::~Rules() {
	for(auto & i : larges) {
		delete(i);
	}
	/*for(auto & r: rules) {
		delete(r);
	}*/
	for(auto & ri: rulesIteration) {
		for(auto & r: *(ri.second))
			delete(r);
		delete(ri.second);
	}
}

void Rules::addLarge(LargeItemSet * a) {
	larges.insert(larges.end(), a);
}

void Rules::computeAllRules() {
	//!create the rules from the LargeItemSets
	for(auto & l : larges) {
		if(l->getIteration() == 1) {
			//cout << "skipping large 1" << endl;
			continue;
		}
		if(l->getAmountTransactions() == 0) //finish the loop
			break;
		//cout << "generating rules for iteration " << l->getIteration() << endl;
		for(auto & i : l->getItemSets()) {
			for(auto & s : i->subItemSets()) {
				ItemSet * b = new ItemSet(i, s);
				
				RuleNode * new_node = new RuleNode(s,b);
				new_node->setNTransactions(i->getSupportCount());
				
				rules.insert(rules.end(), new_node);
			}
		}
	}
	//!create the rules from the LargeItemSets
	
	//!get the support for every part of all rules
	for(auto & r : rules) {
		r->setNTransactionsAntecedent(getFrequency(r->getItemSetAntecedent())); //support for each side
		r->setNTransactionsConsequent(getFrequency(r->getItemSetConsequent()));
		
		r->calculateConfidence(); //remaining variables
		if(r->getConfidence() < confidence)
			continue;
		r->calculateSupport(amount_transactions); //calculate the other data
		r->calculateLift(amount_transactions);
		r->calculateDepthHeight(ontologies);
		
		if(ontologies->isProcessed() == true) {
			#ifdef SEMANTIC_SIMILARITY_ALL
			calculateSemanticSimilarityAll(r);
			#else
			calculateSemanticSimilaritySide(r);
			#endif
		}
	}
	//!get the support for every part of all rules
	
	//sort the rules
	std::sort(rules.begin(), rules.end(), rulesSort);
}

void Rules::print() {
	if(rules.size() != 0) {
		printHeader();
		
		for(auto &i : rules) {
			if(i->getConfidence() < confidence)
				continue;
			
			printNode(i);
		}
	}
	else if(Parameters::verbose) {
		cout << "NO RULES GENERATED!" << endl;
	}
}

void Rules::computeRules(unsigned int iteration) {
	if(iteration <= 1) {
		if(Parameters::verbose)
			cout << "cant calculate for iteration " << iteration << endl;
		return;
	}
	else if(larges.size() < iteration-1) {
		if(Parameters::verbose)
			cout << "can not calculate because there is no large information" << endl;
		return;
	}
	else if(rulesIteration.size() >= iteration-1) {
		if(Parameters::verbose)
			cout << "rules have already been calculated for the specific generation" << endl;
		return;
	}
	
	vector<RuleNode*> * new_rules = new vector<RuleNode*>();
	
	rulesIteration.insert(pair<unsigned int, vector<RuleNode*>*>(iteration, new_rules));
	
	auto & l = larges[iteration-1];
	for(auto & i : l->getItemSets()) {
		for(auto & s : i->subItemSets()) {
			ItemSet * b = new ItemSet(i, s);
			
			RuleNode * new_node = new RuleNode(s,b);
			new_node->setNTransactions(i->getSupportCount());
			
			new_rules->insert(new_rules->end(), new_node);
		}
	}
	//!create the rules from the LargeItemSets
	
	//!get the support for every part of all rules
	for(unsigned int i=0; i < new_rules->size(); i++) {
		RuleNode * r = new_rules->at(i);
		
		r->setNTransactionsAntecedent(getFrequency(r->getItemSetAntecedent())); //support for each side
		r->setNTransactionsConsequent(getFrequency(r->getItemSetConsequent()));
		
		r->calculateConfidence(); //confidence
		
		if(r->getConfidence() < confidence) { //if the confidence is bellow value specified, remove the rule
			delete(r);
			new_rules->erase(new_rules->begin() + i);
			i--;
			continue;
		}
		r->calculateSupport(amount_transactions); //calculate the other data
		r->calculateLift(amount_transactions);
		r->calculateDepthHeight(ontologies);
		
		if(ontologies->isProcessed() == true) {
			#ifdef SEMANTIC_SIMILARITY_ALL
			calculateSemanticSimilarityAll(r);
			#else
			calculateSemanticSimilaritySide(r);
			#endif
		}
	}
	//!get the support for every part of all rules
	
	//sort the rules
	std::sort((*new_rules).begin(), (*new_rules).end(), rulesSort);
	
	if(Parameters::verbose)
		cout << "for iteration " << iteration << " size is " << (*new_rules).size() << endl;
}

void Rules::filterRules(unsigned int iteration) { //TODO: fix memory leak
	if(iteration == 1)
		return;
	map<unsigned int, vector<RuleNode*>*>::iterator it = rulesIteration.find(iteration);
	if(it == rulesIteration.end()) {
		cout << "error trying to find rules in iteration " << iteration << endl;
		return;
	}
	else {
		RuleGroup::ontologies = ontologies;
		
		vector<RuleGroup *> rule_group; //used for filtering
		
		vector<RuleNode *> * rule_vector = it->second;
		
		unsigned int removed = rule_vector->size();
		
		RuleGroup * mergeTo;
		for(auto & r : *rule_vector) {
			bool added = false;
			for(auto & e : rule_group) {
				if(added == false && e->wasUsed() == false && e->addRule(r) == true) {
					added=true;
					mergeTo = e;
					//break;
				}
				//test if there is now another group that can be joined
				else if(added == true && e->ruleBelong(r)) {
					mergeTo->mergeGroup(e);
				}
			}
			
			if(added == false) {
				rule_group.insert(rule_group.end(), new RuleGroup(r));
			}
		}
		
		//!AT THIS POINT ALL THE RULES IN ruleIteration IN INDEX [iteration-1] HAVE INVALID REFERENCE, AND MUST BE IGNORED
		rule_vector->clear();
		//!AT THIS POINT ALL THE RULES IN ruleIteration IN INDEX [iteration-1] HAVE INVALID REFERENCE, AND MUST BE IGNORED
	
		if(Parameters::debug) {
			for(auto & g: rule_group) {
				g->print();
			}
		}
		
		//!now copy the rules that have not been pruned back
		for(auto & i: rule_group) {
			if(i->wasUsed() == false) {
				for(auto & b : *i->getHeads()) {
					rule_vector->insert(rule_vector->end(), b);
				}
			}
		}
		//!now copy the rules that have not been pruned back
		removed -= rule_vector->size();
		
		if(Parameters::verbose)
			cout << removed << " removed" << endl;
			
		for(auto & rg : rule_group) { //clear the vector that was used
			delete(rg);
		}
	}
}

void Rules::print(unsigned int iteration) {
	map<unsigned int, vector<RuleNode*>*>::iterator it = rulesIteration.find(iteration);
	if(it != rulesIteration.end()) {
		if((it->second)->size() != 0) {
			printHeader();
			
			for(auto &i : *(it->second)) {
				if(i->getConfidence() < confidence)
					continue;
				
				printNode(i);
			}
		}
		else if(Parameters::verbose) {
			cout << "NO RULES GENERATED FOR ITERATION " <<  iteration << " !" << endl;
		}
	}
}

void Rules::printHeader() {
	for(unsigned int i = 0; output_format[i]!='\0'; i++) {
		switch (output_format[i]) {
			case 'S':
				cout << "support";
				break;
			case 'C':
				cout << "confidence";
				break;
			case 'T':
				cout << "transactions";
				break;
			case 'a':
				cout << "transactions_antecedent";
				break;
			case 'c':
				cout << "transactions_consequent";
				break;
			case 'E':
				cout << "semantic_similarity";
				break;
			case 'L':
				cout << "lift";
				break;
			case 'D':
				cout << "sumDepth";
				break;
			case 'H':
				cout << "sumHeight";
				break;
			case '1':
				cout << "Antecedent";
				cout << "===>";
				break;
			case '2':
				cout << "Consequent";
				break;
			case 'X':
				cout << endl;
				break;
			default:
				break;
		}
		if(output_format[i] != 'X')
			cout << '\t';
	}
}

void Rules::printNode(RuleNode * i) {
	for(unsigned int k = 0; output_format[k]!='\0'; k++) {
		switch (output_format[k]) {
			case 'S':
				cout << std::setprecision(5) << setw(7) << i->getSupport();
				break;
			case 'C':
				cout << setw(10) << i->getConfidence();
				break;
			case 'T':
				cout << setw(12) << i->getNTransactions();
				break;
			case 'a':
				cout << setw(23) << i->getNTransactionsAntecedent();
				break;
			case 'c':
				cout << setw(23) << i->getNTransactionsConsequent();
				break;
			case 'E':
				cout << setw(19) << i->getSemanticSimilarity();
				break;
			case 'L':
				cout << setw(4) << i->getLift();
				break;
			case 'D':
				cout << setw(8) << i->getSumDepth();
				break;
			case 'H':
				cout << setw(9) << i->getSumHeight();
				break;
			case '1':
				if(try_ontology_name)
					i->getItemSetAntecedent()->printWithOntology(ontologies);
				else
					i->getItemSetAntecedent()->print();
				cout << "===>";
				break;
			case '2':
				if(try_ontology_name)
					i->getItemSetConsequent()->printWithOntology(ontologies);
				else
					i->getItemSetConsequent()->print();
				break;
			case 'X':
				cout << endl;
				break;
			default:
				break;
		}
		if(output_format[k] != 'X')
			cout << '\t';
	}
}

void Rules::migrateIterationRulesToRules() {	
	for(auto & rl : rulesIteration) {
		for(auto & rs : *rl.second) {
			rules.insert(rules.end(), rs);
		}
	}
	
	std::sort(rules.begin(), rules.end(), rulesSort);
}

uint64_t Rules::getFrequency(ItemSet * itemset) {
	for(auto & p : larges[itemset->getAmountElements()-1]->getItemSets()) {
		bool ok = true;
		for(auto & e : itemset->getItemSet()) {
			if(!(p->contains(e.first))) {
				ok = false;
				break;
			}
		}
		if(ok == true) {
			return p->getSupportCount();
			break;
		}
	}
	return 0; //this condition should never occur
}

void Rules::calculateSemanticSimilaritySide(RuleNode * r) {
	if(ontologies->isProcessed() == false) {
		if(Parameters::verbose)
			cout << "no semantic similarity was calculated" << endl;
		return;
	}
		
	double similarity_intercession = 0;
	double similarity_union = 0;

	map <string, bool> * first_ontologies = NULL; //will store all the parents from the 1st itemset
	map <string, bool> * second_ontologies = NULL; //will store all the parents from the 2nd itemset
	
	for(auto & f : r->getItemSetAntecedent()->getItemSet()) { //get all the parent nodes from the 1st itemset (antecedent)
		NodeOntology * search_1 = ontologies->getNode(f.first);
		if(search_1 == NULL) {
			cout << "!!! ERROR LOOKING FOR ONTOLOGY NODE !!!" << endl;
			cout << "Check if the id \"" << f.first << "\" is in the ontology file" << endl;
		}
		map <string, bool> * result_first = search_1->returnOntologies();
		if(result_first != NULL) {
			if(first_ontologies == NULL)
				first_ontologies = result_first;
			else {
				for(auto & e : *result_first) {
					if(first_ontologies->find(e.first) == first_ontologies->end()) {
						first_ontologies->insert(pair<string, bool> (e.first, true));
					}
				}
				delete(result_first);
			}
		}
	}
	for(auto & s : r->getItemSetConsequent()->getItemSet()) { //get all the parent nodes from the 2nd itemset (consequent)
		NodeOntology * search_2 = ontologies->getNode(s.first);
		if(search_2 == NULL) {
			cout << "!!! ERROR LOOKING FOR ONTOLOGY NODE !!!" << endl;
			cout << "Check if the id \"" << s.first << "\" is in the ontology file" << endl;
		}
		map <string, bool> * result_second = search_2->returnOntologies();
		if(result_second != NULL) {
			if(second_ontologies == NULL)
				second_ontologies = result_second;
			else {
				for(auto & e : *result_second) {
					if(second_ontologies->find(e.first) == second_ontologies->end()) {
						second_ontologies->insert(pair<string, bool> (e.first, true));
					}
				}
				delete(result_second);
			}
		}
	}

	for(auto & i : *first_ontologies) { //get the intercession value
		if(second_ontologies->find(i.first) != second_ontologies->end()) {
			similarity_intercession += informationMeasure(i.first);
		}
	}
	
	//now i'll join the two sets to get the union
	for(auto & j : *second_ontologies) {
		first_ontologies->insert(pair<string, bool> (j.first, true));
	}
	//the sets are joined
	
	for(auto & i : *first_ontologies) { //get the union value
		similarity_union += informationMeasure(i.first);
	}
	
	if(first_ontologies != NULL)
		delete(first_ontologies);
	if(second_ontologies != NULL)
		delete(second_ontologies);
		
	r->setSemanticSimilarity(similarity_intercession/similarity_union);
}

void Rules::calculateSemanticSimilarityAll(RuleNode * r) {
	if(ontologies->isProcessed() == false) {
		if(Parameters::verbose)
			cout << "no semantic similarity was calculated" << endl;
		return;
	}
	
	double similarity_intercession = 0;
	double similarity_union = 0;

	vector<map<string,bool> *> ontologies_elem;
	
	for(auto & f : r->getItemSetAntecedent()->getItemSet()) { //get all the parent nodes from the 1st itemset (antecedent)
		NodeOntology * search_1 = ontologies->getNode(f.first);
		map <string, bool> * result_first = search_1->returnOntologies();
		ontologies_elem.insert(ontologies_elem.end(), result_first);
	}
	for(auto & s : r->getItemSetConsequent()->getItemSet()) { //get all the parent nodes from the 2nd itemset (consequent)
		NodeOntology * search_2 = ontologies->getNode(s.first);
		map <string, bool> * result_second = search_2->returnOntologies();
		ontologies_elem.insert(ontologies_elem.end(), result_second);
	}

	//the intercession of all sets
	for(auto & i: *(ontologies_elem[0])) {
		bool ok = true;
		for(unsigned int s=1; s<ontologies_elem.size(); s++) {
			if(ontologies_elem[s]->find(i.first) == ontologies_elem[s]->end()) {
				ok=false;
				break;
			}
		}
		if(ok == true)
			similarity_intercession += informationMeasure(i.first);
	}
	
	//the union of the sets
	for(unsigned int s=1; s<ontologies_elem.size(); s++) {
		for(auto & j: *(ontologies_elem[s]))
			ontologies_elem[0]->insert(pair<string, bool> (j.first, true));
	}
	
	for(auto & i : *(ontologies_elem[0])) { //get the union value
		similarity_union += informationMeasure(i.first);
	}
	
	for(auto & e : ontologies_elem)
		delete(e);
	
	r->setSemanticSimilarity( ((double) similarity_intercession)/ ((double) similarity_union) );
}

double Rules::informationMeasure(string identifier) {
	map <string, uint64_t>::iterator it = itemset_1.find(identifier);
	if(it == itemset_1.end()) {
		if(Parameters::debug)
			cout << identifier << " has no information measure" << endl;
		return 0;
	}
	if(Parameters::debug)
		cout << "M: " << identifier << " " << -log2((double)it->second/(double)amount_transactions) << endl;
	return (-log2((double)it->second/(double)amount_transactions));
}

vector<RuleNode *> & Rules::getRules() {
	return rules;
}

map<string, vector<RuleNode *>> & Rules::getRulesMap() { //will go through the rules and generate the map for faster search
	 //TODO: check, the elements should be sorted, so this hash will be fine
	 
	for(auto & r : rules) {
		map<string, vector<RuleNode *>>::iterator it = rulesMap.find(r->getItemSetAntecedent()->getItemSet().begin()->first);
		if(it == rulesMap.end()) {
			rulesMap.insert(pair<string, vector<RuleNode *>> (r->getItemSetAntecedent()->getItemSet().begin()->first, vector<RuleNode *>() ));
		}
		it = rulesMap.find(r->getItemSetAntecedent()->getItemSet().begin()->first); //now will get the iterator where it was inserted
		
		ItemSet * antecedent = new ItemSet(r->getItemSetAntecedent());
		ItemSet * consequent = new ItemSet(r->getItemSetConsequent());
		it->second.insert(it->second.end(), new RuleNode(antecedent, consequent));
	}
	return rulesMap;
}

bool rulesSort(const RuleNode * a, const RuleNode * b) {
	if(b->getSemanticSimilarity() == a->getSemanticSimilarity())
		return b->getConfidence() < a->getConfidence();
	return b->getSemanticSimilarity() > a->getSemanticSimilarity(); //the less the better
	//return get<0>(b).confidence < get<0>(a).confidence; //to order by confidence
}
