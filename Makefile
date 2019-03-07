
SRC_DIR := src/
OBJ_DIR := obj/

COMMON_OBJ := obj/common/database_normalized.o obj/common/itemset/itemset.o obj/common/itemset/itemset_tree.o obj/common/itemset/itemset_group.o obj/common/genNewTransaction.o obj/common/ontologies/ontology.o obj/common/ontologies/node_ontology.o obj/common/rule/rules.o obj/common/rule/rule_node.o obj/common/rule/rule_group.o

APRIORI_OBJ := obj/apriori/apriori.o obj/apriori/large.o obj/apriori/parameters.o obj/apriori/candidate_apriori/candidate.o obj/apriori/candidate_apriori/candidate_tree.o
MMB_OBJ := obj/max_miner/mmb.o obj/max_miner/large.o obj/max_miner/parameters.o obj/max_miner/candidate_maxminer/candidate.o obj/max_miner/candidate_maxminer/candidate_struct.o
RAPPENDER_OBJ := obj/rule_appender/rule_appender.o obj/rule_appender/parameters.o obj/rule_appender/large.o

APRIORI_CANDIDATE_FOLDER := obj/apriori/candidate_apriori/
MAXMINER_CANDIDATE_FOLDER := obj/max_miner/candidate_maxminer/
ONTOLOGY_FOLDER := obj/common/ontologies/
RULE_FOLDER := obj/common/rule/
NT_FOLDER := obj/common/new_transactions/
ITEMSET_FOLDER := obj/common/itemset/

OBJ_COMMON_DIR := obj/common/
OBJ_APRIORI_DIR := obj/apriori/
OBJ_MAX_MINER_DIR := obj/max_miner/
OBJ_RULEAPPENDER_DIR := obj/rule_appender/

TODO = $(shell grep -r TODO src/rule_appender/ src/apriori/ src/common/)

.PHONY: directories

LD_FLAGS := -Wall -std=c++11 -pthread -g
CC_FLAGS := -Wall -std=c++11 -pthread -g

all: directories apriori rule_appender
	$(info ==TODO==)
	$(info $(TODO))
	$(info ========)

apriori: directories $(APRIORI_OBJ) $(COMMON_OBJ)
	g++ $(LD_FLAGS) -o $@ $(APRIORI_OBJ) $(COMMON_OBJ)

max_miner: directories $(MMB_OBJ) $(COMMON_OBJ)
	g++ $(LD_FLAGS) -o $@ $(MMB_OBJ) $(COMMON_OBJ)
	
rule_appender: directories $(RAPPENDER_OBJ) $(COMMON_OBJ)
	g++ $(LD_FLAGS) -o $@ $(RAPPENDER_OBJ) $(COMMON_OBJ)

#COMMON FILES
$(OBJ_COMMON_DIR)genNewTransaction.o: $(SRC_DIR)common/new_transactions/genNewTransaction.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_COMMON_DIR)ontologies/ontology.o: $(SRC_DIR)common/ontologies/ontology.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_COMMON_DIR)ontologies/node_ontology.o: $(SRC_DIR)common/ontologies/node_ontology.cpp
	g++ $(CC_FLAGS) -c -o $@ $<
	
$(OBJ_COMMON_DIR)rule/rules.o: $(SRC_DIR)common/rule/rules.cpp
	g++ $(CC_FLAGS) -c -o $@ $<
	
$(OBJ_COMMON_DIR)rule/rule_node.o: $(SRC_DIR)common/rule/rule_node.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_COMMON_DIR)rule/rule_group.o: $(SRC_DIR)common/rule/rule_group.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_COMMON_DIR)database_normalized.o: $(SRC_DIR)common/database_normalized.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_COMMON_DIR)itemset/itemset.o: $(SRC_DIR)common/itemset/itemset.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_COMMON_DIR)itemset/itemset_tree.o: $(SRC_DIR)common/itemset/itemset_tree.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_COMMON_DIR)itemset/itemset_group.o: $(SRC_DIR)common/itemset/itemset_group.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

#APRIORI ONLY

$(OBJ_APRIORI_DIR)candidate_apriori/candidate.o: $(SRC_DIR)apriori/candidate_apriori/candidate.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_APRIORI_DIR)candidate_apriori/candidate_tree.o: $(SRC_DIR)apriori/candidate_apriori/candidate_tree.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_APRIORI_DIR)large.o: $(SRC_DIR)apriori/large.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_APRIORI_DIR)parameters.o: $(SRC_DIR)apriori/parameters.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_APRIORI_DIR)apriori.o: $(SRC_DIR)apriori/apriori.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

#MAX_MINER ONLY
$(OBJ_MAX_MINER_DIR)candidate_maxminer/candidate_struct.o: $(SRC_DIR)max_miner/candidate_maxminer/candidate_struct.cpp
	g++ $(CC_FLAGS) -c -o $@ $<
	
$(OBJ_MAX_MINER_DIR)candidate_maxminer/candidate.o: $(SRC_DIR)max_miner/candidate_maxminer/candidate.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_MAX_MINER_DIR)large.o: $(SRC_DIR)max_miner/large.cpp
	g++ $(CC_FLAGS) -c -o $@ $<
	
$(OBJ_MAX_MINER_DIR)parameters.o: $(SRC_DIR)max_miner/parameters.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_MAX_MINER_DIR)mmb.o: $(SRC_DIR)max_miner/mmb.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

#RULE_APPENDER ONLY
$(OBJ_RULEAPPENDER_DIR)large.o: $(SRC_DIR)rule_appender/large.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_RULEAPPENDER_DIR)parameters.o: $(SRC_DIR)rule_appender/parameters.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

$(OBJ_RULEAPPENDER_DIR)rule_appender.o: $(SRC_DIR)rule_appender/rule_appender.cpp
	g++ $(CC_FLAGS) -c -o $@ $<


directories: $(APRIORI_CANDIDATE_FOLDER) $(MAXMINER_CANDIDATE_FOLDER) $(ONTOLOGY_FOLDER) $(RULE_FOLDER) $(NT_FOLDER) $(ITEMSET_FOLDER) $(OBJ_COMMON_DIR) $(OBJ_APRIORI_DIR) $(OBJ_MAX_MINER_DIR) $(OBJ_RULEAPPENDER_DIR)

$(APRIORI_CANDIDATE_FOLDER):
	mkdir -p $@
	
$(MAXMINER_CANDIDATE_FOLDER):
	mkdir -p $@

$(ONTOLOGY_FOLDER):
	mkdir -p $@
	
$(RULE_FOLDER):
	mkdir -p $@
	
$(NT_FOLDER):
	mkdir -p $@

$(ITEMSET_FOLDER):
	mkdir -p $@

$(OBJ_COMMON_DIR):
	mkdir -p $@
	
$(OBJ_APRIORI_DIR):
	mkdir -p $@
	
$(OBJ_MAX_MINER_DIR):
	mkdir -p $@
	
$(OBJ_RULEAPPENDER_DIR):
	mkdir -p $@

clean:
	-rm -rf obj/
	-rm -rf apriori
	-rm -rf max_miner
	-rm -rf rule_appender
