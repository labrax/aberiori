#!/usr/bin/python

from time import time, asctime, gmtime
from subprocess import call

class AprioriRunner:
	def execute(self, support, confidence):
		to_run = self.argsToRun[:]
		to_run.append("-l")
		to_run.append(str(support))
		
		to_run.append("-c")
		to_run.append(str(confidence))
		
		self.run(to_run)
		
	def run_range(self, min_support, max_support, step_support, min_confidence, max_confidence, step_confidence):
		s = min_support
		c = min_confidence
		
		while True:
			if(s > max_support):
				s = min_support
				c += step_confidence
				c = round(c, 2)
				if(c > max_confidence):
					break
				
			self.execute(s, c)
			
			s += step_support
			s = round(s, 2)
		
	def run(self, parameters):
		self.num += 1
		try:
			stdout_file = open("%d_rules.txt" %(self.num), 'w')
			stderr_file = open("%d_err.txt" %(self.num), 'w')
		except IOError:
			print "Could not open output file(s)!"
			info_file.write("%d: %s could not open file\n" %(num, args.join(' ')))
			return -1
		
		parameters.append("-x")
		parameters.append(str(self.num) + "_newtrans.txt")
		
		print "running <%s>" % ' '.join(parameters)

		t0 = time()	
		call(parameters, stdout=stdout_file, stderr=stderr_file)
		t1 = time()
		self.info_file.write("%d\t%s\t%f\n" % (self.num, ' '.join(parameters), (t1-t0)))
		
		stderr_file.close()
		stdout_file.close()
		
	def set_settings(self, itemset_filtering, rule_filtering, append_ontologies, preprocess, max_iteration, verbose, thread):
		self.argsToRun = self.args[:]
		
		if(itemset_filtering == True):
			self.argsToRun.append("-I")
			
		if(rule_filtering == True):
			self.argsToRun.append("-R")
			
		if(append_ontologies == False):
			self.argsToRun.append("-a")
			
		if(preprocess == True):
			self.argsToRun.append("-p")
			
		if(max_iteration > 0):
			self.argsToRun.append("-M")
			self.argsToRun.append(str(max_iteration))
			
		if(verbose == True):
			self.argsToRun.append("-v")
		
		if(thread == 0):
			self.argsToRun.append("-t")
		elif(thread > 0):
			self.argsToRun.append("-t" + str(thread))
		
	def __init__(self, transactions_file, phenotypes_file):
		self.args = ["time", "./apriori", "-f", transactions_file, "-o", phenotypes_file]
		self.num = 0
		
		try:
			self.info_file = open("info.txt", 'a')
		except IOError:
			print "Could not open info.txt file! Exiting"
			exit -1
		gmttime = asctime( gmtime( time() ) )
		self.info_file.write("starting at %s GMT\n"%gmttime)
		
	def __exit__(self):
		info_file.close()
		
		
if __name__ == "__main__":
	a = AprioriRunner("pp.txt", "mam.obo")
	a.set_settings(False, False, True, True, 2, False, 0)
	a.run_range(0.05, 0.5, 0.05, 0.5, 0.95, 0.05)
