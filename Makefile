prompt: prompt.c
	cc -std=c99 -Wall prompt.c -ledit -o prompt

.PHONY : clean
clean:
	-rm -f prompt

.SILENT: