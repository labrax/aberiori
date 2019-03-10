/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "candidate_tree.hpp"

#include <unordered_map>
#include <string>

using std::pair;
using std::unordered_map;
using std::string;

CandidateTree::CandidateTree(unsigned int depth, string identifier, CandidateTree * father) : ItemSetTree(depth, identifier, father) {
}

CandidateTree::~CandidateTree() {
}

void CandidateTree::transactionScan(vector <pair <string, string>> * transaction) {
	if(tp == itemset_node) {
		for(auto &i : itemsets) {
			//if itemset is contained in transaction, it is cool
			
			bool cool = true; //for the itemset
			for(auto &e : i->getItemSet()) {
				bool found = false; //for each element
				for(uint64_t t=0; t<transaction->size(); t++) {
					if((*transaction)[t].second == e.first) { //element is in transaction
						found = true;
						break;
					}
				}
				if(found == false) {
					cool = false;
					break;
				}
			}
			if(cool == true) {
				i->increaseSupportCount();
				//cout << "updating support to " << i->getSupportCount() << endl;
			}
		}
	}
	else if(tp == bucket_node) {		
		/*
		for(auto &i : children) { //paranoid mode
			i.second->transactionScan(transaction);
		}
		*/
		
		for(uint64_t i=0; i<transaction->size(); i++) { //use the tree!
			unordered_map<string, ItemSetTree*>::iterator find = children.find((*transaction)[i].second);
			
			if(find != children.end()) { //if there is something on the hash destination
				((CandidateTree *)find->second)->transactionScan(transaction);
			}
		}
	}
}

void CandidateTree::grabMinimumMaximumSupport(LargeItemSet * dest, uint64_t min_transactions, uint64_t max_transactions) {
	if(tp == itemset_node) {
		for(auto &i : itemsets) {
			if(i->getSupportCount() > min_transactions && i->getSupportCount() <= max_transactions) {
				ItemSet * e  = new ItemSet(i); //to avoid memory leak
				e->setSupportCount(i->getSupportCount());
				dest->insertSet(e);
				//cout << "found one good!" << endl;
			}
		}
	}
	else if(tp == bucket_node) {
		for(auto &i : children) {
			((CandidateTree *)i.second)->grabMinimumMaximumSupport(dest, min_transactions, max_transactions);
		}
	}
}
