CXXC = clang
CXXFLAGS = -Wall -Wextra -lstdc++ --std=c++17 -g -Wno-unknown-pragmas `pkg-config --cflags sfml-all`
LIBS = `pkg-config --libs sfml-all`
FILES = main.cpp Editor.cpp Scroller.cpp EditorWindow.cpp

main: main.cpp Editor.cpp Scroller.cpp
	$(CXXC) $(CXXFLAGS) -o main.out $(FILES) $(LIBS)

clean:
	rm -f ./main.out