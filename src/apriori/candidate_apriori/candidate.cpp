/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

/**
 * TODO: this code includes a very poor way of threading, one way of obtaining more stable results would be divinding it better...
 * 
 */

#include "candidate.hpp"
#include "../parameters.hpp"

#include <vector>
#include <iostream>
#include <thread>

using std::vector;

using std::cout;
using std::endl;

using std::thread;

CandidateItemSet::CandidateItemSet(Ontology & ontologies) : ontologies(ontologies) {
	root = new CandidateTree(0, "", NULL);
}

CandidateItemSet::~CandidateItemSet() {
	delete(root);
}

LargeItemSet * CandidateItemSet::apriori_gen(LargeItemSet * a) {
	LargeItemSet * new_candidate = new LargeItemSet(a->getIteration()+1);
	
	vector<ItemSet *> last_large = a->getItemSets();

	run_apriori_genThreaded(&ontologies, new_candidate, a, &last_large, 0, last_large.size());
	
	return new_candidate;
}

LargeItemSet * CandidateItemSet::apriori_genThreaded(LargeItemSet * a) {
	LargeItemSet * new_candidate = new LargeItemSet(a->getIteration()+1);
	vector<ItemSet *> last_large = a->getItemSets();
	
	unsigned concurentThreadsSupported = Parameters::thread_number;
	
	vector <thread *> threads;
	/*vector <LargeItemSet *> new_candidate_thread;
	for(uint64_t i=0; i < concurentThreadsSupported; i++) {
		new_candidate_thread.insert(new_candidate_thread.end(), new LargeItemSet(a->getIteration()+1));
	}*/
	
	uint64_t NUM_EACH_THREAD = amount_op(last_large.size(), 0, last_large.size())/concurentThreadsSupported;
	
	//cout << "transactions: " << a->getAmountTransactions() << endl;
	
	uint64_t block_init = 0, block_end = 0;
	while(threads.size() < concurentThreadsSupported-1 && block_init != last_large.size()) { //will get until before the last thread
		block_end = block_init;
		
		while(amount_op(last_large.size(), block_init, block_end) < NUM_EACH_THREAD)
			block_end++;

		/*block_end--; //try to compensate
		if(block_end == block_init)
			block_end++;*/
		
		//cout << "thread: " << block_init << ", " << block_end << endl;
		threads.insert(threads.end(), new thread(run_apriori_genThreaded, &ontologies, new_candidate/*_thread[threads.size()]*/, a, &last_large, block_init, block_end));
		
		block_init = block_end;
	}
	if(block_end != last_large.size()) { //put the last thread with the remaining itemsets
		//cout << "thread: " << block_end << ", " << a->getAmountTransactions() << endl;
		threads.insert(threads.end(), new thread(run_apriori_genThreaded, &ontologies, new_candidate/*_thread[threads.size()]*/, a, &last_large, block_end, last_large.size()));
	}
	
	for(auto & t : threads)
		t->join();

	for(auto & t : threads)
		delete(t);
		
	/*for(auto & t : new_candidate_thread) {
		new_candidate->getItemSets().insert(new_candidate->getItemSets().end(), t->getItemSets().begin(), t->getItemSets().end());
		t->getItemSets().clear();
		delete(t);
	}*/
		
	return new_candidate;
}

LargeItemSet * CandidateItemSet::subset(LargeItemSet * a, vector <pair <string, string>> * normalized_transactions, uint64_t min_transactions, uint64_t max_transactions) {
	for(auto &i: a->getItemSets()) {
		ItemSet * e = new ItemSet(i);
		root->insertItemSet(e);
	}
	LargeItemSet * new_large = new LargeItemSet(a->getIteration());

	run_subsetThreaded(root, normalized_transactions, 0, normalized_transactions->size());
	
	root->grabMinimumMaximumSupport(new_large, min_transactions, max_transactions);
	
	return new_large;
}

LargeItemSet * CandidateItemSet::subsetThreaded(LargeItemSet * a, vector <pair <string, string>> * normalized_transactions, uint64_t min_transactions, uint64_t max_transactions) {
	for(auto &i: a->getItemSets()) {
		ItemSet * e = new ItemSet(i);
		root->insertItemSet(e);
	}
	LargeItemSet * new_large = new LargeItemSet(a->getIteration());
	
	//-------------- initialize thread blocks settings for subset function
	static vector <pair<uint64_t, uint64_t>> thread_blocks;
	
	if(thread_blocks.size() == 0) {
		//cout << "--THREADS INFO--" << endl << Parameters::thread_number << " concurrent threads will be used" << endl;
		
		uint64_t initial=0;
		while(thread_blocks.size() < Parameters::thread_number-1) {
			uint64_t new_initial = normalized_transactions->size()*thread_blocks.size()/Parameters::thread_number;
			if(new_initial < initial)
				new_initial = initial;

			uint64_t new_end = normalized_transactions->size()*(thread_blocks.size()+1)/Parameters::thread_number;
			for(initial = new_end; initial < normalized_transactions->size() && (*normalized_transactions)[initial].first == (*normalized_transactions)[new_end].first; ++initial);
			
			thread_blocks.insert(thread_blocks.end(), pair<uint64_t, uint64_t>(new_initial, initial));
		}
		thread_blocks.insert(thread_blocks.end(), pair<uint64_t, uint64_t>(initial, normalized_transactions->size()));
	}
	
	/*for(auto & i: thread_blocks) {
		cout << "subset thread has range: " << i.first << " to " << i.second << endl;
	}
	cout << "--THREADS INFO--" << endl;*/
	//--------------
	
	vector<thread *> threads;
	
	for(uint64_t i=0; i<thread_blocks.size(); i++) {
		//cout << "thread : " << thread_blocks[i].first << " " << thread_blocks[i].second << endl;
		threads.insert(threads.end(), new thread(run_subsetThreaded, root, normalized_transactions, thread_blocks[i].first, thread_blocks[i].second));
	}
	
	for(auto & t: threads)
		t->join();

	for(auto & t: threads)
		delete(t);
	
	//cout << "scanning tree for support" << endl;
	root->grabMinimumMaximumSupport(new_large, min_transactions, max_transactions);
	
	return new_large;
}


void run_apriori_genThreaded(Ontology * ontologies, LargeItemSet * new_candidate, LargeItemSet * a, vector<ItemSet *> * last_large, uint64_t rangeLow, uint64_t rangeHigh) {	
	for(uint64_t i=rangeLow; i<rangeHigh; i++) {
		ItemSet * first = (*last_large)[i];
		for(uint64_t j=i+1; j<last_large->size(); j++) {
			ItemSet * second = (*last_large)[j];
			
			//!this will test if they are similar until the last element
			bool nice = true;
			
			string last_first = first->getItemSet().rbegin()->first;
			map<string, bool>::iterator it = first->getItemSet().begin();
			map<string, bool>::iterator it2 = second->getItemSet().begin();
			
			string last_second = second->getItemSet().rbegin()->first; //!test ontologies
			
			while(it->first != last_first) { //while is not the last
				if(it->first != it2->first) {//its differente, we are not cool
					nice = false;
					break;
				}	
				it++; //increment both iterators
				it2++;
			}
			//!this will test if they are similar until the last element
			
			if(ontologies->isProcessed()) {
				if(nice == true) { //test ontologies correctly!
					for(auto & otest: (*last_large)[i]->getItemSet()) {
						if(ontologies->checkAncestorOneAnother(otest.first, last_second) == true) {
							nice = false;
							break;
						}
					}
				}
			}
			
			if(nice == true) {
				ItemSet * new_itemset = new ItemSet(first); //create new itemset
				new_itemset->insert(it2->first); //with new element
				
				bool ok = true; //!this block will prune candidates
				vector<ItemSet *> subsets = new_itemset->subItemSets(); //get the subsets (without empty and full)
				
				for(auto &s : subsets) { //verify if all the subsets, ItemSets exists on the large
					if(s->getAmountElements() > 1) { //if has size 1 can be skipped
						if(a->contains(s) == false) { //if doesnt contain, doesn't add
							ok = false;
							break;
						}
					}
				}
				for(auto &s: subsets)
					delete(s); //after verifying the subsets delete it
					
				
				//cout << "prune results were " << ok << endl;
				
				if(ok) 
					new_candidate->insertSet(new_itemset); //!this block will prune candidates
				else
					delete(new_itemset); //if the candidate itemset is not used delete the memory allocated
			}
		}
	}
	//cout << "thread exited" << endl;
}

inline uint64_t amount_op_aux(uint64_t a, uint64_t b) {
	return ((b*b*b-a*a*a) - (b-a))/3;
}

inline uint64_t amount_op(uint64_t amount_transactions, uint64_t block_init, uint64_t block_end) { //inside is a quadratic function
	return amount_op_aux(block_init, amount_transactions) - amount_op_aux(block_end, amount_transactions);
}

void run_subsetThreaded(CandidateTree * root, vector <pair <string, string>> * normalized_transactions, uint64_t rangeLow, uint64_t rangeHigh) {
	uint64_t j=0;
	for(uint64_t i=rangeLow; i < rangeHigh; i=j) { //will scan transaction by transaction
		for(j=i; j < normalized_transactions->size() && (*normalized_transactions)[j].first == (*normalized_transactions)[i].first; ++j);
		
		vector<pair <string, string>>::const_iterator first = normalized_transactions->begin() + i; //get the first element in transaction
		vector<pair <string, string>>::const_iterator last = normalized_transactions->begin() + j; //get the last element in transaction
		
		vector<pair <string, string>> transaction(first, last);
		
		root->transactionScan(&transaction);
	}
	//cout << "thread exit" << endl;
}
