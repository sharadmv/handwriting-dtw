default:new
all:
	g++ src/dtw.cpp -ggdb -o bin/dtw
new:
	g++ src/dtw_new.cpp -ggdb -o bin/dtwn
ucr:
	g++ UCR_DTW.cpp -ggdb -o bin/ucr
generate:
	rm gen/*
	g++ src/train.cpp -ggdb -o bin/train
	bin/train
