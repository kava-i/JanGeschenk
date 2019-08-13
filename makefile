all:
	g++ -std=c++1z -Iwebsocketpp main.cpp -lboost_system -lpthread
