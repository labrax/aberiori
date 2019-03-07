/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "ontology.hpp"
#include "node_ontology.hpp"

#include "../parameters.hpp"

#include <iostream>
#include <cstdint>

using std::cerr;
using std::cout;
using std::endl;

using std::pair;

Ontology::Ontology() : filename(string("")) {
	processed = false;
}

Ontology::Ontology(const string & filename) : filename(filename) {
	file.open(filename);
	if(!file.is_open()) {
		cerr << "Error opening \"" << filename << "\" ontologies file" << endl;
	}
	processed = false;
}

Ontology::~Ontology() {
	for(auto & o : ontologies) {
		delete(o.second);
	}
}

const bool & Ontology::isProcessed() {
	return processed;
}

void Ontology::processOntologies() {
	if(file.is_open()) {
		string line;
		
		string id(""), name("");
		bool is_obsolete=false;
		vector<pair<string, string>> is_a;
		vector<string> consider;
		string replaced_by("");
		
		while (getline(file, line)) { //"ref": http://www.geneontology.org/GO.format.obo-1_2.shtml
			unsigned int twopoints_pos = line.find_first_of(':');
			
			if(line == "[Term]") {
				
				if(id == "")
					continue;
				
				createNode(id, name, is_obsolete, &is_a, &consider, replaced_by);
				
				//!reset the data for the next ontology
				id = "";
				name = "";
				is_obsolete = false;
				is_a.clear();
				consider.clear();
				replaced_by = "";
				//!reset the data for the next ontology
			}
			else if(line.substr(0, twopoints_pos) == "id") {
				id = line.substr(twopoints_pos+2);
			}
			else if(line.substr(0, twopoints_pos) == "name") {
				name = line.substr(twopoints_pos+2);
			}
			else if(line.substr(0, twopoints_pos) == "is_a") {
				unsigned int exclamation_pos = line.find_first_of('!');
				//cout << "<" << line.substr(twopoints_pos+2, exclamation_pos-(twopoints_pos+3)) << "> <" << line.substr(exclamation_pos+2) << ">" << endl;
				is_a.insert(is_a.end(), pair<string, string> (line.substr(twopoints_pos+2, exclamation_pos-(twopoints_pos+3)), line.substr(exclamation_pos+2)));
			}
			else if(line.substr(0, twopoints_pos) == "is_obsolete") {
				if(line.substr(twopoints_pos+2) == "true")
					is_obsolete = true;
			}
			else if(line.substr(0, twopoints_pos) == "consider") {
				consider.insert(consider.end(), line.substr(twopoints_pos+2));
			}
			else if(line.substr(0, twopoints_pos) == "replaced_by") {
				replaced_by = line.substr(twopoints_pos+2);
			}
		}
		
		if(id != "") { //save if there is information not saved on the loop
			createNode(id, name, is_obsolete, &is_a, &consider, replaced_by);
		}
		
		//!set depth and height
		for(auto & i : ontologies) {
			if(i.second->getAmountParents() == 0) { //the node is root
				i.second->setDepth(0);
			}
			if(i.second->getAmountChildren() == 0) {
				i.second->setHeight(0);
			}
		}
		//!set depth and height
		processed = true;
	}
}

void Ontology::createNode(string id, string name, bool is_obsolete, vector<pair<string, string>> * is_a, vector<string> * consider, string replaced_by) { //TODO: use /consider/ and /replaced_by/ fields if needed
	NodeOntology * nNode;
	//!create the node
	map<string, NodeOntology *>::iterator it = ontologies.find(id);
	if(it == ontologies.end()) {
		//cout << "doesnt exist" << endl;
		nNode = new NodeOntology(id, name, is_obsolete);
		ontologies.insert(pair<string, NodeOntology *>(id, nNode));
	}
	else {
		//cout << "exists" << endl;
		nNode = it->second;
	}
	//!create the node
	
	//!create the parents
	for(auto & ia : *is_a) {
		map<string, NodeOntology *>::iterator it = ontologies.find(ia.first);
		if(it == ontologies.end()) {
			NodeOntology * father = new NodeOntology(ia.first, ia.second, is_obsolete);
			ontologies.insert(pair<string, NodeOntology *>(ia.first, father));
			father->insertChild(nNode);
			nNode->insertParent(father);
		}
		else {
			NodeOntology * father = it->second;
			father->insertChild(nNode);
			nNode->insertParent(father);
		}
	}
	//!create the parents
}

bool Ontology::checkAncestorOneAnother(string ontologyA, string ontologyB) {
	if(processed == false)
		cerr << "using ontologies without being loaded! " << __FILE__ << ":" << __LINE__ << endl;
	map<string, NodeOntology *>::iterator itA = ontologies.find(ontologyA);
	map<string, NodeOntology *>::iterator itB = ontologies.find(ontologyB);
	
	if(itA == ontologies.end() || itB == ontologies.end()) { //both doesn't exist on the relations
		return false;
	}
	else {
		if(itA->second->isSon(ontologyB))
			return true;
		else if(itB->second->isSon(ontologyA))
			return true;
		return false;
	}
}

bool Ontology::checkSon(string child, string parent) {
	if(processed == false)
		cerr << "using ontologies without being loaded! " << __FILE__ << ":" << __LINE__ << endl;
	map<string, NodeOntology *>::iterator itA = ontologies.find(child);
	map<string, NodeOntology *>::iterator itB = ontologies.find(parent);
	
	if(itA == ontologies.end() || itB == ontologies.end()) { //both doesn't exist on the relations
		return false;
	}
	else {
		if(itA->second->isSon(parent))
			return true;
		return false;
	}	
}

void Ontology::appendOntologies(vector<pair<string, string>> * normalized_transactions) {
	if(processed == false)
		cerr << "using ontologies without being loaded! " << __FILE__ << ":" << __LINE__ << endl;
	uint64_t end=0, increased_size=0; //begin and end indicate the range of a transaction; increased_size the amount of new values inserted into normalized_transactions
	uint64_t initial_size = normalized_transactions->size();
	for(uint64_t begin=0; begin < initial_size; begin=end/*+increased_size*/) { //will scan transaction by transaction
		for(end=begin; end < normalized_transactions->size() && (*normalized_transactions)[end].first == (*normalized_transactions)[begin].first; ++end);
		
		vector<pair <string, string>>::const_iterator first = normalized_transactions->begin() + begin; //get the first element in a transaction
		vector<pair <string, string>>::const_iterator last = normalized_transactions->begin() + end; //get the last element in a transaction
		
		vector<pair <string, string>> transaction(first, last); //new sub-transaction
	
		vector<pair<string, string>> * newtra = getNewOntologies(transaction); //get new ontologies
	
		/*cout << "initial:" << transaction.size() << endl; //print the initial transaction
		for(auto & i : transaction) {
			cout << i.first << " " << i.second << endl;
		}
		
		cout << "additional: " << newtra->size() << endl; //print the new ontologies information
		for(auto & i : *newtra) {
			cout << i.first << " " << i.second << endl;
		} */
		
		/*//!merge the initial with the additional
		unsigned int i, j;
		increased_size = newtra->size();
		for(i=begin, j=0; j<increased_size; ) { //this will pass through normalized_transactions
			if(i < end && (*normalized_transactions)[i].second < (*newtra)[j].second) {
				i++;
			}
			else {
				(*normalized_transactions).insert((*normalized_transactions).begin() + i, (*newtra)[j]);
				j++;
			}
		}
		//!merge the initial with the additional*/
		
		increased_size += newtra->size();
		for(auto & i: *newtra) {
			normalized_transactions->insert(normalized_transactions->end(), i);
		}
		
		delete(newtra);
	}
	if(Parameters::verbose)
		cout << increased_size << " items added to transactions" << endl;
}

void Ontology::print() {
	if(processed == false)
		cerr << "using ontologies without being loaded! " << __FILE__ << ":" << __LINE__ << endl;
	for(auto & o : ontologies) {
		o.second->print();
	}
}

NodeOntology * Ontology::getNode(string identifier) {
	map<string, NodeOntology *>::iterator it = ontologies.find(identifier);
	if(it != ontologies.end()) {
		return it->second;
	}
	else
		return NULL;
}

vector <pair <string, string>> * Ontology::getNewOntologies(vector <pair <string, string>> & transaction) {
	if(processed == false)
		cerr << "using ontologies without being loaded! " << __FILE__ << ":" << __LINE__ << endl;
	map<string, bool> * newOntologies = new map <string, bool>();
	
	for(auto &t : transaction) {
		map<string, bool> * newOntologiesElement;
		
		map<string, NodeOntology *>::iterator it = ontologies.find(t.second);
		if(it != ontologies.end()) {
			newOntologiesElement = it->second->returnOntologies();
			
			for(auto & ne : *newOntologiesElement) { //go through the new "list" of elements
				if(newOntologies->find(ne.first) == newOntologies->end()) { //if it is not on the list known, insert
					newOntologies->insert(ne);
				}
			}
			
			delete(newOntologiesElement);
		}
	}
	
	vector <pair <string, string>> * newTransactions = new vector<pair <string, string>>();
	for(auto & no : *newOntologies) {
		bool ok = true;
		for(auto & tra : transaction) {
			if(tra.second == no.first) {
				ok = false;
				break;
			}
		}
		if(ok == true)
			newTransactions->insert(newTransactions->end(), pair<string, string>(transaction[0].first, no.first));
	}
	delete(newOntologies);
	
	return newTransactions;
}

distance_to Ontology::getDistance(string & a, string & b) {
	distance_to d;
	map<string, NodeOntology *>::iterator itA = ontologies.find(a);
	map<string, NodeOntology *>::iterator itB = ontologies.find(b);
	
	if(itA == ontologies.end() || itB == ontologies.end()) { //at least one of them was not found
		d.isFound = false;
	}
	else {
		d = itA->second->getDistance(itB->second, true);
	}
	return d;
}
