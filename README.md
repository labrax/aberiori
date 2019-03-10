# aberiori 

**DISCLAIMER: USE AT YOUR OWN RISK**

A rule generation supported by ontologies

This project was developed in the University of Aberystwyth 2014.

Author: Victor Roth Cardoso - sponsored by CNPq (Brazil)

Supervisors: Robert Hoehndorf and Georgios Gkoutos
 
## COMPILE
	This program uses features of C++11 standard (available from gcc4.7.2 ?)
	Be sure to have build-essential package on Linux
	This program uses pthreads.
	Use the Makefile (e.g. make -j 8)
	
## RUN
	Run the program as specified in it, or in the end of this section.

	This program will require 'sort' and 'uniq' programs, which will be executed as the preprocessing stage.
	
	You'll need a phenotypes file, or transactions file.
	The ontologies file (of class relationship) is optional. The format /.obo/ 1.2 is accepted.
	
	To show options, run as:
	./apriori -h
	
	---
	use format:
	-a	dont-append-ontologies: only use ontologies for semantic similarity
	-r	filter-results: DISABLED

	-h	help: will print this instruction of use
	-p	preprocessed: will not use 'sort -u' in the file before reading
	-v	verbose: will print some information of execution
	-d	debug: will print the sets information

	-f	transactions-file <file>: input file
	-o	ontologies-file <file>: input file

	-x	new-transactions-file <file>: create a new 'transactions' file using the rules

	-c	confidence <number 0 to 1>: confidence value - default 0.8
	-l	minsupport <number 0 to 1>: minimum support value - default 0.5
	-u	maxsupport <number 0 to 1>: maximum support value - default 1

	-R to enable rules-filtering
	-I to enable itemset-filtering
	-M <number> to limit the number of iterations

	-t	thread <optional: number of threads>: to use parallelization

	options can be guessed, if:
	the phenotypes file is .txt it doesn't need -f
	the ontologies file is .obo it doesn't need -o

	example of use: 
	./apriori -f mousephenotypes.txt -o mammalian_phenotype.obo -t=20
	./apriori mousephenotypes.txt mammalian_phenotype.obo -t -l 0.01 -c 0.3
	---

## TEST MACHINE
	This program was succesfully built using:
		g++ (Ubuntu 4.8.2-19ubuntu1) 4.8.2
	on
		Linux ubuntu-linux 3.13.0-30-generic #54-Ubuntu SMP Mon Jun 9 22:45:01 UTC 2014 x86_64 x86_64 x86_64 GNU/Linux

## SAMPLE FILES

Available in the folder *sample_files*
- sample.obo
- transactions.txt
- other_transaction.txt

Please note the spacing in the rows, the parser was hard-coded.
**The transactions.txt file should be tab-separated**

### SAMPLE CASES

Our ontology has this structure:

codeC -> codeB -> codeA

codeN -> code <- codeX

- Standard use without ontologies
`$ ./apriori sample_files/transactions.txt  -l 0.7 -c 0.1`
> empty result

- Let's add our ontology
`$ ./apriori sample_files/transactions.txt sample_files/sample.obo -l 0.7 -c 0.1`
```
support confidence      transactions    transactions_antecedent transactions_consequent semantic_similarity     lift    sumDepth        sumHeight       Antecedent===>  Consequent
      1          1                 9                          9                       9                -nan        1           0                3       code (my_name(optional)) ===>   codeC (C) 
      1          1                 9                          9                       9                -nan        1           0                3       codeC (C) ===>  code (my_name(optional)) 
```
Now we get the top level relations:

- Let's try a different support!
`$ ./apriori sample_files/transactions.txt sample_files/sample.obo -l 0.6 -c 0.1`
```
support confidence      transactions    transactions_antecedent transactions_consequent semantic_similarity     lift    sumDepth        sumHeight       Antecedent===>  Consequent
0.66667          1                 6                          6                       9                   0        1           1                2       codeB (BBB) ===>        code (my_name(optional)) 
0.66667    0.66667                 6                          9                       6                   0        1           1                2       code (my_name(optional)) ===>   codeB (BBB) 
      1          1                 9                          9                       9                -nan        1           0                3       code (my_name(optional)) ===>   codeC (C) 
      1          1                 9                          9                       9                -nan        1           0                3       codeC (C) ===>  code (my_name(optional)) 
0.66667          1                 6                          6                       6                   0      1.5           2                1       codeB (BBB) ===>        codeN (my_name_other) 
0.66667          1                 6                          6                       6                   0      1.5           2                1       codeN (my_name_other) ===>      codeB (BBB) 
0.66667          1                 6                          6                       9                   0        1           1                2       codeN (my_name_other) ===>      codeC (C) 
0.66667    0.66667                 6                          9                       6                   0        1           1                2       codeC (C) ===>  codeN (my_name_other) 
```
In this case we get some other relations

- Testing sub-classes
> $ ./apriori sample_files/other_transaction.txt
```
support confidence      transactions    transactions_antecedent transactions_consequent semantic_similarity     lift    sumDepth        sumHeight       Antecedent===>  Consequent
      1          1                 2                          2                       2                   0        1           0                0       code ===>       codeN 
      1          1                 2                          2                       2                   0        1           0                0       codeN ===>      code 
      1          1                 2                          2                       2                   0        1           0                0       code ===>       codeX 
      1          1                 2                          2                       2                   0        1           0                0       codeX ===>      code 
      1          1                 2                          2                       2                   0        1           0                0       codeN ===>      codeX 
      1          1                 2                          2                       2                   0        1           0                0       codeX ===>      codeN 
      1          1                 2                          2                       2                   0        1           0                0       code ===>       codeN codeX 
      1          1                 2                          2                       2                   0        1           0                0       codeN ===>      code codeX 
      1          1                 2                          2                       2                   0        1           0                0       code codeN ===> codeX 
      1          1                 2                          2                       2                   0        1           0                0       codeX ===>      code codeN 
      1          1                 2                          2                       2                   0        1           0                0       code codeX ===> codeN 
      1          1                 2                          2                       2                   0        1           0                0       codeN codeX ===>        code 
```

- Testing sub-classes with ontology
> $ ./apriori sample_files/other_transaction.txt sample_files/sample.obo
```
support confidence      transactions    transactions_antecedent transactions_consequent semantic_similarity     lift    sumDepth        sumHeight       Antecedent===>  Consequent                                                          
      1          1                 2                          2                       2                -nan        1           2                0       codeN (my_name_other) ===>      codeX (X)                                           
      1          1                 2                          2                       2                -nan        1           2                0       codeX (X) ===>  codeN (my_name_other)                                               
```
In this case it is filters the ontology relations.

Note that the relations inside the ontology were filtered!
