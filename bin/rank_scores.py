import sys
import os
from os import walk

if __name__ == "__main__":

	if len(sys.argv) != 3:
		print "Usage: ",sys.argv[0]," FEATURE.json REFERENCE_PATH"
		exit(1)

	feature_file = sys.argv[1]

	f = []
	for (dirpath, dirnames, filenames) in walk(sys.argv[2]):
		for fname in filenames:
			print fname+":\t"
			os.system("recognizer/aquila-dtw/dtw "+feature_file+" "+dirpath+fname)

	
