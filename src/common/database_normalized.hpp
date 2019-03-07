/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

/** a general class for loading a file with separated values for phenotypes
 */
 
#pragma once

#include <fstream>
#include <string>
#include <map>
#include <vector>

#include <cstdint>

using std::ifstream;
using std::map;
using std::string;
using std::vector;
using std::pair;

class DatabaseNormalized {
	private:
		string filename;
		ifstream file;
		vector <pair <string, string>> normalized_transactions;
		uint64_t amount_transactions;
		
		void processNormalizedTransactions();
	public:
		DatabaseNormalized(string transactions_file, bool preprocessed);
		~DatabaseNormalized();
		
		void removeDuplicates(); //remove in normalized_transactions duplicates //TOO SLOW!
		
		vector <pair <string, string>> & getNormalizedTransactions();
		
		uint64_t getAmountTransactions(void);
		
		void printNormalizedTransactions();
};

bool normalizedCompare(const pair<string, string>& firstElem, const pair<string, string>& secondElem); //function to compare pairs with string type
