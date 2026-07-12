# --- configuration ---
CXX       = g++
CXXFLAGS  = -std=c++20 -Wall -Wextra -g
TARGET    = photondb
SRCS      = $(shell find src lib -name "*.cpp")
OBJS      = $(SRCS:.cpp=.o)

# --- default target (first one wins) ---
all: $(TARGET)

# --- link step ---
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --- compile step (pattern rule) ---
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- housekeeping ---
.PHONY: all clean run

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)%