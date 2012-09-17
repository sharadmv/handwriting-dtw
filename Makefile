default:all
all:
	g++ src/dtw.cpp -ggdb -o bin/dtw
old:
	g++ src/dtw_old.cpp -ggdb -o bin/dtwo
ucr:
	g++ UCR_DTW.cpp -ggdb -o bin/ucr
generate:
	rm gen/*
	g++ src/train.cpp -ggdb -o bin/train
	bin/train
