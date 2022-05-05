APP_NAME=hashmap
OBJS += compute.o
OBJS += main.o
OBJS += trace_generator.o
OBJS += utils.o

CXX = mpic++ -std=c++11
CXXFLAGS = -g -I. -O3 -ggdb -no-pie -fno-pie -rdynamic -DBOOST_STACKTRACE_USE_ADDR2LINE #-Wall -Wextra

default: $(APP_NAME)

$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAME) *.class
