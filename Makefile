CXX=g++
NAME=solution
OUTPUT=out

.PHONY: all clean

all:
	@rm -rf ${OUTPUT}
	@mkdir ${OUTPUT}
	@${CXX} main.cpp -o ${OUTPUT}/${NAME}

clean:
	@rm -rf ${OUTPUT}
