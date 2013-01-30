AM_CXXFLAGS= \
	-Wall \
	-Werror \
	-std=c++0x \
	-ggdb3

bin_PROGRAMS = monty test_monty

monty_SOURCES=\
	monty.cpp\
	object.cpp\
	message.cpp\
	ast.cpp

AM_LDFLAGS= \
	-ljson

test_monty_LDFLAGS=\
	-lgtest_main -lpthread -ljson

test_monty_SOURCES=\
	test_monty.cpp\
	object.cpp\
	message.cpp\
	ast.cpp
