/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "itemset_tree.hpp"

#include <map>
#include <string>

#include <iostream>

using std::pair;
using std::map;
using std::string;

using std::cout;
using std::endl;

ItemSetTree::ItemSetTree(unsigned int depth, string identifier, ItemSetTree * father) {
	this->depth = depth;
	this->identifier = identifier;
	this->father = father;
	
	tp = itemset_node;
}

ItemSetTree::~ItemSetTree() {
	if(tp == bucket_node) {
		for(auto &i : children)
			delete(i.second);
	}
	else {
		for(auto &i : itemsets) {
			delete(i);
		}
	}
}

type ItemSetTree::getType() {
	 return tp;
}

void ItemSetTree::insertItemSet(ItemSet * a) {
	if(tp == itemset_node) { //itemset_node mode
		if(itemsets.size() <= BUCKET_THRESHOLD) { //stay itemset_node
			itemsets.insert(itemsets.end(), a); //there is no need to check for duplicates (ref: algorithm)
		}
		else { //transform into bucket_node!
			tp = bucket_node;
			for(auto &i : itemsets) {
				//will hash on the nth item
				unordered_map<string, ItemSetTree*>::iterator try_existing = children.find(i->getNthString(depth));
				if(try_existing != children.end()) {
					try_existing->second->insertItemSet(i);
				}
				else {
					ItemSetTree * child = new ItemSetTree(depth+1, i->getNthString(depth), this);
					children.insert(pair<string, ItemSetTree*>(i->getNthString(depth), child)); //initialize new node!
					child->insertItemSet(i);
				}
			}
			itemsets.clear(); //this remove the pointers, as they were already transfered
			this->insertItemSet(a); //will hit the next block
		}
	}
	else { //bucket_node mode
		unordered_map<string, ItemSetTree*>::iterator find = children.find(a->getNthString(depth));
		if(find != children.end()) { //if there is something on the hash destination
			find->second->insertItemSet(a);
		}
		else { //if there is nothing, need to add an element
			ItemSetTree * child = new ItemSetTree(depth+1, a->getNthString(depth), this);
			children.insert(pair<string, ItemSetTree*>(a->getNthString(depth), child));
			child->insertItemSet(a);
		}
	}
}

void ItemSetTree::removeItemSet(ItemSet * a) { //TODO: test code
	if(tp == itemset_node) { //itemset_node mode
		for(unsigned int i=0; i < itemsets.size(); i++) {
			if(itemsets.at(i) == a) { //delete element
				delete(itemsets.at(i));
				itemsets.erase(itemsets.begin() + i);
				break;
			}
		}
	}
	else { //bucket_node mode
		unordered_map<string, ItemSetTree*>::iterator find = children.find(a->getNthString(depth));
		if(find != children.end()) { //if there is something on the hash destination
			find->second->removeItemSet(a);
		}
	}
}

unsigned int ItemSetTree::getDepth() {
	return depth;
}

string & ItemSetTree::getIdentifier() {
	return identifier;
}

bool ItemSetTree::contains(ItemSet * a) {	
	if(tp == itemset_node) {
		for(auto &i: itemsets) {
			bool nice = true;
			for(auto &j : a->getItemSet()) {
				if(i->contains(j.first) == false) {
					nice = false;
					break;
				}
			}
			if(nice == true)
				return true;
		}
	}
	else if(tp == bucket_node) {
		for(auto &i: children) {
			if(i.second->contains(a) == true)
				return true;
		}
	}
	
	return false;
}
