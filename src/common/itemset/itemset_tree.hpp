/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include "itemset.hpp"

#include <string>
#include <unordered_map>
#include <vector>

#define BUCKET_THRESHOLD 15

using std::string;
using std::unordered_map;
using std::vector;

typedef enum { node, bucket_node, itemset_node } type;

class ItemSetTree {
	private:
		unsigned int depth;
		string identifier;
		ItemSetTree * father;
		
	protected:
		type tp;
		unordered_map<string, ItemSetTree *> children; //will be used in the bucket_node mode
		vector<ItemSet *> itemsets; //will be used for the itemset_node mode
		
	public:
		ItemSetTree(unsigned int depth, string identifier, ItemSetTree * father);
		~ItemSetTree();
		
		type getType();
		
		void insertItemSet(ItemSet *);
		void removeItemSet(ItemSet *);
		
		unsigned int getDepth();
		string & getIdentifier();
		
		bool contains(ItemSet *); //recursive without using the tree properties
};
