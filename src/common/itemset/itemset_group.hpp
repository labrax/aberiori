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
#include <vector>

using std::vector;

class ItemSetGroup {
	private:
		ItemSet * ancestors;
		
		vector<ItemSet *> * heads;
		vector<ItemSet *> itemsets_added;
		
		bool hasBeenUsed;
	public:
		static Ontology * ontologies;
		
		ItemSetGroup(ItemSet *);
		~ItemSetGroup();
		
		bool itemsetBelong(ItemSet *);
		bool addItemSet(ItemSet *);
		vector<ItemSet *> * getHeads();
		
		bool wasUsed();
		
		void mergeGroup(ItemSetGroup *);
		void cancelMerge();
		
		void print();
};
