# le

### v 2.0
**Overivew**
At first, this project aims to do loop extraction. Now it can do program extraction.
As before, I give an example to explain what I have done

**Input**
```C
double evaluate(int n, double d) {
  double res = 0.0;
  int t = 3;
  t++;
  int p = n;
  ++p;

  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      res += 1 / i + t;
    }

    for (int i = 0; i * i < n; ++i) {
      res = res - i - p;
      for (int j = 0; j < n; ++j) {
        res *= 2;
      }
    }
  } else {
    for (int i = n; i < 0; ++i) {
      if (d > 10) {
        res *= 2;
      } else {
        d = d - i;
        res += d;
      }
    }
  }

  res = res - d;
  return d - res;
}

```
**Output**
```
{
    'program_name': 'test.c',
    'variables': {},
    'initialize': {},
    'functions': {
        'evaluate': {
            'variables': { 'd': 'double', 'n': 'int', 'p': 'int', 'res': 'double', 't': 'int' },
            'input_variables': [ 'd', 'n' ],
            'paths': {
                'path1': {
                    'constraints': ' (n > 0) ',
                    'path': [ '{block1}', '{loop1}', '{loop2}', '{block2}' ],
                    'return': '(d - res)'
                },
                'path2': {
                    'constraints': ' (!(n > 0)) ',
                    'path': [ '{block1}', '{loop4}', '{block2}' ],
                    'return': '(d - res)'
                }
            },
            'blocks': {
                'block1': {
                    'p': '(p + 1)',
                    't': '(t + 1)'
                },
                'block2': {
                    'res': '(res - d)'
                }
            },
            'loops': {
                'loop1': {
                    'variables': { 'i': 'int' },
                    'initialize': { 'i': '0' },
                    'paths': [
                        {
                            'constraints': ' (i < n) ',
                            'path': ['i': '(i + 1)', 'res': '(res + (((1 / i) + t)))'],
                            'break': 'false'
                        },
                        {
                            'constraints': ' (!(i < n)) ',
                            'path': [],
                            'break': 'true'
                        }
                    ]
                },
                'loop2': {
                    'variables': { 'i': 'int' },
                    'initialize': { 'i': '0' },
                    'paths': [
                        {
                            'constraints': ' ((i * i) < n) ',
                            'path': ['i': '(i + 1)', 'res': '((res - (i)) - (p))', '{loop3}'],
                            'break': 'false'
                        },
                        {
                            'constraints': ' (!((i * i) < n)) ',
                            'path': [],
                            'break': 'true'
                        }
                    ]
                },
                'loop3': {
                    'variables': { 'j': 'int' },
                    'initialize': { 'j': '0' },
                    'paths': [
                        {
                            'constraints': ' (j < n) ',
                            'path': ['j': '(j + 1)', 'res': '(res * (2))'],
                            'break': 'false'
                        },
                        {
                            'constraints': ' (!(j < n)) ',
                            'path': [],
                            'break': 'true'
                        }
                    ]
                },
                'loop4': {
                    'variables': { 'i': 'int' },
                    'initialize': { 'i': 'n' },
                    'paths': [
                        {
                            'constraints': ' (i < 0) && (d > (10)) ',
                            'path': ['i': '(i + 1)', 'res': '(res * (2))'],
                            'break': 'false'
                        },
                        {
                            'constraints': ' (!(i < 0)) ',
                            'path': [],
                            'break': 'true'
                        },
                        {
                            'constraints': ' (i < 0) && (!(d > (10))) ',
                            'path': ['d': '(d - (i))', 'i': '(i + 1)', 'res': '(res + d)'],
                            'break': 'false'
                        }
                    ]
                }
            }
        }
    }
}

```

<br/>
<br/>

### v1.0
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

```bash
cd home_of_this_project/build
cp Makefile.dist Makefile
vim Makefile
# set ROSE_INSTALL_DIR to root directory of rose 
# set BOOST_INSTALL_DIR to root directory of boost
# set LE_ROOT_DIR to root directory of this project
```
	
* Make
```bash
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
