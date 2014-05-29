import sys
import subprocess
import os
from os import walk

def strip_suffix(name):
	return name[:name.find(".")]

if __name__ == "__main__":

	if len(sys.argv) != 3:
		print "Usage: ",sys.argv[0]," FEATURE.json REFERENCE_PATH"
		exit(1)

	feature_file = sys.argv[1]

	print "="*40
	print "  Recognition scores"
	print "="*40


	scoreboard = []

	file_list = walk(sys.argv[2])
	p = subprocess.Popen(["recognizer/dtw/bin/compute_dtw",feature_file,map(lambda x: dirpath+x, ))],stdout=subprocess.PIPE)

	sorted_score = sorted(scoreboard,key=lambda e: e[1])
	print "\n".join(map(lambda e: str(e[1])+"  "+strip_suffix(e[0]),sorted_score))

	cmd = strip_suffix(sorted_score[0][0])+".sh"
	print "Running: ./commands/"+cmd

	os.system("./commands/"+cmd)
