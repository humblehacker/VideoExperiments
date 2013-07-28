CXX=clang++
CXXFLAGS=-I/usr/local/include -std=c++11 -stdlib=libc++
LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system
EXENAME=vidtest
BUILDDIR=build

SRCS=main.cc decode.cc rotate.cc
OBJS=$(patsubst %.cc,$(BUILDDIR)/%.o, $(SRCS))

all : $(EXENAME)

$(BUILDDIR)/%.o : %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

$(EXENAME) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(EXENAME) $(BUILDDIR)

