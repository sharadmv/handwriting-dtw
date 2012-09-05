default:generate
all:
	g++ src/dtw.cpp -ggdb -o bin/dtw
ucr:
	g++ UCR_DTW.cpp -ggdb -o bin/ucr
generate:
	rm gen/*
	g++ src/train.cpp -ggdb -o bin/train
	bin/train
