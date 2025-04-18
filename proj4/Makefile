CC = gcc
CFLAGS = -g -Wall -std=c99 -fopenmp -mavx -mfma -pthread
LDFLAGS = -fopenmp
# CUNIT = -L/home/ff/cs61c/cunit/install/lib -I/home/ff/cs61c/cunit/install/include -lcunit
CUNIT = -lcunit
# PYTHON = -I/usr/include/python3.6 -lpython3.6m
PYTHON_INCLUDE = -I/home/junlang/anaconda3/envs/cs61c/include/python3.6m
PYTHON_LIB = -L/home/junlang/anaconda3/envs/cs61c/lib -lpython3.6m
PYTHON_RPATH = -Wl,-rpath=/home/junlang/anaconda3/envs/cs61c/lib
PYTHON = $(PYTHON_INCLUDE) $(PYTHON_LIB) $(PYTHON_RPATH)

install:
	if [ ! -f files.txt ]; then touch files.txt; fi
	rm -rf build
	xargs rm -rf < files.txt
	python3 setup.py install --record files.txt

uninstall:
	if [ ! -f files.txt ]; then touch files.txt; fi
	rm -rf build
	xargs rm -rf < files.txt

clean:
	rm -f *.o
	rm -f test
	rm -rf build
	rm -rf __pycache__

test:
	rm -f test
	$(CC) $(CFLAGS) mat_test.c matrix.c -o test $(LDFLAGS) $(CUNIT) $(PYTHON)
	./test

.PHONY: test
