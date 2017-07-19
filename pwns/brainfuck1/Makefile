#Make File
########################

CXX=clang++
FLAGS=-fstack-protector -fPIE -pie -Wl,-z,now

brainfuck: BrainFuckJiT.o BrainFuckVM.o
	$(CXX) $(FLAGS) BrainFuckJiT.o BrainFuckVM.o -o brainfuck && strip brainfuck

BrainFuckJiT.o: BrainFuckVM.h
	$(CXX) $(FLAGS) -c BrainFuckJiT.cpp

BrainFuckVM.o: BrainFuckVM.h
	$(CXX) $(FLAGS) -c BrainFuckVM.cpp

clean:
	rm -f *.o
	rm -f brainfuck
