/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

//TODO: join this class, ItemSetGroup with RuleGroup using templates

#include "itemset_group.hpp"
#include "../parameters.hpp"

#include <iostream>

using std::cout;
using std::endl;

Ontology * ItemSetGroup::ontologies = NULL;

ItemSetGroup::ItemSetGroup(ItemSet * itemset) { //initiate group, allocating proper memory
	//RuleNode * new_itemset = new RuleNode(*itemset); //it is considered that all objects received here are ours until removed
	heads = new vector<ItemSet *>();
	heads->insert(heads->end(), itemset);
	ancestors = new ItemSet(itemset);
	hasBeenUsed = false;
}

ItemSetGroup::~ItemSetGroup() {
	for(auto & r: itemsets_added) {
		delete(r);
	}
	if(hasBeenUsed == false) { //if they were not used, free
		for(auto & r: *heads) {
			delete(r);
		}
	}
	delete(heads);
	delete(ancestors);
}

bool ItemSetGroup::itemsetBelong(ItemSet * itemset) { //this function returns true if the itemset belongs to this group
	for(auto & a : itemset->getItemSet()) {//check the itemset
		bool match = false;
		for(auto & ta : ancestors->getItemSet()) {
			if(ontologies->checkAncestorOneAnother(a.first, ta.first)) {
				match = true;
				break;
			}
		}
		if(match == false) {
			//cout << a.first << " didnt match" << endl;
			return false;
		}
	}
	return true;
}

bool ItemSetGroup::addItemSet(ItemSet * itemset) {
	//this function tests if the itemset belongs to the group, if it does then add, if not, return false
	//also modify descendent_ontologies
	
	if(this->itemsetBelong(itemset) == false) //the itemset does not belong to this group
		return false;
	else {
		/** the approach to solve this problem uses mostly the relation of the ontologies
		 * 
		 *  another way, that could be faster (and simpler) is using the sumHeight of each element,
		 *  after checking that they belong to the same hierarchy (and not checking if is son again, multiple times)
		 */
		for(auto & a : itemset->getItemSet()) { //update the youngest for each element
			for(auto & ta : ancestors->getItemSet()) {
				if(ontologies->checkSon(ta.first, a.first)) {
					ancestors->remove(ta.first);
					ancestors->insert(a.first);
					break;
				}
			}
		}
		
		bool added = false; //check if the element was added
		bool may_be_head = false; //check if the element can be head: to the verification, if an itemset is in the same level of another one
		
		/**TODO: check if it is valid to add the following to RuleGroup:
		 * may_be_head: an itemset may be head from a group
		 * may_not_be_head: an itemset may not (it is a son-itemset from another one)
		 * 
		 * right now in the 'heads' are only the better than actual ones and the 'match-ranking' ones
		 * 
		 * wouldn't it be better if in the head were also the ones that win in one element from any of the head?
		 *       or only the ones that win in all?
		**/
		for(unsigned int i=0; i < heads->size(); i++) { //check if the new itemset is better than any of the heads
			auto & h = heads->at(i);
			int rank = 0;
			for(auto & t : itemset->getItemSet()) {
				for(auto & fh : h->getItemSet()) {
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
			if(rank > 0) { //TODO: modify heuristic for itemsets choosing? (any itemset that beat on maiority is better right now)
				itemsets_added.insert(itemsets_added.end(), h); //move the head itemset to the other ones
				
				heads->erase(heads->begin() + i); //remove the itemset from the head

				heads->insert(heads->end(), itemset); //add new head
				added = true;
				break;
			}
			else if(rank == 0) { //if there is a itemset which is not whole parent of another it may be another head of the group
				may_be_head = true;
			}
		}
		if(added == false && may_be_head == true) {
			heads->insert(heads->end(), itemset);
		}
		else if(added == false) { //if it is not better than the heads, add in the other struct
			itemsets_added.insert(itemsets_added.end(), itemset);
		}
	}
	return true;
}

vector<ItemSet *> * ItemSetGroup::getHeads() {
	hasBeenUsed = true;
	return heads;
}

bool ItemSetGroup::wasUsed() {
	return hasBeenUsed;
}

void ItemSetGroup::mergeGroup(ItemSetGroup * itemset_group) {
	if(itemset_group->wasUsed() == false) {
		bool ok = true;
		for(auto & e : *(itemset_group->getHeads())) {
			if(itemsetBelong(e) == false) {
				ok = false;
				break;
			}
		}
		if(ok == true) {
			for(auto & e: *(itemset_group->getHeads())) {
				if(addItemSet(e) == false) {
					cout << ">> Error merging group. Probably error in logic: " __FILE__ << " " << __LINE__ << endl;
				}
			}
			itemset_group->getHeads()->clear();
		}
		else
			itemset_group->cancelMerge();
	}
}

void ItemSetGroup::cancelMerge() {
	hasBeenUsed = false;
}

void ItemSetGroup::print() {
	if(hasBeenUsed == true)
		cout << "--this group's heads have already been used--" << endl;
		
	cout << "group ancestors: ";
	ancestors->printWithOntology(ontologies);
	cout << endl;
	cout << "printing heads" << endl;
	for(auto & h : *heads) {
		cout << "   "
			 << h->getSupportCount() << "\t";
		h->printWithOntology(ontologies);
		cout << endl;
	}
	if(itemsets_added.size() > 0) {
		cout << "  printing others" << endl;
		for(auto & o : itemsets_added) {
			cout << "     "
				 << o->getSupportCount() << "\t";
			o->printWithOntology(ontologies);
			cout << endl;
		}
	}
	
	if(hasBeenUsed == true)
		cout << "!!this group's head has already been used!!" << endl;
}
