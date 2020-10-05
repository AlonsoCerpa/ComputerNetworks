all : client_host.exe server_host.exe slave1_host.exe slave2_host.exe slave3_host.exe slave4_host.exe

client_host.exe : client_host.cpp go_back_n.cpp process_incoming_segments.cpp process_input_from_client.cpp process_timer.cpp
	g++ client_host.cpp go_back_n.cpp process_incoming_segments.cpp process_input_from_client.cpp process_timer.cpp -o client_host.exe -pthread -lncurses

server_host.exe : server_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp
	g++ server_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp -o server_host.exe -pthread

slave1_host.exe : slave1_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp
	g++ slave1_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp -o slave1_host.exe -pthread

slave2_host.exe : slave2_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp
	g++ slave2_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp -o slave2_host.exe -pthread

slave3_host.exe : slave3_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp
	g++ slave3_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp -o slave3_host.exe -pthread

slave4_host.exe : slave4_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp
	g++ slave4_host.cpp go_back_n.cpp process_incoming_segments.cpp process_timer.cpp process_req_ans.cpp -o slave4_host.exe -pthread

clean :
	rm -f client_host.exe server_host.exe slave1_host.exe slave2_host.exe slave3_host.exe slave4_host.exe