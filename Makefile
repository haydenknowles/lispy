prompt: prompt.c
	cc -std=c99 -Wall prompt.c -o prompt

.PHONY : clean
clean:
	-rm -f prompt

.SILENT: