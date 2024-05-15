default: main

%: %.cpp
	g++ -std=c++17 -I. $< -o $@ AGL3Window.cpp  -lepoxy -lGL -lglfw 
clean:
	rm a.out *.o *~ AGL3-example
