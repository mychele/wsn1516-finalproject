###################################################
# Makefile for WSN 1516 final project
###################################################

all: sender receiver ncpacket

sender: misc/sender.cpp
	@clang++ -o sender $?

receiver: misc/receiver.cpp
	@clang++ -o receiver $?

ncpacket: code/NCpacket.cpp
	@clang++ -c -I./code $?

report: /report/report.tex
	@pdflatex $?
codereport: /report/code.tex
	@pdflatex $?
	@pdflatex $?
clean:
	@rm -f *.class *.pdf *.log *.aux *.zip
