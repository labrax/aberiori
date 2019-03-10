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
	root->insertItemSet(new ItemSet(set));
	large_lock.unlock();
}

void LargeItemSet::removeSet(ItemSet * set) {
	for(unsigned int i=0; i<itemset.size(); i++) {
		if(itemset.at(i) == set) {
			delete(itemset.at(i));
			itemset.erase(itemset.begin() + i);
			break;
		}
	}
	root->removeItemSet(set);
}

void LargeItemSet::filterSet(Ontology * ontologies) { 
	//problems: remove itemset from the Tree of data; otherwise it will be fine
	
	ItemSetGroup::ontologies = ontologies;
	
	vector<ItemSetGroup *> itemset_group; //used for filtering
	
	unsigned int removed = itemset.size();
	
	ItemSetGroup * mergeTo;
	for(auto & r : itemset) {
		bool added = false;
		for(auto & e : itemset_group) {
			if(added == false && e->wasUsed() == false && e->addItemSet(r) == true) {
				added=true;
				mergeTo = e;
				//break;
			}
			//test now if there is another group that can be joined
			else if(added == true && e->itemsetBelong(r)) {
				mergeTo->mergeGroup(e);
			}
		}
		
		if(added == false) {
			itemset_group.insert(itemset_group.end(), new ItemSetGroup(r));
		}
	}
	
	//!AT THIS POINT ALL THE ITEMSETS IN HAVE INVALID REFERENCE, AND MUST BE IGNORED
	itemset.clear();
	delete(root);
	root = new ItemSetTree(0, "", NULL);
	//!AT THIS POINT ALL THE ITEMSETS IN HAVE INVALID REFERENCE, AND MUST BE IGNORED

	if(Parameters::debug) {
		for(auto & g: itemset_group) {
			g->print();
		}
	}
	
	//!now copy the rules that have not been pruned back
	for(auto & i: itemset_group) {
		if(i->wasUsed() == false) {
			for(auto & b : *i->getHeads()) {
				itemset.insert(itemset.end(), b);
				root->insertItemSet(new ItemSet(b));
			}
		}
	}
	//!now copy the rules that have not been pruned back
	removed -= itemset.size();
	
	if(Parameters::verbose)
		cout << removed << " removed" << endl;
		
	for(auto & rg : itemset_group) { //clear the vector that was used
		delete(rg);
	}
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
