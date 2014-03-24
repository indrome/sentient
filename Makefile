.PHONY: server feature_extractor recognizer

all: server feature_extractor recognizer 
	@echo "Finished build"

server:
	$(MAKE) -C ./server
feature_extractor:
	$(MAKE) -C ./feature_extractor
recognizer:
	$(MAKE) -C ./recognizer/aquila-dtw
