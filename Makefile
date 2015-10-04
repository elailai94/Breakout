##==============================================================================
## Breakout!
##
## @description: Makefile for breakout.cpp
## @author: Elisha Lai
## @version: 1.0 24/09/2015
##==============================================================================
NAME = "breakout"

# Add $(MAC_OPT) to the compile line for Mac OSX.
MAC_OPT = "-I/opt/X11/include"

all:
  @echo "Compiling..."
	g++ -o $(NAME) $(NAME).cpp -L/usr/X11R6/lib -lX11 -lstdc++ $(MAC_OPT)

run: all
	@echo "Running..."
	./$(NAME) 

clean:
	-rm breakout
