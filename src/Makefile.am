AM_CXXFLAGS= \
	-Wall \
	-Werror \
	-std=c++0x \
	-ggdb3 \
	$(LLVM_CPPFLAGS)

bin_PROGRAMS = monty test_monty

lib_LTLIBRARIES=\
	libmonty.la

libmonty_la_SOURCES=\
	ast.cpp\
	codegen.cpp\
	message.cpp\
	object.cpp\
	parser.cpp\
	rule.cpp

libmonty_la_LDFLAGS=\
	$(LLVM_LDADD)\
	-ljson

LDADD=\
	libmonty.la

monty_SOURCES=\
	monty.cpp

test_monty_LDFLAGS=\
	-lgtest_main -lpthread -ljson

test_monty_SOURCES=\
	test_monty.cpp
