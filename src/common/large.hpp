/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */


//note: this is a dummy header


/** a general class for storing a set of k-itemsets
 */

#ifndef _LARGE_H
#define _LARGE_H

#include "../common/itemset/itemset.hpp"
#include "../common/itemset/itemset_tree.hpp"
#include "../common/itemset/itemset_group.hpp"
#include "../common/ontologies/ontology.hpp"

#include "../common/parameters.hpp"

#include <vector>
#include <mutex>

#include <cstdint>

using std::vector;

using std::mutex;


class LargeItemSet {
	private:
		vector<ItemSet *> itemset;
		ItemSetTree * root; //new approach to solve the slowdown with the function bool contains(ItemSet *);
		
		uint64_t k; //to identify the iteration, also the size
		
		mutex large_lock; //to lock insertSet (apriori_genThreaded)
	public:
		LargeItemSet(uint64_t);
		~LargeItemSet();
		void insertSet(ItemSet * set);
		void removeSet(ItemSet * set);
		void filterSet(Ontology * ontologies); //TODO: implement LargeItemSet::filterSet(Ontology * ontologies)
		
		vector<ItemSet *> & getItemSets();
		uint64_t getAmountTransactions();
		uint64_t getIteration();
		
		bool contains(ItemSet *);
		
		void print();
		void printinfo();
		
		void sort();
};

#endif
