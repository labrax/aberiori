/* author: Victor Roth Cardoso - vroth
 * student sponsored by CNPq
 * 
 * in summer project at Aberystwyth University
 * 
 * 
 * supervisors: Robert Hoehndorf and Georgios Gkoutos
 */

#pragma once

#include "../itemset/itemset.hpp"
#include <cstdint>

class RuleNode {
	private:
		unsigned int sumDepth_antecedent, sumDepth_consequent;
		unsigned int sumHeight_antecedent, sumHeight_consequent;
		
		uint64_t n_transactions;
		uint64_t n_transactions_antecedent;
		uint64_t n_transactions_consequent;
		
		double semantic_similarity;
		
		double support;
		double confidence;
		double lift;

		ItemSet * antecedent;
		ItemSet * consequent;
	public:
		RuleNode();
		RuleNode(ItemSet * antecedent, ItemSet * consequent);
		RuleNode(const RuleNode & other);
		~RuleNode();
		
		void setNTransactions(uint64_t n_transactions);
		void setNTransactionsAntecedent(uint64_t n_transactions);
		void setNTransactionsConsequent(uint64_t n_transactions);
		void setSemanticSimilarity(double ss);
		void setSupport(double support);
		void setConfidence(double confidence);
		void setLift(double lift);
		
		void calculateSupport(uint64_t amount_transactions); //to calculate each variable
		void calculateLift(uint64_t amount_transactions);
		void calculateConfidence(void);
		void calculateDepthHeight(Ontology * ontologies);
		
		ItemSet * getItemSetAntecedent(void) const;
		ItemSet * getItemSetConsequent(void) const;
		
		const uint64_t getNTransactions(void) const;
		const uint64_t getNTransactionsAntecedent(void) const;
		const uint64_t getNTransactionsConsequent(void) const;
		const double getSemanticSimilarity(void) const;
		const double getSupport(void) const;
		const double getConfidence(void) const;
		const double getLift(void) const;
		
		const unsigned int getSumDepth(void) const;
		const unsigned int getSumHeight(void) const;
};
