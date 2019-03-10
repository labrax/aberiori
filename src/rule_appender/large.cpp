/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */
 
#include "large.hpp"

#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;

LargeItemSet::LargeItemSet(uint64_t iteration) {
	this->k = iteration;

	root = new ItemSetTree(0, "", NULL);
}

LargeItemSet::~LargeItemSet() {
	for(auto &i : itemset) {
		delete(i);
	}

	delete(root);
}

void LargeItemSet::insertSet(ItemSet * set) {
	large_lock.lock();
	
	itemset.insert(itemset.end(), set);
	
	ItemSet * e = new ItemSet(set);
	root->insertItemSet(e);
	
	large_lock.unlock();
}

vector<ItemSet *> & LargeItemSet::getItemSets() {
	return itemset;
}

uint64_t LargeItemSet::getAmountTransactions() {
	return itemset.size();
}

uint64_t LargeItemSet::getIteration() {
	return k;
}

bool LargeItemSet::contains(ItemSet * a) {
	return root->contains(a);
	
	/*for(auto &i : itemset) { //this way doesn't utilize the tree
		bool cool = true;
		for(auto &j : a->getItemSet()) {
			if(i->contains(j.first) == false) {
				cool = false;
				break;
			}
		}
		if(cool == true) {
			return true;
		}
	}
	return false;*/
}

void LargeItemSet::print() {
	for(auto &i : itemset) {
		i->print();
		cout << "#SUP: " << i->getSupportCount() << endl;
	}
}

void LargeItemSet::printinfo() {
	if(this->getItemSets().size() != 0)
		cout << "iteraction number: " << this->getIteration() << " results in " << this->getItemSets().size() << " itemsets" << endl;
	else
		cout << "no more itemsets" << endl;

}

void LargeItemSet::sort() {
	std::sort(itemset.begin(), itemset.end(), ItemSetSort);
}
