all: main1.cpp main2.cpp node.h routing_algo.cpp
	g++ -std=c++11 main1.cpp routing_algo.cpp -o rip1
	g++ -std=c++11 main2.cpp routing_algo.cpp -o rip2

run:
	./rip1 < sampleinput.txt > q1_output.txt
	./rip2 < q2_input.txt > q2_output.txt
run_1:
	./rip1 < sampleinput.txt > q1_output.txt
run_2:
	./rip2 < q2_input.txt > q2_output.txt

diff1:
	diff sampleoutput.txt q1_output.txt

diff2:
	diff q2_correct_output.txt q2_output.txt

clean:
	rm rip1 rip2 q1_output.txt q2_output.txt