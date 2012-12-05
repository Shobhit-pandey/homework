TARGET=writeup
HTML=main_html
SRC=launcher_control.c
SRC2=kern_make.sh

default: pdf clean

both: pdf html

dvi: ${TARGET}.tex 
#	pygmentize the input source file -- THIS NAME SHOULD BE SAFE
	pygmentize -f latex -o __${SRC}.tex ${SRC}
	pygmentize -f latex -o __${SRC2}.tex ${SRC2}
#	run latex twice to get references correct
	latex ${TARGET}.tex
#	you can also have a bibtex line here
#	bibtex $(TARGET).tex
	latex $(TARGET).tex
#	remove the pygmentized output to avoid cluttering up the directory
	rm __${SRC}.tex
	rm __${SRC2}.tex

ps: dvi
	dvips -R -Poutline -t letter ${TARGET}.dvi -o ${TARGET}.ps

pdf: ps
	ps2pdf ${TARGET}.ps


html:
	cp ${TARGET}.tex ${HTML}.tex
	latex ${HTML}.tex
	latex2html -split 0 -noshow_section_numbers -local_icons -no_navigation -noinfo -noaddress ${HTML}

	sed 's/<BR><HR>//g' < ${HTML}/index.html > ${HTML}/index2.html
	mv ${HTML}/index2.html ${TARGET}.html
	rm ${HTML}.*
	rm -rf ${HTML}

clean: 
	rm -f ${TARGET}.aux ${TARGET}.dvi ${TARGET}.log ${TARGET}.out ${TARGET}.ps
