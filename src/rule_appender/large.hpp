/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

/** a general class for storing a set of k-itemsets
 */

#pragma once

#include "../common/itemset/itemset.hpp"
#include "../common/itemset/itemset_tree.hpp"

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
		
		vector<ItemSet *> & getItemSets();
		uint64_t getAmountTransactions();
		uint64_t getIteration();
		
		bool contains(ItemSet *);
		
		void print();
		void printinfo();
		
		void sort();
};
