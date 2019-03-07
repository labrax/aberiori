/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */
 
#include "itemset.hpp"
#include "../parameters.hpp"

#include <iostream>

using std::pair;
using std::cout;
using std::endl;

using std::lock;

ItemSet::ItemSet() {
	support_count = 0;
}

ItemSet::ItemSet(ItemSet * copy) {
	support_count = 0;
	for(auto &i : copy->getItemSet()) {
		itemset.insert(pair<string, bool>(i.first, true));
	}
}

ItemSet::ItemSet(ItemSet * original, ItemSet * without) {
	support_count = 0;
	for(auto & j : original->getItemSet()) {
		if(without->contains(j.first) == false) {
			itemset.insert(pair<string, bool>(j.first, true));
		}
	}
}

bool ItemSet::insert(string elem) {
	if(itemset.find(elem) == itemset.end()) {
		itemset.insert(pair<string, bool>(elem, true));
		return true;
	}
	else
		return false;
}

bool ItemSet::remove(string elem) {
	map<string, bool>::iterator find = itemset.find(elem);
	
	if(find == itemset.end())
		return false;
	else {
		itemset.erase(find);
		return true;
	}
}

map <string, bool> & ItemSet::getItemSet() {
	return itemset;
}

unsigned int ItemSet::getAmountElements() {
	return itemset.size();
}

void ItemSet::setSupportCount(uint64_t support_count) {
	this->support_count = support_count;
}

uint64_t ItemSet::getSupportCount() {
	return support_count;
}

void ItemSet::increaseSupportCount() {
	sp_lock.lock();
	support_count++;
	sp_lock.unlock();
}

string ItemSet::getNthString(unsigned int n) {
	unsigned int i=0;
	
	string last = itemset.rbegin()->first;
	map<string, bool>::iterator it = itemset.begin();
	do {
		if(i == n)
			return it->first;
		else
			i++;
	} while(itemset.key_comp()((*it++).first, last));
	
	
	return "";
}

/* bool ItemSet::remove(string elem) {
	map <string, bool>::iterator it = itemset.find(elem);
	if(it != itemset.end()) {
		itemset.erase(it);
		return true;
	}
	else
		return false;
}*/

bool ItemSet::contains(string elem) {
	if(itemset.find(elem) != itemset.end())
		return true;
	else
		return false;
}

vector<ItemSet *> ItemSet::subItemSets() {
	vector<ItemSet *> subsets;
	
	string last = itemset.rbegin()->first;
	map<string, bool>::iterator it = itemset.begin();
	do {
		if(subsets.size() == 0) {
			ItemSet * a0 = new ItemSet();
			ItemSet * a1 = new ItemSet();
			a1->insert(it->first);

			subsets.insert(subsets.end(), a0);
			subsets.insert(subsets.end(), a1);
		}
		else {
			unsigned int old_size = subsets.size();
			for(unsigned int i=0; i<old_size; i++) {
				ItemSet * a0 = new ItemSet(subsets[i]);
				a0->insert(it->first);
				subsets.insert(subsets.end(), a0);
			}
		}
	} while(itemset.key_comp()((*it++).first, last));
	
	
	delete(subsets[0]); //the empty set was needed in the beginning, now deleting
	subsets.erase(subsets.begin());
	
	delete(subsets[subsets.size()]); //the whole original ItemSet is not needed
	subsets.erase(--subsets.end());

	//cerr << "itemset.size() " << itemset.size() << "\tsubsets.size() " << subsets.size() << endl;
	return subsets;
}

vector<ItemSet *> ItemSet::subItemSets(unsigned int maximum_elements) {
	vector<ItemSet *> subsets;
	
	string last = itemset.rbegin()->first;
	map<string, bool>::iterator it = itemset.begin();
	do {
		if(subsets.size() == 0) {
			ItemSet * a0 = new ItemSet();
			ItemSet * a1 = new ItemSet();
			a1->insert(it->first);

			subsets.insert(subsets.end(), a0);
			subsets.insert(subsets.end(), a1);
		}
		else {
			unsigned int old_size = subsets.size();
			for(unsigned int i=0; i<old_size; i++) {
				if(subsets[i]->getAmountElements() < maximum_elements) {
					ItemSet * a0 = new ItemSet(subsets[i]);
					a0->insert(it->first);
					subsets.insert(subsets.end(), a0);
				}
			}
		}
	} while(itemset.key_comp()((*it++).first, last));
	
	
	delete(subsets[0]); //the empty set was needed in the beginning, now deleting
	subsets.erase(subsets.begin());
	
	delete(subsets[subsets.size()]); //the whole original ItemSet is not needed
	subsets.erase(--subsets.end());

	//cerr << "itemset.size() " << itemset.size() << "\tsubsets.size() " << subsets.size() << endl;
	return subsets;
}

vector<ItemSet *> ItemSet::subItemSets(unsigned int minimum_elements, unsigned int maximum_elements) {
	vector<ItemSet *> subsets;
	
	string last = itemset.rbegin()->first;
	map<string, bool>::iterator it = itemset.begin();
	do {
		if(subsets.size() == 0) {
			ItemSet * a0 = new ItemSet();
			ItemSet * a1 = new ItemSet();
			a1->insert(it->first);

			subsets.insert(subsets.end(), a0);
			subsets.insert(subsets.end(), a1);
		}
		else {
			unsigned int old_size = subsets.size();
			for(unsigned int i=0; i<old_size; i++) {
				if(subsets[i]->getAmountElements() < maximum_elements) {
					ItemSet * a0 = new ItemSet(subsets[i]);
					a0->insert(it->first);
					subsets.insert(subsets.end(), a0);
				}
			}
		}
	} while(itemset.key_comp()((*it++).first, last));
	
	
	delete(subsets[0]); //the empty set was needed in the beginning, now deleting
	subsets.erase(subsets.begin());
	
	delete(subsets[subsets.size()]); //the whole original ItemSet is not needed
	subsets.erase(--subsets.end());

	for(unsigned int i=0; i < subsets.size(); i++) {
		if(subsets[i]->getAmountElements() < minimum_elements) { //if has not the required amount, delete
			delete(subsets[i]);
			i--; //fix ordering
		}
	}

	//cerr << "itemset.size() " << itemset.size() << "\tsubsets.size() " << subsets.size() << endl;
	return subsets;
}

void ItemSet::print() {
	for(auto &j: getItemSet()) {
		cout << j.first << " ";
	}
	if(Parameters::verbose)
		cout << "(" << support_count << ") ";
}

void ItemSet::printWithOntology(Ontology * ontologies) {
	for(auto &j: getItemSet()) {
		if(ontologies->isProcessed()) {
			NodeOntology * node = ontologies->getNode(j.first);
			if(node != NULL) {
				if(node->isObsolete())
					cout << j.first << " *obsolete classification* (" << node->getName() << ") ";
				else
					cout << j.first << " (" << node->getName() << ") ";
			}
			else
				cout << j.first << " (?) ";
		}
		else {
			cout << j.first << " ";
		}
	}
	if(Parameters::debug)
		cout << " (#SUPPORT:" << support_count << ") ";
}

bool ItemSetSort(ItemSet * c0, ItemSet * c1) {
	map<string, bool>::iterator i0, i1;
	for(i0 = c0->getItemSet().begin(), i1 = c1->getItemSet().begin(); i0 != c0->getItemSet().end() && i1 != c1->getItemSet().end(); i0++, i1++) {
		if(i0->first != i1->first)
			return i0->first < i1->first;
	}
	return i0->first < i1->first;
}
