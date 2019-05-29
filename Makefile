#
# Build BFS
#

CPP= g++
CC = gcc
CFLAGS = -std=c99  -D WITH_OPENCL -I ~/metaCL_workspace/MetaMorph/include -I ~/metaCL_workspace/MetaMorph/metamorph-backends/opencl-backend -O3
CFLAGS2 = -lm
CPPFLAGS= -D WITH_OPENCL -I ~/metaCL_workspace/MetaMorph/include -I ~/metaCL_workspace/MetaMorph/metamorph-backends/opencl-backend

ifeq ($(shell uname), Darwin)
	OPENCL = -framework OpenCL
else
	OPENCL = -lOpenCL
endif


.SUFFIXES:  .o
OCL_OBJS =  metacl_module.o
OBJS = sssp.o $(OCL_OBJS)



TARGET = ssspmain

$(TARGET) :	$(OBJS)
		g++ -L ~/metaCL_workspace/MetaMorph/lib -o $@ $(OBJS) -lmm_opencl_backend -lmetamorph  $(OPENCL)

#
# CPP rule
#
%.o:	%.cpp 
	$(CPP) $(CPPFLAGS) -c $< $(OPENCL) 


#
# C rule
#
%.o:	%.c 
	$(CC) $(CFLAGS) -c $< $(OPENCL) $(CFLAGS2)


#
# Cleanup
#
clean:
	rm -f *.o *.mod *.bc 


run: 
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/metaCL_workspace/MetaMorph/lib ./ssspmain $(g) 

#
# Count lines of code
#
count:
	rm -f Lines
	for file in $(SRCS); do ./LineCount $$file Lines; done
	gawk -f ./LineReport < Lines >> Lines
	cat Lines

#
# Link compiled files only. No recompile.
#
link:
	 -o $(TARGET) $(OBJS)


