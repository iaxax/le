# le
**Overview**

This is a tool based on [Rose Compiler Infrastructure](http://rosecompiler.org/)  to do loop extraction.

***
**What is loop extraction?**

Make an example to explain that.

source code:

```C++
int s, q, n;
for (int i = 0; i < n; ++i) {
	if (i % 2 == 0) {
		s += 2;
	} else {
		q += i * 2;
	}
}

```

output:

```
==================================================
Loop name: loop1
Loop variables: [ 'i', 'n', 'q', 's' ]
Loop body:
    path: { }
    constraint: (!(i < n))
    break: true

    path: { i: (i + 1), s: (s + 2) }
    constraint: (i < n) && ((i % 2) == 0)
    break: false

    path: { i: (i + 1), q: (q + (i * 2)) }
    constraint: (i < n) && (!((i % 2) == 0))
    break: false

==================================================

```

***

**Build**

* Install Rose, please refer to [here](http://rosecompiler.org/ROSE_HTML_Reference/installation.html) (That takes really a long time for me, so just be patient)

* Modify build/Makefile
	* set ROSE_INSTALL_DIR to root directory of rose 
	* set BOOST_INSTALL_DIR to root directory of boost
	* set LE_ROOT_DIR to root directory of this project
	
* Make
```bash
cd root_of_this_project/build
make
```
***

**Run a test**

```bash
cd root_of_this_project/build
./le test.c
```

You may see something like the output above.

***


**Say somthing**
This tool is like a toy supporting a few language features in C/C++ and may be buggy.If you are interested in this project, welcome to join me.
Also, you can open an issue if you encouter any problem.I will try to help you out as possible as I can.

***
