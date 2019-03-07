/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#include "rule_node.hpp"
#include "../parameters.hpp"

#include <iostream>

using std::cout;
using std::endl;

RuleNode::RuleNode() {
	RuleNode(NULL, NULL);
}

RuleNode::RuleNode(ItemSet * antecedent, ItemSet * consequent): antecedent(antecedent), consequent(consequent) {
	n_transactions=0;
	n_transactions_antecedent=0;
	n_transactions_consequent=0;
	
	semantic_similarity=0;
	
	support=0;
	confidence=0;
	lift=0;
	
	sumDepth_antecedent=0; sumHeight_antecedent=0;
	sumDepth_consequent=0; sumHeight_consequent=0;
}

RuleNode::RuleNode(const RuleNode & other) : sumDepth_antecedent(other.sumDepth_antecedent), sumDepth_consequent(other.sumDepth_consequent), sumHeight_antecedent(other.sumHeight_antecedent), sumHeight_consequent(other.sumHeight_consequent), n_transactions(other.n_transactions), n_transactions_antecedent(other.n_transactions_antecedent), n_transactions_consequent(other.n_transactions_consequent), semantic_similarity(other.semantic_similarity), support(other.support), confidence(other.confidence), lift(other.lift) {
	antecedent = new ItemSet(other.antecedent);
	consequent = new ItemSet(other.consequent);
}

RuleNode::~RuleNode() {
	delete(antecedent);
	delete(consequent);
}

void RuleNode::setNTransactions(uint64_t n_transactions) {
	this->n_transactions = n_transactions;
}

void RuleNode::setNTransactionsAntecedent(uint64_t n_transactions) {
	this->n_transactions_antecedent = n_transactions;
}

void RuleNode::setNTransactionsConsequent(uint64_t n_transactions) {
	this->n_transactions_consequent = n_transactions;
}

void RuleNode::setSemanticSimilarity(double ss) {
	this->semantic_similarity = ss;
}

void RuleNode::setSupport(double support) {
	this->support = support;
}

void RuleNode::setConfidence(double confidence) {
	this->confidence = confidence;
}

void RuleNode::setLift(double lift) {
	this->lift = lift;
}

void RuleNode::calculateSupport(uint64_t amount_transactions) {
	support = (double) n_transactions/amount_transactions;
}

void RuleNode::calculateLift(uint64_t amount_transactions) {
	lift = ((double) amount_transactions*n_transactions)/((double) n_transactions_antecedent * n_transactions_consequent);
}

void RuleNode::calculateConfidence(void) {
	confidence = (double) n_transactions/(double) n_transactions_antecedent;
}

void RuleNode::calculateDepthHeight(Ontology * ontologies) {
	for(auto & i : antecedent->getItemSet()) {
		NodeOntology * node = ontologies->getNode(i.first);
		if(node != NULL) {
			sumDepth_antecedent += node->getDepth();
			sumHeight_antecedent += node->getHeight();
		}
	}

	for(auto & i : consequent->getItemSet()) {
		NodeOntology * node = ontologies->getNode(i.first);
		if(node != NULL) {
			sumDepth_consequent += node->getDepth();
			sumHeight_consequent += node->getHeight();
		}
	}
}

ItemSet * RuleNode::getItemSetAntecedent(void) const {
	return antecedent;
}

ItemSet * RuleNode::getItemSetConsequent(void) const {
	return consequent;
}

const uint64_t RuleNode::getNTransactions(void) const {
	return n_transactions;
}

const uint64_t RuleNode::getNTransactionsAntecedent(void) const {
	return n_transactions_antecedent;
}

const uint64_t RuleNode::getNTransactionsConsequent(void) const {
	return n_transactions_consequent;
}

const double RuleNode::getSemanticSimilarity(void) const {
	return semantic_similarity;
}

const double RuleNode::getSupport(void) const {
	return support;
}

const double RuleNode::getConfidence(void) const {
	return confidence;
}

const double RuleNode::getLift(void) const {
	return lift;
}

const unsigned int RuleNode::getSumDepth(void) const {
	return (sumDepth_antecedent + sumDepth_consequent);
}

const unsigned int RuleNode::getSumHeight(void) const {
	return (sumHeight_antecedent + sumHeight_consequent);
}
