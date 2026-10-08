CXX = clang++
CXXFLAGS = -std=c++17 -I. -I/opt/homebrew/include -DGL_SILENCE_DEPRECATION
LDFLAGS = -L/opt/homebrew/lib -lglfw -framework OpenGL

all: solar

solar: main.cpp glad.c
	$(CXX) $(CXXFLAGS) main.cpp glad.c $(LDFLAGS) -o solar

clean:
	rm -f solar
	rm -rf build
