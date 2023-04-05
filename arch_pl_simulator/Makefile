JC = javac

lab4:
	$(JC) *.java

test1:
	/bin/java lab4 lab4_test1.asm lab4_test1.script > test1R.output
	diff -w test1R.output lab4_test1.output

test2:
	/bin/java lab4 lab4_test2.asm lab4_test1.script > test2R.output
	diff -w test2R.output lab4_test2.output


test3:
	/bin/java lab4 lab4_fib10.asm lab4_test1.script > test3R.output
	diff -w test3R.output lab4_fib10.output

test4:
	/bin/java lab4 lab4_fib20.asm lab4_test1.script > test4R.output
	diff -w test4R.output lab4_fib20.output

	

