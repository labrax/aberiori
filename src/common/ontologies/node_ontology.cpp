/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "node_ontology.hpp"

#include <iostream>
#include <climits>

using std::cout;
using std::endl;

using std::pair;

NodeOntology::NodeOntology(string identifier, string name, bool is_obsolete) : identifier(identifier), name(name), is_obsolete(is_obsolete) {
	max_height = 0;
	max_depth = 0;
}

NodeOntology::~NodeOntology() {
	
}

string & NodeOntology::getIdentifier() {
	return identifier;
}

string & NodeOntology::getName() {
	return name;
}

void NodeOntology::insertParent(NodeOntology * parent) {
	parents.insert(parents.end(), parent);
}

void NodeOntology::insertChild(NodeOntology * child) {
	children.insert(children.end(), child);
}

const unsigned int NodeOntology::getAmountParents() {
	return parents.size();
}

const unsigned int NodeOntology::getAmountChildren() {
	return children.size();
}

unsigned int NodeOntology::getDepth() {
	return max_depth;
}

void NodeOntology::setDepth(unsigned int depth) {
	if(this->max_depth < depth)
		this->max_depth = depth;
	for(auto & i : children) {
		i->setDepth(this->max_depth+1);
	}
}

unsigned int NodeOntology::getHeight() {
	return max_height;
}

void NodeOntology::setHeight(unsigned int height) {
	if(this->max_height < height)
		this->max_height = height;
	for(auto & i : parents) {
		i->setHeight(this->max_height+1);
	}
}

bool NodeOntology::isObsolete() {
	return is_obsolete;
}

bool NodeOntology::isSon(string & identifier) {
	if(identifier == this->identifier)
		return true;

	for(auto & i: parents) {
		if(i->isSon(identifier))
			return true;
	}
	return false;
}

void NodeOntology::print() {
	cout << identifier << ": \"" << name << "\" ";
	if(is_obsolete)
		cout << "OBSOLETE";
	cout << "(" << max_depth << ", " << max_height << ") ";
	if(parents.size() == 0) 
		cout << "NO PARENT ";
	else
		cout << "PARENTS: ";
	for(auto & p : parents) {
		cout << p->identifier << " ";
	}
	if(children.size() == 0)
		cout << "NO CHILDREN";
	else
		cout << "CHILDREN: ";
	for(auto & c : children) {
		cout << c->identifier << " ";
	}
	cout << endl;
}

map <string, bool> * NodeOntology::returnOntologies() {
	map <string, bool> * retOntol = new map<string, bool>();
	retOntol->insert(pair<string, bool> (identifier, true));
	
	if(parents.size() != 0) {	
		for(auto & p : parents) {
			map <string, bool> * newRetOntol = p->returnOntologies();
			for(auto & e : *newRetOntol) {
				if(retOntol->find(e.first) == retOntol->end()) {
					retOntol->insert(pair <string, bool>(e.first, true));
				}
			}
			delete(newRetOntol);
		}
	}
	return retOntol;
}

distance_to NodeOntology::getDistance(NodeOntology * target, bool newSearch) {
	static map<NodeOntology *, bool> passed;
	if(newSearch == true)
		passed.clear();
	distance_to d;
	if(this == target) {
		d.isFound = true;
		d.distance=0;
	}
	else {
		unsigned int minimum_distance = UINT_MAX;
		passed.insert(pair<NodeOntology *, bool>(this, true));
		for(auto & i : children) { //for the children nodes
			if(passed.find(i) == passed.end()) { //haven't passed here
				d = i->getDistance(target, false);
				if(d.isFound == true)
					if(d.distance < minimum_distance)
						minimum_distance = d.distance;
			}
		}
		for(auto & i : parents) { //for the parent nodes
			if(passed.find(i) == passed.end()) {
				d = i->getDistance(target, false);
				if(d.isFound == true)
					if(d.distance < minimum_distance)
						minimum_distance = d.distance;
			}
		}
		if(minimum_distance < UINT_MAX) {
			d.isFound = true;
			d.distance = minimum_distance + 1;
		}
	}
	return d;
}
