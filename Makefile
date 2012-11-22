default:all
all:
	g++ src/dtw.cpp -ggdb -o bin/dtw
	g++ src/dtw_new.cpp -ggdb -o bin/dtwn
	rm test/*
	g++ src/data.cpp -ggdb -o bin/data
	bin/data
new:
	g++ src/dtw_new.cpp -ggdb -o bin/dtwn
ucr:
	g++ UCR_DTW.cpp -ggdb -o bin/ucr
generate:
	rm gen/*
	g++ src/train.cpp -ggdb -o bin/train
	bin/train

test:
	rm test/*
	g++ src/data.cpp -ggdb -o bin/data
	bin/data
