/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include "node_ontology.hpp"

#include <fstream>
#include <map>
#include <string>
#include <vector>

using std::ifstream;
using std::map;
using std::string;

using std::vector;
using std::pair;

class Ontology {
	private:
		const string & filename;
		ifstream file;
		map<string, NodeOntology *> ontologies;
		
		bool processed;
	public:
		Ontology(); //will initiate a "false" Ontology object
		Ontology(const string & filename);
		~Ontology();
		
		const bool & isProcessed();
		
		void processOntologies();
		void createNode(string id, string name, bool is_obsolete, vector<pair<string, string>> * is_a, vector<string> * consider, string replaced_by);

		bool checkAncestorOneAnother(string ontologyA, string ontologyB);
		bool checkSon(string child, string parent);
		
		void appendOntologies(vector<pair<string, string>> * normalized_transactions); //slow!

		void print();
		
		NodeOntology * getNode(string identifier);
		
		vector <pair <string, string>> * getNewOntologies(vector <pair <string, string>> & transaction); //this function will return the new block of the transaction
		
		distance_to getDistance(string & a, string & b);
};
