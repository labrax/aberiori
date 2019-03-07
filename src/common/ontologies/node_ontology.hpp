/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;

using std::map;

typedef struct {
	bool isFound;
	unsigned int distance;
} distance_to;

class NodeOntology {
	private:
		string identifier;
		string name;
		bool is_obsolete;
		vector <NodeOntology *> parents;
		
		vector <NodeOntology *> children;
		unsigned int max_height, max_depth;
	public:
		NodeOntology(string identifier, string name, bool is_obsolete);
		~NodeOntology();

		string & getIdentifier();
		string & getName();
		
		void insertParent(NodeOntology *); //will be used for height
		void insertChild(NodeOntology *); //this is an additional relation for the use of depth
		
		const unsigned int getAmountParents();
		const unsigned int getAmountChildren();
		
		unsigned int getDepth();
		void setDepth(unsigned int);
		
		unsigned int getHeight();
		void setHeight(unsigned int);
		
		bool isObsolete();
		
		bool isSon(string &); //will return true for a check of the same value! -- note: this may return some problems
		
		void print();
		
		map <string, bool> * returnOntologies();
		
		distance_to getDistance(NodeOntology *, bool);
};
