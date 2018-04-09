CXX=g++ -m64
CXXFLAGS=-Iobjs/ -O3 -Wall -fno-tree-vectorize
ISPC=ispc
ISPCFLAGS=-O3 --arch=x86-64 --target=avx

default: column_scan

.PHONY: dirs clean
.PRECIOUS: objs/column_scan.h

dirs:
	/bin/mkdir -p objs/

clean:
	/bin/rm -rf objs *~ column_scan

column_scan: dirs  objs/column_scan.o objs/column_scan_ispc.o
	$(CXX) $(CXXFLAGS) -o $@ objs/column_scan.o objs/column_scan_ispc.o

objs/column_scan.o: column_scan.cpp objs/column_scan_ispc.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

objs/%_ispc.h objs/%_ispc.o: %.ispc
	$(ISPC) $(ISPCFLAGS) $< -o objs/$*_ispc.o -h objs/$*_ispc.h
