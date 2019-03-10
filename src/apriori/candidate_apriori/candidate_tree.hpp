/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include "../../common/itemset/itemset_tree.hpp"
#include "../large.hpp"

#include <string>
#include <vector>

#include <cstdint>

using std::string;
using std::vector;
using std::pair;


class CandidateTree : public ItemSetTree{
	private:

	public:
		CandidateTree(unsigned int depth, string identifier, CandidateTree * father);
		~CandidateTree();
		
		void transactionScan(vector <pair <string, string>> *);
		void grabMinimumMaximumSupport(LargeItemSet *, uint64_t, uint64_t);
};
