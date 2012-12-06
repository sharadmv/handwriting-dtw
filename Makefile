default:all
all:
	#g++ src/dtw.cpp -ggdb -o bin/dtw
	g++ src/dtw2d.cpp -ggdb -o bin/dtw2d
	#g++ src/dtw_new.cpp -ggdb -o bin/dtwn
	#g++ src/UCR_DTW.cpp -ggdb -o bin/ucr
	#g++ src/normalize.cpp -ggdb -o bin/normalize
test:
	rm test/*
	g++ src/data.cpp -ggdb -o bin/data
	bin/data
generate:
	rm gen/*
	g++ src/train.cpp -ggdb -o bin/train
	bin/train
gen_data:
	rm test/*
	g++ src/data.cpp -ggdb -o bin/data
	bin/data
plot_gen:
	python scripts/plot_gen.py ${ARGS}
plot_train:
	python scripts/plot_train.py ${ARGS}
get_data:
	python scripts/get_data.py
normalize:
	rm normal/* 
	bin/normalize ${ARGS}
